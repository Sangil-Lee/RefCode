#include "dbScan.h"		// for scanIoRequest()

#include "irtvGrabFrame.h"
#include "irtvGrabFrameData.h"

#define	USE_GRAB_FRAME_RAW		1	// GrabFrame()�� ����ϴ� ���. SEQ ���� ���� �� convert() �̿�
#define USE_GRAB_FRAME_ORIGINAL	0	// GrabFrameOriginal()�� ����ϴ� ���. SEQ ���� ���� �� write() �̿�

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

	m_pIRTV			= NULL;
	m_rawFp			= NULL;
	m_nGrabFrameCnt	= 0;
	m_width			= 0;
	m_height		= 0;

	return (OK);
}

void IrtvGrabFrame::clear () 
{
	kLog (K_MON, "[IrtvGrabFrame.clear] release resources ... \n");

	m_nGrabFrameCnt	= 0;
	m_width			= 0;
	m_height		= 0;

	if (NULL != m_rawFp) {
		fclose (m_rawFp);
		m_rawFp	= NULL;
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

	return (OK);
}

const int IrtvGrabFrame::setupGrabber () 
{
	kLog (K_INFO, "Step 4: Connect to the grabber\n");

	return (OK);
}

const int IrtvGrabFrame::createCamera () 
{
	kLog (K_INFO, "Step 5: Create a camera object on top of the grabber\n");

	return (OK);
}

const int IrtvGrabFrame::setupBuffer () 
{
	kLog (K_INFO, "Step 8:  Create a buffer for grabbing images\n");

	UInt16 imWidth, imHeight;

	// ���� ǥ��
	IrtvCommand::getInstance().camera.GetFrameSize (m_width, m_height, m_pixelType);
	m_frameSize = m_width * m_height * PixelWidth(m_pixelType);

	kLog (K_MON, "[IrtvGrabFrame::setupBuffer] Get : width(%d) height(%d) pixelType(0x%x) size(%d)\n", 
					m_width, m_height, m_pixelType, m_frameSize);

	IrtvCommand::getInstance().camera.GetImageSize (imWidth, imHeight);

	kLog (K_MON, "[IrtvGrabFrame::setupBuffer] Get : Image(%d/%d)\n", imWidth, imHeight);

	//kLog (K_MON, "[IrtvGrabFrame::setupBuffer] Set : x(%f) y(%f) pixelType(0x%x) \n", 
	//				m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height, m_pixelType);
	//m_command.camera.SetFrameSize (m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height, m_pixelType);
	//m_command.camera.SetUnit (unitTemperatureFactory, TT_Celsius);
	//m_command.camera.SetUnit (unitTemperatureUser, TT_Celsius);

	//m_command.camera.FireAvailablePresetsChanged ();
	//m_command.camera.FireFrameSizeChanged ();

	if ( (0 == m_width) || (0 == m_height) ) {
		kLog (K_ERR, "[IrtvGrabFrame::setupBuffer] Size is invalid. x(%d) y(%d)\n", m_width, m_height);
		return (NOK);
	}

	return (OK);
}

// grab images
const int IrtvGrabFrame::startGrabFrame () 
{
	kLog (K_MON, "[IrtvGrabFrame::startGrabFrame] Grab an image \n");

#if 0	// �׽�Ʈ �ڵ� : Sequence count ����� ���� ������
	if (true == isStoreGrabFrame ()) {
		// Sets the number of sequences output in triggered sequence mode.
		IrtvCommand::getInstance().setTriggerSeqCount (getTotSamps ());
	}
#endif

	//	go into acquire mode (default mode is realtime)
	if (!IrtvCommand::getInstance().camera.StartAcquire ()) {
		IrtvCommand::getInstance().camera.Disconnect();
		kLog (K_ERR, "[startGrabFrame] Can't StartAcquire().\n");
		return (NOK);
	}

	unsigned char *	lPtr = NULL;
	unsigned long	nFrameDataSize;

#if 1
	// ----------------------------------------------------------------------------------------------------------
	// Trigger�� ���� ��� ���� ���� ���, �������� ���� �־� ���� Shot�� ���������� ��Ⱥ�Ǿ� ����
	// �̿� ����, Ʈ���� ����(?)�� ���� �������� �о� ������ �۾��� ������
	// ----------------------------------------------------------------------------------------------------------

	// 1. Ʈ���� ����
	IrtvCommand::getInstance().sendRearmTrigger ();

	// 2. ª�� �ð��� Ÿ�Ӿƿ��� �߻��� ������ ���� �ִ� �������� ��� �о ����. Freerun�� ����Ͽ� 5msec�� ������
	IrtvCommand::getInstance().camera.Timeout = 5;	// msec

	while (true) {
		if (NULL == (lPtr = (unsigned char *)IrtvCommand::getInstance().camera.GrabFrame (nFrameDataSize))) {
			kLog (K_MON, "[startGrabFrame] skip frames were finised : timeout ... \n");
			break;
		}

		IrtvCommand::getInstance().camera.ReleaseFrame(lPtr);

		kLog (K_MON, "[startGrabFrame] skip a frame ... \n");
	}

	// Ÿ�Ӿƿ��� ������ ������ �缳��
	IrtvCommand::getInstance().camera.Timeout = IRTV_GRAP_TIMEOUT_MSEC;
#endif

	kLog (K_MON, "[IrtvGrabFrame::startGrabFrame] ready to grab \n");

	int				numTimeouts = 0;
	epicsUInt32		ipFaultStartTime = 0;
	SImageData		image;
	UInt8			preset = 0;

	while (isRunGrabFrame ()) {
		kLog (K_DEL, "[startGrabFrame] Getting the buffer pointer from the CyBuffer class \n");

#if USE_GRAB_FRAME_RAW
		if (NULL == (lPtr = (unsigned char *)IrtvCommand::getInstance().camera.GrabFrame (nFrameDataSize))) {
#elif USE_GRAB_FRAME_ORIGINAL
		if (NULL == (lPtr = (unsigned char *)IrtvCommand::getInstance().camera.GrabFrameOriginal (image, preset))) {
#endif
			kLog (K_ERR, "[startGrabFrame] grab frame is invalid. timeout ...\n");

			if (IRTV_GRAP_TIMEOUT_CNT <= ++numTimeouts) {
				kLog (K_ERR, "[startGrabFrame] Got too many timeouts, giving up.\n");

				// Frame ��� �Ϸ� ����
				setRunGrabFrame (FALSE);
				break;
			}

			continue;
		}

		numTimeouts	= 0;

#if USE_GRAB_FRAME_ORIGINAL
		lPtr			= image.data.pUInt8;
		nFrameDataSize	= m_frameSize;
#endif

		if (m_frameSize != nFrameDataSize) {
			IrtvCommand::getInstance().camera.ReleaseFrame(lPtr);
			kLog (K_ERR, "[startGrabFrame] size doesn't match : size(%d) != nFrameDataSize(%d)\n", m_frameSize, nFrameDataSize);
			break;
		}

		incGrabFrameCnt ();

		if (1 == getGrabFrameCnt()) {
			kLog (K_MON, "[startGrabFrame] Grab : cnt(%03d) size(%d) : grabbing ...\n", getGrabFrameCnt(), nFrameDataSize);
		}
		else {
			kLog (K_INFO, "[startGrabFrame] Grab : cnt(%03d) size(%d)\n", getGrabFrameCnt(), nFrameDataSize);
		}

		if (true == isStoreGrabFrame ()) {
			// ����� �̹����� �޸𸮿� ������
			storeGrabFrame (lPtr, nFrameDataSize);
		}

		// ����� �̹����� ����� �̿��Ͽ� Camera ���¸� update
		updateGrabStatus (lPtr, nFrameDataSize);

		// ����� �̹����� RAW �����͸� �̿��Ͽ� Calibration�� ���� �µ� ����: ���� Window Size
		applyCalibration (m_width, m_height - 1, lPtr);

		// ����� �̹����� �����޸𸮿� �����
		loadGrabFrame (lPtr, nFrameDataSize);

#if USE_GRAB_FRAME_RAW
		IrtvCommand::getInstance().camera.ReleaseFrame(lPtr);
#endif

		if (true == isStoreGrabFrame ()) {
			if (TRUE == isIpMinFault ()) {
				// Ip min. fault �߻� �� �� 1�� ���� �߰��� �̹��� ���
				if (0 == ipFaultStartTime) {
					ipFaultStartTime	= kEpochTimes ();
				}
				else if (kEpochTimes () >= (ipFaultStartTime + 2)){
					kLog (K_MON, "[startGrabFrame] total(%03d) : grab was stopped by Ip min. fault!!!\n", getGrabFrameCnt());

					// Frame ��� �Ϸ� ����
					setRunGrabFrame (FALSE);
					break;
				}
			}

			if (getGrabFrameCnt() >= getTotSamps ()) {
				kLog (K_MON, "[startGrabFrame] total(%03d) : grab was completed !!!\n", getGrabFrameCnt());

				// Frame ��� �Ϸ� ����
				setRunGrabFrame (FALSE);
				break;
			}
		}
	}

	kLog (K_MON, "[startGrabFrame] total(%03d) vector(%d) \n", getGrabFrameCnt(), m_vGrabFrameMemData.size());

	// Trigger ����
	IrtvCommand::getInstance().sendRearmTrigger ();

	//	go back to realtime mode
	IrtvCommand::getInstance().camera.EndAcquire();

	if (NULL != m_rawFp) {
		fclose (m_rawFp);
		m_rawFp	= NULL;
	}

	return (OK);
}

const int IrtvGrabFrame::stop () 
{
	kLog (K_INFO, "Stop grab image \n");

	return (OK);
}

const int IrtvGrabFrame::loadGrabFrame (unsigned char *data, unsigned int size) 
{
	if (NULL == data) {
		return (NOK);
	}

	kLog (K_DEBUG, "[IrtvGrabFrame.loadGrabFrame] width(%d) height(%d) size(%d)\n", m_width, m_height, size);

#if 0
	m_grabFrameData.setFrameData (m_width, m_height, data);

	gGrabFrameShm.write (m_grabFrameData);

	//if (true == isLiveRefresh ()) {
	//	processLiveData (m_width, m_height, data);
	//}
#else
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

	// ���Ϸ� ����
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
	// 4 ~ 6ms �ҿ�� : �Ϻ� �����Ǿ ���� ������ ���� ���۸���. IRTV_FRAME_BUF_QUEUE_SIZE
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
	// �޸𸮿� ����
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
	if (1 == m_pIRTV->bo_gen_seq_file) {
		// RAW ���Ϸκ��� SEQ ���� ����
		if (OK != convertRawToSEQ ()) {
			return (NOK);
		}
	}

	if (1 == m_pIRTV->bo_gen_tiff_file) {
		// RAW ���Ϸκ��� TIFF ���� ����
		if (OK != convertRawToTIFF ()) {
			return (NOK);
		}
	}

	if (1 == m_pIRTV->bo_gen_avi_file) {
		// RAW ���Ϸκ��� AVI ���� ����
		if (OK != convertRawToAVI ()) {
			return (NOK);
		}
	}

#if 0
	// �޸𸮷κ��� SEQ ���� ����
	if (OK != convertMemToSEQ ()) {
		return (NOK);
	}

	// �޸𸮷κ��� TIFF ���� ����
	if (OK != convertMemToTIFF ()) {
		return (NOK);
	}

#if 0
	// �޸𸮷κ��� AVI ���� ����
	if (OK != convertMemToAVI ()) {
		//return (NOK);
	}
#endif
#endif

	return (OK);
}

const int IrtvGrabFrame::convertRawToTIFF () 
{
	// ���� ����
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

	// ���Ϸ� ����
#if 0
	for (int i = 1; ; i++) {
		// ���Ϸκ��� RAW Data �б�
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data�� TIFF ���Ϸ� ��ȯ
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
		// ���Ϸκ��� RAW Data �б�
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data�� �̿��Ͽ� TIFF ���� ����
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
	// ���� ����
	char	tifFileName[256];

	kLog (K_MON, "[MemToTIFF] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	// ���� ����
	createFile (false);

	makeTiffFilePath (tifFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, 0);

	IrtvTiff	tiffObj;

	if (0 > tiffObj.open (m_pIRTV->shotNumber, tifFileName, 
			(UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height, true, true)) {
		kLog (K_ERR, "[IrtvGrabFrame.convertMemToTIFF] tiff open failed : %s\n", tifFileName);
		return (NOK);
	}

	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		// Vector ���� RAW Data�� �̿��Ͽ� TIFF ���� ����
		tiffObj.write ((UInt16 *)m_vGrabFrameMemData[i].m_pData);
	}

	tiffObj.close ();

	kLog (K_MON, "[MemToTIFF] %s created !!!\n", tifFileName);

	return (OK);
}

const int IrtvGrabFrame::convertRawToAVI () 
{
	// ���� ����
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

	// BMP ���� ����
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

	// ���Ϸ� ����
	for (int i = 1; ; i++) {
		// ���Ϸκ��� RAW Data �б�
		if (1 != fread (m_convertBuf, size, 1, fp)) {
			break;
		}

		// RAW Data�� BMP ���Ϸ� ��ȯ
		makeBMPFilePath (bmpFilePath, m_pIRTV->shotNumber, i);
		pBMP->write (bmpFilePath, (UInt16 *)m_convertBuf);
	}

	fclose (fp);
	fp	= NULL;

	pBMP->close ();

	kLog (K_MON, "[RawToAVI] %s : BMP created !!!\n", aviFilePath);

	// vdub�� �̿��Ͽ� BMP ���ϵ�κ��� AVI ����

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

	// BMP ���� ����
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

	// Vector ���� RAW Data�� �̿��Ͽ� BMP ���� ����
	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		makeBMPFilePath (bmpFilePath, m_pIRTV->shotNumber, i + 1);
		pBMP->write (bmpFilePath, (UInt16 *)m_vGrabFrameMemData[i].m_pData);
	}

	pBMP->close ();

	kLog (K_MON, "[MemToAVII] %s : BMP created !!!\n", aviFilePath);

	// vdub�� �̿��Ͽ� BMP ���ϵ�κ��� AVI ����

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
	// Calibration ���� ���
	bhpSCalInfo *	calib	= (bhpSCalInfo *) irtvGetCalibration ();

	// ���� ����
	char	seqFileName[256];

	kLog (K_MON, "[MemToSEQ] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	// ���� ����
	createFile (false);

#if 0
	if (NULL == calib) {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "sfmov");
	}
	else {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "seq");
	}
#else
	// Ȯ���ڴ� �ڵ����� ������ : sfmov, seq
	makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "");
#endif

	IrtvFileSEQ		irtvFile;

	// SEQ ���� ����
	if (0 > irtvFile.open (m_pIRTV->shotNumber, 
			(UInt16)m_pIRTV->ao_roi_width, (UInt16)m_pIRTV->ao_roi_height, seqFileName, calib)) {
		kLog (K_ERR, "[IrtvGrabFrame.convertMemToSEQ] SEQ open failed : %s\n", seqFileName);
		return (NOK);
	}

	for (unsigned int i = 0; i < m_vGrabFrameMemData.size(); ++i) {
		// Vector ���� RAW Data�� �̿��Ͽ� ���� ����
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
	// Calibration ���� ���
	bhpSCalInfo *	calib	= (bhpSCalInfo *) irtvGetCalibration ();

	// ���� ����
	char	rawFileName[256];
	char	seqFileName[256];
	char	incFileName[256];

	kLog (K_MON, "[RawToSEQ] width(%.0f) height(%.0f)\n", m_pIRTV->ao_roi_width, m_pIRTV->ao_roi_height);

	makeRawFilePath	(rawFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0);

#if 0
	if (NULL == calib) {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "sfmov");
	}
	else {
		makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "seq");
	}
#else
	// Ȯ���ڴ� �ڵ����� ������ : sfmov, seq
	makeShotImagePath (seqFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "");
#endif

	IrtvFileSEQ		irtvFile;

	irtvFile.convert ((int)m_pIRTV->ao_roi_width, (int)m_pIRTV->ao_roi_height, rawFileName, seqFileName, calib);

#if 0	// 20130805 : ��û�� ����, ���� ������
	if (NULL == calib) {
		makeShotImagePath (incFileName, m_pIRTV->so_image_path, m_pIRTV->shotNumber, 0, "inc");

		CopyFile (IRTV_CALIB_INC_FILE_NAME, incFileName, 0);
	}
#endif

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

#if 0	// �����޸𸮿� �����ϴ� ����� �̻��
	// threadFunc_LiveDataThr ���� LiveData ó�� ��û
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
	
	bhpSHeader		header;

	bhpDecodeHeader (&header, data);

	setCameraStatusWithHeader (pIRTV, &header);

	scanIoRequest (pIRTV->ioScanPvt_userCall);
}

