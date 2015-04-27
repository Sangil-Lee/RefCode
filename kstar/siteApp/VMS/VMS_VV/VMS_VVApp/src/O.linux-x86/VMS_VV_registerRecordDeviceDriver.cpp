/* THIS IS A GENERATED FILE. DO NOT EDIT */
/* Generated from ../O.Common/VMS_VV.dbd */

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
epicsShareExtern dset *pvar_dset_devAiSRSRGA;
epicsShareExtern dset *pvar_dset_devGpib;
epicsShareExtern dset *pvar_dset_devAiTPG262;
epicsShareExtern dset *pvar_dset_devAiGP307;
epicsShareExtern dset *pvar_dset_devAiCRP;
epicsShareExtern dset *pvar_dset_devAiTMP;
epicsShareExtern dset *pvar_dset_devAiMBP;
epicsShareExtern dset *pvar_dset_devAiEtherIP;
epicsShareExtern dset *pvar_dset_devAvgLoad;
epicsShareExtern dset *pvar_dset_devCPULoad;
epicsShareExtern dset *pvar_dset_devMEMLoad;
epicsShareExtern dset *pvar_dset_devStatusCheck;
epicsShareExtern dset *pvar_dset_devDiskUsage;
epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devAoSoftRaw;
epicsShareExtern dset *pvar_dset_devAoSoftCallback;
epicsShareExtern dset *pvar_dset_devAoSRSRGA;
epicsShareExtern dset *pvar_dset_devAoCRP;
epicsShareExtern dset *pvar_dset_devAoTMP;
epicsShareExtern dset *pvar_dset_devAoMBP;
epicsShareExtern dset *pvar_dset_devAoEtherIP;
epicsShareExtern dset *pvar_dset_devBiSoft;
epicsShareExtern dset *pvar_dset_devBiSoftRaw;
epicsShareExtern dset *pvar_dset_devBiSRSRGA;
epicsShareExtern dset *pvar_dset_devBiGP307;
epicsShareExtern dset *pvar_dset_devBiCRP;
epicsShareExtern dset *pvar_dset_devBiTMP;
epicsShareExtern dset *pvar_dset_devBiMBP;
epicsShareExtern dset *pvar_dset_devBiEtherIP;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devBoSoftRaw;
epicsShareExtern dset *pvar_dset_devBoSoftCallback;
epicsShareExtern dset *pvar_dset_devBoSRSRGA;
epicsShareExtern dset *pvar_dset_devBoGP307;
epicsShareExtern dset *pvar_dset_devBoCRP;
epicsShareExtern dset *pvar_dset_devBoTMP;
epicsShareExtern dset *pvar_dset_devBoMBP;
epicsShareExtern dset *pvar_dset_devBoEtherIP;
epicsShareExtern dset *pvar_dset_devCalcoutSoft;
epicsShareExtern dset *pvar_dset_devCalcoutSoftCallback;
epicsShareExtern dset *pvar_dset_devEventSoft;
epicsShareExtern dset *pvar_dset_devEvSRSRGA;
epicsShareExtern dset *pvar_dset_devEvCRP;
epicsShareExtern dset *pvar_dset_devEvTMP;
epicsShareExtern dset *pvar_dset_devEvMBP;
epicsShareExtern dset *pvar_dset_devLiSoft;
epicsShareExtern dset *pvar_dset_devLiSRSRGA;
epicsShareExtern dset *pvar_dset_devLiGP307;
epicsShareExtern dset *pvar_dset_devLiCRP;
epicsShareExtern dset *pvar_dset_devLiTMP;
epicsShareExtern dset *pvar_dset_devLiMBP;
epicsShareExtern dset *pvar_dset_devLoSoft;
epicsShareExtern dset *pvar_dset_devLoSoftCallback;
epicsShareExtern dset *pvar_dset_devLoSRSRGA;
epicsShareExtern dset *pvar_dset_devLoCRP;
epicsShareExtern dset *pvar_dset_devLoTMP;
epicsShareExtern dset *pvar_dset_devLoMBP;
epicsShareExtern dset *pvar_dset_devMbbiSoft;
epicsShareExtern dset *pvar_dset_devMbbiSoftRaw;
epicsShareExtern dset *pvar_dset_devMbbiSRSRGA;
epicsShareExtern dset *pvar_dset_devMbbiGP307;
epicsShareExtern dset *pvar_dset_devMbbiCRP;
epicsShareExtern dset *pvar_dset_devMbbiTMP;
epicsShareExtern dset *pvar_dset_devMbbiMBP;
epicsShareExtern dset *pvar_dset_devMbbiEtherIP;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoft;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbbidSRSRGA;
epicsShareExtern dset *pvar_dset_devMbbidCRP;
epicsShareExtern dset *pvar_dset_devMbbidTMP;
epicsShareExtern dset *pvar_dset_devMbbidMBP;
epicsShareExtern dset *pvar_dset_devMbbiDirectEtherIP;
epicsShareExtern dset *pvar_dset_devMbboSoft;
epicsShareExtern dset *pvar_dset_devMbboSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoftCallback;
epicsShareExtern dset *pvar_dset_devMbboSRSRGA;
epicsShareExtern dset *pvar_dset_devMbboCRP;
epicsShareExtern dset *pvar_dset_devMbboTMP;
epicsShareExtern dset *pvar_dset_devMbboMBP;
epicsShareExtern dset *pvar_dset_devMbboEtherIP;
epicsShareExtern dset *pvar_dset_devMbboDirectSoft;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftCallback;
epicsShareExtern dset *pvar_dset_devMbbodSRSRGA;
epicsShareExtern dset *pvar_dset_devMbbodCRP;
epicsShareExtern dset *pvar_dset_devMbbodTMP;
epicsShareExtern dset *pvar_dset_devMbbodMBP;
epicsShareExtern dset *pvar_dset_devMbboDirectEtherIP;
epicsShareExtern dset *pvar_dset_devSiSoft;
epicsShareExtern dset *pvar_dset_devTimestampSI;
epicsShareExtern dset *pvar_dset_devSiSRSRGA;
epicsShareExtern dset *pvar_dset_devSiTPG262;
epicsShareExtern dset *pvar_dset_devSiGP307;
epicsShareExtern dset *pvar_dset_devSiCRP;
epicsShareExtern dset *pvar_dset_devSiTMP;
epicsShareExtern dset *pvar_dset_devSiMBP;
epicsShareExtern dset *pvar_dset_devSiEtherIP;
epicsShareExtern dset *pvar_dset_devUpTime;
epicsShareExtern dset *pvar_dset_devUname;
epicsShareExtern dset *pvar_dset_devIpAddr;
epicsShareExtern dset *pvar_dset_devRemoteCommander;
epicsShareExtern dset *pvar_dset_devSoSoft;
epicsShareExtern dset *pvar_dset_devSoSoftCallback;
epicsShareExtern dset *pvar_dset_devSoSRSRGA;
epicsShareExtern dset *pvar_dset_devSoCRP;
epicsShareExtern dset *pvar_dset_devSoTMP;
epicsShareExtern dset *pvar_dset_devSoMBP;
epicsShareExtern dset *pvar_dset_devSASoft;
epicsShareExtern dset *pvar_dset_devWfSoft;
epicsShareExtern dset *pvar_dset_devWfSRSRGA;
epicsShareExtern dset *pvar_dset_devWfCRP;
epicsShareExtern dset *pvar_dset_devWfTMP;
epicsShareExtern dset *pvar_dset_devWfMBP;
epicsShareExtern dset *pvar_dset_devWfEtherIP;
epicsShareExtern dset *pvar_dset_devTimestampSoft;

