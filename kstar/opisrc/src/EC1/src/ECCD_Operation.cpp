#include "ECCD_Operation.h"

const char *CPSDesc[60] = {
"No Fault", "EMERGENCY STOP", "22KV MAIN VCB ON FAULT", "22KV MAIN VCB TRIP(OVER CURRENT/VOLTAGE)", "6.6KV MAIN VCB ON FAULT", 
"6.6KV MAIN VCB TRIP(OVER CURRENT/VOLTAGE)", "VCS-HV MAIN M/C ON FAULT", "VCS-HV PRECHARGE M/C ON FAULT", "VCS-HV TRIP(OVER CURRENT/VOLTAGE)", "VCS-LV MAIN M/C ON FAULT", 
"VCS-LV PRECHARGE M/C ON FAULT", "VCS-LV TRIP(OVER CURRENT/VOLTAGE)", "MAIN TRANSFORMER OVER HEAT", "MAIN TRANSFORMER GAS", "LV CHOPPER TR OVER HEAT", 
"HV CHOPPER TR1 PVER HEAT", "HV CHOPPER TR2 PVER HEAT", "HV CHOPPER TR3 PVER HEAT", "HV CHOPPER TR4 PVER HEAT", "LV CHOPPER STACK DC OVER VOLTAGE", 
"LV CHOPPER STACK DC UNDER VOLTAGE", "LV CHOPPER STACK DC OVER VOLTAGE", "LV CHOPPER STACK DC UNDER CURRENT", "CHOPPER RETURN OVER CURRENT", "CHOPPER STACK TRIP : LV GDF 1-8", 
"CHOPPER STACK TRIP : LV GDF 9-12, HV GDF 11-14", "CHOPPER STACK TRIP : HV GDF 15,21,22, TEMP 11-15", "CHOPPER STACK TRIP : HV GDF 23-25, TEMP 21-25", "CHOPPER STACK TRIP : HV GDF 31-35, TEMP 31-33", "CHOPPER STACK TRIP : HV GDF 41-45, TEMP 34,35,41", 
"CHOPPER STACK TRIP : HV TEMP 42-45", "HV SENSING OUTPUT OVER VOLTAGE", "HV SENSING OUTPUT UNDER VOLTAGE", "HV SENSING OUTPUT OVER CURRENT", "LCP INTERLOCK OFF", 
"COMMUNICATION ERROR", "LV CHARGE ERROR", "HV CHARGE ERROR", "FPGA WATCHDOG ERROR", "SEQUEMCE ERROR"
};

const char *APSDesc[60] = {
"No Fault", "EMERGENCY STOP", "OUTPUT OVER VOLTAGE", "OUTPUT UNDER VOLTAGE", "SHUNT #1(RETURN) OVER CURRENT", 
"SHUNT #2(HV OUT) OVER CURRENT", "APS DISCHARGE DRIVER ERROR", "APS HVS TANK TEMPERATURE FAULT", "LCP INTERLOCK OFF", "COMMUNICATION ERROR", 
"FPGA WATCHDOG FAULT", "SEQUENCE ERROR", "FIX_ZERNER1_Fault", "FIX_ZERNER2_Fault", "INV_Fault"
};

const char *BPSDesc[60] = {
"No Fault", "EMERGENCY STOP", "STANDBY (MMC) FAULT", "HVDC P/S FAULT", "OUTPUT OVER VOLTAGE", 
"OUTPUT UNDER VOLTAGE", "SHUNT #3(HV OUT) OVER CURRENT", "BPS HVS DRIVER ERROR", "BPS DISCHARGE DRIVER ERROR", "APS HVS TANK TEMPERATURE FAULT", 
"LCP INTERLOCK OFF", "COMMUNICATION ERROR", "FPGA WATCHDOG FAULT", "SEQUENCE ERROR"
};

