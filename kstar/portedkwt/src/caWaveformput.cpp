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
#include "wavePicker.h"
#include "caWaveformput.h"
#include "pv.h"

using namespace std;
void nUpdatesTester ( struct event_handler_args args )
{
	if ( args.status == ECA_NORMAL ) {
		unsigned *pCtr = (unsigned *) args.usr;
		( *pCtr ) ++;
	}    
	else {
		printf ( "subscription update failed for \"%s\" because \"%s\"", 
				ca_name ( args.chid ), ca_message ( args.status ) ); 
	}    
}

void nordEvent( void *obj, pvType /*type*/, int /*count*/, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	if(var == NULL) return;

	short value = val->shortVal[0];
    qDebug( "nordEvnet: %s=%d", var->getName(), value);

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	pWave->SetWavePVCount(value);
}

void countPvEvent( void *obj, pvType /*type*/, int /*count*/, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

	static bool init = false;
    pvVariable *var = ( pvVariable * ) obj;
	if(var == NULL) return;

	short value = val->shortVal[0];
    printf( "countPV: %s=%d\n", var->getName(), value);

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	pWave->SetWavePVCount(value);

	if(init == true)
	{
		pWave->Update(value);
		//pWave->ReSet();
	};
	init = true;
}

void treeResetEvent( void *obj, pvType /*type*/, int /*count*/, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	if(var == NULL) return;
	short value = val->shortVal[0];

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	if(value == 1)
	{
		qDebug( "resetPV: %s=%d", var->getName(), value);
		pWave->ReSet();
	};
}

void doubleXEvent( void *obj, pvType /*type*/, int /*count*/, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	double dvalue = val->doubleVal[0];

	//qDebug( "X-PV Event: %s=%f", var->getName(), dvalue);
	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	QString strpvname = var->getName();

	pWave->SetXData(dvalue);
	pWave->Update(var->getName(), 0);
}

void doubleYEvent( void *obj, pvType /*type*/, int /*count*/, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	double dvalue = val->doubleVal[0];

	//qDebug( "Y-PV Event: %s=%f", var->getName(), dvalue);
	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	QString strpvname = var->getName();

	pWave->SetYData(dvalue);
	pWave->Update(var->getName(), 1);
}

void doubleValIPWaveEvent( void *obj, pvType /*type*/, int count, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	double *dvalue = val->doubleVal;

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;
	//short wavecnt = pWave->GetWavePVCount();

	QString strpvname = var->getName();
	if(strpvname.contains("IP_REF_YWAVE", Qt::CaseInsensitive) == true)
	{
		pWave->SetYWaveData(count, dvalue);
		pWave->Update();
		pWave->ReSet();
	}
	else if(strpvname.contains("IP_REF_XWAVE", Qt::CaseInsensitive) == true)
	{
		pWave->SetXWaveData(count, dvalue);
		pWave->Update();
	};
}

void doubleValWaveXEvent( void *obj, pvType /*type*/, int count, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	QString strpvname = var->getName();

	qDebug("WaveX:%s", var->getName());

	double *dvalue = val->doubleVal;

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;

	pWave->SetXWaveData(count, dvalue);
	pWave->Update();
}
void doubleValWaveYEvent( void *obj, pvType /*type*/, int count, pvValue *val, void *arg, pvStat /*stat*/) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	QString strpvname = var->getName();

	qDebug("WaveY:%s", var->getName());
	double *dvalue = val->doubleVal;

	CAWaveformPut *pWave  = (CAWaveformPut*)arg;

	pWave->SetYWaveData(count, dvalue);
	pWave->Update();
}


void WaveformProperty_Form::PenWidth(int width)
{
	//qDebug("PenWidth:%d", width);
	if(mpWave == NULL ) return;
	//mpWave->setPenWidth(width);
	mpWave->setProperty("penwidth",QVariant(width));
	mpWave->replot();
}
void WaveformProperty_Form::PenColor(const QString & color)
{
	mpWave->setProperty("pencolor",QVariant(color));
	mpWave->replot();
}

void WaveformProperty_Form::SymbolSize(int size)
{
	if(mpWave == NULL ) return;
	mpWave->setProperty("symbolsize",QVariant(size));
	mpWave->replot();
}

void WaveformProperty_Form::SymbolColor(const QString & color)
{
	mpWave->setProperty("symbolcolor",QVariant(color));
	mpWave->replot();
}

void WaveformProperty_Form::XMaxValue(double value)
{
	if(mpWave == NULL ) return;
    mpWave->setXRangeMax(value);
	mpWave->setAxisScale(QwtPlot::xBottom,XMin->value(),value);
	mpWave->replot();
};
void WaveformProperty_Form::DeleteForm()
{
	if(mpWave) mpWave->ResetChild();
}
void WaveformProperty_Form::XMinValue(double value)
{
	if(mpWave == NULL ) return;
    mpWave->setXRangeMin(value);
	mpWave->setAxisScale(QwtPlot::xBottom, value, XMax->value());
	mpWave->replot();
};
void WaveformProperty_Form::YMaxValue(double value)
{
	if(mpWave == NULL ) return;
    mpWave->setYRangeMax(value);
	mpWave->setAxisScale(QwtPlot::yLeft, YMin->value(), value);
	mpWave->replot();
};
void WaveformProperty_Form::YMinValue(double value)
{
	if(mpWave == NULL ) return;
    mpWave->setYRangeMin(value);
	mpWave->setAxisScale(QwtPlot::yLeft, value, YMax->value());
	mpWave->replot();
};
void WaveformProperty_Form::XNearby(int value)
{
	if(mpWave == NULL ) return;
	bool check = (value == 0 ? false:true);
    mpWave->setXNearby(check);
	mpWave->replot();
}
void WaveformProperty_Form::YNearby(int value)
{
	if(mpWave == NULL ) return;
	bool check = (value == 0 ? false:true);
	//qDebug("Value:%d, check:%d", value, check);
    mpWave->setYNearby(check);
	mpWave->replot();
}

void WaveformProperty_Form::ListItemChange(QListWidgetItem *selitem)
{
	for(int i = 0; i < XYValuelist->count();i++)
	{
		QListWidgetItem *pitem = XYValuelist->item(i);
		XYValuelist->closePersistentEditor(pitem);
	};
	XYValuelist->openPersistentEditor(selitem);
}

void WaveformProperty_Form::CurveItemChanged(int index)
{
	if(mpWave == NULL ) return;

	const QwtSymbol *symbol = mpWave->curve()->symbol();

	QwtSymbol *newSymbol = (QwtSymbol*)symbol;
	//newSymbol.setBrush(QBrush(Qt::blue));
	newSymbol->setBrush(QBrush(Qt::blue));

	const bool doReplot = mpWave->autoReplot();
	mpWave->setAutoReplot(false);

	// count draw
	mpWave->curve()->setSymbol(symbol);
	mpWave->curve()->draw(0, mpWave->curve()->dataSize());
	mpWave->curve()->setSymbol(newSymbol);
	mpWave->curve()->draw(index, index);
	mpWave->curve()->setSymbol(symbol);
	mpWave->setAutoReplot(doReplot);
};

void WaveformProperty_Form::reSetSymbol()
{
	if(mpWave == NULL ) return;

	const QwtSymbol *symbol = mpWave->curve()->symbol();
	QwtSymbol *newSymbol = symbol;

	const bool doReplot = mpWave->autoReplot();
	mpWave->setAutoReplot(false);

	// count draw
	mpWave->curve()->setSymbol(newSymbol);
	mpWave->curve()->draw(0, mpWave->curve()->dataSize());
	mpWave->setAutoReplot(doReplot);
}

