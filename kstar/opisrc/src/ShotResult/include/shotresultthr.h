#ifndef _SHOT_RESULT_THR_H
#define _SHOT_RESULT_THR_H
#include <QtGui>
#include <mysql++.h>
#include <iostream>
#include "cadef.h"
#include "dbDefs.h"

#include "kstardbconn.h"
#include "shotresult.h"

using namespace std;

class ShotResult;
class ShotInfo 
{
public:
	string paraname;
	double paramaxval;
};

class SessionSummary;

class MyNode{
public:
	MyNode(const ShotResult *pshot):mpshot(pshot)
	{};
	char        value[20];
	chid        shot_chid;
	const ShotResult *mpshot;
	//unsigned long prevshotnum;
};
class ShotResultThr:public QThread
{
	Q_OBJECT
public:
	ShotResultThr(const ShotResult *pShot);
	virtual ~ShotResultThr();
	//int ReadMDSData(const int shot);
	//int CACommand(string strcmd);
	//void eventCallback(struct event_handler_args eha);
	const ShotResult *mpShot;
public slots:

signals:

protected:
	void run();
};
#endif
