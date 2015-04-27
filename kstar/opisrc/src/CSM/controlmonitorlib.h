/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  qtchaccesslib.h
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
#ifndef __CONTROLMONITOR_LIB_HPP
#define __CONTROLMONITOR_LIB_HPP
#include <QtGui>
#include <QUiLoader>

#include <vector>
#include <string>
#include <list>

#include <cadef.h>
#include <dbDefs.h>

//#include "chaccessdef.h"
#include "csmchaccessdef.h"
#include "controlmonitorthr.h"
#include "ipCheckThr.h"
#include "caDisplayer.h"
#include "caGraphic.h"
#include "caLabel.h"
#include "caBlinkLabel.h"
#include "blinkLine.h"
#include "caListBox.h"
#include "caUitime.h"

using std::string;
using std::vector;

class ControlMonitorThr;
class IPCheckThr;

class ControlMonitorChannel:public QWidget
{
	Q_OBJECT
public:
	ControlMonitorChannel(const QString &uifilename, int index, QWidget *widget = 0);
	//ControlMonitorChannel(const QString &uifilename, int index, const QString &prefix = 0);
	ControlMonitorChannel(const QString &uifilename, int index, const QString &prefix, const QString &windowname);
	ControlMonitorChannel(QWidget *widget);
	ControlMonitorChannel(QWidget *widget, const QList<QString>& pvlist);
	~ControlMonitorChannel();

	void Start();
	void PrintData(const double pvalue);
	void UpdateObj(const QString &keyname, const double &dvalue, const short &severity, const short& precision);
	//++leesi
	void UpdateObj(const QString &keyname, const QString &dvalue, const short &severity, const short& precision = 2);
	void ConnectionStatusObj(const QString &keyname, const short &connstatus);
	QWidget * GetWidget() { return m_pwidget; };
	void SetUiCurIndex(int curindex);
	QString getWindowName() {return m_windowname;};

#if 1
	typedef struct IPCheck {
		QWidget *pobj;
		QString ipAddr;
		int		status;
	};
#endif

	IPCheck ipcheck,*pipcheck;
	typedef QHash<QString, IPCheck> Hash_IPCheck;
	Hash_IPCheck hash_ipcheck;
	QHash<QString, IPCheck>::const_iterator hash_ipcheckiter;
	IPCheckThr *mipCheckthr;

public slots:
	void getValue();
	void updateobj(const QString& objname, const short &severity, const short& precision);
	const bool &getControl() const;
	void setControl(const bool &control);

signals:
	void updatesignal(const QString &objname, const short &severity, const short& precision);

protected:
	void mousePressEvent(QMouseEvent *event);
	bool eventFilter(QObject *target, QEvent *event);
	void timerEvent(QTimerEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
	void init1();
	void init2();
	void init3(const QList<QString>& slist);
	void getchannel(const QWidget *pwidget);
	void removePlots();
	void printchannel();
	void updatevalue();
	void regControlObject(const QWidget *pwidget, const QString& controlObj);
	void timeSet();

	QPixmap		mpixmap;
	QString		m_ctrlmaster;
	bool		m_control;
	bool		m_lastcontrol;
	double			m_data;
	QWidget			*m_pwidget;
	QUiLoader		m_loader;
	const QString	m_filename;
	QString			m_prefix;
	QString			m_windowname;
	int				m_index;
	int				m_curindex;
	int				mhealthcount;
	vector	<int> vecICSHealth;
	QLabel			*m_ptimeLabel;

	ChAccess		chaccess;
	VecChacc		register_chacc;
	VecChacc::iterator	reg_chacciter;
	ControlMonitorThr *mChThr;


	void setpvs(const QWidget *pwidget, const QList<QString>& slist);

	// Hash Table Container for Channel Access
	typedef QHash<QString, ChAccess> Hash_Chacc;
	Hash_Chacc hash_chacc;
	QHash<QString, ChAccess>::const_iterator hash_chacciter;
};
#endif
