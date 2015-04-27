#ifndef ECCD_POWERSUPPLY_2_H
#define ECCD_POWERSUPPLY_2_H

#include <QtGui/QWidget>
#include "ui_ECCD_PowerSupply_2.h"
#include "qtchaccesslib.h"

class ECCD_PowerSupply_2 : public QWidget
{
    Q_OBJECT

public:
    ECCD_PowerSupply_2(QWidget *parent = 0);
    ~ECCD_PowerSupply_2();

private:
    Ui::ECCDPOWERSUPPLY_2 ui;

    AttachChannelAccess *pattach;
};

#endif // ECCD_PowerSupply_2_H
 

