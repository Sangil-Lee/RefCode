#include <string>
#include <cmath>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "cadef.h"
#include "valueToStr.h"

using namespace std;
using namespace xmlrpc_c;

int main(int argc, char *argv[]) 
{
#if 1
	int     c = 0;
	string mod;
	vector<string> strhostlist;
	strhostlist.push_back("opi01"); strhostlist.push_back("opi02"); strhostlist.push_back("opi03");
	strhostlist.push_back("opi04"); strhostlist.push_back("opi05"); strhostlist.push_back("opi06");
	strhostlist.push_back("opi07"); strhostlist.push_back("opi08"); strhostlist.push_back("opi09");
	strhostlist.push_back("opi10"); strhostlist.push_back("opi11"); strhostlist.push_back("opi12");
	strhostlist.push_back("opi13"); strhostlist.push_back("opi14"); strhostlist.push_back("opi15");
	strhostlist.push_back("opi17"); strhostlist.push_back("opi18"); strhostlist.push_back("opi19");
	strhostlist.push_back("da"); strhostlist.push_back("ca"); strhostlist.push_back("mdsplus");
	strhostlist.push_back("backup"); strhostlist.push_back("web");

	vector<string> cmdlist;
	cmdlist.push_back("opiupdate");
	cmdlist.push_back("opipluginupdate");
	cmdlist.push_back("opiinstall");
	cmdlist.push_back("shutdown");
	cmdlist.push_back("reboot");
	cmdlist.push_back("remotestop");
	cmdlist.push_back("remotestart");
	cmdlist.push_back("servicechecker");

	vector<string> serverurllist;
	char *hostlist = 0;
	char *command = 0;
	while ((c = getopt (argc, argv, "h:c:")) !=  -1) 
	{
		switch (c) {
			case  'h':
				hostlist = optarg;
				break;
			case  'c':
				command = optarg;
				break;
			case  '?':
				printf("USAGE: remoteopicontrol -h [remoteopi_IP:... or remoteopi_Hostname:...] -c command[opiupdate or opipluginupdate or opiinstall or shutdown]\n");
				return -1;
		};
	};

	//argument index, count error code

	if (argc != 5 )
	{
		printf("USAGE: remoteopicontrol -h [remoteopi_IP:... or remoteopi_Hostname:...] -c command[opiupdate or opipluginupdate or opiinstall or shutdown]\n");
		return -1;
	};

	char com_buf[1024];
	strcpy(com_buf, command);
	char *com_tok;
	com_tok = strtok ( com_buf, ":" );
	int com_cnt = 0;
	string strcommand = com_tok;
	vector<string> programlist;
	string str_tok;
	if ( strcasecmp(strcommand.c_str(), "remotestart" ) == 0  || 
		strcasecmp(strcommand.c_str(), "remotestop" ) == 0 )
	{
		while ( com_tok != NULL )
		{
			printf("[%d]:%s\n",com_cnt, com_tok);
			str_tok = com_tok;
			programlist.push_back(str_tok);
			com_tok = strtok ( NULL, ":" );
			com_cnt++;
		};
	};

	vector<string>::iterator cmdlistiter;
	cmdlistiter = find(cmdlist.begin(), cmdlist.end(),strcommand);

	if ( cmdlistiter == cmdlist.end () )
	{
		printf("Invalid command\n");
		printf("USAGE: remoteopicontrol -h [remoteopi_IP:... or remoteopi_Hostname:...] -c command[opiupdate or opipluginupdate or opiinstall or shutdown]\n");
		return -1;
	};
	
	vector<string> vec_serverUrl;
	const string strProtocol = "http://";
	const string strport = ":8081/RPC2";
	string serverUrl;
	if ( strcasecmp(hostlist, "all" ) == 0 )
	{
		for (size_t i = 0; i < strhostlist.size();i++)
		{
			serverUrl = strProtocol+strhostlist.at(i)+strport;
			vec_serverUrl.push_back(serverUrl);
		};
	}
	else
	{
		char buf[1024];
		strcpy(buf, hostlist);
		char *tok;
		tok = strtok ( buf, ":" );
		int hostcnt = 0;
		while ( tok != NULL )
		{
			printf("[%d]:%s\n",hostcnt, tok);
			serverUrl = strProtocol+string(tok)+strport;
			vec_serverUrl.push_back(serverUrl);
			tok = strtok ( NULL, ":" );
			hostcnt++;
		};
	};

	clientSimple clientlist[vec_serverUrl.size()];
	value rpcresultlist[vec_serverUrl.size()];
	const string methodName = string("localCmd.") + strcommand;

	if ( strcasecmp(strcommand.c_str(), "shutdown" ) == 0 )
	{
		string yesorno;
		cout << "Do you want to shutdown(y/n)?:";
		getline(cin, yesorno);
		if (strcasecmp(yesorno.c_str(), "n" ) == 0 ) 
		{
			printf("Not correct password\n");
			return -1;
		};
	};
	
	vector<string> prglist;
	prglist.push_back("MenuWindow");
	prglist.push_back("TMS");
	prglist.push_back("VMS");
	prglist.push_back("SCS");
	prglist.push_back("DDS");
	prglist.push_back("ICRH");
	prglist.push_back("Fuel");
	prglist.push_back("java");
	
	if ( strcasecmp(strcommand.c_str(), "remotestop" ) == 0 ||
			strcasecmp(strcommand.c_str(), "remotestart" ) == 0 )
	{
		size_t prgcnt = programlist.size();
		if( prgcnt <= 1 )
		{
			printf("USAGE: remoteopicontrol -h [remoteopi_IP:... or remoteopi_Hostname:...] -c [remotestop | remotestart]:[MenuWindow|TMS|VMS|SCS|DDS|ICRH|Fuel]\n");
			return -1;
		};
		vector<string>::iterator prglistiter;
		prglistiter = find (prglist.begin(), prglist.end(), programlist.at(1));

		if (prglistiter == prglist.end() )
		{
			printf("Error:%s is no in remote execution program list\n", programlist.at(1).c_str() );
			return -1;
		};
	};

	for ( size_t i = 0; i < vec_serverUrl.size();i++)
	{
		try {
			if ( strcasecmp(strcommand.c_str(), "remotestop" ) == 0 )
			{
				clientlist[i].call(vec_serverUrl.at(i), methodName, "s", &rpcresultlist[i], 
						programlist.at(1).c_str());
			}
			else if ( strcasecmp(strcommand.c_str(), "remotestart" ) == 0 )
			{
				string arglist = (programlist.size() <= 2)? "":programlist.at(2);
				clientlist[i].call(vec_serverUrl.at(i), methodName, "ss", &rpcresultlist[i],
						programlist.at(1).c_str(), arglist.c_str());
			}
			else
			{
				clientlist[i].call(vec_serverUrl.at(i), methodName, "", &rpcresultlist[i]);
			};
			string msg = value_string(rpcresultlist[i]);
			printf("%s\n", msg.c_str());
		} catch (girerr::error const error) {
			cerr << "Client threw error: " << error.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		};
	}; 
#else
	string mod;
    try 
	{
        string const serverUrl("http://172.17.100.116:8081/RPC2");
        string const serverUrl2("http://172.17.100.102:8081/RPC2");
        string methodName = string("localCmd.") + string("SENDPV");
		clientSimple myClient;
		clientSimple myClient2;
		string pvname[3] = {"TMS_CSBUS_SPT_SAD001", "TMS_CSBUS_SPT_SAD002","TMS_CSBUS_SPT_SAD003"};

		chid pv_chid[3];
		for ( int i = 0; i < 3; i++)
		{
			ca_create_channel(pvname[i].c_str(), 0, 0, 0, &pv_chid[i]);
		};
		ca_pend_io(0.1);
		struct dbr_time_double data[3];
		int count = 0;
		while ( true )
		{
			for ( int i = 0; i < 3; i++ )
			{
				ca_get(DBR_TIME_DOUBLE, pv_chid[i], (void *)&data[i]);
			};
			ca_pend_io(0.2);
			printf("[Count:%d]->PV1:%f, PV2:%f, PV3:%f\n", count, data[0].value, data[1].value, data[2].value);
			long sec; double msc, msc1;
			GetMicroTime( &sec, &msc );
			printf("[Before->Sec:%ld] [mSec]:%f\n", sec, msc);
			value rpcresult, rpcresult2;
			myClient.call(serverUrl, methodName, "iddd", &rpcresult, count, data[0].value,data[1].value,data[2].value);
			myClient2.call(serverUrl2, methodName, "iddd", &rpcresult2, count, data[0].value,data[1].value,data[2].value);
			string msg = value_string(rpcresult);
			string msg2 = value_string(rpcresult2);
			GetMicroTime( &sec, &msc1 );
			printf("[After->Sec:%ld] [mSec]:%f, [Offset]:%f\n", sec, msc, fabs(msc1-msc));
			cout << "OPI16_MethodCall:" << msg<<" OPI02_MethodCall:"<<msg2<< endl;
			count++;
			//usleep(500000);
			sleep(1);
		};
    } 
	catch (girerr::error const error) 
	{
        cerr << "Client threw error: " << error.what() << endl;
    } 
	catch (...) 
	{
        cerr << "Client threw unexpected error." << endl;
    }
#endif

    return 0;
}
