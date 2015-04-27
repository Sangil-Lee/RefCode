/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

//
#include "ArchiveSheet.h"
#include "ArchiveThreadsController.h"

using namespace std;
static QString encode_pos(int row, int col) 
{
    return QString(col + 'A') + QString::number(row + 1);
}

static void decode_pos(const QString &pos, int *row, int *col)
{
    if (pos.isEmpty()) 
	{
        *col = -1;
        *row = -1;
    } 
	else 
	{
        *col = pos.at(0).toLatin1() - 'A';
        *row = pos.right(pos.size() - 1).toInt() - 1;
    }
}

#if 0
static bool epicsTime2stringsec (const epicsTime &time, QString &txt)
{
    char buffer[30];
    struct local_tm_nano_sec tm = (local_tm_nano_sec) time;
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            tm.ansi_tm.tm_mon + 1,
            tm.ansi_tm.tm_mday,
            tm.ansi_tm.tm_year + 1900,
            tm.ansi_tm.tm_hour,
            tm.ansi_tm.tm_min,
            tm.ansi_tm.tm_sec);
    txt = buffer;
    return true;
}
static bool viewer(void *arg, const char *name, size_t , size_t i, const CtrlInfo &info,
             DbrType type, DbrCount count, const RawValue::Data *value)
{
    //QTableWidget *tableView = (QTableWidget*)arg;
	//tableView -> clear();
	//QString		time; 
	//epicsTime2stringsec(value->stamp, time);
	static int row = 0, col = 0;
	ArchiveSheet *pSheet = (ArchiveSheet*)arg;
    QTableWidget *tableView = pSheet->getTableView();

	//info.show(stdout);
	if(pSheet -> getRowCol()== true)
	{
		row = col = 0;
	};
	stdString	text;
	char timebuf[30];
	epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &(value->stamp));
	RawValue::getValueString(text, type, count, value, &info, RawValue::DECIMAL, 3 ); //NumberFormat format, int prec

    if (i==0)
    {
		row = 0;
		if ( col == 0 )
		{
			tableView -> setHorizontalHeaderItem(0, new QTableWidgetItem("Archived Date"));
			tableView -> setHorizontalHeaderItem(1, new QTableWidgetItem(name));
		}
		else
		{
			tableView -> setHorizontalHeaderItem(col + 1, new QTableWidgetItem(name));
		}
		col++;
    };

	if (col == 1)
	{
		tableView -> setItem(row, 0, new ArchiveSheetItem(timebuf) );
		tableView -> setItem(row, 1, new ArchiveSheetItem(text.c_str()) );
	}
	else
	{
		tableView -> setItem(row, col, new ArchiveSheetItem(text.c_str()) );
	};

	if ( ++row == ArchiveSheet::MAX_COUNT )
	{
		pSheet -> setEpicsTimeStamp(value->stamp);
		qDebug("row: %d", row);
	}
	//qDebug("time: %s, value: %s", time.toStdString().c_str(), text.c_str() );
	//RawValue::getTime(value, time);
	//RawValue::getValueString(text, type, count, value, &info, RawValue::DECIMAL, 3 ); //NumberFormat format, int prec
    //RawValue::show(stdout, type, count, value, &info);
	pSheet->setRowCol(false);
    return true;
}
#endif

ArchiveSheetDelegate::ArchiveSheetDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *ArchiveSheetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
    const QModelIndex &index) const
{
    if(index.column() == 1)
	{
        QDateTimeEdit *editor = new QDateTimeEdit(parent);
        editor->setDisplayFormat("dd/M/yyyy");
        editor->setCalendarPopup(true);
        return editor;
    };

    QLineEdit *editor = new QLineEdit(parent);
    //create a completer with the strings in the column as model.
    QStringList allStrings;
    for(int i = 1; i<index.model()->rowCount(); i++)
	{
        QString strItem(index.model()->data(index.sibling(i, index.column()), Qt::EditRole).toString());
        if(!allStrings.contains(strItem))
            allStrings.append(strItem);
    };
    QCompleter *autoComplete = new QCompleter(allStrings);
    editor->setCompleter(autoComplete);
    connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
    return editor;
}

void ArchiveSheetDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void ArchiveSheetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) 
	{
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
    }
    else 
	{
        QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
        if (dateEditor) 
		{
            dateEditor->setDate(QDate::fromString(index.model()->data(index, Qt::EditRole).toString(), "d/M/yyyy"));
        }
    };
}

void ArchiveSheetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) 
	{
        model->setData(index, edit->text());
    }
    else 
	{
        QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
        if (dateEditor) 
		{
            model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
        }
    };
}


ArchiveSheetItem::ArchiveSheetItem() : QTableWidgetItem(), isResolving(false) {}

ArchiveSheetItem::ArchiveSheetItem(const QString &text) : QTableWidgetItem(text), isResolving(false) {}

QTableWidgetItem *ArchiveSheetItem::clone() const
{
    ArchiveSheetItem *item = new ArchiveSheetItem();
    *item = *this;
    return item;
}

QVariant ArchiveSheetItem::data(int role) const
{
    if (role == Qt::EditRole || role == Qt::StatusTipRole) return formula();

    if (role == Qt::DisplayRole) return display();

    QString t = display().toString();
    bool isNumber = false;
    int number = t.toInt(&isNumber);

    if (role == Qt::TextColorRole) 
	{
        if (!isNumber) return qVariantFromValue(QColor(Qt::black));
        else if (number < 0) return qVariantFromValue(QColor(Qt::red));
        return qVariantFromValue(QColor(Qt::blue));
    }

    if (role == Qt::TextAlignmentRole)
        if (!t.isEmpty() && (t.at(0).isNumber() || t.at(0) == '-'))
            return (int)(Qt::AlignRight | Qt::AlignVCenter);

    return QTableWidgetItem::data(role);
}

void ArchiveSheetItem::setData(int role, const QVariant &value)
{
    QTableWidgetItem::setData(role, value);
    if (tableWidget())
        tableWidget()->viewport()->update();
}

QVariant ArchiveSheetItem::display() const
{
    // avoid circular dependencies
    if (isResolving)
        return QVariant();
    isResolving = true;
    QVariant result = computeFormula(formula(), tableWidget(), this);
    isResolving = false;
    return result;
}

QVariant ArchiveSheetItem::computeFormula(const QString &formula, const QTableWidget *widget,
                                         const QTableWidgetItem *self)
{
    // check if the string is actually a formula or not
    QStringList list = formula.split(' ');
    if (list.isEmpty() || !widget)
        return formula; // its a normal string

    QString op = list.value(0).toLower();

    int firstRow = -1;
    int firstCol = -1;
    int secondRow = -1;
    int secondCol = -1;

    if (list.count() > 1)
        decode_pos(list.value(1), &firstRow, &firstCol);
    if (list.count() > 2)
        decode_pos(list.value(2), &secondRow, &secondCol);

    const QTableWidgetItem *start = widget->item(firstRow, firstCol);
    const QTableWidgetItem *end = widget->item(secondRow, secondCol);

    int firstVal = start ? start->text().toInt() : 0;
    int secondVal = end ? end->text().toInt() : 0;

    QVariant result;
    if (op == "sum") {
        int sum = 0;
        for (int r = firstRow; r <= secondRow; ++r) {
            for (int c = firstCol; c <= secondCol; ++c) {
                const QTableWidgetItem *tableItem = widget->item(r, c);
                if (tableItem && tableItem != self)
                    sum += tableItem->text().toInt();
            }
        }
        result = sum;
    } else if (op == "+") {
        result = (firstVal + secondVal);
    } else if (op == "-") {
        result = (firstVal - secondVal);
    } else if (op == "*") {
        result = (firstVal * secondVal);
    } else if (op == "/") {
        if (secondVal == 0)
            result = QString("nan");
        else
            result = (firstVal / secondVal);
    } else if (op == "=") {
        if (start)
        result = start->text();
    } else {
        result = formula;
    }

    return result;
};

