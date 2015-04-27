#include "ECCD_PowerSupply_2.h"

ECCD_PowerSupply_2::ECCD_PowerSupply_2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
}

ECCD_PowerSupply_2::~ECCD_PowerSupply_2()
{
}

