#ifndef ECCD_POWERSUPPLY_0_H
#define ECCD_POWERSUPPLY_0_H

#include <QtGui/QWidget>
#include "ui_ECCD_PowerSupply_0.h"
#include "qtchaccesslib.h"

// Sub Header File Include
#include "ECCD_PowerSupply_1.h"
#include "ECCD_PowerSupply_2.h"

class ECCD_PowerSupply_0 : public QWidget
{
    Q_OBJECT

public:
    ECCD_PowerSupply_0(QWidget *parent = 0);
    ~ECCD_PowerSupply_0();

// slot define
private slots:
	void slot_ECCDPOWERSUPPLY_1_Show(void);
	void slot_ECCDPOWERSUPPLY_2_Show(void);
	void setpushButton(int);

private:
    Ui::ECCDPOWERSUPPLY_0 ui;

    AttachChannelAccess *pattach;

// Sub Form Class define   
	ECCD_PowerSupply_0		*ECCDPOWERSUPPLY_0;    
	ECCD_PowerSupply_1		*ECCDPOWERSUPPLY_1;    
	ECCD_PowerSupply_2		*ECCDPOWERSUPPLY_2;    
	
};

#endif // ECCD_PowerSupply_0_H
 

