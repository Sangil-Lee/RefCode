#ifndef ECCD_TRANSLINE_2_H
#define ECCD_TRANSLINE_2_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_2.h"
#include "qtchaccesslib.h"
//#include "ECCD_Status_Img.h"

class ECCD_TransLine_2 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_2(QWidget *parent = 0);
    ~ECCD_TransLine_2();

// slot define
private slots:
	int slot_TMP1_Stat_Desc(double);
	int slot_TMP2_Stat_Desc(double);
	int slot_TMP3_Stat_Desc(double);
	int slot_TMP4_Stat_Desc(double);
	int slot_TMP5_Stat_Desc(double);

	void slot_VGV1_Stat_Change1(double);
	void slot_VGV2_Stat_Change1(double);
	void slot_VGV3_Stat_Change1(double);
	void slot_VGV4_Stat_Change1(double);
	void slot_VGV5_Stat_Change1(double);

private:
    Ui::ECCDTRANSLINE_2 ui;

    AttachChannelAccess *pattach;
//    	ECCD_Status_Img		*ECCDSTATUSIMG;
};

#endif // ECCD_TransLine_2_H
 

