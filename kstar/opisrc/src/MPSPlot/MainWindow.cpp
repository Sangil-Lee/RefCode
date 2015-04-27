#include <Qt>
#include "MainWindow.h"

MainWindow::MainWindow()
{
	init();
}

MainWindow::~MainWindow()
{
	qDebug("~MainWindow is called");
}

void MainWindow::init()
{
	chdir("/usr/local/opi/bin");
	makeUI();

	// createStatusBar()
	statusBar()->showMessage(tr("Ready"));
	attachWidget();
	setGeometry(0,0,1280,1024);
	setMaximumSize(1280,1024);
	setMinimumSize(1280,1024);

	createConnects();
	createActions();

}

void MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("MPSPlot", "MPSPlot", 0, QApplication::UnicodeUTF8));
}

void MainWindow::createActions()
{
	if(centralwidget == NULL) return;
}

void MainWindow::createConnects()
{
	if(centralwidget == NULL) return;

	mpWaveplot = centralwidget->findChild<CAMultiwaveplot *>("CAMultiwaveplot");
	if(mpWaveplot) 
	{
		connect(mpWaveplot, SIGNAL(changeIndexColor(int, QString)), this, SLOT(changeIndexColor(int, QString)));
	};

	for(int i = 0; i < 13; i++)
	{
		QString colorstr;
		switch(i)
		{
			case 0:
				colorstr = QColor(Qt::blue).name();
				break;
			case 1:
				colorstr = QColor(Qt::green).name();
				break;
			case 2:
				colorstr = QColor(Qt::red).name();
				break;
			case 3:
				colorstr = QColor(Qt::darkRed).name();
				break;
			case 4:
				colorstr = QColor(Qt::black).name();
				break;
			case 5:
				colorstr = QColor(Qt::magenta).name();
				break;
			case 6:
				colorstr = QColor(Qt::darkMagenta).name();
				break;
			case 7:
				colorstr = QColor(Qt::yellow).name();
				break;
			case 8:
				colorstr = QColor(Qt::white).name();
				break;
			case 9:
				colorstr = QColor(Qt::cyan).name();
				break;
			case 10:
				colorstr = QColor(Qt::darkYellow).name();
				break;
			case 11:
				colorstr = QColor(Qt::darkBlue).name();
				break;
			case 12:
				colorstr = QColor(Qt::darkCyan).name();
				break;
			default:
				break;
		};
		changeIndexColor(i, colorstr);
	}
#if 0
	QColor(Qt::blue).name();
	QColor(Qt::green).name();
	QColor(Qt::red).name();
	QColor(Qt::darkRed).name();
	QColor(Qt::black).name();
	QColor(Qt::magenta).name();
	QColor(Qt::darkMagenta).name();
	QColor(Qt::yellow).name();
	QColor(Qt::white).name();
	QColor(Qt::cyan).name();
	QColor(Qt::darkYellow).name();
	QColor(Qt::darkBlue).name();
	QColor(Qt::darkCyan).name();
	QColor(Qt::darkGray).name();
	QColor(Qt::darkGreen).name();
#endif

}

#if 1
void MainWindow::changeIndexColor(int index, QString colorname)
{
	QString indexLabel = QString("indexLabel_%1").arg(index);
	QLabel *pLabel = findChild<QLabel *> (indexLabel);
	//qDebug("Index:%d, Color:%s", index, colorname.toStdString().c_str());
	if(pLabel)
	{
		QPalette palette;
		QColor color(colorname);
		QBrush brush(color);
		brush.setStyle(Qt::SolidPattern);
		palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
		palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
		palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
		pLabel->setPalette(palette);

		//pLabel->setPalette(QPalette(QColor(colorname)));
		//pLabel->setAutoFillBackground(true);
		pLabel->repaint();
	};
}
#endif

void MainWindow::attachWidget()
{
	pattach = new AttachChannelAccess("../ui/MPSPlot.ui", 1);
	centralwidget = pattach->GetWidget();

	if(centralwidget == NULL) return;
	setCentralWidget(centralwidget);
}

void MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
