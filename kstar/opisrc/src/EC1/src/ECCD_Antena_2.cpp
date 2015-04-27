#include "ECCD_Antena_2.h"

ECCD_Antena_2::ECCD_Antena_2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.caleTC_1, SIGNAL(enteredValue(double)), this, SLOT(slot_SET_DATA(void)));
	connect(ui.capbTC_1, SIGNAL(clicked()), this, SLOT(slot_SET_DATA(void)));
}

ECCD_Antena_2::~ECCD_Antena_2()
{

}

void ECCD_Antena_2::slot_SET_DATA(void)
{
	chid   searchID;
	int    istatus;
	int	 iDATA = 1;
	
	ca_search ("EC1_WCS_SET_DATA", &searchID);
	istatus = ca_pend_io(0.1);
	ca_put(DBR_INT, searchID, &iDATA);
	istatus = ca_pend_io(0.1);
}
