#ifndef __SHOT_RESULT_H
#define __SHOT_RESULT_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mysql++.h>
#include <cadef.h>
#include <dbDefs.h>
#include <iostream>
#include "sessionsummary.h"

using namespace std;

class SessionSummary;

class ShotResult:public QWidget
{
	Q_OBJECT
public:
	ShotResult();
	virtual ~ShotResult();
	bool GetMode(){return mauto;};
	void Update(unsigned int shotno);
	void setParent(QWidget *parent);
	bool isShow();
	void Show();
	void Hide();
	//void sigupdate(int signo);
	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};
	enum POSTABLE{WRITER=0,REMARK,WDATE};

public slots:
	void makeImage();
	void addRemark();
	void deleteRemark();
	void saveRemark();
	void autoManual();
	void resetWindow();
	void reGeneration();
	void hideWindow();
	void loadImage(int shotno);

signals:
	void update();
	void resetwin();

protected:
	virtual void timerEvent(QTimerEvent *e);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	int messageBox(QString msg, const int type=OKBOX);
	int readImage(QLabel *imageLabel, QString filename);
	void exportImage(QString filename);
	int downloadFile(const QString filename);
	int uploadFile(const QString filename);
	void readRemark(unsigned int shotno);
	void readSession(unsigned int shotno);
	void makeImage(unsigned int shotno);
	int isAlphabet(const char *alpha, const int size);
	int parseShotNum();

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QLabel  *milbPCS;
	QLabel  *milbDAQ;
	QLabel  *mlbTitle;
	QPushButton  *mpbMake;
	QPushButton  *mpbClose;
	QPushButton  *mpbAdd;
	QPushButton  *mpbSave;
	QPushButton  *mpbDelete;
	QPushButton  *mpbAuto;
	QPushButton  *mpbRSize;
	QPushButton  *mpbRGen;

	QLineEdit  *mleTf;
	QLineEdit  *mleIp;
	QLineEdit  *mleIpPulse;
	QLineEdit  *mleNe;
	QLineEdit  *mleTe;
	QLineEdit  *mlePr;
	QLineEdit  *mleEchP;
	QLineEdit  *mleRegenS;
	QLineEdit  *mleRegenE;

	QSpinBox  *msbShotNum;
	QTableWidget *mtw_Shotremark;

	SessionSummary *m_pParent;
	map<int,int> mmapIndex;
	vector<unsigned int> mvecShotregen;

	unsigned int mstartshot;
	unsigned int mendshot;
	mysqlpp::Connection m_con;
	const QString mfilepath;
	QPixmap     mpixmap;
	QProcess  getProc;
	QProcess  putProc;
	unsigned int mshotno;
	bool		mauto;
	int			mstid;
};
#endif
