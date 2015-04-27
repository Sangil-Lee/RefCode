#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    #ifdef linux
        #ifdef _X86_
            #include <unistd.h>
        #endif
    #endif

#include <link.h>
#include <dbDefs.h>
#include <dbLock.h>
#include <dbAddr.h>
/* #include <dbAccessDefs.h> */
#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <registryFunction.h>
#include <recSup.h>
#include <subRecord.h>
#include <epicsExport.h>

#define MAGIC                   0xCAFABADA
#define HE89FILENAME            "../../db/HE89.GAS"
#define IDX_PRESSURE            1
#define IDX_TEMPERATURE         2
#define IDX_MASS_DENSITY        3
#define IDX_ENTHALPY            5
#define IDX_VISCOSITY           21

static int gaspSubDebug = 0;
epicsExportAddress(int, gaspSubDebug);

typedef long (*processMethod)(subRecord *precord);

/* external declaration for functions in gasprop.f */

typedef char str3[4];
typedef char str10[11];
typedef char str15[16];
typedef char str63[64];
typedef char str127[128];
typedef char* ptr;

extern void precis_ (int *iprecis);
extern void readcf_ (char **cofile, int *idid);
extern void unilib_ (int *iounit);
extern void calc_ (int *idid, double **prop, char *outpar,
        char *inpar1, double *val1, char *inpar2, double *val2,
        char *cofile, int *iounit);
extern void gunlbl_ (str10 *uarray, int *iounit);
extern void gascon_ (double **constp, str10 *ucona);
extern void gaserr_ (char *message, int *idid);

#ifdef linux
    #ifdef _X86_
        static unsigned long long int asmReadCPUClock(void)
        {
                unsigned long long int x;
                __asm__ volatile(".byte 0x0f, 0x31" : "=A" (x));
                return x;
        }
    #endif
#endif

typedef struct {
    int               iounit, iprecis;
    str10             uarray[32];
    str10             unit[32];
    str127            cofile;

    #ifdef linux
        #ifdef _X86_
        unsigned long long clockRate;
        #endif
    #endif
} commonDataType;

typedef struct {
    unsigned          magic;
    int               idid;
    double            prs, temp;
    double            prop[96];
    commonDataType    *pcommonData;

    #ifdef linux
        #ifdef _X86_
            unsigned long long startClock;
            unsigned long long finishClock;
        #endif
    #endif
} gaspDataType;

static epicsThreadOnceId      threadOnceFlag =  EPICS_THREAD_ONCE_INIT;
static epicsMessageQueueId    queueId;
static epicsThreadId          threadId;
static commonDataType         *pcommonData;

static char *idid_str[] = { "",
                            "single phase in PROP(I,0) only",
                            "sat liquid and vapor in PROP(I,1) and PROP(I,2) only",
                            "mixture properties in PROP(I,0) + [IDID=2]",
                            "sat liquid in PROP(I,1) only",
                            "sat vapor  in PROP(I,2) only" };

static EPICSTHREADFUNC calcGaspThread(void *param)
{
    int        i;
    subRecord *precord;
    gaspDataType *pmyData;
    struct rset  *prset;

    while(epicsMessageQueueReceive(queueId, (void*) &precord, sizeof(subRecord*))) {
        if(gaspSubDebug) {
            epicsMessageQueueShow(queueId, 1);
            printf("Thread %s: Record %s requested thread processing\n",
                   epicsThreadGetNameSelf(),
                   precord->name);
        }

        pmyData = (gaspDataType*) precord->dpvt;
        pcommonData = pmyData->pcommonData;
        prset   = (struct rset*)  precord->rset;

        #ifdef linux
            #ifdef _X86_
                pmyData->startClock = asmReadCPUClock();
            #endif
        #endif

        /* calculate here */
        calc_(&pmyData->idid, &pmyData->prop, 
              "**", 
               "P", &pmyData->prs,
               "T", &pmyData->temp,
               pcommonData->cofile,
               &pcommonData->iounit);

        #ifdef linux
            #ifdef _X86_
                pmyData->finishClock = asmReadCPUClock();
            #endif
        #endif

        if(gaspSubDebug) {
            if(pmyData->idid > 0) printf("(%s) Data information: %s\n", 
                                         precord->name,
                                         idid_str[pmyData->idid]); 
            else                  printf("GASPAK fail in %s\n", precord->name);
            for(i=0; i<66; i++) printf("-"); printf("\n"); 
            printf("%20s, %20s, %20s\n",
                   "single phase", "liquid", "vapor");
            for(i=0; i<66; i++) printf("-"); printf("\n"); 
            for(i=0; i<32; i++) printf("%10.5lf%10s, %10.5lf%10s, %10.5lf%10s\n", 
                                       pmyData->prop[i], pcommonData->unit[i],
                                       pmyData->prop[i+32], pcommonData->unit[i],
                                       pmyData->prop[i+64], pcommonData->unit[i]);
            for(i=0; i<66; i++) printf("-"); printf("\n");
  
            #ifdef linux
                #ifdef _X86_
                    printf("Flight Time: %.3f usec\n", 
                           (float)(pmyData->finishClock - pmyData->startClock)/(float)(pcommonData->clockRate) * 1.E+6);
                #endif
            #endif

        }
               
        /* make record processing again to complete the processing */
        dbScanLock((struct dbCommon*)precord);
        (*prset->process)(precord);
        dbScanUnlock((struct dbCommon*)precord);

    }

    return 0;
}

