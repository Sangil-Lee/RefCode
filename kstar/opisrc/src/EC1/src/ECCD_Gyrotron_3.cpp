#include "ECCD_Gyrotron_3.h"

ECCD_Gyrotron_3::ECCD_Gyrotron_3(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);
	connect(ui.calbGun2_9, SIGNAL(valueChanged(bool)), this, SLOT(slot_HPS_BYPASS_OK_CHANGE(bool)));
	connect(ui.calbGun2_Bypass_ESET, SIGNAL(valueChanged(bool)), this, SLOT(slot_HPS_BYPASS_ESET_CHANGE(bool)));
	connect(ui.calbGun2_Ready_OK, SIGNAL(valueChanged(bool)), this, SLOT(slot_HPS_READY_OK_CHANGE(bool)));
	connect(ui.cadpHeat_StartCTU, SIGNAL(valueChanged(bool)), this, SLOT(slot_HPS_CTU_CHANGE(bool)));
	ui.calbGun2_Bypass_ESET->hide();
	ui.calbGun2_Ready_OK->hide();
	ui.cadpHeat_StartCTU->hide();

//	connect(ui.calbTLCM_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change1(bool)));
//	connect(ui.calbTLCM_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change2(bool)));

//	ui.calbTLCM_1->hide();
//	ui.calbTLCM_2->hide();
	ui.calbGun2_9->hide();
}

ECCD_Gyrotron_3::~ECCD_Gyrotron_3()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}


void ECCD_Gyrotron_3::slot_HPS_BYPASS_OK_CHANGE(bool bMode)
{
	
	if(bMode == true)
	{
		ui.capbHeat_Bypass->show();
	}
	else
	{
		ui.capbHeat_Bypass->hide();
	}
}

void ECCD_Gyrotron_3::slot_HPS_BYPASS_ESET_CHANGE(bool bMode)
{
	bBypassMode = bMode;
	printf("bypass : %d,    ready : %d\n", bBypassMode, bReadyMode);
	if((bBypassMode == true) || (bReadyMode = false))
	{
		ui.capbHeat_Start_1->hide();
	}
	else
	{
		ui.capbHeat_Start_1->show();
	}
}

void ECCD_Gyrotron_3::slot_HPS_READY_OK_CHANGE(bool bMode)
{
	bReadyMode = bMode;
	printf("bypass : %d,    ready : %d\n", bBypassMode, bReadyMode);
	if((bBypassMode == true) || (bReadyMode = false))
	{
		ui.capbHeat_Start_1->hide();
	}
	else
	{
		ui.capbHeat_Start_1->show();
	}
}

void ECCD_Gyrotron_3::slot_HPS_CTU_CHANGE(bool bMode)
{
	if (bMode == false)
	{
//		ui.capbHeat_Current_Set->show();
	}
	else
	{
//		ui.capbHeat_Current_Set->hide();
	}
}
/*
void ECCD_Gyrotron_3::slot_TLCM_Stat_Change1(bool bGyro3Mode)
{

	if(bGyro3Mode == true)
	{
		ui.cadpHeat_3->hide();
	}
	else
	{
		ui.cadpHeat_3->show();
	}
	ui.calbTLCM_1->setAutoFillBackground(true);

}

void ECCD_Gyrotron_3::slot_TLCM_Stat_Change2(bool bGyro3Mode)
{

	if(bGyro3Mode == true)
	{
		ui.cadpHeat_3->show();
	}
	else
	{
		ui.cadpHeat_3->hide();
	}
	ui.calbTLCM_2->setAutoFillBackground(true); 

}
*/
