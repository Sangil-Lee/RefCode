#ifndef ECCD_TRANSLINE_4_H
#define ECCD_TRANSLINE_4_H

#include <QtGui/QWidget>
#include "ui_ECCD_TransLine_4.h"
#include "qtchaccesslib.h"

class ECCD_TransLine_4 : public QWidget
{
    Q_OBJECT

public:
    ECCD_TransLine_4(QWidget *parent = 0);
    ~ECCD_TransLine_4();

private slots:
	void slot_SET_DATA(void);

private:
    Ui::ECCDTRANSLINE_4 ui;

    AttachChannelAccess *pattach;

};

#endif // ECCD_TransLine_4_H
 

