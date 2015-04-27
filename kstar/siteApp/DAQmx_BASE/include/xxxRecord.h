/* xxxRecord.h generated from xxxRecord.dbd */

#ifndef INC_xxxRecord_H
#define INC_xxxRecord_H

#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"

typedef struct xxxRecord {
    char                name[61];   /* Record Name */
    char                desc[41];   /* Descriptor */
    char                asg[29];    /* Access Security Group */
    epicsEnum16         scan;       /* Scan Mechanism */
    epicsEnum16         pini;       /* Process at iocInit */
    epicsInt16          phas;       /* Scan Phase */
    char                evnt[40];   /* Event Name */
    epicsInt16          tse;        /* Time Stamp Event */
    DBLINK              tsel;       /* Time Stamp Link */
    epicsEnum16         dtyp;       /* Device Type */
    epicsInt16          disv;       /* Disable Value */
    epicsInt16          disa;       /* Disable */
    DBLINK              sdis;       /* Scanning Disable */
    epicsMutexId        mlok;       /* Monitor lock */
    ELLLIST             mlis;       /* Monitor List */
    epicsUInt8          disp;       /* Disable putField */
    epicsUInt8          proc;       /* Force Processing */
    epicsEnum16         stat;       /* Alarm Status */
    epicsEnum16         sevr;       /* Alarm Severity */
    epicsEnum16         nsta;       /* New Alarm Status */
    epicsEnum16         nsev;       /* New Alarm Severity */
    epicsEnum16         acks;       /* Alarm Ack Severity */
    epicsEnum16         ackt;       /* Alarm Ack Transient */
    epicsEnum16         diss;       /* Disable Alarm Sevrty */
    epicsUInt8          lcnt;       /* Lock Count */
    epicsUInt8          pact;       /* Record active */
    epicsUInt8          putf;       /* dbPutField process */
    epicsUInt8          rpro;       /* Reprocess  */
    struct asgMember    *asp;       /* Access Security Pvt */
    struct processNotify *ppn;      /* pprocessNotify */
    struct processNotifyRecord *ppnr; /* pprocessNotifyRecord */
    struct scan_element *spvt;      /* Scan Private */
    struct rset         *rset;      /* Address of RSET */
    struct dset         *dset;      /* DSET address */
    void                *dpvt;      /* Device Private */
    struct dbRecordType *rdes;      /* Address of dbRecordType */
    struct lockRecord   *lset;      /* Lock Set */
    epicsEnum16         prio;       /* Scheduling Priority */
    epicsUInt8          tpro;       /* Trace Processing */
    char                bkpt;       /* Break Point */
    epicsUInt8          udf;        /* Undefined */
    epicsTimeStamp      time;       /* Time */
    DBLINK              flnk;       /* Forward Process Link */
    epicsFloat64        val;        /* Current EGU Value */
    DBLINK              inp;        /* Input Specification */
    epicsInt16          prec;       /* Display Precision */
    char                egu[16];    /* Engineering Units */
    epicsFloat32        hopr;       /* High Operating Range */
    epicsFloat32        lopr;       /* Low Operating Range */
    epicsFloat32        hihi;       /* Hihi Alarm Limit */
    epicsFloat32        lolo;       /* Lolo Alarm Limit */
    epicsFloat32        high;       /* High Alarm Limit */
    epicsFloat32        low;        /* Low Alarm Limit */
    epicsEnum16         hhsv;       /* Hihi Severity */
    epicsEnum16         llsv;       /* Lolo Severity */
    epicsEnum16         hsv;        /* High Severity */
    epicsEnum16         lsv;        /* Low Severity */
    epicsFloat64        hyst;       /* Alarm Deadband */
    epicsFloat64        adel;       /* Archive Deadband */
    epicsFloat64        mdel;       /* Monitor Deadband */
    epicsFloat64        lalm;       /* Last Value Alarmed */
    epicsFloat64        alst;       /* Last Value Archived */
    epicsFloat64        mlst;       /* Last Val Monitored */
} xxxRecord;

