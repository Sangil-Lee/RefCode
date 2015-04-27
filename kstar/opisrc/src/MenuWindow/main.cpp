#include <QtGui>

#include "MenuWindow.h"

int checkProcess(const QString processname)
{
	QString checkproc = QString("/sbin/pidof ") + processname;
	FILE *fp = popen (checkproc.toStdString().c_str(), "r");

	char buf[1024];
	while (fgets (buf, sizeof(buf), fp) ) {};
	pclose(fp);


	char tmp[1024];
	strcpy(tmp, buf);
	char *tok;
	tok = strtok ( tmp, " " );
	int hostcnt = 0;
	while ( tok != NULL )
	{
		printf("[%d]:%s\n",hostcnt, tok);
		tok = strtok ( NULL, " " );
		hostcnt++;
	};

	return hostcnt;
}

int main(int argc, char *argv[])
{
#if 1
	if(argc >= 2 && strcmp(argv[1],"--version")==0) 
	{
		printf("MenuWindow version \"%s\"\n",MENUWINDOW_VERSION);
		printf("MenuWindow is the icon collection of the KSTAR Operator Interfaces.\n");
		return 1;
	}
#endif
    QApplication app(argc, argv);

	if (checkProcess("MenuWindow") > 1)
	{
		QMessageBox::information(0, "MenuWindow","MenuWindow was already created!");
		return -1;
	};

    if (!QSystemTrayIcon::isSystemTrayAvailable()) 
	{
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    MenuWindow window;
    window.show();

    return app.exec();
}
