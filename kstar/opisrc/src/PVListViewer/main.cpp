#include "pvlistviewer.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
	
	PVListViewer *plist = new PVListViewer();
    return a.exec(); 
}
