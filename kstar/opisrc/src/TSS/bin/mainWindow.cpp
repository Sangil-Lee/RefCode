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
#include <sys/time.h>


#include <QtGui>

#include "mainWindow.h"

#include "unistd.h"



/*#include "systemState.h" */

//#include "page_main.h"
//#include "page_testCal.h"
//#include "page_timingSet.h"

//QString *ApQStringMonitors;
//static void vFMonitorCallback( struct event_handler_args args);

/*
CltuMain pg_main;
CltuTimingSet pg_timeset;
LocalTestR1 pg_localR1;
*/

//int opi_debug;
//int nUsedSystemCnt;

void connectionCB_LTU(struct connection_handler_args args);

void connectionCB_DDS(struct connection_handler_args args);
void connectionCB_Init(struct connection_handler_args args);

//callbackData cData;
//static unsigned int status_line_cnt;
//int begin_printStatus;


MainWindow *pparent;

QStringList strList_printStatus;
QStringList strList_printReturn;


void MainWindow::assign_system_name()
{

	strcpy(str_sysName[ BOX_LTU],    			"BOX");	
	ltu_feature[BOX_LTU][FEAT_VER] = 			VER2;
	ltu_feature[BOX_LTU][FEAT_CLASS] = 			C_MCH;

	strcpy(str_sysName[ CES_LTU],    			"CES");	
	ltu_feature[CES_LTU][FEAT_VER] = 			VER2;
	ltu_feature[CES_LTU][FEAT_CLASS] = 			C_DIAG;
	
	strcpy(str_sysName[ CTUV1],    			"CTUV1");	
	ltu_feature[CTUV1][FEAT_VER] = 			VER1;
	ltu_feature[CTUV1][FEAT_CLASS] = 			C_MCH;
	
	strcpy(str_sysName[ DDS1A_LTU],  		"DDS1A");
	ltu_feature[DDS1A_LTU][FEAT_VER] =			VER2;
	ltu_feature[DDS1A_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ DDS1B_LTU], 		"DDS1B");
	ltu_feature[DDS1B_LTU][FEAT_VER] = 		VER2;
	ltu_feature[DDS1B_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ ECCD1_LTU],  		"EC1A");
	ltu_feature[ECCD1_LTU][FEAT_VER] = 		VER2;
	ltu_feature[ECCD1_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ ECCD1B_LTU],  		"EC1B");
	ltu_feature[ECCD1B_LTU][FEAT_VER] = 		VER2;
	ltu_feature[ECCD1B_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ ECEHR_LTU],  		"ECEHR");
	ltu_feature[ECEHR_LTU][FEAT_VER] = 		VER2;
	ltu_feature[ECEHR_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ ECEI_LTU],   		"ECEI");
	ltu_feature[ECEI_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ECEI_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ ECH_LTU],    		"ECH");
	ltu_feature[ECH_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ECH_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ ER_LTU],     			"ER");
	ltu_feature[ER_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ER_LTU][FEAT_CLASS] = 			C_DIAG;
	
	strcpy(str_sysName[ VSC_LTU],   			"VSC");
	ltu_feature[VSC_LTU][FEAT_VER] = 			VER1;
	ltu_feature[VSC_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ FILD_LTU],    		"FILD");
	ltu_feature[FILD_LTU][FEAT_VER] = 			VER2;
	ltu_feature[FILD_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ FUEL_LTU],   		"FUEL");
	ltu_feature[FUEL_LTU][FEAT_VER] = 			VER2;
	ltu_feature[FUEL_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ FUEL2_LTU],          "FUEL2");
	ltu_feature[FUEL2_LTU][FEAT_VER] =           VER2;
	ltu_feature[FUEL2_LTU][FEAT_CLASS] =         C_MCH;

	strcpy(str_sysName[ HALPHA_LTU],   		"HALPHA");
	ltu_feature[HALPHA_LTU][FEAT_VER] = 		VER1;
	ltu_feature[HALPHA_LTU][FEAT_CLASS] = 		C_DIAG;

	
	strcpy(str_sysName[ HXR_LTU],    		"HXR");
	ltu_feature[HXR_LTU][FEAT_VER] = 			VER1;
	ltu_feature[HXR_LTU][FEAT_CLASS] = 		C_DIAG;

	strcpy(str_sysName[ HXR2_LTU],    		"HXR2");
	ltu_feature[HXR2_LTU][FEAT_VER] = 			VER2;
	ltu_feature[HXR2_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ ICRH_LTU],   		"ICRH");
	ltu_feature[ICRH_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ICRH_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ IRVB_LTU],   		"IRVB");
	ltu_feature[IRVB_LTU][FEAT_VER] = 			VER2;
	ltu_feature[IRVB_LTU][FEAT_CLASS] = 		C_DIAG;
	
/*
	strcpy(str_sysName[ ITER_FUEL_LTU],   		"ITERFUEL");
	ltu_feature[ITER_FUEL_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ITER_FUEL_LTU][FEAT_CLASS] = 		C_MCH;
*/
	strcpy(str_sysName[ ITER_PCS_LTU], 		"ITERPCS");
	ltu_feature[ITER_PCS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[ITER_PCS_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ LHCD_LTU],   		"LHCD");
	ltu_feature[LHCD_LTU][FEAT_VER] = 			VER2;
	ltu_feature[LHCD_LTU][FEAT_CLASS] = 		C_MCH;
