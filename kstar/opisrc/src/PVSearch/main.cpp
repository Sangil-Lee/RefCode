#include <QtGui>
#include <iostream>
#include <fstream>
#include <QSqlDatabase>
#include <QSqlQuery>
using namespace std;
int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	if ( argc != 2 )
	{
		qDebug("Usage:PVSearch pvname");
		exit(0);
	};
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("kstardb");
	if (!db.open()) 
	{
		QMessageBox::critical(0, qApp->tr("Cannot open database"),
				qApp->tr("Unable to establish a database connection.\n"
					"This example needs SQLite support. Please read "
					"the Qt SQL driver documentation for information how "
					"to build it.\n\n"
					"Click Cancel to exit."), QMessageBox::Cancel);
		return  -1;
	};

	QString sqlstr = QString("SELECT id,recordtype,pvname FROM kstarioc WHERE pvname LIKE '%%1%';").arg(argv[1]);
	qDebug("%s", sqlstr.toStdString().c_str() );
	QSqlQuery sqltest;
	sqltest.exec(sqlstr);
	while (sqltest.next()) 
	{
		QString id = sqltest.value(0).toString();
		QString recordtype = sqltest.value(1).toString();
		QString pvname = sqltest.value(2).toString();
		qDebug("Id:%s, recordytpe:%s, pvname: %s",id.toStdString().c_str(), recordtype.toStdString().c_str(), pvname.toStdString().c_str() );
	}
	db.close();
	return 0;
}
