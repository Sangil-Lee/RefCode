#include "ECCD_Status_ImgFull.h"

ECCD_Status_ImgFull::ECCD_Status_ImgFull(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
	ui.caMode->hide();
	ui.caRFGV->hide();

	connect(ui.caMode, SIGNAL(valueChanged(bool)), this, SLOT(slot_MODECHANGE(bool)));
	connect(ui.caRFGV, SIGNAL(valueChanged(bool)), this, SLOT(slot_RFGVCHANGE(bool)));

	ui.caMode2->hide();
	ui.caRFGV2->hide();

	connect(ui.caMode2, SIGNAL(valueChanged(bool)), this, SLOT(slot_MODECHANGE2(bool)));
	connect(ui.caRFGV2, SIGNAL(valueChanged(bool)), this, SLOT(slot_RFGVCHANGE2(bool)));

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

	ui.lbMOU->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_MOU01.png")));
	ui.lbTank->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_tank.png")));
	ui.calbVC_1->show();
	ui.calbVC_2->show();
	ui.calbVC_3->show();

	ui.cadpWG_2->hide();
	ui.cadpWG_3->hide();
	connect(ui.cadpWG_2, SIGNAL(valueChanged(double)), this, SLOT(slot_WG2_Stat_Change1(double)));
	connect(ui.cadpWG_3, SIGNAL(valueChanged(double)), this, SLOT(slot_WG3_Stat_Change1(double)));
}

ECCD_Status_ImgFull::~ECCD_Status_ImgFull()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_Status_ImgFull::slot_MODECHANGE(bool bMode){
	bKMode = bMode;
	//if (bMode==true)
	if (bMode==false)	//2011-07-19 by son
	{    ui.lbLine01->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_line01_k.png")));
           ui.lbLine02->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_Full_line02_k.PNG")));
	      if (bRFGV==false)
		{ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher_o.png")));}
		else
		{ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher.png")));}
	}
	else
	{    ui.lbLine01->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_line01_d.png")));
	     ui.lbLine02->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_Full_line02_d.PNG")));
	     ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher.png")));
	}
}

void ECCD_Status_ImgFull::slot_MODECHANGE2(bool bMode){
	bKMode2 = bMode;
	//if (bMode==true)
	if (bMode==false)	//2011-07-19 by son
	{    ui.lbLine01_1->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_line01_1_k.png")));
	     ui.lbLine02_1->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_Full_line02_1_k.png")));

	}
	else
	{    ui.lbLine01_1->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_line01_1_d.png")));
	     ui.lbLine02_1->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_Full_line02_1_d.png")));
	}
}

void ECCD_Status_ImgFull::slot_RFGVCHANGE(bool bMode){
 	bRFGV = bMode;
	if (bMode==false)
	{    if (bKMode==true)
		{ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher_o.png")));}
		else
		{ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher.png")));}
	}
	else
	{    ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher.png")));
	}
}

void ECCD_Status_ImgFull::slot_RFGVCHANGE2(bool bMode){
 	bRFGV2 = bMode;
}


int ECCD_Status_ImgFull::slot_TMP1_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.caGrpTMP1->setFillColor(QColor("yellow"));
			ui.caGrpVP1->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.caGrpTMP1->setFillColor(QColor("green"));
			ui.caGrpVP1->setFillColor(QColor("green"));
			break;
		default:
			ui.caGrpTMP1->setFillColor(QColor("red"));
			ui.caGrpVP1->setFillColor(QColor("red"));
			break;
	}
}

int ECCD_Status_ImgFull::slot_TMP2_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.caGrpTMP2->setFillColor(QColor("yellow"));
			ui.caGrpVP2->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.caGrpTMP2->setFillColor(QColor("green"));
			ui.caGrpVP2->setFillColor(QColor("green"));
			break;
		default:
			ui.caGrpTMP2->setFillColor(QColor("red"));
			ui.caGrpVP2->setFillColor(QColor("red"));
			break;
	}
	
}

int ECCD_Status_ImgFull::slot_TMP3_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.caGrpTMP3->setFillColor(QColor("yellow"));
			ui.caGrpVP3->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.caGrpTMP3->setFillColor(QColor("green"));
			ui.caGrpVP3->setFillColor(QColor("green"));
			break;
		default:
			ui.caGrpTMP3->setFillColor(QColor("red"));
			ui.caGrpVP3->setFillColor(QColor("red"));
			break;
	}
}

