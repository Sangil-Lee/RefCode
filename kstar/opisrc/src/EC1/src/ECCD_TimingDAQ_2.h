#ifndef ECCD_TIMINGDAQ_2_H
#define ECCD_TIMINGDAQ_2_H

#include <QtGui/QWidget>
#include "ui_ECCD_TimingDAQ_2.h"
#include "qtchaccesslib.h"

class ECCD_TimingDAQ_2 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TimingDAQ_2(QWidget *parent = 0);
    ~ECCD_TimingDAQ_2();

private:
    Ui::ECCDTIMINGDAQ_2 ui;

    AttachChannelAccess *pattach;
};

#endif // ECCD_TimingDAQ_2_H
 

