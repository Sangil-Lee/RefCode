/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_COMBOBOX_H
#define CA_COMBOBOX_H

#include <QtGui>
#include <QComboBox>
#include "cadef.h"

class CAComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QStringList pvlist READ getPVList WRITE setPVList);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname)
	Q_PROPERTY(bool control READ getControl WRITE setControl)

public:
	explicit CAComboBox(QWidget *parent = NULL);
	virtual ~CAComboBox();

	enum DisplayMode {
		TrueFalse = 0x0000,
		TrueOnly = 0x0001,
		FalseOnly = 0x0002
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initComboBox();

	const QStringList getPVList() const;
	void setPVList(const QStringList pvlist);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const bool &getControl() const;
	void  setControl(const bool control);

public slots:
	virtual void changeValue (const short &connstatus, const int &value);
	void changePvName (const QString &pvname);
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
