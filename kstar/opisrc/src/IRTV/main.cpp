#include <QtGui/QApplication>
#include "mainWindow.h"

#define	IRTV_VERSION	"1.0"

int main(int argc, char *argv[])
{
	if (argc >= 2 && strcmp(argv[1],"--version")==0) {
		printf("IRTV version \"%s\"\n", IRTV_VERSION);
		printf("IRTV is the Operator Interface for the KSTAR. \n");
		return 1;
	}

	QApplication::setStyle(new QWindowsStyle);

    QApplication app(argc, argv);
    MainWindow w;

    w.setWindowTitle("IRTV Control System - KSTAR");
    w.show();

    return app.exec();
}
