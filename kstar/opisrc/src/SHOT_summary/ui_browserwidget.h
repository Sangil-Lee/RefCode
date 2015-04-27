/********************************************************************************
** Form generated from reading ui file 'browserwidget.ui'
**
** Created: Thu Jun 12 11:33:55 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BROWSERWIDGET_H
#define UI_BROWSERWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QSplitter>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "connectionwidget.h"

class Ui_Browser
{
public:
    QAction *insertRowAction;
    QAction *deleteRowAction;
    QVBoxLayout *vboxLayout;
    QSplitter *splitter_2;
    ConnectionWidget *connectionWidget;
    QTableView *table;

    void setupUi(QWidget *Browser)
    {
    if (Browser->objectName().isEmpty())
        Browser->setObjectName(QString::fromUtf8("Browser"));
    Browser->resize(557, 547);
    insertRowAction = new QAction(Browser);
    insertRowAction->setObjectName(QString::fromUtf8("insertRowAction"));
    insertRowAction->setEnabled(false);
    deleteRowAction = new QAction(Browser);
    deleteRowAction->setObjectName(QString::fromUtf8("deleteRowAction"));
    deleteRowAction->setEnabled(false);
    vboxLayout = new QVBoxLayout(Browser);
    vboxLayout->setSpacing(6);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(8, 8, 8, 8);
    splitter_2 = new QSplitter(Browser);
    splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
    splitter_2->setSizePolicy(sizePolicy);
    splitter_2->setOrientation(Qt::Horizontal);
    connectionWidget = new ConnectionWidget(splitter_2);
    connectionWidget->setObjectName(QString::fromUtf8("connectionWidget"));
    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(connectionWidget->sizePolicy().hasHeightForWidth());
    connectionWidget->setSizePolicy(sizePolicy1);
    connectionWidget->setMaximumSize(QSize(200, 16777215));
    splitter_2->addWidget(connectionWidget);
    table = new QTableView(splitter_2);
    table->setObjectName(QString::fromUtf8("table"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy2.setHorizontalStretch(2);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(table->sizePolicy().hasHeightForWidth());
    table->setSizePolicy(sizePolicy2);
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);
    table->setFont(font);
    table->setContextMenuPolicy(Qt::ActionsContextMenu);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setAutoScroll(true);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setGridStyle(Qt::DotLine);
    splitter_2->addWidget(table);

    vboxLayout->addWidget(splitter_2);

    QWidget::setTabOrder(connectionWidget, table);

    retranslateUi(Browser);

    QMetaObject::connectSlotsByName(Browser);
    } // setupUi

    void retranslateUi(QWidget *Browser)
    {
    Browser->setWindowTitle(QApplication::translate("Browser", "Qt SQL Browser", 0, QApplication::UnicodeUTF8));
    insertRowAction->setText(QApplication::translate("Browser", "&Insert Row", 0, QApplication::UnicodeUTF8));
    insertRowAction->setStatusTip(QApplication::translate("Browser", "Inserts a new Row", 0, QApplication::UnicodeUTF8));
    deleteRowAction->setText(QApplication::translate("Browser", "&Delete Row", 0, QApplication::UnicodeUTF8));
    deleteRowAction->setStatusTip(QApplication::translate("Browser", "Deletes the current Row", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Browser);
    } // retranslateUi

};

namespace Ui {
    class Browser: public Ui_Browser {};
} // namespace Ui

#endif // UI_BROWSERWIDGET_H
