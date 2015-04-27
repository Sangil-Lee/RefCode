#include <QtGui>

#include "page_manyTrig.h"
#include "mainWindow.h"


MainWindow *pMain_many;

void connectionCB_manyLTU(struct connection_handler_args args);


LTU_MANY_TRIG::LTU_MANY_TRIG()
{
	pm = NULL;
}

LTU_MANY_TRIG::~LTU_MANY_TRIG()
{

}

void LTU_MANY_TRIG::destroyPageManyTrig()
{
//	printf("destroy page_testcal\n");

}

void LTU_MANY_TRIG::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
	MainWindow *pWnd = (MainWindow*)pm;
	pMain_many = pWnd;
	QStringList qstrList;
	
	pWnd->cb_mTrig_system->clear();

	for(int i=0; i<MAX_LTU_CARD; i++) {
		if( pWnd->Is_New_R2_id(i) )
			qstrList.append( tr("%1").arg(pWnd->str_sysName[i]) );
	}
	pWnd->cb_mTrig_system->insertItems(0, qstrList);
		
	pWnd->tw_mTrig_pulse_list->horizontalHeader()->resizeSection(0, 80); // start time
	pWnd->tw_mTrig_pulse_list->horizontalHeader()->resizeSection(1, 80); // stop time

	Init_OPI_widget();

}

void LTU_MANY_TRIG::Init_OPI_widget()
{
	MainWindow *pWnd = (MainWindow*)pm;

	pWnd->cb_mTrig_system->setCurrentIndex(ER_LTU-1);
	
	pWnd->cb_mTrig_system->setEnabled(true);
	pWnd->cb_mTrig_target_port->setEnabled(true);
	pWnd->btn_mTrig_Connect->setEnabled(true);
	pWnd->btn_mTrig_Disconnect->setEnabled(false);
	pWnd->btn_mTrig_pv_setup->setEnabled(false);
	pWnd->btn_mTrig_start->setEnabled(false);
	pWnd->btn_mTrig_stop->setEnabled(false);

}

void connectionCB_manyLTU(struct connection_handler_args args)
{
	long 	op = args.op;

	for(int sys=0; sys<MAX_LTU_CARD; sys++) {
		for( int j=0; j<R2_PORT_CNT; j++) 
		{
			for( int k=R2_SECT_CNT; k<R2_SECT_CNT_EXP; k++) 
			{
//				if( args.chid == pMain_many->pvCLTU[sys].R2_CLOCK[j][k].cid ) {
//					pMain_many->updatePV(op, pMain_many->pvCLTU[sys].R2_CLOCK[j][k]); 
//					return; 
//				}
				if( args.chid == pMain_many->pvCLTU[sys].R2_T0[j][k].cid ) {
					pMain_many->updatePV(op, pMain_many->pvCLTU[sys].R2_T0[j][k], 1); /* print connection state with last argument 1 */
					return; 
				}
				if( args.chid == pMain_many->pvCLTU[sys].R2_T1[j][k].cid ) { 
					pMain_many->updatePV(op, pMain_many->pvCLTU[sys].R2_T1[j][k]); 
					return; 
				}
			}
		}

	}
}


void LTU_MANY_TRIG::event_removeCurrentRowItem()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget = pWnd->tw_mTrig_pulse_list;

	int row = pTableWidget->currentRow ();

	if( row < 0 ) return;

	pTableWidget->removeRow ( row );
}

