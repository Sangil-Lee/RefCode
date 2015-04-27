#ifndef __FAULTLIST_SUMMARY_H
#define __FAULTLIST_SUMMARY_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mysql++.h>
#include <iostream>
#include <kstardbconn.h>
#include "faultlistadd.h"

using namespace std;

class AddFaultList;

class FaultListSummary:public QWidget
{
	Q_OBJECT
public:
	FaultListSummary();
	virtual ~FaultListSummary();

	int InsertFaultList(string& sname, string& scomponent, string& sfunction, mysqlpp::sql_datetime& fdatetime, string& fclass,
	string& fname, int& fseverity, string& fcause, string& feffect, mysqlpp::sql_datetime& rdatetime, string& rdesc, string& rduration, string& intlevel,
	string& intdesc, int& refshotnum, int& mtbf, string& writer, string& comment, mysqlpp::sql_datetime& wdatetime);

	mysqlpp::Connection & getConnection() {return m_con;};

	enum MSGRE{MSGNOK=-1,MSGOK=0};
	enum MSGBOXTYPE{APPLYCANCELBOX=0,OKBOX=1};
	enum FAULTLIST{NUMBER=0,SNAME,SCOMPONENT,SFUNCTION,FDATE,FCLASS,FNAME,FSEVERITY,FCAUSE,FEFFECT,RDATE,RDESC,
		RDURATION,ILEVEL,IDESC,REFSHOTNUM,MTBF,WRITER,COMMENT,WDATE};

	enum ESUBSYS{GEP=0,CWF,HRS,HDS_VPS,HDS_HCS,CLS_VPS,CLS_HCS,TMS_TEMP,TMS_STRT,VMS_VV,VMS_CR,MPS_TF,MPS_PF1,MPS_PF2,MPS_PF3,MPS_PF4,
		MPS_PF5,MPS_PF6,MPS_PF7,ECH,ICRH,DDS1,DDS2,MD,ECER,MMW_I,HAM,FS,VSS,DTV,SIS,PSI,RMS,CCS,PCS,ICS,SSYSETC};
	enum EFCLASS{HW=0,SW,HWSW,OP,SEQ,BSAG,BINT,UTIL,INSTALL,UC,UNKNOWN,NONCLASS,SECONDARY,FETC};
	enum ELEVEL{L1=0,L2,L3,L4,WARNNING,NONE};

public slots:
	void dbQuery();
	void dbQueryReset();
	void dbFaultlist(bool init);
	void print();
	void saveFile();

	void addFaultlist();
	void addLine();
	void modifyLine();
	void copyFaultlist();
	void modifyFaultlist();
	void deleteFaultlist();
	void dailyReport();
	void columnHideShow(int index);

signals:
	void Search(bool init=false);

protected:

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	int messageBox(QString msg, const int type=1);
	void exportText(QFile &file);
	void exportPDF(QString exportname);

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QTableWidget *m_ptable;
	QComboBox *mcbSubSystem, *mcbFaultType, *mcbLevel, *mcbDescType, *mcbDateType;
	QComboBox *mcbColShowHide, *mcbColumn,*mcbWriterShot;
	QLineEdit *mleDesc,*mleWriterSNS, *mleShotNE;
	QDateTimeEdit *mdtStart, *mdtEnd;

	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;
	int	maddline;
	bool mbaddmode;

};
#endif
