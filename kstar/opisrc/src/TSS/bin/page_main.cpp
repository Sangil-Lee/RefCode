#include <QtGui>

#include "page_main.h"

#include "mainWindow.h"


//#define pmaincb		((MainWindow*)pCltuMain->pm)

//evid evidMain[100];
//int cntEvidMain=0;

#define T_DAY_DIVIDER  (24*60*60)
#define T_HOUR_DIVIDER (60*60)
#define T_MIN_DIVIDER  (60)
#define T_SEC_DIVIDER  (1)

#include "ltuconfigure.h"

using namespace mysqlpp;

CltuMain::CltuMain()
{
	pm = NULL;
//	timerOneSec = NULL;
//	nDDS1ShotStart = nDDS2ShotStart =0;
//	dds1ShotEnd = dds2ShotEnd = 0;

//	cntEvidMain=0;

	QBrush brush_blip_changed(QColor(255, 0, 0, 255));
	QBrush brush_blip_changed2(QColor(255, 255, 0, 255));
	QBrush brush_normal(QColor(222, 222, 222, 255));
	brush_blip_changed.setStyle(Qt::SolidPattern);
	brush_normal.setStyle(Qt::SolidPattern);

	pal_blip_change.setBrush(QPalette::Active, QPalette::Button, brush_blip_changed);
	pal_blip_change.setBrush(QPalette::Active, QPalette::ButtonText, QBrush::QBrush(QColor(0,0,0,255)) );
	pal_blip_change.setBrush(QPalette::Active, QPalette::Window, brush_blip_changed);
	pal_blip_change.setBrush(QPalette::Inactive, QPalette::Button, brush_blip_changed);
	pal_blip_change.setBrush(QPalette::Inactive, QPalette::Window, brush_blip_changed);

	pal_blip_change2.setBrush(QPalette::Active, QPalette::Button, brush_blip_changed2);
	pal_blip_change2.setBrush(QPalette::Active, QPalette::ButtonText, QBrush::QBrush(QColor(0,0,0,255)) );
	pal_blip_change2.setBrush(QPalette::Active, QPalette::Window, brush_blip_changed2);
	pal_blip_change2.setBrush(QPalette::Inactive, QPalette::Button, brush_blip_changed2);
	pal_blip_change2.setBrush(QPalette::Inactive, QPalette::Window, brush_blip_changed2);
	
//	pal_blip_change.setBrush(QPalette::Disabled, QPalette::Button, brush_blip_changed);
//	pal_blip_change.setBrush(QPalette::Disabled, QPalette::Window, brush_blip_changed);

	pal_normal.setBrush(QPalette::Active, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Active, QPalette::Window, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Button, brush_normal);
	pal_normal.setBrush(QPalette::Inactive, QPalette::Window, brush_normal);
//	pal_normal.setBrush(QPalette::Disabled, QPalette::Button, brush_normal);
//	pal_normal.setBrush(QPalette::Disabled, QPalette::Window, brush_normal);


}

CltuMain::~CltuMain()
{
//	if( timerOneSec ){
//		timerOneSec->stop();
//		timerOneSec = NULL;
//	}
}

void CltuMain::destroyPageMain()
{
//	if( timerOneSec ){
//		timerOneSec->stop();
//		timerOneSec = NULL;
//	}
//	usleep(1100000);
//	printf("destroy page_main\n");
}

void CltuMain::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
	
	MainWindow *pWnd = (MainWindow*)pm;

//	InitTW_OpSeq();
	pWnd->tw_OpSeq->horizontalHeader()->resizeSection(0, 150);
	pWnd->tw_OpSeq->horizontalHeader()->resizeSection(1, 100);
	pWnd->tw_OpSeq->horizontalHeader()->resizeSection(2, 100);

#if 1
	timer_Blip_Changed = new QTimer(this);
	connect(timer_Blip_Changed, SIGNAL(timeout()), this, SLOT(timerFunc_OneSec()));
//	timerOneSec->start(1000);
//	pmain->printStatus(LEVEL_NONE, "DAQ Timer: started" );
#endif

//	connect(this, SIGNAL(accepted()), this, SLOT( myAccepted() ));

	string dbname = "LTU";
//	string dbserverhost = "172.17.100.204"; // web server
	string dbserverhost = "172.17.100.212"; // RDB
	string user = "kstar";
	string passwd = "kstar@)12";
	if( get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
	}
	if( !mysqlkstardb.Connect_to_db(dbname, dbserverhost, user, passwd, m_con) ) {
		pWnd->printStatus(LEVEL_ERROR, "MySQL Database connection failed! check again.");
		pWnd->flag_MySQL_con = 0;
	} else {
		pWnd->printReturn(LEVEL_NOTICE, "MySQL Database connection OK!");
		pWnd->flag_MySQL_con = 1;
	}

}

void CltuMain::updateTableWidget()
{
	MainWindow *pWnd = (MainWindow*)pm;

	int row, rowmain;

	// clear all data that saved before.....
	row = pWnd->tw_OpSeq->rowCount();
//	printf("\ncurrent row counter: %d\n", row);
	if( row > 0 ) {
		for(int i=0; i<row; i++) {
			pWnd->tw_OpSeq->removeRow ( 0 );
		}
	}
//	rowmain = pWnd->tw_OpSeq->rowCount();
//	printf("row counter after remove all: %d\n", rowmain);
	// clear all data that saved before.....
//	pmain->tw_OpSeq->clear ();


	row = pWnd->tw_machine->rowCount();
	for(int i=0; i<row; i++) {
		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		item1->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);

		item1->setText(pWnd->tw_machine->item(i, 1)->text() ); // signal description
		item2->setText(pWnd->tw_machine->item(i, 4)->text() ); // sec1 start time
		item3->setText(pWnd->tw_machine->item(i, 5)->text() ); // sec1 stop time
		item1->setBackground(pWnd->tw_machine->item(i, 1)->background ());
		item2->setBackground(pWnd->tw_machine->item(i, 4)->background ());
		item3->setBackground(pWnd->tw_machine->item(i, 5)->background ());

		rowmain = pWnd->tw_OpSeq->rowCount();
		pWnd->tw_OpSeq->setRowCount(rowmain + 1);

		pWnd->tw_OpSeq->setItem(rowmain, 0, item1);
		pWnd->tw_OpSeq->setItem(rowmain, 1, item2);
		pWnd->tw_OpSeq->setItem(rowmain, 2, item3);
	}

	row = pWnd->tw_diagnostics->rowCount();
	for(int i=0; i<row; i++) {
		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		item1->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);

		item1->setText(pWnd->tw_diagnostics->item(i, 1)->text() ); // signal description
		item2->setText(pWnd->tw_diagnostics->item(i, 4)->text() ); // sec1 start time
		item3->setText(pWnd->tw_diagnostics->item(i, 5)->text() ); // sec1 stop time
		item1->setBackground(pWnd->tw_diagnostics->item(i, 1)->background ());
		item2->setBackground(pWnd->tw_diagnostics->item(i, 4)->background ());
		item3->setBackground(pWnd->tw_diagnostics->item(i, 5)->background ());

		rowmain = pWnd->tw_OpSeq->rowCount();
		pWnd->tw_OpSeq->setRowCount(rowmain + 1);

		pWnd->tw_OpSeq->setItem(rowmain, 0, item1);
		pWnd->tw_OpSeq->setItem(rowmain, 1, item2);
		pWnd->tw_OpSeq->setItem(rowmain, 2, item3);
	}

	row = pWnd->tw_ltu2->rowCount();
	for(int i=0; i<row; i++) {
		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		item1->setTextAlignment(Qt::AlignLeft);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);

		item1->setText(pWnd->tw_ltu2->item(i, COL_ID_DESC)->text() ); // signal description
		item2->setText(pWnd->tw_ltu2->item(i, COL_ID2_1_T0)->text() ); // sec1 start time
		item3->setText(pWnd->tw_ltu2->item(i, COL_ID2_1_T1)->text() ); // sec1 stop time
		item1->setBackground(pWnd->tw_ltu2->item(i, COL_ID_DESC)->background ());
		item2->setBackground(pWnd->tw_ltu2->item(i, COL_ID2_1_T0)->background ());
		item3->setBackground(pWnd->tw_ltu2->item(i, COL_ID2_1_T1)->background ());

		rowmain = pWnd->tw_OpSeq->rowCount();
		pWnd->tw_OpSeq->setRowCount(rowmain + 1);

		pWnd->tw_OpSeq->setItem(rowmain, 0, item1);
		pWnd->tw_OpSeq->setItem(rowmain, 1, item2);
		pWnd->tw_OpSeq->setItem(rowmain, 2, item3);
	}

	row = pWnd->tw_ltu2_diag->rowCount();
	for(int i=0; i<row; i++) {
		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		item1->setTextAlignment(Qt::AlignLeft);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);

		item1->setText(pWnd->tw_ltu2_diag->item(i, COL_ID_DESC)->text() ); // signal description
		item2->setText(pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T0)->text() ); // sec1 start time
		item3->setText(pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T1)->text() ); // sec1 stop time
		item1->setBackground(pWnd->tw_ltu2_diag->item(i, COL_ID_DESC)->background ());
		item2->setBackground(pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T0)->background ());
		item3->setBackground(pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T1)->background ());

		rowmain = pWnd->tw_OpSeq->rowCount();
		pWnd->tw_OpSeq->setRowCount(rowmain + 1);

		pWnd->tw_OpSeq->setItem(rowmain, 0, item1);
		pWnd->tw_OpSeq->setItem(rowmain, 1, item2);
		pWnd->tw_OpSeq->setItem(rowmain, 2, item3);
	}
	

}


