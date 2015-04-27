#include <QtGui>

#include "page_dds1.h"

#include "mainWindow.h"

//#include "systemState.h"
#include "nodeDelegate.h"


#include "dds1PVs.h"

//#include "epicsFunc.h"

void dds1ConnectionCallback(struct connection_handler_args args);
callbackData cdds1Data;

static MainWindow *pWnd;

pageDDS1::pageDDS1()
{
//	pm = NULL;
	cdds1Data.pFunc = dds1ConnectionCallback;
	for(int i=0; i<USED_ACQ196; i++){
		touch_T1_PV[i] = 0;
		is_Arm_OK[i] = 0;
//		touch_Clock_PV[i]=0;
//		touch_T0_PV[i]=0;

		acq196Config[i].nClockDivider = 1;
		acq196Config[i].t0 = 1;
		acq196Config[i].t1 = 2;
		acq196Config[i].frame_rate = 10000; // 10KHz
	}
//	dMax_T1_time = 0.0;
	nClockSource = SYS_CLOCK_INTERNAL;
//	nEnable_to_RUN = 1;
}

pageDDS1::~pageDDS1()
{
	printf("this is unnormal... please check...dds 1 termination \n");
#if PV_CONNECT
	if(pWnd->g_opMode != SYS_MODE_INIT )  {
		int iCAStatus=ca_Put_QStr( DDS1A_OpMode, QString( "0") );
		if( iCAStatus  != ECA_NORMAL )
			pWnd->printStatus("ERROR! DDS1A_OpMode(0), %s\n",  ca_message( iCAStatus) );
		iCAStatus=ca_Put_QStr( DDS1B_OpMode, QString( "0") );
		if( iCAStatus  != ECA_NORMAL )
			pWnd->printStatus("ERROR! DDS1B_OpMode(0), %s\n",  ca_message( iCAStatus) );
	}
		
	disconnectPVs();
#endif

}
void pageDDS1::kill_dds1_panel()
{
/* don't apply this.. for sudden IOC dead.  it prevent IOC turn to nothing OP mode ...    2009. 10. 22  */
#if 0
	if(pWnd->g_opMode != SYS_MODE_INIT )  {
		int iCAStatus=ca_Put_QStr( DDS1A_OpMode, QString( "0") );
		if( iCAStatus  != ECA_NORMAL )
			pWnd->printStatus("ERROR! DDS1A_OpMode(0), %s\n",  ca_message( iCAStatus) );
	}
#endif

	disconnectPVs();
}

void pageDDS1::timer_go()
{
	timerOneSec = new QTimer(this);
	connect(timerOneSec, SIGNAL(timeout()), this, SLOT(timerFuncTwoSec()));
	timerOneSec->start(8000);
	pWnd->printStatus( "DDS1 Timer: started" );

}
void pageDDS1::timer_stop()
{
	if( timerOneSec ){
		timerOneSec->stop();
		timerOneSec = NULL;
		pWnd->printStatus( "DDS1 Timer: stop" );
	}
}

void pageDDS1::InitWindow(void *parent)
{
//	pm = (MainWindow*)parent;
	pWnd = (MainWindow*)parent;

	cbFunc.InitWindow( (MainWindow*)parent );

	init_DDS1_GUI();
//	initStactedWidgetAcq196();

#if PV_CONNECT
	copy_ca_names();
	connectPVs();
		
#endif
}

/*
########################################################

Real action for SIGNAL input

########################################################
*/

void pageDDS1::clicked_btn_dds1OPremote()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MD) ) {
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}
		if( !check_Opmode_connection() ){
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}

		pWnd->g_opMode = SYS_MODE_REMOTE;
		nClockSource = SYS_CLOCK_EXTERNAL;
//		nClockSource = SYS_CLOCK_INTERNAL;

		initTableWidgetACQ196();
#if PV_CONNECT
	pvPut_opCondition();
#endif

//	monitor_LTU_start(); don't need LTU monitoring..  2010.6. 2 woong
//	monitor_dds1PV_start(); don't need anymore it was replased with KWT  2010.9.4 woong

	// 2009. 10. 13........  for notice that CCS make disable to DDS1A
//	cbFunc.startMonitoringPV( CCS_SHOTSEQ_START, 0);
	

//	dMax_T1_time = 0.0;

//	}

	set_DDS1_GUI_OPMODE();


#if 1
	pWnd->cb_dds1SampleClk->setEnabled(false);
	pWnd->Edit_ACQ196_1_T0->setEnabled(false);
	pWnd->Edit_ACQ196_1_T1->setEnabled(false);

	pWnd->cb_dds1SampleClk_2->setEnabled(false);
	pWnd->Edit_ACQ196_2_T0->setEnabled(false);
	pWnd->Edit_ACQ196_2_T1->setEnabled(false);

	pWnd->cb_dds1SampleClk_3->setEnabled(false);
	pWnd->Edit_ACQ196_3_T0->setEnabled(false);
	pWnd->Edit_ACQ196_3_T1->setEnabled(false);

	pWnd->cb_dds1SampleClk_4->setEnabled(false);
	pWnd->Edit_ACQ196_4_T0->setEnabled(false);
	pWnd->Edit_ACQ196_4_T1->setEnabled(false);

	pWnd->cb_dds1SampleClk_5->setEnabled(false);
	pWnd->Edit_ACQ196_5_T0->setEnabled(false);
	pWnd->Edit_ACQ196_5_T1->setEnabled(false);

	pWnd->cb_dds1SampleClk_6->setEnabled(false);
	pWnd->Edit_ACQ196_6_T0->setEnabled(false);
	pWnd->Edit_ACQ196_6_T1->setEnabled(false);
#endif
	

//	timer_go();	// 2009. 08. 14 update for T1 value
#if 0
	for(int i=0; i<USED_ACQ196; i++) {
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_DISA[i], QString( "0") );
		usleep(500000);
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_INTN_DISA[i], QString( "1") );
	}
#endif
}

void pageDDS1::clicked_btn_dds1OPcalibration()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MD) ) {
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}
		if( !checkCmd() ){
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}
		pWnd->g_opMode = SYS_MODE_CALIB;
		nClockSource = SYS_CLOCK_INTERNAL;
		
		initTableWidgetACQ196();
#if PV_CONNECT
			pvPut_opCondition();
#endif
		

//	monitor_dds1PV_start();
//	}

	set_DDS1_GUI_OPMODE();

#if 0	
	for(int i=0; i<USED_ACQ196; i++) {
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_DISA[i], QString( "1") );
		usleep(500000);
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_INTN_DISA[i], QString( "0") );
	}
#endif
}

void pageDDS1::clicked_btn_dds1OPLocalTest()
{
//	if( pWnd->g_IsFirstEnterance ){
		if( !pWnd->checkPasswd(TOOLBOX_ID_MD) ) {
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}
		if( !check_Opmode_connection() ){
			pWnd->btn_dds1OPremote->setChecked(false);
			pWnd->btn_dds1OPcalibration->setChecked(false);
			pWnd->btn_dds1OPlocal->setChecked(false);
			return;
		}

		pWnd->g_opMode = SYS_MODE_LOCAL;
		nClockSource = SYS_CLOCK_INTERNAL;

		initTableWidgetACQ196();
#if PV_CONNECT
			pvPut_opCondition();
#endif


//	monitor_dds1PV_start();
//	}

	set_DDS1_GUI_OPMODE();

#if 0	
	for(int i=0; i<USED_ACQ196; i++) {
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_DISA[i], QString( "1") );
		usleep(500000);
		ca_Put_QStr( DDS1A_SOFT_DAQ_START_INTN_DISA[i], QString( "0") );
	}
#endif
}

void pageDDS1::clicked_btn_dds1ResetMode()
{
//	if( !pWnd->g_IsFirstEnterance ) 
//	{
//		pWnd->g_IsFirstEnterance = true;

//		pWnd->killPVmonitoringTimer();
	if( pWnd->g_opMode == SYS_MODE_INIT) return;

	if( pWnd->g_opMode == SYS_MODE_REMOTE )  // 2009. 08. 14 update for T1 value
	{
//		timer_stop();
#if 1
		pWnd->cb_dds1SampleClk->setEnabled(true);
		pWnd->Edit_ACQ196_1_T0->setEnabled(true);
		pWnd->Edit_ACQ196_1_T1->setEnabled(true);

		pWnd->cb_dds1SampleClk_2->setEnabled(true);
		pWnd->Edit_ACQ196_2_T0->setEnabled(true);
		pWnd->Edit_ACQ196_2_T1->setEnabled(true);

		pWnd->cb_dds1SampleClk_3->setEnabled(true);
		pWnd->Edit_ACQ196_3_T0->setEnabled(true);
		pWnd->Edit_ACQ196_3_T1->setEnabled(true);

		pWnd->cb_dds1SampleClk_4->setEnabled(true);
		pWnd->Edit_ACQ196_4_T0->setEnabled(true);
		pWnd->Edit_ACQ196_4_T1->setEnabled(true);

		pWnd->cb_dds1SampleClk_5->setEnabled(true);
		pWnd->Edit_ACQ196_5_T0->setEnabled(true);
		pWnd->Edit_ACQ196_5_T1->setEnabled(true);

		pWnd->cb_dds1SampleClk_6->setEnabled(true);
		pWnd->Edit_ACQ196_6_T0->setEnabled(true);
		pWnd->Edit_ACQ196_6_T1->setEnabled(true);
#endif
	}

	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196, 0);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_2, 1);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_3, 2);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_4, 3);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_5, 4);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_6, 5);

	nClockSource = SYS_CLOCK_INTERNAL;
	pWnd->g_opMode = SYS_MODE_INIT;

#if PV_CONNECT
	pvPut_opCondition();
#endif	
	
//	initStactedWidgetAcq196();
	init_DDS1_GUI();

//	monitor_stop();

	digitizer_widget_clear();

	

