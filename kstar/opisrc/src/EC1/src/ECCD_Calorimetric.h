#ifndef ECCD_CALORIMETRIC_H
#define ECCD_CALORIMETRIC_H

#include <QtGui/QWidget>
#include "ui_ECCD_Calorimetric.h"
#include "qtchaccesslib.h"

class ECCD_Calorimetric : public QWidget
{
    Q_OBJECT

public:
    ECCD_Calorimetric(QWidget *parent = 0);
    ~ECCD_Calorimetric();

private slots:
	void slot_SET_DATA(void);

private:
    Ui::ECCDCALORIMETRIC ui;

    AttachChannelAccess *pattach;
	void SET_DATA();
};

#endif // ECCD_Gyrotron_H
 