void LTU_MANY_TRIG::clicked_btn_mTrig_Connect()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysId, port;
	
	QString qstr = pWnd->cb_mTrig_system->currentText();
	sysId = pWnd->get_system_ID( qstr.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr.toAscii().constData() );
		return ;
	}

	port = pWnd->cb_mTrig_target_port->currentIndex();


	for( int k=R2_SECT_CNT; k<R2_SECT_CNT_EXP; k++) 
	{
//		ca_Connect_STPV( pWnd->pvCLTU[sysId].R2_CLOCK[port][k], (VOIDFUNCPTR)connectionCB_manyLTU);
		ca_Connect_STPV( pWnd->pvCLTU[sysId].R2_T0[port][k], (VOIDFUNCPTR)connectionCB_manyLTU);
		ca_Connect_STPV( pWnd->pvCLTU[sysId].R2_T1[port][k], (VOIDFUNCPTR)connectionCB_manyLTU);
	}

	pWnd->cb_mTrig_system->setEnabled(false);
	pWnd->cb_mTrig_target_port->setEnabled(false);
	pWnd->btn_mTrig_Connect->setEnabled(false);
	pWnd->btn_mTrig_Disconnect->setEnabled(true);
	pWnd->btn_mTrig_pv_setup->setEnabled(true);
	pWnd->btn_mTrig_start->setEnabled(true);
	pWnd->btn_mTrig_stop->setEnabled(true);
	

}
void LTU_MANY_TRIG::clicked_btn_mTrig_Disconnect()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	int sysId, port;
	QString qstr = pWnd->cb_mTrig_system->currentText();
	sysId = pWnd->get_system_ID( qstr.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr.toAscii().constData() );
		return ;
	}
	port = pWnd->cb_mTrig_target_port->currentIndex();

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysId].SET_FREE_RUN.cid, "0" );
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].SET_FREE_RUN.name, 0, ca_message( iCAStatus) );
	}
	

	for( int k=R2_SECT_CNT; k<R2_SECT_CNT_EXP; k++) 
	{
//		ca_Disconnect_STPV( pWnd->pvCLTU[sysId].R2_CLOCK[port][k]);
		ca_Disconnect_STPV( pWnd->pvCLTU[sysId].R2_T0[port][k]);
		ca_Disconnect_STPV( pWnd->pvCLTU[sysId].R2_T1[port][k]);
	}
	
	pWnd->cb_mTrig_system->setEnabled(true);
	pWnd->cb_mTrig_target_port->setEnabled(true);
	pWnd->btn_mTrig_Connect->setEnabled(true);
	pWnd->btn_mTrig_Disconnect->setEnabled(false);
	pWnd->btn_mTrig_pv_setup->setEnabled(false);
	pWnd->btn_mTrig_start->setEnabled(false);
	pWnd->btn_mTrig_stop->setEnabled(false);
	

}
void LTU_MANY_TRIG::clicked_btn_mTrig_list_clear()
{
	MainWindow *pWnd = (MainWindow*)pm;

	int row = pWnd->tw_mTrig_pulse_list->rowCount();
	if( row > 0 ) 
	{
		for(int i=0; i<row; i++) 
		{
			pWnd->tw_mTrig_pulse_list->removeRow ( 0 );
		}
	}
}

void LTU_MANY_TRIG::clicked_btn_mTrig_group_setup()
{
	MainWindow *pWnd = (MainWindow*)pm;

	int rowCnt, prev_row;
	
	double startTime;
	float   pulseWidth;
	float pulseInterval;
	int  pulseCnt;

//	startTime = pWnd->le_mTrig_start_time->text().toDouble() + pWnd->pg_timeset.pg_get_bliptime();
	startTime = pWnd->le_mTrig_start_time->text().toDouble();
	pulseWidth = pWnd->le_mTrig_pulse_width->text().toFloat() * 0.001;
	pulseInterval = pWnd->le_mTrig_pulse_intvl->text().toFloat();
	pulseCnt = pWnd->le_mTrig_pulse_cnt->text().toInt();


//	printf(" start time: %f, width: %f, interval: %f, cnt: %d\n", startTime, pulseWidth, pulseInterval, pulseCnt);

	QTableWidget *pTableWidget = pWnd->tw_mTrig_pulse_list;

	prev_row = pTableWidget->rowCount();
//	printf("prev_row: %d\n", prev_row);
	for(int i=0; i<pulseCnt; i++) {
		QTableWidgetItem *item[2];	
		for(int j=0; j<2; j++) {
			item[j] = new QTableWidgetItem();
			item[j]->setTextAlignment(Qt::AlignCenter);
			item[j]->setText("");
		}

		rowCnt = pTableWidget->rowCount();
		if( rowCnt == 0)
			pTableWidget->setRowCount(1);
		else 
			pTableWidget->setRowCount(rowCnt + 1);
		
		item[0]->setText(QString("%1").arg(startTime + (pulseInterval * i) ) );
		item[1]->setText(QString("%1").arg(startTime + (pulseInterval * i) + pulseWidth ) );
		
		pTableWidget->setItem(rowCnt, 0, item[0]);
		pTableWidget->setItem(rowCnt, 1, item[1]);

	}


}


