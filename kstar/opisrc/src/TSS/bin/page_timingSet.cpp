/****************************************************************************
**
** Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved. 
**
**
*****************************************************************************/


#include <QtGui>

#include "page_timingSet.h"

#include "mainWindow.h"

#include "global.h"

#include "StringEditor.h"


//#include "ddsPVlist.h"
//#include "nodeDelegate.h"

int timer_First_machine = 1;
int timer_First_diagnostics = 1;
int timer_First_ltu2 = 1;
int timer_First_ltu2_diag = 1;





CltuTimingSet::CltuTimingSet()
{
	pm = NULL;
	nPVblockMode = USER_NONE;
	brush_sep1 = QBrush ( QColor(240, 240, 240), Qt::SolidPattern );
	brush_sep2 = QBrush ( QColor(220, 220, 220), Qt::SolidPattern );
	brush_foreColum = QBrush ( QColor(32, 25, 255), Qt::SolidPattern );

	brush_dirt_red = QBrush ( QColor(255, 25, 25), Qt::SolidPattern );
	brush_dirt_green = QBrush ( QColor(25, 255, 25), Qt::SolidPattern );

//	cntEvidMain=0;
	nBlip_time = DEFAULT_BLIP_TIME;
	nBlip_time_CCS = DEFAULT_BLIP_TIME;
	current_query_shot = 0;

	
}

CltuTimingSet::~CltuTimingSet()
{
#if 0
	killOneSecTimer_DDS1();
	killOneSecTimer_DDS2();
#endif


}

void CltuTimingSet::destroyPageTimingSet()
{
//	killOneSecTimer_DDS();
//	printf("destroy page_timingset\n");
}

void CltuTimingSet::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
//	MainWindow *pWnd = (MainWindow*)pm;

	pSTmng = new Cpvst_mng(parent);


//	qthread = new CqueueThread( (MainWindow*)pm );	
//	connect( qthread, SIGNAL(finished() ), qthread, SLOT(deleteLater()) );
//	qthread->start( QThread::TimeCriticalPriority );
//	qthread->start( QThread::NormalPriority );


	InitTW_timingParamSet();

#if 1
	timer_assign_machine = new QTimer(this);	
	connect(timer_assign_machine, SIGNAL(timeout()), this, SLOT(timerFunc_assign_machine()));
	timer_assign_diagnostics = new QTimer(this);
	connect(timer_assign_diagnostics, SIGNAL(timeout()), this, SLOT(timerFunc_assign_diagnostics()));
	timer_assign_ltu2 = new QTimer(this);
	connect(timer_assign_ltu2, SIGNAL(timeout()), this, SLOT(timerFunc_assign_ltu2()));
	timer_assign_ltu2_diag = new QTimer(this);
	connect(timer_assign_ltu2_diag, SIGNAL(timeout()), this, SLOT(timerFunc_assign_ltu2_diag()));

	timer_setup_machine = new QTimer(this);	
	connect(timer_setup_machine, SIGNAL(timeout()), this, SLOT(timerFunc_setup_machine()));
	timer_setup_diagnostics = new QTimer(this);
	connect(timer_setup_diagnostics, SIGNAL(timeout()), this, SLOT(timerFunc_setup_diagnostics()));
	timer_setup_ltu2 = new QTimer(this);
	connect(timer_setup_ltu2, SIGNAL(timeout()), this, SLOT(timerFunc_setup_ltu2()));
	timer_setup_ltu2_diag = new QTimer(this);
	connect(timer_setup_ltu2_diag, SIGNAL(timeout()), this, SLOT(timerFunc_setup_ltu2_diag()));
	
//	timer_assign_machine->start(LTU_SETUP_FLUSH_GAP);
#endif
}

void CltuTimingSet::InitTW_timingParamSet()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QStringList qstrList;


	pWnd->cb_timing_system->clear();
	for(int i=0; i<MAX_LTU_CARD; i++) {
		qstrList.append( tr("%1").arg(pWnd->str_sysName[i]) );
	}

	qstrlist_Clock << tr("0") << tr("1") << tr("2") << tr("5") << tr("10") << tr("20") << tr("50") << tr("100") << tr("200") << tr("500") \
			<< tr("1000") << tr("2000") << tr("5000") << tr("10000") << tr("12500") ;
	
	qstrlist_R1_Port << tr("-1") << tr("1") << tr("2") << tr("3") << tr("4") << tr("5") ;
	qstrlist_R2_Port << tr("-1") << tr("1") << tr("2") << tr("3") << tr("4") << tr("5") << tr("6") << tr("7") << tr("8") ;


//	for( int i=0; i< qstrlist_Clock.size(); i++) {
//		pWnd->cb_timing_sec1Freq->insertItem(i, qstrlist_Clock.at(i)  );
//		pWnd->cb_timing_sec2Freq->insertItem(i, qstrlist_Clock.at(i)  );
//		pWnd->cb_timing_sec3Freq->insertItem(i, qstrlist_Clock.at(i)  );
//	}
		// 2011.4.26   for R2 initial clock to 1Hz
//		pWnd->cb_timing_sec1Freq->setCurrentIndex(1 );
//		pWnd->cb_timing_sec2Freq->setCurrentIndex(1 );
//		pWnd->cb_timing_sec3Freq->setCurrentIndex(1 );
		
	
	pWnd->cb_timing_system->insertItems(0, qstrList);

	
//	pWnd->tw_machine->horizontalHeader()->resizeSection(0, 30); // h/w
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_SYS, WIDTH_V1_SYS); // system
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_DESC, WIDTH_V1_DESC); // description
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_CH, WIDTH_V1_CH); // port
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_POL, WIDTH_V1_POL); // trig level
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_1_T0, WIDTH_V1_T0); // sec1 start time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_1_T1, WIDTH_V1_T1); // sec1 stop time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_1_CLK, WIDTH_V1_CLK); // sec1 clock
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_2_T0, WIDTH_V1_T0); // sec2 start time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_2_T1, WIDTH_V1_T1); // sec2 stop time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_2_CLK, WIDTH_V1_CLK); // sec2 clock
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_3_T0, WIDTH_V1_T0); // sec3 start time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_3_T1, WIDTH_V1_T1); // sec3 stop time
	pWnd->tw_machine->horizontalHeader()->resizeSection(COL_ID_3_CLK, WIDTH_V1_CLK); // sec3 clock

	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_SYS, WIDTH_V1_SYS); // system
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_DESC, WIDTH_V1_DESC); // description
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_CH, WIDTH_V1_CH); // port
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_POL, WIDTH_V1_POL); // trig level
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_1_T0, WIDTH_V1_T0); // sec1 start time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_1_T1, WIDTH_V1_T1); // sec1 stop time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_1_CLK, WIDTH_V1_CLK); // sec1 clock
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_2_T0, WIDTH_V1_T0); // sec2 start time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_2_T1, WIDTH_V1_T1); // sec2 stop time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_2_CLK, WIDTH_V1_CLK); // sec2 clock
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_3_T0, WIDTH_V1_T0); // sec3 start time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_3_T1, WIDTH_V1_T1); // sec3 stop time
	pWnd->tw_diagnostics->horizontalHeader()->resizeSection(COL_ID_3_CLK, WIDTH_V1_CLK); // sec3 clock

	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID_SYS, WIDTH_V2_SYS); // system
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID_DESC, WIDTH_V2_DESC); // description
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID_CH, WIDTH_V2_CH); // port
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_ONOFF, WIDTH_V2_ONOFF); // onoff
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_MODE, WIDTH_V2_MODE); // mode: Clock, Trigger
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_POL, WIDTH_V2_POL); // trig level
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_1_T0, WIDTH_V2_T0); // sec1 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_1_T1, WIDTH_V2_T1); // sec1 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_1_CLK, WIDTH_V2_CLK); // sec1 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_2_T0, WIDTH_V2_T0); // sec2 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_2_T1, WIDTH_V2_T1); // sec2 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_2_CLK, WIDTH_V2_CLK); // sec2 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_3_T0, WIDTH_V2_T0); // sec3 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_3_T1, WIDTH_V2_T1); // sec3 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_3_CLK, WIDTH_V2_CLK); // sec3 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_4_T0, WIDTH_V2_T0); // sec4 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_4_T1, WIDTH_V2_T1); // sec4 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_4_CLK, WIDTH_V2_CLK); // sec4 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_5_T0, WIDTH_V2_T0); // sec5 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_5_T1, WIDTH_V2_T1); // sec5 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_5_CLK, WIDTH_V2_CLK); // sec5 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_6_T0, WIDTH_V2_T0); // sec6 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_6_T1, WIDTH_V2_T1); // sec6 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_6_CLK, WIDTH_V2_CLK); // sec6 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_7_T0, WIDTH_V2_T0); // sec7 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_7_T1, WIDTH_V2_T1); // sec7 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_7_CLK, WIDTH_V2_CLK); // sec7 clock
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_8_T0, WIDTH_V2_T0); // sec8 start time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_8_T1, WIDTH_V2_T1); // sec8 stop time
	pWnd->tw_ltu2->horizontalHeader()->resizeSection(COL_ID2_8_CLK, WIDTH_V2_CLK); // sec8 clock


	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID_SYS, WIDTH_V2_SYS); // system
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID_DESC, WIDTH_V2_DESC); // description
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID_CH, WIDTH_V2_CH); // port
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_ONOFF, WIDTH_V2_ONOFF); // onoff
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_MODE, WIDTH_V2_MODE); // mode: Clock, Trigger
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_POL, WIDTH_V2_POL); // trig level
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_1_T0, WIDTH_V2_T0); // sec1 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_1_T1, WIDTH_V2_T1); // sec1 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_1_CLK, WIDTH_V2_CLK); // sec1 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_2_T0, WIDTH_V2_T0); // sec2 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_2_T1, WIDTH_V2_T1); // sec2 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_2_CLK, WIDTH_V2_CLK); // sec2 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_3_T0, WIDTH_V2_T0); // sec3 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_3_T1, WIDTH_V2_T1); // sec3 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_3_CLK, WIDTH_V2_CLK); // sec3 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_4_T0, WIDTH_V2_T0); // sec4 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_4_T1, WIDTH_V2_T1); // sec4 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_4_CLK, WIDTH_V2_CLK); // sec4 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_5_T0, WIDTH_V2_T0); // sec5 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_5_T1, WIDTH_V2_T1); // sec5 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_5_CLK, WIDTH_V2_CLK); // sec5 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_6_T0, WIDTH_V2_T0); // sec6 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_6_T1, WIDTH_V2_T1); // sec6 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_6_CLK, WIDTH_V2_CLK); // sec6 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_7_T0, WIDTH_V2_T0); // sec7 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_7_T1, WIDTH_V2_T1); // sec7 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_7_CLK, WIDTH_V2_CLK); // sec7 clock
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_8_T0, WIDTH_V2_T0); // sec8 start time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_8_T1, WIDTH_V2_T1); // sec8 stop time
	pWnd->tw_ltu2_diag->horizontalHeader()->resizeSection(COL_ID2_8_CLK, WIDTH_V2_CLK); // sec8 clock

//	pWnd->le_bliptime_my->setText(QString("%1").arg(DEFAULT_BLIP_TIME) );

}


/*
########################################################

Real action for SIGNAL input

########################################################
*/


void CltuTimingSet::clicked_btn_timing_insert()
{
	int row;
	bool isNew;
	int colMax;
	
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;

	QTableWidgetItem *item[MAX_COL2_NUM];

/*
	char strValue[64];
	int iCAStatus = ca_Get_StrVal( pWnd->pvCLTU[DDS1A_LTU].SHOT.cid, strValue );
	if( iCAStatus  != ECA_NORMAL ) printf(" DDS1A Polarity: error! %s\n", ca_message( iCAStatus));
	else printf("DDS1A Polarity: %s\n", strValue);
*/

	isNew = pWnd->Is_New_R2_name(   (char*)pWnd->cb_timing_system->currentText().toAscii().constData()    );

	colMax = (isNew) ? MAX_COL2_NUM : MAX_COL_NUM;
	
	for(int i=0; i<colMax; i++) {
		item[i] = new QTableWidgetItem();
		item[i]->setTextAlignment(Qt::AlignCenter);
		item[i]->setText("");
	}

#if 0
printf("1:%p,2:%p,3:%p,4:%p,5:%p,6:%p,7:%p,8:%p,9:%p,10:%p,11:%p,12:%p,13:%p,14:%p\n",
pWnd->cb_timing_hwsw,
pWnd->cb_timing_system,
pWnd->le_timing_signal,
pWnd->cb_timing_port,
pWnd->cb_timing_pol,
pWnd->le_timing_sec1Start,
pWnd->le_timing_sec1Stop,
pWnd->cb_timing_sec1Freq,
pWnd->le_timing_sec2Start,
pWnd->le_timing_sec2Stop,
pWnd->cb_timing_sec2Freq,
pWnd->le_timing_sec3Start,
pWnd->le_timing_sec3Stop,
pWnd->cb_timing_sec3Freq);
#endif

	pTableWidget = getTableWidget_from_sysname((char*)pWnd->cb_timing_system->currentText().toAscii().constData());
	if( pTableWidget == pWnd->tw_machine )
		pWnd->tabWidget->setCurrentIndex( 0 );
	else if (pTableWidget == pWnd->tw_diagnostics)
		pWnd->tabWidget->setCurrentIndex( 1 );
	else if (pTableWidget == pWnd->tw_ltu2)
		pWnd->tabWidget->setCurrentIndex( 2 );
	else if (pTableWidget == pWnd->tw_ltu2_diag)
		pWnd->tabWidget->setCurrentIndex( 3 );
	else
		pWnd->tabWidget->setCurrentIndex( 0 );



	row = pTableWidget->rowCount();
	pTableWidget->setRowCount(row + 1);

//	item[0]->setText(pWnd->cb_timing_hwsw->currentText());
	item[COL_ID_SYS]->setText(pWnd->cb_timing_system->currentText());
	item[COL_ID_DESC]->setText("hello"); item[COL_ID_DESC]->setTextAlignment(Qt::AlignLeft);
	item[COL_ID_CH]->setText("-1" );


	if( isNew ) {

//		item[COL_ID2_ONOFF]->setText(strOnOff);
		item[COL_ID2_ONOFF]->setFlags(item[COL_ID2_ONOFF]->flags() & ~Qt::ItemIsEditable);
		item[COL_ID2_ONOFF]->setCheckState(Qt::Checked);
		
		item[COL_ID2_MODE]->setText("T"); /* C, T */
		item[COL_ID2_POL]->setText("R" );
		item[COL_ID2_1_T0]->setText("-16" );
		item[COL_ID2_1_T1]->setText("-16" );
		item[COL_ID2_1_CLK]->setText("1" );
/*
		item[COL_ID2_2_T0]->setText(pWnd->le_timing_sec2Start->text() );
		item[COL_ID2_2_T1]->setText(pWnd->le_timing_sec2Stop->text() );
		item[COL_ID2_2_CLK]->setText(pWnd->cb_timing_sec2Freq->currentText() );
		item[COL_ID2_3_T0]->setText(pWnd->le_timing_sec3Start->text() );
		item[COL_ID2_3_T1]->setText(pWnd->le_timing_sec3Stop->text() );
		item[COL_ID2_3_CLK]->setText(pWnd->cb_timing_sec3Freq->currentText() );
*/
		item[COL_ID2_2_T0]->setText("-16" );
		item[COL_ID2_2_T1]->setText("-16" );
		item[COL_ID2_2_CLK]->setText("1" );
		item[COL_ID2_3_T0]->setText("-16" );
		item[COL_ID2_3_T1]->setText("-16" );
		item[COL_ID2_3_CLK]->setText("1" );
		
		item[COL_ID2_4_T0]->setText("-16" );
		item[COL_ID2_4_T1]->setText("-16" );
		item[COL_ID2_4_CLK]->setText("1" );
		item[COL_ID2_5_T0]->setText("-16" );
		item[COL_ID2_5_T1]->setText("-16" );
		item[COL_ID2_5_CLK]->setText("1" );
		item[COL_ID2_6_T0]->setText("-16" );
		item[COL_ID2_6_T1]->setText("-16" );
		item[COL_ID2_6_CLK]->setText("1" );
		item[COL_ID2_7_T0]->setText("-16" );
		item[COL_ID2_7_T1]->setText("-16" );
		item[COL_ID2_7_CLK]->setText("1" );
		item[COL_ID2_8_T0]->setText("-16" );
		item[COL_ID2_8_T1]->setText("-16" );
		item[COL_ID2_8_CLK]->setText("1" );

	} 
	else {
	
		item[COL_ID_POL]->setText("R" );
		item[COL_ID_1_T0]->setText("0" );
		item[COL_ID_1_T1]->setText("0" );
		item[COL_ID_1_CLK]->setText("1");

//		if( pWnd->cb_timing_port->currentIndex() == 4  ){
			/*
			item[COL_ID_2_T0]->setText(pWnd->le_timing_sec2Start->text() );
			item[COL_ID_2_T1]->setText(pWnd->le_timing_sec2Stop->text() );
			item[COL_ID_2_CLK]->setText(pWnd->cb_timing_sec2Freq->currentText() );
			item[COL_ID_3_T0]->setText(pWnd->le_timing_sec3Start->text() );
			item[COL_ID_3_T1]->setText(pWnd->le_timing_sec3Stop->text() );
			item[COL_ID_3_CLK]->setText(pWnd->cb_timing_sec3Freq->currentText() );
			*/
			item[COL_ID_2_T0]->setText("0" );
			item[COL_ID_2_T1]->setText("0" );
			item[COL_ID_2_CLK]->setText("1" );
			item[COL_ID_3_T0]->setText("0" );
			item[COL_ID_3_T1]->setText("0" );
			item[COL_ID_3_CLK]->setText("1" );
//		}
	}
	
	nPVblockMode = USER_INSERT_ONE_LINE;
	for(int i=0; i<colMax; i++) {
//		if( !strcmp("S", pWnd->cb_timing_hwsw->currentText().toAscii().constData() ) )
//			item[i]->setBackground(brush_sw);
		pTableWidget->setItem(row, i, item[i]);
	}

	caput_assign_CLTU_with_input_row(pTableWidget, pTableWidget->rowCount() - 1 );
#if USE_IMMEDIATE_LTU_SETUP
	usleep(500000);
	caput_setup_CLTU_with_input_row(pTableWidget, pTableWidget->rowCount() - 1 );
#endif

	pTableWidget->sortItems( COL_ID_CH, Qt::AscendingOrder);
	pTableWidget->sortItems( COL_ID_SYS, Qt::AscendingOrder);

	pg_set_color_line(pTableWidget);

//	iCAStatus = ca_Put_StrVal(pWnd->pvCLTU[CTUV1].SHOT_TERM.cid, "360");
//	if( iCAStatus != ECA_NORMAL )
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[CTUV1].SHOT_TERM.name, 10, ca_message( iCAStatus) );
//	ca_Put_QStr(  pWnd->pvCLTU[CTUV1].SHOT_TERM, QString("10") );  // this is temporary
	nPVblockMode = USER_NONE;

	if( (pWnd->passwd_timeset_check == 0) && (pWnd->passwd_master_check == 1) )
		pWnd->pg_main.make_blinking_final_setup();	
