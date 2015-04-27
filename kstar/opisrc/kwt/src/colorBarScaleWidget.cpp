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
#include "colorBarScaleWidget.h"

using namespace std;

class ColorBarScaleWidget::PrivateData
{
public:
	//member method
    PrivateData():mWidth(10), mRangeMin(20), mRangeMax(200), mshowcolorbar(true)
    {
	};
public:
	//member data
	QString	mTitle;
	int mWidth;
	int mRangeMin;
	int mRangeMax;
	bool mshowcolorbar;

	ColorMap mColorCode;
	QwtLinearColorMap mLinearColMap;
	Align	malign;
};

ColorBarScaleWidget::ColorBarScaleWidget(QWidget *parent):QwtScaleWidget(QwtScaleDraw::RightScale,parent)
{
	initColorBarScaleWidget();
}

ColorBarScaleWidget::~ColorBarScaleWidget()
{
    delete d_data;
}
#if 1
const QString ColorBarScaleWidget::getTitleName() const
{
	return d_data->mTitle;
}
void ColorBarScaleWidget::setTitleName(const QString &titlename)
{
	d_data->mTitle = titlename;
	setTitle(titlename);
};
#endif

QSize ColorBarScaleWidget::sizeHint() const
{
	return minimumSizeHint();
}

void ColorBarScaleWidget::initColorBarScaleWidget()
{
    //using namespace Qt;
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("ColorBarScaleWidget"));

#if 0
	//QwtLinearColorMap colorMap(QColor("#00007F"), QColor("#7F0000")); // 0.0 ~ 0.9
	QwtLinearColorMap colorMap;
	colorMap.setColorInterval(QColor("#00007F"),QColor("#7F0000"));  //dark blue ~ dark red
	colorMap.addColorStop(0.1, QColor("#0000FF")); // blue
	colorMap.addColorStop(0.2, QColor("#007FFF")); // azure
	colorMap.addColorStop(0.3, QColor("#00FFFF")); // cyan
	colorMap.addColorStop(0.4, QColor("#7FFF00")); // green
	colorMap.addColorStop(0.5, QColor("#7FFF7F")); // light green
	colorMap.addColorStop(0.6, QColor("#FFFF00")); // yellow
	colorMap.addColorStop(0.7, QColor("#FF9600"));
	colorMap.addColorStop(0.8, QColor("#FF4B00")); 
	colorMap.addColorStop(0.9, QColor("#FF0000")); // red 

	//range [min, max] case, x value = (x - min)/(max - min)
	//ex) [20, 200] 50 value -> (50-20)/(200-20)

	QwtDoubleInterval dInterval(0.0, 1.0);
	setColorMap(dInterval, colorMap);
#endif
	setBarWidth(10);
	setColormap(JET);

	setColorBarEnabled(d_data->mshowcolorbar);

#if 0
	// Test to obtain color code
	double interval = interpolationColor(165);
	QColor testColor = d_data->mLinearColMap.color(QwtDoubleInterval(0.0, 1.0), interval);

	qDebug("[InterPolValue:%f]Color Name: %s\n", interval, testColor.name().toStdString().c_str());

	//"#00007F", "#7F0000"
	//0.1, QColor("#0000FF"));
	//0.2, QColor("#007FFF"));
	//0.3, QColor("#00FFFF"));
	//0.4, QColor("#7FFF00"));
	//0.5, QColor("#7FFF7F"));
	//0.6, QColor("#FFFF00"));
	//0.7, QColor("#FF9600"));
	//0.8, QColor("#FF4B00"));
				// <---- Here #FF4700
	//0.9, QColor("#FF0000"));
#endif
};

const int ColorBarScaleWidget::getRangeMin()
{
	return d_data->mRangeMin;
};
void  ColorBarScaleWidget::setRangeMin(const int min)
{
	d_data->mRangeMin = min;
};
const int ColorBarScaleWidget::getRangeMax()
{
	return d_data->mRangeMax;
};
void  ColorBarScaleWidget::setRangeMax(const int max)
{
	d_data->mRangeMax = max;
};

const int ColorBarScaleWidget::getBarWidth()
{
	return d_data->mWidth;
}

ColorBarScaleWidget::ColorMap ColorBarScaleWidget::getColormap() const
{
	return (ColorMap)d_data->mColorCode;
}

