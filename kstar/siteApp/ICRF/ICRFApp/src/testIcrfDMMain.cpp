/* testIcrfDMMain.cpp */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "epicsThread.h"
#include "iocsh.h"

int main(int argc, char *argv[])
{
    if(argc>=2) {
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    return(0);
}