/*
void CltuMain::clicked_main_dds1_port(int id)
{
	if( id < 4 ) {
		pmain->label_main_dds1T0->setText( QString("%1").arg(pmain->cfgDDS1.nT0[id]).toAscii().constData() );
		pmain->label_main_dds1T1->setText( QString("%1").arg(pmain->cfgDDS1.nT1[id]).toAscii().constData() );
		pmain->label_main_dds1Multi->setText(tr("Single") );
	} else if (id == 4)
	{
		pmain->label_main_dds1T0->setText( QString("0").arg(pmain->cfgDDS1.nT0[id]).toAscii().constData() );
		pmain->label_main_dds1T1->setText( QString("0").arg(pmain->cfgDDS1.nT1[id]).toAscii().constData() );
		pmain->label_main_dds1Multi->setText(tr("Multi") );
	}
}

void CltuMain::clicked_main_dds2_port(int id)
{
	if( id < 4 ) {
		pmain->label_main_dds2T0->setText( QString("%1").arg(pmain->cfgDDS2.nT0[id]).toAscii().constData() );
		pmain->label_main_dds2T1->setText( QString("%1").arg(pmain->cfgDDS2.nT1[id]).toAscii().constData() );
		pmain->label_main_dds2Multi->setText(tr("Single") );
	} else if (id == 4)
	{
		pmain->label_main_dds2T0->setText( QString("0").arg(pmain->cfgDDS2.nT0[id]).toAscii().constData() );
		pmain->label_main_dds2T1->setText( QString("0").arg(pmain->cfgDDS2.nT1[id]).toAscii().constData() );
		pmain->label_main_dds2Multi->setText(tr("Multi") );
	}
}
*/


	
void CltuMain::cb_func_blipMonitor(struct event_handler_args args)
{
//	CltuMain *pCltuMain = (CltuMain *)args.usr;
	MainWindow* pWnd = (MainWindow *)args.usr;
//	int iCAStatus;


//	pCltuMain->mutexEventCB.lock();

	double dval=0.0;
	int prev_bliptime_ccs=0;

//	pCltuMain->chidEventCB = args.chid;

//	printf("i'm in cb_func_blipMonitor\n");

//	if( args.count != ( long) ca_element_count( chID ) ){
//		QMessageBox::information(0, "cb_func_blipMonitor", tr("event_handler_args.count is not correct!") );
//		return;
//	}
	if( args.status != ECA_NORMAL){
		QMessageBox::information(0, "cb_func_blipMonitor", tr( "event_handler_args.status is not OK") );
		return;
	}
		
	union db_access_val * pUData = ( db_access_val *) args.dbr;
	
	switch( args.type){
		case DBR_TIME_DOUBLE:
//			pCltuMain->qstrEventCB = QString( "%1").arg( *( &( pUData->tdblval.value) ) );
//			printf("got double value %s, %lf\n", pCltuMain->qstrEventCB.toAscii().constData(), *( &( pUData->tdblval.value)) );
//			pCltuMain->dbCallbackVal = *( &( pUData->tdblval.value));
			dval = *( &( pUData->tdblval.value));
//			printf("got double value %lf\n", dval );
			break;
		default:
			pWnd->printReturn(LEVEL_ERROR, "CltuMain: got a not defined data type!!!\n");
			break;
	}
	prev_bliptime_ccs = pWnd->pg_timeset.pg_get_bliptime_ccs();
	if( (int)dval != prev_bliptime_ccs ) {		
		pWnd->pg_timeset.pg_set_bliptime_ccs((int)dval);
		pWnd->confirm_final_setup = 0;
		pWnd->printReturn(LEVEL_WARNING, "PCS blip time changed, %d -> %d\n", prev_bliptime_ccs, (int)dval);

		if( !pWnd->pg_main.tmr_blip_timer_isActive() ) {
//			pWnd->printStatus(0, "blip check timer start");
			pWnd->pg_main.tmr_blip_timer_start();
		}
	}

//	QEvent * pQEvent = new QEvent( QEvent::User);
//	QCoreApplication::postEvent( pCltuMain, pQEvent);

//	pCltuMain->mutexEventCB.unlock();
}

bool CltuMain::tmr_blip_timer_isActive()
{
	return timer_Blip_Changed->isActive();
}
void CltuMain::tmr_blip_timer_start()
{
	timer_Blip_Changed->start(300);
}
	


bool CltuMain::check_pv_access(chid mypv)
{
	if( ca_write_access(mypv) && ca_read_access( mypv) ){
		return true;
	}
	return false;
}

int CltuMain::cb_register_blipFunc(ST_PV_NODE &node)
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	
	node.flag_eid= 0;
//	if( !check_pv_access( node.cid) ) {
//		pmain->printStatus(LEVEL_ERROR,"ERROR: %s not access\n", node.name );
//		return;
//	}
	if( !node.flag_cid ) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\"  connection\n", node.name );
		return WR_ERROR;
	}

	iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( node.cid), node.cid,
				cb_func_blipMonitor, pWnd, 0.0, 0.0, 0.0, &node.eid, DBE_VALUE);
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "cb_register_blipFunc",  tr("ca_add_masked_array_event() error!"));
		return WR_ERROR;
	}

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "cb_register_blipFunc", tr("ca_pend_io() error!"));
		return WR_ERROR;
	}
	node.flag_eid= 1;
	node.status_flag = PV_STATUS_FIRST_EVENT;

	return WR_OK;
}

void CltuMain::cb_clear_blipFunc(ST_PV_NODE &node)
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	if( node.flag_eid == 0 ) return;
	
	if( !check_pv_access( node.cid) ) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: %s not access\n", node.name );
		return;
	}
	
	iCAStatus = ca_clear_event( node.eid );
	if( iCAStatus != ECA_NORMAL)
		QMessageBox::information(0, "cb_clear_blipFunc", tr("ca_clear_event() error!"));

	node.eid = NULL;
	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL)
		QMessageBox::information(0, "cb_clear_blipFunc", tr("ca_pend_io() error!"));

	node.flag_eid = 0;
	node.status_flag = PV_STATUS_FIRST_EVENT;
}

#if 0
QString CltuMain::sec2HHMMSS(unsigned int timer_sec)
{
	unsigned int day, hour, min, sec;
	QString strTime; //, strH, strM, strS;
	char strBuf[16];
	
	
	if( timer_sec == 0) return QString("HH:MM:SS");

	day = (int) (timer_sec/T_DAY_DIVIDER);
	timer_sec = timer_sec - (day * T_DAY_DIVIDER);
	hour = (int) (timer_sec/T_HOUR_DIVIDER );
	timer_sec = timer_sec - (hour * T_HOUR_DIVIDER);
	min = (int) (timer_sec/T_MIN_DIVIDER );
	timer_sec = timer_sec - (min * T_MIN_DIVIDER);
	sec = (int) (timer_sec/T_SEC_DIVIDER );
/*
	strH = (hour <10 ) ? QString("0%1:").arg(hour) : QString("%1:").arg(hour);
	strM = (min <10 ) ? QString("0%1:").arg(min) : QString("%1:").arg(min);
	strS = (sec <10 ) ? QString("0%1").arg(sec) : QString("%1").arg(sec);
	strTime = strH + strM + strS;
*/
	sprintf(strBuf, "%02d:%02d:%02d", hour, min, sec);

	strTime = QString("%1").arg(strBuf);

	return strTime;
}
#endif