ArchiveSheet::ArchiveSheet(int rows, int cols, QWidget *parent) : QMainWindow(parent)
{
    addToolBar(archiveInfoBar = new QToolBar());
    addToolBar(toolBar = new QToolBar());
	isInitPage = false;
	m_setinterval = 1;
	mfilepath="";

   //formulaInput = new QLineEdit();
    //cellLabel = new QLabel(toolBar);
    //cellLabel->setMinimumSize(80, 0);
    //toolBar->addWidget(cellLabel);
    //toolBar->addWidget(formulaInput);

	QLabel *label = new QLabel();
	label -> setText("Archive Group: ");
	archiveInfoBar->addWidget(label);
	m_arch_combo = new QComboBox();
	archiveInfoBar->addWidget(m_arch_combo);

	QLabel *label1 = new QLabel();
	label1 -> setText("Page No: ");
	archiveInfoBar->addWidget(label1);
	ar_page = new QComboBox();
	archiveInfoBar->addWidget(ar_page);

	archiveInfoBar->addSeparator();

	prevButton = new QPushButton();
	prevButton->setText("Prev"); 
	archiveInfoBar->addWidget(prevButton);

	nextButton = new QPushButton();
	nextButton->setText("Next"); 
	archiveInfoBar->addWidget(nextButton);

	archiveInfoBar->addSeparator();

	QLabel *label3 = new QLabel();
	label3 -> setText(" Current Page Time: ");
	archiveInfoBar->addWidget(label3);
	curstartTime = new QLabel();
	curstartTime -> setText("00/00/0000 00:00:00");
	archiveInfoBar->addWidget(curstartTime);
	QLabel *spacer2 = new QLabel();
	spacer2 -> setText(" -- ");
	archiveInfoBar->addWidget(spacer2);
	curendTime = new QLabel();
	curendTime -> setText("00/00/0000 00:00:00");
	archiveInfoBar->addWidget(curendTime);

	archiveInfoBar->addSeparator();

	QLabel *label2 = new QLabel();
	label2 -> setText(" Start Time: ");
	archiveInfoBar->addWidget(label2);
	startTime = new QLabel();
	startTime -> setText("00/00/0000 00:00:00");
	archiveInfoBar->addWidget(startTime);
	QLabel *label4 = new QLabel();
	label4 -> setText(" End Time: ");
	archiveInfoBar->addWidget(label4);
	endTime = new QLabel();
	endTime -> setText("00/00/0000 00:00:00");
	archiveInfoBar->addWidget(endTime);
	setRowCol(true);
    table = new QTableWidget(rows, cols, this);
    for (int c = 0; c < cols; ++c) 
	{
        QString character(QChar('A' + c));
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    };
    table->setItemPrototype(table->item(rows - 1, cols - 1));
    //table->setItemDelegate(new ArchiveSheetDelegate());
    createActions();
    updateColor(0);
    setupMenuBar();
    setupContextMenu();
    setupContents();
    setCentralWidget(table);
    statusBar();
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(updateStatus(QTableWidgetItem*)));
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(updateColor(QTableWidgetItem*)));
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(updateLineEdit(QTableWidgetItem*)));
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateStatus(QTableWidgetItem*)));
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateLineEdit(QTableWidgetItem*)));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(updateNextPage()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(updatePrevPage()));
    connect(this, SIGNAL(updateSignal()), this, SLOT(updatePage()));
    //connect(ar_page, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePages(int)));
    connect(ar_page, SIGNAL(activated(int)), this, SLOT(updatePages(int)));
    connect(this, SIGNAL(nextPageSignal(int)), this, SLOT(updatePages(int)));
    connect(this, SIGNAL(prevPageSignal(int)), this, SLOT(updatePages(int)));
	m_pagetime = 0.0;
    setWindowTitle(tr("Archivesheet"));

	//qhashVal.reserve(2000);
	//Test();
	//TestPrint();
}
	
ArchiveSheet::~ArchiveSheet()
{
	qDebug("Destruct ArchvieSheet");
	//delete pthrcontroller;
}
bool 
ArchiveSheet::connectToArchiver( const QString& url )
{
    m_arclient.setURL(url.toStdString().c_str());
    stdVector<ArchiveDataClient::ArchiveInfo> archives;
	bool connstatus = m_arclient.getArchives(archives);
    if (connstatus)
    {
        printf("Available Archives:\n");
        for (size_t i=0; i<archives.size(); ++i)
		{
            printf("Key %d: '%s' (%s)\n", archives[i].key, archives[i].name.c_str(),
                   archives[i].path.c_str());
			arch_list << archives[i].name.c_str();
		};
		m_arch_combo -> addItems(arch_list);
    };
	return connstatus;
#if 0
    int                     version;
    stdString               description;
    stdVector<stdString>    hows, stats;
    stdVector<ArchiveDataClient::SeverityInfo> sevrs;

    if (m_arclient.getInfo(version, description, hows, stats, sevrs))
    {
        printf("Version %d: %s", version, description.c_str());
		printf("Request Types:\n");
		for (i=0; i<hows.size(); ++i)
		{
			printf("%3zd: %s\n", i, hows[i].c_str());
		};
		printf("Status Strings:\n");
		for (i=0; i<stats.size(); ++i)
		{
			printf("%3zd: %s\n", i, stats[i].c_str());
		};
		printf("Severity Info:\n");
		for (i=0; i<sevrs.size(); ++i)
		{
			printf("0x%04X: %-22s %-13s %s\n", sevrs[i].num, sevrs[i].text.c_str(),
					(sevrs[i].has_value ? "use value," : "ignore value,"),
					(sevrs[i].txt_stat  ? "use stat" : "stat is count"));
		};
    };
	epicsTime start, end;
	string2epicsTime("08/01/2006 00:00:00", start);
	string2epicsTime("08/03/2006 00:00:00", end);
    // ------------------------------------------------------
    stdVector<ArchiveDataClient::NameInfo> name_infos;
    stdString start_txt, end_txt;
    if (m_arclient.getNames(3, "TFC_MHL_TCU_R", name_infos))
    {
        printf("Names:\n");
        for (i=0; i<name_infos.size(); ++i)
            printf("'%s': %s - %s\n",
                   name_infos[i].name.c_str(),
                   epicsTimeTxt(name_infos[i].start, start_txt),
                   epicsTimeTxt(name_infos[i].end, end_txt));
    }
    // ------------------------------------------------------
#endif
}

//void ArchiveSheet::setEpicTimes(const epicsTimeStamp &eptime)
void ArchiveSheet::setEpicTimes(QString &eptimestr)
{
	qvecEptime.push_back(eptimestr);
}

void ArchiveSheet::setPvNames(const QString &pvname)
{
	QVector<double> qtempvec;
	qhashVal.insert(pvname, qtempvec);
}

void ArchiveSheet::addPvValues(const QString &pvname, double &pvvalue)
{
	QHash<QString, QVector<double> >::iterator i = qhashVal.find(pvname);
	if (i!=qhashVal.end() )
	{
		i.value().push_back(pvvalue);
	};
}

