/*
   * devSysMon.cpp for window
   * initial version
   * EPICS base 3.14.11
   * author leesi member of the interated team in KSTAR
*/
#include <stddef.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <Winsock2.h>
#include <Windows.h>
#include <Winbase.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <tchar.h>
#include <psapi.h>

#include "epicsExit.h"
#include "alarm.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "stringinRecord.h"
#include "aiRecord.h"
#include "longinRecord.h"
#include "epicsExport.h"

#define MAX_RAW_VALUES 20

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "kernel32.lib")\

//// DEFINES FOR COUNTER NAMES ////
#define CNTR_CPU "\\Processor(_Total)\\% Processor Time" // % of cpu in use

#define CNTR_MEMINUSE_BYTES "\\Memory\\Committed Bytes" // mem in use measured in bytes
#define CNTR_MEMAVAIL_BYTES "\\Memory\\Available Bytes" // mem available measured in bytes

#define CNTR_MEMINUSE_KB "\\Memory\\Committed KBytes" // mem in use measured in kbytes
#define CNTR_MEMAVAIL_KB "\\Memory\\Available KBytes" // mem avail in kilobytes

#define CNTR_MEMAVAIL_MB "\\Memory\\Available MBytes" // mem avail in megabytes
#define CNTR_MEMINUSE_PERCENT "\\Memory\\% Committed Bytes In Use" // % of mem in use
#define CNTR_MEMLIMIT_BYTES "\\Memory\\Commit Limit" // commit limit on memory in bytes

// NOTE: Find other counters using the function PdhBrowseCounters() (lookup in MSDN).
// This function was not implemented in this class.
using std::string;
using std::vector;

typedef struct _tag_PDHCounterStruct {
	int nIndex;				// The index of this counter, returned by AddCounter()
	LONG lValue;			// The current value of this counter
    HCOUNTER hCounter;      // Handle to the counter - given to use by PDH Library
    int nNextIndex;         // element to get the next raw value
    int nOldestIndex;       // element containing the oldes raw value
    int nRawCount;          // number of elements containing raw values
    PDH_RAW_COUNTER a_RawValue[MAX_RAW_VALUES]; // Ring buffer to contain raw values
} PDHCOUNTERSTRUCT, *PPDHCOUNTERSTRUCT;

class SysMon
{
public:
	SysMon();
	~SysMon();
	enum SYSTEMPROC {
		CAREPEATER = 0x00000001,
		NTPD	= 0x00000001<<1,
		HTTPD	= 0x00000001<<2,
		JAVA	= 0x00000001<<3
	};

	//// SETUP ////
	bool Initialize(void);
	void Uninitialize(void);

	//// COUNTERS ////
	int AddCounter(const char *pszCounterName);
	bool RemoveCounter(int nIndex);

	//// DATA ////
	bool CollectQueryData(void);
	bool GetStatistics(long *nMin, long *nMax, long *nMean, int nIndex);
	long GetCounterValue(int nIndex);
	//void CheckTime(string &sboottime, string &spasttime);
	void CheckTime();
	string GetCurTime() {return m_curtime;};
	string GetUpTime()  {return m_uptime;};
	void DiskSpaceCheck();
	double GetTotalSize() {return mTotal;};
	double GetAvailSize() {return mAvail;};
	double GetUsedSize()  {return mUsed;};
	unsigned int GetSysProcessStatus();
	int BitCheck( unsigned int src, unsigned int bit);
	string getLocalIPAddr();

protected:
	//// COUNTERS ////
	PPDHCOUNTERSTRUCT getCounterStruct(int nIndex);
	void registerHostService(); 
	string printBitString(unsigned int bitval);

	//// VALUES ////
	bool updateValue(PPDHCOUNTERSTRUCT pCounter);
	bool updateRawValue(PPDHCOUNTERSTRUCT pCounter);

	DWORD getABSSecond (SYSTEMTIME st);
	void absSecondToSystem (DWORD abs, SYSTEMTIME &st);

