#ifndef _CYBER_LOG_SETUP_H
#define _CYBER_LOG_SETUP_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mysql++.h>
#include <iostream>
#include <kstardbconn.h>
#include <logsavethr.h>

using namespace std;
class LogSaveThr;

class CyberLogSetup:public QWidget
{
	Q_OBJECT
public:
	CyberLogSetup();
	virtual ~CyberLogSetup();


public slots:
	int sdnDataInsert();

signals:

protected:
	void timerEvent(QTimerEvent *event);

private:
	void init();
	void regsignals();
	int createtable(string tablename);
	QWidget *m_plogsetup;
	QUiLoader m_loader;
	QLineEdit *mleTestID;
	QLineEdit *mleProName;
	QTextEdit *mteDesc;

	QRadioButton *mrbAuto;
	QRadioButton *mrbManual;

	QRadioButton *mrbSetEnd;
	QRadioButton *mrbSetTimer;

	QSpinBox *msbMin;
	QSpinBox *msbSec;

	QDateTimeEdit *mdteStart;
	QDateTimeEdit *mdteStop;

	QTimeEdit *mteSetTimer;

	QPushButton *mpbSave;
	QPushButton *mpbStop;

	QPushButton *mpbCreate;
	QPushButton *mpbModify;
	QPushButton *mpbDelete;

	QLabel		*mlbCurDateTime;
	QLabel		*mlbRunStop;

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;
	LogSaveThr *mpLogSave;
};
#endif
