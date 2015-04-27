/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  qtchaccessthr.h
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
#ifndef __QT_CH_ACCESS_THR_H
#define __QT_CH_ACCESS_THR_H

#include <QtGui>
#include <vector>
#include "csmchaccessdef.h"
#include "controlmonitorlib.h"

using std::vector;
using std::string;

class ControlMonitorChannel;

class ControlMonitorThr : public QThread
{
	Q_OBJECT
public:
	ControlMonitorThr(VecChacc &vecchacc, ControlMonitorChannel *pattach, QObject *parent = 0);
	virtual ~ControlMonitorThr();
	void setStop(bool bstop){mstop = bstop;};
	bool getStop(){return mstop;};


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
	ControlMonitorChannel	*m_pattach;
	bool        mstop;
};
#endif