typedef enum {
	xxxRecordNAME = 0,
	xxxRecordDESC = 1,
	xxxRecordASG = 2,
	xxxRecordSCAN = 3,
	xxxRecordPINI = 4,
	xxxRecordPHAS = 5,
	xxxRecordEVNT = 6,
	xxxRecordTSE = 7,
	xxxRecordTSEL = 8,
	xxxRecordDTYP = 9,
	xxxRecordDISV = 10,
	xxxRecordDISA = 11,
	xxxRecordSDIS = 12,
	xxxRecordMLOK = 13,
	xxxRecordMLIS = 14,
	xxxRecordDISP = 15,
	xxxRecordPROC = 16,
	xxxRecordSTAT = 17,
	xxxRecordSEVR = 18,
	xxxRecordNSTA = 19,
	xxxRecordNSEV = 20,
	xxxRecordACKS = 21,
	xxxRecordACKT = 22,
	xxxRecordDISS = 23,
	xxxRecordLCNT = 24,
	xxxRecordPACT = 25,
	xxxRecordPUTF = 26,
	xxxRecordRPRO = 27,
	xxxRecordASP = 28,
	xxxRecordPPN = 29,
	xxxRecordPPNR = 30,
	xxxRecordSPVT = 31,
	xxxRecordRSET = 32,
	xxxRecordDSET = 33,
	xxxRecordDPVT = 34,
	xxxRecordRDES = 35,
	xxxRecordLSET = 36,
	xxxRecordPRIO = 37,
	xxxRecordTPRO = 38,
	xxxRecordBKPT = 39,
	xxxRecordUDF = 40,
	xxxRecordTIME = 41,
	xxxRecordFLNK = 42,
	xxxRecordVAL = 43,
	xxxRecordINP = 44,
	xxxRecordPREC = 45,
	xxxRecordEGU = 46,
	xxxRecordHOPR = 47,
	xxxRecordLOPR = 48,
	xxxRecordHIHI = 49,
	xxxRecordLOLO = 50,
	xxxRecordHIGH = 51,
	xxxRecordLOW = 52,
	xxxRecordHHSV = 53,
	xxxRecordLLSV = 54,
	xxxRecordHSV = 55,
	xxxRecordLSV = 56,
	xxxRecordHYST = 57,
	xxxRecordADEL = 58,
	xxxRecordMDEL = 59,
	xxxRecordLALM = 60,
	xxxRecordALST = 61,
	xxxRecordMLST = 62
} xxxFieldIndex;

#ifdef GEN_SIZE_OFFSET

