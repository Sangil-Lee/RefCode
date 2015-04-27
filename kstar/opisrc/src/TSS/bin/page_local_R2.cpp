#include <QtGui>

#include "page_local_R2.h"
#include "mainWindow.h"



LocalTestR2::LocalTestR2()
{
	pm = NULL;
}

LocalTestR2::~LocalTestR2()
{

}

void LocalTestR2::destroyPageTestcal_R2()
{
//	printf("destroy page_testcal\n");
	Init_OPI_widget();
}

void LocalTestR2::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
	MainWindow *pWnd = (MainWindow*)pm;
	QStringList qstrList;

	pWnd->cb_test_system_R2->clear();

	for(int i=0; i<MAX_LTU_CARD; i++) {
		if( pWnd->Is_New_R2_id(i) )
			qstrList.append( tr("%1").arg(pWnd->str_sysName[i]) );
	}
	pWnd->cb_test_system_R2->insertItems(0, qstrList);

	
	
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID_SYS, 60); // system
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID_DESC, 5); // description
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID_CH, 30); // port
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_ONOFF, 30); // onoff
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_MODE, 30); // mode: Clock, Trigger
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_POL, 30); // trig level
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_1_T0, 65); // sec1 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_1_T1, 65); // sec1 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_1_CLK, 40); // sec1 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_2_T0, 65); // sec2 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_2_T1, 65); // sec2 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_2_CLK, 40); // sec2 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_3_T0, 65); // sec3 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_3_T1, 65); // sec3 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_3_CLK, 40); // sec3 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_4_T0, 65); // sec4 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_4_T1, 65); // sec4 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_4_CLK, 40); // sec4 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_5_T0, 65); // sec5 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_5_T1, 65); // sec5 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_5_CLK, 40); // sec5 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_6_T0, 65); // sec6 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_6_T1, 65); // sec6 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_6_CLK, 40); // sec6 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_7_T0, 65); // sec7 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_7_T1, 65); // sec7 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_7_CLK, 40); // sec7 clock
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_8_T0, 65); // sec8 start time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_8_T1, 65); // sec8 stop time
	pWnd->tw_ltu2_local->horizontalHeader()->resizeSection(COL_ID2_8_CLK, 40); // sec8 clock


	pWnd->btn_R2_setMode->setText(STR_R2_FREE_RUN);


	Init_OPI_widget();
	

}

void LocalTestR2::Init_OPI_widget()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int row, iCAStatus, sysID, port;

	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );


	
	pWnd->cb_test_system_R2->setEnabled(true);
	
	pWnd->btn_R2_insert_item->setEnabled(false);
	pWnd->btn_R2_open->setEnabled(false);
	pWnd->btn_R2_save->setEnabled(false);
	pWnd->tw_ltu2_local->setEnabled(false);
	pWnd->btn_test_set_R2->setEnabled(false);
	pWnd->btn_test_shotStart_R2->setEnabled(false);
	pWnd->btn_test_shotStop_R2->setEnabled(false);


	if( pWnd->btn_R2_setMode->isChecked() ) 
	{
/* don't need it. 2013.08.05. End signal integrated in Sart ....		
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT_END.cid, QString( "1") );
		if( iCAStatus != ECA_NORMAL )
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].SHOT_END.name, 1, ca_message( iCAStatus) );	
*/
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SET_FREE_RUN.cid, QString( "0") );
		if( iCAStatus != ECA_NORMAL )
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name, 1, ca_message( iCAStatus) );	
		else
			pWnd->printStatus(LEVEL_NONE,"%s  Disabled!\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name);

		pWnd->btn_R2_setMode->setChecked(false);
		pWnd->btn_R2_setMode->setText(STR_R2_FREE_RUN);
	}
	

	row = pWnd->tw_ltu2_local->rowCount();
	if( row > 0 ) {
		
		nPVblockMode = USER_REMOVE_ONE_LINE;
		for(int i=0; i<row; i++) 
		{
			port = pWnd->tw_ltu2_local->item(0, COL_ID_CH)->text().toInt() - 1;
//			printf("sysID is: %d\n", sysID );
			pWnd->tw_ltu2_local->removeRow ( 0 );
//			caput_removeGivenRowItem(sysID, port); keep final setting. 2013. 3. 28
		}
		nPVblockMode = USER_NONE;
	}

	pWnd->check_calib_admin->setCheckState(Qt::Unchecked);