void WaveformProperty_Form::ItemDelete()
{
	if(mpWave->getMode() == CAWaveformPut::Monitor) 
	{
		QString msg = QString("Configured Monitoring Mode!!");
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};
	int row = XYValuelist->currentRow();
	QListWidgetItem *pitem = XYValuelist->takeItem(row);
	//QListWidgetItem *pitem = XYValuelist->item(row);
	XYValuelist->removeItemWidget(pitem);
};

void WaveformProperty_Form::ItemInsert()
{
	if(mpWave->getMode() == CAWaveformPut::Monitor) 
	{
		QString msg = QString("Configured Monitoring Mode!!");
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};
	int row = -1;
	if((row = XYValuelist->currentRow()) < 0 ) return;
	QString insstr = QString("Index-%1:  ,  ").arg(row+1);
	XYValuelist->insertItem(row+1, insstr);
};

void WaveformProperty_Form::ItemApply()
{
	if(mpWave->getMode() == CAWaveformPut::Monitor) 
	{
		QString msg = QString("Configured Monitoring Mode!!");
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};
	reSetSymbol();
	// count draw
	QListWidgetItem *pItem = NULL;
	QwtArray<double> xData;
	QwtArray<double> yData;
	for(int i = 0; i < XYValuelist->count();i++)
	{
		pItem = XYValuelist->item(i);

		double xval = 0.0;;
		double yval = 0.0;;
		getStrToValue(pItem->text(), xval, yval);
		xData.push_back(xval);
		yData.push_back(yval);
	};
	mpWave->SetData(xData, yData);
	XYValueList();
}

int WaveformProperty_Form::getStrToValue(QString strvalue, double &xval, double &yval)
{
	char str[strvalue.toStdString().size()];
	strcpy (str, strvalue.toStdString().c_str());

	char *pch = NULL;

	pch = strtok(str, ":");

	int i = 0;
	while(pch!=NULL)
	{
		switch(i)
		{
			case 0:
			default:
				break;
			case 1:
				//qDebug("X-Val:%s(%d)", pch, i);
				xval = QString(pch).toDouble();
				break;
			case 2:
				//qDebug("Y-Val:%s(%d)", pch, i);
				yval = QString(pch).toDouble();
				break;
		}
		pch = strtok (NULL, ", ");
		i++;
	};


	return 0;
}

void WaveformProperty_Form::XYValueItemUpdate(const int index,QwtArray<double> &xData, QwtArray<double> &yData)
{
	if(!XYValuelist) return;

	QListWidgetItem *pitem = XYValuelist->item(index);
	QString xval = QString("Index-%1 : %2, ").arg(index).arg(xData[index],0,'f',4);
	QString yval = QString("%1").arg(yData[index],0,'f',4);
	QString strval = xval + yval;
	pitem->setText(strval);
	//qDebug("XYValueItemUpdate: %s", strval.toStdString().c_str());
	//XYValuelist->removeItemWidget(XYValuelist->currentItem());
}
void WaveformProperty_Form::XYValueList()
{
	if(mpWave == NULL) return;

    XMin->setValue(mpWave->getXRangeMin());
    XMax->setValue(mpWave->getXRangeMax());
    YMin->setValue(mpWave->getYRangeMin());
    YMax->setValue(mpWave->getYRangeMax());

    //QPushButton *valueInsert;

	QwtArray<double> xData;
	QwtArray<double> yData;

	mpWave->GetXYData(xData, yData);

	disconnect(XYValuelist, SIGNAL(currentRowChanged(int)), this, SLOT(CurveItemChanged(int)));
	XYValuelist->clear();

	QString xval;
	QString yval;
	//qDebug("XDATA COUNT: %d", xData.count());
	for(int i = 0; i < xData.count(); i++)
	{
		xval = QString("Index-%1 : %2, ").arg(i).arg(xData[i],0,'f',4);
		yval = QString("%1").arg(yData[i],0,'f',4);
		QString strval = xval + yval;
		XYValuelist->addItem(strval);
	};
	connect(XYValuelist, SIGNAL(currentRowChanged(int)), this, SLOT(CurveItemChanged(int)));
}

void WaveformProperty_Form::SetParent(CAWaveformPut *pwave)
{
	mpWave = (CAWaveformPut *)pwave;
}
class CAWaveformPutDraw:public QwtScaleDraw
{
public:
    CAWaveformPutDraw(const int times):mtimes(times)
	{};
private:
	const int mtimes;
};

class CAWaveformPut::PrivateData
{
public:
	//member method
    PrivateData():mpenindex(0), mcanvascolor(QColor(Qt::gray)), mmajlinecolor(QColor(Qt::black)), 
				  mminlinecolor(QColor(Qt::gray)),mselectedpoint(-1), mymin(0), mymax(5000),mxmin(-10),mxmax(50),mshotno(0),mxnearyby(true),mynearyby(true)
    {
	};
public:
	//member data
	QString mypvname;
	QString mxpvname;
	QString mcountpv;
	QwtPlotGrid *mgrid;
	int mpenindex;
	QString mtitle;
	QString mxtitle;
	QString mytitle;
	QColor mcanvascolor;
	QColor mmajlinecolor;
	QColor mminlinecolor;
	QPen mpen;
	QwtSymbol msym;
	int	mselectedpoint;
	double mymin, mymax;
	double mxmin, mxmax;
	int mshotno;
	QwtArray<double> mxData;
	QwtArray<double> myData;
    QwtPlotPicker *mpicker;
	QwtPlotCurve *mpCurve;
	int mpenwidth;
	QColor mpencolor;;
	int msymbolsize;
	QColor msymcolor;;
	QStringList mypvlist;
	QStringList mxpvlist;
	bool mxnearyby;
	bool mynearyby;
	QString mfilename;
	ControlMode mmode;
	pvSystem *msys;
	short mwavecount;
	QwtArray<double> localxWaveData;
	QwtArray<double> localyWaveData;
	QwtArray<double> localxData;
	QwtArray<double> localyData;
	QVector<QwtPlotCurve *> mvecCurve;
};

CAWaveformPut::CAWaveformPut(QWidget *parent):QwtPlot(parent),mUpdate(1), pWavewindow(NULL)
{
    initWaveformPlot();
}

CAWaveformPut::~CAWaveformPut()
{
    delete d_data;
}

QSize CAWaveformPut::sizeHint() const
{
	return minimumSizeHint();
}

void CAWaveformPut::initWaveformPlot()
{
    //using namespace Qt;
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("CAWaveformPut"));
	//setCursor(Qt::PointingHandCursor);

	d_data->mgrid = new QwtPlotGrid;
	d_data->mgrid->enableXMin(true);
	d_data->mgrid->enableYMin(true);
	d_data->mgrid->setMajPen(QPen(Qt::black, 0, Qt::DashLine));
	d_data->mgrid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	d_data->mgrid->attach(this);

	new WavePicker(this);
	//installEventFilter(this);

	d_data->mpCurve = new QwtPlotCurve();
	d_data->mpCurve->setStyle(QwtPlotCurve::Lines);
	d_data->mpCurve->attach(this);

	d_data->mpen.setWidth(1);
	d_data->mpen.setColor(QColor("blue"));
	d_data->mpCurve->setPen(d_data->mpen);

	d_data->msym.setStyle(QwtSymbol::Rect);
	d_data->msym.setPen(QColor(Qt::black));
	//d_data->msym.setColor(QColor(Qt::black));
	d_data->msym.setSize(8);
	d_data->msym.setBrush(QColor(Qt::blue));
	d_data->mpCurve->setSymbol(d_data->msym);
	//d_data->mpCurve->setSymbol(QwtSymbol(QwtSymbol::Ellipse, Qt::black, QColor(Qt::black), QSize(8, 8)));

    d_data->mpicker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
			QwtPicker::PointSelection, QwtPlotPicker::CrossRubberBand, QwtPicker::ActiveOnly, canvas());
	d_data->mpicker->setRubberBandPen(QColor(Qt::green));

	//setYRange(0, 100);
	//setXRange(-20, 100);
	setYRange(d_data->mymin, d_data->mymax);
	setXRange(d_data->mxmin, d_data->mxmax);

	d_data->msys = newPvSystem("ca");
};