	//// VARIABLES ////
	HQUERY m_hQuery; // the query to the PDH
	//CArray<PPDHCOUNTERSTRUCT, PPDHCOUNTERSTRUCT> m_aCounters; // the current counters
	vector<PPDHCOUNTERSTRUCT> m_aCounters; // the current counters
	int m_nNextIndex;
	string m_curtime, m_uptime;
	//unsigned long mTotal, mAvail, mUsed;
	double mTotal, mAvail, mUsed;
	vector<string> m_sysproclist;
	vector<string> m_userproclist;
};

SysMon::SysMon()
{
	// m_nNextIndex is a unique value.  It will not be decremented, even if you remove counters.
	m_nNextIndex = 0;

	registerHostService();
	Initialize(); 
	AddCounter(CNTR_CPU);
	AddCounter(CNTR_MEMAVAIL_KB);
	//AddCounter(CNTR_MEMINUSE_KB);
	AddCounter(CNTR_MEMINUSE_BYTES);
}

SysMon::~SysMon()
{
}

bool SysMon::Initialize()
{
	if (PdhOpenQuery(NULL, 1, &m_hQuery) != ERROR_SUCCESS)
		return false;

	return true;
}
void SysMon::registerHostService()
{
	m_sysproclist.push_back("caRepeater.exe");

}
int SysMon::BitCheck( unsigned int src, unsigned int bit)
{
	return (src&bit) == 0 ? 0 : 1;
}

void SysMon::Uninitialize()
{
	PdhCloseQuery(&m_hQuery);

#if 1
	// clean memory
	for (size_t i=0;i<m_aCounters.size();i++)
	{
		delete m_aCounters.at(i);
	}
#endif
}

int SysMon::AddCounter(const char *pszCounterName)
{
	PPDHCOUNTERSTRUCT pCounter;
	pCounter = new PDHCOUNTERSTRUCT;
	if (!pCounter) return -1;

	// add to current query
	if (PdhAddCounter(m_hQuery, pszCounterName, (DWORD)pCounter, &(pCounter->hCounter)) != ERROR_SUCCESS)
	{
		delete pCounter; // clean mem
		return -1;
	}

	// insert counter into array(s)
	pCounter->nIndex = m_nNextIndex++;
	pCounter->lValue = 0;
	pCounter->nNextIndex = 0;
	pCounter->nOldestIndex = 0;
	pCounter->nRawCount = 0;
	//m_aCounters.Add(pCounter);
	m_aCounters.push_back(pCounter);

	//printf("index:%d\n", pCounter->nIndex);

	return pCounter->nIndex;
}

bool SysMon::RemoveCounter(int nIndex)
{
	PPDHCOUNTERSTRUCT pCounter = getCounterStruct(nIndex);
	if (!pCounter) return false;
	
	if (PdhRemoveCounter(pCounter->hCounter) != ERROR_SUCCESS)
		return false;
		
	return true;
}

bool SysMon::CollectQueryData()
{
	if (PdhCollectQueryData(m_hQuery) != ERROR_SUCCESS) return false;

	return true;
}
string SysMon::getLocalIPAddr()
{
	WSAData wsaData; 

    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) return ""; 

    char slocal[256]; 

    if (gethostname(slocal, 256) == SOCKET_ERROR) return ""; 
    hostent *hos = gethostbyname(slocal); 
    if(hos == NULL) return ""; 

	IN_ADDR addr;
    char IPAddr[256]; 

     if(hos->h_addr_list[0] != NULL)
     {
		 memcpy(&addr, hos->h_addr_list[0], hos->h_length);
		 sprintf(IPAddr, "%s", inet_ntoa(addr));
		 //printf("%s\n", IPAddr);
     }
	
    WSACleanup(); 
    return string(IPAddr); 
};

bool SysMon::updateValue(PPDHCOUNTERSTRUCT pCounter)
{
	PDH_FMT_COUNTERVALUE pdhFormattedValue;

	// get the value from the PDH
	if (PdhGetFormattedCounterValue(pCounter->hCounter, PDH_FMT_LONG, NULL, &pdhFormattedValue) != ERROR_SUCCESS)
		return false;

	// test the value for validity
	if (pdhFormattedValue.CStatus != ERROR_SUCCESS)
		return false;

	// set value
	pCounter->lValue = pdhFormattedValue.longValue;

	return true;
}

