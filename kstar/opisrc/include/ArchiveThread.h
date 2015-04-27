#ifndef __ARCHIVE__THR_H
#define __ARCHIVE__THR_H
#include <QtGui>
// Tools
#include <ToolsConfig.h>
// Storage
#include <CtrlInfo.h>
#include <RawValue.h>
#include "ArchiveDataCommon.h"
#include "ArchiveDataClient.h"

using namespace std;
class ArchiveThread : public QThread
{
	Q_OBJECT
public:
	ArchiveThread(ArchiveDataClient *client, stdVector<stdString> &names, epicsTime &start, epicsTime &end, int count, int how,double interval=0, void *arg=0);
	virtual ~ArchiveThread();

	void setStartTime(const epicsTime start){mstart=start;};

public slots:

signals:

protected:
    void run();

private slots:

private:
    mutable		QMutex mutex;
    mutable		QWaitCondition cond;
    ArchiveDataClient*	m_arclient;
	stdVector<stdString> mpvnames;
	epicsTime mstart, mend;
	double m_interval;
	const int mcount, mhow;
	void *marg;
};
#endif
