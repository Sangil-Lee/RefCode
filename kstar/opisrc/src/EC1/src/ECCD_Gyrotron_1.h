#ifndef ECCD_GYROTRON_1_H
#define ECCD_GYROTRON_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_Gyrotron_1.h"
#include "qtchaccesslib.h"

class ECCD_Gyrotron_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Gyrotron_1(QWidget *parent = 0);
    ~ECCD_Gyrotron_1();
private slots:
	void slot_MMPS_Stat_Desc(double);
	void slot_GMPS1_Stat_Desc(double);
	void slot_GMPS2_Stat_Desc(double);

private:
    Ui::ECCDGYROTRON_1 ui;

    AttachChannelAccess *pattach;
};

#endif // ECCD_Gyrotron_1_H
 