/*
	strcpy(str_sysName[ MC_LTU],   			"MC");
	ltu_feature[MC_LTU][FEAT_VER] = 			VER2;
	ltu_feature[MC_LTU][FEAT_CLASS] = 			C_DIAG;
*/
	strcpy(str_sysName[ MMWI_LTU],   		"MMWI");
	ltu_feature[MMWI_LTU][FEAT_VER] = 			VER2;
	ltu_feature[MMWI_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ MPS1_LTU],   		"MPS1");
	ltu_feature[MPS1_LTU][FEAT_VER] = 			VER2;
	ltu_feature[MPS1_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ MPS2_LTU],   		"MPS2");
	ltu_feature[MPS2_LTU][FEAT_VER] = 			VER2;
	ltu_feature[MPS2_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ MPS3_LTU],   		"MPS3");
	ltu_feature[MPS3_LTU][FEAT_VER] = 			VER2;
	ltu_feature[MPS3_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ NBI1A_LTU],  		"NBI1A");
	ltu_feature[NBI1A_LTU][FEAT_VER] = 			VER2;
	ltu_feature[NBI1A_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ NBI1B_LTU],  		"NBI1B");
	ltu_feature[NBI1B_LTU][FEAT_VER] = 			VER2;
	ltu_feature[NBI1B_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ NBI1C_LTU],  		"NBI1C");
	ltu_feature[NBI1C_LTU][FEAT_VER] = 			VER2;
	ltu_feature[NBI1C_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ NBI1D_LTU],  		"NBI1D");
	ltu_feature[NBI1D_LTU][FEAT_VER] = 			VER2;
	ltu_feature[NBI1D_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ NMS_LTU],  			"NMS");
	ltu_feature[NMS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[NMS_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ PCS_LTU],    		"PCS");
	ltu_feature[PCS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[PCS_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ QDS_LTU],    		"QDS");
	ltu_feature[QDS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[QDS_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ RBA_LTU],    		"RBA");
	ltu_feature[RBA_LTU][FEAT_VER] = 			VER2;
	ltu_feature[RBA_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ RMP_LTU],    		"RMP");
	ltu_feature[RMP_LTU][FEAT_VER] = 			VER2; 
	ltu_feature[RMP_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ SMBI_LTU],    		"SMBI");
	ltu_feature[SMBI_LTU][FEAT_VER] = 			VER1;
	ltu_feature[SMBI_LTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ SXR_LTU],    		"SXR");
	ltu_feature[SXR_LTU][FEAT_VER] = 			VER2;
	ltu_feature[SXR_LTU][FEAT_CLASS] = 		C_DIAG;
	
	strcpy(str_sysName[ TS_LTU],     			"TS");
	ltu_feature[TS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[TS_LTU][FEAT_CLASS] = 			C_DIAG;
	
	strcpy(str_sysName[ TSS_CTU],    		"TSS");
	ltu_feature[TSS_CTU][FEAT_VER] = 			VER2;
	ltu_feature[TSS_CTU][FEAT_CLASS] = 		C_MCH;

	strcpy(str_sysName[ VBS_LTU],    		"VBS");
	ltu_feature[VBS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[VBS_LTU][FEAT_CLASS] = 		C_DIAG;

	strcpy(str_sysName[ VMS_LTU],    		"VMS");
	ltu_feature[VMS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[VMS_LTU][FEAT_CLASS] = 		C_MCH;
	
	strcpy(str_sysName[ XCS_LTU],    		"XCS");
	ltu_feature[XCS_LTU][FEAT_VER] = 			VER2;
	ltu_feature[XCS_LTU][FEAT_CLASS] = 		C_DIAG;
	

}

bool MainWindow::Is_New_R2_id(int ID)
{
/*	if( ID == TSS_CTU ||
		ID == ATCA_LTU ||
		ID == RMP_LTU )
		return TRUE;
*/
	if( ltu_feature[ID][FEAT_VER] == VER2 ) return TRUE;
	
	return FALSE;
}
bool MainWindow::Is_New_R2_name(const char *name)
{
	int sysID = get_system_ID(name);
	if( sysID < 0 )  return FALSE;
	return Is_New_R2_id(sysID);
}
int MainWindow::Is_seq_save()
{
	int val = 0;
	if( ((int)passwd_timeset_check == 0 ) && /* touch */
		((int)passwd_master_check == 1 ))   /* not touch*/
		val  = 1;
	return val;
}
int MainWindow::get_system_ID(const char *str_system)
{
	int ID = -1;
	/* At first time print this line twice. cause check  DDS1, HALPHA frequecy        */
	/* call by "void CltuTimingSet::idchanged_cb_timing_system(QString qstr)"   */
	for(int i=0; i<MAX_LTU_CARD; i++) {
		if( !strcmp(str_sysName[i],  str_system ) ) {
			ID=i;
			break;
		}
	} 
	if( ID < 0 ) printStatus(LEVEL_NONE,"No mached system with \"%s\" \n", str_system );
	
	return ID;
}



MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{

	setupUi(this);

	setWindowTitle(tr("TSS OPI"));
/*	printf("##################################\n");
	printf("TSS OPI \n");
	printf("R 1.3  2009. 10. 29 \n");
	printf("use low frequency, sync to master for DDS1 \n\n");
*/
/*	
	if( getenv("OPI_DEBUG") ){
		opi_debug = atoi(getenv("OPI_DEBUG") );
		epicsPrintf("OPI_DEBUG : %d\n", opi_debug);
	} else opi_debug = 0;
*/
/*	printf("export MALLOC_CHECK_=1\n"); */
	
	assign_system_name();


#if 1	
	int iCAStatus = ca_context_create(ca_enable_preemptive_callback); // ca_disable_preemptive_callback, ca_enable_preemptive_callback
	if (iCAStatus != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying to start channel access.\n", ca_message(iCAStatus) );
		return;
	}
#endif

	qthread = new CqueueThread( this);	
	connect( qthread, SIGNAL(finished() ), qthread, SLOT(deleteLater()) );
//	qthread->start( QThread::TimeCriticalPriority );
	qthread->start( QThread::NormalPriority );

	


	pparent = (MainWindow *)this;
	status_line_cnt = 1;
	passwd_master_check = 1;
	passwd_timeset_check = 1;


//	QBrush brush_select(QColor(42, 60, 176, 255));
	QBrush brush_select(QColor(65, 197, 0, 255));
	QBrush brush_normal(QColor(63, 63, 95, 255));
	brush_select.setStyle(Qt::SolidPattern);
	brush_normal.setStyle(Qt::SolidPattern);

	pal_select.setBrush(QPalette::Active, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Active, QPalette::ButtonText, QBrush::QBrush(QColor(0,0,0,255)) );
	pal_select.setBrush(QPalette::Active, QPalette::Window, brush_select);
	pal_select.setBrush(QPalette::Inactive, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Inactive, QPalette::Window, brush_select);
	pal_select.setBrush(QPalette::Disabled, QPalette::Button, brush_select);
	pal_select.setBrush(QPalette::Disabled, QPalette::Window, brush_select);

	pal_normal.setBrush(QPalette::Active, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Active, QPalette::Window, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Window, brush_normal);
	pal_normal.setBrush(QPalette::Disabled, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Disabled, QPalette::Window, brush_normal);


	brush_error = QBrush ( QColor(223, 0, 0), Qt::SolidPattern ); // red
	brush_notice = QBrush ( QColor(170, 255, 127), Qt::SolidPattern ); // light green
	brush_warning = QBrush ( QColor(255, 255, 0), Qt::SolidPattern ); // yellow


	AttachChannelAccess *pattach1 = new AttachChannelAccess(frm_main_lst);
	AttachChannelAccess *pattach2 = new AttachChannelAccess(frame_infobar);
//	AttachChannelAccess *pattach3 = new AttachChannelAccess(frm_main_shottime); // remove title frame.   2010.08.17 woong 
	AttachChannelAccess *pattach4 = new AttachChannelAccess(frame_blip_envelope);
	AttachChannelAccess *pattach5 = new AttachChannelAccess(frame_CTU_monitor);

	

	/*  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  */
	/* AttachChannelAccess ........ user can attach another UI files into stacked widget.... in this function */
	insert_UI_fileToWidget();
	/*  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  NOTICE . . . .  */
	

	createActions();	// Menu 
	createToolbars();
	createStatusBar();
	


	/**********************************************/
	/**********************************************/


	connect(btn_main, SIGNAL(clicked()), this, SLOT( clicked_btn_main()  ));
//	connect(btn_DB_manager, SIGNAL(clicked()), this, SLOT(clicked_btn_DB_manager()  ));
	connect(btn_timingSet, SIGNAL(clicked()), this, SLOT(clicked_btn_timingSet()  ));
	connect(btn_local_R1, SIGNAL(clicked()), this, SLOT(clicked_btn_local_R1()  ));
	connect(btn_local_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_local_R2()  ));
	connect(btn_local_R2_many_trig, SIGNAL(clicked()), this, SLOT(clicked_btn_local_R2_many_trig()  ));
	connect(btn_sql_db, SIGNAL(clicked()), this, SLOT(clicked_btn_sql_db()  ));


	connect(btn_start_monitoring, SIGNAL(clicked()), this, SLOT(clicked_btn_start_monitoring() ));
	connect(btn_stop_monitoring, SIGNAL(clicked()), this, SLOT(clicked_btn_stop_monitoring() ));

	connect(this, SIGNAL(signal_printStatus(int, char *)), this, SLOT( slot_printStatus( int, char *)  ));
	connect(this, SIGNAL(signal_printReturn(int, char *)), this, SLOT( slot_printReturn( int, char *)  ));
	connect(this, SIGNAL(signal_saveLTUparam() ), this, SLOT( slot_saveLTUparam( )  ));
	connect(this, SIGNAL(signal_update_spinBox() ), this, SLOT( slot_update_spinBox( )  ));

/**********************************************/
/**********************************************/

	
	connect(btn_timing_insert, SIGNAL(clicked()), this, SLOT(clicked_btn_timing_insert( )  ));
//	connect(cb_timing_port, SIGNAL(currentIndexChanged(int)), this, SLOT(idchanged_cb_timing_port(int)  ));
//	connect(cb_timing_system, SIGNAL(currentIndexChanged(QString)), this, SLOT(idchanged_cb_timing_system(QString)  ));
	connect(tw_machine, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged_tw_timingSet(QTableWidgetItem *)  ));
	connect(tw_diagnostics, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged_tw_timingSet(QTableWidgetItem *)  ));
	connect(tw_ltu2, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged_tw_timingSet(QTableWidgetItem *)  ));
	connect(tw_ltu2_diag, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged_tw_timingSet(QTableWidgetItem *)  ));
	connect(btn_final_setup, SIGNAL(clicked()), this, SLOT(clicked_btn_bliptime_final_setup( )  ));
	connect(btn_flush_LTUparam, SIGNAL(clicked()), this, SLOT(clicked_btn_flush_LTUparam( )  ));
	connect(tw_machine, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked_tw_load_info(int, int)) );
	connect(tw_diagnostics, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked_tw_load_info(int, int)) );
	connect(tw_ltu2, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked_tw_load_info(int, int)) );
	connect(tw_ltu2_diag, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked_tw_load_info(int, int)) );
	connect(spinBox_stored_shot, SIGNAL(valueChanged(int)), this, SLOT(valueChanged_sb_stored_shot(int)) );
	connect(btn_single_load, SIGNAL(clicked()), this, SLOT(clicked_btn_single_load( )  ));
	connect(btn_reload, SIGNAL(clicked()), this, SLOT(clicked_btn_reload( )  ));

/**********************************************/
/**********************************************/

	connect(btn_test_set, SIGNAL(clicked()), this, SLOT(clicked_btn_test_set( )  ));
	connect(btn_test_shotStart, SIGNAL(clicked()), this, SLOT(clicked_btn_test_shotStart( )  ));
//	connect(btn_cal_calStart, SIGNAL(clicked()), this, SLOT(clicked_btn_cal_calStart( )  ));


	connect(btn_showinfo_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_showinfo_R2( )  ));
	connect(btn_showTime_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_showTime_R2( )  ));
	connect(btn_showTLK_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_showTLK_R2( )  ));
	connect(btn_showLog_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_showLog_R2( )  ));
	connect(btn_test_set_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_test_setup_R2( )  ));
	connect(btn_test_shotStart_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_test_shotStart_R2( )  ));
	connect(btn_test_shotStop_R2, SIGNAL(clicked()), this, SLOT(clicked_btn_test_shotStop_R2( )  ));	
	connect(btn_R2_setMode, SIGNAL(clicked()), this, SLOT(clicked_btn_R2_setMode( )  ));
	connect(btn_R2_insert_item, SIGNAL(clicked()), this, SLOT(clicked_btn_R2_insert_item( )  ));
	connect(tw_ltu2_local, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged_tw_local_R2(QTableWidgetItem *)  ));
	connect(btn_R2_open, SIGNAL(clicked()), this, SLOT(clicked_btn_R2_open( )  ));
	connect(btn_R2_save, SIGNAL(clicked()), this, SLOT(clicked_btn_R2_save( )  ));


	connect(check_calib_admin, SIGNAL(stateChanged(int)), this, SLOT(stateChanged_check_calib_admin(int) ));
	connect(check_TSS_cal_mode, SIGNAL(stateChanged(int)), this, SLOT(stateChanged_check_TSS_cal_mode(int) ));
	connect(btn_put_LTU_calib_value, SIGNAL(clicked()), this, SLOT(clicked_btn_put_LTU_calib_value() ));

/*************************************************************/
/* many trigger panel */
	connect(btn_mTrig_group_setup, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_group_setup( )  ));
	connect(btn_mTrig_pv_setup, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_pv_setup()  ));
	connect(btn_mTrig_list_clear, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_list_clear()  ));
	connect(btn_mTrig_Connect, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_Connect()  ));
	connect(btn_mTrig_Disconnect, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_Disconnect()  ));
	connect(btn_mTrig_start, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_start()  ));
	connect(btn_mTrig_stop, SIGNAL(clicked()), this, SLOT(clicked_btn_mTrig_stop()  ));




//	timer_listWidget = new QTimer(this);
//	connect(timer_listWidget, SIGNAL(timeout()), this, SLOT(func_timer_listWidget()));
//	timer_listWidget->start(2000);

	pg_main.InitWindow(this);
	pg_timeset.InitWindow(this);
	pg_localR1.InitWindow(this);
	pg_localR2.InitWindow(this);
	pg_manyTrig.InitWindow(this);
	pg_SQL.InitWindow(this);

	initOPIstate();

//	btn_start_monitoring->setEnabled(true);
//	btn_stop_monitoring->setEnabled(false);


	pv_copy_names_LTUs();
	pv_copy_names_others();

	timerConnect = new QTimer(this);
	connect(timerConnect, SIGNAL(timeout()), this, SLOT(timerFunc_Connect()));
	timerConnect->start(1000);

//	pv_connect_LTU();		// move to timerFunc_Connect()  ......  2009.08.31


}

MainWindow::~MainWindow()
{
#if 0
	pv_disconnect();
	printf("pv_disconnect() ... OK\n");
	ca_context_destroy( );
	printf("ca_context_destroy() ... OK\n");

	printf("TSS terminated!\n");
#endif

}

void MainWindow::closeEvent(QCloseEvent *event)
{
//	if( setupWnd )
//		delete setupWnd;

//	printf("i'm closed\n");
	saveConfigFile();
//	stopMonitorPVs();

	pg_main.destroyPageMain();
	pg_timeset.destroyPageTimingSet();
	pg_localR1.destroyPageTestcal();
	pg_localR2.destroyPageTestcal_R2();

	pv_monitoring_stop();



	printf("pv_disconnect() ... ");
	pv_disconnect();
	printf("OK\n");
	printf("ca_context_destroy() ... ");
	ca_context_destroy( );
	printf("OK\n");

	printf("TSS terminated!\n");

//	if( myWindow )
//		myWindow->close();
	event->accept();

	printf("call kill()...\n");
	kill( getpid(), SIGTERM);
	printf("return kill()...\n");

}

void MainWindow::insert_UI_fileToWidget()
{
//	AttachChannelAccess *pattachMain = new AttachChannelAccess(page_main);  // 2009. 11. 13


}

