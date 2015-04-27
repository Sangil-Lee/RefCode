#include <vector>
#include <string>
#include "MultiplotMainWindow.h"

using std::vector;
using std::string;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    MultiplotMainWindow mainWindow;
    mainWindow.resize(800,600);
	int status = a.exec();
    return status; 
}
