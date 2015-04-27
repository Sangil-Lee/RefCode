#include "ECCD_PowerSupply_1.h"

QPalette PS1palette1;
QPalette PS1palette2;
QBrush PS1fontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush PS1backbrush1(QColor(0, 170, 0, 255));		//background green

QBrush PS1fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush PS1backbrush2(QColor(255, 255, 255, 255));	//background white

ECCD_PowerSupply_1::ECCD_PowerSupply_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.capbDG645_1, SIGNAL(clicked()), this, SLOT(slot_SET_NO1(void)));
	connect(ui.capbDG645_2, SIGNAL(clicked()), this, SLOT(slot_SET_NO2(void)));
	connect(ui.capbDG645_3, SIGNAL(clicked()), this, SLOT(slot_SET_NO3(void)));
	connect(ui.capbDG645_4, SIGNAL(clicked()), this, SLOT(slot_SET_NO4(void)));
	connect(ui.caleDG645_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_NO5(void)));

	connect(ui.calbHV_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change1(bool)));
	connect(ui.calbHV_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_HV_Stat_Change2(bool)));

	//set label text
	PS1palette1.setBrush(QPalette::Active, QPalette::WindowText, PS1fontbrush1);
	PS1palette1.setBrush(QPalette::Inactive, QPalette::WindowText, PS1fontbrush1);

	//set background
	PS1palette1.setBrush(QPalette::Active, QPalette::Window, PS1backbrush1);
	PS1palette1.setBrush(QPalette::Inactive, QPalette::Window, PS1backbrush1);

	//set label text
	PS1palette2.setBrush(QPalette::Active, QPalette::WindowText, PS1fontbrush2);
	PS1palette2.setBrush(QPalette::Inactive, QPalette::WindowText, PS1fontbrush2);
		
	//set background
	PS1palette2.setBrush(QPalette::Active, QPalette::Window, PS1backbrush2);
	PS1palette2.setBrush(QPalette::Inactive, QPalette::Window, PS1backbrush2);
}

ECCD_PowerSupply_1::~ECCD_PowerSupply_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}


void ECCD_PowerSupply_1::slot_SET_NO1(void)
{
	SET_DATA(1);
}

void ECCD_PowerSupply_1::slot_SET_NO2(void)
{
	SET_DATA(2);
}

void ECCD_PowerSupply_1::slot_SET_NO3(void)
{
	SET_DATA(3);
}

void ECCD_PowerSupply_1::slot_SET_NO4(void)
{
	SET_DATA(4);
}

void ECCD_PowerSupply_1::slot_SET_NO5(void)
{
	SET_DATA(5);
}

void ECCD_PowerSupply_1::SET_DATA(int iDATA)
{

	chid   searchID;
	int    istatus;

	ca_search ("EC1_DG_SET_NO", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}

void ECCD_PowerSupply_1::slot_HV_Stat_Change1(bool bMode)
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


void ECCD_PowerSupply_1::slot_HV_Stat_Change2(bool bMode)
{
	bHVMode2 = bMode;
	ui.lbHV_3->hide();
	if(bMode == true)
	{
		ui.calbHV_2->setPalette(PS1palette1);	//green
		ui.calbHV_2->show();
	}
	else
	{
		ui.calbHV_2->setPalette(PS1palette2);	//white
		ui.calbHV_2->hide();	
	}
	ui.calbHV_2->setAutoFillBackground(true); 
	if(bHVMode1 == false && bHVMode2 == false) ui.lbHV_3->show();
}