//	pWnd->tabWidget->setCurrentIndex( getTableWidgetID_sysname(pWnd->cb_timing_system->currentText().toAscii().constData() ) );
	
}

void CltuTimingSet::clicked_btn_bliptime_final_setup()
{
	MainWindow *pWnd = (MainWindow*)pm;
	
	if( nPVblockMode != USER_NONE )
	{
		QMessageBox::information(0, "Kstar TSS", tr("Please wait setup done!") );
		return;
	}
	
	flush_ERROR_cnt = 0;
	
	timer_setup_machine->start(LTU_SETUP_FLUSH_GAP);
	pWnd->confirm_final_setup = 1;
}

void CltuTimingSet::clicked_btn_flush_LTUparam()
{
	MainWindow *pWnd = (MainWindow*)pm;

	if( nPVblockMode != USER_NONE )
	{
		QMessageBox::information(0, "Kstar TSS", tr("Please wait setup done!") );
		return;
	}
//	pWnd->printStatus(LEVEL_NONE, "used blip time is \"%d\"", nBlip_time);
/* 2010.8.17 woong*/
	QString qstr;
	int iCAStatus = ca_Get_QStr( pWnd->pvBlipTime.cid, qstr );
	if( iCAStatus  != ECA_NORMAL ) 
	{
		pWnd->printStatus(LEVEL_ERROR, "%s, %s\n", pWnd->pvBlipTime.name, ca_message( iCAStatus) );
		return;
	}
	pg_set_bliptime( qstr.toInt() );

	flush_ERROR_cnt = 0;
	
	timer_assign_machine->start(LTU_SETUP_FLUSH_GAP);
}

void CltuTimingSet::clicked_btn_single_load()
{
	MainWindow *pWnd = (MainWindow*)pm;
//	printf("single load value: %d\n", current_query_shot);
	QTableWidget *pTableWidget = getTableWidget_from_currentTab();
	if( !pTableWidget ) {
		pWnd->printReturn(LEVEL_ERROR, "Can't get table widget");
		return;
		}
	int curRow = pTableWidget->currentRow();

	if( pWnd->pg_main.load_single_line_ltu_setup() == WR_ERROR) return;

	caput_assign_CLTU_with_input_row(pTableWidget, curRow );
#if USE_IMMEDIATE_LTU_SETUP
	usleep(500000);
	caput_setup_CLTU_with_input_row(pTableWidget, curRow );
#endif

	if( (pWnd->passwd_timeset_check == 0) && (pWnd->passwd_master_check == 1) )
		pWnd->pg_main.make_blinking_final_setup();

}
void CltuTimingSet::clicked_btn_reload()
{
	MainWindow *pWnd = (MainWindow*)pm;
//	printf("reload value: %d\n", current_query_shot);

	if( pWnd->pg_main.load_stored_ltu_setup(current_query_shot) == WR_ERROR ) return;

	flush_ERROR_cnt = 0;
	timer_assign_machine->start(LTU_SETUP_FLUSH_GAP);

	if( (pWnd->passwd_timeset_check == 0) && (pWnd->passwd_master_check == 1) )
		pWnd->pg_main.make_blinking_final_setup();

}
#if 0
void CltuTimingSet::idchanged_cb_timing_port(int ID)
{
	MainWindow *pWnd = (MainWindow*)pm;

	bool isNew;

	isNew = pWnd->Is_New_R2_name(   (char*)pWnd->cb_timing_system->currentText().toAscii().constData()    );

	if( isNew ) return;
/*
	if( ID != 4 ) {
		pWnd->frame_section2->setEnabled(false);
		pWnd->frame_section3->setEnabled(false);
	} else {
		pWnd->frame_section2->setEnabled(true);
		pWnd->frame_section3->setEnabled(true);
	}
*/
}
#endif
// if select DDS #2 in combo box, maximum frequency is set to 100 KHz
// others set to 200 KHz
#if 0
void CltuTimingSet::idchanged_cb_timing_system(QString qstr)
{
	MainWindow *pWnd = (MainWindow*)pm;
			
//	int cnt = pWnd->cb_timing_sec1Freq->count();
//	pWnd->printStatus(LEVEL_NONE,"freq cnt: %d", cnt);
//	for( int i=0; i<cnt; i++) {
//		pWnd->cb_timing_sec1Freq->removeItem(i);
//	}

//	pWnd->cb_timing_port->clear();

//	pWnd->cb_timing_sec1Freq->clear();
//	pWnd->cb_timing_sec2Freq->clear();
//	pWnd->cb_timing_sec3Freq->clear();


	if( pWnd->Is_New_R2_name(qstr.toAscii().constData()) ) {
//		pWnd->cb_timing_port->insertItems(0, qstrlist_R2_Port);
		
//		pWnd->frame_section2->setEnabled(true);
//		pWnd->frame_section3->setEnabled(true);
	} 
	else {
//		pWnd->cb_timing_port->insertItems(0, qstrlist_R1_Port);
		
//		pWnd->frame_section2->setEnabled(false);
//		pWnd->frame_section3->setEnabled(false);
	}
	
#if 0	
//	if( !strcmp(pWnd->str_sysName[HALPHA_LTU], qstr.toAscii().constData() ) ) 
	if( pWnd->get_system_ID( qstr.toAscii().constData() ) == HALPHA_LTU )
	{
		for( int i=0; i< 8; i++) {
			pWnd->cb_timing_sec1Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec2Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec3Freq->insertItem(i, qstrlist_Clock.at(i)  );
		}
	} 
//	else if( !strcmp(pWnd->str_sysName[DDS1A_LTU], qstr.toAscii().constData() ) ) 
	else if( pWnd->get_system_ID( qstr.toAscii().constData() ) == DDS1A_LTU )
	{
		for( int i=0; i< 9; i++) {
			pWnd->cb_timing_sec1Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec2Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec3Freq->insertItem(i, qstrlist_Clock.at(i)  );
		}
	} 
	else {
//		pWnd->printStatus(LEVEL_NONE,"qstrlist_Clock.size(): %d", qstrlist_Clock.size() );
		for( int i=0; i< qstrlist_Clock.size(); i++) {
			pWnd->cb_timing_sec1Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec2Freq->insertItem(i, qstrlist_Clock.at(i)  );
//			pWnd->cb_timing_sec3Freq->insertItem(i, qstrlist_Clock.at(i)  );
		}
	}
#endif

	for( int i=0; i< qstrlist_Clock.size(); i++) {
//		pWnd->cb_timing_sec1Freq->insertItem(i, qstrlist_Clock.at(i)  );
//		pWnd->cb_timing_sec2Freq->insertItem(i, qstrlist_Clock.at(i)  );
//		pWnd->cb_timing_sec3Freq->insertItem(i, qstrlist_Clock.at(i)  );
	}
	

	// 2011.4.26   for R2 initial clock to 1Hz
//	pWnd->cb_timing_sec1Freq->setCurrentIndex(1 );
//	pWnd->cb_timing_sec2Freq->setCurrentIndex(1 );
//	pWnd->cb_timing_sec3Freq->setCurrentIndex(1 );

	
}
#endif
void CltuTimingSet::cellClicked_tw_load_info(int row, int column)
{
	MainWindow *pWnd = (MainWindow*)pm;

	QString qstrInfo;
	
//	printf("row: %d, column: %d\n", row, column);

	QTableWidget *pTableWidget;

	pTableWidget = getTableWidget_from_currentTab();

	qstrInfo = "\"";
	qstrInfo.append(pTableWidget->item(row, COL_ID_SYS)->text());
	qstrInfo.append("\"  ch #");
	qstrInfo.append(pTableWidget->item(row, COL_ID_CH)->text());

	pWnd->label_current_item_info->setText( qstrInfo );


}

QTableWidget* CltuTimingSet::getTableWidget_from_currentTab()
{
	MainWindow *pWnd = (MainWindow*)pm;

	if( pWnd->tabWidget->currentIndex() == 0)
		return pWnd->tw_machine;
	else if( pWnd->tabWidget->currentIndex() == 1)
		return pWnd->tw_diagnostics;
	else if( pWnd->tabWidget->currentIndex() == 2)
		return pWnd->tw_ltu2;
	else if( pWnd->tabWidget->currentIndex() == 3)
		return pWnd->tw_ltu2_diag;

	pWnd->printStatus(LEVEL_ERROR, "No correlative system ");
	return 0;
}
QTableWidget* CltuTimingSet::getTableWidget_from_sysname(char *sysname)
{
	MainWindow *pWnd = (MainWindow*)pm;

//	printf("sys name is : %s\n", sysname);
	int sysID = pWnd->get_system_ID(sysname);
	if( sysID < 0 ) {
		printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
		return 0;
	}

	if( pWnd->ltu_feature[sysID][FEAT_VER] == VER1 ) {

		if( pWnd->ltu_feature[sysID][FEAT_CLASS] == C_MCH )
			return pWnd->tw_machine;
		else if (pWnd->ltu_feature[sysID][FEAT_CLASS] == C_DIAG)
			return pWnd->tw_diagnostics;
		else {
			printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
			return 0;
			}
	}
	else if(pWnd->ltu_feature[sysID][FEAT_VER] == VER2 ) 
	{
		if( pWnd->ltu_feature[sysID][FEAT_CLASS] == C_MCH )
			return pWnd->tw_ltu2;
		else if (pWnd->ltu_feature[sysID][FEAT_CLASS] == C_DIAG)
			return pWnd->tw_ltu2_diag;
		else {
			printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
			return 0;
			}
	}
	else {
		printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
		return 0;
	}

	pWnd->printStatus(LEVEL_ERROR, "No correlative system (%s)", sysname);
	return 0;
}

