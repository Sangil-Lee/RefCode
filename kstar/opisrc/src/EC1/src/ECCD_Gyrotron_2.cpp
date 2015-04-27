#include "ECCD_Gyrotron_2.h"

ECCD_Gyrotron_2::ECCD_Gyrotron_2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
}

ECCD_Gyrotron_2::~ECCD_Gyrotron_2()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}
