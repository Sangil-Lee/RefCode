#ifndef __SESSION_SUMMARY_H
#define __SESSION_SUMMARY_H
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
#include "sessionsummarythr.h"
#include "bestshotlist.h"
#include "shotresult.h"
#include "mdsplusdata.h"

using namespace std;

class SessionSummaryThr;
class ShotResult;

class SessionSummary:public QWidget
{
	Q_OBJECT
public:
	SessionSummary();
	virtual ~SessionSummary();

	void SetShotType(const int shottype){mshottype = shottype;};
	void ReadMDSData(int shotno, bool update=true);
	bool GetAuto(){return mbAuto;};
	void SetAuto(const bool bauto){mbAuto=bauto;};
	int	GetUpdateShotNum( unsigned int &ccs_shotno, unsigned int &mds_shotno);
	int ResetCCSPShotN();
	QString GetHostname(){return mhostname;};
	void Update();
	void LastShotNumber(const unsigned int lastshot){mshotno = lastshot;};
	mysqlpp::Connection & getConnection(){return m_con;};
	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};

public slots:
	void showBestIp();
	void showResult();
	void dbSearch();
	void dbRecordDelete();
	void dbRecordModify();
	void executeQuery(string strquery);
	void saveToMySQL(int,int);
	void makeSession();
	void sendShotResult(unsigned int shotno);
	void lastshotRefresh();
	void cbChanged(int index);
	void print();
	void exportPDF();
	void saveSES();
	void readSES();
	int updateTeData();

signals:
	void Search();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void timerEvent(QTimerEvent *event);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	void refreshShot(unsigned int shotno);
	int messageBox(QString msg, const int type=1);
	void hostHide(QString hostname);
	void updateAirPressure(MDSPlusData *mdsplusdata);
	void smooth(double *dest, double *src,const int count, const int size);

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QTableWidget *m_ptable;
	bool mbstart;
	int  mshottype;
	QString mhostname;

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;

	QCheckBox *mckToday;
	QCheckBox *mckDesc;
	QComboBox *mcbSort;
	QComboBox *mcbModify;
	QPushButton *mpbBest;
	QPushButton *mpbSearch;
	QPushButton *mpbRefresh;
	QPushButton *mpbResult;

	QLabel *mlbToday;
	QDateTimeEdit *mdtStart;
	QDateTimeEdit *mdtEnd;
	QLineEdit *mleStart;
	QLineEdit *mleEnd;
	QLineEdit *mleSesNo;
	QLineEdit *mleSestitle;
	QLineEdit *mleSesleader;
	QLineEdit *mleSescmo;
	QSpinBox	  *msbShotNo;
	vector<QString> mPreval,mCurval;
	SessionSummaryThr *mpThr;
	ShotResult *mpShotResult;
	unsigned int	mshotno;
	bool mbAuto;
	QPrinter	mprint;
	QPrinter	mpdfprint;
	QPixmap		mpixmap;
	MDSPlusData *m_mdsplusdata;
};
#endif