int CltuTimingSet::getTableWidgetID_sysname(char *sysname)
{
	MainWindow *pWnd = (MainWindow*)pm;

//	printf("sys name is : %s\n", sysname);
	int sysID = pWnd->get_system_ID(sysname);
	if( sysID < 0 ) {
		printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
		return -1;
	}

	if( pWnd->ltu_feature[sysID][FEAT_VER] == VER1 ) {

		if( pWnd->ltu_feature[sysID][FEAT_CLASS] == C_MCH )
			return 0;
		else if (pWnd->ltu_feature[sysID][FEAT_CLASS] == C_DIAG)
			return 1;
		else {
			printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
			return -1;
			}
	}
	else if(pWnd->ltu_feature[sysID][FEAT_VER] == VER2 ) 
	{
		if( pWnd->ltu_feature[sysID][FEAT_CLASS] == C_MCH )
			return 2;
		else if (pWnd->ltu_feature[sysID][FEAT_CLASS] == C_DIAG)
			return 3;
		else {
			printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
			return -1;
			}
	}
	else {
		printf(" CltuTimingSet::getTableWidget_from_sysname(%s) .. Failed!\n", sysname );
		return -1;
	}
	

#if 0
	if( strcmp(sysname, pWnd->str_sysName[CTUV1]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[PCS_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[MPS1_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[MPS2_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[MPS3_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ECH_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ICRH_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[FUEL_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[NBI1A_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[NBI1B_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[FBM_LTU]) == 0  ) {
//		printf("return machine\n");
		return 0;
	}
	else if( strcmp(sysname, pWnd->str_sysName[DDS1A_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[DDS1B_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[HALPHA_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[RBA_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ER_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[XCS_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[TS_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ECEI_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[HXR_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ECEHR_LTU]) == 0 ) {
//		printf("return diagnostics\n");
		return 1;
	}
	else if( strcmp(sysname, pWnd->str_sysName[TSS_CTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ATCA_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[RMP_LTU]) == 0 ) {
		
		return 2;
	}
	else if( strcmp(sysname, pWnd->str_sysName[TSS_CTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[ATCA_LTU]) == 0 ||
		strcmp(sysname, pWnd->str_sysName[RMP_LTU]) == 0 ) {
		
		return 3;
	}
#endif

	pWnd->printStatus(LEVEL_ERROR, "No correlative system (%s)", sysname);
	return -1;
}

//void CltuTimingSet::currentItemChanged_tw_timingSet( QTableWidgetItem *curr, QTableWidgetItem *prev)
void CltuTimingSet::itemChanged_tw_timingSet( QTableWidgetItem *curr)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;

	int row, column;
	int sysId;
	int nval;
	QString qstrCurr;
	QString qSysName;
	char bufsysName[8];

//	printf("item changed, %s\n", curr->text().toAscii().constData() );

	if( nPVblockMode != USER_NONE )
	{
//		QMessageBox::information(0, "Kstar TSS", tr("Please wait setup done!") );
		return;
	}

/*	printf("item changed inside, %s\n", curr->text().toAscii().constData() ); */
	pTableWidget = getTableWidget_from_currentTab();
	

	row = pTableWidget->currentRow();
	column = pTableWidget->currentColumn();

	sprintf(bufsysName, "%s", pTableWidget->item(row, COL_ID_SYS)->text().toAscii().constData() );
	qSysName = pTableWidget->item(row, COL_ID_SYS)->text();
	sysId =  pWnd->cb_timing_system->findText( qSysName , Qt::MatchExactly);
	if( sysId < 0 ) { QMessageBox::information(0, "Kstar CLTU", tr("system name error!") ); return; }

	qstrCurr = curr->text();	// 2009. 09. 16

//	printf("current row:%d col: %d >> : sysId:%d, name:%s\n", row, column, sysId, bufsysName );	

	nPVblockMode = USER_ITEM_CHANGED;
	if( pWnd->Is_New_R2_id( sysId) ) {
		if( column == COL_ID_CH ) 
		{
			nval = qstrCurr.toInt();
			if( (nval  < 1) || (nval > 8 )) {
				QMessageBox::information(0, "Kstar CLTU", tr("CH select error! (1-8)") );
				nPVblockMode = USER_NONE;
				return;
			}
		}
		else if(  column == COL_ID2_POL )  // set trigger level R/F
		{
/*			if( ( strcmp(qstrCurr.toAscii().constData(), "R") != 0 ) && (strcmp(qstrCurr.toAscii().constData(), "F") != 0) &&
				( strcmp(qstrCurr.toAscii().constData(), "r") != 0 ) && (strcmp(qstrCurr.toAscii().constData(), "f") != 0)) {
				QMessageBox::information(0, "Kstar CLTU", tr("Polarity set error! (R/F)") );
				nPVblockMode = USER_NONE;
				return;
			}
			if ( strcmp(qstrCurr.toAscii().constData(), "r") == 0 )
				 curr->setText("R");
			else if ( strcmp(qstrCurr.toAscii().constData(), "f") == 0 )
				curr->setText("F");
*/
			if(  strcmp(qstrCurr.toAscii().constData(), "R") == 0 ) 
				;
			else if(strcmp(qstrCurr.toAscii().constData(), "F") == 0)
				;
			else if( strcmp(qstrCurr.toAscii().constData(), "r") == 0 )
				curr->setText("R");
			else if (strcmp(qstrCurr.toAscii().constData(), "f") == 0)
				curr->setText("F");
			else 
			{
				QMessageBox::information(0, "Kstar CLTU", tr("Polarity set error! (R/F)") );
				nPVblockMode = USER_NONE;
				return;
			}

		} 
		else if( column == COL_ID2_MODE)
		{
			if(  strcmp(qstrCurr.toAscii().constData(), "C") == 0 ) 
				;
			else if(strcmp(qstrCurr.toAscii().constData(), "T") == 0)
				;
			else if( strcmp(qstrCurr.toAscii().constData(), "c") == 0 )
				curr->setText("C");

			else if (strcmp(qstrCurr.toAscii().constData(), "t") == 0)
				curr->setText("T");
			
			else {
				QMessageBox::information(0, "Kstar CLTU", tr("Mode set error! (C/T)") );
				nPVblockMode = USER_NONE;
				return;
			}

		}
		else if ( column == COL_ID2_ONOFF )
		{
			if ( curr->checkState() == Qt::Checked )
				curr->setBackground(brush_sep1);
			else 
				curr->setBackground(brush_dirt_red);

		}

	}
	else {

		if( column == COL_ID_CH ) { // set CH
			if( qstrCurr.toInt() == 1 || qstrCurr.toInt() == 2 || qstrCurr.toInt() == 3 || qstrCurr.toInt() == 4 || qstrCurr.toInt() == 5 ) ;
			else { 			
				QMessageBox::information(0, "Kstar CLTU", tr("CH select error! (1-5)") );
				nPVblockMode = USER_NONE;
				return;
			}
		} 
		else if(  column == COL_ID_POL )  // set trigger level R/F
		{
			if(  strcmp(qstrCurr.toAscii().constData(), "R") == 0 ) 
				;
			else if(strcmp(qstrCurr.toAscii().constData(), "F") == 0)
				;
			else if( strcmp(qstrCurr.toAscii().constData(), "r") == 0 )
				curr->setText("R");
			else if (strcmp(qstrCurr.toAscii().constData(), "f") == 0)
				curr->setText("F");
			else 
			{
				QMessageBox::information(0, "Kstar CLTU", tr("Polarity set error! (R/F)") );
				nPVblockMode = USER_NONE;
				return;
			}
		}
#if USE_LIMITED_OVER_1KHZ
		else if( ( column == COL_ID_1_CLK ) || ( column == COL_ID_2_CLK ) || ( column == COL_ID_3_CLK ) )  // set frequency of section 1
		{
			if( qstrCurr.toInt() == 1 || qstrCurr.toInt() == 2 || qstrCurr.toInt() == 5 ||
				qstrCurr.toInt() == 10 || qstrCurr.toInt() == 20 || qstrCurr.toInt() == 50 ||
				qstrCurr.toInt() == 100 || qstrCurr.toInt() == 200 || qstrCurr.toInt() == 500 ||
				qstrCurr.toInt() == 1000 || qstrCurr.toInt() == 2000 || qstrCurr.toInt() == 5000 ||
				qstrCurr.toInt() == 10000 || qstrCurr.toInt() == 12500 || qstrCurr.toInt() == 0) ;
			else {
				QMessageBox::information(0, "Kstar CLTU", tr("must set one of \n1,2,5,\n10,20,50,\n100,200,500\n1000,2000,5000,\n10000,12500, 0 (KHz)\nfrequency ") );
				nPVblockMode = USER_NONE;
				return ;
			}
		}
#endif
	}
	nPVblockMode = USER_NONE;
	
	if( column == COL_ID_DESC)
		return;
	else {
		caput_assign_CLTU_with_input_row(pTableWidget, row );
#if USE_IMMEDIATE_LTU_SETUP
		usleep(500000);
		caput_setup_CLTU_with_input_row(pTableWidget, row );
#endif
		}


	if( (pWnd->passwd_timeset_check == 0) && (pWnd->passwd_master_check == 1) )
		pWnd->pg_main.make_blinking_final_setup();
/*
	pWnd->confirm_final_setup = 0;
	if( !pWnd->pg_main.tmr_blip_timer_isActive() ) {
		pWnd->pg_main.tmr_blip_timer_start();
	}
*/
		

}


void CltuTimingSet::caput_removeCurrentRowItem()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget = getTableWidget_from_currentTab();

	int iCAStatus;
	int curSys, port, multi;
	int row = pTableWidget->currentRow ();
	bool isNew;
//	printf("current row: %d\n", row);

	if( row < 0 ) return;

	// find delete system ID;
	curSys =  pWnd->cb_timing_system->findText( pTableWidget->item(row, COL_ID_SYS)->text() , Qt::MatchExactly);	
	isNew = pWnd->Is_New_R2_id(curSys);
	port = pTableWidget->item(row, COL_ID_CH)->text().toInt() - 1;	
	multi = (port == 4) ? 1: 0; 
//	printf("target for delete system: %d, multi:%d\n", curSys, multi);

//	pWnd->nIsUseThisSystem[curSys] = 0;

	nPVblockMode = USER_REMOVE_ONE_LINE;
	pTableWidget->removeRow ( row );

	if( port < 0 ) {
		pg_set_color_line(pTableWidget);
		nPVblockMode = USER_NONE;
		return;
	}

	if( isNew ) {
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_Enable[port].cid, QString( "0") );
		pWnd->pvCLTU[curSys].R2_Enable[port].status_flag |= PV_STATUS_DELETE;
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_Enable[port].name, 0, ca_message( iCAStatus) );

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_ActiveLow[port].cid, QString( "0") );
		pWnd->pvCLTU[curSys].R2_ActiveLow[port].status_flag |= PV_STATUS_DELETE;
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_ActiveLow[port].name, 0, ca_message( iCAStatus) );

		usleep(10000);
	
		for ( int i=0; i<R2_SECT_CNT; i++) 
		{
			
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[curSys].R2_T0[port][i].cid, 0.0 );
			pWnd->pvCLTU[curSys].R2_T0[port][i].status_flag |= PV_STATUS_DELETE;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_T0[port][i].name, 0, ca_message( iCAStatus) );

			iCAStatus = ca_Put_Double( pWnd->pvCLTU[curSys].R2_T1[port][i].cid, 0.0);
			pWnd->pvCLTU[curSys].R2_T1[port][i].status_flag |= PV_STATUS_DELETE;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_T1[port][i].name, 0, ca_message( iCAStatus) );

			iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_CLOCK[port][i].cid, QString( "1") );
			pWnd->pvCLTU[curSys].R2_CLOCK[port][i].status_flag |= PV_STATUS_DELETE;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_CLOCK[port][i].name, 0, ca_message( iCAStatus) );

			usleep(10000);
		}

	}
	else {

	
		if( multi ) {
			for(int i=0; i<3; i++) 
			{
				iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].mT0[i].cid, QString( "0") );
				pWnd->pvCLTU[curSys].mT0[i].status_flag |= PV_STATUS_DELETE ;
				if( iCAStatus  != ECA_NORMAL )	
					pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].mT0[i].name, 0, ca_message( iCAStatus) );
					
	//			usleep(1000);
				iCAStatus =ca_Put_QStr(  pWnd->pvCLTU[curSys].mT1[i].cid, QString( "0") );
				pWnd->pvCLTU[curSys].mT1[i].status_flag |= PV_STATUS_DELETE ;
				if( iCAStatus  != ECA_NORMAL )	
					pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].mT1[i].name, 0, ca_message( iCAStatus) );

	//			usleep(1000);
				iCAStatus =ca_Put_QStr(  pWnd->pvCLTU[curSys].mCLOCK[i].cid, QString( "0") );
				pWnd->pvCLTU[curSys].mCLOCK[i].status_flag |= PV_STATUS_DELETE ;
				if( iCAStatus  != ECA_NORMAL )	
					pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].mCLOCK[i].name, 0, ca_message( iCAStatus) );
				usleep(10000);
		//		printf("sec:%d, mt0:%d, mt1:%d, mclk:%d, mpol:%d\n", i, pcfg.dbmT0[i],  pcfg.dbmT1[i], pcfg.nmClock[i], pcfg.nmTrig );
			}
		}
		else {
			iCAStatus =ca_Put_QStr(  pWnd->pvCLTU[curSys].T0[port].cid, QString( "0") );
			pWnd->pvCLTU[curSys].T0[port].status_flag |= PV_STATUS_DELETE ;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].T0[port].name, 0, ca_message( iCAStatus) );
	//		usleep(1000);
			iCAStatus =ca_Put_QStr(  pWnd->pvCLTU[curSys].T1[port].cid, QString( "0") );
			pWnd->pvCLTU[curSys].T1[port].status_flag |= PV_STATUS_DELETE ;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].T1[port].name, 0, ca_message( iCAStatus) );
	//		usleep(1000);
			iCAStatus =ca_Put_QStr(  pWnd->pvCLTU[curSys].CLOCK[port].cid, QString( "0")  );
			pWnd->pvCLTU[curSys].CLOCK[port].status_flag |= PV_STATUS_DELETE ;
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].CLOCK[port].name, 0, ca_message( iCAStatus) );

			usleep(10000);
		}
	}

	pg_set_color_line(pTableWidget);

	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[curSys].SETUP_Pt[port].cid, "1" );
	pWnd->pvCLTU[curSys].SETUP_Pt[port].status_flag |= PV_STATUS_DELETE ;
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[curSys].SETUP_Pt[port].name, 1, ca_message( iCAStatus) );

	nPVblockMode = USER_NONE;

}

void CltuTimingSet::removeCurrentRowItem()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget = getTableWidget_from_currentTab();

	int row = pTableWidget->currentRow ();

	if( row < 0 ) return;


	nPVblockMode = USER_REMOVE_ONE_LINE;
	pTableWidget->removeRow ( row );
	pg_set_color_line(pTableWidget);
	usleep(10000);
	nPVblockMode = USER_NONE;

}


void CltuTimingSet::fileOpen(QString filename)
{
	MainWindow *pWnd = (MainWindow*)pm;

	FILE *fp;
	int row, nval;
	char bufLineScan[512];
	char temp1[16], temp2[16];

//	ST_QUEUE_STATUS queueNode;
	

//	printf("filename: %s\n", filename.toAscii().constData() );
	if( (fp = fopen(filename.toAscii().constData(), "r") ) == NULL )
	{	
		pWnd->printStatus(LEVEL_ERROR,"Can not open \"%s\" file for read\n", filename.toAscii().constData() );
		return;
	} else {
		if( fgets(bufLineScan, 512, fp) == NULL ) {
			pWnd->printStatus(LEVEL_ERROR," \"%s\" is invalid\n", filename.toAscii().constData() );
			fclose(fp);
			return;
		}
		sscanf(bufLineScan, "%s",  temp1);
		if( strcmp(temp1, "*TSS*") != 0 ) {
			pWnd->printStatus(LEVEL_ERROR," \"%s\" is invalid\n", filename.toAscii().constData() );
			fclose(fp);
			return;
		}

		pg_remove_all_list();
		nPVblockMode = USER_FILE_OPEN;
		while(1) 
		{
			if( fgets(bufLineScan, 512, fp) == NULL ) break;
			
			if( bufLineScan[0] == '#') ;
			else if( bufLineScan[0] == '&' ) {
				sscanf(bufLineScan, "%s %s %d",  temp1, temp2, &nval);
//				pWnd->le_bliptime_my->setText(QString("%1").arg(nval) );
				QString qstr;
				int iCAStatus = ca_Get_QStr( pWnd->pvBlipTime.cid, qstr );
				if( iCAStatus  != ECA_NORMAL ) 
				{
					pWnd->printStatus(LEVEL_ERROR, "%s, %s\n", pWnd->pvBlipTime.name, ca_message( iCAStatus) );
					fclose(fp);
					return;
				}
				pg_set_bliptime( qstr.toInt() );

			}
			else 
			{
				pg_InsertRow_from_lineBuf(bufLineScan);
			}
		}
		pWnd->tw_machine->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_machine->sortItems( COL_ID_SYS, Qt::AscendingOrder);
		
		pWnd->tw_diagnostics->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_diagnostics->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pWnd->tw_ltu2->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_ltu2->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pWnd->tw_ltu2_diag->sortItems( COL_ID_CH, Qt::AscendingOrder);
		pWnd->tw_ltu2_diag->sortItems( COL_ID_SYS, Qt::AscendingOrder);

		pg_set_color_line(pWnd->tw_machine);
		pg_set_color_line(pWnd->tw_diagnostics);
		pg_set_color_line(pWnd->tw_ltu2);
		pg_set_color_line(pWnd->tw_ltu2_diag);
		
		fclose(fp);
		
		nPVblockMode = USER_NONE;
	}

//	queueNode.caller = QUEUE_ID_SETUP_ALL_LIST;
//	pWnd->put_queue_node(queueNode);
	flush_ERROR_cnt = 0;
	timer_assign_machine->start(LTU_SETUP_FLUSH_GAP);

	pWnd->setWindowTitle(filename);

	pWnd->pg_main.make_blinking_final_setup();

//	pg_setup_CLTU_with_all_list();

	
}

