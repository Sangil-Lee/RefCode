#include <QtGui>

#include "callbackFunc.h"

#include "mainWindow.h"

#include "systemState.h"


//#include "dds1PVs.h"

//#include "epicsFunc.h"






CBfunc::CBfunc()
{
	proot = NULL;

	cntEvidMain=0;

}

CBfunc::~CBfunc()
{

}

void CBfunc::InitWindow(void *parent)
{
	proot = (MainWindow*)parent;

//	printf("initWindow(): mainwindow: %p\n", proot);

}



void callbackLTU(struct event_handler_args args )
{
#if 0
	MainWindow *pWnd = (MainWindow *)args.usr;

	int index;
	QVariant qval;
//	float time_tail;
	double dVal;
	char strVal[64];

	if( args.status != ECA_NORMAL){
		printf("callbackLTU(event_handler_args.status is not OK\n" );
		return;
	}

	if( pWnd->g_opMode != SYS_MODE_REMOTE ) return;
	
		
	union db_access_val * pUData = ( db_access_val *) args.dbr;
	
	switch( args.type){

		case DBR_TIME_DOUBLE:
			qval = QVariant( *( &( pUData->tdblval.value)) );
//			sprintf(strval, "%.8f", *( &( pUData->tdblval.value))  );
			dVal = *( &( pUData->tdblval.value));
			break;
		default:
			pWnd->printStatus("ERROR, Wrong input type\n");
			return;
	}

	if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.CLOCK[B1] ) {
		index = pWnd->cb_dds1SampleClk->findData( qval, Qt::MatchExactly);
		if( index != -1 ) {
			pWnd->cb_dds1SampleClk->setCurrentIndex(index );
#if USE_SYNC_SLAVE_TO_MASTER
			pWnd->cb_dds1SampleClk_2->setCurrentIndex(index );
			pWnd->cb_dds1SampleClk_3->setCurrentIndex(index );
			pWnd->cb_dds1SampleClk_4->setCurrentIndex(index );
#endif
		}
		else printf("Wrong input DDS1ALTU.CLOCK[0] : %f\n", qval.toDouble() );
	} else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T0[B1] ) {
		cvt_Double_to_String( dVal, strVal );
		pWnd->Edit_ACQ196_1_T0->setText( strVal );
#if USE_SYNC_SLAVE_TO_MASTER
		pWnd->Edit_ACQ196_2_T0->setText( strVal );
		pWnd->Edit_ACQ196_3_T0->setText( strVal );
		pWnd->Edit_ACQ196_4_T0->setText( strVal );
#endif
	} 
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T1[B1] ) {
		pWnd->pg_dds1.touch_T1_PV[B1] = 1;  /* because we use time tail for DDS. so should check in DDS not LTU */
//		pWnd->printStatus("got T1 PV changed");
	}
	
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.CLOCK[B2] ) {
		index = pWnd->cb_dds1SampleClk_2->findData( qval, Qt::MatchExactly);
		if( index != -1 ) pWnd->cb_dds1SampleClk_2->setCurrentIndex(index );
		else printf("Wrong input DDS1ALTU.CLOCK[1] : %f\n", qval.toDouble() );
	} else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T0[B2] ) {
		cvt_Double_to_String( dVal, strVal );
		pWnd->Edit_ACQ196_2_T0->setText( strVal );
	}
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T1[B2] ) {
		pWnd->pg_dds1.touch_T1_PV[B2] = 1;
//		time_tail = pWnd->getTimeTail(CURR_ACQ196, B2);
//		pWnd->Edit_ACQ196_2_T1->setText( QString("%1").arg( qval.toDouble() - time_tail ) );
	}
	
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.CLOCK[B3] ) {
		index = pWnd->cb_dds1SampleClk_3->findData( qval, Qt::MatchExactly);
		if( index != -1 ) pWnd->cb_dds1SampleClk_3->setCurrentIndex(index );
		else printf("Wrong input DDS1ALTU.CLOCK[2] : %f\n", qval.toDouble() );
	} else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T0[B3] ) {
		cvt_Double_to_String( dVal, strVal );
		pWnd->Edit_ACQ196_3_T0->setText( strVal );
	}
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T1[B3] ) {
		pWnd->pg_dds1.touch_T1_PV[B3] = 1;
//		time_tail = pWnd->getTimeTail(CURR_ACQ196, B3);
//		pWnd->Edit_ACQ196_3_T1->setText( QString("%1").arg( qval.toDouble() - time_tail ) );
	}
	
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.CLOCK[B4] ) {
		index = pWnd->cb_dds1SampleClk_4->findData( qval, Qt::MatchExactly);
		if( index != -1 ) pWnd->cb_dds1SampleClk_4->setCurrentIndex(index );
		else printf("Wrong input DDS1ALTU.CLOCK[3] : %f\n", qval.toDouble() );
	} else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T0[B4] ) {
		cvt_Double_to_String( dVal, strVal );
		pWnd->Edit_ACQ196_4_T0->setText( strVal );
	}
	else if( args.chid == pWnd->pg_dds1.pvDDS1ALTU.T1[B4] ) {
		pWnd->pg_dds1.touch_T1_PV[B4] = 1;
//		time_tail = pWnd->getTimeTail(CURR_ACQ196, B4);
//		pWnd->Edit_ACQ196_4_T1->setText( QString("%1").arg( qval.toDouble() - time_tail ) );
	}
