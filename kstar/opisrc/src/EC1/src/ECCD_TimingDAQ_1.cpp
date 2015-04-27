#include "ECCD_TimingDAQ_1.h"

ECCD_TimingDAQ_1::ECCD_TimingDAQ_1(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.capbDG645_1, SIGNAL(clicked()), this, SLOT(slot_SET_NO1(void)));
	connect(ui.capbDG645_2, SIGNAL(clicked()), this, SLOT(slot_SET_NO2(void)));
	connect(ui.capbDG645_3, SIGNAL(clicked()), this, SLOT(slot_SET_NO3(void)));
	connect(ui.capbDG645_4, SIGNAL(clicked()), this, SLOT(slot_SET_NO4(void)));
	connect(ui.caleDG645_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_NO5(void)));
	
	connect(ui.capbDG645_5, SIGNAL(clicked()), this, SLOT(slot_SET_NO6(void)));

}

ECCD_TimingDAQ_1::~ECCD_TimingDAQ_1()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_TimingDAQ_1::slot_SET_NO1(void)
{
	SET_DATA(1);
}

void ECCD_TimingDAQ_1::slot_SET_NO2(void)
{
	SET_DATA(2);
}

void ECCD_TimingDAQ_1::slot_SET_NO3(void)
{
	SET_DATA(3);
}

void ECCD_TimingDAQ_1::slot_SET_NO4(void)
{
	SET_DATA(4);
}

void ECCD_TimingDAQ_1::slot_SET_NO5(void)
{
	SET_DATA(5);
}

void ECCD_TimingDAQ_1::slot_SET_NO6(void)
{
	SET_DATA(6);
}

void ECCD_TimingDAQ_1::SET_DATA(int iDATA)
{

	chid   searchID;
	int    istatus;

	ca_search ("EC1_DG_SET_NO", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
	

}