void MainWindow::clicked_btn_main()
{
	stackedWidget_main->setCurrentIndex(STACKED_ID_MAIN);
	pg_main.updateTableWidget();
	
	set_unselect_btn();
	btn_main->setPalette(pal_select);

	pg_localR2.Init_OPI_widget();
}
/*
void MainWindow::clicked_btn_DB_manager()
{
	if( passwd_timeset_check && (checkPasswd() == WR_ERROR) ){
		return;
	}

	stackedWidget_main->setCurrentIndex(STACKED_ID_DB_MAN);

	set_unselect_btn();
	btn_DB_manager->setPalette(pal_select);

	pg_localR2.Init_OPI_widget();
	
}
*/
void MainWindow::clicked_btn_timingSet()
{
	if( passwd_timeset_check && (checkPasswd() == WR_ERROR) ){
		return;
	}

	stackedWidget_main->setCurrentIndex(STACKED_ID_TSS);

	set_unselect_btn();
	btn_timingSet->setPalette(pal_select);

	pg_localR2.Init_OPI_widget();
	
}
void MainWindow::clicked_btn_local_R1()
{
	stackedWidget_main->setCurrentIndex(STACKED_ID_R1);
// we don't need default setting for TimingBoard.
//	pg_localR1.setup_all_port(-1);

	set_unselect_btn();
	btn_local_R1->setPalette(pal_select);

	pg_localR2.Init_OPI_widget();
}

void MainWindow::clicked_btn_local_R2()
{
	stackedWidget_main->setCurrentIndex(STACKED_ID_R2);
// we don't need default setting for TimingBoard.
//	pg_localR1.setup_all_port(-1);

	set_unselect_btn();
	btn_local_R2->setPalette(pal_select);

	pg_localR2.Init_OPI_widget();
}
void MainWindow::clicked_btn_local_R2_many_trig()
{
	stackedWidget_main->setCurrentIndex(STACKED_ID_MTRIG);
// we don't need default setting for TimingBoard.
//	pg_localR1.setup_all_port(-1);

	set_unselect_btn();
	btn_local_R2_many_trig->setPalette(pal_select);

//	pg_localR2.Init_OPI_widget();
}
void MainWindow::clicked_btn_sql_db()
{
	stackedWidget_main->setCurrentIndex(STACKED_ID_SQL);
// we don't need default setting for TimingBoard.
//	pg_localR1.setup_all_port(-1);

	set_unselect_btn();
	btn_sql_db->setPalette(pal_select);

	pg_SQL.Init_OPI_widget();
}

void MainWindow::set_unselect_btn()
{
	btn_main->setPalette(pal_normal);
//	btn_DB_manager->setPalette(pal_normal);
	btn_timingSet->setPalette(pal_normal);
	btn_local_R1->setPalette(pal_normal);
	btn_local_R2->setPalette(pal_normal);
	btn_local_R2_many_trig->setPalette(pal_normal);
	btn_sql_db->setPalette(pal_normal);
}

void MainWindow::createActions()
{
	actionNew->setIcon(QIcon(":/images/CLTU_new.png") );
	actionNew->setShortcut(tr("Ctrl+N"));
	actionNew->setStatusTip(tr("Create a new set"));
	connect(actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));

	actionOpen->setIcon(QIcon(":/images/CLTU_open.png") );
	actionOpen->setShortcut(tr("Ctrl+O"));
	actionOpen->setStatusTip(tr("Open an existing file"));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

	actionSave->setIcon(QIcon(":/images/CLTU_save.png") );
	actionSave->setShortcut(tr("Ctrl+S"));
	actionSave->setStatusTip(tr("Save current file"));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));

//	actionSave_As->setStatusTip(tr("Save current file as ..."));
//	connect(actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

	actionPrint->setIcon(QIcon(":/images/CLTU_print.png") );
	actionPrint->setShortcut(tr("Ctrl+P"));
	actionPrint->setStatusTip(tr("Print the current data"));
	connect(actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

	actionExit->setShortcut(tr("Ctrl+Q"));
	actionExit->setStatusTip(tr("Exit the application"));
	connect(actionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));


	actionPrint->setStatusTip(tr("All LTUs set to zero"));
	connect(actionAll_LTUs_reset, SIGNAL(triggered()), this, SLOT(action_All_LTUs_reset()));

	actionPrint->setStatusTip(tr("All List set to zero"));
	connect(actionAll_List_set_to_Zero, SIGNAL(triggered()), this, SLOT(action_All_List_zero()));

	actionPrint->setStatusTip(tr("Save to MySQL"));
	connect(actionSave_this_shot, SIGNAL(triggered()), this, SLOT(action_Save_to_MySQL()));

	actionPrint->setStatusTip(tr("Delete shot in MySQL"));
	connect(actionDelete_this_shot, SIGNAL(triggered()), this, SLOT(action_Delete_shot_MySQL()));


}
//IOC Running Status 
void MainWindow::createToolbars()
{
	char buf[64];
	
	fileToolbar->addAction(actionNew);
	fileToolbar->addAction(actionOpen);
	fileToolbar->addAction(actionSave);
	fileToolbar->addAction(actionPrint);


	
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
	QSpacerItem *tbspacer3 = new QSpacerItem(15, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	CAGraphic *iocCTUV1 = new CAGraphic();
	iocCTUV1->setLineWidth(2);
	iocCTUV1->setMinimumSize(QSize(20,20));
	iocCTUV1->setMaximumSize(QSize(20,20));
	iocCTUV1->setFillColor(QColor("yellow"));
	iocCTUV1->setFillMode(StaticGraphic::Solid);
	iocCTUV1->setLineColor(QColor("black"));
	sprintf(buf, "%s_CTU_HeartBeat", str_sysName[CTUV1] );
	iocCTUV1->setPvname(buf);	
	iocCTUV1->setFillDisplayMode(CAGraphic::ActInact);
	iocCTUV1->setObjectName("CAGraphic_iocCTUV1");

	CAGraphic *iocCTUV2 = new CAGraphic();
	iocCTUV2->setLineWidth(2);
	iocCTUV2->setMinimumSize(QSize(20,20));
	iocCTUV2->setMaximumSize(QSize(20,20));
	iocCTUV2->setFillColor(QColor("yellow"));
	iocCTUV2->setFillMode(StaticGraphic::Solid);
	iocCTUV2->setLineColor(QColor("black"));
	sprintf(buf, "%s_CTU_HeartBeat", str_sysName[TSS_CTU] );
	iocCTUV2->setPvname(buf);	
	iocCTUV2->setFillDisplayMode(CAGraphic::ActInact);
	iocCTUV2->setObjectName("CAGraphic_iocCTUV2");
	

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock();
	wclock1->setMinimumSize(QSize(180,20));
	wclock1->setMaximumSize(QSize(180,20));
//	wclock1->setPvname("TMS_IOC1_WCLOCK.RVAL");
	wclock1->setPvname("CCS_SYS_TIME.RVAL");
	wclock1->setFont(clockfont);
	wclock1->setObjectName("CAWclock_wclock1");

    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":/images/CLTU_kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);
	tblayout->addWidget(iocCTUV1);
	tblayout->addWidget(iocCTUV2);

	tblayout->addItem(tbspacer2);
	tblayout->addWidget(logo);
	
	AttachChannelAccess *pattachTB = new AttachChannelAccess(tbframe);  // must to do... ? 

}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

// http://www.qtcentre.org/forum/f-qt-programming-2/t-qtableview-printing-3796.html#10
void MainWindow::filePrint()
{
/*
	QTextDocument *document = textEdit_status->document();
	QPrinter printer;

	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	document->print(&printer);

	statusBar()->showMessage(tr("File Printing"), 2000);
*/
}

void MainWindow::fileOpen()
{
	if( passwd_timeset_check && (checkPasswd() == WR_ERROR) ){
		return;
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), STR_FILEOPEN_DIR, 
		tr("TSS-Files ( *.tss ) ;; All Files ( *.* ) " ) );

	if (!fileName.isEmpty()) {
		pg_timeset.fileOpen(fileName);
		statusBar()->showMessage(tr("File Opened"), 2000);
	}
}

void MainWindow::fileNew()
{
	pg_timeset.pg_remove_all_list();
	setWindowTitle("TSS OPI");
	statusBar()->showMessage(tr("Clear list items"), 2000);
}

void MainWindow::fileSave()
{
	QStringList strList;
	QString fileName;
	QString qstrTemp = QFileDialog::getSaveFileName(this, tr("Save"), STR_FILEOPEN_DIR,
		tr("TSS-File ( *.tss ) " ) );

	strList = qstrTemp.split(".");
	fileName = strList.at(0);
	fileName.append(".tss");

//	for(int i=0; i<strList.size(); i++) {
//		printf("index: %d: %s\n", i, strList.at(i).toAscii().constData());
//	}


	if (!fileName.isEmpty()) {
		pg_timeset.fileSave(fileName);
		statusBar()->showMessage(tr("Save current timing set"), 2000);
	}
}
/*
void MainWindow::fileSaveAs()
{
	statusBar()->showMessage(tr("Save current digitizer set as ..."), 2000);
}
*/

