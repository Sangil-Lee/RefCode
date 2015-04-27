#ifndef DDS_DESC_DIALOG_H
#define DDS_DESC_DIALOG_H

#include <QDialog>

#include <QMenuBar>
#include "ui_DDS_desc_dialog.h"


class ddsDescDlg : public QDialog, public Ui::ddsDescriptor
{
    Q_OBJECT

public:
    ddsDescDlg(QWidget *parent = 0);
	 ~ddsDescDlg();
	
	void InitWindow( void *);

	void *pm;

protected:

private slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();
	void filePrint();
	void myAccepted();



private:
	QMenuBar *menuBar;
	QMenu *fileMenu;
	
	QAction *actionOpen;
    QAction *actionPrint;
    QAction *actionExit;
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionSave_As;
    

	QString curFile;

	void createActions();
	void createMenus();

};

#endif

