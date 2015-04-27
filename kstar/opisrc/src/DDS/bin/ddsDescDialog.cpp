//#include <QtGui>

#include "ddsDescDialog.h"

#include "mainWindow.h"


ddsDescDlg::ddsDescDlg(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	pm = NULL;

	curFile = QFileInfo("test.txt").canonicalFilePath();

}

ddsDescDlg::~ddsDescDlg()
{

}

void ddsDescDlg::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;

//	menuBar = new QMenuBar;
//	fileMenu = new QMenu(tr("&File"), this);
//	exitAction = fileMenu->addAction(tr("E&xit"));
//	menuBar->addMenu(fileMenu);

	createActions();
	createMenus();

	this->gridLayout->setMenuBar(menuBar);
	setLayout(this->gridLayout);

//	connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));

	connect(this, SIGNAL(accepted()), this, SLOT( myAccepted() ));
}



void ddsDescDlg::createActions()
{
	actionNew = new QAction(tr("&New"), this);
	actionNew->setIcon(QIcon(":/images/icon/new.png") );
	actionNew->setShortcut(tr("Ctrl+N"));
	actionNew->setStatusTip(tr("Create a new set"));
	connect(actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));

	actionOpen = new QAction(tr("&Open..."), this);
	actionOpen->setIcon(QIcon(":/images/icon/open.png") );
	actionOpen->setShortcut(tr("Ctrl+O"));
	actionOpen->setStatusTip(tr("Open an existing file"));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

	actionSave = new QAction(tr("&Save"), this);
	actionSave->setIcon(QIcon(":/images/icon/save.png") );
	actionSave->setShortcut(tr("Ctrl+S"));
	actionSave->setStatusTip(tr("Save current file"));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));

	actionSave_As = new QAction(tr("Save &As..."), this);
	actionSave_As->setStatusTip(tr("Save current file as ..."));
	connect(actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

	actionPrint = new QAction(tr("&Print..."), this);
	actionPrint->setIcon(QIcon(":/images/icon/print.png") );
	actionPrint->setShortcut(tr("Ctrl+P"));
	actionPrint->setStatusTip(tr("Print the current data"));
	connect(actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

	actionExit = new QAction(tr("E&xit"), this);
	actionExit->setShortcut(tr("Ctrl+Q"));
	actionExit->setStatusTip(tr("Exit the dialog"));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(accept()));


}

void ddsDescDlg::createMenus()
{
	menuBar = new QMenuBar;
	fileMenu = menuBar->addMenu(tr("&File"));
	fileMenu->addAction(actionNew);
	fileMenu->addAction(actionOpen);
	fileMenu->addAction(actionSave);
	fileMenu->addAction(actionSave_As);
	fileMenu->addAction(actionPrint);
	fileMenu->addSeparator();
	fileMenu->addAction(actionExit);

}

void ddsDescDlg::filePrint()
{
	QTextDocument *document = textEdit->document();
	QPrinter printer;

	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	document->print(&printer);
}

void ddsDescDlg::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this);

	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) {
			QMessageBox::warning(this, tr("SDI"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
			return;
		}

		QTextStream in(&file);
		QApplication::setOverrideCursor(Qt::WaitCursor);
		textEdit->setPlainText(in.readAll());
		QApplication::restoreOverrideCursor();

//		setCurrentFile(fileName);
		curFile = QFileInfo(fileName).canonicalFilePath();
	}
	

}


void ddsDescDlg::fileNew()
{
	textEdit->clear();
	
}

void ddsDescDlg::fileSave()
{
	 QFile file(curFile);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("SDI"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(curFile)
                              .arg(file.errorString()));
         return;
     }

     QTextStream out(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     out << textEdit->toPlainText();
     QApplication::restoreOverrideCursor();
	
}

void ddsDescDlg::fileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile.toAscii().constData());
	if (fileName.isEmpty())
		return ;
	
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("SDI"),
							tr("Cannot write file %1:\n%2.")
							.arg(fileName)
							.arg(file.errorString()));
		return ;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << textEdit->toPlainText();
	QApplication::restoreOverrideCursor();

	curFile = QFileInfo(fileName).canonicalFilePath();
	
}

void ddsDescDlg::myAccepted()
{

//	pmain->m6802Config.trigger = Edit_M6802_trigger1->text().toInt();
//	pmain->m6802Config.period = Edit_M6802_period1->text().toInt();

//	pmain->acq196Config.trigger = Edit_ACQ196_trigger1->text().toInt();
//	pmain->acq196Config.period = Edit_ACQ196_period1->text().toInt();

}

