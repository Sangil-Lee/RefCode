#include <cassert>
#include <stdexcept>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <base.hpp>
#include <registry.hpp>
#include <server_abyss.hpp>

#include "cadef.h"

using namespace std;
using namespace xmlrpc_c;

class localCmdopiupdate : public method 
{
public:
    localCmdopiupdate() 
	{
        // signature and help strings are documentation -- the client
        // can query this information with a system.methodSignature and
        // system.methodHelp RPC.
        //this->_signature = "i:ii";
        this->_signature = "d:dd";

        this->_help = "This method executes local service (opiupdate)";
    };

	void execute(paramList const&, value *const  retvalP) 
	{
		char hostname[255];
		system("/usr/local/bin/opiupdate &");
		gethostname(hostname, sizeof(hostname) );
		
#if 0
		struct hostent *hostinfo;
		string hostip;
		if (( hostinfo=gethostbyname(hostname)) != NULL )
			hostip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
#endif
		string strhostname = string(hostname)+string(": successful...");
		*retvalP = value_string(strhostname);
	};
};

class localCmdshutdown : public method 
{
public:
    localCmdshutdown() 
	{
        this->_signature = "d:dd";
        this->_help = "This method executes local service (shutdown)";
    };
	void execute(paramList const& , value *const  retvalP) 
	{
		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		string strhostname = string(hostname)+string(": successful...");
		system("shutdown -h now");
		*retvalP = value_string(strhostname);
	};
};

class localCmdopipluginupdate : public method 
{
public:
    localCmdopipluginupdate() 
	{
        this->_signature = "d:dd";
        this->_help = "This method executes local service (opiupdate)";
    };

	void execute(paramList const& , value *const  retvalP) 
	{
		char hostname[255];
		system("/usr/local/bin/opipluginupdate &");
		gethostname(hostname, sizeof(hostname) );
		
		string strhostname = string(hostname)+string(": opipluginupdate successful...");
		*retvalP = value_string(strhostname);
	};
};

class localCmdopiinstall : public method 
{
public:
    localCmdopiinstall() 
	{
        this->_signature = "d:dd";
        this->_help = "This method executes local service (opiupdate)";
    };
	void execute(paramList const& , value *const  retvalP) 
	{
		char hostname[255];
		system("/usr/local/bin/opiinstall &");
		gethostname(hostname, sizeof(hostname) );
		
		string strhostname = string(hostname)+string(": successful...");
		*retvalP = value_string(strhostname);
	};
};
class localCmdremotestop : public method 
{
public:
    localCmdremotestop() 
	{
        this->_signature = "s:s";
        this->_help = "This method executes local service (remotekill)";
	};

	pid_t checkProcess(const string processname)
	{
		string checkproc = string("/sbin/pidof ") + processname;
		FILE *fp = popen (checkproc.c_str(), "r");

		char buf[20];
		while (fgets (buf, sizeof(buf), fp) ) {};
		pclose(fp);

		return atoi(buf);
	};

	void execute(paramList const& paramList, value *const  retvalP) 
	{
		string const processname = paramList.getString(0);
		pid_t killid = checkProcess(processname);

		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		printf("program:%s, pid:%d\n", processname.c_str(), killid);

		string strretval; 
		if (killid != 0) 
		{
			strretval = string(hostname)+string(":successfully stoped ")+processname;
			kill(killid, SIGTERM);
		}
		else
		{
			strretval = string(hostname)+string(": not executed ")+processname;
		};
		*retvalP = value_string(strretval);
	};
};
class localCmdremotestart : public method 
{
private:
	string mhostname;
public:
    localCmdremotestart() 
	{
        this->_signature = "s:ss";
        this->_help = "This method executes local service (remotestart)";
		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		mhostname=hostname;
	};

