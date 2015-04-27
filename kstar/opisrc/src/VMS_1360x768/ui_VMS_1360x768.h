/********************************************************************************
** Form generated from reading ui file 'VMS_1360x768.ui'
**
** Created: Tue Aug 11 13:34:02 2009
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_VMS_1360X768_H
#define UI_VMS_1360X768_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include "caDisplayer.h"
#include "caGraphic.h"
#include "caUitime.h"

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    CADisplayer *TPG262C0_PRS0_13;
    CADisplayer *TPG262C0_PRS0_4;
    CADisplayer *TPG262C0_PRS0_9;
    CAGraphic *caGraphic_3;
    CADisplayer *TPG262C0_PRS0_2;
    CAGraphic *caGraphic;
    CAGraphic *caGraphic_4;
    CADisplayer *TPG262C0_PRS0_3;
    CAUITime *CAUITime;
    CAGraphic *caGraphic_2;
    CADisplayer *TPG262C0_PRS0_11;
    CADisplayer *TPG262C0_PRS0;
    CADisplayer *TPG262C0_PRS0_12;
    CADisplayer *TPG262C0_PRS0_14;
    CADisplayer *TPG262C0_PRS0_10;
    CAGraphic *caGraphic_5;
    QLabel *label_2;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *label_13;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_7;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(1360, 768);
    MainWindow->setDockNestingEnabled(false);
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    label = new QLabel(centralwidget);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(0, 0, 1360, 746));
    label->setMinimumSize(QSize(10, 10));
    label->setMaximumSize(QSize(1360, 768));
    label->setPixmap(QPixmap(QString::fromUtf8(":/images/VMS_main.png")));
    label->setScaledContents(true);
    TPG262C0_PRS0_13 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_13->setObjectName(QString::fromUtf8("TPG262C0_PRS0_13"));
    TPG262C0_PRS0_13->setGeometry(QRect(302, 621, 400, 100));
    TPG262C0_PRS0_13->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_13->setMaximumSize(QSize(400, 100));
    QFont font;
    font.setPointSize(54);
    font.setBold(true);
    font.setWeight(75);
    TPG262C0_PRS0_13->setFont(font);
    TPG262C0_PRS0_13->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_13->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_13->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    TPG262C0_PRS0_4 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_4->setObjectName(QString::fromUtf8("TPG262C0_PRS0_4"));
    TPG262C0_PRS0_4->setGeometry(QRect(878, 136, 400, 100));
    TPG262C0_PRS0_4->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_4->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_4->setFont(font);
    TPG262C0_PRS0_4->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_4->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_4->setProperty("framecol", QVariant(QColor(18, 29, 57)));
    TPG262C0_PRS0_9 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_9->setObjectName(QString::fromUtf8("TPG262C0_PRS0_9"));
    TPG262C0_PRS0_9->setGeometry(QRect(302, 381, 400, 100));
    TPG262C0_PRS0_9->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_9->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_9->setFont(font);
    TPG262C0_PRS0_9->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_9->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_9->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    caGraphic_3 = new CAGraphic(centralwidget);
    caGraphic_3->setObjectName(QString::fromUtf8("caGraphic_3"));
    caGraphic_3->setGeometry(QRect(840, 392, 30, 30));
    caGraphic_3->setMaximumSize(QSize(30, 30));
    caGraphic_3->setLineColor(QColor(30, 47, 93));
    TPG262C0_PRS0_2 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_2->setObjectName(QString::fromUtf8("TPG262C0_PRS0_2"));
    TPG262C0_PRS0_2->setGeometry(QRect(878, 260, 400, 100));
    TPG262C0_PRS0_2->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_2->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_2->setFont(font);
    TPG262C0_PRS0_2->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_2->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_2->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    caGraphic = new CAGraphic(centralwidget);
    caGraphic->setObjectName(QString::fromUtf8("caGraphic"));
    caGraphic->setGeometry(QRect(840, 150, 30, 30));
    caGraphic->setMaximumSize(QSize(30, 30));
    caGraphic->setLineColor(QColor(30, 47, 93));
    caGraphic_4 = new CAGraphic(centralwidget);
    caGraphic_4->setObjectName(QString::fromUtf8("caGraphic_4"));
    caGraphic_4->setGeometry(QRect(840, 514, 30, 30));
    caGraphic_4->setMaximumSize(QSize(30, 30));
    caGraphic_4->setLineColor(QColor(30, 47, 93));
    TPG262C0_PRS0_3 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_3->setObjectName(QString::fromUtf8("TPG262C0_PRS0_3"));
    TPG262C0_PRS0_3->setGeometry(QRect(302, 136, 400, 100));
    TPG262C0_PRS0_3->setMinimumSize(QSize(10, 10));
    TPG262C0_PRS0_3->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_3->setFont(font);
    TPG262C0_PRS0_3->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_3->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_3->setProperty("framecol", QVariant(QColor(18, 29, 57)));
    CAUITime = new CAUITime(centralwidget);
    CAUITime->setObjectName(QString::fromUtf8("CAUITime"));
    CAUITime->setGeometry(QRect(32, 2, 20, 20));
    CAUITime->setProperty("timeset", QVariant(CAUITime::Event));
    CAUITime->setProperty("periodicsecond", QVariant(CAUITime::One));
    caGraphic_2 = new CAGraphic(centralwidget);
    caGraphic_2->setObjectName(QString::fromUtf8("caGraphic_2"));
    caGraphic_2->setGeometry(QRect(840, 272, 30, 30));
    caGraphic_2->setMaximumSize(QSize(30, 30));
    caGraphic_2->setLineColor(QColor(30, 47, 93));
    TPG262C0_PRS0_11 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_11->setObjectName(QString::fromUtf8("TPG262C0_PRS0_11"));
    TPG262C0_PRS0_11->setGeometry(QRect(878, 380, 400, 100));
    TPG262C0_PRS0_11->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_11->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_11->setFont(font);
    TPG262C0_PRS0_11->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_11->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_11->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    TPG262C0_PRS0 = new CADisplayer(centralwidget);
    TPG262C0_PRS0->setObjectName(QString::fromUtf8("TPG262C0_PRS0"));
    TPG262C0_PRS0->setGeometry(QRect(302, 259, 400, 100));
    TPG262C0_PRS0->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0->setFont(font);
    TPG262C0_PRS0->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    TPG262C0_PRS0_12 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_12->setObjectName(QString::fromUtf8("TPG262C0_PRS0_12"));
    TPG262C0_PRS0_12->setGeometry(QRect(878, 500, 400, 100));
    TPG262C0_PRS0_12->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_12->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_12->setFont(font);
    TPG262C0_PRS0_12->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_12->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_12->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    TPG262C0_PRS0_14 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_14->setObjectName(QString::fromUtf8("TPG262C0_PRS0_14"));
    TPG262C0_PRS0_14->setGeometry(QRect(878, 618, 400, 100));
    TPG262C0_PRS0_14->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_14->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_14->setFont(font);
    TPG262C0_PRS0_14->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_14->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_14->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    TPG262C0_PRS0_10 = new CADisplayer(centralwidget);
    TPG262C0_PRS0_10->setObjectName(QString::fromUtf8("TPG262C0_PRS0_10"));
    TPG262C0_PRS0_10->setGeometry(QRect(302, 500, 400, 100));
    TPG262C0_PRS0_10->setMinimumSize(QSize(400, 100));
    TPG262C0_PRS0_10->setMaximumSize(QSize(400, 100));
    TPG262C0_PRS0_10->setFont(font);
    TPG262C0_PRS0_10->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    TPG262C0_PRS0_10->setProperty("lineWidth", QVariant(8));
    TPG262C0_PRS0_10->setProperty("framecol", QVariant(QColor(30, 47, 93)));
    caGraphic_5 = new CAGraphic(centralwidget);
    caGraphic_5->setObjectName(QString::fromUtf8("caGraphic_5"));
    caGraphic_5->setGeometry(QRect(840, 626, 30, 30));
    caGraphic_5->setMaximumSize(QSize(30, 30));
    caGraphic_5->setLineColor(QColor(30, 47, 93));
    label_2 = new QLabel(centralwidget);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(810, 300, 60, 50));
    label_2->setMinimumSize(QSize(60, 0));
    label_2->setMaximumSize(QSize(60, 50));
    QPalette palette;
    QBrush brush(QColor(255, 255, 255, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    QBrush brush1(QColor(115, 115, 115, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_2->setPalette(palette);
    QFont font1;
    font1.setPointSize(32);
    label_2->setFont(font1);
    label_6 = new QLabel(centralwidget);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(810, 544, 60, 50));
    label_6->setMinimumSize(QSize(60, 0));
    label_6->setMaximumSize(QSize(60, 50));
    QPalette palette1;
    palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_6->setPalette(palette1);
    label_6->setFont(font1);
    label_5 = new QLabel(centralwidget);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(810, 418, 60, 50));
    label_5->setMinimumSize(QSize(60, 0));
    label_5->setMaximumSize(QSize(60, 50));
    QPalette palette2;
    palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_5->setPalette(palette2);
    label_5->setFont(font1);
    label_13 = new QLabel(centralwidget);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setGeometry(QRect(840, 180, 32, 50));
    label_13->setMinimumSize(QSize(32, 0));
    label_13->setMaximumSize(QSize(32, 50));
    QPalette palette3;
    palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_13->setPalette(palette3);
    label_13->setFont(font1);
    label_13->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
    label_11 = new QLabel(centralwidget);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setGeometry(QRect(798, 180, 28, 50));
    label_11->setMinimumSize(QSize(28, 0));
    label_11->setMaximumSize(QSize(28, 50));
    QPalette palette4;
    palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_11->setPalette(palette4);
    label_11->setFont(font1);
    label_11->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
    label_12 = new QLabel(centralwidget);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setGeometry(QRect(827, 182, 12, 50));
    label_12->setMinimumSize(QSize(12, 0));
    label_12->setMaximumSize(QSize(12, 50));
    QPalette palette5;
    palette5.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_12->setPalette(palette5);
    QFont font2;
    font2.setPointSize(20);
    label_12->setFont(font2);
    label_12->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
    label_7 = new QLabel(centralwidget);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setGeometry(QRect(810, 658, 60, 50));
    label_7->setMinimumSize(QSize(60, 0));
    label_7->setMaximumSize(QSize(60, 50));
    QPalette palette6;
    palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
    label_7->setPalette(palette6);
    label_7->setFont(font1);
    MainWindow->setCentralWidget(centralwidget);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);

    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    label->setText(QString());
    TPG262C0_PRS0_13->setPvname(QApplication::translate("MainWindow", "HDS_VPS_R_FG2", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_13->setUnit(QString());
    TPG262C0_PRS0_4->setPvname(QApplication::translate("MainWindow", "VMS_VV_RGA_M18", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_4->setUnit(QString());
    TPG262C0_PRS0_9->setPvname(QApplication::translate("MainWindow", "CLS_VPS_R_PRESSURE_FG2", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_9->setUnit(QString());
    caGraphic_3->setPvname(QApplication::translate("MainWindow", "CLB_TF_RGA_FL_ON", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_2->setPvname(QApplication::translate("MainWindow", "VMS_CR_RGA_M4", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_2->setUnit(QString());
    caGraphic->setPvname(QApplication::translate("MainWindow", "VMS_VV_RGA_FL_ON", 0, QApplication::UnicodeUTF8));
    caGraphic_4->setPvname(QApplication::translate("MainWindow", "CLB_PF_RGA_FL_ON", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_3->setPvname(QApplication::translate("MainWindow", "VMS_VV_TPG262C3_P0", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_3->setUnit(QString());
    caGraphic_2->setPvname(QApplication::translate("MainWindow", "VMS_CR_RGA_FL_ON", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_11->setPvname(QApplication::translate("MainWindow", "CLB_TF_RGA_M4", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_11->setUnit(QString());
    TPG262C0_PRS0->setPvname(QApplication::translate("MainWindow", "VMS_CR_TPG262C3_P0", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0->setUnit(QString());
    TPG262C0_PRS0_12->setPvname(QApplication::translate("MainWindow", "CLB_PF_RGA_M4", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_12->setUnit(QString());
    TPG262C0_PRS0_14->setPvname(QApplication::translate("MainWindow", "HDS_VPS_RGA_M4", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_14->setUnit(QString());
    TPG262C0_PRS0_10->setPvname(QApplication::translate("MainWindow", "CLS_VPS_R_PRESSURE_FG1", 0, QApplication::UnicodeUTF8));
    TPG262C0_PRS0_10->setUnit(QString());
    caGraphic_5->setPvname(QApplication::translate("MainWindow", "HDS_VPS_RGA_FL_ON", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("MainWindow", "He", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("MainWindow", "He", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("MainWindow", "He", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("MainWindow", "O", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("MainWindow", "H", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("MainWindow", "2", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("MainWindow", "He", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_VMS_1360X768_H