static const char * const deviceSupportNames[117] = {
    "devAiSoft",
    "devAiSoftRaw",
    "devAiSRSRGA",
    "devGpib",
    "devAiTPG262",
    "devAiGP307",
    "devAiCRP",
    "devAiTMP",
    "devAiMBP",
    "devAiEtherIP",
    "devAvgLoad",
    "devCPULoad",
    "devMEMLoad",
    "devStatusCheck",
    "devDiskUsage",
    "devAoSoft",
    "devAoSoftRaw",
    "devAoSoftCallback",
    "devAoSRSRGA",
    "devAoCRP",
    "devAoTMP",
    "devAoMBP",
    "devAoEtherIP",
    "devBiSoft",
    "devBiSoftRaw",
    "devBiSRSRGA",
    "devBiGP307",
    "devBiCRP",
    "devBiTMP",
    "devBiMBP",
    "devBiEtherIP",
    "devBoSoft",
    "devBoSoftRaw",
    "devBoSoftCallback",
    "devBoSRSRGA",
    "devBoGP307",
    "devBoCRP",
    "devBoTMP",
    "devBoMBP",
    "devBoEtherIP",
    "devCalcoutSoft",
    "devCalcoutSoftCallback",
    "devEventSoft",
    "devEvSRSRGA",
    "devEvCRP",
    "devEvTMP",
    "devEvMBP",
    "devLiSoft",
    "devLiSRSRGA",
    "devLiGP307",
    "devLiCRP",
    "devLiTMP",
    "devLiMBP",
    "devLoSoft",
    "devLoSoftCallback",
    "devLoSRSRGA",
    "devLoCRP",
    "devLoTMP",
    "devLoMBP",
    "devMbbiSoft",
    "devMbbiSoftRaw",
    "devMbbiSRSRGA",
    "devMbbiGP307",
    "devMbbiCRP",
    "devMbbiTMP",
    "devMbbiMBP",
    "devMbbiEtherIP",
    "devMbbiDirectSoft",
    "devMbbiDirectSoftRaw",
    "devMbbidSRSRGA",
    "devMbbidCRP",
    "devMbbidTMP",
    "devMbbidMBP",
    "devMbbiDirectEtherIP",
    "devMbboSoft",
    "devMbboSoftRaw",
    "devMbboSoftCallback",
    "devMbboSRSRGA",
    "devMbboCRP",
    "devMbboTMP",
    "devMbboMBP",
    "devMbboEtherIP",
    "devMbboDirectSoft",
    "devMbboDirectSoftRaw",
    "devMbboDirectSoftCallback",
    "devMbbodSRSRGA",
    "devMbbodCRP",
    "devMbbodTMP",
    "devMbbodMBP",
    "devMbboDirectEtherIP",
    "devSiSoft",
    "devTimestampSI",
    "devSiSRSRGA",
    "devSiTPG262",
    "devSiGP307",
    "devSiCRP",
    "devSiTMP",
    "devSiMBP",
    "devSiEtherIP",
    "devUpTime",
    "devUname",
    "devIpAddr",
    "devRemoteCommander",
    "devSoSoft",
    "devSoSoftCallback",
    "devSoSRSRGA",
    "devSoCRP",
    "devSoTMP",
    "devSoMBP",
    "devSASoft",
    "devWfSoft",
    "devWfSRSRGA",
    "devWfCRP",
    "devWfTMP",
    "devWfMBP",
    "devWfEtherIP",
    "devTimestampSoft"
};

