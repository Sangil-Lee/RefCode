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
#include <mysql++.h>
#include <kstardbconn.h>
#include <iostream>
#include <fstream>
#include <QtGui>
#include <ext/hash_map>

#include "shotsummary.h"
#include "mdsplusdata.h"

using namespace std;
using namespace mysqlpp;

template <class T> class NBIPower
{
public:
	NBIPower(){};
	void operator() (T& elem) const 
	{
		int Vg1 = 0; float C = 0.;
		int choice = 3;
		switch(choice)
		{
			case 1:
				Vg1 = 70; C = 0.51;
				break;
			case 2:
				Vg1 = 80; C = 0.47;
				break;
			case 3:
				Vg1 = 90; C = 0.43;
				break;
			case 4:
			default:
				Vg1 = 95; C = 0.41;
				break;
			
		}
		elem=elem*Vg1*C/1000;
	};
};

template <class T> class ECHPower
{
public:
	ECHPower(){};
	//98x+39x^2
	void operator() (T& elem) const {elem=((98*elem)+(39*elem*elem));};
};

template <class T> class MultipleValue
{
	public:
		MultipleValue(const T& v):value(v){};
		void operator() (T& elem) const {elem*=value;};
	private:
		T value;
};


class ShotInfo
{
public:
	string paraname;
	double paramaxval;
};

class SessionUpdate
{
public:
	SessionUpdate();
	~SessionUpdate();
	int ReadMDSDataIpPulse(const int shotno);
	int ReadMDSDataECHPower(const int shotno);
	int ReadMDSDataNBIPower(const int shotno);
	int UpdateDataTe(const int shotno);
	void LoadTeData();
	static const int OK = 0; 
	static const int NOK = -1; 

private:
	MDSPlusData *m_mdsplus;
	MySQLKSTARDB mysqlkstardb;
	mysqlpp::Connection m_con;
	__gnu_cxx::hash_map<int, string> TeValues;
};

SessionUpdate::SessionUpdate()
{
	string dbname   = "kstarweb";
	string dbserverhost = "172.17.100.204";
	string user     = "root";
	string passwd   = "kstar2004";
	if ( get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
	};

	if( !mysqlkstardb.Connect_to_db(dbname, dbserverhost, user, passwd, m_con) )
	{
		cerr<< "MySQL Database Connection Failed. Check Database Server or Network." << endl;
	};
};

SessionUpdate::~SessionUpdate()
{
};

