#include "Windows.h"
#include "stdio.h"
#include "winperf.h"
#include "winPerfmon.h"

#define DEFAULT_BUFFER_SIZE 40960L

#pragma comment(lib, "advapi32.lib")

PERF_OBJECT_TYPE *FirstObject(PERF_DATA_BLOCK *dataBlock)
{
  return (PERF_OBJECT_TYPE *) ((BYTE *)dataBlock + dataBlock->HeaderLength);
}

PERF_OBJECT_TYPE *NextObject(PERF_OBJECT_TYPE *act)
{
  return (PERF_OBJECT_TYPE *) ((BYTE *)act + act->TotalByteLength);
}

PERF_COUNTER_DEFINITION *FirstCounter(PERF_OBJECT_TYPE *perfObject)
{
  return (PERF_COUNTER_DEFINITION *) ((BYTE *) perfObject + perfObject->HeaderLength);
}

PERF_COUNTER_DEFINITION *NextCounter(PERF_COUNTER_DEFINITION *perfCounter)
{
  return (PERF_COUNTER_DEFINITION *) ((BYTE *) perfCounter + perfCounter->ByteLength);
}

PERF_COUNTER_BLOCK *GetCounterBlock(PERF_INSTANCE_DEFINITION *pInstance)
{
  return (PERF_COUNTER_BLOCK *) ((BYTE *)pInstance + pInstance->ByteLength);
}

PERF_INSTANCE_DEFINITION *FirstInstance (PERF_OBJECT_TYPE *pObject)
{
  return (PERF_INSTANCE_DEFINITION *)  ((BYTE *) pObject + pObject->DefinitionLength);
}

PERF_INSTANCE_DEFINITION *NextInstance (PERF_INSTANCE_DEFINITION *pInstance)
{
  // next instance is after
  //    this instance + this instances counter data
  PERF_COUNTER_BLOCK  *pCtrBlk = GetCounterBlock(pInstance);
  return (PERF_INSTANCE_DEFINITION *) ((BYTE *)pInstance + pInstance->ByteLength + pCtrBlk->ByteLength);
}

char *WideToMulti(wchar_t *source, char *dest, int size)
{
  WideCharToMultiByte(CP_ACP, 0, source, -1, dest, size, 0, 0);

  return dest;
}

