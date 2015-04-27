#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    main_pattach = NULL;

    if(main_pattach==NULL){
        main_pattach= new AttachChannelAccess(this);
        if(main_pattach)
            main_widget = main_pattach->GetWidget();
    }
    static_pattach = new AttachChannelAccess(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(0);

    ui->pushButton->setEnabled(false);
    //ui->dockWidget->setWidget(up_widget);
    //main_widget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void getWavefrom (char *name, char *szPVNames)
{
	if (!name || !szPVNames) {
		return;
	}

	chid    mychid;
	char	pvList[50][40];
	unsigned int elementCount;

	ca_context_create (ca_disable_preemptive_callback);

	ca_create_channel (name, NULL, NULL, 0, &mychid);		//ca_search (name, &mychid);
	ca_pend_io (0.5);

	elementCount = ca_element_count (mychid);

	if (mychid) {
		ca_array_get (DBR_STRING, elementCount, mychid, pvList);
		ca_pend_io (0.5);

		ca_clear_channel (mychid);
		ca_pend_io (0.5);
	}

	ca_context_destroy ();

	szPVNames[0] = 0x00;

	for (unsigned int i = 0; i < elementCount; i++) {
		if (strlen(pvList[i]) > 0) {
			strcat (szPVNames, pvList[i]);
			strcat (szPVNames, "\n");
		}
	}
}

void displayCalorimetricAlarm (char *title, char *pvname)
{
	char	buf[1024];
	getWavefrom (pvname, buf);

#if 0
	char	str[256]; 
	sprintf (str, "<h2>%s</h2>", buf);
	QString html = str;

	QMessageBox msgBox;
	msgBox.setWindowTitle (title);
	msgBox.setText (html);
	msgBox.setTextFormat (Qt::RichText);
	msgBox.setStandardButtons (QMessageBox::Ok);
	msgBox.exec();
#else
	QMessageBox::about (0, title, buf);
#endif
}

void MainWindow::on_btn_calAlmIS1_clicked()
{
	displayCalorimetricAlarm ("IS1", "NB1_LODAQ_ALM_IS1_LIST");
}

void MainWindow::on_btn_calAlmIS2_clicked()
{
	displayCalorimetricAlarm ("IS2", "NB1_LODAQ_ALM_IS2_LIST");
}

void MainWindow::on_btn_calAlmCommon_clicked()
{
	displayCalorimetricAlarm ("Common", "NB1_LODAQ_ALM_COMMON_LIST");
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    kill(getpid(), SIGTERM);
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);

}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}
