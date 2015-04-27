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
	ShmTestTag      readData;

	// assign data to the structure buffer

	CachedChannelAccess    chaccess;

	// --------------------------------------------------
	// create shared memory
	// --------------------------------------------------

	// create shared memory
	//shmPtr = chaccess.open (SHM_KEY, 1000, CachedChannelAccess::RT_SHM_CREAT | CachedChannelAccess::RT_SHM_RDWR);
	shmPtr = chaccess.open (SHM_KEY, 10*sizeof(ShmTestTag), CachedChannelAccess::RT_SHM_RDONLY);

	char strbuff[10];
	for(int i = 0; i < 10; i++)
	{
		chaccess.read (i, &readData, sizeof(ShmTestTag));
		printf("PVName:%s,ival(%d),fval(%f)\n", readData.sval.c_str(), readData.ival, readData.fval);
		sleep(5);
	};

	// close the opened shared memory
	chaccess.close ();
	//chaccess.remove ();

	return 0;
}
