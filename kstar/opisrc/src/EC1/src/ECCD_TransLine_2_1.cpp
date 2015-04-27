#include "ECCD_TransLine_2_1.h"


ECCD_TransLine_2_1::ECCD_TransLine_2_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
	ECCDSTATUSIMG = new ECCD_Status_Img(this);
	ECCDSTATUSIMG->setGeometry(5, 15, 920, 380);
	ECCDSTATUSIMG->show();

}

ECCD_TransLine_2_1::~ECCD_TransLine_2_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