//	emit stateChanged_check_calib_admin(Qt::Unchecked);
	
}

void LocalTestR2::clicked_btn_R2_open()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QString fileName = QFileDialog::getOpenFileName(pWnd);
	
	FILE *fp;
	int row, nval;
	char bufLineScan[512];
	char temp1[16], temp2[16];

	if (!fileName.isEmpty()) {

		if( (fp = fopen(fileName.toAscii().constData(), "r") ) == NULL )
		{	
			pWnd->printStatus(LEVEL_ERROR,"Can not open \"%s\" file for read\n", fileName.toAscii().constData() );
			return;
		} else {
			while(1) 
			{
				if( fgets(bufLineScan, 512, fp) == NULL ) break;
				
				if( bufLineScan[0] == '#') ;
				else 
				{
//					pg_InsertRow_from_lineBuf(bufLineScan);
				}
			}
		}
	
		pWnd->statusBar()->showMessage(tr("File Opened"), 2000);
	}
}

void LocalTestR2::clicked_btn_R2_save()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QString fileName = QFileDialog::getSaveFileName(pWnd);

	if (!fileName.isEmpty()) {
//		pg_timeset.fileSave(fileName);
		pWnd->statusBar()->showMessage(tr("Save current timing set"), 2000);
	}
}

void LocalTestR2::clicked_btn_R2_setMode()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;
	int sysID, port, row;
	passwordDlg passdlg;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );


	if( pWnd->btn_R2_setMode->isChecked() ) {
		if( sysID == TSS_CTU && pWnd->passwd_master_check ) 
		{
			if (passdlg.exec() == QDialog::Accepted) {
				if( strcmp(STR_PASSWD_CALIB, (passdlg.lineEdit_password->text()).toAscii().constData() ) != 0 )
				{
					pWnd->btn_R2_setMode->setChecked(false);
					QMessageBox::information(0, "Kstar TSS", tr("wrong pass word!") ); return;
				}
				pWnd->passwd_master_check = 0;
				pWnd->passwd_timeset_check = 0;
			} 
			else{
				pWnd->btn_R2_setMode->setChecked(false);
				return ;
				}
		}
		

		pWnd->cb_test_system_R2->setEnabled(false);

		pWnd->tw_ltu2_local->setEnabled(true);
		pWnd->btn_test_set_R2->setEnabled(true);
		pWnd->btn_test_shotStart_R2->setEnabled(true);
		pWnd->btn_test_shotStop_R2->setEnabled(true);
		pWnd->btn_R2_insert_item->setEnabled(true);
		pWnd->btn_R2_open->setEnabled(true);
		pWnd->btn_R2_save->setEnabled(true);
		
		pWnd->btn_R2_setMode->setText(STR_R2_SYNC_RUN);

//		if( sysID != TSS_CTU ) 
//		{
			iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SET_FREE_RUN.cid, QString( "1") );
			if( iCAStatus != ECA_NORMAL )
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name, 1, ca_message( iCAStatus) );	
			else
				pWnd->printStatus(LEVEL_NONE,"%s  Enabled!\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name);
//		}
	}
	else 
	{
		
		pWnd->cb_test_system_R2->setEnabled(true);

		pWnd->tw_ltu2_local->setEnabled(false);
		pWnd->btn_test_set_R2->setEnabled(false);
		pWnd->btn_test_shotStart_R2->setEnabled(false);
		pWnd->btn_test_shotStop_R2->setEnabled(false);
		pWnd->btn_R2_insert_item->setEnabled(false);
		pWnd->btn_R2_open->setEnabled(false);
		pWnd->btn_R2_save->setEnabled(false);

		pWnd->btn_R2_setMode->setText(STR_R2_FREE_RUN);

		row = pWnd->tw_ltu2_local->rowCount();
		if( row > 0 ) 
		{
			nPVblockMode = USER_REMOVE_ONE_LINE;
			for(int i=0; i<row; i++) 
			{
				port = pWnd->tw_ltu2_local->item(0, COL_ID_CH)->text().toInt() - 1;
				pWnd->tw_ltu2_local->removeRow ( 0 );
//				caput_removeGivenRowItem(sysID, port);
			}
			nPVblockMode = USER_NONE;
		}
/* don't need it. 2013.08.05. End signal integrated in Sart ....
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT_END.cid, QString( "1") );
		if( iCAStatus != ECA_NORMAL )
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SHOT_END.name, 1, ca_message( iCAStatus) );	
*/
	
//		if( sysID != TSS_CTU ) 
//		{
			iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SET_FREE_RUN.cid, QString( "0") );
			if( iCAStatus != ECA_NORMAL )
				pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name, 1, ca_message( iCAStatus) );	
			else
				pWnd->printStatus(LEVEL_NONE,"%s  Disabled!\n", pWnd->pvCLTU[sysID].SET_FREE_RUN.name);
