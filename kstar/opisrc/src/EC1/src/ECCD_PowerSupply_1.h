#ifndef ECCD_POWERSUPPLY_1_H
#define ECCD_POWERSUPPLY_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_PowerSupply_1.h"
#include "qtchaccesslib.h"

class ECCD_PowerSupply_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_PowerSupply_1(QWidget *parent = 0);
    ~ECCD_PowerSupply_1();

// slot define
private slots:
	void slot_SET_NO1(void);
	void slot_SET_NO2(void);
	void slot_SET_NO3(void);
	void slot_SET_NO4(void);
	void slot_SET_NO5(void);

	void slot_HV_Stat_Change1(bool);
	void slot_HV_Stat_Change2(bool);
private:
	Ui::ECCDPOWERSUPPLY_1 ui;

	AttachChannelAccess *pattach;
	void SET_DATA(int);
	bool bHVMode1, bHVMode2;	
};

#endif // ECCD_PowerSupply_1_H
 

