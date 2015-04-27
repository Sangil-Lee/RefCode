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
#include "caMultiwaveplot.h"

//#include "caArrayData.h"
using namespace std;

void SetProperty_Form::SetParent(CAMultiwaveplot *pPlot)
{
	pWaveplot = pPlot;
}

void SetProperty_Form::DeleteForm()
{
	  if(pWaveplot) pWaveplot->ResetChild();
}

void SetProperty_Form::ConnectSet()
{
	connect(XMaxSB, SIGNAL(valueChanged(double)), this, SLOT(XMaxValue(double)));
	connect(XMinSB, SIGNAL(valueChanged(double)), this, SLOT(XMinValue(double)));
	connect(YMaxSB, SIGNAL(valueChanged(double)), this, SLOT(YMaxValue(double)));
	connect(YMinSB, SIGNAL(valueChanged(double)), this, SLOT(YMinValue(double)));
	connect(WaveColorPB, SIGNAL(clicked(bool)), this, SLOT(PenColor(bool)));
	connect(LineWidthSB, SIGNAL(valueChanged(int)), this, SLOT(PenWidth(int)));
	connect(ShowHidePB, SIGNAL(clicked(bool)), this, SLOT(PenShowHide(bool)));
	connect(WavePVList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(ListItemInfo(QListWidgetItem *)));
}

void SetProperty_Form::ListItemInfo(QListWidgetItem *pItem)
{
	if(pItem)
	{
		int index = WavePVList->currentRow();
		WaveColorPB->setPalette(QPalette(pItem->background().color()));
		const bool show = pWaveplot->getDisplayCurve(index);
		const int width = pWaveplot->getPenWidth(index);

		LineWidthSB->setValue(width);
		ShowHidePB->setChecked(show);
		if(show)
			ShowHidePB->setText("Hide");
		else
			ShowHidePB->setText("Show");
	};


};

void SetProperty_Form::PenShowHide(bool show)
{
	QListWidgetItem *pItem = WavePVList->currentItem();
	QPushButton *pBut = qobject_cast<QPushButton *>(sender());
	if(pItem)
	{
		pWaveplot->penIndex(WavePVList->currentRow());
		pWaveplot->displayCurve(show);
		if(pBut->isChecked() == true)
			pBut->setText("Hide");
		else
			pBut->setText("Show");
	};
}

void SetProperty_Form::PenWidth(int width)
{
	QListWidgetItem *pItem = WavePVList->currentItem();
	if(pItem)
	{
		pWaveplot->penIndex(WavePVList->currentRow());
		pWaveplot->penWidth(width);
	};
};

void SetProperty_Form::PenColor(bool value)
{
	QColor color = QColorDialog::getColor(Qt::green, this);
	QListWidgetItem *pItem = WavePVList->currentItem();

	if (color.isValid()) 
	{
		if(pItem)
		{
			pItem->setBackground(QBrush(color));
			WaveColorPB->setPalette(QPalette(color));
			pWaveplot->penIndex(WavePVList->currentRow());
			pWaveplot->penColor(color.name());
			pWaveplot->IndexLabelColorChange(WavePVList->currentRow(), color.name());
		};
	};
};

void SetProperty_Form::XMaxValue(double value)
{
	if(pWaveplot == NULL)  return;
    pWaveplot->setXRangeMax(value);
};

void SetProperty_Form::XMinValue(double value)
{
	if(pWaveplot == NULL)  return;
    pWaveplot->setXRangeMin(value);
};
void SetProperty_Form::YMaxValue(double value)
{
	if(pWaveplot == NULL)  return;
    pWaveplot->setYRangeMax(value);
};
void SetProperty_Form::YMinValue(double value)
{
	if(pWaveplot == NULL)  return;
    pWaveplot->setYRangeMin(value);
};
void SetProperty_Form::SetPVNames(vector<string> &pvnames)
{ 
	size_t pvcnt = pvnames.size();
	QListWidgetItem *pItem = 0;
	for(size_t i = 0;i<pvcnt;i++)
	{
	//	qDebug("PVName[%d]:%s",i,pvnames.at(i).c_str());
		pItem = new QListWidgetItem(pvnames.at(i).c_str());
		pItem->setBackground(QBrush(QColor(pWaveplot->getColorName(i))));
		WavePVList->insertItem(i,pItem);
		//pWaveplot->getColorName(i);
	};
};

void SetProperty_Form::showPVList()
{
}

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

