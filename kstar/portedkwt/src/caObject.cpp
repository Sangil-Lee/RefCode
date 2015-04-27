/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab
#include "caObject.h"
//
CAObject::CAObject(QWidget *parent): QWidget(parent)
{
};

//
CAObject::~CAObject()
{
};

//
void CAObject::setPvname(const QString &name)
{
    m_pvname = name;
    QString m_pvnamel;
};

//
const QString &CAObject::getPvname() const
{
	return m_pvname;
};

//
void CAObject::setPrefix(const QString &prefix)
{
    m_prefix = prefix;
};

//
const QString &CAObject::getPrefix() const
{
	return m_prefix;
};

//
void CAObject::changeValue (const double &value, const short& severity, const short& precision )
{
	QString sval = QString("%1 K").arg(value, 0, 'f', precision);
	qDebug("CAObject:changeValue:%s, severity:%d", sval.toStdString().c_str(), severity);
};
