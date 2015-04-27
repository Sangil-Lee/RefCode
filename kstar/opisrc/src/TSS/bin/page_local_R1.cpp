#include <QtGui>

#include "page_local_R1.h"
#include "mainWindow.h"


LocalTestR1::LocalTestR1()
{
	pm = NULL;
}

LocalTestR1::~LocalTestR1()
{

}

void LocalTestR1::destroyPageTestcal()
{
//	printf("destroy page_testcal\n");
}

void LocalTestR1::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
	MainWindow *pWnd = (MainWindow*)pm;
	QStringList qstrList;

	pWnd->cb_test_system->clear();

	for(int i=0; i<MAX_LTU_CARD; i++) {
//		qstrList.append( tr("%1").arg(pWnd->str_sysName[i]) );
		if( pWnd->Is_New_R2_id(i) == FALSE )
			qstrList.append( tr("%1").arg(pWnd->str_sysName[i]) );

	}
	pWnd->cb_test_system->insertItems(0, qstrList);


	Initialize();
}



void LocalTestR1::clicked_btn_test_set()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	ST_CLTU_config		cfg_CLTU;

	QString qstr = pWnd->cb_test_system->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );
	
	if ( sysID < 0) {
		return;
	}


	cfg_CLTU.dbT0[0] =   pWnd->le_test_pt1_start->text().toDouble();
	cfg_CLTU.dbT1[0] =   pWnd->le_test_pt1_stop->text().toDouble();
	cfg_CLTU.nClock[0] = (int)(pWnd->le_test_pt1_freq->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.nTrig[0] =  pWnd->cb_test_pt1_pol->currentIndex();
	
	cfg_CLTU.dbT0[1] =   pWnd->le_test_pt2_start->text().toDouble();
	cfg_CLTU.dbT1[1] =   pWnd->le_test_pt2_stop->text().toDouble();
//	cfg_CLTU.nClock[1] = pWnd->cb_test_pt2_freq->currentText().toInt() * 1000;
	cfg_CLTU.nClock[1] = (int)(pWnd->le_test_pt2_freq->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.nTrig[1] =  pWnd->cb_test_pt2_pol->currentIndex();
	
	cfg_CLTU.dbT0[2] =   pWnd->le_test_pt3_start->text().toDouble();
	cfg_CLTU.dbT1[2] =   pWnd->le_test_pt3_stop->text().toDouble();
//	cfg_CLTU.nClock[2] = pWnd->cb_test_pt3_freq->currentText().toInt() * 1000;
	cfg_CLTU.nClock[2] = (int)(pWnd->le_test_pt3_freq->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.nTrig[2] =  pWnd->cb_test_pt3_pol->currentIndex();
	
	cfg_CLTU.dbT0[3] =   pWnd->le_test_pt4_start->text().toDouble();
	cfg_CLTU.dbT1[3] =   pWnd->le_test_pt4_stop->text().toDouble();
//	cfg_CLTU.nClock[3] = pWnd->cb_test_pt4_freq->currentText().toInt() * 1000;
	cfg_CLTU.nClock[3] = (int)(pWnd->le_test_pt4_freq->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.nTrig[3] =  pWnd->cb_test_pt4_pol->currentIndex();
	
	cfg_CLTU.nmTrig =     pWnd->cb_test_pt5_pol1->currentIndex();
	cfg_CLTU.dbmT0[0] =   pWnd->le_test_pt5_start1->text().toDouble();
	cfg_CLTU.dbmT1[0] =   pWnd->le_test_pt5_stop1->text().toDouble();
	cfg_CLTU.nmClock[0] = (int)(pWnd->le_test_pt5_freq1->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.dbmT0[1] =   pWnd->le_test_pt5_start2->text().toDouble();
	cfg_CLTU.dbmT1[1] =   pWnd->le_test_pt5_stop2->text().toDouble();
	cfg_CLTU.nmClock[1] = (int)(pWnd->le_test_pt5_freq2->text().toFloat() * 10000.+0.5)/10;
	cfg_CLTU.dbmT0[2] =   pWnd->le_test_pt5_start3->text().toDouble();
	cfg_CLTU.dbmT1[2] =   pWnd->le_test_pt5_stop3->text().toDouble();
	cfg_CLTU.nmClock[2] = (int)(pWnd->le_test_pt5_freq3->text().toFloat() * 10000.+0.5)/10;
	
/*
	if( opi_debug) {
	for( int i=0; i< 4; i++) {
		pWnd->printStatus(LEVEL_NONE,"port1: t0:%lf, t1:%lf, frq:%d, pol.:%d", cfg_CLTU.dbT0[i], cfg_CLTU.dbT1[i], cfg_CLTU.nClock[i], cfg_CLTU.nTrig[i] );
	}
	for( int i=0; i< 3; i++) {
		pWnd->printStatus(LEVEL_NONE,"mport5: #%d -> t0:%lf, t1:%lf, frq:%d, pol.:%d",i, cfg_CLTU.dbmT0[i], cfg_CLTU.dbmT1[i], cfg_CLTU.nmClock[i], cfg_CLTU.nmTrig );
	}
	}
*/
	if( setup_all_port(sysID, cfg_CLTU) == WR_OK )
		pWnd->printStatus(LEVEL_NONE,"%s setup OK!\n", qstr.toAscii().constData()  );
}

void LocalTestR1::clicked_btn_test_shotStart()
{
	MainWindow *pWnd = (MainWindow*)pm;
	int sysID;
	int iCAStatus;
	static int nlocalStart = 1;
	
	QString qstr = pWnd->cb_test_system->currentText();
	sysID = pWnd->get_system_ID( qstr.toAscii().constData() );

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}

	iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT.cid, QString( "%1").arg( nlocalStart ) );
	nlocalStart = nlocalStart ? 0:1;
	if( iCAStatus != ECA_NORMAL )
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SHOT.name, 1, ca_message( iCAStatus) );	
	else
		pWnd->printStatus(LEVEL_NONE,"%s  ... OK!\n", pWnd->pvCLTU[sysID].SHOT.name);
}

void LocalTestR1::clicked_btn_cal_calStart()
{
	

}













void LocalTestR1::setup_single_port(int sysID,int port)
{
	MainWindow *pWnd = (MainWindow*)pm;
	int iCAStatus;

	if( sysID < 0 || sysID >= MAX_LTU_CARD ) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong system ID (%d)\n", sysID);
		return;
	}
	if( port < 0 || port > 4) {
		pWnd->printStatus(LEVEL_NONE,"got a wrong port ID (%d)\n", port);
		return;
	}
	iCAStatus=ca_Put_StrVal( pWnd->pvCLTU[sysID].SETUP_Pt[port].cid, "1" );	
	if( iCAStatus  != ECA_NORMAL )	
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SETUP_Pt[port].name, 1, ca_message( iCAStatus) );	
//		pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].SETUP_Pt[%d],  %s\n", sysID, port, ca_message( iCAStatus) );	
}

