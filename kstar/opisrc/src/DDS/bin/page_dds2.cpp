#include <QtGui>

#include "page_dds2.h"

#include "mainWindow.h"

//#include "systemState.h"
#include "nodeDelegate.h"

#include "dds2PVs.h"

void dds2ConnectionCallback(struct connection_handler_args args);
callbackData cdds2Data;


static MainWindow *pWnd;


pageDDS2 *pparent;

pageDDS2::pageDDS2()
{
//	pm = NULL;
	for(int i=0; i<MAX_M6802; i++){
		touch_T1_PV[i] = 0;
	}

	cdds2Data.pFunc = dds2ConnectionCallback;
	
}

pageDDS2::~pageDDS2()
{
#if PV_CONNECT
	disconnectPVs();
#endif
}
void pageDDS2::timer_go()
{
	timerOneSec = new QTimer(this);
	connect(timerOneSec, SIGNAL(timeout()), this, SLOT(timerFuncOneSec()));
	timerOneSec->start(1000);
	pWnd->printStatus( "DDS2 Timer: started" );

}
void pageDDS2::timer_stop()
{
	if( timerOneSec ){
		timerOneSec->stop();
		timerOneSec = NULL;
		pWnd->printStatus( "DDS2 Timer: stop" );
	}
}

void pageDDS2::InitWindow(void *parent)
{
//	pm = (MainWindow*)parent;

	pWnd = (MainWindow*)parent;

	cbDDS2.InitWindow( (MainWindow*)parent );

//	g_nUseCntM6802 = 0;

//	InitLocalValue();
	
	pparent = this;
	
#if PV_CONNECT
	connectPVs();

//	pWnd->startMonitoringPV( chid_DDS2_getState, 0 );
#endif

	pWnd->label_dds2Opmode->setText( "Select Mode" );
//	pWnd->label_dds2DAQstate->setText("Not Ready to RUN");

	
}

/*
########################################################

Real action for SIGNAL input

########################################################
*/

void pageDDS2::clicked_btn_dds2OPremote()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MMW) ) {
			pWnd->g_opMode = SYS_MODE_INIT;
			pWnd->btn_dds2OPremote->setChecked(false);
			pWnd->btn_dds2OPcalibration->setChecked(false);
			pWnd->btn_dds2OPtest->setChecked(false);
			return;
		}
		pWnd->g_opMode = SYS_MODE_REMOTE;
		
		initTableWidgetM6802();

		monitor_LTU_start();
//		monitor_dds2PV_start();
//	}
	pWnd->gb_dds2Opmode->setEnabled(false);
	pWnd->gb_dds2digisetup->setEnabled(true);

	pWnd->btn_dds2OPremote->setChecked(true);
	pWnd->btn_dds2OPcalibration->setChecked(false);
	pWnd->btn_dds2OPtest->setChecked(false);

	pWnd->gb_dds2remote->setEnabled(true);
	pWnd->gb_dds2calibration->setEnabled(false);
	pWnd->gb_dds2test->setEnabled(false);

	pWnd->tabWidget_m6802->setEnabled(true);	
	pWnd->label_dds2Opmode->setText("REMOTE Operation" );
/*	TG.Lee Delete 2008.12.05
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_DISA, QString( "0") );
	usleep(500000);
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_INTN_DISA, QString( "1") );
*/
	timer_go();	// 2009. 09. 08 update for T1 value
}

void pageDDS2::clicked_btn_dds2OPcalibration()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MMW) ) {
			pWnd->g_opMode = SYS_MODE_INIT;
			pWnd->btn_dds2OPremote->setChecked(false);
			pWnd->btn_dds2OPcalibration->setChecked(false);
			pWnd->btn_dds2OPtest->setChecked(false);
			return;
		}
		pWnd->g_opMode = SYS_MODE_CALIB;
		
		initTableWidgetM6802();


//		monitor_dds2PV_start();
//	}
	pWnd->gb_dds2Opmode->setEnabled(false);
	pWnd->gb_dds2digisetup->setEnabled(true);

	pWnd->btn_dds2OPremote->setChecked(false);
	pWnd->btn_dds2OPcalibration->setChecked(true);
	pWnd->btn_dds2OPtest->setChecked(false);

	pWnd->gb_dds2remote->setEnabled(false);
	pWnd->gb_dds2calibration->setEnabled(true);
	pWnd->gb_dds2test->setEnabled(false);

	pWnd->tabWidget_m6802->setEnabled(true);	
	pWnd->label_dds2Opmode->setText("Calibration Mode" );
/*	TG.Lee Delete 2008.12.05
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_DISA, QString( "1") );
	usleep(500000);
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_INTN_DISA, QString( "0") );
*/
}

void pageDDS2::clicked_btn_dds2OPtest()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MMW) ) {
			pWnd->g_opMode = SYS_MODE_INIT;
			pWnd->btn_dds2OPremote->setChecked(false);
			pWnd->btn_dds2OPcalibration->setChecked(false);
			pWnd->btn_dds2OPtest->setChecked(false);
			return;
		}
		pWnd->g_opMode = SYS_MODE_LOCAL;

		initTableWidgetM6802();
		
//		monitor_dds2PV_start();
//	}

	pWnd->gb_dds2Opmode->setEnabled(false);
	pWnd->gb_dds2digisetup->setEnabled(true);

	pWnd->btn_dds2OPremote->setChecked(false);
	pWnd->btn_dds2OPcalibration->setChecked(false);
	pWnd->btn_dds2OPtest->setChecked(true);

	pWnd->gb_dds2remote->setEnabled(false);
	pWnd->gb_dds2calibration->setEnabled(false);
	pWnd->gb_dds2test->setEnabled(true);

	pWnd->tabWidget_m6802->setEnabled(true);	
	pWnd->label_dds2Opmode->setText("Test Mode" );

