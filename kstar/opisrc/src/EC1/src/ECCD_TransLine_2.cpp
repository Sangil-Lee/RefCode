#include "ECCD_TransLine_2.h"

const char *TransTMPDesc[8] = {
"ST0P", "WAITING INTERLOCK", "STARTING", "NORMAL OPERATION", "HIGHLOAD",
"   ", "FAILURE", "LOW SPEED"
};

const char *TransGVDesc[3] = {
"OPEN", "CLOSE", "FAULT"
};

const char *TransTMP5Desc[3] = {
"ST0P", "STARTING", "NORMAL OPERATION"
};


QPalette TL2palette1;
QBrush TL2fontbrush1(QColor(255, 255, 255, 255));		//font white
QBrush TL2backbrush1(QColor(0, 150, 0, 255));		//background green

QPalette TL2palette2;
QBrush TL2fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush TL2backbrush2(QColor(255, 255, 255, 255));	//background white

ECCD_TransLine_2::ECCD_TransLine_2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
//	ECCDSTATUSIMG = new ECCD_Status_Img(this);
//	ECCDSTATUSIMG->setGeometry(185, 15, 920, 380);
//	ECCDSTATUSIMG->show();
printf("Trans2 Start!!!\n");

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
	TL2palette1.setBrush(QPalette::Active, QPalette::WindowText, TL2fontbrush1);
	TL2palette1.setBrush(QPalette::Inactive, QPalette::WindowText, TL2fontbrush1);

	//set background
	TL2palette1.setBrush(QPalette::Active, QPalette::Window, TL2backbrush1);
	TL2palette1.setBrush(QPalette::Inactive, QPalette::Window, TL2backbrush1);

	//set label text
	TL2palette2.setBrush(QPalette::Active, QPalette::WindowText, TL2fontbrush2);
	TL2palette2.setBrush(QPalette::Inactive, QPalette::WindowText, TL2fontbrush2);

	//set background
	TL2palette2.setBrush(QPalette::Active, QPalette::Window, TL2backbrush2);
	TL2palette2.setBrush(QPalette::Inactive, QPalette::Window, TL2backbrush2);
}

ECCD_TransLine_2::~ECCD_TransLine_2()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}


int ECCD_TransLine_2::slot_TMP1_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_1->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_1->setFillColor(QColor("green"));
			ui.calbTMP_1->setPalette(TL2palette1);
			ui.calbTMP_1->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_1->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_1->setText((char*)TransTMPDesc[iStat]);
}

int ECCD_TransLine_2::slot_TMP2_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0||iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_2->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_2->setFillColor(QColor("green"));
			ui.calbTMP_2->setPalette(TL2palette1);
			ui.calbTMP_2->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_2->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_2->setText((char*)TransTMPDesc[iStat]);
}


int ECCD_TransLine_2::slot_TMP3_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0||iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_3->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_3->setFillColor(QColor("green"));
			ui.calbTMP_3->setPalette(TL2palette1);
			ui.calbTMP_3->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_3->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_3->setText((char*)TransTMPDesc[iStat]);
}

int ECCD_TransLine_2::slot_TMP4_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.calbTMP_4->setFillColor(QColor("yellow"));
			break;
		case 3:
			//ui.calbTMP_4->setFillColor(QColor("green"));
			ui.calbTMP_4->setPalette(TL2palette1);
			ui.calbTMP_4->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_4->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_4->setText((char*)TransTMPDesc[iStat]);
}

int ECCD_TransLine_2::slot_TMP5_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 0;
	switch(iStat)
	{
		case 1:
			ui.calbTMP_5->setFillColor(QColor("yellow"));
			break;
		case 2:
			//ui.calbTMP_5->setFillColor(QColor("green"));
			ui.calbTMP_5->setPalette(TL2palette1);
			ui.calbTMP_5->setAutoFillBackground(true); 
			break;
		default:
			ui.calbTMP_5->setFillColor(QColor("red"));
			break;
	}
	ui.calbTMP_5->setText((char*)TransTMP5Desc[iStat]);
}


void ECCD_TransLine_2::slot_VGV1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV1->setPalette(TL2palette1);
			ui.calbVGV1->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV1->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV1->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV1->setText((char*)TransGVDesc[iStat]);
}

void ECCD_TransLine_2::slot_VGV2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV2->setPalette(TL2palette1);
			ui.calbVGV2->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV2->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV2->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV2->setText((char*)TransGVDesc[iStat]);
}

void ECCD_TransLine_2::slot_VGV3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV3->setPalette(TL2palette1);
			ui.calbVGV3->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV3->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV3->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV3->setText((char*)TransGVDesc[iStat]);
}

void ECCD_TransLine_2::slot_VGV4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
//printf("TEST iStat : %d\n", iStat);
	switch(iStat)
	{
		case 0:
			ui.calbVGV4->setPalette(TL2palette1);
			ui.calbVGV4->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV4->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV4->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV4->setText((char*)TransGVDesc[iStat]);
}

void ECCD_TransLine_2::slot_VGV5_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 || iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.calbVGV5->setPalette(TL2palette1);
			ui.calbVGV5->setAutoFillBackground(true); 
			break;
		case 1:
			ui.calbVGV5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.calbVGV5->setFillColor(QColor("red"));
			break;
	}
	ui.calbVGV5->setText((char*)TransGVDesc[iStat]);
}
