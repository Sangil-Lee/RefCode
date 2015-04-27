#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

//typedef vector <string> SystemList;
typedef map <string, int> SystemList;

class Engine
{
public:
	Engine();
	~Engine();
	int checkSystem(const string sysName);
	int Start(const string sysName);
	int Stop(const string sysName);
	int MakeScheduleList(const string filename);
	int RunSchedule();

private:
	SystemList systemlist; 
	typedef vector<string> ScheduleList;
	ScheduleList schedlist;
};

Engine::Engine()
{
	systemlist["ECH"]=4816;
	systemlist["HRS"]=4817;
	systemlist["DDS"]=4824;
	systemlist["ICRF"]=4819;
	systemlist["TSS"]=4823;
	systemlist["QDS"]=4815;
	systemlist["ECCD"]=4860;
	systemlist["TMS"]=4812;
	systemlist["VMS"]=4814;
	systemlist["DLS"]=4861;
	systemlist["PMS"]=4851;
	systemlist["CLS"]=4813;
	systemlist["HDS"]=4850;
	systemlist["FUEL"]=4821;
	systemlist["MPS"]=4825;
	systemlist["ICS"]=4826;
}

Engine::~Engine()
{
}

int Engine::Start(const string sysName)
{
	//string strRoot = string("/channel");
	string strRoot = string("/gpfsdata");
	string strOPDir = string("/OP_2011/");
	string strCHDir = strRoot+strOPDir+sysName+string("ArchivedData");
	chdir(strCHDir.c_str());
	string startcmd = string("./2011_4th_")+sysName+string(".cmd");

	//printf("Dir:%s, Start:%s\n", strCHDir.c_str(), startcmd.c_str());
	//FILE *fp = popen(startcmd.c_str(),"r");
	//pclose(fp);
	system(startcmd.c_str());
	return 1;
};

