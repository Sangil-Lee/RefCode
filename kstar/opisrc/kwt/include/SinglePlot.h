#ifndef _SINGLE_PLOT_H
#define _SINGLE_PLOT_H
#include <QtGui>
#include <time.h>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_picker.h>
#include <qwt_math.h>
#include "SinglePlotThread.h"

//const int PLOT_SIZE = 201;      // 0 to 200

class SinglePlotThread;
class SinglePlotZoomer;
class SinglePlot:public QwtPlot
{
    Q_OBJECT
public:
    SinglePlot(const QString& pvname, QWidget* = NULL, const int periodic = 1);
    virtual ~SinglePlot();
	//void GetValue( const double &pvval, const double &totsec );
	//void GetValue( const double &pvval, struct tm &ansitm );
	//void GetValue( const double &pvval, struct local_tm_nano_sec &timenano );
	void GetValue( const double &pvval, const double &totsec, const int& year, const int& month, const int& day );
	void SetSpan( const double &yminspan, const double &ymaxspan );
	void SetSpan( const QString &strmin, const QString &strmax );
	QString GetTime(const double& time);
	void SetUnit(const dbr_string_t unit );
	SinglePlotThread* GetPlotThread(){ return m_plotthr;};


public slots:
    //void setTimerInterval(int interval);
#if 0
    void setYMinSpan(double span);
    void setYMaxSpan(double span);
#else
    void setYMinSpan();
    void setYMaxSpan();
#endif
	void setPenWidth(int);
	//void setPenColor(int);
	void setPenColor(const QString &);
	void setLineStyle(int);

signals:
	void updatesignal();
	void updateDate(QString date);
	void updateLabel(QString );
	void updateLabel1(QString );
	void updateLabel2(QString );
	void updateLabel3(QString );
	void updateLabel4(QString );
	void updateLabel5(QString );

protected:
    virtual void timerEvent(QTimerEvent *e);
	void closeEvent (QCloseEvent *);

private:
	static const int PLOT_SIZE = 501;
    //SinglePlotZoomer *m_zoomer[2];
    void alignScales();
	void printData();
	double m_yminspan, m_ymaxspan;
    double d_y[PLOT_SIZE]; 
    double timeline[PLOT_SIZE]; 
	int		moffset;
	bool	autoscale;
	//double	mtotsec;
	QwtPlotCurve *cLeft;
	QVector<double> vecDataX;
	QVector<double> vecDataY;

    int d_interval; // timer in ms
    int d_timerId;
	const QString m_pvname;

	SinglePlotThread *m_plotthr;
	QPen mpen;
	int mpenwidth,mpencolor,mlinestyle;
	int m_year, m_month, m_day;
    QwtPlotPicker	*mpicker;
	int mCount;
	const int mperiodic;
};

#endif