class CAMultiwaveplot::PrivateData
{
	public:
		//member method
		PrivateData():mpenindex(0), mcanvascolor(QColor(Qt::gray)), mmajlinecolor(QColor(Qt::black)), 
					  mminlinecolor(QColor(Qt::gray)),mreqelement(1024),mmsec(1000),mmrefresh(1000),mbuffpage(1)
		{
			init();
		};
	public:
		//member data
		static const int PLOT_SIZE  = 1024;
		static const int MAX_PV_CNT = 15;
		QString mpvname[MAX_PV_CNT];
		QPen mpen[MAX_PV_CNT];
		double	**marrydata;
		double	*mtimeline;
		double	*mfactor;

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
		int mreqelement;
		int mmsec;
		int mmrefresh;
		int mbuffpage;
		TypeOfCall mtype;
		vector<string> m_vecpvnames;
		vector<chid> m_node;
		int mtimerID;
		double mymin, mymax;
		double mxmin, mxmax;
		QwtPlotPicker *mpicker;
		virtual ~PrivateData();
		WaveForm::SetProperty *pProperty;
	private:
		void init();
		void datafree();
		void dataAlloc();
};

CAMultiwaveplot::PrivateData::~PrivateData()
{
	datafree();
}
void CAMultiwaveplot::PrivateData::init()
{
	dataAlloc();
	std::fill(&mfactor[0],&mfactor[PLOT_SIZE], 1.0);
}
void CAMultiwaveplot::PrivateData::datafree()
{
	free(mtimeline);
	free(mfactor);
}
void CAMultiwaveplot::PrivateData::dataAlloc()
{
//	mtimeline = (double*)calloc(mreqelement, sizeof(double));
	mfactor = (double*)calloc(mreqelement, sizeof(double));
}

CAMultiwaveplot::CAMultiwaveplot(QWidget *parent):QwtPlot(parent)
{
    initMultiPlot();
}

CAMultiwaveplot::~CAMultiwaveplot()
{
	if(!d_data) 
	{
		delete d_data;
		d_data = 0;
	};
	if(!mcaarraydatathread)
	{
		if(mcaarraydatathread->isRunning())
			mcaarraydatathread->wait();
		delete mcaarraydatathread;
		mcaarraydatathread = 0;
	};
}

QSize CAMultiwaveplot::sizeHint() const
{
	return minimumSizeHint();
}

void CAMultiwaveplot::initMultiPlot()
{
    //using namespace Qt;
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("CAMultiwaveplot"));

	d_data->mgrid = new QwtPlotGrid;
	d_data->mgrid->enableXMin(true);
	d_data->mgrid->enableYMin(true);
	d_data->mgrid->setMajPen(QPen(Qt::black, 0, Qt::DashLine));
	d_data->mgrid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	d_data->mgrid->attach(this);
    setCanvasLineWidth(1);
	setLogscale(false);
	setAutoReplot(false);
	canvas()->setCursor(Qt::PointingHandCursor);
	canvas()->installEventFilter(this);
#if 1
	mcaarraydatathread = new QtCAArrayDataThread;
	mcaarraydatathread->SetParent(this); 
	setAxisScale(QwtPlot::yLeft, -15000, 15000);
	d_data->pProperty = NULL;

#endif
};

#if 1
bool CAMultiwaveplot::eventFilter(QObject *o, QEvent *e)
{
	if(o != (QObject*)canvas()) return false;

	QMouseEvent *pE = (QMouseEvent*)e;
	if (e->type() == QEvent::MouseButtonPress)
	{
		if(pE->button() == Qt::RightButton ) 
		{
			ShowProperty();
		};
	}
	return QObject::eventFilter(o, e);
}
#endif

QSize CAMultiwaveplot::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAMultiwaveplot::setPvname(const QString &name)
{
    d_data->mpvname[0] = name;
}

const QString &CAMultiwaveplot::getPvname() const
{
	return d_data->mpvname[0];
}

void CAMultiwaveplot::setPvname1(const QString &name)
{
    d_data->mpvname[1] = name;
}

const QString &CAMultiwaveplot::getPvname1() const
{
	return d_data->mpvname[1];
}

void CAMultiwaveplot::setPvname2(const QString &name)
{
    d_data->mpvname[2] = name;
}

const QString &CAMultiwaveplot::getPvname2() const
{
	return d_data->mpvname[2];
}

void CAMultiwaveplot::setPvname3(const QString &name)
{
    d_data->mpvname[3] = name;
}

const QString &CAMultiwaveplot::getPvname3() const
{
	return d_data->mpvname[3];
}

