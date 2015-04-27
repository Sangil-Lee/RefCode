/* THIS IS A GENERATED FILE. DO NOT EDIT! */
/* Generated from ../O.Common/SXR.dbd */

#include "registryCommon.h"
#include "iocsh.h"
#include "iocshRegisterCommon.h"

extern "C" {
epicsShareExtern rset *pvar_rset_aiRSET;
epicsShareExtern int (*pvar_func_aiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aoRSET;
epicsShareExtern int (*pvar_func_aoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aSubRSET;
epicsShareExtern int (*pvar_func_aSubRecordSizeOffset)(dbRecordType *pdbRecordType);
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

static const char * const recordTypeNames[27] = {
    "ai",
    "ao",
    "aSub",
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

static const recordTypeLocation rtl[27] = {
    {pvar_rset_aiRSET, pvar_func_aiRecordSizeOffset},
    {pvar_rset_aoRSET, pvar_func_aoRecordSizeOffset},
    {pvar_rset_aSubRSET, pvar_func_aSubRecordSizeOffset},
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
epicsShareExtern dset *pvar_dset_devTimestampAI;
epicsShareExtern dset *pvar_dset_devAiGeneralTime;
epicsShareExtern dset *pvar_dset_devAiACQ196RawRead;
epicsShareExtern dset *pvar_dset_devAiNI6123RawRead_ai;
epicsShareExtern dset *pvar_dset_devAiIT6322RawRead;
epicsShareExtern dset *pvar_dset_devAiNI6259RawRead_ai;
epicsShareExtern dset *pvar_dset_devAiAdmin;
epicsShareExtern dset *pvar_dset_devAvgLoad;
epicsShareExtern dset *pvar_dset_devCPULoad;
epicsShareExtern dset *pvar_dset_devMEMLoad;
epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devAoSoftRaw;
epicsShareExtern dset *pvar_dset_devAoSoftCallback;
epicsShareExtern dset *pvar_dset_devAoACQ196Control;
epicsShareExtern dset *pvar_dset_devAoNI6123Control;
epicsShareExtern dset *pvar_dset_devAoIT6322Control;
epicsShareExtern dset *pvar_dset_devAoAdmin;
epicsShareExtern dset *pvar_dset_devBiSoft;
epicsShareExtern dset *pvar_dset_devBiSoftRaw;
epicsShareExtern dset *pvar_dset_devBiAdmin;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devBoSoftRaw;
epicsShareExtern dset *pvar_dset_devBoSoftCallback;
epicsShareExtern dset *pvar_dset_devBoGeneralTime;
epicsShareExtern dset *pvar_dset_devBoACQ196Control;
epicsShareExtern dset *pvar_dset_devBoAdmin;
epicsShareExtern dset *pvar_dset_devCalcoutSoft;
epicsShareExtern dset *pvar_dset_devCalcoutSoftCallback;
epicsShareExtern dset *pvar_dset_devEventSoft;
epicsShareExtern dset *pvar_dset_devLiSoft;
epicsShareExtern dset *pvar_dset_devLiGeneralTime;
epicsShareExtern dset *pvar_dset_devLoSoft;
epicsShareExtern dset *pvar_dset_devLoSoftCallback;
epicsShareExtern dset *pvar_dset_devMbbiSoft;
epicsShareExtern dset *pvar_dset_devMbbiSoftRaw;
epicsShareExtern dset *pvar_dset_devMbbiAdmin;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoft;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoft;
epicsShareExtern dset *pvar_dset_devMbboSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoftCallback;
epicsShareExtern dset *pvar_dset_devMbboDirectSoft;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftCallback;
epicsShareExtern dset *pvar_dset_devSiSoft;
epicsShareExtern dset *pvar_dset_devTimestampSI;
epicsShareExtern dset *pvar_dset_devSiGeneralTime;
epicsShareExtern dset *pvar_dset_devStringinAdmin;
epicsShareExtern dset *pvar_dset_devUpTime;
epicsShareExtern dset *pvar_dset_devUname;
epicsShareExtern dset *pvar_dset_devIpAddr;
epicsShareExtern dset *pvar_dset_devSoSoft;
epicsShareExtern dset *pvar_dset_devSoSoftCallback;
epicsShareExtern dset *pvar_dset_devSoStdio;
epicsShareExtern dset *pvar_dset_devStringoutACQ196Control;
epicsShareExtern dset *pvar_dset_devStringoutAdmin;
epicsShareExtern dset *pvar_dset_devSASoft;
epicsShareExtern dset *pvar_dset_devWfSoft;
epicsShareExtern dset *pvar_dset_devTimestampSoft;

static const char * const deviceSupportNames[61] = {
    "devAiSoft",
    "devAiSoftRaw",
    "devTimestampAI",
    "devAiGeneralTime",
    "devAiACQ196RawRead",
    "devAiNI6123RawRead_ai",
    "devAiIT6322RawRead",
    "devAiNI6259RawRead_ai",
    "devAiAdmin",
    "devAvgLoad",
    "devCPULoad",
    "devMEMLoad",
    "devAoSoft",
    "devAoSoftRaw",
    "devAoSoftCallback",
    "devAoACQ196Control",
    "devAoNI6123Control",
    "devAoIT6322Control",
    "devAoAdmin",
    "devBiSoft",
    "devBiSoftRaw",
    "devBiAdmin",
    "devBoSoft",
    "devBoSoftRaw",
    "devBoSoftCallback",
    "devBoGeneralTime",
    "devBoACQ196Control",
    "devBoAdmin",
    "devCalcoutSoft",
    "devCalcoutSoftCallback",
    "devEventSoft",
    "devLiSoft",
    "devLiGeneralTime",
    "devLoSoft",
    "devLoSoftCallback",
    "devMbbiSoft",
    "devMbbiSoftRaw",
    "devMbbiAdmin",
    "devMbbiDirectSoft",
    "devMbbiDirectSoftRaw",
    "devMbboSoft",
    "devMbboSoftRaw",
    "devMbboSoftCallback",
    "devMbboDirectSoft",
    "devMbboDirectSoftRaw",
    "devMbboDirectSoftCallback",
    "devSiSoft",
    "devTimestampSI",
    "devSiGeneralTime",
    "devStringinAdmin",
    "devUpTime",
    "devUname",
    "devIpAddr",
    "devSoSoft",
    "devSoSoftCallback",
    "devSoStdio",
    "devStringoutACQ196Control",
    "devStringoutAdmin",
    "devSASoft",
    "devWfSoft",
    "devTimestampSoft"
};

static const dset * const devsl[61] = {
    pvar_dset_devAiSoft,
    pvar_dset_devAiSoftRaw,
    pvar_dset_devTimestampAI,
    pvar_dset_devAiGeneralTime,
    pvar_dset_devAiACQ196RawRead,
    pvar_dset_devAiNI6123RawRead_ai,
    pvar_dset_devAiIT6322RawRead,
    pvar_dset_devAiNI6259RawRead_ai,
    pvar_dset_devAiAdmin,
    pvar_dset_devAvgLoad,
    pvar_dset_devCPULoad,
    pvar_dset_devMEMLoad,
    pvar_dset_devAoSoft,
    pvar_dset_devAoSoftRaw,
    pvar_dset_devAoSoftCallback,
    pvar_dset_devAoACQ196Control,
    pvar_dset_devAoNI6123Control,
    pvar_dset_devAoIT6322Control,
    pvar_dset_devAoAdmin,
    pvar_dset_devBiSoft,
    pvar_dset_devBiSoftRaw,
    pvar_dset_devBiAdmin,
    pvar_dset_devBoSoft,
    pvar_dset_devBoSoftRaw,
    pvar_dset_devBoSoftCallback,
    pvar_dset_devBoGeneralTime,
    pvar_dset_devBoACQ196Control,
    pvar_dset_devBoAdmin,
    pvar_dset_devCalcoutSoft,
    pvar_dset_devCalcoutSoftCallback,
    pvar_dset_devEventSoft,
    pvar_dset_devLiSoft,
    pvar_dset_devLiGeneralTime,
    pvar_dset_devLoSoft,
    pvar_dset_devLoSoftCallback,
    pvar_dset_devMbbiSoft,
    pvar_dset_devMbbiSoftRaw,
    pvar_dset_devMbbiAdmin,
    pvar_dset_devMbbiDirectSoft,
    pvar_dset_devMbbiDirectSoftRaw,
    pvar_dset_devMbboSoft,
    pvar_dset_devMbboSoftRaw,
    pvar_dset_devMbboSoftCallback,
    pvar_dset_devMbboDirectSoft,
    pvar_dset_devMbboDirectSoftRaw,
    pvar_dset_devMbboDirectSoftCallback,
    pvar_dset_devSiSoft,
    pvar_dset_devTimestampSI,
    pvar_dset_devSiGeneralTime,
    pvar_dset_devStringinAdmin,
    pvar_dset_devUpTime,
    pvar_dset_devUname,
    pvar_dset_devIpAddr,
    pvar_dset_devSoSoft,
    pvar_dset_devSoSoftCallback,
    pvar_dset_devSoStdio,
    pvar_dset_devStringoutACQ196Control,
    pvar_dset_devStringoutAdmin,
    pvar_dset_devSASoft,
    pvar_dset_devWfSoft,
    pvar_dset_devTimestampSoft
};

epicsShareExtern drvet *pvar_drvet_drvNI6123;
epicsShareExtern drvet *pvar_drvet_drvACQ196;
epicsShareExtern drvet *pvar_drvet_drvNI6259;
epicsShareExtern drvet *pvar_drvet_drvIT6322;
epicsShareExtern drvet *pvar_drvet_drvAdmin;

static const char *driverSupportNames[5] = {
    "drvNI6123",
    "drvACQ196",
    "drvNI6259",
    "drvIT6322",
    "drvAdmin"
};

static struct drvet *drvsl[5] = {
    pvar_drvet_drvNI6123,
    pvar_drvet_drvACQ196,
    pvar_drvet_drvNI6259,
    pvar_drvet_drvIT6322,
    pvar_drvet_drvAdmin
};

epicsShareExtern void (*pvar_func_asSub)(void);
epicsShareExtern void (*pvar_func_initTraceRegister)(void);
epicsShareExtern void (*pvar_func_AdminTaskRegister)(void);
epicsShareExtern void (*pvar_func_iocshSystemCommand)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkInit)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkCalc)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateInit)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateCalc)(void);

epicsShareExtern int *pvar_int_asCaDebug;
epicsShareExtern int *pvar_int_dbRecordsOnceOnly;
epicsShareExtern int *pvar_int_dbBptNotMonotonic;
static struct iocshVarDef vardefs[] = {
	{"asCaDebug", iocshArgInt, (void * const)pvar_int_asCaDebug},
	{"dbRecordsOnceOnly", iocshArgInt, (void * const)pvar_int_dbRecordsOnceOnly},
	{"dbBptNotMonotonic", iocshArgInt, (void * const)pvar_int_dbBptNotMonotonic},
	{NULL, iocshArgInt, NULL}
};

int SXR_registerRecordDeviceDriver(DBBASE *pbase)
{
    if (!pbase) {
        printf("pdbbase is NULL; you must load a DBD file first.\n");
        return -1;
    }
    registerRecordTypes(pbase, 27, recordTypeNames, rtl);
    registerDevices(pbase, 61, deviceSupportNames, devsl);
    registerDrivers(pbase, 5, driverSupportNames, drvsl);
    (*pvar_func_asSub)();
    (*pvar_func_initTraceRegister)();
    (*pvar_func_AdminTaskRegister)();
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
                {"SXR_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    SXR_registerRecordDeviceDriver(*iocshPpdbbase);
}

} // extern "C"
/*
 * Register commands on application startup
 */
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
