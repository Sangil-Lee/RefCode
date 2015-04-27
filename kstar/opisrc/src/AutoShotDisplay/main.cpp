#include <QtUiTools>
#include <QtGui>
#include "qtchaccesslib.h"

//Screen Shot Module
int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	if(argc!=2)
	{
		qDebug("Usage:AutoShotDisplay [Qt_ui file name]");
		exit(-1);
	}
	QString uifile = argv[1];
	AttachChannelAccess attach(uifile,1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
}
