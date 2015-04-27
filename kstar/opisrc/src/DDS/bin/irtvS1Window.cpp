#include <QDebug>

#include "mainWindow.h"
#include "irtvS1Window.h"
#include "irtvDataClient.h"

using namespace std;

static MainWindow *pWnd = NULL;

// display on/off
extern  bool			gbDisplayOnOff;

IRTV_S1Window::IRTV_S1Window (QWidget *parent)
{
	//setPrintLevel (K_INFO);

	pWnd = (MainWindow *)parent;

    setupUi(pWnd);

	init ();
}

IRTV_S1Window::~IRTV_S1Window ()
{
}

void IRTV_S1Window::init ()
{
#if !defined(WIN32)
	btn_FileDialog->setEnabled (false);
	btn_RefreshTime->setEnabled (false);
#else
	btn_DisplayOnOff->setEnabled (false);
#endif

	createActions ();
	createConnects ();

#if defined(WIN32)
	// 공유메모리 열기
	try {
		m_grabFrameShm.open ();
	}
	catch (IrtvGrabFrameShmException &shmErr) {
		pWnd->printStatus ("[IRTV_S1Window::init] shared memory open failed. %s", shmErr.getReason());
	}
#endif

	// ROI View용 Label에 기본 이미지 표시
	QImage image (IRTV_ROI_IMAGE_PATH);
	label_ROIView->setPixmap (QPixmap::fromImage(image));

	startTimer ();

	// 주기적으로 업데이트할 PV들의 chid 초기화
	initCA ();

	startTimer_PV ();

	// ROI 영역 선정 영역 설정
	m_canvas = new IrtvGraphicsScene(this);	// new QGraphicsScene();

	//m_grapicsView = new QGraphicsView(m_canvas, test_groupBox);
	m_grapicsView = new QGraphicsView(m_canvas, label_ROIView);
	m_grapicsView->setBackgroundBrush(QImage(IRTV_ROI_IMAGE_PATH));
	m_grapicsView->setCacheMode(QGraphicsView::CacheBackground);
}

void IRTV_S1Window::createActions ()
{
}