//		}
	}

}

void LocalTestR2::clicked_btn_showinfo_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOW_STATUS.cid, QString( "1") );
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].SHOW_STATUS.name, 0, ca_message( iCAStatus) );	
	
}
void LocalTestR2::clicked_btn_showTime_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOW_TIME.cid, QString( "1") );
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].SHOW_TIME.name, 0, ca_message( iCAStatus) );	
	
}
void LocalTestR2::clicked_btn_showTLK_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOW_TLK.cid, QString( "1") );
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].SHOW_TLK.name, 0, ca_message( iCAStatus) );	
	
}
void LocalTestR2::clicked_btn_showLog_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].LOG_GSHOT.cid, QString( "1") );
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].LOG_GSHOT.name, 0, ca_message( iCAStatus) );	
	
}




void LocalTestR2::clicked_btn_test_setup_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int row;
	int prev_blipTime;

	prev_blipTime = pWnd->pg_timeset.pg_get_bliptime();
	pWnd->pg_timeset.pg_set_bliptime(0);

	row = pWnd->tw_ltu2_local->rowCount();
	if( row > 0) {
		for( int i=0; i<row; i++) {
			pWnd->pg_timeset.caput_assign_CLTU_with_input_row(pWnd->tw_ltu2_local,  i );
			usleep(10000);
			pWnd->pg_timeset.caput_setup_CLTU_with_input_row( pWnd->tw_ltu2_local, i );
		}
		
	}
	else
		pWnd->printStatus(LEVEL_NONE,"There is no target port!\n");

	pWnd->pg_timeset.pg_set_bliptime(prev_blipTime);
		
}

void LocalTestR2::clicked_btn_test_shotStart_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	static int nR2Start = 1;
//	static int nR2End = 1;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
/*
	if( sysID == TSS_CTU ) 
	{
//		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].IRIG_B.cid, QString( "0") );
//		if( iCAStatus != ECA_NORMAL )
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].IRIG_B.name, 0, ca_message( iCAStatus) );	
//		usleep(5000);

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT_END.cid, QString( "%1").arg( nR2End ) );
		nR2End = nR2End ? 0:1;
		if( iCAStatus != ECA_NORMAL )
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SHOT_END.name, 1, ca_message( iCAStatus) );	
		usleep(10000);
	}
*/

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT.cid, QString( "%1").arg( nR2Start ) );
	nR2Start = nR2Start ? 0:1;
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SHOT.name, 1, ca_message( iCAStatus) );	
	else
		pWnd->printStatus(LEVEL_NONE,"%s  ... OK!\n", pWnd->pvCLTU[sysID].SHOT.name);
/*
	if( sysID == TSS_CTU )
	{
		usleep(10000);
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].IRIG_B.cid, QString( "1") );
		if( iCAStatus != ECA_NORMAL )
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].IRIG_B.name, 0, ca_message( iCAStatus) );	
	}
*/
}

void LocalTestR2::clicked_btn_test_shotStop_R2()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT_END.cid, QString( "1") );
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),	%s\n", pWnd->pvCLTU[sysID].SHOT_END.name, 0, ca_message( iCAStatus) );	

}