void ArchiveSheet::Test()
{
	QString pvname = "TMS_001";
	double pvval = 10.123;

	setPvNames(pvname);
	for ( int i = 0; i < 10; i++ )
	{
		pvval += i;
		addPvValues(pvname, pvval);
	}

	pvname = "TMS_002";
	pvval = 100.123;

	setPvNames(pvname);
	for ( int i = 0; i < 10; i++ )
	{
		pvval += i;
		addPvValues(pvname, pvval);
	};
}
void ArchiveSheet::TestPrint()
{
	QHashIterator<QString, QVector<double> > i(qhashVal);
	while (i.hasNext()) 
	{
		i.next();
		QString pvname = i.key();
		qDebug("pvname:%s", pvname.toStdString().c_str());
		for ( int j = 0; j < i.value().size();++j)
		{
			qDebug("value:%f", i.value().at(j));
		}
	}

	for ( int k = 0; k < qvecEptime.size();++k)
	{
		qDebug("date:%s", qvecEptime.at(k).toStdString().c_str());
	}
}

void ArchiveSheet::updateDisplay()
{
	emit updateSignal();
}

void ArchiveSheet::updatePage()
{
	QHashIterator<QString, QVector<double> > i(qhashVal);
	table->setHorizontalHeaderItem(0, new QTableWidgetItem("Archived Date"));
	QString valtext;
	int row = 0, col = 0;
	while (i.hasNext()) 
	{
		++col;
		i.next();
		QString pvname = i.key();
		qDebug("pvname:%s", pvname.toStdString().c_str());
		try
		{
			table->setHorizontalHeaderItem(col, new QTableWidgetItem(pvname));
			for ( int j = 0; j < i.value().size();++j)
			{
				if ( col == 1 )
				{
					table->setItem(j, 0, new QTableWidgetItem(qvecEptime.at(j)) );
				}
				valtext = QString("%1").arg(i.value().at(j));
				table->setItem(j, col, new QTableWidgetItem(valtext) );
			}
			++row;
		}catch(exception &e)
		{
			qDebug("Exception[updatePage]:%s", e.what());
		};
	};

	if ( isInitPage == false )
	{
		isInitPage = true;
		epicsTime start, end, curend;
		string2epicsTime(strStartTime.toStdString().c_str(), start);
		string2epicsTime(strEndTime.toStdString().c_str(), end);
		string2epicsTime(qvecEptime.last().toStdString().c_str(), curend);

		struct epicsTimeStamp epicstart_ts = (epicsTimeStamp)start;
		struct epicsTimeStamp epicend_ts = (epicsTimeStamp)end;
		struct epicsTimeStamp epiccur = (epicsTimeStamp)curend;

		double totaltime = epicsTimeDiffInSeconds( &epicend_ts, &epicstart_ts );
		m_pagetime = epicsTimeDiffInSeconds( &epiccur, &epicstart_ts );
		int totpage = ((int)totaltime%(int)m_pagetime)?(int)(totaltime/m_pagetime)+1:(int)(totaltime/m_pagetime);

		QString strpage;
		for ( int i = 0; i < totpage; i++)
		{
			strpage=QString("%1").arg(i+1);
			ar_page->addItem(strpage);
		};
		curstartTime->setText(strStartTime);
		epicend_ts.secPastEpoch = epicstart_ts.secPastEpoch+(epicsUInt32)m_pagetime;
		epicsTime curepicstamp = epicsTime(epicend_ts);
		struct epicsTimeStamp curendtime;
		curendtime = (epicsTimeStamp)curepicstamp;
		char timebuf[30];
		epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &curendtime);
		curendTime->setText(timebuf);
	};

	statusBar()->showMessage("");
}
void ArchiveSheet::clearTable()
{
	table->clear();
	table->setFocus(Qt::ActiveWindowFocusReason);
	qhashVal.clear();
	qvecEptime.clear();
}

bool ArchiveSheet::getPVNameList( const int archkey, const QString& pvname )
{
	name_infos.clear();
	pvlistView->clear();
    bool connstatus = m_arclient.getNames(archkey, pvname.toStdString().c_str(), name_infos);
    stdString start_txt, end_txt;
	QStringList pvlist;
	if ( connstatus )
	{
        for (size_t i=0; i<name_infos.size(); ++i)
		{
			pvlist << name_infos[i].name.c_str();
		};
		pvlistView -> addItems(pvlist);
	};
	return connstatus;
}

void ArchiveSheet::createActions()
{
    cell_sumAction = new QAction(tr("Sum"), this);
    connect(cell_sumAction, SIGNAL(triggered()), this, SLOT(actionSum()));

    cell_addAction = new QAction(tr("&Add"), this);
    cell_addAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
    connect(cell_addAction, SIGNAL(triggered()), this, SLOT(actionAdd()));

    cell_subAction = new QAction(tr("&Subtract"), this);
    cell_subAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
    connect(cell_subAction, SIGNAL(triggered()), this, SLOT(actionSubtract()));

    cell_mulAction = new QAction(tr("&Multiply"), this);
    cell_mulAction->setShortcut(Qt::CTRL | Qt::Key_multiply);
    connect(cell_mulAction, SIGNAL(triggered()), this, SLOT(actionMultiply()));

    cell_divAction = new QAction(tr("&Divide"), this);
    cell_divAction->setShortcut(Qt::CTRL | Qt::Key_division);
    connect(cell_divAction, SIGNAL(triggered()), this, SLOT(actionDivide()));

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(Qt::CTRL|Qt::Key_F);
    connect(fontAction, SIGNAL(triggered()), this, SLOT(selectFont()));

    colorAction = new QAction(QPixmap(16, 16), tr("Background &Color..."), this);
    connect(colorAction, SIGNAL(triggered()), this, SLOT(selectColor()));

    view_Action = new QAction(tr("&Plotting..."), this);
    view_Action->setShortcut(Qt::CTRL | Qt::Key_V);
    connect(view_Action, SIGNAL(triggered()), this, SLOT(viewPlot()));

    clearAction = new QAction(tr("Clear"), this);
    clearAction->setShortcut(Qt::Key_Delete);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

    aboutArchiveSheet = new QAction(tr("About Archivesheet"), this);
    connect(aboutArchiveSheet, SIGNAL(triggered()), this, SLOT(showAbout()));

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connAction = new QAction(tr("C&onnect..."), this);
    connect(connAction, SIGNAL(triggered()), this, SLOT(connWindow()));

    disconnAction = new QAction(tr("&Disconnect"), this);
    connect(disconnAction, SIGNAL(triggered()), this, SLOT(disconnWindow()));

    setupAction = new QAction(tr("S&etup..."), this);
    connect(setupAction, SIGNAL(triggered()), this, SLOT(setupWindow()));


    openAction = new QAction(tr("O&pen..."), this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openWindow()));

    saveAction = new QAction(tr("S&ave..."), this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveWindow()));

    appendAction = new QAction(tr("A&ppend"), this);
    connect(appendAction, SIGNAL(triggered()), this, SLOT(appendFile()));

    saveasAction = new QAction(tr("S&ave as..."), this);
    connect(saveasAction, SIGNAL(triggered()), this, SLOT(saveasWindow()));

    firstSeparator = new QAction(this);
    firstSeparator->setSeparator(true);

    secondSeparator = new QAction(this);
    secondSeparator->setSeparator(true);
}

