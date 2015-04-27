/* SNC Version 2.0.11: Wed Jan 13 09:54:06 2010: ../sncSRSRGA.stt */

/* Event flags */

/* Program "SRSRGA_ON" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 20
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram SRSRGA_ON;

/* Variable declarations */
struct UserVar {
	short	i;
	short	PvScan[9];
	int	PvScan_M2;
	int	FL_ON;
	int	FL_1;
	int	FL_0;
	int	CDEM_ON;
	int	CDEM_1;
	int	CDEM_0;
	float	FL_READ;
	int	FL_READ_ACT;
	int	ID;
	int	IN0;
	int	CA;
};

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "INIT" in state set "SRSRGA" */

/* Delay function for state "INIT" in state set "SRSRGA" */
static void D_SRSRGA_INIT(SS_ID ssId, struct UserVar *pVar)
{
# line 58 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "INIT" in state set "SRSRGA" */
static long E_SRSRGA_INIT(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 58 "../sncSRSRGA.stt"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "INIT" in state set "SRSRGA" */
static void A_SRSRGA_INIT(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 55 "../sncSRSRGA.stt"
			(pVar->ID) = 1;
			seq_pvPut(ssId, 17 /* ID */, 0);
			printf("ID READ ACTUATED !!\n");
		}
		return;
	}
}
/* Code for state "INIT_1" in state set "SRSRGA" */

/* Delay function for state "INIT_1" in state set "SRSRGA" */
static void D_SRSRGA_INIT_1(SS_ID ssId, struct UserVar *pVar)
{
# line 66 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "INIT_1" in state set "SRSRGA" */
static long E_SRSRGA_INIT_1(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 66 "../sncSRSRGA.stt"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "INIT_1" in state set "SRSRGA" */
static void A_SRSRGA_INIT_1(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 63 "../sncSRSRGA.stt"
			(pVar->FL_0) = 1;
			seq_pvPut(ssId, 13 /* FL_0 */, 0);
			printf("FIL OFF ACTUATED !!\n");
		}
		return;
	}
}
/* Code for state "INIT_2" in state set "SRSRGA" */

/* Delay function for state "INIT_2" in state set "SRSRGA" */
static void D_SRSRGA_INIT_2(SS_ID ssId, struct UserVar *pVar)
{
# line 74 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 0, (5.0));
}

/* Event function for state "INIT_2" in state set "SRSRGA" */
static long E_SRSRGA_INIT_2(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 74 "../sncSRSRGA.stt"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "INIT_2" in state set "SRSRGA" */
static void A_SRSRGA_INIT_2(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 71 "../sncSRSRGA.stt"
			(pVar->CA) = 1;
			seq_pvPut(ssId, 19 /* CA */, 0);
			printf("CALIB. ACTUATED !!\n");
		}
		return;
	}
}
/* Code for state "FIL_OFF" in state set "SRSRGA" */

/* Delay function for state "FIL_OFF" in state set "SRSRGA" */
static void D_SRSRGA_FIL_OFF(SS_ID ssId, struct UserVar *pVar)
{
# line 82 "../sncSRSRGA.stt"
# line 87 "../sncSRSRGA.stt"
}

/* Event function for state "FIL_OFF" in state set "SRSRGA" */
static long E_SRSRGA_FIL_OFF(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 82 "../sncSRSRGA.stt"
	if ((pVar->FL_ON) == 1)
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
# line 87 "../sncSRSRGA.stt"
	if ((pVar->CDEM_ON) == 1)
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "FIL_OFF" in state set "SRSRGA" */
static void A_SRSRGA_FIL_OFF(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 79 "../sncSRSRGA.stt"
			(pVar->FL_1) = 1;
			seq_pvPut(ssId, 12 /* FL_1 */, 0);
			printf("FIL ON !! \n");
		}
		return;
	case 1:
		{
# line 85 "../sncSRSRGA.stt"
			(pVar->CDEM_ON) = 0;
			seq_pvPut(ssId, 10 /* CDEM_ON */, 0);
		}
		return;
	}
}
/* Code for state "DELAY" in state set "SRSRGA" */

