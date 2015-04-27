#include "ArchiveSheet.h"

int main(int argc, char** argv) 
{
    Q_INIT_RESOURCE(archivesheet);
    QApplication app(argc, argv);
    //ArchiveSheet sheet(100000, 21);
    ArchiveSheet sheet(2010, 21);
    sheet.setWindowIcon(QPixmap(":/images/interview.png"));
    sheet.resize(1280, 768);
    sheet.show();
    return app.exec();
}
