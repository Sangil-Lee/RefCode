#include <QtGui>

#include "fileControl.h"

#include "mainWindow.h"

#include "systemState.h"


//#include "ui_DDS_qtmainwindow.h"


//#include "dds1PVs.h"

//#include "epicsFunc.h"






FileCTRL::FileCTRL()
{
	proot = NULL;

}

FileCTRL::~FileCTRL()
{

}

void FileCTRL::InitWindow(void *parent)
{
	proot = (MainWindow*)parent;

}


void FileCTRL::saveACQ196ConfigFile(QTableWidget *pwidget, int num)
{
	int nOnOff;
	char fileName[128];
	char strTag[32];

	FILE *fp; //, *fpTemp;
	QTableWidgetItem *item;

	MainWindow *pWnd = (MainWindow *)proot;

	 if( pWnd->g_opMode == SYS_MODE_INIT ) return;
	 

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


	if( (fp = fopen(fileName, "w") ) == NULL ){	
		QMessageBox::information(0, "DDS1 setup", tr("Can't open DDS1 Board config file") );
		return;
	}


	switch(num) {
	case B1: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_1_T0->text().toAscii().constData()	); break;
	case B2: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_2_T0->text().toAscii().constData()	); break;
	case B3: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_3_T0->text().toAscii().constData()	); break;
	case B4: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_4_T0->text().toAscii().constData()	); break;
	case B5: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_5_T0->text().toAscii().constData()	); break;
	case B6: fprintf(fp, "trigger %s\n", pWnd->Edit_ACQ196_6_T0->text().toAscii().constData()	); break;
	}

	switch(num) {
	case B1: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_1_T1->text().toAscii().constData()	); break;
	case B2: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_2_T1->text().toAscii().constData()	); break;
	case B3: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_3_T1->text().toAscii().constData()	); break;
	case B4: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_4_T1->text().toAscii().constData()	); break;
	case B5: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_5_T1->text().toAscii().constData()	); break;
	case B6: fprintf(fp, "period %s\n", pWnd->Edit_ACQ196_6_T1->text().toAscii().constData()	); break;
	}

	switch(num) {
	case B1: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk->currentText()).toAscii().constData() ); break;
	case B2: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk_2->currentText()).toAscii().constData() ); break;
	case B3: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk_3->currentText()).toAscii().constData() ); break;
	case B4: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk_4->currentText()).toAscii().constData() ); break;
	case B5: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk_5->currentText()).toAscii().constData() ); break;
	case B6: fprintf(fp, "sampleClk %s\n", (pWnd->cb_dds1SampleClk_6->currentText()).toAscii().constData() ); break;
	}


	for( int i=0; i< 96; i++) {
		item = pwidget->item(i, 0);
		if( item->checkState() == Qt::Checked)
			nOnOff = 1;
		else nOnOff = 0;

		item = pwidget->item(i, 1);
		strcpy( strTag, item->text().toAscii().constData() );

		fprintf(fp, "%d\t %d\t %s\n", i, nOnOff, strTag);
	}

	fclose(fp);
		

}


#if 0
void FileCTRL::saveM6802ConfigFile(QTableWidget *pwidget, int num)
{
	int nOnOff;
	char fileName[128];
	char bufLineScan[512];
	char strTag[32];
	char strCmd[128];

	FILE *fp, *fpTemp;
	QTableWidgetItem *item;

	MainWindow *pWnd = (MainWindow *)proot;

	 if( pWnd->g_opMode == SYS_MODE_INIT ) return;
	 	
#if 1
	if ( num == 1 )
		strcpy(fileName, "/home/kstar/dds_config/.M6802-1.cfg");
	else if( num == 2)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-2.cfg");
	else if( num == 3)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-3.cfg");
	else if( num == 4)
		strcpy(fileName, "/home/kstar/dds_config/.M6802-4.cfg");
	else return;
#endif
//	strcpy(fileName, "/root/nfrc/config/test.cfg");

	if( (fp = fopen(fileName, "r") ) == NULL ){	
		QMessageBox::information(0, "DDS setup", tr("Can't make M6802 config file") );
		return;
	}
	if( (fpTemp = fopen("temp6802.cfg", "w") ) == NULL ){	
		QMessageBox::information(0, "DDS setup", tr("Can't make M6802 temp config file") );
		return;
	}

	if( fgets(bufLineScan, 512, fp) == NULL ) return;
	fprintf(fpTemp, "trigger %s\n", pWnd->Edit_M6802_trigger1->text().toAscii().constData()	);
//	printf("scan buf: %s\n", bufLineScan );

	if( fgets(bufLineScan, 512, fp) == NULL ) return;
	fprintf(fpTemp, "period %s\n", pWnd->Edit_M6802_period1->text().toAscii().constData()	);
//	printf("scan buf: %s\n", bufLineScan );
	

	if( fgets(bufLineScan, 512, fp) == NULL ) return;
//	fprintf(fpTemp, "sampleClk %s\n", (cb_dds2SampleClk->currentText()).toAscii().constData()	);
	fprintf(fpTemp, "sampleClk 1\n");

//	printf("scan buf: %s\n", bufLineScan );

	if( pWnd->g_opMode == SYS_MODE_REMOTE ) {
		if( fgets(bufLineScan, 512, fp) == NULL ) return;
		fprintf(fpTemp, "opmode REMOTE\n");
		for( int i=0; i< 32; i++) {
			item = pwidget->item(i, 0);
			if( item->checkState() == Qt::Checked)
				nOnOff = 1;
			else nOnOff = 0;

			item = pwidget->item(i, 1);
			strcpy( strTag, item->text().toAscii().constData() );

			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%d %d %s\n", i+1, nOnOff, strTag);
		}
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
	} 
	else if( pWnd->g_opMode == SYS_MODE_CALIB ) {
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
		if( fgets(bufLineScan, 512, fp) == NULL ) return;
		fprintf(fpTemp, "opmode CALIB\n");
		for( int i=0; i< 32; i++) {
			item = pwidget->item(i, 0);
			if( item->checkState() == Qt::Checked)
				nOnOff = 1;
			else nOnOff = 0;

			item = pwidget->item(i, 1);
			strcpy( strTag, item->text().toAscii().constData() );

			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%d %d %s\n", i+1, nOnOff, strTag);
		}
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
	} 
	else if( pWnd->g_opMode == SYS_MODE_LOCAL ) {
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
		for( int i=0; i< 33; i++) {
			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%s", bufLineScan);
		}
		if( fgets(bufLineScan, 512, fp) == NULL ) return;
		fprintf(fpTemp, "opmode TEST\n");
		for( int i=0; i< 32; i++) {
			item = pwidget->item(i, 0);
			if( item->checkState() == Qt::Checked)
				nOnOff = 1;
			else nOnOff = 0;

			item = pwidget->item(i, 1);
			strcpy( strTag, item->text().toAscii().constData() );

			if( fgets(bufLineScan, 512, fp) == NULL ) return;
			fprintf(fpTemp, "%d %d %s\n", i+1, nOnOff, strTag);
		}
	}
	else {
		fclose(fp);
		fclose(fpTemp);
		return;
	}
//	printf("out of loop\n");
	
	fclose(fp);
	fclose(fpTemp);

	sprintf(strCmd, "rm -f %s", fileName );
	system(strCmd);
	sprintf(strCmd, "mv temp6802.cfg %s", fileName );
	system(strCmd);
	
	
}

#endif



