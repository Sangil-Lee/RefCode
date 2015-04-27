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
#include "caMDSplusplot.h"
#include <values.h>

//#include "caArrayData.h"

using namespace std;

//static double timebase[450559];
//static double shotdata[6][450559];			//or array? you choice

//CAMDSplusplot *gmpmdsplot;
template <class T> class AddValue
{
	private:
		T value;
	public:
		AddValue(const T& v):value(v){};

		void operator () (T& elem) const {elem+=value;};
};

template <class T> class MultipleValue
{
	private:
		T value;
	public:
		MultipleValue(const T& v):value(v){};
		void operator () (T& elem) const {elem*=value;};
};


class EpicsTimeScaleDraw:public QwtScaleDraw
{
public:
    EpicsTimeScaleDraw(){};
    virtual QwtText label(double totsec) const
    {
		int hour = (int)totsec/3600;
		int min = ((int)totsec%3600)/60;
		int sec = ((int)totsec%3600)%60;
        QTime upTime(hour, min,sec);
        return upTime.toString();
    };
#if 0
    virtual QwtText label(double) const
    {
        return QwtText("");
    };
#endif
};

class MDSValueScaleDraw:public QwtScaleDraw
{
public:
	MDSValueScaleDraw(const QString treename):mtreename(treename),mFail(0)
	{
	};
    virtual QwtText label(double value) const
    {
		//QString strValue = QString("%1").arg(value, 0, 'f', 1, ' ');
		//qDebug("X-Value:%s", strValue.toStdString().c_str());
		//if(value >= DBL_MAX)
			//value = 1.0;
		//if(value <= DBL_MIN)
			//value = -1.0;
		qDebug("Debug - X-value: %f", (float)value);
		if(mFail == 1) return QwtText(" 0.0");

		char strbuf[256];
		if(mtreename.compare("PCRC03")==0)
		{
			value = value/1000000;	//MA
			sprintf(strbuf, "%6.1f", value);
		}
		else if(mtreename.compare("POL_HA05")==0)
		{
			value = value/1000000000;
			sprintf(strbuf, "%6.1f", value);
		}
		else if(mtreename.compare("ECE02")==0)
		{
			value = value/1000;
			sprintf(strbuf, "%6.1f", value);
		}
		else if(mtreename.compare("B_PKR251A")==0)
		{
			value = value*100000.;
			sprintf(strbuf, "%6.1f", value);
		}
		else
		{
			sprintf(strbuf, "%6.1f", value);
			//sprintf(strbuf, "%07.1f", value);
		}
		qDebug("X-Value:%s", strbuf);
		QString strValue = QString(strbuf);
		//tickLabel(QFont("Helvetica", 10, QFont::Bold), value);
		return strValue;
	};
	void setFail(const int fail) {mFail = fail;};
private:
	QString mtreename;
	int mFail;
};

class CAMDSplusplot::PrivateData
{
	public:
		//member method
		PrivateData():mpenindex(0), mcanvascolor(QColor(Qt::gray)), mmajlinecolor(QColor(Qt::black)), 
			 mminlinecolor(QColor(Qt::gray)),mreqelement(1024),mmsec(1000),
			 mmrefresh(1000),mshotno(1), mbasetime(0.0), mfactor(0.0), initcount(0)
		{
			init();
		};
	public:
		//member data
		static const int PLOT_SIZE  = 1024;
		static const int MAX_PV_CNT = 10;
		QString mtreename;
		QString mnodename[MAX_PV_CNT];
		QPen mpen[MAX_PV_CNT];
		double	**marrydata;
		double	*mtimeline;
		//double	*mfactor;

		double **mbuffer;

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
		QwtLegend *mlegend;
		int mreqelement;
		int mmsec;
		int mmrefresh;
		int mshotno;
		double mbasetime;
		double	mfactor;
		QString mserverip;
		QString mserverport;

		TypeOfCall mtype;
		vector<string> mvecnode;
		int mtimerID;
		QwtPlotPicker *mpicker;
		virtual ~PrivateData();
		void IncInitCount(){ initcount = 1;};
		int getInitCount(){ return initcount;};
		MDSValueScaleDraw *mpMDSScale;
	private:
		void init();
		void datafree();
		void dataAlloc();
		int initcount;
};
CAMDSplusplot::PrivateData::~PrivateData()
{
	datafree();
}
void CAMDSplusplot::PrivateData::init()
{
	dataAlloc();
	//std::fill(&mfactor[0],&mfactor[PLOT_SIZE], 1.0);
	for(int i = 0; i < MAX_PV_CNT;i++)
		mpCurve[i] = 0;
}
void CAMDSplusplot::PrivateData::datafree()
{
	free(mtimeline);
	//free(mfactor);
}
void CAMDSplusplot::PrivateData::dataAlloc()
{
//	mtimeline = (double*)calloc(mreqelement, sizeof(double));
	//mfactor = (double*)calloc(mreqelement, sizeof(double));
}