const char *LCSDesc[60] = {
"NO Fault", "CPS HVS DRIVE FAULT", "CPS HVS OVER CURRENT", "CATHODE-BODY ARC", "ANODE OVER CURRENT", 
"BODY OVER CURRENT", "KSTAR EMERGENCY", "SLOW INTERLOCK", "GYROTRON RF_REF", "GYROTRON RF_FWD", 
"WINDOW ARC1", "WINDOW ARC2","MITER BAND11", "MITER BAND12", "MITER BAND21", 
"MITER BAND22", "VAC.ION1 ARC", "VAC.ION2 ARC", "VAC.ION3 ARC", "CPS HVS TANK OVERHEAT", 
"CPS HV SENSING PANEL GROUNDED", "CPS HV SENSING PANEL DOOR OPEN", "CPS P/S FAULT", "APS P/S FAULT", "BPS P/S FAULT", 
"SEQUENCE ERROR", "COMMUNICATION ERROR", "EMERGENCY STOP", "CPS SENSING PANEL OVER CURRENT"
};

const char *SlowIntDesc[60] = {
"NO Fault", "KSTAR INTERLOCK L1", "KSTAR INTERLOCK L2", "KSTAR INTERLOCK L3", "KSTAR INTERLOCK L4",  
"GYROTRON FAULT", "WATER COOLING FLOW FAULT", "VACUUM FAULT", "SAFETY KEY", "WATER COLING TEMPERATURE FAULT",
"COLLECTOR T/C FAULT", "EMERGENCY STOP", "OPERATION MODE FAULT","HEATING DEVICE'S ROOM INNER FENCE","HEATING DEVICE'SROOM GROUND BAR"
};

const char *OpTMPDesc[8] = {
"ST0P", "Fault", "STARTING", "NOR. OP", "Fault",
"   ", "Fault", "Fault"
};

const char *OpTMP5Desc[8] = {
"ST0P", "STARTING", "NORMAL OPERATION"
};

const char *OpVacDesc[13] = {
"No Fault","Panel Interlock", "Remote Interlock", "Cable Interlock", "HV not found", 
"HV Fault", "HV Overtemperature", "R.I/O not found", "R.I/O Fault", "Protect", 
"Short Circuit", "Over Volt/Curr", "Zero Meas" 
};

const char *OperGVDesc[3] = {
"OPEN", "CLOSE", "FAULT"
};

const char *OperWGDesc[3] = {
"To KSTAR", "To Dummy", "FAULT"
};

const char *ReadTagName[10] = {
"EC1_PBPS_RTN_TSDS", "EC1_PAPS_RTN_TSDS", "EC1_LTU_RF_ON", "EC1_LTU_RF_PULSE"
};

const char *PSWriteTagName[10] = {
"EC1_PBPS_ECMD_TSDS", "EC1_PBPS_ECMD_TEDS", "EC1_PAPS_ECMD_TSDS", "EC1_PAPS_ECMD_TEDS", "EC1_PAPS_ECMD_MTSDS",
"EC1_PAPS_ECMD_MTEDS", "EC1_PAPS_ECMD_MDUTY", "EC1_PAPS_ECMD_MFREQ", "EC1_PCPS_ECMD_PWS"
};

const char *WriteTagName[12] = {
"EC1A_LTU_P0_SEC0_T0", "EC1A_LTU_P0_SEC0_T1", "EC1A_LTU_P1_SEC0_T0", "EC1A_LTU_P1_SEC0_T1", "EC1A_LTU_P2_SEC0_T0",
"EC1A_LTU_P2_SEC0_T1", "EC1A_LTU_P4_SEC0_T0", "EC1A_LTU_P4_SEC0_T1", "EC1A_LTU_P3_SEC0_T0", "EC1A_LTU_P3_SEC0_T1"
};
const double dDelay = 1.5;
const double dDelay2 = 3.0;
void delay_handler() { /** NO PROCEDURE REQUIRED **/ }

void delay(float fsec)
{
    struct itimerval rtm;
    signal(SIGALRM, (void(*)(int))delay_handler);
    rtm.it_value.tv_sec     = (long)fsec;
    rtm.it_value.tv_usec    = 1000000. * (fsec - rtm.it_value.tv_sec);
    rtm.it_interval.tv_sec  = 0;
    rtm.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL,&rtm,0) == -1 ){
        return;
    }
    pause();
}


QPalette Oppalette1;
QPalette Oppalette2;
QPalette Oppalette3;

QBrush Opfontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush Opbackbrush1(QColor(0, 150, 0, 255));		//background green

