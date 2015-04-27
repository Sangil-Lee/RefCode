#include "ECCD_Main.h"

//int iPage;
QPalette palette1;
QPalette palette2;
QBrush fontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush backbrush1(QColor(0, 150, 0, 255));		//background green

QBrush fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush backbrush2(QColor(255, 255, 255, 255));	//background white


ECCD_Main::ECCD_Main()

{
	//ECCDLOGIN = new ECCD_Login(this);
 	//if (ECCDLOGIN->exec() == QDialog::Accepted)
 	//{
		ui.setupUi(this);
		pattach = new AttachChannelAccess(this);
	
	      ECCDCALORIMETRIC = new ECCD_Calorimetric(this);
	      ECCDCALORIMETRIC->setGeometry(170, 100, 1200, 760);

	      ECCDGYROTRON_0 = new ECCD_Gyrotron_0(this);
	      ECCDGYROTRON_0->setGeometry(170, 100, 1200, 760);

		ECCDOPERATION = new ECCD_Operation(this);
      	ECCDOPERATION->setGeometry(170, 100, 1200, 760);
		ECCDOPERATION->show();
		CloseWdgt=ECCDOPERATION;
	//	setpushButton(0);

		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDOPERATION_Show()));
		connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDSTATUS_Show()));
		connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(slot_ECCDPOWERSUPPLY_0_Show()));
		connect(ui.pushButton_4, SIGNAL(clicked()), this, SLOT(slot_ECCDGYROTRON_0_Show()));     
		connect(ui.pushButton_5, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_0_Show()));
		connect(ui.pushButton_6, SIGNAL(clicked()), this, SLOT(slot_ECCDANTENA_0_Show()));
		connect(ui.pushButton_7, SIGNAL(clicked()), this, SLOT(slot_ECCDCOOLING_Show()));
		connect(ui.pushButton_8, SIGNAL(clicked()), this, SLOT(slot_ECCDCALORIMETRIC_Show()));
		connect(ui.pushButton_9, SIGNAL(clicked()), this, SLOT(slot_ECCDTIMINGDAQ_0_Show()));
		connect(ui.pushButton_10, SIGNAL(clicked()), this, SLOT(slot_ECCDINTERLOCKSAFETY_Show()));
		connect(ui.calbCM_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_CM_Stat_Change1(bool)));
		connect(ui.calbCM_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_CM_Stat_Change2(bool)));
		connect(ui.calbTLCM_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change1(bool)));
		connect(ui.calbTLCM_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change2(bool)));
		connect(ui.calbOPKstar, SIGNAL(valueChanged(bool)), this, SLOT(slot_OP_Stat_Change1(bool)));
		connect(ui.calbOPDummy, SIGNAL(valueChanged(bool)), this, SLOT(slot_OP_Stat_Change2(bool)));
		
		ui.calbKshotstat->hide();
		ui.calbLshotstat->hide();
		connect(ui.calbKshotstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_Shot_Stat_Change1(bool)));
		connect(ui.calbLshotstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_Shot_Stat_Change2(bool)));

		connect(ui.calbKshot, SIGNAL(valueChanged(double)), this, SLOT(slot_Shot_Number_Change1(double)));

		connect(ui.calbDAQstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_DAQ_Stat_Change1(bool)));
		
		ui.calbKshot->show();
		ui.calbLshot->show();
	//	connect(ui.pbInterlock_1, SIGNAL(clicked()), this, SLOT(slot_Goto_InterlockSafety()));

	//	connect(ECCDOPERATION->ui.pbGy_1, SIGNAL(clicked()), this, SLOT(slot_Goto_Page()));
      	slot_ECCDOPERATION_Show();

		//set label text
		palette1.setBrush(QPalette::Active, QPalette::WindowText, fontbrush1);
		palette1.setBrush(QPalette::Inactive, QPalette::WindowText, fontbrush1);

		//set background
		palette1.setBrush(QPalette::Active, QPalette::Window, backbrush1);
		palette1.setBrush(QPalette::Inactive, QPalette::Window, backbrush1);

		//set label text
		palette2.setBrush(QPalette::Active, QPalette::WindowText, fontbrush2);
		palette2.setBrush(QPalette::Inactive, QPalette::WindowText, fontbrush2);
		
		//set background
		palette2.setBrush(QPalette::Active, QPalette::Window, backbrush2);
		palette2.setBrush(QPalette::Inactive, QPalette::Window, backbrush2);

		ui.lbKSTARLogo->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_kstar.gif")));

		connect(ui.calbL1, SIGNAL(valueChanged(bool)), this, SLOT(slot_L1_Stat_Change(bool)));
		connect(ui.calbL2, SIGNAL(valueChanged(bool)), this, SLOT(slot_L2_Stat_Change(bool)));
		connect(ui.calbL3, SIGNAL(valueChanged(bool)), this, SLOT(slot_L3_Stat_Change(bool)));
		connect(ui.calbL4, SIGNAL(valueChanged(bool)), this, SLOT(slot_L4_Stat_Change(bool)));

	//}
	//else
	//{
	//	kill(getpid(), SIGTERM);
	//}
}

