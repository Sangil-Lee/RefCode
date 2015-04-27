/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef CA_OBJECT_H
#define CA_OBJECT_H

#include <QtGui>
#include "qwt_global.h"

class QWT_EXPORT CAObject : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);

public:
	explicit CAObject(QWidget *parent = NULL);
	virtual ~CAObject();
	const QString &getPvname() const;
	const QString &getPrefix() const;
	void  setPvname(const QString &pvname);
	void  setPrefix(const QString &prefix);
	// a set of dummies to help the designer
public slots:
	virtual void changeValue (const double &value, const short& severity, const short& precision );
private:
	QString m_pvname;
	QString m_prefix;
};
#endif