void ArchiveSheet::openWindow()
{
	//QMessageBox::information(this, "Open File", "Open Window will be called");
	QString filename;
    QStringList files = QFileDialog::getOpenFileNames( this, tr("OpenFile"), filename,
                                tr("All Files (*);;Text Files (*.txt)"));
    if (files.count()) 
	{
		//Open File
        filename = files[0];
		openFile(filename);
    };
}
void
ArchiveSheet::openFile(QString &filename)
{
#if 0
	FileHandle file;

	try {
		file.open (filename.toStdString().c_str());
	}
	catch (...) {
		return;
	}

	file.setTokenDelimiter ('\t');
	char name   [FileHandle::MAX_LINE_SIZE];
	char comment[FileHandle::MAX_LINE_SIZE];
	try 
	{
		while(1) 
		{
			if (file.getToken (name, comment) < 0) 
			{
				printf ("end of file\n");
				break;
			}
			printf ("%s:%s\n", name, comment);
		}
	}
	catch (...) 
	{
		return;
	};
#if 1
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QTextStream in(&file);
	int type =0;
	QString line;
	if (!in.atEnd()) line = in.readLine();
	while (!in.atEnd()) 
	{
		line = in.readLine();
		int tokensize = 0;
		for ( int i = 0; i < line.size(); i++ )
		{
			if ( line.at(i) == '\t' )
			{
				tokensize = 0;
			};
			tokensize++;
		};
	};
	file.close();
	string teststr ="AVG,CET01,CET02,3.5,CET03,CET04";
	string getstr;
	fileToken( teststr, getstr, ',');
#endif
#else
	ifstream file(filename.toStdString().c_str());
	string strToken;
	if ( file.fail () ) return;

	int col = 0;
	getline (file, strToken);
	char str[strToken.size()];
	char * pch;
	strcpy ( str, strToken.c_str());
	pch = strtok (str,"\t");
	while (pch != NULL)
	{
		table->setHorizontalHeaderItem(col, new QTableWidgetItem(pch));
		pch = strtok (NULL, "\t");
		col++;
	}


	int row = 0;
	col = 0;
	QString strcontents;
    epicsTime eptime;
	while (!file.eof ())
	{
		getline (file, strToken, '\t');
		if (string2epicsTime(strToken.c_str(), eptime) == false ) 
		{
			row++;
			col = 0;
		};
		strcontents = QString::fromStdString(strToken);
		table->setItem(row, col, new ArchiveSheetItem(strcontents));
		col++;
	};
	file.close ();
#endif
}
void ArchiveSheet::fileToken( string &strtoken, string &token, const char delim )
{
#if 0
	for(size_t i = 0; i < strtoken.size();i++)
	{
		string::size_type idx = strtoken.find (delim) ;
		if (idx == string::npos) 
		{
			token = strtoken;
			qDebug("%s", token.c_str() );
		}
		else if ( idx != string::npos )
		{
			token = strtoken.substr(0, idx);
			qDebug("%s", token.c_str() );
			strtoken = strtoken.substr(idx+1);
			fileToken(strtoken, token, delim);
		};
	};
#else
	string::size_type idx = strtoken.find (delim) ;
	if ( idx != string::npos )
	{
		token = strtoken.substr(0, idx);
		qDebug("%s", token.c_str() );
		strtoken = strtoken.substr(idx+1);
		fileToken(strtoken, token, delim);
	}
	else 
	{
		token = strtoken;
		qDebug("%s", token.c_str() );
	}
#endif
}
void ArchiveSheet::saveWindow()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("SaveFile"),"",
                                tr("All Files (*);;Text Files (*.txt)"));
	if(!fileName.isEmpty())
	{
		//file save
		saveFile(fileName);
	};
}
void ArchiveSheet::appendFile()
{
	if(mfilepath.isEmpty())	
	{
		QMessageBox::information(this, "Append File", "No Saved File!");
		return;
	};

	QFile file(mfilepath);
	file.open(QIODevice::Append|QIODevice::Text);
	QTableWidgetItem *pitem = 0;
	int len = 0;
	file.write("\n", 1);
	int row = 0, col =0;
	for(row=0; row < table->rowCount();row++)
	{
		for (col=0; col < table->columnCount();col++)
		{
			pitem = table->item(row, col); 
			if ( pitem == NULL || pitem->text().compare("") == true) continue;
			len = qstrlen(pitem->text().toStdString().c_str());
			file.write(pitem->text().toStdString().c_str(), len);
			file.write("\t", 1);
		};
		pitem = table->item(row, col); 
		if ( pitem == NULL || pitem->text().compare("") == true) continue;
		file.write("\n", 1);
	};
	file.close();
}
void
ArchiveSheet::saveFile(const QString &filename)
{
#if 0
	FILE		*file;
	mode_t		oldMask;
	oldMask	= umask (0);

	if ( NULL == (file = fopen (filename.toStdString().c_str(), "w")) ) 
	{
		umask (oldMask);
		fclose(file);
		return;
	}
	char *test = "File Write Test";
	size_t len = strlen(test);
	fwrite (test, 1, len, file);
	fclose (file);
#else
	QFile file(filename);
	file.open(QIODevice::WriteOnly|QIODevice::Text);
	//FILE *fp = fopen(filename.toStdString().c_str(), "w");
	//file.open(fp, QIODevice::WriteOnly);
	mfilepath=filename;
	QTableWidgetItem *pitem = 0;
	int len = 0;
	for (int i = 0; i<table->columnCount();i++)
	{
		pitem = table->horizontalHeaderItem(i); 
		if ( pitem == NULL || pitem->text().compare("") == true) continue;
		len = qstrlen(pitem->text().toStdString().c_str());
		if ( len <= 1 ) continue;
		file.write(pitem->text().toStdString().c_str(), len);
		file.write("\t", 1);
	};
	file.write("\n", 1);
	int row = 0, col =0;
	for(row=0; row < table->rowCount();row++)
	{
		for (col=0; col < table->columnCount();col++)
		{
			pitem = table->item(row, col); 
			if ( pitem == NULL || pitem->text().compare("") == true) continue;
			len = qstrlen(pitem->text().toStdString().c_str());
			file.write(pitem->text().toStdString().c_str(), len);
			file.write("\t", 1);
		};
		pitem = table->item(row, col); 
		if ( pitem == NULL || pitem->text().compare("") == true) continue;
		file.write("\n", 1);
	};
	//fclse(fp);
	file.close();
#endif
}
void ArchiveSheet::saveasWindow()
{
	QMessageBox::information(this, "SaveAs File", "SaveAs Window will be called");
}
void ArchiveSheet::showPVList()
{
	//QMessageBox::information(this, "SaveAs File", "SaveAs Window will be called");
	QString pvname = pvNameLine -> text();
	getPVNameList( 1, pvname );
}
void ArchiveSheet::addTblList()
{
	char starttimebuf[30], endtimebuf[30];
	struct epicsTimeStamp timest;
	stdString start_txt, end_txt;
	QList<QListWidgetItem*> listitems = pvlistView -> selectedItems();
	if ( listitems.empty() == true ) return;

	if ( listitems.count() > 20 )
	{
		QMessageBox::information(this, "Error", "Maximum PV List Counts 20!");
		return;
	};
	try
	{
		stdVector<ArchiveDataClient::NameInfo>::iterator iter_nameinfo;
		pvtableView -> setRowCount(listitems.count());
		for ( int i = 0; i < listitems.count(); ++i)
		{
			//qDebug("listitems[%d]:%s", i, listitems[i]->text().toStdString().c_str() );
			string pvname = listitems[i]->text().toStdString();
			for (iter_nameinfo = name_infos.begin(); iter_nameinfo != name_infos.end(); ++iter_nameinfo)
			{
				const char *strtemp = iter_nameinfo->name.c_str();
				if(strcmp(strtemp, pvname.c_str()) != 0) continue;
				pvtableView -> setItem(i, 0, new ArchiveSheetItem(strtemp));

				timest = (epicsTimeStamp)iter_nameinfo->start;
				epicsTimeToStrftime(starttimebuf,28,"%m/%d/%Y %H:%M:%S", &timest);
				timest = (epicsTimeStamp)iter_nameinfo->end;
				epicsTimeToStrftime(endtimebuf,28,"%m/%d/%Y %H:%M:%S", &timest);

				pvtableView -> setItem(i, 1, new ArchiveSheetItem(starttimebuf));
				pvtableView -> setItem(i, 2, new ArchiveSheetItem(endtimebuf));
			};
		};
	}
	catch(...)
	{
		qDebug("Catch Event Exception");
	};
}
void ArchiveSheet::delTblList()
{
	QList<QTableWidgetItem*> tableitems = pvtableView -> selectedItems();
	QList<QTableWidgetItem*>::iterator tableiter;
	for ( int i = 0; i < tableitems.count(); ++i )
	{
		pvtableView -> removeRow(tableitems[i]->row());
	}
}
void ArchiveSheet::clearPVList()
{
	name_infos.clear();
	names.clear();
	pvindexes.clear();
	pvlistView -> clear();
}

