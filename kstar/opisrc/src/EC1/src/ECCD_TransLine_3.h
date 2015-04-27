#ifndef ECCD_TRANSLINE_3_H
#define ECCD_TRANSLINE_3_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_3.h"
#include "qtchaccesslib.h"
#include "ECCD_Status_Img.h"

class ECCD_TransLine_3 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_3(QWidget *parent = 0);
    ~ECCD_TransLine_3();

private:
    Ui::ECCDTRANSLINE_3 ui;

    AttachChannelAccess *pattach;
    	ECCD_Status_Img		*ECCDSTATUSIMG;
};

#endif // ECCD_TransLine_3_H
 