void CAMultiwaveplot::setPvname4(const QString &name)
{
    d_data->mpvname[4] = name;
}

const QString &CAMultiwaveplot::getPvname4() const
{
	return d_data->mpvname[4];
}

void CAMultiwaveplot::setPvname5(const QString &name)
{
    d_data->mpvname[5] = name;
}

const QString &CAMultiwaveplot::getPvname5() const
{
	return d_data->mpvname[5];
}

void CAMultiwaveplot::setPvname6(const QString &name)
{
    d_data->mpvname[6] = name;
}

const QString &CAMultiwaveplot::getPvname6() const
{
	return d_data->mpvname[6];
}

void CAMultiwaveplot::setPvname7(const QString &name)
{
    d_data->mpvname[7] = name;
}

const QString &CAMultiwaveplot::getPvname7() const
{
	return d_data->mpvname[7];
}

void CAMultiwaveplot::setPvname8(const QString &name)
{
    d_data->mpvname[8] = name;
}

const QString &CAMultiwaveplot::getPvname8() const
{
	return d_data->mpvname[8];
}

void CAMultiwaveplot::setPvname9(const QString &name)
{
    d_data->mpvname[9] = name;
}

const QString &CAMultiwaveplot::getPvname9() const
{
	return d_data->mpvname[9];
}

const QString &CAMultiwaveplot::getPvname10() const
{
	return d_data->mpvname[10];
}
void CAMultiwaveplot::setPvname10(const QString &name)
{
    d_data->mpvname[10] = name;
}
const QString &CAMultiwaveplot::getPvname11() const
{
	return d_data->mpvname[11];
}
void CAMultiwaveplot::setPvname11(const QString &name)
{
    d_data->mpvname[11] = name;
}
const QString &CAMultiwaveplot::getPvname12() const
{
	return d_data->mpvname[12];
}
void CAMultiwaveplot::setPvname12(const QString &name)
{
    d_data->mpvname[12] = name;
}
const QString &CAMultiwaveplot::getPvname13() const
{
	return d_data->mpvname[13];
}
void CAMultiwaveplot::setPvname13(const QString &name)
{
    d_data->mpvname[13] = name;
}
const QString &CAMultiwaveplot::getPvname14() const
{
	return d_data->mpvname[14];
}
void CAMultiwaveplot::setPvname14(const QString &name)
{
    d_data->mpvname[14] = name;
}
void CAMultiwaveplot::changePvName (const int index, const QString &pvname)
{
    d_data->mpvname[index] = pvname;
}