void IRTV_S1Window::createConnects ()
{
	connect (this, SIGNAL(signal_updateFilePath()), imageFilePath, SLOT(valueEntered()));
	connect (btn_FileDialog, SIGNAL(clicked()), this, SLOT(on_btn_FileDialog_clicked()));
	connect (btn_RefreshTime, SIGNAL(currentIndexChanged(int)), this, SLOT(on_btn_RefreshTime_changed()));
	connect (btn_DisplayOnOff, SIGNAL(clicked()), this, SLOT(on_btn_DisplayOnOff_clicked()));
	connect (btn_UpdateROIView, SIGNAL(clicked()), this, SLOT(on_btn_UpdateROIView_clicked()));
	connect (btn_ROI_FullScale, SIGNAL(clicked()), this, SLOT(on_btn_ROIFullScale_clicked()));

	connect (edit_ROI_Width, SIGNAL(changedValue(double)), this, SLOT(on_edit_ROI_Width_changed()));
	connect (edit_ROI_Height, SIGNAL(changedValue(double)), this, SLOT(on_edit_ROI_Height_changed()));
	connect (edit_ROI_X, SIGNAL(changedValue(double)), this, SLOT(on_edit_ROI_X_changed()));
	connect (edit_ROI_Y, SIGNAL(changedValue(double)), this, SLOT(on_edit_ROI_Y_changed()));

	connect (edit_FrameRate, SIGNAL(changedValue(double)), this, SLOT(on_edit_FrameRate_changed()));
	//connect (edit_FrameRate, SIGNAL(returnPressed()), this, SLOT(on_edit_FrameRate_entered()));
	//TODO
	//connect (slider_FrameRate, SIGNAL(sliderReleased()), this, SLOT(on_slider_FrameRate_changed()));
	//connect (slider_FrameRate, SIGNAL(sliderMoved(int)), this, SLOT(on_slider_FrameRate_changed()));
	//TODO
	//connect (slider_FrameRate, SIGNAL(valueChanged(int)), this, SLOT(on_slider_FrameRate_changed()));

	connect (table_StoredNUC, SIGNAL(cellClicked(int,int)), this, SLOT(on_table_StoredNUC_cellClickd(int,int)));

	// Radio button for Sync Source
	connect (radio_SyncSource_Free, SIGNAL(clicked()), this, SLOT(on_radio_SyncSource_Free_clicked()));
	connect (radio_SyncSource_SWTrigger, SIGNAL(clicked()), this, SLOT(on_radio_SyncSource_SWTrigger_clicked()));
	connect (radio_SyncSource_External, SIGNAL(clicked()), this, SLOT(on_radio_SyncSource_External_clicked()));

	// Radio button for External Sync Polarity
	connect (radio_PolarityFalling, SIGNAL(clicked()), this, SLOT(on_radio_PolarityFalling_clicked()));
	connect (radio_PolarityRising, SIGNAL(clicked()), this, SLOT(on_radio_PolarityRising_clicked()));

	// FPA Auto Off/On
	connect (radio_FPA_Disable, SIGNAL(clicked()), this, SLOT(on_radio_FPA_Disable_clicked()));
	connect (radio_FPA_Enable, SIGNAL(clicked()), this, SLOT(on_radio_FPA_Enable_clicked()));
	connect (radio_FPA_Auto, SIGNAL(clicked()), this, SLOT(on_radio_FPA_Auto_clicked()));

	// Preset Selection
	connect (radio_Preset0, SIGNAL(clicked()), this, SLOT(on_radio_Preset0_clicked()));
	connect (radio_Preset1, SIGNAL(clicked()), this, SLOT(on_radio_Preset1_clicked()));
	connect (radio_Preset2, SIGNAL(clicked()), this, SLOT(on_radio_Preset2_clicked()));
	connect (radio_Preset3, SIGNAL(clicked()), this, SLOT(on_radio_Preset3_clicked()));

	// NUC Action
	connect (radio_Action_Perform, SIGNAL(clicked()), this, SLOT(on_radio_Action_Perform_clicked()));
	connect (radio_Action_Load, SIGNAL(clicked()), this, SLOT(on_radio_Action_Load_clicked()));
	connect (radio_Action_Unload, SIGNAL(clicked()), this, SLOT(on_radio_Action_Unload_clicked()));
	connect (radio_Action_Save, SIGNAL(clicked()), this, SLOT(on_radio_Action_Save_clicked()));
}

void IRTV_S1Window::updateWindowCoordi ()
{
	// ROI 설정 값 변경
	edit_ROI_X->valueEntered();
	edit_ROI_Y->valueEntered();
	edit_ROI_Width->valueEntered();
	edit_ROI_Height->valueEntered();
}

void IRTV_S1Window::setWindowCoordi (char *x, char *y, char *width, char *height)
{
	edit_ROI_X->setText (x);
	edit_ROI_Y->setText (y);
	edit_ROI_Width->setText (width);
	edit_ROI_Height->setText (height);
}

