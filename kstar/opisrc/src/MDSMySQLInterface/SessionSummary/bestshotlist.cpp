#include "bestshotlist.h"

using namespace mysqlpp;

//BestShotList::BestShotList(SessionSummary *pwidget):m_pParent(pwidget)

BestShotList::BestShotList()
{
	init();
	getUiObject();
	registerSignal();

	QDateTime startdatetime = QDateTime::currentDateTime();
	mdtStart->setDateTime(startdatetime);
	mdtEnd->setDateTime(startdatetime);
	//startTimer(1000);
}

BestShotList::~BestShotList()
{
}

void BestShotList::init()
{
	QFile file ("../ui/BestShotW.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(BestShotW.ui) File not exists!");
		return;
	};
	file.open(QFile::ReadOnly);
	m_pBest = loader.load(&file);
	m_pBest->show();
	file.close();
}
void BestShotList::setParent(QWidget *parent)
{
	m_pParent = (SessionSummary*)parent;
	m_con = m_pParent->getConnection();
	emit Search();
}
void BestShotList::getUiObject()
{
	mlbTodayShot	= m_pBest->findChild<QLabel *>("lbTodayShot");
	mlbTotalShot	= m_pBest->findChild<QLabel *>("lbTotalShot");
	mleTodayIp	= m_pBest->findChild<QLineEdit *>("leTodayIp");
	mleTotalIp	= m_pBest->findChild<QLineEdit *>("leTotalIp");
	mckToday	= m_pBest->findChild<QCheckBox *>("ckToday");
	mckDesc		= m_pBest->findChild<QCheckBox *>("ckDesc");
	mpbSearch	= m_pBest->findChild<QPushButton *>("pbSrch");
	mcbSort		= m_pBest->findChild<QComboBox *>("cbSort");
	mtwBestShot = m_pBest->findChild<QTableWidget *>("twBestShot");
	mdtStart	= m_pBest->findChild<QDateTimeEdit *>("dtStart");
	mdtEnd		= m_pBest->findChild<QDateTimeEdit *>("dtEnd");
	mleStart 	= m_pBest->findChild<QLineEdit *>("leStart");
	mleEnd  	= m_pBest->findChild<QLineEdit *>("leEnd");

	mdtStart->hide();	
	mdtEnd->hide();

	QFont font("Helvetica", 14);
	QStringList labels;
	labels << tr("Shot") << "Time" << tr("Ip\n(kA)") <<tr(">0.1kA\n(msec)")<<tr(">100kA\n(msec)") <<tr("Remark")<<tr("Date");
	mtwBestShot->setColumnCount(labels.size());
	mtwBestShot->setHorizontalHeaderLabels(labels);
	for(int i = 0; i < labels.size(); i++)
	{
		switch(i)
		{
			case 0:
				mtwBestShot->horizontalHeader()->resizeSection(i, 45);
				break;
			case 1:
				mtwBestShot->horizontalHeader()->resizeSection(i, 60);
				break;
			case 2:
				mtwBestShot->horizontalHeader()->resizeSection(i, 85);
				break;
			case 3:
				mtwBestShot->horizontalHeader()->resizeSection(i, 85);
				break;
			case 4:
				mtwBestShot->horizontalHeader()->resizeSection(i, 85);
				break;
			case 5:
				mtwBestShot->horizontalHeader()->resizeSection(i, 350);
				break;
			case 6:
				mtwBestShot->horizontalHeader()->resizeSection(i, 100);
				break;
		};
	};
	mtwBestShot->verticalHeader()->show();
	mtwBestShot->setShowGrid(true);
	mtwBestShot->setSelectionMode(QAbstractItemView::SingleSelection);
	mtwBestShot->setSelectionBehavior(QAbstractItemView::SelectRows);
	mtwBestShot->setFont(font);
	font.setPointSize(11);
	font.setBold(true);
	mtwBestShot->horizontalHeader()->setFont(font);
	mtwBestShot->horizontalHeader()->setFixedHeight(54);
	mtwBestShot->setFont(font);
}
void BestShotList::registerSignal()
{
	connect(this, SIGNAL(Search()), this, SLOT(dbSearch()));
	connect(mpbSearch, SIGNAL(clicked()), this, SLOT(dbSearch()));
	connect(mcbSort, SIGNAL(currentIndexChanged(int)),this, SLOT(cbChanged(int)));

	QPushButton *pclose = m_pBest->findChild<QPushButton *>("pbClose");
	connect(pclose, SIGNAL(clicked()), this, SLOT(Close()));
}
void BestShotList::Close()
{
	m_pBest->close();
	close();
}
void BestShotList::timerEvent(QTimerEvent *event)
{
	static int count = 0;
	qDebug("BestShotList:%d", count++);
}
void BestShotList::dbSearch()
{
	string strquery=""; 
	Query query = m_con.query();
	ResUse res;
	mtwBestShot->setRowCount(0);
	string strOrder= (mckDesc->isChecked()==true)?"DESC":"ASC";
	string strPara="";
	//qDebug("curIndex:%d",mcbSort->currentIndex());
	QString qstrStartVal =mleStart->text();
	QString qstrEndVal = mleEnd->text();
	//mleTodayIp->setText("");
	//mleTotalIp->setText("");
	switch(mcbSort->currentIndex())
	{
		case 1:
			strPara="limit";
			break;
		case 2:
			strPara="plsmacurrent";
			break;
		case 3:
			strPara="ipwidth";
			break;
		case 4:
			strPara="ipwidth100k";
			break;
		case 5:
			strPara="shotdate";
			break;
		case 0:
		default:
			strPara="shotnum";
			break;
	};

	try {
		if( mckToday->isChecked() == true)
		{
			strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k,comment from SHOTSUMMARY where shotdate > CURDATE() and  plsmacurrent > 100000.0 order by %0 %1";
			query<<strquery;
			query.parse();
			res = query.use(strPara.c_str(), strOrder.c_str());
		}
		else
		{
			if( strPara.compare("shotdate") == 0 )
			{
				strquery="select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k,comment from SHOTSUMMARY where UNIX_TIMESTAMP(shotdate) > UNIX_TIMESTAMP('%0') and UNIX_TIMESTAMP(shotdate) <= UNIX_TIMESTAMP('%1') and plsmacurrent > 100000.0 order by shotdate %2";
				query << strquery;
				query.parse();
				QString starttime = mdtStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");
				QString endtime	= mdtEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
				res = query.use(starttime.toStdString().c_str(), endtime.toStdString().c_str(),strOrder.c_str());
			}
			else
			{
				if(qstrStartVal.isEmpty()==true && qstrEndVal.isEmpty()==true)
				{
					strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k, comment from SHOTSUMMARY where plsmacurrent >= 100000.0 order by %0 %1";
					query << strquery;
					query.parse();
					res = query.use(strPara.c_str(), strOrder.c_str());
				}
				else if(qstrStartVal.isEmpty()==true)
				{
					bool check;
					double dendval = qstrEndVal.toDouble(&check);
					if(strPara.compare("plsmacurrent")==0 )
					{
						dendval *= 1000;
						if ( dendval < 100000.0)
						{
							//messageBox
							return;
						};
					}
					else if(strPara.compare("ipwidth")==0 || strPara.compare("ipwidth100k")==0 )
					{
						dendval /= 1000.0;
					}
					if (check == false)
					{
						//messageBox("Data Conversion Error!!",OKBOX);
						return;
					};
					strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k, comment from SHOTSUMMARY where plsmacurrent >= 100000.0 and %0<=%1 order by %0 %2";
					query << strquery;
					query.parse();
					res = query.use(strPara.c_str(), dendval, strOrder.c_str());
				}
				else if(qstrEndVal.isEmpty()==true)
				{
					bool check;
					double dstartval = qstrStartVal.toDouble(&check);
					if (check == false)
					{
						//messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					if(strPara.compare("plsmacurrent")==0 )
					{
						dstartval *= 1000;
						if ( dstartval < 100000.0)
						{
							//messageBox
							return;
						};
					}
					else if(strPara.compare("ipwidth")==0 || strPara.compare("ipwidth100k")==0 )
					{
						dstartval /= 1000.0;
					}
					strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k, comment from SHOTSUMMARY where plsmacurrent >= 100000.0 and %0>=%1 order by %0 %2";
					query << strquery;
					query.parse();
					res = query.use(strPara.c_str(), dstartval, strOrder.c_str());
				}
				else
				{
					bool check;
					double dstartval = qstrStartVal.toDouble(&check);
					if (check == false)
					{
						//messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					double dendval = qstrEndVal.toDouble(&check);
					if (check == false)
					{
						//messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					if(strPara.compare("plsmacurrent")==0 )
					{
						dstartval *= 1000;
						dendval *= 1000;
						if(dstartval>=dendval || dstartval<100000.0 || dendval<100000.0) return;
					}
					else if(strPara.compare("ipwidth")==0 || strPara.compare("ipwidth100k")==0 )
					{
						dstartval /= 1000.0;
						dendval /= 1000.0;
					}
					strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k, comment from SHOTSUMMARY where plsmacurrent >= 100000.0 and %0>=%1 and %0<=%2 order by %0 %3";
					query << strquery;
					query.parse();
					res = query.use(strPara.c_str(), dstartval, dendval, strOrder.c_str());
				}
#if 0
				strquery= "select shotnum,shotdate,plsmacurrent,ipwidth,ipwidth100k, comment from SHOTSUMMARY where plsmacurrent > 100000.0 and UNIX_TIMESTAMP(shotdate)>UNIX_TIMESTAMP('2008-06-01') order by plsmacurrent %0";
				query << strquery;
				query.parse();
				res = query.use(strOrder.c_str());
#endif
			};
		}
		Row row;
		QTableWidgetItem *pItem = 0;
		while (row = res.fetch_row())
		{
			string strshotnum		= row["shotnum"].get_string();
			string strshotdate		= row["shotdate"].get_string();;
			string strcomment		= row["comment"].get_string();;
			//
			QDateTime datetime = QDateTime::fromString(strshotdate.c_str(),"yyyy-MM-dd hh:mm:ss");
			QString date = datetime.date().toString("yyyy-MM-dd");
			QString time = datetime.time().toString();

			double dplmacurrent		= row["plsmacurrent"]/1000.0;
			double dipwidth			= row["ipwidth"]*1000.0;
			double dipwidth100k		= row["ipwidth100k"]*1000.0;

			QString qstrplmacur	= QString("%1").arg(dplmacurrent, 0, 'f', 1);
			QString qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 1);
			QString qstripwidth100k	= QString("%1").arg(dipwidth100k, 0, 'f', 1);

			int row = mtwBestShot->rowCount();
			mtwBestShot->insertRow(row);
			mtwBestShot->setRowHeight(row,25);

			pItem = new QTableWidgetItem(strshotnum.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 0, pItem);

			pItem = new QTableWidgetItem(time);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 1, pItem);

			pItem = new QTableWidgetItem(qstrplmacur);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 2, pItem);

			pItem = new QTableWidgetItem(qstripwidth);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 3, pItem);

			pItem = new QTableWidgetItem(qstripwidth100k);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 4, pItem);

			pItem = new QTableWidgetItem(strcomment.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 5, pItem);

			pItem = new QTableWidgetItem(date);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			mtwBestShot->setItem(row, 6, pItem);
		};
		query.reset();
		strquery= "select max(plsmacurrent) from SHOTSUMMARY where plsmacurrent >= 100000.0";
		query << strquery;
		query.parse();
		res = query.use();
		while (row = res.fetch_row())
		{
			double dmaxplma = row["max(plsmacurrent)"]/1000.0;
			QString strmaxplma = QString("%1[kA]").arg(dmaxplma, 0, 'f', 1);
			mleTotalIp->setText(strmaxplma);

		};
		query.reset();
		strquery= "select max(plsmacurrent) from SHOTSUMMARY where plsmacurrent >= 100000.0 and shotdate > CURDATE()";
		query << strquery;
		query.parse();
		res = query.use();
		while (row = res.fetch_row())
		{
			double dmaxplma = row["max(plsmacurrent)"]/1000.0;
			QString strmaxplma = QString("%1[kA]").arg(dmaxplma, 0, 'f', 1);
			mleTodayIp->setText(strmaxplma);
		};

	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}
void BestShotList::cbChanged(int index)
{
	qDebug("Changed index:%d",index);
#if 1
	switch(index)
	{
		case 5:
			mdtStart->show();
			mdtEnd  ->show();
			mleStart ->hide();
			mleEnd ->hide();
			break;
		default:
			mdtStart->hide();
			mdtEnd  ->hide();
			mleStart ->show();
			mleEnd ->show();
			break;
	};
#endif
}
