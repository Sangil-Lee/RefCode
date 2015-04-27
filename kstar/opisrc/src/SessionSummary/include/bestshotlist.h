#ifndef __BEST_SHOTLIST_H
#define __BEST_SHOTLIST_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mysql++.h>
#include <cadef.h>
#include <dbDefs.h>
#include <iostream>
#include <kstardbconn.h>
#include "sessionsummary.h"

using namespace std;

class SessionSummary;

class BestShotList:public QWidget
{
	Q_OBJECT
public:
	//BestShotList(SessionSummary *pwidget);
	BestShotList();
	virtual ~BestShotList();
	void setParent(QWidget *parent);
	//{m_pParent = (SessionSummary*)m_pParent;};

public slots:
	void dbSearch();
	void cbChanged(int index);
	void Close();

signals:
	void Search();

protected:
	void timerEvent(QTimerEvent *event);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	void readBestShotlist();

	QWidget *m_pBest;
	SessionSummary *m_pParent;
	QUiLoader	loader;

	mysqlpp::Connection m_con;
	QLabel *mlbTodayShot;
	QLabel *mlbTotalShot;
	QLineEdit *mleTodayIp;
	QLineEdit *mleTotalIp;
	QTableWidget *mtwBestShot;
	QCheckBox *mckToday;
	QCheckBox *mckDesc;
	QComboBox *mcbSort;
	QDateTimeEdit *mdtStart;
	QDateTimeEdit *mdtEnd;
	QLineEdit *mleStart;
	QLineEdit *mleEnd;
	QPushButton *mpbSearch;
};
#endif