void LTU_MANY_TRIG::clicked_btn_mTrig_pv_setup()
{
	MainWindow *pWnd = (MainWindow*)pm;

	int iCAStatus, errorCnt;
	int port; /* 0, 1, 2, 3, 4 */
	int sysId;
	int row;
	double t0, t1;
	
	errorCnt = 0;
	QTableWidget *pTableWidget = pWnd->tw_mTrig_pulse_list;

	QString qstr = pWnd->cb_mTrig_system->currentText();
	sysId = pWnd->get_system_ID( qstr.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr.toAscii().constData() );
		return ;
	}

	port = pWnd->cb_mTrig_target_port->currentIndex();
	
#if 1	
	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[sysId].R2_Enable[port].cid, "1" );
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].R2_Enable[port].name, 1, ca_message( iCAStatus) );
		errorCnt++;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysId].R2_Mode[port].cid, "1" ); /* 1: trigger */
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].R2_Mode[port].name, 1, ca_message( iCAStatus) );
		errorCnt++;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysId].R2_ActiveLow[port].cid, "0"); /* 0: Active High */
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].R2_ActiveLow[port].name, 0, ca_message( iCAStatus) );
		errorCnt++;
	}
/*	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysId].SET_FREE_RUN.cid, "1" ); // 1: synchronized 
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].SET_FREE_RUN.name, 1, ca_message( iCAStatus) );
		errorCnt++;
	}
*/
#endif
	
	row = pTableWidget->rowCount();
	if( row > R2_SECT_CNT_EXP )
	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: Many pulse trigger limit is 50! \"%d\" \n", row );
		return;
	}

	if( row > 0) {
		for( int i=0; i<row; i++) 
		{
			t0 =  pTableWidget->item(i, 0)->text().toDouble() + pWnd->pg_timeset.pg_get_bliptime();
			t1 =  pTableWidget->item(i, 1)->text().toDouble() + pWnd->pg_timeset.pg_get_bliptime();

//			printf("sysID: %d, port: %d, sec:%d,  t0: %f, t1: %f\n", sysId, port, i, t0, t1);
#if 1
			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysId].R2_T0[port][i].cid, t0);
			if( iCAStatus  != ECA_NORMAL ) {
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysId].R2_T0[port][i].name, t0, ca_message( iCAStatus) );
				errorCnt++;
			}

			iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysId].R2_T1[port][i].cid, t1);
			if( iCAStatus  != ECA_NORMAL )	{
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysId].R2_T1[port][i].name, t1, ca_message( iCAStatus) );
				errorCnt++;
			}
			
			usleep(10000);
#endif
			
		}
		iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[sysId].SETUP_Pt[port].cid, "1" );
		if( iCAStatus  != ECA_NORMAL )
		{
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysId].SETUP_Pt[port].name, 1, ca_message( iCAStatus) );
			return ;
		}
		else {
			pWnd->printStatus(LEVEL_NONE,"\"%s\" setup OK!\n", pWnd->pvCLTU[sysId].SETUP_Pt[port].name );
		}
			
	}
	else
		pWnd->printStatus(LEVEL_NONE,"There is no target port!\n");

}

void LTU_MANY_TRIG::clicked_btn_mTrig_start()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	int sysId;

	QString qstr = pWnd->cb_mTrig_system->currentText();
	sysId = pWnd->get_system_ID( qstr.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr.toAscii().constData() );
		return ;
	}
	
	
	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[sysId].SHOT.cid, "1" );
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].SHOT.name, 1, ca_message( iCAStatus) );
	}

}

void LTU_MANY_TRIG::clicked_btn_mTrig_stop()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	int sysId;

	QString qstr = pWnd->cb_mTrig_system->currentText();
	sysId = pWnd->get_system_ID( qstr.toAscii().constData() );
	if ( sysId < 0) {
		pWnd->printStatus(LEVEL_ERROR,"Can't find system(%s) to assign!\n", qstr.toAscii().constData() );
		return ;
	}
	
	
	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[sysId].SHOT_END.cid, "1" );
	if( iCAStatus  != ECA_NORMAL )	{
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysId].SHOT_END.name, 1, ca_message( iCAStatus) );
	}

}




