#ifndef ECCD_TIMINGDAQ_1_H
#define ECCD_TIMINGDAQ_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_TimingDAQ_1.h"
#include "qtchaccesslib.h"
//#include <cadef.h>

class ECCD_TimingDAQ_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TimingDAQ_1(QWidget *parent = 0);
    ~ECCD_TimingDAQ_1();

private slots:
	void slot_SET_NO1(void);
	void slot_SET_NO2(void);
	void slot_SET_NO3(void);
	void slot_SET_NO4(void);
	void slot_SET_NO5(void);
	void slot_SET_NO6(void);

private:
    Ui::ECCDTIMINGDAQ_1 ui;

    AttachChannelAccess *pattach;
	void SET_DATA(int);
};


#endif // ECCD_TimingDAQ_1_H
 

