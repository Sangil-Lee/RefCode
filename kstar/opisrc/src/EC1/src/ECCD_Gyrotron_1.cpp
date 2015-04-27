#include "ECCD_Gyrotron_1.h"

const char *LevelDesc[5] = {
"HOLD", "SET", "DOWN", "FLAT TOP(F.T)", "ZERO"
};

QPalette GR1palette1;
QPalette GR1palette2;
QPalette GR1palette3;

QBrush GR1fontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush GR1backbrush1(QColor(0, 150, 0, 255));		//background green

QBrush GR1fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush GR1backbrush2(QColor(255, 255, 255, 255));	//background white

QBrush GR1fontbrush3(QColor(255, 255, 255, 255));	//font white
QBrush GR1backbrush3(QColor(255, 0, 0, 255));	//background red

ECCD_Gyrotron_1::ECCD_Gyrotron_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.cadpMain_1, SIGNAL(valueChanged(double)), this, SLOT(slot_MMPS_Stat_Desc(double)));
	connect(ui.cadpGun1_1, SIGNAL(valueChanged(double)), this, SLOT(slot_GMPS1_Stat_Desc(double)));
	connect(ui.cadpGun2_1, SIGNAL(valueChanged(double)), this, SLOT(slot_GMPS2_Stat_Desc(double)));

	//set label text
	GR1fontbrush1.setStyle(Qt::SolidPattern);
	GR1palette1.setBrush(QPalette::Active, QPalette::WindowText, GR1fontbrush1);
	GR1palette1.setBrush(QPalette::Inactive, QPalette::WindowText, GR1fontbrush1);

	//set background
	GR1backbrush1.setStyle(Qt::SolidPattern);
	GR1palette1.setBrush(QPalette::Active, QPalette::Window, GR1backbrush1);
	GR1palette1.setBrush(QPalette::Inactive, QPalette::Window, GR1backbrush1);

	//set label text
	GR1palette2.setBrush(QPalette::Active, QPalette::WindowText, GR1fontbrush2);
	GR1palette2.setBrush(QPalette::Inactive, QPalette::WindowText, GR1fontbrush2);
		
	//set background
	GR1palette2.setBrush(QPalette::Active, QPalette::Window, GR1backbrush2);
	GR1palette2.setBrush(QPalette::Inactive, QPalette::Window, GR1backbrush2);

	//set label text
	GR1palette3.setBrush(QPalette::Active, QPalette::WindowText, GR1fontbrush3);
	GR1palette3.setBrush(QPalette::Inactive, QPalette::WindowText, GR1fontbrush3);
		
	//set background
	GR1palette3.setBrush(QPalette::Active, QPalette::Window, GR1backbrush3);
	GR1palette3.setBrush(QPalette::Inactive, QPalette::Window, GR1backbrush3);
}

ECCD_Gyrotron_1::~ECCD_Gyrotron_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_Gyrotron_1::slot_MMPS_Stat_Desc(double dStat)
{
	int iStatCode = int(dStat);
	
	ui.lbDescMMPS->setText("");
	ui.lbDescMMPS->setText((char*)LevelDesc[iStatCode]);

	if(iStatCode < 5 && iStatCode > -0.1)
	{
		ui.lbDescMMPS->setPalette(GR1palette1);
	} 
	else
	{
		ui.lbDescMMPS->setPalette(GR1palette3);
		ui.lbDescMMPS->setText("ERROR");
	}
	ui.lbDescMMPS->setAutoFillBackground(true); 
}

void ECCD_Gyrotron_1::slot_GMPS1_Stat_Desc(double dStat)
{
	int iStatCode = int(dStat);
	
	ui.lbDescGMPS1->setText("");
	ui.lbDescGMPS1->setText((char*)LevelDesc[iStatCode]);

	if(iStatCode < 5 && iStatCode > -0.1)
	{
		ui.lbDescGMPS1->setPalette(GR1palette1);
	} 
	else
	{
		ui.lbDescGMPS1->setPalette(GR1palette3);
		ui.lbDescGMPS1->setText("ERROR");
	}
	ui.lbDescGMPS1->setAutoFillBackground(true); 
}

void ECCD_Gyrotron_1::slot_GMPS2_Stat_Desc(double dStat)
{
	int iStatCode = int(dStat);
	
	ui.lbDescGMPS2->setText("");
	ui.lbDescGMPS2->setText((char*)LevelDesc[iStatCode]);

	if(iStatCode < 5 && iStatCode > -0.1)
	{
		ui.lbDescGMPS2->setPalette(GR1palette1);
	} 
	else
	{
		ui.lbDescGMPS2->setPalette(GR1palette3);
		ui.lbDescGMPS2->setText("ERROR");
	}
	ui.lbDescGMPS2->setAutoFillBackground(true); 
}
