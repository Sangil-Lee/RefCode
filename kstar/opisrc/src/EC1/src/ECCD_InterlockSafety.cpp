#include "ECCD_InterlockSafety.h"

const char *IntTMPDesc[8] = {
"ST0P", "WAITING INTERLOCK", "STARTING", "NORMAL OPERATION", "HIGHLOAD",
"   ", "FAILURE", "LOW SPEED"
};

const char *IntTMP5Desc[3] = {
"ST0P", "STARTING", "NORMAL OPERATION"
};

const char *IntGVDesc[3] = {
"OPEN", "CLOSE", "FAULT"
};

const char *IntWGDesc[3] = {
"To KSTAR", "To Dummy", "FAULT"
};

QPalette Intpalette1;
QPalette Intpalette2;
QBrush Intfontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush Intbackbrush1(QColor(0, 150, 0, 255));	//background green

QBrush Intfontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush Intbackbrush2(QColor(255, 255, 255, 255));	//background white

ECCD_InterlockSafety::ECCD_InterlockSafety(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

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

	connect(ui.capbTC_1, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));


	connect(ui.calbHV_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change1(bool)));
	connect(ui.calbHV_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change2(bool)));

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

	//set label text
	Intpalette1.setBrush(QPalette::Active, QPalette::WindowText, Intfontbrush1);
	Intpalette1.setBrush(QPalette::Inactive, QPalette::WindowText, Intfontbrush1);

	//set background
	Intpalette1.setBrush(QPalette::Active, QPalette::Window, Intbackbrush1);
	Intpalette1.setBrush(QPalette::Inactive, QPalette::Window, Intbackbrush1);

	//set label text
	Intpalette2.setBrush(QPalette::Active, QPalette::WindowText, Intfontbrush2);
	Intpalette2.setBrush(QPalette::Inactive, QPalette::WindowText, Intfontbrush2);
		
	//set background
	Intpalette2.setBrush(QPalette::Active, QPalette::Window, Intbackbrush2);
	Intpalette2.setBrush(QPalette::Inactive, QPalette::Window, Intbackbrush2);

	ui.cadpWG_1->hide();
	ui.cadpWG_2->hide();
	ui.cadpWG_3->hide();
	ui.cadpWG_4->hide();
	connect(ui.cadpWG_1, SIGNAL(valueChanged(double)), this, SLOT(slot_WG1_Stat_Change1(double)));
	connect(ui.cadpWG_2, SIGNAL(valueChanged(double)), this, SLOT(slot_WG2_Stat_Change1(double)));
	connect(ui.cadpWG_3, SIGNAL(valueChanged(double)), this, SLOT(slot_WG3_Stat_Change1(double)));
	connect(ui.cadpWG_4, SIGNAL(valueChanged(double)), this, SLOT(slot_WG4_Stat_Change1(double)));

}

ECCD_InterlockSafety::~ECCD_InterlockSafety()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

int ECCD_InterlockSafety::slot_TMP1_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_1->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.calbTMP_1->setPalette(Intpalette1);
			ui.calbTMP_1->setAutoFillBackground(true);
			break;
		default:
			ui.calbTMP_1->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_1->setText((char*)IntTMPDesc[iStat]);
}

int ECCD_InterlockSafety::slot_TMP2_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_2->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.calbTMP_2->setPalette(Intpalette1);
			ui.calbTMP_2->setAutoFillBackground(true);
			break;
		default:
			ui.calbTMP_2->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_2->setText((char*)IntTMPDesc[iStat]);
}


int ECCD_InterlockSafety::slot_TMP3_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_3->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.calbTMP_3->setPalette(Intpalette1);
			ui.calbTMP_3->setAutoFillBackground(true);
			break;
		default:
			ui.calbTMP_3->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_3->setText((char*)IntTMPDesc[iStat]);
}

int ECCD_InterlockSafety::slot_TMP4_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_4->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.calbTMP_4->setPalette(Intpalette1);
			ui.calbTMP_4->setAutoFillBackground(true);
			break;
		default:
			ui.calbTMP_4->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_4->setText((char*)IntTMPDesc[iStat]);
}

int ECCD_InterlockSafety::slot_TMP5_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 0;
	switch(iStat)
	{
		case 1:
			ui.calbTMP_5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbTMP_5->setPalette(Intpalette1);
			ui.calbTMP_5->setAutoFillBackground(true);
			break;
		default:
			ui.calbTMP_5->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_5->setText((char*)IntTMP5Desc[iStat]);
}


void ECCD_InterlockSafety::slot_SET_DATA(void)
{
	chid   searchID;
	int    istatus;
	int	 iDATA = 1;
	
	ca_search ("EC1_WCS_SET_DATA", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}


void ECCD_InterlockSafety::slot_HV_Stat_Change1(bool bMode)
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


void ECCD_InterlockSafety::slot_HV_Stat_Change2(bool bMode)
{
	bHVMode2 = bMode;
	ui.lbHV_3->hide();
	if(bMode == true)
	{
		ui.calbHV_2->setPalette(Intpalette1);	//green
		ui.calbHV_2->show();
	}
	else
	{
		ui.calbHV_2->setPalette(Intpalette2);	//white
		ui.calbHV_2->hide();	
	}
	ui.calbHV_2->setAutoFillBackground(true); 
	if(bHVMode1 == false && bHVMode2 == false) ui.lbHV_3->show();
}

void ECCD_InterlockSafety::slot_VGV1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV1->setPalette(Intpalette1);
			ui.calbVGV1->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV1->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV1->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV1->setText((char*)IntGVDesc[iStat]);
}

void ECCD_InterlockSafety::slot_VGV2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV2->setPalette(Intpalette1);
			ui.calbVGV2->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV2->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV2->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV2->setText((char*)IntGVDesc[iStat]);
}

void ECCD_InterlockSafety::slot_VGV3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV3->setPalette(Intpalette1);
			ui.calbVGV3->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV3->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV3->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV3->setText((char*)IntGVDesc[iStat]);
}

void ECCD_InterlockSafety::slot_VGV4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
//printf("TEST iStat : %d\n", iStat);
	switch(iStat)
	{
		case 0:
			ui.calbVGV4->setPalette(Intpalette1);
			ui.calbVGV4->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV4->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV4->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV4->setText((char*)IntGVDesc[iStat]);
}

void ECCD_InterlockSafety::slot_VGV5_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;

	switch(iStat)
	{
		case 0:
			ui.calbVGV5->setPalette(Intpalette1);
			ui.calbVGV5->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV5->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV5->setText((char*)IntGVDesc[iStat]);
}


void ECCD_InterlockSafety::slot_WG1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_1->setText((char*)IntWGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_1->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_1->setPalette(Intpalette1);
			ui.calbWG_1->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_1->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_1->setText("ERROR");
	}
}

void ECCD_InterlockSafety::slot_WG2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_2->setText((char*)IntGVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_2->setPalette(Intpalette1);
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

void ECCD_InterlockSafety::slot_WG3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_3->setText((char*)IntGVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_3->setPalette(Intpalette1);
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

void ECCD_InterlockSafety::slot_WG4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_4->setText((char*)IntWGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_4->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_4->setPalette(Intpalette1);
			ui.calbWG_4->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_4->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_4->setText("ERROR");
	}

}
