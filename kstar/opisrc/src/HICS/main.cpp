#include <unistd.h>

#include <QObject>
#include "MainWindow.h"

char ui_name[256] = "../ui/HICS_ECH_PWR.ui";

int main (int argc, char *argv[])
{
	if (argc > 1) {
		strcpy(ui_name, argv[1]);
	}
	else {
		if (0 != access(ui_name, F_OK)) {
			strcpy (ui_name, "/usr/local/opi/ui/HICS_ECH_PWR.ui");
		}
	}

	QApplication app(argc, argv);

	MainWindow mainwindow;
	mainwindow.show();
	return app.exec();
}