void ArchiveSheet::getArchiveValues()
{
	statusBar()->showMessage("Archived Data Loading...");
	strStartTime = startdateEdit->text()+" "+starttimeEdit->text();
	strEndTime   = enddateEdit->text()+" "+endtimeEdit->text();

	//clearing information.
	isInitPage = false;
	setRowCol(true);
	ar_page->clear();
	names.clear();
	clearTable();

	startTime->setText(strStartTime);
	endTime->setText(strEndTime);

    epicsTime start, end;
    string2epicsTime(strStartTime.toStdString().c_str(), start);
    string2epicsTime(strEndTime.toStdString().c_str(), end);

	int pvcount = pvtableView -> rowCount();
	for ( int i = 0; i < pvcount ; i++ )
	{
		QTableWidgetItem *pitem = pvtableView -> item( i, 0);
		names.push_back(pitem->text().toStdString().c_str());
	};

	QTime qtime = ivaltimeEdit->time();
	m_interval = qtime.hour()*3600+qtime.minute()*60+qtime.second();
	int how = 0;
	if (Interval()==Qt::Checked)
	{
		if (m_interval == 0 ) m_interval = 1;
		how = SHEET_TYPE;

	}
	else if(Interval()==Qt::Unchecked)
	{
		m_interval = 0;
		how = PLOT_TYPE;
	};

	mpthr = new ArchiveThread(&m_arclient, names, start, end, MAX_COUNT, how, m_interval, this);
	mpthr -> start();
}
void ArchiveSheet::updatePages(int index)
{
	qDebug("CurPage:%d", index);
	statusBar()->showMessage("Archived Data Loading...");
    epicsTime start, end;
    string2epicsTime(strStartTime.toStdString().c_str(), start);
    string2epicsTime(strEndTime.toStdString().c_str(), end);
	struct epicsTimeStamp epicstart_ts = (epicsTimeStamp)start;
	epicstart_ts.secPastEpoch += (epicsUInt32)m_pagetime*index; //page count

	struct epicsTimeStamp epicend_ts;
	epicend_ts.secPastEpoch = epicstart_ts.secPastEpoch+(epicsUInt32)m_pagetime;

	epicsTime curEpicstart = epicsTime(epicstart_ts);
	epicsTime curEpicend = epicsTime(epicend_ts);

	struct epicsTimeStamp curstart, curend;
	curstart = (epicsTimeStamp)curEpicstart;
	curend = (epicsTimeStamp)curEpicend;
	if ( end < curend ) curend = end;

	char timebuf[30], timebuf2[30];
	epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &curstart);
	epicsTimeToStrftime(timebuf2,28,"%m/%d/%Y %H:%M:%S",&curend);

	qDebug("StartTime:%s, EndTime:%s", timebuf, timebuf2);

	curstartTime->setText(timebuf);
	curendTime->setText(timebuf2);

	// Here, table->clear, table->setItem
	clearTable();
	setRowCol(true);
	mpthr -> setStartTime(curEpicstart);
	mpthr -> start();
}
void ArchiveSheet::updateNextPage()
{
	int totpage = ar_page->count();
	int curpage = ar_page->currentIndex();
	if ( totpage == curpage+1 ) return;
	ar_page->setCurrentIndex(++curpage);
	emit  nextPageSignal(curpage);
	//updatePages(curpage);
}
void ArchiveSheet::updatePrevPage()
{
	int curpage = ar_page->currentIndex();
	if ( curpage == 0 ) return;
	ar_page->setCurrentIndex(--curpage);
	emit prevPageSignal(curpage);
}