#endif
}

#if 0
void callbackLTU_DDS2(struct event_handler_args args )
{
	MainWindow *pWnd = (MainWindow *)args.usr;

	int index;
	QVariant qval;
//	float time_tail;
	char strVal[64];
//	QString qstrT0, qstrT1;//, qstrClock;
//	int nClock;
	double dVal;//, dval2;
//	char strBuf[64];



	if( args.status != ECA_NORMAL){
		printf("callbackLTU(event_handler_args.status is not OK\n" );
		return;
	}

	if( pWnd->g_opMode != SYS_MODE_REMOTE ) return;
	
		
	union db_access_val * pUData = ( db_access_val *) args.dbr;
	
	switch( args.type){

		case DBR_TIME_DOUBLE:
			qval = QVariant( *( &( pUData->tdblval.value)) );
//			sprintf(strval, "%.8f", *( &( pUData->tdblval.value))  );
			dVal = *( &( pUData->tdblval.value));
			break;
		default:
			pWnd->printStatus("ERROR, Wrong input type\n");
			return;
	}

	
#if 0
	if( args.chid == pWnd->pg_dds2.chid_DDS2_sampleRate ) {
//		qstrClock = pWnd->pg_dds2.getSampleRateDDS2();
		nClock = pWnd->cb_dds2SampleClk->findText( qval.toString(), Qt::MatchExactly);
		pWnd->cb_dds2SampleClk->setCurrentIndex(nClock );
	}
	else if( args.chid == pWnd->pg_dds2.pvCLTU.T0[0] ) {
//		qstrT0 = ca_Get_QStr( pg_dds2.pvCLTU.T0[port] );
		cvt_Double_to_String(dval, strBuf);
		pWnd->Edit_M6802_trigger1->setText( strBuf );

	}
	else if( args.chid == pWnd->pg_dds2.pvCLTU.T1[0] ) {
//		qstrT1 = ca_Get_QStr( pg_dds2.pvCLTU.T1[port] );
		ca_Get_Double(pWnd->pg_dds2.pvCLTU.T0[0], dval);
		dval2 = *( &( pUData->tdblval.value)) - dval - DDS2_DAQ_TIME_TAIL ;
		cvt_Double_to_String( dval2, strBuf );
		pWnd->Edit_M6802_period1->setText( strBuf );
	}

#endif
	if( args.chid == pWnd->pg_dds2.chid_DDS2_sampleRate ) {
		index = pWnd->cb_dds2SampleClk->findData( qval, Qt::MatchExactly);
		if( index != -1 ) pWnd->cb_dds2SampleClk->setCurrentIndex(index );
		else printf("Wrong input chid_DDS2_sampleRate : %f\n", qval.toDouble() );
	} else if( args.chid == pWnd->pg_dds2.pvCLTU.T0[0]  ) {
//		pWnd->Edit_ACQ196_1_T0->setText(qval.toString());
		cvt_Double_to_String( dVal, strVal );
		pWnd->Edit_M6802_trigger1->setText( strVal );
	} 
	else if( args.chid == pWnd->pg_dds2.pvCLTU.T1[0]  ) {
		pWnd->pg_dds2.touch_T1_PV[0] = 1;
	}

}
#endif



