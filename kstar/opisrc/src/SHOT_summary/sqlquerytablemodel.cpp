/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
** 
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "sqlquerytablemodel.h"

//SqlQueryTableModel::SqlQueryTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase())
SqlQueryTableModel::SqlQueryTableModel(QObject *parent, QSqlDatabase db)
	:QSqlTableModel (parent, db)
{
}

SqlQueryTableModel::~SqlQueryTableModel()
{
}

void SqlQueryTableModel::setQuery(const QSqlQuery &query)
{
	QSqlTableModel::setQuery(query);
}


#if 0
enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

    virtual ~SqlQueryTableModel();

    virtual bool select();

    virtual void setTable(const QString &tableName);
    QString tableName() const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool isDirty(const QModelIndex &index) const;
    void clear();

    virtual void setEditStrategy(EditStrategy strategy);
    EditStrategy editStrategy() const;

    QSqlIndex primaryKey() const;
    QSqlDatabase database() const;
    int fieldIndex(const QString &fieldName) const;

    void sort(int column, Qt::SortOrder order);
    virtual void setSort(int column, Qt::SortOrder order);

    QString filter() const;
    virtual void setFilter(const QString &filter);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRecord(int row, const QSqlRecord &record);
    bool setRecord(int row, const QSqlRecord &record);

    virtual void revertRow(int row);

public Q_SLOTS:
    bool submit();
    void revert();

    bool submitAll();
    void revertAll();

Q_SIGNALS:
    void primeInsert(int row, QSqlRecord &record);

    void beforeInsert(QSqlRecord &record);
    void beforeUpdate(int row, QSqlRecord &record);
    void beforeDelete(int row);

protected:
    //SqlQueryTableModel(SqlQueryTableModelPrivate &dd, QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    virtual bool updateRowInTable(int row, const QSqlRecord &values);
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool deleteRowFromTable(int row);
    virtual QString orderByClause() const;
    virtual QString selectStatement() const;

    void setPrimaryKey(const QSqlIndex &key);
    void setQuery(const QSqlQuery &query);
    QModelIndex indexInQuery(const QModelIndex &item) const;
};

#endif
