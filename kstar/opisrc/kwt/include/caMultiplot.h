/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/


#ifndef CA_MULTIPLOT_H
#define CA_MULTIPLOT_H
#include <vector>
#include <string>

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_data.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <cadef.h>

using std::vector;
using std::string;

class CAMultiplot : public QwtPlot
{
	Q_OBJECT
	Q_ENUMS(ePeriodic)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString pvname1 READ getPvname1 WRITE setPvname1);
	Q_PROPERTY(QString pvname2 READ getPvname2 WRITE setPvname2);
	Q_PROPERTY(QString pvname3 READ getPvname3 WRITE setPvname3);
	Q_PROPERTY(QString pvname4 READ getPvname4 WRITE setPvname4);
	Q_PROPERTY(QString pvname5 READ getPvname5 WRITE setPvname5);
	Q_PROPERTY(QString pvname6 READ getPvname6 WRITE setPvname6);
	Q_PROPERTY(QString pvname7 READ getPvname7 WRITE setPvname7);
	Q_PROPERTY(QString pvname8 READ getPvname8 WRITE setPvname8);
	Q_PROPERTY(QString pvname9 READ getPvname9 WRITE setPvname9);
	Q_PROPERTY(int timeLineCount READ getTimeLineCnt WRITE setTimeLineCnt);

    Q_PROPERTY(QColor canvasColor READ getCanvascolor WRITE setCanvascolor); 
    Q_PROPERTY(QColor majLinecolor READ getMajorlinecolor WRITE setMajorlinecolor); 
    Q_PROPERTY(QColor minLinecolor READ getMinorlinecolor WRITE setMinorlinecolor); 
	Q_PROPERTY(QString plotTitle READ getPlottitle WRITE setPlottitle);
	Q_PROPERTY(QString xTitle READ getXtitle WRITE setXtitle);
	Q_PROPERTY(QString yTitle READ getYtitle WRITE setYtitle);

	Q_PROPERTY(bool logScale READ getLogscale WRITE setLogscale);
	Q_PROPERTY(ePeriodic periodicdata READ getPeriodic WRITE setPeriodic);


public:
	explicit CAMultiplot(QWidget *parent = NULL);
	virtual ~CAMultiplot();

	enum ePeriodic {
		PointOne	= 0x0000,
		PointFive	= 0x0001,
		OneSec		= 0x0002,
		FiveSec		= 0x0003,
		TenSec		= 0x0004
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initMultiPlot();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const QString &getPvname1() const;
	void  setPvname1(const QString &pvname);

	const QString &getPvname2() const;
	void  setPvname2(const QString &pvname);

	const QString &getPvname3() const;
	void  setPvname3(const QString &pvname);

	const QString &getPvname4() const;
	void  setPvname4(const QString &pvname);

	const QString &getPvname5() const;
	void  setPvname5(const QString &pvname);

	const QString &getPvname6() const;
	void  setPvname6(const QString &pvname);

	const QString &getPvname7() const;
	void  setPvname7(const QString &pvname);

	const QString &getPvname8() const;
	void  setPvname8(const QString &pvname);

	const QString &getPvname9() const;
	void  setPvname9(const QString &pvname);

	const bool &getLogscale() const;
	void  setLogscale(const bool setlog);

	const QColor &getCanvascolor();
	void setCanvascolor(const QColor &color);

	const QString &getPlottitle() const;
	void  setPlottitle(const QString &title);

	const QString &getXtitle() const;
	void  setXtitle(const QString &title);

	const QString &getYtitle() const;
	void  setYtitle(const QString &title);

	const QColor &getMajorlinecolor();
	void setMajorlinecolor(const QColor &color);

	const QColor &getMinorlinecolor();
	void setMinorlinecolor(const QColor &color);

	ePeriodic getPeriodic();
	void setPeriodic(ePeriodic periodic);

	const int getTimeLineCnt() const; 
	void setTimeLineCnt(const int timeline);

	void PrintPvNames ();
	//void Run(const int interval);
	void Run();

signals:
	void currentValue(const double &pvval);
	void currentValue1(const double &pvval);
	void currentValue2(const double &pvval);
	void currentValue3(const double &pvval);
	void currentValue4(const double &pvval);
	void currentValue5(const double &pvval);
	void currentValue6(const double &pvval);
	void currentValue7(const double &pvval);
	void currentValue8(const double &pvval);
	void currentValue9(const double &pvval);
	void timeLabel (const QString &strtimelabel);
	void timeLabel1(const QString &strtimelabel);
	void timeLabel2(const QString &strtimelabel);
	void timeLabel3(const QString &strtimelabel);
	void timeLabel4(const QString &strtimelabel);
	void timeLabel5(const QString &strtimelabel);
	void legendLabel(const QString &strcolor, const QString &label);
	void legendLabel1(const QString &strcolor, const QString &label);
	void legendLabel2(const QString &strcolor, const QString &label);
	void legendLabel3(const QString &strcolor, const QString &label);
	void legendLabel4(const QString &strcolor, const QString &label);
	void legendLabel5(const QString &strcolor, const QString &label);
	void legendLabel6(const QString &strcolor, const QString &label);
	void legendLabel7(const QString &strcolor, const QString &label);
	void legendLabel8(const QString &strcolor, const QString &label);
	void legendLabel9(const QString &strcolor, const QString &label);

public slots:
	void changePvName (const int index, const QString &pvname);
	void displayCurve(QwtPlotItem *item, bool on);
	void setYRange(const double min, const double max);
	void setYRange();
	void setYRangeMax(const double max);
	void setYRangeMin(const double min);
	void displayCurve(const bool on, const QString &pvname);

	void penIndex(const int index);
	void penWidth(const int width);
	void penStyle(const QString strstyle);
	void penColor(const QString color);
	void penColor(const QColor &color);
	void factor(const QString strfactor);
	void nonfactors();


private:
	void getPvNames ();
	virtual void timerEvent(QTimerEvent *);
	void getValue(const int index, const double &pvval, const int &totsec, const int count);
	QString getTime(const int &time);
	void setDamp(double damping);
	void plotupdate();
	void signalColor(const int index);

private:
	//data member
	class PrivateData;
	PrivateData *d_data;
	int mCount;
};
#endif