void LocalTestR2::clicked_btn_R2_insert_item()
{
	MainWindow *pWnd = (MainWindow*)pm;
	
	QTableWidget *pTableWidget;
	
	QTableWidgetItem *item[MAX_COL2_NUM];
	int row;

	for(int i=0; i<MAX_COL2_NUM; i++) {
		item[i] = new QTableWidgetItem();
		item[i]->setTextAlignment(Qt::AlignCenter);
		item[i]->setText("");
	}

//	pTableWidget = pWnd->getTableWidget_from_sysname((char*)pWnd->cb_test_system_R2->currentText().toAscii().constData());
	pTableWidget = pWnd->tw_ltu2_local;
	

	row = pTableWidget->rowCount();
	pTableWidget->setRowCount(row + 1);

	QFont font;
	font.setBold(true);

//	item[0]->setText(pWnd->cb_timing_hwsw->currentText());
	item[COL_ID_SYS]->setText(pWnd->cb_test_system_R2->currentText());
	item[COL_ID_DESC]->setText("-"); item[COL_ID_DESC]->setTextAlignment(Qt::AlignLeft);
	item[COL_ID_CH]->setText("1" );

//		item[COL_ID2_ONOFF]->setText(strOnOff);
		item[COL_ID2_ONOFF]->setFlags(item[COL_ID2_ONOFF]->flags() & ~Qt::ItemIsEditable);
		item[COL_ID2_ONOFF]->setCheckState(Qt::Checked);
		
		item[COL_ID2_MODE]->setText("T"); /* C, T */
		item[COL_ID2_POL]->setText("R");
		item[COL_ID2_1_T0]->setText("0" );
		item[COL_ID2_1_T1]->setText("0" );
		item[COL_ID2_1_CLK]->setText("1" );
		item[COL_ID2_1_T0]->setFont(font);
		item[COL_ID2_1_T1]->setFont(font);
		item[COL_ID2_1_CLK]->setFont(font);
		
		item[COL_ID2_2_T0]->setText("0" );
		item[COL_ID2_2_T1]->setText("0" );
		item[COL_ID2_2_CLK]->setText("1" );
		item[COL_ID2_3_T0]->setText("0" );
		item[COL_ID2_3_T1]->setText("0" );
		item[COL_ID2_3_CLK]->setText("1" );
		item[COL_ID2_3_T0]->setFont(font);
		item[COL_ID2_3_T1]->setFont(font);
		item[COL_ID2_3_CLK]->setFont(font);
		
		item[COL_ID2_4_T0]->setText("0" );
		item[COL_ID2_4_T1]->setText("0" );
		item[COL_ID2_4_CLK]->setText("1" );
		item[COL_ID2_5_T0]->setText("0" );
		item[COL_ID2_5_T1]->setText("0" );
		item[COL_ID2_5_CLK]->setText("1" );
		item[COL_ID2_5_T0]->setFont(font);
		item[COL_ID2_5_T1]->setFont(font);
		item[COL_ID2_5_CLK]->setFont(font);
		
		item[COL_ID2_6_T0]->setText("0" );
		item[COL_ID2_6_T1]->setText("0" );
		item[COL_ID2_6_CLK]->setText("1" );
		item[COL_ID2_7_T0]->setText("0" );
		item[COL_ID2_7_T1]->setText("0" );
		item[COL_ID2_7_CLK]->setText("1" );
		item[COL_ID2_7_T0]->setFont(font);
		item[COL_ID2_7_T1]->setFont(font);
		item[COL_ID2_7_CLK]->setFont(font);
		
		item[COL_ID2_8_T0]->setText("0" );
		item[COL_ID2_8_T1]->setText("0" );
		item[COL_ID2_8_CLK]->setText("1" );
		
	nPVblockMode = USER_INSERT_ONE_LINE;

	for(int i=0; i<MAX_COL2_NUM; i++) {
		pTableWidget->setItem(row, i, item[i]);
	}
	pTableWidget->sortItems( COL_ID_CH, Qt::AscendingOrder);
	pTableWidget->sortItems( COL_ID_SYS, Qt::AscendingOrder);

	nPVblockMode = USER_NONE;
	
//	pWnd->pg_timeset.pg_set_color_line(pTableWidget);




}




void LocalTestR2::event_removeCurrentRowItem()
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget = pWnd->tw_ltu2_local;

	int iCAStatus;
	int curSys, port;
	int row = pTableWidget->currentRow ();

	if( row < 0 ) return;

	// find delete system ID;
//	curSys =  pWnd->cb_test_system_R2->findText( pTableWidget->item(row, COL_ID_SYS)->text() , Qt::MatchExactly);	
	nPVblockMode = USER_REMOVE_ONE_LINE;

	curSys =  pWnd->get_system_ID( pTableWidget->item(row, COL_ID_SYS)->text().toAscii().constData() );	
	port = pTableWidget->item(row, COL_ID_CH)->text().toInt() - 1;

//	printf("curSYS is: %d\n", curSys );

	pTableWidget->removeRow ( row );

	caput_removeGivenRowItem(curSys, port);

