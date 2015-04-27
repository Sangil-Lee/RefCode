#ifndef ECCD_GYROTRON_3_H
#define ECCD_GYROTRON_3_H

#include <QtGui/QWidget>
#include "ui_ECCD_Gyrotron_3.h"
#include "qtchaccesslib.h"

class ECCD_Gyrotron_3 : public QWidget
{
    Q_OBJECT

public:
    ECCD_Gyrotron_3(QWidget *parent = 0);
    ~ECCD_Gyrotron_3();

private slots:
	void slot_HPS_BYPASS_OK_CHANGE(bool);
	void slot_HPS_BYPASS_ESET_CHANGE(bool);
	void slot_HPS_READY_OK_CHANGE(bool);
	void slot_HPS_CTU_CHANGE(bool);
	//void slot_TLCM_Stat_Change1(bool);
	//void slot_TLCM_Stat_Change2(bool);

private:
	Ui::ECCDGYROTRON_3 ui;
	AttachChannelAccess *pattach;
	
	bool	bBypassMode, bReadyMode;
};

#endif // ECCD_Gyrotron_3_H
 

