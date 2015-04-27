#ifndef _MULTI_PLOT_H
#define _MULTI_PLOT_H
#include <QtGui>
#include <time.h>
#include <vector>
#include <string>
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
#include <qwt_data.h>
#include "MultiPlotInfo.h"
#include "DataModellerThread.h"

class DataModellerThread;
class MultiplotMainWindow;

class MultiPlotZoomer:public QwtPlotZoomer
{
public:
    MultiPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);
        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size
        setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
        setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
    }
};
class EpicsTimeScaleDraw:public QwtScaleDraw
{
private:
	int timeinterval;

public:
    EpicsTimeScaleDraw();
	void SetTimeInterval(int interval);
    virtual QwtText label(double totsec) const;
};
class MultiPlot:public QwtPlot
{
	Q_OBJECT
public:
    MultiPlot(QWidget* widget);
    MultiPlot(QWidget* widget, const vector<string> &pvnames);
    virtual ~MultiPlot();
	static const int MAX_PV_CNT = 10;

	void Initialize( const int index, const double &pvval);
	void GetValue( const int index, const double &pvval, const double &totsec, const int& year, const int& month, const int& day );
	void SetSpan( const double &yminspan, const double &ymaxspan );
	QString GetTime(const double& time);
	QwtPlotCurve *getCurve(const int index);
	void setZoom(const bool zoom);
	void SetPenWidth(const int index, const int width);
	void SetPenColor(const int index, const QString strcolor);
	void SetLineStyle(const int index, const int style);
	void MakePlotCurve(const vector<string> &pvnames);
	void Execute(const vector<string> &pvnames);
    MultiPlotZoomer *Zoomer(){ return m_zoomer;};
	void SetAutoScale(const bool check);
	void BufferSwapData();
	long GetTimeInterval();
	void SetTimeInterval(int timeinterval);

public slots:
#if 0
    void setYMinSpan(double);
    void setYMaxSpan(double);
#else
    void setYMinSpan();
    void setYMaxSpan();
#endif
	void Replot();
signals:
	void updatesignal();
	void updateDate(QString date);
	void updateValue(int index, QString value);

protected:
    virtual void timerEvent(QTimerEvent *e);
	void closeEvent (QCloseEvent *);

private:
    MultiPlotZoomer *m_zoomer;
	//static const int PLOT_SIZE  = 1001; //
	static const int PLOT_SIZE  = 1201; // 20minutes.
	//static const int PLOT_SIZE  = 1201*5; // 100minutes.
	int m_pvcnt;
	const bool isZoom();
    void alignScales();
	void printData();
	void init();
	void dataCopy(double dest[],double src[], unsigned int &size);

	double m_yminspan, m_ymaxspan;
    double m_data[MAX_PV_CNT][PLOT_SIZE]; 
    double m_databuf[MAX_PV_CNT][PLOT_SIZE]; 
    double timeline[PLOT_SIZE]; 
	int		moffset;
	bool	mautoscale;
	QwtPlotCurve *pCurve[MAX_PV_CNT];
	QwtDoubleRect mzoombase;

    EpicsTimeScaleDraw *m_etscale;
	DataModellerThread *m_plotthr;
	QPen mpen[MAX_PV_CNT];
	int mpenwidth, mpencolor, mlinestyle;
	int m_year, m_month, m_day;
	bool m_bZoom;
	vector<string> m_pvnames;
	long m_timeinterval;
	//QHash<string, QVector<double> > m_datamodel;
	//QHash<double, QVector<double> > mdatabuffer;
};
#endif
