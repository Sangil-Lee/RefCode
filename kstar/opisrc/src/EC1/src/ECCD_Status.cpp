#include "ECCD_Status.h"

ECCD_Status::ECCD_Status(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

      ECCDSTATUSIMGFULL = new ECCD_Status_ImgFull(this);
      ECCDSTATUSIMGFULL->setGeometry(40, 55, 1000, 700);
	ECCDSTATUSIMGFULL->show();
}

ECCD_Status::~ECCD_Status()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}
