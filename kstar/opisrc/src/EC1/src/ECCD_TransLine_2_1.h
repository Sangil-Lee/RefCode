#ifndef ECCD_TRANSLINE_2_1_H
#define ECCD_TRANSLINE_2_1_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_2_1.h"
#include "qtchaccesslib.h"
#include "ECCD_Status_Img.h"

class ECCD_TransLine_2_1 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_2_1(QWidget *parent = 0);
    ~ECCD_TransLine_2_1();

// slot define
private slots:

private:
    Ui::ECCDTRANSLINE_2_1 ui;

    AttachChannelAccess *pattach;
    	ECCD_Status_Img		*ECCDSTATUSIMG;
};

#endif // ECCD_TransLine_2_1_H
 