bool SysMon::updateRawValue(PPDHCOUNTERSTRUCT pCounter)
{
    PPDH_RAW_COUNTER ppdhRawCounter;

    // Assign the next value into the array
    ppdhRawCounter = &(pCounter->a_RawValue[pCounter->nNextIndex]);

	if (PdhGetRawCounterValue(pCounter->hCounter, NULL, ppdhRawCounter) != ERROR_SUCCESS)
		return false;
	
    // update raw counter - up to MAX_RAW_VALUES
    pCounter->nRawCount = min(pCounter->nRawCount + 1, MAX_RAW_VALUES);

    // Update next index - rolls back to zero upon reaching MAX_RAW_VALUES
    pCounter->nNextIndex = (pCounter->nNextIndex + 1) % MAX_RAW_VALUES;

    // The Oldest index remains zero until the array is filled.
    // It will now be the same as the 'next' index since it was previously assigned.
    if (pCounter->nRawCount >= MAX_RAW_VALUES)
        pCounter->nOldestIndex = pCounter->nNextIndex;

	return true;
}

bool SysMon::GetStatistics(long *nMin, long *nMax, long *nMean, int nIndex)
{
	PDH_STATISTICS pdhStats;
	PPDHCOUNTERSTRUCT pCounter = getCounterStruct(nIndex);
	if (!pCounter) return false;

	if (PdhComputeCounterStatistics(pCounter->hCounter, PDH_FMT_LONG, pCounter->nOldestIndex, pCounter->nRawCount, pCounter->a_RawValue, &pdhStats) != ERROR_SUCCESS) 
		return false;

	// set values
	if (pdhStats.min.CStatus != ERROR_SUCCESS)
		*nMin = 0;
	else
		*nMin = pdhStats.min.longValue;

	if (pdhStats.max.CStatus != ERROR_SUCCESS)
		*nMax = 0;
	else
		*nMax = pdhStats.max.longValue;

	if (pdhStats.mean.CStatus != ERROR_SUCCESS)
		*nMean = 0;
	else
		*nMean = pdhStats.mean.longValue;

	return true;
}

long SysMon::GetCounterValue(int nIndex)
{
	PPDHCOUNTERSTRUCT pCounter = getCounterStruct(nIndex);
	if (!pCounter) return -999L;

	// update the value(s)
	if (!updateValue(pCounter)) return -999L;
	if (!updateRawValue(pCounter)) return -999L;

	// return the value
	return pCounter->lValue;
}


PPDHCOUNTERSTRUCT SysMon::getCounterStruct(int nIndex)
{
#if 1
	for (size_t i=0;i<m_aCounters.size();i++)
	{
		if (m_aCounters.at(i)->nIndex == nIndex)
			return m_aCounters.at(i);
	}
#endif
	return NULL;
}

DWORD SysMon::getABSSecond (SYSTEMTIME st)
{
	INT64 i64;
	FILETIME	fst;

	SystemTimeToFileTime(&st, &fst);
	i64 = (((INT64)fst.dwHighDateTime) << 32 ) + fst.dwLowDateTime;

	i64 = (i64/10000000) - ((INT64)145731*86400);

	return (DWORD)i64;
}

void SysMon::absSecondToSystem (DWORD abs, SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	i64 = (abs + (INT64)145731 * 86400)*10000000;

	fst.dwHighDateTime = (DWORD)(i64>>32);
	fst.dwLowDateTime = (DWORD)(i64 & 0xffffffff);

	FileTimeToSystemTime(&fst, &st);
}

void SysMon::DiskSpaceCheck()
{
	UINT type;
	char drive[32];
	ULARGE_INTEGER Avail, Total;
	mTotal = 0, mAvail = 0, mUsed = 0;
	const int MEGA = 1048576; //1024*1024

	for(int i = 'C'; i<='Z'; i++)
	{
		sprintf(drive, "%c:\\",i);
		type = GetDriveType(drive);
		if(type == DRIVE_FIXED)
		{
			GetDiskFreeSpaceEx(drive, &Avail, &Total, NULL);
			mTotal += double(Total.QuadPart/MEGA);
			mAvail += double(Avail.QuadPart/MEGA);
		};
	}
	mUsed = mTotal-mAvail; 
	//printf("Total:%d, Avail:%d, Used:%d[%f%%]\n", (int)mTotal, (int)mAvail, (int)mUsed, (mUsed/mTotal)*100);
}

