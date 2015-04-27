/********************************************************************************
** Form generated from reading ui file 'pvlistviewer.ui'
**
** Created: Thu Jul 12 11:41:25 2007
**      by: Qt User Interface Compiler version 4.2.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/
#include "pvlistviewer.h"


PVListViewer::PVListViewer(QWidget *widget) : QWidget(widget)
{
	makeUI();

};
PVListViewer::~PVListViewer()
{
};
void
PVListViewer::makeUI()
{
	setWindowTitle("PV List Viewer");

	QFont font;
	font.setPointSize(10);
    QLabel *label = new QLabel(this);
	label->setObjectName(QString::fromUtf8("label"));
	label->setGeometry(QRect(4, 20, 71, 22));
	label->setText("System :");
	label->setFont(font);

    QLabel *label_2 = new QLabel(this);
	label_2->setObjectName(QString::fromUtf8("label_2"));
	label_2->setGeometry(QRect(4, 70, 72, 22));
	label_2->setText("PV Name :");
	label_2->setFont(font);

    QLabel *label_3 = new QLabel(this);
	label_3->setObjectName(QString::fromUtf8("label_3"));
	label_3->setGeometry(QRect(4, 120, 68, 22));
	label_3->setText("Clipboard:");
	label_3->setFont(font);

	QLabel *label_5 = new QLabel(this);
	label_5->setObjectName(QString::fromUtf8("label_5"));
	label_5->setGeometry(QRect(6, 170, 68, 22));
	label_5->setText("PV List :");
	label_5->setFont(font);

	system_cb = new QComboBox(this);
	system_cb->setObjectName(QString::fromUtf8("system_cb"));
	system_cb->setGeometry(QRect(78, 20, 111, 25));
	system_cb->setFont(font);
	system_cb->clear();
	system_cb->addItem("ALL");
	system_cb->addItem("TMS");
	system_cb->addItem("VMS");
	system_cb->addItem("CLS");
	system_cb->addItem("QDS");

	pvname_le = new QLineEdit(this);
	pvname_le->setObjectName(QString::fromUtf8("pvname_le"));
	pvname_le->setGeometry(QRect(78, 70, 180, 25));

	pvdrag_le = new QLineEdit(this);
	pvdrag_le->setObjectName(QString::fromUtf8("pvdrag_le"));
	pvdrag_le->setGeometry(QRect(78, 120, 220, 25));
	pvdrag_le->setEchoMode(QLineEdit::Normal);

	search_pb = new QPushButton(this);
	search_pb->setObjectName(QString::fromUtf8("search_pb"));
	search_pb->setGeometry(QRect(307, 70, 75, 27));
	search_pb->setText("&Search");
	search_pb->setFont(font);
	connect(search_pb, SIGNAL(clicked()), SLOT(Search()));

    notconnlist_pb = new QPushButton(this);
	notconnlist_pb->setObjectName(QString::fromUtf8("notconnlist_pb"));
	notconnlist_pb->setGeometry(QRect(105, 160, 85, 27));
	notconnlist_pb->setText("&Save Tr List");
	notconnlist_pb->setFont(font);
	connect(notconnlist_pb, SIGNAL(clicked()), SLOT(NotConnPVList() ));

	pvcheck_pb = new QPushButton(this);
	pvcheck_pb->setObjectName(QString::fromUtf8("pvcheck_pb"));
	pvcheck_pb->setGeometry(QRect(210, 160, 85, 27));
	pvcheck_pb->setText("PV Check");
	pvcheck_pb->setFont(font);
	connect(pvcheck_pb, SIGNAL(clicked()), SLOT(PVListCheck() ));

	close_pb = new QPushButton(this);
	close_pb->setObjectName(QString::fromUtf8("close_pb"));
	close_pb->setGeometry(QRect(315, 160, 85, 27));
	close_pb->setText("&Close");
	close_pb->setFont(font);
	connect(close_pb, SIGNAL(clicked()), SLOT(close()));

	pvlist_tw = new QTableWidget(this);
	pvlist_tw->setObjectName(QString::fromUtf8("pvlist_tw"));
	pvlist_tw->setGeometry(QRect(0, 194, 406, 241));
    pvlist_tw->setColumnCount(3);
    pvlist_tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pvlist_tw->horizontalHeader()->setStretchLastSection(true);
    QStringList labels;
    labels << tr("PVIndex") << tr("PVName") << tr("Status");
    pvlist_tw->setHorizontalHeaderLabels(labels);
    pvlist_tw->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    pvlist_tw->verticalHeader()->hide();
    pvlist_tw->setShowGrid(true);
    pvlist_tw->setSelectionMode(QAbstractItemView::SingleSelection);
    //pvlist_tw->setSelectionMode(QAbstractItemView::MultiSelection);
    pvlist_tw->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(pvlist_tw, SIGNAL(cellClicked(int, int)), SLOT(SelectTableWidgetItem(int, int) ));
	//connect(pvlist_tw, SIGNAL(itemClicked(QTableWidgetItem*)), SLOT(SelectTableWidgetItem(QTableWidgetItem*) ));
	//connect(pvlist_tw, SIGNAL(itemSelectionChanged ()), SLOT(SelectTableWidgetItem() ));
    listcnt = new QLabel(this);
	listcnt->setObjectName(QString::fromUtf8("listcnt"));
	listcnt->setGeometry(QRect(4, 435, 350, 25));
	listcnt->setFont(font);
	listcnt->hide();

	string dbname   = "kstarweb";
	string hostname = "203.230.119.122";
	string user     = "root";
	string passwd   = "kstar2004";
	mysqlpp::Connection con(mysqlpp::use_exceptions);

	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return;
	};

	if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
	{
		return;
	};

	m_con = con;
	show();

};
void 
PVListViewer::SelectTableWidgetItem(int row, int)
{
	//cout<<"Item Text: "<<witem->text();
	QTableWidgetItem * pitem = pvlist_tw->item(row, 1);
	pvdrag_le->setText(pitem ->text());
	pvdrag_le->setSelection(0, pitem ->text().size());
	//pvdrag_le->copy();

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(pitem->text(), QClipboard::Clipboard);
	clipboard->setText(pitem->text(), QClipboard::Selection);

};

void
PVListViewer::NotConnPVList()
{
	if (!pvlist_tw || pvlist_tw->rowCount() <= 0) return;
    QString fileName = QFileDialog::getSaveFileName(this, tr("SaveFile"),"",
                                tr("All Files (*);;Text Files (*.txt)"));
	if(!fileName.isEmpty())
	{
		saveFile(fileName);
	};
};
void
PVListViewer::saveFile(const QString &filename)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly|QIODevice::Text);
	QTableWidgetItem *pitem = 0, *pitem1;
	QString strHeader ="##Not Connected PV List";
	file.write(strHeader.toStdString().c_str(), strHeader.size());
	file.write("\n", 1);
	for ( int i = 0; i < pvlist_tw->rowCount(); i++)
	{
		pitem = pvlist_tw->item(i, 2);
		QString strStatus = pitem->text();
		if( strStatus.compare("Connected") == 0 
				|| strStatus.compare("") == 0 ) continue;

		pitem1 = pvlist_tw->item(i, 1);
		QString pvname = pitem1->text();

		int len = qstrlen(pvname.toStdString().c_str());
		file.write(pvname.toStdString().c_str(), len);
		file.write("\n", 1);
	};
	file.close();
}
void
PVListViewer::PVListCheck()
{
	if (!pvlist_tw) return;

#if 1
	/* Start up Channel Access */
    int result = ca_context_create(ca_disable_preemptive_callback);
    if (result != ECA_NORMAL) 
	{
        qDebug("CA error %s occurred while trying to start channel access.", ca_message(result));
        return;
    };

	/* Connect channels */
	QTableWidgetItem * pitem = 0;
	enum channel_state state;
	int rowcnt = pvlist_tw->rowCount();
	chid channelID[rowcnt];
	//int result = 0;
	for ( int i = 0; i < rowcnt; i++)
	{
		pitem = pvlist_tw->item(i, 1);
		string strpvname = pitem->text().toStdString();
		result = ca_create_channel( strpvname.c_str() , 0, 0, 50, &channelID[i]);
		if (result != ECA_NORMAL) 
		{
			qDebug("CA error %s occurred while trying to create channel.\n", ca_message(result));
		};
	};
	ca_pend_io (1.5);
	int nconncnt = 0;
	for ( int i = 0; i < rowcnt; i++)
	{
		state   = ca_state(channelID[i]);
		//qDebug("pv_state:%d", state);
		pitem = pvlist_tw->item(i, 2);
		if(state==0)
		{
			pitem->setText("PV not found");
			pitem->setForeground(QBrush(QColor("red")));
			nconncnt++;
		}
		else if( state== 1 )
		{
			pitem->setText("Disconnected");
			pitem->setForeground(QBrush(QColor("red")));
			nconncnt++;
		}
		else if (state == 2)
		{
			pitem->setForeground(QBrush(QColor("blue")));
			pitem->setText("Connected");
		};
	};

	QString strlistcnt = QVariant(rowcnt).toString();
	QString strnotcnt = QVariant(nconncnt).toString();

	QString strlist = QString("List Count:")+strlistcnt+QString(", Not Connected PV Count:")+strnotcnt;
    listcnt->setText(strlist);
    listcnt->show();

	/* Shut down Channel Access */
    ca_context_destroy();
