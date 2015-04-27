/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <algorithm>
#include "caMultiplot.h"

using namespace std;

class CAMultiPlotScaleDraw:public QwtScaleDraw
{
public:
    CAMultiPlotScaleDraw(const int times):mtimes(times)
	{};
    virtual QwtText label(double totsec) const
    {
		//qDebug("Total Sec: %f", totsec);
		if(mtimes >=1 )totsec = (totsec/mtimes);
		int hour = (int)totsec/3600;
		int min = ((int)totsec%3600)/60;
		int sec = ((int)totsec%3600)%60;
		QTime upTime(hour, min,sec);
		return upTime.toString();
    };
private:
	const int mtimes;
};

class CAMultiplot::PrivateData
{
public:
	//member method
    PrivateData():mpenindex(0), mcanvascolor(QColor(Qt::gray)), mmajlinecolor(QColor(Qt::black)), 
				  mminlinecolor(QColor(Qt::gray)), mtimes(1), mtimes2(1),mtimelinecount(801)
    {
#if 0
		mdata = (double **)calloc(MAX_PV_CNT, sizeof(double *));

		for(size_t i = 0; i < MAX_PV_CNT; i++)
		{
			mdata[i] = (double *)calloc(1, sizeof(double)*mtimelinecount);
		};

		mtimeline = (double *)calloc(1, sizeof(double)*mtimelinecount);
		mfactor = (double *)calloc(1, sizeof(double)*mtimelinecount);
		std::fill(&mfactor[0],&mfactor[mtimelinecount], 1.0);
#endif
	};
public:
	//member data
	//static const int PLOT_SIZE  = 601;
	//static const int PLOT_SIZE  = 801*10;
	static const int PLOT_SIZE  = 801;
	static const int MAX_PV_CNT = 10;
	QString mpvname[MAX_PV_CNT];
	QPen mpen[MAX_PV_CNT];
#if 0
	double mdata[MAX_PV_CNT][PLOT_SIZE];
	double mtimeline[PLOT_SIZE];
	//double mfactor[PLOT_SIZE];
#else
	double **mdata;
	double *mtimeline;
	double *mfactor;
#endif
	QwtPlotCurve *mpCurve[MAX_PV_CNT];
	QwtPlotGrid *mgrid;
	int mpenindex;
	bool mblog;
	QString mtitle;
	QString mxtitle;
	QString mytitle;
	QColor mcanvascolor;
	QColor mmajlinecolor;
	QColor mminlinecolor;
	int mtimes;
	int mtimes2;
	vector<string> m_vecpvnames;
	vector<chid> m_node;
	//vector<double> m_vecPrevVal;
	double m_PrevVal[MAX_PV_CNT];
	int mtimerID;
    QwtPlotPicker *mpicker;
	double mmax, mmin;
	ePeriodic periodic;
	unsigned int mtimelinecount;

	void MemAssign()
	{
		mdata = (double **)calloc(MAX_PV_CNT, sizeof(double *));
		for(int i = 0; i < MAX_PV_CNT; i++)
		{
			mdata[i] = (double *)calloc(1, sizeof(double)*mtimelinecount);
		};
		mtimeline = (double *)calloc(1, sizeof(double)*mtimelinecount);
		mfactor = (double *)calloc(1, sizeof(double)*mtimelinecount);
		std::fill(&mfactor[0],&mfactor[mtimelinecount], 1.0);
	};
};

CAMultiplot::CAMultiplot(QWidget *parent):QwtPlot(parent),mCount(0)
{
    initMultiPlot();
}

CAMultiplot::~CAMultiplot()
{
	//killTimer(d_data->mtimerID);
    delete d_data;
}

QSize CAMultiplot::sizeHint() const
{
	return minimumSizeHint();
}