#if 0
void CltuMain::timerFuncOneSec()
{

	static int dds1Cnt = 0;
	static int dds2Cnt = 0;

//	dds1ready = ca_Get_QStr(pmain->pvDDS1A.getReady).toInt();
//	dds2ready = ca_Get_QStr(pmain->pvHALPHA.getReady).toInt();

	if( nDDS1ShotStart ) {		
		if( (dShotStartTimeSecDDS1 + pmain->dcurDDS1T1) < pmain->unCurrentSysTime[DDS1A_LTU] ) {
			// stop DAQ, ADC, save.
			ca_Put_QStr((pmain->pvDDS1A.DAQstop, QString("1") );
			dds1ShotEnd = 1;
			nDDS1ShotStart = 0;
			dds1Cnt = 0;
		}
	}
	if( nDDS2ShotStart ) {
		pmain->printStatus(LEVEL_NONE,"1 sec daq dds2");
		if( (dShotStartTimeSecDDS2 + pmain->dcurDDS2T1) < pmain->unCurrentSysTime[HALPHA_LTU] ) {
			// stop DAQ, ADC, save.
			ca_Put_QStr((pmain->pvHALPHA.DAQstop, QString("1") );
			dds2ShotEnd = 1;
			nDDS2ShotStart = 0;
			dds2Cnt = 0;
			pmain->printStatus(LEVEL_NONE,"daq stopped dds2");
		}
	}


	if( dds1ShotEnd ){
		if( dds1Cnt == 1 ) {
			ca_Put_QStr((pmain->pvDDS1A.ADCstop, QString("1") );
			pmain->printStatus(LEVEL_NONE,"DDS1 ADCstop");
		}
		else if( dds1Cnt == 2) {
			ca_Put_QStr((pmain->pvDDS1A.RemoteSave, QString("1") );
			pmain->printStatus(LEVEL_NONE,"DDS1 RemoteSave");
		}
		else if( dds1Cnt == 3) {
			dds1Cnt = 0;
			dds1ShotEnd = 0;
//			ca_Put_QStr((pmain->pvDDS1A.getReady, QString("0") );
//			return;
		}
		dds1Cnt++;
	}

	if( dds2ShotEnd ){
//		printf("dds2Cnt : %d\n", dds2Cnt);
		if( dds2Cnt == 1 ) {
			ca_Put_QStr((pmain->pvHALPHA.ADCstop, QString("1") );
			pmain->printStatus(LEVEL_NONE,"DDS2 ADCstop");
		}
		else if( dds2Cnt == 2) {
			ca_Put_QStr((pmain->pvHALPHA.RemoteSave, QString("1") );
			pmain->printStatus(LEVEL_NONE,"DDS2 RemoteSave");
		}
		else if( dds2Cnt == 3) {
			dds2Cnt = 0;
			dds2ShotEnd = 0;
//			ca_Put_QStr((pmain->pvHALPHA.getReady, QString("0") );
//			return;
		} 
		dds2Cnt++;
	}

/*
	pmain->lb_main_ccsTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[CTUV1] )  );
//	pmain->lb_main_ctuTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[CTUV1] )  );
	pmain->lb_main_dds1Time->setText( sec2HHMMSS( pmain->unCurrentSysTime[DDS1A_LTU] ) );
	pmain->lb_main_dds2Time->setText( sec2HHMMSS( pmain->unCurrentSysTime[HALPHA_LTU] ) );
	pmain->lb_main_pcsTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[PCS_LTU] ) );
	pmain->lb_main_mps1Time->setText( sec2HHMMSS( pmain->unCurrentSysTime[MPS1_LTU] ) );
	pmain->lb_main_mps2Time->setText( sec2HHMMSS( pmain->unCurrentSysTime[MPS2_LTU] ) );
	pmain->lb_main_echTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[ECH_LTU] ) );
	pmain->lb_main_icrhTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[ICRH_LTU] ) );
// Add to by TG.Lee 20080227
	pmain->lb_main_fuelTime->setText( sec2HHMMSS( pmain->unCurrentSysTime[FUEL_LTU] ) );
*/
}
#endif

void CltuMain::timerFunc_OneSec()
{
	MainWindow *pWnd = (MainWindow*)pm;
	static int toggle = 1;
	static int blip_confirm = 1;

	if(  pWnd->pg_timeset.pg_get_bliptime_ccs() != pWnd->pg_timeset.pg_get_bliptime() ) {
		blip_confirm = 0;

		if( toggle )
			pWnd->btn_flush_LTUparam->setPalette(pal_blip_change);
		else
			pWnd->btn_flush_LTUparam->setPalette(pal_blip_change2);

	} else {
		blip_confirm = 1;
		if( pWnd->confirm_final_setup )
			timer_Blip_Changed->stop();
		pWnd->btn_flush_LTUparam->setPalette(pal_normal);
	}

	if( pWnd->confirm_final_setup == 0 ) {
		toggle = toggle ? 0:1;
		
		if( toggle )
			pWnd->btn_final_setup->setPalette(pal_blip_change);
		else
			pWnd->btn_final_setup->setPalette(pal_blip_change2);
		
	} else {
		if( blip_confirm )
			timer_Blip_Changed->stop();
		pWnd->btn_final_setup->setPalette(pal_normal);
		toggle = 1;
	}
	
}

void CltuMain::make_blinking_final_setup()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	iCAStatus=ca_Put_QStr(	pWnd->pvTSSConfirm.cid, QString( "0") ); // Notify confirmed to CCS
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" put 0,  %s\n", pWnd->pvTSSConfirm.name, ca_message( iCAStatus) );
		return;
	}
	pWnd->confirm_final_setup = 0;
	if( !tmr_blip_timer_isActive() ) {
//		pWnd->printStatus(0, "blip check timer start");
		tmr_blip_timer_start();
	}

}

void CltuMain::cb_thread_updateMainStack(ST_QUEUE_STATUS qnode)
{
	
	MainWindow *pWnd = (MainWindow*)pm;
		
	char sysName[8];
//	int iCAStatus;


//	if( nPVblockMode == USER_RESET_ALL_SYS )
//		return;


//	printf("sys: %d, val: %f: mode:%d, type:%d, Port:%d\n", qnode.pEventArg->system, qnode.dval, qnode.pEventArg->mode, qnode.pEventArg->type, qnode.pEventArg->port );
	strcpy(sysName, pWnd->str_sysName[ qnode.pEventArg->system ] );
//	printf("sys:%s\n", sysName );
//	if( !strcmp(sysName, pWnd->str_sysName[ TSS_CTU]) ) 
//	{
//		pWnd->lb_CTU_COMP_TICK->setText( QString( "%1").arg( qnode.dval  ) );
//		return;
//	}

/*
	if( qnode.pEventArg->type != TYPE_R2_SET_FREE_RUN ) 
	{
		pWnd->printStatus(LEVEL_ERROR,"TYPE_R2_SET_FREE_RUN error!" );
		return; 
	}
*/

	switch( qnode.pEventArg->type )
	{
		case TYPE_R2_SET_FREE_RUN:
			if( qnode.dval == 1 )
				pWnd->printReturn(LEVEL_WARNING,"\"%s\" standalone mode !!!", sysName);
			else
				pWnd->printReturn(LEVEL_ERROR,"\"%s\" changed to synch run !!!", sysName);
				
			break;
		case TYPE_SHOTSEQ_START:
			if( !pWnd->flag_MySQL_con ) break;

			if( qnode.dval == 1 ) {
//				pWnd->printReturn(LEVEL_NONE, "Sequence started !!!");
//				save_current_ltu_setup();
				if( pWnd->Is_seq_save() )
				{
					pWnd->callSignal_saveLTUparam();
				}
				else 
					pWnd->printReturn(LEVEL_WARNING, "Sequence started, but archiving not enabled!");
					
			}
			else {
				pWnd->printReturn(LEVEL_NONE, "Sequence stopped !!!");
				usleep(10000);
				pWnd->callSignal_updateSpinBox();
				}

			break;			
		default: 
			pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied type: %d\n", qnode.pEventArg->type );
			break;
	}

//	pWnd->update();  // doesn't work.
//	pTableWidget->repaint();


}

/*******************************************************************************
Company   : NFRI
Function  : cb_register_callbackFunc
Author    : Woong
Parameter : 
	nsys = system ID
	mode = single  or multi trigger
	type = all parameter type
	port = channel ID 0,1,2,3,4

Return    : .
Desc      : 

Date      : 
*******************************************************************************/
int CltuMain::cb_register_main(ST_PV_NODE &node, unsigned int nsys, int mode, int type, int port, int arg)
{
	MainWindow* pWnd = (MainWindow *)pm;

	int iCAStatus;
	ST_ARG_EVENT_CB *myargs;

	node.flag_eid= 0;
//	if( !check_pv_access( node.cid) ) {
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: %s not access\n", node.name );
//		return;
//	}
	if( !node.flag_cid ) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\"  connection\n", node.name );
		return WR_ERROR;
	}

	myargs = (ST_ARG_EVENT_CB *)malloc(sizeof( ST_ARG_EVENT_CB  ) );
	if( !myargs ){
		pWnd->printStatus(LEVEL_ERROR,"register monitor callback : malloc (%s) failed!\n", pWnd->str_sysName[nsys]);
		return WR_ERROR;
	}

	myargs->pMain= pWnd;
	myargs->system = nsys;
	myargs->mode = mode;
	myargs->type = type;
	
	myargs->port = port;
	myargs->arg = arg;
	strcpy( myargs->pvname , node.name );

//	printf("%s, sys:%d, mode:%d, type:%d, port:%d, arg:%d\n", myargs->pvname, myargs->system,
//		myargs->mode, myargs->type, myargs->port, myargs->arg );


	//DBR_TIME_DOUBLE
	iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( node.cid ), node.cid,
				cb_func_main, myargs, 0.0, 0.0, 0.0, &node.eid, DBE_VALUE);
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "CltuMain::cb_register_callbackFunc",  tr("ca_add_masked_array_event() error!"));
		free(myargs);
		return WR_ERROR;
	}