int Engine::RunSchedule()
{
	string strline;
	char *pch = NULL;
	time_t curTime = time(NULL);
	struct tm *curTM = localtime(&curTime);

	for(size_t i = 0; i < schedlist.size(); i++)
	{
		//strline = schedlist.at(i);
		char strBuff[schedlist.at(i).size()];
		strcpy(strBuff, schedlist.at(i).c_str());
		//pch = strtok((char*)strline.c_str(),":");
		pch = strtok(strBuff,":");
		//for test.
		//printf("Token:%s\n", pch);
		if(strcasecmp(pch,"Datetime") == 0)
		{
			pch = strtok (NULL, ":");
			struct tm stTM;
			time_t schedTime=0;
			strptime(pch, "%Y/%m/%d %H-%M-%S", &stTM);
			schedTime = mktime(&stTM);
			if(curTime != schedTime)
			{
				pch = strtok (NULL, ":");
				string pvname = pch;
				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';

				string pvvalue = pch;
				//pvvalue = pvvalue.substr(0, pvvalue.size()-1);

				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					//printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
					//caPut(pvname, dvalue, DBR_DOUBLE);
				};
			}
			else
			{
				pch = strtok(NULL, ":");
			}
		}
		else if(strcasecmp(pch,"Day") == 0)
		{
			//Day
			pch = strtok (NULL, " ");
			int type = 0;
			string sday = pch;
			if(sday.compare("Everyday")==0)			type = 0;
			else if(sday.compare("Monday")==0)		type = 1;
			else if(sday.compare("Tuesday")==0)		type = 2;
			else if(sday.compare("Wednesday")==0)	type = 3;
			else if(sday.compare("Thursday")==0)	type = 4;
			else if(sday.compare("Friday")==0)		type = 5;
			else if(sday.compare("Saturday")==0)	type = 6;
			else if(sday.compare("Sunday")==0)		type = 7;

			int weektype = curTM->tm_wday;

			if(type==0) weektype=0;

			//Time
			pch = strtok (NULL, ":");

			struct tm stTM;
			strptime(pch, "%H-%M-%S", &stTM);

			int setTime = (stTM.tm_hour*3600)+(stTM.tm_min*60)+stTM.tm_sec;       
			int curDayTime = (curTM->tm_hour*3600)+(curTM->tm_min*60)+(curTM->tm_sec);

			//printf("CuDayTime:%d, SetDayTime:%d\n", curDayTime, setTime);

			if((type == weektype) && (setTime==curDayTime)) 
			{
				pch = strtok (NULL, ":");
				string pvname = pch;

				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';
				string pvvalue = pch;
				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					//printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
				};
			}
			else
			{
				pch=strtok(NULL,":");
			};
		}
		else if(strcasecmp(pch,"Every secs") == 0)
		{
			pch = strtok (NULL, ":");
			//int dailysec = time.secsTo(datetime.time());
			int dailysec = (curTM->tm_hour*3600)+(curTM->tm_min*60)+(curTM->tm_sec);
			long secs = strtol(pch, NULL, 10);
			printf("dailysec:%d, setsecs:%ld\n", dailysec, secs);
			if((dailysec % secs) == 0 ) 
			{
				pch = strtok (NULL, ":");
				string pvname = pch;
				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';
				string pvvalue = pch;
				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
					//caPut(pvname, dvalue, DBR_DOUBLE);
				};
			}
			else
			{
				pch = strtok (NULL, ":");
			};
		}
		else
		{
			pch = strtok (NULL, ":");
		};
	};
	return 0;
}
int Engine::MakeScheduleList(const string filename)
{
#if 0
	ifstream schedfile(filename);
	if(schedfile.fail())
	{
		printf("%s Schedule file is not existed\n", filename.c_str());
		return -1;
	};
	string strpolicy;
	int line = 0;
	while(!scedfile.eof())
	{
		getline(schedfile, strpolicy);
	};
#else
	printf("Filename:%s\n", filename.c_str());
	char *strline = NULL;
	size_t len = 0;
	ssize_t read;

	FILE *fp = fopen(filename.c_str(), "r");
	if(fp == NULL) {
		printf("%s Schedule file is not existed\n", filename.c_str());
		pclose(fp);
		return -1;
	};

	char *pch = NULL;
	time_t curTime = time(NULL);
	struct tm *curTM = localtime(&curTime);

	while((read=getline(&strline, &len, fp))!=-1) 
	{
		if(strline[0]=='#') continue;
		schedlist.push_back(strline);
#if 0
		pch = strtok(strline,":");
		//for test.
		//printf("Token:%s\n", pch);
		if(strcasecmp(pch,"Datetime") == 0)
		{
			pch = strtok (NULL, ":");
			//reserveddatetime = QDateTime::fromString (pch, "yyyy/MM/dd hh-mm-ss" );
			struct tm stTM;
			time_t schedTime=0;
			strptime(pch, "%Y/%m/%d %H-%M-%S", &stTM);
			schedTime = mktime(&stTM);
			if(curTime != schedTime)
			{
				pch = strtok (NULL, ":");
				string pvname = pch;
				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';

				string pvvalue = pch;
				//pvvalue = pvvalue.substr(0, pvvalue.size()-1);

				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
					//caPut(pvname, dvalue, DBR_DOUBLE);
				};
			}
			else
			{
				pch = strtok(NULL, ":");
			}
		}
		else if(strcasecmp(pch,"Day") == 0)
		{
			//Day
			pch = strtok (NULL, " ");
			int type = 0;
			string sday = pch;
			if(sday.compare("Everyday")==0)			type = 0;
			else if(sday.compare("Monday")==0)		type = 1;
			else if(sday.compare("Tuesday")==0)		type = 2;
			else if(sday.compare("Wednesday")==0)	type = 3;
			else if(sday.compare("Thursday")==0)	type = 4;
			else if(sday.compare("Friday")==0)		type = 5;
			else if(sday.compare("Saturday")==0)	type = 6;
			else if(sday.compare("Sunday")==0)		type = 7;

			int weektype = curTM->tm_wday;

			if(type==0) weektype=0;

			//Time
			pch = strtok (NULL, ":");

			struct tm stTM;
			strptime(pch, "%H-%M-%S", &stTM);

			int setTime = (stTM.tm_hour*3600)+(stTM.tm_min*60)+stTM.tm_sec;       
			int curDayTime = (curTM->tm_hour*3600)+(curTM->tm_min*60)+(curTM->tm_sec);

			//printf("CuDayTime:%d, SetDayTime:%d\n", curDayTime, setTime);

			if((type == weektype) && (setTime==curDayTime)) 
			{
				pch = strtok (NULL, ":");
				string pvname = pch;

				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';
				string pvvalue = pch;
				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
				};
			}
			else
			{
				pch=strtok(NULL,":");
			};
		}
		else if(strcasecmp(pch,"Every secs") == 0)
		{
			pch = strtok (NULL, ":");
			//int dailysec = time.secsTo(datetime.time());
			int dailysec = (curTM->tm_hour*3600)+(curTM->tm_min*60)+(curTM->tm_sec);
			long secs = strtol(pch, NULL, 10);
			printf("dailysec:%d, setsecs:%ld\n", dailysec, secs);
			if((dailysec % secs) == 0 ) 
			{
				pch = strtok (NULL, ":");
				string pvname = pch;
				pch = strtok (NULL, ":");
				if(pch[strlen(pch)-1] =='\n')
					pch[strlen(pch)-1] ='\0';
				string pvvalue = pch;
				if(strncmp(pvname.c_str(), "RCOMMANDER", 10) != 0)
				{
					printf("caPut:%s(%s)\n", pvname.c_str(), pvvalue.c_str());
					//caPutString(pvname, pvvalue);
				}
				else
				{
					double dvalue = strtod(pvvalue.c_str(), NULL);
					//caPut(pvname, dvalue, DBR_DOUBLE);
				};
			}
			else
			{
				pch = strtok (NULL, ":");
			};
		}
		else
		{
			pch = strtok (NULL, ":");
		};
