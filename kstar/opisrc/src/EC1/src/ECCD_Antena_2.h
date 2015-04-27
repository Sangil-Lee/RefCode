#ifndef ECCD_ANTENA_2_H
#define ECCD_ANTENA_2_H

#include <QtGui/QWidget>
#include "ui_ECCD_Antena_2.h"
#include "qtchaccesslib.h"

class ECCD_Antena_2 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Antena_2(QWidget *parent = 0);
    ~ECCD_Antena_2();

// slot define
private slots:
	void slot_SET_DATA(void);

private:
    Ui::ECCDANTENA_2 ui;
	  
    AttachChannelAccess *pattach;

};

#endif // ECCD_Antena_2_H
 