int LocalTestR1::setup_all_port(int sysID, ST_CLTU_config cfg_CLTU)
{
	MainWindow *pWnd = (MainWindow*)pm;
//	char strval[16];
	int iCAStatus;
	static int nlocalSetup = 1;


//	if(opi_debug) printf("setup system= %d\n", sysID);


	for(int i=0; i<4; i++) 
	{
		// this is temporary parameter, It send value to IOC
//		ca_Put_QStr(  pWnd->pvCLTU[sysID].T0[i], QString( "%1").arg( cfg_CLTU.dbT0[i] ) );
		iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysID].T0[i].cid,  cfg_CLTU.dbT0[i] );
		if( iCAStatus != ECA_NORMAL) {
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].T0[%d](%f),  %s\n", sysID, i, cfg_CLTU.dbT0[i], ca_message( iCAStatus) );	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysID].T0[i].name, cfg_CLTU.dbT0[i], ca_message( iCAStatus) );	
			return WR_ERROR;
		}
//		usleep(1000);
//		ca_Put_QStr(  pWnd->pvCLTU[sysID].T1[i], QString( "%1").arg( cfg_CLTU.dbT1[i] ) );
		iCAStatus = ca_Put_Double( pWnd->pvCLTU[sysID].T1[i].cid,  cfg_CLTU.dbT1[i] );
		if( iCAStatus != ECA_NORMAL) {
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].T1[%d](%f), %s\n", sysID, i, cfg_CLTU.dbT1[i], ca_message( iCAStatus) );	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysID].T1[i].name, cfg_CLTU.dbT1[i], ca_message( iCAStatus) );	
			return WR_ERROR;
		}

