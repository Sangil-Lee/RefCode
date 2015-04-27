#ifndef __QT_CH_ACCESS_THR_H
#define __QT_CH_ACCESS_THR_H

#include <QtGui>
#include <cadef.h>
#include "SinglePlot.h"

class SinglePlot;
typedef struct
{
    chid		mychid;
    evid		myevid;
	const SinglePlot  *plot;
} MYNODE;

class SinglePlotThread : public QThread
{
	Q_OBJECT
public:
	SinglePlotThread(const QString &pvname, SinglePlot *plot, const int periodic = 1);
	virtual ~SinglePlotThread();
	void setStop( const bool stop);
	const bool getStop();

public slots:

signals:

protected:
    void run();
	void printchannel();
	void Epoch2Datetime(int &year, int &month, int &day, int &hour, int &min, int &sec, time_t epochtime=0);

private slots:

private:
	enum PERIODIC{ PointOne	= 0, PointFive, OneSec, FiveSec, TenSec};
	QWidget		*m_pwidget;
    mutable		QMutex mutex;
    mutable		QWaitCondition cond;
	const QString m_pvname;
	SinglePlot *m_plot;
    MYNODE	mynode;
	bool	mstop;
	const int mperiodic;
};
#endif