/* Delay function for state "DELAY" in state set "SRSRGA" */
static void D_SRSRGA_DELAY(SS_ID ssId, struct UserVar *pVar)
{
# line 95 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 0, (5.0));
}

/* Event function for state "DELAY" in state set "SRSRGA" */
static long E_SRSRGA_DELAY(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 95 "../sncSRSRGA.stt"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "DELAY" in state set "SRSRGA" */
static void A_SRSRGA_DELAY(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 92 "../sncSRSRGA.stt"
			(pVar->FL_READ_ACT) = 1;
			seq_pvPut(ssId, 16 /* FL_READ_ACT */, 0);
			seq_pvGet(ssId, 11 /* FL_READ */, 0);
		}
		return;
	}
}
/* Code for state "DELAY2" in state set "SRSRGA" */

/* Delay function for state "DELAY2" in state set "SRSRGA" */
static void D_SRSRGA_DELAY2(SS_ID ssId, struct UserVar *pVar)
{
# line 108 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 0, (2.0));
# line 117 "../sncSRSRGA.stt"
	seq_delayInit(ssId, 1, (2.0));
}

/* Event function for state "DELAY2" in state set "SRSRGA" */
static long E_SRSRGA_DELAY2(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 108 "../sncSRSRGA.stt"
	if ((pVar->FL_READ) > 0.9 && seq_delay(ssId, 0))
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
# line 117 "../sncSRSRGA.stt"
	if ((pVar->FL_READ) <= 0.9 && seq_delay(ssId, 1))
	{
		*pNextState = 6;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "DELAY2" in state set "SRSRGA" */
static void A_SRSRGA_DELAY2(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 105 "../sncSRSRGA.stt"
			for ((pVar->i) = 0; (pVar->i) < 10; (pVar->i)++)
			{
				(pVar->PvScan)[(pVar->i)] = 4;
				seq_pvPut(ssId, 0 /* PvScan */ + ((pVar->i)), 0);
			}
# line 106 "../sncSRSRGA.stt"
			printf("FL_READ = %lf  \n", (pVar->FL_READ));
			printf("SCAN Activated !! \n");
		}
		return;
	case 1:
		{
# line 114 "../sncSRSRGA.stt"
			for ((pVar->i) = 0; (pVar->i) < 10; (pVar->i)++)
			{
				(pVar->PvScan)[(pVar->i)] = 0;
				seq_pvPut(ssId, 0 /* PvScan */ + ((pVar->i)), 0);
			}
# line 115 "../sncSRSRGA.stt"
			printf("FL_READ = %lf  \n", (pVar->FL_READ));
			printf("SCAN Passive !! \n");
		}
		return;
	}
}
/* Code for state "FIL_ON" in state set "SRSRGA" */

/* Delay function for state "FIL_ON" in state set "SRSRGA" */
static void D_SRSRGA_FIL_ON(SS_ID ssId, struct UserVar *pVar)
{
# line 137 "../sncSRSRGA.stt"
# line 142 "../sncSRSRGA.stt"
# line 149 "../sncSRSRGA.stt"
}

/* Event function for state "FIL_ON" in state set "SRSRGA" */
static long E_SRSRGA_FIL_ON(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 137 "../sncSRSRGA.stt"
	if ((pVar->FL_ON) == 0)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 142 "../sncSRSRGA.stt"
	if ((pVar->CDEM_ON) == 1)
	{
		*pNextState = 7;
		*pTransNum = 1;
		return TRUE;
	}
# line 149 "../sncSRSRGA.stt"
	if ((pVar->FL_READ) <= 0.9)
	{
		*pNextState = 3;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "FIL_ON" in state set "SRSRGA" */
static void A_SRSRGA_FIL_ON(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 123 "../sncSRSRGA.stt"
			(pVar->FL_0) = 1;
			seq_pvPut(ssId, 13 /* FL_0 */, 0);
# line 129 "../sncSRSRGA.stt"
			for ((pVar->i) = 0; (pVar->i) < 10; (pVar->i)++)
			{
				(pVar->PvScan)[(pVar->i)] = 0;
				seq_pvPut(ssId, 0 /* PvScan */ + ((pVar->i)), 0);
			}
# line 130 "../sncSRSRGA.stt"
			(pVar->CDEM_0) = 1;
			seq_pvPut(ssId, 15 /* CDEM_0 */, 0);
			(pVar->CDEM_ON) = 0;
			seq_pvPut(ssId, 10 /* CDEM_ON */, 0);
			(pVar->FL_READ_ACT) = 1;
			seq_pvPut(ssId, 16 /* FL_READ_ACT */, 0);
			printf("FIL OFF !! \n");
		}
		return;
	case 1:
		{
# line 139 "../sncSRSRGA.stt"
			(pVar->CDEM_1) = 1;
			seq_pvPut(ssId, 14 /* CDEM_1 */, 0);
			printf("CDEM ON ++ \n");
		}
		return;
	case 2:
		{
# line 144 "../sncSRSRGA.stt"
			(pVar->FL_ON) = 0;
			seq_pvPut(ssId, 9 /* FL_ON */, 0);
			(pVar->FL_0) = 1;
			seq_pvPut(ssId, 13 /* FL_0 */, 0);
			printf("FIL Not ON \n");
		}
		return;
	}
}
/* Code for state "CDEM_ON" in state set "SRSRGA" */

