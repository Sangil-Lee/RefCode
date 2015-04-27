/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/


#ifndef CA_MDSPLUSPLOT_H
#define CA_MDSPLUSPLOT_H
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
#include "mdsplusdata.h"
//#include "caMDSplusplotthr.h"


using std::vector;
using std::string;


class CAMDSplusplot : public QwtPlot
{
	Q_OBJECT
	Q_ENUMS(TypeOfCall)

	Q_PROPERTY(QString treename  READ getTreename  WRITE setTreename);

	Q_PROPERTY(QString nodename  READ getNodename  WRITE setNodename);
	Q_PROPERTY(QString nodename1 READ getNodename1 WRITE setNodename1);
	Q_PROPERTY(QString nodename2 READ getNodename2 WRITE setNodename2);
	Q_PROPERTY(QString nodename3 READ getNodename3 WRITE setNodename3);
	Q_PROPERTY(QString nodename4 READ getNodename4 WRITE setNodename4);
	Q_PROPERTY(QString nodename5 READ getNodename5 WRITE setNodename5);
	Q_PROPERTY(QString nodename6 READ getNodename6 WRITE setNodename6);
	Q_PROPERTY(QString nodename7 READ getNodename7 WRITE setNodename7);
	Q_PROPERTY(QString nodename8 READ getNodename8 WRITE setNodename8);
	Q_PROPERTY(QString nodename9 READ getNodename9 WRITE setNodename9);
    Q_PROPERTY(QColor canvasColor READ getCanvascolor WRITE setCanvascolor); 

    Q_PROPERTY(QColor majLinecolor READ getMajorlinecolor WRITE setMajorlinecolor); 
    Q_PROPERTY(QColor minLinecolor READ getMinorlinecolor WRITE setMinorlinecolor); 

	Q_PROPERTY(QString plotTitle READ getPlottitle WRITE setPlottitle);
	Q_PROPERTY(QString xTitle READ getXtitle WRITE setXtitle);
	Q_PROPERTY(QString yTitle READ getYtitle WRITE setYtitle);

	Q_PROPERTY(bool logScale READ getLogscale WRITE setLogscale);
	Q_PROPERTY(int reqElement READ getREQNOE WRITE setREQNOE);
	Q_PROPERTY(int updateTime READ getUpdateTime WRITE setUpdateTime);
	Q_PROPERTY(int refreshTime READ getRefreshTime WRITE setRefreshTime);
	Q_PROPERTY(double baseTime READ getBaseTime WRITE setBaseTime);
	Q_PROPERTY(double factor READ getFactor WRITE setFactor);
	Q_PROPERTY(TypeOfCall type READ getType WRITE setType);
	Q_PROPERTY(QString serverIP READ getServerIP WRITE setServerIP);
	Q_PROPERTY(QString serverPort READ getServerPort WRITE setServerPort);

public:
	explicit CAMDSplusplot(QWidget *parent = NULL);
	virtual ~CAMDSplusplot();
	enum TypeOfCall {
		CAPendIO,
		CAPendEvent
	};


	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initMDSPlot();

	const QString &getTreename() const;
	void  setTreename(const QString &treename);

	const QString getNodename() const;
	void  setNodename(const QString &nodename);

	const QString &getNodename1() const;
	void  setNodename1(const QString &nodename);

	const QString &getNodename2() const;
	void  setNodename2(const QString &nodename);

	const QString &getNodename3() const;
	void  setNodename3(const QString &nodename);

	const QString &getNodename4() const;
	void  setNodename4(const QString &nodename);

	const QString &getNodename5() const;
	void  setNodename5(const QString &nodename);

	const QString &getNodename6() const;
	void  setNodename6(const QString &nodename);

	const QString &getNodename7() const;
	void  setNodename7(const QString &nodename);

	const QString &getNodename8() const;
	void  setNodename8(const QString &nodename);

	const QString &getNodename9() const;
	void  setNodename9(const QString &nodename);

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

	const int &getREQNOE();
	void setREQNOE(const int &reqelement);

	const int &getUpdateTime();
	void setUpdateTime(const int &msec);

	const int &getRefreshTime();
	void setRefreshTime(const int &msec);

	const double &getBaseTime() const; 
	void setBaseTime(const double &basetime);

	const double &getFactor() const; 
	void setFactor(const double &factor);

	TypeOfCall getType() const;
	void setType(TypeOfCall type);

	const QString &getServerIP() const; 
	void setServerIP(const QString &serverip);

	const QString &getServerPort() const; 
	void setServerPort(const QString &serverport);

	void PrintPvNames ();
	void Run();
	void Start();
	void Stop();
	void SetData();
	const bool isAuto();
	const long getShotNumber();
	//const int caGetValue(const string pvname);
	chid mchid_shotno;
	chid mchid_pfshsum;
signals:
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
	void signalShotNumber(const int shotno);
	void signalShotNumber(QString strshotno);
	void legendChecked(QwtPlotItem *, bool);
	void signalMaxKappa(const QString strmaxval);
	void signalMaxBetan(const QString strmaxval);
	void signalMaxWtot(const QString strmaxval);
	void signalMaxECH(const QString strmaxval);
	void signalMaxICRH(const QString strmaxval);
	void signalMaxNB1(const QString strmaxval);
	void signalMaxNB2(const QString strmaxval);
	void signalMaxIp(const QString strmaxval);
	void signalMaxNe(const QString strmaxval);
	void signalMaxHa(const QString strmaxval);
	void signalMaxTe(const QString strmaxval);
	void signalMaxPre(const QString strmaxval);
	void signalMinValue(const QString strminval);

public slots:
	void changeNodeName (const int index, const QString &pvname);
	void displayCurve(QwtPlotItem *item, bool on);
	void setYRange(const double min, const double max);
	void displayCurve(const bool on, const QString &pvname);

	void penIndex(const int index);
	void penWidth(const int width);
	void penStyle(const QString strstyle);
	void penColor(const QString color);
	void penColor(const QColor &color);
	void factor(const QString strfactor);
	void nonfactors();
	void shotNumber(const int iShotno);
	void shotNumber(const double dShotno);
	void autoManual(const bool bAuto);
	void readShot();
	void shotTreeName(const QString &treename);
	void addNodeName(const QString &nodename);
	void slotReadShot(const int shotno);

private slots:
	void timerEvent();

private:
	//virtual void timerEvent(QTimerEvent *);
	QTimer *mpTimer;
	void getNodeNameAll ();
	QString getTime(const int &time);
	void setDamp(double damping);
	void signalColor(const int index);
	void plotupdate();
	int createCHID();
	void deleteCHID();
	const int caGetShotNo();
	const int caGetPerformShotSummary();
	void screenShot(const int shotno,const QString strType);

private:
	//data member
	class PrivateData;
	PrivateData *d_data;
	MDSPlusData *mdsplusdata;
	bool mbAuto;
	//CAMDSplusplotThr *mmdsdatathread;
};
#endif
