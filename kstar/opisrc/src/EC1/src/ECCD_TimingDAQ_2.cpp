#include "ECCD_TimingDAQ_2.h"

ECCD_TimingDAQ_2::ECCD_TimingDAQ_2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
}

ECCD_TimingDAQ_2::~ECCD_TimingDAQ_2()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}