QBrush Opfontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush Opbackbrush2(QColor(255, 255, 255, 255));	//background white

QBrush Opfontbrush3(QColor(255, 255, 255, 255));	//font white
QBrush Opbackbrush3(QColor(255, 0, 0, 255));	//background red

ECCD_Operation::ECCD_Operation(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	//set label text
	Opfontbrush1.setStyle(Qt::SolidPattern);
	Oppalette1.setBrush(QPalette::Active, QPalette::WindowText, Opfontbrush1);
	Oppalette1.setBrush(QPalette::Inactive, QPalette::WindowText, Opfontbrush1);

	//set background
	Opbackbrush1.setStyle(Qt::SolidPattern);
	Oppalette1.setBrush(QPalette::Active, QPalette::Window, Opbackbrush1);
	Oppalette1.setBrush(QPalette::Inactive, QPalette::Window, Opbackbrush1);

	//set label text
	Oppalette2.setBrush(QPalette::Active, QPalette::WindowText, Opfontbrush2);
	Oppalette2.setBrush(QPalette::Inactive, QPalette::WindowText, Opfontbrush2);
		
	//set background
	Oppalette2.setBrush(QPalette::Active, QPalette::Window, Opbackbrush2);
	Oppalette2.setBrush(QPalette::Inactive, QPalette::Window, Opbackbrush2);

	//set label text
	Oppalette3.setBrush(QPalette::Active, QPalette::WindowText, Opfontbrush3);
	Oppalette3.setBrush(QPalette::Inactive, QPalette::WindowText, Opfontbrush3);
		
	//set background
	Oppalette3.setBrush(QPalette::Active, QPalette::Window, Opbackbrush3);
	Oppalette3.setBrush(QPalette::Inactive, QPalette::Window, Opbackbrush3);

	ui.cadpTP1_3->hide();
	ui.cadpTP2_3->hide();
	ui.cadpTP3_3->hide();
	ui.cadpTP4_3->hide();
	ui.cadpTP5_3->hide();

	connect(ui.cadpTP1_3, SIGNAL(valueChanged(double)), this, SLOT(slot_TMP1_Stat_Desc(double)));
	connect(ui.cadpTP2_3, SIGNAL(valueChanged(double)), this, SLOT(slot_TMP2_Stat_Desc(double)));
	connect(ui.cadpTP3_3, SIGNAL(valueChanged(double)), this, SLOT(slot_TMP3_Stat_Desc(double)));
	connect(ui.cadpTP4_3, SIGNAL(valueChanged(double)), this, SLOT(slot_TMP4_Stat_Desc(double)));
	connect(ui.cadpTP5_3, SIGNAL(valueChanged(double)), this, SLOT(slot_TMP5_Stat_Desc(double)));

	connect(ui.capbDG645_1, SIGNAL(clicked()), this, SLOT(slot_SET_NO1(void)));
	connect(ui.capbDG645_2, SIGNAL(clicked()), this, SLOT(slot_SET_NO2(void)));
	connect(ui.capbDG645_3, SIGNAL(clicked()), this, SLOT(slot_SET_NO3(void)));
	connect(ui.capbDG645_4, SIGNAL(clicked()), this, SLOT(slot_SET_NO4(void)));
	connect(ui.caleDG645_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_NO5(void)));

	connect(ui.cadpFC_1, SIGNAL(valueChanged(double)), this, SLOT(slot_CPSFC_Desc(double)));
	connect(ui.cadpFC_2, SIGNAL(valueChanged(double)), this, SLOT(slot_APSFC_Desc(double)));
	connect(ui.cadpFC_3, SIGNAL(valueChanged(double)), this, SLOT(slot_BPSFC_Desc(double)));
	connect(ui.cadpFC_4, SIGNAL(valueChanged(double)), this, SLOT(slot_LCSFC_Desc(double)));
	connect(ui.cadpFC_5, SIGNAL(valueChanged(double)), this, SLOT(slot_SlowIntFC_Desc(double)));

	connect(ui.calbHV_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change1(bool)));
	connect(ui.calbHV_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change2(bool)));

	connect(ui.cadpVacFC_1, SIGNAL(valueChanged(double)), this, SLOT(slot_VacFC_Desc1(double)));
	connect(ui.cadpVacFC_2, SIGNAL(valueChanged(double)), this, SLOT(slot_VacFC_Desc2(double)));

	ui.lbDescFC_1->show();

	ui.cadpVGV1->hide();
	ui.cadpVGV2->hide();
	ui.cadpVGV3->hide();
	ui.cadpVGV4->hide();
	ui.cadpVGV5->hide();

	connect(ui.cadpVGV1, SIGNAL(valueChanged(double)), this, SLOT(slot_VGV1_Stat_Change1(double)));
	connect(ui.cadpVGV2, SIGNAL(valueChanged(double)), this, SLOT(slot_VGV2_Stat_Change1(double)));
	connect(ui.cadpVGV3, SIGNAL(valueChanged(double)), this, SLOT(slot_VGV3_Stat_Change1(double)));
	connect(ui.cadpVGV4, SIGNAL(valueChanged(double)), this, SLOT(slot_VGV4_Stat_Change1(double)));
	connect(ui.cadpVGV5, SIGNAL(valueChanged(double)), this, SLOT(slot_VGV5_Stat_Change1(double)));

	connect(ui.pbLTU_2, SIGNAL(clicked()), this, SLOT(slot_Send_LTU()));

	ui.cadpWG_1->hide();
	ui.cadpWG_2->hide();
	ui.cadpWG_3->hide();
	ui.cadpWG_4->hide();
	connect(ui.cadpWG_1, SIGNAL(valueChanged(double)), this, SLOT(slot_WG1_Stat_Change1(double)));
	connect(ui.cadpWG_2, SIGNAL(valueChanged(double)), this, SLOT(slot_WG2_Stat_Change1(double)));
	connect(ui.cadpWG_3, SIGNAL(valueChanged(double)), this, SLOT(slot_WG3_Stat_Change1(double)));
	connect(ui.cadpWG_4, SIGNAL(valueChanged(double)), this, SLOT(slot_WG4_Stat_Change1(double)));

	ui.cagpSTstat->hide();
	connect(ui.calbSTstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_ShotTrig_Stat_Change1(bool)));

	connect(ui.pbStanby_on, SIGNAL(clicked()), this, SLOT(slot_Stanby_On(void)));
	connect(ui.pbStanby_off, SIGNAL(clicked()), this, SLOT(slot_Stanby_Off(void)));
}


