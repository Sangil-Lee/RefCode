#ifndef ECCD_ANTENA_0_H
#define ECCD_ANTENA_0_H

#include <QtGui/QWidget>
#include "ui_ECCD_Antena_0.h"
#include "qtchaccesslib.h"

#include "ECCD_lib.h"

// Sub Header File Include
#include "ECCD_Antena_1.h"
#include "ECCD_Antena_2.h"


class ECCD_Antena_0 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Antena_0(QWidget *parent = 0);
    ~ECCD_Antena_0();

// slot define
private slots:
	void slot_ECCDANTENA_1_Show(void);
	void slot_ECCDANTENA_2_Show(void);
	void setpushButton(int);

private:
    Ui::ECCDANTENA_0 ui;

    AttachChannelAccess *pattach;

// Sub Form Class define   
	ECCD_Antena_1		*ECCDANTENA_1;
	ECCD_Antena_2		*ECCDANTENA_2;

};

#endif // ECCD_Antena_0_H
 