void CAMultiwaveplot::getPvNames ()
{
	QString strpv;
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
void CAMultiwaveplot::PrintPvNames ()
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
void CAMultiwaveplot::Start()
{
	mcaarraydatathread->setStop(false);
	mcaarraydatathread->start();
}
void CAMultiwaveplot::Stop()
{
	mcaarraydatathread->setStop(true);
	if(mcaarraydatathread->isRunning())
		mcaarraydatathread->wait();

}
void CAMultiwaveplot::SetDataPtr(const int index, void *data)
{
#if 0
	qDebug("NumberOfElement(1Page):%d, (TotalPage - 1Page): %d",d_data->mreqelement, (d_data->mreqelement*getBufferPage())-d_data->mreqelement);
	//d_data->mbuffer[i] = (double*)calloc(1,  sizeof(double)*d_data->mreqelement*getBufferPage());
	//memmove(d_data->mbuffer[index], d_data->mbuffer[index]+d_data->mreqelement, (d_data->mreqelement*getBufferPage()-d_data->mreqelement)*sizeof(double));
	if ( index == 0 )
	{
		double *bfdata  = d_data->mbuffer[index]+d_data->mreqelement;
		for (int i = 0; i < 10; i++)
		{
			qDebug("Bef-Data[%d]:%f",index, bfdata[i]);
		};
	};
#endif
	//memmove(d_data->mbuffer[index], d_data->mbuffer[index]+(d_data->mreqelement*(getBufferPage()-1)), 
	//		(d_data->mreqelement*getBufferPage()-d_data->mreqelement)*sizeof(double));
	//size_t pvcnt = d_data->m_vecpvnames.size();
#if 1
	for(int i = 1; i<getBufferPage(); i++ )
	{
		memmove(d_data->mbuffer[index]+(d_data->mreqelement*(i-1)), 
				d_data->mbuffer[index]+(d_data->mreqelement*i),
				d_data->mreqelement*sizeof(double));
	};
#else
	memmove(d_data->mbuffer[index], 
			d_data->mbuffer[index]+d_data->mreqelement,
			d_data->mreqelement*sizeof(double));

	memmove(d_data->mbuffer[index]+d_data->mreqelement, 
			d_data->mbuffer[index]+(d_data->mreqelement*(getBufferPage()-1)),
			d_data->mreqelement*sizeof(double));
#endif

	d_data->marrydata[index] = (double *)data;
	memcpy(d_data->mbuffer[index]+(d_data->mreqelement*(getBufferPage()-1)), data, d_data->mreqelement*sizeof(double));
	//memmove(d_data->mbuffer[index], data, d_data->mreqelement*sizeof(double));
	//memmove(d_data->mbuffer[index]+d_data->mreqelement,d_data->mbuffer[index], (d_data->mreqelement*getBufferPage())-d_data->mreqelement);
#if 0
	d_data->mbuffer[index] = (double *)data;
	if ( index == 0 )
	{
		for (int i = 0; i < 10; i++)
		{
			qDebug("Cur-Data[%d]:%f",index, d_data->mbuffer[index][i]);
		};
	};
#endif
}
void CAMultiwaveplot::Run()
{
	getPvNames();
    d_data->mpicker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::PointSelection | QwtPicker::DragSelection, 
        QwtPlotPicker::HLineRubberBand, QwtPicker::ActiveOnly, canvas());
    d_data->mpicker->setRubberBandPen(QColor(Qt::green));
    d_data->mpicker->setTrackerPen(QColor(Qt::black));

	if(getLogscale()==true)
	{
		setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
	};
	PrintPvNames();

	size_t pvcnt = d_data->m_vecpvnames.size();
	d_data->marrydata = (double **)calloc(pvcnt, sizeof(double*));
	d_data->mbuffer  = (double**)calloc(pvcnt, sizeof(double*));
	d_data->mtimeline = (double*)calloc(d_data->mreqelement*getBufferPage(), sizeof(double));
#if 1

	int timeline = d_data->mreqelement*getBufferPage();
    //for (int i = 0; i < d_data->mreqelement*2; i++)
    for (int i = 0; i < timeline; i++)
    {
		d_data->mtimeline[timeline-1-i] = i;
    };

	for( size_t i = 0; i<pvcnt; i++)
	{
		d_data->mbuffer[i] = (double*)calloc(1,  sizeof(double)*d_data->mreqelement*getBufferPage());
	};

	setAxisScale(QwtPlot::xBottom, d_data->mtimeline[0], d_data->mtimeline[timeline-1]);
	//setAxisScale(QwtPlot::xBottom, d_data->mtimeline[timeline-1], d_data->mtimeline[0]);

	mcaarraydatathread->SetPVNames(d_data->m_vecpvnames);
	mcaarraydatathread->SetReqEON(d_data->mreqelement);
	mcaarraydatathread->SetUpdateTime(d_data->mmsec);
	mcaarraydatathread->start();

	d_data->mtimerID=startTimer(d_data->mmrefresh);

	d_data->mpen[0].setBrush(Qt::blue);
	d_data->mpen[1].setBrush(Qt::green);
	d_data->mpen[2].setBrush(Qt::red);
	d_data->mpen[3].setBrush(Qt::darkRed);
	d_data->mpen[4].setBrush(Qt::black);
	d_data->mpen[5].setBrush(Qt::magenta);
	d_data->mpen[6].setBrush(Qt::darkMagenta);
	d_data->mpen[7].setBrush(Qt::yellow);
	d_data->mpen[8].setBrush(Qt::white);
	d_data->mpen[9].setBrush(Qt::cyan);
	d_data->mpen[10].setBrush(Qt::darkYellow);
	d_data->mpen[11].setBrush(Qt::darkBlue);
	d_data->mpen[12].setBrush(Qt::darkCyan);
	d_data->mpen[13].setBrush(Qt::darkGray);
	d_data->mpen[14].setBrush(Qt::darkGreen);
	for(int i = 0; i < PrivateData::MAX_PV_CNT; i++) d_data->mpen[i].setWidth(2);
	userUpdate();

#endif
}
void CAMultiwaveplot::SetData()
{
    // Attach data. max 10 curves use the same timeline array.
	size_t pvcnt = d_data->m_vecpvnames.size();
	for(size_t index = 0; index<pvcnt; index++ )
	{
		//d_data->mpCurve[index] = new QwtPlotCurve(d_data->m_vecpvnames.at(index).c_str()); //for Legend
		d_data->mpCurve[index] = new QwtPlotCurve();
		d_data->mpCurve[index]->setPen(d_data->mpen[index]);
		d_data->mpCurve[index]->setStyle(QwtPlotCurve::Steps);
		d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->mbuffer[index], d_data->mreqelement*getBufferPage());
		//d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->mbuffer[index], d_data->mreqelement*getBufferPage());
		//d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->mbuffer[index], d_data->mreqelement);
		//d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->marrydata[index], d_data->mreqelement);
		//d_data->mpCurve[index]->setRawData(d_data->mtimeline, d_data->marrydata[index], d_data->mreqelement*getBufferPage());
		d_data->mpCurve[index]->attach(this);
		//displayCurve(d_data->mpCurve[index], true);
	};
}
void CAMultiwaveplot::timerEvent(QTimerEvent *)
{
	replot();
}
void CAMultiwaveplot::displayCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);
    
    replot();
}

