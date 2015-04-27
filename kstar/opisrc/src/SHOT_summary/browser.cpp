/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "browser.h"
#include "qsqlconnectiondialog.h"

#include <QtGui>
#include <QtSql>

Browser::Browser(QWidget *parent)
    : QWidget(parent)
{
	//m_strquery = QString("select shotnum as 'no.', shotdate as 'time', tfcurrent as 'I_tf', plsmacurrent as 'I_p', edensity as 'd_e', etemp as 't_e', airpressure as 'P_air', echpow as 'W_ech', comment as 'Remark' from SHOTSUMMARY order by shotnum desc");
	qDebug("Browser!!!");
    setupUi(this);

    table->addAction(insertRowAction);
    table->addAction(deleteRowAction);

    if (QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));

    emit statusMessage(tr("Ready."));
}

Browser::~Browser()
{
}


#if 1
void Browser::exec()
{
	exec(m_strquery);
}

void Browser::exec(const QString &query)
{
	qDebug("exec(QString)");
    QSqlQueryModel *model = new QSqlQueryModel(table);
	QString sqlquery = query;
    //model->setQuery(QSqlQuery(sqlEdit->toPlainText(), connectionWidget->currentDatabase()));
    model->setQuery(QSqlQuery(sqlquery, connectionWidget->currentDatabase()));
    table->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
}
#endif

QSqlError Browser::addConnection(const QString &driver, const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }
    connectionWidget->refresh();

    return err;
}

void Browser::addConnection()
{
#if 0
    QSqlConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (dialog.useInMemoryDatabase()) {
        QSqlDatabase::database("in_mem_db", false).close();
        QSqlDatabase::removeDatabase("in_mem_db");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");
        db.setDatabaseName(":memory:");
        if (!db.open())
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occured while "
                                                                         "opening the connection: ") + db.lastError().text());
        QSqlQuery q("", db);
        q.exec("drop table Movies");
        q.exec("drop table Names");
        q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
        q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
        q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
        q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
        q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
        q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
        q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
        q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
        q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
        q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
        q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
        q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
        connectionWidget->refresh();
    } else {
	#if 0
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());
	#endif
    }
#endif

#if 0
        QSqlError err = addConnection("QMYSQL", "irmis", "kstar_exp.nfri.re.kr",
                           "irmisadmin", "kstar2004", -1 );
#endif
#if 1
        QSqlError err = addConnection("QMYSQL", "kstarweb", "kstar_exp.nfri.re.kr",
                           "root", "kstar2004", -1 );
#endif
#if 0
        QSqlError err = addConnection("QMYSQL", "kstarweb", "172.17.100.101",
                           "root", "kstar2004", -1 );
#endif
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occured while "
                                       "opening the connection: ") + err.text());
		// shbaek
		//exec();
}

void Browser::showTable(const QString &t)
{
	qDebug("showTable");
#if 1
    //QSqlTableModel *model = new QSqlTableModel(table, connectionWidget->currentDatabase());
    SqlQueryTableModel *model = new SqlQueryTableModel(table, connectionWidget->currentDatabase());
    model->setEditStrategy(SqlQueryTableModel::OnRowChange);
    model->setTable(t);
	model->setSort(0, Qt::DescendingOrder);
	model->setFilter("date(shotdate) = (SELECT date(max(shotdate)) FROM SHOTSUMMARY)");
	model->setHeaderData(0, Qt::Horizontal, tr("No"));
	model->setHeaderData(1, Qt::Horizontal, tr("Time"));
	model->setHeaderData(2, Qt::Horizontal, tr("I_tf (A)"));
	model->setHeaderData(3, Qt::Horizontal, tr("Ip (A)"));
	model->setHeaderData(4, Qt::Horizontal, tr("Ne (e19/m^2)"));
	model->setHeaderData(5, Qt::Horizontal, tr("Te (eV)"));
	model->setHeaderData(6, Qt::Horizontal, tr("Pressure (mbar)"));
	model->setHeaderData(7, Qt::Horizontal, tr("ECH_P (V)"));
	model->setHeaderData(8, Qt::Horizontal, tr("Remark"));
	model->setHeaderData(9, Qt::Horizontal, tr("Date"));
	//m_strquery = QString("SELECT shotnum, time_format(shotdate,'%H:\%i:\%s'), round(tfcurrent/1000,1), round(plsmacurrent/1000,1), round(edensity,1), round(etemp,1), airpressure, round(echpow*0.12*1000,1), comment, date(shotdate)  FROM SHOTSUMMARY");
	m_strquery = QString("SELECT shotnum, time_format(shotdate,'%H:\%i:\%s'), tfcurrent, plsmacurrent, edensity, etemp, airpressure, echpow, comment, shotdate  FROM SHOTSUMMARY");
    model->setQuery(QSqlQuery(m_strquery, connectionWidget->currentDatabase()));
    model->select();
	// protected
	//qDebug("Query: %s", model->selectStatement().toStdString().c_str());
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
#endif

    table->setModel(model);
	table->resizeColumnsToContents();
    table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void Browser::showMetaData(const QString &t)
{
    QSqlRecord rec = connectionWidget->currentDatabase().record(t);
    QStandardItemModel *model = new QStandardItemModel(table);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), fld.typeID() == -1
                ? QString(QVariant::typeToName(fld.type()))
                : QString("%1 (%2)").arg(QVariant::typeToName(fld.type())).arg(fld.typeID()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

    table->setModel(model);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateActions();
}

void Browser::insertRow()
{
    //QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    SqlQueryTableModel *model = qobject_cast<SqlQueryTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    table->edit(insertIndex);
}

void Browser::deleteRow()
{
    //QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    SqlQueryTableModel *model = qobject_cast<SqlQueryTableModel *>(table->model());
    if (!model)
        return;

    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setEditStrategy(SqlQueryTableModel::OnManualSubmit);

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }

    model->submitAll();
    //model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setEditStrategy(SqlQueryTableModel::OnRowChange);

    updateActions();
}

void Browser::updateActions()
{
    //bool enableIns = qobject_cast<QSqlTableModel *>(table->model());
    bool enableIns = qobject_cast<SqlQueryTableModel *>(table->model());
    bool enableDel = enableIns && table->currentIndex().isValid();

    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
}

void Browser::about()
{
    QMessageBox::about(this, tr("About"), tr("The SQL Browser demonstration "
        "show how a data browser can be used to visualize the results of SQL"
        "statements on a live database"));
}