void MainWindow::action_All_LTUs_reset()
{
//	ST_QUEUE_STATUS queueNode;

	QMessageBox msgBox;
	msgBox.setIcon( QMessageBox::Warning );
	msgBox.setWindowTitle("Notice");
	msgBox.setText("All LTU system set to be <Zero>!\nAre you sure?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

	switch( msgBox.exec() ) {
		case QMessageBox::Yes:
			break;
		case QMessageBox::No:
			return;
		default: return;
	}

//	queueNode.caller = QUEUE_ID_SET_ZERO;
//	put_queue_node(queueNode);
	pg_timeset.caput_reset_all_sys();
	
}

void MainWindow::action_All_List_zero()
{
//	int nval;	
	pg_timeset.pg_reset_all_list();
//	nval = pg_timeset.pg_get_bliptime_my();
//	pg_timeset.pg_set_bliptime_my(0);
//	pg_timeset.pg_run_time_flush();
//	pg_timeset.pg_set_bliptime_my(nval);
}

void MainWindow::action_Save_to_MySQL()
{
//	int iCAStatus;
	unsigned int shotNum= pg_timeset.pg_get_query_shot();
//	QString qstr;
	
//	iCAStatus = ca_Get_QStr( pvCCS_ShotNumber.cid, qstr );
//	if( iCAStatus  != ECA_NORMAL ) printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pvCCS_ShotNumber.name);
//	shotNum = qstr.toInt();

//	pg_timeset.current_query_shot

	pg_main.save_current_ltu_setup( shotNum );
}
void MainWindow::action_Delete_shot_MySQL()
{
	unsigned int shotNum= pg_timeset.pg_get_query_shot();
	
	pg_main.delete_stored_ltu_info( shotNum );
}


void MainWindow::initOPIstate()
{
	btn_main->setPalette(pal_select);
//	btn_DB_manager->setPalette(pal_normal);
	btn_timingSet->setPalette(pal_normal);
	btn_local_R1->setPalette(pal_normal);

	stackedWidget_main->setCurrentIndex(STACKED_ID_MAIN);
	tabWidget->setCurrentIndex(0);


//	for( int i=0; i<MAX_LTU_CARD; i++) {
//		unCurrentSysTime[i] = 0;
//		nIsUseThisSystem[i] = 0;
//	}

}

void MainWindow::slot_printStatus(int level, char *buf_in )
{
	char strbuf[1024];
	char buf[1024];
	int len;
//	struct timeval mytime;

//	usleep(5000);
	
	for(int i=0; i<strList_printStatus.size(); i++) 
	{
//		printf("strList_printStatus: id (%d): %s\n",i, strList_printStatus.at(0).toLocal8Bit().constData() );

		sprintf(strbuf, "%05d ", status_line_cnt);
		status_line_cnt++;

		sprintf(buf, "%s", strList_printStatus.at(0).toLocal8Bit().constData() );
		len = strlen(buf);
		if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;

		QDateTime dtime = QDateTime::currentDateTime();	
//		QString text = dtime.toString(" yyyy/MM/dd hh:mm:ss : ");
		QString text = dtime.toString(" MM/dd - hh:mm:ss : ");

	//	gettimeofday(&mytime, NULL); 
	//	printf("%ld:%ld\n", mytime.tv_sec, mytime.tv_usec); 
		strcat(strbuf, text.toAscii().constData() );

		strcat(strbuf, buf);
		
	//	printf("slot_printStatus: %s\n", strbuf );	
		
		listWidget_status->addItem( strbuf );		   
		int row = listWidget_status->count() ;
		listWidget_status->setCurrentRow( row-1 );

		switch(level) 
		{
			case LEVEL_NONE:
				break;
			case LEVEL_WARNING:
				listWidget_status->currentItem()->setBackground(brush_warning);
				break;
			case LEVEL_ERROR:
				listWidget_status->currentItem()->setBackground(brush_error);
				break;
			case LEVEL_NOTICE:
				listWidget_status->currentItem()->setBackground(brush_notice);
				break;
		}
	//	listWidget_status->setCurrentRow( row );

		strList_printStatus.removeAt(0);
	}
	
}

void MainWindow::slot_printReturn(int level, char *buf_in )
{
	char strbuf[1024];
	char buf[1024];
	int len;

	for(int i=0; i<strList_printReturn.size(); i++) 
	{
		sprintf(strbuf, " ");

		sprintf(buf, "%s", strList_printReturn.at(0).toLocal8Bit().constData() );
		len = strlen(buf);
		if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;

		QDateTime dtime = QDateTime::currentDateTime();	
//		QString text = dtime.toString("yyyy/MM/dd _ hh:mm:ss : ");
		QString text = dtime.toString("MM/dd - hh:mm:ss : ");

		strcat(strbuf, text.toAscii().constData() );

		strcat(strbuf, buf);
		
		listWidget_return->addItem( strbuf );		   
		int row = listWidget_return->count() ;
		listWidget_return->setCurrentRow( row-1 );

		switch(level) 
		{
			case LEVEL_NONE:
				break;
			case LEVEL_WARNING:
				listWidget_return->currentItem()->setBackground(brush_warning);
				break;
			case LEVEL_ERROR:
				listWidget_return->currentItem()->setBackground(brush_error);
				break;
			case LEVEL_NOTICE:
				listWidget_return->currentItem()->setBackground(brush_notice);
				break;
		}

		strList_printReturn.removeAt(0);
	}
	
}

void MainWindow::printStatus(int level, const char *fmt, ... )
{
	char buf[1024];
	va_list argp;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	strList_printStatus << buf;
	emit signal_printStatus(level, buf);
}
void MainWindow::printReturn(int level, const char *fmt, ... )
{
	char buf[1024];
	va_list argp;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	strList_printReturn << buf;
	emit signal_printReturn(level, buf);
}

void MainWindow::callSignal_saveLTUparam( )
{
	emit signal_saveLTUparam();
}
void MainWindow::callSignal_updateSpinBox( )
{
	emit signal_update_spinBox();
}

void MainWindow::slot_saveLTUparam()
{
	int iCAStatus;
	unsigned int shotNum=0;
	QString qstr;
	
	iCAStatus = ca_Get_QStr( pvCCS_ShotNumber.cid, qstr );
	if( iCAStatus  != ECA_NORMAL ) printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pvCCS_ShotNumber.name);
	shotNum = qstr.toInt();

	pg_main.save_current_ltu_setup(shotNum);
}
void MainWindow::slot_update_spinBox()
{
	update_spinBox_shotNum_minus_one();
}


void MainWindow::saveConfigFile()
{
	FILE *fp;

	if( (fp = fopen("/home/kstar/tss_configure/.cltucfg", "w") ) == NULL )
	{	
		fprintf(stderr, "Can not make CLTU configure file for write\n");
		return;
	} else {
		fprintf(fp, "#level high = 0\n");
		fprintf(fp, "#level low = 1\n");
		fprintf(fp, "#clock unit (KHz)\n");
		fprintf(fp, "#LtuDDS1 enable port t0 t1 clock level\n");

		fprintf(fp, "BLIP_TIME\n");
		fprintf(fp, "%d\n", pg_timeset.pg_get_bliptime() );

		saveTimingTest(fp);
//		saveCalibTest(fp);
//		saveCalibedResult(fp);

		fclose(fp);
	}

}

void MainWindow::saveTimingTest(FILE *fp)
{
	fprintf(fp, "TEST\n");
	fprintf(fp, "%s\n", cb_test_system->currentText().toAscii().constData() );
	fprintf(fp, "P1 %d %lf %lf %f\n", cb_test_pt1_pol->currentIndex(), le_test_pt1_start->text().toDouble(), le_test_pt1_stop->text().toDouble(), le_test_pt1_freq->text().toFloat() );
	fprintf(fp, "P2 %d %lf %lf %f\n", cb_test_pt2_pol->currentIndex(), le_test_pt2_start->text().toDouble(), le_test_pt2_stop->text().toDouble(), le_test_pt2_freq->text().toFloat() );
	fprintf(fp, "P3 %d %lf %lf %f\n", cb_test_pt3_pol->currentIndex(), le_test_pt3_start->text().toDouble(), le_test_pt3_stop->text().toDouble(), le_test_pt3_freq->text().toFloat() );
	fprintf(fp, "P4 %d %lf %lf %f\n", cb_test_pt4_pol->currentIndex(), le_test_pt4_start->text().toDouble(), le_test_pt4_stop->text().toDouble(), le_test_pt4_freq->text().toFloat() );
	fprintf(fp, "P5 %d %lf %lf %f\t\t%lf %lf %f\t\t%lf %lf %f\n", \
			cb_test_pt5_pol1->currentIndex(), le_test_pt5_start1->text().toDouble(), le_test_pt5_stop1->text().toDouble(), le_test_pt5_freq1->text().toFloat(), \
			le_test_pt5_start2->text().toDouble(), le_test_pt5_stop2->text().toDouble(), le_test_pt5_freq2->text().toFloat(), \
			le_test_pt5_start3->text().toDouble(), le_test_pt5_stop3->text().toDouble(), le_test_pt5_freq3->text().toFloat() );
}


void MainWindow::clicked_btn_start_monitoring()
{
//	dirty_monitor = 1;
	pv_monitoring_start();
	btn_start_monitoring->setEnabled(false);
	btn_stop_monitoring->setEnabled(true);	
}
void MainWindow::clicked_btn_stop_monitoring()
{
	pv_monitoring_stop();
	btn_start_monitoring->setEnabled(true);
	btn_stop_monitoring->setEnabled(false);
}

//char pvbuf[20][MAX_PV_NAME_LEN];
void MainWindow::updatePV(long op, ST_PV_NODE &node, int print_on)
{
	if( op == CA_OP_CONN_UP ) {		
		node.flag_cid = 1;
		if( print_on )  printStatus(LEVEL_NONE, "Connected ... \"%s\"\n", node.name);
	}
	else {
		node.flag_cid = 0;
		if( print_on ) printStatus(LEVEL_ERROR,"Disconnected ... \"%s\"\n", node.name);
	}
}

void connectionCB_LTU(struct connection_handler_args args)
{
	long 	op = args.op;

	for(int sys=0; sys<MAX_LTU_CARD; sys++) {
		if( args.chid == pparent->pvCLTU[sys].SHOT.cid ) {
			pparent->updatePV(op, pparent->pvCLTU[sys].SHOT, 1); /* print connection state with last argument 1 */
			return;
		}

		if( pparent->Is_New_R2_id(sys) ) 
		{
			if( args.chid == pparent->pvCLTU[sys].SHOT_END.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SHOT_END); return; }
//			if( args.chid == pparent->pvCLTU[sys].IRIG_B.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].IRIG_B); return; }
			if( args.chid == pparent->pvCLTU[sys].SET_FREE_RUN.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SET_FREE_RUN); return; }
			if( args.chid == pparent->pvCLTU[sys].GET_COMP_NS.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].GET_COMP_NS); return; }
			if( args.chid == pparent->pvCLTU[sys].SET_CAL_VALUE.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SET_CAL_VALUE); return; }
//			if( args.chid == pparent->pvCLTU[sys].SHOW_INFO.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SHOW_INFO); return; }
			if( args.chid == pparent->pvCLTU[sys].SHOW_STATUS.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SHOW_STATUS); return; }
			if( args.chid == pparent->pvCLTU[sys].SHOW_TIME.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SHOW_TIME); return; }
			if( args.chid == pparent->pvCLTU[sys].SHOW_TLK.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SHOW_TLK); return; }
			if( args.chid == pparent->pvCLTU[sys].LOG_GSHOT.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].LOG_GSHOT); return; }

			for( int j=0; j<R2_PORT_CNT; j++) 
			{
				if( args.chid == pparent->pvCLTU[sys].SETUP_Pt[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SETUP_Pt[j]); return; }
			
				if( args.chid == pparent->pvCLTU[sys].R2_ActiveLow[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_ActiveLow[j]); return; }
				if( args.chid == pparent->pvCLTU[sys].R2_Enable[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_Enable[j]); return; }
				if( args.chid == pparent->pvCLTU[sys].R2_Mode[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_Mode[j]); return; }
				
				for( int k=0; k<R2_SECT_CNT; k++) 
				{
					if( args.chid == pparent->pvCLTU[sys].R2_CLOCK[j][k].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_CLOCK[j][k]); return; }
					if( args.chid == pparent->pvCLTU[sys].R2_T0[j][k].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_T0[j][k]); return; }
					if( args.chid == pparent->pvCLTU[sys].R2_T1[j][k].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].R2_T1[j][k]); return; }
				}
			}
		
		}
		else {
		

			for( int j=0; j<4; j++) {
				if( args.chid == pparent->pvCLTU[sys].POL[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].POL[j]); return; }
				if( args.chid == pparent->pvCLTU[sys].CLOCK[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].CLOCK[j]); return;  }
				if( args.chid == pparent->pvCLTU[sys].T0[j].cid )  { pparent->updatePV(op, pparent->pvCLTU[sys].T0[j]);  return;  }
				if( args.chid == pparent->pvCLTU[sys].T1[j].cid )  { pparent->updatePV(op, pparent->pvCLTU[sys].T1[j]); return; }
			}

			for( int j=0; j<5; j++) {
				if( args.chid == pparent->pvCLTU[sys].SETUP_Pt[j].cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].SETUP_Pt[j] ); return; }
			}

			if( args.chid == pparent->pvCLTU[sys].mTRIG.cid ) { pparent->updatePV(op, pparent->pvCLTU[sys].mTRIG ); return; }
			for( int j=0; j<3; j++) {
				if( args.chid == pparent->pvCLTU[sys].mCLOCK[j].cid ) {pparent->updatePV(op, pparent->pvCLTU[sys].mCLOCK[j]); return; }
				if( args.chid == pparent->pvCLTU[sys].mT0[j].cid ) {pparent->updatePV(op, pparent->pvCLTU[sys].mT0[j]); return; }
				if( args.chid == pparent->pvCLTU[sys].mT1[j].cid ) {pparent->updatePV(op, pparent->pvCLTU[sys].mT1[j]); return; }
			}
		}




	}

