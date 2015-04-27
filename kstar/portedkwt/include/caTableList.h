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

/* caTableList.h
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 * Revision 1  2013-05
 * Author: silee
 * Initial revision
 */

#ifndef CA_TABLELIST_H
#define CA_TABLELIST_H

#include <QtGui>
#include <QTableWidget>
#include <cadef.h>
#include <dbDefs.h>

class CATableList : public QTableWidget
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	//Q_PROPERTY(bool control READ getControl WRITE setControl);
	//Q_PROPERTY(int row READ getRow WRITE setRow);
	//Q_PROPERTY(QStringList onoffpvlist READ getOnOffPVList WRITE setOnOffPVList);
	//Q_PROPERTY(QStringList tagpvlist READ getTagPVList WRITE setTagPVList);

public:
	explicit CATableList(QWidget *parent = NULL);
	virtual ~CATableList();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	//const bool &getControl() const;
	//void  setControl(const bool control);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	void init();

	//int getRow() const;
	//void setRow(const int row );
	//const QStringList getOnOffPVList() const;
	//void  setOnOffPVList(const QStringList &onoffpvlist);
	//const QStringList getTagPVList() const;
	//void  setTagPVList(const QStringList &tagpvlist);

	//void InsertHash(const QString pvname, QTableWidgetItem *pItem);
	void InsertHash(QTableWidgetItem *pItem, QString pvname);
	void TableUpdate();
	//QTableWidgetItem * FindItem(QString pvname);
	QString FindProcessVariable(QTableWidgetItem *pItem);

public slots:
	void regSignal();
	virtual void changeValue (const int row, const int col);
	void SLOTTableUpdate();

signals:
	void valueChanged (const bool status);
	void SIGTableUpdate();

private:
	class PrivateData;
	PrivateData *d_data;
	//typedef QHash<QString, QTableWidgetItem *> Hash_Table;
	typedef QHash<QTableWidgetItem *, QString> Hash_Table;
	Hash_Table hashWidgetTable;

	void	changeOnOffValue(const int row, const int col = 0);
	void	changeTagNameValue(const int row, const int col = 1);
	int		caPut(const QString pvname, const QString &value);
};
#endif
