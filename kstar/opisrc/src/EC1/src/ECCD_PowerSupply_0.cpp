#include "ECCD_PowerSupply_0.h"

ECCD_PowerSupply_0::ECCD_PowerSupply_0(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

      ECCDPOWERSUPPLY_1 = new ECCD_PowerSupply_1(this);
      ECCDPOWERSUPPLY_1->setGeometry(160, 40, 950, 760);
      ECCDPOWERSUPPLY_2 = new ECCD_PowerSupply_2(this);
      ECCDPOWERSUPPLY_2->setGeometry(160, 40, 950, 760);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDPOWERSUPPLY_2_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDPOWERSUPPLY_1_Show()));

	slot_ECCDPOWERSUPPLY_2_Show();
}

ECCD_PowerSupply_0::~ECCD_PowerSupply_0()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_PowerSupply_0::slot_ECCDPOWERSUPPLY_1_Show(void){
	ui.label->setText("H.V. P/S of ECCD 1");
	setpushButton(1);
	ECCDPOWERSUPPLY_2->hide();
	ECCDPOWERSUPPLY_1->show();
}
 
void ECCD_PowerSupply_0::slot_ECCDPOWERSUPPLY_2_Show(void){
	ui.label->setText("VCB of ECCD 1");
	setpushButton(0);
	ECCDPOWERSUPPLY_1->hide();
	ECCDPOWERSUPPLY_2->show();
}

void ECCD_PowerSupply_0::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);
	if(inum==0)
	{
		ui.pushButton_2->setFont(font);
		font.setBold(true);
		ui.pushButton->setFont(font);
	}
	else
	{
		ui.pushButton->setFont(font);
		font.setBold(true);
		ui.pushButton_2->setFont(font);
	}
}
