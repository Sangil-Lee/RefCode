#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <epicsTime.h>
#include "MultiPlot.h"

using namespace std;
class SimpleData: public QwtData
{
    // The x values depend on its index and the y values
    // can be calculated from the corresponding x value. 
    // So we don´t need to store the values.
    // Such an implementation is slower because every point 
    // has to be recalculated for every replot, but it demonstrates how
    // QwtData can be used.

public:
    SimpleData(double(*y)(double), size_t size):
        d_size(size),
        d_y(y)
    {
    }

    virtual QwtData *copy() const
    {
        return new SimpleData(d_y, d_size);
    }

    virtual size_t size() const
    {
        return d_size;
    }

    virtual double x(size_t i) const
    {
        return 0.1 * i;
    }

    virtual double y(size_t i) const
    {
        return d_y(x(i));
    }
private:
    size_t d_size;
    double(*d_y)(double);
};

EpicsTimeScaleDraw::EpicsTimeScaleDraw():timeinterval(1)
{
};
void EpicsTimeScaleDraw::SetTimeInterval(int interval)
{ 
	timeinterval = interval;
};
QwtText EpicsTimeScaleDraw::label(double totsec) const
{
#if 1
	int hour = (int)totsec/3600;
	int min = ((int)totsec%3600)/60;
	int sec = ((int)totsec%3600)%60;
	QTime upTime(hour, min,sec);
	return upTime.toString();
#else
	return QwtText("");
#endif
};

//  Initialize main window
MultiPlot::MultiPlot(QWidget *parent):QwtPlot(parent)
{
	init();
}

MultiPlot::MultiPlot(QWidget *parent, const vector<string> &pvnames)
	:QwtPlot(parent),m_pvnames(pvnames)
{
}

MultiPlot::~MultiPlot()
{
	delete m_plotthr;
}
void
MultiPlot::init()
{
    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);

    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, true);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, true);
    alignScales();
	setAutoReplot(false);
	mautoscale = true;
	m_bZoom = false;
	m_timeinterval = 1000000;
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->setMajPen(QPen(Qt::black, 0, Qt::DashLine));
	grid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	grid->attach(this);
    setCanvasLineWidth(2);

	// Only Static Plot, first set freeze
    m_zoomer = new MultiPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, this->canvas());
    m_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    m_zoomer->setRubberBandPen(QColor(Qt::green));
    m_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    m_zoomer->setTrackerPen(QColor(Qt::white));

    m_etscale = new EpicsTimeScaleDraw();
    setAxisScaleDraw(QwtPlot::xBottom, m_etscale);
    //setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    //setAxisScaleDraw(QwtPlot::xBottom, new EpicsTimeScaleDraw());
    //setAxisScale(QwtPlot::xBottom, 0, PLOT_SIZE);
    //setAxisLabelRotation(QwtPlot::xBottom, 0.0);
#if 0
	QWidget *timebox = new QWidget(this);
    timebox->setGeometry(QRect(0, 400, 150, 30));
	QHBoxLayout *tmlayout = new QHBoxLayout(timebox);

	QLabel *timelabel = new QLabel("Test---->", timebox);
	tmlayout->addWidget(timelabel);
#endif

	m_plotthr = 0;
	for(int i = 0; i<MAX_PV_CNT;i++)
	{
		pCurve[i] = 0;
	};

	// Y-Values
    setAxisTitle(QwtPlot::yLeft, "Values");
	connect(this,SIGNAL(updatesignal()),SLOT(Replot()) );
	//startTimer(5000);
}
void
MultiPlot::Execute(const vector<string> &pvnames)
{
	if (!m_plotthr)
	{
		m_plotthr = new DataModellerThread(this, pvnames);
		m_plotthr->start();
	}
	else
	{
		m_plotthr->setStop(true);
		m_plotthr=0;
		m_plotthr = new DataModellerThread(this, pvnames);
		m_plotthr->start();
	};
	replot();
}

QwtPlotCurve *
MultiPlot::getCurve(const int index)
{
	if(!pCurve[index]) return 0;
	return pCurve[index];
}

void
MultiPlot::closeEvent (QCloseEvent *)
{
	m_plotthr->setStop(true);
	delete m_plotthr;
}

