/****************************************************************************
**
** Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved. 
**
**
*****************************************************************************/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include <QtGui>


#include "mainWindow.h"
//#include "pageMagnetic.h"
//#include "nodeDelegate.h"
//#include <QThread>

#if !defined(WIN32)
#include "unistd.h"
#endif

//#include "systemState.h"

#include "nodeDelegate.h"

#include "irtvS1Window.h"
#include "irtvD1Window.h"

//QString *ApQStringMonitors;
//char AcTimestampString[ 40];
//static void vFMonitorCallback( struct event_handler_args args);

//evid evidMain[MAX_EVENT_PV];
//int cntEvidMain=0;

//static unsigned int status_line_cnt;


//QPalette pal_select, pal_normal;

//int g_nRunStopTimerstep;

//QWidget *btn_ER_OP;


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{

	setupUi(this);


	setWindowTitle(tr("DDS OPI"));
	printf("##################################\n");
	printf("DDS OPI \n");
	printf("R 1.5  2011. 3. 30 \n\n");
	
/*	printf("export MALLOC_CHECK=1\n"); */
	status_line_cnt = 1;

#if PV_CONNECT
	iCAStatus = ca_context_create( ca_enable_preemptive_callback);
	if (iCAStatus != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying to start channel access.\n", ca_message(iCAStatus) );
		return;
	}
#endif

//	timerPV = NULL;
//	timerRunStop = NULL;
	myProcess = NULL;

	
	init_pallet_color();


	createActions();	// Menu 
	createToolbars();
	createStatusBar();

	/*  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  */
	/* AttachChannelAccess ........ user can attach another UI files into stacked widget.... in this function */
	insertStackedWidgetPage();
	/*  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  */

	

	// Digital clock make
#if 0
	QGridLayout *gridLayout_clock;
	gridLayout_clock = new QGridLayout(frame_clock);
	gridLayout_clock->setSpacing(6);
	gridLayout_clock->setMargin(9);
	gridLayout_clock->setObjectName(QString::fromUtf8("gridLayout_clock"));
	gridLayout_clock->addWidget(label_4, 0, 0, 1, 1);

	myDClock = new DigitalClock(frame_clock);
	QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(1);
	myDClock->setSizePolicy(sizePolicy);
	myDClock->setAutoFillBackground(true);

	gridLayout_clock->addWidget(myDClock);
#endif


	connect(toolBox, SIGNAL(currentChanged(int )), this, SLOT(toolBox_currentChanged(int ) ) );
	connect(this, SIGNAL(signal_printStatus( char *)), this, SLOT( slot_printStatus( char *)  ));

	connect(btn_toolbox_DDS1, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_DDS1() ));
	connect(btn_toolbox_HAlpha, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_HAlpha() ));
	connect(btn_toolbox_Mirnov, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_Mirnov() ));
	connect(btn_toolbox_Reflecto, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_Reflecto() ));
	connect(btn_toolbox_Probe, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_Probe() ));
	connect(btn_toolbox_Thomson, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_Thomson() ));
	connect(btn_toolbox_XIC, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_XIC() ));
	connect(btn_toolbox_CE, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_CE() ));
	connect(btn_toolbox_BOLO, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_BOLO() ));
	connect(btn_toolbox_SXR, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_SXR() ));
	connect(btn_toolbox_MMWI, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_MMWI() ));
	connect(btn_toolbox_VS, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_VS() ));
	connect(btn_toolbox_FILTER, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_FILTER() ));
	connect(btn_toolbox_HXR, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_HXR() ));
	connect(btn_toolbox_ECEHR, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_ECEHR() ));
	connect(btn_toolbox_SHUT, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_SHUT() ));
	connect(btn_toolbox_POWER, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_POWER() ));
	connect(btn_toolbox_VBS, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_VBS() ));
	connect(btn_toolbox_IRTV_S1, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_IRTV_S1() ));
	connect(btn_toolbox_Mirnov2, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_Mirnov2() ));
	connect(btn_toolbox_FILD, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_FILD() ));
	connect(btn_toolbox_IRTV_D1, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_IRTV_D1() ));
	connect(btn_toolbox_IRVB, SIGNAL(clicked()), this, SLOT(clicked_btn_toolbox_IRVB() ));




/***************************************************************	
   DDS#1 only ... 
*/
#if 0
	connect(btn_dds1OPremote, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1OPremote( ) ) );
	connect(btn_dds1OPcalibration, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1OPcalibration( ) ) );
	connect(btn_dds1OPlocal, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1OPLocalTest( ) ) );
	connect(btn_dds1ResetMode, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1ResetMode( ) ) );
	connect(btn_dds1SetAllchs, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  ));
	connect(btn_dds1SetAllchs_2, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  )); // 2009. 5. 18
	connect(btn_dds1SetAllchs_3, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  ));
	connect(btn_dds1SetAllchs_4, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  ));
	connect(btn_dds1SetAllchs_5, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  ));
	connect(btn_dds1SetAllchs_6, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1SetAllchs()  ));
	connect(btn_dds1DigitizerSetup, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup()  ));
	connect(btn_dds1DigitizerSetup_2, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup_2()  ));
	connect(btn_dds1DigitizerSetup_3, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup_3()  ));
	connect(btn_dds1DigitizerSetup_4, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup_4()  ));
	connect(btn_dds1DigitizerSetup_5, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup_5()  ));
	connect(btn_dds1DigitizerSetup_6, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1DigitizerSetup_6()  ));
	connect(btn_dds1RemoteReady, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1RemoteReady()  ));
	connect(btn_dds1CalibRun, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1CalibRun()  ));
//	connect(btn_dds1RemoteStore, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1RemoteStore()  ));
	connect(btn_dds1ShotNum, SIGNAL(clicked()), this, SLOT(clicked_btn_dds1ShotNum()  ));
	connect(btn_dds1LocalRun, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1LocalRun()  ));

//	connect(ckb_dds1SetupEnable, SIGNAL(clicked()), this, SLOT( clicked_dds1SetupEnable()  ));

//	connect(btn_dds1RemoteSave, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1RemoteSave()  )); // 2009. 5. 13  by woong 
//	connect(btn_dds1LocalSave, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1LocalSave() ));	// 2009. 5. 13
	connect(btn_B1_Local_DAQ, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B1LocalDAQ() ));	// 2009. 5. 20
	connect(btn_B1_Local_Save, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B1LocalSave() ));	// 2009. 5. 20
	connect(btn_B1_Local_DAQ_2, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B2LocalDAQ() ));	// 2009. 5. 20
	connect(btn_B1_Local_Save_2, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B2LocalSave() ));	// 2009. 5. 20
	connect(btn_B1_Local_DAQ_3, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B3LocalDAQ() ));	// 2009. 5. 20
	connect(btn_B1_Local_Save_3, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B3LocalSave() ));	// 2009. 5. 20
	connect(btn_B1_Local_DAQ_4, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B4LocalDAQ() ));	// 2009. 5. 20
	connect(btn_B1_Local_Save_4, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1B4LocalSave() ));	// 2009. 5. 20
	
	connect(btn_dds1StatusUpdate, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1UpdateStatus() ));	// 2009. 5. 20
	connect(btn_dds1CreateShot, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1CreateShot() ));	// 2009. 5. 20

	connect(btn_dds1_Retrieve, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1_Retrieve() ));	// 2009. 9. 04

	connect(cb_dds1SampleClk, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk(QString) ));	// 2009. 10. 20
	connect(cb_dds1SampleClk_2, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk_2(QString) ));	// 2009. 10. 20
	connect(cb_dds1SampleClk_3, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk_3(QString) ));	// 2009. 10. 20
	connect(cb_dds1SampleClk_4, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk_4(QString) ));	// 2009. 10. 20
	connect(cb_dds1SampleClk_5, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk_5(QString) ));	// 2010. 5. 24
	connect(cb_dds1SampleClk_6, SIGNAL(currentIndexChanged(QString)), this, SLOT( idchanged_cb_dds1SampleClk_6(QString) ));	// 2010. 5. 20

	connect(btn_assign_all, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1_assign_all() ));	// 2010. 1. 21
	connect(btn_dds1_Arming, SIGNAL(clicked()), this, SLOT( clicked_btn_dds1_Arming() ));	// 2010. 5. 28

	connect(btn_forced_mode_reset, SIGNAL(clicked()), this, SLOT( clicked_btn_forced_mode_reset() ));	// 2010. 7. 14
#endif


/******************************************************************************************************/
#if 0
	connect(btn_dds2OPremote, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2OPremote( ) ) );
	connect(btn_dds2OPcalibration, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2OPcalibration( ) ) );
	connect(btn_dds2OPtest, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2OPtest( ) ) );
	connect(btn_dds2ResetMode, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2ResetMode( ) ) );
	connect(btn_dds2SetAll_1, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2SetAll()  ));
	connect(btn_dds2SetAll_2, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2SetAll()  ));