//	FIND:
//		printf("exit\n");

}


void connectionCB_DDS(struct connection_handler_args args)
{
	long 	op = args.op;
	
#if 0
// 2013. 2. 28
	if( args.chid == pparent->pvDDS1A[0].sampleRate.cid ) {pparent->updatePV(op, pparent->pvDDS1A[0].sampleRate, 1 ); return; } /* print connection state with argument 1 */
	if( args.chid == pparent->pvDDS1A[0].setT0.cid ) {pparent->updatePV(op, pparent->pvDDS1A[0].setT0 ); return; }
	if( args.chid == pparent->pvDDS1A[0].setT1.cid ) {pparent->updatePV(op, pparent->pvDDS1A[0].setT1 ); return; }
	for( int i=1; i<CNT_DDS1A_NO; i++) {
		if( args.chid == pparent->pvDDS1A[i].sampleRate.cid ) {pparent->updatePV(op, pparent->pvDDS1A[i].sampleRate ); return; }
		if( args.chid == pparent->pvDDS1A[i].setT0.cid ) {pparent->updatePV(op, pparent->pvDDS1A[i].setT0 ); return; }
		if( args.chid == pparent->pvDDS1A[i].setT1.cid ) {pparent->updatePV(op, pparent->pvDDS1A[i].setT1 ); return; }
	}
	if( args.chid == pparent->pvDDS1B[0].sampleRate.cid ) {pparent->updatePV(op, pparent->pvDDS1B[0].sampleRate, 1 ); return; } /* print connection state with argument 1 */
	if( args.chid == pparent->pvDDS1B[0].setT0.cid ) {pparent->updatePV(op, pparent->pvDDS1B[0].setT0 ); return; }
	if( args.chid == pparent->pvDDS1B[0].setT1.cid ) {pparent->updatePV(op, pparent->pvDDS1B[0].setT1 ); return; }
	for( int i=1; i<CNT_DDS1B_NO; i++) {
		if( args.chid == pparent->pvDDS1B[i].sampleRate.cid ) {pparent->updatePV(op, pparent->pvDDS1B[i].sampleRate ); return; }
		if( args.chid == pparent->pvDDS1B[i].setT0.cid ) {pparent->updatePV(op, pparent->pvDDS1B[i].setT0 ); return; }
		if( args.chid == pparent->pvDDS1B[i].setT1.cid ) {pparent->updatePV(op, pparent->pvDDS1B[i].setT1 ); return; }
	}
#endif

/*	if( args.chid == pparent->pvHALPHA.DAQstop.cid ){  pparent->updatePV(op, pparent->pvHALPHA.DAQstop, 1 ); return;	}
	if( args.chid == pparent->pvHALPHA.ADCstop.cid ){  pparent->updatePV(op, pparent->pvHALPHA.ADCstop ); return;	}
	if( args.chid == pparent->pvHALPHA.RemoteSave.cid ){  pparent->updatePV(op, pparent->pvHALPHA.RemoteSave ); return;	}
*/

	if( args.chid == pparent->pvHALPHA.sampleRate.cid ){  pparent->updatePV(op, pparent->pvHALPHA.sampleRate ); return;	}
	if( args.chid == pparent->pvHALPHA.setT0.cid ){  pparent->updatePV(op, pparent->pvHALPHA.setT0 ); return;	}
	if( args.chid == pparent->pvHALPHA.setT1.cid ){  pparent->updatePV(op, pparent->pvHALPHA.setT1 ); return;	}
/*
	if( args.chid == pparent->pvECEHR.sampleRate.cid ){  pparent->updatePV(op, pparent->pvECEHR.sampleRate ); return;	}
	if( args.chid == pparent->pvECEHR.setT0.cid ){  pparent->updatePV(op, pparent->pvECEHR.setT0 ); return;	}
	if( args.chid == pparent->pvECEHR.setT1.cid ){  pparent->updatePV(op, pparent->pvECEHR.setT1 ); return;	}
*/
/*	if( args.chid == pparent->pvHALPHA.getReady.cid ){  pparent->updatePV(op, pparent->pvHALPHA.getReady ); return;	} */

}

void connectionCB_Init(struct connection_handler_args args)
{
	long	op = args.op;
	if( args.chid == pparent->pvBlipTime.cid ) {
		pparent->updatePV(op, pparent->pvBlipTime, 1); /* print connection state with argument 1 */
		return;
	}
	if( args.chid == pparent->pvCCS_ShotNumber.cid ) {
		pparent->updatePV(op, pparent->pvCCS_ShotNumber, 1); /* print connection state with argument 1 */
		return;
	}
	if( args.chid == pparent->pvCCS_SeqStart.cid) {
		pparent->updatePV(op, pparent->pvCCS_SeqStart, 1); /* print connection state with argument 1 */
		return;
	}
	if( args.chid == pparent->pvTSSConfirm.cid ) {
		pparent->updatePV(op, pparent->pvTSSConfirm, 1); /* print connection state with argument 1 */
		return;
	}
	if( args.chid == pparent->pvTSS_CalMode.cid ) { pparent->updatePV(op, pparent->pvTSS_CalMode, 1); return; }
}

