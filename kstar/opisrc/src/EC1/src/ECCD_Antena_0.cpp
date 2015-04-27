#include "ECCD_Antena_0.h"

ECCD_Antena_0::ECCD_Antena_0(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
//	pattach = new AttachChannelAccess(this);

      ECCDANTENA_1 = new ECCD_Antena_1(this);
      ECCDANTENA_1->setGeometry(160, 40, 950, 760);
      ECCDANTENA_2 = new ECCD_Antena_2(this);
      ECCDANTENA_2->setGeometry(160, 40, 950, 760);
      
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDANTENA_1_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDANTENA_2_Show()));
	
	switch(iPage)
	{
	case 0:
		slot_ECCDANTENA_1_Show();
		break;
	case 1:
		slot_ECCDANTENA_2_Show();
		break;
	}
	iPage=0;
}

ECCD_Antena_0::~ECCD_Antena_0()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_Antena_0::slot_ECCDANTENA_1_Show(void){

	ui.label->setText("Launcher of ECCD 1");
	ECCDANTENA_2->hide();
	ECCDANTENA_1->show();
	setpushButton(0);	
}
 
void ECCD_Antena_0::slot_ECCDANTENA_2_Show(void){
	ui.label->setText("Temperature of ECCD 1");
	ECCDANTENA_1->hide();
	ECCDANTENA_2->show();
	setpushButton(1);
}
 
void ECCD_Antena_0::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);
	ui.pushButton->setFont(font);
	ui.pushButton_2->setFont(font);

	font.setBold(true);
	switch (inum)
	{
		case 0:
			ui.pushButton->setFont(font);	
			break;
		case 1:
			ui.pushButton_2->setFont(font);	
			break;
	}
}