static void spawnThreadmakeCommonData(commonDataType **param)
{
    int            i;
    commonDataType *pcommonData;

    #ifdef linux
        #ifdef _X86_
            unsigned long long   clockCounter;
        #endif
    #endif

    queueId  = epicsMessageQueueCreate(200, sizeof(subRecord*));
    threadId = epicsThreadCreate("calcGaspThread",
                                 epicsThreadPriorityLow,
                                 epicsThreadGetStackSize(epicsThreadStackSmall),
                                 calcGaspThread,
                                 NULL);

    pcommonData = (commonDataType*) malloc(sizeof(commonDataType));

    pcommonData->iprecis = 2;
    pcommonData->iounit  = 2;
    strcpy(pcommonData->cofile, HE89FILENAME);

    precis_(&pcommonData->iprecis);
    gunlbl_(pcommonData->uarray, &pcommonData->iounit);
    for(i = 0; i < 32; i++) 
        sscanf(pcommonData->uarray[i], "%s", pcommonData->unit[i]);


    #ifdef linux
        #ifdef _X86_
            clockCounter = asmReadCPUClock(); 
            usleep(1000000);
            pcommonData->clockRate = asmReadCPUClock() - clockCounter;
            if (gaspSubDebug) {
                printf("Thread %s took the CPU clock rate\n",
                       epicsThreadGetNameSelf());
                printf("The estimated clock rate: %.3f MHz\n", (float)pcommonData->clockRate * (float)1.E-6);
            }
        #endif
    #endif

    *param = pcommonData; 
}

static long gaspSubInput(subRecord *precord)
{
    gaspDataType *pmyData       = (gaspDataType*) precord->dpvt;

    pmyData->prs  = precord->a;     /* pressure reading from INPA [KPa] */
    pmyData->temp = precord->b;     /* temperature reading from INPB [K] */

    return 0;
}

static long gaspSubOutput(subRecord *precord)
{
    gaspDataType   *pmyData       = (gaspDataType*) precord->dpvt;

    if(pmyData->idid != 1)   return 0;    /* It is not single phase helium. Thus, nothing to do */

    precord->a = pmyData->prop[IDX_PRESSURE];        /* pressure [KPa] */
    precord->b = pmyData->prop[IDX_TEMPERATURE];     /* temperature [K] */
    precord->c = pmyData->prop[IDX_MASS_DENSITY];    /* density [g/cm3] */
    precord->d = pmyData->prop[IDX_ENTHALPY];        /* enthalpy [J/g] */
    precord->e = pmyData->prop[IDX_VISCOSITY];       /* viscosity [uPa-s] */

    precord->k = pmyData->idid;
    precord->l = (unsigned) pmyData;

    return 0;
}

static long gaspSubInit(subRecord *precord,processMethod process)
{
    gaspDataType *pmyData;

    if (gaspSubDebug)
        printf("Thread %s: Record %s called gaspSubInit(%p, %p)\n",
               epicsThreadGetNameSelf(),
               precord->name, (void*) precord, (void*) process);

    epicsThreadOnce(&threadOnceFlag, (void(*))spawnThreadmakeCommonData, &pcommonData);

    pmyData                  = (gaspDataType*) malloc(sizeof(gaspDataType));
    pmyData->pcommonData     = pcommonData;
    pmyData->magic           = MAGIC;
    precord->dpvt            = (void*) pmyData;

    return(0);
}

static long gaspSubProcess(subRecord *precord)
{
    gaspDataType *pmyData = (gaspDataType*) precord->dpvt;
    
    if (gaspSubDebug * 0)
        printf("Record %s called gaspSubProcess(%p)\n",
               precord->name, (void*) precord);

    if(precord->pact) {
        if(gaspSubDebug)
            printf("Thread %s: Record %s is processing ASYN phaseII\n", epicsThreadGetNameSelf(), precord->name);

        /* prepare output data */
        gaspSubOutput(precord);

        return (0);
    }
   
    if(gaspSubDebug)
        printf("Thread %s: Record %s is processing ASYN phaseI\n", epicsThreadGetNameSelf(), precord->name); 
    precord->pact = TRUE;

    /* fetch data from input */
    gaspSubInput(precord);

    /* request calculation */
    epicsMessageQueueSend(queueId, (void*) &precord, sizeof(subRecord*));
    return(0);
}

static long gaspSubPostInit(subRecord *precord, processMethod process)
{
    return (0);
}

static long gaspSubPostProcess(subRecord *precord)
{
    gaspDataType *pmyData = (gaspDataType*) (unsigned) precord->a;
    commonDataType *pcommonData;
    int propI             = (int) precord->b;
    int propJ             = (int) precord->c;

    if(!pmyData || pmyData->magic != MAGIC) return 0;
    if(propI < -1 || propI > 31 || propJ <0 || propJ > 2 ) return 0;

    pcommonData = pmyData->pcommonData;

    if(propI > -1) {
        strcpy(precord->egu, pcommonData->unit[propI]);
        precord->val = pmyData->prop[propI + (propJ * 32)];
    }

    if(propI < 0) {
        precord->val = pmyData->idid;
    }

    if(gaspSubDebug) {
        printf("Record (%s): %lf %s\n", precord->name, precord->val, precord->egu);
    }
 
    return (0);
}

static registryFunctionRef gaspSubRef[] = {
    {"gaspSubInit",(REGISTRYFUNCTION)gaspSubInit},
    {"gaspSubProcess",(REGISTRYFUNCTION)gaspSubProcess},
    {"gaspSubPostInit", (REGISTRYFUNCTION)gaspSubPostInit},
    {"gaspSubPostProcess", (REGISTRYFUNCTION)gaspSubPostProcess}
};

static void gaspSubRegistrar(void)
{
    registryFunctionRefAdd(gaspSubRef,NELEMENTS(gaspSubRef));
}
epicsExportRegistrar(gaspSubRegistrar);
