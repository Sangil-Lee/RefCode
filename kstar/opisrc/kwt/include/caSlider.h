/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_SLIDER_H
#define CA_SLIDER_H

#include <QtGui>
#include "cadef.h"


class CASlider : public QSlider
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(bool control READ getControl WRITE setControl);
	Q_PROPERTY(bool immediate READ getImmediate WRITE setImmediate);
	Q_PROPERTY(bool tracking READ getTRacking WRITE setTRacking);

public:
	explicit CASlider(QWidget *parent = NULL);
	virtual ~CASlider();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initSlider();

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

	const bool getControl() const;
	void  setControl(const bool control);

	const bool getImmediate() const;
	void  setImmediate(const bool immediate);

	const bool getTRacking() const;
	void  setTRacking(const bool tracking);

	void changeValue (const short &connstatus, const double &value, const short& severity, const short& precision );

	// a set of dummies to help the designer
signals:
	void TrackingValue(QString strvalue);

public slots:
	//void changePvUnit (const QString &pvunit);
	void SetMaximum(const int minvalue);
	void SetMinimum(const int maxvalue);
	void casliderSlot(int val);

protected:
	//virtual void mouseMoveEvent( QMouseEvent *ev);
	virtual void wheelEvent(QWheelEvent *e);
private:
	QString m_pvname;

	class PrivateData;
	PrivateData *d_data;
};
#endif
