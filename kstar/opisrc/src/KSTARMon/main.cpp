#include <QObject>
#include "KSTARMon.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
#if 0
	if(argc != 2) 
	{
		qDebug("Usage:example [uifile_name]");
		exit(0);
	}

	QString uifile = argv[1];
	MainWindow mainwindow(uifile);
	mainwindow.show();
#endif
	chdir("/usr/local/opi/bin");
	QWidget *pwidget = new QWidget;
	KSTARMON_UI::KSTARMon form(pwidget);
	return app.exec();
}
