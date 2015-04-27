/*************************************************************************
 * KSTAR Widget Toolkit (KWT) V 1.0
 * This is Qt-based graphic application development toolkit for EPICS
 * and it was designed to develop operator interfaces to control KSTAR.
 *
 * Copyright (c) 2010 The National Fusion Research Institute
 * National Fusion Research Institute (NFRI)
 * 113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 * KWT is distributed under the terms of the GNU General Public License.
*************************************************************************/

/* caCheckbox.h
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 * Revision 1  2010-03
 * Author: Sulhee Baek
 * Initial revision
 */

#ifndef CA_CHECKBOX_H
#define CA_CHECKBOX_H

#include <QtGui>
#include <QCheckBox>

class CACheckBox : public QCheckBox
{
	Q_OBJECT
	Q_PROPERTY(QStringList pvlist READ getPVList WRITE setPVList);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(bool control READ getControl WRITE setControl);
	Q_PROPERTY(QString label READ getLabel WRITE setLabel);

public:
	explicit CACheckBox(QWidget *parent = NULL);
	virtual ~CACheckBox();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;


	void initCheckBox();

	const QStringList getPVList() const;
	void setPVList(const QStringList pvlist);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const bool &getControl() const;
	void  setControl(const bool control);

	const QString &getLabel() const;
	void  setLabel(const QString &label);

	const bool getStatus();

public slots:
	virtual void changeValue (const int &value);
	void changePvName (const QString &pvname);
	void setStatus (const bool status);
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);
signals:
	void valueChanged (const bool status);
private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