/*    TG.Lee Delete 2008.12.05	
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_DISA, QString( "1") );
	usleep(500000);
	ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_INTN_DISA, QString( "0") );
*/
}

void pageDDS2::clicked_btn_dds2ResetMode()
{
//	if( !pWnd->g_IsFirstEnterance ) 
//	{
//		pWnd->g_IsFirstEnterance = true;

//		pWnd->killPVmonitoringTimer();
	if( pWnd->g_opMode == SYS_MODE_REMOTE ) {	// 2009. 09. 08 update for T1 value
		timer_stop();	
	}


		pWnd->fileFunc.saveM6802ConfigFile(pWnd->tableWidget_M6802_1, 1);
		pWnd->fileFunc.saveM6802ConfigFile(pWnd->tableWidget_M6802_2, 2);
/*		pWnd->fileFunc.saveM6802ConfigFile(pWnd->tableWidget_M6802_3, 3);   */
/*		pWnd->fileFunc.saveM6802ConfigFile(pWnd->tableWidget_M6802_4, 4);   */

		pWnd->g_opMode = SYS_MODE_INIT;

		initStactedWidgetM6802();

		monitor_stop();

		for(int i=0; i< 32; i++) {
			pWnd->tableWidget_M6802_1->removeRow(0);
			pWnd->tableWidget_M6802_2->removeRow(0);
/*			pWnd->tableWidget_M6802_3->removeRow(0);   */
/*			pWnd->tableWidget_M6802_4->removeRow(0);   */
		}
		pWnd->tableWidget_M6802_1->removeColumn(0);
		pWnd->tableWidget_M6802_1->removeColumn(0);
		pWnd->tableWidget_M6802_2->removeColumn(0);
		pWnd->tableWidget_M6802_2->removeColumn(0);
/*		pWnd->tableWidget_M6802_3->removeColumn(0);
		pWnd->tableWidget_M6802_3->removeColumn(0);
*/		
/*		pWnd->tableWidget_M6802_4->removeColumn(0);
		pWnd->tableWidget_M6802_4->removeColumn(0);   
*/

		pWnd->label_dds2Opmode->setText("Select Operation Mode" );
//		pWnd->label_dds2DAQstate->setText("Not Ready to RUN");
//	}
}

void pageDDS2::clicked_btn_dds2SetAll()
{
	int nOnOff;
	int nCurrentTab;
	QTableWidget *pwidget = NULL;

	nCurrentTab = pWnd->tabWidget_m6802->currentIndex();
//	pWnd->printStatus("current tab index(%d)\n", nCurrentTab );
	if( nCurrentTab == 0 )
		pwidget = pWnd->tableWidget_M6802_1;
	else if ( nCurrentTab == 1 )
		pwidget = pWnd->tableWidget_M6802_2;
/*	else if ( nCurrentTab == 2 )
		pwidget = pWnd->tableWidget_M6802_3;
*/		
/*	else if ( nCurrentTab == 3 )
		pwidget = pWnd->tableWidget_M6802_4;		// 2009. 08. 18 add one more ADC
*/
	else {
		pWnd->printStatus("ERROR: got wrong tab index(%d)\n", nCurrentTab );
		return;
		}

	if( pwidget->item(0, 0)->checkState() == Qt::Checked) nOnOff = 1;
	else nOnOff = 0;

	for( int i=0; i< 32; i++) 
	{
		if( nOnOff )pwidget->item(i, 0)->setCheckState(Qt::Checked);
		else pwidget->item(i, 0)->setCheckState(Qt::Unchecked);
//		item = pwidget->item(i, 1);
//		pwidget->item(i, 1)->setText(strTag);
//		pwidget->item(i, 2)->setText(strVoltage);
//		pwidget->item(i, 3)->setText(strSampleRate);
//		pwidget->item(i, 4)->setText(strTrigger);
//		pwidget->item(i, 5)->setText(strPeriod);
	}
}

void pageDDS2::clicked_btn_dds2DigitizerSetup()
{
//	unsigned int nval;
	int iCAStatus;
	QString qstrVal;

	m6802Config.t0 = pWnd->Edit_M6802_trigger1->text().toDouble();
	m6802Config.t1 = pWnd->Edit_M6802_period1->text().toDouble();
	m6802Config.frame_rate = (pWnd->cb_dds2SampleClk->currentText()).toInt();

	pvPutTagNameM6802(0);
	usleep(200000);
	pvPutTagNameM6802(1);
	usleep(500000);
/*	pvPutTagNameM6802(2);
	usleep(500000);
*/	
	
	pvPutChannelBitMaskM6802( 0);
	pvPutChannelBitMaskM6802( 1);
/*	pvPutChannelBitMaskM6802( 2);  */
	if (pWnd->g_mInternalExternal == SYS_CLOCK_INTERNAL ) pvPutInternalSetM6802(0);
	else pvPutExternalSetM6802(0);
	
//	nval = ca_Get_QStr( chid_DDS2_getState).toInt();
//	iCAStatus = ca_Get_QStr(chid_DDS2_getState, qstrVal);
//	pWnd->setDDS2StateFromPV( qstrVal.toInt() );
}
/*
void pageDDS2::clicked_btn_dds2RemoteReady()
{
	ca_Put_QStr( chid_DDS2_REMOTE_READY_BTN, QString( "1") );

//	usleep(500000);

//	ca_Put_QStr( chid_DDS2_remoteReady, QString( "1") );
}
*/
//int dds2_nRunStopTimerstep;
void pageDDS2::clicked_btn_dds2CalibStartStop()
{
//	int nVal;
	if( pWnd->g_mInternalExternal == SYS_CLOCK_INTERNAL ) 
	{
/*
		g_Trigger = m6802Config.t0;
		g_Period = m6802Config.t1;

		runstopTimerCounter = 0.0;
		dds2_nRunStopTimerstep = 1;
		timerRunStop = new QTimer(this);
		connect(timerRunStop, SIGNAL(timeout()), this, SLOT(timerFuncRunStop()));
		timerRunStop->start(1000);
		pWnd->printStatus( "Timer: started" );
*/
		
/*		printf("chid_DDS2_SOFT_DAQ_START_INTN: %d\n", nVal ); */
/* Delete TG.Lee - SNL Program create Time 20081107 
		nVal = ca_Get_QStr( chid_DDS2_SOFT_DAQ_START_INTN).toInt();
		nVal = nVal ? 0 : 1;
		ca_Put_QStr( chid_DDS2_SOFT_DAQ_START_INTN, QString( "%1").arg(nVal) );

		ca_Put_QStr( chid_DDS2_ADCstart, QString( "1") );
	} else {
		ca_Put_QStr( chid_DDS2_ADCstart, QString( "1") );
		usleep(1000000);
		ca_Put_QStr( chid_DDS2_DAQstart, QString( "1") );
	}
*/
/*  Button (BTN) PV is Done the DAQ Process Sequence Routine at SNL Program       */
		ca_Put_QStr( chid_DDS2_LOCAL_READY_BTN, QString("1"));
		
	} else {
		ca_Put_QStr( chid_DDS2_REMOTE_READY_BTN, QString( "1") );
	}
}

