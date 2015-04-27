#ifndef ECCD_TRANSLINE_5_H
#define ECCD_TRANSLINE_5_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_5.h"
#include "qtchaccesslib.h"

class ECCD_TransLine_5 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_5(QWidget *parent = 0);
    ~ECCD_TransLine_5();

private:
    Ui::ECCDTRANSLINE_5 ui;

    AttachChannelAccess *pattach;

};

#endif // ECCD_TransLine_5_H
 