	int checksvc(const string &svcname)
	{
		int chkresult = 0;
		string strresult;
		string chksvc;
		if( svcname.compare("tmsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4812\"") + string(" | wc -l");
		} else if( svcname.compare("vmsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4814\"") + string(" | wc -l");
		} else if( svcname.compare("clsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4813\"") + string(" | wc -l");
		} else if( svcname.compare("hrsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4817\"") + string(" | wc -l");
		} else if( svcname.compare("hdsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4818\"") + string(" | wc -l");
		} else if( svcname.compare("qdsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4815\"") + string(" | wc -l");
		} else if( svcname.compare("icrfengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4819\"") + string(" | wc -l");
		} else if( svcname.compare("echengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4816\"") + string(" | wc -l");
		} else if( svcname.compare("fuelengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4821\"") + string(" | wc -l");
		} else if( svcname.compare("ccsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4820\"") + string(" | wc -l");
		} else if( svcname.compare("mpsengine")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + string("\"ArchiveEngine -port 4825\"") + string(" | wc -l");
		} else if( svcname.compare("tomcat")==0 ) {
			chksvc = string("ps -ef|grep -v grep|grep ") + svcname + string(" | wc -l");
		} else {
			chksvc = string("ps -ef|grep -v grep|gawk '{print $8}'|grep ") + svcname + string(" | wc -l");
		};
		FILE *fp = popen(chksvc.c_str(), "r");
		char buf[4];
		while( fgets(buf, sizeof(buf), fp)) {};
		pclose(fp);
		chkresult = atoi(buf);
		return chkresult;
	};

