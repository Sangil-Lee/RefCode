#ifndef __CYBER_LOG_VIEW_H
#define __CYBER_LOG_VIEW_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mysql++.h>
#include <iostream>
#include <kstardbconn.h>
#include <qwt_global.h>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qwt_data.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>


using namespace std;


class CyberLogView:public QWidget
{
	Q_OBJECT
public:
	CyberLogView();
	virtual ~CyberLogView();

	void ReadCyberLogData();
	bool GetAuto(){return mbAuto;};
	void SetAuto(const bool bauto){mbAuto=bauto;};
	QString GetHostname(){return mhostname;};
	void Update();
	void LastShotNumber(const unsigned int lastshot){mshotno = lastshot;};
	mysqlpp::Connection & getConnection(){return m_con;};
	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};

public slots:
	void dbSearch();
	void saveToMySQL(int,int);
	void makeSession();
	void showDetail();
	void showLoglist();
	void LogSelected();

signals:
	void Search();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void timerEvent(QTimerEvent *event);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	int messageBox(QString msg, const int type=1);
	string string2hex(const string& inputstr, bool upper_case);
	string string2hex(const string& inputstr);
	string hex_to_string(const string& input);
	string bitstring(unsigned int bytedata);
	QwtPlot *mpPlot;
	QwtPlotCurve *mpCurve;

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QWidget	*m_ploglist;
	QWidget	*m_plogdetail;
	QTableWidget *m_ptable;
	QTableWidget *m_psummary;
	bool mbstart;
	int  mshottype;
	QString mhostname;

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;

	QLineEdit *mleTestID;
	QLineEdit *mleTestTime;
	QLineEdit *mleDesc;

	QCheckBox *mckToday;
	QCheckBox *mckDesc;
	QComboBox *mcbSort;
	QComboBox *mcbModify;
	QPushButton *mpbSearch;

	QLabel *mlbToday;
	QDateTimeEdit *mdtStart;
	QDateTimeEdit *mdtEnd;
	QLineEdit *mleStart;
	QLineEdit *mleEnd;
	QLineEdit *mleSestitle;
	QLineEdit *mleSesleader;
	QLineEdit *mleSescmo;
	QSpinBox	  *msbShotNo;
	vector<QString> mPreval,mCurval;
	unsigned int	mshotno;
	bool mbAuto;
	QPrinter	mprint;
	QPrinter	mpdfprint;
	QPixmap		mpixmap;
};
#endif