CAMDSplusplot::CAMDSplusplot(QWidget *parent):QwtPlot(parent)
{
    initMDSPlot();
}

CAMDSplusplot::~CAMDSplusplot()
{
	if(!d_data) 
	{
		delete d_data;
		d_data = 0;
	};

	deleteCHID();
#if 0 
	if(!mmdsdatathread)
	{
		if(mmdsdatathread->isRunning())
			mmdsdatathread->wait();
		delete mmdsdatathread;
		mmdsdatathread = 0;
	};
#endif
	delete mdsplusdata;
}

QSize CAMDSplusplot::sizeHint() const
{
	return minimumSizeHint();
}

void CAMDSplusplot::initMDSPlot()
{
    //using namespace Qt;
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("CAMDSplusplot"));

	d_data->mgrid = new QwtPlotGrid;
	//++leesi 10/19
	//d_data->mgrid->enableXMin(true);
	//d_data->mgrid->enableYMin(true);
	d_data->mgrid->setMajPen(QPen(Qt::gray, 0, Qt::DashLine));
	//++leesi 10/19
	//d_data->mgrid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	d_data->mgrid->attach(this);
    setCanvasLineWidth(1);
	setLogscale(false);
	setAutoReplot(false);
    setAxisScaleDraw(QwtPlot::xBottom, new EpicsTimeScaleDraw());
    //setAxisScaleDraw(QwtPlot::yLeft, new MDSValueScaleDraw());

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

	mdsplusdata = new MDSPlusData;

    // Assign a title
    d_data->mlegend = new QwtLegend;
	//++leesi
    //d_data->mlegend->setItemMode(QwtLegend::CheckableItem);
    d_data->mlegend->setFont(QFont("Helvetica", 0, QFont::Bold));
    d_data->mlegend->setDisplayPolicy(QwtLegend::NoIdentifier,0);

	//++leesi 10/19
    insertLegend(d_data->mlegend, QwtPlot::RightLegend);
	connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),SLOT(displayCurve(QwtPlotItem *, bool)));
	mpTimer = new QTimer(this);
	mpTimer->setInterval(2000);
	connect(mpTimer, SIGNAL(timeout()), this, SLOT(timerEvent())); 
	createCHID();

	setAxisScale(QwtPlot::xBottom, 0, 15);
	setAxisMaxMinor(QwtPlot::xBottom, 0);
	setAxisMaxMinor(QwtPlot::yLeft, 0);
	setAxisMaxMajor(QwtPlot::yLeft, 2);
#if 1
	//mmdsdatathread = new CAMDSplusplotThr();
#endif
};

QSize CAMDSplusplot::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}
const QString &CAMDSplusplot::getTreename() const
{
	return d_data->mtreename;
}
void CAMDSplusplot::setTreename(const QString &treename)
{
    d_data->mtreename = treename;
}
void CAMDSplusplot::setNodename(const QString &nodename)
{
    d_data->mnodename[0] = nodename;
}
const QString CAMDSplusplot::getNodename() const
{
	return d_data->mnodename[0];
}

void CAMDSplusplot::setNodename1(const QString &nodename)
{
    d_data->mnodename[1] = nodename;
}

const QString &CAMDSplusplot::getNodename1() const
{
	return d_data->mnodename[1];
}

void CAMDSplusplot::setNodename2(const QString &nodename)
{
    d_data->mnodename[2] = nodename;
}

const QString &CAMDSplusplot::getNodename2() const
{
	return d_data->mnodename[2];
}

void CAMDSplusplot::setNodename3(const QString &nodename)
{
    d_data->mnodename[3] = nodename;
}

const QString &CAMDSplusplot::getNodename3() const
{
	return d_data->mnodename[3];
}

void CAMDSplusplot::setNodename4(const QString &nodename)
{
    d_data->mnodename[4] = nodename;
}

const QString &CAMDSplusplot::getNodename4() const
{
	return d_data->mnodename[4];
}

void CAMDSplusplot::setNodename5(const QString &nodename)
{
    d_data->mnodename[5] = nodename;
}

const QString &CAMDSplusplot::getNodename5() const
{
	return d_data->mnodename[5];
}

