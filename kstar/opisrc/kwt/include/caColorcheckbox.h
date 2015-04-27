/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_COLOR_CHECKBOX_H
#define CA_COLOR_CHECKBOX_H

#include <QtGui>

//class CAColorLabel : public QLabel
//class CAColorLabel:public QPushButton
//class CAColorLabel:public QRadioButton
class CAColorCheckBox:public QCheckBox
{
	Q_OBJECT

public:
	explicit CAColorCheckBox(QWidget *parent = NULL);
	virtual ~CAColorCheckBox();

	void initLabel();

signals:
	void showLabel( const bool bshow, const QString &label);

public slots:
	virtual void changeColorLabel(const QString &strcolor, const QString &label);

private slots:
	void pshowlabel( const bool bshow);

private:
	//class PrivateData;
	//PrivateData *d_data;
};
#endif