#endif
	};
	if(strline)	free(strline);
	if(fp) pclose(fp);
#endif

	return 0;
};

int Engine::Stop(const string sysName)
{
#if 0
	//ps -ef | grep TMS | grep -v grep | awk '{print $2}'
	//string stopcmd = string("ps -ef | grep ")+sysName+string("| grep ArchiveEngine| awk '{print $2}'");
	string stopcmd = string("ps -ef | grep ")+sysName+string("|grep ArchiveEngine| awk '{print $2}'");

	//printf("stopcmd:%s\n",stopcmd.c_str());

	FILE *fp = popen(stopcmd.c_str(),"r");
	if(!fp)
	{
		fprintf(stderr, "Cannot execute %s\n", stopcmd.c_str());
		pclose(fp);
		return -1;
	};
	const int proclen=1024;
	char strProcNum[proclen];
	//char *strProcNum = NULL;
	long int procID = 0;
	char *pEnd;
	//size_t length;
	//if(getline(&strProcNum,&length,fp)!=-1)
	while(fgets(strProcNum,proclen,fp)!=NULL)
	{
		procID = strtol(strProcNum, &pEnd, 10); 
		printf("%s",strProcNum);
		kill(procID, SIGTERM);
	};
	pclose(fp);
	//return (procID==0?-1:kill(procID,SIGTERM));
#else
	SystemList::iterator sysIter;
	sysIter = systemlist.find(sysName);
	int portno = sysIter->second;
	char sportno[24];
    sprintf(sportno,"%d",portno);
	chdir("/usr/local/bin");
	string command = string("./stopEngine.py --machine localhost --port ")+string(sportno);
	system(command.c_str());
#endif
	return (0);
};

int Engine::checkSystem(const string sysName)
{
	//SystemList::iterator sysIter;
	int retval = -1;
	if(systemlist.find(sysName) != systemlist.end() ) retval = 0;
	return retval;
#if 0
	for(sysIter = systemlist.begin(); sysIter!=systemlist.end(); ++sysIter)
	{
		if(sysName.compare(*sysIter) == 0) 
		{
			retval = 0;
			break;
		};
	};
#endif
}

int main(int argc, char **argv)
{
	string usage = "[USAGE]:$>Engine System[TMS,VMS,..] Start|Stop\n";
	//Engine *engine = new Engine();
	Engine engine;

	int count = argc;

	engine.MakeScheduleList("./Engine.sch");
	while(true)
	{
		engine.RunSchedule();
		sleep(1);
	}

	return 0;
	if(count != 3) 
	{
		printf(usage.c_str());
		exit(1);
	};
	string sysName = string(argv[1]);

	if(engine.checkSystem(sysName)<0)
	{
		printf("System is not accepted\n");
		exit(1);
	};

	string type = string(argv[2]);
	if(strcasecmp("start", type.c_str()) == 0)
	{
		engine.Start(sysName);
	}
	else if(strcasecmp("stop", type.c_str()) == 0)
	{
		engine.Stop(sysName);
	};

	//printf("Arg Count:%d\n", count);
	return 0;
}
