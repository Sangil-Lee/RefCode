/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/


#ifndef COLOR_BAR_SCALE_WIDGET_H
#define COLOR_BAR_SCALE_WIDGET_H

#include <QtGui>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_draw.h>
#include "caColorTilebutton.h"

class CAColorTileButton;
class ColorBarScaleWidget : public QwtScaleWidget
{
	Q_OBJECT
	Q_ENUMS(ColorMap);
	Q_ENUMS(Align);
	Q_PROPERTY(QString titlename READ getTitleName WRITE setTitleName);
	Q_PROPERTY(int barwidth READ getBarWidth WRITE setBarWidth);
	Q_PROPERTY(int rangeMin READ getRangeMin WRITE setRangeMin);
	Q_PROPERTY(int rangeMax READ getRangeMax WRITE setRangeMax);
	Q_PROPERTY(ColorMap colormap READ getColormap WRITE setColormap);
	Q_PROPERTY(Align align READ getAlign WRITE setAlign);
	Q_PROPERTY(bool showcolorbar READ getShowColorBar WRITE setShowColorBar);

public:
	explicit ColorBarScaleWidget(QWidget *parent = NULL);
	virtual ~ColorBarScaleWidget();

	enum ColorMap 
	{
		JET,
		HSV,
		HOT,
		COOL,
		SPRING
	};
	enum Align {
		Right,
		Left,
		Bottom,
		Top
	};

	const QString getTitleName() const;
	void  setTitleName(const QString &titlename);

	const int getBarWidth();
	void  setBarWidth(const int barwidth);

	const int getRangeMin();
	void  setRangeMin(const int min);

	const int getRangeMax();
	void  setRangeMax(const int max);
	
	ColorMap getColormap() const;
	void setColormap(ColorMap colormap);

	Align getAlign() const; 
	void setAlign (Align align);

	bool getShowColorBar() const;
	void setShowColorBar(const bool showbar);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

signals:
	void SendColorCode(const QString pvname, const QString );
	void SetMinValue_Sig(const QString strvalue);
	void SetMaxValue_Sig(const QString strvalue);
	void SetMinValue_Sig(const int minvalue);
	void SetMaxValue_Sig(const int maxvalue);

public slots:
	//void AcceptColorTile(const CAColorTileButton *);
	void ExtractColorCode(const QString pvname, const double valuex);
	void SetMinValue_Slot(const int minvalue);
	void SetMinValue_Slot(const QString strmin);
	void SetMaxValue_Slot(const int maxvalue);
	void SetMaxValue_Slot(const QString strmax);

private:
	void initColorBarScaleWidget();
	double interpolationColor( const double valuex);
	void showTickLabel( const bool show);

private:
	//data member
	class PrivateData;
	PrivateData *d_data;
	QString m_title;
};
#endif
