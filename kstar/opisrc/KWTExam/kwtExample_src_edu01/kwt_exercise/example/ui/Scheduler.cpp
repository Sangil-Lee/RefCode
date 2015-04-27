/********************************************************************************
** Form generated from reading ui file 'Scheduler.ui'
**
** Created: Mon Feb 7 20:36:16 2011
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SCHEDULER_H
#define UI_SCHEDULER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTimeEdit>

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QListWidget *LW_Schedule;
    QDateTimeEdit *DTE_Datetime;
    QLineEdit *LE_PVname;
    QLineEdit *LE_REValue;
    QRadioButton *RB_Datetime;
    QRadioButton *RB_Day;
    QTimeEdit *TE_Time;
    QComboBox *CB_Daily;
    QRadioButton *RB_Secs;
    QSpinBox *SB_Secs;
    QPushButton *PB_Add;
    QPushButton *PB_Delete;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QDialog *Dialog)
    {
    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QString::fromUtf8("Dialog"));
    Dialog->setWindowModality(Qt::WindowModal);
    Dialog->resize(476, 343);
    buttonBox = new QDialogButtonBox(Dialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(110, 310, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
    LW_Schedule = new QListWidget(Dialog);
    LW_Schedule->setObjectName(QString::fromUtf8("LW_Schedule"));
    LW_Schedule->setGeometry(QRect(0, 120, 461, 181));
    DTE_Datetime = new QDateTimeEdit(Dialog);
    DTE_Datetime->setObjectName(QString::fromUtf8("DTE_Datetime"));
    DTE_Datetime->setGeometry(QRect(310, 10, 151, 27));
    LE_PVname = new QLineEdit(Dialog);
    LE_PVname->setObjectName(QString::fromUtf8("LE_PVname"));
    LE_PVname->setGeometry(QRect(80, 10, 113, 27));
    LE_REValue = new QLineEdit(Dialog);
    LE_REValue->setObjectName(QString::fromUtf8("LE_REValue"));
    LE_REValue->setGeometry(QRect(80, 50, 113, 27));
    RB_Datetime = new QRadioButton(Dialog);
    RB_Datetime->setObjectName(QString::fromUtf8("RB_Datetime"));
    RB_Datetime->setGeometry(QRect(210, 10, 90, 23));
    RB_Day = new QRadioButton(Dialog);
    RB_Day->setObjectName(QString::fromUtf8("RB_Day"));
    RB_Day->setGeometry(QRect(210, 40, 90, 23));
    TE_Time = new QTimeEdit(Dialog);
    TE_Time->setObjectName(QString::fromUtf8("TE_Time"));
    TE_Time->setGeometry(QRect(390, 40, 71, 27));
    CB_Daily = new QComboBox(Dialog);
    CB_Daily->setObjectName(QString::fromUtf8("CB_Daily"));
    CB_Daily->setGeometry(QRect(310, 40, 71, 29));
    RB_Secs = new QRadioButton(Dialog);
    RB_Secs->setObjectName(QString::fromUtf8("RB_Secs"));
    RB_Secs->setGeometry(QRect(210, 70, 90, 23));
    SB_Secs = new QSpinBox(Dialog);
    SB_Secs->setObjectName(QString::fromUtf8("SB_Secs"));
    SB_Secs->setGeometry(QRect(310, 70, 61, 27));
    SB_Secs->setMinimum(1);
    SB_Secs->setMaximum(86400);
    PB_Add = new QPushButton(Dialog);
    PB_Add->setObjectName(QString::fromUtf8("PB_Add"));
    PB_Add->setGeometry(QRect(20, 90, 80, 27));
    PB_Delete = new QPushButton(Dialog);
    PB_Delete->setObjectName(QString::fromUtf8("PB_Delete"));
    PB_Delete->setGeometry(QRect(110, 90, 80, 27));
    label = new QLabel(Dialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(5, 14, 54, 18));
    label_2 = new QLabel(Dialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(4, 49, 54, 36));
    label_2->setAlignment(Qt::AlignCenter);

    retranslateUi(Dialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
    DTE_Datetime->setDisplayFormat(QApplication::translate("Dialog", "yyyy/MM/dd hh-mm-ss", 0, QApplication::UnicodeUTF8));
    RB_Datetime->setText(QApplication::translate("Dialog", "Datetime", 0, QApplication::UnicodeUTF8));
    RB_Day->setText(QApplication::translate("Dialog", "Day", 0, QApplication::UnicodeUTF8));
    TE_Time->setDisplayFormat(QApplication::translate("Dialog", "HH:mm:ss", 0, QApplication::UnicodeUTF8));
    CB_Daily->clear();
    CB_Daily->insertItems(0, QStringList()
     << QApplication::translate("Dialog", "Everyday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Monday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Tuesday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Wednesday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Thursday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Friday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Saturday", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Dialog", "Sunday", 0, QApplication::UnicodeUTF8)
    );
    RB_Secs->setText(QApplication::translate("Dialog", "Every secs", 0, QApplication::UnicodeUTF8));
    PB_Add->setText(QApplication::translate("Dialog", "Add", 0, QApplication::UnicodeUTF8));
    PB_Delete->setText(QApplication::translate("Dialog", "Delete", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("Dialog", "PV Name", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("Dialog", "Reserve \n"
" Value", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Dialog);
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

#endif // UI_SCHEDULER_H
