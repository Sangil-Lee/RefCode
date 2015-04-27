#ifndef ECCD_INTERLOCKSAFETY_H
#define ECCD_INTERLOCKSAFETY_H

#include <QtGui/QWidget>
#include "ui_ECCD_InterlockSafety.h"
#include "qtchaccesslib.h"

class ECCD_InterlockSafety : public QWidget
{
    Q_OBJECT

public:
    ECCD_InterlockSafety(QWidget *parent = 0);
    ~ECCD_InterlockSafety();

// slot define
private slots:
	int slot_TMP1_Stat_Desc(double);
	int slot_TMP2_Stat_Desc(double);
	int slot_TMP3_Stat_Desc(double);
	int slot_TMP4_Stat_Desc(double);
	int slot_TMP5_Stat_Desc(double);
	void slot_SET_DATA(void);

	void slot_HV_Stat_Change1(bool);
	void slot_HV_Stat_Change2(bool);

	void slot_VGV1_Stat_Change1(double);
	void slot_VGV2_Stat_Change1(double);
	void slot_VGV3_Stat_Change1(double);
	void slot_VGV4_Stat_Change1(double);
	void slot_VGV5_Stat_Change1(double);

	void slot_WG1_Stat_Change1(double);
	void slot_WG2_Stat_Change1(double);
	void slot_WG3_Stat_Change1(double);
	void slot_WG4_Stat_Change1(double);

private:
	Ui::ECCDINTERLOCKSAFETY ui;

	AttachChannelAccess *pattach;
	bool bHVMode1, bHVMode2;	
};

#endif // ECCD_InterlockSafety_H
 

