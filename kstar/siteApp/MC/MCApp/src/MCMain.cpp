/* MCMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "shareLib.h"
#include "epicsTypes.h"
#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"

/*typedef epicsFloat64 float64;*/


#if 0
//static void exitHandler(int)
void exitHandler(int)
{
	clearAllVXITasks();
     /*   exitSafeIOC();
	epicsAtExit((EPICSEXITFUNC) clearAllVXITasks, NULL);
	clearAllVXITasks(); */
}
#endif
int main(int argc,char *argv[])
{
    if(argc>=2) {    
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
#if 0	
        /*
	epicsAtExit((EPICS_EXIT_HANDLER)exitSafeIOC, NULL);
        signal(SIGINT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
	*/
        signal(SIGTERM, exitHandler);
        signal(SIGINT, exitHandler);
#endif
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