void CAWaveformPut::setXRange(const double min, const double max)
{
	setAxisScale(QwtPlot::xBottom, min, max);
}

void CAWaveformPut::MonitorOn()
{
	if(getMode() != CAWaveformPut::Monitor) return;

	connect(this, SIGNAL(updatesignal(const int)), this, SLOT(UpdateSlot(const int)));

	string xwavepv= getXPvname().toStdString();
	string ywavepv= getYPvname().toStdString();
	string countpv= getCountpv().toStdString();

	vector<string> pvlist;
	pvlist.push_back(xwavepv);
	pvlist.push_back(ywavepv);

#if 1
	pvVariable *var = d_data->msys->newVariable(countpv.c_str());
	int sts = var -> monitorOn(pvTypeSHORT, 1, nordEvent, this );
	if ( sts ) { 
		qDebug( "%s.NORD->monitorOn() failure", countpv.c_str() );
	};
	d_data->msys->pend(1, TRUE );

	var = d_data->msys->newVariable(xwavepv.c_str());
	sts = var -> monitorOn(pvTypeDOUBLE, GetWavePVCount(), doubleValWaveXEvent, this );

	if ( sts ) { 
		qDebug( "%s->monitorOn() failure", xwavepv.c_str() );
	};

	var = d_data->msys->newVariable(ywavepv.c_str());
	sts = var -> monitorOn(pvTypeDOUBLE, GetWavePVCount(), doubleValWaveYEvent, this );

	if ( sts ) { 
		qDebug( "%s->monitorOn() failure", ywavepv.c_str() );
	};

#endif
	d_data->msys->pend(1, FALSE );
};

void CAWaveformPut::MonitorOnIP()
{
	if(getMode() != CAWaveformPut::Monitor) return;

	connect(this, SIGNAL(updatesignal(const int)), this, SLOT(UpdateSlot(const int)));
	connect(this, SIGNAL(updatesignal(const QString, const int)), this, SLOT(UpdateSlot(const QString, const int)));

	string xwavepv= getXPvname().toStdString();
	string ywavepv= getYPvname().toStdString();
	string countpv= getCountpv().toStdString();

	vector<string> pvlist;
	pvlist.push_back(xwavepv);
	pvlist.push_back(ywavepv);

	pvVariable *var = d_data->msys->newVariable(countpv.c_str());
	int sts = var -> monitorOn(pvTypeSHORT, 1, countPvEvent, this );
	if ( sts ) { 
		qDebug( "%s->monitorOn() failure", countpv.c_str() );
	};

	var = d_data->msys->newVariable("CCS_SHOTSEQ_MDSTREE_CREATE");
	sts = var -> monitorOn(pvTypeSHORT, 1,treeResetEvent , this );
	if ( sts ) { 
		qDebug( "%s->monitorOn() failure", "CCS_SHOTSEQ_MDSTREE_CREATE" );
	};

	d_data->msys->pend(1, FALSE );

#if 1
	for (size_t i = 0; i < pvlist.size(); i++)
	{   
		pvVariable *var = d_data->msys->newVariable(pvlist.at(i).c_str());
		int sts = 0;
		sts = var -> monitorOn(pvTypeDOUBLE, 64, doubleValIPWaveEvent, this );
		if ( sts ) { 
			qDebug( "%s->monitorOn() failure", pvlist.at(i).c_str() );
		};
	};  
#endif
	d_data->msys->pend(1, FALSE );

#if 0
	d_data->mpCurve = new QwtPlotCurve();
	d_data->mpCurve->setStyle(QwtPlotCurve::Lines);
	d_data->mpCurve->attach(this);
	d_data->mpen.setWidth(1);
	d_data->mpen.setColor(QColor("blue"));
	d_data->mpCurve->setPen(d_data->mpen);
	d_data->msym.setStyle(QwtSymbol::Ellipse);
	d_data->msym.setPen(QColor(Qt::black));
	d_data->msym.setSize(8);
	d_data->mpCurve->setSymbol(d_data->msym);
#endif

	QStringList xpvlist = d_data->mxpvlist;
	QStringList ypvlist = d_data->mypvlist;

	int xpvcount = xpvlist.count(); 
	int ypvcount = ypvlist.count(); 

	if(xpvcount != ypvcount)
	{
		//Message
		return;
	};

	if(xpvlist.isEmpty() == false)
	{
		for(int i = 0; i<xpvcount;i++)
		{
			QString pvitem = xpvlist.at(i);
			var = d_data->msys->newVariable(pvitem.toStdString().c_str());
			sts = var -> monitorOn(pvTypeDOUBLE, 1, doubleXEvent, this );
			if ( sts ) { 
				qDebug( "%s->monitorOn() failure", pvitem.toStdString().c_str() );
			};

			QwtPlotCurve *pCurve = new QwtPlotCurve();
			pCurve->setStyle(QwtPlotCurve::Lines);
			pCurve->attach(this);
			QPen pen;
			pen.setWidth(3);
			pen.setColor(QColor("red"));
			QwtSymbol sym;
			sym.setStyle(QwtSymbol::Ellipse);
			sym.setPen(QColor(Qt::red));
			sym.setBrush(QColor(Qt::red));
			sym.setSize(6);
			pCurve->setPen(pen);
			pCurve->setSymbol(sym);
			d_data->mvecCurve.push_back(pCurve);
		};
	};
	if(ypvlist.isEmpty() == false)
	{
		for(int i = 0; i<ypvcount;i++)
		{
			QString pvitem = ypvlist.at(i);
			var = d_data->msys->newVariable(pvitem.toStdString().c_str());
			sts = var -> monitorOn(pvTypeDOUBLE, 1, doubleYEvent, this );
			if ( sts ) { 
				qDebug( "%s->monitorOn() failure", pvitem.toStdString().c_str() );
			};
		};
	};

	d_data->msys->pend(1, TRUE );
	d_data->localxData.clear();
	d_data->localyData.clear();

	//for Test
	//startTimer(100);
}

void CAWaveformPut::setYRange(const double min, const double max)
{
	setAxisScale(QwtPlot::yLeft, min, max);
}

QSize CAWaveformPut::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAWaveformPut::setYPvname(const QString &pvname)
{
    d_data->mypvname = pvname;
}

const QString &CAWaveformPut::getYPvname() const
{
	return d_data->mypvname;
}

void CAWaveformPut::setXPvname(const QString &pvname)
{
    d_data->mxpvname = pvname;
}

const QString &CAWaveformPut::getCountpv() const
{
	return d_data->mcountpv;
}
void CAWaveformPut::setCountpv(const QString &pvname)
{
    d_data->mcountpv = pvname;
}
const QStringList CAWaveformPut::getXPVList() const
{
	return d_data->mxpvlist;
}
void  CAWaveformPut::setXPVList(const QStringList pvlist)
{
	d_data->mxpvlist = pvlist;
}
const QStringList CAWaveformPut::getYPVList() const
{
	return d_data->mypvlist;
}
void CAWaveformPut::setYPVList(const QStringList pvlist)
{
	d_data->mypvlist = pvlist;
}

