/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef WAVE_PICKER_H
#define WAVE_PICKER_H

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
#include <qwt_symbol.h>
#include "caWaveformput.h"

class QPoint;
class QCustomEvent;
class QwtPlotCurve;
class CAWaveformPut;

class WavePicker:public QObject
{
	Q_OBJECT
public:
	WavePicker(CAWaveformPut *plot);
	virtual bool eventFilter(QObject *o, QEvent *e);
	virtual bool event(QEvent *);

private:
	CAWaveformPut *plot() {return (CAWaveformPut *)parent();};
	const CAWaveformPut *plot() const {return (CAWaveformPut *)parent();};
	void move(const QPoint &pos);
	void select(const QPoint &pos);
	void remove(const QPoint &pos);
	void insertPoint(const QPoint &pos);
	void movePointBy(int dx, int dy);
	void movePointValue(double dx, double dy);
	int mselectedpoint;
};
#endif
