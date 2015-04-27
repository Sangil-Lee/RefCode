#include <QtUiTools>
#include <QtGui>
#include "qtchaccesslib.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	AttachChannelAccess attach("/usr/local/opi/ui/VVS_NIDAQcontrol.ui",1);
	attach.SetUiCurIndex(1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
}