//	pWnd->pg_timeset.pg_set_color_line(pTableWidget);

	iCAStatus = ca_Put_StrVal(  pWnd->pvCLTU[curSys].SETUP_Pt[port].cid, "1" );
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[curSys].SETUP_Pt[port].name, 1, ca_message( iCAStatus) );

	nPVblockMode = USER_NONE;
	
}

void LocalTestR2::caput_removeGivenRowItem(int curSys,int port)
{
	int iCAStatus;
	MainWindow *pWnd = (MainWindow*)pm;

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_Enable[port].cid, QString( "0") );
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_Enable[port].name, 0, ca_message( iCAStatus) );


	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_ActiveLow[port].cid, QString( "0") );
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_ActiveLow[port].name, 0, ca_message( iCAStatus) );

	usleep(10000);

	for ( int i=0; i<R2_SECT_CNT; i++) 
	{
		iCAStatus = ca_Put_Double( pWnd->pvCLTU[curSys].R2_T0[port][i].cid, 0.0 );
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_T0[port][i].name, 0, ca_message( iCAStatus) );

		iCAStatus = ca_Put_Double( pWnd->pvCLTU[curSys].R2_T1[port][i].cid, 0.0);
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_T1[port][i].name, 0, ca_message( iCAStatus) );

		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[curSys].R2_CLOCK[port][i].cid, QString( "1") );
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[curSys].R2_CLOCK[port][i].name, 0, ca_message( iCAStatus) );

		usleep(10000);
	}

}

void LocalTestR2::itemChanged_tw_local_R2( QTableWidgetItem *curr)
{
	MainWindow *pWnd = (MainWindow*)pm;
	QTableWidget *pTableWidget;

	int row, column;
	int sysId;
	int nval;
	QString qstr;

	if( nPVblockMode != USER_NONE )
		return;

	pTableWidget = pWnd->tw_ltu2_local;

	row = pTableWidget->currentRow();
	column = pTableWidget->currentColumn();

	sysId =  pWnd->get_system_ID( pTableWidget->item(row, COL_ID_SYS)->text().toAscii().constData() );	
	if( sysId < 0 ) { QMessageBox::information(0, "Kstar CLTU", tr("system name error!") ); return; }

	qstr = curr->text();


	if( column == COL_ID_CH ) 
	{
		nval = qstr.toInt();
		if( (nval  < 1) || (nval > 8 )) {
			QMessageBox::information(0, "Kstar CLTU", tr("CH select error! (1-8)") );
			return;
		}
	}
	else if(  column == COL_ID2_POL )  // set trigger level R/F
	{
		if(  strcmp(qstr.toAscii().constData(), "R") == 0 ) 
			;
		else if(strcmp(qstr.toAscii().constData(), "F") == 0)
			;
		else if( strcmp(qstr.toAscii().constData(), "r") == 0 )
			curr->setText("R");
		else if (strcmp(qstr.toAscii().constData(), "f") == 0)
			curr->setText("F");
		else 
		{
			QMessageBox::information(0, "Kstar CLTU", tr("Polarity set error! (R/F)") );
			return;
		}
			
	} 
	else if( column == COL_ID2_MODE) 
	{
		if(  strcmp(qstr.toAscii().constData(), "C") == 0 ) 
			;
		else if(strcmp(qstr.toAscii().constData(), "T") == 0)
			;
		else if( strcmp(qstr.toAscii().constData(), "c") == 0 )
			curr->setText("C");

		else if (strcmp(qstr.toAscii().constData(), "t") == 0)
			curr->setText("T");
		
		else {
			QMessageBox::information(0, "Kstar CLTU", tr("Mode set error! (C/T)") );
			return;
		}
	}

}