void ColorBarScaleWidget::setColormap(ColorMap colormap)
{
	d_data->mColorCode = colormap;

	switch(d_data->mColorCode)
	{
		case HSV:
			break;
		case HOT:
			break;
		case COOL:
			break;
		case SPRING:
			break;
		case JET:
		default:
			{
				d_data->mLinearColMap.setColorInterval(QColor("#00007F"),QColor("#7F0000"));  //dark blue ~ dark red
				d_data->mLinearColMap.addColorStop(0.1, QColor("#0000FF")); // blue
				d_data->mLinearColMap.addColorStop(0.2, QColor("#007FFF")); // azure
				d_data->mLinearColMap.addColorStop(0.3, QColor("#00FFFF")); // cyan
				d_data->mLinearColMap.addColorStop(0.4, QColor("#7FFF00")); // green
				d_data->mLinearColMap.addColorStop(0.5, QColor("#7FFF7F")); // light green
				d_data->mLinearColMap.addColorStop(0.6, QColor("#FFFF00")); // yellow
				d_data->mLinearColMap.addColorStop(0.7, QColor("#FF9600"));
				d_data->mLinearColMap.addColorStop(0.8, QColor("#FF4B00")); 
				d_data->mLinearColMap.addColorStop(0.9, QColor("#FF0000")); // red 
				setColorMap(QwtDoubleInterval(0.0, 1.0), d_data->mLinearColMap);
			};
			break;
	};
}

void ColorBarScaleWidget::setBarWidth(const int barwidth)
{
	d_data->mWidth = barwidth;
	setColorBarWidth(d_data->mWidth);
}

QSize ColorBarScaleWidget::minimumSizeHint() const
{
	int mw = 25;
	int mh = 50;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}
double ColorBarScaleWidget::interpolationColor(const double valuex)
{
	double interpolvalue = 0.0;
	if(d_data->mRangeMin > valuex)
	{
		return d_data->mRangeMin;
	}
	else if(d_data->mRangeMax < valuex)
	{
		return d_data->mRangeMax;
	}

	return (interpolvalue=(valuex - d_data->mRangeMin)/(d_data->mRangeMax - d_data->mRangeMin));
}

#if 0
void ColorBarScaleWidget::AcceptColorTile( const CAColorTileButton *tilebutton)
{
}
#endif
void ColorBarScaleWidget::ExtractColorCode(const QString pvname, const double valuex)
{
	double colorvalue = interpolationColor(valuex);
	QColor color = d_data->mLinearColMap.color(QwtDoubleInterval(0.0, 1.0), colorvalue);
	//qDebug("[InterPolValue:%f]Color Name: %s\n", colorvalue, color.name().toStdString().c_str());

	emit SendColorCode(pvname, color.name());
}
void ColorBarScaleWidget::SetMinValue_Slot(const int minvalue)
{
	setRangeMin(minvalue);
	emit SetMinValue_Sig(minvalue);
	if(!getShowColorBar())
		showTickLabel(false);
}
void ColorBarScaleWidget::SetMaxValue_Slot(const int maxvalue)
{
	setRangeMax(maxvalue);
	emit SetMaxValue_Sig(maxvalue);
	if(!getShowColorBar())
		showTickLabel(false);
}
void ColorBarScaleWidget::SetMinValue_Slot(const QString strmin)
{
	int minvalue = strmin.toInt();
	setRangeMin(minvalue);
	emit SetMinValue_Sig(minvalue);
	if(!getShowColorBar())
		showTickLabel(false);
}
void ColorBarScaleWidget::SetMaxValue_Slot(const QString strmax)
{
	int maxvalue = strmax.toInt();
	setRangeMax(maxvalue);
	emit SetMaxValue_Sig(maxvalue);
	if(!getShowColorBar())
		showTickLabel(false);
}
ColorBarScaleWidget::Align ColorBarScaleWidget::getAlign() const
{
	return d_data->malign;
}
void ColorBarScaleWidget::setAlign (ColorBarScaleWidget::Align align)
{
	d_data->malign = align;
	switch(align)
	{
		case Right:
			setAlignment(QwtScaleDraw::RightScale);
			break;
		case Left:
			setAlignment(QwtScaleDraw::LeftScale);
			break;
		case Bottom:
			setAlignment(QwtScaleDraw::BottomScale);
			break;
		case Top:
			setAlignment(QwtScaleDraw::TopScale);
			break;
	}
}

bool ColorBarScaleWidget::getShowColorBar() const
{
	return d_data->mshowcolorbar;
}

void ColorBarScaleWidget::setShowColorBar(const bool showbar)
{
	d_data->mshowcolorbar = showbar;
	showTickLabel(showbar);
	setColorBarEnabled(showbar);
}
void ColorBarScaleWidget::showTickLabel(const bool show)
{
	QwtLinearScaleEngine scaleEngine;
	if(show) {
		setScaleDiv ( scaleEngine.transformation (), scaleEngine.divideScale( 0.0, 1.0, 0, 0 ));
	}
	else {
		scaleEngine.setAttribute (QwtScaleEngine::IncludeReference);
		scaleEngine.setAttribute (QwtScaleEngine::Floating);
		scaleEngine.setMargins (0, 0);
		setScaleDiv (scaleEngine.transformation() , scaleEngine.divideScale( getRangeMin(), getRangeMax(), 5, 10, getRangeMax()/10 ));
	};
};
