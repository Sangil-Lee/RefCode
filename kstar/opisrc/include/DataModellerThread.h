#ifndef __DATAMODELLER_THR_H
#define __DATAMODELLER_THR_H
#include <QtGui>
#include <vector>
#include <string>
#include <cadef.h>
#include "MultiPlot.h"

using namespace std;
class MultiPlot;
#if 0
typedef struct
{
    chid		mychid;
    evid		myevid;
	const MultiPlot  *plot;
} MYNODE;
#endif

class DataModellerThread : public QThread
{
	Q_OBJECT
public:
	DataModellerThread(MultiPlot *plot, const vector<string> &pvnames);
	virtual ~DataModellerThread();

	void SetPvnames(const vector<string> &pvnames);
	void setStop( const bool stop ) { isstop = stop;};
	const bool getStop() { return isstop;};

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
	mutable MultiPlot *m_plot;
	vector<string> m_pvnames;
	vector<chid> m_node;
	bool        isstop;
};
#endif