/*	connect(btn_dds2SetAll_3, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2SetAll()  )); */
/*	connect(btn_dds2SetAll_4, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2SetAll()  ));  */
	connect(btn_dds2DigitizerSetup, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2DigitizerSetup()  ));
/*	connect(btn_dds2RemoteReady, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2RemoteReady()  )); */
	connect(btn_dds2CalibStartStop, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2CalibStartStop()  ));
	connect(btn_dds2RemoteStore, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2RemoteStore()  ));
	connect(btn_dds2ShotNum, SIGNAL(clicked()), this, SLOT(clicked_btn_dds2ShotNum()  ));
	connect(btn_dds2TestRun, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2TestRun()  ));
	connect(btn_dds2DAQabort, SIGNAL(clicked()), this, SLOT( clicked_btn_dds2DAQabort()  ));

	connect(ckb_dds2SetupEnable, SIGNAL(clicked()), this, SLOT( clicked_dds2SetupEnable()  ));
#endif
	

	
	cbFunc.InitWindow(this); // must do this first than below lines.
//	pg_dds1.InitWindow(this);
//	pg_dds2.InitWindow(this);
//	fileFunc.InitWindow(this);

//	pg_dds1.initStactedWidgetAcq196();
//	pg_dds2.initStactedWidgetM6802();


//	pthread = new QueueThread( this);	
//	connect( pthread, SIGNAL(finished() ), pthread, SLOT(deleteLater()) );
//	pthread->start();
 
 	initOPIstate();


}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
#if 0
	if(g_opMode != SYS_MODE_INIT ) 
	{
		if( g_nCurrentSystem == CURR_ACQ196 ) {
/* we can emergency reset by button..  2010.09.04
			if( g_opMode != SYS_MODE_INIT) {
				QMessageBox msgBox;
				msgBox.setIcon( QMessageBox::Warning );
				msgBox.setWindowTitle("Warning");
				msgBox.setText("Must release operation mode");
				msgBox.exec();
				event->ignore();
				return;
			}
*/
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196, 0);
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196_2, 1);
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196_3, 2);
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196_4, 3);
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196_5, 4);
			fileFunc.saveACQ196ConfigFile(tableWidget_ACQ196_6, 5);

		} 
//		else if (g_nCurrentSystem == CURR_M6802 ) {
//			fileFunc.saveM6802ConfigFile(tableWidget_M6802_1, 1);
//			fileFunc.saveM6802ConfigFile(tableWidget_M6802_2, 2);
/*			fileFunc.saveM6802ConfigFile(tableWidget_M6802_3, 3);   */
/*			fileFunc.saveM6802ConfigFile(tableWidget_M6802_4, 4);   */
//		}
	
		
	}
#endif
//	pg_dds1.kill_dds1_panel();
	
	usleep(1000000);
	
//	printf("delete DClock...");
//	delete myDClock;
//	printf("OK!\n");
	
#if PV_CONNECT
	printf("call ca_destroy()...");
	ca_context_destroy( );
	printf("OK!\n");
#endif
	
#if !defined(WIN32)
/********** for VNC Viewer *********/ /*  2007. 7. 13.	 NFRC modify */
	if( myProcess) {
		printf("call kill()...");
		kill(myProcess->pid(), SIGTERM);
		printf("OK!\n");
	}
/********** for VNC Viewer *********/ /*  2007. 7. 13.	 NFRC modify */ 
#endif

	event->accept();
	
#if defined(WIN32)
	qApp->quit();
#else
	printf("call kill()...\n");
	kill( getpid(), SIGTERM);
	printf("return kill()...\n");
#endif

}

