#ifndef SFW_INIT_CONTROL_H
#define SFW_INIT_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include <epicsExport.h>
#include <iocsh.h>

void initControl(const char *name);

/* Information needed by iocsh */
static const iocshArg     initControlArg0 = {"name", iocshArgString};
static const iocshArg    *initControlArgs[] = {&initControlArg0};
static const iocshFuncDef initControlFuncDef = {"initControl", 1, initControlArgs};

/* Wrapper called by iocsh, selects the argument types that initControl needs */
static void initControlCallFunc(const iocshArgBuf *args) {
    initControl(args[0].sval);
}

/* Registration routine, runs at startup */
static void initControlRegister(void) {
    iocshRegister(&initControlFuncDef, initControlCallFunc);
}
epicsExportRegistrar(initControlRegister);

#ifdef __cplusplus
}
#endif

#endif

