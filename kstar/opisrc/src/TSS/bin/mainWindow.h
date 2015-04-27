/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef FRAME_LAYOUT_H
#define FRAME_LAYOUT_H

#include <stdio.h>

#include <QMainWindow>

#include <QIcon>
#include <QList>
#include <QPixmap>
#include <QMutex>
#include <QQueue>
#include <QTimer>



#include <signal.h>

#include "ui_TSS_qtmainwindow.h"
//#include "digitalclock.h"
#include "passwordDialog.h"

//#include "epicsfnc.h"
//#include <alarmString.h>
//#include <envDefs.h>

#include "cadef.h"

//#include <QAssistantClient>
//#include <signal.h>
//#include <sys/types.h>clicked_localStartStop
//#include "kstar_errMdef.h"
//#include "MultiplotMainWindow.h"
//#include "ArchiveSheet.h"


#include "page_main.h"
#include "page_local_R1.h"
#include "page_local_R2.h"

#include "page_timingSet.h"
#include "page_manyTrig.h"
#include "page_sqlDB.h"


#include "qtchaccesslib.h"
#include "caGraphic.h"
#include "caWclock.h"

#include "global.h"
#include "pvLists.h"

#include "queueThread.h"

#ifndef WR_OK
#define WR_OK	1
#endif

#ifndef WR_ERROR
#define WR_ERROR 0
#endif


//#define PV_CONNECT	1

//#define PV_CONNECT_DDS1		1
//#define PV_CONNECT_HALPHA		1

//#define DDS1_DAQ_TIME_TAIL (10.0)
#define DDS1A_LTU_TAIL_UNDER_2K		(100.0)  // (65.0)
#define DDS1A_LTU_TAIL_UNDER_5K		(60.0)  // (35.0)
#define DDS1A_LTU_TAIL_UNDER_10K		(50.0)  // (15.0)
#define DDS1A_LTU_TAIL_DEFAULT		(50.0)  // (10.0)
#define DDS1A_LTU_TAIL_V2_100SEC		(100.0)




#define HALPHA_DAQ_TIME_TAIL (4.0)

class QStackedWidget;
class QTableWidget;

class QAction;
class QActionGroup;

class QListWidget;

class DigitalClock;


//extern int opi_debug;

class MainWindow : public QMainWindow, public Ui::qtMainWindow
{
    Q_OBJECT

//	int iCAStatus;

//	QMutex mutexPrintStatus;
//	chid chidEventCB;
//	int nPVconnect;

public:
	MainWindow(QWidget *parent = 0);
	 ~MainWindow();

	QPalette pal_select, pal_normal;
//	unsigned int unCurrentSysTime[MAX_LTU_CARD];
//	int nIsUseThisSystem[MAX_LTU_CARD];
//	double dcurDDS1T1, dcurDDS2T1;
	

	void printStatus(int level, const char *fmt, ...);
	void printReturn(int level, const char *fmt, ...);
	void callSignal_saveLTUparam();
	void callSignal_updateSpinBox();


//	void printStatus_addItem(const char *fmt, ...);
	unsigned int status_line_cnt;
//	QTimer *timer_listWidget;

	
	
	void saveConfigFile();
	void saveTimingTest(FILE *);
//	void saveCalibTest(FILE *);
//	void saveCalibedResult(FILE *);


	void put_queue_node(ST_QUEUE_STATUS );

	char str_sysName[MAX_LTU_CARD][10];
	int ltu_feature[MAX_LTU_CARD][2];
	int get_system_ID(const char *);
	bool Is_New_R2_id( int ID);
	bool Is_New_R2_name( const char *);
	int Is_seq_save();


	void updatePV(long , ST_PV_NODE &, int print_on=0);
	void update_spinBox_shotNum_minus_one();



	ST_CLTU_PV		pvCLTU[MAX_LTU_CARD];
	
	ST_DDS_PV		pvDDS1A[CNT_DDS1A_NO];
	ST_DDS_PV		pvDDS1B[CNT_DDS1B_NO];
	ST_DDS_PV		pvHALPHA;
/*	ST_DDS_PV		pvECEHR; */
	ST_PV_NODE 		pvBlipTime;
	ST_PV_NODE 		pvCCS_ShotNumber;
	ST_PV_NODE 		pvCCS_SeqStart;
	ST_PV_NODE 		pvTSSConfirm;
	ST_PV_NODE 		pvTSS_CalMode;

	CltuMain pg_main;
	CltuTimingSet pg_timeset;
	LocalTestR1 pg_localR1;
	LocalTestR2 pg_localR2;
	LTU_MANY_TRIG pg_manyTrig;
	LTU_SQL_DB pg_SQL;

