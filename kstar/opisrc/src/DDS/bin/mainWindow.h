/****************************************************************************
**
** Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved. 
**
**
*****************************************************************************/

#ifndef FRAME_LAYOUT_H
#define FRAME_LAYOUT_H

#include <QMainWindow>

#include <QIcon>
#include <QList>
#include <QPixmap>
#include <QMutex>
//#include <QQueue>
#include <QTextCursor>

#include <signal.h>

#include "ui_DDS_qtmainwindow.h"
#include "ddsDescDialog.h"
//#include "digitalclock.h"
#include "passwordDialog.h"
#include "queueThread.h"
//#include "fileControl.h"

/*
#include "epicsfnc.h"
//#include <alarmString.h>
#include <envDefs.h>
*/

#include "global.h"

#include "cadef.h"


//#include <QAssistantClient>
//#include <signal.h>
//#include <sys/types.h>clicked_localStartStop
//#include "kstar_errMdef.h"
//#include "MultiplotMainWindow.h"
//#include "ArchiveSheet.h"
#include "qtchaccesslib.h"
#include "caGraphic.h"
#include "caWclock.h"

#include "callbackFunc.h"

//#include "page_dds1.h"
//#include "page_dds2.h"






//#define pmain	((MainWindow*)pm)

#define PV_CONNECT		1

#if defined(WIN32)
#define usleep(usec)	epicsThreadSleep(usec/1000000.0)
#endif

//class QListWidget;
//class QListWidgetItem;
//class QStackedWidget;
//class QTableWidget;

//class QAction;
//class QActionGroup;


class DigitalClock;



class MainWindow : public QMainWindow, public Ui::qtMainWindow
{
    Q_OBJECT

	int iCAStatus;
	
public:
	MainWindow(QWidget *parent = 0);
	 ~MainWindow();

	QPalette pal_select, pal_normal, pal_warning;
	
	QMutex mutexEventCB;
	QString qstrEventCB;
	chid chidEventCB;
	double dbCallbackVal;


	CBfunc cbFunc;
//	pageDDS1 pg_dds1;
//	pageDDS2 pg_dds2;

//	FileCTRL fileFunc;
	


	int g_nCurrentSystem;
	int g_opMode;
	int g_mInternalExternal;
	int g_Trigger;
	int g_Period;
	

	char str_passwd[32];

	void printStatus(const char *fmt, ...);	
	unsigned int status_line_cnt;


	void setDDS1StateFromPV(ST_QUEUE_STATUS );

	int checkPasswd(int ID);
	void pvPut_opCondition();


	void patchMessage(chid *, unsigned int);
	void putQMyNode(ST_QUEUE_STATUS);

protected:

	void closeEvent(QCloseEvent *event);


signals:
	void signal_printStatus(char *fmt);

public slots:
	void slot_printStatus( char *fmt);


private slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();
	void filePrint();

	void linkSignalDB();
	void linkJSCOPE();
	void linkOpenEditor();



	void toolBox_currentChanged(int index);

	void clicked_btn_toolbox_DDS1();
	void clicked_btn_toolbox_HAlpha();
	void clicked_btn_toolbox_Mirnov();
	void clicked_btn_toolbox_Mirnov2();
	void clicked_btn_toolbox_Reflecto();
	void clicked_btn_toolbox_Probe();
	void clicked_btn_toolbox_Thomson();
	void clicked_btn_toolbox_XIC();
	void clicked_btn_toolbox_CE();
	void clicked_btn_toolbox_BOLO();
	void clicked_btn_toolbox_SXR();
	void clicked_btn_toolbox_MMWI();
	void clicked_btn_toolbox_VS();
	void clicked_btn_toolbox_FILTER();
	void clicked_btn_toolbox_HXR();
	void clicked_btn_toolbox_ECEHR();
	void clicked_btn_toolbox_SHUT();
	void clicked_btn_toolbox_POWER();
	void clicked_btn_toolbox_VBS();
	void clicked_btn_toolbox_IRTV_S1();
	void clicked_btn_toolbox_FILD();
	void clicked_btn_toolbox_IRTV_D1();
	void clicked_btn_toolbox_IRVB();

	
/***************************************************************	
   DDS#1 only ... 
*/
#if 0
	void clicked_btn_dds1OPremote( );
	void clicked_btn_dds1OPcalibration( );
	void clicked_btn_dds1OPLocalTest( );
	void clicked_btn_dds1ResetMode( );
	void clicked_btn_dds1SetAllchs();
	void clicked_btn_dds1DigitizerSetup();
	void clicked_btn_dds1DigitizerSetup_2();
	void clicked_btn_dds1DigitizerSetup_3();
	void clicked_btn_dds1DigitizerSetup_4();
	void clicked_btn_dds1DigitizerSetup_5();
	void clicked_btn_dds1DigitizerSetup_6();
	void clicked_btn_dds1RemoteReady();
	void clicked_btn_dds1CalibRun();
	void clicked_btn_dds1ShotNum();
	void clicked_btn_dds1LocalRun();
	void clicked_btn_dds1B1LocalDAQ();
	void clicked_btn_dds1B1LocalSave();
	void clicked_btn_dds1B2LocalDAQ();
	void clicked_btn_dds1B2LocalSave();
	void clicked_btn_dds1B3LocalDAQ();
	void clicked_btn_dds1B3LocalSave();
	void clicked_btn_dds1B4LocalDAQ();
	void clicked_btn_dds1B4LocalSave();

	void clicked_btn_dds1UpdateStatus();
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
#endif
/********************************************************/
/*
	void clicked_btn_dds2OPremote( );
	void clicked_btn_dds2OPcalibration( );
	void clicked_btn_dds2OPtest( );
	void clicked_btn_dds2ResetMode( );
	void clicked_btn_dds2SetAll();
	void clicked_btn_dds2DigitizerSetup();
	void clicked_btn_dds2CalibStartStop();
	void clicked_btn_dds2RemoteStore();
	void clicked_btn_dds2ShotNum();
	void clicked_btn_dds2TestRun();
	void clicked_btn_dds2DAQabort();

	void clicked_dds2SetupEnable();
*/

private:
	void createActions();
	void createToolbars();
	void createStatusBar();



	void setAllButtonLabelCondition();
	void setToolBox_btn_color_default();

	void initOPIstate();

	void init_pallet_color();



	void setDigitizerUsingEPICS();


	QTimer *timerPV;

	QueueThread *pthread;


	QToolBar *logoTB;
	QTextEdit *textEdit;

	
/*
	static void vFMonitorCallback( struct event_handler_args args);	
	bool event( QEvent *);
*/

/********** for VNC Viewer *********/ /*  2007. 7. 13.   NFRC modify */
	QProcess *myProcess;
	void insertStackedWidgetPage();
/********** for VNC Viewer *********/ /*  2007. 7. 13.   NFRC modify */

};


/*
 class SpreadSheetItem : public QTableWidgetItem
 {
 public:
     SpreadSheetItem();
     SpreadSheetItem(const QString &text);

     QTableWidgetItem *clone() const;

     QVariant data(int role) const;
     void setData(int role, const QVariant &value);
     QVariant display() const;

     inline QString formula() const
         { return QTableWidgetItem::data(Qt::DisplayRole).toString(); }

     static QVariant computeFormula(const QString &formula,
                                    const QTableWidget *widget,
                                    const QTableWidgetItem *self = 0);

 private:
     mutable bool isResolving;
 };
*/

#endif



