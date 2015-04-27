#include <QObject>
//#include "MainWindow.h"
#include "ECCD_Main.h"
//#include "qtchaccesslib.h

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
//	MainWindow mainwindow;
	ECCD_Main mainwindow;
	mainwindow.show();
	return app.exec();
}
