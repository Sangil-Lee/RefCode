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
#include <kstardbconn.h>
#include "shotresultthr.h"

using namespace std;

class ShotResultThr;

class ShotResult:public QWidget
{
	Q_OBJECT
public:
	ShotResult();
	virtual ~ShotResult();

	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};

public slots:
	void makeImage();
	void loadImage(int shotno);

signals:

protected:

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

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QLabel  *milbPCS;
	QLabel  *milbDAQ;
	QPushButton  *mpbMake;
	QSpinBox  *msbShotNum;

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;
	const QString mfilepath;
	QPixmap     mpixmap;
	QProcess  getProc;
	QProcess  putProc;
};
#endif