void SessionUpdate::LoadTeData()
{
	ifstream file("TeData.txt");
	if ( file.fail () )
	{
		qDebug("TeData.txt filename not found");
		return;
	};

	string strToken;
	while (!file.eof ())
	{
		getline (file, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		int index = 0;
		int shotno = 0;
		string str_shotvalue;
		while (pch != NULL)
		{
			switch(index)
			{
				case 0:
					//qDebug("ShotNum:%s", pch);
					shotno = atoi(pch);
					break;
				case 1:
					//qDebug("Value:%s", pch);
					str_shotvalue = string(pch);
					break;
			};
			if(!(pch = strtok (NULL," \t,"))) continue;
			index++;
		};
		TeValues[shotno]=str_shotvalue;
	};
}
int SessionUpdate::UpdateDataTe(const int shotno)
{
	//Read Te ascii data file and update DB
#if 0
	pItemcomm = m_ptable->item(currow, 7);
	strValue = pItemcomm -> text();
	query2 << "update SHOTSUMMARY set etemp=%0:etemp where shotnum=%1:shotnum"; //OK
	query2.parse();
	query2.def["etemp"]=strValue.toDouble();
#endif
	qDebug("TeValues[%d]=%s",shotno, TeValues[shotno].c_str());
	//DB Update
	try
	{
		Query query = m_con.query();
		sql_bigint sqlshotno = shotno;
		query << "update SHOTSUMMARY set etemp=%0:etemp where shotnum=%2:shotnum"; //OK
		query.parse();
		query.def["etemp"]=strtod(TeValues[shotno].c_str(),NULL);
		query.def["shotnum"]=sqlshotno;
		cout<<"Query: " << query.preview() << endl;

		ResNSel sqlres = query.execute();
		if(sqlres.success)
			printf("[]- SessionSummary Updated:%d\n", shotno);
		else
			printf("[]- SessionSummary Update Failed:%d\n", shotno);

	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return NOK;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return NOK;
	};

	return OK;
}
int SessionUpdate::ReadMDSDataNBIPower(const int shotno)
{
	MDSPlusData *mdsplus = new MDSPlusData;
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	ShotInfo shotinfo;
	double mxVal = 0;
	//string strPara = "NB1_PB1";
	string strPara = "NB11_ig1";
	bool check;
	mdsplus->setMDSDataInfo("heating",strPara,shotno,check);
	if(check==MDSPlusData::FAIL) 
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		return NOK;
	};

	size = mdsplus->getNodeSize();
	timebase = new double[size];
	shotdata = new double[size];

	if(mdsplus->getShotData(timebase, shotdata, size)<0)
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		delete timebase;
		delete shotdata;
		return NOK;
	};

	for_each(&shotdata[0], &shotdata[size], NBIPower<double>());
	mxVal = *max_element(&shotdata[0], &shotdata[size]);

	printf("NBIPower-Size:%d\n",size);
	printf("Max of [%s]:%f\n",strPara.c_str(), mxVal);

	shotinfo.paraname = strPara;
	shotinfo.paramaxval = mxVal;
	delete timebase;
	delete shotdata;

	//DB Update
	try
	{
		Query query = m_con.query();
		sql_bigint sqlshotno = shotno;
		query << "update SHOTSUMMARY set nbi1power=%0:nbi1power where shotnum=%2:shotnum"; //OK
		//query << "update SHOTSUMMARY set ipwidth=%0:ipwidth where shotnum=%1:shotnum"; //OK
		query.parse();
		query.def["nbi1power"]=mxVal;
		query.def["shotnum"]=sqlshotno;
		cout<<"Query: " << query.preview() << endl;

		ResNSel sqlres = query.execute();
		if(sqlres.success)
			printf("[]- SessionSummary Updated:%d\n", shotno);
		else
			printf("[]- SessionSummary Update Failed:%d\n", shotno);

	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return NOK;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return NOK;
	};

	delete mdsplus;
	return OK;
}
int SessionUpdate::ReadMDSDataECHPower(const int shotno)
{
	MDSPlusData *mdsplus = new MDSPlusData;
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	ShotInfo shotinfo;
	double mxVal = 0;
	string strPara = "ECH_VFWD1:FOO";
	bool check;
	mdsplus->setMDSDataInfo("heating",strPara,shotno,check);
	if(check==MDSPlusData::FAIL) 
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		return NOK;
	};

	size = mdsplus->getNodeSize();
	timebase = new double[size];
	shotdata = new double[size];

	if(mdsplus->getShotData(timebase, shotdata, size)<0)
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		delete timebase;
		delete shotdata;
		return NOK;
	};

	//98x+39x^2
	for_each(&shotdata[0], &shotdata[size], ECHPower<double>());
	mxVal = *max_element(&shotdata[0], &shotdata[size]);

	printf("ECHPower-Size:%d\n",size);
	printf("Max of [%s]:%f\n",strPara.c_str(), mxVal);

	shotinfo.paraname = strPara;
	shotinfo.paramaxval = mxVal;
	delete timebase;
	delete shotdata;

	//DB Update
	try
	{
		Query query = m_con.query();
		sql_bigint sqlshotno = shotno;
		query << "update SHOTSUMMARY set echpow=%0:echpow where shotnum=%2:shotnum"; //OK
		//query << "update SHOTSUMMARY set ipwidth=%0:ipwidth where shotnum=%1:shotnum"; //OK
		query.parse();
		query.def["echpow"]=mxVal;
		query.def["shotnum"]=sqlshotno;
		cout<<"Query: " << query.preview() << endl;

		ResNSel sqlres = query.execute();
		if(sqlres.success)
			printf("[]- SessionSummary Updated:%d\n", shotno);
		else
			printf("[]- SessionSummary Update Failed:%d\n", shotno);

	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return NOK;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return NOK;
	};

	delete mdsplus;
	return OK;
}
int SessionUpdate::ReadMDSDataIpPulse(const int shotno)
{
	MDSPlusData *mdsplus = new MDSPlusData;
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	ShotInfo shotinfo;
	double mxVal = 0;
	string strPara = "PCRC03";
	bool check;
	vector<double> dtimebase;
	vector<double> dtimebase100k;
	double preval = 0.0;
	mdsplus->setMDSDataInfo("pcs_kstar",strPara,shotno,check);
	if(check==MDSPlusData::FAIL) 
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		return NOK;
	};

	size = mdsplus->getNodeSize();
	timebase = new double[size];
	shotdata = new double[size];

	if(mdsplus->getShotData(timebase, shotdata, size)<0)
	{
		shotinfo.paraname = strPara;
		shotinfo.paramaxval = 0;
		delete timebase;
		delete shotdata;
		return NOK;
	};

	for_each(&shotdata[0], &shotdata[size], MultipleValue<double>(-1.0));
	mxVal = *max_element(&shotdata[0], &shotdata[size]);
	unsigned int count = (size<140000+500)?0:140000+500; //2011
	unsigned int endloop = (count == 0)?size:count+130000;

#if 0
	ofstream outfile("./Ip_Test.txt");
	for(int i = 0; i < size; i++)
	{
		printf("%i  X:%f  Y:%f, Count:%d", i, timebase[i], shotdata[i],count);
		outfile <<"count: " <<i<<" X: "<<timebase[i]<<" Y: "<<shotdata[i]<<endl;
	};
	printf("Ip-Size:%d, EndLoop:%d\n",size, endloop);
	outfile.close();
	return -1;
