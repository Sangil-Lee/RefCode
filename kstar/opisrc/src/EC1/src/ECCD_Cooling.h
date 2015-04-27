#ifndef ECCD_COOLING_H
#define ECCD_COOLING_H

#include <QtGui/QWidget>
#include "ui_ECCD_Cooling.h"
#include "qtchaccesslib.h"

class ECCD_Cooling : public QWidget
{
    Q_OBJECT

public:
    ECCD_Cooling(QWidget *parent = 0);
    ~ECCD_Cooling();

private slots:
	void slot_SET_DATA(void);

private:
    Ui::ECCDCOOLING ui;

    AttachChannelAccess *pattach;
	void SET_DATA();
};

#endif // ECCD_Cooling_H
 