//QMutex mutexCBfunc;

void callbackMonitor(struct event_handler_args args)
{
	MainWindow *pWnd = (MainWindow *)args.usr;


//	bool block = true;
	
//	block = mutexCBfunc.tryLock();
//	if( !block ) {
//		QMessageBox::information(0, "_callbackMonitor", "corrupt mutex lock!" );
//		return;
//	}
	chid mychid;
	double dval=0.0;

//	pWnd->chidEventCB = args.chid;
	mychid = args.chid;

//	if( args.count != ( long) ca_element_count( chID ) ){
//		QMessageBox::information(0, "callbackMonitor", tr("event_handler_args.count is not correct!") );
//		return;
//	}
	if( args.status != ECA_NORMAL){
		pWnd->printStatus("callbackMonitor(event_handler_args.status is not OK\n" );
		return;
	}
		
	union db_access_val * pUData = ( db_access_val *) args.dbr;
	
	switch( args.type){
//		case DBR_TIME_STRING:
//			pcltuMain->qstrEventCB = QString( "%1").arg( *( &( pUData->tstrval.value) ) );
//			break;
		case DBR_TIME_DOUBLE:
//			qstrEventCB = QString( "%1").arg( *( &( pUData->tdblval.value) ) );
//			printf("got double value, %lf\n",  *( &( pUData->tdblval.value)) ); 
//			pWnd->dbCallbackVal = *( &( pUData->tdblval.value));
			dval =  *( &( pUData->tdblval.value));
//			pWnd->cbFunc.patchMessage(&mychid , (unsigned int)(*( &( pUData->tdblval.value))) );

			break;
		default:
			break;
	}
//	printf("_callback: chid: %d, val: %d\n", mychid, (unsigned int)(*( &( pUData->tdblval.value))) );

//	QEvent * pQEvent = new QEvent( QEvent::User);
//	QCoreApplication::postEvent( pWnd, pQEvent);

//	QCoreApplication::postEvent( this, pQEvent);
	pWnd->patchMessage(&mychid , (unsigned int)dval);

//	pWnd->cbFunc.patchMessage(&mychid , (unsigned int)dval);

//	mutexCBfunc.unlock();
}



void CBfunc::startMonitoringPV(chid chID, int mode )
{
	int iCAStatus=0;
/*
	if( ca_read_access( chID) == 0 ){
		QMessageBox::information(0, "startMonitoringPV", tr( "There is not Read Access") );
		return;
	}
*/

	if (mode == 0 ) {
//		printf("startMonitoringPV: mainwindow: %p\n", (MainWindow *)proot);
		
		iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( chID), chID,
				callbackMonitor, (MainWindow *)proot, 0.0, 0.0, 0.0, &evidMain[cntEvidMain], DBE_VALUE);
	} else if( mode==1) {
		iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( chID), chID,
				callbackLTU, (MainWindow *)proot, 0.0, 0.0, 0.0, &evidMain[cntEvidMain], DBE_VALUE);
	}
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "startMonitoringPV",  tr("ca_add_masked_array_event() error!"));
		return;
	}
	cntEvidMain++;
/*	printf("cntEvidMain: %d\n", cntEvidMain ); */

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "startMonitoringPV", tr("ca_pend_io() error!"));
		return;
	}

}

#if 0
void CBfunc::startMonitoringPV_DDS2(chid chID, int mode )
{
	int iCAStatus=0;

	if (mode == 0 ) {
		iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( chID), chID,
				callbackMonitor, (MainWindow *)proot, 0.0, 0.0, 0.0, &evidMain[cntEvidMain], DBE_VALUE);
	} else if( mode==1) {
		iCAStatus = ca_add_masked_array_event( DBR_TIME_DOUBLE,
				ca_element_count( chID), chID,
				callbackLTU_DDS2, (MainWindow *)proot, 0.0, 0.0, 0.0, &evidMain[cntEvidMain], DBE_VALUE);
	}
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "startMonitoringPV",  tr("ca_add_masked_array_event() error!"));
		return;
	}
	cntEvidMain++;

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		QMessageBox::information(0, "startMonitoringPV", tr("ca_pend_io() error!"));
		return;
	}

}
#endif