//	cntEvidMain++;

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "CltuMain::cb_register_callbackFunc", tr("ca_pend_io() error!"));
		free(myargs);
		return WR_ERROR;
	}
	node.flag_eid= 1;
//	node.first_event = 1;
	node.status_flag = PV_STATUS_FIRST_EVENT;

	return WR_OK;

}

void CltuMain::cb_clear_callbackFunc(ST_PV_NODE &node)
{
	int iCAStatus;
	MainWindow* pWnd = (MainWindow *)pm;

	if( node.flag_eid == 0 ) return;
	
	if( !check_pv_access( node.cid) ) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: %s not access\n", node.name );
		return;
	}

	iCAStatus = ca_clear_event( node.eid );
	if( iCAStatus != ECA_NORMAL)
		QMessageBox::information(0, "CltuMain::cb_clear_callbackFunc", tr("ca_clear_event() error!"));
	
	node.eid = NULL;
	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL)
		QMessageBox::information(0, "CltuMain::cb_clear_callbackFunc", tr("ca_pend_io() error!"));
	
	node.flag_eid = 0;
//	node.first_event = 0;
	node.status_flag = PV_STATUS_FIRST_EVENT;

}

void CltuMain::cb_func_main(struct event_handler_args args)
{
//	CltuTimingSet *pwnd = (CltuTimingSet *)args.usr;

	ST_QUEUE_STATUS queueNode;

	ST_ARG_EVENT_CB *pargs = (ST_ARG_EVENT_CB *)args.usr;

	MainWindow* pWnd = (MainWindow *)pargs->pMain;


//	pwnd->mutexEventCB.lock();

//	pwnd->chidEventCB = args.chid;

//	pWnd->printStatus(LEVEL_NONE,"i'm in callbackMonitor\n");

#if 0
	if( pWnd->pg_timeset.pSTmng->check_is_first_event(pargs) == 1 ){
//		printf("first event\n");
		/* we got first event */
		return;
	}
#endif
//	printf("got event\n"); // 100% got the message... here..
	

	if( args.count != ( long) ca_element_count( args.chid ) ){
		pWnd->printStatus(LEVEL_NONE,"CltuTimingSet >> event_handler_args.count is not correct!" );
		return;
	}

	if( pargs->type == TYPE_R2_GET_COMP_NS)
		queueNode.caller = QUEUE_ID_CALIBRATION;
	
	else if( pargs->type == TYPE_R2_SET_FREE_RUN)
		queueNode.caller = QUEUE_ID_MAINWND;
	
	else if( pargs->type == TYPE_SHOTSEQ_START)		
		queueNode.caller = QUEUE_ID_MAINWND;
	
	else 
		queueNode.caller = QUEUE_ID_TIMESET;
	
	queueNode.pEventArg = pargs;
	queueNode.dval = 0.0;
//	strcpy( queueNode.pvname , pargs.pvname );

	if( args.status != ECA_NORMAL){
		QMessageBox::information(0, "callbackMonitor", tr( "event_handler_args.status is not OK") );
		return;
	}

	union db_access_val * pUData = ( db_access_val *) args.dbr;

	switch( args.type){
//		case DBR_TIME_STRING:
//			pCltuMain->qstrEventCB = QString( "%1").arg( *( &( pUData->tstrval.value) ) );
//			break;
		case DBR_TIME_DOUBLE:
//			pwnd->qstrEventCB = QString( "%1").arg( *( &( pUData->tdblval.value) ) );
//			printf("got double value %s, %lf\n", pwnd->qstrEventCB.toAscii().constData(), *( &( pUData->tdblval.value)) );
//			pwnd->dbCallbackVal = *( &( pUData->tdblval.value));
			queueNode.dval = *( &( pUData->tdblval.value));
			
			break;
		default: 
			pWnd->printStatus(LEVEL_NONE,"CltuTimingSet: got a not defined data type!!!\n");
			break;
	}

	
//	pWnd->printStatus(LEVEL_NONE,"send queue node \n");
	pWnd->put_queue_node(queueNode);

//	QEvent * pQEvent = new QEvent( QEvent::User);
//	QCoreApplication::postEvent( pwnd, pQEvent);
//	pwnd->mutexEventCB.unlock();
}

// file save
#if 0
void CltuMain::save_current_ltu_setup()
{
	QString qstr;
	double dbVal;
	int iCAStatus ;
	MainWindow *pWnd = (MainWindow*)pm;
	pWnd->printReturn(LEVEL_NONE, "Sequence started !!!");
	FILE *fp;
	char filename[32];
	char strDesc[64];

	char sysname[6];
	int id, nblipTime;

//	sprintf(filename, "test.txt");

	iCAStatus = ca_Get_QStr( pWnd->pvCCS_ShotNumber.cid, qstr );
	if( iCAStatus  != ECA_NORMAL ) sprintf(filename, "%s/xxx.dat", STR_FILEOPEN_DIR);
	else sprintf(filename, "%s/%d.dat", STR_FILEOPEN_DIR, qstr.toInt());



	nblipTime = pWnd->pg_timeset.pg_get_bliptime();
	
	if( (fp = fopen(filename, "w") ) == NULL )
	{	
		pWnd->printStatus(LEVEL_NONE,"Can not open \"%s\" file for write\n", filename);
		return;
	}
	
	fprintf(fp, "*TSS*\n");
//	fprintf(fp, "# system, description,    port, pol,    start(s), stop(s), clock(KHz), ...\n");
	fprintf(fp, "# enclosed, system, description, Ch, (onoff, C/T), polarity, T0, T1, Clock(KHz), ... \n");
	fprintf(fp, "# time is offset \n");
	fprintf(fp, "# Time is Offset from Blip time. \n");


	fprintf(fp, "& Blip_time: %d\n", nblipTime);
	
	for( int sys=0; sys<MAX_LTU_CARD; sys++) 
	{
		if( ca_read_access( pWnd->pvCLTU[sys].SHOT.cid ) ) 
		{	
//			fprintf(fp, "%s connected(%d)\n", pWnd->str_sysName[sys], sys);

			if( pWnd->Is_New_R2_id(sys) ) 
			{ 
				for(int j=0; j<R2_PORT_CNT; j++) 
				{
					fprintf(fp, "%d ", pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) );
					
					fprintf(fp, "%s ", pWnd->str_sysName[sys]);

					pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], j, strDesc);
					fprintf(fp, "\"%s\" ", strDesc);
			
					/* Channel Index */
					fprintf(fp, "%d ", j);

					/* Enable: 1, Disable: 0 */
					iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_Enable[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_Enable[j].name);
					fprintf(fp, "%d ", qstr.toInt());
					
					/* Clock: 0,  Trigger: 1 */
					iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_Mode[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_Mode[j].name);
					if( qstr.toInt() ) fprintf(fp, "T ");
					else fprintf(fp, "C "); 
//					fprintf(fp, "%d ", qstr.toInt());
					

					/* Active High: 0(R),  Active Low: 1(F) */
					iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_ActiveLow[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_ActiveLow[j].name);
//					fprintf(fp, "%d \t", qstr.toInt());
					if( qstr.toInt() ) fprintf(fp, "F ");
					else fprintf(fp, "R "); 


					for(int k=0; k<R2_SECT_CNT; k++) 
					{
						/* Absolute Start time T0: sec */
						iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].R2_T0[j][k].cid, dbVal );
						if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_T0[j][k].name);
						fprintf(fp, "%lf ", dbVal - nblipTime );

						/* Absolute End time T1: sec */
						iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].R2_T1[j][k].cid, dbVal );
						if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_T1[j][k].name);
						fprintf(fp, "%lf ", dbVal - nblipTime );
						
						/* Section Clock Hz */
						iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_CLOCK[j][k].cid, qstr );
						if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_CLOCK[j][k].name);
						fprintf(fp, "%.3f ", qstr.toFloat() /1000.0);

					}
					
					/* Next line */
					fprintf(fp, "\n");
					
				}
				
			}
			else 
			{
				for( int j=0; j<4; j++) 
				{
					fprintf(fp, "%d ", pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) );
					
					fprintf(fp, "%s ", pWnd->str_sysName[sys]);
					
					pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], j, strDesc);
					fprintf(fp, "\"%s\" ", strDesc);
			
					/* Channel Index */
					fprintf(fp, "%d ", j);
