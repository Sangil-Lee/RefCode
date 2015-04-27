#include "SinglePlotWindow.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
	if (argc != 2 )
	{
		qDebug("Wrong Argument!");
		exit(0);
	};
	qDebug("PVName:%s", argv[1]);
    SinglePlotWindow mainWindow(argv[1]);

    //mainWindow.resize(800,400);
    mainWindow.show();

	int status = a.exec();
    return status; 
}
