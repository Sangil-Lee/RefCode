#ifndef ECCD_OPERATION_H
#define ECCD_OPERATION_H

#include <QtGui/QWidget>
#include "ui_ECCD_Operation.h"
#include "qtchaccesslib.h"
#include <sys/time.h>
#include <signal.h>

class ECCD_Operation : public QWidget
{
    Q_OBJECT

public:
    ECCD_Operation(QWidget *parent = 0);
    ~ECCD_Operation();

    Ui::ECCDOPERATION ui;

// slot define
private slots:
	int slot_TMP1_Stat_Desc(double);
	int slot_TMP2_Stat_Desc(double);
	int slot_TMP3_Stat_Desc(double);
	int slot_TMP4_Stat_Desc(double);
	int slot_TMP5_Stat_Desc(double);

	void slot_SET_NO1(void);
	void slot_SET_NO2(void);
	void slot_SET_NO3(void);
	void slot_SET_NO4(void);
	void slot_SET_NO5(void);
	void slot_CPSFC_Desc(double);
	void slot_APSFC_Desc(double);
	void slot_BPSFC_Desc(double);
	void slot_LCSFC_Desc(double);
	void slot_SlowIntFC_Desc(double);

	void slot_HV_Stat_Change1(bool);
	void slot_HV_Stat_Change2(bool);

	void slot_VacFC_Desc1(double);
	void slot_VacFC_Desc2(double);

	void slot_VGV1_Stat_Change1(double);
	void slot_VGV2_Stat_Change1(double);
	void slot_VGV3_Stat_Change1(double);
	void slot_VGV4_Stat_Change1(double);
	void slot_VGV5_Stat_Change1(double);

	void slot_Send_LTU(void);

	void slot_WG1_Stat_Change1(double);
	void slot_WG2_Stat_Change1(double);
	void slot_WG3_Stat_Change1(double);
	void slot_WG4_Stat_Change1(double);

	void slot_ShotTrig_Stat_Change1(bool);

	void slot_Stanby_On(void);
	void slot_Stanby_Off(void);
private:
//    Ui::ECCDOPERATION ui;

	AttachChannelAccess *pattach;
	void SET_DATA(int);
	bool bHVMode1, bHVMode2;	

};

#endif // ECCD_Operation_H
 

 