//		usleep(1000);
		iCAStatus = ca_Put_QStr(  pWnd->pvCLTU[sysID].CLOCK[i].cid, QString( "%1").arg( cfg_CLTU.nClock[i] ) );
		if( iCAStatus != ECA_NORMAL) {
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].CLOCK[%d](%d), %s\n", sysID, i, cfg_CLTU.nClock[i], ca_message( iCAStatus) );	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].CLOCK[i].name, cfg_CLTU.nClock[i], ca_message( iCAStatus) );
			return WR_ERROR;
		}

//		usleep(1000);
		iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].POL[i].cid, QString( "%1").arg( cfg_CLTU.nTrig[i] ) );
		if( iCAStatus != ECA_NORMAL) {
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].POL[%d](%d), %s\n", sysID, i, cfg_CLTU.nTrig[i], ca_message( iCAStatus) );	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].POL[i].name, cfg_CLTU.nTrig[i], ca_message( iCAStatus) );
			return WR_ERROR;
		}
//		usleep(1000);
		usleep(300000);
		// set real IOC port config for one to four......
//		iCAStatus=ca_Put_StrVal(  pWnd->pvCLTU[sysID].SETUP_Pt[i].cid, "1" );
		iCAStatus=ca_Put_QStr(	pWnd->pvCLTU[sysID].SETUP_Pt[i].cid, QString( "%1").arg( nlocalSetup )	);
		if( iCAStatus != ECA_NORMAL) {
//			pWnd->printStatus(LEVEL_ERROR,"ERROR: pvCLTU[%d].SETUP_Pt[%d], %s\n", sysID, i, ca_message( iCAStatus) );	
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SETUP_Pt[i].name, nlocalSetup, ca_message( iCAStatus) );
			return WR_ERROR;
		}
		usleep(100000);

	}

	/* set multi trigger port config */
	iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].mTRIG.cid, QString( "%1").arg( cfg_CLTU.nmTrig ) );
	if( iCAStatus != ECA_NORMAL) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].mTRIG.name, cfg_CLTU.nmTrig, ca_message( iCAStatus) );
		return WR_ERROR;
	}
//	usleep(10000);
	for(int i=0; i<3; i++) 
	{
		// set port config for each shot term.
		iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].mT0[i].cid, QString( "%1").arg( cfg_CLTU.dbmT0[i] ) );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysID].mT0[i].name, cfg_CLTU.dbmT0[i], ca_message( iCAStatus) );
			return WR_ERROR;
			}
//		usleep(1000);
		iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].mT1[i].cid, QString( "%1").arg( cfg_CLTU.dbmT1[i] ) );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%f),  %s\n", pWnd->pvCLTU[sysID].mT1[i].name, cfg_CLTU.dbmT1[i], ca_message( iCAStatus) );
			return WR_ERROR;
			}

