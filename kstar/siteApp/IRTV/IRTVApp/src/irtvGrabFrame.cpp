#include "dbScan.h"		// for scanIoRequest()

#include "irtvGrabFrame.h"
#include "irtvGrabFrameData.h"

IrtvGrabFrame::IrtvGrabFrame () 
{
	init ();
}

IrtvGrabFrame::IrtvGrabFrame (ST_IRTV *pIRTV) 
{
	init ();

	m_pIRTV	= pIRTV;
}

IrtvGrabFrame::~IrtvGrabFrame () 
{
	clear ();
}

const int IrtvGrabFrame::init () 
{
	kLog (K_MON, "[IrtvGrabFrame.init] create shared memory\n");

	lCamera			= NULL;
	lDocument		= NULL;
	lBuffer			= NULL;
	pObjParam		= NULL;

	m_pIRTV			= NULL;
	m_rawFp			= NULL;

	m_nGrabFrameCnt	= 0;
	m_width			= 0;
	m_height		= 0;

#if 0
	if (NULL == gGrabFrameShm.create ()) {
		return (NOK);
	}
#endif

	return (OK);
}

void IrtvGrabFrame::clear () 
{
	kLog (K_MON, "[IrtvGrabFrame.clear] release resources ... \n");

	m_nGrabFrameCnt	= 0;
	m_width			= 0;
	m_height		= 0;

	lGrabber.Disconnect ();

	if (NULL != m_rawFp) {
		fclose (m_rawFp);
		m_rawFp	= NULL;
	}

	if (NULL != lCamera) {
		delete lCamera;
		lCamera	= NULL;
	}

	if (NULL != lDocument) {
		delete lDocument;
		lDocument	= NULL;
	}

	if (NULL != lBuffer) {
		delete lBuffer;
		lBuffer	= NULL;
	}

	releaseGrabFrameMem ();
}

void IrtvGrabFrame::releaseGrabFrameMem () 
{
	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); i++) {
		free (m_vGrabFrameMemData[i].m_pData);
		m_vGrabFrameMemData[i].m_pData	= NULL;
	}

	m_vGrabFrameMemData.clear ();
}

const int IrtvGrabFrame::setup () 
{
	clear ();

	if (OK != setupConfig ()) {
		return (NOK);
	}

	if (OK != setupGrabber ()) {
		return (NOK);
	}

	if (OK != createCamera ()) {
		return (NOK);
	}

	if (OK != setupBuffer ()) {
		return (NOK);
	}

#if 0
	//TODO
	if (OK != registerCallback ()) {
		return (NOK);
	}
#endif

	return (OK);
}

const int IrtvGrabFrame::run () 
{
	if (OK != startGrabFrame ()) {
		return (NOK);
	}

	return (OK);
}

const int IrtvGrabFrame::setupConfig () 
{
	kLog (K_INFO, "Step 1: Open the configuration file\n");

	// Step 1:  Open the configuration file.  We create a XML document with the name of the file as parameter.
	// Coyote GUI의 설정 정보를 XML 파일로 저장한 뒤, iocBoot\iocIRTV에 설치
	lDocument = new CyXMLDocument (IRTV_CONFIG_FILE_NAME);

	if (lDocument->LoadDocument() != CY_RESULT_OK) {
		kLog (K_ERR, "LoadDocument failed for %s\n", IRTV_CONFIG_FILE_NAME);
		return (NOK);
	}

	kLog (K_INFO, "Step 2: Create a copnfiguration object and fill it from the XML document\n");

	// Step 2a: Create a configuration object and fill it from the XML document.
	if (lConfig.LoadFromXML (*lDocument) != CY_RESULT_OK) {
		kLog (K_ERR, "LoadFromXML failed for %s\n", IRTV_CONFIG_FILE_NAME);
		return (NOK);
	}

	kLog (K_INFO, "Step 3: Set the configuration on the entry to connect to\n");

	// Step 3a: Set the configuration on the entry to connect to.
	// In this case, we only have one entry, so index 0, is good.
	lConfig.SetIndex( 0 );

	return (OK);
}

const int IrtvGrabFrame::setupGrabber () 
{
	kLog (K_INFO, "Step 4: Connect to the grabber\n");

	CyResult	ret;

	// Step 4: Connect to the grabber.  It will use the currently
	// selected entry in the config object, hence step 3.
	if ( (ret = lGrabber.Connect( lConfig )) != CY_RESULT_OK ) {
		kLog (K_ERR, "lGrabber.Connect failed (%d)\n", ret);
		return (NOK);
	}

	return (OK);
}