/* Delay function for state "CDEM_ON" in state set "SRSRGA" */
static void D_SRSRGA_CDEM_ON(SS_ID ssId, struct UserVar *pVar)
{
# line 168 "../sncSRSRGA.stt"
# line 174 "../sncSRSRGA.stt"
}

/* Event function for state "CDEM_ON" in state set "SRSRGA" */
static long E_SRSRGA_CDEM_ON(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 168 "../sncSRSRGA.stt"
	if ((pVar->FL_ON) == 0)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 174 "../sncSRSRGA.stt"
	if ((pVar->CDEM_ON) == 0)
	{
		*pNextState = 8;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "CDEM_ON" in state set "SRSRGA" */
static void A_SRSRGA_CDEM_ON(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 154 "../sncSRSRGA.stt"
			(pVar->FL_0) = 1;
			seq_pvPut(ssId, 13 /* FL_0 */, 0);
# line 160 "../sncSRSRGA.stt"
			for ((pVar->i) = 0; (pVar->i) < 10; (pVar->i)++)
			{
				(pVar->PvScan)[(pVar->i)] = 0;
				seq_pvPut(ssId, 0 /* PvScan */ + ((pVar->i)), 0);
			}
# line 161 "../sncSRSRGA.stt"
			(pVar->CDEM_0) = 1;
			seq_pvPut(ssId, 15 /* CDEM_0 */, 0);
			(pVar->CDEM_ON) = 0;
			seq_pvPut(ssId, 10 /* CDEM_ON */, 0);
			(pVar->FL_READ_ACT) = 1;
			seq_pvPut(ssId, 16 /* FL_READ_ACT */, 0);
			printf("FIL OFF !! \n");
		}
		return;
	case 1:
		{
# line 171 "../sncSRSRGA.stt"
			(pVar->CDEM_0) = 1;
			seq_pvPut(ssId, 15 /* CDEM_0 */, 0);
			printf("CDEM OFF -- \n");
		}
		return;
	}
}
/* Code for state "CDEM_OFF" in state set "SRSRGA" */

/* Delay function for state "CDEM_OFF" in state set "SRSRGA" */
static void D_SRSRGA_CDEM_OFF(SS_ID ssId, struct UserVar *pVar)
{
# line 193 "../sncSRSRGA.stt"
# line 199 "../sncSRSRGA.stt"
}

