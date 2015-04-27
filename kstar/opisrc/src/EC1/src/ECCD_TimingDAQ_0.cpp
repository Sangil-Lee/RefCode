#include "ECCD_TimingDAQ_0.h"

ECCD_TimingDAQ_0::ECCD_TimingDAQ_0(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

      ECCDTIMINGDAQ_1 = new ECCD_TimingDAQ_1(this);
      ECCDTIMINGDAQ_1->setGeometry(160, 40, 950, 760);

      ECCDTIMINGDAQ_2 = new ECCD_TimingDAQ_2(this);
      ECCDTIMINGDAQ_2->setGeometry(160, 40, 950, 760);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDTIMINGDAQ_1_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDTIMINGDAQ_2_Show()));

	slot_ECCDTIMINGDAQ_1_Show();

}

ECCD_TimingDAQ_0::~ECCD_TimingDAQ_0()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_TimingDAQ_0::slot_ECCDTIMINGDAQ_1_Show(void){
	ui.label->setText("Timing of ECCD 1");
	ECCDTIMINGDAQ_2->hide();
	ECCDTIMINGDAQ_1->show();
	setpushButton(0);
} 

void ECCD_TimingDAQ_0::slot_ECCDTIMINGDAQ_2_Show(void){
	ui.label->setText("DAQ of ECCD 1");
	ECCDTIMINGDAQ_1->hide();
	ECCDTIMINGDAQ_2->show();
	setpushButton(1);
}

void ECCD_TimingDAQ_0::setpushButton(int inum){
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
