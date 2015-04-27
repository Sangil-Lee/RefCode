#include "panel_tfdispform.h"
#include "ui_panel_tfdispform.h"

panel_tfdispform::panel_tfdispform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_tfdispform)
{
    ui->setupUi(this);

    tfdispform_pattach = new AttachChannelAccess(this);

}

panel_tfdispform::~panel_tfdispform()
{
    delete ui;
}

void panel_tfdispform::changeEvent(QEvent *e)
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

void panel_tfdispform::on_pushButton_clicked()
{
     emit OnClickNewPv();
}
