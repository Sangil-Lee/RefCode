#include "ECCD_TransLine_5.h"

ECCD_TransLine_5::ECCD_TransLine_5(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
}

ECCD_TransLine_5::~ECCD_TransLine_5()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}