void CltuTimingSet::fileSave(QString filename)
{
	MainWindow *pWnd = (MainWindow*)pm;

	FILE *fp;
	
	if( (fp = fopen(filename.toAscii().constData(), "w") ) == NULL )
	{	
		pWnd->printStatus(LEVEL_NONE,"Can not open \"%s\" file for write\n", filename.toAscii().constData() );
		return;
	} else {
		fprintf(fp, "*TSS*\n");
		fprintf(fp, "# system, description,    port, pol,    start(s), stop(s), clock(KHz), ...\n");
		fprintf(fp, "# Time is Offset from Blip time. \n");

		fprintf(fp, "& Blip_time: %d\n", nBlip_time);

		fileSave_List( pWnd->tw_machine, fp );

		fileSave_List( pWnd->tw_diagnostics, fp);

		fileSave_List_R2( pWnd->tw_ltu2, fp);

		fileSave_List_R2( pWnd->tw_ltu2_diag, fp);
/*
		row = pWnd->tw_machine->rowCount();
		for( int i=0; i<row; i++) {
//			fprintf(fp, ".\n");
//			fprintf(fp, "%s\n", pWnd->tw_machine->item(i, 2)->text().toAscii().constData() ); // description 
			sprintf(buf_sys, "%s", pWnd->tw_machine->item(i, COL_ID_SYS)->text().toAscii().constData() );
			len = strlen(buf_sys);
			strncat(buf_sys, buf_space, 6-len);
			
			sprintf(buf_des, "\"%s\"", pWnd->tw_machine->item(i, COL_ID_DESC)->text().toAscii().constData() );
			len = strlen(buf_des);
			strncat(buf_des, buf_space, 20-len);

			sprintf(buf_1_t0, "%s", pWnd->tw_machine->item(i, COL_ID_1_T0)->text().toAscii().constData() );
			len = strlen(buf_1_t0); strncat(buf_1_t0, buf_space, TM_SIZE-len);

			sprintf(buf_1_t1, "%s", pWnd->tw_machine->item(i, COL_ID_1_T1)->text().toAscii().constData() );
			len = strlen(buf_1_t1); strncat(buf_1_t1, buf_space, TM_SIZE-len);

			sprintf(buf_1_clk, "%s", pWnd->tw_machine->item(i, COL_ID_1_CLK)->text().toAscii().constData() );
			len = strlen(buf_1_clk); strncat(buf_1_clk, buf_space, TM_SIZE-len);
			
			
			sprintf( buf_line, "%s %s %s   %s     %s %s %s",  buf_sys, buf_des, \
					pWnd->tw_machine->item(i, COL_ID_CH)->text().toAscii().constData(), \
					pWnd->tw_machine->item(i, COL_ID_POL)->text().toAscii().constData(), \
					buf_1_t0, buf_1_t1, buf_1_clk );
			
			if(  pWnd->tw_machine->item(i, COL_ID_CH)->text().toInt() == 5 ) {
				sprintf(buf_2_t0, "%s", pWnd->tw_machine->item(i, COL_ID_2_T0)->text().toAscii().constData() );
				len = strlen(buf_2_t0); strncat(buf_2_t0, buf_space, TM_SIZE-len);

				sprintf(buf_2_t1, "%s", pWnd->tw_machine->item(i, COL_ID_2_T1)->text().toAscii().constData() );
				len = strlen(buf_2_t1); strncat(buf_2_t1, buf_space, TM_SIZE-len);

				sprintf(buf_2_clk, "%s", pWnd->tw_machine->item(i, COL_ID_2_CLK)->text().toAscii().constData() );
				len = strlen(buf_2_clk); strncat(buf_2_clk, buf_space, TM_SIZE-len);

				sprintf(buf_3_t0, "%s", pWnd->tw_machine->item(i, COL_ID_3_T0)->text().toAscii().constData() );
				len = strlen(buf_3_t0); strncat(buf_3_t0, buf_space, TM_SIZE-len);

				sprintf(buf_3_t1, "%s", pWnd->tw_machine->item(i, COL_ID_3_T1)->text().toAscii().constData() );
				len = strlen(buf_3_t1); strncat(buf_3_t1, buf_space, TM_SIZE-len);

				sprintf(buf_3_clk, "%s", pWnd->tw_machine->item(i, COL_ID_3_CLK)->text().toAscii().constData() );
				len = strlen(buf_3_clk); strncat(buf_3_clk, buf_space, TM_SIZE-len);
			
				sprintf(buf_multi_tail, "   %s %s %s   %s %s %s", \
					buf_2_t0, buf_2_t1, buf_2_clk, buf_3_t0, buf_3_t1, buf_3_clk );
				strcat(buf_line, buf_multi_tail );
			}

			fprintf(fp, "%s\n", buf_line);

		}
*/
		fclose(fp);
	}
	pWnd->setWindowTitle(filename);

}


void CltuTimingSet::fileSave_List(QTableWidget *pTableWidget, FILE *fp)
{
	#define TM_SIZE	12

	int row, len;
	char buf_sys[8], buf_des[64], buf_space[32], buf_multi_tail[64];
	char buf_1_t0[TM_SIZE], buf_1_t1[TM_SIZE], buf_1_clk[TM_SIZE];
	char buf_2_t0[TM_SIZE], buf_2_t1[TM_SIZE], buf_2_clk[TM_SIZE];
	char buf_3_t0[TM_SIZE], buf_3_t1[TM_SIZE], buf_3_clk[TM_SIZE];
	char buf_line[1024];


	row = pTableWidget->rowCount();
	for( int i=0; i<row; i++) {
		
		sprintf(buf_space, "                                                       "); // must be here.   2010. 7. 28 
		
//			fprintf(fp, ".\n");
//			fprintf(fp, "%s\n", pWnd->tw_machine->item(i, 2)->text().toAscii().constData() ); // description 
		sprintf(buf_sys, "%s", pTableWidget->item(i, COL_ID_SYS)->text().toAscii().constData() );
//		len = strlen(buf_sys);
//		strncat(buf_sys, buf_space, 6-len);
		
		sprintf(buf_des, "\"%s\"", pTableWidget->item(i, COL_ID_DESC)->text().toAscii().constData() );
//		len = strlen(buf_des);
//		strncat(buf_des, buf_space, 20-len);

		sprintf(buf_1_t0, "%s", pTableWidget->item(i, COL_ID_1_T0)->text().toAscii().constData() );
//		len = strlen(buf_1_t0); strncat(buf_1_t0, buf_space, TM_SIZE-len);

		sprintf(buf_1_t1, "%s", pTableWidget->item(i, COL_ID_1_T1)->text().toAscii().constData() );
//		len = strlen(buf_1_t1); strncat(buf_1_t1, buf_space, TM_SIZE-len);

		sprintf(buf_1_clk, "%s", pTableWidget->item(i, COL_ID_1_CLK)->text().toAscii().constData() );
//		len = strlen(buf_1_clk); strncat(buf_1_clk, buf_space, TM_SIZE-len);
		
		
		sprintf( buf_line, "%s %s %s   %s     \t%s %s %s",  buf_sys, buf_des, 
			pTableWidget->item(i, COL_ID_CH)->text().toAscii().constData(),
			pTableWidget->item(i, COL_ID_POL)->text().toAscii().constData(),
			buf_1_t0, buf_1_t1, buf_1_clk );

		
		if(  pTableWidget->item(i, COL_ID_CH)->text().toInt() == 5 ) {
			sprintf(buf_2_t0, "%s", pTableWidget->item(i, COL_ID_2_T0)->text().toAscii().constData() );
//			len = strlen(buf_2_t0); strncat(buf_2_t0, buf_space, TM_SIZE-len);

			sprintf(buf_2_t1, "%s", pTableWidget->item(i, COL_ID_2_T1)->text().toAscii().constData() );
//			len = strlen(buf_2_t1); strncat(buf_2_t1, buf_space, TM_SIZE-len);

			sprintf(buf_2_clk, "%s", pTableWidget->item(i, COL_ID_2_CLK)->text().toAscii().constData() );
//			len = strlen(buf_2_clk); strncat(buf_2_clk, buf_space, TM_SIZE-len);

			sprintf(buf_3_t0, "%s", pTableWidget->item(i, COL_ID_3_T0)->text().toAscii().constData() );
//			len = strlen(buf_3_t0); strncat(buf_3_t0, buf_space, TM_SIZE-len);

			sprintf(buf_3_t1, "%s", pTableWidget->item(i, COL_ID_3_T1)->text().toAscii().constData() );
//			len = strlen(buf_3_t1); strncat(buf_3_t1, buf_space, TM_SIZE-len);

			sprintf(buf_3_clk, "%s", pTableWidget->item(i, COL_ID_3_CLK)->text().toAscii().constData() );
//			len = strlen(buf_3_clk); strncat(buf_3_clk, buf_space, TM_SIZE-len);
		
			sprintf(buf_multi_tail, "   \t%s %s %s   \t%s %s %s", \
				buf_2_t0, buf_2_t1, buf_2_clk, buf_3_t0, buf_3_t1, buf_3_clk );
			strcat(buf_line, buf_multi_tail );
		}

		fprintf(fp, "%s\n", buf_line);

	}
}

void CltuTimingSet::fileSave_List_R2(QTableWidget *pTableWidget, FILE *fp)
{
	#define TM_SIZE	12

	int row, len;
	int nval;
	char buf_sys[8], buf_des[128], buf_space[32];
	char buf_line[1024];


	row = pTableWidget->rowCount();
	for( int i=0; i<row; i++) {
		
		sprintf(buf_space, "                                                       "); // must be here.   2010. 7. 28 
		
//			fprintf(fp, ".\n");
//			fprintf(fp, "%s\n", pWnd->tw_machine->item(i, 2)->text().toAscii().constData() ); // description 
		sprintf(buf_sys, "%s", pTableWidget->item(i, COL_ID_SYS)->text().toAscii().constData() );
		len = strlen(buf_sys);
		strncat(buf_sys, buf_space, 6-len);
		
		sprintf(buf_des, "\"%s\"", pTableWidget->item(i, COL_ID_DESC)->text().toAscii().constData() );
/*		len = strlen(buf_des);
		strncat(buf_des, buf_space, 20-len);
		*/

		nval = ( pTableWidget->item(i, COL_ID2_ONOFF)->checkState() == Qt::Checked ) ? 1 : 0;
		
		sprintf( buf_line, "%s %s %s %d %s   %s     %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",  buf_sys, buf_des, \
				pTableWidget->item(i, COL_ID_CH)->text().toAscii().constData(), nval, \
				pTableWidget->item(i, COL_ID2_MODE)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_POL)->text().toAscii().constData(), \
				
				pTableWidget->item(i, COL_ID2_1_T0)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_1_T1)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_1_CLK)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_2_T0)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_2_T1)->text().toAscii().constData(), \
				pTableWidget->item(i, COL_ID2_2_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_3_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_3_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_3_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_4_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_4_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_4_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_5_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_5_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_5_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_6_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_6_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_6_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_7_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_7_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_7_CLK)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_8_T0)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_8_T1)->text().toAscii().constData(), \
			pTableWidget->item(i, COL_ID2_8_CLK)->text().toAscii().constData() );

		fprintf(fp, "%s\n", buf_line);

	}
}



bool CltuTimingSet::check_pv_access(chid mypv)
{
	if( ca_write_access(mypv) && ca_read_access( mypv) ){
		return true;
	}
	return false;
}


void CltuTimingSet::cb_func_TimeSetting(struct event_handler_args args)
{
//	CltuTimingSet *pwnd = (CltuTimingSet *)args.usr;

	ST_QUEUE_STATUS queueNode;

	ST_ARG_EVENT_CB *pargs = (ST_ARG_EVENT_CB *)args.usr;

	MainWindow* pWnd = (MainWindow *)pargs->pMain;


//	pwnd->mutexEventCB.lock();

//	pwnd->chidEventCB = args.chid;

//	pWnd->printStatus(LEVEL_NONE,"i'm in callbackMonitor\n");


	if( pWnd->pg_timeset.pSTmng->check_is_first_event(pargs) == 1 ){
//		printf("first event\n");
		/* we got first event */
		return;
	}

//	printf("got event\n"); // 100% got the message... here..
	

	if( args.count != ( long) ca_element_count( args.chid ) ){
		pWnd->printStatus(LEVEL_NONE,"CltuTimingSet >> event_handler_args.count is not correct!" );
		return;
	}

	if( pargs->type == TYPE_R2_GET_COMP_NS) {
		queueNode.caller = QUEUE_ID_CALIBRATION;
//		printf("got event.  R2_GET_COMP_NS\n"); // 100% got the message... here..
	}
	else if( pargs->type == TYPE_R2_SET_FREE_RUN)
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

void CltuTimingSet::cb_thread_updateTimingTable(ST_QUEUE_STATUS qnode)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;
	
	int row, find_row, find_col;
	char sysName[8];
//	int iCAStatus;
	char strBuf[64];


//	if( nPVblockMode == USER_RESET_ALL_SYS )
//		return;


//	printf("sys: %d, val: %f: mode:%d, type:%d, Port:%d\n", qnode.pEventArg->system, qnode.dval, qnode.pEventArg->mode, qnode.pEventArg->type, qnode.pEventArg->port );
	strcpy(sysName, pWnd->str_sysName[ qnode.pEventArg->system ] );
//	printf("sys:%s\n", sysName );

	pTableWidget = getTableWidget_from_sysname(sysName);


	if( pSTmng->get_pv_status_flag(qnode.pEventArg) & PV_STATUS_DELETE ) {
		pSTmng->set_pv_status_delete_clear(qnode.pEventArg);
		return;
	}


	if( qnode.pEventArg->type != TYPE_R2_GET_COMP_NS ) 
	{

		row = pTableWidget->rowCount();
		if( row < 1 ) { 
			pWnd->printReturn(LEVEL_WARNING,"somebody touched \"%s\" any system in list table !!!\n", qnode.pEventArg->pvname);
			return;
		}

	//	printf("row: %d\n", row);
	// printf("sys: %d, val: %f: mode:%d, type:%d, Port:%d\n", qnode.pEventArg->system, qnode.dval, qnode.pEventArg->mode, qnode.pEventArg->type, qnode.pEventArg->port );

		find_row = -1;
		find_col = -1;
		for(int i=0; i<row; i++) 
		{
	//		pWnd->printStatus(LEVEL_NONE,"sys name : %s\n", pTableWidget->item(i, 1)->text().toAscii().constData() );
			if(	!strcmp( pTableWidget->item(i, COL_ID_SYS)->text().toAscii().constData(), sysName) )
			{
				if( qnode.pEventArg->mode == MODE_MULTI ) {
					if( pTableWidget->item(i, COL_ID_CH)->text().toInt() == 5 ) {
						find_row = i;
						break;
					}
				}
				else if(  qnode.pEventArg->mode == MODE_SINGLE ) {
					if( pTableWidget->item(i, COL_ID_CH)->text().toInt() == (qnode.pEventArg->port+1) ) {
						find_row = i;
						break;
					}
				
				}
				else if(  qnode.pEventArg->mode == MODE_NEW_R2) {
					if( pTableWidget->item(i, COL_ID_CH)->text().toInt() == (qnode.pEventArg->port+1) ) {
						find_row = i;
						break;
					}
				}
				else {
					pWnd->printReturn(LEVEL_ERROR,"ERROR: there is no matched channel \"%s\" ch: %d\n", sysName, qnode.pEventArg->port+1 );
					break;
				}
			}
		}

	//	printf("find_row:%d\n", find_row );

		if( find_row < 0) {
			if( nPVblockMode != USER_RESET_ALL_SYS )
				pWnd->printReturn(LEVEL_WARNING,"somebody touched \"%s\" ch: %d\n", sysName, qnode.pEventArg->port+1 );
			return;
		}

		if( !(pSTmng->get_pv_status_flag(qnode.pEventArg) & PV_STATUS_TOUCH) ){
			pWnd->printReturn(LEVEL_WARNING,"\"%s\" was corrupted!\n", qnode.pEventArg->pvname);
		}
	}


	switch( qnode.pEventArg->type )
	{
		case TYPE_SINGLE_TRIG:
			if( qnode.dval > 0 ) pTableWidget->item(find_row, COL_ID_POL)->setText( "F" );
			else pTableWidget->item(find_row, COL_ID_POL)->setText( "R" );
			
			find_col = COL_ID_POL;

			pSTmng->print_return_msg(qnode );
			break;
		case TYPE_SINGLE_T0:
			find_col = COL_ID_1_T0;
			cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
			pTableWidget->item(find_row, COL_ID_1_T0)->setText( strBuf);

			pSTmng->print_return_msg(qnode );
			break;
		case TYPE_SINGLE_T1:
			find_col = COL_ID_1_T1;
#if 0
			if( qnode.pEventArg->system == DDS1A_LTU   ) {
				int clk = (int)(pTableWidget->item(find_row, COL_ID_1_CLK)->text().toFloat()*10000.+0.5)/10;
				if( (pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) != 4 )
					cvt_Double_to_String( qnode.dval - pg_getTimeTail(clk)-nBlip_time, strBuf );
				else 
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
				
				pTableWidget->item(find_row, COL_ID_1_T1)->setText( strBuf   );
			}
			else if(qnode.pEventArg->system == DDS1B_LTU) {
				int clk = (int)(pTableWidget->item(find_row, COL_ID_1_CLK)->text().toFloat()*10000.+0.5)/10;
				if( ((pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) == 0) ||
  					((pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) == 1) 	)
					cvt_Double_to_String( qnode.dval - pg_getTimeTail(clk)-nBlip_time, strBuf );
				else 
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
				pTableWidget->item(find_row, COL_ID_1_T1)->setText( strBuf   );
			}
#endif			
			if ( (qnode.pEventArg->system == HALPHA_LTU) && 
					((pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) == ONLY_TARGET_M6802_PORT) )  {
				pTableWidget->item(find_row, COL_ID_1_T1)->setText( QString("%1").arg( qnode.dval - HALPHA_DAQ_TIME_TAIL-nBlip_time  )	 );

			}
/* 2012. 9. 6 request from TGLee */
/*			else if ( (qnode.pEventArg->system == ECEHR_LTU) && 
					((pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) == ONLY_TARGET_M6802_PORT) )  {
				pTableWidget->item(find_row, COL_ID_1_T1)->setText( QString("%1").arg( qnode.dval - HALPHA_DAQ_TIME_TAIL-nBlip_time  )	 );

			} */
			else {
				cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
				pTableWidget->item(find_row, COL_ID_1_T1)->setText( strBuf );
			}

			pSTmng->print_return_msg(qnode );
			break;
		case TYPE_SINGLE_CLOCK:
			find_col = COL_ID_1_CLK;
			pTableWidget->item(find_row, COL_ID_1_CLK)->setText( QString("%1").arg( qnode.dval / 1000. )   );

			pSTmng->print_return_msg(qnode );
			break;

		case TYPE_MULTI_TRIG:
			find_col = COL_ID_POL;
				if( qnode.dval > 0 ) pTableWidget->item(find_row, COL_ID_POL)->setText( "F" );
				else pTableWidget->item(find_row, COL_ID_POL)->setText( "R" );
				
				pSTmng->print_return_msg(qnode );
				break;
		case TYPE_MULTI_CLOCK:
				if( qnode.pEventArg->port == 0 ) {
					find_col = COL_ID_1_CLK;
					pTableWidget->item(find_row, COL_ID_1_CLK)->setText( QString("%1").arg( qnode.dval / 1000. )   );
				} 
				else if ( qnode.pEventArg->port == 1 ) {
					find_col = COL_ID_2_CLK;
					pTableWidget->item(find_row, COL_ID_2_CLK)->setText( QString("%1").arg( qnode.dval / 1000. )   );
				}
				else if (  qnode.pEventArg->port == 2 ) {
					find_col = COL_ID_3_CLK;
					pTableWidget->item(find_row, COL_ID_3_CLK)->setText( QString("%1").arg( qnode.dval / 1000. )   );
				}
				else
					pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied Multi-Clock port: %d\n", qnode.pEventArg->port );

				pSTmng->print_return_msg(qnode );
				break;
		case TYPE_MULTI_T0:
				if( qnode.pEventArg->port == 0 ) {
					find_col = COL_ID_1_T0;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_1_T0)->setText( strBuf  );
				} 
				else if ( qnode.pEventArg->port == 1 ) {
					find_col = COL_ID_2_T0;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_2_T0)->setText( strBuf  );
				}
				else if (  qnode.pEventArg->port == 2 ) {
					find_col = COL_ID_3_T0;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_3_T0)->setText( strBuf  );
				}
				else
					pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied Multi-T0 port: %d\n", qnode.pEventArg->port );

				pSTmng->print_return_msg(qnode );
				break;
		case TYPE_MULTI_T1:
				if( qnode.pEventArg->port == 0 ) {
					find_col = COL_ID_1_T1;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_1_T1)->setText( strBuf  );
				} 
				else if ( qnode.pEventArg->port == 1 ) {
					find_col = COL_ID_2_T1;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_2_T1)->setText( strBuf  );
				}
				else if (  qnode.pEventArg->port == 2 ) {
					find_col = COL_ID_3_T1;
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
					pTableWidget->item(find_row, COL_ID_3_T1)->setText( strBuf  );
				}
				else
					pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied Multi-T1 port: %d\n", qnode.pEventArg->port );

				pSTmng->print_return_msg(qnode );
				break;
		case TYPE_SINGLE_SETUP:
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_MULTI_SETUP:
			pSTmng->print_return_msg(qnode);
			break;

		/**************************************************************/
		/*	 In case of New R2 */
		case TYPE_R2_ActiveLow:
			find_col = COL_ID2_POL;
			if( qnode.dval > 0 ) pTableWidget->item(find_row, COL_ID2_POL)->setText( "F" );
				else pTableWidget->item(find_row, COL_ID2_POL)->setText( "R" );
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_R2_Enable:
			find_col = COL_ID2_ONOFF;
			if( qnode.dval > 0 ) pTableWidget->item(find_row, COL_ID2_ONOFF)->setCheckState(Qt::Checked);
				else pTableWidget->item(find_row, COL_ID2_ONOFF)->setCheckState(Qt::Unchecked);
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_R2_Mode:
			find_col = COL_ID2_MODE;
			if( qnode.dval > 0 ) pTableWidget->item(find_row, COL_ID2_MODE)->setText( "T" );
				else pTableWidget->item(find_row, COL_ID2_MODE)->setText( "C" );
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_R2_CLOCK:
			find_col = (qnode.pEventArg->arg *3 + 8 );
			pTableWidget->item(find_row, (qnode.pEventArg->arg *3 + 8 ) )->setText( QString("%1").arg( qnode.dval / 1000. )   );
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_R2_T0:
			find_col = (qnode.pEventArg->arg *3 + 6 );
			cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
			pTableWidget->item(find_row, (qnode.pEventArg->arg *3 + 6 ))->setText( strBuf);

			pSTmng->print_return_msg(qnode );
			break;
		case TYPE_R2_T1:
