/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_IMAGEMBBI_H
#define CA_IMAGEMBBI_H

#include <QtGui>
#include "cadef.h"

class CAImageMbbi : public QWidget
{
	Q_OBJECT
	Q_ENUMS(DisplayMode)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname)
	Q_PROPERTY(DisplayMode dmode READ getDisplaymode WRITE setDisplaymode)

	Q_PROPERTY(QPixmap zrpxm READ getZrpixmap WRITE setZrpixmap)
	Q_PROPERTY(QPixmap onpxm READ getOnpixmap WRITE setOnpixmap)
	Q_PROPERTY(QPixmap twpxm READ getTwpixmap WRITE setTwpixmap)
	Q_PROPERTY(QPixmap thpxm READ getThpixmap WRITE setThpixmap)
	Q_PROPERTY(QPixmap frpxm READ getFrpixmap WRITE setFrpixmap)
	Q_PROPERTY(QPixmap fvpxm READ getFvpixmap WRITE setFvpixmap)
	Q_PROPERTY(QPixmap sxpxm READ getSxpixmap WRITE setSxpixmap)
	Q_PROPERTY(QPixmap svpxm READ getSvpixmap WRITE setSvpixmap)
	Q_PROPERTY(QPixmap eipxm READ getEipixmap WRITE setEipixmap)
	Q_PROPERTY(QPixmap nipxm READ getNipixmap WRITE setNipixmap)
	Q_PROPERTY(QPixmap tepxm READ getTepixmap WRITE setTepixmap)
	Q_PROPERTY(QPixmap elpxm READ getElpixmap WRITE setElpixmap)
	Q_PROPERTY(QPixmap tvpxm READ getTvpixmap WRITE setTvpixmap)
	Q_PROPERTY(QPixmap ttpxm READ getTtpixmap WRITE setTtpixmap)
	Q_PROPERTY(QPixmap ftpxm READ getFtpixmap WRITE setFtpixmap)
	Q_PROPERTY(QPixmap ffpxm READ getFfpixmap WRITE setFfpixmap)

	Q_PROPERTY(QPixmap falsepxm READ getFalsepixmap WRITE setFalsepixmap)
	Q_PROPERTY(QPixmap truepxm READ getTruepixmap WRITE setTruepixmap)
	Q_PROPERTY(int index READ getIndex WRITE setIndex)

public:
	explicit CAImageMbbi(QWidget *parent = NULL);
	virtual ~CAImageMbbi();

	enum DisplayMode {
		Choice = 0x0000,
		TrueFalse = 0x0001
	}; 

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initCAImageMbbi();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	DisplayMode getDisplaymode() const;
	void setDisplaymode(DisplayMode dmode);

	const QPixmap &getZrpixmap() const;
	void  setZrpixmap(const QPixmap &zrpxm);

	const QPixmap &getOnpixmap() const;
	void  setOnpixmap(const QPixmap &onpxm);

	const QPixmap &getTwpixmap() const;
	void  setTwpixmap(const QPixmap &twpxm);

	const QPixmap &getThpixmap() const;
	void  setThpixmap(const QPixmap &thpxm);

	const QPixmap &getFrpixmap() const;
	void  setFrpixmap(const QPixmap &frpxm);

	const QPixmap &getFvpixmap() const;
	void  setFvpixmap(const QPixmap &fvpxm);

	const QPixmap &getSxpixmap() const;
	void  setSxpixmap(const QPixmap &sxpxm);

	const QPixmap &getSvpixmap() const;
	void  setSvpixmap(const QPixmap &svpxm);

	const QPixmap &getEipixmap() const;
	void  setEipixmap(const QPixmap &eipxm);

	const QPixmap &getNipixmap() const;
	void  setNipixmap(const QPixmap &nipxm);

	const QPixmap &getTepixmap() const;
	void  setTepixmap(const QPixmap &tepxm);

	const QPixmap &getElpixmap() const;
	void  setElpixmap(const QPixmap &elpxm);

	const QPixmap &getTvpixmap() const;
	void  setTvpixmap(const QPixmap &tvpxm);

	const QPixmap &getTtpixmap() const;
	void  setTtpixmap(const QPixmap &ttpxm);

	const QPixmap &getFtpixmap() const;
	void  setFtpixmap(const QPixmap &ftpxm);

	const QPixmap &getFfpixmap() const;
	void  setFfpixmap(const QPixmap &ffpxm);

	const QPixmap &getFalsepixmap() const;
	void  setFalsepixmap(const QPixmap &falsepxm);

	const QPixmap &getTruepixmap() const;
	void  setTruepixmap(const QPixmap &truepxm);

	void  setTruepxm(const QPixmap &truepxm);
	void  setFalsepxm(const QPixmap &falsepxm);

	int getIndex() const;
	void  setIndex(int index);

public slots:
	virtual void changeValue (const short &connstatus, const int &value);
private:
	class PrivateData;
	PrivateData *d_data;

};
#endif