void CAMDSplusplot::setNodename6(const QString &nodename)
{
    d_data->mnodename[6] = nodename;
}

const QString &CAMDSplusplot::getNodename6() const
{
	return d_data->mnodename[6];
}

void CAMDSplusplot::setNodename7(const QString &nodename)
{
    d_data->mnodename[7] = nodename;
}

const QString &CAMDSplusplot::getNodename7() const
{
	return d_data->mnodename[7];
}

void CAMDSplusplot::setNodename8(const QString &nodename)
{
    d_data->mnodename[8] = nodename;
}

const QString &CAMDSplusplot::getNodename8() const
{
	return d_data->mnodename[8];
}

void CAMDSplusplot::setNodename9(const QString &nodename)
{
    d_data->mnodename[9] = nodename;
}

const QString &CAMDSplusplot::getNodename9() const
{
	return d_data->mnodename[9];
}
const QString &CAMDSplusplot::getServerIP() const 
{
	return d_data->mserverip;
}
void CAMDSplusplot::setServerIP(const QString &serverip)
{
	d_data->mserverip = serverip;
}
const QString &CAMDSplusplot::getServerPort() const 
{
	return d_data->mserverport;
}
void CAMDSplusplot::setServerPort(const QString &serverport)
{
	d_data->mserverport = serverport;
}
void CAMDSplusplot::changeNodeName (const int index, const QString &pvname)
{
    d_data->mnodename[index] = pvname;
}

void CAMDSplusplot::getNodeNameAll ()
{
	QString strpv;
	d_data->mvecnode.clear();
	for(int i=0;i<PrivateData::MAX_PV_CNT;i++)
	{
		strpv=d_data->mnodename[i];
		if(strpv.isEmpty()==true||strpv.isNull()==true) continue;
		d_data->mvecnode.push_back(strpv.toStdString());
	};
}
void CAMDSplusplot::PrintPvNames ()
{
#if 1
	size_t nodecnt = d_data->mvecnode.size();
	for(size_t i = 0;i<nodecnt;i++)
	{
		qDebug("NodeName[%d]:%s",i,d_data->mvecnode.at(i).c_str());
	};
#endif
}
void CAMDSplusplot::Start()
{
#if 0
	mmdsdatathread->start();
#endif
}
void CAMDSplusplot::Stop()
{
#if 0
	if(mmdsdatathread->isRunning())
		mmdsdatathread->wait();
#endif

}
void CAMDSplusplot::Run()
{
	getNodeNameAll();
	d_data->mtimerID=startTimer(d_data->mmrefresh);
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
}
void CAMDSplusplot::SetData()
{
    // Attach data. max 10 curves use the same timeline array.
}
const bool CAMDSplusplot::isAuto()
{
	return mbAuto;
}
//void CAMDSplusplot::timerEvent(QTimerEvent *)
void CAMDSplusplot::timerEvent()
{
	static int ccspfshsum = 0;
	//const int pfshsummary = caGetValue("CCS_PERFORM_SHOT_SUMMARY"); 
	const int pfshsummary = caGetPerformShotSummary();

	if ( ccspfshsum != pfshsummary )
	{
		const int shotno = caGetShotNo();
		if (shotno < 0) return;
		if(pfshsummary == 1)
		{
			sleep(20);
			slotReadShot(shotno-1);
		};
		ccspfshsum = pfshsummary;
	}
	else
	{
		qDebug("CCS_PERFORM_SHOT_SUMMARY:%d did not execute", pfshsummary);
	};
	//replot();
}
int CAMDSplusplot::createCHID()
{
	ca_create_channel("CCS_SHOT_NUMBER",NULL, NULL,20,&mchid_shotno);
	int status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("Creat Channel Error:%s\n",ca_name(mchid_shotno));
		return -1;
	};
	//ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",NULL, NULL,20,&mchid_pfshsum);
	ca_create_channel("PCS_LS_STATUS",NULL, NULL,20,&mchid_pfshsum);
	status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("Creat Channel Error:%s\n",ca_name(mchid_pfshsum));
		return -1;
	};
	return 0;
}
void CAMDSplusplot::deleteCHID()
{
	ca_clear_channel(mchid_shotno);
	ca_pend_io(0.5);
	ca_clear_channel(mchid_pfshsum);
	ca_pend_io(0.5);
}
const int CAMDSplusplot::caGetShotNo()
{
	struct dbr_time_long shotdata;
	ca_get(DBR_TIME_LONG, mchid_shotno, (void *)&shotdata);
	int status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("SHOT number: %s denied access\n",ca_name(mchid_shotno));
		return -1;
	};
	qDebug("Shot Number: %d", shotdata.value);
	return (shotdata.value);
};
const int CAMDSplusplot::caGetPerformShotSummary()
{
	struct dbr_time_long shotdata;
	ca_get(DBR_TIME_LONG, mchid_pfshsum, (void *)&shotdata);
	int status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("Perform SHOT number: %s denied access\n",ca_name(mchid_pfshsum));
		return -1;
	};
	return (shotdata.value);
};
#if 0
const int CAMDSplusplot::caGetValue(const string pvname)
{
	struct dbr_time_long shotdata;
	chid shot_id;
	ca_create_channel(pvname.c_str(),NULL, NULL,20,&shot_id);
	int status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("CreatChannel Error:%s\n",ca_name(shot_id));
		return -1;
	};
	ca_get(DBR_TIME_LONG, shot_id, (void *)&shotdata);
	status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		qDebug("SHOT number: %s denied access\n",ca_name(shot_id));
		return -1;
	};
	ca_clear_channel(shot_id);
	ca_pend_io(0.5);
	return (shotdata.value);
};
#endif
void CAMDSplusplot::displayCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(!on);
#if 0
    QWidget *w = legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);
    
    replot();
