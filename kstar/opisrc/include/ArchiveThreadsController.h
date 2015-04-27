#ifndef __ARCHIVE__THR_CON_H
#define __ARCHIVE__THR_CON_H
// -*- c++ -*-
// kasemir@lanl.gov

// Tools
#include <ToolsConfig.h>
// Storage
#include <CtrlInfo.h>
#include <RawValue.h>
#include "ArchiveDataCommon.h"
#include "ArchiveDataClient.h"
#include "ArchiveThread.h"

class ArchiveThread;
class ArchiveThreadsController:public QObject
{
	Q_OBJECT
public:
	ArchiveThreadsController(ArchiveDataClient *client, stdVector<stdString> &names,
			epicsTime &start, epicsTime &end, int count, int how, double interval =0, void *arg =0);
	virtual ~ArchiveThreadsController();
    void run();

public slots:

signals:

protected:

private slots:

private:
	QWidget		*m_pwidget;
    mutable		QMutex mutex;
    mutable		QWaitCondition cond;
	stdVector<ArchiveThread*> vecQThread;
};
#endif
