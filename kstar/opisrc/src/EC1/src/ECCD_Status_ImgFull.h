#ifndef ECCD_Status_ImgFull_H
#define ECCD_Status_ImgFull_H

#include <QtGui/QWidget>
#include "ui_ECCD_Status_ImgFull.h"
#include "qtchaccesslib.h"

class ECCD_Status_ImgFull : public QWidget
{
    Q_OBJECT

public:
    ECCD_Status_ImgFull(QWidget *parent = 0);
    ~ECCD_Status_ImgFull();

private slots:
	void slot_MODECHANGE(bool);
	void slot_RFGVCHANGE(bool);
	void slot_MODECHANGE2(bool);
	void slot_RFGVCHANGE2(bool);
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

	void slot_WG2_Stat_Change1(double);
	void slot_WG3_Stat_Change1(double);

private:
	Ui::ECCDSTATUSIMGFULL ui;
	AttachChannelAccess *pattach;
	bool bRFGV, bKMode, bRFGV2, bKMode2;
};

#endif // ECCD_Status_ImgFull_H
 

