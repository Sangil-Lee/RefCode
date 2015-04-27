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
#include <qwt_math.h>
#include "SinglePlotThread.h"

//const int PLOT_SIZE = 201;      // 0 to 200

class SinglePlotThread;
class SinglePlotZoomer;
class SinglePlot:public QwtPlot
{
    Q_OBJECT
public:
    SinglePlot(const QString& pvname, QWidget* = NULL);
    virtual ~SinglePlot();
	//void GetValue( const double &pvval, const int &totsec );
	void GetValue( const double &pvval, struct tm &ansitm );
	void SetSpan( const double &yminspan, const double &ymaxspan );

public slots:
    //void setTimerInterval(int interval);
    void setYMinSpan(double span);
    void setYMaxSpan(double span);
	void setPenWidth(int);
	void setPenColor(int);
	void setLineStyle(int);
signals:
	void updatesignal();
	void updateDate(QString date);

protected:
    //virtual void timerEvent(QTimerEvent *e);

private:
	static const int PLOT_SIZE = 501;
    //SinglePlotZoomer *m_zoomer[2];
    void alignScales();
	double m_yminspan, m_ymaxspan;
    double d_y[PLOT_SIZE]; 

    double timeline[PLOT_SIZE]; 
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
};

#endif