const QString &CAWaveformPut::getXPvname() const
{
	return d_data->mxpvname;
}

const double CAWaveformPut::getYRangeMax() const
{
	return d_data->mymax;
};

void CAWaveformPut::setYRangeMax(const double max)
{
	d_data->mymax = max;
	setAxisScale(QwtPlot::yLeft, d_data->mymin, d_data->mymax);
	replot();
}

const double CAWaveformPut::getYRangeMin() const
{
	return d_data->mymin;
};

void CAWaveformPut::setYRangeMin(const double min)
{
	d_data->mymin = min;
	setAxisScale(QwtPlot::yLeft, d_data->mymin, d_data->mymax);
	replot();
}

const double CAWaveformPut::getXRangeMax() const
{
	return d_data->mxmax;
};

void CAWaveformPut::setXRangeMax(const double max)
{
	d_data->mxmax = max;
	setAxisScale(QwtPlot::xBottom, d_data->mxmin, d_data->mxmax);
	replot();
}

const double CAWaveformPut::getXRangeMin() const
{
	return d_data->mxmin;
};

void CAWaveformPut::setXRangeMin(const double min)
{
	d_data->mxmin = min;
	setAxisScale(QwtPlot::xBottom, d_data->mxmin, d_data->mxmax);
	replot();
}

void CAWaveformPut::penIndex(const int index)
{
	d_data->mpenindex = index;
}
void CAWaveformPut::penWidth(const int width)
{
#if 1
	d_data->mpen.setWidth(width);
	d_data->mpCurve->setPen(d_data->mpen);
#endif
	replot();
}
void CAWaveformPut::penStyle(const QString strstyle)
{
#if 1
	if (strstyle.toLower().compare("lines") == 0 )
		d_data->mpCurve->setStyle(QwtPlotCurve::Lines);
	else if (strstyle.toLower().compare("dots") == 0 )
		d_data->mpCurve->setStyle(QwtPlotCurve::Dots);
	else if (strstyle.toLower().compare("sticks") == 0 )
		d_data->mpCurve->setStyle(QwtPlotCurve::Sticks);
	else
		d_data->mpCurve->setStyle(QwtPlotCurve::Steps);
#endif

	replot();
}

void CAWaveformPut::penColor(const QString /*color*/)
{
	replot();
}

void CAWaveformPut::setPlottitle(const QString &title)
{
    d_data->mtitle = title;
	setTitle(title.toStdString().c_str());
	plotupdate();
}
const QString &CAWaveformPut::getPlottitle() const
{
	return d_data->mtitle;
}

void CAWaveformPut::setXtitle(const QString &title)
{
    setAxisTitle(QwtPlot::xBottom, title.toStdString().c_str());
	d_data->mxtitle = title;
	plotupdate();
}
const QString &CAWaveformPut::getXtitle() const
{
	return d_data->mxtitle;
}

void CAWaveformPut::setYtitle(const QString &title)
{
    setAxisTitle(QwtPlot::yLeft, title.toStdString().c_str());
	d_data->mytitle = title;
	plotupdate();
}
const QString &CAWaveformPut::getYtitle() const
{
	return d_data->mytitle;
}

void CAWaveformPut::setCanvascolor(const QColor &color)
{
	setCanvasBackground(color);
	d_data->mcanvascolor = color;
	plotupdate();
}
const QColor &CAWaveformPut::getCanvascolor()
{

	return d_data->mcanvascolor;
}

void CAWaveformPut::setMajorlinecolor(const QColor &color)
{
	d_data->mmajlinecolor = color;
	d_data->mgrid->setMajPen(QPen(color, 0, Qt::DashLine));
	plotupdate();
}
const QColor &CAWaveformPut::getMajorlinecolor()
{

	return d_data->mmajlinecolor;
}

void CAWaveformPut::setMinorlinecolor(const QColor &color)
{
	d_data->mgrid->setMinPen(QPen(color , 0, Qt::DotLine));
	d_data->mminlinecolor = color;
	plotupdate();
}
const QColor &CAWaveformPut::getMinorlinecolor()
{
	return d_data->mminlinecolor;
}

void CAWaveformPut::plotupdate()
{
	replot();
	update();
}

#if 0
bool CAWaveformPut::eventFilter(QObject *o, QEvent *e)
{
	//Area of QwtPlot is differnt with area of canvas of the plot.
		//-> move wavePicker.cpp eventFilter

	if(e->type() == QEvent::MouseButtonDblClick)
	{
		QMouseEvent *pEvent = (QMouseEvent*)e;
		const QwtPolygon &pa = d_data->mpicker->selection();
		qDebug("PA.Count: %d", pa.count());
		if(pa.count() == 0) return false;

		if(d_data->mpicker->selectionFlags() & QwtPicker::PointSelection)
		{
			//double xVal = invTransform(xBottom, pa[0].x());
			//double yVal = invTransform(yLeft,   pa[0].y());
			int xVal = static_cast<int> (invTransform(xBottom, pa[0].x()));
			int yVal = static_cast<int> (invTransform(yLeft,   pa[0].y()));
			//qDebug("X-Value:%f, Y-Value:%f", xVal, yVal);
			//qDebug("PA.X[%d] = E.X[%d], PA.Y[%d] = E.Y[%d]", pa[0].x(), pa[0].y(), pEvent->pos().x(), pEvent->pos().y());
			d_data->mxData.push_back(xVal);
			d_data->myData.push_back(yVal);
		};
		d_data->mpicker->resetSelection();
		d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
		printdata();
	}
	replot();
	return QObject::eventFilter(o, e);
}
#endif