//void SysMon::CheckTime(string &sboottime, string &spasttime)
void SysMon::CheckTime()
{
	char str[255];
	LARGE_INTEGER now, freq;

	SYSTEMTIME st, boot;

	int sec, min, hour, day;
	DWORD abs;

	QueryPerformanceCounter(&now);
	QueryPerformanceFrequency(&freq);
	sec = (int)(now.QuadPart/freq.QuadPart);

	day  = sec/86400;
	hour = (sec%86400)/3600;
	min = (sec%3600)/60;

	GetLocalTime(&st);
	abs=getABSSecond (st);

	absSecondToSystem (abs-sec, boot);

	//sprintf(str, "%d/%d/%d %d:%d", boot.wYear, boot.wMonth, boot.wDay, boot.wHour, boot.wMinute);
	sprintf(str, "%d/%d/%d %d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	m_curtime = string(str);

	//sprintf(str, "%d days %d hour %d min %d sec", day, hour, min, sec%60);
	sprintf(str, "%d days %d:%d", day, hour, min);
	m_uptime = string(str);
}
unsigned int SysMon::GetSysProcessStatus()
{
    //TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
        return (0);
	};

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    char szProcessName[MAX_PATH];
	unsigned int procstatus = 0;
	HANDLE hProcess = 0;

    // Print the name and process identifier for each process.
    for ( DWORD i = 0; i < cProcesses; i++ )
	{
        if( aProcesses[i] != 0 )
		{
			//hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_ALL_ACCESS, TRUE, aProcesses[i] );
			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, aProcesses[i] );
			if (NULL != hProcess )
			{
				HMODULE hMod;
				DWORD cbNeeded;
				// Get a handle to the process.
				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, MAX_PATH);
					//GetModuleFileNameEx( hProcess, hMod, szProcessName, MAX_PATH);

					//printf("Modname:%s\n", szProcessName);
					if(strcmp(szProcessName, "caRepeater.exe") == 0)
						procstatus |= CAREPEATER;
#if 0
					// if it can be checked ntpd.exe, then commnet out
					else if(strcmp(szProcessName, "ntpd.exe") == 0)
						procstatus |= NTPD;
#endif
				};
			};
			// Print the process name and identifier.
			CloseHandle( hProcess );
		};
	};

	if(BitCheck(procstatus, CAREPEATER) == 1)
		procstatus &= ~CAREPEATER;
	else if(BitCheck(procstatus, CAREPEATER) == 0)
		procstatus |= CAREPEATER;

#if 0
	if(BitCheck(procstatus, NTPD) == 1)
		procstatus &= ~NTPD;
	else if(BitCheck(procstatus, NTPD) == 0)
		procstatus |= NTPD;
#endif

	//printf("%s\n", printBitString(procstatus).c_str());;
	return (procstatus);
}

string SysMon::printBitString(unsigned int bitval)
{
	unsigned int ckVal = ~0u - (~0u >> 1);
	char bitChar[64];
	int count = 0;
	while(ckVal !=0)
	{
		if( (bitval & ckVal)!=0) {
			bitChar[count] = '1';
		}else{
			bitChar[count] = '0';
		};

		ckVal = ckVal >> 1;
		count++;
	};

	bitChar[count] = '\0';
	
	return (string (bitChar));
}

SysMon	sysPerfMon;

typedef void (*EPICS_EXIT_HANDLER) (void* arg);

//New String Record
static long init_record_s (void *pStringIn);
static long readSysName (void *pStringIn);

struct devSysName_tag{
	long	number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
}devSysName = {
	//num=5
	5,
	NULL,
	NULL,
	init_record_s,
	NULL,
	readSysName
};
extern "C" {
	epicsExportAddress(dset, devSysName);
};

