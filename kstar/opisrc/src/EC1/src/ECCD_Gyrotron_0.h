#ifndef ECCD_GYROTRON_0_H
#define ECCD_GYROTRON_0_H

#include <QtGui/QWidget>
#include "ui_ECCD_Gyrotron_0.h"
#include "qtchaccesslib.h"

#include "ECCD_lib.h"

// Sub Header File Include
#include "ECCD_Gyrotron_1.h"
#include "ECCD_Gyrotron_2.h"
#include "ECCD_Gyrotron_3.h"
#include "ECCD_Gyrotron_4.h"

class ECCD_Gyrotron_0 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Gyrotron_0(QWidget *parent = 0);
    ~ECCD_Gyrotron_0();

// slot define
private slots:
	void slot_ECCDGYROTRON_1_Show(void);
	void slot_ECCDGYROTRON_2_Show(void);
	void slot_ECCDGYROTRON_3_Show(void);
	void slot_ECCDGYROTRON_4_Show(void);
	void setpushButton(int);

private:
    Ui::ECCDGYROTRON_0 ui;

    AttachChannelAccess *pattach;

// Sub Form Class define   
	ECCD_Gyrotron_1		*ECCDGYROTRON_1;
	ECCD_Gyrotron_2		*ECCDGYROTRON_2;
	ECCD_Gyrotron_3		*ECCDGYROTRON_3;
	ECCD_Gyrotron_4		*ECCDGYROTRON_4;

};
#endif // ECCD_Gyrotron_0_H
 