int ECCD_Status_ImgFull::slot_TMP4_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 7) iStat = 0;
	switch(iStat)
	{
		case 2:
			ui.caGrpTMP4->setFillColor(QColor("yellow"));
			ui.caGrpVP4->setFillColor(QColor("yellow"));
			break;
		case 3:
			ui.caGrpTMP4->setFillColor(QColor("green"));
			ui.caGrpVP4->setFillColor(QColor("green"));
			break;
		default:
			ui.caGrpTMP4->setFillColor(QColor("red"));
			ui.caGrpVP4->setFillColor(QColor("red"));
			break;
	}
}

int ECCD_Status_ImgFull::slot_TMP5_Stat_Desc(double dStatus){
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 0;
	switch(iStat)
	{
		case 1:
			ui.caGrpTMP5->setFillColor(QColor("yellow"));
			ui.caGrpVP5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpTMP5->setFillColor(QColor("green"));
			ui.caGrpVP5->setFillColor(QColor("green"));
			break;
		default:
			ui.caGrpTMP5->setFillColor(QColor("red"));
			ui.caGrpVP5->setFillColor(QColor("red"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_VGV1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.caGrpGV1->setFillColor(QColor("green"));
			break;
		case 1:
			ui.caGrpGV1->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpGV1->setFillColor(QColor("red"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_VGV2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.caGrpGV2->setFillColor(QColor("green"));
			break;
		case 1:
			ui.caGrpGV2->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpGV2->setFillColor(QColor("red"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_VGV3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.caGrpGV3->setFillColor(QColor("green"));
			break;
		case 1:
			ui.caGrpGV3->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpGV3->setFillColor(QColor("red"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_VGV4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.caGrpGV4->setFillColor(QColor("green"));
			break;
		case 1:
			ui.caGrpGV4->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpGV4->setFillColor(QColor("red"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_VGV5_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;
	switch(iStat)
	{
		case 0:
			ui.caGrpGV5->setFillColor(QColor("green"));
			break;
		case 1:
			ui.caGrpGV5->setFillColor(QColor("yellow"));
			break;
		case 2:
			ui.caGrpGV5->setFillColor(QColor("red"));
			break;
	}

}

void ECCD_Status_ImgFull::slot_WG2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	switch(iStat)
	{
		//open
		case 0:
			ui.calbRFOpen_1->setFillColor(QColor("red"));
			ui.calbRFOpen_2->setFillColor(QColor("red"));
			ui.calbRFClose_1->setFillColor(QColor("lightGray"));
			ui.calbRFClose_2->setFillColor(QColor("lightGray"));
			ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher_o.png")));
			break;
		
		//close
		case 1:
			ui.calbRFOpen_1->setFillColor(QColor("lightGray"));
			ui.calbRFOpen_2->setFillColor(QColor("lightGray"));
			ui.calbRFClose_1->setFillColor(QColor("green"));
			ui.calbRFClose_2->setFillColor(QColor("green"));
			ui.lbslauncher->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_s_launcher.png")));
			break;
		
		//Fault
		case 2:
			ui.calbRFOpen_1->setFillColor(QColor("lightGray"));
			ui.calbRFOpen_2->setFillColor(QColor("lightGray"));
			ui.calbRFClose_1->setFillColor(QColor("lightGray"));
			ui.calbRFClose_2->setFillColor(QColor("lightGray"));
			break;
	}
}

void ECCD_Status_ImgFull::slot_WG3_Stat_Change1(double dStatus)
{

	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	switch(iStat)
	{
		//open
		case 0:
			ui.calbShutOpen_1->setFillColor(QColor("red"));
			ui.calbShutOpen_2->setFillColor(QColor("red"));
			ui.calbShutClose_1->setFillColor(QColor("lightGray"));
			ui.calbShutClose_2->setFillColor(QColor("lightGray"));
			break;
		
		//close
		case 1:
			ui.calbShutOpen_1->setFillColor(QColor("lightGray"));
			ui.calbShutOpen_2->setFillColor(QColor("lightGray"));
			ui.calbShutClose_1->setFillColor(QColor("green"));
			ui.calbShutClose_2->setFillColor(QColor("green"));
			break;

		//Fault
		case 2:
			ui.calbShutOpen_1->setFillColor(QColor("lightGray"));
			ui.calbShutOpen_2->setFillColor(QColor("lightGray"));
			ui.calbShutClose_1->setFillColor(QColor("lightGray"));
			ui.calbShutClose_2->setFillColor(QColor("lightGray"));
			break;
	}

}

