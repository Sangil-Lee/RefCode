#include <QtUiTools>
#include <QtGui>
#include <iostream>
#include "qtchaccesslib.h"

using namespace std;

int main (int argc, char *argv[])
{
	//printf("\'%c\' C\n", 128);

#if 0
	int ascii = 167;
	for(int i = 0; i < 256;i++)
	{
	//QChar c(i);
	//qDebug("%c", c.toAscii());
	qDebug("%c", i);
	};
#endif

	QApplication app(argc, argv);
	AttachChannelAccess attach("/usr/local/opi/ui/GCDS_DAQ.ui",1);
	attach.SetUiCurIndex(1);
	QWidget *pwidget = attach.GetWidget();
	pwidget->show();
	return app.exec();
}