//		usleep(1000);
		iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].mCLOCK[i].cid, QString( "%1").arg( cfg_CLTU.nmClock[i] ) );
		if( iCAStatus != ECA_NORMAL) {
			pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].mCLOCK[i].name, cfg_CLTU.nmClock[i], ca_message( iCAStatus) );
			return WR_ERROR;
			}

		usleep(200000);
	}
	usleep(300000);
	iCAStatus=ca_Put_QStr(  pWnd->pvCLTU[sysID].SETUP_Pt[4].cid, QString( "%1").arg( nlocalSetup )  );
	nlocalSetup = nlocalSetup ? 0:1;
	if( iCAStatus != ECA_NORMAL) {
		pWnd->printStatus(LEVEL_ERROR,"ERROR: \"%s\" (%d),  %s\n", pWnd->pvCLTU[sysID].SETUP_Pt[4].name, 1, ca_message( iCAStatus) );
		return WR_ERROR;
	}
	
// you don't need set shot term......
//	ca_Put_QStr(  pWnd->pvCLTU[sysID].SHOT_TERM.cid, QString("20") );
	return WR_OK;
}






void LocalTestR1::Initialize()
{
	MainWindow *pWnd = (MainWindow*)pm;

	pWnd->pg_timeset.pg_set_bliptime(16);
	pWnd->pg_timeset.pg_set_bliptime_ccs(16);
#if 0
	FILE *fp;
	char buf[64];
	double dbStart, dbStop;
	int nTrig;
	int nid;
//	char fClock[16];
	float fClock, fClock1, fClock2;
	char bufLineScan[512];

	double dbStart1, dbStop1;
	double dbStart2, dbStop2;
//	char strClock1[16];
//	char strClock2[16];
	
	int curTestSystem;


	if( (fp = fopen("/home/kstar/tss_configure/.cltucfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can not open CLTU configure file for read\n");	
		return;
	} else {
		while(1) 
		{
			if( fgets(bufLineScan, 128, fp) == NULL ) break;
			if( bufLineScan[0] == '#') ;
			else {
				sscanf(bufLineScan, "%s", buf);
				if( strcmp(buf, "TEST") == 0) {
					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s", buf); //printf("1: %s\n", buf);
					curTestSystem = pWnd->cb_test_system->findText( buf, Qt::MatchExactly); // Returns the index of the item containing the given text; otherwise returns -1.
					if( curTestSystem < 0) QMessageBox::information(0, "Timing test", tr("system type error!") );
					pWnd->cb_test_system->setCurrentIndex(curTestSystem );

					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s %d %lf %lf %f", buf,  &nTrig, &dbStart, &dbStop, &fClock); //P1 0 1 20 1
					pWnd->le_test_pt1_start->setText(QString("%1").arg(dbStart) );
					pWnd->le_test_pt1_stop->setText(QString("%1").arg(dbStop) );
					pWnd->le_test_pt1_freq->setText(QString("%1").arg(fClock) );
//					nid = pWnd->cb_test_pt1_freq->findText( fClock, Qt::MatchExactly);
//					if( nid < 0) QMessageBox::information(0, "Timing test", tr("port1 clock value error!") );
//					pWnd->cb_test_pt1_freq->setCurrentIndex(nid );
					pWnd->cb_test_pt1_pol->setCurrentIndex(nTrig);


					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s %d %lf %lf %f", buf,  &nTrig, &dbStart, &dbStop, &fClock); //P1 0 1 20 1
					pWnd->le_test_pt2_start->setText(QString("%1").arg(dbStart) );
					pWnd->le_test_pt2_stop->setText(QString("%1").arg(dbStop) );
					pWnd->le_test_pt2_freq->setText(QString("%1").arg(fClock) );
//					pWnd->cb_test_pt2_freq->setCurrentIndex(  pWnd->cb_test_pt2_freq->findText( fClock, Qt::MatchExactly)   );
					pWnd->cb_test_pt2_pol->setCurrentIndex(nTrig);

					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s %d %lf %lf %f", buf,  &nTrig, &dbStart, &dbStop, &fClock); //P1 0 1 20 1
					pWnd->le_test_pt3_start->setText(QString("%1").arg(dbStart) );
					pWnd->le_test_pt3_stop->setText(QString("%1").arg(dbStop) );
					pWnd->le_test_pt3_freq->setText(QString("%1").arg(fClock) );
//					pWnd->cb_test_pt3_freq->setCurrentIndex(  pWnd->cb_test_pt3_freq->findText( fClock, Qt::MatchExactly)   );
					pWnd->cb_test_pt3_pol->setCurrentIndex(nTrig);

					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s %d %lf %lf %f", buf,  &nTrig, &dbStart, &dbStop, &fClock); //P1 0 1 20 1
					pWnd->le_test_pt4_start->setText(QString("%1").arg(dbStart) );
					pWnd->le_test_pt4_stop->setText(QString("%1").arg(dbStop) );
					pWnd->le_test_pt4_freq->setText(QString("%1").arg(fClock) );
//					pWnd->cb_test_pt4_freq->setCurrentIndex(  pWnd->cb_test_pt4_freq->findText( fClock, Qt::MatchExactly)   );
					pWnd->cb_test_pt4_pol->setCurrentIndex(nTrig);

					fgets(bufLineScan, 128, fp);
					sscanf(bufLineScan, "%s %d %lf %lf %f\t\t%lf %lf %f\t\t%lf %lf %f", buf, \
											&nTrig, &dbStart, &dbStop, &fClock, \
											&dbStart1, &dbStop1, &fClock1, \
											&dbStart2, &dbStop2, &fClock2 );
					pWnd->le_test_pt5_start1->setText(QString("%1").arg(dbStart) );
					pWnd->le_test_pt5_stop1->setText(QString("%1").arg(dbStop) );
					pWnd->le_test_pt5_freq1->setText(QString("%1").arg(fClock) );
//					pWnd->cb_test_pt5_freq1->setCurrentIndex(  pWnd->cb_test_pt5_freq1->findText( fClock, Qt::MatchExactly)   );
					pWnd->cb_test_pt5_pol1->setCurrentIndex(nTrig);
					pWnd->le_test_pt5_start2->setText(QString("%1").arg(dbStart1) );
					pWnd->le_test_pt5_stop2->setText(QString("%1").arg(dbStop1) );
					pWnd->le_test_pt5_freq2->setText(QString("%1").arg(fClock1) );
//					pWnd->cb_test_pt5_freq2->setCurrentIndex(  pWnd->cb_test_pt5_freq2->findText( fClock1, Qt::MatchExactly)   );
					pWnd->le_test_pt5_start3->setText(QString("%1").arg(dbStart2) );
					pWnd->le_test_pt5_stop3->setText(QString("%1").arg(dbStop2) );
					pWnd->le_test_pt5_freq3->setText(QString("%1").arg(fClock2) );
//					pWnd->cb_test_pt5_freq3->setCurrentIndex(  pWnd->cb_test_pt5_freq3->findText( fClock2, Qt::MatchExactly)   );

				}
/*				else if( strcmp(buf, "CALIBRATION") == 0) {
					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_pcs->setCheckState(Qt::Checked);

					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_mps1->setCheckState(Qt::Checked);

					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_mps2->setCheckState(Qt::Checked);

					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_ech->setCheckState(Qt::Checked);

					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_icrh->setCheckState(Qt::Checked);

					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_dds1->setCheckState(Qt::Checked);
					fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%s %d", buf, &nid);
					if( nid ) pWnd->check_cal_dds2->setCheckState(Qt::Checked);
					
				} 
*/				else if ( strcmp(buf, "BLIP_TIME")==0 ) {
						fgets(bufLineScan, 128, fp); sscanf(bufLineScan, "%d", &nid);
						pWnd->pg_timeset.pg_set_bliptime(nid);
						pWnd->pg_timeset.pg_set_bliptime_ccs(nid);
//						pWnd->le_bliptime_my->setText(QString("%1").arg(nid) );
				}
			} // end of if not '#'
		} // while(1)

		fclose(fp);
	}
#endif

}



