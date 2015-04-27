/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_MBBOBUTTON_H
#define CA_MBBOBUTTON_H

#include <QtGui>
#include "cadef.h"

class CAMbboButton : public QWidget
{
	Q_OBJECT
	Q_ENUMS(AlignPolicy)
	Q_PROPERTY(AlignPolicy align READ getAlignPolicy WRITE setAlignPolicy)
	Q_PROPERTY(QStringList pvlist READ getPVList WRITE setPVList);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname)
	Q_PROPERTY(QStringList textlist READ getTextlist WRITE setTextlist)
	Q_PROPERTY(QStringList bitlist READ getBitlist WRITE setBitlist)

	Q_PROPERTY(bool control READ getControl WRITE setControl)
	Q_PROPERTY(bool confirm READ getConfirm WRITE setConfirm)

	Q_PROPERTY(QColor selColor READ getSelColor WRITE setSelColor)
	Q_PROPERTY(QColor unselColor READ getUnselColor WRITE setUnselColor)

public:
	explicit CAMbboButton(QWidget *parent = NULL);
	virtual ~CAMbboButton();

	enum DisplayMode {
		TrueFalse = 0x0000,
		TrueOnly = 0x0001,
		FalseOnly = 0x0002
	};

	enum AlignPolicy {
		Horizontal = 0x0000,
		Vertical = 0x0001
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initMbboButton();
	void init2MbboButton();

	const QStringList getPVList() const;
	void setPVList(const QStringList pvlist);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const QStringList &getTextlist() const;
	void setTextlist(const QStringList &textlist);

	const QStringList getBitlist() const;
	void setBitlist(const QStringList intlist);

	const bool &getControl() const;
	void  setControl(const bool control);

	const bool &getConfirm() const;
	void  setConfirm(const bool confirm);

	AlignPolicy getAlignPolicy() const;
	void  setAlignPolicy(AlignPolicy align);

	const QColor &getSelColor() const;
	void setSelColor(const QColor &selColor);

	const QColor &getUnselColor() const;
	void setUnselColor(const QColor &unselColor);

public slots:
	virtual void changeValue (const short &connstatus, const int &value);
	void changePvName (const QString &pvname);
	void clicked(int id);
	void print(const bool status);
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);
signals:
	void valueChanged(int id);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