const int IrtvGrabFrame::createCamera () 
{
	kLog (K_INFO, "Step 5: Create a camera object on top of the grabber\n");

	// Step 5: Create a camera object on top of the grabber.  This camera
	// object takes care of configuring both the iPORT and the camera.

	// Find the camera type from the configuration
	char lCameraType[128];
	lConfig.GetDeviceType( lCameraType, sizeof( lCameraType ) );

	kLog (K_INFO, "[createCamera] lReg.FindCamera\n");

	// Find the camera in the registry
	CyCameraRegistry lReg;
	if (lReg.FindCamera( lCameraType ) != CY_RESULT_OK) {
		kLog (K_ERR, "[createCamera] lReg.FindCamera failed\n");
		return 1;
	}

	kLog (K_INFO, "[createCamera] lReg.CreateCamera\n");

	// Create the camera.  The previous operation placed the registry 
	// internal settings on the found camera.  The next step will create
	// a camera object of that type.
	lCamera = 0;

	if (lReg.CreateCamera( &lCamera, &lGrabber ) != CY_RESULT_OK) {
		kLog (K_ERR, "[createCamera] lReg.CreateCamera failed\n");
		return 1;
	}

	kLog (K_INFO, "Step 6: Load the camera settings from the XML document\n");

	// Step 6: Load the camera settings from the XML document
	if (lCamera->LoadFromXML( *lDocument ) != CY_RESULT_OK) {
		kLog (K_ERR, "[createCamera] lCamera->LoadFromXML \n");
		return 1;
	}

	return (OK);
}

const int IrtvGrabFrame::setupBuffer () 
{
	kLog (K_INFO, "Step 8:  Create a buffer for grabbing images\n");

	// 동적으로 화면의 크기 변경
	lCamera->SetOffsetX( (UInt16)m_pIRTV->ao_roi_x );
	lCamera->SetOffsetY( (UInt16)m_pIRTV->ao_roi_y );
	lCamera->SetSizeX( (UInt32)m_pIRTV->ao_roi_width );
	lCamera->SetSizeY( (UInt32)m_pIRTV->ao_roi_height + 1 );

	// Local과 Camera에 기록
	lCamera->UpdateToCamera (true);

	// Camera condiguration dialog 실행
	//lCamera->ShowDialog ();

	// Step 8:  Create a buffer for grabbing images.
	// Get some information from the camera
	lCamera->GetSizeX( m_width );
	lCamera->GetSizeY( m_height );
	lCamera->GetDecimationX( lDecimationX );
	lCamera->GetDecimationY( lDecimationY );
	lCamera->GetBinningX( lBinningX );
	lCamera->GetBinningY( lBinningY );
	lCamera->GetEffectivePixelType( lPixelType );	// CyGrayscale8::ID

	if ( ( lDecimationX != 0 ) && ( lDecimationY != 0 ) && ( lBinningX != 0 ) && ( lBinningY != 0 ) ) 		{
		m_width	 = (( m_width / lBinningX ) + (( m_width % lBinningX ) ? 1 : 0));
		m_width  = (( m_width / lDecimationX ) + (( m_width % lDecimationX ) ? 1 : 0));
		m_height = (( m_height / lBinningY ) + (( m_height % lBinningY ) ? 1 : 0));
		m_height = (( m_height / lDecimationY ) + (( m_height % lDecimationY ) ? 1 : 0));
	}

	kLog (K_MON, "X(%d) Y(%d) W(%d) H(%d) type(%d)\n", lBinningX, lBinningY, m_width, m_height, lPixelType);

	if ( (0 == m_width) || (0 == m_height) ) {
		kLog (K_ERR, "[IrtvGrabFrame::setupBuffer] Size is invalid. x(%d) y(%d)\n", m_width, m_height);
		return (NOK);
	}

	// Create the buffer.
	lBuffer = new CyImageBuffer (m_width, m_height, lPixelType);

	if (NULL == lBuffer) {
		return (NOK);
	}

	lBuffer->SetQueueSize (IRTV_FRAME_BUF_QUEUE_SIZE);
	lBuffer->SetQueueMode (false);

	return (OK);
}

const int IrtvGrabFrame::registerCallback () 
{
	kLog (K_INFO, "register callback for GPIO \n");

	lGrabber.GetDevice().RegisterCallback2( CyDevice::CB_ON_INTERRUPT_GPIO,
								  (CyDevice::Callback2)CallbackFunction,
								   reinterpret_cast<void*>( NULL ) );

	return (OK);
}