	int confirm_final_setup;
	char passwd_master_check; // default: 1
	char passwd_timeset_check; // default  = 1;
	char flag_MySQL_con;



protected:
	void closeEvent(QCloseEvent *event);
	
	int checkPasswd();
	void assign_system_name();

	CqueueThread *qthread;

//	int dirty_monitor;

signals:
	void signal_printStatus(int level, char *fmt);
	void signal_printReturn(int level, char *fmt);
	void signal_saveLTUparam();
	void signal_update_spinBox();
	

public slots:
	void slot_printStatus(int level, char *fmt);
	void slot_printReturn(int level, char *fmt);
	void slot_saveLTUparam();
	void slot_update_spinBox();

//	void stateChanged_check_calib_admin(int);
	
private slots:
	void fileNew();
	void fileOpen();
	void fileSave();
//	void fileSaveAs();
	void filePrint();
	void action_All_LTUs_reset();
	void action_All_List_zero();
	void action_Save_to_MySQL();
	void action_Delete_shot_MySQL();


	void clicked_btn_main();
//	void clicked_btn_DB_manager();
	void clicked_btn_timingSet();
	void clicked_btn_local_R1();
	void clicked_btn_local_R2();
	void clicked_btn_local_R2_many_trig();
	void clicked_btn_sql_db();


//	void printStatus_clear();
//	void clicked_btn_connect_PVs();
	void clicked_btn_start_monitoring();
	void clicked_btn_stop_monitoring();
//	void func_timer_listWidget();



/*******************************************************/	
/* timing set panel 				*/
	void clicked_btn_timing_insert();
//	void clicked_btn_timing_done();

//	void idchanged_cb_timing_port(int);
//	void idchanged_cb_timing_system(QString);
//	void currentItemChanged_tw_timingSet( QTableWidgetItem *, QTableWidgetItem *);
	void itemChanged_tw_timingSet( QTableWidgetItem *);
	void cellClicked_tw_load_info(int, int);
	void valueChanged_sb_stored_shot(int);

	void clicked_btn_bliptime_final_setup();
	void clicked_btn_flush_LTUparam();
//	void textChanged_le_bliptime_my(QString);
	void clicked_btn_single_load();
	void clicked_btn_reload();


/*******************************************************/
/*   test/cal set panel 							*/
	void clicked_btn_test_set();
	void clicked_btn_test_shotStart();
	void clicked_btn_mTrig_list_clear();
	void clicked_btn_mTrig_Connect();
	void clicked_btn_mTrig_Disconnect();

//	void clicked_btn_cal_calStart();

	void clicked_btn_showinfo_R2();
	void clicked_btn_showTime_R2();
	void clicked_btn_showTLK_R2();
	void clicked_btn_showLog_R2();
	void clicked_btn_test_setup_R2();
	void clicked_btn_test_shotStart_R2();
	void clicked_btn_test_shotStop_R2();
	void clicked_btn_R2_setMode();
	void clicked_btn_R2_insert_item();
	void itemChanged_tw_local_R2(QTableWidgetItem *);
	void clicked_btn_R2_open();
	void clicked_btn_R2_save();


	void stateChanged_check_calib_admin(int); // move to public slot
	void stateChanged_check_TSS_cal_mode(int);
	void clicked_btn_put_LTU_calib_value();

/*************************************************************/
/* many trigger panel */
	void clicked_btn_mTrig_group_setup();
	void clicked_btn_mTrig_pv_setup();
	void clicked_btn_mTrig_start();
	void clicked_btn_mTrig_stop();



/*******************************************************/

	void timerFunc_Connect();

private:
	void createActions();
//	void createMenus();
	void createToolbars();
	void createStatusBar();

	void pv_copy_names_LTUs();
	void pv_copy_names_others();
	void pv_connect_LTU();

	void pv_connect_DDS1();
	void pv_connect_HALPHA();
	void pv_connect_Init();
	void pv_disconnect();	
	bool pv_connection_checking();

//	void startMonitorPVs();
//	void stopMonitorPVs();
	void pv_monitoring_stop();
	void pv_monitoring_start();
	void initOPIstate();

	void set_unselect_btn();

	

	DigitalClock *myDClock;
	QGridLayout *gridLayout_clock;

	QTimer *timerConnect;


	QToolBar *logoTB;
	QTextEdit *textEdit;

	QBrush brush_error;
	QBrush brush_warning;
	QBrush brush_notice;


	bool event( QEvent *);

	/*	2009. 11. 03.   for SHBACK */
	void insert_UI_fileToWidget();
	/*	2009. 11. 03.   for SHBACK */


};

#endif