ECCD_Operation::~ECCD_Operation()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}


int ECCD_Operation::slot_TMP1_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_1->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_1->setFillColor(QColor("green"));
			ui.calbTMP_1->setPalette(Oppalette1);
			ui.calbTMP_1->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_1->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_1->setText((char*)OpTMPDesc[iStat]);
}

int ECCD_Operation::slot_TMP2_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_2->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_2->setFillColor(QColor("green"));
			ui.calbTMP_2->setPalette(Oppalette1);
			ui.calbTMP_2->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_2->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_2->setText((char*)OpTMPDesc[iStat]);
}

int ECCD_Operation::slot_TMP3_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_3->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_3->setFillColor(QColor("green"));
			ui.calbTMP_3->setPalette(Oppalette1);
			ui.calbTMP_3->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_3->setFillColor(QColor("red"));			
			break;
	}
	ui.calbTMP_3->setText((char*)OpTMPDesc[iStat]);
}

int ECCD_Operation::slot_TMP4_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_4->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_4->setFillColor(QColor("green"));
			ui.calbTMP_4->setPalette(Oppalette1);
			ui.calbTMP_4->setAutoFillBackground(true);
			ui.calbTMP_4->setLineWidth(1); 
			break;
		default:
			ui.calbTMP_4->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_4->setText((char*)OpTMPDesc[iStat]);
}

int ECCD_Operation::slot_TMP5_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 0;
	switch(iStat)
	{
		case 1:
			ui.calbTMP_5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbTMP_5->setPalette(Oppalette1);
			ui.calbTMP_5->setAutoFillBackground(true);
			ui.calbTMP_5->setLineWidth(1); 
			break;
		default:
			ui.calbTMP_5->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_5->setText((char*)OpTMP5Desc[iStat]);
}

void ECCD_Operation::slot_SET_NO1(void)
{
	SET_DATA(1);
}

