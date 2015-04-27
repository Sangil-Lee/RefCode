//KSTAR MainWindow
#ifndef	MAIN_WINDOW_H
#define	MAIN_WINDOW_H

#include <QtGui>
#include <QAssistantClient>
#include <signal.h>
#include <sys/types.h>
#include "MultiplotMainWindow.h"
#include "qtchaccesslib.h"
#include "caGraphic.h"
#include "caWclock.h"
#include "caLabel.h"
#include "password.h"
#define MPS_VERSION "3.0 MPS was developed for the 2010 operation."

class QAssistantClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
	virtual ~MainWindow();

	enum DISPLYDOCKTYPE{DISPLAYTYPE=0,DISPLAYTYPE1,DISPLAYTYPE2, DISPLAYTYPE3, DISPLAYTYPE4, DISPLAYTYPE5, DISPLAYTYPE6, 
		DISPLAYTYPE7, DISPLAYTYPE8, DISPLAYTYPE9, DISPLAYTYPE10, DISPLAYTYPE11, DISPLAYTYPE12, 
		DISPLAYTYPE13, DISPLAYTYPE14, DISPLAYTYPE15, DISPLAYTYPE16, DISPLAYTYPE17, DISPLAYTOT};

private slots:
	void showFullWindow(bool top);
	void showMultiplot();
	void showArchiverviewer();
	void showSignalDB();
	void showManual();
	void showAboutMPS();
	void setDefaultIndex(int); 
	void changeText(const QString & str); 

private:
	bool eventFilter(QObject *, QEvent *);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent *event );
	void init();
	void createActions();
	void createConnects();
	void makeUI();
	void registerPushButtonsEvent();
	bool displayDockWidget(const int type, QPushButton *pobj);
	void setEnableButtons(QPushButton *pobj);
	void attachAllWidget();
	void setIndex(const int index);
	static QAssistantClient *assistant;
private:
	//QActions
	vector<AttachChannelAccess *> vecACHAcc;
    QAction *action_Print;
    QAction *action_Exit;

    QAction *action_Multiplot;
    QAction *action_Archiverviewer;
    QAction *action_SignalDB;
	QAction *action_Manual;
	QAction *action_AboutMPS;


	//QWidgets
    QWidget *centralwidget;
    QWidget *widget;
    //QWidget *dockWidgetContents;
    QScrollArea *dockWidgetContents;
    QWidget *tab_0;
    QWidget *tab_1;
    QWidget *tab_2;
    QWidget *layoutWidget_2;
	QWidget *page;
	QWidget *page_2;


	//QLayout
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout0;
    QVBoxLayout *vboxLayout1;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;

	//QPushButtons
    QPushButton *pushButton[DISPLAYTOT];
    QPushButton *addbutton;

	//Menu
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Edit;
    QMenu *menu_Util;
    QMenu *menu_View;
	QMenu *menu_Help;

	//QSpacerItems
    QSpacerItem *spacerItem5;
    QSpacerItem *spacerItem4;
    QSpacerItem *spacerItem3;
    QSpacerItem *spacerItem2;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem0;

	//etc
    QDockWidget *dockWidget;
    QLabel *label;
    QTabWidget *tabWidget;
    QToolBar *toolBar;
    QStatusBar *statusbar;
    QFrame *frame0;
    QFrame *frame1;
    QFrame *frame2;
    QFrame *frame3;
    QFrame *msgframe;
	QStringList	displayDocklist;
	QStringList	passWord;
	//QStackedWidget *stackedWidget;
	QStackedLayout *stackedWidget;
	QTextEdit *textEdit;
	//QProcess *myProcess;
	QProcess archiver;
	QProcess archiver2;
	QProcess archiver3;
	QProcess archiver4;
	QProcess signaldb;
	QProcess *proc;
	AttachChannelAccess *pattach;
	QPixmap		mpixmap;
};
#endif
