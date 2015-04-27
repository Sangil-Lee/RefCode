/* @(#)serverdefs.h	1.149 05/25/07
******************************************************************************
FILE: serverdefs.h

This file contains the definitions used by the basic infrastructure
code of the control system. This includes the waveserver, host_realtime,
lockserver and realtime executables.

------------------------------------------------------------------------------
Modifications:
1/29/93: created by J. Ferron
11/1/94: version 10
11/18/94: BGP Changed for version 10
12/13/94: BGP added case_wv2mp.
12/16/94: BGP added code for simulation routines.
12/19/94: add diagnostic printout command.
2/17/95: BGP changes for new vector_info structure.
3/7/95: BGP organized file into sections.
3/20/96: BGP changes for 5cpu version of pcs.
5/21/96: BGP, changes for 6 cpu version of pcs.
11/13/97: BGP add hostshotphase structures (for host parameter data)
12/03/97: BGP add RESTORE_REQUEST message for new restore method.
09/10/99: BDJ add linked_names_list structure definition
11/04/99: BGP changes to support endian differences.
1/00: JRF changes for 32/64 bit compatibility
5/20/2000: JRF  remove the PCSUPGRADE macros, leave only the upgrade code.
4/11/2001: RDJ  add location dependent infra installdefs.h
******************************************************************************
*/
#include "descriptors.h"

#ifndef _RUN_REALTIME_ /* The hc860 compiler doesn't like this file. */
#include "socketcom.h"
#endif

#include "processor_type.h"
#include "convert_functions.h"
/*
Define the macros for descriptor generation.  This first definition
defines the macros so that they simply generate the variable
definition code.
*/
#define DESCRIPTORS_PART_1
#include "descriptors.h"
#undef DESCRIPTORS_PART_1

#include "realtime_data.h"

#ifndef _SERVERDEFS_
#define _SERVERDEFS_

/*
==============================================================================
SECTION: macro keyword definitions
==============================================================================
*/
/*
------------------------------------------------------------------------------
general
------------------------------------------------------------------------------
*/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DEBUG 0      /* set to 1 to get debug printouts */
#define PHASEDEBUG 1
#define DEBUG_SEQCHANGELIST 1 /* set to 1 to have phase sequence
                              change list summary printed. */
#define DEBUG_CHANGELIST 0 /* set to 1 to have change list summary printed
                              in wvmp etc. */
#define DEBUG_PIDCHANGELIST 0 /* set to 1 to have change list summary printed
                              in make_pidtau */

#define SAVEROUTINE_INDEX(ENUM) (((C_CODE*0x10000) + (A_CODE*0x100)) + ENUM)
		/* macro used in generating a unique saveroutine index for 
		   each saveroutine defined in an algorithm master file. 
		   in generating this unique index, this macro assumes
		   limits on the number of categories (256), algorithms per
		   category (256) and saveroutine codes per algorithm (256) */

#define LAST_ELEMENT {0,} /* use to mark end of array of unknown size */
/*
------------------------------------------------------------------------------
Modes waveserver can run in.
This is determined from a command line argument to the waveserver
which corresponds to the RUNMODE.
------------------------------------------------------------------------------
*/
#define MODE_IS_NORMAL 0
#define MODE_IS_TEST 1
#define MODE_IS_STANDALONE 2
#define MODE_IS_FUTURE_SHOT 3
/*
------------------------------------------------------------------------------
Types of "software" test modes.
These are the values that the software_test_mode flag in the global
parameter data can have.  Note that a nonzero value indicates that a
test mode is selected.  The code uses if(software_test_mode) to detect
that any one of the possible test modes is selected.
------------------------------------------------------------------------------
*/
#define NORMAL_OPERATION_MODE 0
#define SOFTWARE_TEST_MODE 1
#define SIMULATION_TEST_MODE 2
#define SETUP_TEST_MODE 3
/*
------------------------------------------------------------------------------
macros which uniquely identify each of the different vector types
used in the plasma control software
------------------------------------------------------------------------------
*/
#define TARGET_VECTOR 0
#define ISTARGET_VECTOR 1
#define ERROR_VECTOR 2
#define P_VECTOR 3
#define SHAPE_VECTOR 4
#define INTCOMMAND_VECTOR 5
#define FPCOMMAND_VECTOR 6
#define FUNCTION_VECTOR 7
#define COMMUNICATION_VECTOR 8
#define PIDTAU_VECTOR 9

/*
------------------------------------------------------------------------------
macros which indicate which of the vector_info fields in the
cat_alg structure a vector belongs to
------------------------------------------------------------------------------
*/
#define T_VECTOR 1
#define CT_VECTOR 1
#define IST_VECTOR 1
#define FST_VECTOR 1
#define SAT_VECTOR 1
#define PTR_VECTOR 2
#define E_VECTOR 3
#define S_VECTOR 4
#define CMD_VECTOR 5
#define FCN_VECTOR 6

/*
Maximum number of elements of data needed to define a change list entry.
continuous target vector element needs 2 (slope, intercept)
pidtau vector element needs 3 (tau_p, tau_d, tau_i)
step vector elements need 1
pointer into a specified parameter data block requires 2
*/
#define MAX_CHANGE_DATA (4)

/*
------------------------------------------------------------------------------
Memory allocation.
------------------------------------------------------------------------------
*/

/*
In-line macro which combines the memory region number and the
block order index of a parameter data block into one argument
that is used in the put_param_block function calls.
*/
#define BLOCK_MEMORY_INDEX(MEMORY_REGION,BLOCK_ORDER_INDEX) \
  ((MEMORY_REGION<<16) + (BLOCK_ORDER_INDEX&0xffff))

/*
Index into various arrays that hold values relevant to the memory
regions created on the real time cpu.  The rtheap memory region
consists of two parts: noncacheable and cacheable.  Other types 
of memory regions include generic memory and shared memory.
Installation specific types of memory (e.g., DMA or reflective)
should be defined using MEMORY_INSTALL_SPECIFIC.
*/
#define NONCACHEABLE 0
#define CACHEABLE 1

#define ADDR_RT 0
#define ADDR_HOST 1
/*
Mask bits describing a memory region.
The first 8 bits give the master cpu that controls
the initialization of the shared memory.
*/
#define MEMORY_CONTIGUOUS 256  /* memory regions for cpus follow each other */
#define MEMORY_NO_SFILE 512    /* don't include memory region in the S file */
#define MEMORY_NO_DELETE 1024  /* don't delete memory region */
/*
Types of memory regions
*/
#define MEMORY_NONCACHEABLE 0
#define MEMORY_CACHEABLE 1
#define MEMORY_GENERIC 2
#define MEMORY_SHAREABLE 3
#define MEMORY_INSTALL_SPECIFIC 4

#define RT_MEM_BLOCK_COUNT 1  /* 1 default block, cacheable & noncacheable,
                                 in the real time cpu's memory. */
/*
Classifications of memory usage in the real time cpus.  These codes
are used by getscmem to keep track of how much memory is devoted to
the various purposes for diagnostic use.  Each memory region has a
set of memory usage.
*/
#define MU_TOTAL 0         /* total memory for memory region */
#define MU_RAW 1           /* raw data dma buffer */
#define MU_PROC 2          /* storage of processed data vs time. */
#define MU_CHANGE 3        /* change list data */
#define MU_RTHEAP 4        /* basic rt heap pointer structure */
#define MU_CATARR 5        /* arrays with one element per category */
#define MU_STRUCTURES 6    /* basic data structures */
#define MU_PIDLOOKUP 7     /* pid lookup tables */
#define MU_PHASE 8         /* phase and phase sequence descriptors and 
                              associated data */
#define MU_FAST 9          /* fast sampled processed data. */
#define MU_PARAMETER 10    /* parameter data */
#define MU_SCRATCH 11      /* scratch memory */
#define MU_MESSAGE 12      /* message memory */

#define MU_TYPE_COUNT 13   /* number of categories of memory use. */

/*
Different parts of the memory.  Each is assigned to either the
cacheable or noncacheable region by calling the function
"set_cache_preference".  The installation can call this function
(usually in a parameters routine) to override the defaults.
Also, each of the first groups of sections can be put into a 
separate memory region which in theory can be shared between
processors in the same box or between cpus using shared memory
like reflective memory.
*/
/*
Likely candidates to be put into separate memory region since
the contents can be updated by other cpus.  Note that the 
parameter data can be put in any memory region on a block-by-block
basis using the BLOCK_MEMORY_INDEX macro to specify the region and
the block order index in the calls to the parameter data functions.
*/
#define RTPARAMETERS_PREFERENCE 0
#define INSTALL_BUFFER_PREFERENCE 1
#define TIMETOSTOP_FLAG_PREFERENCE 2
#define ADCOMBUFFER_PREFERENCE 3

/*
These sections could be shared by multiple cpus in the
same box using a system shared memory region.
NOTES of caution:
The cpus that read the contents would need to wait
until the cpu that writes has finished writing - and the
contents could change mid-cycle on the cpus that only read.
*/
#define BASELINE_PREFERENCE 4
#define DATAVECTOR_PREFERENCE 5
#define PHYSICSVECTOR_PREFERENCE 6

#define ADCOMBUFLOOP_PREFERENCE 7

/*
Unlikely candidates, but allowed (note that these are all
two-dimensional arrays: [max_samples][size of vector]
*/
#define PIDLOOKUP_PREFERENCE 8
#define ADTARGET_PREFERENCE 9
#define POINTER_TARGET_PREFERENCE 10
#define PIDTAUVECTOR_PREFERENCE 11
#define DMABUFFER_PREFERENCE 12
#define SHAPEVECTOR_PREFERENCE 13
#define ERRORVECTOR_PREFERENCE 14
#define FPCOMMAND_PREFERENCE 15
#define INTCOMMAND_PREFERENCE 16
#define PVECTOR_PREFERENCE 17
#define ALLDATA_PREFERENCE 18
#define RTMSG_LOG_PREFERENCE 19

/*
The following sections CANNOT be put into a separate memory
region - they must go in the rtmemory, but they can be
put into either the cacheable or noncacheable regions.
*/
#define RTHEAP_PREFERENCE 21  /* keep as first one that cannot be moved */
#define FUNCTIONVECTOR_PREFERENCE 22
#define MEMORY_REGIONS_PREFERENCE 23
#define ADDELTAT_PREFERENCE 24
#define ADMB_PREFERENCE 25
#define VERTEXTIME_PREFERENCE 26
#define PHSSEQ_STACK_PREFERENCE 27
#define PHASE_STACK_PREFERENCE 28
#define CURRENT_PHS_SEQ_PREFERENCE 29
#define CURRENT_PHS_SEQ_NUMBER_PREFERENCE 30
#define CURRENT_PHASE_PREFERENCE 31
#define PHASE_INIT_FUNCTION_PREFERENCE 32
#define CURRENT_PARAMDATA_PREFERENCE 33
#define CHANGE_CACHE_PREFERENCE 34
#define TARGETVECTOR_MAP_PREFERENCE 35
#define CATTARGETCOUNT_PREFERENCE 36
#define PHSSEQ_COUNT_PREFERENCE 37
#define PHASE_COUNT_PREFERENCE 38
#define PHSSEQENTRIES_PREFERENCE 39
#define PHASEENTRIES_PREFERENCE 40
#define INITIAL_MB_PREFERENCE 41
#define INITIAL_CT_PREFERENCE 42
#define PHASECHANGES_PREFERENCE 43
#define SEQCHANGES_PREFERENCE 44
#define OTHER_CPUS_PREFERENCE 45
#define RTMESSAGES_PREFERENCE 46

/* OBSOLETE */
#define RTSCRATCH_PREFERENCE 49

#define LAST_INT_PREFERENCE 50 /* always last */
#define NUM_PREFERENCES (LAST_INT_PREFERENCE+1)

/*
Flag bits for each preference for each region.
*/
#define MEMORY_OVERLAY 1    /* overlay region, i.e., use same pointer */

/*
Mask bits describing a realtime message.
*/
#define RTMESSAGE_NOSEND_BUFFER 1   /* no send buffer needed - 
                                       normally space for the send
                                       buffer is allocated which is
                                       the same size as the buffer 
                                       that receives the message */
#define RTMESSAGE_WRITE_READ_PAIR 2 /* create two buffers on dest -
                                       one to write to and one to read
                                       from; at end of the cycle the
                                       content of the write buffer is
                                       copied to the read buffer */
#define RTMESSAGE_SWING_BUFFER 4    /* use two buffers on destination -
                                       each will be written to and
                                       the reader will always get 
                                       the last buffer written to 
                                       even if it was returned earlier;
                                       can specify more than 2 buffers */
#define RTMESSAGE_RETURN_LATEST 8   /* always return latest message
                                       in multiple buffers - if no
                                       new message, then return NULL */
#define RTMESSAGE_WAIT_FOR_DATA 16  /* reader will always wait for
                                       data to arrive; a timeout must
                                       be given in case data does not
                                       arrive */
#define RTMESSAGE_NOCOUNTERS 32     /* do not add counters to buffer;
                                       normally a counter is added in
                                       front of the message and at the
                                       end; the end counter changing
                                       indicates the message has arrived,
                                       the beginning counter is used to
                                       make sure the message is not in
                                       the process of being written */
#define RTMESSAGE_SHARE_BUFFERS 64  /* overlay read and write buffers -
                                       useful for messages that are 
                                       written to shared memory, just
                                       fill the write buffer and 
                                       "the message" has arrived. */

/*
------------------------------------------------------------------------------
Virtual and physical cpu numbers.

Virtual cpu numbers are used to indicate in a redefinable manner
the particular cpu when writing algorithm and category code.

Physical cpu numbers refer to the actual real time cpu boards.
------------------------------------------------------------------------------
*/
#define CPUA   1
#define CPUB   2
#define CPUC   3
#define CPUD   4
#define CPUE   5
#define CPUF   6
#define CPUG   7
#define CPUH   8
#define CPUI   9
#define CPUJ  10
#define CPUK  11
#define CPUL  12
#define CPUM  13
#define CPUN  14
#define CPUO  15
#define CPUP  16
#define CPUQ  17
#define CPUR  18
#define CPUS  19
#define CPUT  20
#define CPUU  21
#define CPUV  22
#define CPUW  23
#define CPUX  24

#define CPU1   1
#define CPU2   2
#define CPU3   3
#define CPU4   4
#define CPU5   5
#define CPU6   6
#define CPU7   7
#define CPU8   8
#define CPU9   9
#define CPU10 10
#define CPU11 11
#define CPU12 12
#define CPU13 13
#define CPU14 14
#define CPU15 15
#define CPU16 16
#define CPU17 17
#define CPU18 18
#define CPU19 19
#define CPU20 20
#define CPU21 21
#define CPU22 22
#define CPU23 23
#define CPU24 24

