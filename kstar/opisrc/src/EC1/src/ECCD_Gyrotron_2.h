#ifndef ECCD_GYROTRON_2_H
#define ECCD_GYROTRON_2_H

#include <QtGui/QWidget>
#include "ui_ECCD_Gyrotron_2.h"
#include "qtchaccesslib.h"

class ECCD_Gyrotron_2 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Gyrotron_2(QWidget *parent = 0);
    ~ECCD_Gyrotron_2();


private:
    Ui::ECCDGYROTRON_2 ui;

    AttachChannelAccess *pattach;


};

#endif // ECCD_Gyrotron_2_H
 

