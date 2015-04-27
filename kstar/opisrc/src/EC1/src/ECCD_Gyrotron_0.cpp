#include "ECCD_Gyrotron_0.h"

int iPage;

ECCD_Gyrotron_0::ECCD_Gyrotron_0(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	ECCDGYROTRON_1 = new ECCD_Gyrotron_1(this);
	ECCDGYROTRON_1->setGeometry(160, 40, 950, 760);
	ECCDGYROTRON_2 = new ECCD_Gyrotron_2(this);
	ECCDGYROTRON_2->setGeometry(160, 40, 950, 760);
	ECCDGYROTRON_3 = new ECCD_Gyrotron_3(this);
	ECCDGYROTRON_3->setGeometry(160, 40, 950, 760);
	ECCDGYROTRON_4 = new ECCD_Gyrotron_4(this);
	ECCDGYROTRON_4->setGeometry(160, 40, 950, 760);

	connect(ui.pushButton,   SIGNAL(clicked()), this, SLOT(slot_ECCDGYROTRON_1_Show()));
	connect(ui.pushButton_1, SIGNAL(clicked()), this, SLOT(slot_ECCDGYROTRON_2_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDGYROTRON_3_Show()));
	connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(slot_ECCDGYROTRON_4_Show()));

	switch(iPage)
	{
	case 0:
		slot_ECCDGYROTRON_1_Show();
		break;
	case 1:
		slot_ECCDGYROTRON_2_Show();
		break;
	case 2:
		slot_ECCDGYROTRON_3_Show();
		break;
	case 3:
		slot_ECCDGYROTRON_4_Show();
		break;
	}
	iPage=0;
	
}

ECCD_Gyrotron_0::~ECCD_Gyrotron_0()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

// slot Define
void ECCD_Gyrotron_0::slot_ECCDGYROTRON_1_Show(void){
	ui.label->setText("Magnet P/S of ECCD 1");
	ECCDGYROTRON_2->hide();
	ECCDGYROTRON_3->hide();
	ECCDGYROTRON_4->hide();
	ECCDGYROTRON_1->show();
	setpushButton(0);
}
 
void ECCD_Gyrotron_0::slot_ECCDGYROTRON_2_Show(void){
	ui.label->setText("Collector P/S of ECCD 1");
	ECCDGYROTRON_1->hide();
	ECCDGYROTRON_3->hide();
	ECCDGYROTRON_4->hide();
	ECCDGYROTRON_2->show();
	setpushButton(1);
}

void ECCD_Gyrotron_0::slot_ECCDGYROTRON_3_Show(void){
	ui.label->setText("Heater P/S of ECCD 1");
	ECCDGYROTRON_1->hide();
	ECCDGYROTRON_2->hide();
	ECCDGYROTRON_4->hide();
	ECCDGYROTRON_3->show();
	setpushButton(2);
}

void ECCD_Gyrotron_0::slot_ECCDGYROTRON_4_Show(void){
	ui.label->setText("Coll. surface temp of ECCD 1");
	ECCDGYROTRON_1->hide();
	ECCDGYROTRON_2->hide();
	ECCDGYROTRON_3->hide();
	ECCDGYROTRON_4->show();
	setpushButton(3);
}


void ECCD_Gyrotron_0::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);
	ui.pushButton->setFont(font);
	ui.pushButton_1->setFont(font);
	ui.pushButton_2->setFont(font);
	ui.pushButton_3->setFont(font);
	font.setBold(true);
	switch (inum)
	{
		case 0:
			ui.pushButton->setFont(font);	
			break;
		case 1:
			ui.pushButton_1->setFont(font);	
			break;
		case 2:
			ui.pushButton_2->setFont(font);	
			break;
		case 3:
			ui.pushButton_3->setFont(font);	
			break;
	}
}
