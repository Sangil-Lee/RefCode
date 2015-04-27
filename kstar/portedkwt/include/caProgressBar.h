/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_PROGRESSBAR_H
#define CA_PROGRESSBAR_H

#include <QtGui>
#include <QProgressBar>
#include "cadef.h"


class CAProgressBar : public QProgressBar
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);

public:
	explicit CAProgressBar(QWidget *parent = NULL);
	virtual ~CAProgressBar();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initProgressBar();

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

	void changeValue (const short &connstatus, const double &value, const short& severity, const short& precision );
	// a set of dummies to help the designer
signals:
public slots:
protected:

private:
	QString m_pvname;
	class PrivateData;
	PrivateData *d_data;
};
#endif