CAWinPerfmon::CAWinPerfmon()
{
	lasttraffic = 0.0;
	CurrentInterface = -1;
	CurrentTrafficType = AllTraffic;
	GetInterfaces();
}
CAWinPerfmon::~CAWinPerfmon()
{
}
int CAWinPerfmon::GetInterfaceTotalTraffic(int index)
{
	int		totaltraffic = 0;
#if 0
	POSITION	pos;
	pos= TotalTraffics.FindIndex(index);
	if(pos!=NULL)
	{
		totaltraffic = TotalTraffics.GetAt(pos); 
		if(totaltraffic == 0.0)
		{
			GetTraffic(index);
			pos= TotalTraffics.FindIndex(index);
			if(pos!=NULL)
			{
				totaltraffic = TotalTraffics.GetAt(pos); 
			}
		}
	}
#else
	totaltraffic = TotalTraffics[index];
	if(totaltraffic == 0.0)
	{
		GetTraffic(index);
		totaltraffic = TotalTraffics[index]; 
	}
#endif

	return(totaltraffic);
}
double CAWinPerfmon::GetTraffic(int interfaceNumber)
{
	try
	{
		string InterfaceName = Interfaces[interfaceNumber];
		// buffer for performance data
		unsigned char *data = new unsigned char [DEFAULT_BUFFER_SIZE];
		// return value from RegQueryValueEx: ignored for this application
		int type;
		// Buffer size
		int size = DEFAULT_BUFFER_SIZE;
		// return value of RegQueryValueEx
		int ret;
		
#if 1 
		// request performance data from network object (index 510) 
		while((ret = RegQueryValueEx(HKEY_PERFORMANCE_DATA, reinterpret_cast<LPCSTR> ("510"), 0, 
						reinterpret_cast<LPDWORD> (&type), data, reinterpret_cast<LPDWORD> (&size))) != ERROR_SUCCESS) 
		{
			if(ret == ERROR_MORE_DATA) 
			{
				// buffer size was too small, increase allocation size
				size += DEFAULT_BUFFER_SIZE;
				delete [] data;
				data = new unsigned char [size];
			} 
			else 
			{
				// some unspecified error has occured
				return 1;
			};
		};
#endif

		PERF_DATA_BLOCK *dataBlockPtr = (PERF_DATA_BLOCK *)data;
		
		// enumerate first object of list
		PERF_OBJECT_TYPE *objectPtr = FirstObject(dataBlockPtr);
		
		for(int a=0 ; a<(int)dataBlockPtr->NumObjectTypes ; a++) 
		{
			char nameBuffer[255];
			
			if(objectPtr->ObjectNameTitleIndex == 510) 
			{
				// Calculate the offset
				int processIdOffset = ULONG_MAX;
				
				PERF_COUNTER_DEFINITION *counterPtr = FirstCounter(objectPtr);
				for(int b=0 ; b<(int)objectPtr->NumCounters ; b++) 
				{
					if((int)counterPtr->CounterNameTitleIndex == CurrentTrafficType)
						processIdOffset = counterPtr->CounterOffset;
					counterPtr = NextCounter(counterPtr);
				};
				if(processIdOffset == ULONG_MAX) {
					delete [] data;
					return 1;
				}
				
				// Find first instance
				PERF_INSTANCE_DEFINITION *instancePtr = FirstInstance(objectPtr);
				int fullTraffic;
				int traffic;
				for(int b=0 ; b<objectPtr->NumInstances ; b++) 
				{
					// evaluate pointer to name
					wchar_t *namePtr = (wchar_t *) ((BYTE *)instancePtr + instancePtr->NameOffset);
					
					// get PERF_COUNTER_BLOCK of this instance
					PERF_COUNTER_BLOCK *counterBlockPtr = GetCounterBlock(instancePtr);
					
					// now we have the interface name
					char *pName = WideToMulti(namePtr, nameBuffer, sizeof(nameBuffer));
					fullTraffic = *((int *) ((BYTE *)counterBlockPtr + processIdOffset));
					//TotalTraffics[b]=fullTraffic;

					// If the interface the currently selected interface?
					if(InterfaceName.compare(pName) == 0)
					{
						traffic = *((int *) ((BYTE *)counterBlockPtr + processIdOffset));
						double acttraffic = (double)traffic;
						double trafficdelta;
						// Do we handle a new interface (e.g. due a change of the interface number
#if 0
						if(CurrentInterface != interfaceNumber)
						{
							lasttraffic = acttraffic;
							trafficdelta = 0.0;
							CurrentInterface = interfaceNumber;
						}
						else
#endif
						{
							trafficdelta = acttraffic - lasttraffic;
							lasttraffic = acttraffic;
						}
						delete [] data;
						return(trafficdelta);
					}
					// next instance
					instancePtr = NextInstance(instancePtr);
				}
			}
			// next object in list
			objectPtr = NextObject(objectPtr);
		}
		delete [] data;
		return 0;
	}
	catch(...)
	{
		return 0;
	}
}
void CAWinPerfmon::SetTrafficType(int trafficType)
{
	CurrentTrafficType = trafficType;
}
BOOL CAWinPerfmon::GetInterfaces()
{
	try
	{
		unsigned char *data = (unsigned char*)malloc(DEFAULT_BUFFER_SIZE);
		DWORD type;
		DWORD size = DEFAULT_BUFFER_SIZE;
		DWORD ret;
		Interfaces.clear();

		memset(data, 0, size);

		char s_key[4096];
		sprintf_s(s_key, "%d", 510);
		
		ret = RegQueryValueEx(HKEY_PERFORMANCE_DATA, s_key, 0, &type, data, &size);
		while(ret != ERROR_SUCCESS) 
		{
			while(ret == ERROR_MORE_DATA) 
			{
				size += DEFAULT_BUFFER_SIZE;
				data = (unsigned char*) realloc(data, size);
			} 
			if(ret != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}
		PERF_DATA_BLOCK	 *dataBlockPtr = (PERF_DATA_BLOCK *)data;
		dataBlockPtr;
		PERF_OBJECT_TYPE *objectPtr = FirstObject(dataBlockPtr);
		//printf("NumObjType :%d, DataPointer: %p, ObjTypePointer: %p\n",(int)dataBlockPtr->NumObjectTypes,dataBlockPtr,  objectPtr);
		for(int a=0 ; a<(int)dataBlockPtr->NumObjectTypes ; a++) 
		{
			char nameBuffer[255];
			if(objectPtr->ObjectNameTitleIndex == 510) 
			{
				DWORD processIdOffset = ULONG_MAX;
				PERF_COUNTER_DEFINITION *counterPtr = FirstCounter(objectPtr);
				
				for(int b=0 ; b<(int)objectPtr->NumCounters ; b++) 
				{
					if(counterPtr->CounterNameTitleIndex == 520)
						processIdOffset = counterPtr->CounterOffset;
					
					counterPtr = NextCounter(counterPtr);
				}
				
				if(processIdOffset == ULONG_MAX) {
					free(data);
					return 1;
				}
				
				PERF_INSTANCE_DEFINITION *instancePtr = FirstInstance(objectPtr);
				
				for(int b=0 ; b<objectPtr->NumInstances ; b++) 
				{
					wchar_t *namePtr = (wchar_t *) ((BYTE *)instancePtr + instancePtr->NameOffset);
					PERF_COUNTER_BLOCK *counterBlockPtr = GetCounterBlock(instancePtr);
					char *pName = WideToMulti(namePtr, nameBuffer, sizeof(nameBuffer));
					DWORD bandwith = *((DWORD *) ((BYTE *)counterBlockPtr + processIdOffset));				
					DWORD tottraff = 0;
					printf("Name:%s\n",pName);
					Interfaces.push_back(pName);
#if 0
					Bandwidths[b] = bandwith;
					TotalTraffics[b] = tottraff;  // initial 0, just for creating the list
					instancePtr = NextInstance(instancePtr);
#endif
					//Bandwidths.AddTail(bandwith);
					//TotalTraffics.AddTail(tottraff);  // initial 0, just for creating the list
				};
			};
			objectPtr = NextObject(objectPtr);
		};
		free(data);
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}
string CAWinPerfmon::GetInterfaceName(const int index)
{
	return Interfaces.at(index);
}

string CAWinPerfmon::GetSystemName()
{
	//SYSTEM_INFO si;
	//GetSystemInfo(&si);
	char sysName[255];
	int len = 256;
	GetComputerName(sysName, reinterpret_cast<LPDWORD>(&len));
	return string(sysName);
}
string CAWinPerfmon::GetUserName()
{
	char userName[64];
	int len = 64;
	::GetUserName(userName, reinterpret_cast<LPDWORD>(&len));
	return string(userName);
}
