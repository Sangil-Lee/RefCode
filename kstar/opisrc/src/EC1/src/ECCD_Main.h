#ifndef ECCD_MAIN_H
#define ECCD_MAIN_H

#include <QtGui/QWidget>
#include <signal.h>

#include "ui_ECCD_Main.h"
#include "ECCD_Operation.h"
#include "qtchaccesslib.h"

#include "ECCD_lib.h"

// Sub Header File Include
#include "ECCD_Status.h"
#include "ECCD_PowerSupply_0.h"
#include "ECCD_Gyrotron_0.h"
#include "ECCD_TransLine_0.h"
#include "ECCD_Antena_0.h"
#include "ECCD_Cooling.h"
#include "ECCD_Calorimetric.h"
#include "ECCD_TimingDAQ_0.h"
#include "ECCD_InterlockSafety.h"
#include "ECCD_Login.h"

class ECCD_Main : public QMainWindow
{
    Q_OBJECT

public:
    ECCD_Main();
    ~ECCD_Main();
	//virtual ~ECCD_Main();
// slot define
private slots:
	void slot_ECCDOPERATION_Show(void);
	void slot_ECCDSTATUS_Show(void);
	void slot_ECCDPOWERSUPPLY_0_Show(void);
	void slot_ECCDGYROTRON_0_Show(void);
	void slot_ECCDTRANSLINE_0_Show(void);
	void slot_ECCDANTENA_0_Show(void);
	void slot_ECCDCOOLING_Show(void);
	void slot_ECCDCALORIMETRIC_Show(void);
	void slot_ECCDTIMINGDAQ_0_Show(void);
	void slot_ECCDINTERLOCKSAFETY_Show(void);

	void slot_CM_Stat_Change1(bool);
	void slot_CM_Stat_Change2(bool);

	void slot_TLCM_Stat_Change1(bool);
	void slot_TLCM_Stat_Change2(bool);

	void slot_OP_Stat_Change1(bool);
	void slot_OP_Stat_Change2(bool);

	void slot_Shot_Stat_Change1(bool);
	void slot_Shot_Stat_Change2(bool);

	void slot_DAQ_Stat_Change1(bool);

	void slot_Shot_Number_Change1(double);

	void slot_L1_Stat_Change(bool);
	void slot_L2_Stat_Change(bool);
	void slot_L3_Stat_Change(bool);
	void slot_L4_Stat_Change(bool);

// Page Jump slot
	void slot_Goto_Gyrotron_0(void);
	void slot_Goto_Gyrotron_1(void);
	void slot_Goto_Gyrotron_2(void);
	void slot_Goto_Transline_0(void);
	void slot_Goto_Transline_1(void);
	void slot_Goto_Transline_2(void);
	void slot_Goto_Antena_0(void);
	void slot_Goto_InterlockSafety(void);
	void slot_Goto_TimingDAQ_0(void);


private:
//    void attachWidget();
	AttachChannelAccess *pattach;
	void closeEvent(QCloseEvent *event);
	void setpushButton(int);

	Ui::ECCDMAIN ui;

	QWidget		   *CloseWdgt;
	
	bool	bEMode, bPMode, bTLRemoteMode, bTLLocalMode, bKstarMode, bDummyMode;	
	
// Sub Form Class define   
	
    	ECCD_Operation		*ECCDOPERATION;
	ECCD_Status			*ECCDSTATUS;
	ECCD_PowerSupply_0		*ECCDPOWERSUPPLY_0;    
	ECCD_Gyrotron_0		*ECCDGYROTRON_0;
	ECCD_TransLine_0		*ECCDTRANSLINE_0;
    	ECCD_Antena_0			*ECCDANTENA_0;
    	ECCD_Cooling			*ECCDCOOLING;
	ECCD_Calorimetric		*ECCDCALORIMETRIC;
    	ECCD_TimingDAQ_0		*ECCDTIMINGDAQ_0;
    	ECCD_InterlockSafety	*ECCDINTERLOCKSAFETY;
	ECCD_Login   		*ECCDLOGIN;

};

#endif // ECCD_Main_H
 

 
