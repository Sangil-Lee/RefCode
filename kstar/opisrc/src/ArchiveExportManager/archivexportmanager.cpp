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

class remoteExportArchiveExport : public method 
{
private:
	string mhostname;
public:
    remoteExportArchiveExport() 
	{
        this->_signature = "s:sss";
        this->_help = "This method executes local service (remoteExport)";
		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		mhostname=hostname;
	};

	int checksvc(const string &filename)
	{
		int chkresult = 0;
		string strresult;
		string chksvc;
		chksvc = string("ps -ef|grep -v grep|grep ArchiveExport|grep ")+filename+string("|wc -l");
		FILE *fp = popen(chksvc.c_str(), "r");
		char buf[4];
		while( fgets(buf, sizeof(buf), fp)) {};
		pclose(fp);
		chkresult = atoi(buf);
		return chkresult;
	};

	int runchild( const string command, const string arglist, const string filename )
	{
		//int nchk = checksvc(filename);
		pid_t pid = fork();
		//if (pid == -1 || nchk <=0 ) return -1;
		if (pid == -1 ) return -1;
		if (pid == 0) 
		{
			string strcmd = command+arglist;
			execl( "/bin/bash", "sh", "-c", strcmd.c_str(), NULL);
			exit (-1);
		};
		return 1;
	}

	void execute(paramList const& paramList, value *const  retvalP) 
	{
		string const processname = paramList.getString(0);
		string const arglist1   = paramList.getString(1);
		string const arglist2   = paramList.getString(2);

		//printf("program:%s, param list1:%s, list2:%s\n", processname.c_str(), arglist1.c_str(), arglist2.c_str());
		printf("program:%s, filename:%s\n", processname.c_str(), arglist2.c_str());

		string strretval;
		int check = runchild( processname, arglist1, arglist2);
		string status = (check < 0)?": file generation stop.":": file generation start";
		//strretval = mhostname+status+processname;
		strretval = mhostname+status;
		*retvalP = value_string(strretval);
		//*retvalP = value_string(strretval);
	};
};
class remoteExportFileGet : public method 
{
private:
	string mhostname;
public:
    remoteExportFileGet() 
	{
        this->_signature = "s:ss";
        this->_help = "This method executes local service (remoteExportFileGet)";
		char hostname[255];
		gethostname(hostname, sizeof(hostname) );
		mhostname=hostname;
	};

	int checksvc(const string &filename)
	{
		int chkresult = 0;
		string strresult;
		string chksvc;
		chksvc = string("ps -ef|grep -v grep|grep ArchiveExport|grep ")+filename+string("|wc -l");
		FILE *fp = popen(chksvc.c_str(), "r");
		char buf[4];
		while( fgets(buf, sizeof(buf), fp)) {};
		pclose(fp);
		chkresult = atoi(buf);
		return chkresult;
	};

	void execute(paramList const& paramList, value *const  retvalP) 
	{
		string const processname = paramList.getString(0);
		string const arglist1   = paramList.getString(1);

		string strretval;
		int nOK = 0;
		string strret;
		int nchk = checksvc(arglist1);
		string filepath = string("/home/kstar/retrievedata/") + arglist1;
		int fileok=access (filepath.c_str(), F_OK);
		if(nchk == 0 && fileok == 0) nOK = 1;
		if (nOK == 1 ) strret = "1";
		else strret = "0";

		*retvalP = value_string(strret);

		//*retvalP = value_string(strretval);
	};
};

int main(int const, const char ** const) 
{
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
	{
		printf("register signal SIGCHLD... error.\n");
		exit(-1);
	};
    try {
        registry localSvcReg;
        methodPtr const remoteExport(new remoteExportArchiveExport);
        methodPtr const remoteFileGet(new remoteExportFileGet);

        localSvcReg.addMethod("remoteExport.ArchiveExport", remoteExport);
        localSvcReg.addMethod("remoteExport.FileGet", remoteFileGet);

        //serverAbyss archiveExportRemoteServer( localSvcReg, 8082,/*TCP port on which to listen*/ 
				//"/tmp/archiveexport.log"/*Log file*/);
        serverAbyss archiveExportRemoteServer( localSvcReg, 8082);
        
        archiveExportRemoteServer.run();
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }
    return 0;
}
