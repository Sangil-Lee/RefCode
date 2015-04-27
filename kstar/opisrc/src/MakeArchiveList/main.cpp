#include <sys/time.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include <string>

using namespace std;

void MicroTimePrint ( long *sec, double *msec )
{
	struct timeval tp;

	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;
		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		printf("sec: %d, msec:%f\n", *sec, *msec);
	};
};


int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Usage: MakeArchiveList filename");
		printf("file format: pvname	periodictime(sec)");
		exit(-1);
	}
	char    *srcfilename = argv[1]; 

	//set
	vector<string> srcfile_set;
	vector<string> scan_set;
	vector<string>::iterator pvname_set_iter;
	vector<string>::iterator scan_set_iter;

	ifstream srcfile(srcfilename);

	if ( srcfile.fail () ) 
	{
		printf("source file not found\n");
		return -1;
	};

	string strToken;
	int line = 0;

	while (!srcfile.eof ())
	{
		getline (srcfile, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		string pvname = string(pch);
		string strScan;
		//printf( "%s:",pvname.c_str());
		while (pch != NULL)
		{
			if(!(pch = strtok (NULL," \t,"))) continue;
			strScan = string(pch);
			//printf("%s",strScan.c_str());
		};
		//set
		pvname_set_iter = find(srcfile_set.begin(), srcfile_set.end(), pvname);
		if ( pvname_set_iter != srcfile_set.end() ) 
		{
			printf("Already existed PVName:%s in source file:%s\n", pvname.c_str(), srcfilename);
			continue;
		};
		srcfile_set.push_back(pvname);
		scan_set.push_back(strScan);
		line++;
		//printf("\n");
	};
	char buff[300];
#if 1
	for (size_t i = 0; i < srcfile_set.size();i++)
	{
		//sprintf(buff,"channelname%snameperiod%speriodmonitorchannel",
				//srcfile_set.at(i).c_str(),scan_set.at(i).c_str());
		sprintf(buff,"%s%s%s%s%s",
				"<channel><name>",srcfile_set.at(i).c_str(),"</name><period>",scan_set.at(i).c_str(),"period>");
		printf("%s\n", buff);
	}
#else
	for(pvname_set_iter = srcfile_set.begin(); pvname_set_iter != srcfile_set.end(); ++pvname_set_iter)
	{
		printf("%s\n", pvname_set_iter->c_str());
	}
#endif
	srcfile.close();
	return 0;
}