static const dset * const devsl[117] = {
    pvar_dset_devAiSoft,
    pvar_dset_devAiSoftRaw,
    pvar_dset_devAiSRSRGA,
    pvar_dset_devGpib,
    pvar_dset_devAiTPG262,
    pvar_dset_devAiGP307,
    pvar_dset_devAiCRP,
    pvar_dset_devAiTMP,
    pvar_dset_devAiMBP,
    pvar_dset_devAiEtherIP,
    pvar_dset_devAvgLoad,
    pvar_dset_devCPULoad,
    pvar_dset_devMEMLoad,
    pvar_dset_devStatusCheck,
    pvar_dset_devDiskUsage,
    pvar_dset_devAoSoft,
    pvar_dset_devAoSoftRaw,
    pvar_dset_devAoSoftCallback,
    pvar_dset_devAoSRSRGA,
    pvar_dset_devAoCRP,
    pvar_dset_devAoTMP,
    pvar_dset_devAoMBP,
    pvar_dset_devAoEtherIP,
    pvar_dset_devBiSoft,
    pvar_dset_devBiSoftRaw,
    pvar_dset_devBiSRSRGA,
    pvar_dset_devBiGP307,
    pvar_dset_devBiCRP,
    pvar_dset_devBiTMP,
    pvar_dset_devBiMBP,
    pvar_dset_devBiEtherIP,
    pvar_dset_devBoSoft,
    pvar_dset_devBoSoftRaw,
    pvar_dset_devBoSoftCallback,
    pvar_dset_devBoSRSRGA,
    pvar_dset_devBoGP307,
    pvar_dset_devBoCRP,
    pvar_dset_devBoTMP,
    pvar_dset_devBoMBP,
    pvar_dset_devBoEtherIP,
    pvar_dset_devCalcoutSoft,
    pvar_dset_devCalcoutSoftCallback,
    pvar_dset_devEventSoft,
    pvar_dset_devEvSRSRGA,
    pvar_dset_devEvCRP,
    pvar_dset_devEvTMP,
    pvar_dset_devEvMBP,
    pvar_dset_devLiSoft,
    pvar_dset_devLiSRSRGA,
    pvar_dset_devLiGP307,
    pvar_dset_devLiCRP,
    pvar_dset_devLiTMP,
    pvar_dset_devLiMBP,
    pvar_dset_devLoSoft,
    pvar_dset_devLoSoftCallback,
    pvar_dset_devLoSRSRGA,
    pvar_dset_devLoCRP,
    pvar_dset_devLoTMP,
    pvar_dset_devLoMBP,
    pvar_dset_devMbbiSoft,
    pvar_dset_devMbbiSoftRaw,
    pvar_dset_devMbbiSRSRGA,
    pvar_dset_devMbbiGP307,
    pvar_dset_devMbbiCRP,
    pvar_dset_devMbbiTMP,
    pvar_dset_devMbbiMBP,
    pvar_dset_devMbbiEtherIP,
    pvar_dset_devMbbiDirectSoft,
    pvar_dset_devMbbiDirectSoftRaw,
    pvar_dset_devMbbidSRSRGA,
    pvar_dset_devMbbidCRP,
    pvar_dset_devMbbidTMP,
    pvar_dset_devMbbidMBP,
    pvar_dset_devMbbiDirectEtherIP,
    pvar_dset_devMbboSoft,
    pvar_dset_devMbboSoftRaw,
    pvar_dset_devMbboSoftCallback,
    pvar_dset_devMbboSRSRGA,
    pvar_dset_devMbboCRP,
    pvar_dset_devMbboTMP,
    pvar_dset_devMbboMBP,
    pvar_dset_devMbboEtherIP,
    pvar_dset_devMbboDirectSoft,
    pvar_dset_devMbboDirectSoftRaw,
    pvar_dset_devMbboDirectSoftCallback,
    pvar_dset_devMbbodSRSRGA,
    pvar_dset_devMbbodCRP,
    pvar_dset_devMbbodTMP,
    pvar_dset_devMbbodMBP,
    pvar_dset_devMbboDirectEtherIP,
    pvar_dset_devSiSoft,
    pvar_dset_devTimestampSI,
    pvar_dset_devSiSRSRGA,
    pvar_dset_devSiTPG262,
    pvar_dset_devSiGP307,
    pvar_dset_devSiCRP,
    pvar_dset_devSiTMP,
    pvar_dset_devSiMBP,
    pvar_dset_devSiEtherIP,
    pvar_dset_devUpTime,
    pvar_dset_devUname,
    pvar_dset_devIpAddr,
    pvar_dset_devRemoteCommander,
    pvar_dset_devSoSoft,
    pvar_dset_devSoSoftCallback,
    pvar_dset_devSoSRSRGA,
    pvar_dset_devSoCRP,
    pvar_dset_devSoTMP,
    pvar_dset_devSoMBP,
    pvar_dset_devSASoft,
    pvar_dset_devWfSoft,
    pvar_dset_devWfSRSRGA,
    pvar_dset_devWfCRP,
    pvar_dset_devWfTMP,
    pvar_dset_devWfMBP,
    pvar_dset_devWfEtherIP,
    pvar_dset_devTimestampSoft
};