#if 0
			if( qnode.pEventArg->system == DDS1A_LTU   ) {
				find_col = COL_ID2_1_T1;
				cvt_Double_to_String( qnode.dval - DDS1A_LTU_TAIL_V2_100SEC - nBlip_time, strBuf );
				pTableWidget->item(find_row, COL_ID2_1_T1)->setText( strBuf   );
			}
			else if(qnode.pEventArg->system == DDS1B_LTU) {
				find_col = COL_ID2_1_T1;
				if( (pTableWidget->item(find_row, COL_ID_CH)->text().toInt()-1) <= 3  )
					cvt_Double_to_String( qnode.dval - DDS1A_LTU_TAIL_V2_100SEC - nBlip_time, strBuf );
				else 
					cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
				pTableWidget->item(find_row, COL_ID2_1_T1)->setText( strBuf   );
			}
			else
			{
#endif
				find_col = (qnode.pEventArg->arg *3 + 7 );
				cvt_Double_to_String( qnode.dval-nBlip_time, strBuf );
				pTableWidget->item(find_row, (qnode.pEventArg->arg *3 + 7 ))->setText( strBuf);
//			}

			pSTmng->print_return_msg(qnode );
			break;
		case TYPE_R2_SETUP:
			pSTmng->print_return_msg(qnode);
			break;
		case TYPE_R2_GET_COMP_NS:
			pWnd->lb_LTU_COMP_TICK->setText( QString( "%1").arg( qnode.dval  ) );
			break;
			
		default: 
			pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied type: %d\n", qnode.pEventArg->type );
			break;
	}
/*
	if( find_row >= 0 && find_col >= 0 ) {
		pTableWidget->item(find_row, find_col)->setBackground(brush_dirt_red);
		update();  // doesn't work.	
	}
*/
//	pWnd->update();  // doesn't work.
//	pTableWidget->repaint();


}

void CltuTimingSet::timerFunc_assign_machine()
{
	static int tmr_row = 0, tmr_cnt = 0;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_machine ) 
	{
		timer_First_machine = 0;
		
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row = pWnd->tw_machine->rowCount();
//		printf("tmr_row: %d\n", tmr_row);
		if( tmr_row < 1 ) 
		{
			timer_assign_machine->stop();
			timer_First_machine  = 1;
			pWnd->printStatus(LEVEL_NOTICE,"NOTICE: No target in the machine tab!\n" );
			
			tmr_row = tmr_cnt = 0;
			nPVblockMode = USER_NONE;
			timer_assign_diagnostics->start(LTU_SETUP_FLUSH_GAP);
			return;
		}
	} 
	else 
	{
		if( tmr_cnt < tmr_row ) {
//			printf("1: tmr_row: %d: tmr_cnt: %d\n", tmr_row, tmr_cnt);
			if( caput_assign_CLTU_with_input_row(pWnd->tw_machine,  tmr_cnt ) == WR_ERROR )
				flush_ERROR_cnt++;
			tmr_cnt++;
		}
#if USE_IMMEDIATE_LTU_SETUP
		else if( tmr_cnt < tmr_row*2 ) {
//			printf("2: %d\n", tmr_cnt - tmr_row );
			caput_setup_CLTU_with_input_row(pWnd->tw_machine,  tmr_cnt - tmr_row );  // final setup timing board...
			tmr_cnt++;
		}
#endif
		else 
		{
//			printf("3: tmr_row: %d: tmr_cnt: %d\n", tmr_row, tmr_cnt);
			timer_assign_machine->stop();			
			timer_First_machine  = 1;
			tmr_row = tmr_cnt = 0;
			nPVblockMode = USER_NONE;
			timer_assign_diagnostics->start(LTU_SETUP_FLUSH_GAP);
		}
	}
}
void CltuTimingSet::timerFunc_assign_diagnostics()
{
	static int tmr_row1 = 0, tmr_cnt1 = 0;
//	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_diagnostics ) 
	{
		timer_First_diagnostics = 0;
		
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row1 = pWnd->tw_diagnostics->rowCount();
//		printf("tmr_row1: %d\n", tmr_row1);
		if( tmr_row1 < 1 ) 
		{
			timer_assign_diagnostics->stop();
			timer_First_diagnostics  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the diagnostic tab!\n" );
			
			tmr_row1 = tmr_cnt1 = 0;
			nPVblockMode = USER_NONE;
			timer_assign_ltu2->start(LTU_SETUP_FLUSH_GAP);
			return;
		}
	} 
	else 
	{
		if( tmr_cnt1 < tmr_row1 ) {
//			printf("1: tmr_row1: %d: tmr_cnt1: %d\n", tmr_row1, tmr_cnt1);
			if( caput_assign_CLTU_with_input_row(pWnd->tw_diagnostics,  tmr_cnt1 ) == WR_ERROR )
				flush_ERROR_cnt++;
			tmr_cnt1++;
		}
#if USE_IMMEDIATE_LTU_SETUP		
		else if( tmr_cnt1 < tmr_row1*2 ) {
//			printf("2: %d\n", tmr_cnt1 - tmr_row1 );
			caput_setup_CLTU_with_input_row( pWnd->tw_diagnostics, tmr_cnt1 - tmr_row1 );  // final setup timing board...
			tmr_cnt1++;
		}
#endif
		else 
		{
//			printf("3: tmr_row1: %d: tmr_cnt1: %d\n", tmr_row1, tmr_cnt1);
			timer_assign_diagnostics->stop();
			timer_First_diagnostics  = 1;
			tmr_row1 = tmr_cnt1 = 0;
			nPVblockMode = USER_NONE;
			timer_assign_ltu2->start(LTU_SETUP_FLUSH_GAP);
		}
	}
}
void CltuTimingSet::timerFunc_assign_ltu2()
{
	static int tmr_row2 = 0, tmr_cnt2 = 0;
//	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_ltu2) 
	{
		timer_First_ltu2 = 0;
		
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row2 = pWnd->tw_ltu2->rowCount();
//		printf("tmr_row2: %d\n", tmr_row2);
		if( tmr_row2 < 1 ) 
		{
			timer_assign_ltu2->stop();
			timer_First_ltu2  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the new LTU tab!\n" );
			
			tmr_row2 = tmr_cnt2 = 0;
			nPVblockMode = USER_NONE;
			timer_assign_ltu2_diag->start(LTU_SETUP_FLUSH_GAP);
			return;
			
		}
	} 
	else 
	{
		if( tmr_cnt2 < tmr_row2 ) {
//			printf("1: tmr_row2: %d: tmr_cnt2: %d\n", tmr_row2, tmr_cnt2);
			if( caput_assign_CLTU_with_input_row(pWnd->tw_ltu2,  tmr_cnt2 ) == WR_ERROR )
				flush_ERROR_cnt++;
			tmr_cnt2++;
		}
#if USE_IMMEDIATE_LTU_SETUP		
		else if( tmr_cnt2 < tmr_row2*2 ) {
//			printf("2: %d\n", tmr_cnt2 - tmr_row2 );
			caput_setup_CLTU_with_input_row( pWnd->tw_ltu2, tmr_cnt2 - tmr_row2 );  // final setup timing board...
			tmr_cnt2++;
		}
#endif
		else 
		{
//			printf("3: tmr_row2: %d: tmr_cnt2: %d\n", tmr_row2, tmr_cnt2);
			timer_assign_ltu2->stop();
			timer_First_ltu2  = 1;
			tmr_row2 = tmr_cnt2 = 0;
			nPVblockMode = USER_NONE;
			timer_assign_ltu2_diag->start(LTU_SETUP_FLUSH_GAP);

		}
	}
}
void CltuTimingSet::timerFunc_assign_ltu2_diag()
{
	static int tmr_row3 = 0, tmr_cnt3 = 0;
//	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_ltu2_diag) 
	{
		timer_First_ltu2_diag = 0;
		
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row3 = pWnd->tw_ltu2_diag->rowCount();
//		printf("tmr_row3: %d\n", tmr_row3);
		if( tmr_row3 < 1 ) 
		{
			timer_assign_ltu2_diag->stop();
			timer_First_ltu2_diag  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the new LTU tab!\n" );
			
			tmr_row3 = tmr_cnt3 = 0;
			nPVblockMode = USER_NONE;
			return;
		}
	} 
	else 
	{
		if( tmr_cnt3 < tmr_row3 ) {
//			printf("1: tmr_row3: %d: tmr_cnt3: %d\n", tmr_row3, tmr_cnt3);
			if( caput_assign_CLTU_with_input_row(pWnd->tw_ltu2_diag,  tmr_cnt3 ) == WR_ERROR )
				flush_ERROR_cnt++;
			tmr_cnt3++;
		}
#if USE_IMMEDIATE_LTU_SETUP		
		else if( tmr_cnt3 < tmr_row3*2 ) {
//			printf("2: %d\n", tmr_cnt3 - tmr_row3 );
			caput_setup_CLTU_with_input_row( pWnd->tw_ltu2_diag, tmr_cnt3 - tmr_row3 );  // final setup timing board...
			tmr_cnt3++;
		}
#endif
		else 
		{
//			printf("3: tmr_row3: %d: tmr_cnt3: %d\n", tmr_row3, tmr_cnt3);
			timer_assign_ltu2_diag->stop();
			timer_First_ltu2_diag  = 1;
			tmr_row3 = tmr_cnt3 = 0;
			nPVblockMode = USER_NONE;

			if( flush_ERROR_cnt > 0) 
				pWnd->printStatus(LEVEL_ERROR, "Error happened! count %d\n", flush_ERROR_cnt );
		}
	}
}