	int runchild( const string command, const string arglist )
	{
		int svcchk = checksvc(command);
		if (svcchk>0)	 return -1;

		pid_t pid = fork();
		if (pid == -1) return -1;
		if (pid == 0) 
		{
			string strcmd;
			if (command.compare("ntpd")== 0){
				strcmd = "service ntpd start";
			} else if (command.compare("sysMonitor")== 0){
				chdir("/usr/local/epics/siteApp/iocBoot/iocsysMonitor");
				strcmd = "screen -d -m ./st.cmd";
			} else if (command.compare("tmsengine")== 0){
				chdir("/SNFS1/TMSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("vmsengine")== 0){
				chdir("/SNFS1/VMSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("clsengine")== 0){
				chdir("/SNFS1/CLSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("hrsengine")== 0){
				chdir("/SNFS1/HRSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("hdsengine")== 0){
				chdir("/SNFS1/SHDSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("qdsengine")== 0){
				chdir("/SNFS1/QDSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("icrfengine")== 0){
				chdir("/SNFS1/ICRFArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("echengine")== 0){
				chdir("/SNFS1/ECHArchiveData");
				strcmd = "./arch.cmd";
			} else if (command.compare("fuelengine")== 0){
				chdir("/SNFS1/FUELArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("ccsengine")== 0){
				chdir("/SNFS1/PF1Test");
				strcmd = "./arch.cmd";
			} else if (command.compare("mpsengine")== 0){
				chdir("/SNFS1/MPSArchivedData");
				strcmd = "./arch.cmd";
			} else if (command.compare("mdsip")== 0){
				strcmd = "mdsip";
			} else if (command.compare("httpd")== 0){
				strcmd = "apachectl start";
			} else if (command.compare("nsrd")== 0){
				strcmd = "nsrd";
			} else if (command.compare("tomcat")== 0){
				chdir("/usr/local/tomcat5/bin");
				strcmd = "./startup.sh";
			}
			execl( "/bin/bash", "sh", "-c", strcmd.c_str(), NULL);
			exit (-1);
		};
		return 1;
	}

	void execute(paramList const& paramList, value *const  retvalP) 
	{
		string const processname = paramList.getString(0);
		string const arglist   = paramList.getString(1);
		string const strexecute = processname + string(" ")+arglist + string("&");

		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		//printf("program:%s, arglist:%s\n", processname.c_str(), arglist.c_str());
		//printf("program:%s\n", processname.c_str());
		int check = runchild( processname, arglist);
		string status;

		if( check<0 ) status = ":already started ";
		else status = ":successfully started ";
		string strretval = string(hostname)+status+processname;
		*retvalP = value_string(strretval);
	};
};
class localCmdservicechecker : public method 
{
private:
	string mhostname;
public:
	//map <string, vector<string> > mSvclist;
	//map <string, vector<string> >::iterator mSvclistiter;
	vector<string>  mSvclist;
    localCmdservicechecker() 
	{
        this->_signature = "s:ss";
        this->_help = "This method executes local service check";

		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		mhostname=string(hostname)+string("\n");
		registerHostService(hostname);
	};

	void registerHostService(const char *hostname)
	{
		//mSvclist.insert(make_pair(hostname, svclist));
		if(strcasecmp(hostname,"opi01") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("VMS");
			mSvclist.push_back("Periodicscreenshot");
		}
		else if(strcasecmp(hostname,"opi02") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("TMS");
			mSvclist.push_back("Periodicscreenshot");
		}
		else if(strcasecmp(hostname,"opi03") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("SCS");
			mSvclist.push_back("CLTU");
			mSvclist.push_back("sysMonitor");
		}
		else if(strcasecmp(hostname,"opi04") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
		}
		else if(strcasecmp(hostname,"opi05") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
		}
		else if(strcasecmp(hostname,"opi06") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
		}
		else if(strcasecmp(hostname,"opi07") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
		}
		else if(strcasecmp(hostname,"opi08") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
		}
		else if(strcasecmp(hostname,"opi09") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi10") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
		}
		else if(strcasecmp(hostname,"opi11") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi12") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
		}
		else if(strcasecmp(hostname,"opi13") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi14") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi15") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi17") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi18") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"opi19") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("MenuWindow");
			mSvclist.push_back("TMS");
			mSvclist.push_back("VMS");
		}
		else if(strcasecmp(hostname,"data") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("http");
			//mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("ArchiveExportManager");
		}
		else if(strcasecmp(hostname,"channel") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("nfsd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("\"ArchiveEngine -port 4812\"");
			mSvclist.push_back("\"ArchiveEngine -port 4813\"");
			mSvclist.push_back("\"ArchiveEngine -port 4814\"");
			mSvclist.push_back("\"ArchiveEngine -port 4815\"");
			mSvclist.push_back("\"ArchiveEngine -port 4816\"");
			mSvclist.push_back("\"ArchiveEngine -port 4817\"");
			mSvclist.push_back("\"ArchiveEngine -port 4818\"");
			mSvclist.push_back("\"ArchiveEngine -port 4819\"");
			mSvclist.push_back("\"ArchiveEngine -port 4820\"");
			mSvclist.push_back("\"ArchiveEngine -port 4821\"");
			mSvclist.push_back("\"ArchiveEngine -port 4825\"");
			//mSvclist.insert(make_pair(hostname, svclist));
		}
		else if(strcasecmp(hostname,"mdsplus") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("mdsip");
		}
		else if(strcasecmp(hostname,"backup") == 0 )
		{
			mSvclist.push_back("ntpd");
			//mSvclist.push_back("caRepeater");
			mSvclist.push_back("sysMonitor");
			mSvclist.push_back("nsrd");
		}
		else if(strcasecmp(hostname,"kstar_exp") == 0 )
		{
			mSvclist.push_back("ntpd");
			mSvclist.push_back("httpd");
			//mSvclist.push_back("caRepeater");
			mSvclist.push_back("tomcat");
		}
		vector<string>::iterator svclistiter;
		for(svclistiter=mSvclist.begin(); svclistiter!=mSvclist.end();++svclistiter)
		{
			printf("Svclist:%s\n",(*svclistiter).c_str());
		};
	};
	//int checksvc(const string &svcname)
	string checksvc(const string &svcname)
	{
		//int chkresult = 0;
		string strresult;
		string chksvc;
		if( svcname.compare("tomcat")==0 || svcname.find("ArchiveEngine")!=string::npos ) 
		{
			chksvc = string("ps -ef|grep -v grep|grep ") + svcname + string(" | wc -l");
		}
		else
		{
			chksvc = string("ps -ef|grep -v grep|gawk '{print $8}'|grep ") + svcname + string(" | wc -l");
		}
		FILE *fp = popen(chksvc.c_str(), "r");
		char buf[4];
		while( fgets(buf, sizeof(buf), fp)) {};
		pclose(fp);
		//chkresult = atoi(buf);
		//return chkresult;
		strresult=string(";")+string(buf);
		return strresult;
	};
	void execute(paramList const& paramList, value *const  retvalP) 
	{
		vector<string>::iterator svclistiter;
		string strretval;
		string strstatus;
		strretval = mhostname;
		for(svclistiter=mSvclist.begin(); svclistiter!=mSvclist.end();++svclistiter)
		{
			strstatus=checksvc(*svclistiter);
			if(mhostname.compare("channel\n")==0)
			{
				string svcname= (*svclistiter);
				string::size_type idx;
				if( idx = svcname.find("4812") != string::npos ) {
					svcname = "TMS ArchiveEngine";
				} else if( idx = svcname.find("4813") != string::npos ) {
					svcname = "CLS ArchiveEngine";
				} else if( idx = svcname.find("4814") != string::npos ) {
					svcname = "VMS ArchiveEngine";
				} else if( idx = svcname.find("4815") != string::npos ) {
					svcname = "QDS ArchiveEngine";
				} else if( idx = svcname.find("4816") != string::npos ) {
					svcname = "ECH ArchiveEngine";
				} else if( idx = svcname.find("4817") != string::npos ) {
					svcname = "HRS ArchiveEngine";
				} else if( idx = svcname.find("4818") != string::npos ) {
					svcname = "HDS ArchiveEngine";
				} else if( idx = svcname.find("4819") != string::npos ) {
					svcname = "ICRF ArchiveEngine";
				} else if( idx = svcname.find("4820") != string::npos ) {
					svcname = "CCS ArchiveEngine";
				} else if( idx = svcname.find("4821") != string::npos ) {
					svcname = "FUEL ArchiveEngine";
				} else if( idx = svcname.find("4825") != string::npos ) {
					svcname = "MPS ArchiveEngine";
				}
				strretval += svcname+strstatus;
			}
			else
			{
				strretval += (*svclistiter)+strstatus;
			};
		};
		//strretval.erase(remove(strretval.begin(), strretval.end(), '\n'));
		*retvalP = value_string(strretval);
	};
};

int main(int const argc,  const char ** const argv) 
{
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
	{
		printf("register signal SIGCHLD... error.\n");
		exit(-1);
	};

	if ( argc !=2 ) 
	{
		printf("USAGE:remotesvr [tcp port:8081].\n");
		exit(-1);
	}

	int portno =atoi(argv[1]);
    try {
        registry localSvcReg;
        methodPtr const opiupdate(new localCmdopiupdate);
        methodPtr const opipluginupdate(new localCmdopipluginupdate);
        methodPtr const opiinstall(new localCmdopiinstall);
        methodPtr const shutdown(new localCmdshutdown);
        methodPtr const remotestop(new localCmdremotestop);
        methodPtr const remotestart(new localCmdremotestart);
        methodPtr const servicechecker(new localCmdservicechecker);

        localSvcReg.addMethod("localCmd.opiupdate", opiupdate);
        localSvcReg.addMethod("localCmd.opipluginupdate", opipluginupdate);
        localSvcReg.addMethod("localCmd.opiinstall", opiinstall);
        localSvcReg.addMethod("localCmd.shutdown", shutdown);
        localSvcReg.addMethod("localCmd.remotestop", remotestop);
        localSvcReg.addMethod("localCmd.remotestart", remotestart);
        localSvcReg.addMethod("localCmd.servicechecker", servicechecker);

        //serverAbyss remoteCmdServer( localSvcReg, 8081,/*TCP port on which to listen*/ 
				//"/tmp/xmlrpc_log"/*Log file*/);
        serverAbyss remoteCmdServer( localSvcReg, portno,/*TCP port on which to listen*/ 
				"/tmp/xmlrpc_log"/*Log file*/);
        
        remoteCmdServer.run();
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }
    return 0;
}
