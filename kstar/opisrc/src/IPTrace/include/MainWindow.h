#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <signal.h>
#include "qtchaccesslib.h"
#include "caWaveformput.h"
#include "pv.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

private slots:
	//void PBAction();

private:
	void init();
	void createActions();
	void createConnects();
	void attachWidget();
	void makeUI();
	void closeEvent(QCloseEvent *);

	CAWaveformPut* mpWaveform;

	QWidget *centralwidget;
	AttachChannelAccess *pattach;

	//pvSystem *msys;
};
#endif