void IRTV_S1Window::on_btn_FileDialog_clicked ()
{
	QString dir = QFileDialog::getExistingDirectory (
		pWnd, 
		tr("Open Directory"),
		IRTV_FILE_DIALOG_PATH,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	// Image File Path를 위한 LineEdit에 선택한 디렉토리 표시
	imageFilePath->setText (dir);
	
	// PV 설정 요청
	emit signal_updateFilePath();
}

void IRTV_S1Window::on_btn_RefreshTime_changed ()
{
	int	nCurrIdx	= btn_RefreshTime->currentIndex ();
	QString	str		= btn_RefreshTime->currentText ();

	qDebug ("on_btn_RefreshTime_changed : nSelected ... %d, %d", nCurrIdx, str.toInt());

	if (0 > nCurrIdx) {
		return;
	}

	m_nTimerMillis	= 1000 / str.toInt();

	stopTimer ();
	startTimer ();
}

// 서버에 대한 통신 연결 및 해제
void IRTV_S1Window::on_btn_DisplayOnOff_clicked ()
{
	if (true != m_bDisplayOn) {
		Password pword("kstar2011", new QWidget());
		if (pword.getStatus() != true) {
			return;
		}
	}

	m_bDisplayOn	= !m_bDisplayOn;

	if (true == m_bDisplayOn) {
		btn_DisplayOnOff->setText ("Display OFF");
	}
	else {
		btn_DisplayOnOff->setText ("Display ON");
	}

	// whether DataSendThr sends the heartbeat message or not
	gbDisplayOnOff	= m_bDisplayOn;

	qDebug ("on_btn_DisplayOnOff_clicked : clicked ... : %d", m_bDisplayOn);

#if !defined(IRTV_USE_SHM_DATA)
	// 서버로부터 취득 이미지 데이터를 수신할 쓰레드 생성
	if (false == m_bDataRecvThr) {
		createDataRecvThr (&m_grabFrameData);

		m_bDataRecvThr	= true;
	}
#endif
}

// ROI View 영역에 이미지 갱신
void IRTV_S1Window::on_btn_UpdateROIView_clicked ()
{
	qDebug ("on_btn_UpdateROIView_clicked : clicked ...");

	updateGrabFrameData ();

	// 이미지 크기 정보 얻기
	int		width	= m_grabFrameData.getWidth ();
	int		height	= m_grabFrameData.getHeight ();

	if ( ( 0 >= width) || (0 >= height) ) {
		QMessageBox::information (0, "IRTV", tr("There is no grab image in buffer!") );
		return;
	}

	pWnd->printStatus ("Updated the image of ROI view");

	// RAW 데이터를 RGB로 변환
	convertFrameToRGB ((unsigned short *)(m_grabFrameData.getFrameData () + IRTV_GET_HEADER_SIZE(width)), (unsigned char *)m_frameBuffer, width, height);

	QImage image ((uchar *)m_frameBuffer, width, height, IRTV_IMG_FORMAT);

	// ROI View용 Label에 이미지 표시
	//label_ROIView->setPixmap (QPixmap::fromImage(image));
	m_grapicsView->setBackgroundBrush(image);

	qDebug ("on_btn_UpdateROIView_clicked : finished ... ");
}

// Full Scale로 ROI View 영역의 이미지 갱신
void IRTV_S1Window::on_btn_ROIFullScale_clicked ()
{
	qDebug ("on_btn_ROIFullScale_clicked : clicked ...");

	m_canvas->reDrawRect (0.0, 0.0, 640.0, 512.0);

	// ROI 설정 값 변경
	edit_ROI_X->valueEntered();
	edit_ROI_Y->valueEntered();
	edit_ROI_Width->valueEntered();
	edit_ROI_Height->valueEntered();

	qDebug ("on_btn_ROIFullScale_clicked : finished ... ");
}

void IRTV_S1Window::on_edit_ROI_Width_changed ()
{
	QString	str		= edit_ROI_Width->text ();
	m_canvas->reDrawRect (m_canvas->getXOffset(), m_canvas->getYOffset(), str.toDouble(), m_canvas->getHeight());
}

void IRTV_S1Window::on_edit_ROI_Height_changed ()
{
	QString	str		= edit_ROI_Height->text ();
	m_canvas->reDrawRect (m_canvas->getXOffset(), m_canvas->getYOffset(), m_canvas->getWidth(), str.toDouble());
}

void IRTV_S1Window::on_edit_ROI_X_changed ()
{
	QString	str		= edit_ROI_X->text ();
	m_canvas->reDrawRect (str.toDouble(), m_canvas->getYOffset(), m_canvas->getWidth(), m_canvas->getHeight());
}

void IRTV_S1Window::on_edit_ROI_Y_changed ()
{
	QString	str		= edit_ROI_Y->text ();
	m_canvas->reDrawRect (m_canvas->getXOffset(), str.toDouble(), m_canvas->getWidth(), m_canvas->getHeight());
}

void IRTV_S1Window::on_edit_FrameRate_changed ()
{
	QString	str		= edit_FrameRate->text ();
	double	currRate	= slider_FrameRate->value ();

	if ( (str.toDouble() > 0) && (currRate != str.toDouble()) ) {
		qDebug ("on_edit_FrameRate_changed : rate ... %f / %d", str.toDouble(), (int)str.toDouble());

		slider_FrameRate->setValue ((int)str.toDouble());
	}
}

void IRTV_S1Window::on_slider_FrameRate_changed ()
{
	qDebug ("on_slider_FrameRate_changed : value ... %d", slider_FrameRate->value());

	char szVal[64];
	sprintf (szVal, "%.1f", (float)slider_FrameRate->value());
	edit_FrameRate->setText (szVal);

	// 드래그시에는 edit_FrameRate에 표시되는 값만 변경하고, 마우스가 Release될 때 PV 변경
	if (true != slider_FrameRate->isSliderDown ()) {
		edit_FrameRate->valueEntered();
	}
}

void IRTV_S1Window::on_table_StoredNUC_cellClickd (int row, int col)
{
	QTableWidgetItem	*item = table_StoredNUC->item(row, 0);

	if (item) {
		QByteArray ba = item->text().toLatin1();
		const char *str	= ba.data();
		qDebug ("on_table_StoredNUC_cellClickd : row(%d) col(%d) name(%s)", row, col, str);
		writePV ("IRTV_S1_SEL_NUC_NAME", str);
	}
}

void IRTV_S1Window::on_radio_SyncSource_Free_clicked ()
{
	writePV ("IRTV_S1_SYNC_SOURCE", "0");
}

void IRTV_S1Window::on_radio_SyncSource_SWTrigger_clicked ()
{
	writePV ("IRTV_S1_SYNC_SOURCE", "3");
}

void IRTV_S1Window::on_radio_SyncSource_External_clicked ()
{
	writePV ("IRTV_S1_SYNC_SOURCE", "1");
}

void IRTV_S1Window::on_radio_PolarityFalling_clicked ()
{
	writePV ("IRTV_S1_SYNC_POLARITY", "0");
}

void IRTV_S1Window::on_radio_PolarityRising_clicked ()
{
	writePV ("IRTV_S1_SYNC_POLARITY", "1");
}

void IRTV_S1Window::on_radio_FPA_Disable_clicked ()
{
	writePV ("IRTV_S1_FPA_COOLER_AUTO", "0");
}

void IRTV_S1Window::on_radio_FPA_Enable_clicked ()
{
	writePV ("IRTV_S1_FPA_COOLER_AUTO", "1");
}

void IRTV_S1Window::on_radio_FPA_Auto_clicked ()
{
	writePV ("IRTV_S1_FPA_COOLER_AUTO", "2");
}

void IRTV_S1Window::on_radio_Preset0_clicked ()
{
	writePV ("IRTV_S1_SEL_PRESET_NUM", "0");
}

void IRTV_S1Window::on_radio_Preset1_clicked ()
{
	writePV ("IRTV_S1_SEL_PRESET_NUM", "1");
}

void IRTV_S1Window::on_radio_Preset2_clicked ()
{
	writePV ("IRTV_S1_SEL_PRESET_NUM", "2");
}

void IRTV_S1Window::on_radio_Preset3_clicked ()
{
	writePV ("IRTV_S1_SEL_PRESET_NUM", "3");
}

void IRTV_S1Window::on_radio_Action_Perform_clicked ()
{
	writePV ("IRTV_S1_NUC_ACTION_TYPE", "0");
}

void IRTV_S1Window::on_radio_Action_Load_clicked ()
{
	writePV ("IRTV_S1_NUC_ACTION_TYPE", "1");
}

void IRTV_S1Window::on_radio_Action_Unload_clicked ()
{
	writePV ("IRTV_S1_NUC_ACTION_TYPE", "2");
}

void IRTV_S1Window::on_radio_Action_Save_clicked ()
{
	writePV ("IRTV_S1_NUC_ACTION_TYPE", "3");
}

void IRTV_S1Window::startTimer ()
{
	if (NULL == m_QTimer) {
		if (NULL == (m_QTimer = new QTimer(this))) {
			return;
		}
	}

	if (NULL != m_QTimer) {
		connect (m_QTimer, SIGNAL(timeout()), this, SLOT(timerCallback()));

		m_QTimer->start (m_nTimerMillis);

		pWnd->printStatus( "IRTV Timer : started" );
	}
}

void IRTV_S1Window::stopTimer ()
{
	if (NULL != m_QTimer) {
		m_QTimer->stop();

		pWnd->printStatus( "IRTV Timer : stop" );
	}
}

void IRTV_S1Window::timerCallback ()
{
	//qDebug ("Timer ... ");

	// 공유메모리 내의 이미지 Frame Data를 얻어와 화면에 표시
	displayGrabFrame ();
}

void IRTV_S1Window::startTimer_PV ()
{
	if (NULL == m_QTimerPV) {
		if (NULL == (m_QTimerPV = new QTimer(this))) {
			return;
		}
	}

	if (NULL != m_QTimerPV) {
		connect (m_QTimerPV, SIGNAL(timeout()), this, SLOT(timerCallback_PV()));

		m_QTimerPV->start (IRTV_PV_UPDATE_TIMER_MSEC);

		pWnd->printStatus( "IRTV Timer for PVs : started" );
	}
}

void IRTV_S1Window::timerCallback_PV ()
{
	// 등록된 PV들의 값 얻기 : Stored NUCs 목록 표시
	getCA ();
}

void IRTV_S1Window::updateGrabFrameData ()
{
	// 공유메모리로부터 Frame 이미지 정보 및 데이터 얻기
#if defined(WIN32) && defined(IRTV_USE_SHM_DATA)
	try {
		m_grabFrameShm.read (m_grabFrameData);
	}
	catch (...) {
		try {
			m_grabFrameShm.open ();
		}
		//catch (IrtvGrabFrameShmException &shmErr2) {
		catch (...) {
		}
	}
#endif
}

void IRTV_S1Window::displayGrabFrame ()
{
	int	width, height;

	updateGrabFrameData ();

	// Frame 변경 여부 확인
	if (m_grabFrameData.getFrameSeqNum () == m_nFrameSeqNum) {
		return;
	}

	// 마지막 Frame 일련번호 저장
	m_nFrameSeqNum	= m_grabFrameData.getFrameSeqNum ();

	// 이미지 크기 정보 얻기
	width	= m_grabFrameData.getWidth ();
	height	= m_grabFrameData.getHeight ();

	if ( ( 0 >= width) || (0 >= height) ) {
		return;
	}

	pWnd->printStatus ("%.f : width(%d) height(%d) size(%d)", m_nFrameSeqNum, width, height, m_grabFrameData.getFrameSize ());

	// RAW 데이터를 RGB로 변환
	convertFrameToRGB ((unsigned short *)(m_grabFrameData.getFrameData () + IRTV_GET_HEADER_SIZE(width)), (unsigned char *)m_frameBuffer, width, height);

	QImage image ((uchar *)m_frameBuffer, width, height, IRTV_IMG_FORMAT);

	// Live View용 Label에 이미지 표시
	label_LiveView->setPixmap (QPixmap::fromImage(image));
}

//------------------------------------------------------------------
// Channel Access : KWT의 caArrayThread.cpp 파일을 참조함
//------------------------------------------------------------------

void IRTV_S1Window::initCA ()
{
	IRTV_CA_INFO	caInfo;

	caInfo.value	= NULL;
	caInfo.status	= ECA_DISCONN;
	caInfo.bInit	= 0;

	caInfo.name = "IRTV_S1_STORED_NUC_LIST";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_SYNC_SOURCE";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_SYNC_POLARITY";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_FPA_COOLER_AUTO";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_SEL_PRESET_NUM";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_NUC_ACTION_TYPE";
	m_vCAInfoList.push_back (caInfo);

	caInfo.name = "IRTV_S1_MAX_RATE";
	m_vCAInfoList.push_back (caInfo);

	//ca_task_initialize ();
	//ca_context_create (ca_disable_preemptive_callback);
	ca_context_create (ca_enable_preemptive_callback);


	for (unsigned int i = 0; i < m_vCAInfoList.size(); i++) {
#if 0
		int		size;

		ca_create_channel (m_vCAInfoList[i].name.c_str(), NULL, NULL, CA_PRIORITY, &m_vCAInfoList[i].chanid);
		//ca_search (pvName.Buffer(), &chanId);
		ca_pend_io(0.5);

		m_vCAInfoList[i].nElems		= ca_element_count (m_vCAInfoList[i].chanid);
		m_vCAInfoList[i].dbfType	= ca_field_type (m_vCAInfoList[i].chanid);
		m_vCAInfoList[i].dbrType	= dbf_type_to_DBR_TIME(m_vCAInfoList[i].dbfType);
		size						= dbr_size_n (m_vCAInfoList[i].dbrType, m_vCAInfoList[i].nElems);
		m_vCAInfoList[i].value		= calloc (1, size);

		qDebug ("initCA : pv(%s) chid(%d) elems(%d) type(%d/%d) size(%d)", 
			m_vCAInfoList[i].name.c_str(), (int)m_vCAInfoList[i].chanid, (int)m_vCAInfoList[i].nElems, 
			(int)m_vCAInfoList[i].dbfType, (int)m_vCAInfoList[i].dbrType, size);
#else
		ca_create_channel (m_vCAInfoList[i].name.c_str(), irtvConnectionCallback, &m_vCAInfoList[i], CA_PRIORITY, &m_vCAInfoList[i].chanid);
#endif
	}

	//TODO
#if 1
	ca_pend_event(1.0);
#endif
}

void IRTV_S1Window::getCA ()
{
	char			name[64], time[64], value[64];
	char			*ptr;
	unsigned int	i, j;
	int				bChanged;

	for (i = 0; i < m_vCAInfoList.size(); i++) {
		if (ECA_CONN != m_vCAInfoList[i].status) {
			continue;
		}

		ca_array_get (m_vCAInfoList[i].dbrType, m_vCAInfoList[i].nElems, m_vCAInfoList[i].chanid, m_vCAInfoList[i].value);

		if (ECA_NORMAL != ca_pend_io(0.5)) {
			qDebug ("%s denied access", m_vCAInfoList[i].name.c_str());
			continue;
		}
	
		if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_STORED_NUC_LIST")) {
			bChanged	= false;

			for (j = 0; j < m_vCAInfoList[i].nElems; j++) {
				strcpy (name, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, j));

				if (NULL == (ptr = strrchr(name, ','))) {
					break;
				}

				strcpy (time, ptr + 1);
				name[strlen(name)-strlen(ptr)]	= 0x00;

				if (0 != QString::compare (name, table_StoredNUC->item(j,0)->text())) {
					bChanged = true;
					break;
				}

				if (0 != QString::compare (time, table_StoredNUC->item(j,1)->text())) {
					bChanged = true;
					break;
				}
			}

			if (bChanged) {
				// 테이블의 이전 내용 삭제
				table_StoredNUC->clearContents();

				for (j = 0; j < m_vCAInfoList[i].nElems; j++) {
					strcpy (name, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, j));

					if (NULL == (ptr = strrchr(name, ','))) {
						break;
					}

					strcpy (time, ptr + 1);
					name[strlen(name)-strlen(ptr)]	= 0x00;

					// 테이블에 목록 표시
					table_StoredNUC->setItem (j, 0, new QTableWidgetItem (name));
					table_StoredNUC->setItem (j, 1, new QTableWidgetItem (time));

					//qDebug ("GetCA : pv(%s) chid(%d) num(%d) val(%s)", m_vCAInfoList[i].name.c_str(), m_vCAInfoList[i].chanid, j, buf);
				}

				// 테이블 다시 표시
				table_StoredNUC->repaint();
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_SYNC_SOURCE")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			if (0 == strcmp(value, "0")) {
				radio_SyncSource_Free->setChecked (true);
				radio_SyncSource_SWTrigger->setChecked (false);
				radio_SyncSource_External->setChecked (false);
			}
			else if (0 == strcmp(value, "3")) {
				radio_SyncSource_Free->setChecked (false);
				radio_SyncSource_SWTrigger->setChecked (true);
				radio_SyncSource_External->setChecked (false);
			}
			else if (0 == strcmp(value, "1")) {
				radio_SyncSource_Free->setChecked (false);
				radio_SyncSource_SWTrigger->setChecked (false);
				radio_SyncSource_External->setChecked (true);
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_SYNC_POLARITY")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			if (0 == strcmp(value, "0")) {
				radio_PolarityFalling->setChecked (true);
				radio_PolarityRising->setChecked (false);
			}
			else {
				radio_PolarityFalling->setChecked (false);
				radio_PolarityRising->setChecked (true);
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_FPA_COOLER_AUTO")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			if (0 == strcmp(value, "0")) {
				radio_FPA_Disable->setChecked (true);
				radio_FPA_Enable->setChecked (false);
				radio_FPA_Auto->setChecked (false);
			}
			else if (0 == strcmp(value, "1")) {
				radio_FPA_Disable->setChecked (false);
				radio_FPA_Enable->setChecked (true);
				radio_FPA_Auto->setChecked (false);
			}
			else if (0 == strcmp(value, "2")) {
				radio_FPA_Disable->setChecked (false);
				radio_FPA_Enable->setChecked (false);
				radio_FPA_Auto->setChecked (true);
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_SEL_PRESET_NUM")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			if (0 == strcmp(value, "0")) {
				radio_Preset0->setChecked (true);
				radio_Preset1->setChecked (false);
				radio_Preset2->setChecked (false);
				radio_Preset3->setChecked (false);
			}
			else if (0 == strcmp(value, "1")) {
				radio_Preset0->setChecked (false);
				radio_Preset1->setChecked (true);
				radio_Preset2->setChecked (false);
				radio_Preset3->setChecked (false);
			}
			else if (0 == strcmp(value, "2")) {
				radio_Preset0->setChecked (false);
				radio_Preset1->setChecked (false);
				radio_Preset2->setChecked (true);
				radio_Preset3->setChecked (false);
			}
			else if (0 == strcmp(value, "3")) {
				radio_Preset0->setChecked (false);
				radio_Preset1->setChecked (false);
				radio_Preset2->setChecked (false);
				radio_Preset3->setChecked (true);
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_NUC_ACTION_TYPE")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			if (0 == strcmp(value, "0")) {
				radio_Action_Perform->setChecked (true);
				radio_Action_Load->setChecked (false);
				radio_Action_Unload->setChecked (false);
				radio_Action_Save->setChecked (false);
			}
			else if (0 == strcmp(value, "1")) {
				radio_Action_Perform->setChecked (false);
				radio_Action_Load->setChecked (true);
				radio_Action_Unload->setChecked (false);
				radio_Action_Save->setChecked (false);
			}
			else if (0 == strcmp(value, "2")) {
				radio_Action_Perform->setChecked (false);
				radio_Action_Load->setChecked (false);
				radio_Action_Unload->setChecked (true);
				radio_Action_Save->setChecked (false);
			}
			else if (0 == strcmp(value, "3")) {
				radio_Action_Perform->setChecked (false);
				radio_Action_Load->setChecked (false);
				radio_Action_Unload->setChecked (false);
				radio_Action_Save->setChecked (true);
			}
		}
		else if (0 == strcmp (m_vCAInfoList[i].name.c_str(), "IRTV_S1_MAX_RATE")) {
			strcpy (value, val2str (m_vCAInfoList[i].value, m_vCAInfoList[i].dbrType, 0));

			float	nMaxFrameRate;
			sscanf (value, "%g", &nMaxFrameRate);

			if ( (0 < nMaxFrameRate) && (nMaxFrameRate < 1000) ) {
				slider_FrameRate->setMaximum ((int)nMaxFrameRate);
				//qDebug ("slider_FrameRate->maximum : %d", slider_FrameRate->maximum());
			}
		}
	}
}
