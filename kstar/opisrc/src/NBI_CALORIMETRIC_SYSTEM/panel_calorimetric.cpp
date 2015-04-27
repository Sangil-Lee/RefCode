#include "panel_calorimetric.h"
#include "ui_panel_calorimetric.h"

panel_calorimetric::panel_calorimetric(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_calorimetric)
{
    ui->setupUi(this);
    calorimetric_attach = new AttachChannelAccess(this);    
    static_attach = new AttachChannelAccess(ui->stackedWidget);
    //tab1_attach = new AttachChannelAccess(ui->tabWidget);
    //tab2_attach = new AttachChannelAccess(ui->tabWidget_2);

	// 20130621 : To fix capturing screen of hidden page in partial
    ui->stackedWidget->setCurrentIndex(1);    

    ui->stackedWidget->setCurrentIndex(0);    
    ui->pushButton->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
    //ui->tabWidget->setCurrentIndex(0);
    //ui->tabWidget_2->setCurrentIndex(0);
    //ui->pushButton->setEnabled(false);

    //ui->tabWidget
}

panel_calorimetric::~panel_calorimetric()
{
    delete ui;
}

void panel_calorimetric::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void panel_calorimetric::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //ui->pushButton->setEnabled(false);
    //ui->pushButton_2->setEnabled(true);    
    ui->pushButton->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
    ui->pushButton_2->setStyleSheet("");
    ui->pushButton_3->setStyleSheet("");
}

void panel_calorimetric::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //ui->pushButton_2->setEnabled(false);
    //ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("");
    ui->pushButton_2->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
    ui->pushButton_3->setStyleSheet("");

}

void panel_calorimetric::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    //ui->pushButton_2->setEnabled(false);
    //ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("");
    ui->pushButton_2->setStyleSheet("");
    ui->pushButton_3->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");

}
