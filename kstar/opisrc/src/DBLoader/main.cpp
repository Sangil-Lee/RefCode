#include <iostream>
#include <fstream>
#include <QtGui>
#include <QSqlDatabase>
#include <QSqlQuery>
using namespace std;
int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	if ( argc != 2 )
	{
		qDebug("Usage:DBLoader [DB filename]");
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

	ifstream file(argv[1]);
	qDebug("argc:%d, argv[1]:%s", argc, argv[1]);
	if ( file.fail () ) 
	{
		qDebug("DB filename not found");
		return -1;
	};
	//QSqlQuery query("create table kstarioc (id int primary key, "
			//"recordtype varchar(10), pvname varchar(30), description varchar(60) )");
	QSqlQuery query;
	query.exec("create table kstarioc (id int primary key, "
			"recordtype varchar(10), pvname varchar(30) )");

	string strToken;
	vector<string> pvnames;
	int line = 0;			/*line=id, first pch=recordtype, second pch=pvname, third pch = description*/
	QString strquery;
	while (!file.eof ())
	{
		getline (file, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		QString strRecordtype = pch;

		int pvnamesize = strlen(pch); 
		char pvname[pvnamesize];
		strncpy(pvname, pch, pvnamesize-1);

		int index = 0;
		QString strPvname, strDesc;
		while (pch != NULL)
		{
			index = 0;
			if(!(pch = strtok (NULL," \t,"))) continue;
			if (index == 0) 
			{
				strPvname = pch;
				qDebug("%s", pch);
			}
			else if (index == 1) 
			{
				strDesc = pch;
				qDebug("%s", pch);
			};
			index++;
		};
		line++;
		QString strLine = QString("%1").arg(line, 0);
		strquery = QString("insert into kstarioc values('%1', '%2', '%3')").arg(strLine, strRecordtype,strPvname);
		qDebug("%s", strquery.toStdString().c_str() );
		query.exec(strquery);
	};
	file.close ();

	bool isseek;
	QString sqlstr = QString("SELECT id,recordtype,pvname FROM kstarioc WHERE pvname='TFC16_MHL_TCU_R02';");
	QSqlQuery sqltest;
	//sqltest.exec("SELECT id, recordtype, pvname FROM kstarioc WHERE pvname='TFC16_MHL_TCU_R02';");
	sqltest.exec(sqlstr);
#if 1
	while (sqltest.next()) 
	{
		QString id = sqltest.value(0).toString();
		QString recordtype = sqltest.value(1).toString();
		QString pvname = sqltest.value(2).toString();
		qDebug("Id:%s, recordytpe:%s, pvname:%s",id.toStdString().c_str(), recordtype.toStdString().c_str(), pvname.toStdString().c_str() );
	}
	return 0;
#else
	int i = 0;
	while (sqltest.next()) 
	{
		QString id = sqltest.value(0).toString();
		QString recordtype = sqltest.value(1).toString();
		QString pvname = sqltest.value(0).toString();
		qDebug("Id:%s, recordytpe:%s, pvname:%s",id.toStdString().c_str(), recordtype.toStdString().c_str(), pvname.toStdString().c_str() );
		i++;
	}
#endif
}