void ECCD_Operation::slot_SET_NO2(void)
{
	SET_DATA(2);
}

void ECCD_Operation::slot_SET_NO3(void)
{
	SET_DATA(3);
}

void ECCD_Operation::slot_SET_NO4(void)
{
	SET_DATA(4);
}

void ECCD_Operation::slot_SET_NO5(void)
{
	SET_DATA(5);
}

void ECCD_Operation::SET_DATA(int iDATA)
{

	chid   searchID;
	int    istatus;

	ca_search ("EC1_DG_SET_NO", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}

void ECCD_Operation::slot_CPSFC_Desc(double dStat)
{
	int iFaultCode = 0;
	if (dStat < 0)
	{ 
		dStat == 0;
	}
	else
	{
		iFaultCode = int(dStat);
	}
	
	ui.lbDescFC_1->setText("");
	ui.lbDescFC_1->setText((char*)CPSDesc[iFaultCode]);

	if(iFaultCode == 0)
	{
		ui.lbDescFC_1->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescFC_1->setPalette(Oppalette3);
	}
	ui.lbDescFC_1->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_APSFC_Desc(double dStat)
{
	int iFaultCode = 0;
	if (dStat < 0)
	{ 
		dStat == 0;
	}
	else
	{
		iFaultCode = int(dStat);
	}
	ui.lbDescFC_2->setText("");
	ui.lbDescFC_2->setText((char*)APSDesc[iFaultCode]);

	if(iFaultCode == 0)
	{
		ui.lbDescFC_2->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescFC_2->setPalette(Oppalette3);
	}
	ui.lbDescFC_2->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_BPSFC_Desc(double dStat)
{
	int iFaultCode = 0;
	if (dStat < 0)
	{ 
		dStat == 0;
	}
	else
	{
		iFaultCode = int(dStat);
	}
	ui.lbDescFC_3->setText("");
	ui.lbDescFC_3->setText((char*)BPSDesc[iFaultCode]);

	if(iFaultCode == 0)
	{
		ui.lbDescFC_3->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescFC_3->setPalette(Oppalette3);
	}
	ui.lbDescFC_3->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_LCSFC_Desc(double dStat)
{
	int iFaultCode = 0;
	if (dStat < 0)
	{ 
		dStat == 0;
	}
	else
	{
		iFaultCode = int(dStat);
	}
	ui.lbDescFC_4->setText("");
	ui.lbDescFC_4->setText((char*)LCSDesc[iFaultCode]);

	if(iFaultCode == 0)
	{
		ui.lbDescFC_4->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescFC_4->setPalette(Oppalette3);
	}
	ui.lbDescFC_4->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_SlowIntFC_Desc(double dStat)
{
	int iFaultCode = 0;
	if (dStat < 0)
	{ 
		dStat == 0;
	}
	else
	{
		iFaultCode = int(dStat);
	}
	ui.lbDescFC_5->setText("");
	ui.lbDescFC_5->setText((char*)SlowIntDesc[iFaultCode]);

	if(iFaultCode == 0)
	{
		ui.lbDescFC_5->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescFC_5->setPalette(Oppalette3);
	}
	ui.lbDescFC_5->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_HV_Stat_Change1(bool bMode)
{
	bHVMode1 = bMode;
	ui.lbHV_3->hide();
	if(bMode == true)
	{
		ui.calbHV_1->setFillColor(QColor("red"));
		ui.calbHV_1->show();
	}
	else
	{
		ui.calbHV_1->setFillColor(QColor("white"));	
		ui.calbHV_1->hide();
	}
	if(bHVMode1 == false && bHVMode2 == false) ui.lbHV_3->show();
}


void ECCD_Operation::slot_HV_Stat_Change2(bool bMode)
{
	bHVMode2 = bMode;
	ui.lbHV_3->hide();
	if(bMode == true)
	{
		ui.calbHV_2->setPalette(Oppalette1);	//green
		ui.calbHV_2->show();
	}
	else
	{
		ui.calbHV_2->setPalette(Oppalette2);	//white
		ui.calbHV_2->hide();	
	}
	ui.calbHV_2->setAutoFillBackground(true); 
	if(bHVMode1 == false && bHVMode2 == false) ui.lbHV_3->show();
}

void ECCD_Operation::slot_VacFC_Desc1(double dStat)
{
	int iFaultCode = int(dStat);
	ui.lbDescVacFC_1->setText((char*)OpVacDesc[iFaultCode]);
	
	if(iFaultCode == 0)
	{
		ui.lbDescVacFC_1->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescVacFC_1->setPalette(Oppalette3);
	}
	ui.lbDescVacFC_1->setAutoFillBackground(true); 
	//ui.lbDescVacFC_1->setFrameShape(QFrame::Panel);
	//ui.lbDescVacFC_1->setFrameShadow(QFrame::Plain);
	//ui.lbDescVacFC_1->setLineWidth(5);
}

void ECCD_Operation::slot_VacFC_Desc2(double dStat)
{
	int iFaultCode = int(dStat);
	ui.lbDescVacFC_2->setText((char*)OpVacDesc[iFaultCode]);
	
	if(iFaultCode == 0)
	{
		ui.lbDescVacFC_2->setPalette(Oppalette1);
	} 
	else
	{
		ui.lbDescVacFC_2->setPalette(Oppalette3);
	}
	ui.lbDescVacFC_2->setAutoFillBackground(true); 
}

void ECCD_Operation::slot_VGV1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV1->setPalette(Oppalette1);
			ui.calbVGV1->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV1->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV1->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV1->setText((char*)OperGVDesc[iStat]);
}

void ECCD_Operation::slot_VGV2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV2->setPalette(Oppalette1);
			ui.calbVGV2->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV2->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV2->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV2->setText((char*)OperGVDesc[iStat]);
}

void ECCD_Operation::slot_VGV3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV3->setPalette(Oppalette1);
			ui.calbVGV3->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV3->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV3->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV3->setText((char*)OperGVDesc[iStat]);
}

void ECCD_Operation::slot_VGV4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
//printf("TEST iStat : %d\n", iStat);
	switch(iStat)
	{
		case 0:
			ui.calbVGV4->setPalette(Oppalette1);
			ui.calbVGV4->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV4->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV4->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV4->setText((char*)OperGVDesc[iStat]);
}

void ECCD_Operation::slot_VGV5_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
//printf("TEST iStat : %d\n", iStat);
	switch(iStat)
	{
		case 0:
			ui.calbVGV5->setPalette(Oppalette1);
			ui.calbVGV5->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV5->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV5->setText((char*)OperGVDesc[iStat]);
}


void ECCD_Operation::slot_WG1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_1->setText((char*)OperWGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_1->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_1->setPalette(Oppalette1);
			ui.calbWG_1->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_1->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_1->setText("ERROR");
	}
}

void ECCD_Operation::slot_WG2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_2->setText((char*)OperGVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_2->setPalette(Oppalette1);
			ui.calbWG_2->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbWG_2->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbWG_2->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_2->setText("ERROR");
	}
}

void ECCD_Operation::slot_WG3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_3->setText((char*)OperGVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_3->setPalette(Oppalette1);
			ui.calbWG_3->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbWG_3->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbWG_3->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_3->setText("ERROR");			
	}

}