static long init_record_s(void *pStringIn)
{
	struct stringinRecord *pstrin = (struct stringinRecord*)pStringIn;
	if(recGblInitConstantLink(&pstrin->inp, DBF_STRING, &pstrin->val))
		pstrin->udf=FALSE;

	return(0);
}

static long readSysName(void *pStringIn)
{
	struct stringinRecord *pstrin = (struct stringinRecord*)pStringIn;

	struct vmeio *pVmeIO = &pstrin->inp.value.vmeio;
	string strParam = pVmeIO->parm;

	char strBuff[255];
	int len = 256;
	if(strParam.compare("SYSNAME") == 0 )
	{
		//Set System Name.
		::GetComputerName(strBuff, reinterpret_cast<LPDWORD>(&len));
	}
	else if(strParam.compare("USER") == 0 )
	{
		::GetUserName(strBuff, reinterpret_cast<LPDWORD>(&len));
	}
	else if(strParam.compare("OSVER") == 0 )
	{
		OSVERSIONINFOEX osvx;
		ZeroMemory(&osvx, sizeof(OSVERSIONINFOEX));
		osvx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		VerifyVersionInfo(&osvx, 0, 0);
		if((osvx.wProductType==VER_NT_WORKSTATION)&&(osvx.dwMajorVersion==6)&&(osvx.dwMinorVersion==1))
		{
			sprintf(strBuff, "%s", "Windows 7");
		}
		else if((osvx.wProductType==VER_NT_WORKSTATION)&&(osvx.dwMajorVersion==6)&&(osvx.dwMinorVersion==0))
		{
			sprintf(strBuff, "%s", "Windows Vista");
		}
		else if((osvx.wProductType!=VER_NT_WORKSTATION)&&(osvx.dwMajorVersion==6)&&(osvx.dwMinorVersion==1))
		{
			sprintf(strBuff, "%s", "Windows Server 2008 R2");
		}
		else if((osvx.wProductType!=VER_NT_WORKSTATION)&&(osvx.dwMajorVersion==6)&&(osvx.dwMinorVersion==0))
		{
			sprintf(strBuff, "%s", "Windows Server 2008");
		}
		else if((osvx.dwMajorVersion==5)&&(osvx.dwMinorVersion==2))
		{
			sprintf(strBuff, "%s", "Windows Server 2003/2003 R2");
		}
		else if((osvx.dwMajorVersion==5)&&(osvx.dwMinorVersion==1))
		{
			sprintf(strBuff, "%s", "Windows XP");
		}
		//else if((osvx.dwMajorVersion==5)&&(osvx.dwMinorVersion==0))
		else
		{
			sprintf(strBuff, "%s", "Windows 2000");
		};
	}
	else if(strParam.compare("IPADDR") == 0 )
	{
		strcpy(strBuff, sysPerfMon.getLocalIPAddr().c_str());
		//sysPerfMon.getLocalIPAddr();

	};
	strcpy(pstrin->val, strBuff);
	return(0);
}

//New ai record type.
static long init_record_a (void *);
static long readCpuMemInfo (void *pAI);

struct devCpuMemInfo_tag{
	long	number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
	DEVSUPFUN	special_linconv;
}devCpuMemInfo = {
	//num=6 ** ai 경우 이것이 5라면 init_record가 등록되지 않는다.
	6,
	NULL,
	NULL,
	init_record_a,
	NULL,
	readCpuMemInfo,
	NULL
};

extern "C" {
	epicsExportAddress(dset, devCpuMemInfo);
};

long init_record_a(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;
	if(recGblInitConstantLink(&pai->inp, DBF_DOUBLE, &pai->val))
		pai->udf=FALSE;

	return(0);
}

static long readCpuMemInfo(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;
	struct vmeio *pVmeIO = &pai->inp.value.vmeio;
	string strParam = pVmeIO->parm;
	long lvalue = 0;

	if(!sysPerfMon.CollectQueryData())	return (0);

	if(strParam.compare("CPUINFO") == 0 )
	{
		//long lmin, lmax, lmean;
		//sysPerfMon.GetStatistics(&lmin, &lmax, &lmean, 1);
		lvalue = sysPerfMon.GetCounterValue(0);
	}
	else if(strParam.compare("MEMAVINFO") == 0 )
	{
		lvalue = sysPerfMon.GetCounterValue(1);
	}
	else if(strParam.compare("MEMUSEDINFO") == 0 )
	{
		lvalue = sysPerfMon.GetCounterValue(2)/1024;
	}

	pai->val = static_cast<double> (lvalue);

	return(2);
}

