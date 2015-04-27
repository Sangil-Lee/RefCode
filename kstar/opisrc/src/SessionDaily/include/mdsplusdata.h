#ifndef _MDSPLUSDATA_H_
#define _MDSPLUSDATA_H_
#include <stdio.h>
#include <string>
#include <cmath>
#include <numeric>
#include <mdslib.h>
#include <mysql++.h>

#include "kstardbconn.h"

using namespace std;

class MDSPlusData
{
	//member function.
public:
	MDSPlusData();
	MDSPlusData(const string nodename, const string treename, const int shot);
	virtual ~MDSPlusData();
	
	int getShotData();
	string getShotTime();
	int getShotData(double *timebase, double *shotdata, int size);
	void setMDSDataInfo(const string nodename, const string treename, const int shot, bool &check);
	int mdsConnection(const string stlurlport="172.17.100.200:8300");
	int getNodeSize() {return msize;};
	enum CHECK{FAIL=0, OK};

private:
	int getNodeLen();
	
	//member variable.
private:
	string mnodename;
	string mtreename;
	int mshot;
	int msize;
	double *mtimebase;
	double *mshotdata;
	int	msocket;
};
#endif
