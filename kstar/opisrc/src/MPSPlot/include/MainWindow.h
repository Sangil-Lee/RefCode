#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <signal.h>
#include "qtchaccesslib.h"
#include "caMultiwaveplot.h"
#include "pv.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

public slots:
	void changeIndexColor(int index, QString colorname);

private:
	void init();
	void createActions();
	void createConnects();
	void attachWidget();
	void makeUI();
	void closeEvent(QCloseEvent *);

	CAMultiwaveplot* mpWaveplot;
	QWidget *centralwidget;
	AttachChannelAccess *pattach;
};
#endif