void pageDDS2::clicked_btn_dds2RemoteStore()
{
/* Delete TG.Lee - SNL Program create Time 20081107 
	ca_Put_QStr( chid_DDS2_RemoteSave, QString("1") );  */
	ca_Put_QStr( chid_DDS2_REMOTE_STORE_BTN, QString("1"));	
	pWnd->printStatus( "Remote Model tree send finished! ");
}

void pageDDS2::clicked_btn_dds2ShotNum()
{
	ca_Put_QStr( chid_DDS2_shotNumber, QString( "%1").arg( pWnd->Edit_dds2ShotNum->text().toAscii().constData() ) );
}

void pageDDS2::clicked_btn_dds2DAQabort()
{
//	if( timerRunStop ) timerRunStop->stop();
//	pWnd->printStatus( ">>> timerRunStop: stopted" );
/*	timerRunStop = NULL;
	dds2_nRunStopTimerstep = 1;
*/
/* Delete TG.Lee - SNL Program create Time 20081107 
	ca_Put_QStr( chid_DDS2_DAQstop, QString("1") ); // stop data saveing to local SATA2 or SAS HDD
	usleep(1500000);
	ca_Put_QStr( chid_DDS2_ADCstop, QString("1") );
*/
	ca_Put_QStr( chid_DDS2_DAQ_ABORT_BTN, QString("1"));
}











/*
#####################################################################################

Local function 

#####################################################################################
*/