ECCD_Main::~ECCD_Main()
{
      disconnect(ui.pushButton, 0, 0, 0);

      if(ui.pushButton){ delete ui.pushButton; ui.pushButton = NULL; }
      if(ECCDOPERATION){ delete ECCDOPERATION; ECCDOPERATION = NULL; }
}



void ECCD_Main::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}


void ECCD_Main::slot_ECCDOPERATION_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDOPERATION = new ECCD_Operation(this);
      ECCDOPERATION->setGeometry(170, 100, 1200, 760);
	ECCDOPERATION->show();
	CloseWdgt = ECCDOPERATION;
	setpushButton(0);
	connect(ECCDOPERATION->ui.pbGy_1, SIGNAL(clicked()), this, SLOT(slot_Goto_Gyrotron_0()));
	connect(ECCDOPERATION->ui.pbGy_2, SIGNAL(clicked()), this, SLOT(slot_Goto_Gyrotron_0()));
	connect(ECCDOPERATION->ui.pbGy_3, SIGNAL(clicked()), this, SLOT(slot_Goto_Gyrotron_0()));
	connect(ECCDOPERATION->ui.pbGy_4, SIGNAL(clicked()), this, SLOT(slot_Goto_Gyrotron_1()));
	connect(ECCDOPERATION->ui.pbGy_5, SIGNAL(clicked()), this, SLOT(slot_Goto_Gyrotron_2()));

	connect(ECCDOPERATION->ui.pbTrn_1, SIGNAL(clicked()), this, SLOT(slot_Goto_Transline_0()));
	connect(ECCDOPERATION->ui.pbTrn_2, SIGNAL(clicked()), this, SLOT(slot_Goto_Transline_1()));
	connect(ECCDOPERATION->ui.pbTrn_3, SIGNAL(clicked()), this, SLOT(slot_Goto_Transline_1()));
	connect(ECCDOPERATION->ui.pbTrn_4, SIGNAL(clicked()), this, SLOT(slot_Goto_Transline_1()));
	connect(ECCDOPERATION->ui.pbPol_1, SIGNAL(clicked()), this, SLOT(slot_Goto_Transline_2()));

	connect(ECCDOPERATION->ui.pbLau_1, SIGNAL(clicked()), this, SLOT(slot_Goto_Antena_0()));

	connect(ECCDOPERATION->ui.pbLTU_1, SIGNAL(clicked()), this, SLOT(slot_Goto_TimingDAQ_0()));
	
	//connect(ECCDOPERATION->ui.pbLTU_2, SIGNAL(clicked()), this, SLOT(slot_Goto_TimingDAQ_0()));

}
 
void ECCD_Main::slot_ECCDSTATUS_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDSTATUS = new ECCD_Status(this);
      ECCDSTATUS->setGeometry(170, 100, 1200, 760);
	ECCDSTATUS->show();
	CloseWdgt = ECCDSTATUS;
	setpushButton(1);
}
 