void MultiPlot::SetSpan( const double &yminspan, const double &ymaxspan )
{ 
	m_yminspan = yminspan;
	m_ymaxspan = ymaxspan;
}
QString MultiPlot::GetTime(const double &time)
{
	int hour = (int)time/3600;
	int min = ((int)time%3600)/60;
	int sec = ((int)time%3600)%60;
	char strtime[20];
	sprintf(strtime,"%02d:%02d:%02d",hour, min, sec);
	return QString(strtime);
};
void MultiPlot::dataCopy(double dest[], double src[], unsigned int &size)
{ 
	copy(dest, dest+size, src);
};
void MultiPlot::BufferSwapData()
{ 
	size_t pvcnt = m_pvnames.size();
	unsigned int size = PLOT_SIZE; 
	for (size_t i=0; i<pvcnt ;i++)
	{
		dataCopy(m_databuf[i], m_data[i], size);
	};
};
void MultiPlot::Replot()
{ 
	mzoombase.setLeft(timeline[PLOT_SIZE-1]);
	mzoombase.setRight(timeline[0]);
	mzoombase.setTop(m_yminspan);
	mzoombase.setBottom(m_ymaxspan);
	m_zoomer->setZoomBase(mzoombase);
	replot();
}
void MultiPlot::Initialize( const int index, const double &pvval)
{ 
	for ( int i = 0; i < PLOT_SIZE;i++ )
	{
		m_data[index][i] = pvval;
	};
};
void MultiPlot::GetValue( const int index, const double &pvval, const double &totsec, const int& year, const int& month, const int& day )
{ 
	//std::rotate(&timeline[0], &timeline[1], &timeline[PLOT_SIZE]);	//good
	// Data Shift
	if ( isZoom() == true ) 
	{
		for ( int i = PLOT_SIZE - 1; i > 0; i-- )
		{
			m_databuf[index][i] = m_databuf[index][i-1];
		};
		m_databuf[index][0] = pvval;
		return;
	};
	for ( int i = PLOT_SIZE - 1; i > 0; i-- )
	{
		m_databuf[index][i] = m_data[index][i] = m_data[index][i-1];
	};
	m_databuf[index][0] = m_data[index][0] = pvval;
#if 1
	if ( index == 0 )
	{
#if 0
		for ( int j = 0; j<PLOT_SIZE; j++ )
		{
			timeline[j] = (totsec-j);
			qDebug("timeline:%lf",timeline[j]);
		};
#else
		timeline[0] = totsec;
		for ( int j = 0; j<PLOT_SIZE; j++ )
		{
			timeline[j+1] = timeline[j]-(m_timeinterval/1000000);
			//qDebug("totSec:%lf, timeline:%lf,%lf",totsec, timeline[j], m_timeinterval/1000000);
		};
#endif
		//qDebug("FirstTime:%lf, LastTime:%lf,DiffTime:%lf",timeline[PLOT_SIZE-1],timeline[0], timeline[PLOT_SIZE-1]-timeline[0]);
		setAxisScale(QwtPlot::xBottom, timeline[PLOT_SIZE-1], timeline[0]);
	};
#endif
	if((m_year != year+1900 || m_month != month+1 || m_day != day) && isZoom() == false )
	{
		m_year    = year+1900;
		m_month   = month+1;
		m_day     = day;
		QDate date(m_year, m_month, m_day);
		QString datetime = "Date: "+date.toString(Qt::ISODate);
		emit updateDate(datetime);
	};
	if (mautoscale == true)
	{
		size_t pvcnt = m_pvnames.size();
		double maxval[pvcnt];
		double minval[pvcnt];
		for (size_t i=0; i<pvcnt; i++)
		{
			if(pCurve[i]==NULL) continue;
			maxval[i] = pCurve[i]->maxYValue();
			minval[i] = pCurve[i]->minYValue();
		};
		m_yminspan = *min_element(&minval[0], &minval[pvcnt-1]);
		m_ymaxspan = *max_element(&maxval[0], &maxval[pvcnt-1]);
	};

	QString strValue = QString("%1").arg(pvval, 0);
	emit updateValue(index, strValue);
	emit updatesignal();
}
void MultiPlot::printData()
{
}
//  Set a plain canvas frame and align the scales to it
//
void MultiPlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.
    canvas()->setFrameStyle(QFrame::Box | QFrame::Sunken);
    canvas()->setLineWidth(1);
    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget ) scaleWidget->setMargin(0);
        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw ) scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}
