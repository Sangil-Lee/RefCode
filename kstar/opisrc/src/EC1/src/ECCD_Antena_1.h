#ifndef ECCD_ANTENA_1_H
#define ECCD_ANTENA_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_Antena_1.h"
#include "qtchaccesslib.h"
#include "ECCD_Status_Img.h"

class ECCD_Antena_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Antena_1(QWidget *parent = 0);
    ~ECCD_Antena_1();
// slot define
private slots:
	void slot_WG3_Stat_Change1(double);

private:
    Ui::ECCDANTENA_1 ui;

    AttachChannelAccess *pattach;
    	ECCD_Status_Img		*ECCDSTATUSIMG;


};

#endif // ECCD_Antena_H
 