void ECCD_Main::slot_ECCDPOWERSUPPLY_0_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDPOWERSUPPLY_0 = new ECCD_PowerSupply_0(this);
      ECCDPOWERSUPPLY_0->setGeometry(170, 100, 1200, 760);
	ECCDPOWERSUPPLY_0->show();
	CloseWdgt = ECCDPOWERSUPPLY_0;
	setpushButton(2);
}

void ECCD_Main::slot_ECCDGYROTRON_0_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      //ECCDGYROTRON_0 = new ECCD_Gyrotron_0(this);
      //ECCDGYROTRON_0->setGeometry(170, 100, 1200, 760);
      ECCDGYROTRON_0->show();
	//CloseWdgt = ECCDGYROTRON_0;
	setpushButton(3);
}

void ECCD_Main::slot_ECCDTRANSLINE_0_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDTRANSLINE_0 = new ECCD_TransLine_0(this);
      ECCDTRANSLINE_0->setGeometry(170, 100, 1200, 760);
	ECCDTRANSLINE_0->show();
	CloseWdgt = ECCDTRANSLINE_0;
	setpushButton(4);
}
 
void ECCD_Main::slot_ECCDANTENA_0_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDANTENA_0 = new ECCD_Antena_0(this);
      ECCDANTENA_0->setGeometry(170, 100, 1200, 760);
      ECCDANTENA_0->show();
	CloseWdgt = ECCDANTENA_0;
	setpushButton(5);
}
 
void ECCD_Main::slot_ECCDCOOLING_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDCOOLING = new ECCD_Cooling(this);
      ECCDCOOLING->setGeometry(170, 100, 1200, 760);
	ECCDCOOLING->show();
	CloseWdgt = ECCDCOOLING;
	setpushButton(6);
}
 
void ECCD_Main::slot_ECCDCALORIMETRIC_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      //ECCDCALORIMETRIC = new ECCD_Calorimetric(this);
      //ECCDCALORIMETRIC->setGeometry(170, 100, 1200, 760);
	ECCDCALORIMETRIC->show();
	//CloseWdgt = ECCDCALORIMETRIC;
	setpushButton(7);
}
 
void ECCD_Main::slot_ECCDTIMINGDAQ_0_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDTIMINGDAQ_0 = new ECCD_TimingDAQ_0(this);
      ECCDTIMINGDAQ_0->setGeometry(170, 100, 1200, 760);
	ECCDTIMINGDAQ_0->show();
	CloseWdgt = ECCDTIMINGDAQ_0;
	setpushButton(8);
}
/* 
void ECCD_Main::slot_ECCDTIMINGDAQ_1_Show(void){
	CloseWdgt->hide();
      ECCDTIMINGDAQ_1 = new ECCD_TimingDAQ_1(this);
      ECCDTIMINGDAQ_1->setGeometry(170, 100, 1200, 760);
      ECCDTIMINGDAQ_1->show();
	CloseWdgt = ECCDTIMINGDAQ_1;
} 

void ECCD_Main::slot_ECCDTIMINGDAQ_2_Show(void){
	CloseWdgt->hide();
      ECCDTIMINGDAQ_2 = new ECCD_TimingDAQ_2(this);
      ECCDTIMINGDAQ_2->setGeometry(170, 100, 1200, 760);
      ECCDTIMINGDAQ_2->show();
	CloseWdgt = ECCDTIMINGDAQ_2;
}
*/ 
void ECCD_Main::slot_ECCDINTERLOCKSAFETY_Show(void){
	ECCDCALORIMETRIC->hide();
	ECCDGYROTRON_0->hide();
	CloseWdgt->hide();
      ECCDINTERLOCKSAFETY = new ECCD_InterlockSafety(this);
      ECCDINTERLOCKSAFETY->setGeometry(170, 100, 1200, 760);
	ECCDINTERLOCKSAFETY->show();
	CloseWdgt = ECCDINTERLOCKSAFETY;
	setpushButton(9);
}