void CAMultiplot::initMultiPlot()
{
    //using namespace Qt;
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("CAMultiplot"));

	d_data->mgrid = new QwtPlotGrid;
	d_data->mgrid->enableXMin(true);
	d_data->mgrid->enableYMin(true);
	d_data->mgrid->setMajPen(QPen(Qt::black, 0, Qt::DashLine));
	d_data->mgrid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	d_data->mgrid->attach(this);
    setCanvasLineWidth(1);
	setLogscale(false);
	setAutoReplot(false);
	//setAxisAutoScale(QwtPlot::xBottom);
	//qDebug("SetAxisAutoScale(QwtPlot::xBottom:%d)", axisAutoScale(QwtPlot::xBottom));

	setPeriodic(OneSec);

    //setAxisScaleDraw(QwtPlot::xBottom, new CAMultiPlotScaleDraw());
	//setAxisScale(QwtPlot::xBottom, d_data->mtimeline[PrivateData::PLOT_SIZE-1], d_data->mtimeline[0]);
};

QSize CAMultiplot::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAMultiplot::setPvname(const QString &name)
{
    d_data->mpvname[0] = name;
}

const QString &CAMultiplot::getPvname() const
{
	return d_data->mpvname[0];
}

void CAMultiplot::setPvname1(const QString &name)
{
    d_data->mpvname[1] = name;
}

const QString &CAMultiplot::getPvname1() const
{
	return d_data->mpvname[1];
}

void CAMultiplot::setPvname2(const QString &name)
{
    d_data->mpvname[2] = name;
}

const QString &CAMultiplot::getPvname2() const
{
	return d_data->mpvname[2];
}

void CAMultiplot::setPvname3(const QString &name)
{
    d_data->mpvname[3] = name;
}

const QString &CAMultiplot::getPvname3() const
{
	return d_data->mpvname[3];
}

void CAMultiplot::setPvname4(const QString &name)
{
    d_data->mpvname[4] = name;
}

const QString &CAMultiplot::getPvname4() const
{
	return d_data->mpvname[4];
}

void CAMultiplot::setPvname5(const QString &name)
{
    d_data->mpvname[5] = name;
}

const QString &CAMultiplot::getPvname5() const
{
	return d_data->mpvname[5];
}

void CAMultiplot::setPvname6(const QString &name)
{
    d_data->mpvname[6] = name;
}

const QString &CAMultiplot::getPvname6() const
{
	return d_data->mpvname[6];
}

void CAMultiplot::setPvname7(const QString &name)
{
    d_data->mpvname[7] = name;
}

const QString &CAMultiplot::getPvname7() const
{
	return d_data->mpvname[7];
}

void CAMultiplot::setPvname8(const QString &name)
{
    d_data->mpvname[8] = name;
}

const QString &CAMultiplot::getPvname8() const
{
	return d_data->mpvname[8];
}

void CAMultiplot::setPvname9(const QString &name)
{
    d_data->mpvname[9] = name;
}

const QString &CAMultiplot::getPvname9() const
{
	return d_data->mpvname[9];
}

void CAMultiplot::changePvName (const int index, const QString &pvname)
{
    d_data->mpvname[index] = pvname;
}

void CAMultiplot::getPvNames ()
{
	QString strpv;
	//chid  chan_id;
	//int status;
	for(int i=0;i<PrivateData::MAX_PV_CNT;i++)
	{
		strpv=d_data->mpvname[i];
		if(strpv.compare("")==0 || strpv.compare("pvname")==0) continue;
#if 0
		status = ca_search(strpv.toStdString().c_str(), &chan_id);
		SEVCHK(status,NULL);
		status = ca_pend_io(1.0);
		if(status != ECA_NORMAL)
		{
			ca_clear_channel(chan_id);
			QString errmsg = QString("Not Found PV Name: ")+strpv;
			qDebug("Not Found %s\n", strpv.toStdString().c_str());
			QMessageBox::information(this, "Error!",errmsg);
			continue;
		};
#endif
		d_data->m_vecpvnames.push_back(strpv.toStdString());
	};
}
void CAMultiplot::PrintPvNames ()
{
#if 1
	size_t pvcnt = d_data->m_vecpvnames.size();
	for(size_t i = 0;i<pvcnt;i++)
	{
		qDebug("PVName[%d]:%s",i,d_data->m_vecpvnames.at(i).c_str());
	};
#else
	QString strpvcheck;
	for(int i=0;i<PrivateData::MAX_PV_CNT;i++)
	{
		strpvcheck=d_data->mpvname[i];
		qDebug("PVName[%d]:%s",i,strpvcheck.toStdString().c_str());
	};
#endif
}
//void CAMultiplot::Run(const int interval)
void CAMultiplot::Run()
{
	getPvNames();

    d_data->mpicker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::PointSelection | QwtPicker::DragSelection, 
        QwtPlotPicker::HLineRubberBand, QwtPicker::ActiveOnly, canvas());
        //QwtPlotPicker::VLineRubberBand, QwtPicker::AlwaysOn, canvas());
    d_data->mpicker->setRubberBandPen(QColor(Qt::green));
	//HLineRubberBand & PointSelection
    //d_data->mpicker->setRubberBand(QwtPicker::CrossRubberBand);
    d_data->mpicker->setTrackerPen(QColor(Qt::black));

    //connect(d_data->mpicker, SIGNAL(moved(const QPoint &)), SLOT(moved(const QPoint &)));

	if(getLogscale()==true)
	{
		setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
	};
	PrintPvNames();

	size_t pvcnt = d_data->m_vecpvnames.size();
