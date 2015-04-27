/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QObject>
#include "MainWindow.h"
#include "qtchaccesslib.h"

int main(int argc, char *argv[])
{
#if 0
	if(argc >= 2 && strcmp(argv[1],"--version")==0)
    {
	    printf("VMS version \"%s\"\n",VMS_VERSION);
	    printf("VMS is the Oerator Interface \nfor the KSTAR Vacuum Pumping System \nand Current Lead System.\n");
		return 1;
    }
#else
	int c = 0;
	int ver = 0;
	char *capture = 0;
	while ((c = getopt (argc, argv, "vc:")) !=  -1)
	{
		switch (c) 
		{
			case  'v':
				ver = 1;
				break;
			case  'c':
				capture = optarg;
				break;
			case  '?':
			default :
				printf("USAGE: VMS -v[version] -c[capture] [Y(Yes) or N(No)]\n");
				break;
		};
	};

	if(ver == 1)
	{
	    printf("VMS release %s\n",VMS_VERSION);
	    printf("VMS is the Operator Interface for the KSTAR Vacuum Pumping System and Current Lead System.\n");
		exit(1);
	};

	bool bcap = false;
	if(capture != 0)
	{
		if ( strcasecmp(capture, "Y") == 0 || strcasecmp(capture, "Yes") == 0 )
		{
			bcap = true;
			printf("Start the VMS screen capture every 30seconds.\n");
		};
	};
#endif
    QApplication app(argc, argv);
    MainWindow mainwindow(bcap);

    mainwindow.show();
    return app.exec();
}