void CBfunc::stopMonitoringPV()
{
	int iCAStatus;
	
/*	printf("stop monitoring,  cntEvidMain: %d\n", cntEvidMain ); */
	
	for(int i=0; i< cntEvidMain; i++) {
		iCAStatus = ca_clear_event( evidMain[i]);
		if( iCAStatus != ECA_NORMAL)
			QMessageBox::information(0, "stopMonitoringPV", tr("ca_clear_event() error!"));
		evidMain[i] = NULL;
		iCAStatus = ca_pend_io( 1.0);
		if( iCAStatus != ECA_NORMAL)
			QMessageBox::information(0, "stopMonitoringPV", tr("ca_pend_io() error!"));
	}
	cntEvidMain = 0;
}

#if 0
void CBfunc::patchMessage(chid *myid, unsigned int uintVal )
{
	ST_QUEUE_STATUS queueNode;
	MainWindow *pWnd = (MainWindow *)proot;

//	printf("patchMsg: chid: %d, val: %d\n", (unsigned int)(*myid), uintVal);
//	printf("patchMsg: DDS1_getState: %d\n", (unsigned int)pWnd->pg_dds1.DDS1_getState[B1] );


	if( *myid == pWnd->pg_dds1.DDS1_getState[B1] )
	{
		queueNode.status = uintVal;
		queueNode.id = B1;
		pWnd->putQMyNode( queueNode );
//		printf("DDS1_getState[B1]: 0x%x\n", queueNode.status );
	} else if( *myid == pWnd->pg_dds1.DDS1_getState[B2] ) {
		queueNode.status = uintVal;
		queueNode.id = B2;
		pWnd->putQMyNode( queueNode );
	} else if( *myid == pWnd->pg_dds1.DDS1_getState[B3] ) {
		queueNode.status = uintVal;
		queueNode.id = B3;
		pWnd->putQMyNode( queueNode );
	} else if( *myid == pWnd->pg_dds1.DDS1_getState[B4] ) {
		queueNode.status = uintVal;
		queueNode.id = B4;
		pWnd->putQMyNode( queueNode );
	}
	
	else if( *myid == pWnd->pg_dds2.chid_DDS2_getState ) {
//		pWnd->g_opStateDDS2 = uintVal;
		pWnd->printStatus("g_opStateDDS2: 0x%x\n", uintVal);
		pWnd->setDDS2StateFromPV(uintVal);
	}
	
	else if ( *myid == pWnd->pg_dds1.DDS1_BX_PARSING_CNT[B1] ) {
//			printf("DDS1_BX_PARSING_CNT[B1]: %d\n", uintVal);
		pWnd->label_dev1_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_PARSING_CNT[B2] ) {
		pWnd->label_dev2_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_PARSING_CNT[B3] ) {
		pWnd->label_dev3_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_PARSING_CNT[B4] ) {
		pWnd->label_dev4_cnt->setText(QString("%1").arg( uintVal) );
	} 
	else if ( *myid == pWnd->pg_dds1.DDS1_BX_MDS_SND_CNT[B1] ) {
//			printf("DDS1_BX_MDS_SND_CNT[B1]: %d\n", uintVal);
		pWnd->label_dev1_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_MDS_SND_CNT[B2] ) {
		pWnd->label_dev2_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_MDS_SND_CNT[B3] ) {
		pWnd->label_dev3_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_MDS_SND_CNT[B4] ) {
		pWnd->label_dev4_cnt->setText(QString("%1").arg( uintVal) );
	}
	else if ( *myid == pWnd->pg_dds1.DDS1_BX_DAQING_CNT[B1] ) {
		pWnd->label_dev1_cnt->setText(QString("%1").arg( uintVal ) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_DAQING_CNT[B2] ) {
		pWnd->label_dev2_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_DAQING_CNT[B3] ) {
		pWnd->label_dev3_cnt->setText(QString("%1").arg( uintVal) );
	} else if ( *myid == pWnd->pg_dds1.DDS1_BX_DAQING_CNT[B4] ) {
		pWnd->label_dev4_cnt->setText(QString("%1").arg( uintVal) );
	} 
	else {
		pWnd->printStatus("got undef chidEventCB: %d\n", (unsigned int)(*myid) );
	}
}
#endif





