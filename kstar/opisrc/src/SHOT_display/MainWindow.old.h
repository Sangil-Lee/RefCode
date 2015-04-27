//KSTAR MainWindow
#ifndef	MAIN_WINDOW_H
#define	MAIN_WINDOW_H

#include <QtGui>
#include <QAssistantClient>
#include <signal.h>
#include <sys/types.h>
//#include "MultiplotMainWindow.h"
//#include "ArchiveSheet.h"
#include "qtchaccesslib.h"
#include "caGraphic.h"
#include "caWclock.h"
#include "password.h"
#define TMS_VERSION "2.3.0. TMS, HDS, and CLS were modified for 2009 operation."

using std::vector;
class QAssistantClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
	virtual ~MainWindow();

	enum DISPLYDOCKTYPE{DISPLAYTYPE=0,DISPLAYTYPE1,DISPLAYTYPE2, DISPLAYTYPE3, DISPLAYTYPE4, DISPLAYTYPE5, DISPLAYTYPE6, 
		DISPLAYTYPE7, DISPLAYTYPE8, DISPLAYTYPE9, DISPLAYTYPE10, DISPLAYTYPE11, DISPLAYTYPE12, 
		DISPLAYTYPE13, DISPLAYTYPE14, DISPLAYTYPE15, DISPLAYTYPE16, DISPLAYTYPE17, DISPLAYTYPE18, 
		DISPLAYTYPE19, DISPLAYTYPE20, DISPLAYTYPE21, DISPLAYTYPE22, DISPLAYTOT};

private slots:
	void showMultiplot();
	void showArchivesheet();
	void showPVListViewer();
	void showArchiverviewer();
	void showSignalDB();
	void showManual();
	void showAboutTMS();
	void setDefaultIndex(int); 
	void changeText(const QString & str); 

	void expCA();
	void CA();
	void expCA();

private:
	bool eventFilter(QObject *, QEvent *);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent *event );
	void init();
	void createActions();
	void createConnects();
	void makeUI();
	void createToolbars();
	bool displayDockWidget(const int type, QPushButton *pobj);
	void setEnableButtons(QPushButton *pobj);
	void attachAllWidget();
	void setIndex(const int index);


private:
	static QAssistantClient *assistant;
	vector<AttachChannelAccess *> vecACHAcc;
	//QActions
    QAction *action_Print;
    QAction *action_Exit;

    QAction *action_Multiplot;
    QAction *action_Archivesheet;
    QAction *action_Archiverviewer;
    QAction *action_SignalDB;
    QAction *action_PVListV;
	QAction *action_Manual;
	QAction *action_AboutTMS;

	QAction *expAction;
	QAction *diagAction;
	QAction *macAction;

	//QWidgets
    QWidget *centralwidget;
    QWidget *widget;
    QWidget *dockWidgetContents;
    QWidget *tab_0;
    QWidget *tab_1;
    QWidget *tab_2;
    QWidget *tab_3;
    QWidget *layoutWidget_2;
	QWidget *page;
	QWidget *page_2;


	//QLayout
    QVBoxLayout *vdockLayout;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout0;
    QVBoxLayout *vboxLayout1;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;
    QVBoxLayout *vboxLayout4;

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
    QSpacerItem *spacerItem6;
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
    QFrame *frame;
    QFrame *frame2;
    QFrame *frame3;
    QFrame *frame4;
    QFrame *msgframe;
	QStackedWidget *stackedWidget;
	QTextEdit *textEdit;
	//QProcess *myProcess;
	QProcess  archiver;
	QProcess  archiver2;
	QProcess  archiver3;
	QProcess  archiver4;
	QProcess  signaldb;
    QLabel *logo;
    CAWclock *wclock1;
	CAGraphic *ioc1HB, *ioc2HB, *ioc3HB, *ioc4HB, *ioc5HB, *ioc6HB;
	AttachChannelAccess *pattach;
    QPixmap     mpixmap;
};
#endif
