//KSTAR MainWindow
#ifndef	MAIN_WINDOW_H
#define	MAIN_WINDOW_H

#include <QtGui>
#include <QAssistantClient>
#include <signal.h>
#include <sys/types.h>

#include "qtchaccesslib.h"
#include "caGraphic.h"
#include "caWclock.h"
#include "password.h"
#include "ArchiveDataClient.h"

using namespace std;

class QAssistantClient;

class PVandValue {
public:
	string pvname;
	double dvalue;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
	virtual ~MainWindow();

	enum DISPLYDOCKTYPE{DISPLAYTYPE=0,DISPLAYTYPE1,DISPLAYTYPE2,DISPLAYTOT};

	int mloop;
	int marraycount;

	void AddEPICSTimeStamp(const int page, const epicsTimeStamp & timestamp);
	void AddPVAndValue(const int page, const string & pvname, const double & dvalue);
	int CurrentPage() {return stackedWidget -> currentIndex();};

protected:
	bool connectToArchiver( const QString& url="http://172.17.100.200/archive/cgi/ArchiveDataServer.cgi");
	virtual void timerEvent(QTimerEvent *event);

private slots:
	void showFullWindow(bool top);
	void setDefaultIndex(int); 
	void changeText(const QString & str); 
	void getChannelValues();
	void readValue(int index);
	void incDouble();
	void increase();
	void decrease();
	void decDouble();
	void timerStop();
	void timerRun();

private:
	bool eventFilter(QObject *, QEvent *);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent *event );
	void init();
	void createConnects();
	void makeUI();
	void registerPushButtonsEvent();
	bool displayDockWidget(const int type, QPushButton *pobj);
	void setEnableButtons(QPushButton *pobj);
	void attachAllWidget();
	void setIndex(const size_t index);

private:
	static const int MAX_COUNT = 10000;
	enum A_TYPE{PLOT_TYPE = 0, SHEET_TYPE=4};
	static QAssistantClient *assistant;
	vector <int> keyindex;

	//vector<AttachChannelAccess *> vecACHAcc;
	vector<QWidget *> vecACHAcc;

	ArchiveDataClient   m_arclient;
	stdVector<stdString> mpage1names, mpage2names;

	PVandValue pvvalue;
	//vector<PVandValue> vecPVValue;
	vector<PVandValue> vecPage1Value, vecPage2Value;

	vector<epicsTimeStamp> vecPage1Stamp, vecPage2Stamp;
	typedef QHash<QString, CAColorTileButton*> Hash_Tile;
	Hash_Tile hash_tilepage1, hash_tilepage2;
	QHash<QString, CAColorTileButton*>::const_iterator hash_tileiter;

	//vector<TileObject> vecTileData;
	//QActions
    QAction *action_Exit;

	//QWidgets
    QWidget *centralwidget;
    QWidget *widget;
    QWidget *dockWidgetContents;
    QWidget *tab_0;
    QWidget *layoutWidget_2;
	QWidget *page;
	QWidget *page_2;

    QComboBox *archiveList;
    QDateTimeEdit *Start_dateTime;
    QDateTimeEdit *End_dateTime;
    QPushButton *RButton;
    QSlider *timeSlider;
    QLabel *currentTimeLabel;

	//QLayout
    QVBoxLayout *vdockLayout;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout0;

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

	//etc
    QDockWidget *dockWidget;
    QLabel *label;
    QTabWidget *tabWidget;
    QToolBar *toolBar;
    QStatusBar *statusbar;
    QFrame *msgframe;
	QStringList	displayDocklist;
	QStringList passWord;
	QStackedWidget *stackedWidget;
	QTextEdit *textEdit;

    QSplitter *splitter;
    QPushButton *incButtonDouble;
    QPushButton *incButton;
    QPushButton *decButton;
    QPushButton *decButtonDouble;

    QSplitter *splitter2;
    QPushButton *stopButton;
    QPushButton *startButton;

    QLabel *logo;
	//AttachChannelAccess *pattach;
	QWidget *pwidget;
    QPixmap     mpixmap;
	int mtimerID;
};
#endif