//		pWnd->label_dds1Opmode->setText("Select Mode" ); // 2009. 11. 05
/*		pWnd->label_dds1DAQstate->setText("Not Ready to RUN");
		pWnd->label_dds1DAQstate_2->setText("Not Ready to RUN");
		pWnd->label_dds1DAQstate_3->setText("Not Ready to RUN");
		pWnd->label_dds1DAQstate_4->setText("Not Ready to RUN");
*/
/*		pWnd->label_dev1_cnt->setText(QString("0") );	// 2009. 08. 04
		pWnd->label_dev2_cnt->setText(QString("0") );
		pWnd->label_dev3_cnt->setText(QString("0") );
		pWnd->label_dev4_cnt->setText(QString("0") );

		// 2009. 10. 13. 
		pWnd->label_ccs_block->setText(QString("Run Condition") );
		pWnd->label_ccs_block->setPalette(pWnd->pal_normal);
*/
//	}
}
void pageDDS1::clicked_btn_dds1SetAllchs()
{
	int nOnOff;
	int nCurrentTab;
	QTableWidget *pwidget = NULL;


	nCurrentTab = pWnd->tabWidget_acq196->currentIndex();
	if( nCurrentTab == 0 )
		pwidget = pWnd->tableWidget_ACQ196;
	else if ( nCurrentTab == 1 )
		pwidget = pWnd->tableWidget_ACQ196_2;
	else if ( nCurrentTab == 2 )
		pwidget = pWnd->tableWidget_ACQ196_3;
	else if ( nCurrentTab == 3 )
		pwidget = pWnd->tableWidget_ACQ196_4;
	else if ( nCurrentTab == 4 )
		pwidget = pWnd->tableWidget_ACQ196_5;
	else if ( nCurrentTab == 5 )
		pwidget = pWnd->tableWidget_ACQ196_6;
	else return;

	if( pwidget->item(0, 0)->checkState() == Qt::Checked) nOnOff = 1;
	else nOnOff = 0;

	for( int i=0; i< 96; i++) 
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

void pageDDS1::clicked_btn_dds1DigitizerSetup(int ID)
{
	int iCAStatus;
	
	switch(ID) {
	case B1:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_1_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_1_T1->text().toDouble();
//		acq196Config[ID].frame_rate = (pWnd->cb_dds1SampleClk->currentText()).toInt();
//		pWnd->label_dev1_cnt->setText(QString("0") );	// 2009. 08. 04
		break;
	case B2:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_2_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_2_T1->text().toDouble();
//		acq196Config[ID].frame_rate = (pWnd->cb_dds1SampleClk_2->currentText()).toInt();
//		pWnd->label_dev2_cnt->setText(QString("0") );
		break;
	case B3:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_3_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_3_T1->text().toDouble();
//		acq196Config[ID].frame_rate = (pWnd->cb_dds1SampleClk_3->currentText()).toInt();
//		pWnd->label_dev3_cnt->setText(QString("0") );
		break;
	case B4:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_4_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_4_T1->text().toDouble();
//		acq196Config[ID].frame_rate = (pWnd->cb_dds1SampleClk_4->currentText()).toInt();
//		pWnd->label_dev4_cnt->setText(QString("0") );
		break;
	case B5:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_5_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_5_T1->text().toDouble();
//		pWnd->label_dev5_cnt->setText(QString("0") );
		break;
	case B6:
		acq196Config[ID].t0 = pWnd->Edit_ACQ196_6_T0->text().toDouble();
		acq196Config[ID].t1 = pWnd->Edit_ACQ196_6_T1->text().toDouble();
//		pWnd->label_dev6_cnt->setText(QString("0") );
		break;
	default: break;
	}


	if( (nClockSource == SYS_CLOCK_INTERNAL) && (acq196Config[ID].t0 < 1) ) {
		QMessageBox::information(0, "Kstar DDS", tr(" T0 required higher than one second for local test!!!!") );
		return;
	}

/************************************************************ 2009. 09. 01*/
// this for B1 is master......  move to TSS     2009. 10. 29
//#if USE_MASTER_SLAVE
#if 0	
	if( (pWnd->g_opMode ==SYS_MODE_REMOTE) &&  (ID != B1) ) 
	{
		acq196Config[ID].nClockDivider = acq196Config[B1].frame_rate/acq196Config[ID].frame_rate ;	// 2009. 10. 20 
		pWnd->printStatus("nClock Divider[%d] = %d\n", ID, acq196Config[ID].nClockDivider);

		iCAStatus=ca_Put_QStr( DDS1_BX_DIVIDER[ID], QString( "%1").arg( acq196Config[ID].nClockDivider ) );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1_BX_DIVIDER[%d]:%d, %s\n", ID, acq196Config[ID].nClockDivider ,  ca_message( iCAStatus) );
	}
#endif

/*************************************************************/	

	iCAStatus=ca_Put_QStr( DDS1_clearStatus[ID], QString( "1") );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! DDS1_clearStatus[%d], %s\n", ID,  ca_message( iCAStatus) );
//	usleep(100000);

	pvPut_ChannelBitMaskACQ196(ID);
//	usleep(100000);
	pvPut_TagNameACQ196(ID);	
//	usleep(200000);
	if ( nClockSource == SYS_CLOCK_INTERNAL ) 
		pvPut_InternalClockACQ196(ID);
	else
		pvPut_ExternalClockACQ196(ID);

//	queueNode.id = ID;
//	queueNode.status = ca_Get_QStr( DDS1_getState[ID] ).toInt();
//	pWnd->setDDS1StateFromPV(queueNode);
}

/* hand over command to CCS */
void pageDDS1::clicked_btn_dds1START_remote()
{
#if 0
	int iCAStatus;

	for( int i=0; i<USED_ACQ196; i++) 
	{
		iCAStatus=ca_Put_QStr( DDS1A_ADCstart[i], QString( "1") );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus("ERROR! DDS1A_ADCstart[%d], %s\n", i,  ca_message( iCAStatus) );
			return;
		}
		usleep(500000);
		iCAStatus=ca_Put_QStr( DDS1A_DAQstart[i], QString( "1") );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus("ERROR! DDS1A_DAQstart[%d], %s\n", i,  ca_message( iCAStatus) );
			return;
		}

		iCAStatus=ca_Put_QStr( DDS1A_SOFT_BX_REMOTE_READY[i], QString( "1") );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus("ERROR! DDS1A_SOFT_BX_REMOTE_READY[%d], %s\n", i,  ca_message( iCAStatus) );
			return;
		}
		usleep(500000);
	}
#endif
//	ca_Put_QStr( DDS1A_remoteReady, QString( "1") );
/*
	iCAStatus=ca_Put_QStr( DDS1A_TO_CCS_NOTIFY_READY, QString( "1") );
	if( iCAStatus != ECA_NORMAL) {
		pWnd->printStatus("ERROR! DDS1A_TO_CCS_NOTIFY_READY[%d], %s\n", 1,  ca_message( iCAStatus) );
		return;
	}
	*/
	pWnd->gb_dds1digisetup->setEnabled(false);
	pWnd->btn_dds1ResetMode->setEnabled(false);

	pWnd->btn_dds1RemoteReady->setEnabled(false);
	pWnd->btn_dds1_Retrieve->setEnabled(true);



	
}

void pageDDS1::clicked_btn_dds1_Arming()
{
//	for(int i=0; i<USED_ACQ196; i++)
//		is_Arm_OK[i] = 0;

	pWnd->btn_dds1LocalRun->setEnabled(false);

	ca_Put_QStr( DDS1A_ARM_ENABLE, QString( "1") );
	ca_Put_QStr( DDS1B_ARM_ENABLE, QString( "1") );
//	usleep(500000);
	timer_go();
	usleep(500000);
}

void pageDDS1::clicked_btn_forced_mode_reset()
{
	QMessageBox msgBox;
	msgBox.setIcon( QMessageBox::Warning );
	msgBox.setWindowTitle("Notice");
	msgBox.setText("DDS1A,B operation mode will be \"None\"!\nAre you sure?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

	switch( msgBox.exec() ) {
		case QMessageBox::Yes:
			break;
		case QMessageBox::No:
			return;
		default: return;
	}

	ca_Put_QStr( DDS1A_RESET, QString( "1") );
	ca_Put_QStr( DDS1B_RESET, QString( "1") );
	usleep(500000);
/*	ca_Put_QStr( DDS1A_OpMode, QString( "0") );
	ca_Put_QStr( DDS1B_OpMode, QString( "0") );
	usleep(500000); */

	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196, 0);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_2, 1);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_3, 2);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_4, 3);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_5, 4);
	pWnd->fileFunc.saveACQ196ConfigFile(pWnd->tableWidget_ACQ196_6, 5);

	nClockSource = SYS_CLOCK_INTERNAL;
	pWnd->g_opMode = SYS_MODE_INIT;

	init_DDS1_GUI();
//	monitor_stop();

	for(int i=0; i< 96; i++) {
		pWnd->tableWidget_ACQ196->removeRow(0);
		pWnd->tableWidget_ACQ196_2->removeRow(0);
		pWnd->tableWidget_ACQ196_3->removeRow(0);
		pWnd->tableWidget_ACQ196_4->removeRow(0);
		pWnd->tableWidget_ACQ196_5->removeRow(0);
		pWnd->tableWidget_ACQ196_6->removeRow(0);
	}
	pWnd->tableWidget_ACQ196->removeColumn(0);
	pWnd->tableWidget_ACQ196->removeColumn(0);
	pWnd->tableWidget_ACQ196_2->removeColumn(0);
	pWnd->tableWidget_ACQ196_2->removeColumn(0);
	pWnd->tableWidget_ACQ196_3->removeColumn(0);
	pWnd->tableWidget_ACQ196_3->removeColumn(0);
	pWnd->tableWidget_ACQ196_4->removeColumn(0);
	pWnd->tableWidget_ACQ196_4->removeColumn(0);
	pWnd->tableWidget_ACQ196_5->removeColumn(0);
	pWnd->tableWidget_ACQ196_5->removeColumn(0);
	pWnd->tableWidget_ACQ196_6->removeColumn(0);
	pWnd->tableWidget_ACQ196_6->removeColumn(0);
	

}