// grab images
const int IrtvGrabFrame::startGrabFrame () 
{
	kLog (K_MON, "[IrtvGrabFrame::startGrabFrame] Grab an image \n");

	// Clear the buffer queue
	lBuffer->ClearQueue ();

	// LockForRead() failed (18)에 따라, Arming 시 호출하도록 순서를 조정함
	if (lCamera->StartGrabbing (CyChannel(0), *lBuffer) != CY_RESULT_OK ) {
		delete lCamera;
		lCamera	= NULL;
		return (NOK);
	}

	kLog (K_MON, "[IrtvGrabFrame::startGrabFrame] ready to grab \n");

	unsigned char *	lPtr = NULL;
	unsigned long	nFrameDataSize;

	CyResult ret;

	while (isRunGrabFrame ()) {
		kLog (K_DEL, "[startGrabFrame] Getting the buffer pointer from the CyBuffer class \n");

		if ( CY_RESULT_OK != (ret = lBuffer->LockForRead ( (void**) &lPtr, &nFrameDataSize, CyBuffer::FLAG_NO_WAIT)) ) {
			kLog (K_ERR, "[startGrabFrame] lBuffer->LockForRead failed (%d)\n", ret);
			continue;
		}

		if ( (NULL == lPtr) || (0 >= nFrameDataSize) ) {
			kLog (K_ERR, "[startGrabFrame] grab frame is invalid. nFrameDataSize(%d)\n", nFrameDataSize);
			continue;
		}

		incGrabFrameCnt ();

		kLog (K_INFO, "[startGrabFrame] Grab : cnt(%03d) timestamp(%lu) size(%d)\n", 
			getGrabFrameCnt(), lBuffer->GetReadTimestamp(), nFrameDataSize);

		if (true == isStoreGrabFrame ()) {
			// 취득한 이미지를 메모리에 저장함
			storeGrabFrame (lPtr, nFrameDataSize);
		}

		// 취득한 이미지의 헤더를 이용하여 Camera 상태를 update
		updateGrabStatus (lPtr, nFrameDataSize);

		// 취득한 이미지의 RAW 데이터를 이용하여 Calibration을 통해 온도 측정: 실제 Window Size
		applyCalibration (m_width, m_height - 1, lPtr);

		// 취득한 이미지를 공유메모리에 기록함
		loadGrabFrame (lPtr, nFrameDataSize);

		lBuffer->SignalReadEnd ();

		if (true == isStoreGrabFrame ()) {
			if (getGrabFrameCnt() >= getTotSamps ()) {
				kLog (K_MON, "[startGrabFrame] total(%03d) : grab was completed !!!\n", getGrabFrameCnt());

				// Frame 취득 완료 설정
				setRunGrabFrame (FALSE);
				break;
			}
		}
	}

	kLog (K_INFO, "[startGrabFrame] total(%03d) vector(%d) \n", getGrabFrameCnt(), m_vGrabFrameMemData.size());

	lCamera->StopGrabbing (CyChannel (0));

	if (NULL != m_rawFp) {
		fclose (m_rawFp);
		m_rawFp	= NULL;
	}

	return (OK);
}

const int IrtvGrabFrame::stop () 
{
	kLog (K_INFO, "Stop grab image \n");

	lGrabber.Disconnect();

	if (NULL != lCamera) {
		delete lCamera;
		lCamera	= NULL;
	}

	return (OK);
}

const int IrtvGrabFrame::loadGrabFrame (unsigned char *data, unsigned int size) 
{
	if (NULL == data) {
		return (NOK);
	}

#if 0
	kLog (K_DEBUG, "[IrtvGrabFrame.loadGrabFrame] width(%d) height(%d) size(%d)\n", m_width, m_height, size);

	m_grabFrameData.setFrameData (m_width, m_height, data);

	gGrabFrameShm.write (m_grabFrameData);

	//if (true == isLiveRefresh ()) {
	//	processLiveData (m_width, m_height, data);
	//}
#else
	kLog (K_DEBUG, "[IrtvGrabFrame.loadGrabFrame] width(%d) height(%d) size(%d)\n", m_width, m_height, size);

	int	nHdrSize	= IRTV_GET_HEADER_SIZE (m_width);

	m_grabFrameData.setFrameData (m_width, m_height - 1, data + nHdrSize);

	gGrabFrameShm.write (m_grabFrameData);

	//if (true == isLiveRefresh ()) {
	//	processLiveData (m_width, m_height - 1, data + nHdrSize);
	//}
#endif

	return (OK);
}

const bool IrtvGrabFrame::isLiveRefresh () 
{
	kLog (K_INFO, "[IrtvGrabFrame.isLiveRefresh] cnt(%d) rate(%f) refresh(%f)\n", 
		getGrabFrameCnt (), m_pIRTV->sample_rate, m_pIRTV->ao_refresh_time);

	if ( (0 == m_pIRTV->sample_rate) || (0 == m_pIRTV->ao_refresh_time) ) {
		return (false);
	}

	int	interval	= (int)( ((int)m_pIRTV->sample_rate) * m_pIRTV->ao_refresh_time / 1000);

	if ( (0 == interval)  || ! (getGrabFrameCnt () % interval) ) {
		return (true);
	}
	
	return (false);
}