void ECCD_Main::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);
	ui.pushButton->setFont(font);
	ui.pushButton_2->setFont(font);
	ui.pushButton_3->setFont(font);
	ui.pushButton_4->setFont(font);
	ui.pushButton_5->setFont(font);
	ui.pushButton_6->setFont(font);
	ui.pushButton_7->setFont(font);
	ui.pushButton_8->setFont(font);
	ui.pushButton_9->setFont(font);
	ui.pushButton_10->setFont(font);

	ui.CALBfaIntl_1_0->show();
	ui.CALBfaIntl_1_1->show();
	ui.CALBfaIntl_1_2->show();
	ui.CALBfaIntl_1_3->show();
	ui.CALBfaIntl_1_4->show();
	ui.CALBfaIntl_1_5->show();

	ui.CALBslIntl_1_0->show();
	ui.CALBslIntl_1_1->show();
	ui.CALBslIntl_1_2->show();
	ui.CALBslIntl_1_3->show();
	ui.CALBslIntl_1_4->show();
	ui.CALBslIntl_1_5->show();

	font.setBold(true);

	switch (inum)
	{
		case 0:
			ui.pushButton->setFont(font);	
			break;
		case 1:
			ui.pushButton_2->setFont(font);	
			break;
		case 2:
			ui.pushButton_3->setFont(font);	
			break;
		case 3:
			ui.pushButton_4->setFont(font);	
			break;
		case 4:
			ui.pushButton_5->setFont(font);
			break;
		case 5:
			ui.pushButton_6->setFont(font);	
			break;
		case 6:
			ui.pushButton_7->setFont(font);	
			break;
		case 7:
			ui.pushButton_8->setFont(font);	
			break;
		case 8:
			ui.pushButton_9->setFont(font);	
			break;
		case 9:
			ui.pushButton_10->setFont(font);	
			break;
	}
}

void ECCD_Main::slot_Goto_Gyrotron_0(){
        iPage=0;
	slot_ECCDGYROTRON_0_Show();
}

void ECCD_Main::slot_Goto_Gyrotron_1(){
        iPage=1;
	slot_ECCDGYROTRON_0_Show();
}

void ECCD_Main::slot_Goto_Gyrotron_2(){
        iPage=2;
	slot_ECCDGYROTRON_0_Show();
}

void ECCD_Main::slot_Goto_Transline_0(){
        iPage=0;
	slot_ECCDTRANSLINE_0_Show();
}

void ECCD_Main::slot_Goto_Transline_1(){
        iPage=1;
	slot_ECCDTRANSLINE_0_Show();
}

void ECCD_Main::slot_Goto_Transline_2(){
        iPage=2;
	slot_ECCDTRANSLINE_0_Show();
}

void ECCD_Main::slot_Goto_Antena_0(){
        iPage=0;
	slot_ECCDANTENA_0_Show();
}

void ECCD_Main::slot_Goto_InterlockSafety(){
        iPage=0;
	slot_ECCDINTERLOCKSAFETY_Show();
}

void ECCD_Main::slot_Goto_TimingDAQ_0(){
        iPage=0;
	slot_ECCDTIMINGDAQ_0_Show();
}

void ECCD_Main::slot_CM_Stat_Change1(bool bMode)
{
	bEMode = bMode;
	ui.lbCM_3->hide();
	if(bMode == true)
	{
		ui.calbCM_1->setPalette(palette1);
		ui.calbCM_1->show();
	}
	else
	{
		ui.calbCM_1->setPalette(palette2);
		ui.calbCM_1->hide();
	}
	ui.calbCM_1->setAutoFillBackground(true);
	if(bEMode == false && bPMode == false) ui.lbCM_3->show(); 
}

