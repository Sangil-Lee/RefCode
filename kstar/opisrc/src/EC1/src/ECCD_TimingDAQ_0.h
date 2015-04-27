#ifndef ECCD_TIMINGDAQ_0_H
#define ECCD_TIMINGDAQ_0_H

#include <QtGui/QWidget>
#include "ui_ECCD_TimingDAQ_0.h"
#include "qtchaccesslib.h"

// Sub Header File Include
#include "ECCD_TimingDAQ_1.h"
#include "ECCD_TimingDAQ_2.h"

class ECCD_TimingDAQ_0 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TimingDAQ_0(QWidget *parent = 0);
    ~ECCD_TimingDAQ_0();

// slot define
private slots:
	void slot_ECCDTIMINGDAQ_1_Show(void);
	void slot_ECCDTIMINGDAQ_2_Show(void);
	void setpushButton(int);

private:
    Ui::ECCDTIMINGDAQ_0 ui;

    AttachChannelAccess *pattach;

// Sub Form Class define   
    	ECCD_TimingDAQ_1		*ECCDTIMINGDAQ_1;
    	ECCD_TimingDAQ_2		*ECCDTIMINGDAQ_2;

};

#endif // ECCD_TimingDAQ_0_H
 

