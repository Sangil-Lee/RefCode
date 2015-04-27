//#include <QtUiTools>
//#include <QtGui>
//#include "qtchaccesslib.h"
#include <QObject>
#include "MainWindow.h"

int main (int argc, char *argv[])
{
/*
	QApplication app(argc, argv);
	AttachChannelAccess attach("/usr/local/opi/ui/RMP.ui",1);
	attach.SetUiCurIndex(1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
*/
	 QApplication app(argc, argv);
     MainWindow mainwindow;
     mainwindow.show();
     return app.exec();
}