void CltuTimingSet::timerFunc_setup_machine()
{
	static int tmr_row12 = 0, tmr_cnt12 = 0;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_machine ) 
	{
		timer_First_machine = 0;
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row12 = pWnd->tw_machine->rowCount();
		if( tmr_row12 < 1 ) 
		{
			timer_setup_machine->stop();
			timer_First_machine  = 1;
			pWnd->printStatus(LEVEL_NOTICE,"NOTICE: No target in the machine tab!\n" );
			
			tmr_row12 = tmr_cnt12 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_diagnostics->start(LTU_SETUP_FLUSH_GAP);
			return;
		}
	} else  {
		if( tmr_cnt12 < tmr_row12 ) {
			if( caput_setup_CLTU_with_input_row(pWnd->tw_machine,  tmr_cnt12 ) == WR_ERROR )
				flush_ERROR_cnt++;
			
			tmr_cnt12++;
		}
		else 
		{
			timer_setup_machine->stop();			
			timer_First_machine  = 1;
			tmr_row12 = tmr_cnt12 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_diagnostics->start(LTU_SETUP_FLUSH_GAP);
		}
	}
}
void CltuTimingSet::timerFunc_setup_diagnostics()
{
	static int tmr_row13 = 0, tmr_cnt13 = 0;
	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_diagnostics ) 
	{
		timer_First_diagnostics = 0;
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row13 = pWnd->tw_diagnostics->rowCount();
		if( tmr_row13 < 1 ) 
		{
			timer_setup_diagnostics->stop();
			timer_First_diagnostics  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the diagnostic tab!\n" );
			
			tmr_row13 = tmr_cnt13 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_ltu2->start(LTU_SETUP_FLUSH_GAP);
			return;
		}
	} else {
	
		if( tmr_cnt13 < tmr_row13 ) {
			if( caput_setup_CLTU_with_input_row(pWnd->tw_diagnostics,  tmr_cnt13 ) == WR_ERROR )
				flush_ERROR_cnt++;
			
			tmr_cnt13++;
		}
		else 
		{
			timer_setup_diagnostics->stop();
			timer_First_diagnostics  = 1;
			tmr_row13 = tmr_cnt13 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_ltu2->start(LTU_SETUP_FLUSH_GAP);

		}
	}
}
void CltuTimingSet::timerFunc_setup_ltu2()
{
	static int tmr_row14 = 0, tmr_cnt14 = 0;
	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_ltu2 ) 
	{
		timer_First_ltu2 = 0;
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row14 = pWnd->tw_ltu2->rowCount();
		if( tmr_row14 < 1 ) 
		{
			timer_setup_ltu2->stop();
			timer_First_ltu2  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the machine V2 LTU tab!\n" );

			tmr_row14 = tmr_cnt14 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_ltu2_diag->start(LTU_SETUP_FLUSH_GAP);
			return;
						
		}
	} else {
	
		if( tmr_cnt14 < tmr_row14 ) {
			if( caput_setup_CLTU_with_input_row(pWnd->tw_ltu2,  tmr_cnt14 ) == WR_ERROR )
				flush_ERROR_cnt++;
			
			tmr_cnt14++;
		}
		else 
		{
			timer_setup_ltu2->stop();
			timer_First_ltu2  = 1;
			tmr_row14 = tmr_cnt14 = 0;
			nPVblockMode = USER_NONE;
			timer_setup_ltu2_diag->start(LTU_SETUP_FLUSH_GAP);
			
		}
	}
}
void CltuTimingSet::timerFunc_setup_ltu2_diag()
{
	static int tmr_row15 = 0, tmr_cnt15 = 0;
	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	if( timer_First_ltu2_diag ) 
	{
		timer_First_ltu2_diag = 0;
		nPVblockMode = USER_RESET_ALL_SYS;
		tmr_row15 = pWnd->tw_ltu2_diag->rowCount();
		if( tmr_row15 < 1 ) 
		{
			timer_setup_ltu2_diag->stop();
			timer_First_ltu2_diag  = 1;
			pWnd->printStatus(LEVEL_NOTICE, "NOTICE: No target in the diagnostic V2 LTU tab!\n" );
			
			tmr_row15 = tmr_cnt15 = 0;
			nPVblockMode = USER_NONE;
			iCAStatus=ca_Put_QStr(	pWnd->pvTSSConfirm.cid, QString( "1") ); // Notify confirmed to CCS /// 2010.08.17 
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" put 1,  %s\n", pWnd->pvTSSConfirm.name, ca_message( iCAStatus) );

			return;
		}
	} else {
	
		if( tmr_cnt15 < tmr_row15 ) {
			if( caput_setup_CLTU_with_input_row(pWnd->tw_ltu2_diag,  tmr_cnt15 ) == WR_ERROR )
				flush_ERROR_cnt++;
			
			tmr_cnt15++;
		}
		else 
		{
			timer_setup_ltu2_diag->stop();
			timer_First_ltu2_diag  = 1;
			tmr_row15 = tmr_cnt15 = 0;
			nPVblockMode = USER_NONE;


			if( flush_ERROR_cnt > 0) { 
				pWnd->printStatus(LEVEL_ERROR, "Error happened! count %d\n", flush_ERROR_cnt );
				pWnd->printStatus(LEVEL_NOTICE, "but set the TSS confirm PV\n" );
			}
//			else  2013. 7. 8. 
//			{
				iCAStatus=ca_Put_QStr(	pWnd->pvTSSConfirm.cid, QString( "1") ); // Notify confirmed to CCS 
				if( iCAStatus  != ECA_NORMAL )	
					pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" put 1,  %s\n", pWnd->pvTSSConfirm.name, ca_message( iCAStatus) );
//			}
			
		}
	}
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
int CltuTimingSet::cb_register_callbackFunc(ST_PV_NODE &node, unsigned int nsys, int mode, int type, int port, int arg)
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


	//DBR_TIME_DOUBLE
	iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( node.cid ), node.cid,
				cb_func_TimeSetting, myargs, 0.0, 0.0, 0.0, &node.eid, DBE_VALUE);
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "CltuTimingSet::cb_register_callbackFunc",  tr("ca_add_masked_array_event() error!"));
		free(myargs);
		return WR_ERROR;
	}
//	cntEvidMain++;

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "CltuTimingSet::cb_register_callbackFunc", tr("ca_pend_io() error!"));
		free(myargs);
		return WR_ERROR;
	}
	node.flag_eid= 1;
//	node.first_event = 1;
	node.status_flag = PV_STATUS_FIRST_EVENT;

	return WR_OK;

}

void CltuTimingSet::cb_clear_callbackFunc(ST_PV_NODE &node)
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
		QMessageBox::information(0, "CltuTimingSet::cb_clear_callbackFunc", tr("ca_clear_event() error!"));
	
	node.eid = NULL;
	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL)
		QMessageBox::information(0, "CltuTimingSet::cb_clear_callbackFunc", tr("ca_pend_io() error!"));
	
	node.flag_eid = 0;
//	node.first_event = 0;
	node.status_flag = PV_STATUS_FIRST_EVENT;

}




float CltuTimingSet::pg_getTimeTail(int frameRate)
{
	float time_tail;

	if( frameRate < 0 )			time_tail = (-1);
	else if( frameRate < 2000 )	time_tail = DDS1A_LTU_TAIL_UNDER_2K;		
	else if( frameRate < 5000 )	time_tail = DDS1A_LTU_TAIL_UNDER_5K;
	else if( frameRate < 10000 )	time_tail = DDS1A_LTU_TAIL_UNDER_10K;
	else time_tail = DDS1A_LTU_TAIL_DEFAULT;

	return time_tail;
}




void CltuTimingSet::pg_set_bliptime(int nval)
{
	MainWindow *pWnd = (MainWindow*)pm;
	nBlip_time = nval;
	pWnd->printStatus(LEVEL_NONE,"Current Blip time: %d\n", nBlip_time );	
}
int CltuTimingSet::pg_get_bliptime()
{
	return nBlip_time;
}

void CltuTimingSet::pg_set_bliptime_ccs(int nval)
{
	nBlip_time_CCS = nval;
}
int CltuTimingSet::pg_get_bliptime_ccs()
{
	return nBlip_time_CCS;
}



void CltuTimingSet::pg_InsertRow_from_lineBuf( char *bufLineScan)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;
	char IsNew=0;
	int nval=0;

	CStringEditor CEditor;
	
	QTableWidgetItem *item[MAX_COL2_NUM];

	int maxCOL=0;

	int row=0;
//	int nCh;

//	int nEnclose;  //2012. 2. 15 --> removed because we use MySQL 2012. 7. 13
	char strSystem[16];
	char strDescription[32];
	char strPort[4];
	char strOnOff[4]; // 2011.3.25
	char strMode[4]; // 2011.3.25 Clock or Trigger /* C, T */
	char strPol[4];
	char strSec1Start[64];
	char strSec1Stop[64];
	char strSec1Freq[8];
	char strSec2Start[64];
	char strSec2Stop[64];
	char strSec2Freq[8];
	char strSec3Start[64];
	char strSec3Stop[64];
	char strSec3Freq[8];

	char strSec4Start[64];
	char strSec4Stop[64];
	char strSec4Freq[8];
	char strSec5Start[64];
	char strSec5Stop[64];
	char strSec5Freq[8];
	char strSec6Start[64];
	char strSec6Stop[64];
	char strSec6Freq[8];
	char strSec7Start[64];
	char strSec7Stop[64];
	char strSec7Freq[8];
	char strSec8Start[64];
	char strSec8Stop[64];
	char strSec8Freq[8];

	

	char buf3[512];


//	sscanf(bufLineScan, "%d %s ", &nEnclose, strSystem  ); removed because we use MySQL 2012. 7. 13
	sscanf(bufLineScan, "%s ", strSystem  );
	IsNew = pWnd->Is_New_R2_name(strSystem);

	
	CEditor.getStrBetweenQuot(bufLineScan, strDescription);
	CEditor.getStrAfterQuot( bufLineScan,  buf3 );
	if( IsNew) {
		sscanf(buf3, "%s %s %s %s  %s %s %s   %s %s %s   %s %s %s   %s %s %s   %s %s %s   %s %s %s   %s %s %s   %s %s %s", \
					strPort, strOnOff, strMode, strPol, \
					strSec1Start, strSec1Stop, strSec1Freq , \
					strSec2Start, strSec2Stop, strSec2Freq , \
					strSec3Start, strSec3Stop, strSec3Freq , \
					strSec4Start, strSec4Stop, strSec4Freq , \
					strSec5Start, strSec5Stop, strSec5Freq , \
					strSec6Start, strSec6Stop, strSec6Freq , \
					strSec7Start, strSec7Stop, strSec7Freq , \
					strSec8Start, strSec8Stop, strSec8Freq  );
		maxCOL = MAX_COL2_NUM;
	}
	else {
		sscanf(buf3, "%s %s  %s %s %s   %s %s %s   %s %s %s", \
					strPort, strPol, \
					strSec1Start, strSec1Stop, strSec1Freq , \
					strSec2Start, strSec2Stop, strSec2Freq , \
					strSec3Start, strSec3Stop, strSec3Freq  );
		maxCOL = MAX_COL_NUM;
	}
/* removed because we use MySQL 2012. 7. 13
	if( nEnclose == 0 ) // 2012. 2. 16
	{
//		printf("%s, ch %s  not enclosed!\n", strSystem, strPort);
		return;		
	}
*/
	for(int i=0; i<maxCOL; i++) {
		item[i] = new QTableWidgetItem();
		item[i]->setTextAlignment(Qt::AlignCenter);
	}

	pTableWidget = getTableWidget_from_sysname(strSystem);
	if( !pTableWidget ) {
		pWnd->printStatus(LEVEL_ERROR, "No correlative system (%s)", strSystem);
		return;
	}
/*
	item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
	if( nOnOff )item0->setCheckState(Qt::Checked);
	else item0->setCheckState(Qt::Unchecked);
*/
//	nCh = atoi(strPort);
//	printf("strPort: %s, nCh: %d\n", strPort, nCh);
//	sprintf(strPort, "%d", nCh+1);
//	printf("strPort: %s, nCh: %d\n", strPort, nCh);
	

	row = pTableWidget->rowCount();
	pTableWidget->setRowCount(row + 1);


	if( IsNew) 
	{
		item[COL_ID_SYS]->setText(strSystem);
		item[COL_ID_DESC]->setText(strDescription); item[COL_ID_DESC]->setTextAlignment(Qt::AlignLeft);
		item[COL_ID_CH]->setText(strPort );

//		item[COL_ID2_ONOFF]->setText(strOnOff);
		nval = atoi(strOnOff);
		item[COL_ID2_ONOFF]->setFlags(item[COL_ID2_ONOFF]->flags() & ~Qt::ItemIsEditable);
		if( nval )item[COL_ID2_ONOFF]->setCheckState(Qt::Checked);
		else item[COL_ID2_ONOFF]->setCheckState(Qt::Unchecked);
	
		
		item[COL_ID2_MODE]->setText(strMode); /* C, T */
		item[COL_ID2_POL]->setText(strPol );
		item[COL_ID2_1_T0]->setText(strSec1Start );
		item[COL_ID2_1_T1]->setText(strSec1Stop );
		item[COL_ID2_1_CLK]->setText(strSec1Freq );
		item[COL_ID2_2_T0]->setText(strSec2Start );
		item[COL_ID2_2_T1]->setText(strSec2Stop );
		item[COL_ID2_2_CLK]->setText(strSec2Freq );
		item[COL_ID2_3_T0]->setText(strSec3Start );
		item[COL_ID2_3_T1]->setText(strSec3Stop );
		item[COL_ID2_3_CLK]->setText(strSec3Freq );
		item[COL_ID2_4_T0]->setText(strSec4Start );
		item[COL_ID2_4_T1]->setText(strSec4Stop );
		item[COL_ID2_4_CLK]->setText(strSec4Freq );
		item[COL_ID2_5_T0]->setText(strSec5Start );
		item[COL_ID2_5_T1]->setText(strSec5Stop );
		item[COL_ID2_5_CLK]->setText(strSec5Freq );
		item[COL_ID2_6_T0]->setText(strSec6Start );
		item[COL_ID2_6_T1]->setText(strSec6Stop );
		item[COL_ID2_6_CLK]->setText(strSec6Freq );
		item[COL_ID2_7_T0]->setText(strSec7Start );
		item[COL_ID2_7_T1]->setText(strSec7Stop );
		item[COL_ID2_7_CLK]->setText(strSec7Freq );
		item[COL_ID2_8_T0]->setText(strSec8Start );
		item[COL_ID2_8_T1]->setText(strSec8Stop );
		item[COL_ID2_8_CLK]->setText(strSec8Freq );

	}
	else {
		item[COL_ID_SYS]->setText(strSystem);
		item[COL_ID_DESC]->setText(strDescription); item[COL_ID_DESC]->setTextAlignment(Qt::AlignLeft);
		item[COL_ID_CH]->setText(strPort );
		item[COL_ID_POL]->setText(strPol );
		item[COL_ID_1_T0]->setText(strSec1Start );
		item[COL_ID_1_T1]->setText(strSec1Stop );
		item[COL_ID_1_CLK]->setText(strSec1Freq );
		if( !strcmp(strPort, "5") ) 
		{
			item[COL_ID_2_T0]->setText(strSec2Start );
			item[COL_ID_2_T1]->setText(strSec2Stop );
			item[COL_ID_2_CLK]->setText(strSec2Freq );
			item[COL_ID_3_T0]->setText(strSec3Start );
			item[COL_ID_3_T1]->setText(strSec3Stop );
			item[COL_ID_3_CLK]->setText(strSec3Freq );
		}

	}


	for(int i=0; i<maxCOL; i++) 
		pTableWidget->setItem(row, i, item[i]);

}

void CltuTimingSet::pg_set_color_line(QTableWidget *pTableWidget)
{
	MainWindow *pWnd = (MainWindow*)pm;
	char strBuf[8];
	QString qstr;
	int rowCnt, toggle=0;
	int colCnt=0;
	

	rowCnt = pTableWidget->rowCount();

	colCnt = pTableWidget->columnCount();

	if( rowCnt < 1) {
		pWnd->printStatus(LEVEL_NONE,"WARNING: There is no target to color assign!\n" );
		return;
	}
	
	sprintf(strBuf, "%s", pTableWidget->item(0, COL_ID_SYS)->text().toAscii().constData() );

	for( int i=0; i< rowCnt; i++) 
	{
		qstr = pTableWidget->item(i, COL_ID_SYS)->text();
//		printf("cur: %d, %s\n", i, qstr.toAscii().constData() );
		
		if( strcmp(qstr.toAscii().constData(), strBuf) != 0 ) {
			toggle = toggle ? 0:1;
			sprintf(strBuf, "%s", qstr.toAscii().constData() );
			
//			printf("toggle: %d, 0x%x, %s\n", toggle, toggle, strBuf);
		}

		if( toggle ) {
			for(int k=0; k<colCnt; k++)
				pTableWidget->item(i, k)->setBackground(brush_sep2);
		} else{
			for(int k=0; k<colCnt; k++)
				pTableWidget->item(i, k)->setBackground(brush_sep1);
		}		
		QFont font;
		font.setBold(true);

		if ( pTableWidget == pWnd->tw_ltu2 || pTableWidget == pWnd->tw_ltu2_diag) {

			if ( pTableWidget->item(i, COL_ID2_ONOFF)->checkState() == Qt::Checked )
				pTableWidget->item(i, COL_ID2_ONOFF)->setBackground(brush_sep1);
			else 
				pTableWidget->item(i, COL_ID2_ONOFF)->setBackground(brush_dirt_red);
			
			pTableWidget->item(i, COL_ID2_1_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_1_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_1_CLK)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_3_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_3_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_3_CLK)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_5_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_5_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_5_CLK)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_7_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_7_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID2_7_CLK)->setForeground(brush_foreColum);

			
			pTableWidget->item(i, COL_ID2_1_T0)->setFont(font);
			pTableWidget->item(i, COL_ID2_1_T1)->setFont(font);
			pTableWidget->item(i, COL_ID2_1_CLK)->setFont(font);
			pTableWidget->item(i, COL_ID2_3_T0)->setFont(font);
			pTableWidget->item(i, COL_ID2_3_T1)->setFont(font);
			pTableWidget->item(i, COL_ID2_3_CLK)->setFont(font);
			pTableWidget->item(i, COL_ID2_5_T0)->setFont(font);
			pTableWidget->item(i, COL_ID2_5_T1)->setFont(font);
			pTableWidget->item(i, COL_ID2_5_CLK)->setFont(font);
			pTableWidget->item(i, COL_ID2_7_T0)->setFont(font);
			pTableWidget->item(i, COL_ID2_7_T1)->setFont(font);
			pTableWidget->item(i, COL_ID2_7_CLK)->setFont(font);
		}
		else {
			pTableWidget->item(i, COL_ID_1_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID_1_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID_1_CLK)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID_3_T0)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID_3_T1)->setForeground(brush_foreColum);
			pTableWidget->item(i, COL_ID_3_CLK)->setForeground(brush_foreColum);

			pTableWidget->item(i, COL_ID_1_T0)->setFont(font);
			pTableWidget->item(i, COL_ID_1_T1)->setFont(font);
			pTableWidget->item(i, COL_ID_1_CLK)->setFont(font);
			pTableWidget->item(i, COL_ID_3_T0)->setFont(font);
			pTableWidget->item(i, COL_ID_3_T1)->setFont(font);
			pTableWidget->item(i, COL_ID_3_CLK)->setFont(font);
		}


		

	}

}

