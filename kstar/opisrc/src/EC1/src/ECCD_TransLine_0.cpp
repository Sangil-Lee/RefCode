#include "ECCD_TransLine_0.h"

ECCD_TransLine_0::ECCD_TransLine_0(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
//	pattach = new AttachChannelAccess(this);

      ECCDTRANSLINE_1 = new ECCD_TransLine_1(this);
      ECCDTRANSLINE_1->setGeometry(160, 40, 950, 760);
      ECCDTRANSLINE_2 = new ECCD_TransLine_2(this);
      ECCDTRANSLINE_2->setGeometry(0, 450, 1100, 300);
      ECCDTRANSLINE_2_1 = new ECCD_TransLine_2_1(this);
      ECCDTRANSLINE_2_1->setGeometry(160, 40, 950, 450);

      ECCDTRANSLINE_3 = new ECCD_TransLine_3(this);
      ECCDTRANSLINE_3->setGeometry(160, 40, 950, 760);
      ECCDTRANSLINE_4 = new ECCD_TransLine_4(this);
      ECCDTRANSLINE_4->setGeometry(160, 40, 950, 760);
      ECCDTRANSLINE_5 = new ECCD_TransLine_5(this);
      ECCDTRANSLINE_5->setGeometry(160, 40, 950, 760);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_1_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_2_Show()));
	connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_3_Show()));
	connect(ui.pushButton_4, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_4_Show()));
	connect(ui.pushButton_5, SIGNAL(clicked()), this, SLOT(slot_ECCDTRANSLINE_5_Show()));

	switch(iPage)
	{
	case 0:
		slot_ECCDTRANSLINE_1_Show();
		break;
	case 1:
		slot_ECCDTRANSLINE_2_Show();
		break;
	case 2:
		slot_ECCDTRANSLINE_3_Show();
		break;
	case 3:
		slot_ECCDTRANSLINE_4_Show();
		break;
	case 4:
		slot_ECCDTRANSLINE_5_Show();
		break;

	}
	iPage=0;
}

ECCD_TransLine_0::~ECCD_TransLine_0()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_TransLine_0::slot_ECCDTRANSLINE_1_Show(void){

	ui.label->setText("W/G S/W, RF G/V of ECCD 1");
	ECCDTRANSLINE_2->hide();
	ECCDTRANSLINE_3->hide();
	ECCDTRANSLINE_4->hide();
	ECCDTRANSLINE_5->hide();
	ECCDTRANSLINE_2_1->hide();
	ECCDTRANSLINE_1->show();
	setpushButton(0);	
}
 
void ECCD_TransLine_0::slot_ECCDTRANSLINE_2_Show(void){
	ui.label->setText("Vacuum of ECCD 1");
	ECCDTRANSLINE_1->hide();
	ECCDTRANSLINE_3->hide();
	ECCDTRANSLINE_4->hide();
	ECCDTRANSLINE_5->hide();
	ECCDTRANSLINE_2->show();
	ECCDTRANSLINE_2_1->show();
	setpushButton(1);
}
 
void ECCD_TransLine_0::slot_ECCDTRANSLINE_3_Show(void){
	ui.label->setText("Polarization of ECCD 1");
	ECCDTRANSLINE_1->hide();
	ECCDTRANSLINE_2->hide();
	ECCDTRANSLINE_4->hide();
	ECCDTRANSLINE_5->hide();
	ECCDTRANSLINE_3->show();
	ECCDTRANSLINE_2_1->hide();
	setpushButton(2);
}

void ECCD_TransLine_0::slot_ECCDTRANSLINE_4_Show(void){
	ui.label->setText("TC of ECCD 1");
	ECCDTRANSLINE_1->hide();
	ECCDTRANSLINE_2->hide();
	ECCDTRANSLINE_3->hide();
	ECCDTRANSLINE_5->hide();
	ECCDTRANSLINE_4->show();
	ECCDTRANSLINE_2_1->hide();
	setpushButton(3);
}

void ECCD_TransLine_0::slot_ECCDTRANSLINE_5_Show(void){
	ui.label->setText("Dummy2 of ECCD 1");
	ECCDTRANSLINE_1->hide();
	ECCDTRANSLINE_2->hide();
	ECCDTRANSLINE_3->hide();
	ECCDTRANSLINE_4->hide();
	ECCDTRANSLINE_5->show();
	ECCDTRANSLINE_2_1->hide();
	setpushButton(4);
}

void ECCD_TransLine_0::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);
	ui.pushButton->setFont(font);
	ui.pushButton_2->setFont(font);
	ui.pushButton_3->setFont(font);
	ui.pushButton_4->setFont(font);
	ui.pushButton_5->setFont(font);
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
	}
}
