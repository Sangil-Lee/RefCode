#include "panel_pfsetform.h"
#include "ui_panel_pfsetform.h"

panel_pfsetform::panel_pfsetform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_pfsetform)
{
    ui->setupUi(this);

    pfsetform_pattach = new AttachChannelAccess(this);
}

panel_pfsetform::~panel_pfsetform()
{
    delete ui;
}

void panel_pfsetform::changeEvent(QEvent *e)
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

void panel_pfsetform::on_pushButton_clicked()
{
      emit OnClickNewPv();
}