/*
------------------------------------------------------------------------------
Definitions primarily used by host_realtime
------------------------------------------------------------------------------
*/
#define NUMRTFILES 2
#define SFILE 0
#define RFILE 1
#define HOST_LENGTH 41  /* one greater than the max number of chars in the
                           name of a host for the lock or wave server. 
                           IMPORTANT NOTE: This
                           value must match the value used by idl routines
                           in the user interface that start up server 
                           processes */
#define TESTFILENAME "s\0\0\0\0\0\0\0\0\0\0\0"

/*
------------------------------------------------------------------------------
Return status from the realtime code.
------------------------------------------------------------------------------
*/
#define NORMAL_RETURN 0    /* no errors */
#define ABORT_RETURN  2    /* return value from real time code
                             when the shot aborts before trigger. */
#define ALTERSEQUENCEBUFFERSFULL 3 /* return value when the alter phase or alter
                              phase sequence buffer fills before the
                              discharge is complete. */
#define ALTERPHASEBUFFERSFULL 4 /* return value when the alter phase or alter
                              phase sequence buffer fills before the
                              discharge is complete. */
#define NOSEQUENCESTACKENTRY 5 /*return value when the previous phase
			       sequence cannot be located in the phase
			       sequence stack.*/
#define TIMETOSTOP_RETURN 6 /* normal return */
#define FAULT_RETURN 7 /* fault occurred after trigger */
#define TIMEOUT_RETURN 8 /* timeout occurred */

/*
------------------------------------------------------------------------------
shot state in realtime
------------------------------------------------------------------------------
*/
#define RTPRESHOT 1
#define RTDURINGSHOT 2
#define RTPOSTSHOT 3

/*
------------------------------------------------------------------------------
Definitions primarily used by waveserver
------------------------------------------------------------------------------
*/
#define QUEUEBUSY 1 /* indicates to the client that the queue handler
                       is busy. */
#define QUEUEIDLE 0 /* indicates to the client tht the queue handler
                       is idle. */
#define PERMANENT 1 /* indicates a phase cannot be deleted. */
#define TEMPORARY 0 /* indicates that a phase can be deleted. */
#define DOIO 0L       /* Alter the io handler's version of the data. */
#define DOQUEUE 1L    /* Alter the work queue's version of the data. */

#define FORUSER 1    /* return data for the user interface */
#define FORHOSTRTCPU 2    /* return data for the host_realtime routine. */
#define FORHOSTPARAM 3 /* return data for the host_realtime routine. */
#define FORALL 4     /* return data for ALL blocks for the user interface */

/*
Standard descriptions for labeled sets of parameter data blocks.
*/
#define SD_ACCESS_CONTROL      "StaticData: Access Control"
#define SD_LABELED_MATRICES    "StaticData: Labeled Matrices"
#define SD_LABELED_INT_ARRAY   "StaticData: Labeled Integer Array"
#define SD_LABELED_FLOAT_ARRAY "StaticData: Labeled Float Array"
#define SD_LABELED_STRUCTURE   "StaticData: Labeled Structure"

#define DOVAXFILES 1L /* Flag to specify that the vax archive files are to
                         be written. */
#define DOTARGETS 0L   /* Flag to specify that the target vectors should be
                          computed. */
#define LABEL_LENGTH 30 /* length of the shot label string
			   Dependencies:
			   a) In control.h the shot_label global variable
			 must be initialized with at least
			 LABEL_LENGTH  + 1 characters.
			 b) The IDL routine getaccess.pro limits the
			 shot_label
			 length so it needs to know the value of
			 LABEL_LENGTH.
			 c) The IDL routine initlabel.pro 
			 needs to know the value of
			 LABEL_LENGTH.*/
#define SHOTFILEVERSION 1 /* Version number of the shot archive file. */
#define BLANKS12 "            "
#define BLANKS24 "                        "
#define WAVERTVOLTSPERBIT (200.0f/65536.0f)

/*
Periodic action flags (bitmask).
*/
#define ACTION_DO_AT_LOCKOUT 1           /* do action at first lockout */
#define ACTION_DO_NOT_RECORD_CHANGE 2    /* do not record change to history
                                            file and notify users */
#define ACTION_DO_ONLY_FOR_IO_COPY 4     /* execute action function
                                            only for the IO queue */
#define ACTION_DO_ONLY_FOR_PROCESSED_COPY 8  /* execute action function
                                                only for the WORK queue */
/*
Periodic action calling flags.
*/
#define ACTION_CALLED_FOR_RAWDATA 1  /* action called due to raw data change */
#define ACTION_CALLED_AT_LOCKOUT  2  /* action called at first lockout */
#define ACTION_CALLED_AT_INTERVAL 3  /* action called at periodic time */

/*
Name of the pointname used to store the pcs setup.
*/
#define pcs_setup_pointname "PCSSETUP"

/*
Maximum number of real time computers.
*/
#define MAX_VCPUS (24 + 1)  /* Maximum number of "virtual" cpus that
                              a category can utilize. The extra is provided
                              so that there is space for the terminating
                              zero in some arrays. */
#define MAX_PCPUS (24)      /* Maximum number of "physical" cpus that
                              can be defined.  This macro is used to
                              dimension arrays that hold information
                              about each physical CPU. */
/*
Maximum number of target vectors that can be affected by a single waveform
(plus one to allow for a terminating 0).  This is also set in structuredefs.pro.
*/
#define MAX_TARGETS_AFFECTED (100+1)

/*
Maximum number of discrete y value grid points that can be defined for
a waveform.  Must be at least 2.  This is also set in structuredefs.pro.
*/
#define MAX_WAVEFORM_GRIDPOINTS 20

/*
Size of pointname character array in archive_vector_info structure
and size of character arrays used for pointnames.  The installation
can provide its absolute maximum pointname size through the shared
variable POINTNAME_SIZE.
*/
#define MAX_POINTNAME_SIZE 20

/*
The minimum amount of space allocated on the real time cpu
for the record of phase sequence and phase
changes.
*/
#define MIN_ALTERPHSSEQ_COUNT 500
#define MIN_ALTERPHASE_COUNT 500

/*
Maximum number of cached restore structures.
*/
#define MAX_CACHE 2

/*
Minimum allowed values for time constants used in the pid calculations.
The values specified on time constant waveforms are compared to these values
in the function check_pidtau_values.  This function looks for values that are
too small. If the time constants are too much smaller than the control cycle
time, the digital filters used in the pid calculation can give nonsense
results.  The filter output can ring after a large step in the input value.

These macros are also used as the minimum Y axis values for PID tau waveforms.

Units: tauP, tauD = milliseconds
       tauI = seconds
*/
/* xxx temporarily set to 0, until we automatically fix the
   raw data errors that would result otherwise:
#define MIN_PID_TAUP_VALUE 0.1
#define MIN_PID_TAUD_VALUE 0.1
#define MIN_PID_TAUI_VALUE 1.0e-4
*/
#define MIN_PID_TAUP_VALUE 0.0
#define MIN_PID_TAUD_VALUE 0.0
#define MIN_PID_TAUI_VALUE 0.0

/*
Entrybehavior codes for shot phases and phase sequences.
These three START_AT_ ... flags must match the usage in timecrit.s
*/
#define START_AT_BEGINNING 1 /* entrybehavior flag value*/
#define START_WHERE_LEFT_OFF 2 /* entrybehavior flag value*/
#define START_AT_EVOLVED_TIME 3 /* entrybehavior flag value*/
#define START_AT_ANCHOR_AND_EVOLVED_TIME 4 /* entrybehavior flag value*/

/*
Access control mask codes for categories, shot phases, and data items.
*/
#define AC_NORESTORE 1 /* access control mask value*/
#define AC_NOWORLD_WRITE 2 /* access control mask value*/

#define AC_INITIALIZE 1 /* flag for check_access_control */

/*
Archive_mask values used in the vector_info structure.
xxx note that this is used in selected areas of write_vector routines.
*/
#define NOARCHIVE 0
#define ARCHIVE_FLOAT 1
#define ARCHIVE_INT 2
/*
Definitions used to maintain the list of raw data problems.
*/
#define ADD_RAW_DATA_PROBLEM 1 /*  Flag to indicate that a problem exists and 
                                  should be added to the list. */
#define REMOVE_RAW_DATA_PROBLEM 2  /*Flag to indicate that the particular
                                    problem does not exist and all
                                    references to that problem should be 
				    removed from the list.*/
#define APPEND_RAW_DATA_PROBLEM 3 /*  Flag to indicate that 
                                  a problem exists and 
                                  should be added to the list. */
#define RAW_PROBLEM_TEXT_LENGTH_LIMIT 1024 /*  The maximum number of characters
                                    that can be in the text string describing
                                    a raw data problem.*/
#define RAW_PROBLEM_WARNING 1 /* Indicates a raw data problem which is just
                                a warning of a bad condition.*/
#define RAW_PROBLEM_FATAL 2 /* Indicates a raw data problem which is so bad
                              that it will cause failure of the shot.*/
/*
------------------------------------------------------------------------------
waveserver message types

Note that these message type macros must be coordinated with the
values used in the IDL user interface code since that code cannot simply
include this file.
------------------------------------------------------------------------------
*/
#define SHUTDOWN_WAVESERVER 911                 /* message type - no reply */
#define REMOVE_USER_INFO 912                   	/* message type - no reply */

#define DATA_REQUEST 1                          /* message type */
#define CHANGE_PHASETABLE 2                     /* message type */
#define NEW_VERTICES 3                          /* message type */
#define LOCKOUT_IN 4                            /* message type */
#define UNLOCK_IN 5                             /* message type */
#define ENDOFSHOT 6                             /* message type */
#define WRITEVAXDATA 7                          /* message type */
#define NEW_PARAMETERDATA 8                     /* message type */
#define LOCKSERVER_INFO 9                       /* message type */
#define DELETE_SHOTPHASE 10                     /* message type */
#define RESTORE_PARAMETERDATA 11                /* message type */
#define BLOCK_MESSAGE 12                        /* message type */
#define SIMSERVER_INFO 13                       /* message type */
#define CHANGE_CATEGORY 14                      /* message type */
#define QUEUED_DATA_REQUEST 15                  /* message type */
#define RESTORE_REQUEST 16                    	/* message type */
#define ADD_USER_INFO 17                    	/* message type */
#define FLUSH_RESTORE_CACHE 18                 	/* message type */
#define EXECUTE_COMMAND 19                	/* message type */
#define SHIFT_PHASE 20                		/* message type */
#define START_OF_CHANGE 21             		/* message type */
#define END_OF_CHANGE 22             		/* message type */
#define PERIODIC_ACTION 23             		/* message type */

/*
Special message types from host_realtime
*/
#define CPU_NUMBER 101                          /* message type */

#define CPU_COUNT 1                                 /* data type */
#define WAVEFORM_STRUCTURE_ARRAY 2                  /* data type */
#define CATEGORIES_ALGORITHMS 3                     /* data type */
#define PHASE_TABLE 4                               /* data type */
#define VERTEX_SET 5                                /* data type */
#define SHOT_LABEL 6                                /* data type */
#define PARAMETER_DATA 7                            /* data type */
#define LOCKOUT_STATE 8                             /* data type */
#define STEP_TARGET_VECTORS 9                       /* data type */
#define STRUCTURENAMES 10			    /* data type */
#define STARTTIMES 11                               /* data type */
#define PHASE_NAME 12                               /* data type */
#define WAVE_SETTINGS 13                            /* data type */
#define CATEGORY_ACCESS_CONTROL 14                  /* data type */
#define LOCKOUT_FLAG 15                             /* data type */
#define HOST_INFO 20                                /* data type */
#define WHICH_CATEGORIES 21                         /* data type */
#define WHICH_VIRTUAL_CPU 22                        /* data type */
#define RT_HEAP_MISC 23                             /* data type */
#define CPUHEAP 24                                  /* data type */
#define TARGETVECTOR_MAP_ARRAY 25                   /* data type */
#define CATTARGETCOUNT 26                           /* data type */
#define PHSSEQENTRIES_STRUCT 27                     /* data type */
#define CHANGELISTS 28                              /* data type */
#define PHASEENTRIES_STRUCT 29                      /* data type */
#define PHASECHANGELISTS 30                         /* data type */
#define EXTRA_FILE_INFO 31			    /* data type */
#define OTHER_CPU_INFO 32			    /* data type */
#define LS_PORT_HOST 33			    	    /* data type */
#define SS_PORT_HOST 34			    	    /* data type */
#define WV_TIMETOSTOP 35			    /* data type */
#define VECTOR_INFO 36			    	    /* data type */
#define SAVE_CODE_INFO 37			    /* data type */
#define SAVE_CODE_POINTNAMES 38			    /* data type */
#define INSTALLATION_DATA 39			    /* data type */
#define CAT_NAMES_LIST 40			    /* data type */
#define ALG_NAMES_LIST 41			    /* data type */
#define TYP_NAMES_LIST 42			    /* data type */
#define PNT_NAMES_LIST 43			    /* data type */
#define RESTORE_STRUCT 44			    /* data type */
#define PRINT_DEBUG_DATA 45			    /* data type */
#define RAWDATA_VECTOR_INFO 46			    /* data type */
#define DIODATA_VECTOR_INFO 47			    /* data type */
#define HOST_NAMES_LIST 48			    /* data type */
#define RESTORE_CHANGE_HISTORY 49		    /* data type */
#define MSG_PORT_HOST 50			    /* data type */
#define PHSSEQ_INFO 51			            /* data type */
#define PHSSEQ_IDS 52			            /* data type */
#define PHASE_ID 53			            /* data type */
#define PHASE_STACK_DATA 54		            /* data type */
#define PHSSEQ_STACK_DATA 55		            /* data type */
#define MEMORY_REGIONS_DATA 56		            /* data type */
#define WHICH_MEMORY_REGION 57		            /* data type */
#define RTMESSAGE_DATA 58		            /* data type */

#define DATSTR_ERROR 1				/* data structure index */
#define DATSTR_SUBSETS 2			/* data structure index */
#define DATSTR_POINTNAMES 3	                /* data structure index */
#define DATSTR_TARGET 4	                	/* data structure index */
#define DATSTR_SHAPE 5	                  	/* data structure index */
#define DATSTR_FUNCTION 6	                /* data structure index */
#define DATSTR_SAVEROUTINES 7	                /* data structure index */
#define DATSTR_STDPOINTNAMES 8	                /* data structure index */

#define PHASE_CHANGELIST_DATA  1                /* debug data type */
#define PHSSEQ_CHANGELIST_DATA 2                /* debug data type */
#define PRINT_MEMORY_USAGE     3                /* debug data type */
#define GETSCMEM_PRINT         4                /* debug data type */
#define RAW_PROBLEM_MESSAGES   6                /* debug data type */
#define PHASE_PARAM_DATA_SUMMARY 7              /* debug data type */
#define PHASE_PARAM_DATA       8                /* debug data type */
#define FUNCTIONS_LIST         9                /* debug data type */
#define PHASE_VERTICES        10                /* debug data type */
#define PRINT_USER_INFO       11                /* debug data type */
#define PRINT_HISTORY         12                /* debug data type */
#define PRINT_SETUP_CHANGES   13                /* debug data type */
#define PRINT_CPU_INFO        14                /* debug data type */