void CAWaveformPut::MoveValue(const double xval, const double yval)
{
	if(getMode() == CAWaveformPut::Monitor) return;
	if(d_data->mselectedpoint < 0) return;
	QwtArray<double> xData(d_data->mpCurve->dataSize());
	QwtArray<double> yData(d_data->mpCurve->dataSize());
	for ( int i = 0; i < d_data->mpCurve->dataSize(); i++ )
	{
		if ( i == d_data->mselectedpoint )
		{
			xData[i] = xval;
			yData[i] = yval;
		}
		else
		{
			xData[i] = d_data->mpCurve->x(i);
			yData[i] = d_data->mpCurve->y(i);
		}
	};
	SetData(xData, yData);

	const QwtSymbol *symbol = d_data->mpCurve->symbol();
	QwtSymbol *newSymbol = symbol;
	//newSymbol.setBrush(QBrush(Qt::blue));
	newSymbol->setBrush(QBrush(Qt::blue));

	const bool doReplot = autoReplot();
	setAutoReplot(false);

	d_data->mpCurve->setSymbol(newSymbol);
	d_data->mpCurve->draw(d_data->mselectedpoint, d_data->mselectedpoint);
	d_data->mpCurve->setSymbol(symbol);
	setAutoReplot(doReplot);

	if(pWavewindow)
		pWavewindow->XYValueItemUpdate(d_data->mselectedpoint, xData, yData);
};
void CAWaveformPut::MovePoint(const QPoint &pos)
{
	if(getMode() == CAWaveformPut::Monitor) return;
	if(d_data->mselectedpoint < 0) return;
	QwtArray<double> xData(d_data->mpCurve->dataSize());
	QwtArray<double> yData(d_data->mpCurve->dataSize());
	for ( int i = 0; i < d_data->mpCurve->dataSize(); i++ )
	{
		if ( i == d_data->mselectedpoint )
		{
			//xData[i] = TruncVal(invTransform(d_data->mpCurve->xAxis(), pos.x()), 3);
			//yData[i] = TruncVal(invTransform(d_data->mpCurve->yAxis(), pos.y()), 3);
			//xData[i] = trunc(invTransform(d_data->mpCurve->xAxis(), pos.x()));
			//yData[i] = trunc(invTransform(d_data->mpCurve->yAxis(), pos.y()));

			double xVal = static_cast<double> (invTransform(d_data->mpCurve->xAxis(), pos.x()));
			double yVal = static_cast<double> (invTransform(d_data->mpCurve->yAxis(), pos.y()));

#if 1
			//int yfactor = getYFactor();
			int xfactor = getXFactor();
			//qDebug("factor=%d, yVal=%f, yVal/factor=%f, nearbyint(yVal/factor)=%f, nearbyint(yVal/factor)*factor=%f", factor, yVal, yVal/100, nearbyint(yVal/100),nearbyint(yVal/100)*100);
#endif

			if(d_data->mxnearyby)
				xData[i] = nearbyint(xVal/xfactor)*xfactor;
			else
				xData[i] = xVal;

			if(d_data->mynearyby)
				yData[i] = nearbyint(yVal/xfactor)*xfactor;
			else
				yData[i] = yVal;
		}
		else
		{
			xData[i] = d_data->mpCurve->x(i);
			yData[i] = d_data->mpCurve->y(i);
		}
	};
	SetData(xData, yData);

	const QwtSymbol *symbol = d_data->mpCurve->symbol();
	QwtSymbol *newSymbol = symbol;
	//newSymbol.setBrush(QBrush(Qt::blue));
	newSymbol->setBrush(QBrush(Qt::blue));

	const bool doReplot = autoReplot();
	setAutoReplot(false);

	d_data->mpCurve->setSymbol(newSymbol);
	d_data->mpCurve->draw(d_data->mselectedpoint, d_data->mselectedpoint);
	d_data->mpCurve->setSymbol(symbol);
	setAutoReplot(doReplot);

	if(pWavewindow)
		pWavewindow->XYValueItemUpdate(d_data->mselectedpoint, xData, yData);
};

double CAWaveformPut::TruncVal(double orgvalue, int n)
{
	double point = pow(10, n);
	return (floor(orgvalue * point)/point);
}
int CAWaveformPut::getXFactor()
{
    int xVal = (int)(getXRangeMax() - getXRangeMin());

	int factor=1;
	if(xVal >= 1 && xVal < 99)
		factor = 1;
	else if(xVal >= 100 && xVal < 999)
		factor = 10;
	else if(xVal >= 1000 && xVal < 9999)
		factor = 100;
	else if(xVal >= 10000 && xVal < 99999)
		factor = 1000;

	return factor;

	return 0;
}
int CAWaveformPut::getYFactor()
{
    int yVal = (int) (getYRangeMax() - getYRangeMin());
	
	int factor=1;
	if(yVal >= 1 && yVal < 99)
		factor = 1;
	else if(yVal >= 100 && yVal < 999)
		factor = 10;
	else if(yVal >= 1000 && yVal < 9999)
		factor = 100;
	else if(yVal >= 10000 && yVal < 99999)
		factor = 1000;
	else if(yVal >= 100000 && yVal < 999999)
		factor = 10000;

	return factor;
}

void CAWaveformPut::InsertPoint(const QPoint &pos)
{
	if(getMode() == CAWaveformPut::Monitor) return;
	if(d_data->mpicker->selectionFlags() & QwtPicker::PointSelection)
	{
		//double xVal = TruncVal(static_cast<double> (invTransform(xBottom, pos.x())), 3);
		//double yVal = TruncVal(static_cast<double> (invTransform(yLeft,   pos.y())), 3);
		//double xVal = trunc(static_cast<double> (invTransform(xBottom, pos.x())));
		//double yVal = trunc(static_cast<double> (invTransform(yLeft,   pos.y())/100))*100;
		//double xVal = nearbyint(static_cast<double> (invTransform(xBottom, pos.x())));


		double xVal = static_cast<double> (invTransform(xBottom, pos.x()));
		double yVal = static_cast<double> (invTransform(yLeft,   pos.y()));

		int yfactor = getYFactor();
		int xfactor = getXFactor();
		
		//qDebug("xfactor=%d,yfactor=%d, yVal=%f, yVal/factor=%f, nearbyint(yVal/factor)=%f, nearbyint(yVal/factor)*factor=%f", xfactor,yfactor, yVal, yVal/100, nearbyint(yVal/100),nearbyint(yVal/100)*100);
		//if(yfactor>1)
		if(d_data->mynearyby)
			yVal = nearbyint(yVal/yfactor)*yfactor;

		//if(xfactor>1)
		if(d_data->mxnearyby)
			xVal = nearbyint(xVal/xfactor)*xfactor;

		//ceil(3), floor(3), lrint(3), nearbyint(3), rint(3), round(3)
		//qDebug("xVal:%f(%f), yVal:%f(%f)", TruncVal(xVal, 3), xVal, TruncVal(yVal, 3), yVal); 
		d_data->mxData.push_back(xVal);
		d_data->myData.push_back(yVal);
	};
	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	printdata();
	replot();
	if(pWavewindow)
		pWavewindow->XYValueList();
}

QwtPlotCurve *CAWaveformPut::curve()
{
	QwtPlotCurve *pCurve = d_data->mpCurve;

	return pCurve;
};

void CAWaveformPut::SetData(QwtArray<double> &xData, QwtArray<double> &yData )
{
	d_data->mpCurve->setSamples(xData, yData);
	replot();
	d_data->mxData = xData;
	d_data->myData = yData;
#if 0
	//for ( int i = 0; i < d_data->mpCurve->dataSize(); i++ )
	for ( int i = 0; i < xData.count(); i++ )
	{
		d_data->mxData[i] = xData[i];
		d_data->myData[i] = yData[i];
	};
#endif
}

void CAWaveformPut::printdata()
{
	for ( int i = 0; i < d_data->mpCurve->dataSize(); i++ )
	{
		//qDebug("X[%d]=%f, Y[%d]=%f", i,d_data->mxData[i], i, d_data->myData[i]);
	};
}

void CAWaveformPut::RemovePoint(const int index)
{
	if(index == -1 ) return;
	d_data->mxData.remove(index);
	d_data->myData.remove(index);

	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	replot();
	if(pWavewindow)
		pWavewindow->XYValueList();
}
const int CAWaveformPut::getPenWidth()
{
	return d_data->mpenwidth;
};
void CAWaveformPut::setPenWidth (const int penwidth)
{
	d_data->mpenwidth = penwidth;
	d_data->mpen.setWidth(penwidth);
	d_data->mpCurve->setPen(d_data->mpen);
};

const QColor& CAWaveformPut::getPenColor()
{
	return d_data->mpencolor;;
}
void CAWaveformPut::setPenColor(const QColor &color)
{
	d_data->mpencolor = color;
	d_data->mpen.setColor(color);
	d_data->mpCurve->setPen(d_data->mpen);
}

const int CAWaveformPut::getSymbolSize()
{
	return d_data->msymbolsize;
};
void CAWaveformPut::setSymbolSize (const int symbolsize)
{
	d_data->msymbolsize = symbolsize;
	d_data->msym.setSize(symbolsize);
	d_data->mpCurve->setSymbol(d_data->msym);
}

const QColor& CAWaveformPut::getSymbolColor()
{
	return d_data->msymcolor;;
}
void CAWaveformPut::setSymbolColor(const QColor &color)
{
	d_data->msymcolor = color;
	d_data->msym.setBrush(color);
	d_data->mpCurve->setSymbol(d_data->msym);
	//d_data->mpCurve->setSymbol(QwtSymbol(QwtSymbol::Ellipse, Qt::black, QColor(Qt::black), QSize(8, 8)));
}


