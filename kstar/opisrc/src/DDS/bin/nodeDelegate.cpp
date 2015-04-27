/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "nodeDelegate.h"


NodeDelegate::NodeDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *NodeDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & /* option */,
                                     const QModelIndex &index) const
{
	if( index.column() == 2 ) {
		QComboBox *comboBox = new QComboBox(parent);
		comboBox->addItem(tr("-0.1 ~ 0.1"));
		comboBox->addItem(tr("-1 ~ 1"));
		comboBox->addItem(tr("-5 ~ 5"));
		comboBox->addItem(tr("-10 ~ 10"));

		connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
		return comboBox;
	} 
	else if( index.column() == 3 ) {
		QComboBox *comboBox = new QComboBox(parent);	
		comboBox->addItem(tr("200"));
		comboBox->addItem(tr("100"));
		comboBox->addItem(tr("50"));

		connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
		return comboBox;
	}
	else {
		QLineEdit *editor = new QLineEdit(parent);		
		connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
		return editor;
	}
	return NULL;
}

void NodeDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	if(  index.column() == 2 || index.column() == 3  ) {
		QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
		if (!comboBox)
			return;
		int pos = comboBox->findText(index.model()->data(index).toString(),
									 Qt::MatchExactly);
		comboBox->setCurrentIndex(pos);		
	} else {
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		if( !edit) return;
		edit->setText(index.model()->data(index, Qt::EditRole).toString());

	}
}

void NodeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	if( index.column() == 2 || index.column() == 3 ) {
		QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
		if (!comboBox)
			return;

		model->setData(index, comboBox->currentText());

	} else {
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		if (edit) {
			model->setData(index, edit->text());
		} else {
//			QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
//			if (dateEditor) {
//				model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
//			}
		}		
	}
}

void NodeDelegate::emitCommitData()
{
    emit commitData(qobject_cast<QWidget *>(sender()));
}


void NodeDelegate::commitAndCloseEditor()
 {
     QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
     emit commitData(editor);
     emit closeEditor(editor);
 }
