#include <QtUiTools>
#include <QtGui>
#include "qtchaccesslib.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	AttachChannelAccess attach("CALabelTest.ui",1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
}