void CAWaveformPut::caXYArrayPut()
{
	//caArrayPut(XAXIS);
	//caArrayPut(YAXIS);

	caListPut();
}

void CAWaveformPut::caXYArrayGet()
{
	int xcount = 0;
	caArrayGet(XAXIS, xcount);
	caArrayGet(YAXIS, xcount);
	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	replot();
}
int CAWaveformPut::checkXList(int & index)
{
	int sts = 0;
	if(d_data->mxpvlist.count() == 0 || 0 == d_data->mxData.count())
	{
		sts = -1;
		return sts;
	}

	double value=-1., prevalue = -1.;
	for(int i = 0; i<d_data->mxData.count();i++)
	{
		value = d_data->mxData[i];
		//qDebug("Value:%f, Preval:%f", value, prevalue);
		if(value < prevalue)
		{
			index = i;
			sts = -1;
			break;
		};
		prevalue = value;
	};

	return sts;
}
#if 0
void CAWaveformPut::caListPut()
{
	QStringList xpvlist = d_data->mxpvlist;
	if( xpvlist.count() < d_data->mxData.count() ) return;

	int chkIndex = 0;
	if(checkXList(chkIndex) < 0)
	{
		qDebug("Error Invalid Time Set(Index:%d)", chkIndex);
		QString msg = QString("Error Invalid Time Set(Index:%1)").arg(chkIndex);
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};

	if(xpvlist.isEmpty() == false)
	{
		//for(int i = 0; i<xpvlist.count();i++)
		for(int i = 0; i<d_data->mxData.count();i++)
		{
			QString pvitem = xpvlist.at(i);
			double value = d_data->mxData[i];
			caPut(pvitem, value);
		};

	};

	QStringList ypvlist = d_data->mypvlist;
	if( ypvlist.count() < d_data->myData.count() ) return;
	if(ypvlist.isEmpty() == false)
	{
		//for(int i = 0; i<ypvlist.count();i++)
		for(int i = 0; i<d_data->myData.count();i++)
		{
			QString pvitem = ypvlist.at(i);
			double value = d_data->myData[i];
			caPut(pvitem, value);
		};
	};
};
#else
void CAWaveformPut::caListPut()
{
	QStringList xpvlist = d_data->mxpvlist;
	QStringList ypvlist = d_data->mypvlist;
	if( xpvlist.count() < d_data->mxData.count() ) return;
	if( ypvlist.count() < d_data->myData.count() ) return;

	int datacount = d_data->mxData.count();
	chid channelID[2][datacount];
	int status = 0;

	int chkIndex = 0;
	if(checkXList(chkIndex) < 0)
	{
		qDebug("Error Invalid Time Set(Index:%d)", chkIndex);
		QString msg = QString("Error Invalid Time Set(Index:%1)").arg(chkIndex);
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};

	if(xpvlist.isEmpty() == false)
	{
		for(int i = 0; i<datacount;i++)
		{
			QString pvitem = xpvlist.at(i);
			double value = d_data->mxData[i];
			status = ca_search_and_connect(pvitem.toStdString().c_str(), &channelID[0][i], NULL, NULL);
			status = ca_pend_io ( 1.0 );
			SEVCHK (status, 0);
			ca_put(DBR_DOUBLE, channelID[0][i], &value);
		};
	};
	if(ypvlist.isEmpty() == false)
	{
		for(int i = 0; i<datacount;i++)
		{
			QString pvitem = ypvlist.at(i);
			double value = d_data->myData[i];
			status = ca_search_and_connect(pvitem.toStdString().c_str(), &channelID[1][i], NULL, NULL);
			status = ca_pend_io ( 1.0 );
			SEVCHK (status, 0);
			ca_put(DBR_DOUBLE, channelID[1][i], &value);
		};
	};
	ca_flush_io ();
	epicsThreadSleep ( 0.1 );
	ca_poll ();

	if (status == ECA_TIMEOUT)
	{
		qDebug("In CAWaveformPut - caPut timed out: Data was not written.\n");
		return;
	};

	//ca_clear_channel(channelID);
	for(int i = 0; i < datacount;i++)
	{
		ca_clear_channel(channelID[0][i]);
		ca_clear_channel(channelID[1][i]);
	}
};
#endif

int CAWaveformPut::caPut(const QString pvname, const double value)
{
	chid channelID;
	int status = 0;
	unsigned count = 0u;

	if(pvname.isEmpty() == true) return -1;
	memset(&channelID, 0, sizeof(chid));

	status = ca_search_and_connect(pvname.toStdString().c_str(), &channelID, NULL, NULL);
	SEVCHK ( status, 0 );
	status = ca_add_event ( DBR_GR_DOUBLE, channelID, nUpdatesTester, &count, 0 );
	SEVCHK ( status, 0 );
	status = ca_pend_io ( 1.0 );
	SEVCHK (status, 0);
	epicsThreadSleep ( 0.1 );
	ca_poll ();

	if(status != ECA_NORMAL)
	{
		ca_clear_channel(channelID);
		qDebug("Not Found Pv\n");
		return status;
	};

	ca_put(DBR_DOUBLE, channelID, &value);
	ca_flush_io ();
	epicsThreadSleep ( 0.1 );
	ca_poll ();

	if (status == ECA_TIMEOUT)
	{
		qDebug("In CAWaveformPut - caPut timed out: Data was not written.\n");
		return status;
	};

	ca_clear_channel(channelID);
	return status;
}

double CAWaveformPut::caGet(const QString pvname)
{
	chid channelID;
	int status = ca_search_and_connect(pvname.toStdString().c_str(), &channelID, NULL, NULL);
	status = ca_pend_io(2.0);

	if(status != ECA_NORMAL)
	{
		ca_clear_channel(channelID);
		qDebug("Not Found Pv:%s",pvname.toStdString().c_str());
		return 0.0;
	};

	struct dbr_time_double dbrvalue;
	ca_get(DBR_TIME_DOUBLE, channelID, (void *)&dbrvalue);
	status = ca_pend_io(2.0);

	if(status != ECA_NORMAL)
	{
		qDebug("%s access\n",ca_name(channelID));
		return 0.0;
	};
	//qDebug("%f\n",dbrvalue.value);

	return (dbrvalue.value);
}
int CAWaveformPut::caArrayGet (const int type, int &xcount)
{
	int status = 0;

	//double count = caGet(getCountpv()); Not Used
	if(type == XAXIS)
	{
		xcount = 0;
		d_data->mxData.clear();
		QStringList xpvlist = d_data->mxpvlist;
		if(xpvlist.isEmpty() == false)
		{
			//for(int i = 0; i<(int)count;i++)
			for(int i = 0; i<xpvlist.count();i++)
			{
				QString pvitem = xpvlist.at(i);
				double value = caGet(pvitem);
				if(i != 0 && value == 0.0) break;
				d_data->mxData.push_back(value);
				//qDebug("X-Val:%f", value);
				xcount++;
			};
		};
	}
	else
	{
		d_data->myData.clear();
		QStringList ypvlist = d_data->mypvlist;
		if(ypvlist.isEmpty() == false)
		{
			//for(int i = 0; i<(int)count;i++)
			//for(int i = 0; i<ypvlist.count();i++)
			for(int i = 0; i<xcount;i++)
			{
				QString pvitem = ypvlist.at(i);
				double value = caGet(pvitem);
				//qDebug("Y-Val:%f", value);
				d_data->myData.push_back(value);
			};
		};

	};

	return status;
}

