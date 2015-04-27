#ifndef ECCD_STATUS_H
#define ECCD_STATUS_H

#include <QtGui/QWidget>
#include "ui_ECCD_Status.h"
#include "qtchaccesslib.h"
#include "ECCD_Status_ImgFull.h"

class ECCD_Status : public QWidget
{
    Q_OBJECT

public:
    ECCD_Status(QWidget *parent = 0);
    ~ECCD_Status();

private:
    Ui::ECCDSTATUS ui;

    AttachChannelAccess *pattach;
// Sub Form Class define   
    	ECCD_Status_ImgFull		*ECCDSTATUSIMGFULL;

};

#endif // ECCD_Status_H
 

