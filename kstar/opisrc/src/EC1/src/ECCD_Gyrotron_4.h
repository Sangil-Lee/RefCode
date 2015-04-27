#ifndef ECCD_GYROTRON_4_H
#define ECCD_GYROTRON_4_H

#include <QtGui/QWidget>
#include "ui_ECCD_Gyrotron_4.h"
#include "qtchaccesslib.h"

class ECCD_Gyrotron_4 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Gyrotron_4(QWidget *parent = 0);
    ~ECCD_Gyrotron_4();

private slots:
	void slot_SET_DATA(void);

private:
    Ui::ECCDGYROTRON_4 ui;

    AttachChannelAccess *pattach;
	//void SET_DATA();
};

#endif // ECCD_Gyrotron_4_H
 

