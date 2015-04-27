#ifndef __QT_CH_ACCESS_THR_H
#define __QT_CH_ACCESS_THR_H

#include <QtGui>
#include <vector>
#include "chaccessdef.h"
#include "qtchaccesslib.h"
//#include "cachedchannelaccess.h"

using std::vector;
using std::string;

class AttachChannelAccess;

class CHNODE
{
public:
	CHNODE():chindex(0),objname(QString("")),pvname(QString("")),
			 status(0), onceConnected(0), acacc(NULL)
	{
		memset(&ch_id, 0, sizeof(chid));
		memset(&ev_id, 0, sizeof(ev_id));
	};
	unsigned int    chindex;
	QString			objname;
	QString			pvname;
    chid			ch_id;
	evid			ev_id;
	chtype			dbfType;
	int				status;
	char			onceConnected;
	AttachChannelAccess	*acacc;
	//++leesi
	int             dbrequest;
	QTableWidgetItem *pItem;
};

class ChannelAccessThr : public QThread
{
	Q_OBJECT
public:
	//ChannelAccessThr(VecChacc &vecchacc, QWidget *widget, QObject *parent = 0);
	ChannelAccessThr(VecChacc &vecchacc, AttachChannelAccess *pattach, QObject *parent = 0);
	virtual ~ChannelAccessThr();
	void setStop(bool bstop){mstop = bstop;};
	bool getStop(){return mstop;};
	bool getClearEvents();
	bool getAddEvents();
	void setClearEvents(const bool bevent);
	void setAddEvents(const bool bevent);
	void ClearEventsOnPage(const int page);
	void AddEventsOnPage(const int page);

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
	VecChacc	register_chacc;
	VecChacc::iterator	reg_chacciter;
	AttachChannelAccess	*m_pattach;
	bool        mstop;
	vector<CHNODE> vecnode;
	bool mbclearev;
	bool mbaddev;
	//CachedChannelAccess mCachedData;
};
#endif
