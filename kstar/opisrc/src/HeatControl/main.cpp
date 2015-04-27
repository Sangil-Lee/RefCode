#include <QObject>
#include "MainWindow.h"

int main (int argc, char *argv[])
{
	QString usage = "USAGE:HeatingPermission or HeatingPermission <CPO:Physics Operator or HO:Heating Operator>";

	char hostname[64];
	hostname[0] = '\0';;

	gethostname(hostname,sizeof(hostname));

	QString qhostname = hostname;

#if 0
	if(argc != 2) 
	{
		qDebug("%s",usage.toStdString().c_str());
		exit(-1);
	};
#endif

	QString optype;

	if(argc == 1)
		optype = "";
	else if(argc == 2)
		optype = argv[1];
	else
	{
		qDebug("%s",usage.toStdString().c_str());
		exit(-1);
	};

	if(optype.isEmpty() == false)
	{
		if(optype.compare("CPO", Qt::CaseInsensitive) != 0 && optype.compare("HO", Qt::CaseInsensitive) != 0 )
		{
			qDebug("%s",usage.toStdString().c_str());
			exit(-1);
		};
#if 1
		if(optype.compare("CPO", Qt::CaseInsensitive) == 0 && qhostname.compare("opi05",Qt::CaseInsensitive) != 0)
		{
			qDebug("%s","HeatingPermission CPO can be used in PCS1(opi05)");
			exit(-1);
		}
#endif
	};


	QApplication app(argc, argv);
	MainWindow mainwindow(optype);

	mainwindow.show();
	return app.exec();
}