void ArchiveSheet::getPVIndexes()
{
	int curindex = pvlistView->currentRow();
	if ( pvlistView -> item (curindex) -> isSelected() )
	{
		pvindexes.push_back(curindex);
	}
	else
	{
		stdVector<int>::iterator pos = std::find ( pvindexes.begin(), pvindexes.end(), curindex );
		if ( pos != pvindexes.end() ) pvindexes.erase(pos);
	};
}
void ArchiveSheet::setupWindow()
{
    QDialog setupDialog(this);
    setupDialog.setWindowTitle("Setup");
	setupDialog.setMinimumSize(800, 700);
	setupDialog.setMaximumSize(800, 700);

	QLabel *pLabel = new QLabel(&setupDialog);
	pLabel -> setText("Archive Group:");
	pLabel -> setGeometry(3, 10, 80, 25);
    QComboBox *archListCombo = new QComboBox(&setupDialog);
    archListCombo->setObjectName(QString::fromUtf8("archListCombo"));
    archListCombo->setGeometry(QRect(110, 10, 130, 25));
    archListCombo->addItems(arch_list);
    archListCombo->setEditable(false);

	QLabel *pLabel2 = new QLabel(&setupDialog);
	pLabel2->setText("PV Name Search:"); 
	pLabel2->setGeometry(3, 55, 95, 25);

	QLabel *pLabel3 = new QLabel(&setupDialog);
	pLabel3->setText("PV Name List "); 
	pLabel3->setGeometry(3, 95, 95, 25);

    QWidget *widget = new QWidget(&setupDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(110, 50, 220, 70));
    QVBoxLayout *vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));

    pvNameLine = new QLineEdit(widget);
    pvNameLine->setObjectName(QString::fromUtf8("pvNameLine"));
    vboxLayout->addWidget(pvNameLine);

    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));

    QSpacerItem *spacerItem = new QSpacerItem(161, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    QHBoxLayout *hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));

    QPushButton *searchButton = new QPushButton(widget);
    searchButton->setObjectName(QString::fromUtf8("searchButton"));
    searchButton->setText(QString::fromUtf8("&Search"));
    connect(searchButton, SIGNAL(clicked()), this, SLOT(showPVList()));

    hboxLayout1->addWidget(searchButton);

    QPushButton *clearButton = new QPushButton(widget);
    clearButton->setObjectName(QString::fromUtf8("clearButton"));
    clearButton->setText(QString::fromUtf8("&Clear"));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPVList()));
    hboxLayout1->addWidget(clearButton);

    hboxLayout->addLayout(hboxLayout1);
    vboxLayout->addLayout(hboxLayout);
    QWidget *widget1 = new QWidget(&setupDialog);
    widget1->setObjectName(QString::fromUtf8("widget1"));
    widget1->setGeometry(QRect(270, 320, 90, 60));

    QVBoxLayout *vboxLayout1 = new QVBoxLayout(widget1);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

    QPushButton *addlistButton = new QPushButton(widget1);
    addlistButton->setObjectName(QString::fromUtf8("addlistButton"));
	connect(addlistButton, SIGNAL(clicked()), this, SLOT(addTblList()));
    addlistButton->setText(QString::fromUtf8("&Add>>"));

    vboxLayout1->addWidget(addlistButton);

    QPushButton *deleteButton = new QPushButton(widget1);
    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(delTblList()));
    deleteButton->setText(QString::fromUtf8("<<&Delete"));

    vboxLayout1->addWidget(deleteButton);

    pvlistView = new QListWidget(&setupDialog);
    pvlistView->setObjectName(QString::fromUtf8("pvlistView"));
    pvlistView->setGeometry(QRect(10, 120, 250, 530));
    pvlistView->setSelectionMode(QAbstractItemView::MultiSelection);

    //pvlistView->setSortingEnabled(true);
	//connect(pvlistView, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(getPVIndexes()) );

	QLabel *pLabel4 = new QLabel(&setupDialog);
	pLabel4->setText("Retrieve PV Name List"); 
	pLabel4->setGeometry(350, 95, 200, 25);

    pvtableView = new QTableWidget(0, 3, &setupDialog);
    pvtableView->setObjectName(QString::fromUtf8("pvtableView"));
    pvtableView->setGeometry(QRect(370, 120, 390, 530));
    pvtableView->setHorizontalHeaderItem(0, new QTableWidgetItem("PV Name"));
    pvtableView->setHorizontalHeaderItem(1, new QTableWidgetItem("Archived Start Time"));
    pvtableView->setHorizontalHeaderItem(2, new QTableWidgetItem("Archived End Time"));
    pvtableView->setSelectionMode(QAbstractItemView::MultiSelection);
    pvtableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QWidget *widget2 = new QWidget(&setupDialog);
    widget2->setObjectName(QString::fromUtf8("widget2"));
    widget2->setGeometry(QRect(570, 660, 211, 29));

    QHBoxLayout *hboxLayout2 = new QHBoxLayout(widget2);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));

    QPushButton *okButton = new QPushButton(widget2);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setText(QString::fromUtf8("&OK"));
    connect(okButton, SIGNAL(clicked()), &setupDialog, SLOT(accept()));
    hboxLayout2->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton(widget2);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
    cancelButton->setText(QString::fromUtf8("&Cancel"));
    connect(cancelButton, SIGNAL(clicked()), &setupDialog, SLOT(reject()));
    hboxLayout2->addWidget(cancelButton);

    QWidget *widget3 = new QWidget(&setupDialog);
    widget3->setObjectName(QString::fromUtf8("widget3"));
    widget3->setGeometry(QRect(210, 660, 221, 29));

    QHBoxLayout *hboxLayout3 = new QHBoxLayout(widget3);
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));

    QPushButton *saveButton = new QPushButton(widget3);
    saveButton->setObjectName(QString::fromUtf8("saveButton"));
    saveButton->setText(QString::fromUtf8("&Savefile"));
    hboxLayout3->addWidget(saveButton);

    QPushButton *readButton = new QPushButton(widget3);
    readButton->setObjectName(QString::fromUtf8("readButton"));
    readButton->setText(QString::fromUtf8("&Readfile"));
    hboxLayout3->addWidget(readButton);

#if 1 //leesi
	QWidget *widgeta;
	QVBoxLayout *vboxLayouta;
	QHBoxLayout *hboxLayouta;
	QLabel *label;

	QHBoxLayout *hboxLayout1a;
	QLabel *label_5;

	widgeta = new QWidget(&setupDialog);
	widgeta->setObjectName(QString::fromUtf8("widgeta"));
	widgeta->setGeometry(QRect(470, 20, 276, 70));
	vboxLayouta = new QVBoxLayout(widgeta);
	vboxLayouta->setSpacing(6);
	vboxLayouta->setMargin(0);
	vboxLayouta->setObjectName(QString::fromUtf8("vboxLayouta"));
	hboxLayouta = new QHBoxLayout();
	hboxLayouta->setSpacing(6);
	hboxLayouta->setMargin(0);
	hboxLayouta->setObjectName(QString::fromUtf8("hboxLayouta"));
	label = new QLabel(widgeta);
	label->setObjectName(QString::fromUtf8("label"));
	QFont font;
	font.setPointSize(11);
	label->setFont(font);

	hboxLayouta->addWidget(label);

	startdateEdit = new QDateEdit(widgeta);
	startdateEdit->setObjectName(QString::fromUtf8("startdateEdit"));
	startdateEdit->setFont(font);
	startdateEdit->setDate(QDate(2006, 8, 1));

	hboxLayouta->addWidget(startdateEdit);

	starttimeEdit = new QTimeEdit(widgeta);
	starttimeEdit->setObjectName(QString::fromUtf8("starttimeEdit"));
	starttimeEdit->setFont(font);

	hboxLayouta->addWidget(starttimeEdit);


	vboxLayouta->addLayout(hboxLayouta);

	hboxLayout1a = new QHBoxLayout();
	hboxLayout1a->setSpacing(6);
	hboxLayout1a->setMargin(0);
	hboxLayout1a->setObjectName(QString::fromUtf8("hboxLayout1a"));
	label_5 = new QLabel(widgeta);
	label_5->setObjectName(QString::fromUtf8("label_5"));
	label_5->setFont(font);

	hboxLayout1a->addWidget(label_5);

	enddateEdit = new QDateEdit(widgeta);
	enddateEdit->setObjectName(QString::fromUtf8("enddateEdit"));
	enddateEdit->setFont(font);
	enddateEdit->setDate(QDate(2006, 8, 1));
	enddateEdit->setMaximumDate(QDate(7999, 12, 31));

	hboxLayout1a->addWidget(enddateEdit);

	endtimeEdit = new QTimeEdit(widgeta);
	endtimeEdit->setObjectName(QString::fromUtf8("endtimeEdit"));
	endtimeEdit->setFont(font);

	hboxLayout1a->addWidget(endtimeEdit);
	vboxLayouta->addLayout(hboxLayout1a);

	QHBoxLayout *hboxLayout2a = new QHBoxLayout();
	hboxLayout2a->setSpacing(6);
	hboxLayout2a->setMargin(0);
	hboxLayout2a->setObjectName(QString::fromUtf8("hboxLayout2a"));
	
	QLabel *label_6 = new QLabel(widgeta);
	label_6->setObjectName(QString::fromUtf8("label_6"));
	label_6->setFont(font);

	hboxLayout2a->addWidget(label_6);

	ivaltimeEdit = new QTimeEdit(widgeta);
	ivaltimeEdit->setObjectName(QString::fromUtf8("endtimeEdit"));
	ivaltimeEdit->setFont(font);
	hboxLayout2a->addWidget(ivaltimeEdit);
	pivalcheck = new QCheckBox(widgeta);
	pivalcheck -> setCheckState(Qt::Checked);
	m_setinterval = Qt::Checked;

    connect(pivalcheck, SIGNAL(stateChanged(int)), this, SLOT(setInterval(int)));

	hboxLayout2a->addWidget(pivalcheck);
	vboxLayouta->addLayout(hboxLayout2a);

	QMetaObject::connectSlotsByName(&setupDialog);
	label->setText(QApplication::translate("Form", "Start Time:", 0, QApplication::UnicodeUTF8));
	startdateEdit->setDisplayFormat(QApplication::translate("Form", "MM/dd/yyyy", 0, QApplication::UnicodeUTF8));
	starttimeEdit->setDisplayFormat(QApplication::translate("Form", "hh:mm:ss", 0, QApplication::UnicodeUTF8));
	label_5->setText(QApplication::translate("Form", "End Time:", 0, QApplication::UnicodeUTF8));
	label_6->setText(QApplication::translate("Form", "Interval:", 0, QApplication::UnicodeUTF8));
	enddateEdit->setDisplayFormat(QApplication::translate("Form", "MM/dd/yyyy", 0, QApplication::UnicodeUTF8));
	endtimeEdit->setDisplayFormat(QApplication::translate("Form", "hh:mm:ss", 0, QApplication::UnicodeUTF8));
	ivaltimeEdit->setDisplayFormat(QApplication::translate("Form", "hh:mm:ss", 0, QApplication::UnicodeUTF8));