void MainWindow::pv_copy_names_LTUs()
{
	char buf_sys_name[MAX_PV_NAME_LEN];
	char tmp_buf[10];
	char tmp_buf2[10];
	
	for( int i=0; i<MAX_LTU_CARD; i++) {
		if( (i == CTUV1) || (i == TSS_CTU) )	sprintf(buf_sys_name, "%s_CTU_", str_sysName[i]);
		else 		sprintf(buf_sys_name, "%s_LTU_", str_sysName[i]);

		strcpy(pvCLTU[i].SHOT.name, buf_sys_name);	strcat(pvCLTU[i].SHOT.name, "shotStart");

		if( Is_New_R2_id(i) ) 
		{
			strcpy(pvCLTU[i].SHOT_END.name, buf_sys_name);	strcat(pvCLTU[i].SHOT_END.name, "SHOT_END");
			strcpy(pvCLTU[i].SET_FREE_RUN.name, buf_sys_name);	strcat(pvCLTU[i].SET_FREE_RUN.name, "SET_FREE_RUN");
//			strcpy(pvCLTU[i].IRIG_B.name, buf_sys_name);	strcat(pvCLTU[i].IRIG_B.name, "IRIGB_ENABLE");
			strcpy(pvCLTU[i].GET_COMP_NS.name, buf_sys_name);	strcat(pvCLTU[i].GET_COMP_NS.name, "GET_COMP_NS");
			strcpy(pvCLTU[i].SET_CAL_VALUE.name, buf_sys_name);	strcat(pvCLTU[i].SET_CAL_VALUE.name, "SET_CAL_VALUE");
//			strcpy(pvCLTU[i].SHOW_INFO.name, buf_sys_name);	strcat(pvCLTU[i].SHOW_INFO.name, "SHOW_INFO");
			strcpy(pvCLTU[i].SHOW_STATUS.name, buf_sys_name);	strcat(pvCLTU[i].SHOW_STATUS.name, "SHOW_STATUS");
			strcpy(pvCLTU[i].SHOW_TIME.name, buf_sys_name);	strcat(pvCLTU[i].SHOW_TIME.name, "SHOW_TIME");
			strcpy(pvCLTU[i].SHOW_TLK.name, buf_sys_name);	strcat(pvCLTU[i].SHOW_TLK.name, "SHOW_TLK");
			strcpy(pvCLTU[i].LOG_GSHOT.name, buf_sys_name);	strcat(pvCLTU[i].LOG_GSHOT.name, "LOG_GSHOT");

			for( int j=0; j<R2_PORT_CNT; j++) {
				sprintf(tmp_buf, "P%d_", j);
				
				sprintf(tmp_buf2, "setup_p%d", j);
				strcpy(pvCLTU[i].SETUP_Pt[j].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[j].name, tmp_buf2);
				
				strcpy(pvCLTU[i].R2_ActiveLow[j].name, buf_sys_name);	
					strcat(pvCLTU[i].R2_ActiveLow[j].name, tmp_buf); strcat(pvCLTU[i].R2_ActiveLow[j].name, "ActiveLow");
				strcpy(pvCLTU[i].R2_Enable[j].name, buf_sys_name);
					strcat(pvCLTU[i].R2_Enable[j].name, tmp_buf); strcat(pvCLTU[i].R2_Enable[j].name, "ENABLE");
				strcpy(pvCLTU[i].R2_Mode[j].name, buf_sys_name);
					strcat(pvCLTU[i].R2_Mode[j].name, tmp_buf); strcat(pvCLTU[i].R2_Mode[j].name, "MODE");
			}

			for( int j=0; j<R2_PORT_CNT; j++) 
			{
				sprintf(tmp_buf, "P%d_", j);
				for( int k=0; k<R2_SECT_CNT_EXP; k++) 
				{
					sprintf(tmp_buf2, "SEC%d_", k);
					
					strcpy(pvCLTU[i].R2_CLOCK[j][k].name, buf_sys_name);
						strcat(pvCLTU[i].R2_CLOCK[j][k].name, tmp_buf);
						strcat(pvCLTU[i].R2_CLOCK[j][k].name, tmp_buf2);
						strcat(pvCLTU[i].R2_CLOCK[j][k].name, "CLOCK");

					strcpy(pvCLTU[i].R2_T0[j][k].name, buf_sys_name);
						strcat(pvCLTU[i].R2_T0[j][k].name, tmp_buf);
						strcat(pvCLTU[i].R2_T0[j][k].name, tmp_buf2);
						strcat(pvCLTU[i].R2_T0[j][k].name, "T0");

					strcpy(pvCLTU[i].R2_T1[j][k].name, buf_sys_name);
						strcat(pvCLTU[i].R2_T1[j][k].name, tmp_buf);
						strcat(pvCLTU[i].R2_T1[j][k].name, tmp_buf2);
						strcat(pvCLTU[i].R2_T1[j][k].name, "T1");
				}
			}

		}
		else 
		{
			strcpy(pvCLTU[i].POL[0].name, buf_sys_name);	strcat(pvCLTU[i].POL[0].name, "Trig_p0");
			strcpy(pvCLTU[i].POL[1].name, buf_sys_name);	strcat(pvCLTU[i].POL[1].name, "Trig_p1");
			strcpy(pvCLTU[i].POL[2].name, buf_sys_name);	strcat(pvCLTU[i].POL[2].name, "Trig_p2");
			strcpy(pvCLTU[i].POL[3].name, buf_sys_name);	strcat(pvCLTU[i].POL[3].name, "Trig_p3");
			
			strcpy(pvCLTU[i].CLOCK[0].name, buf_sys_name);	strcat(pvCLTU[i].CLOCK[0].name, "Clock_p0");
			strcpy(pvCLTU[i].CLOCK[1].name, buf_sys_name);	strcat(pvCLTU[i].CLOCK[1].name, "Clock_p1");
			strcpy(pvCLTU[i].CLOCK[2].name, buf_sys_name);	strcat(pvCLTU[i].CLOCK[2].name, "Clock_p2");
			strcpy(pvCLTU[i].CLOCK[3].name, buf_sys_name);	strcat(pvCLTU[i].CLOCK[3].name, "Clock_p3");

			strcpy(pvCLTU[i].T0[0].name, buf_sys_name);	strcat(pvCLTU[i].T0[0].name, "T0_p0");
			strcpy(pvCLTU[i].T0[1].name, buf_sys_name);	strcat(pvCLTU[i].T0[1].name, "T0_p1");
			strcpy(pvCLTU[i].T0[2].name, buf_sys_name);	strcat(pvCLTU[i].T0[2].name, "T0_p2");
			strcpy(pvCLTU[i].T0[3].name, buf_sys_name);	strcat(pvCLTU[i].T0[3].name, "T0_p3");

			strcpy(pvCLTU[i].T1[0].name, buf_sys_name);	strcat(pvCLTU[i].T1[0].name, "T1_p0");
			strcpy(pvCLTU[i].T1[1].name, buf_sys_name);	strcat(pvCLTU[i].T1[1].name, "T1_p1");
			strcpy(pvCLTU[i].T1[2].name, buf_sys_name);	strcat(pvCLTU[i].T1[2].name, "T1_p2");
			strcpy(pvCLTU[i].T1[3].name, buf_sys_name);	strcat(pvCLTU[i].T1[3].name, "T1_p3");

			strcpy(pvCLTU[i].SETUP_Pt[0].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[0].name, "setup_p0");
			strcpy(pvCLTU[i].SETUP_Pt[1].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[1].name, "setup_p1");
			strcpy(pvCLTU[i].SETUP_Pt[2].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[2].name, "setup_p2");
			strcpy(pvCLTU[i].SETUP_Pt[3].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[3].name, "setup_p3");
			strcpy(pvCLTU[i].SETUP_Pt[4].name, buf_sys_name);	strcat(pvCLTU[i].SETUP_Pt[4].name, "setup_p4");

			strcpy(pvCLTU[i].mTRIG.name, buf_sys_name);	strcat(pvCLTU[i].mTRIG.name, "mTrig");

			strcpy(pvCLTU[i].mCLOCK[0].name, buf_sys_name);	strcat(pvCLTU[i].mCLOCK[0].name, "mClock_sec0");
			strcpy(pvCLTU[i].mCLOCK[1].name, buf_sys_name);	strcat(pvCLTU[i].mCLOCK[1].name, "mClock_sec1");
			strcpy(pvCLTU[i].mCLOCK[2].name, buf_sys_name);	strcat(pvCLTU[i].mCLOCK[2].name, "mClock_sec2");

			strcpy(pvCLTU[i].mT0[0].name, buf_sys_name);	strcat(pvCLTU[i].mT0[0].name, "mT0_sec0");
			strcpy(pvCLTU[i].mT0[1].name, buf_sys_name);	strcat(pvCLTU[i].mT0[1].name, "mT0_sec1");
			strcpy(pvCLTU[i].mT0[2].name, buf_sys_name);	strcat(pvCLTU[i].mT0[2].name, "mT0_sec2");

			strcpy(pvCLTU[i].mT1[0].name, buf_sys_name);	strcat(pvCLTU[i].mT1[0].name, "mT1_sec0");
			strcpy(pvCLTU[i].mT1[1].name, buf_sys_name);	strcat(pvCLTU[i].mT1[1].name, "mT1_sec1");
			strcpy(pvCLTU[i].mT1[2].name, buf_sys_name);	strcat(pvCLTU[i].mT1[2].name, "mT1_sec2");

		}
	
	}
	
}

void MainWindow::pv_copy_names_others()
{
	char buf_sys_name[MAX_PV_NAME_LEN];
#if 0
// 2013. 2. 28 
	for( int i=0; i<CNT_DDS1A_NO; i++) {
		sprintf(buf_sys_name, "DDS1A_B%d_", (i+1) );
		strcpy(pvDDS1A[i].sampleRate.name, buf_sys_name);	strcat(pvDDS1A[i].sampleRate.name, "sampleRate");
		strcpy(pvDDS1A[i].setT0.name, buf_sys_name);	strcat(pvDDS1A[i].setT0.name, "setT0");
		strcpy(pvDDS1A[i].setT1.name, buf_sys_name);	strcat(pvDDS1A[i].setT1.name, "setT1");
		strcpy(pvDDS1A[i].divider.name, buf_sys_name);	strcat(pvDDS1A[i].divider.name, "ClockDivider");
	}
	for( int i=0; i<CNT_DDS1B_NO; i++) {
		sprintf(buf_sys_name, "DDS1B_B%d_", (CNT_DDS1A_NO+i+1) );		
		strcpy(pvDDS1B[i].sampleRate.name, buf_sys_name);	strcat(pvDDS1B[i].sampleRate.name, "sampleRate");
		strcpy(pvDDS1B[i].setT0.name, buf_sys_name);	strcat(pvDDS1B[i].setT0.name, "setT0");
		strcpy(pvDDS1B[i].setT1.name, buf_sys_name);	strcat(pvDDS1B[i].setT1.name, "setT1");
		strcpy(pvDDS1B[i].divider.name, buf_sys_name);	strcat(pvDDS1B[i].divider.name, "ClockDivider");
	}
#endif
	strcpy(pvHALPHA.sampleRate.name, "HALPHA_sampleRate");
	strcpy(pvHALPHA.setT0.name, "HALPHA_setT0");
	strcpy(pvHALPHA.setT1.name, "HALPHA_setT1");
/*
	strcpy(pvECEHR.sampleRate.name, "ECEHR_sampleRate");
	strcpy(pvECEHR.setT0.name, "ECEHR_setT0");
	strcpy(pvECEHR.setT1.name, "ECEHR_setT1");
*/
	strcpy(pvBlipTime.name, str_CCS_BLIP_TIME);
	strcpy(pvCCS_ShotNumber.name, str_CCS_SHOT_NUMBER);
	strcpy(pvCCS_SeqStart.name, str_CCS_SHOTSEQ_START);
	strcpy(pvTSSConfirm.name, str_CCS_TSS_CONFIRM);
	strcpy(pvTSS_CalMode.name, str_TSS_SET_CAL_MODE);


}

void MainWindow::pv_connect_LTU()
{
	for( int i=0; i<MAX_LTU_CARD; i++) 
	{
		ca_Connect_STPV( pvCLTU[i].SHOT, (VOIDFUNCPTR)connectionCB_LTU);

		if( Is_New_R2_id(i) ) 
		{
			ca_Connect_STPV( pvCLTU[i].SHOT_END, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].SET_FREE_RUN, (VOIDFUNCPTR)connectionCB_LTU);
//			ca_Connect_STPV( pvCLTU[i].IRIG_B, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].GET_COMP_NS, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].SET_CAL_VALUE, (VOIDFUNCPTR)connectionCB_LTU);
//			ca_Connect_STPV( pvCLTU[i].SHOW_INFO, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].SHOW_STATUS, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].SHOW_TIME, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].SHOW_TLK, (VOIDFUNCPTR)connectionCB_LTU);
			ca_Connect_STPV( pvCLTU[i].LOG_GSHOT, (VOIDFUNCPTR)connectionCB_LTU);


			for( int j=0; j<R2_PORT_CNT; j++) 
			{
				ca_Connect_STPV( pvCLTU[i].SETUP_Pt[j], (VOIDFUNCPTR)connectionCB_LTU);
			
				ca_Connect_STPV( pvCLTU[i].R2_ActiveLow[j], (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].R2_Enable[j], (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].R2_Mode[j], (VOIDFUNCPTR)connectionCB_LTU);
				
				for( int k=0; k<R2_SECT_CNT; k++) 
				{
					ca_Connect_STPV( pvCLTU[i].R2_CLOCK[j][k], (VOIDFUNCPTR)connectionCB_LTU);
					ca_Connect_STPV( pvCLTU[i].R2_T0[j][k], (VOIDFUNCPTR)connectionCB_LTU);
					ca_Connect_STPV( pvCLTU[i].R2_T1[j][k], (VOIDFUNCPTR)connectionCB_LTU);
					
				}
			}
		}
		else 
		{
			for( int j=0; j<4; j++) {
				ca_Connect_STPV( pvCLTU[i].POL[j], (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].CLOCK[j],  (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].T0[j],  (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].T1[j],  (VOIDFUNCPTR)connectionCB_LTU);
			}

			for( int j=0; j<5; j++) {
				ca_Connect_STPV( pvCLTU[i].SETUP_Pt[j],  (VOIDFUNCPTR)connectionCB_LTU);
			}

			ca_Connect_STPV( pvCLTU[i].mTRIG,  (VOIDFUNCPTR)connectionCB_LTU);
			for( int j=0; j<3; j++) {
				ca_Connect_STPV( pvCLTU[i].mCLOCK[j],  (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].mT0[j],  (VOIDFUNCPTR)connectionCB_LTU);
				ca_Connect_STPV( pvCLTU[i].mT1[j],  (VOIDFUNCPTR)connectionCB_LTU);
			}
		}
		
//		usleep(1000);
	}

}

