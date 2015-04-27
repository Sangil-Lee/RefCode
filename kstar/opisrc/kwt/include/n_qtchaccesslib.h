// qtchaccesslib
#ifndef __QTCHACCESSLIB_HPP
#define __QTCHACCESSLIB_HPP
#include <QtGui>
#include <QUiLoader>

#include <vector>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include <cadef.h>
#include <dbDefs.h>

#include "chaccessdef.h"
#include "qtchaccessthr.h"
#include "caDisplayer.h"
#include "caPushbutton.h"
#include "caBobutton.h"
#include "caCheckbox.h"
#include "caMbbobutton.h"
#include "caLineedit.h"
#include "caLabel.h"
#include "caImageMbbi.h"
#include "caWclock.h"
#include "caGraphic.h"
#include "caMultiplot.h"
#include "caMultiwaveplot.h"
#include "caUitime.h"
#include "password.h"
#include "SinglePlotWindow.h"

using std::string;
using std::vector;

class ChannelAccessThr;

class AttachChannelAccess:public QWidget
{
	Q_OBJECT

public:
	AttachChannelAccess(const QString &uifilename, int index, QWidget *widget = 0);
	AttachChannelAccess(QWidget *widget);
	AttachChannelAccess(QWidget *widget, const QList<QString>& pvlist);
	//AttachChannelAccess(const QString &uifilename, QWidget *widget, const QList<QString>& pvlists, const int index = 0);
	~AttachChannelAccess();

	void Start();
	void PrintData(const double pvalue);
	void UpdateObj(const QString &keyname, const double &dvalue, const short &severity, const short& precision);
	/* shbaek added next UpdateObj for DBR_STS_STRING */
	//++leesi
	void UpdateObj(const QString &keyname, const QString &dvalue, const short &severity, const short& precision = 2);
	void ConnectionStatusObj(const QString &keyname, const short &connstatus);
	QWidget * GetWidget() { return m_pwidget; };
	void SetUiCurIndex(int curindex);

public slots:
	void getValue();
	void capushbuttonpressSlot();
	void capushbuttonreleaseSlot();
	void cabobuttonSlot(bool check);
	void cacheckboxSlot(bool check);
	void cmodechangedSlot(bool cmode);
	void cambbobuttonSlot(int id);
	void calineeditSlot(double value);
	void calineeditSlot(QString value);
	void updateobj(const QString& objname, const short &severity, const short& precision);
	const bool &getControl() const;
	void setControl(const bool &control);

signals:
	void updatesignal(const QString &objname, const short &severity, const short& precision);

protected:
	void mousePressEvent(QMouseEvent *event);
	bool eventFilter(QObject *target, QEvent *event);
	void timerEvent ( QTimerEvent * event );

private:
	void init();
	void init2();
	void init3(const QList<QString>& slist);
	void getchannel(const QWidget *pwidget);
	void removePlots();
	void printchannel();
	void updatevalue();
	void regControlObject(const QWidget *pwidget, const QString& controlObj);
	void timeSet();
#if 0
	const QString &getControlMaster();
	void setControlMaster(const QString &ctrlmaster);
#endif


	QString		m_ctrlmaster;
	bool		m_control;
	bool		m_lastcontrol;
	double			m_data;
	QWidget			*m_pwidget;
	//QList<QString>	m_pvlists;
	QUiLoader		m_loader;
	const QString	m_filename;
	int				m_index;
	int				m_curindex;

	ChAccess		chaccess;
	VecChacc		register_chacc;
	VecChacc::iterator	reg_chacciter;
	ChannelAccessThr *mChThr;

	// shbaek added next 4 lines for control master CA
	ChAccess		master_chaccess;
	VecChacc		master_register_chacc;
	VecChacc::iterator	master_reg_chacciter;
	ChannelAccessThr *master_ChThr;

	void setpvs(const QWidget *pwidget, const QList<QString>& slist);
	//QList<QObject*> tabList;


	// Hash Table Container for Channel Access
	typedef QHash<QString, ChAccess> Hash_Chacc;
	Hash_Chacc hash_chacc;
	QHash<QString, ChAccess>::const_iterator hash_chacciter;

	//smart pointer boost library for memory management.
	typedef boost::shared_ptr<SinglePlotWindow> SinglePlotPtr;
	// shbaek changed following one line to the next to solve smartptr memory corruption problem
	vector<SinglePlotPtr> vecPlotPtr;
	//vector<SinglePlotPtr>* vecPlotPtr;
};
#endif