void pageDDS1::clicked_btn_dds1START_local()
{
//	int iCAStatus;
/*	
	if( pWnd->g_mInternalExternal == SYS_CLOCK_INTERNAL ) 
	{
		int nVal;
		for( int i=0; i<USED_ACQ196; i++) {
			nVal = ca_Get_QStr( DDS1A_SOFT_DAQ_START_INTN[i] ).toInt();
			nVal = nVal ? 0 : 1;
			ca_Put_QStr( DDS1A_SOFT_DAQ_START_INTN[i], QString( "%1").arg(nVal) );
			
			ca_Put_QStr( DDS1A_ADCstart[i], QString( "1") );
		}
*/
	ca_Put_QStr( DDS1A_RUN_START, QString( "1") );
	ca_Put_QStr( DDS1B_RUN_START, QString( "1") );
	usleep(500000);

/*
	for( int i=0; i<USED_ACQ196; i++) 
	{
		iCAStatus=ca_Put_QStr( DDS1A_SOFT_BX_INTNAL_DAQ[i], QString( "1") );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus("ERROR! DDS1A_SOFT_BX_INTNAL_DAQ[%d], %s\n", i,  ca_message( iCAStatus) );
			return;
		}
		usleep(100000);
	}
*/
	// prevent human mistake of pushbutton..... 2009. 10. 19.
//	usleep(1000000);
/*
	}
	else {
		iCAStatus=ca_Put_QStr( DDS1A_ADCstart[B1], QString( "1") );
		usleep(500000);
		iCAStatus=ca_Put_QStr( DDS1A_DAQstart[B1], QString( "1") );
		usleep(500000);
	}
*/
}


void pageDDS1::clicked_btn_dds1ShotNum()
{
	int iCAStatus=ca_Put_StrVal( DDS1A_shotNumber, pWnd->Edit_dds1ShotNum->text().toAscii().constData()  );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! DDS1A_shotNumber, %s\n", ca_message( iCAStatus) );
	iCAStatus=ca_Put_StrVal( DDS1B_shotNumber, pWnd->Edit_dds1ShotNum->text().toAscii().constData()  );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! DDS1B_shotNumber, %s\n", ca_message( iCAStatus) );
}

