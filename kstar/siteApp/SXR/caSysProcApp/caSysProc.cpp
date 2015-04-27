#include <stddef.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

#include "cadef.h"

using std::vector;
using std::string;

vector<string> sysProcList;

enum SYSTEMPROC {
	CAREPEATER  = 0x00000001,
	NTPD        = 0x00000001<<1,
	HTTPD       = 0x00000001<<2,
	JAVA        = 0x00000001<<3,
	NSRD        = 0x00000001<<4,
	MMFSD       = 0x00000001<<5,
	MYSQLD      = 0x00000001<<6,
	MDSIP       = 0x00000001<<7,
	GATEWAY     = 0x00000001<<8,
	NIMXS       = 0x00000001<<9,
	CASYSPROC   = 0x00000001<<10
};

static int checkProcess(const string processname)
{
	string checkproc = string("pidof ") + processname;
	char * buff = NULL;
	size_t len = 0;
	ssize_t read;

	FILE *fp = NULL;
	fp = popen (checkproc.c_str(), "r");
	fflush(fp);
	if (fp == NULL)
	{
		fprintf(stderr,"Cannot execute %s\n", checkproc.c_str());
		pclose(fp);
		return 0;
	};

	int retVal = 0;
	while ((read = getline(&buff, &len, fp)) != -1)
	{
		char *token = NULL;
		token = strtok(buff, " ");
		string strCmd = string(token);
		retVal = strtol(token, NULL, 10);
	};
	if(buff) free(buff);
	pclose(fp);
	return retVal;
};

static unsigned int getSysProcessStatus()
{
	vector<string>::iterator svclistiter;
	unsigned int procstatus = 0;
	for(svclistiter=sysProcList.begin(); svclistiter!=sysProcList.end();++svclistiter)
	{
		string strprocess = svclistiter->c_str();
		int check = checkProcess(strprocess.c_str());
		if(strprocess.compare("caRepeater") == 0)
		{
			if (check == 0)
				procstatus |= CAREPEATER;
			else
				procstatus &= ~CAREPEATER;
		}
		else if(strprocess.compare("ntpd") == 0)
		{
			if (check == 0)
				procstatus |= NTPD;
			else
				procstatus &= ~NTPD;
		}
		else if(strprocess.compare("nimxs") == 0)
		{
			if (check == 0)
				procstatus |= NIMXS;
			else
				procstatus &= ~NIMXS;
		}
		else if(strprocess.compare("caSysProc") == 0)
		{
			if (check == 0)
				procstatus |= CASYSPROC;
			else
				procstatus &= ~CASYSPROC;
		}
		//printf("%s(%d)\n", strprocess.c_str(), check);
	};
	return procstatus;
}
template <class T> int caPut(const int type, const string pvname, T &value = 0)
{
	chid  chan_id;
	int status = ca_search(pvname.c_str(), &chan_id);

	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		printf("Not Found PV(%s)\n", pvname.c_str());
		return status;
	};

	switch(type)
	{
		case DBR_INT:
			{
			int intval = value;
			status = ca_put(DBR_INT, chan_id, &intval);
			}
			break;
		case DBR_LONG:
			{
			long longval = (long) value;
			status = ca_put(DBR_LONG, chan_id, &longval);
			};
			break;
		case DBR_FLOAT:
			{
			float fval = (float) value;
			status = ca_put(DBR_FLOAT, chan_id, &fval);
			};
			break;
		case DBR_DOUBLE:
			{
			double dval = value;
			status = ca_put(DBR_DOUBLE, chan_id, &dval);
			};
			break;
		case DBR_CHAR:
			{
			char cval = value;
			status = ca_put(DBR_CHAR, chan_id, &cval);
			};
			break;
		default:
			break;
	};

	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT) {
		printf("caPut timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	return status;
};



int main(int argc,char **argv)
{
	setenv("EPICS_CA_ADDR_LIST", "localhost", 1);
	setenv("EPICS_CA_AUTO_ADDR_LIST", "no", 1);
#if 0
	if(checkProcess("caSysProc")!=0)
	{
		printf("caSysProc already exists!\n");
		exit(0);
	}
#endif

	sysProcList.push_back("caRepeater");
	sysProcList.push_back("ntpd");
	sysProcList.push_back("nimxs");
	sysProcList.push_back("caSysProc");

    SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");
	unsigned int sysprocbit = 0, oldsysprocbit = 0;
	static bool first = true;
	while(true)
	{
		if(first)
		{
			oldsysprocbit = sysprocbit = getSysProcessStatus();
			caPut(DBR_INT,"SXR_SYS_PROC", sysprocbit);
		}
		else
		{
			sysprocbit = getSysProcessStatus();
			if(oldsysprocbit != sysprocbit)
			{
				caPut(DBR_INT,"SXR_SYS_PROC", sysprocbit);
			};
		};
		oldsysprocbit = sysprocbit;
		//printf("%d\n",sysprocbit);
		sleep(5);
	};
    ca_context_destroy();
    return(0);
}