int CAWaveformPut::caArrayPut (const int type, int /*count*/)
{
	//chid channelID;
	int status = 0;
	QString pvname;  
	if(type == XAXIS)
		pvname = getXPvname();
	else
		pvname = getYPvname();

	if(pvname.isEmpty() == true) return -1;

#if 0
	status = ca_search(pvname.toStdString().c_str(), &channelID);
	status = ca_pend_io(1.0);

	if(status != ECA_NORMAL)
	{
		ca_clear_channel(channelID);
		qDebug("Not Found Pv\n");
		return status;
	};
#endif

	//int datacount = mxData.count();
	//status = ca_array_put (DBR_DOUBLE, datacount, channelID, pdbuf);

	if(type == XAXIS)
	{
#if 0
		d_data->mxData.push_front(d_data->mxData.count());
		status = ca_array_put (DBR_DOUBLE, d_data->mxData.count(), channelID, d_data->mxData.data());
		//d_data->mxData.pop_front();
		d_data->mxData.remove(0);
#else
		QString valuelist = QString("caput -a %1 %2 %3 ").arg(pvname).arg(d_data->mxData.count()).arg(d_data->mxData.count());
		for(int i = 0; i < d_data->mxData.count(); i++)
		{
			valuelist += QString("%1 ").arg(d_data->mxData[i],0,'f',4);
		}
		valuelist += QString("&");
		//qDebug("X-Valuelist: %s", valuelist.toStdString().c_str());
		system(valuelist.toStdString().c_str());
#endif
	}
	else
	{
#if 0
		d_data->myData.push_front(d_data->myData.count());
		status = ca_array_put (DBR_DOUBLE, d_data->myData.count(), channelID, d_data->myData.data());
		//d_data->myData.pop_front();
		d_data->myData.remove(0);
#else
		QString valuelist = QString("caput -a %1 %2 %3 ").arg(pvname).arg(d_data->myData.count()).arg(d_data->myData.count());
		for(int i = 0; i < d_data->myData.count(); i++)
		{
			valuelist += QString("%1 ").arg(d_data->myData[i],0,'f',4);
		}
		valuelist += QString("&");
		//qDebug("Y-Valuelist: %s", valuelist.toStdString().c_str());
		system(valuelist.toStdString().c_str());
#endif
	};

#if 0
	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT)
	{
		qDebug("In CAWaveformPut - caArrayPut timed out: Data was not written.\n");
		return status;
	};
	ca_clear_channel(channelID);
#endif
	return status;
};

double CAWaveformPut::getDistance(const QPoint &pos)
{
	QwtPlotCurve *curve = NULL;
	double dist = 10e10;
	int index = -1;

	const QwtPlotItemList& itmList = itemList();
	for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it )
	{
		if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve )
		{
			QwtPlotCurve *c = (QwtPlotCurve*)(*it);
			double d;
			int idx = c->closestPoint(pos, &d);
			//qDebug ("index: %d, distance:%f", idx, d);
			if ( d < dist )
			{
				curve = c;
				index = idx;
				dist = d;
			};
		};
	};
	return dist;
}

void CAWaveformPut::ShowProperty(const QPoint &pos)
{
	//if(getMode() == CAWaveformPut::Monitor) return;
	if(getDistance(pos) < 10) return;

	if(!pWavewindow)
	{
		pWavewindow =  new WaveForm::WaveformProperty();
		pWavewindow->SetParent(this);
		pWavewindow->XYValueList();
	};
}
void CAWaveformPut::reSetSymbol()
{
	const QwtSymbol *symbol = d_data->mpCurve->symbol();
	QwtSymbol *newSymbol = symbol;

	const bool doReplot = autoReplot();
	setAutoReplot(false);

	// count draw
	d_data->mpCurve->setSymbol(newSymbol);
	d_data->mpCurve->draw(0, d_data->mpCurve->dataSize());
	setAutoReplot(doReplot);
};
void CAWaveformPut::SetSelectedPoint(const int index) 
{
	d_data->mselectedpoint = index;
	if(index < 0) return;
	if(pWavewindow)
	{
		pWavewindow->XYValuelist->setCurrentRow(index);
	}
	else
	{
		reSetSymbol();
		const QwtSymbol *symbol = d_data->mpCurve->symbol();

		QwtSymbol *newSymbol = symbol;
		//newSymbol.setBrush(QBrush(Qt::blue));
		newSymbol->setBrush(QBrush(Qt::blue));

		const bool doReplot = autoReplot();
		setAutoReplot(false);

		d_data->mpCurve->setSymbol(newSymbol);
		d_data->mpCurve->draw(index, index);
		d_data->mpCurve->setSymbol(symbol);
		setAutoReplot(doReplot);
	};
};

void CAWaveformPut::GetXYData(QwtArray<double>& xdata, QwtArray<double>& ydata)
{
	xdata = d_data->mxData;
	ydata = d_data->myData;
}

void CAWaveformPut::shotNumber(const int shot)
{
	d_data->mshotno = shot;
}

int CAWaveformPut::DataCheck()
{
	if(!d_data->mpCurve) return (-1);
	if(d_data->mpCurve->dataSize() <= 0) return (-1);
	return 0;
}
double CAWaveformPut::getSelectedPointXValue()
{
	return(d_data->mpCurve->x(d_data->mselectedpoint));
}
double CAWaveformPut::getSelectedPointYValue()
{
	return(d_data->mpCurve->y(d_data->mselectedpoint));
}
const int CAWaveformPut::TransformX(const int dx)
{
	//qDebug("X-Pos:%f",d_data->mpCurve->x(d_data->mselectedpoint));
	return(transform(d_data->mpCurve->xAxis(),d_data->mpCurve->x(d_data->mselectedpoint)) + dx);
}
const int CAWaveformPut::TransformY(const int dy)
{
	//qDebug("Y-Pos:%f",d_data->mpCurve->y(d_data->mselectedpoint));
	return(transform(d_data->mpCurve->yAxis(),d_data->mpCurve->y(d_data->mselectedpoint)) + dy);
}

