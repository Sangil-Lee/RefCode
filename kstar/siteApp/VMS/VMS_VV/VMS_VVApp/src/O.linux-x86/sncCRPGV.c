/* SNC Version 2.0.11: Tue Jan 12 17:02:56 2010: ../sncCRPGV.stt */

/* Event flags */
#define ef_SEQ_START	1

/* Program "CRPGV_ON" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 6
#define NUM_EVENTS 1
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram CRPGV_ON;

/* Variable declarations */
struct UserVar {
	int	SEQ_START;
	int	ENABLE;
	int	CRP1_GV;
	int	CRP2_GV;
	double	DELAY;
	double	DELAY_MONITOR;
};

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "MAIN" in state set "CRP_GV_SYNC" */

/* Delay function for state "MAIN" in state set "CRP_GV_SYNC" */
static void D_CRP_GV_SYNC_MAIN(SS_ID ssId, struct UserVar *pVar)
{
# line 37 "../sncCRPGV.stt"
}

/* Event function for state "MAIN" in state set "CRP_GV_SYNC" */
static long E_CRP_GV_SYNC_MAIN(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 37 "../sncCRPGV.stt"
	if (seq_efTestAndClear(ssId, ef_SEQ_START) && (pVar->SEQ_START) == 1 && (pVar->ENABLE) == 1)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "MAIN" in state set "CRP_GV_SYNC" */
static void A_CRP_GV_SYNC_MAIN(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 35 "../sncCRPGV.stt"
			printf("Go to State DELAY \n");
			(pVar->DELAY_MONITOR) = (pVar->DELAY);
		}
		return;
	}
}
/* Code for state "DELAY" in state set "CRP_GV_SYNC" */

/* Delay function for state "DELAY" in state set "CRP_GV_SYNC" */
static void D_CRP_GV_SYNC_DELAY(SS_ID ssId, struct UserVar *pVar)
{
# line 45 "../sncCRPGV.stt"
	seq_delayInit(ssId, 0, (1.0));
# line 55 "../sncCRPGV.stt"
	seq_delayInit(ssId, 1, (1.0));
}

/* Event function for state "DELAY" in state set "CRP_GV_SYNC" */
static long E_CRP_GV_SYNC_DELAY(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 45 "../sncCRPGV.stt"
	if (seq_delay(ssId, 0) && ((pVar->DELAY_MONITOR) > 0))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 55 "../sncCRPGV.stt"
	if (seq_delay(ssId, 1))
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "DELAY" in state set "CRP_GV_SYNC" */
static void A_CRP_GV_SYNC_DELAY(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 42 "../sncCRPGV.stt"
			(pVar->DELAY_MONITOR) = (pVar->DELAY_MONITOR) - 1;
			seq_pvPut(ssId, 5 /* DELAY_MONITOR */, 0);
			printf("DELAY_MONITOR = %f \n", (pVar->DELAY_MONITOR));
		}
		return;
	case 1:
		{
# line 49 "../sncCRPGV.stt"
			(pVar->CRP1_GV) = 0;
			(pVar->CRP2_GV) = 0;
			seq_pvPut(ssId, 1 /* CRP1_GV */, 0);
			seq_pvPut(ssId, 2 /* CRP2_GV */, 0);
			printf("DELAY = %f \n", (pVar->DELAY));
			printf("CRP_GV Closed!! \n");
		}
		return;
	}
}

/* Exit handler */
static void exit_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/************************ Tables ***********************/

/* Database Blocks */
static struct seqChan seqChan[NUM_CHANNELS] = {
  {"CCS_SHOTSEQ_START", (void *)OFFSET(struct UserVar, SEQ_START), "SEQ_START", 
    "int", 1, 2, 1, 1, 0, 0, 0},

  {"VMS_VV_CRP1_FV_HO", (void *)OFFSET(struct UserVar, CRP1_GV), "CRP1_GV", 
    "int", 1, 3, 0, 1, 0, 0, 0},

  {"VMS_VV_CRP2_FV_HO", (void *)OFFSET(struct UserVar, CRP2_GV), "CRP2_GV", 
    "int", 1, 4, 0, 1, 0, 0, 0},

  {"VMS_VV_ENABLE", (void *)OFFSET(struct UserVar, ENABLE), "ENABLE", 
    "int", 1, 5, 0, 1, 0, 0, 0},

  {"VMS_VV_DELAY", (void *)OFFSET(struct UserVar, DELAY), "DELAY", 
    "double", 1, 6, 0, 1, 0, 0, 0},

  {"VMS_VV_DELAY_MONITOR", (void *)OFFSET(struct UserVar, DELAY_MONITOR), "DELAY_MONITOR", 
    "double", 1, 7, 0, 1, 0, 0, 0},

};

/* Event masks for state set CRP_GV_SYNC */
	/* Event mask for state MAIN: */
static bitMask	EM_CRP_GV_SYNC_MAIN[] = {
	0x00000026,
};
	/* Event mask for state DELAY: */
static bitMask	EM_CRP_GV_SYNC_DELAY[] = {
	0x00000080,
};

/* State Blocks */

static struct seqState state_CRP_GV_SYNC[] = {
	/* State "MAIN" */ {
	/* state name */       "MAIN",
	/* action function */ (ACTION_FUNC) A_CRP_GV_SYNC_MAIN,
	/* event function */  (EVENT_FUNC) E_CRP_GV_SYNC_MAIN,
	/* delay function */   (DELAY_FUNC) D_CRP_GV_SYNC_MAIN,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_CRP_GV_SYNC_MAIN,
	/* state options */   (0)},

	/* State "DELAY" */ {
	/* state name */       "DELAY",
	/* action function */ (ACTION_FUNC) A_CRP_GV_SYNC_DELAY,
	/* event function */  (EVENT_FUNC) E_CRP_GV_SYNC_DELAY,
	/* delay function */   (DELAY_FUNC) D_CRP_GV_SYNC_DELAY,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_CRP_GV_SYNC_DELAY,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "CRP_GV_SYNC" */ {
	/* ss name */            "CRP_GV_SYNC",
	/* ptr to state block */ state_CRP_GV_SYNC,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "";

/* State Program table (global) */
struct seqProgram CRPGV_ON = {
	/* magic number */       20000315,
	/* *name */              "CRPGV_ON",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};


/* Register sequencer commands and program */

void CRPGV_ONRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&CRPGV_ON);
}
epicsExportRegistrar(CRPGV_ONRegistrar);

