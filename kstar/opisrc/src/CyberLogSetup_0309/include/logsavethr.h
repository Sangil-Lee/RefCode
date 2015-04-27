#ifndef _LOG_SAVE_THR_H
#define _LOG_SAVE_THR_H

#include <QtGui>
#include <mysql++.h>

#include "kstardbconn.h"
#include "logsetup.h"

using namespace std;

class CyberLogSetup;

class LogSaveThr:public QThread
{
	Q_OBJECT
public:
	LogSaveThr(const CyberLogSetup *pSetup);
	virtual ~LogSaveThr();
	const CyberLogSetup *mpSetup;

public slots:

signals:

protected:
	void run();
};

#endif
