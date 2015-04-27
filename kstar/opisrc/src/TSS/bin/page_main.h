#ifndef CLTU_MAIN_H
#define CLTU_MAIN_H

#include <QObject>
#include <QEvent>
#include <QMutex>
#include <QTimer>
#include <QPalette>

//#include "cadef.h"

#include "epicsFunc.h"
#include "global.h"

#include <kstardbconn.h>



class CltuMain :  public QObject
{
    Q_OBJECT
	
//	QMutex mutexEventCB;
//	QString qstrEventCB;
//	chid chidEventCB;

public:
	CltuMain();
	~CltuMain();
	void destroyPageMain();

	QPalette pal_blip_change, pal_blip_change2, pal_normal;
//	evid evidMain[100];
//	int cntEvidMain;

	void InitWindow( void *);
	void updateTableWidget();


	int cb_register_blipFunc(ST_PV_NODE &);
	void cb_clear_blipFunc(ST_PV_NODE &);
	bool check_pv_access(chid);

	bool tmr_blip_timer_isActive();
	void tmr_blip_timer_start();

	void make_blinking_final_setup();

	void cb_thread_updateMainStack(ST_QUEUE_STATUS );

	int cb_register_main(ST_PV_NODE &, unsigned int, int, int, int, int arg = 0 );
	void cb_clear_callbackFunc(ST_PV_NODE &);

	void save_current_ltu_setup(unsigned int);
	int load_stored_ltu_setup(unsigned int );
	int load_single_line_ltu_setup( );
	void delete_stored_ltu_info(unsigned int );
	

protected:
	void *pm;
	
//	QString sec2HHMMSS(unsigned int data);
//	double dval_bliptime;
/*
	double dShotStartTimeSecDDS1;
	double dShotStartTimeSecDDS2;
	int nDDS1ShotStart, nDDS2ShotStart;
	int dds1ShotEnd, dds2ShotEnd;
*/

	QTimer *timer_Blip_Changed;

private slots:
	void timerFunc_OneSec();
	

private:
//	void InitTW_OpSeq();

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;
	

	static void cb_func_blipMonitor(struct event_handler_args args);
//	bool event( QEvent *);
	static void cb_func_main(struct event_handler_args args);


};

#endif