//					fprintf(fp, "\tPort: %d\n", j);


					/* Active High: 0(R),  Active Low: 1(F) */
					iCAStatus = ca_Get_QStr(pWnd->pvCLTU[sys].POL[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].POL[j].name);
//					fprintf(fp, "%d \t", qstr.toInt());
					if( qstr.toInt() ) fprintf(fp, "F ");
					else fprintf(fp, "R "); 

					
					/* Absolute Start time T0: sec */
					iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].T0[j].cid, dbVal );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].T0[j].name);
					fprintf(fp, "%lf ", dbVal - nblipTime );

					/* Absolute End time T1: sec */
					iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].T1[j].cid, dbVal );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].T1[j].name);
					fprintf(fp, "%lf ", dbVal - nblipTime );
					
					/* Section Clock KHz */
					iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].CLOCK[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].CLOCK[j].name);
					fprintf(fp, "%.3f ", qstr.toFloat() /1000.0);
					

					/* Next line */
					fprintf(fp, "\n");

				}


				fprintf(fp, "%d ", pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], 4) );
				
				fprintf(fp, "%s ", pWnd->str_sysName[sys]);
					
				pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], 4, strDesc);
				fprintf(fp, "\"%s\" ", strDesc);
		
				/* Last multi trigger channel Index */
				fprintf(fp, "4 ");
					

				/* Active High: 0(R),  Active Low: 1(F) */
				iCAStatus = ca_Get_QStr(pWnd->pvCLTU[sys].mTRIG.cid, qstr );
				if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mTRIG.name);
//				fprintf(fp, "%d \t", qstr.toInt());
				if( qstr.toInt() ) fprintf(fp, "F ");
				else fprintf(fp, "R "); 
					

				for( int j=0; j<3; j++)
				{
					/* Absolute Start time T0: sec */
					iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].mT0[j].cid, dbVal );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mT0[j].name);
					fprintf(fp, "%lf ", dbVal - nblipTime );

					/* Absolute End time T1: sec */
					iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].mT1[j].cid, dbVal );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mT1[j].name);
					fprintf(fp, "%lf ", dbVal - nblipTime );
					
					/* Section Clock KHz */
					iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].mCLOCK[j].cid, qstr );
					if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mCLOCK[j].name);
					fprintf(fp, "%.3f ", qstr.toFloat() /1000.0);

				}

				/* Next line */
				fprintf(fp, "\n");


			}

		}
		else {
//			fprintf(fp, "%s disconnected(%d)\n", pWnd->str_sysName[sys], sys);
		}
	}

	fclose(fp);

	pWnd->printReturn(LEVEL_NONE, "DB created.");
}
#endif

// SQL insert.
#if 1
void CltuMain::save_current_ltu_setup(unsigned int shotNum)
{
	QString qstr;
	int iCAStatus ;
	double dbVal;
	MainWindow *pWnd = (MainWindow*)pm;
	pWnd->printReturn(LEVEL_NONE, "Saving to SQL DB");
	
	int nblipTime;

//	unsigned int shotNum;
	char shotdate[24];
	int nVer;
	int isEnclosed;
	int sysId;
	char sysname[30];
	char strDesc[40];
	int chId;
	int enable;
	int modeCT;
	int pol;
	double dT0[R2_SECT_CNT];
	double dT1[R2_SECT_CNT];
	double dClk[R2_SECT_CNT];

//	sprintf(filename, "test.txt");

//	iCAStatus = ca_Get_QStr( pWnd->pvCCS_ShotNumber.cid, qstr );
//	if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCCS_ShotNumber.name);
//	shotNum = qstr.toInt();

	nblipTime = pWnd->pg_timeset.pg_get_bliptime();

	QDateTime dtime = QDateTime::currentDateTime();	
	QString text = dtime.toString("yyyy/MM/dd-hh:mm:ss");

	strcpy(shotdate, text.toAscii().constData() );


	mysqlpp::Query query = m_con.query();

	
	for( int sys=0; sys<MAX_LTU_CARD; sys++) 
	{
		sysId = sys;
		sprintf(sysname, pWnd->str_sysName[sys]);

		if( pWnd->pg_timeset.pg_is_enclosed_ltu(pWnd->str_sysName[sys]) ) 
		{

			if( ca_read_access( pWnd->pvCLTU[sys].SHOT.cid ) ) 
			{	
	//			fprintf(fp, "%s connected(%d)\n", pWnd->str_sysName[sys], sys);
	//			printf("query %s\n", sysname);

				if( pWnd->Is_New_R2_id(sys) ) 
				{ 
					nVer = VER2;
					
					for(int j=0; j<R2_PORT_CNT; j++) 
					{
	//					if( pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) ) isEnclosed =1;
	//					else  isEnclosed = 0;
						isEnclosed = pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) ? 1 : 0;
						if( isEnclosed ) 
						{
							pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], j, strDesc);
							chId = j;

							/* Enable: 1, Disable: 0 */
							iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_Enable[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_Enable[j].name);
							enable = qstr.toInt();

							
							/* Clock: 0,  Trigger: 1, nothing: 2*/
							iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_Mode[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_Mode[j].name);
							modeCT = qstr.toInt();

							/* Active High: 0(R),  Active Low: 1(F) */
							iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_ActiveLow[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_ActiveLow[j].name);
							pol = qstr.toInt();


							for(int k=0; k<R2_SECT_CNT; k++) 
							{
								/* offset from bliptime T0: sec */
								iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].R2_T0[j][k].cid, dbVal );
								if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_T0[j][k].name);
								dT0[k] = dbVal - nblipTime;

								/* offset from bliptime T1: sec */
								iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].R2_T1[j][k].cid, dbVal );
								if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_T1[j][k].name);
								dT1[k] = dbVal - nblipTime;

								
								/* Section Clock KHz */
								iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].R2_CLOCK[j][k].cid, qstr );
								if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].R2_CLOCK[j][k].name);
								dClk[k] = qstr.toFloat() /1000.0;

							}
							
							/* Next line */
		//					fprintf(fp, "\n");
							// insert one field here!!!!!!
		//					printf("%d, %s %d, %d, sysId:%d, %s, %s, ch%d, %d, %d, %d, (%.10f, %.10f, %f)\n", shotNum,shotdate,  nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
		//							dT0[0], dT1[0], dClk[0] );
