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

/* HEPAK Property INDEX */

#define MAGIC                   0xCADBAFDA
#define IDX_QUALITY		0
#define IDX_PRESSURE            1
#define IDX_TEMPERATURE         2
#define IDX_MASS_DENSITY        3
#define IDX_SPECIFIC_VOLUME	4
#define IDX_Z_PV_OVER_RT        5
#define IDX_DPTSAT		6
#define	IDX_LATENTHEAT		7
#define IDX_ENTROPY		8
#define	IDX_ENTHALPY		9
#define	IDX_HELMHOLTZ		10
#define	IDX_INTERNAL_ENERGY	11
#define	IDX_GIBBS_ENERGY	12
#define	IDX_FUGACITY		13
#define IDX_CP			14
#define	IDX_CV			15
#define	IDX_GAMMA		16
#define	IDX_EXPANSIVITY		17
#define	IDX_GRUNEISEN		18
#define IDX_COMPRESSIBILITY	19
#define	IDX_SOUND_VELOCITY	20
#define	IDX_JTCOEFFICIENT	21
#define	IDX_DPDDT		22
#define IDX_DPDTD		23
#define	IDX_VDHDVP		24
#define IDX_VISCOSITY		25
#define	IDX_CONDUCTIVITY	26
#define	IDX_PRANDTN		27
#define	IDX_THERMALDIFF		28
#define	IDX_SURFACE_TENSION	29
#define IDX_DIELECTRIC		30
#define	IDX_REFRACTION		31
#define	IDX_DTV			32
#define IDX_DTP			33
#define	IDX_RHOSRHO		34
#define	IDX_SOUND_VELOCITY_2	35
#define IDX_SOUND_VELOCITY_4	36
#define	IDX_GORTER_MELLINK	37
#define	IDX_SFTC		38
#define	IDX_DIFF_T_LAMBDA	39
#define	IDX_RESERVED		40
#define IDX_RESERVED2		41
#define NUM_OF_IDX		42

typedef struct {
	int index;
	char *propName;
	char *propUnit;
} tableHePak;