#endif

	printf("Ip-Size:%d, StartLoop: %d, EndLoop:%d\n",size,count, endloop);

	// check 
	double dshotval = mxVal;
	if( mxVal < 0 )
	{
		//for(unsigned int i = count; i<(count+130000); i++)
		for(unsigned int i = count; i<endloop; i++)
		{
			dshotval = fabs(shotdata[i]);
			if(dshotval>=100000.0)//100kA	2011
			{
				dtimebase.push_back(timebase[i]);
				preval = dshotval;
				if(dshotval>=100000.0)//100kA
				{
					dtimebase100k.push_back(timebase[i]);
				};
			}
		};
	}
	else
	{
		//for(unsigned int i = count; i<(count+130000); i++) // 2011
		for(unsigned int i = count; i<endloop; i++)
		{
			dshotval = shotdata[i];
			if(dshotval>=100000.0)//100kA 2011
			{
				dtimebase.push_back(timebase[i]); //2010
				if(dshotval>=100000.0)//100kA
				{
					dtimebase100k.push_back(timebase[i]); //2010
				};
			};
		};
	};

	printf("Max of [%s]:%f\n",strPara.c_str(), mxVal);
	shotinfo.paraname = strPara;
	shotinfo.paramaxval = mxVal;
	delete timebase;
	delete shotdata;

	double mxTime = 0.0;
	double mnTime = 0.0;
	if(dtimebase.empty() == false) 
	{
		mxTime = *max_element(dtimebase.begin(), dtimebase.end());
		mnTime = *min_element(dtimebase.begin(), dtimebase.end());
	};

	double timeWidth = 0.0;
	timeWidth = mxTime-mnTime;

	double mxTime100k = 0.0;
	double mnTime100k = 0.0;
	if(dtimebase100k.empty() == false) 
	{
		mxTime100k = *max_element(dtimebase100k.begin(), dtimebase100k.end());
		mnTime100k = *min_element(dtimebase100k.begin(), dtimebase100k.end());
	};

	double timeWidth100k = mxTime100k-mnTime100k;

	printf("Ip-Pulsewidth >0.1K:maxTime:%f, minTime:%f, timewidth(sec):%f\n",mxTime, mnTime,timeWidth);
	printf("Ip-Pulsewidth >100K:maxTime100k:%f, minTime100k:%f, timewidth100k(sec):%f\n",mxTime100k, mnTime100k,timeWidth100k);

	//DB Update
	try
	{
		Query query = m_con.query();
		sql_bigint sqlshotno = shotno;
		query << "update SHOTSUMMARY set ipwidth=%0:ipwidth, ipwidth100k=%1:ipwidth100k where shotnum=%2:shotnum"; //OK
		//query << "update SHOTSUMMARY set ipwidth=%0:ipwidth where shotnum=%1:shotnum"; //OK
		query.parse();
		query.def["ipwidth"]=timeWidth;
		query.def["ipwidth100k"]=timeWidth100k;
		query.def["shotnum"]=sqlshotno;
		cout<<"Query: " << query.preview() << endl;

		ResNSel sqlres = query.execute();
		if(sqlres.success)
			printf("[]- SessionSummary Updated:%d\n", shotno);
		else
			printf("[]- SessionSummary Update Failed:%d\n", shotno);

	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return NOK;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return NOK;
	};

	delete mdsplus;
	return OK;
}

int main(int argc, char *argv[])
{
	int c;

	string strusage = "USAGE: SessionUpdate -s [StartShotNUM] -e [EndShotNum] -f [DBField, 0:IpWidth, 1:ECHPower, 2:NBIPower, 3:Te]";

	int startshot = 0;
	int endshot = 0;
	int field = 0;

	if(argc <= 1)
	{
		printf("%s\n", strusage.c_str());
		exit(0);
	}

	while((c=getopt(argc, argv, "s:e:f:")) != -1)
	{
		switch(c) {
			case 's':
				startshot = atoi(optarg);
				break;
			case 'e':
				endshot = atoi(optarg);
				break;
			case 'f':
				 field = atoi(optarg);
				 break;
			case '?':
			default:
				printf("%s\n", strusage.c_str());
				exit(0);
		};
	};

	printf("StartShot:%d, EndShot:%d>>>>>>\n", startshot, endshot);
	SessionUpdate	session;
	if(field == 3) session.LoadTeData();
	for( int i = startshot; i <= endshot; i++)
	{
		switch(field)
		{
			case 1:
				session.ReadMDSDataECHPower(i);
				break;
			case 2:
				session.ReadMDSDataNBIPower(i);
				break;
			case 3:
				session.UpdateDataTe(i);
				break;
			case 0:
			default:
				session.ReadMDSDataIpPulse(i);
				break;
		}
		usleep(10000);
	};
	printf("<<<<<Update End>>>>>>>\n");

	return 0;
}

