/********************************************************************************
** Form generated from reading ui file 'SCS_Cool_Down.ui'
**
** Created: Thu Apr 17 01:51:47 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SCS_COOL_DOWN_H
#define UI_SCS_COOL_DOWN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "caUitime.h"

class Ui_Form
{
public:
    CAUITime *CAUITime;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QWidget *Form)
    {
    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));
    Form->resize(1080, 800);
    CAUITime = new CAUITime(Form);
    CAUITime->setObjectName(QString::fromUtf8("CAUITime"));
    CAUITime->setGeometry(QRect(29, 14, 20, 20));
    groupBox = new QGroupBox(Form);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(128, 120, 102, 132));
    vboxLayout = new QVBoxLayout(groupBox);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    pushButton = new QPushButton(groupBox);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    vboxLayout->addWidget(pushButton);

    pushButton_2 = new QPushButton(groupBox);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

    vboxLayout->addWidget(pushButton_2);

    pushButton_3 = new QPushButton(groupBox);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

    vboxLayout->addWidget(pushButton_3);


    retranslateUi(Form);

    QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
    Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("Form", "GroupBox", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("Form", "PushButton", 0, QApplication::UnicodeUTF8));
    pushButton_2->setText(QApplication::translate("Form", "PushButton", 0, QApplication::UnicodeUTF8));
    pushButton_3->setText(QApplication::translate("Form", "PushButton", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Form);
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

#endif // UI_SCS_COOL_DOWN_H