static tableHePak pTableHePakData[] = {
	{ IDX_QUALITY,		"Quality",		"[-]"		},		/* 0 */
	{ IDX_PRESSURE,		"Pressure",		"Pa"		},		/* 1 */
	{ IDX_TEMPERATURE,	"Temperature",		"K"		},		/* 2 */
 	{ IDX_MASS_DENSITY,	"Density",		"kg/m3"		},		/* 3 */
	{ IDX_SPECIFIC_VOLUME,	"Specific Volume",	"m3/kg" 	},		/* 4 */
	{ IDX_Z_PV_OVER_RT,	"Z = PV/RT",		"[-]"		},		/* 5 */
	{ IDX_DPTSAT,		"DPTSat",		"Pa/K"		},		/* 6 */
	{ IDX_LATENTHEAT,	"Latent Heat",		"J/kg"		},		/* 7 */
	{ IDX_ENTROPY,		"Entropy",		"J/kg-K"	},		/* 8 */
	{ IDX_ENTHALPY,		"Enthalpy",		"J/kg"		},		/* 9 */
	{ IDX_HELMHOLTZ,	"Helmholtz",		"J/kg"		},		/* 10 */
	{ IDX_INTERNAL_ENERGY,  "Internal Energy",	"J/kg"		},		/* 11 */
	{ IDX_GIBBS_ENERGY,  	"Gibbs Energy",		"J/kg"		},		/* 12 */
	{ IDX_FUGACITY,		"Fugacity",		"[-]"		},		/* 13 */
	{ IDX_CP,		"Cp",			"J/kg-K"	},		/* 14 */
	{ IDX_CV,		"Cv",			"J/kg-K"	},		/* 15 */
	{ IDX_GAMMA,		"Gamma",		"[-]"		},		/* 16 */
	{ IDX_EXPANSIVITY,	"Expansivity",		"[-]"		},		/* 17 */
	{ IDX_GRUNEISEN,	"Gruneisen",		"[-]"		},		/* 18 */
	{ IDX_COMPRESSIBILITY,	"Compressibility",	"1/Pa"		},		/* 19 */
	{ IDX_SOUND_VELOCITY, 	"Sound Velocity",	"m/s"		},		/* 20 */
	{ IDX_JTCOEFFICIENT,	"Joul-Thomson Coefficient", "K/Pa"	},		/* 21 */
	{ IDX_DPDDT,		"dP/dD at constant T",	"Pa-m3/kg"	},		/* 22 */
	{ IDX_DPDTD,		"dP/dT at constant D",	"Pa/K"		},		/* 23 */
	{ IDX_VDHDVP,		"V*dH/dV at constant P", "J/kg"		},		/* 24 */
	{ IDX_VISCOSITY,	"Viscosity",		"Pa-s"		},		/* 25 */
	{ IDX_CONDUCTIVITY,	"Conductivity",		"W/m-K"		},		/* 26 */
	{ IDX_PRANDTN,		"Prandtl Number",	"[-]"		},		/* 27 */
	{ IDX_THERMALDIFF,	"Thermal Diffusivity",  "m2/s"		},		/* 28 */
	{ IDX_SURFACE_TENSION,  "Surface Tension",	"N/m"		},		/* 29 */
	{ IDX_DIELECTRIC,	"Dielelctric Constant - 1.0", "[-]"	},		/* 30 */
	{ IDX_REFRACTION,	"Refractive Index - 1.0", "[-]"		},		/* 31 */
	{ IDX_DTV,		"Isochoric dT to the lambda line: dT(V)", "K" },	/* 32 */
	{ IDX_DTP,		"Isobaric dT to the lambda line :dT(P)", "K" },		/* 33 */
	{ IDX_RHOSRHO,		"Superfluid Density Fraction", "[-]"	},		/* 34 */
	{ IDX_SOUND_VELOCITY_2,	"2nd Sound Velocity", 	"m2/s"		},		/* 35 */
	{ IDX_SOUND_VELOCITY_4,	"4th Sound Velocity",	"m2/s"		},		/* 36 */
	{ IDX_GORTER_MELLINK,	"Gorter-Mellink Mutual Friction constant", "m-s/kg" },	/* 37 */
	{ IDX_SFTC,		"SuperFluid Thermal Conductivity Function", "W3/m5-kg" },/* 38 */
	{ IDX_DIFF_T_LAMBDA,    "Lambda line temperature (T - T_lambda)", "K" },	/* 39 */
	{ IDX_RESERVED,		"Reserved",		"[-]" 		},		/* 40 */
	{ IDX_RESERVED2,	"Reserved",		"[-]"		},		/* 41 */
	{ -1, 			NULL, NULL }						/* 42 */
};


static int hepSubDebug = 0;
epicsExportAddress(int, hepSubDebug);

typedef long (*processMethod)(subRecord *precord);

/* external declaration for functions in heprop.for */
extern void calc_ (int *idid, double **prp, int *jout,
		   int *j1, double *valu1,
		   int *j2, double *valu2,
		   int *jprecs, int *junits);


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
    int               iounit, iprecis, jout;
    int               prop1_index, prop2_index;
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
    double            prop[NUM_OF_IDX * 3];
    commonDataType    *pcommonData;
    tableHePak	      *pTableHePak;

    #ifdef linux
        #ifdef _X86_
            unsigned long long startClock;
            unsigned long long finishClock;
        #endif
    #endif
} hepDataType;

static epicsThreadOnceId      threadOnceFlag =  EPICS_THREAD_ONCE_INIT;
static epicsMessageQueueId    queueId;
static epicsThreadId          threadId;
static commonDataType         *pcommonData = NULL;

static char *idid_str[] = { "reserved phase",
                            "single phase in PROP(I,0) only",
                            "sat liquid and vapor in PROP(I,1) and PROP(I,2) only",
                            "mixture properties in PROP(I,0) + [IDID=2]",
                            "sat liquid in PROP(I,1) only",
                            "sat vapor  in PROP(I,2) only" };