/*
------------------------------------------------------------------------------
Here are the codes to identify the data structure to be changed by a change
list item.
------------------------------------------------------------------------------
*/
#define INITIAL_CT 1              /* Initial value of Continuous target 
				     vector element */
#define CT_CHANGE 2               /* Continuous target vector element */
#define FST_CHANGE 3              /* Float step target vector element */
#define IST_CHANGE 4              /* integer step target vector element */
#define PIDTAU_CHANGE 5           /* pidtau array */

#define PTRT_SCRATCH_CHANGE 6     /* a pointer in the pointer target vector
				     to a location in the real time scratch
				     space */
#define PTRT_PARAMBLOCK_CHANGE 7  /* a pointer in the pointer target vector
				     to a location in a
				     block of parameter data */
#define PTR_PARAM_CHANGE 8        /* OBSOLETE: 
				     pointer to part of the parameter data */

#define FUNCTION_CHANGE 9         /* element of the function vector */

#define PHSSEQ_PTR_CHANGE 10      /* OBSOLETE:
				     pointer to a phase sequence descriptor */
#define PTR_SCRATCH_CHANGE 11     /* OBSOLETE:
				     a pointer in an integer step target 
				     vector element to a location in the 
				     real time scratch space */
#define PTR_PARAMBLOCK_CHANGE 12  /* OBSOLETE:
				     a pointer in an integer step target 
				     vector element to a location in a
				     block of  parameter data */

/*
------------------------------------------------------------------------------
Definitions primarily for the use of the lock server
------------------------------------------------------------------------------
*/
#define SHUTDOWN_LOCKSERVER 911         /* message type */
#define SET_UNLOCK 1            	/* message type */
#define STATE_QUESTION 2        	/* message type */
#define SHOT_FINISHED 3         	/* message type */
#define SET_FIRSTLOCK 4         	/* message type */
#define SET_FINALLOCK 5         	/* message type */
#define ABORT_SHOT 6            	/* message type */
#define SET_RELOCK 7            	/* message type */
#define SHOT_FINISHED_AND_ABORTED 8   	/* message type */
#define ARCHIVE_FILES_WRITTEN 9       	/* message type */
#define SHOTSETUP_COMPLETE 10          	/* message type */
#define START_TEST_SHOT 11            	/* message type */
#define REALTIME_ROUTINE_STARTING 12    /* message type */
#define PING_FROM_NEXTSHOT_UI 13	/* message type */
#define GET_DATE_TIME 14	        /* message type */
#define SEND_MEMORY_ADDRESSES 15	/* message type */
#define GET_MEMORY_ADDRESSES 16		/* message type */

#define LS_FIRST_INSTALL_MSG 100	/* start of message type codes
					   available for installation use */

#define NULLSTATE 1             /* process state */
#define FIRSTLOCKOUT 2          /* process state */
#define FINALLOCKOUT 3          /* process state */
#define UNLOCKED 4              /* process state */
#define DURINGSHOT 5            /* process state */
#define ABORTED 6               /* process state */
#define WRITINGFILES 7          /* process state */

#define WAVESERVER_WRITEFILE (-1)  /* "cpu number" to accompany the
                                      ARCHIVE_FILES_WRITTEN message
                                      to the lockserver from the waveserver.*/

/*
------------------------------------------------------------------------------
Definitions primarily for the use of the message server
------------------------------------------------------------------------------
*/
#define SHUTDOWN_MSGSERVER 911         /* message type */

/*
==============================================================================
SECTION: general structure definitions
==============================================================================
*/
/*
------------------------------------------------------------------------------
Type definition: rtheap_offset

A variable of this type contains a byte offset into the area of memory
on the real time processor that is allocated for the  PCS memory heap.
This variable type is used so that this type of offset value is easily
identifiable.  An rtheap_offset value is converted on the real time processor
to a pointer.

This offset type is defined as a signed integer to allow for negative offsets
to be specified.
------------------------------------------------------------------------------
*/
#define GEN_rtheap_offset \
DSTART(                                              D_rtheap_offset)\
DGEN  (typedef int rtheap_offset;           ,INTTYPE, 1)\
DEND
GEN_rtheap_offset

/*
------------------------------------------------------------------------------
STRUCTURE: raw_data_problem_desc
------------------------------------------------------------------------------
*/
struct raw_data_problem_desc {
                          int message_at_fault; /*  Message identifier of
                                           the message that first caused this
                                           problem to appear.*/
                          int category; /*The category in which the raw data
                                          problem is located.*/
                          int number; /*The index of the phase in which the
                                        raw data problem is located.*/
                          char *type; /*A short character string identifying
                                        the particular problem.*/
                          char *text; /*A string describing the problem to the
                                        user.*/
                          int text_length; /*The number of characters in the
                                          text string describing the problem.*/
                          int  severity;/*Indicates how bad the problem is.*/
                          struct raw_data_problem_desc *next;
                          struct raw_data_problem_desc *previous;
                         };
/*
------------------------------------------------------------------------------
STRUCTURE: rtmessage
------------------------------------------------------------------------------
*/
#ifndef MAX_RTMESSAGE
#define MAX_RTMESSAGE 32
#endif

#define GEN_rtmessage \
SDSTART(struct rtmessage {                         ,D_rtmessage)\
DGEN   (             char name[MAX_RTMESSAGE];     ,CHARTYPE,MAX_RTMESSAGE   )\
DGEN   (                       int source_cpu;     ,INTTYPE,1     )\
DGEN   (                       int dest_cpu;       ,INTTYPE,1     )\
DGEN   (                       int size;           ,INTTYPE,1     )\
DGEN   (                       int buffer_count;   ,INTTYPE,1     )\
DGEN   (                       int category;       ,INTTYPE,1     )\
DGEN   (                       int phase_number;   ,INTTYPE,1     )\
DGEN   (                       int sendbuf_memory_region; ,INTTYPE,1     )\
DGEN   (                       int dest_memory_region;    ,INTTYPE,1     )\
DGEN   (                       int flags;          ,INTTYPE,1     )\
DGEN   (                       int timeout;        ,INTTYPE,1     )\
DGEN   (                       int used;           ,INTTYPE,1     )\
NDGEN  (         rtheap_offset sendbuf_offset; ,NESTEDTYPE,1,D_rtheap_offset)\
NDGEN  (         rtheap_offset dest_offset;    ,NESTEDTYPE,1,D_rtheap_offset)\
SDEND  (                    };                                    )
GEN_rtmessage

/*
------------------------------------------------------------------------------
STRUCTURE: memory_region
------------------------------------------------------------------------------
*/
#define GEN_memory_region \
SDSTART(struct memory_region {                     ,D_memory_region)\
DGEN   (                       int size;           ,INTTYPE,1     )\
DGEN   (                       int type;           ,INTTYPE,1     )\
DGEN   (                       int flag;           ,INTTYPE,1     )\
DGEN   (                       int key;            ,INTTYPE,1     )\
SDEND  (                    };                                    )
GEN_memory_region

/*
------------------------------------------------------------------------------
STRUCTURE: data_request
------------------------------------------------------------------------------
*/
#define GEN_data_request \
SDSTART(struct data_request {                      ,D_data_request)\
DGEN   (                       int message_type;   ,INTTYPE,1     )\
DGEN   (                       int data_type;      ,INTTYPE,1     )\
DGEN   (                       int identifier1;    ,INTTYPE,1     )\
DGEN   (                       int identifier2;    ,INTTYPE,1     )\
DGEN   (                       int identifier3;    ,INTTYPE,1     )\
DGEN   (                       int identifier4;    ,INTTYPE,1     )\
DGEN   (                       int identifier5;    ,INTTYPE,1     )\
SDEND  (                    };                                    )
GEN_data_request

/*
------------------------------------------------------------------------------
STRUCTURE: cat_alg_str
Must match what the waveserver sends for the data request:
CATEGORIES_ALGORITHMS
------------------------------------------------------------------------------
*/
struct cat_alg_str {
    int category_index,algorithm_index,version_index;
    char *name;
    char *parampro;
    char *identifier;
    struct cat_alg_str *next_str;
};

/*
------------------------------------------------------------------------------
STRUCTURE: phase_struct_str
Must match what the waveserver sends for the data request:
PHASE_TABLE
------------------------------------------------------------------------------
*/
struct phase_struct_str {
    int number;
    int category;
    int algorithm;
    int entry_behavior;
    int permanent;
    float anchor_time;
    char *name;
    struct phase_struct_str *next_str;
};

/*
------------------------------------------------------------------------------
STRUCTURE: waveform_str
Must match what the waveserver sends for the data request:
WAVEFORM_STRUCTURE_ARRAY
------------------------------------------------------------------------------
*/
struct waveform_str {
    float scale[4];
    char  *name;
    char  *xunits;
    char  *yunits;
    char  *restore_point;
    float restore_scale;
    float restore_offset;
    char  *archive_point;
    char  *description;
    struct waveform_str *next_str;
};

/*
------------------------------------------------------------------------------
STRUCTURE: indexed_waveform
Structure used for nongridded waveforms that have "/LabelsBlock"
in their yunits string.
Note that the character strings have to be a set size since
they are embedded in a structure.
------------------------------------------------------------------------------
*/
#define GEN_indexed_waveform \
SDSTART(struct indexed_waveform {                      ,D_indexed_waveform) \
DGEN   (                         char ylabel[64];      ,CHARTYPE,64   )\
DGEN   (                         char description[64]; ,CHARTYPE,64   )\
DGEN   (                         float yscales[2];     ,FLOATTYPE,2   )\
DGEN   (                         float ymin;           ,FLOATTYPE,1   )\
DGEN   (                         float ymax;           ,FLOATTYPE,1   )\
SDEND  (                         };                                   )
GEN_indexed_waveform
/*
------------------------------------------------------------------------------
STRUCTURE: vertices_str
Must match what the waveserver sends for the data request:
VERTEX_SET
------------------------------------------------------------------------------
*/
struct vertices_str {
    float x,y;
    struct vertices_str *next_str;
};

/*
------------------------------------------------------------------------------
STRUCTURE: phasetable_return
Structure used in returning the phase table info.
See also structure phase_struct_str which is the same except it has a name.
------------------------------------------------------------------------------
*/
#define GEN_phasetable_return \
SDSTART(struct phasetable_return {                     ,D_phasetable_return) \
DGEN   (                         int number;           ,INTTYPE,1     )\
DGEN   (                         int category;         ,INTTYPE,1     )\
DGEN   (                         int algorithm;        ,INTTYPE,1     )\
DGEN   (                         int entrybehavior;    ,INTTYPE,1     )\
DGEN   (                         int permanent;        ,INTTYPE,1     )\
DGEN   (                         float anchor_time;    ,FLOATTYPE,1   )\
SDEND  (                         };                                   )
GEN_phasetable_return
/*
------------------------------------------------------------------------------
STRUCTURE: change_phasetable
------------------------------------------------------------------------------
*/
#define GEN_change_phasetable \
SDSTART(struct change_phasetable {                     ,D_change_phasetable)\
DGEN   (                       int message_type;           ,INTTYPE,1     )\
DGEN   (                       int userid;                 ,INTTYPE,1     )\
DGEN   (                       int category;               ,INTTYPE,1     )\
DGEN   (                       int number;                 ,INTTYPE,1     )\
DGEN   (                       int algorithm;              ,INTTYPE,1     )\
DGEN   (                       int entrybehavior;          ,INTTYPE,1     )\
DGEN   (                       int access_control;         ,INTTYPE,1     )\
DGEN   (                       float anchor_time;          ,FLOATTYPE,1   )\
SDEND  (                    };   )
GEN_change_phasetable
/*
------------------------------------------------------------------------------
STRUCTURE: shift_phase
------------------------------------------------------------------------------
*/
#define GEN_shift_phase \
SDSTART(struct shift_phase {                               ,D_shift_phase)\
DGEN   (                       int message_type;           ,INTTYPE,1     )\
DGEN   (                       int userid;                 ,INTTYPE,1     )\
DGEN   (                       int category;               ,INTTYPE,1     )\
DGEN   (                       int number;                 ,INTTYPE,1     )\
DGEN   (                       float xshift;               ,FLOATTYPE,1   )\
SDEND  (                    };   )
GEN_shift_phase
/*
------------------------------------------------------------------------------
STRUCTURE: new_vertices
------------------------------------------------------------------------------
*/
#define GEN_new_vertices \
SDSTART(struct new_vertices {                               ,D_new_vertices)\
DGEN   (                    int message_type;               ,INTTYPE,1     )\
DGEN   (                    int userid;                     ,INTTYPE,1     )\
DGEN   (                    int category;                   ,INTTYPE,1     )\
DGEN   (                    int algorithm;                  ,INTTYPE,1     )\
DGEN   (                    int subset;                     ,INTTYPE,1     )\
DGEN   (                    int windex;                     ,INTTYPE,1     )\
DGEN   (                    int phase;                      ,INTTYPE,1     )\
DGEN   (                    int n;                          ,INTTYPE,1     )\
SDEND  (                     };                                            )
GEN_new_vertices
/*
------------------------------------------------------------------------------
STRUCTURE: change_parameterdata
structure used by a NEW_PARAMETERDATA message 
------------------------------------------------------------------------------
*/
#define GEN_change_parameterdata \
SDSTART(struct change_parameterdata {                 ,D_change_parameterdata)\
DGEN   (                  int message_type;               ,INTTYPE,1     )\
DGEN   (                  int userid;                     ,INTTYPE,1     )\
DGEN   (                  int number;                     ,INTTYPE,1     )\
DGEN   (                  int category;                   ,INTTYPE,1     )\
SDEND  (                    };                                           )
GEN_change_parameterdata
/*
------------------------------------------------------------------------------
STRUCTURE: delete_shotphase
------------------------------------------------------------------------------
*/
#define GEN_delete_shotphase \
SDSTART(struct delete_shotphase {                       ,D_delete_shotphase)\
DGEN   (                        int message_type;           ,INTTYPE,1     )\
DGEN   (                        int userid;                 ,INTTYPE,1     )\
DGEN   (                        int category;               ,INTTYPE,1     )\
DGEN   (                        int number;                 ,INTTYPE,1     )\
SDEND  (                        };                                         )
GEN_delete_shotphase
/*
------------------------------------------------------------------------------
STRUCTURE: vertex
Structure to hold the data at a waveform vertex.
------------------------------------------------------------------------------
*/
#define GEN_vertex \
SDSTART(struct vertex {               ,D_vertex)\
DGEN   (               float x;       ,FLOATTYPE,1)  /* The x axis value. */\
DGEN   (               float y;       ,FLOATTYPE,1)  /* The y axis value. */\
SDEND  (              };                                             )
GEN_vertex
/*
------------------------------------------------------------------------------
STRUCTURE: ivertex
Structure to hold the data at a waveform vertex in the case where the
y values are integers.
------------------------------------------------------------------------------
*/
#define GEN_ivertex \
SDSTART(struct ivertex {              ,D_ivertex)\
DGEN   (               float x;       ,FLOATTYPE,1)  /* The x axis value. */\
DGEN   (               int   y;       ,INTTYPE,1)    /* The y axis value. */\
SDEND  (              };                                             )
GEN_ivertex
/*
------------------------------------------------------------------------------
STRUCTURE: waveform_variable
Structure that hold the information for a waveform that can be altered
by the operator.  Other data, in struct waveform, is hardwired.
------------------------------------------------------------------------------
*/
struct waveform_variable {
                          struct vertex *data; /* An array giving the
                                                  data defining the vertices
                                                  of the waveform. */
                          int numvertex;       /* The number of vertices that
                                                  have been defined for the
                                                  waveform; i.e. the number
                                                  of entries in the array
                                                  pointed to by "data." */
                          int access_control;  /* Mask for access control */
                         };
