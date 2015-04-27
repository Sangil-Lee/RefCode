#include <QObject>
#include "MainWindow.h"

char ui_name[256];

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("Usage: %s uifilename \n", argv[0]);
		exit (1);
	}

	strcpy(ui_name, argv[1]);

	QApplication app(argc, argv);

	MainWindow mainwindow;
	mainwindow.show();
	return app.exec();
}
