#ifndef PAGE_DDS1_H
#define PAGE_DDS1_H

#include <QObject>
#include <QTableWidget>

#include "global.h"

#include "callbackFunc.h"


class pageDDS1 : public QObject
{
    Q_OBJECT

public:
    pageDDS1();
	 ~pageDDS1();


//	ST_ca_ctu pvDDS1ALTU;
	int nClockSource;
//	int nClockDivider[USED_ACQ196];
//	double dMax_T1_time;

//	char touch_Clock_PV[USED_ACQ196];
//	char touch_T0_PV[USED_ACQ196];
	char touch_T1_PV[USED_ACQ196];

	char is_Arm_OK[USED_ACQ196];


//	int nEnable_to_RUN;

	void InitWindow( void *);
//	void initStactedWidgetAcq196();
/*	void InitLocalValue(int ID);
	void saveConfigreFile(int ID);
*/	void kill_dds1_panel();

	void clicked_btn_dds1OPremote();
	void clicked_btn_dds1OPcalibration();
	void clicked_btn_dds1OPLocalTest();
	void clicked_btn_dds1ResetMode();
	void clicked_btn_dds1SetAllchs();
	void clicked_btn_dds1DigitizerSetup(int ID);
	void clicked_btn_dds1START_remote();
//	void clicked_btn_dds1LocalRun();  same function to bellow.
	void clicked_btn_dds1START_local();
	void clicked_btn_dds1ShotNum();	

	void clicked_btn_singleRunDAQ(int);
	void clicked_btn_singleRunSave(int);
	void clicked_btn_dds1CreateShot();

	void clicked_btn_dds1_Retrieve();

	void idchanged_cb_dds1SampleClk(QString);
	void idchanged_cb_dds1SampleClk_2(QString);
	void idchanged_cb_dds1SampleClk_3(QString);
	void idchanged_cb_dds1SampleClk_4(QString);
	void idchanged_cb_dds1SampleClk_5(QString);
	void idchanged_cb_dds1SampleClk_6(QString);

	void clicked_btn_dds1_assign_all();
	void clicked_btn_dds1_Arming();
	void clicked_btn_forced_mode_reset();
	
		

//	chid DDS1_getState[USED_ACQ196];
//	chid DDS1_BX_PARSING_CNT[USED_ACQ196];
//	chid DDS1_BX_MDS_SND_CNT[USED_ACQ196];
//	chid DDS1_BX_DAQING_CNT[USED_ACQ196];	// 2009. 08. 14
	chid CCS_SHOTSEQ_START;    // 2009. 10. 13.   

	void timer_go();
	void timer_stop();
	void monitor_LTU_start();
	void monitor_dds1PV_start();
	void monitor_stop();

	void digitizer_widget_clear(); // 2010. 9. 1


protected:
//	void *pm;
	
	QTimer *timerOneSec;
	QTimer *timerAssignAll;
	
	CBfunc cbFunc;

	void initTableWidgetACQ196();
	void setupACQ196Table(QTableWidget *pwidgsetupet, int num);
	void pvPut_TagNameACQ196(int ID);
	void pvPut_ChannelBitMaskACQ196(int num);
	void pvPut_InternalClockACQ196(int num);
	void pvPut_ExternalClockACQ196(int num);
	void pvPut_opCondition();

	void copy_ca_names();
	void connectPVs();
	void connectLTU_callback();
	void disconnectPVs();
	void connectTagPVs(int ID);
	void disconnectTagPVs(int id);

	int initPVmonitoringTimer();
	void killPVmonitoringTimer();

	void init_DDS1_GUI();
	void set_DDS1_GUI_OPMODE();

	int checkCmd();
	int check_Opmode_connection();


private slots:
	void timerFuncTwoSec();
	void timerFunc_AssignAll();


private:

//	double runstopTimerCounter;
//	QTimer *timerRunStop;

	ST_BOARD_CONFIG acq196Config[USED_ACQ196];
	double g_Trigger;
	double g_Period;
	

};

#endif