#if 1
#if 0
    for (int i = 0; i< PrivateData::PLOT_SIZE; i++)
    {
		for (int j = 0; j<PrivateData::MAX_PV_CNT;j++)
		{
			if(getLogscale() == true)
				d_data->mdata[j][i] = 1;
			else
				d_data->mdata[j][i] = 0;
		};
		d_data->mtimeline[PrivateData::PLOT_SIZE-1-i] = i;
    };
#endif

	//Run(1000); //OneSec
	int interval = 1000;
	switch(getPeriodic())
	{
		case PointOne:
			interval = 100;
			d_data->mtimes = 10;
			break;
		case PointFive:
			interval = 500;
			d_data->mtimes = 2;
			break;
		case FiveSec:
			interval = 5000;
			d_data->mtimes = 0.2;
			d_data->mtimes2 = 5;
			break;
		case TenSec:
			interval = 10000;
			d_data->mtimes = 0.1;
			d_data->mtimes2 = 10;
			break;
		case OneSec:
			interval = 1000;
			d_data->mtimes = 1;
			break;
	};

	setAxisScaleDraw(QwtPlot::xBottom, new CAMultiPlotScaleDraw(d_data->mtimes));
	d_data->MemAssign();
	d_data->mtimerID=startTimer(interval);

	qDebug("Interval:%d, TimerID:%d", interval, d_data->mtimerID);

#if 0
    // Assign a title
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    insertLegend(legend, QwtPlot::RightLegend);
#endif

	d_data->mpen[0].setBrush(Qt::blue);
	d_data->mpen[1].setBrush(Qt::green);
	d_data->mpen[2].setBrush(Qt::red);
	d_data->mpen[3].setBrush(Qt::darkRed);
	d_data->mpen[4].setBrush(Qt::black);
	d_data->mpen[5].setBrush(Qt::magenta);
	d_data->mpen[6].setBrush(Qt::darkMagenta);
	d_data->mpen[7].setBrush(Qt::yellow);
	d_data->mpen[8].setBrush(Qt::darkBlue);
	d_data->mpen[9].setBrush(Qt::cyan);

    // Attach data. max 10 curves use the same timeline array.
	for(size_t index = 0; index<pvcnt; index++ )
	{
		//d_data->mpCurve[index] = new QwtPlotCurve(d_data->m_vecpvnames.at(index).c_str()); //for Legend
		d_data->mpCurve[index] = new QwtPlotCurve();
		d_data->mpCurve[index]->setPen(d_data->mpen[index]);
		d_data->mpCurve[index]->setStyle(QwtPlotCurve::Steps);
		//d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->mdata[index], PrivateData::PLOT_SIZE);
		d_data->mpCurve[index]->attach(this);
		signalColor(index);
		//displayCurve(d_data->mpCurve[index], true);
	};
	//connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),SLOT(displayCurve(QwtPlotItem *, bool)));

