#include <QObject>
#include "MainWindow.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	if(argc != 2) 
	{
		qDebug("Usage:example [uifile_name]");
		exit(0);
	}

	QString uifile = argv[1];
	MainWindow mainwindow(uifile);
	mainwindow.show();
	return app.exec();
}
