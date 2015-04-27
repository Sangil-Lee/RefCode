// qtchaccesslib
#ifndef __QTCHACCESSLIB_HPP
#define __QTCHACCESSLIB_HPP
#include <QtCore>
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <QWidget>

#include <vector>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include <cadef.h>
#include <dbDefs.h>

#include "chaccessdef.h"
#include "caDisplayer.h"
#include "qtchaccessthr.h"
#include "SinglePlotWindow.h"
#include "caPushbutton.h"
#include "caUitime.h"
#if 1
#include "caBobutton.h"
#include "caCheckbox.h"
#include "caMbbobutton.h"
#include "caComboBox.h"
#include "caLineedit.h"
#include "caLabel.h"
#include "caImageMbbi.h"
#include "caWclock.h"
#include "caGraphic.h"
#include "caMultiplot.h"
#include "caMultiwaveplot.h"
#include "password.h"
#include "caQtSignal.h"
#include "caScreenShot.h"
#include "caScheduler.h"
#include "caSlider.h"
#include "caProgressBar.h"
#include "caColorTilebutton.h"
#include "caTableList.h"
#include "caPopup.h"
//#include "caWaveformput.h"
#endif


using std::string;
using std::vector;


QT_BEGIN_NAMESPACE
class QString;
class QUiLoader;
class ChannelAccessThr;
QT_END_NAMESPACE


class AttachChannelAccess:public QWidget
{
	Q_OBJECT

public:
	AttachChannelAccess(const QString &uifilename, int index, bool bmonitor = false, QWidget *widget = 0);
	~AttachChannelAccess();

	void SetUiCurIndex(const int curindex);
	void ConnectionStatusObj(const QString &keyname, const short &connstatus);
	void UpdateObj(const QString &keyname, const QString &dvalue, const short &severity, const short& precision = 2);
	void UpdateObj(const QString &keyname, const double &dvalue, const short &severity, const short& precision);
	QWidget * GetWidget() { return m_pwidget; };


public slots:
	void getValue();

	void capushbuttonpressSlot();
	void capushbuttonreleaseSlot();
	void cabobuttonSlot(bool check);
	void cacheckboxSlot(bool check);
	void cambbobuttonSlot(int id);
	void cacomboboxSlot(int value);
	void calineeditSlot(double value);
	void casliderSlot();
	void actionSlot(int type);
	void calineeditSlot(QString value);

	void updateobj(const QString& objname, const short &severity, const short& precision);
	const bool &getControl() const;
	void setControl(const bool &control);

signals:
	void updatesignal(const QString &objname, const short &severity, const short& precision);

protected:
	bool eventFilter(QObject *target, QEvent *event);
	void timerEvent ( QTimerEvent * event );

private:
	void init();
	void timeSet();
    void regControlObject(const QWidget *pwidget, const QString& controlObj);
	void removePlots();
	void getchannel(const QWidget *pwidget);
	void caPutList(const QStringList &pvlist, const QVariant &value);

	QString		m_ctrlmaster;
	bool		m_control;
	bool		m_lastcontrol;
	double			m_data;
	QWidget			*m_pwidget;
	QUiLoader		m_loader;
	const QString	m_filename;
	int				m_index;
	int				m_curindex;
	bool            m_bmon;

	ChAccess		chaccess;
	VecChacc		register_chacc;
	VecChacc::iterator	reg_chacciter;

	ChannelAccessThr *mChThr;

	// Hash Table Container for Channel Access
	typedef QHash<QString, ChAccess> Hash_Chacc;
	Hash_Chacc hash_chacc;
	QHash<QString, ChAccess>::const_iterator hash_chacciter;

	//smart pointer boost library for memory management.
    typedef boost::shared_ptr<SinglePlotWindow> SinglePlotPtr;
    // shbaek changed following one line to the next to solve smartptr memory corruption problem
    vector<SinglePlotPtr> vecPlotPtr;


};
#endif
