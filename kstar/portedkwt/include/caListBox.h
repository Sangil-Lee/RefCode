/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_LISTBOX_H
#define CA_LISTBOX_H

#include "cadef.h"
#include <QtGui>
#include <QListWidget>

class CAListBox : public QListWidget
{
	Q_OBJECT
	//Q_ENUMS(ProcessMode UserProcess SystemProcess)
	Q_ENUMS(ProcessMode)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(ProcessMode cmode READ getProcessmode WRITE setProcessmode);
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);

public:
	explicit CAListBox(QWidget *parent = NULL);
	virtual ~CAListBox();

	enum ProcessMode {
		User = 0x0000,
		System = 0x0001
	};
	enum UserProcess {
		TMS = 0x00000001,
		VMS = 0x00000001<<1,
		ECH = 0x00000001<<2,
		SCS = 0x00000001<<3,
		FUEL = 0x00000001<<4,
		GCDS = 0x00000001<<5,
		ICRH = 0x00000001<<6,
		CSM = 0x00000001<<7,
		MPS = 0x00000001<<8,
		TMSENGINE = 0x00000001<<15,
		VMSENGINE = 0x00000001<<16,
		CLSENGINE = 0x00000001<<17,
		HDSENGINE = 0x00000001<<18,
		HRSENGINE = 0x00000001<<19,
		ICSENGINE = 0x00000001<<20,
		PMSENGINE = 0x00000001<<21
	};
	enum SystemProcess {
		CAREPEATER	= 0x00000001,
		NTPD		= 0x00000001<<1,
		HTTPD		= 0x00000001<<2,
		JAVA		= 0x00000001<<3,
		NSRD		= 0x00000001<<4,
		MMFSD		= 0x00000001<<5,
		MYSQLD		= 0x00000001<<6,
		MDSIP		= 0x00000001<<7,
		GATEWAY		= 0x00000001<<8,
		NIMXS		= 0x00000001<<9
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void init();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	ProcessMode getProcessmode() const;
	void setProcessmode(ProcessMode cmode);

	const QString &getPrefix() const;
	void  setPrefix(const QString &prefix);

public slots:
	virtual void changeValue (const short &connstatus, const double &value, const short& severity);

signals:

private:
	class PrivateData;
	PrivateData *d_data;
	int bitCheck(unsigned int src, unsigned int org);
};
#endif
