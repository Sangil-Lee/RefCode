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
	SinglePlotThread(const QString &pvname, const SinglePlot *plot);
	virtual ~SinglePlotThread();

public slots:

signals:

protected:
    void run();
	void printchannel();

private slots:

private:
	QWidget		*m_pwidget;
    mutable		QMutex mutex;
    mutable		QWaitCondition cond;
	const QString m_pvname;
	const SinglePlot *m_plot;
    MYNODE	mynode;
};
#endif
