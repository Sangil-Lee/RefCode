#ifndef PAGE_DDS2_H
#define  PAGE_DDS2_H

#include <QObject>
#include <QTableWidget>

#include "global.h"

#include "callbackFunc.h"


class pageDDS2 : public QObject
{
    Q_OBJECT

public:
    pageDDS2();
	 ~pageDDS2();

//	void *pm;
	
	ST_ca_ctu pvCLTU;

	char touch_T1_PV[MAX_M6802];

	void InitWindow( void *);
	void initStactedWidgetM6802();
//	void InitLocalValue();
//	void saveConfigreFile();



	void clicked_btn_dds2OPremote();
	void clicked_btn_dds2OPcalibration();
	void clicked_btn_dds2OPtest();
	void clicked_btn_dds2ResetMode();
	void clicked_btn_dds2SetAll();
	void clicked_btn_dds2DigitizerSetup();
//	void clicked_btn_dds2RemoteReady();
//	void clicked_btn_dds1TestRun();  same function to bellow.
	void clicked_btn_dds2CalibStartStop();
	void clicked_btn_dds2RemoteStore();
	void clicked_btn_dds2ShotNum();	
	void clicked_btn_dds2DAQabort();

	void timer_go();
	void timer_stop();	
	void monitor_LTU_start();
	void monitor_dds2PV_start();
	void monitor_stop();


	QString getSampleRateDDS2();
	QString getDDS2_T0();

//	chid chid_DDS2_getState;
	chid chid_DDS2_sampleRate;		//   2009. 8. 25

protected:
	QTimer *timerOneSec;
	CBfunc cbDDS2;

	void initTableWidgetM6802();
	void setupM6802Table(QTableWidget *pwidgsetupet, int num);
	void pvPutTagNameM6802(int ID);
	void pvPutChannelBitMaskM6802(int num);
	void pvPutInternalSetM6802(int num);
	void pvPutExternalSetM6802(int num);
	void pvPut_opCondition();

	void connectPVs();
	void connectTagPVs();
	void disconnectPVs();
	void disconnectTagPVs();

	int initPVmonitoringTimer();
	void killPVmonitoringTimer();


private slots:
	void timerFuncOneSec();

private:

	double runstopTimerCounter;
	QTimer *timerRunStop;

	ST_BOARD_CONFIG m6802Config;
//	int g_nUseCntM6802;
	double g_Trigger;
	double g_Period;
	

};

#endif