void pageDDS1::clicked_btn_singleRunDAQ(int ID)
{
//	int iCAStatus;
//	nVal = ca_Get_QStr( DDS1A_SOFT_DAQ_START_INTN[ID] ).toInt();
//	nVal = nVal ? 0 : 1;
//	ca_Put_QStr( DDS1A_SOFT_DAQ_START_INTN[ID], QString( "%1").arg(nVal) );
//	ca_Put_QStr( DDS1A_ADCstart[ID], QString( "1") );

//	printf("caput : DDS1A_SOFT_BX_INTNAL_DAQ[%d], %s\n", ID, str_DDS1A_SOFT_BX_INTNAL_DAQ[ID] );

	if( nClockSource== SYS_CLOCK_INTERNAL ) {
/*		iCAStatus=ca_Put_QStr( DDS1A_SOFT_BX_INTNAL_DAQ[ID], QString( "1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_SOFT_BX_INTNAL_DAQ[%d], %s\n", ID,  ca_message( iCAStatus) ); */
	}
	else if( nClockSource == SYS_CLOCK_EXTERNAL ) {
#if 0
		iCAStatus=ca_Put_QStr( DDS1A_ADCstart[ID], QString( "1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_ADCstart[%d], %s\n", ID,  ca_message( iCAStatus) );
		usleep(500000);

		iCAStatus=ca_Put_QStr( DDS1A_DAQstart[ID], QString( "1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_DAQstart[%d], %s\n", ID,  ca_message( iCAStatus) );
		
		iCAStatus=ca_Put_QStr( DDS1A_SOFT_BX_REMOTE_READY[ID], QString( "1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_SOFT_BX_REMOTE_READY[%d], %s\n", ID,  ca_message( iCAStatus) );
#endif
	} else {	
		pWnd->printStatus(" Wrong Mode: %d\n", nClockSource );
	}
}

void pageDDS1::clicked_btn_singleRunSave(int ID)
{
#if 0
	int iCAStatus;
	if( nClockSource == SYS_CLOCK_INTERNAL ) {
		iCAStatus=ca_Put_QStr( DDS1A_LocalSave[ID], QString("1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_LocalSave[%d], %s\n", ID,  ca_message( iCAStatus) );
		
	} else if( nClockSource == SYS_CLOCK_EXTERNAL )  {
#if 0
		iCAStatus=ca_Put_QStr( DDS1A_RemoteSave[ID], QString("1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! DDS1A_RemoteSave[%d], %s\n", ID,  ca_message( iCAStatus) );
#endif
	} else {
		pWnd->printStatus(" Wrong Mode: %d\n", nClockSource );
	}
#endif
}

void pageDDS1::clicked_btn_dds1CreateShot()
{
	int iCAStatus=ca_Put_StrVal( DDS1A_CREATE_LOCAL_SHOT,  pWnd->Edit_dds1ShotNum->text().toAscii().constData()  );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! DDS1A_CREATE_LOCAL_SHOT, %s\n",  ca_message( iCAStatus) );
	iCAStatus=ca_Put_StrVal( DDS1B_CREATE_LOCAL_SHOT,  pWnd->Edit_dds1ShotNum->text().toAscii().constData()  );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! DDS1B_CREATE_LOCAL_SHOT, %s\n",  ca_message( iCAStatus) );
}

void pageDDS1::clicked_btn_dds1_Retrieve()
{
	int iCAStatus;
	QString qstr;
/*
	if( (iCAStatus = ca_Get_QStr(  DDS1A_SOFT_SHOTSEQ_STARTED, qstr) ) != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! >> DDS1A_SOFT_SHOTSEQ_STARTED: %s\n",  ca_message( iCAStatus) );
		return;
	}
	if( qstr.toInt() > 0 ) {
		pWnd->printStatus("NOTIFY! >> [%d] CCS shot sequence started. (DDS1A_SOFT_SHOTSEQ_STARTED)\n", qstr.toInt() );
		pWnd->printStatus("NOTIFY! >> wait untill shot sequence finished!\n" );	
		return;
	}
*/
	if( (iCAStatus = ca_Get_QStr(  CCS_SHOTSEQ_START, qstr) ) != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! >> DDS1A_STATUS: %s\n",  ca_message( iCAStatus) );
		return;
	}
	if( qstr.toInt() ==  1 ) {
		pWnd->printStatus("NOTIFY! >> [%d] CCS shot sequence started. (DDS1A arming now)\n", qstr.toInt() );
		pWnd->printStatus("NOTIFY! >> wait untill shot sequence finished!\n" ); 
		return;
	}

	

/*
	iCAStatus=ca_Put_QStr( DDS1A_TO_CCS_NOTIFY_READY, QString( "0") );
	if( iCAStatus != ECA_NORMAL) {
		pWnd->printStatus("ERROR! DDS1A_TO_CCS_NOTIFY_READY[%d], %s\n", 0,  ca_message( iCAStatus) );
	} */
	pWnd->gb_dds1digisetup->setEnabled(true);
	pWnd->btn_dds1ResetMode->setEnabled(true);

	pWnd->btn_dds1RemoteReady->setEnabled(true);
	pWnd->btn_dds1_Retrieve->setEnabled(false);
}

void pageDDS1::idchanged_cb_dds1SampleClk(QString qstr)
{
//	acq196Config[B1].frame_rate = (pWnd->cb_dds1SampleClk->currentText()).toInt();
	acq196Config[B1].frame_rate = qstr.toInt();
}
void pageDDS1::idchanged_cb_dds1SampleClk_2(QString qstr)
{
//	int changedClk = (pWnd->cb_dds1SampleClk_2->currentText()).toInt();
	acq196Config[B2].frame_rate = qstr.toInt();
}
void pageDDS1::idchanged_cb_dds1SampleClk_3(QString qstr)
{
	acq196Config[B3].frame_rate = qstr.toInt();
}
void pageDDS1::idchanged_cb_dds1SampleClk_4(QString qstr)
{
	acq196Config[B4].frame_rate = qstr.toInt();
}
void pageDDS1::idchanged_cb_dds1SampleClk_5(QString qstr)
{
	int changedClk = qstr.toInt();
	acq196Config[B5].frame_rate = changedClk;
}
void pageDDS1::idchanged_cb_dds1SampleClk_6(QString qstr)
{
	int changedClk = qstr.toInt();
	acq196Config[B6].frame_rate = changedClk;
}

void pageDDS1::clicked_btn_dds1_assign_all()
{
//	for(int i=0; i<USED_ACQ196; i++)
//		clicked_btn_dds1DigitizerSetup(i);
	
	timerAssignAll= new QTimer(this);
	connect(timerAssignAll, SIGNAL(timeout()), this, SLOT(timerFunc_AssignAll()));
	timerAssignAll->start(600);
	pWnd->gb_MD_panel->setEnabled(false);
}
void pageDDS1::timerFunc_AssignAll()
{
	static int bd_cnt = 0;
	if( bd_cnt < USED_ACQ196) {
		clicked_btn_dds1DigitizerSetup(bd_cnt); bd_cnt++;
		pWnd->printStatus("B%d setup", bd_cnt );
	} else {
		bd_cnt = 0;
		timerAssignAll->stop();
		timerAssignAll = NULL;
		pWnd->gb_MD_panel->setEnabled(true);
	}
}


















/*
#####################################################################################

Local function 

#####################################################################################
*/
void pageDDS1::copy_ca_names()
{
	char strPrefix[8];
	
	for( int i=0; i<USED_ACQ196; i++) 
	{
		if( i<DDS1A_USED_CNT) 
			strcpy(strPrefix, "DDS1A");
		else
			strcpy(strPrefix, "DDS1B");

		sprintf( str_DDS1_chRbitMask[i], "%s_B%d_chRbitMask", strPrefix, (i+1));
		sprintf( str_DDS1_chMbitMask[i], "%s_B%d_chMbitMask", strPrefix, (i+1));
		sprintf( str_DDS1_chLbitMask[i], "%s_B%d_chLbitMask", strPrefix, (i+1));
		sprintf( str_DDS1_setInternal[i], "%s_B%d_setInternal", strPrefix, (i+1));
		sprintf( str_DDS1_setExternal[i], "%s_B%d_setExternal", strPrefix, (i+1));
		sprintf( str_DDS1_sampleRate[i], "%s_B%d_sampleRate", strPrefix, (i+1));
		sprintf( str_DDS1_setT0[i], 	"%s_B%d_setT0", strPrefix, (i+1));
		sprintf( str_DDS1_setT1[i],  "%s_B%d_setT1", strPrefix, (i+1));
//		sprintf( str_DDS1_getState[i], "%s_B%d_getState", strPrefix, (i+1));
		sprintf( str_DDS1_clearStatus[i], "%s_B%d_clearStatus", strPrefix, (i+1));

//		sprintf( str_DDS1_BX_PARSING_CNT[i], "%s_B%d_PARSING_CNT", strPrefix, (i+1));
//		sprintf( str_DDS1_BX_MDS_SND_CNT[i], "%s_B%d_MDS_SND_CNT", strPrefix, (i+1));
//		sprintf( str_DDS1_BX_DAQING_CNT[i], "%s_B%d_DAQING_CNT", strPrefix, (i+1));	// 2009. 08. 14

		sprintf( str_DDS1_BX_DIVIDER[i], "%s_B%d_ClockDivider", strPrefix, (i+1));	// 2009. 09. 01
		sprintf( str_DDS1_BX_CARD_MODE[i], "%s_B%d_CardMode", strPrefix, (i+1));	// 2009. 09. 01
	
	}
}

void pageDDS1::connectPVs()
{

	if( ca_ConnectCB( str_DDS1A_ARM_ENABLE,	DDS1A_ARM_ENABLE, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_ARM_ENABLE connect errer!" );
	if( ca_ConnectCB( str_DDS1A_RUN_START,	DDS1A_RUN_START, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_RUN_START connect errer!" );
	if( ca_ConnectCB( str_DDS1A_STATUS,	DDS1A_STATUS, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_STATUS connect errer!" );
	if( ca_ConnectCB( str_DDS1A_ERROR,	DDS1A_ERROR, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_ERROR connect errer!" );
	if( ca_ConnectCB( str_DDS1A_OpMode,	DDS1A_OpMode, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_OpMode connect errer!" );
	if( ca_ConnectCB( str_DDS1A_shotNumber, DDS1A_shotNumber, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1A_shotNumber connect errer!" );
	ca_ConnectCB( str_DDS1A_CREATE_LOCAL_SHOT, DDS1A_CREATE_LOCAL_SHOT, cdds1Data);
	ca_ConnectCB( str_DDS1A_RESET, DDS1A_RESET, cdds1Data);

	if( ca_ConnectCB( str_DDS1B_ARM_ENABLE,	DDS1B_ARM_ENABLE, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_ARM_ENABLE connect errer!" );
	if( ca_ConnectCB( str_DDS1B_RUN_START,	DDS1B_RUN_START, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_RUN_START connect errer!" );
	if( ca_ConnectCB( str_DDS1B_STATUS,	DDS1B_STATUS, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_STATUS connect errer!" );
	if( ca_ConnectCB( str_DDS1B_ERROR,	DDS1B_ERROR, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_ERROR connect errer!" );
	if( ca_ConnectCB( str_DDS1B_OpMode,	DDS1B_OpMode, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_OpMode connect errer!" );
	if( ca_ConnectCB( str_DDS1B_shotNumber, DDS1B_shotNumber, cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1B_shotNumber connect errer!" );
	ca_ConnectCB( str_DDS1B_CREATE_LOCAL_SHOT, DDS1B_CREATE_LOCAL_SHOT, cdds1Data);
	ca_ConnectCB( str_DDS1B_RESET, DDS1B_RESET, cdds1Data);



/*
	if( ca_ConnectCB( str_DDS1A_TO_CCS_NOTIFY_READY, DDS1A_TO_CCS_NOTIFY_READY, cdds1Data) != ECA_NORMAL )
		pWnd->printStatus( "DDS1A_TO_CCS_NOTIFY_READY connect errer!" ); */
	if( ca_ConnectCB( str_CCS_SHOTSEQ_START, CCS_SHOTSEQ_START, cdds1Data) != ECA_NORMAL )
		pWnd->printStatus( "CCS_SHOTSEQ_START connect errer!" );
	
	
	for( int i=0; i<USED_ACQ196; i++) {
	
		if( ca_ConnectCB( str_DDS1_chRbitMask[i], DDS1_chRbitMask[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_chRbitMask connect errer!" );
		if( ca_ConnectCB( str_DDS1_chMbitMask[i], DDS1_chMbitMask[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_chMbitMask connect errer!" );
		if( ca_ConnectCB( str_DDS1_chLbitMask[i], DDS1_chLbitMask[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_chLbitMask connect errer!" );
	
		if( ca_ConnectCB( str_DDS1_setInternal[i], DDS1_setInternal[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_setInternal connect errer!" );
		if( ca_ConnectCB( str_DDS1_setExternal[i], DDS1_setExternal[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_setExternal connect errer!" );
		if( ca_ConnectCB( str_DDS1_sampleRate[i], DDS1_sampleRate[i], cdds1Data) != ECA_NORMAL ) pWnd->printStatus( "DDS1_sampleRate connect errer!" );
		
		ca_ConnectCB( str_DDS1_setT0[i], DDS1_setT0[i], cdds1Data);
		ca_ConnectCB( str_DDS1_setT1[i], DDS1_setT1[i], cdds1Data);
	
//		ca_ConnectCB( str_DDS1_getState[i], DDS1_getState[i], cdds1Data);
		ca_ConnectCB( str_DDS1_clearStatus[i], DDS1_clearStatus[i], cdds1Data);


//		ca_ConnectCB( str_DDS1_BX_PARSING_CNT[i], DDS1_BX_PARSING_CNT[i], cdds1Data);
//		ca_ConnectCB( str_DDS1_BX_MDS_SND_CNT[i], DDS1_BX_MDS_SND_CNT[i], cdds1Data);

//		ca_ConnectCB( str_DDS1_BX_DAQING_CNT[i], DDS1_BX_DAQING_CNT[i], cdds1Data);		// 2009. 08. 14

		ca_ConnectCB( str_DDS1_BX_DIVIDER[i], DDS1_BX_DIVIDER[i], cdds1Data);				// 2009. 09. 01
		ca_ConnectCB( str_DDS1_BX_CARD_MODE[i], DDS1_BX_CARD_MODE[i], cdds1Data);		// 2009. 09. 01		


		connectTagPVs(i);


	}

/*	connectTagPVs(B1);
	connectTagPVs(B2);
	connectTagPVs(B3);
	connectTagPVs(B4);
	connectTagPVs(B5);
	connectTagPVs(B6);
*/
/* don't need LTU connection...    2010.6.2  woong
	{
		ca_ConnectCB( DDS1A_LTU_ShotTerm,	pvDDS1ALTU.SHOT_TERM, cdds1Data);
	
		ca_ConnectCB( DDS1A_LTU_Trig_p0,	pvDDS1ALTU.TRIG[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Trig_p1,	pvDDS1ALTU.TRIG[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Trig_p2,	pvDDS1ALTU.TRIG[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Trig_p3,	pvDDS1ALTU.TRIG[3], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Clock_p0,		pvDDS1ALTU.CLOCK[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Clock_p1,		pvDDS1ALTU.CLOCK[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Clock_p2,		pvDDS1ALTU.CLOCK[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_Clock_p3,		pvDDS1ALTU.CLOCK[3], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T0_p0,			pvDDS1ALTU.T0[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T0_p1,			pvDDS1ALTU.T0[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T0_p2,			pvDDS1ALTU.T0[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T0_p3,			pvDDS1ALTU.T0[3], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T1_p0,			pvDDS1ALTU.T1[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T1_p1,			pvDDS1ALTU.T1[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T1_p2,			pvDDS1ALTU.T1[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_T1_p3,			pvDDS1ALTU.T1[3], cdds1Data);
	
		ca_ConnectCB( DDS1A_LTU_setup_p0,	pvDDS1ALTU.SETUP_Pt[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_setup_p1,	pvDDS1ALTU.SETUP_Pt[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_setup_p2,	pvDDS1ALTU.SETUP_Pt[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_setup_p3,	pvDDS1ALTU.SETUP_Pt[3], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_setup_p4,	pvDDS1ALTU.SETUP_Pt[4], cdds1Data);
	
		ca_ConnectCB( DDS1A_LTU_mTrig,		pvDDS1ALTU.mTRIG, cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mClock_sec0, pvDDS1ALTU.mCLOCK[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mClock_sec1, pvDDS1ALTU.mCLOCK[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mClock_sec2, pvDDS1ALTU.mCLOCK[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT0_sec0,	pvDDS1ALTU.mT0[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT0_sec1,	pvDDS1ALTU.mT0[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT0_sec2,	pvDDS1ALTU.mT0[2], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT1_sec0,	pvDDS1ALTU.mT1[0], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT1_sec1,	pvDDS1ALTU.mT1[1], cdds1Data);
		ca_ConnectCB( DDS1A_LTU_mT1_sec2,	pvDDS1ALTU.mT1[2], cdds1Data);
	}
*/
}

void pageDDS1::connectLTU_callback()
{
/*
	cbFunc.startMonitoringPV( pvDDS1ALTU.SHOT_TERM, 1);

	for(int i=0; i<4; i++){
		cbFunc.startMonitoringPV( pvDDS1ALTU.TRIG[i], 1);
		cbFunc.startMonitoringPV( pvDDS1ALTU.CLOCK[i], 1);
		cbFunc.startMonitoringPV( pvDDS1ALTU.T0[i], 1);
		cbFunc.startMonitoringPV( pvDDS1ALTU.T1[i], 1);
	}
	for(int i=0; i<5; i++){
		cbFunc.startMonitoringPV( pvDDS1ALTU.SETUP_Pt[i], 1);
	}

	cbFunc.startMonitoringPV( pvDDS1ALTU.mTRIG, 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mCLOCK[0], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mCLOCK[1], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mCLOCK[2], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT0[0], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT0[1], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT0[2], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT1[0], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT1[1], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.mT1[2], 1);
*/
}

void pageDDS1::disconnectPVs()
{
	ca_Disconnect( DDS1A_ARM_ENABLE);
	ca_Disconnect( DDS1A_RUN_START);
	ca_Disconnect( DDS1A_STATUS);
	ca_Disconnect( DDS1A_ERROR);
	ca_Disconnect( DDS1A_OpMode );
	ca_Disconnect( DDS1A_shotNumber );
	ca_Disconnect( DDS1A_CREATE_LOCAL_SHOT );
	ca_Disconnect( DDS1A_RESET);

	ca_Disconnect( DDS1B_ARM_ENABLE);
	ca_Disconnect( DDS1B_RUN_START);
	ca_Disconnect( DDS1B_STATUS);
	ca_Disconnect( DDS1B_ERROR);
	ca_Disconnect( DDS1B_OpMode );
	ca_Disconnect( DDS1B_shotNumber );
	ca_Disconnect( DDS1B_CREATE_LOCAL_SHOT );
	ca_Disconnect( DDS1B_RESET);

	

//	ca_Disconnect( DDS1A_TO_CCS_NOTIFY_READY);	// 2009. 09. 04 
	ca_Disconnect( CCS_SHOTSEQ_START);		// 2009. 10. 13 
		
	for( int i=0; i<USED_ACQ196; i++) {
	
		ca_Disconnect( DDS1_chRbitMask[i] );
		ca_Disconnect( DDS1_chMbitMask[i] );
		ca_Disconnect( DDS1_chLbitMask[i] );
		ca_Disconnect( DDS1_setInternal[i] );
		ca_Disconnect( DDS1_setExternal[i] );
		ca_Disconnect( DDS1_sampleRate[i] );		
		
		ca_Disconnect( DDS1_setT0[i]);
		ca_Disconnect( DDS1_setT1[i]);
		
//		ca_Disconnect( DDS1_getState[i]);
		ca_Disconnect( DDS1_clearStatus[i]);
		

//		ca_Disconnect( DDS1_BX_PARSING_CNT[i]);
//		ca_Disconnect( DDS1_BX_MDS_SND_CNT[i]);


//		ca_Disconnect( DDS1_BX_DAQING_CNT[i]);				// 2009. 08. 14

		disconnectTagPVs(i);
	}
	
/*	disconnectTagPVs(B1);
	disconnectTagPVs(B2);
	disconnectTagPVs(B3);
	disconnectTagPVs(B4);
	disconnectTagPVs(B5);
	disconnectTagPVs(B6); */
/*
		ca_Disconnect( pvDDS1ALTU.SHOT_TERM );
		for(int j=0; j<4; j++) {
			ca_Disconnect( pvDDS1ALTU.TRIG[j] );
			ca_Disconnect( pvDDS1ALTU.CLOCK[j] );
			ca_Disconnect( pvDDS1ALTU.T0[j] );
			ca_Disconnect( pvDDS1ALTU.T1[j] );
		ca_Disconnect( pvDDS1ALTU.SETUP_Pt[j] );
		}		
		for(int j=0; j<3; j++) {
			ca_Disconnect( pvDDS1ALTU.mCLOCK[j] );
			ca_Disconnect( pvDDS1ALTU.mT0[j] );
			ca_Disconnect( pvDDS1ALTU.mT1[j] );
		}
		ca_Disconnect( pvDDS1ALTU.SETUP_Pt[4] );
		ca_Disconnect( pvDDS1ALTU.mTRIG );
*/
}


void pageDDS1::initTableWidgetACQ196()
{
//	int width;
/*	QStringList labels;
	labels << tr("On") << tr("TAG") ;

	pWnd->tableWidget_ACQ196->setColumnCount(2);
	pWnd->tableWidget_ACQ196->setHorizontalHeaderLabels(labels);
	pWnd->tableWidget_ACQ196->setItemDelegate(new NodeDelegate(this));

	pWnd->tableWidget_ACQ196->horizontalHeader()->resizeSection(0, 30);
	pWnd->tableWidget_ACQ196->horizontalHeader()->resizeSection(1, 200);
*/
	digitizer_widget_clear();

	setupACQ196Table(pWnd->tableWidget_ACQ196, B1);
	setupACQ196Table(pWnd->tableWidget_ACQ196_2, B2);
	setupACQ196Table(pWnd->tableWidget_ACQ196_3, B3);
	setupACQ196Table(pWnd->tableWidget_ACQ196_4, B4);
	setupACQ196Table(pWnd->tableWidget_ACQ196_5, B5);
	setupACQ196Table(pWnd->tableWidget_ACQ196_6, B6);

	pWnd->tabWidget_acq196->setCurrentIndex(0);

}

void pageDDS1::setupACQ196Table(QTableWidget *pwidget, int num)
{
	int row;
	int nCh, nOnOff;
	char fileName[128];
	char bufLineScan[512];
	char strTag[32];
//	char strVoltage[16];
//	float fVoltage;
	int nSampleRate;
	char strTrigger[32];
	char strPeriod[32];
	FILE *fp;
	char buf[64];
//	char strMode[16];

	QStringList labels;
	labels << tr("On") << tr("TAG") ;


	pwidget->setColumnCount(2);
	pwidget->setHorizontalHeaderLabels(labels);
	pwidget->setItemDelegate(new NodeDelegate(this));

	pwidget->horizontalHeader()->resizeSection(0, 30);
	pwidget->horizontalHeader()->resizeSection(1, 177);

	QLineEdit *ptrigger=NULL;
	QLineEdit *pperiod=NULL;
	QComboBox *pclock=NULL;


	if( pWnd->g_opMode == SYS_MODE_REMOTE )
		sprintf(fileName, "/home/kstar/dds_config/DDS1_%d_remote.cfg", num+1 );
	else if ( pWnd->g_opMode == SYS_MODE_LOCAL )
		sprintf(fileName, "/home/kstar/dds_config/DDS1_%d_local.cfg", num+1 );
	else if ( pWnd->g_opMode == SYS_MODE_CALIB)
		sprintf(fileName, "/home/kstar/dds_config/DDS1_%d_calib.cfg", num+1 );
	else {
		printf("wrong operation mode (%d) !! \n", pWnd->g_opMode );
		return;
	}

	
	if ( num == B1 ){
		ptrigger = pWnd->Edit_ACQ196_1_T0;
		pperiod = pWnd->Edit_ACQ196_1_T1;
		pclock = pWnd->cb_dds1SampleClk;
	} else if( num == B2) {
		ptrigger = pWnd->Edit_ACQ196_2_T0;
		pperiod = pWnd->Edit_ACQ196_2_T1;
		pclock = pWnd->cb_dds1SampleClk_2;
	} else if( num == B3) {
		ptrigger = pWnd->Edit_ACQ196_3_T0;
		pperiod = pWnd->Edit_ACQ196_3_T1;
		pclock = pWnd->cb_dds1SampleClk_3;
	} else if( num == B4) {
		ptrigger = pWnd->Edit_ACQ196_4_T0;
		pperiod = pWnd->Edit_ACQ196_4_T1;
		pclock = pWnd->cb_dds1SampleClk_4;
	} else if( num == B5) {
		ptrigger = pWnd->Edit_ACQ196_5_T0;
		pperiod = pWnd->Edit_ACQ196_5_T1;
		pclock = pWnd->cb_dds1SampleClk_5;
	} else if( num == B6) {
		ptrigger = pWnd->Edit_ACQ196_6_T0;
		pperiod = pWnd->Edit_ACQ196_6_T1;
		pclock = pWnd->cb_dds1SampleClk_6;
	} else {
		printf(" >> setupACQ196Table(%d) wrong argument!\n", num);
		return;
	}


	if( (fp = fopen(fileName, "r") ) == NULL )
	{	
//		fprintf(stderr, "Can't Read dds2 channel-tag mapping file\n");
		sprintf(bufLineScan, "Can't read %s file", fileName);
		QMessageBox::information(0, "DDS setup", bufLineScan );
		return;
	} else {
//		printf("\"%s\" open OK!\n", fileName);	
	}

	if( fgets(bufLineScan, 512, fp) == NULL ){ fclose(fp); return; }
	sscanf(bufLineScan, "%s %s", buf, strTrigger);
	ptrigger->setText(strTrigger);

	if( fgets(bufLineScan, 512, fp) == NULL ) { fclose(fp); return; }
	sscanf(bufLineScan, "%s %s", buf, strPeriod);	
	pperiod->setText( strPeriod );

//	printf("%d, trig:perido:( %s, %s )\n", num,  strTrigger, strPeriod );

	if( fgets(bufLineScan, 512, fp) == NULL ){ fclose(fp); return; }
	sscanf(bufLineScan, "%s %d", buf, &nSampleRate);
	if( nSampleRate <= 0) nSampleRate = 1000;
	switch(nSampleRate) {
		case 1000: pclock->setCurrentIndex(0); break;
		case 2000: pclock->setCurrentIndex(1); break;
		case 5000: pclock->setCurrentIndex(2); break;
		case 10000: pclock->setCurrentIndex(3); break;
		case 20000: pclock->setCurrentIndex(4); break;
		case 50000: pclock->setCurrentIndex(5); break;
		case 100000: pclock->setCurrentIndex(6); break;
		case 200000: pclock->setCurrentIndex(7); break;
		default: printf("wrong sample rate %d\n", nSampleRate); {
			fclose(fp);
			return;
			}
	}
//	Edit_ACQ196_sampleclk1->setText( strSampleRate );

	

	for( int i=0; i< 96; i++) {

		if( fgets(bufLineScan, 512, fp) == NULL ) { fclose(fp); return; }
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
	
}

/*
void pageDDS1::initStactedWidgetAcq196()
{
	pWnd->gb_dds1Opmode->setEnabled(true);

	pWnd->btn_dds1OPremote->setChecked(false);
	pWnd->btn_dds1OPcalibration->setChecked(false);
	pWnd->btn_dds1OPlocal->setChecked(false);

	pWnd->gb_dds1digisetup->setEnabled(false);
	pWnd->gb_dds1remote->setEnabled(false);
	pWnd->gb_dds1calibration->setEnabled(false);
	pWnd->gb_dds1Local->setEnabled(false);
	tabWidget_acq196->setEnabled(false); 

}
*/

/* input ID: 0~3 */
void pageDDS1::connectTagPVs(int ID)
{
	char pvName[32];
	char strPrefix[8];


	if( ID < DDS1A_USED_CNT) 
		strcpy(strPrefix, "DDS1A");
	else
		strcpy(strPrefix, "DDS1B");
		

	for( int i=0; i< 96; i++) {
		sprintf(pvName, "%s_B%d_tag_%d", strPrefix, (ID+1), i);
		if( ca_ConnectCB( pvName, DDS1A_tag[ID][i], cdds1Data) != ECA_NORMAL ) 
			pWnd->printStatus( QString("DDS1A_tag_%1 connect error").arg(i).toAscii().constData() );
			
//		usleep(10000);
	}
}

void pageDDS1::disconnectTagPVs(int id)
{
	for( int i=0; i< 96; i++) {
		ca_Disconnect(DDS1A_tag[id][i] );
	}
}

void pageDDS1::pvPut_TagNameACQ196(int ID)
{
	int iCAStatus;
	char strTag[40];
	QTableWidget *pwidget = NULL;
	QTableWidgetItem *item = NULL;
	QTableWidgetItem *item2 = NULL;

//	printf("board: %d \n", ID );

	switch(ID) {
		case B1: pwidget = pWnd->tableWidget_ACQ196; break;
		case B2: pwidget = pWnd->tableWidget_ACQ196_2; break;
		case B3: pwidget = pWnd->tableWidget_ACQ196_3; break;
		case B4: pwidget = pWnd->tableWidget_ACQ196_4; break;
		case B5: pwidget = pWnd->tableWidget_ACQ196_5; break;
		case B6: pwidget = pWnd->tableWidget_ACQ196_6; break;
		default: 
			pWnd->printStatus(QString("pvPut_TagNameACQ196(ID=%1)... error").arg(ID).toAscii().constData() ); 
			return;
	}

	if( !pwidget)  {
		pWnd->printStatus("ERROR!  can't get the correct pointer!"); 
		return;
	}

	for( int i=0; i< 96; i++) 
	{
		item = pwidget->item(i, 0);
		if( item->checkState() == Qt::Checked)
		{
			item2 = pwidget->item(i, 1);
			strcpy( strTag, item2->text().toAscii().constData() );

			iCAStatus=ca_Put_QStr( DDS1A_tag[ID][i], QString( "%1").arg( strTag ) );
//			printStatus( ca_Disconnect( pWnd->chidChannelToPV ).toAscii().constData() );
		}
		if( i == 32 )
			usleep(300000);
		else if( i == 64 )
			usleep(300000);
	}
}

/*
input range: 0,~ 3
*/
void pageDDS1::pvPut_ChannelBitMaskACQ196(int num)
{
	int iCAStatus;
	QTableWidget *pwidget = NULL;
	QTableWidgetItem *item = NULL;
	
	switch(num) {
		case B1: pwidget = pWnd->tableWidget_ACQ196; break;
		case B2: pwidget = pWnd->tableWidget_ACQ196_2; break;
		case B3: pwidget = pWnd->tableWidget_ACQ196_3; break;
		case B4: pwidget = pWnd->tableWidget_ACQ196_4; break;
		case B5: pwidget = pWnd->tableWidget_ACQ196_5; break;
		case B6: pwidget = pWnd->tableWidget_ACQ196_6; break;
		default:
			pWnd->printStatus(QString("pvPut_ChannelBitMaskACQ196(num=%1)... error").arg(num).toAscii().constData() ); 
			return;
	}

	acq196Config[num].channelBitMask[0][ACQ196_RIGHT] = 0x0;
	acq196Config[num].channelBitMask[0][ACQ196_MIDDLE] = 0x0;
	acq196Config[num].channelBitMask[0][ACQ196_LEFT] = 0x0;
	for( int i=0; i< 96; i++) {
		item = pwidget->item(i, 0);
		if( item->checkState() == Qt::Checked) {
			if( i < 32 ) 
				acq196Config[num].channelBitMask[0][ACQ196_RIGHT] |=  0x1 << i ;
			else if( i< 64 )
				acq196Config[num].channelBitMask[0][ACQ196_MIDDLE] |=  0x1 << (i-32) ;
			else 
				acq196Config[num].channelBitMask[0][ACQ196_LEFT] |=  0x1 << (i-64) ;
		}
	}

//	fprintf(stdout, "bitmask : 0x%X\n", acq196Config.channel_bitmask[0] );
	iCAStatus=ca_Put_QStr( DDS1_chRbitMask[num], QString( "%1").arg( acq196Config[num].channelBitMask[0][ACQ196_RIGHT] ) );
	iCAStatus=ca_Put_QStr( DDS1_chMbitMask[num], QString( "%1").arg( acq196Config[num].channelBitMask[0][ACQ196_MIDDLE] ) );
	iCAStatus=ca_Put_QStr( DDS1_chLbitMask[num], QString( "%1").arg( acq196Config[num].channelBitMask[0][ACQ196_LEFT] ) );

}

void pageDDS1::pvPut_InternalClockACQ196(int ID)
{
	int iCAStatus;
	iCAStatus=ca_Put_QStr( DDS1_setInternal[ID], QString( "1") );
	iCAStatus=ca_Put_QStr( DDS1_sampleRate[ID], QString( "%1").arg( acq196Config[ID].frame_rate)  );
	if( (iCAStatus = ca_Put_Double(  DDS1_setT0[ID], acq196Config[ID].t0) ) != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> DDS1_setT0[%d](%f) : %s\n", ID, acq196Config[ID].t0,  ca_message( iCAStatus) );
	if( (iCAStatus = ca_Put_Double(  DDS1_setT1[ID], acq196Config[ID].t1) ) != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> DDS1_setT1[%d](%f) : %s\n", ID, acq196Config[ID].t1,  ca_message( iCAStatus) );

}

/*
Board index is same to port number    2009. 7. 30
*/
void pageDDS1::pvPut_ExternalClockACQ196(int ID)
{
	double time_tail;
//	char buf[32];
	int iCAStatus;

	if( acq196Config[ID].frame_rate < 2000 ) 		time_tail = DDS1_LTU_TAIL_UNDER_2K;
	else if( acq196Config[ID].frame_rate < 5000 )  time_tail = DDS1_LTU_TAIL_UNDER_5K;
	else if( acq196Config[ID].frame_rate < 10000 )  time_tail = DDS1_LTU_TAIL_UNDER_10K;
	else time_tail = DDS1_LTU_TAIL_DEFAULT;
	
	iCAStatus=ca_Put_QStr( DDS1_setExternal[ID], QString( "1") );
	
#if 0 // this part to be done in TSS  2009. 10. 29
	iCAStatus=ca_Put_QStr( DDS1_sampleRate[ID], QString("%1").arg( acq196Config[ID].frame_rate ) );
	if( (iCAStatus = ca_Put_Double(  DDS1_setT0[ID], acq196Config[ID].t0) ) != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> DDS1_setT0[%d](%f) : %s\n", ID, acq196Config[ID].t0,  ca_message( iCAStatus) );
	if( (iCAStatus = ca_Put_Double(  DDS1_setT1[ID], acq196Config[ID].t1) ) != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> DDS1_setT1[%d](%f) : %s\n", ID, acq196Config[ID].t1,  ca_message( iCAStatus) );

/************************************************************ 2009. 09. 01*/
	double total_time = acq196Config[ID].t1 + time_tail;
#if USE_MASTER_SLAVE
	if( ID == B1 ) {
#endif
	// set PV to DDS1A_LTU system.
	iCAStatus=ca_Put_QStr( pvDDS1ALTU.CLOCK[ID], QString( "%1").arg( acq196Config[ID].frame_rate ) );
	usleep(200000);	// it would be good for TSS recognize.
	iCAStatus=ca_Put_QStr( pvDDS1ALTU.TRIG[ID], QString( "0") ); // always active High(= 0)	
	if( (iCAStatus = ca_Put_Double(  pvDDS1ALTU.T0[ID], acq196Config[ID].t0) ) != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> pvDDS1ALTU.T0[%d](%f) : %s\n", ID, acq196Config[ID].t0,  ca_message( iCAStatus) );

	iCAStatus = ca_Put_Double(  pvDDS1ALTU.T1[ID], acq196Config[ID].t1+ time_tail );
	if( iCAStatus  != ECA_NORMAL )
		pWnd->printStatus("ERROR! >> pvDDS1ALTU.T1[%d](%f) : %s\n", ID, acq196Config[ID].t1+ time_tail,  ca_message( iCAStatus) );

	iCAStatus=ca_Put_QStr( pvDDS1ALTU.SETUP_Pt[ID], QString("1") );
	if( iCAStatus != ECA_NORMAL)
		pWnd->printStatus("ERROR! pvDDS1ALTU.SETUP_Pt[%d], %s\n", ID,  ca_message( iCAStatus) );

	if( total_time > dMax_T1_time) dMax_T1_time = total_time;
#if USE_MASTER_SLAVE	
	}
#endif
	
// this for B1 is master......
// 슬레이브 보드가 마스터 보드보다 더 긴 시간의 clock 이 필요할 경우....
// extend master clock out period......
#if USE_MASTER_SLAVE
	
	if( total_time > dMax_T1_time && ( ID != B1) ) {
		dMax_T1_time = total_time;
		iCAStatus = ca_Put_Double( pvDDS1ALTU.T1[B1], dMax_T1_time );
		if( iCAStatus  != ECA_NORMAL )
			pWnd->printStatus("ERROR! >> pvDDS1ALTU.T1[%d](%f) : %s\n", B1, dMax_T1_time,	ca_message( iCAStatus) );

		iCAStatus=ca_Put_QStr( pvDDS1ALTU.SETUP_Pt[B1], QString("1") );
		if( iCAStatus != ECA_NORMAL)
			pWnd->printStatus("ERROR! pvDDS1ALTU.SETUP_Pt[%d], %s\n", B1,  ca_message( iCAStatus) );
	}
#endif

#endif

}
void pageDDS1::digitizer_widget_clear()
{
	for(int i=0; i< 96; i++) {
		// remove always first row
		pWnd->tableWidget_ACQ196->removeRow(0);
		pWnd->tableWidget_ACQ196_2->removeRow(0);
		pWnd->tableWidget_ACQ196_3->removeRow(0);
		pWnd->tableWidget_ACQ196_4->removeRow(0);
		pWnd->tableWidget_ACQ196_5->removeRow(0);
		pWnd->tableWidget_ACQ196_6->removeRow(0);
	}
	pWnd->tableWidget_ACQ196->removeColumn(0);
	pWnd->tableWidget_ACQ196->removeColumn(0);
	pWnd->tableWidget_ACQ196_2->removeColumn(0);
	pWnd->tableWidget_ACQ196_2->removeColumn(0);
	pWnd->tableWidget_ACQ196_3->removeColumn(0);
	pWnd->tableWidget_ACQ196_3->removeColumn(0);
	pWnd->tableWidget_ACQ196_4->removeColumn(0);
	pWnd->tableWidget_ACQ196_4->removeColumn(0);
	pWnd->tableWidget_ACQ196_5->removeColumn(0);
	pWnd->tableWidget_ACQ196_5->removeColumn(0);
	pWnd->tableWidget_ACQ196_6->removeColumn(0);
	pWnd->tableWidget_ACQ196_6->removeColumn(0);
}

int pageDDS1::checkCmd()
{
	double dval = -1;
	double dval2 = -1;
	int nval=0;
	char strPrefix[8];
	char buff[64];
	
	int iCAStatus=ca_Get_Double(DDS1A_OpMode, dval);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1A_OpMode:%d, %s\n", (int)dval,  ca_message( iCAStatus) );
		return 0;
	}
	iCAStatus=ca_Get_Double(DDS1B_OpMode, dval2);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1B_OpMode:%d, %s\n", (int)dval2,  ca_message( iCAStatus) );
		return 0;
	}
	QMessageBox msgBox;	
	msgBox.setIcon( QMessageBox::Warning );
	msgBox.setWindowTitle("Notice");

	for( int i=0; i<2; i++) 
	{
		if( i == 0 ) {
			strcpy(strPrefix, "DDS1A");
			nval = (int)dval;
		}			
		else if( i == 1 ) {
			strcpy(strPrefix, "DDS1B");
			nval = (int)dval2;
		}
	
		switch( nval  ) {
			case SYS_MODE_REMOTE:
				sprintf(buff, "Somebody preempt \"%s\" to <Remote> operation!", strPrefix);
				msgBox.setText(buff);
				msgBox.exec();
				return 0;
			case SYS_MODE_CALIB:
				sprintf(buff, "Somebody preempt \"%s\" to <Calibration> mode!", strPrefix);
				msgBox.setText(buff);
				msgBox.exec();
				return 0;
			case SYS_MODE_LOCAL:
				sprintf(buff, "Somebody preempt \"%s\" to <Local> operation!", strPrefix);
				msgBox.setText(buff);
				msgBox.exec();
				return 0;
			case SYS_MODE_INIT:
				break;
			default: 
				sprintf(buff, "\"%s\" op mode has wrong value!", strPrefix);
				msgBox.setText(buff);
				msgBox.exec();
				return 0;
		}
	}

	return 1;
}

int pageDDS1::check_Opmode_connection()
{
	double dval = -1;
	double dval2 = -1;
	int nval=0;
	char strPrefix[8];
	char buff[64];
	
	int iCAStatus=ca_Get_Double(DDS1A_OpMode, dval);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1A_OpMode:%d, %s\n", (int)dval,  ca_message( iCAStatus) );
		return 0;
	}
	iCAStatus=ca_Get_Double(DDS1B_OpMode, dval2);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1B_OpMode:%d, %s\n", (int)dval2,  ca_message( iCAStatus) );
		return 0;
	}
	

	for( int i=0; i<2; i++) 
	{
		if( i == 0 ) {
			strcpy(strPrefix, "DDS1A");
			nval = (int)dval;
		}			
		else if( i == 1 ) {
			strcpy(strPrefix, "DDS1B");
			nval = (int)dval2;
		}
	
		switch( nval  ) {
			case SYS_MODE_REMOTE:
				pWnd->printStatus( "Somebody preempt \"%s\" to <Remote> operation!", strPrefix);
				break;
			case SYS_MODE_CALIB:
				pWnd->printStatus("Somebody preempt \"%s\" to <Calibration> mode!", strPrefix);
				break;
			case SYS_MODE_LOCAL:
				pWnd->printStatus( "Somebody preempt \"%s\" to <Local> operation!", strPrefix);
				break;
			case SYS_MODE_INIT:
				break;
			default: 
				QMessageBox msgBox;	
				msgBox.setIcon( QMessageBox::Warning );
				msgBox.setWindowTitle("Notice");
				sprintf(buff, "\"%s\" op mode has wrong value!", strPrefix);
				msgBox.setText(buff);
				msgBox.exec();
				return 0;
		}
	}

	return 1;
}


void pageDDS1::pvPut_opCondition()
{
	/*  Important.....................					2009. 10. 17   */
	/*  you must set Operation mode befor set Card Mode */

	double dval = 99;
	double dval2 = 99;
	
	int iCAStatus=ca_Get_Double(DDS1A_OpMode, dval);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1A_OpMode:%d, %s\n", (int)dval,  ca_message( iCAStatus) );
		return ;
	}
	if( dval < 1 || pWnd->g_opMode == SYS_MODE_INIT ) {
		iCAStatus=ca_Put_QStr( DDS1A_OpMode, QString( "%1").arg( pWnd->g_opMode ) );
		if( iCAStatus  != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! DDS1A_OpMode(%d), %s\n", pWnd->g_opMode,  ca_message( iCAStatus) );
			return;
		}
	}
	iCAStatus=ca_Get_Double(DDS1B_OpMode, dval2);
	if( iCAStatus  != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! get DDS1B_OpMode:%d, %s\n", (int)dval2,  ca_message( iCAStatus) );
		return ;
	}
	if( dval2 < 1 || pWnd->g_opMode == SYS_MODE_INIT  ) {
		iCAStatus=ca_Put_QStr( DDS1B_OpMode, QString( "%1").arg( pWnd->g_opMode ) );
		if( iCAStatus  != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! DDS1B_OpMode(%d), %s\n", pWnd->g_opMode,  ca_message( iCAStatus) );
			return;
		}
	}
	
#if USE_MASTER_SLAVE

	switch( pWnd->g_opMode ) {
	case SYS_MODE_REMOTE: 

		iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[B1], QString( "%1").arg( CARD_MODE_MASTER ) );
		if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus("ERROR! DDS1A_B1_CardMode(%d), %s\n", CARD_MODE_MASTER,  ca_message( iCAStatus) );
		
		iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[B2], QString( "%1").arg( CARD_MODE_SLAVE ) );
		if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus("ERROR! DDS1A_B2_CardMode(%d), %s\n", CARD_MODE_SLAVE,  ca_message( iCAStatus) );
		
		iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[B3], QString( "%1").arg( CARD_MODE_SLAVE ) );
		if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus("ERROR! DDS1A_B3_CardMode(%d), %s\n", CARD_MODE_SLAVE,  ca_message( iCAStatus) );
		
		iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[B4], QString( "%1").arg( CARD_MODE_SLAVE ) );
		if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus("ERROR! DDS1A_B4_CardMode(%d), %s\n", CARD_MODE_SLAVE,  ca_message( iCAStatus) );
		
		break;
	case SYS_MODE_CALIB:
	case SYS_MODE_LOCAL:

		for(int i=0; i<USED_ACQ196; i++) {
			iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[i], QString( "%1").arg( CARD_MODE_SINGLE ) );
			if( iCAStatus  != ECA_NORMAL )
				pWnd->printStatus("ERROR! DDS1A_B%d_CardMode(%d), %s\n", i+1, CARD_MODE_SINGLE,  ca_message( iCAStatus) );
		}
		break;
	case SYS_MODE_INIT:	break;
	default: break;
	}
#else 
	for(int i=0; i<USED_ACQ196; i++) 
	{
		iCAStatus=ca_Put_QStr( DDS1_BX_CARD_MODE[i], QString( "%1").arg( CARD_MODE_SINGLE ) );
		if( iCAStatus  != ECA_NORMAL )
			pWnd->printStatus("ERROR! DDS1A_B%d_CardMode(%d), %s\n", i+1, CARD_MODE_SINGLE,  ca_message( iCAStatus) );
	}

#endif

		
}


void dds1ConnectionCallback(struct connection_handler_args args)
{
	chid	chid = args.chid;
	long 	op = args.op;

	if( chid == DDS1A_OpMode) {
		if( op == CA_OP_CONN_UP )
			pWnd->printStatus("Connected ... DDS1A\n");
		else pWnd->printStatus("Disconnected ... DDS1A\n");
	}
	if( chid == DDS1B_OpMode) {
		if( op == CA_OP_CONN_UP )
			pWnd->printStatus("Connected ... DDS1B\n");
		else pWnd->printStatus("Disconnected ... DDS1B\n");
	}
	for( int i=0; i< USED_ACQ196; i++) {
		if( chid == DDS1A_tag[i][0] ) {
			if( op == CA_OP_CONN_UP )
				pWnd->printStatus("Connected ... ACQ196 #%d\n", i);
			else 
				pWnd->printStatus("Disconnected ... ACQ196 #%d\n", i);
			
			break;
		}
	}
}


void pageDDS1::timerFuncTwoSec()
{
//	char strval[64];
//	int iCAStatus;
//	static int nVal;
//	double dval;


	pWnd->btn_dds1LocalRun->setEnabled(true);

	timer_stop();


#if 0
	
	if( pWnd->g_opMode != SYS_MODE_REMOTE ) return;

/*	if( touch_T1_PV[B1] ) { */

		if( (iCAStatus = ca_Get_StrVal(  DDS1_setT1[B1], strval) ) != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS1_setT1[B1]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		pWnd->Edit_ACQ196_1_T1->setText( strval  );
#if USE_SYNC_SLAVE_TO_MASTER
		pWnd->Edit_ACQ196_2_T1->setText( strval  );
		pWnd->Edit_ACQ196_3_T1->setText( strval  );
		pWnd->Edit_ACQ196_4_T1->setText( strval  );
#endif

/*		touch_T1_PV[B1] = 0;
	} */
		
/******************************	2009. 11. 3 ***************************************/
	if( (iCAStatus = ca_Get_StrVal(  DDS1_setT0[B1], strval) ) != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! >> DDS1_setT0[B1]: %s\n",  ca_message( iCAStatus) );
		return;
	}
	pWnd->Edit_ACQ196_1_T0->setText( strval  );
	pWnd->Edit_ACQ196_2_T0->setText( strval  );
	pWnd->Edit_ACQ196_3_T0->setText( strval  );
	pWnd->Edit_ACQ196_4_T0->setText( strval  );


	if( (iCAStatus = ca_Get_StrVal(  DDS1_sampleRate[B1], strval) ) != ECA_NORMAL ) {
		pWnd->printStatus("ERROR! >> DDS1_sampleRate[B1]: %s\n",  ca_message( iCAStatus) );
		return;
	}
	int index = pWnd->cb_dds1SampleClk->findData( strval, Qt::MatchExactly);
	if( index != -1 ) {
		pWnd->cb_dds1SampleClk->setCurrentIndex(index );
		pWnd->cb_dds1SampleClk_2->setCurrentIndex(index );
		pWnd->cb_dds1SampleClk_3->setCurrentIndex(index );
		pWnd->cb_dds1SampleClk_4->setCurrentIndex(index );
	}
/****************************** 2009. 11. 3 ***************************************/



#if !USE_MASTER_SLAVE
	if( touch_T1_PV[B2] ) {	
		if( (iCAStatus = ca_Get_StrVal(  DDS1_setT1[B2], strval) ) != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS1_setT1[B2]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		pWnd->Edit_ACQ196_2_T1->setText( strval );

		touch_T1_PV[B2] = 0;
	}

	if( touch_T1_PV[B3] ) {	
		if( (iCAStatus = ca_Get_StrVal(  DDS1_setT1[B3], strval) ) != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS1_setT1[B3]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		pWnd->Edit_ACQ196_3_T1->setText( strval );

		touch_T1_PV[B3] = 0;
	}

	if( touch_T1_PV[B4] ) {
		if( (iCAStatus = ca_Get_StrVal(  DDS1_setT1[B4], strval) ) != ECA_NORMAL ) {
			pWnd->printStatus("ERROR! >> DDS1_setT1[B4]: %s\n",  ca_message( iCAStatus) );
			return;
		}
		pWnd->Edit_ACQ196_4_T1->setText( strval );

		touch_T1_PV[B4] = 0;
	}
#endif
/*
	if( !nEnable_to_RUN ) {   // 2009. 10. 13.   
		if( nVal ) {
			pWnd->label_ccs_block->setPalette(pWnd->pal_warning);
			nVal = 0;
		} else {
			pWnd->label_ccs_block->setPalette(pWnd->pal_normal);
			nVal = 1;
		}
	}
*/
#endif

}

void pageDDS1::monitor_dds1PV_start()
{
#if 0
	for(int i=0; i<USED_ACQ196; i++) {
		pWnd->cbFunc.startMonitoringPV( DDS1_getState[i], 0 );
		pWnd->cbFunc.startMonitoringPV( DDS1_BX_PARSING_CNT[i], 0);
		pWnd->cbFunc.startMonitoringPV( DDS1_BX_MDS_SND_CNT[i], 0 );
		pWnd->cbFunc.startMonitoringPV( DDS1_BX_DAQING_CNT[i], 0 );
	}
#endif
#if 0
	for(int i=0; i<USED_ACQ196; i++) {
		cbFunc.startMonitoringPV( DDS1_getState[i], 0 );
		cbFunc.startMonitoringPV( DDS1_BX_PARSING_CNT[i], 0);
		cbFunc.startMonitoringPV( DDS1_BX_MDS_SND_CNT[i], 0 );
		cbFunc.startMonitoringPV( DDS1_BX_DAQING_CNT[i], 0 );
	}
#endif
}

void pageDDS1::monitor_LTU_start()
{
#if 0
	for(int i=0; i<4; i++){
//		pWnd->cbFunc.startMonitoringPV( pg_dds1.pvDDS1ALTU.TRIG[i], 1);
		pWnd->cbFunc.startMonitoringPV( pvDDS1ALTU.CLOCK[i], 1);
		pWnd->cbFunc.startMonitoringPV( pvDDS1ALTU.T0[i], 1);
		pWnd->cbFunc.startMonitoringPV( pvDDS1ALTU.T1[i], 1);
	}
#endif

#if 0	
#if USE_MASTER_SLAVE
/*  Notify !!     Notify !!     Notify !!  ********************************/
/* we use just one CLK/TRG channel as a master mode 		2009. 09. 01 */
	cbFunc.startMonitoringPV( pvDDS1ALTU.CLOCK[0], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.T0[0], 1);
	cbFunc.startMonitoringPV( pvDDS1ALTU.T1[0], 1);
#else 

	for(int i=0; i<4; i++){
		cbFunc.startMonitoringPV( pvDDS1ALTU.CLOCK[i], 1);
		cbFunc.startMonitoringPV( pvDDS1ALTU.T0[i], 1);
		cbFunc.startMonitoringPV( pvDDS1ALTU.T1[i], 1);
	}

#endif
#endif

}

void pageDDS1::monitor_stop()
{
//	pWnd->cbFunc.stopMonitoringPV();
	cbFunc.stopMonitoringPV();
}

void pageDDS1::init_DDS1_GUI()
{
//	pWnd->label_dds1Opmode->setText( "Select Mode" ); // 2009. 11. 05
/*
	pWnd->label_dds1DAQstate->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_2->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_3->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_4->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_5->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_6->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_7->setText("Not Initialized.");
	pWnd->label_dds1DAQstate_8->setText("Not Initialized.");

	pWnd->label_dev1_cnt->setText(QString("0") );	// 2009. 08. 04
	pWnd->label_dev2_cnt->setText(QString("0") );
	pWnd->label_dev3_cnt->setText(QString("0") );
	pWnd->label_dev4_cnt->setText(QString("0") );
	pWnd->label_dev5_cnt->setText(QString("0") );
	pWnd->label_dev6_cnt->setText(QString("0") );
	pWnd->label_dev7_cnt->setText(QString("0") );
	pWnd->label_dev8_cnt->setText(QString("0") );
*/
	// 2009. 10. 13. 
//	pWnd->label_ccs_block->setText(QString("Run Condition") );
//	pWnd->label_ccs_block->setPalette(pWnd->pal_normal);
		
/******************************************************/
/****************************************** 2009. 09.07 */
#if 1
	pWnd->btn_B1_Local_DAQ->setEnabled(false);
	pWnd->btn_B1_Local_Save->setEnabled(false);
	pWnd->btn_B1_Local_DAQ_2->setEnabled(false);
	pWnd->btn_B1_Local_Save_2->setEnabled(false);
	pWnd->btn_B1_Local_DAQ_3->setEnabled(false);
	pWnd->btn_B1_Local_Save_3->setEnabled(false);
	pWnd->btn_B1_Local_DAQ_4->setEnabled(false);
	pWnd->btn_B1_Local_Save_4->setEnabled(false);
	pWnd->btn_B1_Local_DAQ_5->setEnabled(false);
	pWnd->btn_B1_Local_Save_5->setEnabled(false);
	pWnd->btn_B1_Local_DAQ_6->setEnabled(false);
	pWnd->btn_B1_Local_Save_6->setEnabled(false);
#endif
/****************************************** 2009. 09.07 */		


	pWnd->gb_dds1Opmode->setEnabled(true);
	pWnd->gb_dds1digisetup->setEnabled(false);
	pWnd->gb_dds1remote->setEnabled(false);
	pWnd->gb_dds1calibration->setEnabled(false);
	pWnd->gb_dds1Local->setEnabled(false);


	pWnd->btn_dds1OPremote->setChecked(false);
	pWnd->btn_dds1OPcalibration->setChecked(false);
	pWnd->btn_dds1OPlocal->setChecked(false);
	
	pWnd->btn_dds1ResetMode->setEnabled(false);
	
/*	tabWidget_acq196->setEnabled(false); */


}

void pageDDS1::set_DDS1_GUI_OPMODE()
{
	switch( pWnd->g_opMode ) {
	case SYS_MODE_REMOTE:
		pWnd->gb_dds1remote->setEnabled(true);
		pWnd->gb_dds1calibration->setEnabled(false);
		pWnd->gb_dds1Local->setEnabled(false);
		break;
	case SYS_MODE_CALIB:
		pWnd->gb_dds1remote->setEnabled(false);
		pWnd->gb_dds1calibration->setEnabled(true);
		pWnd->gb_dds1Local->setEnabled(false);
		break;
	case SYS_MODE_LOCAL:
		pWnd->gb_dds1remote->setEnabled(false);
		pWnd->gb_dds1calibration->setEnabled(false);
		pWnd->gb_dds1Local->setEnabled(true);
		break;
	default: 
		pWnd->printStatus("ERROR! DDS1A OP mode wrong! [%d]", pWnd->g_opMode );
		return;
	}
	
	pWnd->gb_dds1Opmode->setEnabled(false);
	pWnd->btn_dds1ResetMode->setEnabled(true); // 2009-12-03
	pWnd->gb_dds1digisetup->setEnabled(true);

	pWnd->tabWidget_acq196->setEnabled(true);	
	
}





