#ifndef CLTU_TIMINGSET_H
#define CLTU_TIMINGSET_H

#include <QObject>
#include <QTimer>
#include <QBrush>
#include <QTableWidgetItem>
#include <QMutex>
#include <QEvent>
#include <QWidget>


#include "global.h"

#include "epicsFunc.h"
//#include "queueThread.h"
#include "pvST_mng.h"
#include "pvLists.h"



#define MAX_COL_NUM	13
#define MAX_COL2_NUM	30


#define COL_ID_SYS	0
#define COL_ID_DESC	1
#define COL_ID_CH	2


#define COL_ID_POL	3
#define COL_ID_1_T0		4
#define COL_ID_1_T1		5
#define COL_ID_1_CLK	6
#define COL_ID_2_T0		7
#define COL_ID_2_T1		8
#define COL_ID_2_CLK	9
#define COL_ID_3_T0		10
#define COL_ID_3_T1		11
#define COL_ID_3_CLK	12


#define COL_ID2_ONOFF	3
#define COL_ID2_MODE	4
#define COL_ID2_POL		5
#define COL_ID2_1_T0	6
#define COL_ID2_1_T1	7
#define COL_ID2_1_CLK	8
#define COL_ID2_2_T0	9
#define COL_ID2_2_T1	10
#define COL_ID2_2_CLK	11
#define COL_ID2_3_T0	12
#define COL_ID2_3_T1	13
#define COL_ID2_3_CLK	14
#define COL_ID2_4_T0	15
#define COL_ID2_4_T1	16
#define COL_ID2_4_CLK	17
#define COL_ID2_5_T0	18
#define COL_ID2_5_T1	19
#define COL_ID2_5_CLK	20
#define COL_ID2_6_T0	21
#define COL_ID2_6_T1	22
#define COL_ID2_6_CLK	23
#define COL_ID2_7_T0	24
#define COL_ID2_7_T1	25
#define COL_ID2_7_CLK	26
#define COL_ID2_8_T0	27
#define COL_ID2_8_T1	28
#define COL_ID2_8_CLK	29









class CltuTimingSet : public QWidget
{
	Q_OBJECT

	QMutex mutexEventCB;
	QString qstrEventCB;
	chid chidEventCB;
	double dbCallbackVal;

public:
    CltuTimingSet();
	 ~CltuTimingSet();
	void destroyPageTimingSet();

	
//	evid evidMain[500];
//	int cntEvidMain;
	


	void InitWindow( void *);

	void clicked_btn_timing_insert();
//	void clicked_btn_timing_done();
//	void idchanged_cb_timing_port(int);
//	void idchanged_cb_timing_system(QString);
//	void currentItemChanged_tw_timingSet( QTableWidgetItem *, QTableWidgetItem *);
	void itemChanged_tw_timingSet( QTableWidgetItem *);	
	void clicked_btn_flush_LTUparam();
	void clicked_btn_bliptime_final_setup();
	void cellClicked_tw_load_info(int, int);
	void clicked_btn_single_load();
	void clicked_btn_reload();


	void caput_removeCurrentRowItem();
	void removeCurrentRowItem();

	void fileOpen(QString);
	void fileSave(QString);
	void fileSave_List(QTableWidget *, FILE *);
	void fileSave_List_R2(QTableWidget *, FILE *);


	int cb_register_callbackFunc(ST_PV_NODE &, unsigned int, int, int, int, int arg = 0 );
	void cb_clear_callbackFunc(ST_PV_NODE &);
	bool check_pv_access(chid);

	void cb_thread_updateTimingTable(ST_QUEUE_STATUS );

	float pg_getTimeTail(int frameRate);

	void caput_reset_all_sys();
	void pg_reset_all_list();
//	void pg_setup_CLTU_with_all_list();
	void pg_remove_all_list();
	
	int pg_get_bliptime();
	int pg_get_bliptime_ccs();
	void pg_set_bliptime(int);
	void pg_set_bliptime_ccs(int);
	void pg_run_time_flush();
	void pg_set_color_line(QTableWidget *);
	void pg_set_query_shot(unsigned int num);
	unsigned int pg_get_query_shot();
	bool pg_is_enclosed(char *, int );
	bool pg_is_enclosed_ltu(char *);
	void pg_get_description(char *, int, char *);
	void pg_set_block_pv(int mode);
	void pg_set_release_pv();

	QTableWidget* getTableWidget_from_currentTab();
	QTableWidget* getTableWidget_from_sysname(char*);
	int getTableWidgetID_sysname(char*);



	int caput_assign_CLTU_with_input_row(QTableWidget *, int );
	int caput_setup_CLTU_with_input_row(QTableWidget *, int);
	int caput_CLTU_with_Arg(int nsys, int port,  ST_CLTU_config pcfg);
	int caput_CLTU_with_Arg_R2(int nsys, int port,  ST_CLTU_config pcfg);
	int caput_DDS1_with_Arg_LTU(int nsys, int port,  ST_CLTU_config &cfg, ST_DDS_PV &pvDDS1);
	int caput_DDS1_with_Arg_V2_LTU(int nsys, int sect,  ST_CLTU_config &cfg, ST_DDS_PV &pvDDS1);


protected:
	
	void *pm;

	Cpvst_mng *pSTmng;
//	void *pSTmng;
	



	void InitTW_timingParamSet();

	void pg_InsertRow_from_lineBuf( char *);

	
	int nPVblockMode; 

	int nBlip_time;
	int nBlip_time_CCS;

	int flush_ERROR_cnt;

	unsigned int current_query_shot;



private slots:
	void timerFunc_assign_machine();
	void timerFunc_assign_diagnostics();
	void timerFunc_assign_ltu2();
	void timerFunc_assign_ltu2_diag();
	void timerFunc_setup_machine();
	void timerFunc_setup_diagnostics();
	void timerFunc_setup_ltu2();
	void timerFunc_setup_ltu2_diag();


private:
	
	QTimer *timer_assign_machine;
	QTimer *timer_assign_diagnostics;
	QTimer *timer_assign_ltu2;
	QTimer *timer_assign_ltu2_diag;
	QTimer *timer_setup_machine;
	QTimer *timer_setup_diagnostics;
	QTimer *timer_setup_ltu2;
	QTimer *timer_setup_ltu2_diag;
//	void killOneSecTimer_DDS();
//	unsigned int oneSecTimerCounter_DDS;

	QStringList qstrlist_Clock;
	QStringList qstrlist_R1_Port;
	QStringList qstrlist_R2_Port;



	static void cb_func_TimeSetting(struct event_handler_args args);


	QBrush brush_sep1;
	QBrush brush_sep2;

	QBrush brush_foreColum;

	QBrush brush_dirt_red;
	QBrush brush_dirt_green;

};

#endif