#if 1
							LTU_CONFIGURE ltuInsert(shotNum, shotdate, nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
									dT0[0], dT1[0], dClk[0],
									dT0[1], dT1[1], dClk[1],
									dT0[2], dT1[2], dClk[2],
									dT0[3], dT1[3], dClk[3],
									dT0[4], dT1[4], dClk[4],
									dT0[5], dT1[5], dClk[5],
									dT0[6], dT1[6], dClk[6],
									dT0[7], dT1[7], dClk[7] ); 
		/*
							LTU_CONFIGURE ltuInsert(12, "3mon", 1, 1, 12, "TEST", "good", 1, 1, 1, 1, 
									1.0, 1.0, 1.0,
									2.0, 2.0, 2.0,
									3.0, 3.0, 3.0,
									4.0, 4.0, 4.0,
									5.0, 5.0, 5.0,
									6.0, 6.0, 6.0,
									7.0, 7.0, 7.0,
									8.0, 8.0, 8.0 ); */
		// 					printf("query %s, ch%d\n", sysname, chId);
							query.insert(ltuInsert);
		//					printf("\t call execute\n");
							query.execute();
		//					printf("\t after execute\n");
#endif
						}
					}
				}
				else 
				{
	//				sprintf(cVer, "v1");
					nVer = VER1;
					
					for( int j=0; j<4; j++) 
					{
	//					if( pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) ) sprintf(cEnclosed, "include");
	//					else  sprintf(cEnclosed, "exclude");
						isEnclosed = pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], j) ? 1 : 0;
						if( isEnclosed )
						{
							pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], j, strDesc);
							chId = j;
							
							/* Enable: 1, Disable: 0 */
							enable = 1;
							/* Clock: 0,  Trigger: 1, nothing: 2*/
							modeCT = 2; 
							
							/* Active High: 0(R),  Active Low: 1(F) */
							iCAStatus = ca_Get_QStr(pWnd->pvCLTU[sys].POL[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].POL[j].name);
							pol = qstr.toInt(); //2013. 7. 23
//							pol = ( qstr.toInt() == 0 ) ? 1: 0;  // polarity switched.  turn back. '13. 7. 25

							
							/* Absolute Start time T0: sec */
							iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].T0[j].cid, dbVal );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].T0[j].name);
							dT0[0] = dbVal - nblipTime;

							/* Absolute End time T1: sec */
							iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].T1[j].cid, dbVal );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].T1[j].name);
							dT1[0] = dbVal - nblipTime;
							
							/* Section Clock KHz */
							iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].CLOCK[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].CLOCK[j].name);
							dClk[0] = qstr.toFloat() /1000.0;

		//					printf("%d, %s %d, %d, sysId:%d, %s, %s, ch%d, %d, %d, %d, (%f, %f, %f)\n", shotNum,shotdate,  nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
		//							dT0[0], dT1[0], dClk[0] );
#if 1					
							LTU_CONFIGURE ltuInsert(shotNum, shotdate, nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
									dT0[0], dT1[0], dClk[0],
									0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0. );
							query.insert(ltuInsert);
							query.execute();
#endif
						}
					}
					
					isEnclosed = pWnd->pg_timeset.pg_is_enclosed(pWnd->str_sysName[sys], 4) ? 1 : 0;
					if( isEnclosed )
					{
						pWnd->pg_timeset.pg_get_description(pWnd->str_sysName[sys], 4, strDesc);
						chId = 4;

						/* Enable: 1, Disable: 0 */
						enable = 1;
						/* Clock: 0,  Trigger: 1, nothing: 2*/
						modeCT = 2; 
						
						/* Active High: 0(R),  Active Low: 1(F) */
						iCAStatus = ca_Get_QStr(pWnd->pvCLTU[sys].mTRIG.cid, qstr );
						if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mTRIG.name);
						pol = qstr.toInt();
							

						for( int j=0; j<3; j++)
						{
							/* Absolute Start time T0: sec */
							iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].mT0[j].cid, dbVal );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mT0[j].name);
							dT0[j] = dbVal - nblipTime;

							/* Absolute End time T1: sec */
							iCAStatus = ca_Get_Double( pWnd->pvCLTU[sys].mT1[j].cid, dbVal );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mT1[j].name);
							dT1[j] = dbVal - nblipTime;
							
							/* Section Clock KHz */
							iCAStatus = ca_Get_QStr( pWnd->pvCLTU[sys].mCLOCK[j].cid, qstr );
							if( iCAStatus  != ECA_NORMAL ) pWnd->printStatus(LEVEL_ERROR, "Can not read \"%s\" ", pWnd->pvCLTU[sys].mCLOCK[j].name);
							dClk[j] = qstr.toFloat() /1000.0;

						}

		//				printf("%d, %s %d, %d, sysId:%d, %s, %s, ch%d, %d, %d, %d, (%f, %f, %f)\n", shotNum,shotdate,  nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
		//							dT0[0], dT1[0], dClk[0] );
#if 1
						LTU_CONFIGURE ltuInsert(shotNum, shotdate, nVer, isEnclosed, sysId, sysname, strDesc, chId, enable, modeCT, pol, 
							dT0[0], dT1[0], dClk[0],   dT0[1], dT1[1], dClk[1],   dT0[2], dT1[2], dClk[2],
							 0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0. );
						query.insert(ltuInsert);
						query.execute();
#endif
					}
				}

			}
			else {
	//			fprintf(fp, "%s disconnected(%d)\n", pWnd->str_sysName[sys], sys);
				pWnd->printReturn(LEVEL_WARNING, "%s disconnected(%d)\n", pWnd->str_sysName[sys], sys);
			}
		}
	}

	pWnd->printReturn(LEVEL_NONE, "%d DB created.", shotNum);
}
#endif


