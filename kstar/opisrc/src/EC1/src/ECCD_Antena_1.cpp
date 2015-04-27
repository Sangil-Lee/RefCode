#include "ECCD_Antena_1.h"

const char *LanGVDesc[3] = {
"OPEN", "CLOSE", "FAULT"
};

QPalette Lanpalette1;
QBrush Lanfontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush Lanbackbrush1(QColor(0, 150, 0, 255));	//background green

ECCD_Antena_1::ECCD_Antena_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

      ECCDSTATUSIMG = new ECCD_Status_Img(this);
      ECCDSTATUSIMG->setGeometry(5, 15, 920, 380);
	ECCDSTATUSIMG->show();

	//set label text
	Lanpalette1.setBrush(QPalette::Active, QPalette::WindowText, Lanfontbrush1);
	Lanpalette1.setBrush(QPalette::Inactive, QPalette::WindowText, Lanfontbrush1);

	//set background
	Lanpalette1.setBrush(QPalette::Active, QPalette::Window, Lanbackbrush1);
	Lanpalette1.setBrush(QPalette::Inactive, QPalette::Window, Lanbackbrush1);

	ui.cadpWG_3->hide();
	connect(ui.cadpWG_3, SIGNAL(valueChanged(double)), this, SLOT(slot_WG3_Stat_Change1(double)));
}

ECCD_Antena_1::~ECCD_Antena_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}


void ECCD_Antena_1::slot_WG3_Stat_Change1(double dStatus)
{
	int iStat = int(dStatus);
	if(iStat < 0 && iStat > 2) iStat = 2;

	ui.calbWG_3->setText((char*)LanGVDesc[iStat]);
	switch(iStat)
	{
		case 0:
			ui.calbWG_3->setPalette(Lanpalette1);
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
