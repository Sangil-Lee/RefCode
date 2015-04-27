#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <link.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <dbLock.h>
#include <dbAddr.h>
#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <registryFunction.h>
#include <recSup.h>
#include <subRecord.h>
#include <epicsExport.h>
#include <waveformRecord.h>


#if 0
string  Epoch2Datetimestr ( time_t epochtime = 0 )
{
	struct tm *ptime;
	int year = 0;
	int month = 0, day = 0, hour = 0, minute = 0, second = 0;

	//ptime = ( epochtime == 0 ) ? localtime ((const time_t*)time(NULL)) : localtime ( &epochtime );
	if ( epochtime == 0 ) epochtime = time (NULL);

	ptime =	localtime ( &epochtime );

	year	= ptime -> tm_year + 1900;
	month	= ptime -> tm_mon + 1;
	day		= ptime -> tm_mday;
	hour	= ptime -> tm_hour;
	minute  = ptime -> tm_min;
	second  = ptime -> tm_sec;

	return ( IntToStr (year) + "/" + IntToStr (month,"%02d") + "/" + IntToStr (day,"%02d")+ " "+ IntToStr (hour,"%02d") + ":" + IntToStr (minute,"%02d") + ":" + IntToStr(second,"%02d") );
};
#endif

using namespace std;

class MakeIpRef 
{
public:
	MakeIpRef();
	virtual ~MakeIpRef();

public:
	int ReadIPRefFile();
	//int ParsingSave(const int count, float *valueA, float *valueB);
	int ParsingSave(int &count, float *valueA, float *valueB);
	void SetFileName(const string filename) {mfilename=filename;};
	enum FILETIMECHECK {SAME=0,DIFF};

protected:
	time_t prevfiletime;
	time_t curfiletime;
	vector<float> vecXDataSet;
	vector<float> vecYDataSet;

private:
	int checkFileTime(const time_t curtime);
	string mfilename;
};

MakeIpRef::MakeIpRef():prevfiletime(0),curfiletime(0)
{
}

MakeIpRef::~MakeIpRef()
{
}

int MakeIpRef::checkFileTime(const time_t curtime)
{
	if(curtime == prevfiletime) return SAME;

	prevfiletime = curtime;
	return DIFF;
}

//int MakeIpRef::ParsingSave(const int count, float *valueA, float *valueB)
int MakeIpRef::ParsingSave(int &count, float *valueA, float *valueB)
{
	string filename;
	if(mfilename.empty() == true)
		filename = "/home/kstar/ipref.temp";
	else
		filename = mfilename;

	vecXDataSet.clear();
	vecYDataSet.clear();

	ifstream file(filename.c_str());
	if ( file.fail () ) 
	{
		printf("filename not found:%s\n", filename.c_str());
		return -1;
	};
	string strToken;

	/*first pch=xwaveset, second pch=ywaveset*/

	while (!file.eof ())
	{
		getline (file, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char *pEnd;

		float xval, yval;

		xval = strtof(strToken.c_str(), &pEnd);
		yval = strtof(pEnd, 0);

		//printf("X-Val:%f, Y-Val:%f\n", xval, yval);

		vecXDataSet.push_back(xval);
		vecYDataSet.push_back(fabs(yval));

#if 0
		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;

		int index = 0;
		while (pch != NULL)
		{
			index = 0;
			string strval = pch;
			strtof
			if(!(pch = strtok (NULL," \t,"))) continue;
			if (index == 0) 
			{
				strPvname = pch;
				qDebug("%s", pch);
			}
			else if (index == 1) 
			{
				strDesc = pch;
				qDebug("%s", pch);
			};
			index++;
		};
#endif
	};
	count = vecXDataSet.size();

	for(size_t i = 0; i <(size_t) count; i++)
	{
		if(i < vecXDataSet.size() )
			valueA[i] = vecXDataSet.at(i);
#if 0
		else
			valueA[i] = 0.0;
#endif

		if(i < vecYDataSet.size() )
			valueB[i] = vecYDataSet.at(i);
#if 0
		else
			valueB[i] = 0.0;
#endif

		//printf("ValueA[%d]:%f, ValueB[%d]:%f\n", i, valueA[i], i, valueB[i]);
	};

	file.close ();
	return 0;
}

int MakeIpRef::ReadIPRefFile()
{
	struct stat fileStat;
	if(stat(mfilename.c_str(), &fileStat) < 0)
		return -1;

	curfiletime = fileStat.st_mtime;

	if( checkFileTime(curfiletime) == SAME ) return -1;


	struct tm *timeinfo = (struct tm*)localtime(&curfiletime);
	printf("File Last Modification---> \n");
	printf("Year(%d),Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d):\n", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	return 0;
}

MakeIpRef *gIpRef;
static long MakeIPRefProc (subRecord *precord)
{
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);

	waveformRecord *precordLinkA = (waveformRecord *)pdbAddrA->precord;
	waveformRecord *precordLinkB = (waveformRecord *)pdbAddrB->precord;

	float *pfieldLinkA =  (float*)pdbAddrA->pfield;
	float *pfieldLinkB =  (float*)pdbAddrB->pfield;

	if(gIpRef->ReadIPRefFile() < 0) return -1;

	int no_elements = pdbAddrA->no_elements;

	if(gIpRef->ParsingSave(no_elements, pfieldLinkA, pfieldLinkB) < 0) return -1;
	
#if 0
	for(size_t i = 0; i < no_elements; i++)
	{
		printf("ValueA[%d]:%f, ValueB[%d]:%f\n", i, pfieldLinkA[i], i, pfieldLinkB[i]);
	};
#endif

	precordLinkA->nord = no_elements;
	precordLinkB->nord = no_elements;
	precord->val = no_elements;

	//printf("**DB Process**\n");
	dbProcess((dbCommon*)precordLinkA);
	dbProcess((dbCommon*)precordLinkB);

	return 0;
}
/*subroutine record*/
static long MakeIPRefInit(const subRecord *precord)
{
	gIpRef = new MakeIpRef();
	gIpRef->SetFileName("/home/kstar/ipref.temp"); 
	return 0;
}

epicsRegisterFunction(MakeIPRefInit);
epicsRegisterFunction(MakeIPRefProc);
