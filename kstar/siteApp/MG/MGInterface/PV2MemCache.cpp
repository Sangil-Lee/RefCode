// pvsimpleCC.cc,v 1.1.1.1 2000/04/04 03:23:09 wlupton Exp
//
// Very simple C++ program to demonstrate pv classes

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <ext/hash_map>

#include "pv.h"
#include "libmemcached/memcached.h"


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
	KSTARInterface();
	~KSTARInterface();
	int WriteMemCachedPoint(const char *pvname, double value);
	int WriteMemCachedPoint(const char *pvname, string svalue);
	int Register(const string pvname);

	//data
private:
	vector<string> mpvlist;
	__gnu_cxx::hash_map<string, string> mtable;

private:
	void init();
	void maptable();

	memcached_server_st *servers;
	memcached_st *memc;    
	memcached_return rc;    
};

KSTARInterface::KSTARInterface():servers(0)
{
	init();
}

KSTARInterface::~KSTARInterface()
{
};

void KSTARInterface::init()
{
#if 0
	char *key = "PVCCC";    
	char *value = "45.678";    
	char *retvalue = NULL;    
	size_t retlength;    
	uint32_t flags;    
#endif

	/* Create an empty memcached interface */     
	memc = memcached_create(NULL);    

	/* Append a server to the list */                                               
	servers = memcached_server_list_append(servers, "172.18.54.136", 11211, &rc);    

	/* Update the memcached structure with the updated server list */     
	rc = memcached_server_push(memc, servers);    

	if (rc == MEMCACHED_SUCCESS)                                                    
		fprintf(stderr,"Successfully added server\n");    
	else                                  
		fprintf(stderr,"Couldn't add server: %s\n",memcached_strerror(memc, rc));    

}

int KSTARInterface::WriteMemCachedPoint(const char *pvname, double value)
{
	//printf("PVName(%s), Value(%f)\n", pvname, value);
	rc = memcached_set(memc, pvname, strlen(pvname), (char *)&value, sizeof(value), (time_t)0, (uint32_t)0);
	if (rc != MEMCACHED_SUCCESS)                                                                                              
		fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));      
	return 0;
}

int KSTARInterface::WriteMemCachedPoint(const char *pvname, string svalue)
{
	printf("PVName(%s), Value(%s)\n", pvname, svalue.c_str());
	rc = memcached_set(memc, pvname, strlen(pvname), svalue.c_str(), strlen(svalue.c_str()), (time_t)0, (uint32_t)0);
	if (rc != MEMCACHED_SUCCESS)                                                                                              
		fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));      
	return 0;
}

int KSTARInterface::Register(const string pvname)
{
	string dhpoint = string("DHP_")+pvname;
	mtable[pvname]=dhpoint;
	return 0;
}

void KSTARInterface::maptable()
{
#if 0
	mtable["MPS_PF1_BI0_01"]   = "DHP_MPS_PF1_BI0_01";
	mtable["MPS_PF2_BI0_01"]   = "DHP_MPS_PF2_BI0_01";
	mtable["MPS_PF3U_BI0_01"]  = "DHP_MPS_PF3U_BI0_01";
	mtable["MPS_PF3L_BI0_01"]  = "DHP_MPS_PF3L_BI0_01";
	mtable["MPS_PF4U_BI0_01"]  = "DHP_MPS_PF4U_BI0_01";
	mtable["MPS_PF4L_BI0_01"]  = "DHP_MPS_PF4L_BI0_01";
	mtable["MPS_PF5U_BI0_01"]  = "DHP_MPS_PF5U_BI0_01";
	mtable["MPS_PF5L_BI0_01"]  = "DHP_MPS_PF5L_BI0_01";
	mtable["MPS_PF6U_BI0_01"]  = "DHP_MPS_PF6U_BI0_01";
	mtable["MPS_PF6L_BI0_01"]  = "DHP_MPS_PF6L_BI0_01";
	mtable["MPS_PF7_BI0_01"]   = "DHP_MPS_PF7_BI0_01";
	mtable["MPS_PF1_IO_BI_00"] = "DHP_MPS_PF1_IO_BI_00";
	mtable["ICS_CURTIME"]	   = "DHP_ICS_CURTIME";
#endif
};

// end cogent

// event handler (get/put completion; monitors)
void doubleValEvent( void *obj, pvType type, int count, pvValue *val, void *arg, pvStat stat) 
{
	if(arg == NULL) return;

    pvVariable *var = ( pvVariable * ) obj;
	double dvalue = val->doubleVal[0];
    //printf( "doubleValEvent: %s=%f\n", var->getName(), dvalue);

	KSTARInterface *pKstar = (KSTARInterface*)arg;

	pKstar->WriteMemCachedPoint(var->getName(), dvalue);
}

void stringEvent( void *obj, pvType type, int count, pvValue *val, void *arg, pvStat stat) 
{
	if(arg == NULL) return;
    pvVariable *var = ( pvVariable * ) obj;
    printf( "stringEvent: %s=%s\n", var->getName(),val->charVal);

	return;
	KSTARInterface *pKstar = (KSTARInterface*)arg;
	string svalue = string(val->stringVal[0]);
	pKstar->WriteMemCachedPoint(var->getName(), svalue);
}

// main program
int main( int argc, char *argv[] ) 
{
    // system and variable names
    const string sysName = "ca";
	vector < string > pvlist;

	//MPS
#if 1
	pvlist.push_back("TEST_sinA");
	pvlist.push_back("TEST_sinB");
	pvlist.push_back("TEST_sinC");
#endif

	KSTARInterface *pKstar = new KSTARInterface();

    // create system
    pvSystem *sys = newPvSystem( sysName.c_str() );

    if ( sys == NULL ) 
	{
		printf( "newPvSystem( %s ) failure\n", sysName.c_str() );
		return -1;
    };

	for (size_t i = 0; i < pvlist.size(); i++)
	{
		//pKstar->Register(pvlist.at(i));
		pvVariable *var = sys->newVariable(pvlist.at(i).c_str());
		int sts = 0;
		sts = var -> monitorOn(pvTypeDOUBLE, 1, doubleValEvent, pKstar );
#if 0
		if(pvlist.at(i).compare("ICS_CURTIME")==0 || pvlist.at(i).compare("ICS_SHOTINFO_SHOTTIME")==0 ) 
			sts = var -> monitorOn(pvTypeSTRING, 1, stringEvent, pKstar );
		else
			sts = var -> monitorOn(pvTypeDOUBLE, 1, doubleValEvent, pKstar );
#endif
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

