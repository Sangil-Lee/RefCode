#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <signal.h>
#include "qtchaccesslib.h"

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
		void attachWidget();
		void makeUI();
		void closeEvent(QCloseEvent *);
	
		QWidget *centralwidget;
		AttachChannelAccess *pattach;
};
#endif