void MainWindow::insertStackedWidgetPage()
{

	AttachChannelAccess *pattachInfo = new AttachChannelAccess(frame_shot_info);  // 2009. 10. 04
//	AttachChannelAccess *pattachDDS2 = new AttachChannelAccess(page_dds2);  // 2010. 7.21 
//	AttachChannelAccess *pattachDDS1A = new AttachChannelAccess(page_dds1);  // 2009. 11. 05

	AttachChannelAccess *pattachMD = new AttachChannelAccess("/usr/local/opi/ui/DDS_MD.ui", 1);
	if (pattachMD->GetWidget())
	{
		QWidget *wdgMD = (QWidget*)stackedWidget_main->widget(STACKED_MD_DDS1A);
		if( !wdgMD ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdgMD );
		stackedWidget_main->insertWidget(STACKED_MD_DDS1A, pattachMD->GetWidget());
	}	

	AttachChannelAccess *pattach2 = new AttachChannelAccess("/usr/local/opi/ui/DDS_Shutter.ui", 1);
	if (pattach2->GetWidget())
	{
		QWidget *wdg2 = (QWidget*)stackedWidget_main->widget(STACKED_SHUTTER_SHUTTER);
		if( !wdg2 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg2 );
		stackedWidget_main->insertWidget(STACKED_SHUTTER_SHUTTER, pattach2->GetWidget());
	}

	AttachChannelAccess *pattach3 = new AttachChannelAccess("/usr/local/opi/ui/DDS_EREF.ui", 1);
	if (pattach3->GetWidget())
	{
		QWidget *wdg3 = (QWidget*)stackedWidget_main->widget(STACKED_ELECTRON_REFLECTO);
		if( !wdg3 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg3 );
		stackedWidget_main->insertWidget(STACKED_ELECTRON_REFLECTO, pattach3->GetWidget());
//		btn_ER_OP = (QWidget *)pattach3->GetWidget()->caMbboBtn_ER_OP;
	}

	AttachChannelAccess *pattach4 = new AttachChannelAccess("/usr/local/opi/ui/DDS_RBA.ui", 1);
	if (pattach4->GetWidget())
	{
		QWidget *wdg4 = (QWidget*)stackedWidget_main->widget(STACKED_IMAGE_BOLOMETER);
		if( !wdg4 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg4 );
		stackedWidget_main->insertWidget(STACKED_IMAGE_BOLOMETER, pattach4->GetWidget());
	}

	AttachChannelAccess *pattach5 = new AttachChannelAccess("/usr/local/opi/ui/DDS_MC.ui", 1);
	if (pattach5->GetWidget())
	{
		QWidget *wdg5 = (QWidget*)stackedWidget_main->widget(STACKED_MD_MIRNOV);
		if( !wdg5 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg5 );
		stackedWidget_main->insertWidget(STACKED_MD_MIRNOV, pattach5->GetWidget());
	}
	
	AttachChannelAccess *pattach6 = new AttachChannelAccess("/usr/local/opi/ui/DDS_TS.ui", 1);
	if (pattach6->GetWidget())
	{
		QWidget *wdg6 = (QWidget*)stackedWidget_main->widget(STACKED_ELECTRON_THOMSON);
		if( !wdg6 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg6 );
		stackedWidget_main->insertWidget(STACKED_ELECTRON_THOMSON, pattach6->GetWidget());
	}

	AttachChannelAccess *pattach7 = new AttachChannelAccess("/usr/local/opi/ui/DDS_SXR.ui", 1);
	if (pattach7->GetWidget())
	{
		QWidget *wdg7 = (QWidget*)stackedWidget_main->widget(STACKED_IMAGE_SOFT_XR);
		if( !wdg7 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg7 );
		stackedWidget_main->insertWidget(STACKED_IMAGE_SOFT_XR, pattach7->GetWidget());
	}
	
	/* ECE HR */
	AttachChannelAccess *pattach8 = new AttachChannelAccess("/usr/local/opi/ui/DDS_ECEHR.ui", 1);
	if (pattach8->GetWidget())
	{
		QWidget *wdg8 = (QWidget*)stackedWidget_main->widget(STACKED_ECE_HR);
		if( !wdg8 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg8 );
		stackedWidget_main->insertWidget(STACKED_ECE_HR, pattach8->GetWidget());
	}

	/* POWER */
	AttachChannelAccess *pattach9 = new AttachChannelAccess("/usr/local/opi/ui/DDS_Power.ui", 1);
	if (pattach9->GetWidget())
	{
		QWidget *wdg9 = (QWidget*)stackedWidget_main->widget(STACKED_POWER);
		if( !wdg9 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg9 );
		stackedWidget_main->insertWidget(STACKED_POWER, pattach9->GetWidget());
	}

	AttachChannelAccess *pattach10 = new AttachChannelAccess("/usr/local/opi/ui/DDS_MMWI.ui", 1);
	if (pattach10->GetWidget())
	{
		QWidget *wdg10 = (QWidget*)stackedWidget_main->widget(STACKED_ELECTRON_MMWI);
		if( !wdg10 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg10 );
		stackedWidget_main->insertWidget(STACKED_ELECTRON_MMWI, pattach10->GetWidget());
	}

	AttachChannelAccess *pattach11 = new AttachChannelAccess("/usr/local/opi/ui/DDS_VSS.ui", 1);
	if (pattach11->GetWidget())
	{
		QWidget *wdg11 = (QWidget*)stackedWidget_main->widget(STACKED_SPECTRO_VS_SPECT);
		if( !wdg11 ) {
			QMessageBox::information(0, "Kstar VSS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg11 );
		stackedWidget_main->insertWidget(STACKED_SPECTRO_VS_SPECT, pattach11->GetWidget());
	}

	AttachChannelAccess *pattach12 = new AttachChannelAccess("/usr/local/opi/ui/DDS_VBS.ui", 1);
	if (pattach12->GetWidget())
	{
		QWidget *wdg12 = (QWidget*)stackedWidget_main->widget(STACKED_VBS);
		if( !wdg12 ) {
			QMessageBox::information(0, "Kstar VBS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg12 );
		stackedWidget_main->insertWidget(STACKED_VBS, pattach12->GetWidget());
	}

	AttachChannelAccess *pattach13 = new AttachChannelAccess("/usr/local/opi/ui/DDS_HALPHA.ui", 1);
	if (pattach13->GetWidget())
	{
		QWidget *wdg13 = (QWidget*)stackedWidget_main->widget(STACKED_H_ALPHA);
		if( !wdg13 ) {
			QMessageBox::information(0, "Kstar HAlpha", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg13 );
		stackedWidget_main->insertWidget(STACKED_H_ALPHA, pattach13->GetWidget());
	}
	
	AttachChannelAccess *pattach14 = new AttachChannelAccess("/usr/local/opi/ui/DDS_MCWIN.ui", 1);
	if (pattach14->GetWidget())
	{
		QWidget *wdg14 = (QWidget*)stackedWidget_main->widget(STACKED_MD_MIRNOV2);
		if( !wdg14 ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR MCWIN") );
			return;
		}
		stackedWidget_main->removeWidget( wdg14 );
		stackedWidget_main->insertWidget(STACKED_MD_MIRNOV2, pattach14->GetWidget());
	}

	AttachChannelAccess *pattach15 = new AttachChannelAccess("/usr/local/opi/ui/DDS_FILD.ui", 1);
	if (pattach15->GetWidget())
	{
		QWidget *wdg15 = (QWidget*)stackedWidget_main->widget(STACKED_FILD);
		if( !wdg15 ) {
			QMessageBox::information(0, "Kstar FILD", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdg15 );
		stackedWidget_main->insertWidget(STACKED_FILD, pattach15->GetWidget());
	}

	// -------------------------------------------------------------
	// 20120705 yunsw : creates main window for IRTV
	// -------------------------------------------------------------
	QWidget *wdgIRTV = (QWidget*)stackedWidget_main->widget(STACKED_IRTV_S1);
	new IRTV_S1Window (wdgIRTV);

	AttachChannelAccess *pattachIRTV = new AttachChannelAccess(wdgIRTV);
	if (pattachIRTV->GetWidget())
	{
		QWidget *wdgIRTV = (QWidget*)stackedWidget_main->widget(STACKED_IRTV_S1);
		if( !wdgIRTV ) {
			QMessageBox::information(0, "Kstar IRTV", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdgIRTV );
		stackedWidget_main->insertWidget(STACKED_IRTV_S1, pattachIRTV->GetWidget());
	}

	wdgIRTV = (QWidget*)stackedWidget_main->widget(STACKED_IRTV_D1);
	new IRTV_D1Window (wdgIRTV);

	pattachIRTV = new AttachChannelAccess(wdgIRTV);
	if (pattachIRTV->GetWidget())
	{
		QWidget *wdgIRTV = (QWidget*)stackedWidget_main->widget(STACKED_IRTV_D1);
		if( !wdgIRTV ) {
			QMessageBox::information(0, "Kstar IRTV", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdgIRTV );
		stackedWidget_main->insertWidget(STACKED_IRTV_D1, pattachIRTV->GetWidget());
	}

	AttachChannelAccess *pIRVB = new AttachChannelAccess("/usr/local/opi/ui/DDS_IRVB.ui", 1);
	if (pIRVB->GetWidget())
	{
		QWidget *wdgIRVB = (QWidget*)stackedWidget_main->widget(STACKED_IMAGE_IRVB);
		if( !wdgIRVB ) {
			QMessageBox::information(0, "Kstar DDS", tr("get StackedWidget ERROR") );
			return;
		}
		stackedWidget_main->removeWidget( wdgIRVB );
		stackedWidget_main->insertWidget(STACKED_IMAGE_IRVB, pIRVB->GetWidget());
	}
}


void MainWindow::toolBox_currentChanged(int index)
{
	setToolBox_btn_color_default();
	
	switch(index) {
		case TOOLBOX_ID_MD: 
			stackedWidget_main->setCurrentIndex(STACKED_MD_DDS1A);
			btn_toolbox_DDS1->setPalette(pal_select);
			break;
		case TOOLBOX_ID_MMW:
			stackedWidget_main->setCurrentIndex(STACKED_H_ALPHA);
			btn_toolbox_HAlpha->setPalette(pal_select);
			break;
		case TOOLBOX_ID_ELECTRON:
			stackedWidget_main->setCurrentIndex(STACKED_ELECTRON_REFLECTO); 
			btn_toolbox_Reflecto->setPalette(pal_select);
			break;
		case TOOLBOX_ID_ION:
			stackedWidget_main->setCurrentIndex(STACKED_ION_XIC_SPECTRO); 
			btn_toolbox_XIC->setPalette(pal_select);
			break;
		case TOOLBOX_ID_IMAGE: 
			stackedWidget_main->setCurrentIndex(STACKED_IMAGE_BOLOMETER); 
			btn_toolbox_BOLO->setPalette(pal_select);
			break;
		case TOOLBOX_ID_SPECTRO: 
			stackedWidget_main->setCurrentIndex(STACKED_SPECTRO_VS_SPECT);
			btn_toolbox_VS->setPalette(pal_select);
			break;
		case TOOLBOX_ID_SHUTTER:
			stackedWidget_main->setCurrentIndex(STACKED_SHUTTER_SHUTTER); 
			btn_toolbox_SHUT->setPalette(pal_select);
			break;
		default: break;
	}
//	stackedWidget_main->setCurrentIndex(index);
/*
	if(index == TOOLBOX_ID_SPECTRO) {
		myProcess = new QProcess(this);
		printStatus( "call vncviewer!\n");
		QString program="vncviewer";
		QStringList arguments;
		//arguments << "172.17.102.51";
		arguments <<"-ViewOnly"<<"-LowColourLevel"<<"2"<< "172.17.102.52:5900";
		myProcess->start(program, arguments);
		qDebug("Process id (init): %d", (int)myProcess->pid());
	} 
*/
}

int MainWindow::checkPasswd(int ID)
{
//	char buf[64];
	passwordDlg passdlg;
//	int index = toolBox->currentIndex();

	switch(ID) {
		case TOOLBOX_ID_MD: strcpy(str_passwd, STR_PASSWD_MD); break;
		case TOOLBOX_ID_MMW: strcpy(str_passwd, STR_PASSWD_MMW); break;
		case TOOLBOX_ID_ELECTRON: strcpy(str_passwd, STR_PASSWD_ELECTRON); break;
		case TOOLBOX_ID_ION: strcpy(str_passwd, STR_PASSWD_ION); break;
		case TOOLBOX_ID_IMAGE: strcpy(str_passwd, STR_PASSWD_IMAGE); break;
		case TOOLBOX_ID_SPECTRO: strcpy(str_passwd, STR_PASSWD_SPECTRO); break;
		case TOOLBOX_ID_SHUTTER: strcpy(str_passwd, STR_PASSWD_SHUTTER); break;
		
		default: 
			return 0;
	}

	if ( passdlg.exec() == QDialog::Accepted)
	{
		if( strcmp(str_passwd, (passdlg.lineEdit_password->text()).toAscii().constData() ) != 0 ) {
			QMessageBox::information(0, "Kstar DDS", tr("wrong pass word!") );
//			toolBox->setCurrentIndex(0);
			return 0;
		} else {

//			stackedWidget_main->setCurrentIndex(index);
//			prev_stackedWidget_index = index;
//			prev_toolBox_index = index;
//			toolBox->setCurrentIndex(prev_toolBox_index);

			switch(ID) {
				case TOOLBOX_ID_MD: g_nCurrentSystem = CURR_ACQ196; break;
				case TOOLBOX_ID_MMW: g_nCurrentSystem = CURR_M6802; break;
				default: return 0;
			}
/*
			sprintf(buf, "prev_stackedWidget_index : %d", prev_stackedWidget_index);
			printStatus( buf );
			sprintf(buf, "prev_toolBox_index : %d", prev_toolBox_index);
			printStatus( buf );
*/
#if 0
			if( !initPVmonitoringTimer() ) {
				g_nCurrentSystem = 0;
				QMessageBox::information(0, "Kstar DDS", tr("check IOC running!") );
				return 0;
			}
#endif
//			g_IsFirstEnterance = false;
			

		}

		return 1; /* return OK */
	}

	return 0; /* return ERROR */

}
/*
void MainWindow::linkLogout()
{
	killPVmonitoringTimer();

	saveConfigreFile();

	initOPIstate();

	setAllButtonLabelDisable();
}
*/

void MainWindow::createActions()
{
	actionNew->setIcon(QIcon(":images/DDS_new.png") );
	actionNew->setShortcut(tr("Ctrl+N"));
	actionNew->setStatusTip(tr("Create a new set"));
	connect(actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));

	actionOpen->setIcon(QIcon(":images/DDS_open.png") );
	actionOpen->setShortcut(tr("Ctrl+O"));
	actionOpen->setStatusTip(tr("Open an existing file"));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

	actionSave->setIcon(QIcon(":images/DDS_save.png") );
	actionSave->setShortcut(tr("Ctrl+S"));
	actionSave->setStatusTip(tr("Save current file"));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));

	actionSave_As->setStatusTip(tr("Save current file as ..."));
	connect(actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

	actionPrint->setIcon(QIcon(":images/DDS_print.png") );
	actionPrint->setShortcut(tr("Ctrl+P"));
	actionPrint->setStatusTip(tr("Print the current data"));
	connect(actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

	actionExit->setShortcut(tr("Ctrl+Q"));
	actionExit->setStatusTip(tr("Exit the application"));
	connect(actionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));


	actionJSCOPE->setStatusTip(tr("Open JSope"));
	connect(actionJSCOPE, SIGNAL(triggered()), this, SLOT(linkJSCOPE()));

	actionSignalDB->setStatusTip(tr("Open web browser"));
	connect(actionSignalDB, SIGNAL(triggered()), this, SLOT(linkSignalDB()));

	actionOpenEditor->setStatusTip(tr("Open Descriptor Dialog"));
	connect(actionOpenEditor, SIGNAL(triggered()), this, SLOT(linkOpenEditor()));


//	actionLogout->setStatusTip(tr("Logout OPI") );
//	connect(actionLogout, SIGNAL(triggered()), this, SLOT(linkLogout()));

}

void MainWindow::createToolbars()
{
	fileToolbar->addAction(actionNew);
	fileToolbar->addAction(actionOpen);
	fileToolbar->addAction(actionSave);
	fileToolbar->addAction(actionPrint);

#if 0
	// logout
	QToolBar *adminToolbar = new QToolBar;
	addToolBar(adminToolbar);
	
	adminToolbar->setIconSize(QSize(18, 18) );
	adminToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	adminToolbar->setWindowTitle(tr("Format Actions"));
	actionLogout->setIcon(QIcon(":images/DDS_undo.png") );
	adminToolbar->addAction(actionLogout );
#endif

#if 1	
	logoTB = new QToolBar(this);
	logoTB->setObjectName(QString::fromUtf8("logoToolBar"));

	logoTB->setOrientation(Qt::Horizontal);
	addToolBar(static_cast<Qt::ToolBarArea>(4), logoTB);

	QFrame *tbframe = new QFrame();
    logoTB->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(240, 10, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Preferred, QSizePolicy::Fixed);


	CAGraphic *iocDDS1A = new CAGraphic();		// DDS1A
	iocDDS1A->setLineWidth(2);
	iocDDS1A->setMinimumSize(QSize(20,20));
	iocDDS1A->setMaximumSize(QSize(20,20));
	iocDDS1A->setFillColor(QColor("yellow"));
	iocDDS1A->setFillMode(StaticGraphic::Solid);
	iocDDS1A->setLineColor(QColor("black"));
	iocDDS1A->setPvname("DDS1A_HEARTBEAT");
	iocDDS1A->setFillDisplayMode(CAGraphic::ActInact);
	iocDDS1A->setObjectName("CAGraphic_iocDDS1A");

	CAGraphic *iocDDS1B = new CAGraphic();		// DDS1B
	iocDDS1B->setLineWidth(2);
	iocDDS1B->setMinimumSize(QSize(20,20));
	iocDDS1B->setMaximumSize(QSize(20,20));
	iocDDS1B->setFillColor(QColor("yellow"));
	iocDDS1B->setFillMode(StaticGraphic::Solid);
	iocDDS1B->setLineColor(QColor("black"));
	iocDDS1B->setPvname("DDS1B_HEARTBEAT");
	iocDDS1B->setFillDisplayMode(CAGraphic::ActInact);
	iocDDS1B->setObjectName("CAGraphic_iocDDS1B");

	CAGraphic *iocMC = new CAGraphic();		// Mirnov Coil 
	iocMC->setLineWidth(2);
	iocMC->setMinimumSize(QSize(20,20));
	iocMC->setMaximumSize(QSize(20,20));
	iocMC->setFillColor(QColor("yellow"));
	iocMC->setFillMode(StaticGraphic::Solid);
	iocMC->setLineColor(QColor("black"));
	iocMC->setPvname("MC_HEARTBEAT");
	iocMC->setFillDisplayMode(CAGraphic::ActInact);
	iocMC->setObjectName("CAGraphic_iocMC");
	
	CAGraphic *iocMCWIN = new CAGraphic();		// Mirnov Coil2 
	iocMCWIN->setLineWidth(2);
	iocMCWIN->setMinimumSize(QSize(20,20));
	iocMCWIN->setMaximumSize(QSize(20,20));
	iocMCWIN->setFillColor(QColor("yellow"));
	iocMCWIN->setFillMode(StaticGraphic::Solid);
	iocMCWIN->setLineColor(QColor("black"));
	iocMCWIN->setPvname("MCWIN_HEARTBEAT");
	iocMCWIN->setFillDisplayMode(CAGraphic::ActInact);
	iocMCWIN->setObjectName("CAGraphic_iocMCWIN");
	

	CAGraphic *iocHALPHA = new CAGraphic();		// HALPHA
	iocHALPHA->setLineWidth(2);
	iocHALPHA->setMinimumSize(QSize(20,20));
	iocHALPHA->setMaximumSize(QSize(20,20));
	iocHALPHA->setFillColor(QColor(255, 255, 0));
	iocHALPHA->setFillMode(StaticGraphic::Solid);
	iocHALPHA->setLineColor(QColor("black"));
	iocHALPHA->setPvname("HALPHA_HeartBeat");
	iocHALPHA->setFillDisplayMode(CAGraphic::ActInact);
	iocHALPHA->setObjectName("CAGraphic_iocHALPHA");

	CAGraphic *iocECEHR = new CAGraphic();		// ECEHR
	iocECEHR->setLineWidth(2);
	iocECEHR->setMinimumSize(QSize(20,20));
	iocECEHR->setMaximumSize(QSize(20,20));
	iocECEHR->setFillColor(QColor(255, 255, 0));
	iocECEHR->setFillMode(StaticGraphic::Solid);
	iocECEHR->setLineColor(QColor("black"));
	iocECEHR->setPvname("ECEHR_HeartBeat");
	iocECEHR->setFillDisplayMode(CAGraphic::ActInact);
	iocECEHR->setObjectName("CAGraphic_iocECEHR");

	CAGraphic *iocER = new CAGraphic();		// Edge Reflectometer 
	iocER->setLineWidth(2);
	iocER->setMinimumSize(QSize(20,20));
	iocER->setMaximumSize(QSize(20,20));
	iocER->setFillColor(QColor("yellow"));
	iocER->setFillMode(StaticGraphic::Solid);
	iocER->setLineColor(QColor("black"));
	iocER->setPvname("ER_HEARTBEAT");
	iocER->setFillDisplayMode(CAGraphic::ActInact);
	iocER->setObjectName("CAGraphic_iocER");

	CAGraphic *iocTS = new CAGraphic();		//X-ray Imaging Crystal Spectroscopy 	
	iocTS->setLineWidth(2);
	iocTS->setMinimumSize(QSize(20,20));
	iocTS->setMaximumSize(QSize(20,20));
	iocTS->setFillColor(QColor("yellow"));
	iocTS->setFillMode(StaticGraphic::Solid);
	iocTS->setLineColor(QColor("black"));
	iocTS->setPvname("TS1_HEARTBEAT");
	iocTS->setFillDisplayMode(CAGraphic::ActInact);
	iocTS->setObjectName("CAGraphic_iocTS");

	CAGraphic *iocRBA = new CAGraphic();		//Resistive Bolometer Array 
	iocRBA->setLineWidth(2);
	iocRBA->setMinimumSize(QSize(20,20));
	iocRBA->setMaximumSize(QSize(20,20));
	iocRBA->setFillColor(QColor("yellow"));
	iocRBA->setFillMode(StaticGraphic::Solid);
	iocRBA->setLineColor(QColor("black"));
	iocRBA->setPvname("RBA_HEARTBEAT");
	iocRBA->setFillDisplayMode(CAGraphic::ActInact);
	iocRBA->setObjectName("CAGraphic_iocRBA");

	CAGraphic *iocSXR = new CAGraphic();		//Resistive Bolometer Array 
	iocSXR->setLineWidth(2);
	iocSXR->setMinimumSize(QSize(20,20));
	iocSXR->setMaximumSize(QSize(20,20));
	iocSXR->setFillColor(QColor("yellow"));
	iocSXR->setFillMode(StaticGraphic::Solid);
	iocSXR->setLineColor(QColor("black"));
	iocSXR->setPvname("SXR_HEARTBEAT");
	iocSXR->setFillDisplayMode(CAGraphic::ActInact);
	iocSXR->setObjectName("CAGraphic_iocSXR");

	CAGraphic *iocMMWI = new CAGraphic();		//MMWI 
	iocMMWI->setLineWidth(2);
	iocMMWI->setMinimumSize(QSize(20,20));
	iocMMWI->setMaximumSize(QSize(20,20));
	iocMMWI->setFillColor(QColor("yellow"));
	iocMMWI->setFillMode(StaticGraphic::Solid);
	iocMMWI->setLineColor(QColor("black"));
	iocMMWI->setPvname("MMWI_HEARTBEAT");
	iocMMWI->setFillDisplayMode(CAGraphic::ActInact);
	iocMMWI->setObjectName("CAGraphic_iocMMWI");

	CAGraphic *iocVSS = new CAGraphic();		//VSS 
	iocVSS->setLineWidth(2);
	iocVSS->setMinimumSize(QSize(20,20));
	iocVSS->setMaximumSize(QSize(20,20));
	iocVSS->setFillColor(QColor("yellow"));
	iocVSS->setFillMode(StaticGraphic::Solid);
	iocVSS->setLineColor(QColor("black"));
	iocVSS->setPvname("VSS_HEARTBEAT");
	iocVSS->setFillDisplayMode(CAGraphic::ActInact);
	iocVSS->setObjectName("CAGraphic_iocVSS");

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock();
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("CCS_SYS_TIME.RVAL");
	wclock1->setFont(clockfont);
	wclock1->setObjectName("CAWclock_wclock1");
	
    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":images/DDS_kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);

	tblayout->addWidget(iocDDS1A);
	tblayout->addWidget(iocDDS1B);
	tblayout->addWidget(iocMC);
	tblayout->addWidget(iocMCWIN);
	tblayout->addWidget(iocHALPHA);
	tblayout->addWidget(iocMMWI);
	tblayout->addWidget(iocECEHR);
	tblayout->addWidget(iocER);
	tblayout->addWidget(iocTS);
	tblayout->addWidget(iocRBA);
	tblayout->addWidget(iocSXR);
	tblayout->addWidget(iocVSS);
	
	tblayout->addItem(tbspacer2);
	tblayout->addWidget(logo);
	
	AttachChannelAccess *pattachTLB = new AttachChannelAccess(tbframe);  

#endif


}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

// http://www.qtcentre.org/forum/f-qt-programming-2/t-qtableview-printing-3796.html#10
void MainWindow::filePrint()
{
/*	QTextDocument *document = textEdit_status->document();
	QPrinter printer;

	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	document->print(&printer);
*/

/*	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog dlg(&printer, this);

	if (dlg.exec() == QDialog::Accepted){
		// calculate the total width/height table would need without scaling    
		const int rows = tableWidget_ACQ196->model()->rowCount();
		const int cols = tableWidget_ACQ196->model()->columnCount(); 
		double totalWidth = 0.0; 
		for (int c = 0; c < cols; ++c)    { 
			totalWidth += tableWidget_ACQ196->columnWidth(c);   
		}

		double totalHeight = 0.0;
		for (int r = 0; r < rows; ++r)    { 
			totalHeight += tableWidget_ACQ196->rowHeight(r);
		}

		// redirect table's painting on a pixmap  
		QPixmap pixmap((int)totalWidth, (int)totalHeight);  
		QPainter::setRedirected(tableWidget_ACQ196->viewport(), &pixmap);  
		QPaintEvent event(QRect(0, 0, totalWidth, totalHeight));   
		QApplication::sendEvent(tableWidget_ACQ196->viewport(), &event);  
		QPainter::restoreRedirected(tableWidget_ACQ196->viewport());   
		// print scaled pixmap 
		QPainter painter(&printer);  
		painter.drawPixmap(printer.pageRect(), pixmap, pixmap.rect());
	}
*/
	statusBar()->showMessage(tr("File Printing"), 2000);
}

void MainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this);

//	if (!fileName.isEmpty())
//		;

	statusBar()->showMessage(tr("File Opened"), 2000);

}

void MainWindow::linkOpenEditor()
{

	ddsDescDlg descdlg;

	descdlg.InitWindow(this);

	if ( descdlg.exec() == QDialog::Accepted)
	{
		printStatus("Close test editor");
	}

}

void MainWindow::fileNew()
{
	statusBar()->showMessage(tr("Create new digitizer set"), 2000);
}

void MainWindow::fileSave()
{
	statusBar()->showMessage(tr("Save current digitizer set"), 2000);
}

void MainWindow::fileSaveAs()
{
	statusBar()->showMessage(tr("Save current digitizer set as ..."), 2000);
}

void MainWindow::linkJSCOPE()
{
#ifdef	WIN32
	system("cd c:\\Program files\\MDSplus");
	system("java -jar jScope.jar");
#else
//	system("/usr/local/mdsplus/bin/jScope &");
	system("java -Xms128m -Xmx512m -jar /home/kstar/jScope/jScope.jar &");
#endif
}

void MainWindow::linkSignalDB()
{
	char buf[64];

	sprintf(buf, "firefox www.nfrc.re.kr &");
#ifndef	WIN32
	system(buf);
#endif

}


void MainWindow::initOPIstate()
{	
	g_nCurrentSystem = 0;
	g_opMode = SYS_MODE_INIT;

	g_mInternalExternal = SYS_CLOCK_INTERNAL;

	toolBox->setCurrentIndex(TOOLBOX_ID_MD);
	stackedWidget_main->setCurrentIndex(STACKED_MD_DDS1A);	//setCurrentWidget(page_entrance);

//	emit signal_printStatus(buf);
//	emit toolBox->currentChanged(TOOLBOX_ID_MD);


	setToolBox_btn_color_default();
	btn_toolbox_DDS1->setPalette(pal_select);

}

void MainWindow::setAllButtonLabelCondition()
{
	label_rogowskiCoil->setPalette(pal_normal);
	label_fluxLoops->setPalette(pal_normal);
	label_magneticProbe->setPalette(pal_normal);
	label_lockedMode->setPalette(pal_normal);
	label_saddleLoop->setPalette(pal_normal);
	label_diamagneticLoop->setPalette(pal_normal);
	label_vesselCurrent->setPalette(pal_normal);
	label_haloCmonitor->setPalette(pal_normal);
	
/*	label_interferometer->setPalette(pal_normal);   */

	// toolbox label
	label_rogowskiCoil->setEnabled(false);
	label_fluxLoops->setEnabled(false);
	label_magneticProbe->setEnabled(false);
	label_lockedMode->setEnabled(false);
	label_saddleLoop->setEnabled(false);
	label_diamagneticLoop->setEnabled(false);
	label_vesselCurrent->setEnabled(false);
	label_haloCmonitor->setEnabled(false);
/*	label_interferometer->setEnabled(false);  */
}

void MainWindow::setToolBox_btn_color_default()
{
	btn_toolbox_DDS1->setPalette(pal_normal);
	btn_toolbox_Mirnov->setPalette(pal_normal);
	btn_toolbox_HAlpha->setPalette(pal_normal);
	btn_toolbox_VBS->setPalette(pal_normal);
	btn_toolbox_MMWI->setPalette(pal_normal);
	btn_toolbox_ECEHR->setPalette(pal_normal);
	
	btn_toolbox_Reflecto->setPalette(pal_normal);
	btn_toolbox_Probe->setPalette(pal_normal);
	btn_toolbox_Thomson->setPalette(pal_normal);
	btn_toolbox_XIC->setPalette(pal_normal);
	btn_toolbox_CE->setPalette(pal_normal);
	btn_toolbox_BOLO->setPalette(pal_normal);
	btn_toolbox_SXR->setPalette(pal_normal);
	btn_toolbox_VS->setPalette(pal_normal);
	btn_toolbox_FILTER->setPalette(pal_normal);
	btn_toolbox_HXR->setPalette(pal_normal);
	
	btn_toolbox_SHUT->setPalette(pal_normal);
	btn_toolbox_POWER->setPalette(pal_normal);
	btn_toolbox_IRTV_S1->setPalette(pal_normal);
	btn_toolbox_Mirnov2->setPalette(pal_normal);
	btn_toolbox_FILD->setPalette(pal_normal);
	btn_toolbox_IRTV_D1->setPalette(pal_normal);
	btn_toolbox_IRVB->setPalette(pal_normal);
	

}

void MainWindow::slot_printStatus(char *buf )
{
	char strbuf[1024];
	int len;
//	struct timeval mytime;

	sprintf(strbuf, "%05d", status_line_cnt);
	status_line_cnt++;

	len = strlen(buf);
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;


	QDateTime dtime = QDateTime::currentDateTime();	
	QString text = dtime.toString("<yyyy:MM:dd-hh:mm:ss> : ");

//	gettimeofday(&mytime, NULL); 
//	printf("%ld:%ld\n", mytime.tv_sec, mytime.tv_usec); 
	strcat(strbuf, text.toAscii().constData() );

	strcat(strbuf, buf);
	
	listWidget_status->addItem( strbuf );
		   
	int row = listWidget_status->count() ;
	listWidget_status->setCurrentRow( row-1 );
	
}

void MainWindow::printStatus(const char *fmt, ... )
{
	char buf[1024];
	va_list argp;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

//	printf("send signal: %s", buf);
	
	emit signal_printStatus(buf);
}


void MainWindow::init_pallet_color()
{
	QBrush brush_warning(QColor(255, 0, 0, 255));
	QBrush brush_select(QColor(85, 170, 0, 255));
	QBrush brush_normal(QColor(107, 107, 159, 255));
	QBrush brush_black(QColor(0, 0, 0, 255));
	brush_select.setStyle(Qt::SolidPattern);
	brush_warning.setStyle(Qt::SolidPattern);
	brush_normal.setStyle(Qt::SolidPattern);

	pal_select.setBrush(QPalette::Active, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Active, QPalette::Window, brush_select);
	pal_select.setBrush(QPalette::Active, QPalette::ButtonText,  brush_black);
	pal_select.setBrush(QPalette::Inactive, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Inactive, QPalette::Window, brush_select);
	pal_select.setBrush(QPalette::Disabled, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Disabled, QPalette::Window, brush_select);

	pal_warning.setBrush(QPalette::Active, QPalette::Button, brush_warning);
	pal_warning.setBrush(QPalette::Active, QPalette::Window, brush_warning);
	pal_warning.setBrush(QPalette::Inactive, QPalette::Button, brush_warning);
	pal_warning.setBrush(QPalette::Inactive, QPalette::Window, brush_warning);
	pal_warning.setBrush(QPalette::Disabled, QPalette::Button, brush_warning);
	pal_warning.setBrush(QPalette::Disabled, QPalette::Window, brush_warning);

	pal_normal.setBrush(QPalette::Active, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Active, QPalette::Window, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Window, brush_normal);
	pal_normal.setBrush(QPalette::Disabled, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Disabled, QPalette::Window, brush_normal);

}


//void MainWindow::setDDS1StateFromPV(int ID)
void MainWindow::setDDS1StateFromPV(ST_QUEUE_STATUS queueNode)
{
#if 0
	char buf[1024];
	int nCnt = 0;
	
//	sprintf(buf, "Status: ID:%d, 0x%X\n", queueNode.id, queueNode.status );
//	printStatus(buf);
//	printf(buf);
	
	strcpy(buf, "");

	if( queueNode.status & TASK_AFTER_SHOT_PROCESS  ) {
		strcat(buf, "Data Parsing / ");
		nCnt++;
	} 	
	else if ( queueNode.status & TASK_DATA_PUT_STORAGE ) {
		strcat(buf, "Storing to MDS Tree / ");
		nCnt++;
	} 
	else if ( queueNode.status & TASK_DURING_DAQ) {
		strcat(buf, "During DAQ / ");
		nCnt++;
	} 
	else if ( queueNode.status & TASK_WAITING_TRIGGER ) {
		strcat(buf, "Waiting Trigger / ");
		nCnt++;
	} 
	else if ( queueNode.status & TASK_ARM_ENABLED ) {
		strcat(buf, "Arming / ");
		nCnt++;
		pg_dds1.is_Arm_OK[queueNode.id] = 1; /* 2010.8.12   to button enable control   woong */
	}
	else if ( (queueNode.status & TASK_SYSTEM_READY) &&
		(queueNode.status & TASK_SYSTEM_IDLE) )
	{	
		strcpy(buf, "Ready to RUN. / ");
		nCnt++;
	} 
/*	if ( queueNode.status & TASK_REMOTE_STORAGE ) {
		strcat(buf, "Storing to Remote Tree / ");
		nCnt++;
	} */
	if( queueNode.status & TASK_CHANNELIZATION_FAIL  ) {
		strcat(buf, "Parsing error! / ");
		nCnt++;
	} 
	
	if( queueNode.status & TASK_STORAGE_FAIL  ) {
		strcat(buf, "Storing failed!! / ");
		nCnt++;
	}
/*
	if( queueNode.status & TASK_READY_TO_SAVE ) {
		strcat(buf, "Click to Save / ");
		nCnt++;
	}
*/
	if( queueNode.status & TASK_STORAGE_FINISH ) {
		strcat(buf, "Finished! / ");
		nCnt++;
	}
	
	if( nCnt == 0) 
		strcpy(buf, "Not Initialized. / ");
	
	switch(queueNode.id){
		case B1: label_dds1DAQstate->setText(buf); break;
		case B2: label_dds1DAQstate_2->setText(buf); break;
		case B3: label_dds1DAQstate_3->setText(buf); break;
		case B4: label_dds1DAQstate_4->setText(buf); break;
		case B5: label_dds1DAQstate_5->setText(buf); break;
		case B6: label_dds1DAQstate_6->setText(buf); break;
		case B7: label_dds1DAQstate_7->setText(buf); break;
		case B8: label_dds1DAQstate_8->setText(buf); break;
	}

	nCnt = 0;
	for(int i=0; i<USED_ACQ196; i++) {
		if( pg_dds1.is_Arm_OK[i] == 1) nCnt++;
	}
	if( nCnt == USED_ACQ196 )
		btn_dds1LocalRun->setEnabled(true);
	#endif
}

//void MainWindow::setDDS2StateFromPV(unsigned int nval)
//{
//		btn_sendReady->setEnabled(false);
/*	if ( nval & TASK_SYSTEM_READY)
	{	
		label_dds2DAQstate->setText( "SYSTEM READY" );
	} else {
		if( nval & TASK_AFTER_SHOT_PROCESS  ) {
			label_dds2DAQstate->setText( "Data channelization ..." );
		} else if ( nval & TASK_DATA_PUT_STORAGE ) {
			label_dds2DAQstate->setText( "Data storing ..." );
		} else if ( nval & TASK_WAITING_TRIGGER ) {
			label_dds2DAQstate->setText( "DAQ starting ..." );
		} else if ( !(nval & TASK_WAITING_TRIGGER ) ) {
			label_dds2DAQstate->setText( "DAQ stopped" );
		} else if ( nval & TASK_ARM_ENABLED ) {
			label_dds2DAQstate->setText( "ADC starting ..." );
		} else if ( !(nval & TASK_ARM_ENABLED) ) {
			label_dds2DAQstate->setText( "ADC stopped" );
		}
	}
	*/
//}
/*
void MainWindow::initDAQBoardState()
{
	if( g_nCurrentSystem == CURR_M6802 )
//		pg_dds2.InitLocalValue();
	else if( g_nCurrentSystem == CURR_ACQ196 ) {
		pg_dds1.InitLocalValue(B1);
		pg_dds1.InitLocalValue(B2);
		pg_dds1.InitLocalValue(B3);
		pg_dds1.InitLocalValue(B4);

	}
}
*/
int swap32(unsigned int *n)
{
	unsigned char *cptr, tmp0, tmp1;

	cptr = (unsigned char*)n;
	
	tmp0 = cptr[0];
	tmp1 = cptr[1];

	cptr[0] = cptr[3];
	cptr[1] = cptr[2];
	cptr[2] = tmp1;
	cptr[3] = tmp0;

	return 1;
}

/*
void MainWindow::timerFuncMonitorPV()
{
	if( g_mInternalExternal == SYS_CLOCK_EXTERNAL )
		updateTrigClockFromPV();
}
*/
void MainWindow::putQMyNode(ST_QUEUE_STATUS queueNode)
{
	pthread->putQueue( queueNode );
}

void MainWindow::patchMessage(chid *myid, unsigned int uintVal )
{
#if 0
	ST_QUEUE_STATUS queueNode;

//	printf("MainWindow::patchMessage: chid: %d, val: %d\n", (unsigned int) *myid, uintVal);
/*
	if( *myid == pg_dds1.DDS1_getState[B1] )
	{
		queueNode.status = uintVal;
		queueNode.id = B1;
		pthread->putQueue( queueNode );
//		printf("DDS1_getState[B1]: 0x%x\n", queueNode.status );
	} else if( *myid == pg_dds1.DDS1_getState[B2] ) {
		queueNode.status = uintVal;
		queueNode.id = B2;
		pthread->putQueue( queueNode );
//		printf("DDS1_getState[B2]: 0x%x\n", queueNode.status );
	} else if( *myid == pg_dds1.DDS1_getState[B3] ) {
		queueNode.status = uintVal;
		queueNode.id = B3;
		pthread->putQueue( queueNode );
//		printf("DDS1_getState[B3]: 0x%x\n", queueNode.status );
	} else if( *myid == pg_dds1.DDS1_getState[B4] ) {
		queueNode.status = uintVal;
		queueNode.id = B4;
		pthread->putQueue( queueNode );
//		printf("DDS1_getState[B4]: 0x%x\n", queueNode.status );
	}
*/
/*
	for( int i=0; i< USED_ACQ196; i++) 
	{
		if( *myid == pg_dds1.DDS1_getState[i] )
		{
			queueNode.status = uintVal;
			queueNode.id = i;
			pthread->putQueue( queueNode );
	//		printf("DDS1_getState[B1]: 0x%x\n", queueNode.status );
			return;
		}
	}
*/
/*
	if( *myid == pg_dds2.chid_DDS2_getState ) {
		queueNode.status = uintVal;
		queueNode.id = QUEUE_THREAD_DDS2;
		pthread->putQueue( queueNode );
//		setDDS2StateFromPV(uintVal);
//		printf("g_opStateDDS2: %d\n", uintVal);
	}
*/

	if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B1]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B1] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B1] ) 
	) {
		label_dev1_cnt->setText(QString("%1").arg(  uintVal ) );
	} else if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B2]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B2] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B2] ) 
	) {
		label_dev2_cnt->setText(QString("%1").arg(  uintVal ) );
	} else if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B3]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B3] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B3] ) 
	) {
		label_dev3_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B4]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B4] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B4] ) 
	) {
		label_dev4_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B5]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B5] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B5] ) 
	) {
		label_dev5_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( (*myid == pg_dds1.DDS1_BX_PARSING_CNT[B6]) ||
		( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B6] ) ||
		( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B6] ) 
	) {
		label_dev6_cnt->setText(QString("%1").arg(  uintVal) );
	}