int CltuMain::load_stored_ltu_setup(unsigned int shot)
{
	MainWindow *pWnd = (MainWindow*)pm;

//	FILE *fp;
//	char bufLineScan[512];
//	char filename[32];
//	char temp1[16], temp2[16];
	
/*	

	QString fileName = STR_FILEOPEN_DIR; 
	fileName.append(QString( "/%1").arg( shot ));
	fileName.append(".dat");


	printf("filename: %s\n", fileName.toAscii().constData() );

	pWnd->pg_timeset.fileOpen(fileName);
*/
	QTableWidget *pTableWidget;
	char IsNew=0;
	QTableWidgetItem *item[MAX_COL2_NUM];
	int maxCOL=0;
	char strSystem[10];
	char strBuf[64];

	int rowCnt=0;
	int totalCnt = 0;

	int sysID;
	int port;
//	double dds1_tail = 0.0;


	pWnd->pg_timeset.pg_remove_all_list();
	pWnd->pg_timeset.pg_set_block_pv(USER_FILE_OPEN);
//	printf("Load from DB with shot %d\n", shot);

	Row row;
	Query query = m_con.query();
	QString qstrQuery = "select * from LTU_CONFIGURE where shotnum = " + QString("%1 and enclose=1").arg(shot) ;
	query << qstrQuery.toStdString();
	query.parse();
	ResUse res = query.use();
	try {
		while( row = res.fetch_row() )
		{
			strcpy(strSystem, row["sysName"].get_string().c_str() );
			IsNew = pWnd->Is_New_R2_name(strSystem);
			sysID = pWnd->get_system_ID(strSystem);

//			printf("sysname: %s, ch %d, descr: %s\n", strSystem, (int)row["ch"], row["descr"].get_string().c_str() );
#if 1
			if( IsNew) 
				maxCOL = MAX_COL2_NUM;
			else 
				maxCOL = MAX_COL_NUM;

			for(int i=0; i<maxCOL; i++) {
				item[i] = new QTableWidgetItem();
				item[i]->setTextAlignment(Qt::AlignCenter);
			}

			pTableWidget = pWnd->pg_timeset.getTableWidget_from_sysname(strSystem);
			if( !pTableWidget ) {
				pWnd->printStatus(LEVEL_ERROR, "No correlative system (%s)", strSystem);
				return WR_ERROR;
			}

			rowCnt = pTableWidget->rowCount();
			pTableWidget->setRowCount(rowCnt + 1);


			item[COL_ID_SYS]->setText( row["sysName"].get_string().c_str() );
			item[COL_ID_DESC]->setText( row["descr"].get_string().c_str() ); item[COL_ID_DESC]->setTextAlignment(Qt::AlignLeft);
			item[COL_ID_CH]->setText( QString("%1").arg(row["ch"] + 1) );
			port = (int)row["ch"]; /* 0, 1, 2, ~ 7 */
//			if( row["pol"] ) item[COL_ID2_POL]->setText("F" );
//			else item[COL_ID2_POL]->setText("R" ); 

//			item[COL_ID_1_T0]->setText( QString("%1").arg((double)row["sec1T0"]) );
//			item[COL_ID_1_T1]->setText(  QString("%1").arg((double)row["sec1T1"]) );
//			item[COL_ID_1_CLK]->setText(  QString("%1").arg((double)row["sec1clk"]) );
//			printf("%s, ch%d (%f, %f, %f)\n",  row["sysName"].get_string().c_str(), (int)row["ch"], (double)row["sec1T0"], (double)row["sec1T1"], (double)row["sec1clk"] );

			if( IsNew ) 
			{
				item[COL_ID2_ONOFF]->setFlags(item[COL_ID2_ONOFF]->flags() & ~Qt::ItemIsEditable);
				if( row["enable"] )item[COL_ID2_ONOFF]->setCheckState(Qt::Checked);
				else item[COL_ID2_ONOFF]->setCheckState(Qt::Unchecked);

				if( (int)row["mode"] == 1 ) item[COL_ID2_MODE]->setText("T" ); /* Clock: 0,  Trigger: 1, nothing: 2*/
				else if( (int)row["mode"] == 0  ) item[COL_ID2_MODE]->setText("C" ); 
				else item[COL_ID2_MODE]->setText("N/A" ); 

				if( row["pol"] ) item[COL_ID2_POL]->setText("F" ); /* Active High: 0(R),  Active Low: 1(F) */
				else item[COL_ID2_POL]->setText("R" ); 

				cvt_Double_to_String( (double)row["sec1T0"], strBuf ); item[COL_ID2_1_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec1T1"], strBuf ); item[COL_ID2_1_T1]->setText( strBuf ); 
#if 0				
				if( sysID == DDS1A_LTU  ) {
					dds1_tail = pWnd->pg_timeset.pg_getTimeTail( (int)row["sec1clk"]);
					cvt_Double_to_String( (double)row["sec1T1"] - dds1_tail, strBuf ); item[COL_ID2_1_T1]->setText( strBuf );
				} else if( sysID == DDS1B_LTU  && port < 4) {
					dds1_tail = pWnd->pg_timeset.pg_getTimeTail( (int)row["sec1clk"]);
					cvt_Double_to_String( (double)row["sec1T1"] - dds1_tail, strBuf ); item[COL_ID2_1_T1]->setText( strBuf );
				} 
/*	2012. 9. 19 mistake. HALPHA use old version 
				else if ( (sysID == HALPHA_LTU) && (port == ONLY_TARGET_M6802_PORT ) )
				{
					cvt_Double_to_String( (double)row["sec1T1"] - HALPHA_DAQ_TIME_TAIL, strBuf ); item[COL_ID2_1_T1]->setText( strBuf );
				} 
*/
/* 2012. 9. 6 request from TGLee */
/*				else if ( (sysID == ECEHR_LTU) && (port == ONLY_TARGET_M6802_PORT ) )
				{
					cvt_Double_to_String( (double)row["sec1T1"] - HALPHA_DAQ_TIME_TAIL, strBuf ); item[COL_ID2_1_T1]->setText( strBuf );
				}
*/
				else {
					cvt_Double_to_String( (double)row["sec1T1"], strBuf ); item[COL_ID2_1_T1]->setText( strBuf ); 
				}
#endif
				cvt_Double_to_String( (double)row["sec1clk"], strBuf ); item[COL_ID2_1_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec2T0"], strBuf ); item[COL_ID2_2_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec2T1"], strBuf ); item[COL_ID2_2_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec2clk"], strBuf ); item[COL_ID2_2_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec3T0"], strBuf ); item[COL_ID2_3_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec3T1"], strBuf ); item[COL_ID2_3_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec3clk"], strBuf ); item[COL_ID2_3_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec4T0"], strBuf ); item[COL_ID2_4_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec4T1"], strBuf ); item[COL_ID2_4_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec4clk"], strBuf ); item[COL_ID2_4_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec5T0"], strBuf ); item[COL_ID2_5_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec5T1"], strBuf ); item[COL_ID2_5_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec5clk"], strBuf ); item[COL_ID2_5_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec6T0"], strBuf ); item[COL_ID2_6_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec6T1"], strBuf ); item[COL_ID2_6_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec6clk"], strBuf ); item[COL_ID2_6_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec7T0"], strBuf ); item[COL_ID2_7_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec7T1"], strBuf ); item[COL_ID2_7_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec7clk"], strBuf ); item[COL_ID2_7_CLK]->setText(  strBuf );
				cvt_Double_to_String( (double)row["sec8T0"], strBuf ); item[COL_ID2_8_T0]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec8T1"], strBuf ); item[COL_ID2_8_T1]->setText( strBuf );
				cvt_Double_to_String( (double)row["sec8clk"], strBuf ); item[COL_ID2_8_CLK]->setText(  strBuf );
 
			} 
			else 
			{
				if( row["pol"] ) item[COL_ID_POL]->setText("F" ); /* Active High: 0(R),  Active Low: 1(F) */
				else item[COL_ID_POL]->setText("R" ); 


				cvt_Double_to_String( (double)row["sec1T0"], strBuf ); item[COL_ID_1_T0]->setText( strBuf );
				if ( (sysID == HALPHA_LTU) && (port == ONLY_TARGET_M6802_PORT ) ) {
					cvt_Double_to_String( (double)row["sec1T1"] - HALPHA_DAQ_TIME_TAIL, strBuf ); item[COL_ID_1_T1]->setText( strBuf );
				} else {
					cvt_Double_to_String( (double)row["sec1T1"], strBuf ); item[COL_ID_1_T1]->setText( strBuf );
				}
				cvt_Double_to_String( (double)row["sec1clk"], strBuf ); item[COL_ID_1_CLK]->setText( strBuf );
			
				if( (int)row["ch"] == 4 ) /* 0, 1, 2, 3, 4 */
				{
					cvt_Double_to_String( (double)row["sec2T0"], strBuf ); item[COL_ID_2_T0]->setText( strBuf );
					cvt_Double_to_String( (double)row["sec2T1"], strBuf ); item[COL_ID_2_T1]->setText( strBuf );
					cvt_Double_to_String( (double)row["sec2clk"], strBuf ); item[COL_ID_2_CLK]->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3T0"], strBuf ); item[COL_ID_3_T0]->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3T1"], strBuf ); item[COL_ID_3_T1]->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3clk"], strBuf ); item[COL_ID_3_CLK]->setText( strBuf );

				}


			}


			for(int i=0; i<maxCOL; i++) 
				pTableWidget->setItem(rowCnt, i, item[i]);
#endif
			totalCnt++;
		}
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};

	if( totalCnt ) 
	{
		pWnd->tw_machine->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_machine->sortItems( COL_ID_SYS, Qt::AscendingOrder);
		
		pWnd->tw_diagnostics->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_diagnostics->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pWnd->tw_ltu2->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_ltu2->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pWnd->tw_ltu2_diag->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_ltu2_diag->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pWnd->pg_timeset.pg_set_color_line(pWnd->tw_machine);
		pWnd->pg_timeset.pg_set_color_line(pWnd->tw_diagnostics);
		pWnd->pg_timeset.pg_set_color_line(pWnd->tw_ltu2);
		pWnd->pg_timeset.pg_set_color_line(pWnd->tw_ltu2_diag);
		
		pWnd->printReturn(LEVEL_NONE, "Shot #%d DB loaded.", shot);
	}
	else
		pWnd->printReturn(LEVEL_WARNING, "No shot #%d in DB.", shot);

	pWnd->pg_timeset.pg_set_release_pv();

	if( totalCnt == 0) return WR_ERROR;

	return WR_OK;

}

