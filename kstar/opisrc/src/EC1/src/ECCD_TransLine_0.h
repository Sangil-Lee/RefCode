#ifndef ECCD_TRANSLINE_0_H
#define ECCD_TRANSLINE_0_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_0.h"
#include "qtchaccesslib.h"

#include "ECCD_lib.h"

// Sub Header File Include
#include "ECCD_TransLine_1.h"
#include "ECCD_TransLine_2.h"
#include "ECCD_TransLine_3.h"
#include "ECCD_TransLine_4.h"
#include "ECCD_TransLine_5.h"
#include "ECCD_TransLine_2_1.h"

class ECCD_TransLine_0 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_0(QWidget *parent = 0);
    ~ECCD_TransLine_0();

// slot define
private slots:
	void slot_ECCDTRANSLINE_1_Show(void);
	void slot_ECCDTRANSLINE_2_Show(void);
//	void slot_ECCDTRANSLINE_2_1_Show(void);
	void slot_ECCDTRANSLINE_3_Show(void);
	void slot_ECCDTRANSLINE_4_Show(void);
	void slot_ECCDTRANSLINE_5_Show(void);
	void setpushButton(int);

private:
    Ui::ECCDTRANSLINE_0 ui;

    AttachChannelAccess *pattach;

// Sub Form Class define   
	ECCD_TransLine_1		*ECCDTRANSLINE_1;
	ECCD_TransLine_2		*ECCDTRANSLINE_2;
	ECCD_TransLine_2_1	*ECCDTRANSLINE_2_1;
	ECCD_TransLine_3		*ECCDTRANSLINE_3;
	ECCD_TransLine_4		*ECCDTRANSLINE_4;
	ECCD_TransLine_5		*ECCDTRANSLINE_5;

};

#endif // ECCD_TransLine_0_H
 