void CAMultiwaveplot::displayCurve(const bool show)
{
	d_data->mpCurve[d_data->mpenindex]->setVisible(show);
	replot();
}

void CAMultiwaveplot::displayCurve(const bool on, const QString &pvname)
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

void CAMultiwaveplot::setYRange(const double min, const double max)
{
	setAxisScale(QwtPlot::yLeft, min, max);
	replot();
}

void CAMultiwaveplot::penIndex(const int index)
{
	d_data->mpenindex = index;
}
void CAMultiwaveplot::penWidth(const int width)
{
	d_data->mpen[d_data->mpenindex].setWidth(width);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}

const int CAMultiwaveplot::getPenWidth(const int index)
{
	return d_data->mpen[index].width();
}

void CAMultiwaveplot::penStyle(const QString strstyle)
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

void CAMultiwaveplot::penColor(const QString color)
{
	d_data->mpen[d_data->mpenindex].setColor(QColor(color));
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}

QString CAMultiwaveplot::getColorName(const int index)
{
	return d_data->mpen[index].color().name();
}

const bool CAMultiwaveplot::getDisplayCurve(const int index)
{
	bool show = true;
	if(d_data->mpCurve[index]->isVisible() == true)
		show = true;
	else
		show = false;
	return show;
}

void CAMultiwaveplot::penColor(const QColor &color)
{
	d_data->mpen[d_data->mpenindex].setColor(color);
	d_data->mpCurve[d_data->mpenindex]->setPen(d_data->mpen[d_data->mpenindex]);
	replot();
}
void CAMultiwaveplot::factor(const QString strfactor)
{
	double dfactor = strfactor.toDouble();
	if(dfactor == 0.0) dfactor = 1.0;
	d_data->mfactor[d_data->mpenindex] = dfactor;
}
void CAMultiwaveplot::nonfactors()
{
	std::fill(&d_data->mfactor[0],&d_data->mfactor[PrivateData::PLOT_SIZE], 1.0);
}
void CAMultiwaveplot::userUpdate()
{
	static bool bclick = false;
	QPushButton *puserU = qobject_cast<QPushButton *>(sender());
	if(!puserU) return;

	if (bclick == true)
	{
		puserU -> setText("Stop");
		bclick = false;
		//mcaarraydatathread->SetUpdatable(bclick);
	}
	else
	{
		puserU -> setText("Start");
		bclick = true;
		mcaarraydatathread->SetUpdatable(bclick);
	};
}

QString CAMultiwaveplot::getTime(const int &time)
{
	int hour = time/3600;
	int min = (time%3600)/60;
	int sec = (time%3600)%60;
	char strtime[20];
	sprintf(strtime,"%02d:%02d:%02d",hour, min, sec);
	return QString(strtime);
};
void CAMultiwaveplot::setLogscale(const bool setlog)
{
    d_data->mblog = setlog;
}

const bool &CAMultiwaveplot::getLogscale() const
{
	return d_data->mblog;
}

void CAMultiwaveplot::setPlottitle(const QString &title)
{
    d_data->mtitle = title;
	setTitle(title.toStdString().c_str());
	plotupdate();
}
const QString &CAMultiwaveplot::getPlottitle() const
{
	return d_data->mtitle;
}

void CAMultiwaveplot::setXtitle(const QString &title)
{
    setAxisTitle(QwtPlot::xBottom, title.toStdString().c_str());
	d_data->mxtitle = title;
	plotupdate();
}
const QString &CAMultiwaveplot::getXtitle() const
{
	return d_data->mxtitle;
}

