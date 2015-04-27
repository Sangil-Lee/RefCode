#include "ECCD_TransLine_1.h"

const char *TL1GVDesc[3] = {
"OPEN", "CLOSE", "FAULT"
};

const char *TL1WGDesc[3] = {
"To KSTAR", "To Dummy", "FAULT"
};

QPalette TL1palette1;
QPalette TL1palette2;
QPalette TL1palette3;

QBrush TL1fontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush TL1backbrush1(QColor(0, 170, 0, 255));		//background green

QBrush TL1fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush TL1backbrush2(QColor(255, 255, 255, 255));	//background white

QBrush TL1fontbrush3(QColor(255, 255, 255, 255));	//font white
QBrush TL1backbrush3(QColor(255, 0, 0, 255));	//background red

ECCD_TransLine_1::ECCD_TransLine_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
      ECCDSTATUSIMG = new ECCD_Status_Img(this);
      ECCDSTATUSIMG->setGeometry(5, 15, 920, 380);
	ECCDSTATUSIMG->show();

	//set label text
	TL1fontbrush1.setStyle(Qt::SolidPattern);
	TL1palette1.setBrush(QPalette::Active, QPalette::WindowText, TL1fontbrush1);
	TL1palette1.setBrush(QPalette::Inactive, QPalette::WindowText, TL1fontbrush1);

	//set background
	TL1backbrush1.setStyle(Qt::SolidPattern);
	TL1palette1.setBrush(QPalette::Active, QPalette::Window, TL1backbrush1);
	TL1palette1.setBrush(QPalette::Inactive, QPalette::Window, TL1backbrush1);

	//set label text
	TL1palette2.setBrush(QPalette::Active, QPalette::WindowText, TL1fontbrush2);
	TL1palette2.setBrush(QPalette::Inactive, QPalette::WindowText, TL1fontbrush2);
		
	//set background
	TL1palette2.setBrush(QPalette::Active, QPalette::Window, TL1backbrush2);
	TL1palette2.setBrush(QPalette::Inactive, QPalette::Window, TL1backbrush2);

	//set label text
	TL1palette3.setBrush(QPalette::Active, QPalette::WindowText, TL1fontbrush3);
	TL1palette3.setBrush(QPalette::Inactive, QPalette::WindowText, TL1fontbrush3);
		
	//set background
	TL1palette3.setBrush(QPalette::Active, QPalette::Window, TL1backbrush3);
	TL1palette3.setBrush(QPalette::Inactive, QPalette::Window, TL1backbrush3);

	ui.cadpWG_1->hide();
	ui.cadpWG_2->hide();
	ui.cadpWG_3->hide();
	ui.cadpWG_4->hide();
	connect(ui.cadpWG_1, SIGNAL(valueChanged(double)), this, SLOT(slot_WG1_Stat_Change1(double)));
	connect(ui.cadpWG_2, SIGNAL(valueChanged(double)), this, SLOT(slot_WG2_Stat_Change1(double)));
	connect(ui.cadpWG_3, SIGNAL(valueChanged(double)), this, SLOT(slot_WG3_Stat_Change1(double)));
	connect(ui.cadpWG_4, SIGNAL(valueChanged(double)), this, SLOT(slot_WG4_Stat_Change1(double)));
}

ECCD_TransLine_1::~ECCD_TransLine_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_TransLine_1::slot_WG1_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_1->setText((char*)TL1WGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_1->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_1->setPalette(TL1palette1);
			ui.calbWG_1->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_1->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_1->setText("ERROR");
	}
}

void ECCD_TransLine_1::slot_WG2_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_2->setText((char*)TL1GVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_2->setPalette(TL1palette1);
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

void ECCD_TransLine_1::slot_WG3_Stat_Change1(double dStatus)
{

	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_3->setText((char*)TL1GVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_3->setPalette(TL1palette1);
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

void ECCD_TransLine_1::slot_WG4_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_4->setText((char*)TL1WGDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_4->setFillColor(QColor("yellow"));
			break;
		case 1:
			ui.calbWG_4->setPalette(TL1palette1);
			ui.calbWG_4->setAutoFillBackground(true); 
			break;
		case 2:
			ui.calbWG_4->setFillColor(QColor("red"));
			break;
		default:
			ui.calbWG_4->setText("ERROR");
	}
}