#endif
}

void CAMDSplusplot::displayCurve(const bool on, const QString &pvname)
{
	size_t pvcnt = d_data->mvecnode.size();
	size_t index = 0;
	for(; index<pvcnt; index++ )
	{
		if (d_data->mvecnode.at(index).compare(pvname.toStdString()) == 0 ) break;
	};
	d_data->mpCurve[index]->setVisible(on);
    replot();
}

void CAMDSplusplot::setYRange(const double min, const double max)
{
	setAxisScale(QwtPlot::yLeft, min, max);
	replot();
}

void CAMDSplusplot::penIndex(const int index)
{
	d_data->mpenindex = index;
}
void CAMDSplusplot::penWidth(const int width)
{
	d_data->mpen[d_data->mpenindex].setWidth(width);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}
void CAMDSplusplot::shotNumber(const int shotno)
{
	d_data->mshotno = shotno;
	qDebug("shotno-1:%d", shotno);
}
void CAMDSplusplot::shotNumber(const double shotno)
{
	qDebug("shotno-2:%f", shotno);
	d_data->mshotno = static_cast<int> (shotno);
}
void CAMDSplusplot::shotTreeName(const QString &treename)
{
	d_data->mtreename = treename;
}
void CAMDSplusplot::addNodeName(const QString &nodename)
{
	d_data->mnodename[0] = nodename;
}
void CAMDSplusplot::slotReadShot(const int shotno)
{
	static int count = 0;
	d_data->mshotno = shotno;
	qDebug("SlotShotNo[%d]:%d", count++, d_data->mshotno);
	readShot();
	emit signalShotNumber(shotno);
}
#if 1
void CAMDSplusplot::readShot()
{
	//qDebug("readShot");
	//gmpmdsplot = this;
	getNodeNameAll();
	string treename = d_data->mtreename.toStdString();
	int shotno = d_data->mshotno;
	int displaycount = d_data->mvecnode.size();
	bool check;
	if (displaycount == 0) return;

	mdsplusdata->setMDSDataInfo(treename,d_data->mvecnode.at(0),shotno,check);
	//if(check == MDSPlusData::FAIL) return;

	if(d_data->getInitCount()==0) 
	{
		qDebug("Treename:%s",getNodename().toStdString().c_str());
		//setAxisScaleDraw(QwtPlot::yLeft, new MDSValueScaleDraw(getNodename()));
		d_data->mpMDSScale = new MDSValueScaleDraw(getNodename());
		setAxisScaleDraw(QwtPlot::yLeft, d_data->mpMDSScale);
	};

	d_data->IncInitCount();

	int size = mdsplusdata->getNodeSize();
	if(size < 0 ) 
	{
		for(int i=0;i<displaycount;i++)
		{
			string nodename = d_data->mnodename[i].toStdString();
			if(nodename.empty() == true) continue;
			QWidget *w = legend()->find(d_data->mpCurve[i]);
			if(w)
			{
				//const int defaultsize = 1024;
				//double timebase[defaultsize]; 
				//double shotdata[defaultsize];
				//memset(shotdata, 0, defaultsize);
				//d_data->mpCurve[i]->setRawData(timebase, shotdata, defaultsize);
				//displayCurve(d_data->mpCurve[i], true);

				const int defaultsize = 1024;
				double timebase[defaultsize];
				double shotdata[defaultsize];
				memset(shotdata, 0, defaultsize);
				//setAxisScale(QwtPlot::yLeft, 10, defaultsize);
				d_data->mpMDSScale->setFail(1);
				d_data->mpCurve[i]->setRawData(timebase, shotdata, defaultsize);
				displayCurve(d_data->mpCurve[i], true);

				if(nodename.compare("KAPPA") == 0)
				{
					emit signalMaxKappa("");
				}
				else if(nodename.compare("BETAN") == 0)
				{
					emit signalMaxBetan("");
				}
				else if(nodename.compare("WTOT_DLM03") == 0)
				{
					emit signalMaxWtot("");
				}
				else if(nodename.compare("ECH_VFWD1") == 0)
				{
					emit signalMaxECH("");
				}
				else if(nodename.compare("ICRF_FWD") == 0)
				{
					emit signalMaxICRH("");
				}
				else if(nodename.compare("NB11_PNB") == 0)
				{
					emit signalMaxNB1("");
				}
				else if(nodename.compare("NB12_PNB") == 0)
				{
					emit signalMaxNB2("");
				}
				else if(nodename.compare("PCRC03") == 0)
				{
					emit signalMaxIp("");
				}
				else if(nodename.compare("NE_INTER01") == 0)
				{
					emit signalMaxNe("");
				}
				else if(nodename.compare("POL_HA05") == 0)
				{
					emit signalMaxHa("");
				}
				else if(nodename.compare("ECE02") == 0)
				{
					emit signalMaxTe("");
				}
				else if(nodename.compare("B_PKR251A") == 0)
				{
					emit signalMaxPre("");
				}
			};
		};
		replot();
		return;
	};

	//if(size > 100000) size = 100000;
	qDebug("1-Display Count:%d, ArraySize:%d", displaycount,size);

	//check limit -a -> stack size.
	//if requested stack memory smaller than account stack memory, 
	//U should reconfigure statck size(ulimit -s 102400, or /etc/securilty/limits.conf)
#if 0
	double timebase[size];
	double shotdata[displaycount][size];			//or array? you choice
#else

#if 0
	//++ leesi 10/22
	double *timebase = new double[size];
	double *shotdata[displaycount];
	for(int i = 0;i<displaycount;i++)
	{
		shotdata[i]=new double[size];
	};
#endif
	//double *timebase = 0;
	double *timebase[displaycount];
	double *shotdata[displaycount];
	for(int i = 0;i<displaycount;i++)
	{
		shotdata[i]= 0;
		timebase[i]= 0;
	};
#endif
	//for_each(&d_data->mpCurve[0], &d_data->mpCurve[PrivateData::MAX_PV_CNT], Delete);
	for(int i = 0;i<displaycount;i++)
	{
		string nodename = d_data->mnodename[i].toStdString();
		if(nodename.empty() == true) continue;

		mdsplusdata->setMDSDataInfo(treename,nodename,shotno,check);
		if(check == MDSPlusData::FAIL) 
		{
			if(nodename.compare("KAPPA") == 0)
			{
				emit signalMaxKappa("");
			}
			else if(nodename.compare("BETAN") == 0)
			{
				emit signalMaxBetan("");
			}
			else if(nodename.compare("WTOT_DLM03") == 0)
			{
				emit signalMaxWtot("");
			}
			else if(nodename.compare("ECH_VFWD1") == 0)
			{
				emit signalMaxECH("");
			}
			else if(nodename.compare("ICRF_FWD") == 0)
			{
				emit signalMaxICRH("");
			}
			else if(nodename.compare("NB11_PNB") == 0)
			{
				emit signalMaxNB1("");
			}
			else if(nodename.compare("NB12_PNB") == 0)
			{
				emit signalMaxNB2("");
			}
			else if(nodename.compare("PCRC03") == 0)
			{
				emit signalMaxIp("");
			}
			else if(nodename.compare("NE_INTER01") == 0)
			{
				emit signalMaxNe("");
			}
			else if(nodename.compare("POL_HA05") == 0)
			{
				emit signalMaxHa("");
			}
			else if(nodename.compare("ECE02") == 0)
			{
				emit signalMaxTe("");
			}
			else if(nodename.compare("B_PKR251A") == 0)
			{
				emit signalMaxPre("");
			}
			d_data->mpMDSScale->setFail(1);
			QWidget *w = legend()->find(d_data->mpCurve[i]);
			if(w != 0)
				displayCurve(d_data->mpCurve[i], true);
			replot();
			//continue;
			return;
		};
		d_data->mpMDSScale->setFail(0);

		QWidget *w = legend()->find(d_data->mpCurve[i]);
		if(w == 0)
		{
			//++leesi 10/19
			//d_data->mpCurve[i] = new QwtPlotCurve(d_data->mnodename[i]);
			d_data->mpCurve[i] = new QwtPlotCurve;
			d_data->mpen[i].setWidth(3);
			d_data->mpCurve[i]->setPen(d_data->mpen[i]);
			d_data->mpCurve[i]->setStyle(QwtPlotCurve::Steps);
			d_data->mpCurve[i]->attach(this);
		};

		int size = mdsplusdata->getNodeSize();
		if(size < 0) continue;
		//if(size > 100000) size = 100000;
		//double *timebase = new double[size];
		//double *shotdata = new double[size];  //pointer good?
		timebase[i]=new double[size];
		shotdata[i]=new double[size];

		if(mdsplusdata->getShotData(timebase[i], shotdata[i], size)<0) //if(mdsplusdata->getShotData()<0)
		{
			continue;
		};
		//if (i == displaycount-1) for_each(&timebase[0], &timebase[size], add11);
		if(i == displaycount-1)	for_each(&timebase[i][0], &timebase[i][size], AddValue<double>(getBaseTime()));
		if(d_data->mfactor != 0.0)	
		{
			if(nodename.compare("ICRF_FWD") != 0)
				for_each(&shotdata[i][0], &shotdata[i][size], MultipleValue<double>(getFactor()));
		};

		d_data->mpCurve[i]->setRawData(timebase[i], shotdata[i], size);
		double maxvalue = *max_element(shotdata[i], &shotdata[i][size]);

		if(nodename.compare("KAPPA") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxKappa(strmaxval);
		}
		else if(nodename.compare("BETAN") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxBetan(strmaxval);
		}
		else if(nodename.compare("WTOT_DLM03") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxWtot(strmaxval);
		}
		else if(nodename.compare("ECH_VFWD1") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxECH(strmaxval);
		}
		else if(nodename.compare("ICRF_FWD") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxICRH(strmaxval);
		}
		else if(nodename.compare("NB11_PNB") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxNB1(strmaxval);
		}
		else if(nodename.compare("NB12_PNB") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxNB2(strmaxval);
		}
		else if(nodename.compare("PCRC03") == 0)
		{
			maxvalue = maxvalue/1000000;	//MA
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxIp(strmaxval);
		}
		else if(nodename.compare("NE_INTER01") == 0)
		{
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxNe(strmaxval);
		}
		else if(nodename.compare("POL_HA05") == 0)
		{
			maxvalue = maxvalue/1000000000;
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxHa(strmaxval);
		}
		else if(nodename.compare("ECE02") == 0)
		{
			maxvalue = maxvalue/1000;
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxTe(strmaxval);
		}
		else if(nodename.compare("B_PKR251A") == 0)
		{
			maxvalue = maxvalue*100000.;
			QString strmaxval = QString("%1").arg(maxvalue,0, 'f', 1);
			emit signalMaxPre(strmaxval);
		}
		displayCurve(d_data->mpCurve[i], false);
	};
	replot();
#if 1
	//if(d_data->mnodename[0]=="PCVCM03")
	if(d_data->mnodename[0]=="PCRC03") //2012
	{
		QString strshotno = QString("#%1").arg(shotno);
		emit signalShotNumber(strshotno);
		screenShot(shotno, "pcsdata_shot");

		string strCmd = "caput ICS_PCSDATA_SCREENSHOT 1";
		system(strCmd.c_str());
	}
	else if(d_data->mnodename[0]=="KAPPA")
	{
		QString strshotno = QString("#%1").arg(shotno);
		emit signalShotNumber(strshotno);
		screenShot(shotno, "daqdata_shot");

		string strCmd = "caput ICS_DAQDATA_SCREENSHOT 1";
		system(strCmd.c_str());
	};
#endif
#if 1
#if 0
	if(timebase) 
	{
		qDebug("Destroy timebase");
		delete timebase;
	};
#endif
	for(int i = 0;i<displaycount;i++)
	{
		qDebug("Destroy shotdata[%d]",i);
		if(shotdata[i]) delete shotdata[i];
		if(timebase[i]) delete timebase[i];
	};
#endif
}
#else
void CAMDSplusplot::readShot()
{
	//memory allocation test.
	getNodeNameAll();
	string treename = d_data->mtreename.toStdString();
	int shotno = d_data->mshotno;
	int displaycount = d_data->mvecnode.size();
	bool check;

	if (displaycount == 0) return;

	mdsplusdata->setMDSDataInfo(treename,d_data->mvecnode.at(0),shotno,check);
	int size = mdsplusdata->getNodeSize();

	//clear();

	if(size < 0 ) return;

	qDebug("1-Display Count:%d, ArraySize:%d", displaycount,size);

	double *timebase = 0;
	double *shotdata = 0;

	for(int i = 0;i<displaycount;i++)
	{
		string nodename = d_data->mnodename[i].toStdString();
		if(nodename.empty() == true) continue;
		QWidget *w = legend()->find(d_data->mpCurve[i]);
		if(w == 0)
		{
			if(d_data->mpCurve[i]==0)
			{
				d_data->mpCurve[i] = new QwtPlotCurve(d_data->mnodename[i]);
				d_data->mpen[i].setWidth(3);
				d_data->mpCurve[i]->setPen(d_data->mpen[i]);
				d_data->mpCurve[i]->setStyle(QwtPlotCurve::Steps);
				d_data->mpCurve[i]->attach(this);
			};
		};
		mdsplusdata->setMDSDataInfo(treename,nodename,shotno,check);
		//if(check == MDSPlusData::FAIL) break;

		int size = mdsplusdata->getNodeSize();
		if(size < 0) continue;

		timebase = new double[size];
		shotdata = new double[size];  //pointer good?


		if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
		{
			continue;
		};

		if(i == displaycount-1)	for_each(&timebase[0], &timebase[size], AddValue<double>(getBaseTime()));
		if(d_data->mfactor != 0.0)	for_each(&shotdata[0], &shotdata[size], MultipleValue<double>(getFactor()));

		d_data->mpCurve[i]->setRawData(timebase, shotdata, size);
		displayCurve(d_data->mpCurve[i], false);
	};
	replot();
#if 1
		if(timebase) 
		{
			qDebug("Destroy timebase");
			delete timebase;
		};
		if(shotdata) 
		{
			qDebug("Destroy shotdata");
			if(shotdata) delete shotdata;
		};
#endif
#if 1
	if(d_data->mnodename[0]=="PCVCM03")
	{
		QString strshotno = QString("#%1").arg(shotno);
		emit signalShotNumber(strshotno);
		screenShot(shotno, "pcsdata_shot");
	}
	else if(d_data->mnodename[0]=="PCLV23")
	{
		QString strshotno = QString("#%1").arg(shotno);
		emit signalShotNumber(strshotno);
		screenShot(shotno, "daqdata_shot");
		//string shotResultCmd = "caput DEV02_SHOTRESULT 1";
		//system(shotResultCmd.c_str());
	};
#endif
}
#endif
void CAMDSplusplot::autoManual(const bool bAuto)
{
	//Channel Access Thread, 
	QPushButton *automanual = qobject_cast<QPushButton *>(sender());
	if(bAuto)
	{
		automanual ->setText("Manual");
		//Thread Start.
		mbAuto = false;
		mpTimer->start();
	}
	else
	{
		automanual ->setText("Auto");
		//"CCS_PERFORM_SHOT_SUMMARY"
		//Thread Stop.
		mbAuto = true;
		mpTimer->stop();
	};
}
void CAMDSplusplot::penStyle(const QString strstyle)
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
void CAMDSplusplot::penColor(const QString color)
{
	d_data->mpen[d_data->mpenindex].setColor(QColor(color));
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	signalColor(d_data->mpenindex);
	replot();
}

