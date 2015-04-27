#include <QtUiTools>
#include <QtGui>
#include "qtchaccesslib.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	AttachChannelAccess attach("/usr/local/opi/ui/TMS_Main_1360x768.ui",1);
	attach.SetUiCurIndex(1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
}
