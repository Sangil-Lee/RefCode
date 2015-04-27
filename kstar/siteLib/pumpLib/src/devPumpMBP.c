/*
 * MBP device support
 */

#include <epicsStdio.h>
#include <devCommonGpib.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************
 *
 * The following define statements are used to declare the names to be used
 * for the dset tables.   
 *
 * A DSET_AI entry must be declared here and referenced in an application
 * database description file even if the device provides no AI records.
 *
 ******************************************************************************/
#define DSET_AI     devAiMBP
#define DSET_AO     devAoMBP
#define DSET_BI     devBiMBP
#define DSET_BO     devBoMBP
#define DSET_EV     devEvMBP
#define DSET_LI     devLiMBP
#define DSET_LO     devLoMBP
#define DSET_MBBI   devMbbiMBP
#define DSET_MBBID  devMbbidMBP
#define DSET_MBBO   devMbboMBP
#define DSET_MBBOD  devMbbodMBP
#define DSET_SI     devSiMBP
#define DSET_SO     devSoMBP
#define DSET_WF     devWfMBP

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     5    /* I/O must complete within this time */
#define TIMEWINDOW  2   /* Wait this long after device timeout */

static int convertData();
static int devPumpMBPDebug = 0;
epicsExportAddress(int, devPumpMBPDebug);

#define EOS1 "\r"
#define EOS2 "\r\n"

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
     FILL10,
     FILL10,
     /* Param 20, Electrical suppy voltage                           0.1V */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V2\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 21, Dry pump phase current                             0.1A */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V3\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 22, Dry pump pwer                                     0.1kW */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V4\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 23, Voltage reading from dry pump thermistor          0.1mV */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V5\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 24, Imbalance in dry pump phase current               0.005% */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V6\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 25, Mechanical booster pump phase current,            0.1A */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V7\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 26, Mechanical booster pump power                     0.1kW */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V8\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 27, Voltage reading from mechanical booster pump thermistor 0.1mV */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V9\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 28, Imbalance in mechanical booster pump phase current 0.005% */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V10\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 29, Total running time                                hours */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V14\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     
     /* Param30, Hours on process                                   hours */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V16\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 31, Time to stop                                      seconds */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V21\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 32, Final stage purge nitrogen flow                   ml s-1 */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V32\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 33, Total nitrogen purge flow                         ml s-1 */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V35\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 34, Exhaust pressure,                                 0.1 kPa */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V39\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 35, Shaft-seals purge pressure,                       0.1 kPa */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V40\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 36, Time for gas sensors to zero                      seconds */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V48\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 37, Analog water flow                                 ml s-1 */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V52\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 38, Active gauge pressure                             Pa or V */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V53\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 39, Mechanical booster pump motor temperature         0.1 K */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V54\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     
     /* Param 40, Dry pump motor temperature                        0.1 K */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V55\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 41, Exhaust temperature                               0.1 K */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V56\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 42, Dry pump body temperature                         0.1 K */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V57\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 43, Inverter current                                  0.1 A */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V172\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 44, Inverter power                                    0.1 kW */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V173\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 45, Inverter speed                                    0.1 Hz */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V174\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},
     /* Param 46, Inverter torque                                   0.005% */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "?V175\r", NULL, 0, 32, 0,0,0, NULL, NULL, EOS2},

     FILL, FILL, FILL,

     /* Param 50, Serial number */
     {&DSET_SI, GPIBREAD, IB_Q_LOW, "?S\r",    NULL, 0, 32, 0,0,0, NULL, NULL, EOS2}
 

};

/* The following is the number of elements in the command array above.  */
#define NUMPARAMS sizeof(gpibCmds)/sizeof(struct gpibCmd)

/******************************************************************************
 * Initialize device support parameters
 *
 *****************************************************************************/
static long init_ai(int parm)
{
    if(parm==0) {
        devSupParms.name = "devMBP";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
        devSupParms.debugFlag = &devPumpMBPDebug;
    }
    return(0);
}