#endif
	chid mychid[pvcnt];
	for(size_t i = 0; i<pvcnt; i++)
	{
		ca_create_channel(d_data->m_vecpvnames.at(i).c_str(), NULL, NULL, 10, &mychid[i]);
		d_data->m_node.push_back(mychid[i]);
		ca_pend_io(1.0);
	};
}
void CAMultiplot::timerEvent(QTimerEvent *)
{
	//for imsi.
	size_t pvcnt = d_data->m_vecpvnames.size();
	struct dbr_time_double data[pvcnt];
	//struct local_tm_nano_sec tm;
	//epicsTime  stamp;
	int status;
	enum channel_state state;
	//if(mCount < PrivateData::PLOT_SIZE) mCount++;
	if(mCount < d_data->mtimelinecount) mCount++;

	//Local Time, Not IOC Time
	QTime time;
	QTime localtime = QTime::currentTime();
	int totsec = time.secsTo(localtime);
	//double totsec = time.msecsTo(localtime)/1000.0;

#if 0
	for ( int j = 0; j<PrivateData::PLOT_SIZE; j++ )
	{
		d_data->mtimeline[j]=(totsec-j);
	};
	setAxisScale(QwtPlot::xBottom, d_data->mtimeline[PrivateData::PLOT_SIZE-1], d_data->mtimeline[0]);
#endif

	for(size_t i = 0; i < pvcnt; i++)
	{
		state = ca_state(d_data -> m_node.at(i));
		//PV Not Found or Disconnected
		status = ca_pend_io(0.1);
		if(state <=1 ) continue;
		ca_get(DBR_TIME_DOUBLE,d_data->m_node.at(i), (void *)&data[i]);
		status = ca_pend_io(0.1);
		if(status != ECA_NORMAL)
		{
			QString errmsg = QString("Connection Failed: ")+ca_name(d_data->m_node.at(i));
			//QMessageBox::information(this, "Error!",errmsg);
			//killTimer(d_data->mtimerID);
			qDebug("getValue error - in caMultplot(%s), replace prev value(%f)",errmsg.toStdString().c_str(), d_data->m_PrevVal[i]);
			getValue(i, d_data->m_PrevVal[i], totsec, mCount);
		}
		else
		{
			d_data->m_PrevVal[i]=data[i].value;
			getValue(i, data[i].value,totsec, mCount);
		}
		//stamp = data[0].stamp;
		//tm = (local_tm_nano_sec) stamp;
		//totsec = tm.ansi_tm.tm_hour*3600+tm.ansi_tm.tm_min*60+tm.ansi_tm.tm_sec;
	};
	replot();
};
void CAMultiplot::getValue(const int index, const double &pvval, const int &totsec, const int count)
{ 
	// Data Shift
	// Dynamic Data Shift
	//for (int i=PrivateData::PLOT_SIZE-1;i>0 ;i--)

	int xscale = count;
	//if(count == PrivateData::PLOT_SIZE) xscale=PrivateData::PLOT_SIZE-1;

	if(count == d_data->mtimelinecount) xscale=d_data->mtimelinecount-1;

	for (int i=xscale;i>0 ;i--)
	{
		d_data->mdata[index][i] = d_data->mdata[index][i-1];
	};

	//if(count < PrivateData::PLOT_SIZE) mCount++;

#if 1
	// No need in CAMultiplot
	if(getLogscale()==true)
	{
		//d_data->mdata[index][0] = rand();
		d_data->mdata[index][0] = fabs(pvval)*(d_data->mfactor[index]);
	}
	else
	{
		d_data->mdata[index][0] = pvval*(d_data->mfactor[index]);
	};
#endif

#if 1
	if( index == 0 )
	{
		//qDebug("Total Sec: %d", totsec);
		//for ( int j = 0; j<PrivateData::PLOT_SIZE; j++ )
		for ( int j = 0; j<d_data->mtimelinecount; j++ )
		{
			//d_data->mtimeline[j]=(totsec-j);
			//10Hz
			//d_data->mtimeline[j]=(totsec*10-j);
			if(d_data -> mtimes >= 1)
				d_data->mtimeline[j]=((totsec*(d_data->mtimes))-j);
			else
				d_data->mtimeline[j]=(totsec-(j*d_data->mtimes2));
		};
		//setAxisScale(QwtPlot::xBottom, d_data->mtimeline[PrivateData::PLOT_SIZE-1], d_data->mtimeline[0]);
		setAxisScale(QwtPlot::xBottom, d_data->mtimeline[d_data->mtimelinecount-1], d_data->mtimeline[0]);
	};
	d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->mdata[index], xscale);
