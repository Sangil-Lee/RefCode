#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"
#include "userclass.h"
#include "signal.h"


extern IfDriver	*pifdriver;

//typedef void (*EPICS_EXIT_HANDLER)(void *arg);
#if 1
static void exitSafeIOC(void)
{
    printf("IOC safe exit routine...\n");
	exit(0);
}
#endif
static void exitHandler(int)
{
	exitSafeIOC();
}

int main(int argc,char *argv[])
{
    if(argc>=2) {    
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
	//epicsAtExit((EPICS_EXIT_HANDLER)exitSafeIOC, NULL);
	//signal(SIGINT, SIG_IGN);
	//signal(SIGTERM, SIG_IGN);
	signal(SIGTERM, exitHandler);
	signal(SIGINT, exitHandler);
    iocsh(NULL);
    epicsExit(0);
	delete pifdriver;
    return(0);
}
