/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_COLOR_TILE_BUTTON_H
#define CA_COLOR_TILE_BUTTON_H

#include <QtGui>
#include "cadef.h"
//#include "colorBarScaleWidget.h"

class CAColorTileButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);

public:
	explicit CAColorTileButton(QWidget *parent = NULL);
	virtual ~CAColorTileButton();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initPushButton();
	const bool &getValue() const;

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	void changeValue (const short &connstatus, const double &value);

public slots:
	void GetColorCode(const QString pvname, const QString color);

signals:
	//void ConnectColorMapBar( const CAColorTileButton *);	
	void GetColorFromColorMapBar(const QString pvname, const double valuex );

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