#if 0
void MultiPlot::setYMinSpan(double span)
{
	mautoscale = false;
	m_yminspan = span;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	//replot();
}
void MultiPlot::setYMaxSpan(double span)
{	
	mautoscale = false;
	m_ymaxspan = span;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	//replot();
}
#else
void MultiPlot::setYMinSpan()
{
	QLineEdit *minspan = qobject_cast<QLineEdit *>(sender());
	QVariant varspan = QVariant(minspan->text());

	bool check;
	double span = varspan.toDouble(&check);

	if (check == false)
	{
		QMessageBox::information(this, "Error!","Input Error: "+minspan->text());
	}
	else
	{
		//qDebug("%.23f", span);
		mautoscale = false;
		m_yminspan = span;
		setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
		replot();
	}
}
void MultiPlot::setYMaxSpan()
{
	QLineEdit *maxspan = qobject_cast<QLineEdit *>(sender());
	QVariant varspan = QVariant(maxspan->text());
	bool check;
	double span = varspan.toDouble(&check);

	if (check == false)
	{
		QMessageBox::information(this, "Error!","Input Error: "+maxspan->text());
	}
	else
	{
		mautoscale = false;
		m_ymaxspan = span;
		setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
		replot();
	}
}
#endif
void MultiPlot::timerEvent(QTimerEvent *)
{
	static int timereventcnt = 0;
	qDebug("timerevent:%d", timereventcnt++);
};
const bool MultiPlot::isZoom()
{ 
	return m_bZoom;
}
void MultiPlot::setZoom( const bool zoom)
{ 
	m_bZoom = zoom;
	if (zoom==true)
	{
	};
}
void MultiPlot::MakePlotCurve(const vector<string> &pvnames)
{
	m_pvnames = pvnames;
    for (int i = 0; i< PLOT_SIZE; i++)
    {
		for (size_t j = 0; j<pvnames.size();j++)
		{
			m_databuf[j][i] = m_data[j][i] = 0;
		};
		timeline[PLOT_SIZE-i] = i;
    };
	for(int i = 0; i<MAX_PV_CNT; i++ )
	{
		if(pCurve[i] != 0)
		{
			pCurve[i]->detach();
		};
	};
	for(size_t index = 0; index<pvnames.size(); index++ )
	{
		pCurve[index] = new QwtPlotCurve();
		pCurve[index]->setPen(mpen[index]);
		pCurve[index]->setStyle(QwtPlotCurve::Steps);
		pCurve[index]->setRawData(timeline, m_data[index], PLOT_SIZE);
		pCurve[index]->attach(this);
	};
};
void MultiPlot::SetPenWidth(const int index, const int width)
{
	mpen[index].setWidth(width+1);
    pCurve[index]->setPen(mpen[index]);
}
void MultiPlot::SetPenColor(const int index, const QString strcolor)
{
	mpen[index].setColor(QColor(strcolor));
    pCurve[index]->setPen(mpen[index]);
	replot();
}
void MultiPlot::SetLineStyle(const int index, const int style)
{
	switch (style)
	{
		case 1:
			pCurve[index]->setStyle(QwtPlotCurve::Lines);
			break;
		case 2:
			pCurve[index]->setStyle(QwtPlotCurve::Dots);
			break;
		case 3:
			pCurve[index]->setStyle(QwtPlotCurve::Sticks);
			break;
		case 0:
		default:
			pCurve[index]->setStyle(QwtPlotCurve::Steps);
			break;
	}
	replot();
}
void MultiPlot::SetAutoScale(const bool check)
{
	mautoscale=check;
}
long MultiPlot::GetTimeInterval()
{
	return m_timeinterval;
}
void MultiPlot::SetTimeInterval(int timeinterval)
{
	m_etscale->SetTimeInterval(timeinterval);
	m_timeinterval = timeinterval*1000000;
}