const int IrtvGrabFrame::saveToTIFF (void *data, unsigned short cols, unsigned rows, bool bCompress) 
{
	if (NULL == data) {
		return (NOK);
	}

	kLog (K_INFO, "[IrtvGrabFrame.saveToTIFF] cols(%d) rows(%d)\n", cols, rows);

	// 파일로 저장
	IrtvTiff	tiffObj;

	if (0 > tiffObj.open (0, IRTV_LIVE_GRAB_FRAME_TMP_PATH, cols, rows, false, bCompress)) {
		return (NOK);
	}

	tiffObj.write ((UInt16 *)data);
	tiffObj.close ();

	remove (IRTV_LIVE_GRAB_FRAME_PATH);
	rename (IRTV_LIVE_GRAB_FRAME_TMP_PATH, IRTV_LIVE_GRAB_FRAME_PATH);

	kLog (K_INFO, "[IrtvGrabFrame.saveToTIFF] cols(%d) rows(%d) end ...\n", cols, rows);

	return (OK);
}

const int IrtvGrabFrame::saveToRAW (void *data, unsigned short cols, unsigned rows) 
{
	if (NULL == data) {
		return (NOK);
	}

	kLog (K_INFO, "[IrtvGrabFrame.saveToRAW] cols(%d) rows(%d)\n", cols, rows);

	FILE	*fp = NULL;

	if (NULL == (fp = fopen (IRTV_LIVE_GRAB_RAW_TMP_PATH, "wb"))) {
		printf("Can't open file %s\n", IRTV_LIVE_GRAB_FRAME_TMP_PATH);
		return (NOK);
	}

	fwrite (data, 2, cols * rows, fp);
	fclose (fp);

	remove (IRTV_LIVE_GRAB_RAW_PATH);
	rename (IRTV_LIVE_GRAB_RAW_TMP_PATH, IRTV_LIVE_GRAB_RAW_PATH);

	kLog (K_INFO, "[IrtvGrabFrame.saveToRAW] cols(%d) rows(%d) end ...\n", cols, rows);

	return (OK);
}

const int IrtvGrabFrame::storeGrabFrame (unsigned char *data, unsigned int size) 
{
	if (NULL == data) {
		return (NOK);
	}

#if 1
	// 4 ~ 6ms 소요됨 : 일부 지연되어도 일정 수량에 대해 버퍼링됨. IRTV_FRAME_BUF_QUEUE_SIZE
	if (NULL == m_rawFp) {
		createFile (true);
	}

	if (NULL != m_rawFp) {
		fwrite (data, 1, size, m_rawFp);

		kLog (K_DEBUG, "[IrtvGrabFrame.storeGrabFrame] size = %d\n", size);
	}
#endif

	//TODO WORK
#if 0
	// 메모리에 저장
	IrtvGrabFrameMemInfo	rec;
	
	rec.m_nSize	= size;
	rec.m_pData = (void *)malloc(size);

	if (NULL == rec.m_pData) {
		kLog (K_ERR, "[IrtvGrabFrame.storeGrabFrame] malloc failed\n");
		return (NOK);
	}

	memcpy (rec.m_pData, data, size);

	m_vGrabFrameMemData.push_back (rec);
#endif

	return (OK);
}

const int IrtvGrabFrame::generateImageFiles () 
{
#if 1
	// RAW 파일로부터 SEQ 파일 생성
	if (OK != convertRawToSEQ ()) {
		return (NOK);
	}

	// RAW 파일로부터 TIFF 파일 생성
	if (OK != convertRawToTIFF ()) {
		return (NOK);
	}

#if 0
	// RAW 파일로부터 AVI 파일 생성
	if (OK != convertRawToAVI ()) {
		return (NOK);
	}
#endif
#endif

#if 0
	// 메모리로부터 SEQ 파일 생성
	if (OK != convertMemToSEQ ()) {
		return (NOK);
	}

	// 메모리로부터 TIFF 파일 생성
	if (OK != convertMemToTIFF ()) {
		return (NOK);
	}

#if 0
	// 메모리로부터 AVI 파일 생성
	if (OK != convertMemToAVI ()) {
		//return (NOK);
	}
#endif
#endif

	return (OK);
}