static long readSysTime (void *pIn);
static long init_record_t (void *pIn);

struct devReadSysTime_tag{
	long	number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
}devReadSysTime = {
	5,
	NULL,
	NULL,
	init_record_t,
	NULL,
	readSysTime
};

extern "C" {
	epicsExportAddress(dset, devReadSysTime);
};

static long init_record_t(void *pStringIn)
{
	struct stringinRecord *pstrin = (struct stringinRecord*)pStringIn;
	if(recGblInitConstantLink(&pstrin->inp, DBF_STRING, &pstrin->val))
		pstrin->udf=FALSE;

	return(0);
}
static long readSysTime(void *pIn)
{
	struct stringinRecord *pStrIn = (struct stringinRecord*)pIn;
	struct vmeio *pVmeIO = &pStrIn->inp.value.vmeio;

	string strParam = pVmeIO->parm;
	string strtime;

	if(strParam.compare("CURTIME") == 0 )
	{
		sysPerfMon.CheckTime();
		strtime = sysPerfMon.GetCurTime();
	}
	else if(strParam.compare("UPTIME") == 0 )
	{
		strtime = sysPerfMon.GetUpTime();
	}
	strcpy(pStrIn->val, strtime.c_str());

	return(2);
}

static long readDiskSize (void *);
static long init_record_d(void *);

struct devReadDiskSize_tag{
	long	number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
	DEVSUPFUN	special_linconv;
}devReadDiskSize = {
	6,
	NULL,
	NULL,
	init_record_d,
	NULL,
	readDiskSize,
	NULL
};

extern "C" {
	epicsExportAddress(dset, devReadDiskSize);
};

long init_record_d(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;
	if(recGblInitConstantLink(&pai->inp, DBF_DOUBLE, &pai->val))
		pai->udf=FALSE;

	return(0);
}

static long readDiskSize(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;
	struct vmeio *pVmeIO = &pai->inp.value.vmeio;
	string strParam = pVmeIO->parm;

	double disksize = 0;

	if(strParam.compare("DISKTOTAL") == 0 )
	{
		sysPerfMon.DiskSpaceCheck();
		disksize = sysPerfMon.GetTotalSize();
	}
	else if(strParam.compare("DISKAVAIL") == 0 )
	{
		disksize = sysPerfMon.GetAvailSize();
	}
	else if(strParam.compare("DISKUSED") == 0 )
	{
		disksize = sysPerfMon.GetUsedSize();
	}
	pai->val = disksize;
	return(2);
}

static long readProcStatus (void *);
static long init_record_proc(void *);

struct devReadProcStatus_tag{
	long	number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
	DEVSUPFUN	special_linconv;
}devReadProcStatus = {
	6,
	NULL,
	NULL,
	init_record_proc,
	NULL,
	readProcStatus,
	NULL
};

extern "C" {
	epicsExportAddress(dset, devReadProcStatus);
};

long init_record_proc(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;
	if(recGblInitConstantLink(&pai->inp, DBF_DOUBLE, &pai->val))
		pai->udf=FALSE;

	return(0);
}

static long readProcStatus(void *pAI)
{
	struct aiRecord *pai = (struct aiRecord*)pAI;

	struct vmeio *pVmeIO = &pai->inp.value.vmeio;
	string strParam = pVmeIO->parm;

	unsigned int procstatus = 0;

	//printf("SYSPROC:%d\n", sysPerfMon.GetSysProcessStatus());
	if(strParam.compare("SYSPROC") == 0 )
	{
		procstatus = sysPerfMon.GetSysProcessStatus();
	}
	else if(strParam.compare("USERPROC") == 0 )
	{
	}
	pai->val = static_cast<double>(procstatus);
	return(2);
};