//void CAWaveformPut::openPattern(const QString file)
void CAWaveformPut::openPattern()
{
	QString pattern = QString("/usr/local/opi/bin/") + getFilename();
	if(d_data->mshotno != 0) pattern = QString("%1").arg(d_data->mshotno);

	QFile patternfile(pattern);
	if(!patternfile.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QTextStream inpattern(&patternfile);
	
	d_data->mxData.clear();
	d_data->myData.clear();
	char *pch = NULL;
	int i = 0;
	while(!inpattern.atEnd())
	{
		QString line = inpattern.readLine();
		//qDebug("patten:%s", line.toStdString().c_str());
		char str[line.toStdString().size()];
		strcpy (str, line.toStdString().c_str());
		pch = strtok(str, ", ");
		while(pch!=NULL)
		{
			switch(i)
			{
				case 0:
					//qDebug("XValue:%s", pch);
					d_data->mxData.push_back(QString(pch).toDouble());
					break;
				case 1:
					//qDebug("YValue:%s", pch);
					d_data->myData.push_back(QString(pch).toDouble());
				default:
					break;
			}
			pch = strtok (NULL, ", ");
		};
		i++;
	};

	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	replot();
	patternfile.close();
}
void CAWaveformPut::savePattern()
{
	int chkIndex = 0;
	if(checkXList(chkIndex) < 0)
	{
		qDebug("Error Invalid Time Set(Index:%d)", chkIndex);
		QString msg = QString("Error Invalid Time Set(Index:%1)").arg(chkIndex);
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};

	QString pattern = QString("/usr/local/opi/bin/") + getFilename();
	if(d_data->mshotno != 0) pattern = QString("%1").arg(d_data->mshotno);


	QFile patternfile(pattern);

	if(!patternfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;

	QTextStream outpattern(&patternfile);

	for (int i = 0; i < d_data->mxData.count(); i++)
		outpattern << d_data->mxData[i] << " ";

	outpattern << endl;

	for (int i = 0; i < d_data->myData.count(); i++)
		outpattern << d_data->myData[i] << " ";

	patternfile.close();
}
const bool CAWaveformPut::getXNearby() const
{
	return d_data->mxnearyby;
}
void CAWaveformPut::setXNearby(const bool xnearby)
{
	d_data->mxnearyby = xnearby;
}
const bool CAWaveformPut::getYNearby() const
{
	return d_data->mynearyby;
}
void CAWaveformPut::setYNearby(const bool ynearby)
{
	d_data->mynearyby = ynearby;
}
const QString & CAWaveformPut::getFilename() const
{
	return d_data->mfilename;
}
void  CAWaveformPut::setFilename(const QString &filename)
{
	d_data->mfilename = filename;
}
void  CAWaveformPut::saveAsPattern()
{
	int chkIndex = 0;
	if(checkXList(chkIndex) < 0)
	{
		qDebug("Error Invalid Time Set(Index:%d)", chkIndex);
		QString msg = QString("Error Invalid Time Set(Index:%1)").arg(chkIndex);
		QMessageBox msgBox;
		msgBox.setText(msg);
		msgBox.exec();
		return;
	};

	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/kstar/untitled.dat", tr("Pattern (*.dat)"));
	QFile patternfile(filename);

	if(!patternfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;

	QTextStream outpattern(&patternfile);

	for (int i = 0; i < d_data->mxData.count(); i++)
		outpattern << d_data->mxData[i] << " ";

	outpattern << endl;

	for (int i = 0; i < d_data->myData.count(); i++)
		outpattern << d_data->myData[i] << " ";

	patternfile.close();
}

void  CAWaveformPut::openAsPattern()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/kstar/", tr("Pattern (*.dat)"));

	QFile patternfile(filename);
	if(!patternfile.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QTextStream inpattern(&patternfile);
	
	d_data->mxData.clear();
	d_data->myData.clear();
	char *pch = NULL;
	int i = 0;
	while(!inpattern.atEnd())
	{
		QString line = inpattern.readLine();
		//qDebug("patten:%s", line.toStdString().c_str());
		char str[line.toStdString().size()];
		strcpy (str, line.toStdString().c_str());
		pch = strtok(str, ", ");
		while(pch!=NULL)
		{
			switch(i)
			{
				case 0:
					//qDebug("XValue:%s", pch);
					d_data->mxData.push_back(QString(pch).toDouble());
					break;
				case 1:
					//qDebug("YValue:%s", pch);
					d_data->myData.push_back(QString(pch).toDouble());
				default:
					break;
			}
			pch = strtok (NULL, ", ");
		};
		i++;
	};

	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	replot();
	patternfile.close();
}
CAWaveformPut::ControlMode CAWaveformPut::getMode() const
{
	return d_data->mmode;
}
void CAWaveformPut::setMode(ControlMode mode)
{
	d_data->mmode = mode;
}
void CAWaveformPut::SetWavePVCount(const short value)
{
	d_data->mwavecount = value;
}

short CAWaveformPut::GetWavePVCount()
{
	//short count = (short)caGet(getCountpv());
	//return count;
	return d_data->mwavecount;
}
void CAWaveformPut::SetYData(const double &dvalue)
{
	d_data->localyData.push_back(dvalue);
	emit valuesignal(dvalue);
}
void CAWaveformPut::SetXData(const double &dvalue)
{
	size_t xdatasize = d_data->localxData.size();
	size_t ydatasize = d_data->localyData.size();
	//qDebug("XDataCnt:%d, YDataCount:%d", xdatasize, ydatasize);

#if 1
	if(ydatasize < xdatasize ) 
	{
		//qDebug("XDataCnt:%d, YDataCount:%d", xdatasize, ydatasize);
		//return;
		double prevalue = d_data->localyData.at(ydatasize-1);
		d_data->localyData.push_back(prevalue);
	};
#endif
	d_data->localxData.push_back(dvalue);

#if 0
	size_t xdatasize = d_data->localxData.size();
	size_t ydatasize = d_data->localyData.size();
	//qDebug("XDataCnt:%d, YDataCount:%d", xdatasize, ydatasize);
#endif
}
void CAWaveformPut::SetYWaveData(const int count, const double *dvalue)
{
	d_data->localyWaveData.clear();
	for(int i = 0; i < count; i++)
	{
		//qDebug("Y-Wave:%f", dvalue[i]);
		d_data->localyWaveData.push_back(dvalue[i]);
	};
}
void CAWaveformPut::SetXWaveData(const int count, const double *dvalue)
{
	d_data->localxWaveData.clear();
	for(int i = 0; i < count; i++)
	{
		//qDebug("X-Wave:%f", dvalue[i]);
		d_data->localxWaveData.push_back(dvalue[i]);
	};
}

void CAWaveformPut::Update(const int count)
{
	emit updatesignal(count);
}

void CAWaveformPut::Update(const QString nameindex, const int xytype)
{
	emit updatesignal(nameindex, xytype);
}

void CAWaveformPut::UpdateSlot(const QString nameindex, const int xytype)
{
	QStringList pvlist;
	if(xytype == 0)
		pvlist = d_data->mxpvlist;
	else
		pvlist = d_data->mypvlist;

	//qDebug("index-%d", index);
	int index = pvlist.indexOf(nameindex);
	if(index < 0) return;

	//qDebug("vecCurve[%d]:%p", index, d_data->mvecCurve.at(index));
	d_data->mvecCurve.at(index)->setSamples(d_data->localxData,d_data->localyData);;
	//d_data->mvecCurve.at(0)->setSamples(d_data->localxData,d_data->localyData);;
	replot();
}

void CAWaveformPut::UpdateSlot(const int count)
{
	d_data->mxData.clear();
	d_data->myData.clear();
	//qDebug("Update Slot Called...");
	for(int i= 0; i < count; i++)
	{
		d_data->mxData.push_back(d_data->localxWaveData.at(i));
		d_data->myData.push_back(d_data->localyWaveData.at(i));
		//qDebug("X-Val:%f, Y-Val:%f", d_data->localxWaveData.at(i), d_data->localyWaveData.at(i));
	};
	d_data->mpCurve->setSamples(d_data->mxData, d_data->myData);
	replot();
}

void CAWaveformPut::Update()
{
	qDebug("Update Called...:%d", mUpdate);
	if(mUpdate == 2) 
	{
		Update(d_data->mwavecount);
		mUpdate = 0;
	};
	mUpdate++;
}

void CAWaveformPut::timerEvent (QTimerEvent * /*event*/)
{
	//IP Current Simulation.
	static double xvalue = -1.0;
	static double yvalue = 1.0;
	d_data->localxData.push_back(xvalue);

	d_data->localyData.push_back(yvalue);;
	//d_data->myData;
	//d_data->mxData;

	xvalue += 0.01;

	d_data->mvecCurve.at(0)->setSamples(d_data->localxData,d_data->localyData);;
	replot();
}
//for Test
void CAWaveformPut::reSet()
{
	d_data->localxData.clear();
	d_data->localyData.clear();;
	d_data->mvecCurve.at(0)->setSamples(d_data->localxData,d_data->localyData);;
	replot();
}
void CAWaveformPut::ReSet()
{
	emit resetsignal();
}
