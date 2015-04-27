#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <epicsTime.h>
#include <QMessageBox>
#include "SinglePlot.h"
#include "SinglePlotWindow.h"

using namespace std;

class EpicsTimeScaleDraw:public QwtScaleDraw
{
public:
    EpicsTimeScaleDraw(){};
#if 0
    virtual QwtText label(double) const
    {
        return QwtText("");
    };
#else
    virtual QwtText label(double totsec) const
    {
		int hour = (int)totsec/3600;
		int min = ((int)totsec%3600)/60;
		int sec = ((int)totsec%3600)%60;
        QTime upTime(hour, min,sec);
        return upTime.toString();
    };
#endif
};

class SinglePlotZoomer:public QwtPlotZoomer
{
public:
    SinglePlotZoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):QwtPlotZoomer(xAxis, yAxis, canvas)
    {
       // setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);
        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size
        setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
        setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
    }
};

//  Initialize main window
SinglePlot::SinglePlot(const QString &pvname, QWidget *parent, const int periodic): QwtPlot(parent),m_pvname(pvname),mpen(Qt::blue),mCount(0),mperiodic(periodic)
{
    // Disable polygon clipping
	//Qwt 5.x
    //QwtPainter::setDeviceClipping(false);
	//Qwt 6.1.x
    QwtPainter::setPolylineSplitting(false);
	//leesi
	//d_interval = 1000;
	//d_timerId = -1;

    mpicker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::RectRubberBand, 
					QwtPicker::ActiveOnly, canvas());
    mpicker->setRubberBandPen(QColor(Qt::green));
	mpicker->setTrackerPen(QColor(Qt::black));


    // We don't need the cache here
    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);
	QwtPlotCanvas *pCanvas = (QwtPlotCanvas*)canvas(); 
    //pCanvas->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    pCanvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, true);

    alignScales();
	setAutoReplot(false);
    
	autoscale = true;
    //  Initialize data
    for (int i = 0; i< PLOT_SIZE; i++)
    {
        d_y[i] = 0;
		timeline[PLOT_SIZE - 1 - i] = i;
    };
	vecDataX.reserve(PLOT_SIZE);
	vecDataY.reserve(PLOT_SIZE);

	//std::transform(vecDataX.begin(), vecDataX.end(), vecDataX.begin(), bind2nd(multiplies<double>(),0.5) );
	vecDataX.fill(0, PLOT_SIZE);
	vecDataY.fill(0, PLOT_SIZE);

    // Assign a title
    //setTitle("A Test for High Refresh Rates");
    setTitle(m_pvname);
    //insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

    cLeft = new QwtPlotCurve();

    // Set curve styles
    cLeft->setPen(mpen);
    cLeft->setStyle(QwtPlotCurve::Lines);

    // Attach (don't copy) data. Both curves use the same x array.
	// leesi++ for dynamic flow
    //cLeft->setRawData(timeline, d_y, PLOT_SIZE);

    cLeft->attach(this);

	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->setMajorPen(QPen(Qt::black, 0, Qt::DashLine));
	grid->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
	grid->attach(this);
    //setCanvasLineWidth(2);
    setLineWidth(2);

#if 1
	// Only Static Plot
    m_zoomer[0] = new SinglePlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, (QwtPlotCanvas*)canvas());
    m_zoomer[0]->setRubberBand(QwtPicker::RectRubberBand);
    m_zoomer[0]->setRubberBandPen(QColor(Qt::green));
    m_zoomer[0]->setTrackerMode(QwtPicker::ActiveOnly);
    m_zoomer[0]->setTrackerPen(QColor(Qt::white));
    m_zoomer[0]->setEnabled(true);
    m_zoomer[1] = new SinglePlotZoomer(QwtPlot::xTop, QwtPlot::yRight, (QwtPlotCanvas*)canvas());
    m_zoomer[1]->setEnabled(true);
#endif
#if 0
	//  Insert zero line at y = 0
	QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    mY->setLineStyle(QwtPlotMarker::HLine);
    mY->setYValue(0.0);
    mY->attach(this);
#endif
    setAxisScaleDraw(QwtPlot::xBottom, new EpicsTimeScaleDraw());
    //setAxisTitle(QwtPlot::xBottom, "\nTime");
    setAxisTitle(QwtPlot::xBottom, "Time");
    setAxisScale(QwtPlot::xBottom, 0, PLOT_SIZE);
    setAxisLabelRotation(QwtPlot::xBottom, 0.0);

	// Y-Values
    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, -10, 10);
	//
	connect(this,SIGNAL(updatesignal()),this,SLOT(replot()) );

	//leesi++ for test
    //setTimerInterval(1000); 
	//(void)startTimer(100); // 1 second
	m_plotthr = new SinglePlotThread(m_pvname, this, mperiodic);
	m_plotthr -> start();
}

SinglePlot::~SinglePlot()
{
	//m_plotthr->setStop(true);
	//delete m_plotthr;
}