/*	
	else if ( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B1] ) {
		label_dev1_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B2] ) {
		label_dev2_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B3] ) {
		label_dev3_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_MDS_SND_CNT[B4] ) {
		label_dev4_cnt->setText(QString("%1").arg(  uintVal) );
	}
	else if ( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B1] ) {
		label_dev1_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B2] ) {
		label_dev2_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B3] ) {
		label_dev3_cnt->setText(QString("%1").arg(  uintVal) );
	} else if ( *myid == pg_dds1.DDS1_BX_DAQING_CNT[B4] ) {
		label_dev4_cnt->setText(QString("%1").arg(  uintVal) );
	}  */
	// 2009. 10. 13........  for notice that CCS make disable to DDS1A
/*	else if(  *myid == pg_dds1.CCS_SHOTSEQ_START ) {
		pg_dds1.nEnable_to_RUN = uintVal;
		if( uintVal ) {
			label_ccs_block->setText(QString("Approved to Run\nby CCS") );
			label_ccs_block->setPalette(pal_normal);
		} else {
			label_ccs_block->setText(QString("Blocked to Run\nby CCS") );
			label_ccs_block->setPalette(pal_warning);
		}
	} 
*/
	else {
		printStatus("got undef chidEventCB: %x, %d\n", (unsigned int)*myid,  uintVal);
	}
