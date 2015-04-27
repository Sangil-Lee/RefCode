#include "ECCD_Calorimetric.h"
#include <qwt_painter.h>

ECCD_Calorimetric::ECCD_Calorimetric(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.caleFlow_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_2, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_3, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_4, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_5, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_6, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_7, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_8, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_9, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.caleFlow_10, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
}

ECCD_Calorimetric::~ECCD_Calorimetric()
{
//	if(ui.label) {delete ui.label; ui.label = NULL; }
}

void ECCD_Calorimetric::slot_SET_DATA(void)
{
	chid   searchID;
	int    istatus;
	int	 iDATA = 1;
	
	ca_search ("EC1_WCS_SET_DATA", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}