static EPICSTHREADFUNC calcHepThread(void *param)
{
    int        i;
    subRecord *precord;
    hepDataType *pmyData;
    tableHePak *pTableHePak;
    struct rset  *prset;

    while(epicsMessageQueueReceive(queueId, (void*) &precord, sizeof(subRecord*))) {
        if(hepSubDebug) {
            epicsMessageQueueShow(queueId, 1);
            printf("Thread %s: Record %s requested thread processing, private(%p)\n",
                   epicsThreadGetNameSelf(),
                   precord->name,
		   (void*) precord->dpvt);
        }

        pmyData = (hepDataType*) precord->dpvt;
        pcommonData = pmyData->pcommonData;
        pTableHePak = pmyData->pTableHePak;
        prset   = (struct rset*)  precord->rset;

        #ifdef linux
            #ifdef _X86_
                pmyData->startClock = asmReadCPUClock();
            #endif
        #endif

        /* calculate here */
	calc_(&pmyData->idid, (double**) &pmyData->prop, &pcommonData->jout,
              &pcommonData->prop1_index, &pmyData->prs,
              &pcommonData->prop2_index, &pmyData->temp,
              &pcommonData->iprecis,     &pcommonData->iounit);


        #ifdef linux
            #ifdef _X86_
                pmyData->finishClock = asmReadCPUClock();
            #endif
        #endif

        if(hepSubDebug) {
            if(pmyData->idid > 0) printf("(%s) Data information: %s\n", 
                                         precord->name,
                                         idid_str[pmyData->idid]); 
            else                  printf("GASPAK fail in %s\n", precord->name);
            for(i=0; i<106; i++) printf("-"); printf("\n"); 
            printf("%40s, %20s, %20s, %20s\n",
                   "property", "single phase", "liquid", "vapor");
            for(i=0; i<106; i++) printf("-"); printf("\n"); 
            for(i=0; i<NUM_OF_IDX; i++) printf("%40s: %10.2lf%10s, %10.2lf%10s, %10.2lf%10s\n", 
						(pTableHePak+i)->propName,
                                                pmyData->prop[i], (pTableHePak+i)->propUnit,
                                                pmyData->prop[i+NUM_OF_IDX], (pTableHePak+i)->propUnit,
                                                pmyData->prop[i+NUM_OF_IDX*2], (pTableHePak+i)->propUnit);
            for(i=0; i<106; i++) printf("-"); printf("\n");
  
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
    commonDataType *pcommonData;

    #ifdef linux
        #ifdef _X86_
            unsigned long long   clockCounter;
        #endif
    #endif

    queueId  = epicsMessageQueueCreate(200, sizeof(subRecord*));
    threadId = epicsThreadCreate("calcHepThread",
                                 epicsThreadPriorityLow,
                                 epicsThreadGetStackSize(epicsThreadStackSmall),
                                 (EPICSTHREADFUNC) calcHepThread,
                                 (void*) NULL);

    pcommonData = (commonDataType*) malloc(sizeof(commonDataType));

    if(hepSubDebug) printf("CommonData Allocation (%p)\n", pcommonData);

    pcommonData->jout    = 123456789;
    pcommonData->iprecis = 2;
    pcommonData->iounit  = 1;
    pcommonData->prop1_index = IDX_PRESSURE;
    pcommonData->prop2_index = IDX_TEMPERATURE;


    #ifdef linux
        #ifdef _X86_
            clockCounter = asmReadCPUClock(); 
            usleep(1000000);
            pcommonData->clockRate = asmReadCPUClock() - clockCounter;
            if (hepSubDebug) {
                printf("Thread %s took the CPU clock rate\n",
                       epicsThreadGetNameSelf());
                printf("The estimated clock rate: %.3f MHz\n", (float)pcommonData->clockRate * (float)1.E-6);
            }
        #endif
    #endif

    *param = pcommonData; 

}

static long hepSubInput(subRecord *precord)
{
    hepDataType *pmyData       = (hepDataType*) precord->dpvt;

    pmyData->idid = 1;
    pmyData->prs  = precord->a;     /* pressure reading from INPA [KPa] */
    pmyData->temp = precord->b;     /* temperature reading from INPB [K] */

    return 0;
}

static long hepSubOutput(subRecord *precord)
{
    hepDataType   *pmyData       = (hepDataType*) precord->dpvt;
    tableHePak     *pTableHePak  = pmyData->pTableHePak;


    if(pmyData->idid != 1)   return 0;    /* It is not single phase helium. Thus, nothing to do */


    precord->a = pmyData->prop[IDX_PRESSURE];        
    precord->b = pmyData->prop[IDX_TEMPERATURE];    
    precord->c = pmyData->prop[IDX_MASS_DENSITY];  
    precord->d = pmyData->prop[IDX_ENTHALPY];    
    precord->e = pmyData->prop[IDX_VISCOSITY];  

    precord->k = pmyData->idid;
    precord->l = (unsigned) pmyData;

    precord->val = pmyData->prop[IDX_MASS_DENSITY];
    strcpy(precord->egu, (pTableHePak+IDX_MASS_DENSITY)->propUnit);  

    return 0;
}

static long hepSubInit(subRecord *precord,processMethod process)
{
    hepDataType *pmyData;

    if (hepSubDebug)
        printf("Thread %s: Record %s called hepSubInit(%p, %p)\n",
               epicsThreadGetNameSelf(),
               precord->name, (void*) precord, (void*) process);

    epicsThreadOnce(&threadOnceFlag, (void(*))spawnThreadmakeCommonData, &pcommonData);

    pmyData                  = (hepDataType*) malloc(sizeof(hepDataType));
    pmyData->pcommonData     = pcommonData;
    pmyData->magic           = MAGIC;
    pmyData->pTableHePak     = pTableHePakData;
    precord->dpvt            = (void*) pmyData;

	printf("Private data Alloc: %d bytes\n", sizeof(hepDataType));

    return(0);
}

static long hepSubProcess(subRecord *precord)
{
    
    if (hepSubDebug)
        printf("Record %s called hepSubProcess(%p), private(%p)\n",
               precord->name, (void*) precord, (void*) precord->dpvt);

    if(precord->pact) {
        if(hepSubDebug)
            printf("Thread %s: Record %s is processing ASYN phaseII\n", epicsThreadGetNameSelf(), precord->name);

        /* prepare output data */
        hepSubOutput(precord); 

        return (0);
    }
   
    if(hepSubDebug)
        printf("Thread %s: Record %s is processing ASYN phaseI\n", epicsThreadGetNameSelf(), precord->name); 
    precord->pact = TRUE;

    /* fetch data from input */
    hepSubInput(precord);

    /* request calculation */
    epicsMessageQueueSend(queueId, (void*) &precord, sizeof(subRecord*));
    return(0);
}

static long hepSubPostInit(subRecord *precord, processMethod process)
{
    return (0);
}

static long hepSubPostProcess(subRecord *precord)
{
    hepDataType *pmyData = (hepDataType*) (unsigned) precord->a;
    commonDataType *pcommonData;
    tableHePak     *pTableHePak;

    int propI             = (int) precord->b;
    int propJ             = (int) precord->c;

    if(!pmyData || pmyData->magic != MAGIC) return 0;
    if(propI <= -1 || propI >= NUM_OF_IDX || propJ <0 || propJ > 2 ) return 0;

    pcommonData = pmyData->pcommonData;
    pTableHePak = pmyData->pTableHePak;

    if(propI > -1) {
        strcpy(precord->egu, (pTableHePak+propI)->propUnit);
        precord->val = pmyData->prop[propI + (propJ * NUM_OF_IDX)];
    }

    if(propI < 0) {
        precord->val = pmyData->idid;
    }

    if(hepSubDebug) {
        printf("Record (%s): %lf %s\n", precord->name, precord->val, precord->egu);
    }
 
    return (0);
}

static registryFunctionRef hepSubRef[] = {
    {"hepSubInit",(REGISTRYFUNCTION)hepSubInit},
    {"hepSubProcess",(REGISTRYFUNCTION)hepSubProcess},
    {"hepSubPostInit", (REGISTRYFUNCTION)hepSubPostInit},
    {"hepSubPostProcess", (REGISTRYFUNCTION)hepSubPostProcess}
};

static void hepSubRegistrar(void)
{
    registryFunctionRefAdd(hepSubRef,NELEMENTS(hepSubRef));
}
epicsExportRegistrar(hepSubRegistrar);