void IrtvGrabFrame::setCameraStatusWithHeader (ST_IRTV *pIRTV, bhpSHeader *pHeader) 
{
	if ( (NULL == pIRTV) || (NULL == pHeader) ) {
		return;
	}
	
	// TODO : �µ� ������ ��Ȯ���� �ʾ�, �ǽð� ������Ʈ�� ū �ǹ� ����
#if 0
	// Camera CCD�� �µ��� �ƴϹǷ�, ������Ʈ�� ������
	//pIRTV->ai_sts_fpa_temp	= pHeader->Temp;
	
	pIRTV->ai_sts_preset	= pHeader->ActivePreset;
	pIRTV->ai_sts_int_time	= pHeader->IntegrationTime;

	printCameraHeader (pHeader);
#endif
}

void IrtvGrabFrame::printCameraHeader (bhpSHeader *pHeader) 
{
	kLog (K_DEBUG, "temp(%f, %f, %f, %f) Resolution(%d) \n",
		pHeader->FPATemp, pHeader->FrontPanelTemp, pHeader->AirGapTemp, pHeader->InternalTemp,
		pHeader->ADResolution);

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

#if 0 //TODO++
iacfSObjectParameters *IrtvGrabFrame::getObjParam () 
{
	if (NULL == pObjParam) {
		pObjParam = (iacfSObjectParameters *)malloc(sizeof(iacfSObjectParameters));

		iacfCalibDefaultObjectParameters (pObjParam);
	}

	return (pObjParam);
}
#endif

int IrtvGrabFrame::applyCalibration (const int width, const int height, unsigned char *data) 
{
	//TODO TEMP : ���� �µ� ������ �ʿ��� ��� ����� ������
#if 0
	kLog (K_INFO , "[applyCalib] start... \n");
	
	// Calibration ���� ���
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
		if ( (NULL != extName) && (0x00 != extName[0]) ) {
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