void ECCD_Operation::slot_WG4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_4->setText((char*)OperWGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_4->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_4->setPalette(Oppalette1);
			ui.calbWG_4->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_4->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_4->setText("ERROR");
	}
}


void ECCD_Operation::slot_Send_LTU()
{
	chid   searchID;
	int    istatus;
	float  fltData, fltBPS, fltRFon, fltRFPulse;
	int	i, iData;
	float	fltWrite[10];
	float fltPSWrite[10] = {0.01, 0, 0.01, 0, 0, 0, 50, 0, 0, 0};

	//RF Pulse length get
	ca_search (ReadTagName[3], &searchID);
	istatus = ca_pend_io(0.1);
	ca_get(DBR_FLOAT, searchID, &fltData);
	istatus = ca_pend_io(0.1);
	fltRFPulse = fltData;
printf("RF Pulse length : %f\n", fltRFPulse);
	//CPS Pulse length put
	fltPSWrite[8] = fltRFPulse+0.01;

	//BPS,APS Start delay put
	for(i=0;i<9;i++)
	{
		ca_search (PSWriteTagName[i], &searchID);
		istatus = ca_pend_io(0.1);
		fltData = fltPSWrite[i];
printf("fltPSwrite%d : %f               %f\n", i, fltPSWrite[i], fltData);
		ca_put(DBR_FLOAT, searchID, &fltData);
		istatus = ca_pend_io(0.1);
	}

	//PowerSupply Send On
	ca_search ("EC1_PSPLC_ECMD_SEND", &searchID);
	istatus = ca_pend_io(0.1);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(1.0);

	//delay(1);	

	//PowerSupply Send Off
	ca_search ("EC1_PSPLC_ECMD_SEND", &searchID);
	istatus = ca_pend_io(0.1);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(1.0);

	//RF ON-TIME get
	ca_search (ReadTagName[2], &searchID);
	istatus = ca_pend_io(2.0);
	ca_get(DBR_FLOAT, searchID, &fltData);
	istatus = ca_pend_io(0.1);
	fltRFon = fltData;
printf("RFOn-time : %f\n", fltRFon);
	//BPS Start delay get
	ca_search (ReadTagName[0], &searchID);
	istatus = ca_pend_io(0.1);
	ca_get(DBR_FLOAT, searchID, &fltData);
	istatus = ca_pend_io(0.1);
	fltBPS = fltData;
printf("BPS Start delay : %f\n", fltBPS);

	fltBPS = 0.01;
	//Calulation Timming
	fltWrite[0] = fltRFon - (3+fltBPS) + 16;
	fltWrite[1] = fltWrite[0]+0.0001;

	fltWrite[2] = fltRFon - (fltBPS) + 16;
	fltWrite[3] = fltWrite[2]+0.0001;

	fltWrite[4] = 0;//fltRFon + 15;//0;//fltRFon - (1+3+fltBPS) + 16;
	fltWrite[5] = fltWrite[4]+0.1;

	fltWrite[6] = fltWrite[4];
	fltWrite[7] = fltWrite[5];
	
	fltWrite[8] = fltRFon + 15;
	fltWrite[9] = fltWrite[8]+0.1;
	
	for(i=0;i<10;i++)
	{
		ca_search (WriteTagName[i], &searchID);
		istatus = ca_pend_io(0.1);
		fltData = fltWrite[i];
printf("fltwrite%d : %f               %f\n", i, fltWrite[i], fltData);
		ca_put(DBR_FLOAT, searchID, &fltData);
		istatus = ca_pend_io(0.1);

	}
}