void ECCD_Main::slot_CM_Stat_Change2(bool bMode)
{
	bPMode = bMode;
	ui.lbCM_3->hide();
	if(bMode == true)
	{
		ui.calbCM_2->setFillColor(QColor("red"));
		ui.calbCM_2->show();
	}
	else
	{
		ui.calbCM_2->setPalette(palette2);
		ui.calbCM_2->hide();
	}
	ui.calbCM_2->setAutoFillBackground(true); 
	if(bEMode == false && bPMode == false) ui.lbCM_3->show();
}

void ECCD_Main::slot_TLCM_Stat_Change1(bool bMode)
{
	bTLRemoteMode = bMode;
	ui.lbTLCM_3->hide();
	if(bMode == true)
	{
		ui.calbTLCM_1->setPalette(palette1);
		ui.calbTLCM_1->show();
	}
	else
	{
		ui.calbTLCM_1->setPalette(palette2);
		ui.calbTLCM_1->hide();
	}
	ui.calbTLCM_1->setAutoFillBackground(true);
	if(bTLRemoteMode == false && bTLLocalMode == false) ui.lbTLCM_3->show(); 
}

void ECCD_Main::slot_TLCM_Stat_Change2(bool bMode)
{
	bTLLocalMode = bMode;
	ui.lbTLCM_3->hide();
	if(bMode == true)
	{
		ui.calbTLCM_2->setFillColor(QColor("red"));
		ui.calbTLCM_2->show();
	}
	else
	{
		ui.calbTLCM_2->setPalette(palette2);
		ui.calbTLCM_2->hide();
	}
	ui.calbTLCM_2->setAutoFillBackground(true); 
	if(bTLRemoteMode == false && bTLLocalMode == false) ui.lbTLCM_3->show();
}

void ECCD_Main::slot_OP_Stat_Change1(bool bMode)
{
	chid   searchID;
	int    istatus;
	int iData = 0;

	bKstarMode = bMode;
	if(bMode == false)
	{
		ui.calbOPKstar->setPalette(palette2);
		ui.calbOPKstar->setAutoFillBackground(true);

		//KSTAR EMERGENCY DISABLE ON
		ca_search ("EC1_PLCS_ECMD_KEMGDIS", &searchID);
		istatus = ca_pend_io(3.0);
		iData = 1;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.5);

		//KSTAR EMERGENCY DISABLE Off
		ca_search ("EC1_PLCS_ECMD_KEMGDIS", &searchID);
		istatus = ca_pend_io(3.0);
		iData = 0;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.5);

	}
	else
	{
		//KSTAR EMERGENCY ENABLE ON
		ca_search ("EC1_PLCS_ECMD_KEMGEN", &searchID);
		istatus = ca_pend_io(3.0);
		iData = 1;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.5);

		//KSTAR EMERGENCY ENABLE Off
		ca_search ("EC1_PLCS_ECMD_KEMGEN", &searchID);
		istatus = ca_pend_io(3.0);
		iData = 0;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.5);
	}
}

void ECCD_Main::slot_OP_Stat_Change2(bool bMode)
{
	chid   searchID;
	int    istatus;
	int iData = 0;

	bDummyMode = bMode;	
	if(bMode == false)
	{
		ui.calbOPDummy->setPalette(palette2);
		ui.calbOPDummy->setAutoFillBackground(true);
	}
	else
	{
		//KSTAR EMERGENCY DISABLE ON
		ca_search ("EC1_PLCS_ECMD_KEMGDIS", &searchID);
		istatus = ca_pend_io(0.1);
		iData = 1;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.0);

		//KSTAR EMERGENCY DISABLE Off
		ca_search ("EC1_PLCS_ECMD_KEMGDIS", &searchID);
		istatus = ca_pend_io(3.0);
		iData = 0;
		ca_put(DBR_INT, searchID, &iData);
		istatus = ca_pend_io(1.5);
	}
}