/* Event function for state "CDEM_OFF" in state set "SRSRGA" */
static long E_SRSRGA_CDEM_OFF(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 193 "../sncSRSRGA.stt"
	if ((pVar->FL_ON) == 0)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 199 "../sncSRSRGA.stt"
	if ((pVar->CDEM_ON) == 1)
	{
		*pNextState = 7;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "CDEM_OFF" in state set "SRSRGA" */
static void A_SRSRGA_CDEM_OFF(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 179 "../sncSRSRGA.stt"
			(pVar->FL_0) = 1;
			seq_pvPut(ssId, 13 /* FL_0 */, 0);
# line 185 "../sncSRSRGA.stt"
			for ((pVar->i) = 0; (pVar->i) < 10; (pVar->i)++)
			{
				(pVar->PvScan)[(pVar->i)] = 0;
				seq_pvPut(ssId, 0 /* PvScan */ + ((pVar->i)), 0);
			}
# line 186 "../sncSRSRGA.stt"
			(pVar->FL_READ_ACT) = 1;
			seq_pvPut(ssId, 16 /* FL_READ_ACT */, 0);
			(pVar->CDEM_0) = 1;
			seq_pvPut(ssId, 15 /* CDEM_0 */, 0);
			(pVar->CDEM_ON) = 0;
			seq_pvPut(ssId, 10 /* CDEM_ON */, 0);
			printf("FIL OFF !!\n");
		}
		return;
	case 1:
		{
# line 196 "../sncSRSRGA.stt"
			(pVar->CDEM_1) = 1;
			seq_pvPut(ssId, 14 /* CDEM_1 */, 0);
			printf("CDEM ON ++ \n");
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
  {"VMS_CR_RGA_M2_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[0]), "PvScan[0]", 
    "short", 1, 1, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M4_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[1]), "PvScan[1]", 
    "short", 1, 2, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M12_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[2]), "PvScan[2]", 
    "short", 1, 3, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M18_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[3]), "PvScan[3]", 
    "short", 1, 4, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M28_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[4]), "PvScan[4]", 
    "short", 1, 5, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M32_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[5]), "PvScan[5]", 
    "short", 1, 6, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M40_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[6]), "PvScan[6]", 
    "short", 1, 7, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M44_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[7]), "PvScan[7]", 
    "short", 1, 8, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_M55_ION.SCAN", (void *)OFFSET(struct UserVar, PvScan[8]), "PvScan[8]", 
    "short", 1, 9, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_FL_ON", (void *)OFFSET(struct UserVar, FL_ON), "FL_ON", 
    "int", 1, 10, 0, 1, 0, 0, 0},

  {"VMS_CR_RGA_CDEM_ON", (void *)OFFSET(struct UserVar, CDEM_ON), "CDEM_ON", 
    "int", 1, 11, 0, 1, 0, 0, 0},

  {"VMS_CR_RGA_FL", (void *)OFFSET(struct UserVar, FL_READ), "FL_READ", 
    "float", 1, 12, 0, 1, 0, 0, 0},

  {"VMS_CR_RGA_FL_1.PROC", (void *)OFFSET(struct UserVar, FL_1), "FL_1", 
    "int", 1, 13, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_FL_0.PROC", (void *)OFFSET(struct UserVar, FL_0), "FL_0", 
    "int", 1, 14, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_CDEM_1.PROC", (void *)OFFSET(struct UserVar, CDEM_1), "CDEM_1", 
    "int", 1, 15, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_CDEM_0.PROC", (void *)OFFSET(struct UserVar, CDEM_0), "CDEM_0", 
    "int", 1, 16, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_FL.PROC", (void *)OFFSET(struct UserVar, FL_READ_ACT), "FL_READ_ACT", 
    "int", 1, 17, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_ID.PROC", (void *)OFFSET(struct UserVar, ID), "ID", 
    "int", 1, 18, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_IN0.PROC", (void *)OFFSET(struct UserVar, IN0), "IN0", 
    "int", 1, 19, 0, 0, 0, 0, 0},

  {"VMS_CR_RGA_CA.PROC", (void *)OFFSET(struct UserVar, CA), "CA", 
    "int", 1, 20, 0, 0, 0, 0, 0},

};

/* Event masks for state set SRSRGA */
	/* Event mask for state INIT: */
static bitMask	EM_SRSRGA_INIT[] = {
	0x00000000,
};
	/* Event mask for state INIT_1: */
static bitMask	EM_SRSRGA_INIT_1[] = {
	0x00000000,
};
	/* Event mask for state INIT_2: */
static bitMask	EM_SRSRGA_INIT_2[] = {
	0x00000000,
};
	/* Event mask for state FIL_OFF: */
static bitMask	EM_SRSRGA_FIL_OFF[] = {
	0x00000c00,
};
	/* Event mask for state DELAY: */