void ECCD_Operation::slot_ShotTrig_Stat_Change1(bool bMode)
{
/*
	if(bMode == true)
	{
		ui.cagpSTstat->setGeometry(419, 13, 20, 20);
	}
	else
	{
		ui.cagpSTstat->setGeometry(550, 13, 20, 20);

	}
*/
}

void ECCD_Operation::slot_Stanby_On(void)
{
	chid   searchID;
	int    istatus;
	int	i, iData;

	//CPS On Set
	ca_search ("EC1_PCPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);

	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

	//CPS On Unset
	ca_search ("EC1_PCPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);


	//APS On Set
	ca_search ("EC1_PAPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

	//APS On Unset
	ca_search ("EC1_PAPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);


	//BPS On Set
	ca_search ("EC1_PBPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

	//BPS On Unset
	ca_search ("EC1_PBPS_ECMD_STBY", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

}

void ECCD_Operation::slot_Stanby_Off(void)
{
	chid   searchID;
	int    istatus;
	int	i, iData;

	//CPS Off Set
	ca_search ("EC1_PCPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

	//CPS Off Unset
	ca_search ("EC1_PCPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);


	//APS Off Set
	ca_search ("EC1_PAPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(1.0);

	//APS Off Unset
	ca_search ("EC1_PAPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);


	//BPS Off Set
	ca_search ("EC1_PBPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 1;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

	//BPS Off Unset
	ca_search ("EC1_PBPS_ECMD_STBYOF", &searchID);
	istatus = ca_pend_io(dDelay);
	iData = 0;
	ca_put(DBR_INT, searchID, &iData);
	istatus = ca_pend_io(dDelay2);

}