void CAMultiwaveplot::setYtitle(const QString &title)
{
    setAxisTitle(QwtPlot::yLeft, title.toStdString().c_str());
	d_data->mytitle = title;
	plotupdate();
}
const QString &CAMultiwaveplot::getYtitle() const
{
	return d_data->mytitle;
}

void CAMultiwaveplot::setCanvascolor(const QColor &color)
{
	setCanvasBackground(color);
	d_data->mcanvascolor = color;
	plotupdate();
}
const QColor &CAMultiwaveplot::getCanvascolor()
{
	return d_data->mcanvascolor;
}

void CAMultiwaveplot::setMajorlinecolor(const QColor &color)
{
	d_data->mmajlinecolor = color;
	d_data->mgrid->setMajPen(QPen(color, 0, Qt::DashLine));
	plotupdate();
}
const QColor &CAMultiwaveplot::getMajorlinecolor()
{
	return d_data->mmajlinecolor;
}

void CAMultiwaveplot::setMinorlinecolor(const QColor &color)
{
	d_data->mgrid->setMinPen(QPen(color , 0, Qt::DotLine));
	d_data->mminlinecolor = color;
	plotupdate();
}

const QColor &CAMultiwaveplot::getMinorlinecolor()
{
	return d_data->mminlinecolor;
}

const int &CAMultiwaveplot::getREQNOE()
{
	return d_data->mreqelement;
}

void CAMultiwaveplot::setREQNOE(const int &reqelement)
{
	d_data->mreqelement = reqelement;
	plotupdate();
}
const int &CAMultiwaveplot::getUpdateTime()
{
	return d_data->mmsec;
}
void 
CAMultiwaveplot::setUpdateTime(const int &msec)
{
	d_data->mmsec = msec;
	plotupdate();
}
const int &CAMultiwaveplot::getRefreshTime()
{
	return d_data->mmrefresh;
}
void CAMultiwaveplot::setRefreshTime(const int &msec)
{
	d_data->mmrefresh = msec;
	plotupdate();
}
const int &CAMultiwaveplot::getBufferPage()
{
	return d_data->mbuffpage;
}
void CAMultiwaveplot::setBufferPage(const int &pagecnt)
{
	d_data->mbuffpage = pagecnt;
	plotupdate();
}
CAMultiwaveplot::TypeOfCall CAMultiwaveplot::getType() const
{
	return d_data->mtype;
}
void 
CAMultiwaveplot::setType(CAMultiwaveplot::TypeOfCall type)
{
	d_data->mtype = type;
	plotupdate();
}

void CAMultiwaveplot::plotupdate()
{
	replot();
	update();
}

void CAMultiwaveplot::ShowProperty()
{
	if(d_data->pProperty == NULL)
	{
		d_data->pProperty = new WaveForm::SetProperty();
		d_data->pProperty -> SetParent(this);
		d_data->pProperty -> SetPVNames(d_data->m_vecpvnames);
	};
}
void CAMultiwaveplot::setXRangeMax(const double max)
{
	if(d_data->pProperty == NULL) return;
	d_data->mxmax = max;
	d_data->mxmin = d_data->pProperty->XMinSB->value();
	setAxisScale(QwtPlot::xBottom, d_data->mxmax, d_data->mxmin);
	replot();
}

void CAMultiwaveplot::setXRangeMin(const double min)
{
	if(d_data->pProperty == NULL) return;
	d_data->mxmax = d_data->pProperty->XMaxSB->value();
	d_data->mxmin = min; 
	setAxisScale(QwtPlot::xBottom, d_data->mxmax, d_data->mxmin);
	replot();
}
void CAMultiwaveplot::setYRangeMax(const double max)
{
	if(d_data->pProperty == NULL) return;
	d_data->mymax = max;
	d_data->mymin = d_data->pProperty->YMinSB->value();
	setAxisScale(QwtPlot::yLeft, d_data->mymin, d_data->mymax);
	replot();
}

void CAMultiwaveplot::setYRangeMin(const double min)
{
	if(d_data->pProperty == NULL) return;
	d_data->mymax = d_data->pProperty->YMaxSB->value();
	d_data->mymin = min; 
	setAxisScale(QwtPlot::yLeft, d_data->mymin, d_data->mymax);
	replot();
}
void CAMultiwaveplot::IndexLabelColorChange(int index, QString colorname)
{
	emit changeIndexColor(index, colorname);
}

void CAMultiwaveplot::ResetChild() 
{
	d_data->pProperty = NULL; 
};