void CltuTimingSet::pg_remove_all_list()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int row;
	
	nPVblockMode = USER_FILE_NEW;
	row = pWnd->tw_machine->rowCount();
	if( row > 0 ) {
		for(int i=0; i<row; i++)
			pWnd->tw_machine->removeRow ( 0 );
	}
	row = pWnd->tw_diagnostics->rowCount();
	if( row > 0 ) {
		for(int i=0; i<row; i++)
			pWnd->tw_diagnostics->removeRow ( 0 );
	}
	row = pWnd->tw_ltu2->rowCount();
	if( row > 0 ) {
		for(int i=0; i<row; i++)
			pWnd->tw_ltu2->removeRow ( 0 );
	}
	row = pWnd->tw_ltu2_diag->rowCount();
	if( row > 0 ) {
		for(int i=0; i<row; i++)
			pWnd->tw_ltu2_diag->removeRow ( 0 );
	}
	usleep(10000);
	nPVblockMode = USER_NONE;
}

void CltuTimingSet::pg_reset_all_list()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int row;
	nPVblockMode = USER_RESET_ALL_SYS;
	
	row = pWnd->tw_machine->rowCount();
	if( row > 0 ) 
	{
		for(int i=0; i<row; i++) 
		{
			pWnd->tw_machine->item(i, COL_ID_1_T0)->setText(QString("0"));
			pWnd->tw_machine->item(i, COL_ID_1_T1)->setText("0");
			pWnd->tw_machine->item(i, COL_ID_1_CLK)->setText("0");
			int port = pWnd->tw_machine->item(i, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
			if( port == PORT_NUM_MULTI ) 
			{
				pWnd->tw_machine->item(i, COL_ID_2_T0)->setText("0");
				pWnd->tw_machine->item(i, COL_ID_2_T1)->setText("0");
				pWnd->tw_machine->item(i, COL_ID_2_CLK)->setText("0");
				pWnd->tw_machine->item(i, COL_ID_3_T0)->setText("0");
				pWnd->tw_machine->item(i, COL_ID_3_T1)->setText("0");
				pWnd->tw_machine->item(i, COL_ID_3_CLK)->setText("0");
			}
		}
	}
	
	row = pWnd->tw_diagnostics->rowCount();
	printf( "row: %d\n", row);
	if( row > 0 ) 
	{
		for(int i=0; i<row; i++) 
		{
			pWnd->tw_diagnostics->item(i, COL_ID_1_T0)->setText("0");
			pWnd->tw_diagnostics->item(i, COL_ID_1_T1)->setText("0");
			pWnd->tw_diagnostics->item(i, COL_ID_1_CLK)->setText("0");
			int port = pWnd->tw_diagnostics->item(i, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
			if( port == PORT_NUM_MULTI ) 
			{
				pWnd->tw_diagnostics->item(i, COL_ID_2_T0)->setText("0");
				pWnd->tw_diagnostics->item(i, COL_ID_2_T1)->setText("0");
				pWnd->tw_diagnostics->item(i, COL_ID_2_CLK)->setText("0");
				pWnd->tw_diagnostics->item(i, COL_ID_3_T0)->setText("0");
				pWnd->tw_diagnostics->item(i, COL_ID_3_T1)->setText("0");
				pWnd->tw_diagnostics->item(i, COL_ID_3_CLK)->setText("0");
			}
		}
	}

	row = pWnd->tw_ltu2->rowCount();
	if( row > 0 ) 
	{
		for(int i=0; i<row; i++) 
		{
			pWnd->tw_ltu2->item(i, COL_ID2_1_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_1_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_1_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_2_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_2_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_2_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_3_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_3_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_3_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_4_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_4_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_4_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_5_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_5_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_5_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_6_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_6_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_6_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_7_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_7_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_7_CLK)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_8_T0)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_8_T1)->setText("0");
			pWnd->tw_ltu2->item(i, COL_ID2_8_CLK)->setText("0");
		}
	}

	row = pWnd->tw_ltu2_diag->rowCount();
	if( row > 0 ) 
	{
		for(int i=0; i<row; i++) 
		{
			pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_1_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_1_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_2_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_2_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_2_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_3_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_3_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_3_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_4_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_4_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_4_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_5_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_5_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_5_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_6_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_6_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_6_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_7_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_7_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_7_CLK)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_8_T0)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_8_T1)->setText("0");
			pWnd->tw_ltu2_diag->item(i, COL_ID2_8_CLK)->setText("0");
		}
	}
	
	nPVblockMode = USER_NONE;
}

void CltuTimingSet::pg_run_time_flush()
{
	timer_assign_machine->start(LTU_SETUP_FLUSH_GAP);
}
void CltuTimingSet::pg_set_query_shot(unsigned int num)
{
	current_query_shot = num;
//	printf("input value: %d\n", current_query_shot);
}
unsigned int CltuTimingSet::pg_get_query_shot()
{
	return current_query_shot;
}

bool CltuTimingSet::pg_is_enclosed(char *givenName, int ch)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;
	int port;
	QString qstr_sysName;
	
	
	pTableWidget = getTableWidget_from_sysname(givenName);

	int row = pTableWidget->rowCount ();
	bool enclosed = false;

	if( row < 1 ){
//		printf("%s, row cnt: %d\n", givenName, row);
		return enclosed;
	}

	for(int i=0; i< row; i++) 
	{
		port = pTableWidget->item(i, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
		qstr_sysName = pTableWidget->item(i, COL_ID_SYS)->text();

		if( !strcmp(qstr_sysName.toAscii().constData(),  givenName )  && ( ch == port ) ) {
			// got it...
			enclosed = true;
			break;
		}

	}

	return enclosed;
}

bool CltuTimingSet::pg_is_enclosed_ltu(char *givenName)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;
	QString qstr_sysName;

	pTableWidget = getTableWidget_from_sysname(givenName);
	int row = pTableWidget->rowCount ();
	bool enclosed = false;

	if( row < 1 ){
		return enclosed;
	}

	for(int i=0; i< row; i++) 
	{
		qstr_sysName = pTableWidget->item(i, COL_ID_SYS)->text();
		if( !strcmp(qstr_sysName.toAscii().constData(),  givenName )   ) {
			// got it...
			enclosed = true;
			break;
		}
	}

	return enclosed;
}

void CltuTimingSet::pg_get_description(char *givenName, int ch, char *strDesc)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;
	int port;
	QString qstr_sysName;
	
	
	pTableWidget = getTableWidget_from_sysname(givenName);
	int row = pTableWidget->rowCount ();


	sprintf(strDesc, "Hello!");

	if( row < 1 ){
		return ;
	}

	for(int i=0; i< row; i++) 
	{
		port = pTableWidget->item(i, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
		qstr_sysName = pTableWidget->item(i, COL_ID_SYS)->text();

		if( !strcmp(qstr_sysName.toAscii().constData(),  givenName )  && ( ch == port ) ) {
			// got it...
			sprintf(strDesc, pTableWidget->item(i, COL_ID_DESC)->text().toAscii().constData() );
			break;
		}

	}

	return ;
}


void CltuTimingSet::caput_reset_all_sys()
{
	int i, sysid, port, nerror;
	int iCAStatus;

	MainWindow *pWnd = (MainWindow*)pm;


	nPVblockMode = USER_RESET_ALL_SYS;
	
	//	sysid = DDS1A_LTU;
	for( sysid=0; sysid<MAX_LTU_CARD; sysid++) {
		
		nerror = 0;

		iCAStatus = ca_Put_Double(  pWnd->pvCLTU[sysid].mTRIG.cid, ACTIVE_HIGH_VAL ); /*( ACTIVE_HIGH_VAL  */
		if( iCAStatus  != ECA_NORMAL )
			nerror++;
		
		for( i=0; i<3; i++) 
		{
			if( nerror > 0) break;

			iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysid].mCLOCK[i].cid, QString( "0") );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
			}
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysid].mT0[i].cid, 0 );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}
					
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysid].mT1[i].cid, 0 );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}
//			usleep(100000);
		}

		for(port=0; port<4; port++) 
		{
			if( nerror > 0) break;

			iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysid].CLOCK[port].cid, QString( "0") );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}

			iCAStatus = ca_Put_Double(  pWnd->pvCLTU[sysid].POL[port].cid, ACTIVE_HIGH_VAL  );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}

			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysid].T0[port].cid, 0 );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}

			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysid].T1[port].cid, 0 );
			if( iCAStatus  != ECA_NORMAL )	{
				nerror++;
				}
		}

		usleep(100000);
		
		for( port=0; port<5; port++) 
		{
			if( nerror > 0) break;

			usleep(100000);
			iCAStatus = ca_Put_StrVal( pWnd->pvCLTU[sysid].SETUP_Pt[port].cid, "1" );
			if( iCAStatus  != ECA_NORMAL ) nerror++;
		}

//		usleep(300000);
		if( nerror > 0) 
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" LTU set to zero... ERROR!", pWnd->str_sysName[sysid] );
		else 
			pWnd->printStatus(LEVEL_NONE," \"%s\" LTU set to zero... OK!", pWnd->str_sysName[sysid] );

	}
		
	nPVblockMode = USER_NONE;
	
}

/* port:  0, 1, 2, 3, 4 */
int CltuTimingSet::caput_CLTU_with_Arg_R2(int nsys, int port,  ST_CLTU_config pcfg)
{
	MainWindow *pWnd = (MainWindow*)pm;

	int iCAStatus, errorCnt;
	int tPort=0;
	
	errorCnt = 0;
//////////////////////////////////////////////////
// 2011. 5. 12   this is for new LTU V2.  woong
// be careful,  clock must be set when trigger setting.
#if 0
	if( nsys == DDS1A_LTU ) {
		if( port == 0 || port == 2 || port == 4 || port == 6 ) {  /* clock */
			tPort = port / 2;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1A[tPort] )==WR_ERROR ) errorCnt++;
		}
		pcfg.dbT1[0] += DDS1A_LTU_TAIL_V2_100SEC;
	}
	else if ( nsys == DDS1B_LTU  ) {
		if( port == 0 || port == 2  ) { 
			tPort = port / 2;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1B[tPort] )==WR_ERROR ) errorCnt++;
		}
		if( port <= 3 )
			pcfg.dbT1[0] += DDS1A_LTU_TAIL_V2_100SEC;
	}
#endif

#if 0
	if( nsys == DDS1A_LTU ) {  // 2012. 11. 20.. trig & clock split....
		if( port == 4 ) {  /* clock */
			tPort = port / 2;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1A[2] )==WR_ERROR ) errorCnt++;

			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1A[0] )==WR_ERROR ) errorCnt++;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1A[1] )==WR_ERROR ) errorCnt++;
			
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1B[0] )==WR_ERROR ) errorCnt++;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1B[1] )==WR_ERROR ) errorCnt++;

			pcfg.dbT1[0] += DDS1A_LTU_TAIL_V2_100SEC;// should be placed in this position.. last..
		}
		if( port == 6 ) {  /* clock B4 Probe*/
			tPort = port / 2;
			if( caput_DDS1_with_Arg_V2_LTU( nsys, 0,  pcfg, pWnd->pvDDS1A[3] )==WR_ERROR ) errorCnt++;

			pcfg.dbT1[0] += DDS1A_LTU_TAIL_V2_100SEC;
		}
		
	}
#endif
	
//////////////////////////////////////////////////


	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].R2_Enable[port].cid, QString( "%1").arg( (int)pcfg.OnOff ) );
	pWnd->pvCLTU[nsys].R2_Enable[port].status_flag |= PV_STATUS_TOUCH;
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].R2_Enable[port].name, (int)pcfg.OnOff, ca_message( iCAStatus) );
		errorCnt++;
	}

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].R2_Mode[port].cid, QString( "%1").arg( (int)pcfg.Mode) );
	pWnd->pvCLTU[nsys].R2_Mode[port].status_flag |= PV_STATUS_TOUCH;
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].R2_Mode[port].name, (int)pcfg.Mode, ca_message( iCAStatus) );
		errorCnt++;
	}

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].R2_ActiveLow[port].cid, QString( "%1").arg( pcfg.nTrig[0] ) );
	pWnd->pvCLTU[nsys].R2_ActiveLow[port].status_flag |= PV_STATUS_TOUCH;
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].R2_ActiveLow[port].name, pcfg.nTrig[0], ca_message( iCAStatus) );
		errorCnt++;
	}
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].POL[port].name, pcfg.nTrig[0], ca_message( iCAStatus) );


	for ( int i=0; i<R2_SECT_CNT; i++) 
	{
		if( errorCnt > 0 ) return errorCnt;
		
		iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].R2_T0[port][i].cid, pcfg.dbT0[i] );
		pWnd->pvCLTU[nsys].R2_T0[port][i].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL ) {
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].R2_T0[port][i].name, pcfg.dbT0[i], ca_message( iCAStatus) );
			errorCnt++;
		}

		iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].R2_T1[port][i].cid, pcfg.dbT1[i] );
		pWnd->pvCLTU[nsys].R2_T1[port][i].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	{
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].R2_T1[port][i].name, pcfg.dbT1[i], ca_message( iCAStatus) );
			errorCnt++;
		}

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].R2_CLOCK[port][i].cid, QString( "%1").arg( pcfg.nClock[i] ) );
		pWnd->pvCLTU[nsys].R2_CLOCK[port][i].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	{
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].R2_CLOCK[port][i].name, pcfg.nClock[i] , ca_message( iCAStatus) );
			errorCnt++;
		}
	}


	return errorCnt;


}

int CltuTimingSet::caput_CLTU_with_Arg(int nsys, int port,  ST_CLTU_config pcfg)
{
	MainWindow *pWnd = (MainWindow*)pm;

	int iCAStatus, errorCnt;

//	double dTrueT1; // 2009. 10. 27

	errorCnt = 0;

	if( port == PORT_NUM_MULTI ) 
	{
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].mTRIG.cid, QString( "%1").arg( pcfg.nTrig[0] ) );
		pWnd->pvCLTU[nsys].mTRIG.status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	
			errorCnt++;
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].mTRIG.name, pcfg.nTrig[0], ca_message( iCAStatus) );

		for(int i=0; i<3; i++) 
		{
			if( errorCnt > 0) break;
			iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[nsys].mCLOCK[i].cid, QString( "%1").arg( pcfg.nClock[i] ) );
			pWnd->pvCLTU[nsys].mCLOCK[i].status_flag |= PV_STATUS_TOUCH;
			if( iCAStatus  != ECA_NORMAL )	
				errorCnt++;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%dHz),  %s\n", pWnd->pvCLTU[nsys].mCLOCK[i].name, pcfg.nClock[i], ca_message( iCAStatus) );
			
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].mT0[i].cid, pcfg.dbT0[i] );
			pWnd->pvCLTU[nsys].mT0[i].status_flag |= PV_STATUS_TOUCH;
			if( iCAStatus  != ECA_NORMAL )	
				errorCnt++;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].mT0[i].name, pcfg.dbT0[i], ca_message( iCAStatus) );
				
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].mT1[i].cid, pcfg.dbT1[i] );
			pWnd->pvCLTU[nsys].mT1[i].status_flag |= PV_STATUS_TOUCH;
			if( iCAStatus  != ECA_NORMAL )	
				errorCnt++;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].mT1[i].name, pcfg.dbT1[i], ca_message( iCAStatus) );