const int IrtvGrabFrame::convertRawToTIFF () 
{
	// 파일 열기
	char	rawFileName[256];
	char	tifFileName[256];
	FILE	*fp		= NULL;
	int		size	= calcBuffSize ((int)m_pIRTV->ao_roi_width, (int)m_pIRTV->ao_roi_height);
	int		nCnt;

	kLog (K_MON, "[RawToTIFF] size(%d) width(%.0f) height(%.0f)\n", 
		size, m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	makeRawFilePath	(rawFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0);

	if (NULL == (fp = fopen (rawFileName, "rb"))) {
		kLog (K_ERR, "[IrtvGrabFrame.convertRawToTIFF] create failed : %s\n", rawFileName);
		return (NOK);
	}

	// 파일로 저장
#if 0
	for (int i = 1; ; i++) {
		// 파일로부터 RAW Data 읽기
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data를 TIFF 파일로 변환
		makeTiffFilePath (tifFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, i);
		irtvSaveTIFF (tifFileName, m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height + 1, (UInt16 *)m_convertBuf, 0);
	}
#else
	makeTiffFilePath (tifFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, 0);

	IrtvTiff	tiffObj;

	//if (0 > tiffObj.open (tifFileName, (UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height, true, true)) {
	if (0 > tiffObj.open (m_pIRTV->shotNumber, tifFileName, 
			(UInt16)m_pIRTV->ai_sts_width, (UInt16)m_pIRTV->ai_sts_height, true, true)) {
		kLog (K_ERR, "[IrtvGrabFrame.convertRawToTIFF] tiff open failed : %s\n", tifFileName);
		fclose (fp);
		return (NOK);
	}

	for (nCnt = 0; ; nCnt++) {
		// 파일로부터 RAW Data 읽기
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data를 이용하여 TIFF 파일 생성
		tiffObj.write ((UInt16 *)m_convertBuf);
	}

	tiffObj.close ();
#endif

	fclose (fp);
	fp	= NULL;

	kLog (K_MON, "[RawToTIFF] %s created !!!\n", tifFileName);

	return (OK);
}

const int IrtvGrabFrame::convertMemToTIFF () 
{
	// 파일 열기
	char	tifFileName[256];

	kLog (K_MON, "[MemToTIFF] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	// 폴더 생성
	createFile (false);

	makeTiffFilePath (tifFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, 0);

	IrtvTiff	tiffObj;

	if (0 > tiffObj.open (m_pIRTV->shotNumber, tifFileName, 
			(UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height, true, true)) {
		kLog (K_ERR, "[IrtvGrabFrame.convertMemToTIFF] tiff open failed : %s\n", tifFileName);
		return (NOK);
	}

	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		// Vector 내의 RAW Data를 이용하여 TIFF 파일 생성
		tiffObj.write ((UInt16 *)m_vGrabFrameMemData[i].m_pData);
	}

	tiffObj.close ();

	kLog (K_MON, "[MemToTIFF] %s created !!!\n", tifFileName);

	return (OK);
}

const int IrtvGrabFrame::convertRawToAVI () 
{
	// 파일 열기
	char	rawFileName[256];
	char	aviFilePath[256];
	char	bmpPathName[256];
	char	bmpFilePath[256];
	char	firstBMPPathName[256];
	FILE	*fp		= NULL;
	int		size	= calcBuffSize ((int)m_pIRTV->ao_roi_width, (int)m_pIRTV->ao_roi_height);
	//int		nCnt;

	kLog (K_MON, "[RawToAVI] size(%d) width(%.0f) height(%.0f)\n", 
		size, m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	makeRawFilePath	(rawFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0);

	if (NULL == (fp = fopen (rawFileName, "rb"))) {
		kLog (K_ERR, "[IrtvGrabFrame.convertRawToAVI] create failed : %s\n", rawFileName);
		return (NOK);
	}

	makeAviFilePath (aviFilePath, m_pIRTV->shotNumber);

	// BMP 폴더 생성
	makeBMPPath (bmpPathName, m_pIRTV->shotNumber);
	makeDir (bmpPathName, true);

	IrtvBMP	*pBMP	= new IrtvBMP ();

	if (NULL == pBMP) {
		return (NOK);
	}

	if (0 > pBMP->open (m_pIRTV->shotNumber, (UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height)) {
		delete (pBMP);

		kLog (K_ERR, "[RawToAVI] AVI open failed : %s\n", aviFilePath);
		return (NOK);
	}

	// 파일로 저장
	for (int i = 1; ; i++) {
		// 파일로부터 RAW Data 읽기
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data를 BMP 파일로 변환
		makeBMPFilePath (bmpFilePath, m_pIRTV->shotNumber, i);
		pBMP->write (bmpFilePath, (UInt16 *)m_convertBuf);
	}

	fclose (fp);
	fp	= NULL;

	pBMP->close ();

	kLog (K_MON, "[RawToAVI] %s : BMP created !!!\n", aviFilePath);

	// vdub을 이용하여 BMP 파일들로부터 AVI 생성

	char	cmd[256];

	makeBMPFilePath (firstBMPPathName, m_pIRTV->shotNumber, 1);

	sprintf (cmd, "%s\\vdub /s %s\\kstar_irtv.vcf /p %s %s /r",
		IRTV_AVI_VDUB_HOME, IRTV_AVI_VDUB_HOME, firstBMPPathName, aviFilePath);

	kLog (K_MON, "[RawToAVI] cmd(%s)\n", cmd);

	system (cmd);

	kLog (K_MON, "[RawToAVI] %s : AVI created !!!\n", aviFilePath);

	delete (pBMP);

	kLog (K_MON, "[RawToAVI] %s created !!!\n", aviFilePath);

	return (OK);
}

const int IrtvGrabFrame::convertMemToAVI () 
{
	char	aviFilePath[256];
	char	bmpPathName[256];
	char	bmpFilePath[256];
	char	firstBMPPathName[256];

	kLog (K_MON, "[MemToAVI] shot(%d) width(%.0f) height(%.0f)\n", 
		m_pIRTV->shotNumber, m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	makeAviFilePath (aviFilePath, m_pIRTV->shotNumber);

	// BMP 폴더 생성
	makeBMPPath (bmpPathName, m_pIRTV->shotNumber);
	makeDir (bmpPathName, true);

	IrtvBMP	*pBMP	= new IrtvBMP ();

	if (NULL == pBMP) {
		return (NOK);
	}

	if (0 > pBMP->open (m_pIRTV->shotNumber, (UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height)) {
		delete (pBMP);

		kLog (K_ERR, "[MemToAVI] AVI open failed : %s\n", aviFilePath);
		return (NOK);
	}

	// Vector 내의 RAW Data를 이용하여 BMP 파일 생성
	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		makeBMPFilePath (bmpFilePath, m_pIRTV->shotNumber, i + 1);
		pBMP->write (bmpFilePath, (UInt16 *)m_vGrabFrameMemData[i].m_pData);
	}

	pBMP->close ();

	kLog (K_MON, "[MemToAVII] %s : BMP created !!!\n", aviFilePath);

	// vdub을 이용하여 BMP 파일들로부터 AVI 생성

	char	cmd[256];

	makeBMPFilePath (firstBMPPathName, m_pIRTV->shotNumber, 1);

	sprintf (cmd, "%s\\vdub /s %s\\kstar_irtv.vcf /p %s %s /r",
		IRTV_AVI_VDUB_HOME, IRTV_AVI_VDUB_HOME, firstBMPPathName, aviFilePath);

	kLog (K_MON, "[MemToAVI] cmd(%s)\n", cmd);

	system (cmd);

	kLog (K_MON, "[MemToAVI] %s : AVI created !!!\n", aviFilePath);

	delete (pBMP);

	return (OK);
}

const int IrtvGrabFrame::convertMemToSEQ () 
{
	// Calibration 정보 얻기
	iacfCalib	calib	= irtvGetCalibration ();

	// 파일 열기
	char	seqFileName[256];

	kLog (K_MON, "[MemToSEQ] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	// 폴더 생성
	createFile (false);

	if (NULL == calib) {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "sfmov");
	}
	else {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "seq");
	}

	IrtvFileSEQ		irtvFile;

	// SEQ 파일 생성
	if (0 > irtvFile.open (m_pIRTV->shotNumber, 
			(UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height, seqFileName, calib)) {
		kLog (K_ERR, "[IrtvGrabFrame.convertMemToSEQ] SEQ open failed : %s\n", seqFileName);
		return (NOK);
	}

	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		// Vector 내의 RAW Data를 이용하여 파일 생성
		if (0 > irtvFile.write ((UInt16 *)m_vGrabFrameMemData[i].m_pData)) {
			irtvFile.close ();

			kLog (K_ERR, "[IrtvGrabFrame.convertMemToSEQ] SEQ write failed : %s\n", seqFileName);
			return (NOK);
		}
	}

	irtvFile.close ();

	kLog (K_MON, "[MemToSEQ] %s created !!!\n", seqFileName);

	return (OK);
}

const int IrtvGrabFrame::convertRawToSEQ () 
{
	// Calibration 정보 얻기
	iacfCalib	calib	= irtvGetCalibration ();

	// 파일 열기
	char	rawFileName[256];
	char	seqFileName[256];
	char	incFileName[256];

	kLog (K_MON, "[RawToSEQ] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	makeRawFilePath	(rawFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0);

	if (NULL == calib) {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "sfmov");
	}
	else {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "seq");
	}

	IrtvFileSEQ		irtvFile;

	irtvFile.convert ((int)m_pIRTV->ao_roi_width, (int)m_pIRTV->ao_roi_height, rawFileName, seqFileName, calib);

	if (NULL == calib) {
		makeShotImagePath (incFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "inc");

		CopyFile (IRTV_CALIB_INC_FILE_NAME, incFileName, 0);
	}

	kLog (K_MON, "[RawToSEQ] %s created !!!\n", seqFileName);

	return (OK);
}

const int IrtvGrabFrame::createFile (const bool bCreateRawFile) 
{
	char	path[256];
	char	rawFileName[256];

	if (OK != makeDir (m_pIRTV->so_image_path, FALSE)) {
		return (NOK);
	}

	sprintf (path , "%s\\%06d", m_pIRTV->so_image_path, m_pIRTV->shotNumber);

	if (OK != makeDir (path, TRUE)) {
		return (NOK);
	}

	if (true == bCreateRawFile) {
		makeRawFilePath	(rawFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0);

		if (NULL == (m_rawFp = fopen (rawFileName, "wb+"))) {
			kLog (K_ERR, "[IrtvGrabFrame.createFile] create failed : %s\n", rawFileName);
			return (NOK);
		}

		kLog (K_MON, "[IrtvGrabFrame.createFile] %s was created\n", rawFileName);
	}

	return (OK);
}

const int IrtvGrabFrame::makeDir (const char *pDirName, bool bForced) 
{
	if (NULL == pDirName) {
		return (NOK);
	}

	kLog (K_INFO, "[IrtvGrabFrame.makeDir] dir(%s) bForced(%d)\n", pDirName, bForced);

	// exist
	if (0 == access (pDirName, 0x00)) {
		return (OK);
	}

	int		oldMode	= umask (0);

	// make directory
	if (0 > _mkdir (pDirName)) {
		umask (oldMode);

		kLog (K_ERR, "[IrtvGrabFrame::makeDir] %s create failed : %s\n", pDirName, strerror(errno));
		return (NOK);
	}

	umask (oldMode);

	return (OK);
}

void IrtvGrabFrame::setIRTVAddr (ST_IRTV *pIRTV) 
{
	m_pIRTV	= pIRTV;
}

#if 0
const int IrtvGrabFrame::processLiveData (unsigned short cols, unsigned int rows, void *data)
{
#if 0
	saveToTIFF (data, cols, rows);
#else
	gLiveFrameData.setFrameData (cols, rows, (unsigned char *)data);

	kLog (K_DEBUG, "[IrtvGrabFrame::processLiveData] send signal to LiveThr \n");

#if 0	// 공유메모리에 저장하는 관계로 미사용
	// threadFunc_LiveDataThr 에게 LiveData 처리 요청
	gLiveFrameDataSync.signal ();
#endif
#endif

	return (OK);
}
#endif

void IrtvGrabFrame::setCameraStatus (ST_IRTV *pIRTV, void *data) 
{
	if ( (NULL == pIRTV) || (NULL == data) ) {
		return;
	}
	
	iacfSHeader		header;

	iacfDecodeHeader (&header, data);

	setCameraStatusWithHeader (pIRTV, &header);

	scanIoRequest (pIRTV->ioScanPvt_userCall);
}

void IrtvGrabFrame::setCameraStatusWithHeader (ST_IRTV *pIRTV, iacfSHeader *pHeader) 
{
	// 온도 정보가 정확하지 않아, 실시간 업데이트가 큰 의미 없음
#if 0
	if ( (NULL == pIRTV) || (NULL == pHeader) ) {
		return;
	}
	
	// Camera CCD의 온도가 아니므로, 업데이트를 생략함
	//pIRTV->ai_sts_fpa_temp	= pHeader->Temp;
	
	pIRTV->ai_sts_preset	= pHeader->ActivePreset;
	pIRTV->ai_sts_int_time	= pHeader->IntegrationTime;

#endif
	printCameraHeader (pHeader);
}

void IrtvGrabFrame::printCameraHeader (iacfSHeader *pHeader) 
{
	kLog (K_DEBUG, "temp(%f, %f, %f, %f) Resolution(%d) \n",
		pHeader->Temp, pHeader->FrontPanelTemp, pHeader->AirGapTemp, pHeader->DewarTemp,
		pHeader->Resolution);

	kLog (K_DEBUG, "counter(%d) intTime(%f) irig(%d %02d:%02d:%02d.%03d.%03d) \n",
		pHeader->FrameCounter, pHeader->IntegrationTime,
		pHeader->IRIGTime.Day, pHeader->IRIGTime.Hour, pHeader->IRIGTime.Minute, pHeader->IRIGTime.Second,
		pHeader->IRIGTime.Millisecond, pHeader->IRIGTime.Microsecond);
}

const bool IrtvGrabFrame::isRunGrabFrame () 
{ 
	return (1 == m_pIRTV->bRunGrabFrame); 
}

void IrtvGrabFrame::setRunGrabFrame (const bool bRunGrabFrame) 
{ 
	m_pIRTV->bRunGrabFrame	= bRunGrabFrame; 
}

void IrtvGrabFrame::setGrabFrameCnt (const unsigned int nGrabFrameCnt) 
{ 
	m_nGrabFrameCnt = nGrabFrameCnt; 
} 

void IrtvGrabFrame::incGrabFrameCnt () 
{ 
	++m_nGrabFrameCnt; 
	m_pIRTV->nGrabFrameCnt	= m_nGrabFrameCnt;
}

const int IrtvGrabFrame::getGrabFrameCnt () 
{ 
	return (m_nGrabFrameCnt); 
}

void IrtvGrabFrame::updateGrabStatus (unsigned char *data, unsigned int size) 
{
	setCameraStatus (m_pIRTV, data);
}

iacfSObjectParameters *IrtvGrabFrame::getObjParam () 
{
	if (NULL == pObjParam) {
		pObjParam = (iacfSObjectParameters *)malloc(sizeof(iacfSObjectParameters));

		iacfCalibDefaultObjectParameters (pObjParam);
	}

	return (pObjParam);
}

int IrtvGrabFrame::applyCalibration (const int width, const int height, unsigned char *data) 
{
	//TODO TEMP : 추후 온도 측정이 필요할 경우 사용할 예정임
#if 0
	kLog (K_INFO , "[applyCalib] start... \n");
	
	// Calibration 정보 얻기
	iacfCalib				calib	= irtvGetCalibration ();
	iacfSObjectParameters	*param	= getObjParam ();
	unsigned short			*frame	= (unsigned short *)data;
	unsigned int			size	= width * height * sizeof(float);
	float					*reduce = (float *)malloc(size);

	kLog (K_INFO, "[applyCalib] w(%d) h(%d) size(%d)\n", width, height, size);
	kLog (K_INFO, "[applyCalib] %f %f %f %f %f %f %f %f\n", 
		param->atmosphereTemp, param->distance, param->emissivity, param->estAtmosphericTransmission,
		param->extOpticsTemp, param->extOpticsTransmission, param->reflectedTemp, param->relativeHumidity);

	int ret = iacfCalibApply (calib, frame, width, height, iacfuntTemperatureC, param, reduce, size);

	kLog (K_INFO, "[applyCalib] [0, 0] Raw(%4d) Temp(%f) \n", frame[640+0], reduce[0]);
	kLog (K_INFO, "[applyCalib] [1, 0] Raw(%4d) Temp(%f) \n", frame[640+1], reduce[1]);
	kLog (K_INFO, "[applyCalib] [2, 0] Raw(%4d) Temp(%f) \n", frame[640+2], reduce[2]);
	kLog (K_INFO, "[applyCalib] [3, 0] Raw(%4d) Temp(%f) \n", frame[640+3], reduce[3]);
	kLog (K_INFO, "[applyCalib] [4, 0] Raw(%4d) Temp(%f) \n", frame[640+4], reduce[4]);
	kLog (K_INFO, "[applyCalib] [5, 0] Raw(%4d) Temp(%f) \n", frame[640+5], reduce[5]);

	free (reduce);
#endif

	return (OK);
}

char *IrtvGrabFrame::makeShotImagePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum, const char *extName) 
{
	if (NULL != pathName) {
		if (NULL != extName) {
			sprintf (pathName, "%s\\%06d\\%s.%s", imagePath, shotNumber, makeFileNameWithPostfix(shotNumber), extName);
		}
		else {
			sprintf (pathName, "%s\\%06d\\%s", imagePath, shotNumber, makeFileNameWithPostfix(shotNumber));
		}
	}

	return (pathName);
}

char *IrtvGrabFrame::makeRawFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum) 
{
	return (makeShotImagePath (pathName, imagePath, shotNumber, seqNum, "raw"));
}

char *IrtvGrabFrame::makeTiffFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum, const int pageNo) 
{
	return (makeShotImagePath (pathName, imagePath, shotNumber, seqNum, "tif"));
}

