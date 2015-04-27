#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include "cachedchannelaccess.h"

using std::string;

const char  SHM_KEY[]  = "0x50000000";
// buffer size
const int   BUFFER_LEN  = 24;  

typedef struct {
	//char    sval[BUFFER_LEN];
	string  sval;
	int     ival;   
	float   fval;   
} ShmTestTag;


int main()
{
	char            *shmPtr         = NULL;
	ShmTestTag      writeData;
	//ShmTestTag      readData;

	// assign data to the structure buffer

	CachedChannelAccess    chaccess;

	// --------------------------------------------------
	// create shared memory
	// --------------------------------------------------

	// create shared memory
	//shmPtr = chaccess.open (SHM_KEY, 1000, CachedChannelAccess::RT_SHM_CREAT | CachedChannelAccess::RT_SHM_RDWR);
	shmPtr = chaccess.open (SHM_KEY, 10*sizeof(ShmTestTag), CachedChannelAccess::RT_SHM_CREAT | CachedChannelAccess::RT_SHM_RDWR);
	// write message to shared memory
	//chaccess.write (shmPtr, &writeData, sizeof(ShmTestTag));
	char strbuff[10];
	for(int i = 0; i < 10; i++)
	{

		sprintf(strbuff,"PV_NAME_%d",i);
		//strcpy (writeData.sval, strbuff);
		writeData.sval = string(strbuff);
		writeData.ival = 10+i;
		writeData.fval = 10.99+i;
		chaccess.write (i, &writeData, sizeof(ShmTestTag));
		printf("Write:Name(%s), ival(%d), fval(%f)\n", writeData.sval, writeData.ival, writeData.fval);
		sleep(5);
	};

	// close the opened shared memory
	chaccess.close ();
	//chaccess.remove ();
	return 0;
}
