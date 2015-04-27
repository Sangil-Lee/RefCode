#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <ext/hash_map>

#include "pv.h"

using namespace std;

#if 1
namespace __gnu_cxx {
	template <>
		struct hash<std::string> {
			size_t operator() (const std::string& x) const {
				return hash<const char*>()(x.c_str());
			}
		};
};
#endif

class KSTARInterface 
{
public:
	KSTARInterface(pvSystem *sys);
	~KSTARInterface();
	int WriteICSPV(string pvname, double value);
	int WriteICSPV(string pvname, string svalue);
	void ICSGWList(vector<string> &gwlist);

	//data
private:
	vector<string> mpvlist;
	__gnu_cxx::hash_map<string, pvVariable *> mappoint;

private:
	void init();
	pvSystem *mpsys;
};

KSTARInterface::KSTARInterface(pvSystem *sys):mpsys(sys)
{
	init();
}

KSTARInterface::~KSTARInterface()
{
};

void KSTARInterface::init()
{
}

void KSTARInterface::ICSGWList(vector<string> &gwlist)
{
	for (size_t i = 0; i < gwlist.size(); i++)
	{
		string pvname = gwlist.at(i);
		size_t posdel = pvname.find(":");
		if(posdel == string::npos) continue;

		pvname.replace(posdel, 1, "_");
		transform(pvname.begin(), pvname.end(), pvname.begin(), ::toupper);
		string epicspvname = string("ICS_") + pvname;
		pvVariable *var = mappoint[epicspvname];
		if(!var)
		{
			var = mpsys->newVariable(epicspvname.c_str());
			epicsThreadSleep(0.05);
			if(!var) continue;
			mappoint[epicspvname] = var;
		};
	};
}

int KSTARInterface::WriteICSPV(string name, double value)
{
	if(name.empty() == true) return (-1);
	string pvname = name;

	size_t posdel = pvname.find(":");
	if(posdel == string::npos) return (-1);

	pvname.replace(posdel, 1, "_");
	transform(pvname.begin(), pvname.end(), pvname.begin(), ::toupper);

	string epicspvname = string("ICS_") + pvname;

	pvVariable *var = mappoint[epicspvname];
	if(!var)
	{
		var = mpsys->newVariable(epicspvname.c_str());
		mpsys->pend(0.05, TRUE);
		//epicsThreadSleep(0.05);
		if(!var) return (-1);
		
		mappoint[epicspvname] = var;
		//printf("New Variable[%s:%p]:%f\n", epicspvname.c_str(), var, value);
	};

	pvValue pvVal;
	pvVal.doubleVal[0] = value;

#if 0
	printf("PVName[%s]:%f\n", epicspvname.c_str(), pvVal.doubleVal[0]);
#endif
	if(!var->getConnected())return(-1);

	//printf("PV(%s):->value(%f)\n",epicspvname.c_str(), value);
	var->put(pvTypeDOUBLE, 1, &pvVal);

	return 0;
}

int KSTARInterface::WriteICSPV(string pvname,  string svalue)
{
	return 0;
}


// event handler (get/put completion; monitors)
void doubleValEvent( void *obj, pvType type, int count, pvValue *val, void *arg, pvStat stat) 
{
    pvVariable *var = ( pvVariable * ) obj;
    //printf( "doubleValEvent: %s=%g\n", var->getName(), val->doubleVal[0] );
	if(arg == NULL) return;
	KSTARInterface *pKstar = (KSTARInterface*)arg;
	pKstar->WriteICSPV(var->getName(), val->doubleVal[0] );
}

void stringEvent( void *obj, pvType type, int count, pvValue *val, void *arg, pvStat stat) 
{
    pvVariable *var = ( pvVariable * ) obj;
    //printf( "stringEvent: %s=%s\n", var->getName(), val->stringVal[0] );
	if(arg == NULL) return;
	KSTARInterface *pKstar = (KSTARInterface*)arg;
	string svalue = string(val->stringVal[0]);
	pKstar->WriteICSPV(var->getName(), svalue);
}

// main program
int main( int argc, char *argv[] ) 
{
	setenv("EPICS_CA_ADDR_LIST", "172.17.100.198 172.17.100.199 172.17.100.130 172.17.250.101", 1);
	setenv("EPICS_CA_AUTO_ADDR_LIST", "no", 1);

    // system and variable names
    const string sysName = "ca";
	vector < string > pvlist;
	//GW1
	pvlist.push_back("cagateway1:active");
	pvlist.push_back("cagateway1:inactive");
	pvlist.push_back("cagateway1:connecting");
	pvlist.push_back("cagateway1:dead");
	pvlist.push_back("cagateway1:connected");
	pvlist.push_back("cagateway1:unconnected");
	pvlist.push_back("cagateway1:vctotal");
	pvlist.push_back("cagateway1:pvtotal");
	pvlist.push_back("cagateway1:clientEventRate");
	pvlist.push_back("cagateway1:clientPostRate");
	pvlist.push_back("cagateway1:serverEventRate");
	pvlist.push_back("cagateway1:serverPostRate");

	//GW2
	pvlist.push_back("cagateway2:active");
	pvlist.push_back("cagateway2:inactive");
	pvlist.push_back("cagateway2:connecting");
	pvlist.push_back("cagateway2:dead");
	pvlist.push_back("cagateway2:connected");
	pvlist.push_back("cagateway2:unconnected");
	pvlist.push_back("cagateway2:vctotal");
	pvlist.push_back("cagateway2:pvtotal");
	pvlist.push_back("cagateway2:clientEventRate");
	pvlist.push_back("cagateway2:clientPostRate");
	pvlist.push_back("cagateway2:serverEventRate");
	pvlist.push_back("cagateway2:serverPostRate");

	//OFFICEGW
	pvlist.push_back("officegw:active");
	pvlist.push_back("officegw:inactive");
	pvlist.push_back("officegw:connecting");
	pvlist.push_back("officegw:dead");
	pvlist.push_back("officegw:connected");
	pvlist.push_back("officegw:unconnected");
	pvlist.push_back("officegw:vctotal");
	pvlist.push_back("officegw:pvtotal");
	pvlist.push_back("officegw:clientEventRate");
	pvlist.push_back("officegw:clientPostRate");
	pvlist.push_back("officegw:serverEventRate");
	pvlist.push_back("officegw:serverPostRate");

    // create system
    pvSystem *sys = newPvSystem( sysName.c_str() );

    if ( sys == NULL ) 
	{
		printf( "newPvSystem( %s ) failure\n", sysName.c_str() );
		return -1;
    };

	KSTARInterface *pKstar = new KSTARInterface(sys);
	pKstar->ICSGWList(pvlist);
	for (size_t i = 0; i < pvlist.size(); i++)
	{
		pvVariable *var = sys->newVariable(pvlist.at(i).c_str());
		int sts = 0;
		sts = var -> monitorOn(pvTypeDOUBLE, 1, doubleValEvent, pKstar );
		if ( sts ) {
			printf( "%s->monitorOn() failure\n", pvlist.at(i).c_str() );
		};
	};

	//forever
	while(1)
		sys->pend(1, TRUE );

	printf("After pend... exit \n" );
    // tidy up
	//delete var;
	//delete sys;
    return 0;
}