void pageDDS2::connectPVs()
{
	if(  ca_ConnectCB( str_DDS2_setOpMode,	chid_DDS2_setOpMode, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_setOpMode connect errer!" );
//	if(  ca_ConnectCB( str_DDS2_DAQ_STATUS,	chid_DDS2_DAQ_STATUS, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_DAQ_STATUS connect errer!" );
	if(  ca_ConnectCB( str_DDS2_ADCstart,	chid_DDS2_ADCstart, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_ADCstart connect errer!" );
	if(  ca_ConnectCB( str_DDS2_DAQstart,	chid_DDS2_DAQstart, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_DAQstart connect errer!" );
	if(  ca_ConnectCB( str_DDS2_DAQstop,		chid_DDS2_DAQstop, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_DAQstop connect errer!" );
	if(  ca_ConnectCB( str_DDS2_ADCstop,		chid_DDS2_ADCstop, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_ADCstop connect errer!" );
	if(  ca_ConnectCB( str_DDS2_LocalSave,	chid_DDS2_LocalSave, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_LocalSave connect errer!" );
	if(  ca_ConnectCB( str_DDS2_RemoteSave,	chid_DDS2_RemoteSave, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_RemoteSave connect errer!" );

	if(  ca_ConnectCB( str_DDS2_master_chBitMask, chid_DDS2_master_chBitMask, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_master_chBitMask connect errer!" );
	if(  ca_ConnectCB( str_DDS2_Dev2_chBitMask, chid_DDS2_Dev2_chBitMask, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_Dev2_chBitMask connect errer!" );
/*	if(  ca_ConnectCB( str_DDS2_Dev3_chBitMask, chid_DDS2_Dev3_chBitMask, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_Dev3_chBitMask connect errer!" );  */
/*	if(  ca_ConnectCB( str_DDS2_Dev4_chBitMask, chid_DDS2_Dev4_chBitMask, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_Dev4_chBitMask connect errer!" );  */

	if(  ca_ConnectCB( str_DDS2_setInternal, chid_DDS2_setClkInternal, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_setInternal connect errer!" );
	if(  ca_ConnectCB( str_DDS2_setExternal, chid_DDS2_setClkExternal, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_setExternal connect errer!" );
	if(  ca_ConnectCB( str_DDS2_sampleRate, chid_DDS2_sampleRate, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_sampleRate connect errer!" );
	if(  ca_ConnectCB( str_DDS2_shotNumber, chid_DDS2_shotNumber, cdds2Data) != ECA_NORMAL ) pWnd->printStatus( "DDS2_shotNumber connect errer!" );
	ca_ConnectCB( str_DDS2_setT0, chid_DDS2_setT0, cdds2Data);
	ca_ConnectCB( str_DDS2_setT1, chid_DDS2_setT1, cdds2Data);
/*	TG.Lee Delete 2008.12.05 
	ca_ConnectCB( str_DDS2_SOFT_DAQ_START_INTN, chid_DDS2_SOFT_DAQ_START_INTN, cdds2Data);
	ca_ConnectCB( str_DDS2_SOFT_DAQ_START_DISA, chid_DDS2_SOFT_DAQ_START_DISA, cdds2Data);
	ca_ConnectCB( str_DDS2_SOFT_DAQ_START_INTN_DISA, chid_DDS2_SOFT_DAQ_START_INTN_DISA, cdds2Data);
*/
/* Add PV 4 btn - SNL Program Create Time 20081107. Edit by TG.Lee   */
	ca_ConnectCB( str_DDS2_REMOTE_READY_BTN, chid_DDS2_REMOTE_READY_BTN, cdds2Data);
	ca_ConnectCB( str_DDS2_LOCAL_READY_BTN, chid_DDS2_LOCAL_READY_BTN, cdds2Data);
	ca_ConnectCB( str_DDS2_REMOTE_STORE_BTN, chid_DDS2_REMOTE_STORE_BTN, cdds2Data);
	ca_ConnectCB( str_DDS2_DAQ_ABORT_BTN, chid_DDS2_DAQ_ABORT_BTN, cdds2Data);

//	ca_ConnectCB( str_DDS2_getState, chid_DDS2_getState, cdds2Data);
	
	ca_ConnectCB( str_DDS2_setTrigInternal, chid_DDS2_setTrigInternal, cdds2Data);
	ca_ConnectCB( str_DDS2_setTrigExternal, chid_DDS2_setTrigExternal, cdds2Data);

	connectTagPVs();


	{
		ca_ConnectCB( str_DDS2_LTU_ShotTerm,	pvCLTU.SHOT_TERM, cdds2Data);
	
		ca_ConnectCB( str_DDS2_LTU_Trig_p0,	pvCLTU.TRIG[0], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Trig_p1,	pvCLTU.TRIG[1], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Trig_p2,	pvCLTU.TRIG[2], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Trig_p3,	pvCLTU.TRIG[3], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_Clock_p0,		pvCLTU.CLOCK[0], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Clock_p1,		pvCLTU.CLOCK[1], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Clock_p2,		pvCLTU.CLOCK[2], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_Clock_p3,		pvCLTU.CLOCK[3], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_T0_p0,			pvCLTU.T0[0], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T0_p1,			pvCLTU.T0[1], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T0_p2,			pvCLTU.T0[2], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T0_p3,			pvCLTU.T0[3], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_T1_p0,			pvCLTU.T1[0], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T1_p1,			pvCLTU.T1[1], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T1_p2,			pvCLTU.T1[2], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_T1_p3,			pvCLTU.T1[3], cdds2Data);
	
		ca_ConnectCB( str_DDS2_LTU_setup_p0,	pvCLTU.SETUP_Pt[0], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_setup_p1,	pvCLTU.SETUP_Pt[1], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_setup_p2,	pvCLTU.SETUP_Pt[2], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_setup_p3,	pvCLTU.SETUP_Pt[3], cdds2Data);
//		ca_ConnectCB( str_DDS2_LTU_setup_p4,	pvCLTU.SETUP_Pt[4], cdds2Data);
/*
		ca_ConnectCB( str_DDS2_LTU_mTrig,		pvCLTU.mTRIG, cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mClock_sec0, pvCLTU.mCLOCK[0], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mClock_sec1, pvCLTU.mCLOCK[1], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mClock_sec2, pvCLTU.mCLOCK[2], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT0_sec0,	pvCLTU.mT0[0], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT0_sec1,	pvCLTU.mT0[1], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT0_sec2,	pvCLTU.mT0[2], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT1_sec0,	pvCLTU.mT1[0], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT1_sec1,	pvCLTU.mT1[1], cdds2Data);
		ca_ConnectCB( str_DDS2_LTU_mT1_sec2,	pvCLTU.mT1[2], cdds2Data);
*/
	}


}

void pageDDS2::disconnectPVs()
{
	ca_Disconnect( chid_DDS2_setOpMode );
//	ca_Disconnect( chid_DDS2_DAQ_STATUS );
	ca_Disconnect( chid_DDS2_ADCstart );
	ca_Disconnect( chid_DDS2_DAQstart );
	ca_Disconnect( chid_DDS2_DAQstop );
	ca_Disconnect( chid_DDS2_ADCstop );
	ca_Disconnect( chid_DDS2_LocalSave );
	ca_Disconnect( chid_DDS2_RemoteSave );
	ca_Disconnect( chid_DDS2_master_chBitMask );
	ca_Disconnect( chid_DDS2_Dev2_chBitMask );
/*	ca_Disconnect( chid_DDS2_Dev3_chBitMask );  2010.10.12   */
/*	ca_Disconnect( chid_DDS2_Dev4_chBitMask );	 2009. 08. 18 add one more ADC  */
	ca_Disconnect( chid_DDS2_setClkInternal );
	ca_Disconnect( chid_DDS2_setClkExternal );
	ca_Disconnect( chid_DDS2_sampleRate );
	ca_Disconnect( chid_DDS2_shotNumber );

	ca_Disconnect( chid_DDS2_setT0);
	ca_Disconnect( chid_DDS2_setT1);
/*	TG.Lee Delete 2008.12.05\
	ca_Disconnect( chid_DDS2_SOFT_DAQ_START_INTN );
	ca_Disconnect( chid_DDS2_SOFT_DAQ_START_DISA);
	ca_Disconnect( chid_DDS2_SOFT_DAQ_START_INTN_DISA);
*/
/* Add PV 4 btn - SNL Program Create Time 20081107. Edit by TG.Lee   */
	ca_Disconnect( chid_DDS2_REMOTE_READY_BTN);
	ca_Disconnect( chid_DDS2_LOCAL_READY_BTN);
	ca_Disconnect( chid_DDS2_REMOTE_STORE_BTN);
	ca_Disconnect( chid_DDS2_DAQ_ABORT_BTN);

//	ca_Disconnect( chid_DDS2_getState);

	ca_Disconnect( chid_DDS2_setTrigInternal );
	ca_Disconnect( chid_DDS2_setTrigExternal );
	
	disconnectTagPVs();
	
	
	
	ca_Disconnect( pvCLTU.SHOT_TERM );
//	for(int j=0; j<4; j++) {
	for(int j=0; j<1; j++) {		
		ca_Disconnect( pvCLTU.TRIG[j] );
		ca_Disconnect( pvCLTU.CLOCK[j] );
		ca_Disconnect( pvCLTU.T0[j] );
		ca_Disconnect( pvCLTU.T1[j] );
		ca_Disconnect( pvCLTU.SETUP_Pt[j] );
	}		
//	for(int j=0; j<3; j++) {
//		ca_Disconnect( pvCLTU.mCLOCK[j] );
//		ca_Disconnect( pvCLTU.mT0[j] );
//		ca_Disconnect( pvCLTU.mT1[j] );
//	}
//	ca_Disconnect( pvCLTU.SETUP_Pt[4] );
//	ca_Disconnect( pvCLTU.mTRIG );

}


void pageDDS2::initTableWidgetM6802()
{
//	int width;
/*	QStringList labels;
	labels << tr("On") << tr("TAG") ;

	pWnd->tableWidget_M6802_1->setColumnCount(2);
	pWnd->tableWidget_M6802_1->setHorizontalHeaderLabels(labels);
	pWnd->tableWidget_M6802_1->setItemDelegate(new NodeDelegate(this));
	pWnd->tableWidget_M6802_1->horizontalHeader()->resizeSection(0, 30);
	pWnd->tableWidget_M6802_1->horizontalHeader()->resizeSection(1, 200);

	pWnd->tableWidget_M6802_2->setColumnCount(2);
	pWnd->tableWidget_M6802_2->setHorizontalHeaderLabels(labels);
	pWnd->tableWidget_M6802_2->setItemDelegate(new NodeDelegate(this));
	pWnd->tableWidget_M6802_2->horizontalHeader()->resizeSection(0, 30);
	pWnd->tableWidget_M6802_2->horizontalHeader()->resizeSection(1, 200);

	pWnd->tableWidget_M6802_3->setColumnCount(2);
	pWnd->tableWidget_M6802_3->setHorizontalHeaderLabels(labels);
	pWnd->tableWidget_M6802_3->setItemDelegate(new NodeDelegate(this));
	pWnd->tableWidget_M6802_3->horizontalHeader()->resizeSection(0, 30);
	pWnd->tableWidget_M6802_3->horizontalHeader()->resizeSection(1, 200);

	pWnd->tableWidget_M6802_4->setColumnCount(2);
	pWnd->tableWidget_M6802_4->setHorizontalHeaderLabels(labels);
	pWnd->tableWidget_M6802_4->setItemDelegate(new NodeDelegate(this));
	pWnd->tableWidget_M6802_4->horizontalHeader()->resizeSection(0, 30);
	pWnd->tableWidget_M6802_4->horizontalHeader()->resizeSection(1, 200);
*/
	setupM6802Table(pWnd->tableWidget_M6802_1, 1);
	setupM6802Table(pWnd->tableWidget_M6802_2, 2);
/*	setupM6802Table(pWnd->tableWidget_M6802_3, 3);   */
/*	setupM6802Table(pWnd->tableWidget_M6802_4, 4);   */

	pWnd->tabWidget_m6802->setCurrentIndex(0);
	
#if PV_CONNECT
	pvPut_opCondition();
#endif
}

void pageDDS2::setupM6802Table(QTableWidget *pwidget, int num)
{
	int row;
	int nCh, nOnOff;
	char fileName[128];
	char bufLineScan[512];
	char strTag[32];
//	char strVoltage[16];
//	float fVoltage;
	int nSampleRate;
	char strTrigger[16];
	char strPeriod[16];
	FILE *fp;
	char buf[64];
	char strMode[16];

	if( pWnd->g_opMode == SYS_MODE_INIT ) return;


	QStringList labels;
	labels << tr("On") << tr("TAG") ;


	pwidget->setColumnCount(2);
	pwidget->setHorizontalHeaderLabels(labels);
	pwidget->setItemDelegate(new NodeDelegate(this));

	pwidget->horizontalHeader()->resizeSection(0, 30);
	pwidget->horizontalHeader()->resizeSection(1, 200);

	

	if ( num == 1 )
		strcpy(fileName, "/home/kstar/dds_config/.M6802-1.cfg");
	else if( num == 2)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-2.cfg");
	else if( num == 3)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-3.cfg");
/*	else if( num == 4)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-4.cfg"); 
*/
	else return;


	if( (fp = fopen(fileName, "r") ) == NULL )
	{	
//		fprintf(stderr, "Can't Read dds2 channel-tag mapping file\n");
		QMessageBox::information(0, "DDS setup", tr("Can't read M6802 config file") );
		return;
	}

	if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
	sscanf(bufLineScan, "%s %s\n", buf, strTrigger);
	pWnd->Edit_M6802_trigger1->setText(strTrigger);	

	if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
	sscanf(bufLineScan, "%s %s\n", buf, strPeriod);	
	pWnd->Edit_M6802_period1->setText( strPeriod );

	if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
	sscanf(bufLineScan, "%s %d\n", buf, &nSampleRate);
	switch(nSampleRate) {
		case 1: pWnd->cb_dds2SampleClk->setCurrentIndex(0); break;
		case 10: pWnd->cb_dds2SampleClk->setCurrentIndex(1); break;
		case 20: pWnd->cb_dds2SampleClk->setCurrentIndex(2); break;
		case 50: pWnd->cb_dds2SampleClk->setCurrentIndex(3); break;
		case 100: pWnd->cb_dds2SampleClk->setCurrentIndex(4); break;
		default: {
			fclose(fp);
			return;
			}
	}
//	Edit_M6802_sampleclk1->setText( strSampleRate );

	if( pWnd->g_opMode == SYS_MODE_REMOTE )
		strcpy(strMode, "REMOTE");
	else if( pWnd->g_opMode == SYS_MODE_CALIB )
		strcpy(strMode, "CALIB");
	else if( pWnd->g_opMode == SYS_MODE_LOCAL )
		strcpy(strMode, "TEST");
	else {
		fclose(fp);
		return;
	}

	if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
	sscanf(bufLineScan, "%s %s\n", strPeriod, buf);
	while(1){
		if( !strcmp(strMode, buf) )
			break;
		if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
		sscanf(bufLineScan, "%s %s\n", strPeriod, buf);
	}

	for( int i=0; i< 32; i++) {

		if( fgets(bufLineScan, 512, fp) == NULL ) {fclose(fp); return;}
		sscanf(bufLineScan, "%d %d %s", &nCh, &nOnOff, strTag);

		row = pwidget->rowCount();
		pwidget->setRowCount(row + 1);
//		pwidget->setRowCount(i+1);

		QString SensorName = tr("");
		QTableWidgetItem *item0 = new QTableWidgetItem(SensorName);
	//	item0->setData(Qt::UserRole, tr("fileName"));
		item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
		if( nOnOff )item0->setCheckState(Qt::Checked);
		else item0->setCheckState(Qt::Unchecked);

		QTableWidgetItem *item1 = new QTableWidgetItem(strTag);

		item1->setTextAlignment(Qt::AlignLeft);

		pwidget->setItem(row, 0, item0);
		pwidget->setItem(row, 1, item1);
//		pwidget->openPersistentEditor(item1);
	}

	fclose(fp);

//	g_nUseCntM6802++;
}

void pageDDS2::initStactedWidgetM6802()
{
	pWnd->gb_dds2Opmode->setEnabled(true);

	pWnd->btn_dds2OPremote->setChecked(false);
	pWnd->btn_dds2OPcalibration->setChecked(false);
	pWnd->btn_dds2OPtest->setChecked(false);

	pWnd->gb_dds2digisetup->setEnabled(false);
	pWnd->gb_dds2remote->setEnabled(false);
	pWnd->gb_dds2calibration->setEnabled(false);
	pWnd->gb_dds2test->setEnabled(false);
/*	tabWidget_m6802->setEnabled(false); */

}

void pageDDS2::pvPutTagNameM6802(int ID)
{
	char strTag[40];
	QTableWidget *pwidget = NULL;
	QTableWidgetItem *item = NULL;

	switch(ID) {
		case 0: pwidget = pWnd->tableWidget_M6802_1; break;
		case 1: pwidget = pWnd->tableWidget_M6802_2; break;
/*		case 2: pwidget = pWnd->tableWidget_M6802_3; break;  */
/*		case 3: pwidget = pWnd->tableWidget_M6802_4; break;   */
		case 4: break;
		default: 
			pWnd->printStatus(QString("pvPutTagNameM6802(ID=%1)... error").arg(ID).toAscii().constData() ); 
			return;
	}

	for( int i=0; i< 32; i++) {
		item = pwidget->item(i, 0);
		if( item->checkState() == Qt::Checked)
		{
			item = pwidget->item(i, 1);
			strcpy( strTag, item->text().toAscii().constData() );

			ca_Put_QStr( chid_DDS2_tag[(32*ID)+i], QString( "%1").arg( strTag ) );
//			pWnd->printStatus( QString("tag %1 ok").arg(i).toAscii().constData() );
		}
	}
}

void pageDDS2::connectTagPVs()
{
	char pvName[32];
	
	for( int i=0; i< 96; i++) {
		sprintf(pvName, "DDS2_tag_%d", i);
//		if( ca_ca_Connect( QString(pvName), chid_DDS2_tag[i]) != ECA_NORMAL ) 
		if( ca_ConnectCB( pvName, chid_DDS2_tag[i], cdds2Data) != ECA_NORMAL )
			pWnd->printStatus( QString("DDS2_tag_%1 connect error").arg(i).toAscii().constData() );
		
/*		usleep(10000); */
	}

}

void pageDDS2::disconnectTagPVs()
{
	for( int i=0; i< 96; i++) {
		ca_Disconnect(chid_DDS2_tag[i] );
	}
}


void pageDDS2::pvPutChannelBitMaskM6802(int num)
{
	QTableWidget *pwidget = NULL;
	QTableWidgetItem *item = NULL;
	
	switch(num) {
		case 0: pwidget = pWnd->tableWidget_M6802_1; break;
		case 1: pwidget = pWnd->tableWidget_M6802_2; break;
/*		case 2: pwidget = pWnd->tableWidget_M6802_3; break;  */
/*		case 3: pwidget = pWnd->tableWidget_M6802_4; break; */
		case 4: break;
		default:
			pWnd->printStatus(QString("pvPutChannelBitMaskM6802(num=%1)... error").arg(num).toAscii().constData() ); 
			return;
	}

	m6802Config.channelBitMask[num][0] = 0x0;
	for( int i=0; i< 32; i++) {
		item = pwidget->item(i, 0);
		if( item->checkState() == Qt::Checked) {
			m6802Config.channelBitMask[num][0] |=  0x1 << i ;
		}
	}

//	fprintf(stdout, "bitmask : 0x%X\n", m6802Config.channel_bitmask[0] );
	switch(num) {
	case 0: ca_Put_QStr( chid_DDS2_master_chBitMask, QString( "%1").arg( m6802Config.channelBitMask[num][0] ) ); break;
	case 1: ca_Put_QStr( chid_DDS2_Dev2_chBitMask, QString( "%1").arg( m6802Config.channelBitMask[num][0] ) ); break;
/*	case 2: ca_Put_QStr( chid_DDS2_Dev3_chBitMask, QString( "%1").arg( m6802Config.channelBitMask[num][0] ) ); break;   */
/*	case 3: ca_Put_QStr( chid_DDS2_Dev4_chBitMask, QString( "%1").arg( m6802Config.channelBitMask[num][0] ) ); break;   */
	default:  break;
	}

}

void pageDDS2::pvPutInternalSetM6802(int ID)
{
	if( ID == 0 ){
//	fprintf(stdout, "shot number : %s\n", msg.toAscii().constData()  );
		ca_Put_QStr( chid_DDS2_sampleRate, QString( "%1").arg( (pWnd->cb_dds2SampleClk->currentText()).toAscii().constData() ) );

		ca_Put_QStr( chid_DDS2_setT0, QString( "%1").arg( m6802Config.t0 ) );
		ca_Put_QStr( chid_DDS2_setT1, QString( "%1").arg( m6802Config.t0 + m6802Config.t1 ) );
		usleep(500000);		
		ca_Put_QStr( chid_DDS2_setTrigInternal, QString( "1") );
		ca_Put_QStr( chid_DDS2_setClkInternal, QString( "1") );
		
	}
}


void pageDDS2::pvPutExternalSetM6802(int ID)
{

	int port = 0;
//	printf("put external infor DDS2: port %d\n", port );
	if( ID == 0) {
/*		ca_Put_QStr( chid_DDS2_setExternal, QString( "1") );  */ // not use external clock,, very important......
		ca_Put_QStr( chid_DDS2_setT0, QString( "%1").arg( m6802Config.t0 ) );
		ca_Put_QStr( chid_DDS2_setT1, QString( "%1").arg( m6802Config.t0 + m6802Config.t1 ) );
		ca_Put_QStr( chid_DDS2_sampleRate, QString("%1").arg( m6802Config.frame_rate ) );
		usleep(500000);
		ca_Put_QStr( chid_DDS2_setTrigExternal, QString( "1") );
		ca_Put_QStr( chid_DDS2_setClkInternal, QString( "1") );
		usleep(500000);

		// set PV to DDS2_LTU system.
		ca_Put_QStr( pvCLTU.T0[port], QString( "%1").arg( m6802Config.t0 ) );
		ca_Put_QStr( pvCLTU.T1[port], QString( "%1").arg( m6802Config.t0 + m6802Config.t1  + DDS2_DAQ_TIME_TAIL) ); 
		usleep(500000);
//		ca_Put_QStr( pvCLTU.CLOCK[port], QString( "%1").arg( m6802Config.frame_rate * 1000 * 64 ) );
//		ca_Put_QStr( pvCLTU.CLOCK[port], QString( "6400000") );
		ca_Put_QStr( pvCLTU.CLOCK[port], QString( "1000") );
		ca_Put_QStr( pvCLTU.TRIG[port], QString( "0") ); // always active High...
		usleep(500000);
		ca_Put_QStr( pvCLTU.SETUP_Pt[port], QString("1") );

	}
}

void pageDDS2::pvPut_opCondition()
{
	int dds2_opmode = -1;
	if( pWnd->g_opMode == SYS_MODE_REMOTE )
		dds2_opmode = 1;
	else if( pWnd->g_opMode == SYS_MODE_CALIB)
		dds2_opmode = 2;
	else if( pWnd->g_opMode == SYS_MODE_LOCAL)
		dds2_opmode = 3;
	else 
		printf("Wrong DDS2 op mode.\n" );

	int iCAStatus=ca_Put_QStr( chid_DDS2_setOpMode, QString( "%1").arg( dds2_opmode ) );
	if( iCAStatus  != ECA_NORMAL )
		pWnd->printStatus("ERROR! DDS2_setOpMode(%d), %s\n", dds2_opmode,  ca_message( iCAStatus) );
	
}
#if 0
void pageDDS2::InitLocalValue()
{
	FILE *fp;
	char buf[64], buf1[32];
	char bufLineScan[512];
	int nCh=0, nSampleClk=0, shotNumber=0;
	double nTrigger=0, nPeriod=0;

	if( (fp = fopen(".ddscfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can not open \".ddscfg\" file\n");
		m6802Config.frame_rate = 50; //KHz
		m6802Config.channel_mask = 96;
		m6802Config.t0 = 2;
		m6802Config.t1 = 3;
	} else {
		while(1) 
		{
			if( fgets(bufLineScan, 128, fp) == NULL ) break;
			if( bufLineScan[0] == '#') ;
			else {
				sscanf(bufLineScan, "%s %s", buf, buf1);
				if( strcmp(buf, "CH") == 0) {
					nCh = atoi(buf1);
/*					fprintf(stdout, "channel cnt befor use: %d\n", nCh );	*/
				} else if( strcmp(buf, "SAMPLECLK") == 0) {
					nSampleClk = atoi(buf1);
/*					fprintf(stdout, "sample clock befor use: %d\n", nSampleClk ); */
				} else if( strcmp(buf, "OPMODE") == 0) {
					if( strcmp(buf1, "LOCAL") == 0) {
						pWnd->g_opMode = SYS_MODE_LOCAL;
/*						fprintf(stdout, "op mode use: local\n"); */
					}
					else if (strcmp(buf1, "REMOTE") == 0) {
						pWnd->g_opMode = SYS_MODE_REMOTE;
/*						fprintf(stdout, "op mode use: remote\n"); */
					}
//					setCheckedModeBtn();
				} else if( strcmp(buf, "TRIGGER" ) == 0 ) {
					nTrigger = atof(buf1);
				} else if( strcmp(buf, "PERIOD" ) == 0 ) {
					nPeriod = atof(buf1);
				} else if( strcmp(buf, "SHOTNUMBER" ) == 0 ) {
					shotNumber = atoi(buf1);
				}
			}			
		} // while(1)
		fclose(fp);

		m6802Config.frame_rate = nSampleClk;
		m6802Config.channel_mask = nCh;
		m6802Config.t0 = nTrigger;
		m6802Config.t1 = nPeriod;
		for( int i=0; i< MAX_M6802; i++) {
			m6802Config.channelBitMask[i][0] = 0x0;
//			m6802Config.channelBitMask[i][ACQ196_MIDDLE] = 0x0;		// don't need this   2009.08.19
//			m6802Config.channelBitMask[i][ACQ196_LEFT] = 0x0;
//			m6802Config.channelBitMask[i][ACQ196_LAST] = 0x0;
		}

	}

}
#endif
/*
void pageDDS2::saveConfigreFile()
{
	FILE *fp;
	int nCh, nFrmRate;
	double dbTrig, dbPeid;

	if( (fp = fopen(".ddscfg", "w") ) == NULL )
	{	
		fprintf(stderr, "Can not create \".ddscfg\" file.\n");
		return;
	} else {
		if( m6802Config.frame_rate <= 0) m6802Config.frame_rate = 1;
		nCh = m6802Config.channel_mask;
		nFrmRate = m6802Config.frame_rate;
		dbTrig = m6802Config.t0;
		dbPeid = m6802Config.t1;
		fprintf(fp, "SHOTNUMBER\t%s\n", pWnd->Edit_dds2ShotNum->text().toAscii().constData());
		fprintf(fp, "CH\t%d\n", nCh);
		fprintf(fp, "SAMPLECLK\t%d\n", nFrmRate);
		fprintf(fp, "#op mode must be LOCAL or REMOTE\n");
		if( pWnd->g_opMode == SYS_MODE_LOCAL )
			fprintf(fp, "OPMODE\tLOCAL\n");
		else	fprintf(fp, "OPMODE\tREMOTE\n");
		fprintf(fp, "TRIGGER\t%f\n", dbTrig);
		fprintf(fp, "PERIOD\t%f\n", dbPeid);
	}

	fclose(fp);

}
*/
void dds2ConnectionCallback(struct connection_handler_args args)
{
	chid	chid = args.chid;
	long 	op = args.op;

	if( chid == chid_DDS2_setOpMode) {
		if( op == CA_OP_CONN_UP )
			pWnd->printStatus("Connected ... DDS2\n");
		else pWnd->printStatus("Disconnected ... DDS2\n");
	}
	
	if( chid == pparent->pvCLTU.SETUP_Pt[0] ) {
		if( op == CA_OP_CONN_UP )
			pWnd->printStatus("Connected ... DDS2_LTU\n");
		else pWnd->printStatus("Disconnected ... DDS2_LTU\n");
	}

	if( chid == chid_DDS2_tag[0] )
		if( op == CA_OP_CONN_UP ) {
			pWnd->printStatus("Connected ... M6802 #1\n");
		}
	if( chid == chid_DDS2_tag[32] )
		if( op == CA_OP_CONN_UP ) {
			pWnd->printStatus("Connected ... M6802 #2\n");
		}
	if( chid == chid_DDS2_tag[64] )
		if( op == CA_OP_CONN_UP ) {
			pWnd->printStatus("Connected ... M6802 #3\n");
		}


}

QString pageDDS2::getDDS2_T0()
{
	QString qstrVal;
	int iCAStatus = ca_Get_QStr( chid_DDS2_setT0, qstrVal);
	return qstrVal;
}

QString pageDDS2::getSampleRateDDS2()
{
	QString qstrVal;
	int iCAStatus = ca_Get_QStr( chid_DDS2_sampleRate, qstrVal);
//	return ca_Get_QStr( chid_DDS2_sampleRate );
	return qstrVal;
}

void pageDDS2::monitor_LTU_start()
{
	

	cbDDS2.startMonitoringPV_DDS2( chid_DDS2_sampleRate, 1 );
	cbDDS2.startMonitoringPV_DDS2( pvCLTU.T0[0], 1 );
	cbDDS2.startMonitoringPV_DDS2( pvCLTU.T1[0], 1 );
}

void pageDDS2::monitor_dds2PV_start()
{
//	cbDDS2.startMonitoringPV_DDS2( chid_DDS2_getState, 0 );
}

void pageDDS2::monitor_stop()
{
	cbDDS2.stopMonitoringPV();
}



void pageDDS2::timerFuncOneSec()
{
	char strval[64];
	int iCAStatus;
	double dval, dval1, dval2;
	
	if( pWnd->g_opMode != SYS_MODE_REMOTE ) return;

	if( touch_T1_PV[0] ) {

		iCAStatus=ca_Get_Double(pWnd->pg_dds2.pvCLTU.T0[0], dval);
		if( iCAStatus != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS2 T0[0]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		iCAStatus=ca_Get_Double(pWnd->pg_dds2.pvCLTU.T1[0], dval1);
		if( iCAStatus != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS2 T1[0]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		dval2 = dval1 - dval - DDS2_DAQ_TIME_TAIL ;
		cvt_Double_to_String( dval2, strval );
		pWnd->Edit_M6802_period1->setText( strval );
		
		touch_T1_PV[B1] = 0;

	}

}