#endif
}

/*
void MainWindow::clicked_dds2SetupEnable()
{
	if( ckb_dds2SetupEnable->isChecked() )
		tabWidget_m6802->setEnabled(true);
	else tabWidget_m6802->setEnabled(false);
}
*/
	
	
	

void MainWindow::clicked_btn_toolbox_DDS1()
{
	stackedWidget_main->setCurrentIndex(STACKED_MD_DDS1A);
	setToolBox_btn_color_default();
	btn_toolbox_DDS1->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_HAlpha()
{
	stackedWidget_main->setCurrentIndex(STACKED_H_ALPHA);
	setToolBox_btn_color_default();
	btn_toolbox_HAlpha->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_Mirnov()
{
	stackedWidget_main->setCurrentIndex(STACKED_MD_MIRNOV);
	setToolBox_btn_color_default();
	btn_toolbox_Mirnov->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_Reflecto()
{
	stackedWidget_main->setCurrentIndex(STACKED_ELECTRON_REFLECTO);
	setToolBox_btn_color_default();
	btn_toolbox_Reflecto->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_Probe()
{
	stackedWidget_main->setCurrentIndex(STACKED_ELECTRON_PROBE);
	setToolBox_btn_color_default();
	btn_toolbox_Probe->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_Thomson()
{
	stackedWidget_main->setCurrentIndex(STACKED_ELECTRON_THOMSON);
	setToolBox_btn_color_default();
	btn_toolbox_Thomson->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_XIC()
{
	stackedWidget_main->setCurrentIndex(STACKED_ION_XIC_SPECTRO);
	setToolBox_btn_color_default();
	btn_toolbox_XIC->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_CE()
{
	stackedWidget_main->setCurrentIndex(STACKED_ION_CE_SPECTRO);
	setToolBox_btn_color_default();
	btn_toolbox_CE->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_BOLO()
{
	stackedWidget_main->setCurrentIndex(STACKED_IMAGE_BOLOMETER);
	setToolBox_btn_color_default();
	btn_toolbox_BOLO->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_SXR()
{
	stackedWidget_main->setCurrentIndex(STACKED_IMAGE_SOFT_XR);
	setToolBox_btn_color_default();
	btn_toolbox_SXR->setPalette(pal_select);	
}
void MainWindow::clicked_btn_toolbox_IRVB()
{
	stackedWidget_main->setCurrentIndex(STACKED_IMAGE_IRVB);
	setToolBox_btn_color_default();
	btn_toolbox_IRVB->setPalette(pal_select);	
}

void MainWindow::clicked_btn_toolbox_MMWI()
{
	stackedWidget_main->setCurrentIndex(STACKED_ELECTRON_MMWI);
	setToolBox_btn_color_default();
	btn_toolbox_MMWI->setPalette(pal_select);	
}
void MainWindow::clicked_btn_toolbox_VS()
{
	stackedWidget_main->setCurrentIndex(STACKED_SPECTRO_VS_SPECT);
	setToolBox_btn_color_default();
	btn_toolbox_VS->setPalette(pal_select);	
}
void MainWindow::clicked_btn_toolbox_FILTER()
{
	stackedWidget_main->setCurrentIndex(STACKED_SPECTRO_FILTER);
	setToolBox_btn_color_default();
	btn_toolbox_FILTER->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_HXR()
{
	stackedWidget_main->setCurrentIndex(STACKED_SPECTRO_HARD_XR);
	setToolBox_btn_color_default();
	btn_toolbox_HXR->setPalette(pal_select);	
}

void MainWindow::clicked_btn_toolbox_ECEHR()
{
	stackedWidget_main->setCurrentIndex(STACKED_ECE_HR);
	setToolBox_btn_color_default();
	btn_toolbox_ECEHR->setPalette(pal_select);
}

void MainWindow::clicked_btn_toolbox_SHUT()
{
	stackedWidget_main->setCurrentIndex(STACKED_SHUTTER_SHUTTER);
	setToolBox_btn_color_default();
	btn_toolbox_SHUT->setPalette(pal_select);
}

void MainWindow::clicked_btn_toolbox_POWER()
{
	stackedWidget_main->setCurrentIndex(STACKED_POWER);
	setToolBox_btn_color_default();
	btn_toolbox_POWER->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_VBS()
{
	stackedWidget_main->setCurrentIndex(STACKED_VBS);
	setToolBox_btn_color_default();
	btn_toolbox_VBS->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_IRTV_S1()
{
	stackedWidget_main->setCurrentIndex(STACKED_IRTV_S1);
	setToolBox_btn_color_default();
	btn_toolbox_IRTV_S1->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_IRTV_D1()
{
	stackedWidget_main->setCurrentIndex(STACKED_IRTV_D1);
	setToolBox_btn_color_default();
	btn_toolbox_IRTV_D1->setPalette(pal_select);
}

void MainWindow::clicked_btn_toolbox_Mirnov2()
{
	stackedWidget_main->setCurrentIndex(STACKED_MD_MIRNOV2);
	setToolBox_btn_color_default();
	btn_toolbox_Mirnov2->setPalette(pal_select);
}
void MainWindow::clicked_btn_toolbox_FILD()
{
	Password pword("kstarfild", new QWidget());
	if (pword.getStatus() != true) {
		return;
	}

	stackedWidget_main->setCurrentIndex(STACKED_FILD);
	setToolBox_btn_color_default();
	btn_toolbox_FILD->setPalette(pal_select);
}




/*
########################################################

DDS #1 action for SIGNAL input

########################################################
*/
#if 0
void MainWindow::clicked_btn_dds1OPremote()
{
	pg_dds1.clicked_btn_dds1OPremote();
}

void MainWindow::clicked_btn_dds1OPcalibration()
{
	pg_dds1.clicked_btn_dds1OPcalibration();
}

void MainWindow::clicked_btn_dds1OPLocalTest()
{
	pg_dds1.clicked_btn_dds1OPLocalTest();
}

void MainWindow::clicked_btn_dds1ResetMode()
{
	pg_dds1.clicked_btn_dds1ResetMode();

}

void MainWindow::clicked_btn_dds1SetAllchs()
{
	pg_dds1.clicked_btn_dds1SetAllchs();
}
/*
void MainWindow::clicked_btn_dds1SetAllchs_2()
{
	pg_dds1.clicked_btn_dds1SetAllchs();
}
void MainWindow::clicked_btn_dds1SetAllchs_3()
{
	pg_dds1.clicked_btn_dds1SetAllchs();
}
void MainWindow::clicked_btn_dds1SetAllchs_4()
{
	pg_dds1.clicked_btn_dds1SetAllchs();
}
*/

void MainWindow::clicked_btn_dds1DigitizerSetup()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B1);
}
void MainWindow::clicked_btn_dds1DigitizerSetup_2()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B2);
}
void MainWindow::clicked_btn_dds1DigitizerSetup_3()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B3);
}
void MainWindow::clicked_btn_dds1DigitizerSetup_4()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B4);
}
void MainWindow::clicked_btn_dds1DigitizerSetup_5()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B5);
}
void MainWindow::clicked_btn_dds1DigitizerSetup_6()
{
	pg_dds1.clicked_btn_dds1DigitizerSetup(B6);
}




void MainWindow::clicked_btn_dds1RemoteReady()
{
	pg_dds1.clicked_btn_dds1START_remote();
}

void MainWindow::clicked_btn_dds1CalibRun()
{
	pg_dds1.clicked_btn_dds1START_local();
}
/*
void MainWindow::clicked_btn_dds1RemoteStore()
{
	pg_dds1.clicked_btn_dds1RemoteStore();
}
*/

void MainWindow::clicked_btn_dds1ShotNum()
{
	pg_dds1.clicked_btn_dds1ShotNum();
}

void MainWindow::clicked_btn_dds1LocalRun()
{
//	pg_dds1.clicked_btn_dds1LocalRun();
	pg_dds1.clicked_btn_dds1START_local();
}
/*
void MainWindow::clicked_btn_dds1RemoteSave()
{
	pg_dds1.clicked_btn_dds1RemoteStore();
}

void MainWindow::clicked_btn_dds1LocalSave()
{
	pg_dds1.clicked_btn_dds1LocalStore();
}
*/
void MainWindow::clicked_btn_dds1B1LocalDAQ()
{
	pg_dds1.clicked_btn_singleRunDAQ(B1);
}
void MainWindow::clicked_btn_dds1B1LocalSave()
{
	pg_dds1.clicked_btn_singleRunSave(B1);
}
void MainWindow::clicked_btn_dds1B2LocalDAQ()
{
	pg_dds1.clicked_btn_singleRunDAQ(B2);
}
void MainWindow::clicked_btn_dds1B2LocalSave()
{
	pg_dds1.clicked_btn_singleRunSave(B2);
}
void MainWindow::clicked_btn_dds1B3LocalDAQ()
{
	pg_dds1.clicked_btn_singleRunDAQ(B3);
}
void MainWindow::clicked_btn_dds1B3LocalSave()
{
	pg_dds1.clicked_btn_singleRunSave(B3);
}
void MainWindow::clicked_btn_dds1B4LocalDAQ()
{
	pg_dds1.clicked_btn_singleRunDAQ(B4);
}
void MainWindow::clicked_btn_dds1B4LocalSave()
{
	pg_dds1.clicked_btn_singleRunSave(B4);
}
void MainWindow::clicked_btn_dds1UpdateStatus()
{
#if 0
	ST_QUEUE_STATUS queueNode;
	char strval[32];
	int iCAStatus;
	for( int i=0; i<USED_ACQ196; i++) {	
		queueNode.id = i;
//		queueNode.status = ca_Get_QStr( pg_dds1.DDS1_getState[i] ).toInt();
		if( (iCAStatus = ca_Get_StrVal(pg_dds1.DDS1_getState[i], strval) ) != ECA_NORMAL ) {
			printStatus("ERROR! >> %s\n", ca_message( iCAStatus) );
			return;
		}
		queueNode.status = atoi(strval);
//		printStatus("getState[%d] = %s: 0x%X\n", i, strval, queueNode.status);
		setDDS1StateFromPV(queueNode);


		if( (iCAStatus = ca_Get_StrVal(pg_dds1.DDS1_BX_MDS_SND_CNT[i], strval) ) != ECA_NORMAL ) {
			printStatus("ERROR! >> %s\n", ca_message( iCAStatus) );
			return;
		}
		switch(i){
			case B1: label_dev1_cnt->setText(strval ); break;
			case B2: label_dev2_cnt->setText(strval ); break;
			case B3: label_dev3_cnt->setText(strval ); break;
			case B4: label_dev4_cnt->setText(strval ); break;
			case B5: label_dev5_cnt->setText(strval ); break;
			case B6: label_dev6_cnt->setText(strval ); break;
			case B7: label_dev7_cnt->setText(strval ); break;
			case B8: label_dev8_cnt->setText(strval ); break;
			default: break;
		}

		usleep(100000);
	}
#endif
}
void MainWindow::clicked_btn_dds1CreateShot()
{
	pg_dds1.clicked_btn_dds1CreateShot();
}
void MainWindow::clicked_btn_dds1_Retrieve()
{
	pg_dds1.clicked_btn_dds1_Retrieve();
}
void MainWindow::idchanged_cb_dds1SampleClk(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk(qstr);
}
void MainWindow::idchanged_cb_dds1SampleClk_2(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk_2(qstr);
}
void MainWindow::idchanged_cb_dds1SampleClk_3(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk_3(qstr);
}
void MainWindow::idchanged_cb_dds1SampleClk_4(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk_4(qstr);
}
void MainWindow::idchanged_cb_dds1SampleClk_5(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk_5(qstr);
}
void MainWindow::idchanged_cb_dds1SampleClk_6(QString qstr)
{
	pg_dds1.idchanged_cb_dds1SampleClk_6(qstr);
}
void MainWindow::clicked_btn_dds1_assign_all()	
{
	pg_dds1.clicked_btn_dds1_assign_all();
}
void MainWindow::clicked_btn_dds1_Arming()
{
	pg_dds1.clicked_btn_dds1_Arming();
}
void MainWindow::clicked_btn_forced_mode_reset()
{
	pg_dds1.clicked_btn_forced_mode_reset();
}
#endif
/*
########################################################

DDS #2 action for SIGNAL input

########################################################
*/
#if 0
void MainWindow::clicked_btn_dds2OPremote()
{
	g_mInternalExternal = SYS_CLOCK_EXTERNAL;
	pg_dds2.clicked_btn_dds2OPremote();

//	g_opStateDDS2 = ca_Get_QStr( pg_dds2.chid_DDS2_getState).toInt();
//	setDDS2StateFromPV();
}

void MainWindow::clicked_btn_dds2OPcalibration()
{
	g_mInternalExternal = SYS_CLOCK_INTERNAL;
	pg_dds2.clicked_btn_dds2OPcalibration();

//	g_opStateDDS2 = ca_Get_QStr( pg_dds2.chid_DDS2_getState).toInt();
//	setDDS2StateFromPV();
}

void MainWindow::clicked_btn_dds2OPtest()
{
	g_mInternalExternal = SYS_CLOCK_INTERNAL;
	pg_dds2.clicked_btn_dds2OPtest();

//	g_opStateDDS2 = ca_Get_QStr( pg_dds2.chid_DDS2_getState).toInt();
//	setDDS2StateFromPV();
}

void MainWindow::clicked_btn_dds2ResetMode()
{
	g_mInternalExternal = SYS_CLOCK_INTERNAL;
	pg_dds2.clicked_btn_dds2ResetMode();
	g_mInternalExternal = SYS_CLOCK_INTERNAL;
}

void MainWindow::clicked_btn_dds2SetAll()
{
	pg_dds2.clicked_btn_dds2SetAll();
}

void MainWindow::clicked_btn_dds2DigitizerSetup()
{
	pg_dds2.clicked_btn_dds2DigitizerSetup();
}
/*
void MainWindow::clicked_btn_dds2RemoteReady()
{
	pg_dds2.clicked_btn_dds2RemoteReady();
}
*/
void MainWindow::clicked_btn_dds2CalibStartStop()
{
	pg_dds2.clicked_btn_dds2CalibStartStop();
}

void MainWindow::clicked_btn_dds2RemoteStore()
{
	pg_dds2.clicked_btn_dds2RemoteStore();
}


void MainWindow::clicked_btn_dds2ShotNum()
{
	pg_dds2.clicked_btn_dds2ShotNum();
}

void MainWindow::clicked_btn_dds2TestRun()
{
//	pg_dds2.clicked_btn_dds2TestRun();
	pg_dds2.clicked_btn_dds2CalibStartStop();
}

void MainWindow::clicked_btn_dds2DAQabort()
{
	pg_dds2.clicked_btn_dds2DAQabort();
}
#endif

/*
void MainWindow::clicked_btn_ER_OP(int nval)
{
	printf("ER opmode: %d\n", nval);

}
*/