#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int xxxRecordSizeOffset(dbRecordType *prt)
{
    xxxRecord *prec = 0;
    prt->papFldDes[xxxRecordNAME]->size = sizeof(prec->name);
    prt->papFldDes[xxxRecordDESC]->size = sizeof(prec->desc);
    prt->papFldDes[xxxRecordASG]->size = sizeof(prec->asg);
    prt->papFldDes[xxxRecordSCAN]->size = sizeof(prec->scan);
    prt->papFldDes[xxxRecordPINI]->size = sizeof(prec->pini);
    prt->papFldDes[xxxRecordPHAS]->size = sizeof(prec->phas);
    prt->papFldDes[xxxRecordEVNT]->size = sizeof(prec->evnt);
    prt->papFldDes[xxxRecordTSE]->size = sizeof(prec->tse);
    prt->papFldDes[xxxRecordTSEL]->size = sizeof(prec->tsel);
    prt->papFldDes[xxxRecordDTYP]->size = sizeof(prec->dtyp);
    prt->papFldDes[xxxRecordDISV]->size = sizeof(prec->disv);
    prt->papFldDes[xxxRecordDISA]->size = sizeof(prec->disa);
    prt->papFldDes[xxxRecordSDIS]->size = sizeof(prec->sdis);
    prt->papFldDes[xxxRecordMLOK]->size = sizeof(prec->mlok);
    prt->papFldDes[xxxRecordMLIS]->size = sizeof(prec->mlis);
    prt->papFldDes[xxxRecordDISP]->size = sizeof(prec->disp);
    prt->papFldDes[xxxRecordPROC]->size = sizeof(prec->proc);
    prt->papFldDes[xxxRecordSTAT]->size = sizeof(prec->stat);
    prt->papFldDes[xxxRecordSEVR]->size = sizeof(prec->sevr);
    prt->papFldDes[xxxRecordNSTA]->size = sizeof(prec->nsta);
    prt->papFldDes[xxxRecordNSEV]->size = sizeof(prec->nsev);
    prt->papFldDes[xxxRecordACKS]->size = sizeof(prec->acks);
    prt->papFldDes[xxxRecordACKT]->size = sizeof(prec->ackt);
    prt->papFldDes[xxxRecordDISS]->size = sizeof(prec->diss);
    prt->papFldDes[xxxRecordLCNT]->size = sizeof(prec->lcnt);
    prt->papFldDes[xxxRecordPACT]->size = sizeof(prec->pact);
    prt->papFldDes[xxxRecordPUTF]->size = sizeof(prec->putf);
    prt->papFldDes[xxxRecordRPRO]->size = sizeof(prec->rpro);
    prt->papFldDes[xxxRecordASP]->size = sizeof(prec->asp);
    prt->papFldDes[xxxRecordPPN]->size = sizeof(prec->ppn);
    prt->papFldDes[xxxRecordPPNR]->size = sizeof(prec->ppnr);
    prt->papFldDes[xxxRecordSPVT]->size = sizeof(prec->spvt);
    prt->papFldDes[xxxRecordRSET]->size = sizeof(prec->rset);
    prt->papFldDes[xxxRecordDSET]->size = sizeof(prec->dset);
    prt->papFldDes[xxxRecordDPVT]->size = sizeof(prec->dpvt);
    prt->papFldDes[xxxRecordRDES]->size = sizeof(prec->rdes);
    prt->papFldDes[xxxRecordLSET]->size = sizeof(prec->lset);
    prt->papFldDes[xxxRecordPRIO]->size = sizeof(prec->prio);
    prt->papFldDes[xxxRecordTPRO]->size = sizeof(prec->tpro);
    prt->papFldDes[xxxRecordBKPT]->size = sizeof(prec->bkpt);
    prt->papFldDes[xxxRecordUDF]->size = sizeof(prec->udf);
    prt->papFldDes[xxxRecordTIME]->size = sizeof(prec->time);
    prt->papFldDes[xxxRecordFLNK]->size = sizeof(prec->flnk);
    prt->papFldDes[xxxRecordVAL]->size = sizeof(prec->val);
    prt->papFldDes[xxxRecordINP]->size = sizeof(prec->inp);
    prt->papFldDes[xxxRecordPREC]->size = sizeof(prec->prec);
    prt->papFldDes[xxxRecordEGU]->size = sizeof(prec->egu);
    prt->papFldDes[xxxRecordHOPR]->size = sizeof(prec->hopr);
    prt->papFldDes[xxxRecordLOPR]->size = sizeof(prec->lopr);
    prt->papFldDes[xxxRecordHIHI]->size = sizeof(prec->hihi);
    prt->papFldDes[xxxRecordLOLO]->size = sizeof(prec->lolo);
    prt->papFldDes[xxxRecordHIGH]->size = sizeof(prec->high);
    prt->papFldDes[xxxRecordLOW]->size = sizeof(prec->low);
    prt->papFldDes[xxxRecordHHSV]->size = sizeof(prec->hhsv);
    prt->papFldDes[xxxRecordLLSV]->size = sizeof(prec->llsv);
    prt->papFldDes[xxxRecordHSV]->size = sizeof(prec->hsv);
    prt->papFldDes[xxxRecordLSV]->size = sizeof(prec->lsv);
    prt->papFldDes[xxxRecordHYST]->size = sizeof(prec->hyst);
    prt->papFldDes[xxxRecordADEL]->size = sizeof(prec->adel);
    prt->papFldDes[xxxRecordMDEL]->size = sizeof(prec->mdel);
    prt->papFldDes[xxxRecordLALM]->size = sizeof(prec->lalm);
    prt->papFldDes[xxxRecordALST]->size = sizeof(prec->alst);
    prt->papFldDes[xxxRecordMLST]->size = sizeof(prec->mlst);
    prt->papFldDes[xxxRecordNAME]->offset = (char *)&prec->name - (char *)prec;
    prt->papFldDes[xxxRecordDESC]->offset = (char *)&prec->desc - (char *)prec;
    prt->papFldDes[xxxRecordASG]->offset = (char *)&prec->asg - (char *)prec;
    prt->papFldDes[xxxRecordSCAN]->offset = (char *)&prec->scan - (char *)prec;
    prt->papFldDes[xxxRecordPINI]->offset = (char *)&prec->pini - (char *)prec;
    prt->papFldDes[xxxRecordPHAS]->offset = (char *)&prec->phas - (char *)prec;
    prt->papFldDes[xxxRecordEVNT]->offset = (char *)&prec->evnt - (char *)prec;
    prt->papFldDes[xxxRecordTSE]->offset = (char *)&prec->tse - (char *)prec;
    prt->papFldDes[xxxRecordTSEL]->offset = (char *)&prec->tsel - (char *)prec;
    prt->papFldDes[xxxRecordDTYP]->offset = (char *)&prec->dtyp - (char *)prec;
    prt->papFldDes[xxxRecordDISV]->offset = (char *)&prec->disv - (char *)prec;
    prt->papFldDes[xxxRecordDISA]->offset = (char *)&prec->disa - (char *)prec;
    prt->papFldDes[xxxRecordSDIS]->offset = (char *)&prec->sdis - (char *)prec;
    prt->papFldDes[xxxRecordMLOK]->offset = (char *)&prec->mlok - (char *)prec;
    prt->papFldDes[xxxRecordMLIS]->offset = (char *)&prec->mlis - (char *)prec;
    prt->papFldDes[xxxRecordDISP]->offset = (char *)&prec->disp - (char *)prec;
    prt->papFldDes[xxxRecordPROC]->offset = (char *)&prec->proc - (char *)prec;
    prt->papFldDes[xxxRecordSTAT]->offset = (char *)&prec->stat - (char *)prec;
    prt->papFldDes[xxxRecordSEVR]->offset = (char *)&prec->sevr - (char *)prec;
    prt->papFldDes[xxxRecordNSTA]->offset = (char *)&prec->nsta - (char *)prec;
    prt->papFldDes[xxxRecordNSEV]->offset = (char *)&prec->nsev - (char *)prec;
    prt->papFldDes[xxxRecordACKS]->offset = (char *)&prec->acks - (char *)prec;
    prt->papFldDes[xxxRecordACKT]->offset = (char *)&prec->ackt - (char *)prec;
    prt->papFldDes[xxxRecordDISS]->offset = (char *)&prec->diss - (char *)prec;
    prt->papFldDes[xxxRecordLCNT]->offset = (char *)&prec->lcnt - (char *)prec;
    prt->papFldDes[xxxRecordPACT]->offset = (char *)&prec->pact - (char *)prec;
    prt->papFldDes[xxxRecordPUTF]->offset = (char *)&prec->putf - (char *)prec;
    prt->papFldDes[xxxRecordRPRO]->offset = (char *)&prec->rpro - (char *)prec;
    prt->papFldDes[xxxRecordASP]->offset = (char *)&prec->asp - (char *)prec;
    prt->papFldDes[xxxRecordPPN]->offset = (char *)&prec->ppn - (char *)prec;
    prt->papFldDes[xxxRecordPPNR]->offset = (char *)&prec->ppnr - (char *)prec;
    prt->papFldDes[xxxRecordSPVT]->offset = (char *)&prec->spvt - (char *)prec;
    prt->papFldDes[xxxRecordRSET]->offset = (char *)&prec->rset - (char *)prec;
    prt->papFldDes[xxxRecordDSET]->offset = (char *)&prec->dset - (char *)prec;
    prt->papFldDes[xxxRecordDPVT]->offset = (char *)&prec->dpvt - (char *)prec;
    prt->papFldDes[xxxRecordRDES]->offset = (char *)&prec->rdes - (char *)prec;
    prt->papFldDes[xxxRecordLSET]->offset = (char *)&prec->lset - (char *)prec;
    prt->papFldDes[xxxRecordPRIO]->offset = (char *)&prec->prio - (char *)prec;
    prt->papFldDes[xxxRecordTPRO]->offset = (char *)&prec->tpro - (char *)prec;
    prt->papFldDes[xxxRecordBKPT]->offset = (char *)&prec->bkpt - (char *)prec;
    prt->papFldDes[xxxRecordUDF]->offset = (char *)&prec->udf - (char *)prec;
    prt->papFldDes[xxxRecordTIME]->offset = (char *)&prec->time - (char *)prec;
    prt->papFldDes[xxxRecordFLNK]->offset = (char *)&prec->flnk - (char *)prec;
    prt->papFldDes[xxxRecordVAL]->offset = (char *)&prec->val - (char *)prec;
    prt->papFldDes[xxxRecordINP]->offset = (char *)&prec->inp - (char *)prec;
    prt->papFldDes[xxxRecordPREC]->offset = (char *)&prec->prec - (char *)prec;
    prt->papFldDes[xxxRecordEGU]->offset = (char *)&prec->egu - (char *)prec;
    prt->papFldDes[xxxRecordHOPR]->offset = (char *)&prec->hopr - (char *)prec;
    prt->papFldDes[xxxRecordLOPR]->offset = (char *)&prec->lopr - (char *)prec;
    prt->papFldDes[xxxRecordHIHI]->offset = (char *)&prec->hihi - (char *)prec;
    prt->papFldDes[xxxRecordLOLO]->offset = (char *)&prec->lolo - (char *)prec;
    prt->papFldDes[xxxRecordHIGH]->offset = (char *)&prec->high - (char *)prec;
    prt->papFldDes[xxxRecordLOW]->offset = (char *)&prec->low - (char *)prec;
    prt->papFldDes[xxxRecordHHSV]->offset = (char *)&prec->hhsv - (char *)prec;
    prt->papFldDes[xxxRecordLLSV]->offset = (char *)&prec->llsv - (char *)prec;
    prt->papFldDes[xxxRecordHSV]->offset = (char *)&prec->hsv - (char *)prec;
    prt->papFldDes[xxxRecordLSV]->offset = (char *)&prec->lsv - (char *)prec;
    prt->papFldDes[xxxRecordHYST]->offset = (char *)&prec->hyst - (char *)prec;
    prt->papFldDes[xxxRecordADEL]->offset = (char *)&prec->adel - (char *)prec;
    prt->papFldDes[xxxRecordMDEL]->offset = (char *)&prec->mdel - (char *)prec;
    prt->papFldDes[xxxRecordLALM]->offset = (char *)&prec->lalm - (char *)prec;
    prt->papFldDes[xxxRecordALST]->offset = (char *)&prec->alst - (char *)prec;
    prt->papFldDes[xxxRecordMLST]->offset = (char *)&prec->mlst - (char *)prec;
    prt->rec_size = sizeof(*prec);
    return 0;
}
epicsExportRegistrar(xxxRecordSizeOffset);

#ifdef __cplusplus
}
#endif
#endif /* GEN_SIZE_OFFSET */

#endif /* INC_xxxRecord_H */