epicsShareExtern drvet *pvar_drvet_drvTPG262;
epicsShareExtern drvet *pvar_drvet_drvGP307;
epicsShareExtern drvet *pvar_drvet_drvEtherIP;

static const char *driverSupportNames[3] = {
    "drvTPG262",
    "drvGP307",
    "drvEtherIP"
};

static struct drvet *drvsl[3] = {
    pvar_drvet_drvTPG262,
    pvar_drvet_drvGP307,
    pvar_drvet_drvEtherIP
};

epicsShareExtern void (*pvar_func_asSub)(void);
epicsShareExtern void (*pvar_func_asyn)(void);
epicsShareExtern void (*pvar_func_drvGenericSerialRegisterCommands)(void);
epicsShareExtern void (*pvar_func_drvTPG262Registrar)(void);
epicsShareExtern void (*pvar_func_drvGP307Registrar)(void);
epicsShareExtern void (*pvar_func_drvEtherIP_Register)(void);
epicsShareExtern void (*pvar_func_iocshSystemCommand)(void);
epicsShareExtern void (*pvar_func_SRSRGA_ONRegistrar)(void);
epicsShareExtern void (*pvar_func_SRSRGA2_ONRegistrar)(void);
epicsShareExtern void (*pvar_func_CRPGV_ONRegistrar)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkInit)(void);
epicsShareExtern void (*pvar_func_register_func_subIfNetworkCalc)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateInit)(void);
epicsShareExtern void (*pvar_func_register_func_subHealthStateCalc)(void);