#endif

	switch(index)
	{
		case 0:
			emit currentValue(pvval);
			break;
		case 1:
			emit currentValue1(pvval);
			break;
		case 2:
			emit currentValue2(pvval);
			break;
		case 3:
			emit currentValue3(pvval);
			break;
		case 4:
			emit currentValue4(pvval);
			break;
		case 5:
			emit currentValue5(pvval);
			break;
		case 6:
			emit currentValue6(pvval);
			break;
		case 7:
			emit currentValue7(pvval);
			break;
		case 8:
			emit currentValue8(pvval);
			break;
		case 9:
			emit currentValue9(pvval);
			break;
	};
#if 0
	emit timeLabel(getTime(totsec));
	emit timeLabel1(getTime(totsec-100));
	emit timeLabel2(getTime(totsec-200));
	emit timeLabel3(getTime(totsec-300));
	emit timeLabel4(getTime(totsec-400));
	emit timeLabel5(getTime(totsec-500));
	QwtDoubleRect rect1 = m_zoomer->zoomBase();
	QwtDoubleRect rect2 = m_zoomer->zoomRect();
	qDebug("Base:top:%f, left:%f, bottom:%f, right:%f", rect1.top(), rect1.left(), rect1.bottom(), rect1.right());
	qDebug("Rect:top:%f, left:%f, bottom:%f, right:%f", rect2.top(), rect2.left(), rect2.bottom(), rect2.right());
	double offset = 0.0;
	if (autoscale == true)
	{
		m_ymaxspan = *max_element(&m_data[0], &m_data[PrivateData::PLOT_SIZE]);
		m_yminspan = *min_element(&m_data[0], &m_data[PrivateData::PLOT_SIZE]);
		offset = m_ymaxspan - m_yminspan;
	};
#endif
#if 0
	if ( index == 0 )
	{
		for ( int j = 0; j<PrivateData::PLOT_SIZE; j++ )
		{
			d_data->mtimeline[j] = (totsec-j);
		};
	};
#endif
	//setAxisScale(QwtPlot::yLeft, m_yminspan, m_ymaxspan);

	//replot(); -> timerEvent
}
void CAMultiplot::displayCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);
    
    replot();
}

void CAMultiplot::displayCurve(const bool on, const QString &pvname)
{
	size_t pvcnt = d_data->m_vecpvnames.size();
	size_t index = 0;
	for(; index<pvcnt; index++ )
	{
		if (d_data->m_vecpvnames.at(index).compare(pvname.toStdString()) == 0 ) break;
	};
	d_data->mpCurve[index]->setVisible(on);
    replot();
}

void CAMultiplot::setYRange(const double min, const double max)
{
	setAxisScale(QwtPlot::yLeft, min, max);
	replot();
}
void CAMultiplot::setYRange()
{
	setAxisScale(QwtPlot::yLeft, d_data->mmin,d_data->mmax);
	replot();
}
void CAMultiplot::setYRangeMax(const double max)
{
	d_data->mmax = max;
}
void CAMultiplot::setYRangeMin(const double min)
{
	//setAxisScale(QwtPlot::yLeft, min, max);
	d_data->mmin = min;
}
void CAMultiplot::penIndex(const int index)
{
	d_data->mpenindex = index;
}
void CAMultiplot::penWidth(const int width)
{
	d_data->mpen[d_data->mpenindex].setWidth(width);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}
void CAMultiplot::penStyle(const QString strstyle)
{
	if (strstyle.toLower().compare("lines") == 0 )
		d_data->mpCurve[d_data->mpenindex]->setStyle(QwtPlotCurve::Lines);
	else if (strstyle.toLower().compare("dots") == 0 )
		d_data->mpCurve[d_data->mpenindex]->setStyle(QwtPlotCurve::Dots);
	else if (strstyle.toLower().compare("sticks") == 0 )
		d_data->mpCurve[d_data->mpenindex]->setStyle(QwtPlotCurve::Sticks);
	else
		d_data->mpCurve[d_data->mpenindex]->setStyle(QwtPlotCurve::Steps);

	replot();
}
void CAMultiplot::penColor(const QString color)
{
	d_data->mpen[d_data->mpenindex].setColor(QColor(color));
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	signalColor(d_data->mpenindex);
	replot();
}

