#ifndef _SESSION_SUMMARY_THR_H
#define _SESSION_SUMMARY_THR_H
#include <QtGui>
#include <mysql++.h>
#include <iostream>
#include "cadef.h"
#include "dbDefs.h"

#include "kstardbconn.h"
#include "sessionsummary.h"

using namespace std;

class ShotInfo 
{
public:
	string paraname;
	double paramaxval;
};

class SessionSummary;

class MyNode{
public:
	//MyNode(const SessionSummary *psummary):prevshotnum(0),mpsummary(psummary)
	MyNode(const SessionSummary *psummary):mpsummary(psummary)
	{};
	char        value[20];
	chid        summary_chid;
	const SessionSummary *mpsummary;
	//unsigned long prevshotnum;
};
class SessionSummaryThr:public QThread
{
	Q_OBJECT
public:
	SessionSummaryThr(const SessionSummary *pSession);
	virtual ~SessionSummaryThr();
	//int ReadMDSData(const int shot);
	//int CACommand(string strcmd);
	//void eventCallback(struct event_handler_args eha);
	const SessionSummary *mpSession;
public slots:

signals:

protected:
	void run();
};
#endif