static bitMask	EM_SRSRGA_DELAY[] = {
	0x00000000,
};
	/* Event mask for state DELAY2: */
static bitMask	EM_SRSRGA_DELAY2[] = {
	0x00001000,
};
	/* Event mask for state FIL_ON: */
static bitMask	EM_SRSRGA_FIL_ON[] = {
	0x00001c00,
};
	/* Event mask for state CDEM_ON: */
static bitMask	EM_SRSRGA_CDEM_ON[] = {
	0x00000c00,
};
	/* Event mask for state CDEM_OFF: */
static bitMask	EM_SRSRGA_CDEM_OFF[] = {
	0x00000c00,
};

/* State Blocks */

static struct seqState state_SRSRGA[] = {
	/* State "INIT" */ {
	/* state name */       "INIT",
	/* action function */ (ACTION_FUNC) A_SRSRGA_INIT,
	/* event function */  (EVENT_FUNC) E_SRSRGA_INIT,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_INIT,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_INIT,
	/* state options */   (0)},

	/* State "INIT_1" */ {
	/* state name */       "INIT_1",
	/* action function */ (ACTION_FUNC) A_SRSRGA_INIT_1,
	/* event function */  (EVENT_FUNC) E_SRSRGA_INIT_1,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_INIT_1,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_INIT_1,
	/* state options */   (0)},

	/* State "INIT_2" */ {
	/* state name */       "INIT_2",
	/* action function */ (ACTION_FUNC) A_SRSRGA_INIT_2,
	/* event function */  (EVENT_FUNC) E_SRSRGA_INIT_2,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_INIT_2,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_INIT_2,
	/* state options */   (0)},

	/* State "FIL_OFF" */ {
	/* state name */       "FIL_OFF",
	/* action function */ (ACTION_FUNC) A_SRSRGA_FIL_OFF,
	/* event function */  (EVENT_FUNC) E_SRSRGA_FIL_OFF,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_FIL_OFF,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_FIL_OFF,
	/* state options */   (0)},

	/* State "DELAY" */ {
	/* state name */       "DELAY",
	/* action function */ (ACTION_FUNC) A_SRSRGA_DELAY,
	/* event function */  (EVENT_FUNC) E_SRSRGA_DELAY,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_DELAY,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_DELAY,
	/* state options */   (0)},

	/* State "DELAY2" */ {
	/* state name */       "DELAY2",
	/* action function */ (ACTION_FUNC) A_SRSRGA_DELAY2,
	/* event function */  (EVENT_FUNC) E_SRSRGA_DELAY2,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_DELAY2,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_DELAY2,
	/* state options */   (0)},

	/* State "FIL_ON" */ {
	/* state name */       "FIL_ON",
	/* action function */ (ACTION_FUNC) A_SRSRGA_FIL_ON,
	/* event function */  (EVENT_FUNC) E_SRSRGA_FIL_ON,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_FIL_ON,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_FIL_ON,
	/* state options */   (0)},

	/* State "CDEM_ON" */ {
	/* state name */       "CDEM_ON",
	/* action function */ (ACTION_FUNC) A_SRSRGA_CDEM_ON,
	/* event function */  (EVENT_FUNC) E_SRSRGA_CDEM_ON,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_CDEM_ON,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_CDEM_ON,
	/* state options */   (0)},

	/* State "CDEM_OFF" */ {
	/* state name */       "CDEM_OFF",
	/* action function */ (ACTION_FUNC) A_SRSRGA_CDEM_OFF,
	/* event function */  (EVENT_FUNC) E_SRSRGA_CDEM_OFF,
	/* delay function */   (DELAY_FUNC) D_SRSRGA_CDEM_OFF,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_SRSRGA_CDEM_OFF,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "SRSRGA" */ {
	/* ss name */            "SRSRGA",
	/* ptr to state block */ state_SRSRGA,
	/* number of states */   9,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "";

/* State Program table (global) */
struct seqProgram SRSRGA_ON = {
	/* magic number */       20000315,
	/* *name */              "SRSRGA_ON",
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

void SRSRGA_ONRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&SRSRGA_ON);
}
epicsExportRegistrar(SRSRGA_ONRegistrar);

