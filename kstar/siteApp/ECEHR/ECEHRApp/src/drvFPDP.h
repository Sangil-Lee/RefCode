#ifndef DRV_FPDP_H
#define DRV_FPDP_H

#include <semaphore.h>

#if 0
#include "dpio2.h"
#endif

#define FILE_NAME_RAWDATA		"/data/rawdata.dat"


#define FPDP_START		1
#define FPDP_STOP			2
#define FPDP_OPENFILE	3
#define FPDP_KILL			99



/* Globals */
int          dmaSize;
/*unsigned int cnt_DMAcallback; */

/*int      *buffer_in;*/
unsigned int     *buffer_in;

int 		*buffer_A;
int 		*buffer_B;
int 		*buffer_write;
int		dmaToggle;

sem_t dmaDoneSemaphore_in;

/*int msgFPDP; */
/*int usedCH; */



int drvM6802_init_FPDP();
int call_inLoop();
int doCacheInvalidate();
int  startFpdp();
int stopFpdp();
void terminate();






#endif
