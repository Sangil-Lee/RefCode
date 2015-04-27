#ifndef __FAULTLIST_ADD_H
#define __FAULTLIST_ADD_H
#include <QtGui>
#include <QObject>
#include <QUiLoader>
#include <mysql++.h>
#include <iostream>
#include <kstardbconn.h>
#include "faultlistsummary.h"

using namespace std;

class FaultListSummary;

class AddFaultList:public QWidget
{
	Q_OBJECT
public:
	AddFaultList(bool modify=false);
	virtual ~AddFaultList();
	void setParent(QWidget *parent);
	void setItemString(vector<string> &itemlist);
	void updateCurtime();

public slots:
	void dbSearch();
	void Close();
	void InsertUpdate();
	void Reset();

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	int messageBox(QString msg, const int type=1);

	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};
	//enum FAULTLIST{NUMBER=0, SUBSYSTEM,FDATE,FTYPE,FDESC,RDATE,RDESC,ILEVEL,IDESC,WRITER,COMMENT,REFSHOTNUM,WDATE};
	enum FAULTLIST{NUMBER=0,SNAME,SCOMPONENT,SFUNCTION,FDATE,FCLASS,FNAME,FSEVERITY,FCAUSE,FEFFECT,RDATE,RDESC,
		RDURATION,ILEVEL,IDESC,REFSHOTNUM,MTBF,WRITER,COMMENT,WDATE};

	QWidget *m_pFaultList;
	FaultListSummary *m_pParent;
	QUiLoader	loader;
	QComboBox	*mcbSubsystem;
	QComboBox	*mcbFType;
	QComboBox	*mcbILevel;
	QDateTimeEdit	*mdtFDate;
	QDateTimeEdit	*mdtRDate;
	QTextEdit		*mteRDesc,*mteIDesc,*mteComment;
	QPushButton		*mpbAdd;
	QLabel			*mlbTitle;
	QLineEdit		*mleWriter,*mleCompo,*mleFunc,*mleFName,*mleFCause,*mleFEffect,*mleRDuration;
	QSpinBox		*msbRefShotNo,*msbFSeverity,*msbMTBF;
	vector<string>	mitemlist;
	mysqlpp::Connection m_con;
	const bool mmodify;
};
#endif
