#ifndef ECCD_TRANSLINE_1_H
#define ECCD_TRANSLINE_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_1.h"
#include "qtchaccesslib.h"
#include "ECCD_Status_Img.h"

class ECCD_TransLine_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_1(QWidget *parent = 0);
    ~ECCD_TransLine_1();

private slots:
	void slot_WG1_Stat_Change1(double);
	void slot_WG2_Stat_Change1(double);
	void slot_WG3_Stat_Change1(double);
	void slot_WG4_Stat_Change1(double);

private:
    Ui::ECCDTRANSLINE_1 ui;

    AttachChannelAccess *pattach;
    	ECCD_Status_Img		*ECCDSTATUSIMG;
};

#endif // ECCD_TransLine_1_H
 