/*
------------------------------------------------------------------------------
STRUCTURE: phase_changelist
Definition of an entry in the change list for a shot phase.
------------------------------------------------------------------------------
*/
#define GEN_phase_changelist \
SDSTART(struct phase_changelist {                ,D_phase_changelist)\
DGEN   (          unsigned int time;             ,INTTYPE,1) \
                                    /* Time to make the change, \
                                       in units\
                                       of shot phase clock ticks since the\
                                       start of the shot phase. */\
DGEN   (          int data_structure;            ,INTTYPE,1)  \
                                    /* Identifier for the \
                                       data structure being changed. */\
DGEN   (          int element;                   ,INTTYPE,1)   \
                                       /* Number of the element in the data\
                                          structure that is being changed. */\
DGEN   (          char *element_name;            ,POINTERTYPE,1)    \
					  /* Name of the data structure \
                                             element.*/\
DGEN   (          struct {                       ,STRUCTURETYPE,1)\
DGEN   (          int intdata[MAX_CHANGE_DATA];  ,INTTYPE,MAX_CHANGE_DATA)\
DGEN   (          float floatdata[MAX_CHANGE_DATA]; ,FLOATTYPE,MAX_CHANGE_DATA)\
DGEN   (                 } data;                 ,STRUCTUREEND,0 )\
                                         /* data needed to construct the\
                                            change list entry in the real \
                                            time computer memory. */\
DGEN   (          struct phase_changelist *next_change; ,POINTERTYPE,1) \
                                         /* pointer to\
                                            next change in the list */\
SDEND  (                        };   )
GEN_phase_changelist

/*
------------------------------------------------------------------------------
STRUCTURE: shotphase
Definition of a shot phase.
------------------------------------------------------------------------------
*/
struct shotphase {
                  int number;    /* The identifying index of the shot phase.
                                    A value unique to this shot phase that
                                    is used to identify the phase in, for
                                    instance, vertices of a waveform for 
                                    a shot phase sequence. */
                  int category;  /* The index of the category for which this
                                    shot phase is defined. */
                  char *name;    /* The name used by the operator to identify
                                    the shot phase. */
                  int algorithm; /* The control algorithm  used by this shot
                                    phase. */
                  int permanent; /* Set to 1 if this phase cannot be deleted.
                                    0 if it can be deleted. */
                  char *parameters;  /* Pointer to the parameter data defined
                                        for this shot phase. */
                  struct waveform_variable *wavelist;  /* An array of 
                                    structures that hold the variable data for
                                    each of the waveforms used by this shot
                                    phase. */
                  int first_waveform; /* The index in the "waveforms" array
                                    of fixed waveform data of the first 
                                    waveform used by the algorithm assigned
                                    to this shot phase. */
                  int numwaveforms;  /* The number of waveforms used by the
                                    algorithm assigned to this shot phase. */
                  int entrybehavior; /* An index indicating the behavior when
                                     this shot phase is started during a 
                                     discharge. */
                  float anchor_time; /* Time in seconds where a phase
                                     with an entry behavior of
                                     START_AT_ANCHOR_AND_EVOLVED_TIME will
                                     anchor its vertices. */
                  int access_control; /* A mask used for restoring and updating
                                     this shot phase. */
                  int check_access_control; /* A flag to tell routines
                                     whether to check access control before 
                                     operating on a data item. */
                  int whichqueue;       /* The queue the phase is in */
                  struct phase_changelist *changes[MAX_VCPUS]; /* The address
                                     of the head of the change list for this
                                     shot phase on each of the virtual cpus
                                     used by the category for which this is a
                                     shot phase. */
                  int change_count[MAX_VCPUS]; /* The number of entries in the
                                     change list for each of the virtual cpus.
                                              */
                  rtheap_offset *rtparameters[MAX_VCPUS]; /* Space to store the
                                                  offsets into each memory
						  region of the
                                                  space allocated on each of
                                                  the virtual cpus for
                                                  parameter data for this 
                                                  phase.
                                                  */
                  int *rtparamsize[MAX_VCPUS]; /* array of number of bytes 
                                           allocated in each memory region
                                           on each of the virtual 
                                           cpus for storage of parameter
                                           data. */
                  rtheap_offset rtscratch[MAX_VCPUS];  /* Space to store the
                                                  offsets into the real time
						  heap  memory area of the
                                                  space allocated on each of
                                                  the virtual cpus for
                                                  scratch data for this 
                                                  phase.
                                                  */
                  int rtscratchsize[MAX_VCPUS]; /* number of bytes allocated
                                           on each of the virtual cpus
                                           for storage of scratch data. */
                  struct periodic_action *actions; /* Pointer to periodic
                                            actions for the phase */
                  struct shotphase *nextphase; /* Pointer to the next
                                            shot phase descriptor in the
                                            linked list. */
                 };