#endif

};
void
PVListViewer::Search()
{
	int count =0;
	pvlist_tw->setRowCount(0);
	listcnt->hide();
	try
	{
		mysqlpp::Query query = m_con.query();
		mysqlpp::ResUse res;

		QString qstrsysname = system_cb->currentText();
		QString qstrpvsub  = pvname_le->text();

		//query << "select pvindex, pvname, system from kstarpvtable where system=%0q and pvname like %1q";
		string strquery;
		if (qstrsysname.compare("ALL") == 0 && qstrpvsub.isEmpty() == true)
		{
			//strquery = "select pvindex, pvname, system from kstarpvtable";
			strquery = "select pvindex, pvname from kstarpvtable";
			query << strquery; 
			query.parse();
			res = query.use();
		}
		else if (qstrsysname.compare("ALL") == 0 && qstrpvsub.isEmpty() == false)
		{
			//strquery = "select pvindex, pvname, system from kstarpvtable where pvname like %0q";
			strquery = "select pvindex, pvname from kstarpvtable where pvname like %0q";
			query << strquery; 
			query.parse();
			string strpvsub = string("%")+pvname_le->text().toStdString()+string("%");
			res = query.use(strpvsub.c_str());
		}
		else if (qstrsysname.compare("ALL") != 0 && qstrpvsub.isEmpty() == true)
		{
			//strquery = "select pvindex, pvname, system from kstarpvtable where system=%0q";
			strquery = "select pvindex, pvname from kstarpvtable where system=%0q";
			query << strquery; 
			query.parse();
			res = query.use(qstrsysname.toStdString().c_str());
		}
		else
		{
			//strquery = "select pvindex, pvname, system from kstarpvtable where system=%0q and pvname like %1q";
			strquery = "select pvindex, pvname from kstarpvtable where system=%0q and pvname like %1q";
			query << strquery; 
			query.parse();
			string strpvsub = string("%")+pvname_le->text().toStdString()+string("%");
			res = query.use(qstrsysname.toStdString().c_str(), strpvsub.c_str());
		}
		
		mysqlpp::Row row;
		// Display the result set
		while (row = res.fetch_row())
		{
			cout << "pvindex: " << row["pvindex"]<<" pvname: " << row["pvname"] << endl;
			string strpvname  = row["pvname"].get_string();
			string strpvindex = row["pvindex"].get_string();;
			//string strsystem  = row["system"].get_string();;
			int row = pvlist_tw->rowCount();
			pvlist_tw->insertRow(row);
			pvlist_tw->setItem(row, 0, new QTableWidgetItem(strpvindex.c_str()));
			pvlist_tw->setItem(row, 1, new QTableWidgetItem(strpvname.c_str()));
			pvlist_tw->setItem(row, 2, new QTableWidgetItem(""));
			count++;
		};
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return;
	}
	catch (const mysqlpp::Exception& er) {
		printf("PV Count: %d\n", count);
		pvlist_tw->update();
		cerr << er.what() << endl;
		return;
	};

	return;
}