#endif

    if( setupDialog.exec() ) 
	{
		setupDialog.hide();
		getArchiveValues();
    };
};
void ArchiveSheet::setInterval(int interval)
{
	//qDebug("SetInterval:%d, %d", interval, Qt::Checked);
	if(pivalcheck->checkState()==Qt::Checked)
		ivaltimeEdit->setDisabled(false);
	else
		ivaltimeEdit->setDisabled(true);
	m_setinterval = interval;
}

const int ArchiveSheet::Interval()
{
	return m_setinterval;
}

void ArchiveSheet::disconnWindow()
{
	m_arclient.setURL("");
	mfilepath="";
	arch_list.clear();
	m_arch_combo->clear();
	ar_page->clear();
	clearTable();
}

void ArchiveSheet::connWindow()
{
    QDialog addDialog(this);
    addDialog.setWindowTitle("Connect To Archiver Server");
	addDialog.setMinimumSize(400, 100);
	addDialog.setMaximumSize(400, 100);

	QWidget *widget = new QWidget(&addDialog);
	widget->setObjectName(QString::fromUtf8("widget"));
	widget->setGeometry(QRect(0, 0, 400, 100));

    QLineEdit *editor = new QLineEdit(widget);
	editor -> setText("http://DataAnalysis/archive/cgi/ArchiveDataServer.cgi");
	
    QVBoxLayout *vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->addWidget(editor);

    QPushButton *okButton = new QPushButton(tr("OK"), widget);
    okButton -> setDefault(true);
    connect(okButton, SIGNAL(clicked()), &addDialog, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(tr("CANCEL"), widget);
    connect(cancelButton, SIGNAL(clicked()), &addDialog, SLOT(reject()));
	
    QHBoxLayout *hboxLayout = new QHBoxLayout();	//No parenet creation.
    hboxLayout->setSpacing(10);
    hboxLayout->setMargin(10);
    hboxLayout->addWidget(okButton);
    hboxLayout->addWidget(cancelButton);

    QHBoxLayout *hboxLayout1 = new QHBoxLayout();	//No parenet creation.
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
	QSpacerItem *spacerItem = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem);
    hboxLayout1->addLayout(hboxLayout);
    vboxLayout->addLayout(hboxLayout1);

    if( addDialog.exec() ) 
	{
		QString strUrl = editor -> text();
		if (!connectToArchiver( strUrl ))
		{
			QMessageBox::information(&addDialog,"MessageBox",strUrl+" connection Fail");
		};
    };
}

