/* THIS IS A GENERATED FILE. DO NOT EDIT */
/* Generated from ../O.Common/s7plcApp.dbd */

#include "registryCommon.h"

extern "C" {
epicsShareExtern rset *pvar_rset_aiRSET;
epicsShareExtern int (*pvar_func_aiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aoRSET;
epicsShareExtern int (*pvar_func_aoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_biRSET;
epicsShareExtern int (*pvar_func_biRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_boRSET;
epicsShareExtern int (*pvar_func_boRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcRSET;
epicsShareExtern int (*pvar_func_calcRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcoutRSET;
epicsShareExtern int (*pvar_func_calcoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_compressRSET;
epicsShareExtern int (*pvar_func_compressRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_dfanoutRSET;
epicsShareExtern int (*pvar_func_dfanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_eventRSET;
epicsShareExtern int (*pvar_func_eventRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_fanoutRSET;
epicsShareExtern int (*pvar_func_fanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longinRSET;
epicsShareExtern int (*pvar_func_longinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longoutRSET;
epicsShareExtern int (*pvar_func_longoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiRSET;
epicsShareExtern int (*pvar_func_mbbiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiDirectRSET;
epicsShareExtern int (*pvar_func_mbbiDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboRSET;
epicsShareExtern int (*pvar_func_mbboRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboDirectRSET;
epicsShareExtern int (*pvar_func_mbboDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_permissiveRSET;
epicsShareExtern int (*pvar_func_permissiveRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_selRSET;
epicsShareExtern int (*pvar_func_selRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_seqRSET;
epicsShareExtern int (*pvar_func_seqRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stateRSET;
epicsShareExtern int (*pvar_func_stateRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringinRSET;
epicsShareExtern int (*pvar_func_stringinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringoutRSET;
epicsShareExtern int (*pvar_func_stringoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subRSET;
epicsShareExtern int (*pvar_func_subRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subArrayRSET;
epicsShareExtern int (*pvar_func_subArrayRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_waveformRSET;
epicsShareExtern int (*pvar_func_waveformRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_timestampRSET;
epicsShareExtern int (*pvar_func_timestampRecordSizeOffset)(dbRecordType *pdbRecordType);

static const char * const recordTypeNames[26] = {
    "ai",
    "ao",
    "bi",
    "bo",
    "calc",
    "calcout",
    "compress",
    "dfanout",
    "event",
    "fanout",
    "longin",
    "longout",
    "mbbi",
    "mbbiDirect",
    "mbbo",
    "mbboDirect",
    "permissive",
    "sel",
    "seq",
    "state",
    "stringin",
    "stringout",
    "sub",
    "subArray",
    "waveform",
    "timestamp"
};

static const recordTypeLocation rtl[26] = {
    {pvar_rset_aiRSET, pvar_func_aiRecordSizeOffset},
    {pvar_rset_aoRSET, pvar_func_aoRecordSizeOffset},
    {pvar_rset_biRSET, pvar_func_biRecordSizeOffset},
    {pvar_rset_boRSET, pvar_func_boRecordSizeOffset},
    {pvar_rset_calcRSET, pvar_func_calcRecordSizeOffset},
    {pvar_rset_calcoutRSET, pvar_func_calcoutRecordSizeOffset},
    {pvar_rset_compressRSET, pvar_func_compressRecordSizeOffset},
    {pvar_rset_dfanoutRSET, pvar_func_dfanoutRecordSizeOffset},
    {pvar_rset_eventRSET, pvar_func_eventRecordSizeOffset},
    {pvar_rset_fanoutRSET, pvar_func_fanoutRecordSizeOffset},
    {pvar_rset_longinRSET, pvar_func_longinRecordSizeOffset},
    {pvar_rset_longoutRSET, pvar_func_longoutRecordSizeOffset},
    {pvar_rset_mbbiRSET, pvar_func_mbbiRecordSizeOffset},
    {pvar_rset_mbbiDirectRSET, pvar_func_mbbiDirectRecordSizeOffset},
    {pvar_rset_mbboRSET, pvar_func_mbboRecordSizeOffset},
    {pvar_rset_mbboDirectRSET, pvar_func_mbboDirectRecordSizeOffset},
    {pvar_rset_permissiveRSET, pvar_func_permissiveRecordSizeOffset},
    {pvar_rset_selRSET, pvar_func_selRecordSizeOffset},
    {pvar_rset_seqRSET, pvar_func_seqRecordSizeOffset},
    {pvar_rset_stateRSET, pvar_func_stateRecordSizeOffset},
    {pvar_rset_stringinRSET, pvar_func_stringinRecordSizeOffset},
    {pvar_rset_stringoutRSET, pvar_func_stringoutRecordSizeOffset},
    {pvar_rset_subRSET, pvar_func_subRecordSizeOffset},
    {pvar_rset_subArrayRSET, pvar_func_subArrayRecordSizeOffset},
    {pvar_rset_waveformRSET, pvar_func_waveformRecordSizeOffset},
    {pvar_rset_timestampRSET, pvar_func_timestampRecordSizeOffset}
};

epicsShareExtern dset *pvar_dset_devAiSoft;
epicsShareExtern dset *pvar_dset_devAiSoftRaw;
epicsShareExtern dset *pvar_dset_s7plcAi;
epicsShareExtern dset *pvar_dset_devAvgLoad;
epicsShareExtern dset *pvar_dset_devCPULoad;
epicsShareExtern dset *pvar_dset_devMEMLoad;
epicsShareExtern dset *pvar_dset_devStatusCheck;
epicsShareExtern dset *pvar_dset_devDiskUsage;
epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devAoSoftRaw;
epicsShareExtern dset *pvar_dset_devAoSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcAo;
epicsShareExtern dset *pvar_dset_devBiSoft;
epicsShareExtern dset *pvar_dset_devBiSoftRaw;
epicsShareExtern dset *pvar_dset_s7plcBi;
epicsShareExtern dset *pvar_dset_s7plcStat;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devBoSoftRaw;
epicsShareExtern dset *pvar_dset_devBoSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcBo;
epicsShareExtern dset *pvar_dset_devCalcoutSoft;
epicsShareExtern dset *pvar_dset_devCalcoutSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcCalcout;
epicsShareExtern dset *pvar_dset_devEventSoft;
epicsShareExtern dset *pvar_dset_devLiSoft;
epicsShareExtern dset *pvar_dset_s7plcLongin;
epicsShareExtern dset *pvar_dset_devLoSoft;
epicsShareExtern dset *pvar_dset_devLoSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcLongout;
epicsShareExtern dset *pvar_dset_devMbbiSoft;
epicsShareExtern dset *pvar_dset_devMbbiSoftRaw;
epicsShareExtern dset *pvar_dset_s7plcMbbi;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoft;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoftRaw;
epicsShareExtern dset *pvar_dset_s7plcMbbiDirect;
epicsShareExtern dset *pvar_dset_devMbboSoft;
epicsShareExtern dset *pvar_dset_devMbboSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcMbbo;
epicsShareExtern dset *pvar_dset_devMbboDirectSoft;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcMbboDirect;
epicsShareExtern dset *pvar_dset_devSiSoft;
epicsShareExtern dset *pvar_dset_devTimestampSI;
epicsShareExtern dset *pvar_dset_s7plcStringin;
epicsShareExtern dset *pvar_dset_devUpTime;
epicsShareExtern dset *pvar_dset_devUname;
epicsShareExtern dset *pvar_dset_devIpAddr;
epicsShareExtern dset *pvar_dset_devRemoteCommander;
epicsShareExtern dset *pvar_dset_devSoSoft;
epicsShareExtern dset *pvar_dset_devSoSoftCallback;
epicsShareExtern dset *pvar_dset_s7plcStringout;
epicsShareExtern dset *pvar_dset_devSASoft;
epicsShareExtern dset *pvar_dset_devWfSoft;
epicsShareExtern dset *pvar_dset_s7plcWaveform;
epicsShareExtern dset *pvar_dset_devTimestampSoft;

static const char * const deviceSupportNames[57] = {
    "devAiSoft",
    "devAiSoftRaw",
    "s7plcAi",
    "devAvgLoad",
    "devCPULoad",
    "devMEMLoad",
    "devStatusCheck",
    "devDiskUsage",
    "devAoSoft",
    "devAoSoftRaw",
    "devAoSoftCallback",
    "s7plcAo",
    "devBiSoft",
    "devBiSoftRaw",
    "s7plcBi",
    "s7plcStat",
    "devBoSoft",
    "devBoSoftRaw",
    "devBoSoftCallback",
    "s7plcBo",
    "devCalcoutSoft",
    "devCalcoutSoftCallback",
    "s7plcCalcout",
    "devEventSoft",
    "devLiSoft",
    "s7plcLongin",
    "devLoSoft",
    "devLoSoftCallback",
    "s7plcLongout",
    "devMbbiSoft",
    "devMbbiSoftRaw",
    "s7plcMbbi",
    "devMbbiDirectSoft",
    "devMbbiDirectSoftRaw",
    "s7plcMbbiDirect",
    "devMbboSoft",
    "devMbboSoftRaw",
    "devMbboSoftCallback",
    "s7plcMbbo",
    "devMbboDirectSoft",
    "devMbboDirectSoftRaw",
    "devMbboDirectSoftCallback",
    "s7plcMbboDirect",
    "devSiSoft",
    "devTimestampSI",
    "s7plcStringin",
    "devUpTime",
    "devUname",
    "devIpAddr",
    "devRemoteCommander",
    "devSoSoft",
    "devSoSoftCallback",
    "s7plcStringout",
    "devSASoft",
    "devWfSoft",
    "s7plcWaveform",
    "devTimestampSoft"
};

static const dset * const devsl[57] = {
    pvar_dset_devAiSoft,
    pvar_dset_devAiSoftRaw,
    pvar_dset_s7plcAi,
    pvar_dset_devAvgLoad,
    pvar_dset_devCPULoad,
    pvar_dset_devMEMLoad,
    pvar_dset_devStatusCheck,
    pvar_dset_devDiskUsage,
    pvar_dset_devAoSoft,
    pvar_dset_devAoSoftRaw,
    pvar_dset_devAoSoftCallback,
    pvar_dset_s7plcAo,
    pvar_dset_devBiSoft,
    pvar_dset_devBiSoftRaw,
    pvar_dset_s7plcBi,
    pvar_dset_s7plcStat,
    pvar_dset_devBoSoft,
    pvar_dset_devBoSoftRaw,
    pvar_dset_devBoSoftCallback,
    pvar_dset_s7plcBo,
    pvar_dset_devCalcoutSoft,
    pvar_dset_devCalcoutSoftCallback,
    pvar_dset_s7plcCalcout,
    pvar_dset_devEventSoft,
    pvar_dset_devLiSoft,
    pvar_dset_s7plcLongin,
    pvar_dset_devLoSoft,
    pvar_dset_devLoSoftCallback,
    pvar_dset_s7plcLongout,
    pvar_dset_devMbbiSoft,
    pvar_dset_devMbbiSoftRaw,
    pvar_dset_s7plcMbbi,
    pvar_dset_devMbbiDirectSoft,
    pvar_dset_devMbbiDirectSoftRaw,
    pvar_dset_s7plcMbbiDirect,
    pvar_dset_devMbboSoft,
    pvar_dset_devMbboSoftRaw,
    pvar_dset_devMbboSoftCallback,
    pvar_dset_s7plcMbbo,
    pvar_dset_devMbboDirectSoft,
    pvar_dset_devMbboDirectSoftRaw,
    pvar_dset_devMbboDirectSoftCallback,
    pvar_dset_s7plcMbboDirect,
    pvar_dset_devSiSoft,
    pvar_dset_devTimestampSI,
    pvar_dset_s7plcStringin,
    pvar_dset_devUpTime,
    pvar_dset_devUname,
    pvar_dset_devIpAddr,
    pvar_dset_devRemoteCommander,
    pvar_dset_devSoSoft,
    pvar_dset_devSoSoftCallback,
    pvar_dset_s7plcStringout,
    pvar_dset_devSASoft,
    pvar_dset_devWfSoft,
    pvar_dset_s7plcWaveform,
    pvar_dset_devTimestampSoft
};

epicsShareExtern drvet *pvar_drvet_s7plc;

static const char *driverSupportNames[1] = {
    "s7plc"
};

static struct drvet *drvsl[1] = {
    pvar_drvet_s7plc
};

epicsShareExtern void (*pvar_func_asSub)(void);
epicsShareExtern void (*pvar_func_s7plcRegister)(void);
epicsShareExtern void (*pvar_func_iocshSystemCommand)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkInit)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkCalc)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateInit)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateCalc)(void);

epicsShareExtern int *pvar_int_asCaDebug;
epicsShareExtern int *pvar_int_dbRecordsOnceOnly;
epicsShareExtern int *pvar_int_s7plcDebug;
static struct iocshVarDef vardefs[] = {
	{"asCaDebug", iocshArgInt, (void * const)pvar_int_asCaDebug},
	{"dbRecordsOnceOnly", iocshArgInt, (void * const)pvar_int_dbRecordsOnceOnly},
	{"s7plcDebug", iocshArgInt, (void * const)pvar_int_s7plcDebug},
	{NULL, iocshArgInt, NULL}
};

int s7plcApp_registerRecordDeviceDriver(DBBASE *pbase)
{
    registerRecordTypes(pbase, 26, recordTypeNames, rtl);
    registerDevices(pbase, 57, deviceSupportNames, devsl);
    registerDrivers(pbase, 1, driverSupportNames, drvsl);
    (*pvar_func_asSub)();
    (*pvar_func_s7plcRegister)();
    (*pvar_func_iocshSystemCommand)();
    (*pvar_func_register_func_subIfNetworkInit)();
    (*pvar_func_register_func_subIfNetworkCalc)();
    (*pvar_func_register_func_subHealthStateInit)();
    (*pvar_func_register_func_subHealthStateCalc)();
    iocshRegisterVariable(vardefs);
    return 0;
}

/* registerRecordDeviceDriver */
static const iocshArg registerRecordDeviceDriverArg0 =
                                            {"pdbbase",iocshArgPdbbase};
static const iocshArg *registerRecordDeviceDriverArgs[1] =
                                            {&registerRecordDeviceDriverArg0};
static const iocshFuncDef registerRecordDeviceDriverFuncDef =
                {"s7plcApp_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    s7plcApp_registerRecordDeviceDriver(pdbbase);
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