char *IrtvGrabFrame::makeSeqFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum) 
{
	return (makeShotImagePath (pathName, imagePath, shotNumber, seqNum, "seq"));
}

char *IrtvGrabFrame::makeAviPath (char *pathName, const int shotNumber) 
{
	if (NULL != pathName) {
		sprintf (pathName, "%s", IRTV_AVI_PATH);
	}

	return (pathName);
}

char *IrtvGrabFrame::makeAviFilePath (char *pathName, const int shotNumber) 
{
	if (NULL != pathName) {
		char	tmp[256];
		makeAviPath (tmp, shotNumber);
		sprintf (pathName, "%s\\%s.avi", tmp, makeFileNameWithPostfix(shotNumber));
	}

	return (pathName);
}

char *IrtvGrabFrame::makeBMPPath (char *pathName, const int shotNumber) 
{
	if (NULL != pathName) {
		sprintf (pathName, "%s\\%06d", IRTV_BMP_PATH, shotNumber);
	}

	return (pathName);
}

char *IrtvGrabFrame::makeBMPFilePath (char *pathName, const int shotNumber, const int seqNum) 
{
	if (NULL != pathName) {
		char	tmp[256];
		makeBMPPath (tmp, shotNumber);
		sprintf (pathName, "%s\\%s-%05d.bmp", tmp, makeFileNameWithPostfix(shotNumber), seqNum);
	}

	return (pathName);
}

char *IrtvGrabFrame::makeFileNameWithPostfix (const int shotNumber) 
{
	static char	szFileName[256];
	sprintf (szFileName, "%06d_%s", shotNumber, sfwEnvGet("IRTV_IMAGE_POSTFIX"));

	return (szFileName);
}

const bool IrtvGrabFrame::isStoreGrabFrame () 
{
	return (TRUE == m_pIRTV->bStoreGrabFrame);
}