void ECCD_Main::slot_Shot_Stat_Change1(bool bMode)
{
	chid   searchID;
	int    istatus;
	int iData = 0;
//	int iShotNum = 0;

	if(bMode == true)
	{
		//ui.cablb_1->show();
		//ui.cagpBlank1->show();
		if (bKstarMode == true)
		{
/*			
			//KSTAR ShotNumber get
			ca_search ("CCS_SHOT_NUMBER", &searchID);
			istatus = ca_pend_io(0.1);
			ca_get(DBR_INT, searchID, &iShotNum);
			istatus = ca_pend_io(0.1);

			//Local ShotNumber put
			ca_search ("EC1_DAQ_AO_LOCALSHOT", &searchID);
			istatus = ca_pend_io(0.1);
			ca_put(DBR_INT, searchID, &iShotNum);
			istatus = ca_pend_io(0.1);
*/
			//DAQ Start On
			ca_search ("EC1_DAQ_CMD_START", &searchID);
			istatus = ca_pend_io(0.1);
			iData = 1;
			ca_put(DBR_INT, searchID, &iData);
			istatus = ca_pend_io(1.0);

			//DAQ Start Off
			ca_search ("EC1_DAQ_CMD_START", &searchID);
			istatus = ca_pend_io(0.1);
			iData = 0;
			ca_put(DBR_INT, searchID, &iData);
			istatus = ca_pend_io(1.0);
		}
	}
	else
	{
		//ui.cablb_1->hide();
		//ui.cagpBlank1->hide();
	}
}

void ECCD_Main::slot_Shot_Stat_Change2(bool bMode)
{

	//bDummyMode = bMode;
	if(bMode == true)
	{
		//ui.cablb_2->show();
	}
	else
	{
		//ui.cablb_2->hide();
		//ui.cagpBlank2->hide();
	}
}


void ECCD_Main::slot_DAQ_Stat_Change1(bool bMode)
{
	
	if(bMode == true)
	{
		if (bKstarMode == true)
		{
			//ui.cablb_1->show();
			//ui.cagpBlank1->show();
			ui.calbKshot->show();
			//ui.cagpBlank2->show();
			ui.calbLshot->show();
		}
		else
		{
			//ui.cablb_1->hide();
			//ui.cagpBlank1->hide();
			//ui.cablb_2->hide();
			//ui.cagpBlank2->hide();
		}

		if (bDummyMode == true)
		{
			//ui.cablb_2->show();
			//ui.cagpBlank2->show();
			ui.calbLshot->show();
		}
		else
		{
			//ui.cablb_2->hide();
			//ui.cagpBlank2->hide();
		}

	}
	else
	{
		//ui.cablb_1->hide();
		//ui.cablb_2->hide();
		//ui.cagpBlank1->hide();
		//ui.cagpBlank2->hide();
	}
}

void ECCD_Main::slot_Shot_Number_Change1(double dStatus)
{
/*	chid   searchID;
	int    istatus;
	int iShotNum = int(dStatus);

	if (bKstarMode == true)
	{
		//Local ShotNumber put
		ca_search ("EC1_DAQ_AO_LOCALSHOT", &searchID);
		istatus = ca_pend_io(0.1);
		ca_put(DBR_INT, searchID, &iShotNum);
		istatus = ca_pend_io(0.1);
	}
*/
}

void ECCD_Main::slot_L1_Stat_Change(bool bMode)
{
	if(bMode == true)
	{
		ui.calbL1->show();
	}
	else
	{
		ui.calbL1->hide();
	}
}

void ECCD_Main::slot_L2_Stat_Change(bool bMode)
{
	if(bMode == true)
	{
		ui.calbL2->show();
	}
	else
	{
		ui.calbL2->hide();
	}
}

void ECCD_Main::slot_L3_Stat_Change(bool bMode)
{
	if(bMode == true)
	{
		ui.calbL3->show();
	}
	else
	{
		ui.calbL3->hide();
	}
}

void ECCD_Main::slot_L4_Stat_Change(bool bMode)
{
	if(bMode == true)
	{
		ui.calbL4->show();
	}
	else
	{
		ui.calbL4->hide();
	}
}