void CAMDSplusplot::signalColor(const int index)
{
	switch(index)
	{
		case 0:
			emit legendLabel(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 1:
			emit legendLabel1(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 2:
			emit legendLabel2(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 3:
			emit legendLabel3(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 4:
			emit legendLabel4(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 5:
			emit legendLabel5(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 6:
			emit legendLabel6(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 7:
			emit legendLabel7(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 8:
			emit legendLabel8(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		case 9:
			emit legendLabel9(d_data->mpen[index].color().name(), d_data->mvecnode.at(index).c_str());
			break;
		default:
			break;
	};
}

void CAMDSplusplot::penColor(const QColor &color)
{
	d_data->mpen[d_data->mpenindex].setColor(color);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}
void CAMDSplusplot::factor(const QString strfactor)
{
	double dfactor = strfactor.toDouble();
	if(dfactor == 0.0) dfactor = 1.0;
	//d_data->mfactor[d_data->mpenindex] = dfactor;
}
void CAMDSplusplot::nonfactors()
{
	//std::fill(&d_data->mfactor[0],&d_data->mfactor[PrivateData::PLOT_SIZE], 1.0);
}
QString CAMDSplusplot::getTime(const int &time)
{
	int hour = time/3600;
	int min = (time%3600)/60;
	int sec = (time%3600)%60;
	char strtime[20];
	sprintf(strtime,"%02d:%02d:%02d",hour, min, sec);
	return QString(strtime);
};
void CAMDSplusplot::setLogscale(const bool setlog)
{
    d_data->mblog = setlog;
}

const bool &CAMDSplusplot::getLogscale() const
{
	return d_data->mblog;
}

void CAMDSplusplot::setPlottitle(const QString &title)
{
#if 1
    d_data->mtitle = title;
	setTitle(title.toStdString().c_str());
	plotupdate();
#endif
}
const QString &CAMDSplusplot::getPlottitle() const
{
	return d_data->mtitle;
}

void CAMDSplusplot::setXtitle(const QString &title)
{
    setAxisTitle(QwtPlot::xBottom, title.toStdString().c_str());
	d_data->mxtitle = title;
	plotupdate();
}
const QString &CAMDSplusplot::getXtitle() const
{
	return d_data->mxtitle;
}

void CAMDSplusplot::setYtitle(const QString &title)
{
    setAxisTitle(QwtPlot::yLeft, title.toStdString().c_str());
	d_data->mytitle = title;
	plotupdate();
}
const QString &CAMDSplusplot::getYtitle() const
{
	return d_data->mytitle;
}

void CAMDSplusplot::setCanvascolor(const QColor &color)
{
	setCanvasBackground(color);
	d_data->mcanvascolor = color;
	plotupdate();
}
const QColor &CAMDSplusplot::getCanvascolor()
{
	return d_data->mcanvascolor;
}

void CAMDSplusplot::setMajorlinecolor(const QColor &color)
{
	d_data->mmajlinecolor = color;
	d_data->mgrid->setMajPen(QPen(color, 0, Qt::DashLine));
	plotupdate();
}
const QColor &CAMDSplusplot::getMajorlinecolor()
{
	return d_data->mmajlinecolor;
}

void CAMDSplusplot::setMinorlinecolor(const QColor &color)
{
	d_data->mgrid->setMinPen(QPen(color , 0, Qt::DotLine));
	d_data->mminlinecolor = color;
	plotupdate();
}

const QColor &CAMDSplusplot::getMinorlinecolor()
{
	return d_data->mminlinecolor;
}

const int &CAMDSplusplot::getREQNOE()
{
	return d_data->mreqelement;
}

void CAMDSplusplot::setREQNOE(const int &reqelement)
{
	d_data->mreqelement = reqelement;
	plotupdate();
}
const int &CAMDSplusplot::getUpdateTime()
{
	return d_data->mmsec;
}
void 
CAMDSplusplot::setUpdateTime(const int &msec)
{
	d_data->mmsec = msec;
	plotupdate();
}
const int &CAMDSplusplot::getRefreshTime()
{
	return d_data->mmrefresh;
}
void CAMDSplusplot::setRefreshTime(const int &msec)
{
	d_data->mmrefresh = msec;
	plotupdate();
}
const double &CAMDSplusplot::getBaseTime() const 
{
	return d_data->mbasetime;
}
void CAMDSplusplot::setBaseTime(const double &basetime)
{
	d_data->mbasetime = basetime;
	plotupdate();
}
const double &CAMDSplusplot::getFactor() const 
{
	return d_data->mfactor;
}
void CAMDSplusplot::setFactor(const double &factor)
{
	d_data->mfactor = factor;
	plotupdate();
}
CAMDSplusplot::TypeOfCall CAMDSplusplot::getType() const
{
	return d_data->mtype;
}
void 
CAMDSplusplot::setType(CAMDSplusplot::TypeOfCall type)
{
	d_data->mtype = type;
	plotupdate();
}

void CAMDSplusplot::plotupdate()
{
	replot();
	update();
}
const long CAMDSplusplot::getShotNumber()
{
	return 0L;
}
void 
CAMDSplusplot::screenShot(const int shotno,const QString strType)
{
	QString strShotno = QString("%1.png").arg(shotno);
	QString filename = QString("/home/kstar/screenshot/")+strType+strShotno;
	qDebug("screeShot Filename:%s", filename.toStdString().c_str());
	QString format = "png";
	QWidget *parentWidget = (QWidget*)parent();
	//QPixmap mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), 0, 25,1280,995);
	QPixmap mpixmap = QPixmap::grabWidget(parentWidget,0,15,1280,995);
	mpixmap.save(filename, format.toAscii());
}