void MainWindow::pv_connect_DDS1()
{
	for( int i=0; i<CNT_DDS1A_NO; i++) {
		if( ca_Connect_STPV( pvDDS1A[i].sampleRate, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1A[i].sampleRate.name );
		
		if( ca_Connect_STPV( pvDDS1A[i].setT0, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1A[i].setT0.name );
		
		if( ca_Connect_STPV( pvDDS1A[i].setT1, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1A[i].setT1.name );
/*
		if( ca_Connect_STPV( pvDDS1A[i].divider, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1A[i].divider.name );
*/
	}
	for( int i=0; i<CNT_DDS1B_NO; i++) {
		if( ca_Connect_STPV( pvDDS1B[i].sampleRate, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1B[i].sampleRate.name );
		
		if( ca_Connect_STPV( pvDDS1B[i].setT0, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1B[i].setT0.name );
		
		if( ca_Connect_STPV( pvDDS1B[i].setT1, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) 
			printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvDDS1B[i].setT1.name );
	}

}
void MainWindow::pv_connect_HALPHA()
{
/*	
	if( ca_Connect_STPV( pvHALPHA.DAQstop, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_DAQstop connect errer!" );
	if( ca_Connect_STPV( pvHALPHA.ADCstop, (VOIDFUNCPTR)connectionCB_DDS) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_ADCstop connect errer!" );
	if( ca_Connect_STPV( pvHALPHA.RemoteSave, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_RemoteSave connect errer!" );
*/	if( ca_Connect_STPV( pvHALPHA.sampleRate, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_sampleRate connect errer!" );
	if( ca_Connect_STPV( pvHALPHA.setT0, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_setT0 connect errer!" );
	if( ca_Connect_STPV( pvHALPHA.setT1, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_setT1 connect errer!" );
/*	if( ca_Connect_STPV( pvHALPHA.getReady, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "HALPHA_REMOTE_READY connect errer!" ); */
/*
	if( ca_Connect_STPV( pvECEHR.sampleRate, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "ECEHR_sampleRate connect errer!" );
	if( ca_Connect_STPV( pvECEHR.setT0, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "ECEHR_setT0 connect errer!" );
	if( ca_Connect_STPV( pvECEHR.setT1, (VOIDFUNCPTR)connectionCB_DDS ) != ECA_NORMAL ) printStatus(LEVEL_NONE, "ECEHR_setT1 connect errer!" );
*/

}
void MainWindow::pv_connect_Init()
{
	if( ca_Connect_STPV( pvBlipTime, (VOIDFUNCPTR)connectionCB_Init) != ECA_NORMAL ) 
		printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvBlipTime.name );
	
	if( ca_Connect_STPV( pvCCS_ShotNumber, (VOIDFUNCPTR)connectionCB_Init) != ECA_NORMAL ) 
		printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvCCS_ShotNumber.name );

	if( ca_Connect_STPV( pvCCS_SeqStart, (VOIDFUNCPTR)connectionCB_Init) != ECA_NORMAL ) 
		printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvCCS_SeqStart.name );

	if( ca_Connect_STPV( pvTSSConfirm, (VOIDFUNCPTR)connectionCB_Init) != ECA_NORMAL ) 
		printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvTSSConfirm.name );

	if( ca_Connect_STPV( pvTSS_CalMode, (VOIDFUNCPTR)connectionCB_Init) != ECA_NORMAL ) 
		printStatus(LEVEL_ERROR, "ERROR: %s connection!\n", pvTSS_CalMode.name );
}

void MainWindow::pv_monitoring_start()
{
	printStatus(LEVEL_NONE,"PV monitoring started!");

	pg_main.cb_register_blipFunc(pvBlipTime);
	pg_main.cb_register_main(pvCCS_SeqStart, 0, 0, TYPE_SHOTSEQ_START, 0, 0); // 2012. 1. 18
#if 1
	for( int sys=0; sys<MAX_LTU_CARD; sys++) 
	{
		if( ca_read_access( pvCLTU[sys].SHOT.cid ) ) /* 2010. 2. 19 */
		{	
			if( Is_New_R2_id(sys) ) 
			{ 
				pg_timeset.cb_register_callbackFunc( pvCLTU[sys].GET_COMP_NS, sys, MODE_NEW_R2, TYPE_R2_GET_COMP_NS, 0);
				pg_timeset.cb_register_callbackFunc( pvCLTU[sys].SET_FREE_RUN, sys, MODE_NEW_R2, TYPE_R2_SET_FREE_RUN, 0);
//				pg_main.cb_register_main( pvCLTU[sys].SET_FREE_RUN, sys, MODE_NEW_R2, TYPE_R2_SET_FREE_RUN, 0);
				
				for(int j=0; j<R2_PORT_CNT; j++) 
				{
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].SETUP_Pt[j], sys, MODE_NEW_R2, TYPE_R2_SETUP, j);
					
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_ActiveLow[j], sys, MODE_NEW_R2, TYPE_R2_ActiveLow, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_Enable[j], sys, MODE_NEW_R2, TYPE_R2_Enable, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_Mode[j], sys, MODE_NEW_R2, TYPE_R2_Mode, j );
					
					for(int k=0; k<R2_SECT_CNT; k++) {
						pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_CLOCK[j][k], sys, MODE_NEW_R2, TYPE_R2_CLOCK, j, k );
						pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_T0[j][k], sys, MODE_NEW_R2, TYPE_R2_T0, j, k );
						pg_timeset.cb_register_callbackFunc( pvCLTU[sys].R2_T1[j][k], sys, MODE_NEW_R2, TYPE_R2_T1, j, k );
					}
				}
			
			}
			else {
			
				for( int j=0; j<4; j++) {
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].POL[j], sys, MODE_SINGLE, TYPE_SINGLE_TRIG, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].CLOCK[j], sys, MODE_SINGLE, TYPE_SINGLE_CLOCK, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].T0[j], sys, MODE_SINGLE, TYPE_SINGLE_T0, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].T1[j], sys, MODE_SINGLE, TYPE_SINGLE_T1, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].SETUP_Pt[j], sys, MODE_SINGLE, TYPE_SINGLE_SETUP, j );
				}
				pg_timeset.cb_register_callbackFunc(  pvCLTU[sys].mTRIG, sys, MODE_MULTI, TYPE_MULTI_TRIG, 0 );			
				for( int j=0; j<3; j++) {
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].mCLOCK[j], sys, MODE_MULTI, TYPE_MULTI_CLOCK, j );
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].mT0[j], sys, MODE_MULTI, TYPE_MULTI_T0, j);
					pg_timeset.cb_register_callbackFunc( pvCLTU[sys].mT1[j], sys, MODE_MULTI, TYPE_MULTI_T1, j);
				}
				pg_timeset.cb_register_callbackFunc( pvCLTU[sys].SETUP_Pt[4], sys, MODE_MULTI, TYPE_MULTI_SETUP, 4);
			}

		}
	}
#endif

}

void MainWindow::pv_monitoring_stop()
{
	pg_main.cb_clear_blipFunc(pvBlipTime);
	pg_main.cb_clear_callbackFunc(pvCCS_SeqStart); // 2012. 1. 18

	for( int i=0; i<MAX_LTU_CARD; i++) {

		if( Is_New_R2_id(i) ) 
		{ 
			pg_timeset.cb_clear_callbackFunc( pvCLTU[i].GET_COMP_NS);
			pg_timeset.cb_clear_callbackFunc( pvCLTU[i].SET_FREE_RUN);
			
			for(int j=0; j<R2_PORT_CNT; j++) 
			{
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].SETUP_Pt[j] );
				
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_ActiveLow[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_Enable[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_Mode[j] );
				
				for(int k=0; k<R2_SECT_CNT; k++) {
					pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_CLOCK[j][k] );
					pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_T0[j][k] );
					pg_timeset.cb_clear_callbackFunc( pvCLTU[i].R2_T1[j][k] );
				}
			}
			
		}
		else 
		{
			for( int j=0; j<4; j++) {
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].POL[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].CLOCK[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].T0[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].T1[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].SETUP_Pt[j] );
			}
			pg_timeset.cb_clear_callbackFunc(  pvCLTU[i].mTRIG );
			for( int j=0; j<3; j++) {
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].mCLOCK[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].mT0[j] );
				pg_timeset.cb_clear_callbackFunc( pvCLTU[i].mT1[j] );
			}
			pg_timeset.cb_clear_callbackFunc( pvCLTU[i].SETUP_Pt[4] );
		}
	}
	printStatus(LEVEL_NONE,"PV monitoring stopped!");
}

void MainWindow::pv_disconnect()
{
	for( int i=0; i<MAX_LTU_CARD; i++) {
		ca_Disconnect_STPV(  pvCLTU[i].SHOT);

		if( Is_New_R2_id(i) ) 
		{
			ca_Disconnect_STPV( pvCLTU[i].SHOT_END);
			ca_Disconnect_STPV( pvCLTU[i].SET_FREE_RUN);
//			ca_Disconnect_STPV( pvCLTU[i].IRIG_B);

			ca_Disconnect_STPV( pvCLTU[i].GET_COMP_NS);
			ca_Disconnect_STPV( pvCLTU[i].SET_CAL_VALUE);
//			ca_Disconnect_STPV( pvCLTU[i].SHOW_INFO);
			ca_Disconnect_STPV( pvCLTU[i].SHOW_STATUS);
			ca_Disconnect_STPV( pvCLTU[i].SHOW_TIME);
			ca_Disconnect_STPV( pvCLTU[i].SHOW_TLK);
			ca_Disconnect_STPV( pvCLTU[i].LOG_GSHOT);

			for( int j=0; j<R2_PORT_CNT; j++) 
			{
				ca_Disconnect_STPV( pvCLTU[i].SETUP_Pt[j]);
				
				ca_Disconnect_STPV( pvCLTU[i].R2_ActiveLow[j]);
				ca_Disconnect_STPV( pvCLTU[i].R2_Enable[j]);
				ca_Disconnect_STPV( pvCLTU[i].R2_Mode[j]);
				
				for( int k=0; k<R2_SECT_CNT; k++) 
				{
					ca_Disconnect_STPV( pvCLTU[i].R2_CLOCK[j][k]);
					ca_Disconnect_STPV( pvCLTU[i].R2_T0[j][k]);
					ca_Disconnect_STPV( pvCLTU[i].R2_T1[j][k]);
				}
			}

		}
		else 
		{

			for( int j=0; j<4; j++) {
				ca_Disconnect_STPV( pvCLTU[i].POL[j]);
				ca_Disconnect_STPV( pvCLTU[i].CLOCK[j]);
				ca_Disconnect_STPV( pvCLTU[i].T0[j]);
				ca_Disconnect_STPV( pvCLTU[i].T1[j]);
			}

			for( int j=0; j<5; j++) {
				ca_Disconnect_STPV( pvCLTU[i].SETUP_Pt[j]);
			}

			ca_Disconnect_STPV( pvCLTU[i].mTRIG);
			for( int j=0; j<3; j++) {
				ca_Disconnect_STPV(  pvCLTU[i].mCLOCK[j]);
				ca_Disconnect_STPV(  pvCLTU[i].mT0[j]);
				ca_Disconnect_STPV(  pvCLTU[i].mT1[j]);
			}
		}



	}

	//   setup DDS1  ***********
//	ca_Disconnect( pvDDS1A.DAQstop);
//	ca_Disconnect( pvDDS1A.ADCstop);
//	ca_Disconnect( pvDDS1A.RemoteSave);
//	ca_Disconnect( pvDDS1A.sampleRate);
//	ca_Disconnect( pvDDS1A.setT0);
//	ca_Disconnect( pvDDS1A.setT1);
//	ca_Disconnect( pvDDS1A.getReady);
#if 0
	for( int i=0; i<CNT_DDS1A_NO; i++) {
		ca_Disconnect_STPV( pvDDS1A[i].sampleRate);
		ca_Disconnect_STPV( pvDDS1A[i].setT0 );
		ca_Disconnect_STPV( pvDDS1A[i].setT1 );
/*		ca_Disconnect_STPV( pvDDS1A[i].divider ); */

	}
	for( int i=0; i<CNT_DDS1B_NO; i++) {
		ca_Disconnect_STPV( pvDDS1B[i].sampleRate);
		ca_Disconnect_STPV( pvDDS1B[i].setT0 );
		ca_Disconnect_STPV( pvDDS1B[i].setT1 );
	}
#endif
	//   setup HALPHA  ***********
/*	ca_Disconnect_STPV( pvHALPHA.DAQstop);
	ca_Disconnect_STPV( pvHALPHA.ADCstop);
	ca_Disconnect_STPV( pvHALPHA.RemoteSave); */
	ca_Disconnect_STPV( pvHALPHA.sampleRate);
	ca_Disconnect_STPV( pvHALPHA.setT0);
	ca_Disconnect_STPV( pvHALPHA.setT1);
/*	ca_Disconnect_STPV( pvHALPHA.getReady); */
/*	
	ca_Disconnect_STPV( pvECEHR.sampleRate);
	ca_Disconnect_STPV( pvECEHR.setT0);
	ca_Disconnect_STPV( pvECEHR.setT1);
*/


	//   another system relateion PV ***********
	ca_Disconnect_STPV( pvBlipTime);  /* 2010.08.17 */
	ca_Disconnect_STPV( pvCCS_ShotNumber);  /* 2012.01.17 */
	ca_Disconnect_STPV( pvCCS_SeqStart);  /* 2012.01.18 */
	ca_Disconnect_STPV( pvTSSConfirm);
	ca_Disconnect_STPV( pvTSS_CalMode);

}