void LocalTestR2::stateChanged_check_calib_admin(int state)
{
	MainWindow *pWnd = (MainWindow*)pm;
//	int iCAStatus;

	if( state ) 
	{
		if( pWnd->passwd_master_check ) 
		{
			passwordDlg passdlg;
			if ( passdlg.exec() == QDialog::Accepted)
			{
				if( strcmp(STR_PASSWD_CALIB, (passdlg.lineEdit_password->text()).toAscii().constData() ) != 0 )
				{
					pWnd->check_calib_admin->setCheckState(Qt::Unchecked);
					QMessageBox::information(0, "Kstar TSS", tr("wrong pass word!") );
					return;
				}				
				
			} 
			else {
				pWnd->check_calib_admin->setCheckState(Qt::Unchecked);
				return ;
				}
			
			pWnd->passwd_master_check = 0;
			pWnd->passwd_timeset_check = 0;
		}

		pWnd->gb_TSS_calibration->setEnabled(true);
//		pWnd->caBoButton_irigb->setEnabled(true);
		
	}
	else 
	{
/*		iCAStatus = ca_Put_QStr(  pWnd->pvTSS_CalMode.cid, QString( "0") );
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvTSS_CalMode.name, state, ca_message( iCAStatus) );
		else
			pWnd->printStatus(LEVEL_NONE,"\"%s\" (%d)",  pWnd->pvTSS_CalMode.name, state );
*/
		pWnd->check_TSS_cal_mode->setCheckState(Qt::Unchecked);
		pWnd->gb_TSS_calibration->setEnabled(false);		
	}
}

void LocalTestR2::stateChanged_check_TSS_cal_mode(int state)
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	if( state ) 
	{
		iCAStatus = ca_Put_QStr(  pWnd->pvTSS_CalMode.cid, QString( "1") );
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvTSS_CalMode.name, state, ca_message( iCAStatus) );
		else
			pWnd->printStatus(LEVEL_NONE,"\"%s\" (%d)",  pWnd->pvTSS_CalMode.name, state);
	}
	else 
	{
		iCAStatus = ca_Put_QStr(  pWnd->pvTSS_CalMode.cid, QString( "0") );
		if( iCAStatus  != ECA_NORMAL )	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvTSS_CalMode.name, state, ca_message( iCAStatus) );
		else
			pWnd->printStatus(LEVEL_NONE,"\"%s\" (%d)",  pWnd->pvTSS_CalMode.name, state);
	}

}

void LocalTestR2::clicked_btn_put_LTU_calib_value()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	int nval, sysID;


	nval =   pWnd->le_LTU_calib_val->text().toInt();

//	sysID = pWnd->get_system_ID( pWnd->lb_target_LTU->text().toAscii().constData() );
	QString qstr = pWnd->cb_test_system_R2->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );
	

	
	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SET_CAL_VALUE.cid, QString( "%1").arg( nval) );
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n",  pWnd->pvCLTU[sysID].SET_CAL_VALUE.name, nval, ca_message( iCAStatus) );
	else
		pWnd->printStatus(LEVEL_NONE,"\"%s\" (%d)",  pWnd->pvCLTU[sysID].SET_CAL_VALUE.name, nval );

}





void LocalTestR2::cb_thread_calibration(ST_QUEUE_STATUS qnode)
{
	MainWindow *pWnd = (MainWindow*)pm;
	
	char sysName[8];
//	int iCAStatus;


//	if( nPVblockMode == USER_RESET_ALL_SYS )
//		return;


//	printf("sys: %d, val: %f: mode:%d, type:%d, Port:%d\n", qnode.pEventArg->system, qnode.dval, qnode.pEventArg->mode, qnode.pEventArg->type, qnode.pEventArg->port );
	strcpy(sysName, pWnd->str_sysName[ qnode.pEventArg->system ] );
//	printf("sys:%s\n", sysName );
	if( !strcmp(sysName, pWnd->str_sysName[ TSS_CTU]) ) 
	{
		pWnd->lb_CTU_COMP_TICK->setText( QString( "%1").arg( qnode.dval  ) );
		return;
	}


	if( qnode.pEventArg->type != TYPE_R2_GET_COMP_NS ) 
	{
		pWnd->printStatus(LEVEL_ERROR,"TYPE_R2_GET_COMP_NS error!" );
		return;	
	}


	switch( qnode.pEventArg->type )
	{
		case TYPE_R2_GET_COMP_NS:
			pWnd->lb_LTU_COMP_TICK->setText( QString( "%1").arg( qnode.dval  ) );
			pWnd->lb_target_LTU->setText( sysName );
//			pWnd->lb_recommend_value->setText( QString( "%1").arg( qnode.dval/2 + 31  ) );
			pWnd->lb_recommend_value->setText( QString( "%1").arg( qnode.dval/2 ) );
			break;
			
		default: 
			pWnd->printStatus(LEVEL_ERROR,"ERROR: not applied type: %d\n", qnode.pEventArg->type );
			break;
	}

//	pWnd->update();  // doesn't work.
//	pTableWidget->repaint();


}





