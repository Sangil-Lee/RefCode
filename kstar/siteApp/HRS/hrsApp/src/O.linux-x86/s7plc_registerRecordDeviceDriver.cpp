/* THIS IS A GENERATED FILE. DO NOT EDIT */
/* Generated from s7plc.dbd */

#include "registryCommon.h"

extern "C" {
epicsShareExtern dset *pvar_dset_s7plcMbboDirect;
epicsShareExtern dset *pvar_dset_s7plcMbbiDirect;
epicsShareExtern dset *pvar_dset_s7plcBi;
epicsShareExtern dset *pvar_dset_s7plcBo;
epicsShareExtern dset *pvar_dset_s7plcLongin;
epicsShareExtern dset *pvar_dset_s7plcLongout;
epicsShareExtern dset *pvar_dset_s7plcMbbo;
epicsShareExtern dset *pvar_dset_s7plcMbbi;
epicsShareExtern dset *pvar_dset_s7plcAo;
epicsShareExtern dset *pvar_dset_s7plcAi;
epicsShareExtern dset *pvar_dset_s7plcStringout;
epicsShareExtern dset *pvar_dset_s7plcStringin;
epicsShareExtern dset *pvar_dset_s7plcWaveform;
epicsShareExtern dset *pvar_dset_s7plcStat;
epicsShareExtern dset *pvar_dset_s7plcCalcout;

static const char * const deviceSupportNames[15] = {
    "s7plcMbboDirect",
    "s7plcMbbiDirect",
    "s7plcBi",
    "s7plcBo",
    "s7plcLongin",
    "s7plcLongout",
    "s7plcMbbo",
    "s7plcMbbi",
    "s7plcAo",
    "s7plcAi",
    "s7plcStringout",
    "s7plcStringin",
    "s7plcWaveform",
    "s7plcStat",
    "s7plcCalcout"
};

static const dset * const devsl[15] = {
    pvar_dset_s7plcMbboDirect,
    pvar_dset_s7plcMbbiDirect,
    pvar_dset_s7plcBi,
    pvar_dset_s7plcBo,
    pvar_dset_s7plcLongin,
    pvar_dset_s7plcLongout,
    pvar_dset_s7plcMbbo,
    pvar_dset_s7plcMbbi,
    pvar_dset_s7plcAo,
    pvar_dset_s7plcAi,
    pvar_dset_s7plcStringout,
    pvar_dset_s7plcStringin,
    pvar_dset_s7plcWaveform,
    pvar_dset_s7plcStat,
    pvar_dset_s7plcCalcout
};

epicsShareExtern drvet *pvar_drvet_s7plc;

static const char *driverSupportNames[1] = {
    "s7plc"
};

static struct drvet *drvsl[1] = {
    pvar_drvet_s7plc
};

epicsShareExtern void (*pvar_func_s7plcRegister)(void);

epicsShareExtern int *pvar_int_s7plcDebug;
static struct iocshVarDef vardefs[] = {
	{"s7plcDebug", iocshArgInt, (void * const)pvar_int_s7plcDebug},
	{NULL, iocshArgInt, NULL}
};

int s7plc_registerRecordDeviceDriver(DBBASE *pbase)
{
    registerDevices(pbase, 15, deviceSupportNames, devsl);
    registerDrivers(pbase, 1, driverSupportNames, drvsl);
    (*pvar_func_s7plcRegister)();
    iocshRegisterVariable(vardefs);
    return 0;
}

/* registerRecordDeviceDriver */
static const iocshArg registerRecordDeviceDriverArg0 =
                                            {"pdbbase",iocshArgPdbbase};
static const iocshArg *registerRecordDeviceDriverArgs[1] =
                                            {&registerRecordDeviceDriverArg0};
static const iocshFuncDef registerRecordDeviceDriverFuncDef =
                {"s7plc_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    s7plc_registerRecordDeviceDriver(pdbbase);
}

} // extern "C"
/*
 * Register commands on application startup
 */
#include "iocshRegisterCommon.h"
class IoccrfReg {
  public:
    IoccrfReg() {
        iocshRegisterCommon();
        iocshRegister(&registerRecordDeviceDriverFuncDef,registerRecordDeviceDriverCallFunc);
    }
};
#if !defined(__GNUC__) || !(__GNUC__<2 || (__GNUC__==2 && __GNUC_MINOR__<=95))
namespace { IoccrfReg iocshReg; }
#else
IoccrfReg iocshReg;
#endif