void
SinglePlot::closeEvent (QCloseEvent *)
{
	//++leesi, for exit of QThread safely.
	m_plotthr->setStop(true);
	if(m_plotthr->isRunning()) m_plotthr->wait();
	delete m_plotthr;
}

void SinglePlot::SetSpan( const double &yminspan, const double &ymaxspan )
{
	m_yminspan = yminspan;
	m_ymaxspan = ymaxspan;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	replot();
}
void SinglePlot::SetSpan( const QString &strmin, const QString &strmax )
{ 
	QVariant varminspan = QVariant(strmin);
	QVariant varmaxspan = QVariant(strmax);
	bool check1, check2;
	double span1 = varminspan.toDouble(&check1);
	double span2 = varmaxspan.toDouble(&check2);

	if ( check1 == false || check1 == false)
	{
		QMessageBox::information(this, "Error!","Input Error: Invalid value");
		return;
	};
	m_yminspan = span1;
	m_ymaxspan = span2;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	replot();
}
void SinglePlot::setPenWidth(int width)
{
	mpenwidth = width;
	mpen.setWidth(mpenwidth+1);
    cLeft->setPen(mpen);
	replot();
}
//void SinglePlot::setPenColor(int color)
void SinglePlot::setPenColor(const QString &strcolor)
{
#if 0
	QString strcolor;
	switch(color)
	{
		case 1:
			strcolor = "red";
			break;
		case 2:
			strcolor = "black";
			break;
		case 3:
			strcolor = "green";
			break;
		case 4:
			strcolor = "dark blue";
			break;
		case 0:
		default:
			strcolor = "blue";
			break;
	}
#endif
	mpen.setColor(QColor(strcolor));
    cLeft->setPen(mpen);
	replot();
}
void SinglePlot::setLineStyle(int style)
{
	//items<<"Lines"<<"Steps"<<"Scatters";
	switch (style)
	{
		case 1:
			cLeft->setStyle(QwtPlotCurve::Steps);
			break;
		case 2:
			cLeft->setStyle(QwtPlotCurve::Dots);
			break;
		case 3:
			cLeft->setStyle(QwtPlotCurve::Sticks);
			break;
		case 0:
		default:
			cLeft->setStyle(QwtPlotCurve::Lines);
			break;
	}
	replot();
}
#if 0
bool SinglePlot::GetMicroTime ( long *sec, double *msec )
{
	struct timeval tp;
	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;

		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		return true;
	}
	return false;
};
#endif
QString SinglePlot::GetTime(const double &time)
{
	int hour = (int)time/3600;
	int min = ((int)time%3600)/60;
	int sec = ((int)time%3600)%60;
	char strtime[20];
	sprintf(strtime,"%02d:%02d:%02d",hour, min, sec);
	return QString(strtime);
};
void SinglePlot::GetValue( const double &pvval, const double &totsec, const int& year, const int& month, const int& day )
{ 
	//std::rotate(&timeline[0], &timeline[1], &timeline[PLOT_SIZE]);	//good
	// Data Shift
	//double totsec = timenano.ansi_tm.tm_hour*3600+timenano.ansi_tm.tm_min*60+timenano.ansi_tm.tm_sec;
#if 1
	//int xscale = mCount;
	//if(mCount == PLOT_SIZE) xscale = PLOT_SIZE - 1;
	//for ( int i = PLOT_SIZE - 1; i > 0; i-- )
	for ( int i = mCount; i > 0; i-- )
	{
		d_y[i] = d_y[i-1];
	};
    d_y[0] = pvval;

	if(mCount < PLOT_SIZE) mCount++;

#if 0
	double offset = 0.0;
	if (autoscale == true)
	{
		m_ymaxspan = *max_element(&d_y[0], &d_y[PLOT_SIZE]);
		m_yminspan = *min_element(&d_y[0], &d_y[PLOT_SIZE]);
		offset = m_ymaxspan - m_ymaxspan;
	};
	setAxisScale(QwtPlot::yLeft, m_yminspan - offset, m_ymaxspan + offset);
#endif
	for ( int j = 0; j<PLOT_SIZE; j++ )
	{
		timeline[j] = (totsec-j);
	};
	setAxisScale(QwtPlot::xBottom, timeline[PLOT_SIZE-1], timeline[0]);
	if(m_year != year+1900 || m_month != month+1 || m_day != day )
	{
		m_year    = year+1900;
		m_month   = month+1;
		m_day     = day;

		QDate date(m_year, m_month, m_day);
		QString datetime = "Date: "+date.toString(Qt::ISODate);
		emit updateDate(datetime);
	};

	// leesi++
	//this line has a problem, u should use it one time in initial routine.
	//setRawData use replot finally.
	// leesi++ for dynamic flow
    //cLeft->setRawData(timeline, d_y, mCount);
    cLeft->setRawSamples(timeline, d_y, mCount);
	emit updatesignal();
#if 0
	QString timestamp = GetTime(totsec-50);
	emit updateLabel(timestamp);
	timestamp = GetTime(totsec-40);
	emit updateLabel1(timestamp);
	timestamp = GetTime(totsec-30);
	emit updateLabel2(timestamp);
	timestamp = GetTime(totsec-20);
	emit updateLabel3(timestamp);
	timestamp = GetTime(totsec-10);
	emit updateLabel4(timestamp);
	timestamp = GetTime(totsec);
	emit updateLabel5(timestamp);
#endif
#else
	//qDebug("XSize:%d", vecDataX.size());
	if (vecDataX.size() == PLOT_SIZE ) 
	{
		vecDataX.erase(vecDataX.begin());
	}
	vecDataX.push_back(totsec);
	if (vecDataY.size() == PLOT_SIZE ) 
	{
		vecDataY.erase(vecDataY.begin());
	};
	vecDataY.push_back(pvval);
#endif
}
void SinglePlot::printData()
{
#if 0
	QVector<double>::const_iterator xdataiter,ydataiter;
	for ( xdataiter = vecDataX.begin(); xdataiter != vecDataX.end(); ++xdataiter )
	{
		qDebug("time:%f", *xdataiter);
	}
	for ( ydataiter = vecDataY.begin(); ydataiter != vecDataY.end(); ++ydataiter )
	{
		qDebug("value:%f", *ydataiter);
	}
#endif
}
void SinglePlot::timerEvent(QTimerEvent *)
{
#if 0
	printData();
	QVector<double>::const_iterator dataiter;
	int i = 0;
	for ( dataiter = vecDataX.begin(), i=0; dataiter != vecDataX.end(); ++dataiter,i++)
	{
		timeline[i] = *dataiter;
	}
	for ( dataiter = vecDataY.begin(), i=0; dataiter != vecDataY.end(); ++dataiter,i++)
	{
		d_y[i] = *dataiter;
	}
    setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
    //setAxisScale(QwtPlot::xBottom, timeline[PLOT_SIZE - 1], timeline[0]);
    //setAxisScale(QwtPlot::xBottom, timeline[0], timeline[PLOT_SIZE]);
    cLeft->setRawData(timeline, d_y, PLOT_SIZE);
#else
	//replot();
#endif
}
//
//  Set a plain canvas frame and align the scales to it
//
void SinglePlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.
    ((QwtPlotCanvas*)canvas())->setFrameStyle(QFrame::Box | QFrame::Sunken );
    ((QwtPlotCanvas*)canvas())->setLineWidth(1);
    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget ) scaleWidget->setMargin(0);
        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw ) scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}