void ArchiveSheet::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(connAction);
    fileMenu->addAction(disconnAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(setupAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(appendAction);
    fileMenu->addAction(saveasAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *calcMenu = menuBar()->addMenu(tr("&Calculator"));
    calcMenu->addAction(cell_addAction);
    calcMenu->addAction(cell_subAction);
    calcMenu->addAction(cell_mulAction);
    calcMenu->addAction(cell_divAction);
    calcMenu->addAction(cell_sumAction);
    calcMenu->addSeparator();
    calcMenu->addAction(colorAction);
    calcMenu->addAction(fontAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(view_Action);

    menuBar()->addSeparator();

    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutArchiveSheet);
}

//void ArchiveSheet::updateStatus(QTableWidgetItem *item)
void ArchiveSheet::updateStatus(QTableWidgetItem *)
{
#if 0
    if (item && item == table->currentItem()) {
        statusBar()->showMessage(item->data(Qt::StatusTipRole).toString(), 1000);
        cellLabel->setText("Cell: (" + encode_pos(table->row(item), table->column(item)) + ")");
    }
#else
	statusBar()->showMessage("Archived Data Loading...", 1000);
#endif
}

void ArchiveSheet::updateColor(QTableWidgetItem *item)
{
    QPixmap pix(16, 16);
    QColor col;
    if (item)
        col = item->backgroundColor();
    if (!col.isValid())
        col = palette().base().color();

    QPainter pt(&pix);
    pt.fillRect(0, 0, 16, 16, col);

    QColor lighter = col.light();
    pt.setPen(lighter);
    QPoint lightFrame[] = { QPoint(0, 15), QPoint(0, 0), QPoint(15, 0) };
    pt.drawPolyline(lightFrame, 3);

    pt.setPen(col.dark());
    QPoint darkFrame[] = { QPoint(1, 15), QPoint(15, 15), QPoint(15, 1) };
    pt.drawPolyline(darkFrame, 3);

    pt.end();

    colorAction->setIcon(pix);
}

//void ArchiveSheet::updateLineEdit(QTableWidgetItem *item)
void ArchiveSheet::updateLineEdit(QTableWidgetItem *)
{
#if 0
    if (item != table->currentItem())
        return;
    if (item)
        formulaInput->setText(item->data(Qt::EditRole).toString());
    else
        formulaInput->clear();
#endif
}

void ArchiveSheet::returnPressed()
{
#if 0
    QString text = formulaInput->text();
    int row = table->currentRow();
    int col = table->currentColumn();
    QTableWidgetItem *item = table->item(row, col);
    if (!item)
        table->setItem(row, col, new ArchiveSheetItem(text));
    else
        item->setData(Qt::EditRole, text);
    table->viewport()->update();
#endif
}

void ArchiveSheet::selectColor()
{
    QTableWidgetItem *item = table->currentItem();
    QColor col = item ? item->backgroundColor() : table->palette().base().color();
    col = QColorDialog::getColor(col, this);
    if (!col.isValid())
        return;

    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (selected.count() == 0)
        return;

    foreach(QTableWidgetItem *i, selected)
        if (i) i->setBackgroundColor(col);

    updateColor(table->currentItem());
}

void ArchiveSheet::selectFont()
{
    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (selected.count() == 0)
        return;
    bool ok = false;
    QFont fnt = QFontDialog::getFont(&ok, font(), this);
    if (!ok)
        return;
    foreach(QTableWidgetItem *i, selected)
        if (i) i->setFont(fnt);
}

bool ArchiveSheet::runInputDialog(const QString &title,
                                 const QString &c1Text,
                                 const QString &c2Text,
                                 const QString &opText,
                                 const QString &outText,
                                 QString *cell1, QString *cell2, QString *outCell)
{

    QStringList rows, cols;
    for (int c = 0; c < table->columnCount(); ++c)
        cols << QChar('A' + c);
    for (int r = 0; r < table->rowCount(); ++r)
        rows << QString::number(1 + r);

    QDialog addDialog(this);
    addDialog.setWindowTitle(title);

    QGroupBox group(title, &addDialog);
    group.setMinimumSize(250, 100);

    QLabel cell1Label(c1Text, &group);
    QComboBox cell1RowInput(&group);
    int c1Row, c1Col;
    decode_pos(*cell1, &c1Row, &c1Col);
    cell1RowInput.addItems(rows);
    cell1RowInput.setCurrentIndex(c1Row);
    QComboBox cell1ColInput(&group);
    cell1ColInput.addItems(cols);
    cell1ColInput.setCurrentIndex(c1Col);

    QLabel operatorLabel(opText, &group);
    operatorLabel.setAlignment(Qt::AlignHCenter);

    QLabel cell2Label(c2Text, &group);
    QComboBox cell2RowInput(&group);
    int c2Row, c2Col;
    decode_pos(*cell2, &c2Row, &c2Col);
    cell2RowInput.addItems(rows);
    cell2RowInput.setCurrentIndex(c2Row);
    QComboBox cell2ColInput(&group);
    cell2ColInput.addItems(cols);
    cell2ColInput.setCurrentIndex(c2Col);

    QLabel equalsLabel("=", &group);
    equalsLabel.setAlignment(Qt::AlignHCenter);

    QLabel outLabel(outText, &group);
    QComboBox outRowInput(&group);
    int outRow, outCol;
    decode_pos(*outCell, &outRow, &outCol);
    outRowInput.addItems(rows);
    outRowInput.setCurrentIndex(outRow);
    QComboBox outColInput(&group);
    outColInput.addItems(cols);
    outColInput.setCurrentIndex(outCol);

    QPushButton cancelButton(tr("Cancel"), &addDialog);
    connect(&cancelButton, SIGNAL(clicked()), &addDialog, SLOT(reject()));

    QPushButton okButton(tr("OK"), &addDialog);
    okButton.setDefault(true);
    connect(&okButton, SIGNAL(clicked()), &addDialog, SLOT(accept()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(&okButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(&cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout(&addDialog);
    dialogLayout->addWidget(&group);
    dialogLayout->addStretch(1);
    dialogLayout->addItem(buttonsLayout);

    QHBoxLayout *cell1Layout = new QHBoxLayout;
    cell1Layout->addWidget(&cell1Label);
    cell1Layout->addSpacing(10);
    cell1Layout->addWidget(&cell1ColInput);
    cell1Layout->addSpacing(10);
    cell1Layout->addWidget(&cell1RowInput);

    QHBoxLayout *cell2Layout = new QHBoxLayout;
    cell2Layout->addWidget(&cell2Label);
    cell2Layout->addSpacing(10);
    cell2Layout->addWidget(&cell2ColInput);
    cell2Layout->addSpacing(10);
    cell2Layout->addWidget(&cell2RowInput);

    QHBoxLayout *outLayout = new QHBoxLayout;
    outLayout->addWidget(&outLabel);
    outLayout->addSpacing(10);
    outLayout->addWidget(&outColInput);
    outLayout->addSpacing(10);
    outLayout->addWidget(&outRowInput);

    QVBoxLayout *vLayout = new QVBoxLayout(&group);
    vLayout->addItem(cell1Layout);
    vLayout->addWidget(&operatorLabel);
    vLayout->addItem(cell2Layout);
    vLayout->addWidget(&equalsLabel);
    vLayout->addStretch(1);
    vLayout->addItem(outLayout);

    if (addDialog.exec()) {
        *cell1 = cell1ColInput.currentText() + cell1RowInput.currentText();
        *cell2 = cell2ColInput.currentText() + cell2RowInput.currentText();
        *outCell = outColInput.currentText() + outRowInput.currentText();
        return true;
    }

    return false;
}


void ArchiveSheet::actionSum()
{

    int row_first = 0;
    int row_last = 0;
    int row_cur = 0;

    int col_first = 0;
    int col_last = 0;
    int col_cur = 0;

    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (!selected.isEmpty()) {
        QTableWidgetItem *first = selected.first();
        QTableWidgetItem *last = selected.last();
        row_first = table->row(first);
        row_last = table->row(last);
        col_first = table->column(first);
        col_last = table->column(last);
    }

    QTableWidgetItem *current = table->currentItem();
    if (current) {
        row_cur = table->row(current);
        col_cur = table->column(current);
    }

    QString cell1 = encode_pos(row_first, col_first);
    QString cell2 = encode_pos(row_last, col_last);
    QString out = encode_pos(row_cur, col_cur);


    if (runInputDialog(tr("Sum cells"), tr("First cell:"), tr("Last cell:"),
                       QString("%1").arg(QChar(0x03a3)), tr("Output to:"),
                       &cell1, &cell2, &out)) {
        int row, col;
        decode_pos(out, &row, &col);
        table->item(row, col)->setText("sum " + cell1 + " " + cell2);
    }
}


void ArchiveSheet::actionMath_helper(const QString &title, const QString &op)
{
    QString cell1 = "C1";
    QString cell2 = "C2";
    QString out = "C3";

    QTableWidgetItem *current = table->currentItem();
    if (current)
        out = encode_pos(table->currentRow(), table->currentColumn());

    if (runInputDialog(title, tr("Cell 1"), tr("Cell 2"), op, tr("Output to:"),
                       &cell1, &cell2, &out)) {
        int row, col;
        decode_pos(out, &row, &col);
        table->item(row, col)->setText(op + " " + cell1 + " " + cell2);
    }
}


void ArchiveSheet::actionAdd()
{
    actionMath_helper(tr("Addition"), "+");
}

void ArchiveSheet::viewPlot()
{
	QMessageBox::information(this, "Plotting", "Plot Window will be called");
}

void ArchiveSheet::actionSubtract()
{
    actionMath_helper(tr("Subtraction"), "-");
}

void ArchiveSheet::actionMultiply()
{
    actionMath_helper(tr("Multiplication"), "*");
}

void ArchiveSheet::actionDivide()
{
    actionMath_helper(tr("Division"), "/");
}

void ArchiveSheet::clear()
{
    foreach (QTableWidgetItem *i, table->selectedItems())
        i->setText("");
}

void ArchiveSheet::setupContextMenu()
{
    addAction(cell_addAction);
    addAction(cell_subAction);
    addAction(cell_mulAction);
    addAction(cell_divAction);
    addAction(cell_sumAction);
    addAction(firstSeparator);
    addAction(colorAction);
    addAction(fontAction);
    addAction(secondSeparator);
    addAction(clearAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void ArchiveSheet::setupContents()
{
    QColor titleBackground(Qt::lightGray);
    QFont titleFont = table->font();
    titleFont.setBold(true);
}

void ArchiveSheet::setEpicsTimeStamp(const epicsTimeStamp &ts)
{
	epicstime = ts; 
}

const char *htmlText =
"<HTML>"
"<p><b>This demo shows use of <c>QTableWidget</c> with custom handling for"
" individual cells.</b></p>"
"<p>Using a customized table item we make it possible to have dynamic"
" output in different cells. The content that is implemented for this"
" particular demo is:"
"<ul>"
"<li>Adding two cells.</li>"
"<li>Subtracting one cell from another.</li>"
"<li>Multiplying two cells.</li>"
"<li>Dividing one cell with another.</li>"
"<li>Summing the contents of an arbitrary number of cells.</li>"
"</HTML>";


void ArchiveSheet::showAbout()
{
    QMessageBox::about(this, "About Archivesheet", htmlText);
}