epicsShareExtern int *pvar_int_asCaDebug;
epicsShareExtern int *pvar_int_dbRecordsOnceOnly;
epicsShareExtern int *pvar_int_drvGenericSerialDebug;
epicsShareExtern int *pvar_int_devPumpCRPDebug;
epicsShareExtern int *pvar_int_devPumpTMPDebug;
epicsShareExtern int *pvar_int_devPumpMBPDebug;
static struct iocshVarDef vardefs[] = {
	{"asCaDebug", iocshArgInt, (void * const)pvar_int_asCaDebug},
	{"dbRecordsOnceOnly", iocshArgInt, (void * const)pvar_int_dbRecordsOnceOnly},
	{"drvGenericSerialDebug", iocshArgInt, (void * const)pvar_int_drvGenericSerialDebug},
	{"devPumpCRPDebug", iocshArgInt, (void * const)pvar_int_devPumpCRPDebug},
	{"devPumpTMPDebug", iocshArgInt, (void * const)pvar_int_devPumpTMPDebug},
	{"devPumpMBPDebug", iocshArgInt, (void * const)pvar_int_devPumpMBPDebug},
	{NULL, iocshArgInt, NULL}
};

int VMS_VV_registerRecordDeviceDriver(DBBASE *pbase)
{
    registerRecordTypes(pbase, 26, recordTypeNames, rtl);
    registerDevices(pbase, 117, deviceSupportNames, devsl);
    registerDrivers(pbase, 3, driverSupportNames, drvsl);
    (*pvar_func_asSub)();
    (*pvar_func_asyn)();
    (*pvar_func_drvGenericSerialRegisterCommands)();
    (*pvar_func_drvTPG262Registrar)();
    (*pvar_func_drvGP307Registrar)();
    (*pvar_func_drvEtherIP_Register)();
    (*pvar_func_iocshSystemCommand)();
    (*pvar_func_SRSRGA_ONRegistrar)();
    (*pvar_func_SRSRGA2_ONRegistrar)();
    (*pvar_func_CRPGV_ONRegistrar)();
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
                {"VMS_VV_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    VMS_VV_registerRecordDeviceDriver(pdbbase);
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