#if 0
void SinglePlot::setYMinSpan(double span)
{
	autoscale = false;
	m_yminspan = span;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	replot();
}
void SinglePlot::setYMaxSpan(double span)
{	
	autoscale = false;
	m_ymaxspan = span;
	setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
	replot();
}
#else
void SinglePlot::setYMinSpan()
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
		autoscale = false;
		m_yminspan = span;
		setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
		replot();
	}
}
void SinglePlot::setYMaxSpan()
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
		autoscale = false;
		m_ymaxspan = span;
		setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);
		replot();
	}
}
#endif

void
SinglePlot::SetUnit(const dbr_string_t unit )
{
	QString unitstr = QString("Values ( ") + QString(unit)+" )";
    setAxisTitle(QwtPlot::yLeft, unitstr.toStdString().c_str());
};

#if 0
void SinglePlot::setTimerInterval(double ms)
{
    d_interval = qRound(ms);
    if ( d_timerId >= 0 )
    {
        killTimer(d_timerId);
        d_timerId = -1;
    };
    if (d_interval >= 0 )
	{
        d_timerId = startTimer(d_interval);
	};
}
//  Generate new values 
void SinglePlot::timerEvent(QTimerEvent *)
{
    static double phase = 0.0;

	// leesi++ for test
    //if (phase > (M_PI - 0.0001)) 
        //phase = 0.0;

    // y moves from left to right:
    // Shift y array right and assign new value to y[0].
    for ( int i = PLOT_SIZE - 1; i > 0; i-- )
	{
        d_y[i] = d_y[i-1];
	};
    d_y[0] = sin(phase) * (-1.0 + 2.0 * double(rand()) / double(RAND_MAX));

    for ( int j = 0; j < PLOT_SIZE - 1; j++ ) // Data Shift
	{
        d_z[j] = d_z[j+1];
	};

	// leesi++ for test
    //d_z[PLOT_SIZE - 1] = 0.8 - (2.0 * phase/M_PI) + 0.4 * 
        //double(rand()) / double(RAND_MAX);
    d_z[PLOT_SIZE - 1] = sin(phase); 
	static int cnt = 0;
	qDebug("value:%f, count:%d",d_z[PLOT_SIZE - 1], cnt++); // Last Value Display.
    // update the display
    replot();
    phase += M_PI * 0.02;
}
#endif
