/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  ipCheckThr.h
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 *
 * Revision 1  2006-12-29
 * Author: Sangil Lee [silee]
 * Initial revision
 *
 * ---------------------------------------------------------------------------
 *  * DESCRIPTION *
 * ---------------------------------------------------------------------------
 *
 */
#ifndef __QT_IPCHECK_THR_H
#define __QT_IPCHECK_THR_H

#include <QtGui>
#include <vector>
#include "controlmonitorlib.h"

using std::vector;
using std::string;

#if 0
typedef struct IPCheck {
	QWidget *pobj;
	QString ipAddr;
	int		status;
};
#endif

class ControlMonitorChannel;

class IPCheckThr : public QThread
{
	Q_OBJECT
public:
	IPCheckThr(ControlMonitorChannel *pattach, QObject *parent = 0);
	virtual ~IPCheckThr();
	void setStop(bool bstop){mstop = bstop;};
	bool getStop(){return mstop;};


public slots:

signals:

protected:
    void run();

private slots:

private:
	QWidget		*m_pwidget;
	ControlMonitorChannel *m_pattach;
    mutable		QMutex mutex;
    mutable		QWaitCondition cond;
	bool        mstop;
};
#endif
