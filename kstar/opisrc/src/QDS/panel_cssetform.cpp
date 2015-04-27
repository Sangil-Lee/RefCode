#include "panel_cssetform.h"
#include "ui_panel_cssetform.h"

panel_cssetform::panel_cssetform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_cssetform)
{
    ui->setupUi(this);

    cssetform_pattach = new AttachChannelAccess(this);
}

panel_cssetform::~panel_cssetform()
{
    delete ui;
}

void panel_cssetform::changeEvent(QEvent *e)
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

void panel_cssetform::on_pushButton_clicked()
{
      emit OnClickNewPv();
}
