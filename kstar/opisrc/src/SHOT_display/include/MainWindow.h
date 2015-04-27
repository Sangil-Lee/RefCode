#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

//[[linupark added
//#define ATTACHALL
//]]linupark added


#include <QtGui>
#include <signal.h>
#include "qtchaccesslib.h"
#include <sys/types.h>




class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		virtual ~MainWindow();

	private:
		void init();
		void createActions();
		void createConnects();
		void createToolbars();
		void attachWidget();
		vector<AttachChannelAccess *> vecACHAcc;
		QStackedWidget *stackedWidget;
		QHBoxLayout *hbox;

		void makeUI();
		void closeEvent(QCloseEvent *);
		QWidget *centralwidget;
		AttachChannelAccess *pattach;
		QToolBar *ToolBar;

		QAction *expAction;
		QAction *macAction;

	private slots:
		void expCA();
		void macCA();


};
#endif
