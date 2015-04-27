#include <QtUiTools>
#include <QtGui>
#include "controlmonitorlib.h"

//Screen Shot Module
int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
#if 0
	if(argc!=2)
	{
		qDebug("Usage:ControlSystemMonitoring [Qt_ui file name]");
		exit(-1);
	}
	QString uifile = argv[1];
	static ControlMonitorChannel *attach = new ControlMonitorChannel(uifile,1);
#endif
	chdir("/usr/local/opi/bin");
	//static ControlMonitorChannel *attach = new ControlMonitorChannel("../ui/ControlSystemMonitoring_Test.ui",1);
	ControlMonitorChannel *attach = new ControlMonitorChannel("../ui/ControlSystemMonitoring.ui",1);
	QWidget *pwidget = attach->GetWidget();
	pwidget->show();
	return app.exec();
}
