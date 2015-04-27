#include "ECCD_Cooling.h"

ECCD_Cooling::ECCD_Cooling(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

/*
	connect(ui.caleTC_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTC_1, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
*/	

	connect(ui.caleTemp_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_2, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_3, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_4, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_5, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_6, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_7, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_8, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_9, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_10, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_11, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_12, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_13, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleTemp_14, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));

	connect(ui.capbTemp_1, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_2, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_3, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_4, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_5, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_6, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_7, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_8, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_9, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_10, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_11, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_12, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_13, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTemp_14, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));

	connect(ui.caleFR_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_2, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_3, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_4, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_5, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_6, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_7, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_8, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_9, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_10, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_11, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_12, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_13, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_14, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_15, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_16, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_17, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFR_18, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));

	connect(ui.capbFR_1, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_2, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_3, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_4, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_5, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_6, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_7, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_8, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_9, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_10, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_11, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_12, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_13, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_14, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_15, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_16, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_17, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbFR_18, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));

	//Flourinert Disable
	ui.cadpInTemp_15->hide();
	ui.cadpInTemp_16->hide();
	ui.cadpTemp_15->hide();
	ui.cadpTemp_16->hide();
	ui.calbTemp_65->hide();
	ui.calbTemp_66->hide();
	ui.caleTemp_13->hide();
	ui.caleTemp_14->hide();
	ui.capbTemp_13->hide();
	ui.capbTemp_14->hide();
	ui.cadpTemp_13->hide();
	ui.cadpInTemp_13->hide();
	ui.calbTemp_63->hide();
	ui.caleTemp_7->hide();
	ui.capbTemp_7->hide();
}

ECCD_Cooling::~ECCD_Cooling()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_Cooling::slot_SET_DATA(void)
{
	chid   searchID;
	int    istatus;
	int	 iDATA = 1;
	
	ca_search ("EC1_WCS_SET_DATA", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}

