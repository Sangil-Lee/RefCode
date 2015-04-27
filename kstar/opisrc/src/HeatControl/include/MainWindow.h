#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <signal.h>
#include "qtchaccesslib.h"
#include "pv.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(const QString optype);
	virtual ~MainWindow();

private slots:
	void PBAction();

private:
	void init();
	void createActions();
	void createConnects();
	void attachWidget();
	void makeUI();
	void closeEvent(QCloseEvent *);

	QPushButton *pbNBIA;
	QPushButton *pbNBIB;
	QPushButton *pbECH110;
	QPushButton *pbECH170;
	QPushButton *pbLHCD;
	QPushButton *pbICRF;
	QString moptype;

	QWidget *centralwidget;
	AttachChannelAccess *pattach;

	//pvSystem *msys;
};
#endif