void CAMultiplot::signalColor(const int index)
{
	switch(index)
	{
		case 0:
			emit legendLabel(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 1:
			emit legendLabel1(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 2:
			emit legendLabel2(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 3:
			emit legendLabel3(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 4:
			emit legendLabel4(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 5:
			emit legendLabel5(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 6:
			emit legendLabel6(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 7:
			emit legendLabel7(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 8:
			emit legendLabel8(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		case 9:
			emit legendLabel9(d_data->mpen[index].color().name(), d_data->m_vecpvnames.at(index).c_str());
			break;
		default:
			break;
	};
}

void CAMultiplot::penColor(const QColor &color)
{
	d_data->mpen[d_data->mpenindex].setColor(color);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}
void CAMultiplot::factor(const QString strfactor)
{
	double dfactor = strfactor.toDouble();
	if(dfactor == 0.0) dfactor = 1.0;
	d_data->mfactor[d_data->mpenindex] = dfactor;
}
void CAMultiplot::nonfactors()
{
	std::fill(&d_data->mfactor[0],&d_data->mfactor[d_data->mtimelinecount], 1.0);
}
QString CAMultiplot::getTime(const int &time)
{
	int hour = time/3600;
	int min = (time%3600)/60;
	int sec = (time%3600)%60;
	char strtime[20];
	sprintf(strtime,"%02d:%02d:%02d",hour, min, sec);
	return QString(strtime);
};
void CAMultiplot::setLogscale(const bool setlog)
{
    d_data->mblog = setlog;
}

const bool &CAMultiplot::getLogscale() const
{
	return d_data->mblog;
}

void CAMultiplot::setPlottitle(const QString &title)
{
    d_data->mtitle = title;
	setTitle(title.toStdString().c_str());
	plotupdate();
}
const QString &CAMultiplot::getPlottitle() const
{
	return d_data->mtitle;
}

void CAMultiplot::setXtitle(const QString &title)
{
    setAxisTitle(QwtPlot::xBottom, title.toStdString().c_str());
	d_data->mxtitle = title;
	plotupdate();
}
const QString &CAMultiplot::getXtitle() const
{
	return d_data->mxtitle;
}

void CAMultiplot::setYtitle(const QString &title)
{
    setAxisTitle(QwtPlot::yLeft, title.toStdString().c_str());
	d_data->mytitle = title;
	plotupdate();
}
const QString &CAMultiplot::getYtitle() const
{
	return d_data->mytitle;
}

void CAMultiplot::setCanvascolor(const QColor &color)
{
	setCanvasBackground(color);
	d_data->mcanvascolor = color;
	plotupdate();
}
const QColor &CAMultiplot::getCanvascolor()
{

	return d_data->mcanvascolor;
}

void CAMultiplot::setMajorlinecolor(const QColor &color)
{
	d_data->mmajlinecolor = color;
	d_data->mgrid->setMajPen(QPen(color, 0, Qt::DashLine));
	plotupdate();
}
const QColor &CAMultiplot::getMajorlinecolor()
{

	return d_data->mmajlinecolor;
}

void CAMultiplot::setMinorlinecolor(const QColor &color)
{
	d_data->mgrid->setMinPen(QPen(color , 0, Qt::DotLine));
	d_data->mminlinecolor = color;
	plotupdate();
}
const QColor &CAMultiplot::getMinorlinecolor()
{
	return d_data->mminlinecolor;
}

void CAMultiplot::setPeriodic(ePeriodic periodic)
{
	d_data->periodic = periodic;
}

CAMultiplot::ePeriodic CAMultiplot::getPeriodic()
{
	return d_data->periodic;
}

const int CAMultiplot::getTimeLineCnt() const 
{
	return d_data->mtimelinecount;
}

void CAMultiplot::setTimeLineCnt(const int timeline)
{
	d_data->mtimelinecount = timeline;
}

void CAMultiplot::plotupdate()
{
	replot();
	update();
}