int CltuMain::load_single_line_ltu_setup()
{
	MainWindow *pWnd = (MainWindow*)pm;

	int readCnt = 0;
	bool IsNew=0;
	QTableWidget *pTableWidget;
	
	unsigned int shotNum= pWnd->pg_timeset.pg_get_query_shot();
	int chId = 0;
	char sysName[10];
	int sysId = 0;
	int curRow;
	char strBuf[64];
//	double dds1_tail = 0.0;
	
	pTableWidget = pWnd->pg_timeset.getTableWidget_from_currentTab();
	curRow = pTableWidget->currentRow();
	if (curRow < 0) {
		pWnd->printReturn(LEVEL_NOTICE, "No target LTU!");
		return WR_ERROR;
	}
	sprintf(sysName, "%s", pTableWidget->item(curRow, COL_ID_SYS)->text().toAscii().constData());
	chId = pTableWidget->item(curRow, COL_ID_CH)->text().toInt(); /* 1, 2, 3, ~ 8 */
	sysId = pWnd->get_system_ID(sysName);

	printf("Try to load DB with shot %d, %s, ch %d\n", shotNum, sysName, chId);

	Row row;
	Query query = m_con.query();
	string strquery = "select * from LTU_CONFIGURE where shotnum =%0 and ch=%1 and sysID=%2";
	query << strquery;
	query.parse();
	ResUse res = query.use(shotNum, chId-1, sysId);
	try {
		while( row = res.fetch_row() )
		{
			readCnt++;
		}
	}  catch (const Exception& er) {
		cerr << er.what() << endl;
	};
	if(readCnt > 1) {
		pWnd->printReturn(LEVEL_ERROR, "Duplicated Record!Must be check! shot %d, %s, ch %d\n", shotNum, sysName, chId);
		return WR_ERROR;
	}
	else if ( readCnt == 0 ) {
		pWnd->printReturn(LEVEL_WARNING, "There is no slot! shot %d, %s, ch %d\n", shotNum, sysName, chId);
		return WR_ERROR;
	}
//	printf("OK! readCnt: %d\n", readCnt);
	query.reset();
	query << strquery;
	query.parse();
	res = query.use(shotNum, chId-1, sysId);
	readCnt = 0;
	try {		
		while( row = res.fetch_row() )
		{
			IsNew = pWnd->Is_New_R2_id(sysId);

			pWnd->pg_timeset.pg_set_block_pv(USER_ITEM_CHANGED);

			pTableWidget->item(curRow, COL_ID_DESC)->setText( row["descr"].get_string().c_str() );

			if( IsNew ) 
			{
				if( (int)row["enable"] )pTableWidget->item(curRow,COL_ID2_ONOFF)->setCheckState(Qt::Checked);
				else pTableWidget->item(curRow,COL_ID2_ONOFF)->setCheckState(Qt::Unchecked);

				if( (int)row["mode"] == 1 ) pTableWidget->item(curRow,COL_ID2_MODE)->setText("T" ); /* Clock: 0,  Trigger: 1, nothing: 2*/
				else if( (int)row["mode"] == 0  ) pTableWidget->item(curRow,COL_ID2_MODE)->setText("C" ); 
				else pTableWidget->item(curRow,COL_ID2_MODE)->setText("N/A" ); 

				if( (int)row["pol"] ) pTableWidget->item(curRow,COL_ID2_POL)->setText("F" ); /* Active High: 0(R),  Active Low: 1(F) */
				else pTableWidget->item(curRow,COL_ID2_POL)->setText("R" ); 

				cvt_Double_to_String( (double)row["sec1T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_1_T0)->setText( strBuf );
#if 0
				if( sysId == DDS1A_LTU  ) {
					dds1_tail = pWnd->pg_timeset.pg_getTimeTail( (int)row["sec1clk"]);
					cvt_Double_to_String( (double)row["sec1T1"] - dds1_tail, strBuf ); pTableWidget->item(curRow,COL_ID2_1_T1)->setText(  strBuf );
				} else if( sysId == DDS1B_LTU  && (chId-1) < 4) {
					dds1_tail = pWnd->pg_timeset.pg_getTimeTail( (int)row["sec1clk"]);
					cvt_Double_to_String( (double)row["sec1T1"] - dds1_tail, strBuf ); pTableWidget->item(curRow,COL_ID2_1_T1)->setText(  strBuf );
				} 
#endif
				if ( (sysId == HALPHA_LTU) && ((chId-1) == ONLY_TARGET_M6802_PORT ) )
				{
					cvt_Double_to_String( (double)row["sec1T1"] - HALPHA_DAQ_TIME_TAIL, strBuf ); pTableWidget->item(curRow,COL_ID2_1_T1)->setText(  strBuf );
				}
/* 2012. 9. 6 request from TGLee */
/*				else if ( (sysId == ECEHR_LTU) && ((chId-1) == ONLY_TARGET_M6802_PORT ) )
				{
					cvt_Double_to_String( (double)row["sec1T1"] - HALPHA_DAQ_TIME_TAIL, strBuf ); pTableWidget->item(curRow,COL_ID2_1_T1)->setText(  strBuf );
				}
*/				else {
					cvt_Double_to_String( (double)row["sec1T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_1_T1)->setText(  strBuf );
				}
				cvt_Double_to_String( (double)row["sec1clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_1_CLK)->setText(strBuf );
				cvt_Double_to_String( (double)row["sec2T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_2_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec2T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_2_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec2clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_2_CLK)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec3T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_3_T0)->setText( strBuf );
 				cvt_Double_to_String( (double)row["sec3T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_3_T1)->setText(strBuf );
				cvt_Double_to_String( (double)row["sec3clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_3_CLK)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec4T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_4_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec4T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_4_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec4clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_4_CLK)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec5T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_5_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec5T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_5_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec5clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_5_CLK)->setText(strBuf );
				cvt_Double_to_String( (double)row["sec6T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_6_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec6T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_6_T1)->setText(strBuf );
				cvt_Double_to_String( (double)row["sec6clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_6_CLK)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec7T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_7_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec7T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_7_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec7clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_7_CLK)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec8T0"], strBuf ); pTableWidget->item(curRow,COL_ID2_8_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec8T1"], strBuf ); pTableWidget->item(curRow,COL_ID2_8_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec8clk"], strBuf ); pTableWidget->item(curRow,COL_ID2_8_CLK)->setText( strBuf );

			} 
			else 
			{
				if( (int)row["pol"] ) pTableWidget->item(curRow,COL_ID_POL)->setText("F" ); /* Active High: 0(R),  Active Low: 1(F) */
				else pTableWidget->item(curRow,COL_ID_POL)->setText("R" ); 
			
				cvt_Double_to_String( (double)row["sec1T0"], strBuf );pTableWidget->item(curRow,COL_ID_1_T0)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec1T1"], strBuf );pTableWidget->item(curRow,COL_ID_1_T1)->setText( strBuf );
				cvt_Double_to_String( (double)row["sec1clk"], strBuf );pTableWidget->item(curRow,COL_ID_1_CLK)->setText( strBuf );
			
				if( (int)row["ch"] == 4 ) /* 0, 1, 2, 3, 4 */
				{
					cvt_Double_to_String( (double)row["sec2T0"], strBuf );pTableWidget->item(curRow,COL_ID_2_T0)->setText( strBuf );
					cvt_Double_to_String( (double)row["sec2T1"], strBuf );pTableWidget->item(curRow,COL_ID_2_T1)->setText( strBuf );
					cvt_Double_to_String( (double)row["sec2clk"], strBuf );pTableWidget->item(curRow,COL_ID_2_CLK)->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3T0"], strBuf );pTableWidget->item(curRow,COL_ID_3_T0)->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3T1"], strBuf );pTableWidget->item(curRow,COL_ID_3_T1)->setText( strBuf );
					cvt_Double_to_String( (double)row["sec3clk"], strBuf );pTableWidget->item(curRow,COL_ID_3_CLK)->setText( strBuf );

				}


			}
			pWnd->pg_timeset.pg_set_release_pv();

			pWnd->printReturn(LEVEL_NONE, "Loading from DB with shot #%d, %s, ch %d", shotNum, sysName, chId);
			
			readCnt++;
		}
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
/*
	if( readCnt != 1 ) {
		pWnd->printReturn(LEVEL_ERROR, "Single load duplicated!(%d)", readCnt);
		return WR_ERROR;
	}
*/
	return WR_OK;

}

void CltuMain::delete_stored_ltu_info(unsigned int shotNum)
{
	MainWindow *pWnd = (MainWindow*)pm;

	try {
		mysqlpp::Query query = m_con.query();
		QString qstrQuery = "delete from LTU_CONFIGURE where shotnum = " + QString("%1").arg(shotNum) ;
		query << qstrQuery.toStdString();
		query.parse();
		query.use(); // or execute()
	
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};

	pWnd->printReturn(LEVEL_NONE, "SQL DB shot %d deleted.", shotNum);

}