bool MainWindow::pv_connection_checking()
{
	int connetCnt = 0;
	for(int sys=0; sys<MAX_LTU_CARD; sys++) 
	{
		if( pvCLTU[sys].SHOT.flag_cid) 
			connetCnt++;
	}

	if( connetCnt != MAX_LTU_CARD ) {
		printStatus(LEVEL_NONE, "connected/total = \"%d/%d\"\n", connetCnt, MAX_LTU_CARD);
		return false;
	}
	return true;	
}


void MainWindow::put_queue_node(ST_QUEUE_STATUS node)
{
	qthread->putQueueNode(node);
}

bool MainWindow::event( QEvent * pQEvent)
{
//	printf("bool MainWindow::event( QEvent * pQEvent)\n");
	if( pQEvent->type() == QEvent::KeyPress){
		QKeyEvent *ke = static_cast<QKeyEvent *>(pQEvent);
		if( ke->key() == Qt::Key_Delete )
		{
//			printf("del key pressed\n");
			if( stackedWidget_main->currentIndex() == STACKED_ID_TSS)
				pg_timeset.caput_removeCurrentRowItem();
			else if (stackedWidget_main->currentIndex() == STACKED_ID_R2)
				pg_localR2.event_removeCurrentRowItem();
			else if (stackedWidget_main->currentIndex() == STACKED_ID_MTRIG)
				pg_manyTrig.event_removeCurrentRowItem();
		}
		return true;
	}else{
		return QMainWindow::event( pQEvent);
	}
}

int MainWindow::checkPasswd()
{
	passwordDlg passdlg;
	if ( passdlg.exec() == QDialog::Accepted)
	{
		if( strcmp(STR_PASSWD, (passdlg.lineEdit_password->text()).toAscii().constData() ) == 0 ) {
			passwd_timeset_check = 0;
			return WR_OK;
		} 
		else if (strcmp(STR_PASSWD_CALIB, (passdlg.lineEdit_password->text()).toAscii().constData() ) == 0 ) {
			QMessageBox::information(0, "Kstar TSS", tr("got Master pass word!") );
			passwd_master_check = 0;
			passwd_timeset_check = 0;
			return WR_OK;
		} else {
			QMessageBox::information(0, "Kstar TSS", tr("wrong pass word!") );
			return WR_ERROR;

		}
		
	} else
		return WR_ERROR;

	return WR_ERROR;
}

void MainWindow::timerFunc_Connect()
{	
	static int ConnectTimerCnt = 0;

	if( ConnectTimerCnt == 0) {
		pv_connect_Init();
//		pv_connect_DDS1();
	}
	else if( ConnectTimerCnt == 1) 
		pv_connect_HALPHA();
	else if( ConnectTimerCnt == 2)
		pv_connect_LTU();
//	else if( ConnectTimerCnt == 3  ) ;		
//		update_spinBox_shotNum_minus_one();
	else if( ConnectTimerCnt >= 3) 
	{
		if( pv_connection_checking() || ConnectTimerCnt > 6) 
		{
			pv_monitoring_start();
			btn_start_monitoring->setEnabled(false);
			btn_stop_monitoring->setEnabled(true);

			timerConnect->stop();	
			delete timerConnect;
		}
	}
	
	ConnectTimerCnt++;

}

void MainWindow::update_spinBox_shotNum_minus_one()
{
	QString qstr;
	int iCAStatus = ca_Get_QStr( pvCCS_ShotNumber.cid, qstr );
	if( iCAStatus  != ECA_NORMAL ) {
		printStatus(LEVEL_ERROR, "%s, %s\n", pvCCS_ShotNumber.name, ca_message( iCAStatus) );
		return;
	}
	spinBox_stored_shot->setValue( qstr.toInt() - 1 );
}

void MainWindow::clicked_btn_timing_insert()
{
	pg_timeset.clicked_btn_timing_insert();
}
/*void MainWindow::clicked_btn_timing_done()
{
	pg_timeset.clicked_btn_timing_done();
}*/
/*
void MainWindow::idchanged_cb_timing_port(int ID)
{
	pg_timeset.idchanged_cb_timing_port(ID);
}
*/
/*
void MainWindow::idchanged_cb_timing_system(QString qstr)
{
	pg_timeset.idchanged_cb_timing_system(qstr);
}
*/
void MainWindow::itemChanged_tw_timingSet(QTableWidgetItem *curr)
{
	pg_timeset.itemChanged_tw_timingSet(curr);
}

void MainWindow::clicked_btn_bliptime_final_setup()
{
	pg_timeset.clicked_btn_bliptime_final_setup();
}
void MainWindow::clicked_btn_flush_LTUparam()
{
	pg_timeset.clicked_btn_flush_LTUparam();
//	qthread->call_pg_setup_CLTU_with_all_list();
}
void MainWindow::cellClicked_tw_load_info(int row, int column)
{
	pg_timeset.cellClicked_tw_load_info(row,  column);
}
void MainWindow::valueChanged_sb_stored_shot(int i)
{
	pg_timeset.pg_set_query_shot(i);
}
void MainWindow::clicked_btn_single_load()
{
	pg_timeset.clicked_btn_single_load();
}
void MainWindow::clicked_btn_reload()
{
	pg_timeset.clicked_btn_reload();
}





void MainWindow::clicked_btn_test_set()
{
	pg_localR1.clicked_btn_test_set();
}

void MainWindow::clicked_btn_test_shotStart()
{
	pg_localR1.clicked_btn_test_shotStart();
}
/*
void MainWindow::clicked_btn_cal_calStart()
{
	pg_localR1.clicked_btn_cal_calStart();
}

void MainWindow::textChanged_le_bliptime_my(QString qstr)
{
	pg_timeset.pg_set_bliptime_my( qstr.toInt() ); //.nBlip_time = qstr.toInt();
}
*/

void MainWindow::clicked_btn_showinfo_R2()
{
	pg_localR2.clicked_btn_showinfo_R2();
}
void MainWindow::clicked_btn_showTime_R2()
{
	pg_localR2.clicked_btn_showTime_R2();
}
void MainWindow::clicked_btn_showTLK_R2()
{
	pg_localR2.clicked_btn_showTLK_R2();
}
void MainWindow::clicked_btn_showLog_R2()
{
	pg_localR2.clicked_btn_showLog_R2();
}

void MainWindow::clicked_btn_test_setup_R2()
{
	pg_localR2.clicked_btn_test_setup_R2();
}

void MainWindow::clicked_btn_test_shotStart_R2()
{
	pg_localR2.clicked_btn_test_shotStart_R2();
}
void MainWindow::clicked_btn_test_shotStop_R2()
{
	pg_localR2.clicked_btn_test_shotStop_R2();
}
void MainWindow::clicked_btn_R2_setMode()
{
	pg_localR2.clicked_btn_R2_setMode();
}
void MainWindow::clicked_btn_R2_insert_item()
{
	pg_localR2.clicked_btn_R2_insert_item();
}
void MainWindow::itemChanged_tw_local_R2(QTableWidgetItem *curr)
{
	pg_localR2.itemChanged_tw_local_R2(curr);
}
void MainWindow::stateChanged_check_calib_admin(int state)
{
	pg_localR2.stateChanged_check_calib_admin(state);
}
void MainWindow::stateChanged_check_TSS_cal_mode(int state)
{
	pg_localR2.stateChanged_check_TSS_cal_mode(state);
}
void MainWindow::clicked_btn_put_LTU_calib_value()
{
	pg_localR2.clicked_btn_put_LTU_calib_value();
}
void MainWindow::clicked_btn_R2_open()
{
	pg_localR2.clicked_btn_R2_open();
}
void MainWindow::clicked_btn_R2_save()
{
	pg_localR2.clicked_btn_R2_save();
}

void MainWindow::clicked_btn_mTrig_group_setup()
{
	pg_manyTrig.clicked_btn_mTrig_group_setup();
}
void MainWindow::clicked_btn_mTrig_pv_setup()
{
	pg_manyTrig.clicked_btn_mTrig_pv_setup();
}
void MainWindow::clicked_btn_mTrig_list_clear()
{
	pg_manyTrig.clicked_btn_mTrig_list_clear();
}
void MainWindow::clicked_btn_mTrig_Connect()
{
	pg_manyTrig.clicked_btn_mTrig_Connect();
}
void MainWindow::clicked_btn_mTrig_Disconnect()
{
	pg_manyTrig.clicked_btn_mTrig_Disconnect();
}
void MainWindow::clicked_btn_mTrig_start()
{
	pg_manyTrig.clicked_btn_mTrig_start();
}
void MainWindow::clicked_btn_mTrig_stop()
{
	pg_manyTrig.clicked_btn_mTrig_stop();
}


