#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
// Base
#include <epicsVersion.h>
// Tools
#include <AutoPtr.h>
#include <BinaryTree.h>
#include <RegularExpression.h>
#include <epicsTimeHelper.h>
#include <ArgParser.h>
// Storage
#include <SpreadsheetReader.h>
#include <AutoIndex.h>
#include "cadef.h"
#include "valueToStr.h"

using namespace std;
using namespace xmlrpc_c;

static int GetFile(const string filename)
{
	string cmd="ncftpget -DD -u kstar -p kstar2007 da ~/remotedata /home/kstar/retrievedata/" + filename;
	//string cmd="ncftpget -DD -u kstar -p kstar2007 opi02 ~/remotedata /home/kstar/retrievedata/" + filename;
	FILE *fp = popen(cmd.c_str(), "r");
	pclose(fp);

	return 0;
};

int main(int argc, char *argv[]) 
{
	int     c = 0;
	string mod;
	char *pvfile=0, *pvlist=0, *starttime=0, *endtime=0, 
		 *precision=0, *average=0, *spreadsheet=0, *linear=0,*format=0, *system=0,*outputfile=0 ;
	//f: pvfilelist, l: pvlist, s:starttime, e:endtime, p: precision, 
	//L:LCS, A:average, S:spreadsheet,R:Linear>, F:format<decimal|engineering|exponential>
	while ((c = getopt (argc, argv, "f:l:s:e:p:o:L:F:A:S:R:")) !=  -1) 
	{
		switch (c) {
			case  'f':
				pvfile = optarg;
				break;
			case  'l':
				pvlist = optarg;
			case  's':
				starttime = optarg;
				break;
			case  'e':
				endtime = optarg;
				break;
			case  'p':
				precision = optarg;
				break;
			case  'o':
				outputfile = optarg;
				break;
			case  'L':
				system = optarg;
				break;
			case  'F':
				format = optarg;
				break;
			case  'A':
				average = optarg;
				break;
			case  'S':
				spreadsheet = optarg;
				break;
			case  'R':
				linear = optarg;
				break;
			case  '?':
				printf("USAGE: RemoteArchiveExport \n");
				return -1;
		};
	};

	//"f:l:s:e:p:o:F:S:R:"
	//"f:l:s:e:p:o:L:F:A:S:R:"
	printf("f:%s,l:%s,s:%s,e:%s,p:%s,o:%s,L:%s,F:%s,A:%s,S:%s,R:%s\n",pvfile, pvlist,starttime,endtime, 
		 precision,outputfile,system, format, average, spreadsheet, linear);

	if(pvfile == 0 && pvlist == 0)
	{
		fprintf(stderr, "Need Options: pvlist or pvfile.\n");
		exit(-1);
	}
	if ( outputfile == 0)
	{
		fprintf(stderr, "need export file.\n");
		exit(-1);
	};
	string strofile = string(" -output /home/kstar/retrievedata/") + string(outputfile);

	vector<string> vecpvlist;
	vector<string>::iterator vecpvlistiter;
	if(pvfile != 0 && pvlist==0)
	{
		ifstream pvfilename(pvfile);
		if( pvfilename.fail())
		{
			printf("PV File Not Found.\n");
			exit(-1);
		}
		string strToken;
		while(!pvfilename.eof())
		{
			getline(pvfilename, strToken);
			if(strToken[0] == '#' || strToken.empty()==true) continue;

			char str[strToken.size()];
			strcpy (str, strToken.c_str());

			char *pch = 0;
			if(!(pch = strtok (str," \t,"))) continue;
			string pvname = pch;
			vecpvlistiter = find(vecpvlist.begin(), vecpvlist.end(), pvname);
			if ( vecpvlistiter != vecpvlist.end() )
			{
				printf("Already existed PVName:%s\n", pvname.c_str());
				continue;
			};
			vecpvlist.push_back(pvname);
		};
	}
	else if ( pvlist!=0 && pvfile ==0 )
	{
		char *pch = 0;
		pch = strtok (pvlist," \t,");
		string pvname;
		while(pch != NULL)
		{
			pvname = pch;
#if 1
			vecpvlistiter = find(vecpvlist.begin(), vecpvlist.end(), pvname);
			if ( vecpvlistiter != vecpvlist.end() )
			{
				printf("Already existed PVName:%s\n", pvname.c_str());
				continue;
			};
#endif
			vecpvlist.push_back(pvname);
			pch = strtok(NULL, " \t,");
		};
	};

	string strPVlists;
	size_t idx = 0;
	for(vecpvlistiter=vecpvlist.begin(); vecpvlistiter!=vecpvlist.end(); ++vecpvlistiter)
	{
		strPVlists += (*vecpvlistiter)+" ";
		//printf("pvlist:%s\n", (*vecpvlistiter).c_str());
		//printf("strpvlist:%s\n", strPVlists.c_str());
		idx++;
	}

	AutoPtr<epicsTime> start, end;
	if (starttime != 0)
	{
		start = new epicsTime;
		if (!string2epicsTime(starttime, *start))
		{
			fprintf(stderr, "Parse error for start time '%s'\n",
					starttime);
			start = 0;
			exit(-1);
		};
	};
	if (endtime != 0)
	{
		end = new epicsTime();
		if (!string2epicsTime(endtime, *end))
		{
			fprintf(stderr, "Parse error for end time '%s'\n",
					endtime);
			end = 0;
			exit(-1);
		};
	};
	if (start && end && *start > *end)
	{   // Could simply swap start and end, but assume the user is
		// confused and should rethink the request.
		fprintf(stderr, "start time is greater than end time.\n");
		return -1;
	};

	string strStartTime = string("-start \"") + string(starttime)+string("\"");
	string strEndTime = string(" -end \"") + string(endtime)+string("\"");
	string strPrec = (precision == 0)?"":string("-precision ")+string(precision);
	string strIndex;
	if ( system != 0 )
	{
		if (strcasecmp(system, "TMS") == 0 ) {
			strIndex = "/CA_NFS/TMSArchivedData/index ";
		} else if (strcasecmp(system, "TMSHall") == 0 ) {
			strIndex = "/CA_NFS/TMSHallArchivedData/index ";
		} else if (strcasecmp(system, "VMS") == 0 ) {
			strIndex = "/CA_NFS/VMSArchivedData/index ";
		} else if (strcasecmp(system, "CLS") == 0 ) {
			strIndex = "/CA_NFS/CLSArchivedData/index ";
		} else if (strcasecmp(system, "HRS") == 0 ) {
			strIndex = "/CA_NFS/HRSArchivedData/index ";
		} else if (strcasecmp(system, "HDS") == 0 ) {
			strIndex = "/CA_NFS/SHDSArchivedData/index ";
		} else if (strcasecmp(system, "ICRF") == 0 ) {
			strIndex = "/CA_NFS/ICRFArchivedData/index_icrf ";
		} else if (strcasecmp(system, "PF1Test") == 0 ) {
			strIndex = "/CA_NFS/PF1TestArchivedData/index ";
		} else if (strcasecmp(system, "ECH") == 0 ) {
			strIndex = "/CA_NFS/ECHArchivedData/index ";
		} else if (strcasecmp(system, "QDS") == 0 ) {
			strIndex = "/CA_NFS/QDSArchivedData/index ";
		} else if (strcasecmp(system, "TSS") == 0 ) {
			strIndex = "/CA_NFS/TSSArchivedData/index ";
		} else if (strcasecmp(system, "DDS") == 0 ) {
			strIndex = "/CA_NFS/DDSArchivedData/index ";
		} else if (strcasecmp(system, "FUEL") == 0 ) {
			strIndex = "/CA_NFS/FUELArchivedData/index ";
		} else if (strcasecmp(system, "ICS") == 0 ) {
			strIndex = "/CA_NFS/ICSArchivedData/index ";
		} else if (strcasecmp(system, "MPS") == 0 ) {
			strIndex = "/CA_NFS/MPSArchivedData/index ";
		} else{
			strIndex = "/usr/local/ArchiverTest/index ";
		}
	};

	string strFormat;
	if ( format != 0 )
	{
		switch(atoi(format))
		{
			case 1:
				strFormat=string(" -format d ");
				break;
			case 2:
				strFormat=string(" -format en ");
				break;
			case 3:
				strFormat=string(" -format ex ");
				break;
			default:
				break;
		};
	};
	string strType;
	if ( average != 0 )
	{
		strType=string(" -average ") + string(average);
	}
	else if (spreadsheet != 0)
	{
		strType=string(" -spreadsheet ") + string(spreadsheet);
	}
	else if (linear != 0)
	{
		strType=string(" -linear ") + string(linear);
	};

	//"/usr/local/ArchiverTest/index WF:CALC:SIN984 WF:CALC:SIN985 
	//-start \"03/13/2008 00:00:00.000\" -end \"03/14/2008 00:00:00.000\" 
	//-no_text -precision 6 -average 1 -output /home/kstar/retrievedata/data.txt";

	string remotecmd = strIndex + strPVlists + strStartTime + strEndTime + string(" -no_text ") 
		+ strFormat+ strPrec + strType + strofile;
	//printf("remotecmd:%s\n", remotecmd.c_str());

	const string strProtocol = "http://";
	const string strport = "da:8082/RPC2";
	//const string strport = "opi02:8082/RPC2";
	string serverUrl;

	serverUrl = strProtocol+strport;

	clientSimple clientlist;
	value rpcresultlist;
	string resultmsg;
	string filename = outputfile;
	try {
		string methodName = string("remoteExport.") + string("ArchiveExport");
		//string arglist="/usr/local/ArchiverTest/index WF:CALC:SIN984 WF:CALC:SIN985 -start \"03/13/2008 00:00:00.000\" -end \"03/14/2008 00:00:00.000\" -no_text -precision 6 -average 1 -output /home/kstar/retrievedata/data.txt";
		clientlist.call(serverUrl, methodName, "sss", &rpcresultlist,"ArchiveExport ", remotecmd.c_str(), filename.c_str());
		resultmsg = value_string(rpcresultlist);
		printf("%s\n", resultmsg.c_str());
#if 1
		int count = 0;
		while(true)
		{
			methodName = string("remoteExport.") + string("FileGet");
			{
				clientSimple clientfileget;
				value rpcresultlist;
				//file checking;
				clientfileget.call(serverUrl, methodName, "ss", &rpcresultlist,"ArchiveExport ",filename.c_str() );
				string strret = value_string(rpcresultlist);
				if( strret.compare("1") == 0 )
				{
					GetFile(filename);
					break;
				};
			};
			printf("file generation...%d\n",count);
			count++;
			sleep(1);
		};
		printf("retreive time:%d\n", count);
#endif
	} catch (girerr::error const error) {
		cerr << "Client threw error: " << error.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	};
    return 0;
}