/*
------------------------------------------------------------------------------
STRUCTURE: periodic_action
------------------------------------------------------------------------------
*/
struct periodic_action {
                  int interval;        /* interval in seconds if periodic */
                  int flags;           /* action flags */
                  char *data_item;     /* raw data item that triggers action */
                  int data_entry;      /* = 0 if executed in both queues,
                                          > 0 if only work queue */
                  int (*function)(int,struct shotphase *,int); /* Pointer
                                             to the function that is
                                             executed */
                  time_t last_done;    /* last time done */
                  int done_flag;       /* flag to indicate action was done */
                  struct periodic_action *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: hostshotphase
Definition of a host realtime computer shot phase.
------------------------------------------------------------------------------
*/
struct hostshotphase {
                  int category;  /* The index of the category for which this
                                    shot phase is defined. */
                  int algorithm; /* The control algorithm  used by this shot
                                    phase. */
                  int index;     /* index of the phase */
                  char *name;    /* name of the phase */
                  char *id;      /* identifier: catid,phsname,algid */
                  char *parameters; /* Host parameter data */
                 };
/*
------------------------------------------------------------------------------
STRUCTURE: hostphsseq
Definition of a host realtime computer shot phase sequence.
------------------------------------------------------------------------------
*/
struct hostphsseq {
                  int number;   /* Index of the phase sequence. */
                  int category; /* Category that the phase sequence belongs
                                   to. */
                  char *id;     /* Identifier: catid,phsseqname */
                 };
/*
------------------------------------------------------------------------------
STRUCTURE: default_shotphase
Structure to specify the list of default shot phases.
Note that the default algorithm is specified by its "identifier"
rather than its algorithm code to make this default specification 
independent of algorithm code assignment.
------------------------------------------------------------------------------
*/
struct default_shotphase {
                  int category;  /* The index of the category for which this
                                    shot phase is defined. */
                  char *name;    /* The name used by the operator to identify
                                    the shot phase. */
                  char *algident; /* The identifier for the control algorithm 
                                     used by this shot phase. */
                  int entrybehavior; /* An index indicating the behavior when
                                     this shot phase is started during a 
                                     discharge. */
                  int access_control; /* A mask used for restoring and updating
                                     this shot phase. */
                  float anchor_time; /* Time in seconds where a phase
                                     with an entry behavior of
                                     START_AT_ANCHOR_AND_EVOLVED_TIME will
                                     anchor its vertices. */
                         };
/*
------------------------------------------------------------------------------
STRUCTURE: phaseptrs
A structure to hold the addresses of the heads of the linked lists of
shot phase descriptors for a given category.  There are two lists,
one maintained by the io handler routine and one by the queue handler
routine. 
------------------------------------------------------------------------------
*/

struct phaseptrs {
                  struct shotphase *forqueue;  /* Head of the list maintained
                                                  by the queue handler
                                                  routine. */
                  struct shotphase *forio;     /* Head of the list maintained
                                                  by the io handler routine.
                                               */
                 };
/*
------------------------------------------------------------------------------
STRUCTURE: wvphsseq_changelist
An entry in the change list for a phase sequence.
------------------------------------------------------------------------------
*/
struct wvphsseq_changelist {
                         unsigned int time;  /* Time to make the change, 
                                       in units
                                       of shot phase clock ticks since the
                                       start of the shot phase sequence. */
                         int intdata;  /* The data for the change.  This
                                       is always the identifying index of
                                       the shot phase to be started at the
                                       specified time. */
                         struct wvphsseq_changelist *next_change; 
                                         /* pointer to
                                            next change in the list */
                        };
/*
------------------------------------------------------------------------------
STRUCTURE: wvphsseq
Descriptor of a phase sequence as maintained in the waveform server.
------------------------------------------------------------------------------
*/
struct wvphsseq {
                  int number;	/* Index of the phase sequence. */
                  int category; /* Category that the phase sequence belongs
                                   to. */
                  char *name;   /* Name used by the operator to identify
                                   the phase sequence. */
                  struct waveform_variable *wavelist; /* Structure to hold
                                   the variable values for the waveform
                                   that defines this phase sequence. */
                  int first_waveform;  /* Index in the waveforms array of
                                   the waveform for this phase sequence. */
                  int entrybehavior;  /* Behavior of the phase sequence
                                   when the phase sequence is started. */
                  struct wvphsseq_changelist *changes; /* Head of the change
                                   list for this phase sequence. */
                  int change_count; /* Number of entries in the change list
                                   for this phase sequence. */
                  struct wvphsseq *nextseq;  /* Pointer to the next entry in
                                   the linked list of phase sequence
                                   descriptors. */
                };                 
/*
------------------------------------------------------------------------------
STRUCTURE: initialize_wvphsseq
A structure providing the info with which to initialize a phase sequence
descriptor.
------------------------------------------------------------------------------
*/
struct initialize_wvphsseq {
                  int number;	/* Index of the phase sequence. */
                  int category; /* Category that the phase sequence belongs
                                   to. */
                  char *name;   /* Name used by the operator to identify
                                   the phase sequence. */
                  int entrybehavior;  /* Behavior of the phase sequence
                                   when the phase sequence is started. */
                           };                 
/*
------------------------------------------------------------------------------
STRUCTURE: seqptrs
A structure to hold the addresses of the heads of the linked lists of
shot phase sequence descriptors for a given category.  There are two lists,
one maintained by the io handler routine and one by the queue handler
routine. 
------------------------------------------------------------------------------
*/
struct seqptrs {
                struct wvphsseq *forqueue; /* head of the list for the 
                                              queue handler. */
                struct wvphsseq *forio;    /* head of the list for the 
                                              io handler. */
               };

/*
------------------------------------------------------------------------------
STRUCTURE: all_vector_info
structure used in the algorithm master files to initialize
vector information to be used in archiving of vector elements to ptdata.
This version has fields for vector type and the virtual cpu the vector is on.
------------------------------------------------------------------------------
*/
struct all_vector_info {
        int type;
        int vcpu;
        char *element_name;
        int element_number;
        int archive_mask;
        float inherent_number;
        float physics_zero;
};

/*
------------------------------------------------------------------------------
STRUCTURE: vector_info
structure primarily used in the algorithm master files to initialize
vector information to be used in archiving of vector elements to ptdata.
------------------------------------------------------------------------------
*/
struct vector_info {
        char *element_name;
        int element_number;
        int archive_mask;
        float inherent_number;
        float physics_zero;
};

/*
------------------------------------------------------------------------------
STRUCTURE: archive_vector_info
this structure is used to pass a vector info structure from one process
to another over a socket.  the primary difference between this structure
and the vector_info structure is that element_name is a character string
and not a pointer.
------------------------------------------------------------------------------
*/
#define GEN_archive_vector_info \
SDSTART(struct archive_vector_info {                    ,D_archive_vector_info)\
DGEN   (	int read_status;                        ,INTTYPE,1)\
DGEN   (        char element_name[MAX_POINTNAME_SIZE];  ,CHARTYPE,\
						MAX_POINTNAME_SIZE)\
DGEN   (        int element_number;                     ,INTTYPE,1)\
DGEN   (        int archive_mask;                       ,INTTYPE,1)\
DGEN   (        float inherent_number;                  ,FLOATTYPE,1)\
DGEN   (        float physics_zero;                     ,FLOATTYPE,1)\
SDEND  (                           };      )
GEN_archive_vector_info

/*
------------------------------------------------------------------------------
STRUCTURE: rtmessage_register_struct
A structure which includes extra information to the rtmessage_register
function.
------------------------------------------------------------------------------
*/
struct rtmessage_register_struct {
                unsigned int timeout;  /* timeout in microseconds */
                int buffer_count;  /* multiplier for receive buffer */
                int sendbuf_memory_region;  /* use instead of func. arg */
                int dest_memory_region;     /* use instead of func. arg */
                int dummy;    /* placeholder so struct initialization
                                 can end with a comma */
                         };
/*
------------------------------------------------------------------------------
STRUCTURE: forarchive_struct
A structure which includes values needed to archive parameter data.
------------------------------------------------------------------------------
*/
struct forarchive_struct {
                processor_type_mask client_mask;  /* The processor type mask. */
                         };
/*
------------------------------------------------------------------------------
STRUCTURE: fromarchive_struct
A structure which includes values needed to restore parameter data.
------------------------------------------------------------------------------
*/
struct fromarchive_struct {
                processor_type_mask client_mask;  /* The processor type mask. */
                int which_queue;		  /* which queue */
                char *group_name;		  /* group to match */
                char *new_name;			  /* new name to replace */
                int legacy_names_count;           /* count of parameter data
                                                     names to change */
                int legacy_groups_count;          /* count of parameter data
                                                     groups to append "_a" */
                char **legacy_names_old;          /* array of old names */
                char **legacy_names_new;          /* array of new names */
                char **legacy_groups;             /* array of group names */
                          };
/*
------------------------------------------------------------------------------
STRUCTURE: forhost_struct
A structure which includes values needed to pass data to the host_realtime cpu.
------------------------------------------------------------------------------
*/
struct forhost_struct {
                processor_type_mask client_mask;  /* The processor type mask. */
                int highest_block_index;          /* Highest block index for
						     parameter data blocks */
                      };
/*
------------------------------------------------------------------------------
STRUCTURE: foruser_struct
A structure which includes values needed to pass data to the user interface.
------------------------------------------------------------------------------
*/
struct foruser_struct {
                processor_type_mask client_mask;  /* The processor type mask. */
                int forwho;                       /* For which case */
                int string_count;                 /* # of strings to follow */
                char **strings;                   /* block names to return */
                      };
/*
------------------------------------------------------------------------------
STRUCTURE: fromuser_struct
A structure which includes values needed to get data from the user interface.
------------------------------------------------------------------------------
*/
struct fromuser_struct {
                processor_type_mask client_mask;  /* The processor type mask. */
                int which_queue;		  /* which queue */
                       };
/*
------------------------------------------------------------------------------
STRUCTURE: vector_case_info
A structure that contains info about the vector case statement
using the various case_ macros.  This info is useful for generating
code since the vector element macro is saved as well as its data entry.
------------------------------------------------------------------------------
*/
struct vector_case_info {
                         char *type;           /* case macro used */
                         int data_entry;       /* data entry number */
                         char *de_macro;       /* actual de macro used */
                         int element_number;   /* The vector element number */
                         char *element_macro;  /* actual element macro used */
                         int vcpu;             /* virtual cpu number */
                         char *waveform1;      /* waveform name used */
                         char *waveform2;      /* second waveform name used */
                         char *waveform3;      /* third waveform name used */
                         float offsets[3];     /* up to 3 offsets */
                         float factors[3];     /* up to 3 factors */
                         char *block_index;    /* block index for paramdata */
                         struct vector_case_info *next_info; /* pointer to
                                                  next info in the list */
                        };
/*
------------------------------------------------------------------------------
STRUCTURE: algorithm_usage
A substructure of a catalg structure that provides data that is
used only for describing an algorithm.
------------------------------------------------------------------------------
*/
struct algorithm_usage {
                char *parampro;  /* Pointer to a string giving the name
                                    of the idl procedure that the user
                                    interface should call to allow modifiction
                                    of the parameter data for the algorithm */
                void (*vectorfunction)(int, struct shotphase *); /* Pointer
                                             to the function that knows
                                             how to compute target vectors
                                             etc. from raw input data */
                void (*paramarchivefcn)(struct shotphase *,
                                        char **, int *,
                                        short **, int *,
                                        int **, int *,
                                        float **, int *,
                                        double **, int *,
                                        struct forarchive_struct *); /*
                                             Pointer to the function that
                                             to the function that
                                             will return the parameter data
                                             to be archived. */
                void (*paramrestorefcn)(struct shotphase *,
                                        char *, int,
                                        short *, int,
                                        int *, int,
                                        float *, int,
                                        double *, int,
                                        struct fromarchive_struct *); /*
                                             Pointer to the function that
                                             to the function that
                                             will put the parameter data
                                             where it belongs after it
                                             is restored. */
                void (*parameterfunction)(struct shotphase *); /* Pointer 
                                             to the function that
                                             will do any required 
                                             initialization of the parameter
                                             data for the algorithm. */
                void (*paramforhostfcn)(struct shotphase *, int,
                                        char **, int *,int,
                                        struct forhost_struct *); /*
                                             Pointer to the function that
                                             to the function that
                                             will return the data to be
                                             passed to a specified virtual
                                             real time cpu. */
                void (*paramforuserfcn)(struct shotphase *,
                                        char **, int *,
                                        struct foruser_struct *); /*
                                             Pointer to the function that
                                             to the function that
                                             will return the data to be
                                             passed to the user interface
                                             for modification. */
                void (*paramfromuserfcn)(struct shotphase *,
                                         char *, int,
                                         struct fromuser_struct *); /*
                                             Pointer to the function that
                                             to the function that
                                             will accept the data returned
                                             by the user interface and store
                                             it away properly. */
                int (*paramsizefcn)(struct shotphase *, int, int);
                                          /* Pointer 
                                             to the function that
                                             will return the number of
                                             bytes needed for
                                             storage of parameter data on the
                                             specified virtual cpu for the
                                             specified shot phase. */
                void (*vertexfunction)(struct shotphase *); /* Pointer 
                                             to the function that
                                             will do any required 
                                             initialization of waveforms 
                                             for the algorithm. */
                int (*scratchsizefcn)(struct shotphase *, int);
                                          /* Pointer
                                             to the function that
                                             will return the number of
                                             bytes needed for
                                             scratch memory on the
                                             specified virtual cpu for the
                                             specified shot phase. */
		struct vector_info *target_info[MAX_VCPUS]; /* structures
						containing information on
						target vector elements, used
						primarily for archiving.  there
						is one structure defined for
						each virtual cpu */
		struct vector_info *pointer_target_info[MAX_VCPUS]; 
                                             /* structures
						containing information on
						pointer target vector 
						elements, used
						primarily for archiving.  there
						is one structure defined for
						each virtual cpu */
		struct vector_info *error_info[MAX_VCPUS]; /* structures
						containing information on
						error vector elements, used
						primarily for archiving.  there
						is one structure defined for
						each virtual cpu */
		struct vector_info *shape_info[MAX_VCPUS]; /* structures
						containing information on
						shape vector elements, used
						primarily for archiving.  there
						is one structure defined for
						each virtual cpu */
		struct vector_info *command_info[MAX_VCPUS]; /* structures
						containing information on
						command vector elements, used
						primarily for archiving.  there
						is one structure defined for
						each virtual cpu */
                int param_targets[MAX_TARGETS_AFFECTED]; 
                                            /* List of the "data entry"
                                               numbers of data that is
                                               derived from the raw data that
                                               needs to be recalculated when
                                               the parameter data changes. */
                int global_targets[MAX_TARGETS_AFFECTED]; 
                                            /* List of the "data entry"
                                               numbers of data that is
                                               derived from the raw data that
                                               needs to be recalculated when
                                               the global parameter 
                                               data changes. */
                int unattached_targets[MAX_TARGETS_AFFECTED]; 
                                            /* List of the "data entry"
                                               numbers of data that is
                                               not attached to some raw
                                               data. This is data
                                               derived from the raw data that
                                               needs to be recalculated when
                                               the shot phase is initialized.
                                               These are data entry number
                                               that are not coupled to a
                                               specific piece of raw data
                                               and so are not specifically
                                               listed for computation
                                               anywhere else. */
		struct all_vector_info *all_info; /* array of structures
						containing information on
						all vector elements, used
						primarily for archiving.  This
						structure has in it 
						the virtual cpu */
		struct vector_case_info *case_info; /* linked list
						structure has in it 
						the virtual cpu */
                        };
/*
------------------------------------------------------------------------------
STRUCTURE: category_usage
A substructure of a catalg structure that provides data that is
used only for describing a category.
------------------------------------------------------------------------------
*/
struct category_usage {
                       void (*vertexfunction)(struct wvphsseq *); /* Pointer 
                                             to the function that
                                             will do any required 
                                             initialization of phase sequence
                                             waveforms for the category. */
                       void (*vectorfunction)(int); /* Pointer
                                             to the function that knows
                                             how to compute target vectors
                                             etc. from raw input data */
                       int phasetable_targets[MAX_TARGETS_AFFECTED]; 
                                          /* List of the "data entry"
                                             numbers of data that
                                             needs to be recalculated when
                                             any modifications to the phase
				             table of this category are made.*/
                       int cpu_count; /* # of real time computers used by
                                         this category. */
                       int *cpu_map;  /* Pointer to the array that
                                         maps between virtual cpu 
                                         number (the index into this array)
                                         and physical cpu number (the content
                                         of this array) for the category. */
     		       int continuous_target_count[MAX_VCPUS];
                                      /* # of continuous target vector elements
                                         used by this category
                                         on each virtual cpu. */
                       int target_count[MAX_VCPUS];
                                      /* # of target vector elements used
                                         by this category 
                                         on each virtual cpu. */
                       int pointer_target_count[MAX_VCPUS];
                                      /* # of pointer target vector 
					 elements used
                                         by this category 
                                         on each virtual cpu. */
                       int error_count[MAX_VCPUS];
                                      /* # of error vector elements used
                                         by this category 
                                         on each virtual cpu. 
                                         Same as the number of P vector and
                                         pidtau vector elements used. */
                       int shape_count[MAX_VCPUS];
                                      /* # of shape vector elements used
                                         by this category 
                                         on each virtual cpu. */
                       int command_count[MAX_VCPUS];
                                      /* # of command vector elements used
                                         by this category on each virtual 
					 cpu. */
                       int function_count[MAX_VCPUS];
                                      /* # of function vector elements used
                                         by this category on each virtual
                                         cpu. */
                       int *continuous_target_map[MAX_VCPUS];
                                      /* Pointer to an array that contains
                                         the index in the portion of the
                                         target vector which contains the
                                         "continuous" elements of
                                         each element used by this category.
                                      */ 
                       int access_control;
                                      /* The default value of the access 
                                         control mask for this category. */
                      };                                      
/*
------------------------------------------------------------------------------
STRUCTURE: cat_alg
A structure used to describe a control category or algorithm
in the waveform server process.
------------------------------------------------------------------------------
*/
struct cat_alg {
                int category;  /* Category code number */
                int algorithm; /* Algorithm code number (0 for an entry
                                  describing a category) */
                int version;          /* Number giving the "minor" version 
                                         of the category or algorithm */
                int subset_count;     /* Number of waveform subsets defined by
                                         by this category or algorithm */
                char **subset_names;  /* Pointer to a list of strings giving
                                         the names of the waveform subsets
                                         for the category or algorithm. */
                char *name;    /* Category or algorithm name string */
		char *identifier;  /* Pointer to a character string that
                                      is used by archived data to identify
                                      the category or algorithm.  
                                      This is equivalent to
                                      a "major version number." */
                int *function_map[MAX_VCPUS];
                                       /* Pointer to an array that contains
                                         the index in the function vector of
                                         each element used by this catalg.
                                        */
                char **functionnames[MAX_VCPUS];
                                      /* Pointer to arrays of strings,
                                         one array for each virtual cpu,
                                         each giving a list of names
                                         of functions in the realtime
                                         code.  Each named function has
                                         its address copied into the function
                                         vector in the element number given
                                         by a value from the function_map
                                         array. There is a one-to-one
                                         correspondance between elements
                                         in the functionnames array for a
                                         given virtual cpu and the
                                         function_map array for the same cpu.
                                         This info for a category gives the
                                         default set of functions, and for
                                         an algorithm gives the actual
                                         functions to be used.
                                         */
                struct algorithm_usage alguse;    /* Only relevant for a
                                                    catalg entry which is
                                                    for the algorithm. This
                                                    structure can be expanded
                                                    without having to update
                                                    all of the algorithm
                                                    master files. */
                struct category_usage catuse;    /* Only relevant for a
                                                    catalg entry which is
                                                    for the category. This
                                                    structure can be expanded
                                                    without having to update
                                                    all of the algorithm
                                                    master files. */
               };
/*
------------------------------------------------------------------------------
STRUCTURE: host_cat_alg
A structure used to describe a control algorithm
in the host_realtime process.
------------------------------------------------------------------------------
*/
struct host_cat_alg {
                int category;  /* Category code number */
                int algorithm; /* Algorithm code number */
                int (*initfunction)(struct rtshotphase *,
				    struct hostshotphase *,
				    struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do pre-shot
                                           initialization work for this
                                           algorithm. 
					   If this is a null pointer,
                                           then no function is called.*/
                int (*endfunction)(struct rtshotphase *,
				   struct hostshotphase *,
				   struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do post-shot
                                           cleanup work for this
                                           algorithm. 
					   If this is a null pointer,
                                           then no function is called.*/
		char *identifier;  /* Pointer to a character string that
                                      identifies the category or algorithm. */
                int (*calcfunction)(struct rtshotphase *,
				   struct hostshotphase *,
				   struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do post-shot
                                           calculations for this
                                           algorithm. 
					   If this is a null pointer,
                                           then no function is called.*/
                int (*archivefunction)(struct rtshotphase *,
				   struct hostshotphase *,
				   struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do post-shot
                                           archiving of data.
					   If this is a null pointer,
                                           then no function is called.*/
               };
/*
------------------------------------------------------------------------------
STRUCTURE: rt_cat_alg
A structure used to describe a control algorithm
in the realtime process.
------------------------------------------------------------------------------
*/
struct rt_cat_alg {
                int category;  /* Category code number */
                int algorithm; /* Algorithm code number */
                void (*initfunction)(struct rtshotphase *,
				     struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do pre-shot
                                           initialization work for this
                                           algorithm.  
					   If this is a null pointer,
                                           then no function is called.*/
                void (*endfunction)(struct rtshotphase *,
				    struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called to do post-shot
                                           cleanup work for this
                                           algorithm.  
					   If this is a null pointer,
                                           then no function is called.*/
                void (*phasebeginfcn)(struct rtshotphase *,
                                      struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called in real time each time a 
                                           shot phase using this algorithm
                                           is started at the beginning of the
                                           phase. If this is a null pointer,
                                           then no function is called.*/
                void (*phaseenterfcn)(struct rtshotphase *,
                                      struct rt_heap_misc *); /* Pointer 
                                           to the function 
                                           called in real time each time a 
                                           shot phase using this algorithm
                                           is entered at a time other than
                                           the beginning of the phase. 
                                           If this is a null pointer, then
                                           no function is called. */
               };
/*
------------------------------------------------------------------------------
STRUCTURE: shotphase_message
Structure of data transmitted when the shot phase table is returned through
the socket.
------------------------------------------------------------------------------
*/
struct shotphase_message {        
                  int number;
                  int category;
                  int algorithm;
                  float start_time;
                  float end_time;
                 };
/*
------------------------------------------------------------------------------
STRUCTURE: name_change
Structure with old data item name and the current name.
------------------------------------------------------------------------------
*/
struct name_change {        
                  int category;        /* Category code number */
                  int algorithm;       /* Algorithm code number */
                  char *old_name;      /* old data item name */
                  char *current_name;  /* current data item name */
                 };
/*
------------------------------------------------------------------------------
STRUCTURE: waveform
Structure defining a waveform.  This structure holds the values that
are fixed (not changed by the operator).
------------------------------------------------------------------------------
*/
struct waveform {
                 int message_length; /* Number of bytes required in the 
                                        message to transmit the info about
                                        this waveform to the user interface.
					Obsolete in the 32/64 bit compatible
					PCS upgrade.
                                     */
                 float scale[4];     /* Default plot scales for this waveform
                                     */
                 char *name;         /* waveform name. */
                 char *xunits;       /* the units of the x axis on a plot of
                                        the waveform. */
                 char *yunits;       /* the units of the y axis on a plot of
                                        the waveform. */
                 char *restorepoint; /* The pointname to access when restoring
                                        this waveform from an archived target
                                        vector element. */
                 float restorescale;  /* A scale factor to multiply the
                                         restored data by before it is copied
                                         to the waveform. */
                 float restoreoffset; /* An offset to add to the
                                         restored data before it is copied
                                         to the waveform. */
                 char *archivepoint;  /* The pointname to use when archiving
                                         this waveform by storing a target
                                         vector element. */
                 char *description;   /* The description string to use on
                                         a plot of the waveform. */
                 int category;        /* The category that uses this waveform
                                      */
                 int algorithm;      /* The algorithm that uses this waveform
                                      */
                 int targets[MAX_TARGETS_AFFECTED]; /* The "data entry" value
                                        indicating which processed data must
                                        be recalculated when the vertices for
                                        this waveform are changed. */
                 int step;           /* set to 1 if this is a step waveform,
                                        set to 0 if this is a continuous 
                                        waveform. */
                 int gridded;        /* set to -1 if this is a gridded
                                        waveform, 0 if it is continuous and
                                        a value greater than zero to indicate
                                        a count of values that the y axis
                                        of the waveform vertices can have. */
                 float gridpoints[MAX_WAVEFORM_GRIDPOINTS]; /* If this is
                                        a gridded waveform, two values that
                                        give the zero offset and delta
                                        of the yaxis grid values, otherwise
                                        if "gridded" > 0, this array lists
                                        the allowed y axis values. */
                 float ymin;         /* The minimum value that the y value of
                                        a vertex can have. */
                 float ymax;         /* The maximum value that the y value of
                                        a vertex can have. */
                 int subset;         /* The index of the subset to which this
                                        waveform belongs. 0 if subsets are
                                        not used. */
                 int access_control; /* The default value of the access control
                                        mask for this waveform. */
                };
/*
------------------------------------------------------------------------------
STRUCTURE: externaldata
Structure defining external data, i.e. data that is found outside
the current phase.  This structure declares what data is needed
and where it can be found.  It also declares information about this
algorithm so that the vectors function can be called when the
external data changes.  Note that the last few fields are filled
in at runtime so any fields to be added must come before these.
------------------------------------------------------------------------------
*/
struct externaldata {
                 int category;       /* The category that uses this data */
                 int algorithm;      /* The algorithm that uses this data */
                 char *name;         /* data item name */
		 char *cat_id;       /* identifier of the category where the
                                        data can be found */
		 char *alg_id;       /* identifier of the algorithm where the
                                        data can be found */
                 char *phase_name;   /* the name of the phase where the
                                        data can be found */
                 int targets[MAX_TARGETS_AFFECTED]; /* The "data entry" value
                                        indicating which processed data must
                                        be recalculated when the data is 
                                        changed */

/*  do not add fields below this line for compile time values */

                 int num_indices;    /* number of indices */
                 int *wv_indices;    /* indices of the waveform(s) where 
                                        data is found */
                };
/*
------------------------------------------------------------------------------
STRUCTURE: phasenumber
Structure that holds the next index to be assigned to a shot phase.
------------------------------------------------------------------------------
*/
struct phasenumber {
                     int forqueue; /* The value to be used when adding a
                                      shot phase to the queue handler's
                                      list. */
                     int forio; /* The value to be used when adding a
                                      shot phase to the io handler's    
                                      list. */
                   };
/*
------------------------------------------------------------------------------
STRUCTURE: rt_heap_misc_offsets
This is the structure that is equivalent to rt_heap_misc but which contains
offsets in the address space the real time processor rather than pointers.
This structure is defined in realtime_data.h so that its definition is
next to the definition of rt_heap_misc.  But, this structure isn't actually
used on the real time processor so its definition is only included for
PCS server processes.
------------------------------------------------------------------------------
*/
#define OFFSET_STRUCTURE
#include "realtime_data.h"
#undef OFFSET_STRUCTURE
/*
------------------------------------------------------------------------------
STRUCTURE: host_info_offsets
Info to provide to host_realtime routines that is not contained in
the rtheap structures. All values that indicate a memory location are given
as a byte offset into the region of memory on the real time processor
that is allocated for the PCS memory heap.
------------------------------------------------------------------------------
*/
#define GEN_host_info_offsets \
SDSTART(struct host_info_offsets {             ,D_host_info_offsets         ) \
DGEN   (          int size_pidlookup;               ,INTTYPE,1) \
                                     /* Number of bytes in the block\
                                             of memory on the real time cpu\
                                             that were allocated for pid\
                                             lookup tables. */\
DGEN   (          rtheap_offset *targetvector_map;  ,POINTERTYPE,1)\
                                     /* An array of\
                                           offsets in the address space\
                                           of the real time cpu, each\
                                           to an array of ints.\
                                           Each array will specify\
                                           the offsets\
                                           into the target vector of all\
                                           elements used by the category.\
                                           The content of this\
                                           targetvector_map array is\
                                           copied into the space pointed\
                                           to by targetvector_map in rtheap.\
                                           */\
DGEN   (          int *phsseq_count;                ,POINTERTYPE,1)     \
                                     /* The number of phase sequence\
                                           descriptors defined for each\
                                           category. This info is copied into\
                                           the location pointed to by\
                                           rtheap.phsseq_count */\
DGEN   (          int *phase_count;                 ,POINTERTYPE,1)     \
                                     /* The number of shot phase \
                                           descriptors defined for each\
                                           category. This info is copied into\
                                           the location pointed to by\
                                           rtheap.phase_count */\
DGEN   (          rtheap_offset *phsseqentries;     ,POINTERTYPE,1) \
                                     /* An offset in the\
                                           address space of the real time\
                                           cpu for each\
                                           category to the array of phase seq.\
                                           descriptors for that category\
					   (struct phs_tmg_seq). \
                                           This info is copied into the \
                                           location pointed to by\
                                           rtheap.phsseqentries */\
DGEN   (          rtheap_offset *phaseentries;      ,POINTERTYPE,1) \
                                     /* A offset in the\
                                           address space of the real time\
                                           cpu for each\
                                           category to the array of phase\
                                           descriptors for that category\
					   (struct rtshotphase). \
                                           This info is copied into the \
                                           location pointed to by\
                                           rtheap.phaseentries. */\
NDGEN  (          rtheap_offset initial_mb;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset in the\
                                         address space of the real time\
                                         cpu to a block of memory allocated to\
                                         hold all of the initial\
                                         slope/intercept arrays required by\
                                         all of the shot phases for all\
                                         of the categories executed on\
                                         a given physical cpu. */\
NDGEN  (          rtheap_offset initial_ct;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset in the\
                                         address space of the real time\
                                         cpu to a block of memory allocated to\
                                         hold all of the initial\
                                         ct value arrays required by\
                                         all of the shot phases for all\
                                         of the categories executed on\
                                         a given physical cpu. */\
DGEN   (          int mb_count;                     ,INTTYPE,1)  \
                                     /* The number of slope/intercept pairs for\
                                   which space was allocated in the block\
                                   pointed to by initial_mb. */\
NDGEN  (          rtheap_offset phasechanges;   ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset in the\
                                   address space of the real time\
                                   cpu to a block of memory that is\
                                   the correct size to hold all of the\
                                   shot phase change lists. */\
DGEN   (          int size_phasechanges;            ,INTTYPE,1)  \
                                     /* The size in bytes of the block\
                                   pointed to by phasechanges */\
NDGEN  (          rtheap_offset seqchanges;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset in the\
                                   address space of the real time\
                                   cpu to a block of memory that is\
                                   the correct size to hold all of the\
                                   phase sequence change lists. */\
DGEN   (          int size_seqchanges;              ,INTTYPE,1)  \
                                     /* The size in bytes of the block\
                                   pointed to by seqchanges */\
NDGEN  (          rtheap_offset adrtheap;       ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset (in the\
                                           address space of the rt\
                                           cpu) of the block of memory\
                                           to be used for the rtheap\
                                           structure. Filled in during\
                                           the process of memory allocation\
                                           for the rt cpu. */\
DGEN   (          struct other_cpu_info *other_cpus; ,POINTERTYPE,1) \
                                     /* A pointer to \
                                       an array of structures that holds\
                                       information about the rtcpus\
                                       in the system.\
                                       This array is filled\
                                       by fill_rtheap and is copied to the\
                                       memory of the rt cpu. */\
SDEND (                 };   )
GEN_host_info_offsets
/*
-------------------------------------------------------------------------------
STRUCTURE: cpu_constant:
Structure to hold constants for a particular CPU.

This structure really shouldn't be part of the infrastructure.
Eventually, it will be removed.

This structure provides information that is used to fill in the
cpu_rt_heap_offsets structure.  Together, these structures are used to provide
information for the various assembly language routines that are used by
control algorithms.  Originally, it was anticipated that this structure would
need to be unique to each CPU.  However, it hasn't worked out that way so
now we define a single structure that is common to all CPUs.
-------------------------------------------------------------------------------
*/
struct cpu_constant {
                      int bs_pid_group1;   /* Index in the pidtau vector
                                              of the 1st element used for
                                              calculation group 1. */
                      int bs_pid_group2;   /* Number in the pidtau vector
                                              of the 1st element used for
                                              calculation group 2. */
                      int bl_pid_group1;   /* Number of elements in the
                                              P vector, error vector etc.
                                              that are in pid
                                              calculation group 1. */
                      int bl_pid_group2;   /* Number of elements in the
                                              P vector, error vector etc.
                                              that are in pid
                                              calculation group 2. */
                      int bs_pidgains;     /* Index in the target vector
                                              of the first pid gain value. */
                      int bs_sat;          /* Index in the target vector
                                              of the first saturation value
                                              for standard outputs. */
                      int start_standard;   /* Offset (in number of vector
                                               elements) into the command
                                               vectors to the start of
                                               the standard outputs. */
                      int bs_t_standard;    /* Index in the target vector
                                               of the first element that is
                                               the preprogrammed output value
                                               for a standard output. */
                     };
/*
------------------------------------------------------------------------------
STRUCTURE: cpu_specs
Structure that provides data about a real time cpu.
------------------------------------------------------------------------------
*/
struct cpu_specs {
                    int cpu_number; /* index of the cpu. */
                    processor_type_mask processor_type; 
                                        /* The processor type mask.  This 
					   specifies such attributes as
					   endianness, 32/64 bit, whether 8
					   byte values are aligned on 8 byte
					   addresses.
					*/
		    int cpu_memory_max; /* maximum amount of memory allocated
					   for rtheap and all related 
                                           structures for this cpu. Shouldn't
                                           be larger than the available
                                           amount of memory on the cpu card.
                                        */
                    int supercard_number; /* Unit number of the Supercard
                                           that corresponds to the physical
                                           cpu. e.g. for cs1 this is 1 */
                    unsigned int supercard_vme_address; 
                                        /* The vme base address of
                                           the memory for the supercard unit
                                           specified in supercard_number. */
                    int avg_cycle_time;  /* Average time of a feedback cycle
                                            on this cpu in units of fine
                                            scale clock ticks.  This value is
                                            used to adjust the time threshold
                                            for a shot phase tick and for
                                            saving data to get more accurate
                                            time values. */
                    int piddtmin;   /* The minimum delta T value to use
                                       when constructing pid lookup tables for
                                       this cpu in units of fine scale clock
                                       ticks. */
                    int piddtmax;   /* The maximum delta T value to use
                                       when constructing pid lookup tables for
                                       this cpu in units of fine scale clock
                                       ticks. */
                    int piddtinc;   /* A value to be used as an exponent.
                                       2 raised to this power is the increment
                                       in delta T between entries in a pid
                                       lookup table on this cpu in units of
                                       fine scale clock ticks. */
                    struct cpu_rt_heap_offsets *cpuheap;
                                    /* Pointer to the cpu heap structure. */
                    struct cpu_constant *cpuconstants;
                                   /* Pointer to the structure that provides 
                                      constants for a particular CPU that are 
                                      used to fill in the cpuheap structure. */
                    int cat_count;	/* Number of categories executed on
                                           this rt cpu.  This value is
                                           filled in during waveserver
                                           startup. */
                    struct host_info_offsets *extra_info; 
                                        /* Pointer to the structure
                                           that holds extra info that is
                                           precalculated for each rt cpu but
                                           which doesn't fit into the 
                                           standard rt heap structures. */
                    struct rt_heap_misc_offsets *rtheap; 
                                        /* Pointer to the structure
			 which will hold the information to be loaded into 
			 the standard
			 rt heap structure for this cpu. 
			 All information in this
			 structure about the memory layout of 
			 the real time processor
			 is recorded as an offset from the beginning 
			 of the area of
			 memory allocated for the real time routines. */
                    unsigned int old_next_rt_memory_byte[RT_MEM_BLOCK_COUNT];
                                         /* next byte for memory alloction
                                            on rt processor. Filled in during
                                           the process of memory allocation
                                           for the rt cpu. */
                    int old_rtmemory_usage[MU_TYPE_COUNT]; /* counters to record
                                              how much memory is used on the
                                              physical cpu. Filled in during
                                           the process of memory allocation
                                           for the rt cpu.*/

                    int bs_c_phsseq;      /* Index in the communication vector
                                             of the first element in the block
                                        that points to phase timing sequence
                                        numbers. */
                    int *which_categories;  /* An array listing the cat codes
                                           of the categories executed on a
                                           given physical cpu. */
                    int *which_virtual_cpu; /* An array listing the virtual
                                           cpu that a given physical cpu plays
                                           the role of for the corresponding
                                           category in the which_categories
                                           array. */
                    int start_data_acq_index; /* Index of the element in 
                                           the function vector that will
                                           hold the address of the routine
                                           that will start acquisition of
                                           a new set of digitizer data. */
/*
                    The following variables store the 
                    number of elements in each of the vectors on the real
                    time cpu.
*/

                    int target_size;      /* target vector */
                    int ctarget_size;     /* continuous target vector */
                    int pointer_target_size; /* pointer target vector */
                    int error_size;       /* error vector */
                    int pvector_size;     /* P vector */
                    int pidtau_size;      /* pidtau vector */
                    int shape_size;       /* shape vector */
                    int cmd_size;         /* intcommand vector */
                    int comm_size;        /* communication vector */
                    int function_size;    /* function vector */
                    int fpcmd_size;       /* fpcommand vector */
/*
===== All fields below are filled in by the waveserver at runtime =====

                    The following fields store values associated with
                    realtime memory regions.  The installation can define
                    any number of memory regions, but the first two are
                    always the NONCACHEABLE and CACHEABLE regions of the
                    realtime heap.  Others can be defined for regular
                    memory and shareable memory or an installation specific
                    memory region which could be DMA or reflective memory.
*/
                    int num_memory_regions; 
                                         /* number of memory regions
                                            on rt processor. */
                    int num_memory_ints; 
                                         /* number of ints needed for
                                            memory region addresses
                                            on rt processor. */
                    int *rt_memory_type; 
                                         /* array of values, one for each
                                            memory region, which has the
                                            type of region each region is. */
                    int *rt_memory_flag; 
                                         /* array of values, one for each
                                            memory region, which has the
                                            flag masks for each region. */
                    unsigned int *next_rt_memory_byte; 
                                         /* array of values, one for each
                                            memory region, which has the
                                            next byte for memory allocation
                                            on rt processor. */
                    int *rtmemory_start[NUM_PREFERENCES]; /* starting byte
                                              for each section of memory for
                                              each region on the
                                              physical cpu. */
                    int *rtmemory_usage[MU_TYPE_COUNT]; /* counters to record
                                              how much memory is used 
                                              for each type of memory for
                                              each region on the
                                              physical cpu. */
/*
                    The following fields store the host name and
                    processor number of the realtime cpu.  They get
                    filled in by the waveserver after the realtime
                    cpus communicate with the waveserver at startup.
*/
                    char hostname[32];    /* host name of rt cpu */
                    int processor_number; /* processor number of rt cpu */
/*
                    The following fields store the numbers, offsets,
                    and sizes of the fields in the dma_region.  The
                    first three fields are required to be the following:
                    an array for the raw data, an array for times, and
                    an array of digital data (where each bit is archived
                    separately).  There must be at least two times and
                    they must be at least 4 bytes in size.  Other fields
                    may follow these but the installation is responsible
                    for setting them and archiving them.
*/
                    int offset_times;        /* offset to the times array */
                    int offset_diodata;      /* offset to the digital array */
                    int offset_install;      /* offset to other fields */
                    int numrawdata;          /* number of raw data channels */
                    int numtimes;            /* number of times */
                    int numdiodata;          /* number of digital channels */
                    int size_of_rawdata;     /* size in bytes of a raw value */
                    int size_of_times;       /* size in bytes of a time value */
                    int size_of_diodata;     /* size of a digital value */
                 };

/* 
------------------------------------------------------------------------------
STRUCTURE: port_host_info
structure used to exchange information about a process' port and
host name information to other processes.
------------------------------------------------------------------------------
*/
#define GEN_port_host_info \
SDSTART(struct port_host_info  {                        ,D_port_host_info)\
DGEN   (		  int message_code;             ,INTTYPE,1)\
DGEN   (		  unsigned short port;          ,SHORTTYPE,1)\
DGEN   (		  char host[HOST_LENGTH];       ,CHARTYPE,HOST_LENGTH)\
SDEND  (			};   )
GEN_port_host_info
/* 
------------------------------------------------------------------------------
STRUCTURE: extra_file_info
structure used to hold extra information to be written to the end of
the supercard memory dump file, which is not readily available from
the other structures in the supercard's memory.
------------------------------------------------------------------------------
*/
#define GEN_extra_file_info \
SDSTART(struct extra_file_info {                          ,D_extra_file_info)\
                                      /* sizes of various vectors */\
                                      /* refer to cpu_specs */\
DGEN   (            int target_size;                      ,INTTYPE,1)\
DGEN   (            int ctarget_size;                     ,INTTYPE,1)\
DGEN   (            int pointer_target_size;              ,INTTYPE,1)\
DGEN   (            int error_size;                       ,INTTYPE,1)\
DGEN   (            int pvector_size;                     ,INTTYPE,1)\
DGEN   (            int pidtau_size;                      ,INTTYPE,1)\
DGEN   (            int shape_size;                       ,INTTYPE,1)\
DGEN   (            int cmd_size;                         ,INTTYPE,1)\
DGEN   (            int comm_size;                        ,INTTYPE,1)\
DGEN   (            int function_size;                    ,INTTYPE,1)\
DGEN   (            int fpcmd_size;                       ,INTTYPE,1)\
DGEN   (            int numpiddeltat;                     ,INTTYPE,1)\
                                          /* num entries in pid controller */\
	    /* data which changes after shot, save initial values */\
DGEN   (	    int numrawsets_left;                  ,INTTYPE,1)\
DGEN   (	    int fastdatasets;                     ,INTTYPE,1)\
SDEND  (                      };                                    )
GEN_extra_file_info
/* 
------------------------------------------------------------------------------
STRUCTURE: save_code_info
structure used in collecting information used in the archiving of the
alldata array elements to ptdata.  each of the algorithm master files
can define one or more of these structures which are collected into
the control.h file and then compiled into the waveserver program.
------------------------------------------------------------------------------
*/
#define NUM_ALLDATA_POINTS 7 /* this number should be same as NUMHIGHFREQ,
			      defined in realtime_data.h */
struct save_code_info {
	int cpu_code;
	int cat_code;
	int alg_code;
	int number;
	char *name;
	struct vector_info pnames[NUM_ALLDATA_POINTS]; 
};

/* 
------------------------------------------------------------------------------
STRUCTURE: send_save_code_info
structure used in sending information about savecodes from one process to
another over a socket.
------------------------------------------------------------------------------
*/
#define GEN_send_save_code_info \
SDSTART(struct send_save_code_info {           ,D_send_save_code_info)\
DGEN   (	int cpu_code;                  ,INTTYPE,1)\
DGEN   (	int cat_code;                  ,INTTYPE,1)\
DGEN   (	int alg_code;                  ,INTTYPE,1)\
DGEN   (	int number;                    ,INTTYPE,1)\
DGEN   (	char name[25];                 ,CHARTYPE,25)\
SDEND  (                           };                                )
GEN_send_save_code_info
/*
------------------------------------------------------------------------------
STRUCTURE: linked_names_list
------------------------------------------------------------------------------
*/
struct linked_names_list {
    char *name;
    struct linked_names_list *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: linked_data_entry
------------------------------------------------------------------------------
*/
struct linked_data_entry {
    int category;
    int algorithm;
    int phase_number;
    int data_entry;
    int extdata_flag;
    struct linked_data_entry *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: user_info
------------------------------------------------------------------------------
*/
struct user_info {
    int userid;
    int instance;
    int identifier;
    unsigned short uiserver_port;
    char *username;
    char *hostname;
    char *display;
    struct user_info *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: possible_change
------------------------------------------------------------------------------
*/
struct possible_change {
    int userid;
    char *username;
    char *category;
    char *phase;
    char *data_item;
    struct possible_change *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: stringsll
------------------------------------------------------------------------------
*/
struct stringsll {
    char *str;
    struct stringsll *next;
};

/*
------------------------------------------------------------------------------
STRUCTURE: installation_data

Structure containing installation specific information to be passed
between processes.
------------------------------------------------------------------------------
*/
#define GEN_installation_data \
SDSTART(struct installation_data {                       ,D_installation_data)\
DGEN   (	int max_cpus;                            ,INTTYPE,1)\
DGEN   (	int actual_cpus;                         ,INTTYPE,1)\
SDEND  (                         };                                )
GEN_installation_data
/*
------------------------------------------------------------------------------
STRUCTURE: diagnostic_infra

Setup structure containing infrastructure data.
The first structure in non-cpu specific, 
the second is one structure per cpu.
------------------------------------------------------------------------------
*/
#define GEN_diagnostic_infra \
SDSTART(struct diagnostic_infra {                   ,D_diagnostic_infra)\
\
/* elements which apply to a single shot */\
DGEN   (	int hardware_test;                         ,INTTYPE,   1)\
		             /* Set to 1 for hardware test mode, 0\
				otherwise. */\
DGEN   (	int software_test;                         ,INTTYPE,   1)\
                              /* Set to 1 for regular software test mode,\
				 2 for simulation software test mode,\
				 0 otherwise. */\
DGEN   (	int test_shot;                             ,INTTYPE,   1)\
                              /* The shot number to use during\
				 execution in either hardware or\
				 sofware test mode. */\
DGEN   (	unsigned int timetostop;                   ,INTTYPE,   1)\
                              /* The absolute time, in units of\
				 fine scale clock ticks, that the\
				 feedback control should stop. */\
DGEN   (	int simport;                               ,INTTYPE,   1)\
                              /* The port number of the simserver */\
DGEN   (	char simhost[44];                          ,CHARTYPE, 44)\
                              /* The host name where the simserver\
				 is running (size HOST_LENGTH rounded up) */\
SDEND  (                     };                              )
GEN_diagnostic_infra

#define GEN_diagnostic_cpu_infra \
SDSTART(struct diagnostic_cpu_infra {           ,D_diagnostic_cpu_infra)\
\
DGEN   (	int firstsample;                ,INTTYPE,  1)\
                              /* The absolute time in units of fine\
			         scale clock ticks that the first\
				 sample of acquired and processed data\
				 should be saved. */\
DGEN   (	int numsamples;                 ,INTTYPE,  1)\
                              /* The number of samples of acquired and\
				 processed data that should be saved.\
					 */\
DGEN   (	int sampleinterval;             ,INTTYPE,  1)\
                              /* The time interval, in units of fine\
			         scale clock ticks, between saving\
				 samples of acquired and processed\
				 data. */\
DGEN   (	int fastdatastart;              ,INTTYPE,  1)\
                              /* The absolute time, in units of \
				 fine scale clock ticks, to start\
				 saving data that is acquired on\
				 every control loop cycle. */\
DGEN   (	int numfastsamples;             ,INTTYPE,  1)\
                              /* The number of fast data samples that\
				 should be saved. */\
DGEN   (	int saveroutine;                ,INTTYPE,  1)\
                              /* An index indicating which control\
				 algorithm routine is allowed to write\
				 into the fast data storage area. */\
DGEN   (	int savesfile;                  ,INTTYPE,  1)\
                              /* flags for each of the realtime \
				 cpus indicating whether or not to\
				 write the s file after a shot */\
SDEND  (                     };                                   )
GEN_diagnostic_cpu_infra
/*
------------------------------------------------------------------------------
STRUCTURE: starttime
Used to create a list of the start times for a given shot phase.
------------------------------------------------------------------------------
*/
struct starttime 
{
    float time1; /*the time the phase starts.*/
    float time2; /* when the switch to another phase is made*/
    int absolute; /*are the time values relative or absolute?*/
    char *name;   /*name of the phase sequence where the phase change
		    times are defined.*/
    struct starttime *next; /*next structure in the list*/
};
/*
------------------------------------------------------------------------------
STRUCTURE: data_exchange
------------------------------------------------------------------------------
*/
#define name1_len 40
#define name2_len 40
#define name3_len 40
#define GEN_data_exchange \
SDSTART(struct data_exchange {                           ,D_data_exchange)\
DGEN   (    int  type;                                   ,INTTYPE,1)\
DGEN   (    int  identifier1;                            ,INTTYPE,1)\
DGEN   (    int  identifier2;                            ,INTTYPE,1)\
DGEN   (    int  identifier3;                            ,INTTYPE,1)\
DGEN   (    int  identifier4;                            ,INTTYPE,1)\
DGEN   (    int  identifier5;                            ,INTTYPE,1)\
DGEN   (    int  identifier6;                            ,INTTYPE,1)\
DGEN   (    int  identifier7;                            ,INTTYPE,1)\
DGEN   (    char name1[name1_len];                       ,CHARTYPE,name1_len)\
DGEN   (    char name2[name2_len];                       ,CHARTYPE,name2_len)\
DGEN   (    char name3[name3_len];                       ,CHARTYPE,name3_len)\
SDEND  (                     };                                        )
GEN_data_exchange
/*
==============================================================================
SECTION: waveserver macro definitions
C preprocessor macro definitions for use in the _vectors functions
to define the separate switch statement cases for each target vector.
==============================================================================
*/

#define STRCAT(STRING1,STRING2) STRING1#STRING2

#define STRCAT3(STRING1,STRING2,STRING3) #STRING1#STRING2#STRING3

#define STRCAT4(STRING1,STRING2,STRING3,STRING4) #STRING1#STRING2#STRING3#STRING4

/* The token which results from these concatenations must be defined. */
#define CONCAT(TOKEN1,TOKEN2) TOKEN1##TOKEN2

#define CONCAT3(TOKEN1,TOKEN2,TOKEN3) TOKEN1##TOKEN2##TOKEN3

#define CONCAT4(TOKEN1,TOKEN2,TOKEN3,TOKEN4) TOKEN1##TOKEN2##TOKEN3##TOKEN4

/*
******************************************************************************
MACRO: store_data_entry

store_data_entry(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro should be used if the case for the data entry does
not use any of the case macros defined below.  It will add an
entry to the vector_case_info list in the algorithm usage
so the code generator can still get the macros used for the
data entry and the element number.
******************************************************************************
*/
#define \
 store_data_entry(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
            store_de("do_nothing",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);

/*
******************************************************************************
MACRO: store_de_paramblock

store_de_paramblock(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: pointer to data object descriptor.
FACTOR: pointer to data object descriptor.

Use this macro before calling case_ptrt_paramblock_wvmp.
******************************************************************************
*/
#define \
 store_de_paramblock(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX) \
            store_de3("direct_call",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,#OFFSET,#FACTOR,0.0,1.0,#BLOCK_INDEX,phase);
/*
******************************************************************************
MACRO: case_do_nothing

case_do_nothing(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro should be used if the case statement does not do anything
(like for parameter data).  It will add an
entry to the vector_case_info list in the algorithm usage
so the code generator can still get the macros used for the
data entry and the element number.
******************************************************************************
*/
#define \
 case_do_nothing(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("do_nothing",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            break; 

/*
******************************************************************************
MACRO: case_do_code

case_do_code(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro should be used if the case for the data entry does
not use any of the other case macros defined below.  It will add an
entry to the vector_case_info list in the algorithm usage
so the code generator can still get the macros used for the
data entry and the element number.
******************************************************************************
*/
#define \
 case_do_code(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR,CODE) \
         case THE_DE: \
            store_de("do_code",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
	    {\
		CODE\
	    }\
            break; 

/*
******************************************************************************
MACRO: case_wvmp

case_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the target vector element specified by THE_T
on the virtual cpu indicated by CPU
to be generated by multiplying the waveform by FACTOR and then adding
OFFSET. This target vector is identified as data entry THE_DE.

The target vector element is assumed to be part of the
"continuous" target vector segment. 

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single vertex at t=0 is generated 
with value equal to OFFSET.  This can be used to generate a
constant (in time) continuous target vector element.
******************************************************************************
*/
#define \
 case_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,CT_CHANGE,0); \
            break; 

/*
******************************************************************************
MACRO: case_fst_wvmp

case_fst_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the specified element in the floating step
segment of the target vector 
to be generated by mulitiplying the waveform by FACTOR and then adding
OFFSET. 

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value at t=0 is generated 
with value equal to OFFSET.  This can be used to generate a
constant (in time) floating step target vector element.
******************************************************************************
*/
#define \
 case_fst_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("fst_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,FST_CHANGE,0); \
            break; 

/*
******************************************************************************
MACROs: case_ist_wvmp, case_ist_wvmp_check

case_ist_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)
case_ist_wvmp_check(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the specified element in the integer step
segment of the target vector 
to be generated by mulitiplying the waveform by FACTOR and then adding
OFFSET. 

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value at t=0 is generated 
with integer value equal to OFFSET.  This can be used to generate a
constant (in time) integer step target vector element.

The "_check" version of this macro is intended for use with waveforms
with vertices which are an index into an array of data objects.  The valid
vertices cannot specify a data object that doesn't exist.  The waveform
must use the "/LabelsBlockRange" feature to determine what the grid
labels should be and the allowable range of Y axis values.  This macro adds
a call to a routine (param_check_index_waveform) that checks all of the
waveform vertices and inserts a fatal raw data problem if any invalid
vertices are located. A separate call to param_check_index_waveform
should also be made when the parameter data blocks containing the data 
object array are changed.
******************************************************************************
*/
#define \
 case_ist_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("ist_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,IST_CHANGE,0); \
            break; 

#define \
 case_ist_wvmp_check(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,IST_CHANGE,0); \
            param_check_index_waveform(phase,WAVEFORM); \
            break; 
/*
******************************************************************************
MACRO: case_category_offset

case_category_offset(THE_DE,THE_T,CPU,CATIDENT,CATCPU)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS]) of the virtual
     cpu on which the target vector exists
CATIDENT: identifier of the category for which the offset should be
     computed.
CATCPU: the virtual cpu of the category for which the offset is being
     computed.

This macro generates an integer step target vector value.  The value
is the index (0 based) in an array of dimension cat_count of the specified
category on the category's specified virtual cpu.
******************************************************************************
*/
#define \
 case_category_offset(THE_DE,THE_T,CPU,CATIDENT,CATCPU) \
         case THE_DE: \
            store_de("category_offset",THE_DE,#THE_DE,THE_T,#THE_T,CATCPU,\
                CATIDENT,0.0,0.0,phase);\
            make_category_offset(CATIDENT,CATCPU,THE_T,CPU,phase); \
            break; 
/*
******************************************************************************
MACRO: case_ptr_param_wvmp

case_ptr_param_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the specified element in the integer step
segment of the target vector 
to be generated by mulitiplying the waveform by FACTOR and then adding
OFFSET.  The change list entry is generated as type PTR_PARAM_CHANGE
to indicate that the integer data is an offset into a block of parameter
data to which the address of the parameter data block should be added
to get the real time value of the ist element.

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value assigned to t=0 is generated 
with integer value equal to OFFSET.  This can be used to generate a
constant (in time) pointer into the parameter data.
******************************************************************************
*/
#define \
 case_ptr_param_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("ptr_param_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,PTR_PARAM_CHANGE,0); \
            break; 
/*
******************************************************************************
MACRO: case_ptr_paramblock_wvmp

case_ptr_paramblock_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices
BLOCK_INDEX: the  "block order index" of the particular parameter data block.

This macro causes the data for the specified element in the integer step
segment of the target vector 
to be generated by mulitiplying the waveform by FACTOR and then adding
OFFSET.  The change list entry is generated as type PTR_PARAMBLOCK_CHANGE
to indicate that the integer data is an offset into a specified block 
of parameter
data to which the address of the parameter data block should be added
to get the real time value of the ist element.

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value assigned to t=0 is generated 
with integer value equal to OFFSET.  This can be used to generate a
constant (in time) pointer into the parameter data block.
******************************************************************************
*/
#define \
 case_ptr_paramblock_wvmp(THE_DE,THE_T,CPU,\
                          WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX) \
         case THE_DE: \
            store_de3("ptr_paramblock_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,"NULL","NULL",OFFSET,FACTOR,CPU,BLOCK_INDEX,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,\
                 PTR_PARAMBLOCK_CHANGE,BLOCK_INDEX); \
            break; 
/*
******************************************************************************
MACRO: case_ptrt_paramblock_wvmp

case_ptrt_paramblock_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX)

This macro causes the data for the specified element in the pointer target
vector to be a pointer to a location at a specified offset 
into a particular parameter 
data block. The offset is generated by mulitiplying the 
waveform by the byte size given by FACTOR, 
then adding the number of bytes given by OFFSET.
The waveform vertices are convergence to integers before 
performing this calculation.

Here, OFFSET and FACTOR are pointers to data object descriptors
(or NULL).  The
convert_size function is used along with the processor 
type of the real time processor
indicated by CPU to determine the size in bytes of the 
data object described by each of these descriptors.
If the pointer is NULL, the corresponding value (offset or factor) is
set to 0 (i.e. the pointer is to the beginning of the parameter data block.

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value assigned to t=0 is generated 
with integer value equal to the size determined from OFFSET.  
This can be used to generate a
constant (in time) pointer into the parameter data block.

The change list entry is generated as type PTRT_PARAMBLOCK_CHANGE
to indicate that the integer data is an offset into a specified 
block of parameter
data to which the address of the parameter data block should be added
to get the real time value of the pointer target vector element.

THE_DE: data entry index
THE_T:  pointer target vector index
CPU: virtual cpu number.
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: a  pointer to a data object descriptor for which 
        the size in bytes is used as the value to add to the waveform vertices.
        If the pointer is NULL, no descriptor is expected and the size value
        is set to 0.
FACTOR: a  pointer to a data object descriptor for which 
        the size in bytes is used as the value to  scale the waveform vertices.
        If the pointer is NULL, no descriptor is expected and the size value
        is set to 0.
BLOCK_INDEX: the  "block order index" of the particular parameter data block.

******************************************************************************
*/
#define \
 case_ptrt_paramblock_wvmp(THE_DE,THE_T,CPU,\
                          WAVEFORM,OFFSET,FACTOR,BLOCK_INDEX) \
         case THE_DE: \
         { \
/* NOTE OFFSET and FACTOR are both STRUCT_DESCRIPTORS pointers */\
            store_de3("ptrt_paramblock_wvmp",THE_DE,#THE_DE,THE_T,\
                #THE_T,CPU,WAVEFORM,#OFFSET,#FACTOR,0.0,1.0,\
                #BLOCK_INDEX,phase);\
            wv_offset(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,\
                      PTRT_PARAMBLOCK_CHANGE,BLOCK_INDEX); \
            break; \
        }
/*
******************************************************************************
MACRO: case_ptr_scratch_wvmp

case_ptr_scratch_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the specified element in the integer step
segment of the target vector 
to be generated by mulitiplying the waveform by FACTOR and then adding
OFFSET.  The change list entry is generated as type PTR_SCRATCH_CHANGE
to indicate that the integer data is an offset into a block of scratch
memory to which the address of the scratch memory block should be added
to get the real time value of the ist element.

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value assigned to t=0 is generated 
with integer value equal to OFFSET.  This can be used to generate a
constant (in time) pointer into the scratch memory.
******************************************************************************
*/
#define \
 case_ptr_scratch_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("ptr_scratch_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvmp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,\
                 PTR_SCRATCH_CHANGE,0); \
            break; 

/*
******************************************************************************
MACRO: case_ptrt_scratch_wvmp

case_ptrt_scratch_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

This macro causes the data for the specified element in the pointer
target vector to be equal to a pointer to a location at a specified offset
into the scratch data area for the particular shot phase.
The offset is generated by multiplying the 
waveform by the byte size given by FACTOR, 
then adding the number of bytes given by OFFSET.
The waveform vertices are convergence to integers before 
performing this calculation.

Here, OFFSET and FACTOR are pointers to data object descriptors
(or NULL).  The
convert_size function is used along with the processor 
type of the real time processor
indicated by CPU to determine the size in bytes of the 
data object described by each of these descriptors.
If the pointer is NULL, the corresponding value (offset or factor) is
set to 0 (i.e. the pointer is to the beginning of the scratch memory area).

If WAVEFORM is a null pointer, a waveform is not used and FACTOR is
ignored.  Instead, a single value assigned to t=0 is generated 
with integer value equal to OFFSET.  This can be used to generate a
constant (in time) pointer into the scratch memory area.

The change list entry is generated as type PTRT_SCRATCH_CHANGE
to indicate that the integer data is an offset into the scratch memory area
to which the address of the scratch memory area should be added
to get the real time value of the pointer vector element.

THE_DE: data entry index
THE_T: target vector index
CPU:  virtual cpu number.
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: a  pointer to a data object descriptor for which 
        the size in bytes is used as the value to add to the waveform vertices.
        If the pointer is NULL, no descriptor is expected and the size value
        is set to 0.
FACTOR: a  pointer to a data object descriptor for which 
        the size in bytes is used as the value to  scale the waveform vertices.
        If the pointer is NULL, no descriptor is expected and the size value
        is set to 0.

******************************************************************************
*/
#define \
 case_ptrt_scratch_wvmp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
/* NOTE OFFSET and FACTOR are both STRUCT_DESCRIPTORS pointers */\
            store_de("ptrt_scratch_wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,0.0,1.0,phase);\
            wv_offset(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,\
                      PTRT_SCRATCH_CHANGE,0); \
            break; 

/*
******************************************************************************
MACRO: case_fst_wvdp

case_fst_wvdp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.
OFFSET: the value to add to the waveform vertices
FACTOR: the value to scale the waveform vertices

This macro causes the data for the element in the floating step
segment of the target vector 
to be generated by dividing the waveform into FACTOR and then adding
OFFSET. 
******************************************************************************
*/
#define \
 case_fst_wvdp(THE_DE,THE_T,CPU,WAVEFORM,OFFSET,FACTOR) \
         case THE_DE: \
            store_de("fst_wvdp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            wvdp(WAVEFORM,OFFSET,FACTOR,THE_T,CPU,phase,FST_CHANGE); \
            break;

/*
******************************************************************************
MACRO: case_wv2mp

case_wv2mp(THE_DE,THE_T,CPU,WV1,WV2,OFFSET,FACTOR)

This macro causes the data for the element of the continuous
segment of the target vector specified by THE_T
to be generated by multiplying together two waveforms and then
multiplying the result by FACTOR and then adding
OFFSET. The waveforms are specified by the waveform name.
Both waveforms must be in the algorithm used by "phase".

When the actual waveform argument is specified it
must include the quotes normally provided for a string constant.
******************************************************************************
*/
#define \
 case_wv2mp(THE_DE,THE_T,CPU,WV1,WV2,OFFSET,FACTOR) \
         case THE_DE: \
            store_de2("wv2mp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WV1,WV2,OFFSET,FACTOR,phase);\
            wv2mp(WV1,WV2,OFFSET,FACTOR,THE_T,CPU,phase,CT_CHANGE); \
            break;

/*
******************************************************************************
MACRO: case_fst_wv2mp

case_fst_wv2mp(THE_DE,THE_T,CPU,WV1,WV2,OFFSET,FACTOR)

This macro causes the data for the element of the floating step 
segment of the target vector specified by THE_T
to be generated by multiplying together two waveforms and then
multiplying the result by FACTOR and then adding
OFFSET. The waveforms are specified by the waveform name.
Both waveforms must be in the algorithm used by "phase".

When the actual waveform argument is specified it
must include the quotes normally provided for a string constant.
******************************************************************************
*/
#define \
 case_fst_wv2mp(THE_DE,THE_T,CPU,WV1,WV2,OFFSET,FACTOR) \
         case THE_DE: \
            store_de2("fst_wv2mp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WV1,WV2,OFFSET,FACTOR,phase);\
            wv2mp(WV1,WV2,OFFSET,FACTOR,THE_T,CPU,phase,FST_CHANGE); \
            break;

/*
******************************************************************************
MACRO: case_make_pidtau

case_make_pidtau(THE_DE,THE_T,CPU,WAVEFORM1,OFFSET1,FACTOR1,
                 WAVEFORM2,OFFSET2,FACTOR2,WAVEFORM3,OFFSET3,FACTOR3)

THE_DE: data entry index
THE_T: target vector index
CPU: cpu index (into an array of dimension [MAX_VCPUS])
WAVEFORM1- 3: string giving the waveform nameS. Must be used by the algorithm
          used in 'phase'. 1 = taup waveform, 2 = taud waveform,3=taui
          waveform
OFFSET1 - 3: the value to add to the waveform vertices
FACTOR1 - 3: the value to scale the waveform vertices

This macro causes the data for the specified element in the pidtau vector
to be generated by mulitiplying the waveforms by FACTOR and then adding
OFFSET and then using them to generate a pidtau vector change list entry.
******************************************************************************
*/
#define \
 case_make_pidtau(THE_DE,THE_T,CPU,WAVEFORM1,OFFSET1,FACTOR1, \
                 WAVEFORM2,OFFSET2,FACTOR2,WAVEFORM3,OFFSET3,FACTOR3) \
         case THE_DE: \
            store_de_pidtau("make_pidtau",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM1,WAVEFORM2,WAVEFORM3,\
                OFFSET1,OFFSET2,OFFSET3,FACTOR1,FACTOR2,FACTOR3,phase);\
            make_pidtau(WAVEFORM1,WAVEFORM2,WAVEFORM3,\
                        OFFSET1,OFFSET2,OFFSET3,\
                        FACTOR1,FACTOR2,FACTOR3,\
                        THE_T,CPU,phase); \
            break; 
/*
******************************************************************************
MACRO: case_phsseq_ptr

case_phsseq_ptr(THE_DE,THE_T,CATIDENT,CATVCPU,WAVEFORM,CPU)

THE_DE: data entry index
THE_T: target vector index
CPU: virtual cpu number of the category given by "phase"
     on which the target vector is located.
WAVEFORM: string giving the waveform name. Must be used by the algorithm
          used in 'phase'.  Each waveform vertex Y value must be equal to
          the identifying "number" of a phase sequence of the category
          given by CATIDENT.
CATIDENT: the identifier of the category for which the waveform gives
          phase sequence numbers.
CATVCPU:  the virtual cpu index of the virtual cpu of the category CATIDENT
          on which the descriptor with the address to be calculated is
          located.
This macro takes the vertices of the waveform and converts each one into
the address of the phase sequence descriptor with "phase sequence number"
given by the Y value of the vertex.  The phase sequence is one associated
with the category given by CATIDENT.  The address is the address of the
descriptor in the memory of the rtcpu given by CATVCPU.  The address
is placed in the target vector (as an integer) in the element THE_T
on the virtual cpu CPU of the category given by "phase."
******************************************************************************
*/
#define \
 case_phsseq_ptr(THE_DE,THE_T,CATIDENT,CATVCPU,WAVEFORM,CPU) \
         case THE_DE: \
            store_de("wvmp",THE_DE,#THE_DE,THE_T,#THE_T,CPU,\
                WAVEFORM,OFFSET,FACTOR,phase);\
            make_phsseq_address(WAVEFORM,THE_T,CPU,phase,CATIDENT,CATVCPU); \
            break; 

#endif /* end of #ifndef _SERVERDEFS_ */
