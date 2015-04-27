/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef STATICGRAPHIC_H
#define STATICGRAPHIC_H

#include <math.h>
#include <QtGui>


class StaticGraphic : public QWidget
{
	Q_OBJECT
	Q_ENUMS(Geometry FMode GeomDirection)
	Q_PROPERTY(Geometry geometryType READ geometryType WRITE setGeometryType)
	Q_PROPERTY(GeomDirection geomDirection READ geomDirection WRITE setGeomDirection)
	Q_PROPERTY(FMode fillMode READ fillMode WRITE setFillMode)
	Q_PROPERTY(QColor fillColor READ fillColor0 WRITE setFillColor0)
	//Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor)
	Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
	Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
	Q_PROPERTY(bool blink READ getBlink WRITE setBlink)

public:
	explicit StaticGraphic(QWidget *parent = NULL);
	virtual ~StaticGraphic();

	enum Geometry {
                Rectangle = 0x0000,
                RoundRectangle = 0x0001,
                Ellipse = 0x0002,
                RightArrow = 0x0003,
                LeftArrow = 0x0004,
                UpArrow = 0x0005,
                DownArrow = 0x0006,
                Coil = 0x0007,
                LeftCapsule = 0x0008,
                RightCapsule = 0x0009,
                HPValve = 0x0010,
                VPValve = 0x0011,
                HMValve = 0x0012,
                VMValve = 0x0013,
                HEValve = 0x0014,
                VEValve = 0x0015,
				HGValve = 0x0016,
				VGValve = 0x0017,
				HP2Valve = 0x0018,
				VP2Valve = 0x0019,
				HAValve = 0x001A,
				VAValve = 0x001B,
				HSValve = 0x001C,
				VSValve = 0x001D,
                VacuumPump = 0x0020,
                CRP = 0x0021,
                TMP = 0x0022,
				BP = 0x0023,
				DP = 0x0024,
				RP = 0x0025,
				DRP = 0x0026,
                VacuumGauge = 0x0027,
                VacuumChamber = 0x0028,
                BellJar = 0x0029,
				VacuumVessel = 0x002A,
				DiffusionPump = 0x002B
	};

	enum FMode {
		Solid = 0x0000,
		Empty = 0x0001
	};

	enum GeomDirection {
		Default = 0x0000,
		East = 0x0001,
		West = 0x0002,
		South = 0x0003,
		North = 0x0004
	};


	Geometry geometryType() const;
	void setGeometryType(Geometry gtype);

	GeomDirection geomDirection() const;
	void setGeomDirection(GeomDirection gdir);

	FMode fillMode() const;
	void setFillMode(FMode fillmode);

	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;

	void initStaticGraphic();
	void drawStaticGraphic(QPainter *p);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const QColor &fillColor0() const;
	void setFillColor0(const QColor &fillColor);

	const QColor &fillColor() const;
	void setFillColor(const QColor &fillColor);

	const QColor &lineColor0() const;
	void setLineColor0(const QColor &lineColor);

	const QColor &lineColor() const;
	void setLineColor(const QColor &lineColor);

	const int lineWidth();
	void setLineWidth(const int lineWidth);

	const QPainterPath getClosedPath();
	void setClosedPath(Geometry gtype) const;
	
	const QPainterPath getOpenPath();
	void setOpenPath(Geometry gtype) const;

	bool getBlink();
	void setBlink(bool blink);
	bool blinkStatus();

	virtual void paintEvent (QPaintEvent *);
	void paint();
public slots:
	void startBlink ();
	void stopBlink ();


protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void timerEvent (QTimerEvent *event);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