//			usleep(100000);
		}

	} 
	
	else 
	{
//		dTrueT1 = pcfg.dbT1[0];
#if 0
		if( (nsys == DDS1A_LTU) && (port != 4)) {
			if( caput_DDS1_with_Arg_LTU( nsys, port,  pcfg, pWnd->pvDDS1A[port] )==WR_ERROR )
			{
				errorCnt++;
			}
		}
		else if ( (nsys == DDS1B_LTU) && (port==0 || port==1) ) {
			if( caput_DDS1_with_Arg_LTU( nsys, port,  pcfg, pWnd->pvDDS1B[port] )==WR_ERROR )
			{
				errorCnt++;
			}
		}
#endif
		if( (nsys == HALPHA_LTU) && (port == ONLY_TARGET_M6802_PORT ) )	/* 2009. 10. 30 */ 
		{
/*	HALPHA must not to be affected by TSS Clock setting. 2009. 10. 29
			iCAStatus=ca_Put_QStr(  pWnd->pvHALPHA.sampleRate.cid, pWnd->tw_machine->item(targetRow, 7)->text() ); // unit: KHz
			if( iCAStatus  != ECA_NORMAL )	
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvHALPHA.sampleRate.name, pcfg.nClock[0]/1000, ca_message( iCAStatus) );

			pcfg.nClock[0] = 100000; //  not use external clock, DDS#2 use only internal clock
*/
			pcfg.nTrig[0] = ACTIVE_HIGH_VAL;
			iCAStatus=ca_Put_Double(  pWnd->pvHALPHA.setT0.cid,  pcfg.dbT0[0] ); // Trigger
			if( iCAStatus  != ECA_NORMAL )	
				errorCnt++;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvHALPHA.setT0.name, pcfg.dbT0[0], ca_message( iCAStatus) );

			iCAStatus=ca_Put_Double(  pWnd->pvHALPHA.setT1.cid,  pcfg.dbT1[0]  ); // not Period time just T1
			if( iCAStatus  != ECA_NORMAL )	
				errorCnt++;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvHALPHA.setT1.name, pcfg.dbT1[0], ca_message( iCAStatus) );
			
			pcfg.dbT1[0] += HALPHA_DAQ_TIME_TAIL;
		}
/* 2012. 9. 6 request from TGLee */
#if 0
		else if( (nsys == ECEHR_LTU) && (port == ONLY_TARGET_M6802_PORT ) )	/* 2010. 08. 09 woong*/ 
		{
			pcfg.nTrig[0] = ACTIVE_HIGH_VAL;
			iCAStatus=ca_Put_Double(  pWnd->pvECEHR.setT0.cid,  pcfg.dbT0[0] );
			if( iCAStatus  != ECA_NORMAL )	 errorCnt++;

			iCAStatus=ca_Put_Double(  pWnd->pvECEHR.setT1.cid,  pcfg.dbT1[0]  );
			if( iCAStatus  != ECA_NORMAL )	 errorCnt++;
			
			pcfg.dbT1[0] += HALPHA_DAQ_TIME_TAIL;
		}
#endif

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].CLOCK[port].cid, QString( "%1").arg( pcfg.nClock[0] ) );
		pWnd->pvCLTU[nsys].CLOCK[port].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	
			errorCnt++;
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%dHz),  %s\n", pWnd->pvCLTU[nsys].CLOCK[port].name, pcfg.nClock[0], ca_message( iCAStatus) );

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[nsys].POL[port].cid, QString( "%1").arg( pcfg.nTrig[0] ) );
		pWnd->pvCLTU[nsys].POL[port].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	
			errorCnt++;
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[nsys].POL[port].name, pcfg.nTrig[0], ca_message( iCAStatus) );


		iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].T0[port].cid, pcfg.dbT0[0] );
		pWnd->pvCLTU[nsys].T0[port].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	
			errorCnt++;
//		printf("\"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].T0[port].name, pcfg.dbT0[0], ca_message( iCAStatus) );


		iCAStatus = ca_Put_Double( pWnd->pvCLTU[nsys].T1[port].cid, pcfg.dbT1[0] );	// this value is tail attached.. 2009. 08. 24
		pWnd->pvCLTU[nsys].T1[port].status_flag |= PV_STATUS_TOUCH;
		if( iCAStatus  != ECA_NORMAL )	
			errorCnt++;
//		printf("\"%s\" (%f),  %s\n", pWnd->pvCLTU[nsys].T1[port].name, pcfg.dbT1[0], ca_message( iCAStatus) );


	}/* end of single port setup */

	return errorCnt;
	

}

int CltuTimingSet::caput_DDS1_with_Arg_LTU(int nsys, int port,  ST_CLTU_config &pcfg, ST_DDS_PV &pvDDS1)
{
//	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	iCAStatus=ca_Put_QStr(	pvDDS1.sampleRate.cid, QString( "%1").arg( pcfg.nClock[0] ) ); // unit: Hz
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%dHz),  %s\n", pvDDS1[port].sampleRate.name, pcfg.nClock[0], ca_message( iCAStatus) );

	pcfg.nTrig[0] = ACTIVE_HIGH_VAL;
	
	iCAStatus = ca_Put_Double( pvDDS1.setT0.cid, pcfg.dbT0[0] );
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),	%s\n", pvDDS1[port].setT0.name, pcfg.dbT0[0], ca_message( iCAStatus) );

	iCAStatus = ca_Put_Double( pvDDS1.setT1.cid, pcfg.dbT1[0] );
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;
//				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),	%s\n", pvDDS1[port].setT1.name, pcfg.dbT1[0], ca_message( iCAStatus) );

	pcfg.dbT1[0] += pg_getTimeTail( pcfg.nClock[0] );

	return WR_OK;
}

int CltuTimingSet::caput_DDS1_with_Arg_V2_LTU(int nsys, int sect,  ST_CLTU_config &pcfg, ST_DDS_PV &pvDDS1)
{
//	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	iCAStatus=ca_Put_QStr(	pvDDS1.sampleRate.cid, QString( "%1").arg( pcfg.nClock[sect] ) ); // unit: Hz
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;

	pcfg.nTrig[sect] = ACTIVE_HIGH_VAL;
	
	iCAStatus = ca_Put_Double( pvDDS1.setT0.cid, pcfg.dbT0[sect] );
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;

	iCAStatus = ca_Put_Double( pvDDS1.setT1.cid, pcfg.dbT1[sect] );
	if( iCAStatus  != ECA_NORMAL )	
		return WR_ERROR;

	return WR_OK;
}

	
int CltuTimingSet::caput_assign_CLTU_with_input_row(QTableWidget *pTableWidget, int targetRow)
{
	MainWindow *pWnd = (MainWindow*)pm;

//	int iCAStatus;
	bool isNew;
	QString qstr_sysName;
	int port; /* 0, 1, 2, 3, 4 */
	int sysId;
//	int nVal;
	ST_CLTU_config  pcfg;
//	double dTrueT1; // 2009. 10. 27



	qstr_sysName = pTableWidget->item(targetRow, COL_ID_SYS)->text();
	sysId = pWnd->get_system_ID( qstr_sysName.toAscii().constData() );	
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr_sysName.toAscii().constData() );
		return WR_ERROR;
	}
	isNew = pWnd->Is_New_R2_id(sysId);
	
	port = pTableWidget->item(targetRow, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
	if (port < 0 ) return WR_ERROR;
//	multi = (port == 4) ? 1: 0; 
//	pcfg.nMode = (port == 4) ? 1: 0; // 0: single, 1: multi 

	if( isNew ) {
//		pcfg.OnOff = ( pTableWidget->item(targetRow, COL_ID2_ONOFF)->checkState() == Qt::Checked ) ? 1 : 0;
		if( pTableWidget->item(targetRow, COL_ID2_ONOFF)->checkState() == Qt::Checked ) {
//			printf(" NEW enable contidion: checked\n");
			pcfg.OnOff  = 1;
		}
		else if ( pTableWidget->item(targetRow, COL_ID2_ONOFF)->checkState() == Qt::Unchecked ) {			
//			printf(" NEW enable contidion: unchecked\n");
			pcfg.OnOff = 0;
		}

		pcfg.Mode = (strcmp(pTableWidget->item(targetRow, COL_ID2_MODE)->text().toAscii().constData(), "T" ) == 0) ? 1 : 0;
		pcfg.nTrig[0] = (strcmp(pTableWidget->item(targetRow, COL_ID2_POL)->text().toAscii().constData(), "R" ) == 0) ? 0 : 1;
		
		pcfg.dbT0[0] = pTableWidget->item(targetRow, COL_ID2_1_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[0] = pTableWidget->item(targetRow, COL_ID2_1_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[0] = (int)(pTableWidget->item(targetRow, COL_ID2_1_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[0] == pcfg.dbT1[0] ) { pcfg.dbT0[0] = pcfg.dbT1[0] = 0.0; }

		pcfg.dbT0[1] = pTableWidget->item(targetRow, COL_ID2_2_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[1] = pTableWidget->item(targetRow, COL_ID2_2_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[1] = (int)(pTableWidget->item(targetRow, COL_ID2_2_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[1] == pcfg.dbT1[1] ) { pcfg.dbT0[1] = pcfg.dbT1[1] = 0.0; }

	
		pcfg.dbT0[2] = pTableWidget->item(targetRow, COL_ID2_3_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[2] = pTableWidget->item(targetRow, COL_ID2_3_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[2] = (int)(pTableWidget->item(targetRow, COL_ID2_3_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[2] == pcfg.dbT1[2] ) { pcfg.dbT0[2] = pcfg.dbT1[2] = 0.0; }

		
		pcfg.dbT0[3] = pTableWidget->item(targetRow, COL_ID2_4_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[3] = pTableWidget->item(targetRow, COL_ID2_4_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[3] = (int)(pTableWidget->item(targetRow, COL_ID2_4_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[3] == pcfg.dbT1[3] ) { pcfg.dbT0[3] = pcfg.dbT1[3] = 0.0; }

		pcfg.dbT0[4] = pTableWidget->item(targetRow, COL_ID2_5_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[4] = pTableWidget->item(targetRow, COL_ID2_5_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[4] = (int)(pTableWidget->item(targetRow, COL_ID2_5_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[4] == pcfg.dbT1[4] ) { pcfg.dbT0[4] = pcfg.dbT1[4] = 0.0; }

		pcfg.dbT0[5] = pTableWidget->item(targetRow, COL_ID2_6_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[5] = pTableWidget->item(targetRow, COL_ID2_6_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[5] = (int)(pTableWidget->item(targetRow, COL_ID2_6_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[5] == pcfg.dbT1[5] ) { pcfg.dbT0[5] = pcfg.dbT1[5] = 0.0; }

		pcfg.dbT0[6] = pTableWidget->item(targetRow, COL_ID2_7_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[6] = pTableWidget->item(targetRow, COL_ID2_7_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[6] = (int)(pTableWidget->item(targetRow, COL_ID2_7_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[6] == pcfg.dbT1[6] ) { pcfg.dbT0[6] = pcfg.dbT1[6] = 0.0; }

		pcfg.dbT0[7] = pTableWidget->item(targetRow, COL_ID2_8_T0)->text().toDouble() + nBlip_time;
		pcfg.dbT1[7] = pTableWidget->item(targetRow, COL_ID2_8_T1)->text().toDouble() + nBlip_time;
		pcfg.nClock[7] = (int)(pTableWidget->item(targetRow, COL_ID2_8_CLK)->text().toFloat() * 10000.+0.5)/10;
		if( pcfg.dbT0[7] == pcfg.dbT1[7] ) { pcfg.dbT0[7] = pcfg.dbT1[7] = 0.0; }

		if( caput_CLTU_with_Arg_R2(sysId, port, pcfg ) == 0 ) 
			pWnd->printStatus(LEVEL_NONE,"\"%s\" CH#%d assign OK!\n", pWnd->str_sysName[sysId], port+1 );
		else {
			pWnd->printStatus(LEVEL_ERROR,"\"%s\" CH#%d assign ERROR!\n", pWnd->str_sysName[sysId], port+1 );
			return WR_ERROR;
		}

	} 
	else {

		pcfg.nTrig[0] = (strcmp(pTableWidget->item(targetRow, COL_ID_POL)->text().toAscii().constData(), "R" ) == 0) ? 0 : 1;
		if( port == PORT_NUM_MULTI ) 
		{
			pcfg.dbT0[0] = pTableWidget->item(targetRow, COL_ID_1_T0)->text().toDouble() + nBlip_time;
			pcfg.dbT1[0] = pTableWidget->item(targetRow, COL_ID_1_T1)->text().toDouble() + nBlip_time;
			pcfg.nClock[0] = (int)(pTableWidget->item(targetRow, COL_ID_1_CLK)->text().toFloat() * 10000.+0.5)/10;

			pcfg.dbT0[1] = pTableWidget->item(targetRow, COL_ID_2_T0)->text().toDouble() + nBlip_time;
			pcfg.dbT1[1] = pTableWidget->item(targetRow, COL_ID_2_T1)->text().toDouble() + nBlip_time;
			pcfg.nClock[1] = (int)(pTableWidget->item(targetRow, COL_ID_2_CLK)->text().toFloat() * 10000.+0.5)/10;
		
			pcfg.dbT0[2] = pTableWidget->item(targetRow, COL_ID_3_T0)->text().toDouble() + nBlip_time;
			pcfg.dbT1[2] = pTableWidget->item(targetRow, COL_ID_3_T1)->text().toDouble() + nBlip_time;
			pcfg.nClock[2] = (int)(pTableWidget->item(targetRow, COL_ID_3_CLK)->text().toFloat() * 10000.+0.5)/10;

		}
		else {
			pcfg.dbT0[0] = pTableWidget->item(targetRow, COL_ID_1_T0)->text().toDouble() + nBlip_time;
			pcfg.dbT1[0] = pTableWidget->item(targetRow, COL_ID_1_T1)->text().toDouble() + nBlip_time;
			pcfg.nClock[0] = (int)(pTableWidget->item(targetRow, COL_ID_1_CLK)->text().toFloat() * 10000. + 0.5)/10;
		}// single trigger port  0,1,2,3  

		if( caput_CLTU_with_Arg(sysId, port, pcfg ) == 0 ) 
			pWnd->printStatus(LEVEL_NONE,"\"%s\" CH#%d assign OK!\n", pWnd->str_sysName[sysId], port+1 );
		else {
			pWnd->printStatus(LEVEL_ERROR,"\"%s\" CH#%d assign ERROR!\n", pWnd->str_sysName[sysId], port+1 );
			return WR_ERROR;
		}
	
	}

	return WR_OK;
}

int CltuTimingSet::caput_setup_CLTU_with_input_row(QTableWidget *pTableWidget, int targetRow)
{
	MainWindow *pWnd = (MainWindow*)pm;

	int iCAStatus;
	QString qstr_sysName;
	int port;
	int sysId;
	static int putVal = 1;
	char buf[8];

	port = pTableWidget->item(targetRow, COL_ID_CH)->text().toInt() -1 ; /* 0, 1, 2, 3, 4 */
	qstr_sysName = pTableWidget->item(targetRow, COL_ID_SYS)->text();
	sysId = pWnd->get_system_ID( qstr_sysName.toAscii().constData() );
//	printf("curr sys: %s\n", qstr_sysName.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to setup!\n", qstr_sysName.toAscii().constData() );
		return WR_ERROR;
	}

	sprintf(buf, "%d", putVal );

	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[sysId].SETUP_Pt[port].cid, buf );
	pWnd->pvCLTU[sysId].SETUP_Pt[port].status_flag |= PV_STATUS_TOUCH;
	if( iCAStatus  != ECA_NORMAL )
	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysId].SETUP_Pt[port].name, 1, ca_message( iCAStatus) );
		return WR_ERROR;
	}
	else {
		pWnd->printStatus(LEVEL_NONE,"\"%s\" setup OK!\n", pWnd->pvCLTU[sysId].SETUP_Pt[port].name );
		putVal = putVal ? 0: 1;
	}

	return WR_OK;

}

/*
void CltuTimingSet::pg_setup_CLTU_with_all_list()
{
	int row;
	MainWindow *pWnd = (MainWindow*)pm;
	
	nPVblockMode = USER_RESET_ALL_SYS;
	row = pWnd->tw_machine->rowCount();
	if( row < 1 ) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: There is no target board!\n" );
		return;
	}
	
	for(int i=0; i<row; i++) {		
		caput_assign_CLTU_with_input_row(i );
		usleep(100000);
	}
	nPVblockMode = USER_NONE;
}
*/

void CltuTimingSet::pg_set_block_pv( int mode)
{
	nPVblockMode = mode;
}

void CltuTimingSet::pg_set_release_pv()
{
	nPVblockMode = USER_NONE;
}




