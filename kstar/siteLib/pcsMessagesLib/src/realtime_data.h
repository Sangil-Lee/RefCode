/* @(#)realtime_data.h	1.37 05/14/07
*******************************************************************************
FILE: realtime_data.h

Structure definitions for the real time code.

Modifications:
11/1/94: version 10, created by J. Ferron
11/28/94: add toggle_mask
12/01/94: BGP updated rtshotphase structure to add parameter_data size.
12/5/94: add functionvector and functionvector_count to rtheap.
         remove algorithm_code from rtheap
         remove algorithm_code from rtshotphase
1/24/95: add to rtheap
3/13/95: JRF, handle scratch memory and phase entry functions in rtshotphase
12/99:JRF, conversions for 64 bit processor compatibility:
a) define the new structure rt_heap_misc_offsets
5/20/2000: JRF  remove the PCSUPGRADE macros, leave only the upgrade code.

*******************************************************************************
*/
#ifndef _REALTIME_DATA_
#define _REALTIME_DATA_

#include "processor_type.h"
/*
Define the macros for descriptor generation.  This first definition
defines the macros so that they simply generate the variable
definition code.
*/
#define DESCRIPTORS_PART_1
#include "descriptors.h"
#undef DESCRIPTORS_PART_1

/* 
----------------------------------------------------------------------
Definitions. 
----------------------------------------------------------------------
*/
#define RT_HEAP_GAP 32 /* The number of bytes in the
			  gap at the beginning of the real time processor
			  memory heap which is not allocated for storage by
			  rtmemory.c. For best performance this probably should
			  be a multiple of the cache line size (this is
			  essential for the i860). In this gap the
			  structure s_file_access_info is written to provide
			  data for the s file access routines.  The size of
			  gap must be at least as large as the size of
			  the s_file_access_info structure.
			  */
#define RT_CACHE_LINE_SIZE 32 /* number of bytes in a cache line in the rt
                                 cpu.  Left at the value appropriate
				 for the i860 since separating the memory
				 into 2 portions according to cacheability
				 isn't important on the Alpha.*/
/*
----------------------------------------------------------------------
STRUCTURE: s_file_access_info
This structure is located exactly at the beginning of the real time
memory heap so that it is also located exactly at the beginning of
the S file.
This structure provides the information that the S file access routines
need to locate the various real time memory heap components within the
S file.
*/
#define GEN_s_file_access_info \
SDSTART(      struct s_file_access_info{               ,D_s_file_access_info)\
NDGEN(             processor_type_mask local_processor_type;\
                                                       ,NESTEDTYPE,1,\
                                                        D_processor_type_mask)\
                                  /*\
                                  The type of processor that wrote the S file.\
                                  The value here is in bigendian format.\
                                  */\
DGEN(              struct rt_heap_misc *rtheap;         ,POINTERTYPE,1)\
                                  /*\
                                  The pointer in the address space of the\
                                  real time processor to the rt_heap_misc\
                                  structure.\
                                  */\
DGEN(              void *scheap;                        ,POINTERTYPE,1)\
                                  /*\
                                  The pointer in the address space of the\
                                  real time processor to the start of the\
                                  real time heap memory area.\
                                  */\
SDEND(                                  };                            )
GEN_s_file_access_info
/*
----------------------------------------------------------------------
STRUCTURE: dma_region
The default dma input buffer structure. Spaces for the raw data, two times 
(convertcycletime and previouscycletime), and digital data are provided.
The structure is always allocated memory on an 8 byte address boundary.
Also convertcycletime must be on an 8 byte boundary (this is satisfied
as long as NUMCHANNELS is a multiple of 2 which is satisifed by the
additional requirement of the getdata code
that NUMCHANNELS be a multiple of 8).

The length of this structure must be a multiple of 8 bytes so that
in an array of structures each entry in the array has an address that
is a multiple of 8 bytes.

As each new set of data is read in by the code, previouscycletime is
filled with the value of convertcycletime obtained from the previous
set of data.

When a set of dma data is saved, diodata is written with the value 
read from the digital io board register.

Ideally, we would have liked to have used a C structure to define
the dma_region, but since NUMCHANNELS is an installation specific
value which is not known when compiling the infrastructure code,
we have defined the dma_region as an array of unions of floats and
unsigned integers.  The first NUMCHANNELS number of entries in
a single dma_region are floating point values for the rawdata.
This is followed by four unsigned integers to store a corresponding
convert cycle time, a previous cycle time, a location for digital
io data masks, and a dummy space.

The C structure definition for this would be:
struct dma_region 
{
	float rawdata[NUMCHANNELS];
	unsigned int times[2];
        unsigned int diodata;
        unsigned int dummy; (* unused space *)
};

The installation can provide its own structure in place of this default
one.  It requires that the first three fields be for the same purpose,
though the type of variable for the rawdata can be different.
There must be at least 2 times and they must be unsigned ints.
The diodata can be 1, 2, or 4 bytes.  The count of raw data
channels (set by the installation macro NUMCHANNELS) gets into the 
infrastructure through the shared global numchannels.  This value
can be 0.  The count of diodata is 1 unless the installation sets it
to something else via the use of the function fill_dma_region_constants.
Any other fields can 
follow the diodata, but it is up to the installation to archive them.
The rt_heap_misc structure has the offsets to the times, the diodata, and
to the installation fields; the number of channels, times, and diodata
values; and the size of a single value for all three.  In addition, the
stride for the dma_region (or its equivalent) is also stored.
From these values it is possible to access each field.
----------------------------------------------------------------------
*/
typedef union {
        char  		cnum;
        short 		snum;
        float 		fnum;
        unsigned int	inum;
} DMA_REGION;

#define NUM_DMA_REGION_INTS 4	/* number of ints in a single dma_region */
#define DMA_CONVERTCYCLETIME 0  /* index for the storage locations       */
#define DMA_PREVIOUSCYCLETIME 1 /*  in the dma_regions which can be used */
#define DMA_DIODATA 2           /*  when retrieving data out of a        */
#define DMA_DUMMY 3             /*  single dma_region.                   */
/*
Descriptor for a complete dma region. The count of floats must be filled
in when the descriptor is used.
NOTE: each item in a DGEN statement creates a variable, thus,
unique names are used.
*/
#define GEN_dma_region \
DSTART(                                           D_dma_region)\
DGEN  ( float dma_region_rawdata[dummy_NUMCHANNELS]; ,FLOATTYPE,1)\
DGEN  (	unsigned int dma_region_times[2];            ,INTTYPE,  2)\
DGEN  (	unsigned int dma_region_diodata;             ,INTTYPE,  1)\
DGEN  (	unsigned int dma_region_dummy;               ,INTTYPE,  1)\
DEND
/*
----------------------------------------------------------------------
STRUCTURE: pidstore
Array of floats for temporary storage of pid results.
Note: the timecrit.s routine assumes that the size of this structure
is 16 bytes.
----------------------------------------------------------------------
*/
#define NUMPIDTEMPS 4

#define GEN_pidstore \
DSTART(                                       D_pidstore)\
DGEN  (typedef float pidstore[NUMPIDTEMPS];   ,FLOATTYPE,NUMPIDTEMPS)\
DEND
GEN_pidstore
/*
----------------------------------------------------------------------
STRUCTURE: pid_lookup
Structure to hold one lookup table entry for a pid controller.
The routine get_new_data in timecrit_routines.s
expects this structure to have a length of 24 bytes.
----------------------------------------------------------------------
*/
#define GEN_pid_lookup \
SDSTART(struct pid_lookup {                 ,D_pid_lookup)\
                                    /* two parameters for proportional part */\
DGEN   (         float p1;                  ,FLOATTYPE,1)\
DGEN   (         float p2;                  ,FLOATTYPE,1)\
                                   /* two parameters for derivative part */\
DGEN   (         float d1;                  ,FLOATTYPE,1)\
DGEN   (         float d2;                  ,FLOATTYPE,1)\
                                   /* two parameters for integral part */\
DGEN   (         float i1;                  ,FLOATTYPE,1)\
DGEN   (         float i2;                  ,FLOATTYPE,1)\
SDEND  (                  };)
GEN_pid_lookup
/*
----------------------------------------------------------------------
STRUCTURE: highfreqdata
Structure to hold all of the high freq data collected in a specified interval.
----------------------------------------------------------------------
*/
#define NUMHIGHFREQ 7 /* Number of storage arrays for high frequency
                         data other than the sample times. */
#define GEN_highfreqdata \
SDSTART(struct highfreqdata {                         ,D_highfreqdata   )\
DGEN   (          unsigned int time;                  ,INTTYPE,  1)      \
DGEN   (          float data[NUMHIGHFREQ];            ,FLOATTYPE,NUMHIGHFREQ)  \
SDEND  (                    };                                       )
GEN_highfreqdata

/*
----------------------------------------------------------------------
STRUCTURE: phsseq_change
Phase sequence change list entry.
----------------------------------------------------------------------
*/
#define GEN_phsseq_change \
SDSTART(struct phsseq_change {                         ,D_phsseq_change)\
DGEN   (          unsigned int time;                   ,INTTYPE,       1) \
                                     /* Time with respect to the start\
                                        of the phase sequence that the change\
                                        is to take place in units of phase\
                                        clock ticks. */\
DGEN   (          struct rtshotphase *phase;            ,POINTERTYPE,  1)  \
                                     /* Pointer to the shot\
                                        phase to change to at the specified\
                                        time */\
SDEND  (                        };                                      )
GEN_phsseq_change
/*
----------------------------------------------------------------------
STRUCTURE: alterphsseq
Structures to be stored on phase and phase sequence change stacks.
----------------------------------------------------------------------
*/
#define GEN_alterphsseq \
SDSTART(struct alterphsseq {                              ,D_alterphsseq)\
DGEN   (          unsigned int time;                      ,INTTYPE,1)\
                                          /* Absolute time in units of\
                                             fine scale clock ticks\
                                             that the change in phase\
                                             sequence was made. */\
DGEN   (	  unsigned int relative_time;             ,INTTYPE,1)\
                                          /* time with respect\
                                             to the start of the phase\
                                             sequence\
                                             to which we jumped when the\
                                             phase sequence was entered, in\
                                             units of shot phase clock ticks.\
                                             This can differ from zero if the\
                                             entry behavior for the phase\
                                             sequence allows a start at a \
                                             time other than the beginning\
                                             of the sequence. */\
DGEN   (          int category;                           ,INTTYPE,1)\
                                          /* The category for which the\
                                             change was made. */\
DGEN   (          struct phs_tmg_seq *newseq;             ,POINTERTYPE,1)\
                                          /* pointer to the\
                                             descriptor of the phase sequence\
                                             to which the change was made. */\
SDEND  (          };                                                )
GEN_alterphsseq
/*
----------------------------------------------------------------------
STRUCTURE: alterphase
----------------------------------------------------------------------
*/
#define GEN_alterphase \
SDSTART(   struct alterphase {                       ,D_alterphase )\
DGEN   (          unsigned int time;                 ,INTTYPE,1    )\
                                          /* Absolute time in units of\
                                             fine scale clock ticks\
                                             that the change in phase\
                                             was made. */\
DGEN   (	  unsigned int relative_time;        ,INTTYPE,1    )\
                                          /* time with respect\
                                             to the start of the shot phase\
                                             to which we jumped when the\
                                             new shot phase was entered, in\
                                             units of shot phase clock ticks.\
                                             This can differ from zero if the\
                                             entry behavior for the shot\
                                             phase allows a start at a \
                                             time other than the beginning\
                                             of the phase. */\
DGEN   (          int category;                      ,INTTYPE,1    )\
                                          /* The category for which the\
                                             change was made. */\
DGEN   (          struct rtshotphase *newphase;      ,POINTERTYPE,1)\
                                          /* pointer to the\
                                             descriptor of the phase \
                                             to which the change was made. */\
SDEND  (                      };                                   )
GEN_alterphase
/*
----------------------------------------------------------------------
STRUCTURE: mbdata
slope/intercept pair for target vector computation.
----------------------------------------------------------------------
*/
#define GEN_mbdata \
SDSTART(   struct mbdata {                              ,D_mbdata   )\
DGEN   (                  float slope;                  ,FLOATTYPE,1)\
DGEN   (                  float intercept;              ,FLOATTYPE,1)\
SDEND  (                 };                                         )
GEN_mbdata
/*
----------------------------------------------------------------------
STRUCTUREs:  phase_change_timecount
             phase_change_idata
             phase_change_fdata
UNION: phase_change_entry

In the rtshotphase structure, change_list_head and change_list_next 
point to the beginning
of an entry in the change list for the shot phase.  
Each entry in the change list is structured
as follows:
   int time_to_make_the_change; Time with respect to the start of the shot
                                phase to make the changes, in units of shot
                                phase clock ticks. There is only one
                                change list entry for a given time.
   int number_of_changes_to_make; The number of changes in this entry of the
                                  change list.  All of these changes take
                                  place at the same time.
   A list of number_of_changes_to_make pairs of values.
      unsigned int *address;  The address of the value that is to be changed.
      unsigned int  value;  The value to copy to the specified address. 
                    This can really
                    be either a float or an int since the code handling the
                    change list simply copies the value from one location
                    to another.

Because the number of address/value pairs is not predictable, 
a change list is composed of an array of unions.  Each union contains
either the time/count pair of values or an address/value pair of values.
----------------------------------------------------------------------
*/
#define GEN_phase_change_timecount \
SDSTART(struct phase_change_timecount {             ,D_phase_change_timecount)\
DGEN   (          unsigned int time;                ,INTTYPE, 1)\
                                    /* Time with respect to \
				       the start of the shot\
				       phase to make the changes, \
				       in units of shot\
				       phase clock ticks.\
				       */\
DGEN   (          int count;                        ,INTTYPE, 1)\
                                    /* The number of changes \
				       in this entry of the\
				       change list.  All of these changes take\
				       place at the same time.\
				       */\
SDEND  (                              };                       )
GEN_phase_change_timecount

#define GEN_phase_change_idata \
SDSTART(struct phase_change_idata {                  ,D_phase_change_idata)\
DGEN   (          int *address;                     ,POINTERTYPE, 1)\
                                        /* Pointer to the location where the\
					   data should be copied. */\
DGEN   (          int value;                        ,INTTYPE, 1)\
                                        /* The value to copy to the specified \
					   address.\
					*/\
SDEND  (                         };                            )
GEN_phase_change_idata

#define GEN_phase_change_fdata \
SDSTART(struct phase_change_fdata {                  ,D_phase_change_fdata)\
DGEN   (          float *address;                    ,POINTERTYPE, 1)\
                                        /* Pointer to the location where the\
					   data should be copied. */\
DGEN   (          float value;                      ,FLOATTYPE, 1)\
                                        /* The value to copy to the specified \
					   address. \
					*/\
SDEND  (                         };                            )
GEN_phase_change_fdata

#define GEN_change_list_entry \
UDSTART(union change_list_entry {                   ,D_change_list_entry)\
NDGEN  (         struct phase_change_timecount timecount;    \
                                       ,NESTEDTYPE,1,D_phase_change_timecount)\
NDGEN  (         struct phase_change_idata idata;    \
                                       ,NESTEDTYPE,1,D_phase_change_idata)\
NDGEN  (         struct phase_change_fdata fdata;    \
                                       ,NESTEDTYPE,1,D_phase_change_fdata)\
UDEND  (                         };                            )
GEN_change_list_entry

/*
----------------------------------------------------------------------
STRUCTURE: phs_tmg_seq
phase timing sequence descriptor.
----------------------------------------------------------------------
*/
#define GEN_phs_tmg_seq \
SDSTART(   struct phs_tmg_seq {                     ,D_phs_tmg_seq)\
DGEN   (          int index;                        ,INTTYPE, 1)\
                                        /* Phase timing sequence index */\
DGEN   (          int entrybehavior;                ,INTTYPE, 1)\
                                        /* How this phase timing sequence\
                                           behaves when we start using it */\
DGEN   (          int entry_count;                ,INTTYPE, 1)\
                                        /* The number of times this phase\
					   sequence has been entered.*/\
DGEN   (          unsigned int first_entry_time;    ,INTTYPE, 1)\
                                        /* Absolute time in\
                                           units of phase clock ticks that\
                                           the phase timing sequence was\
                                           first started during the shot. */\
DGEN   (          unsigned int current_time;        ,INTTYPE, 1)\
                                        /* For a sequence \
                                           currently in use, the current time\
                                           with respect to the start of the\
                                           shot phase sequence in units of\
                                           phase clock ticks. */\
DGEN   (          struct phsseq_change *change_list_head;   ,POINTERTYPE, 1)\
                                        /* Pointer \
                                           to the first entry in \
                                           the change list for this phase\
                                           sequence */\
DGEN   (          struct phsseq_change *change_list_next;   ,POINTERTYPE, 1)\
                                        /* Pointer \
                                           to the next entry in \
                                           the change list for this phase\
                                           sequence */\
DGEN   (          int change_list_entry_count;              ,INTTYPE, 1)\
					 /* The number of structures of type \
					    phsseq_change that are in the \
					    change list for this phase \
					    sequence. \
					  */ \
DGEN   (          struct rtshotphase *current_phase;        ,POINTERTYPE, 1)\
                                        /* Pointer to the\
					   descriptor for the\
                                           shot phase currently in use for\
				           this phase sequence */\
SDEND  (                      };                                           )
GEN_phs_tmg_seq

/*
------------------------------------------------------------------------------
STRUCTURE: rt_memory_region
Info about memory regions.
------------------------------------------------------------------------------
*/
#define GEN_rt_memory_region \
SDSTART(struct rt_memory_region {                  ,D_rt_memory_region)\
DGEN   (                       char *addr[2];      ,POINTERTYPE,2 )\
DGEN   (                       int size;           ,INTTYPE,1     )\
DGEN   (                       int type;           ,INTTYPE,1     )\
DGEN   (                       int flag;           ,INTTYPE,1     )\
DGEN   (                       int key;            ,INTTYPE,1     )\
DGEN   (                       int shmid;          ,INTTYPE,1     )\
SDEND  (                    };                                    )
GEN_rt_memory_region

/*
------------------------------------------------------------------------------
STRUCTURE: rt_message
Info about rtmessages
------------------------------------------------------------------------------
*/
#ifndef MAX_RTMESSAGE
#define MAX_RTMESSAGE 32
#endif
#define GEN_rt_message \
SDSTART(struct rt_message {                     ,D_rt_message)\
DGEN   (             char name[MAX_RTMESSAGE];  ,CHARTYPE,MAX_RTMESSAGE)\
DGEN   (             int sendbuf_memory_region; ,INTTYPE,2 )\
DGEN   (             int dest_memory_region;    ,INTTYPE,2 )\
DGEN   (             int sendbuf_offset;        ,INTTYPE,2 )\
DGEN   (             int dest_offset;           ,INTTYPE,2 )\
DGEN   (             int size;                  ,INTTYPE,1 )\
DGEN   (             int buffer_count;          ,INTTYPE,1 )\
DGEN   (             int dest_cpu;              ,INTTYPE,1 )\
DGEN   (             int flags;                 ,INTTYPE,1 )\
DGEN   (             int timeout;               ,INTTYPE,1 )\
DGEN   (             int counter;               ,INTTYPE,1 )\
DGEN   (             int count_waiting;         ,INTTYPE,1 )\
DGEN   (             int buffer_index;          ,INTTYPE,1 )\
SDEND  (                  };                               )
GEN_rt_message

/*
----------------------------------------------------------------------
STRUCTURE: other_cpu_info
Info about "other rt cpus" which are rtcpus other than the one that
has the data in this structure.
----------------------------------------------------------------------
*/
#define NUM_MOVABLE_PREFERENCES 19 /* must match ALLDATA_PREFERENCE+1 */

#define GEN_other_cpu_info \
SDSTART(struct other_cpu_info {                     ,D_other_cpu_info)\
DGEN   (          int cpu_number;                   ,INTTYPE, 1)\
                                        /* index (value from 1 to num real\
					  time cpus) of cpu other than\
					  this cpu. */\
DGEN   (          int *timetostop_flag;             ,POINTERTYPE, 1)\
                                        /* VME bus address of the\
                                   OBSOLETE    timetostop flag word. */\
DGEN   (          float *commbuffer;                ,POINTERTYPE, 1)\
                                        /* VME bus address of the       \
                                   OBSOLETE    communication buffer. */\
DGEN   (          void *install_buffer;            ,POINTERTYPE, 1)\
                                        /* VME bus address of the       \
                                   OBSOLETE    buffer to receive installation\
                                               dependent data. */\
DGEN   (          int timetostop_flag_offset;              ,INTTYPE, 1)\
                                        /* Offset into the real time \
                                           processor's memory heap area \
                                           where the timetostop flag word \
                                           is located. */\
DGEN   (          int commbuffer_offset;                ,INTTYPE, 1)\
                                        /* Offset into the real time \
                                           processor's memory heap area \
                                           where the communication buffer \
                                           is located. */\
DGEN   (          int install_buffer_offset;            ,INTTYPE, 1)\
                                        /* Offset into the real time \
                                           processor's memory heap area \
                                           where the buffer to receive \
                                           installation dependent data values \
                                           is located. */\
DGEN   (          int memory_index[NUM_MOVABLE_PREFERENCES]; \
                                  ,INTTYPE, NUM_MOVABLE_PREFERENCES)\
                                        /* The index of the memory region\
                                           a particular section of rt memory\
                                           uses.  Use this index into\
                                           the memory_regions array of the\
                                           rtheap to get the proper starting\
                                           address of the section. */\
DGEN   (          int num_memory_regions;               ,INTTYPE, 1)\
                                        /* The number of memory regions. */\
DGEN   (          int num_memory_ints;                  ,INTTYPE, 1)\
                                        /* The number of memory ints for\
                                           one rt cpu. */\
DGEN   (          int *memory_ints;                     ,POINTERTYPE, 1)\
                                        /* Pointer to the memory ints for rt\
                                           cpu which are really addresses.\
                                           These addresses will be either\
                                           32 bit or 64 bit.  The\
                                           installation can use these\
                                           as needed. */\
SDEND  (                      };                               )
GEN_other_cpu_info
/*
----------------------------------------------------------------------
STRUCTURE: rt_heap_misc
Structure that contains all information necessary to find the various
other data structures in memory.
Also, a few misc global values are stored here.
----------------------------------------------------------------------
*/
#define GEN_rt_heap_misc \
SDSTART(struct rt_heap_misc {                 ,D_rt_heap_misc      ) \
/*\
Misc. important data values.\
*/\
DGEN   (	  void *rtmemory;                        ,POINTERTYPE,  1) \
                                        /* Pointer to the beginning of the\
					   real time heap memory area.*/\
DGEN   (          int rtmemory_length;                   ,INTTYPE,  1) \
                                        /* Length, in bytes, of the real time\
					   heap memory area.*/\
DGEN   (          float const_one;                       ,FLOATTYPE,1) \
                                        /* initialized to 1.0 */\
DGEN   (	  float two23;                           ,FLOATTYPE,1) \
                                        /* Initialized to 2^23 = 8388608.0 */\
DGEN   (          int numtargets;                        ,INTTYPE,  1) \
                                        /* number of elements in the\
                                              "continuous" portion of the\
                                              target vector. For the \
                                       convenience of the code this value is\
                                       (number of elements/8 - 2) */\
DGEN   (          unsigned int next_shtphs_tick;          ,INTTYPE,  1) \
                                        /* absolute  time\
  				        of the next shot phase clock tick \
                                        in units of fine scale clock ticks. */\
DGEN   (	  unsigned int next_target_calc_time;     ,INTTYPE,  1) \
                                        /* absolute time\
                                       in units of fine scale clock ticks\
                                       at which the target vector should be\
                                       recalculated. */\
DGEN   (          unsigned int shtphs_tick_interval;      ,INTTYPE,  1) \
                                        /* length (in\
                                         fine scale clock ticks) of one shot\
                                         phase clock tick. */\
DGEN   (	  unsigned int shtphs_tick_count;         ,INTTYPE,  1) \
                                        /* The total number\
                                         of shot phase ticks that have passed\
                                         in the shot = current absolute\
                                         time in units of shot phase clock\
                                         ticks. */\
DGEN   (	  unsigned int currenttime;               ,INTTYPE,   1) \
                                        /* The current value of\
					 the absolute time in units of fine\
                                         scale clock ticks. */\
DGEN   (	  unsigned int previoustime;               ,INTTYPE,   1) \
                                        /* The value of the\
					 the absolute time on the previous\
					 cycle in units of fine\
                                         scale clock ticks. This value is\
					 initialized to 0 at the start of the\
					 shot. */\
DGEN   (	  unsigned int update_time;               ,INTTYPE,   1) \
                                        /* The time to use for updating\
					 the state in units of fine\
                                         scale clock ticks. */\
DGEN   (	  unsigned int category_count_c;          ,INTTYPE,   1) \
                                        /* The number of\
                                         control categories executed on the\
                                         cpu.*/\
DGEN   (	  unsigned int category_count_a;          ,INTTYPE,   1) \
                                        /* The number of\
                                         control categories executed on the\
                                         cpu minus 1 (so it is all ready to\
                                         be a loop counter for assembly\
					 language functions) */\
DGEN   (          unsigned int targetcalc_flag;           ,INTTYPE,  1) \
                                        /* Indicates,when not\
                                         zero, that the target vectors should\
                                         be calculated without checking\
                                         whether the time for the next\
                                         calculation has been reached. */\
DGEN   (	  unsigned int sampleincrement;           ,INTTYPE,  1) \
                                        /* Time in units of\
                                         fine scale clock ticks between \
                                         samples of raw and calculated data\
                                         that are saved. */\
DGEN   (	  unsigned int numrawsets_left;           ,INTTYPE,  1) \
                                        /* Number of samples\
                                         of raw and calculated data left\
                                         to save.  At the beginning of the \
                                         shot this is initialized to the\
                                         number of sets of data to save.\
                                         This value is not decremented\
                                         for the branch test because it is\
                                         necessary to advance the pointers\
                                         one more time than the number of \
                                         sets of samples that are saved. \
                                         There must be memory allocated for \
                                         one more set of data than the number\
                                         written here at the beginning of the\
                                         shot (except for the raw data \
                                         buffer where there must be two more\
                                         buffer locations than the number\
                                         here). */\
DGEN   (           unsigned int nextsample;               ,INTTYPE,  1) \
                                        /* Absolute \
                                         time in units of\
                                         fine scale clock ticks to save\
                                         the next sample of raw and\
                                         calculated data. At the beginning\
                                         of the shot this is initialized\
                                         to the time to save the first\
                                         sample. */\
DGEN   (	  int nexttoggle;                         ,INTTYPE,  1) \
                                        /* next value to load into the\
                                             watchdog toggle register */\
DGEN   (          unsigned int software_test_mode;        ,INTTYPE,  1) \
                                        /* Set to non-zero\
                                         if we are running software test mode.\
                                         */\
DGEN   (          unsigned int hardware_test_mode;        ,INTTYPE,  1) \
                                        /* Set to non-zero\
                                         if we are running hardware test mode.\
                                         */\
DGEN   (	  int numchannels;                        ,INTTYPE,  1) \
                                        /* The number of input\
                                         data channels.*/\
DGEN   (	  int datasetcount;                       ,INTTYPE,  1) \
                                        /* The number of input\
                                         data channels adjusted for the\
                                         convenience of the getdata routine:\
                                         value = ((# channels/8) - 2).  Note\
                                         the number of channels must be a\
                                         multiple of 8 for this value to be\
					 useful.  In fact, this value is \
					 really only relevant to DIII-D and \
					 should be considered obsolete at\
					 other installations.*/\
DGEN   (	  unsigned int timezero;                  ,INTTYPE,  1) \
                                        /* The value of the TR\
                                         register that corresponds to zero\
                                         time.  For normal operation this is\
                                         zero.  For hardware test mode this\
                                         is the random value in the TR \
                                         counter at the start of the control\
                                         code. */\
DGEN   (          unsigned int num_memory_regions;        ,INTTYPE, 1)\
                                        /* Number of memory regions */\
DGEN   (          unsigned int total_memory_ints;         ,INTTYPE, 1)\
                                        /* Number of memory ints used\
                                           for memory region addresses\
                                           for all cpus */\
DGEN   (          unsigned int num_rt_messages;           ,INTTYPE, 1)\
                                        /* Number of rtmessages */\
DGEN   (          unsigned int fastdatastart;             ,INTTYPE,  1) \
                                        /* Absolute\
                                         time to start saving\
                                         the fast sampled data in units of\
                                         fine scale clock ticks. */\
DGEN   (          unsigned int fastdatasets;              ,INTTYPE,  1) \
                                        /* Number of fast data \
                                        sets to save. Memory must be \
                                        allocated for one more entry than\
                                        the value here.*/\
DGEN   (          int fastdatainterval;                   ,INTTYPE,  1) \
                                        /* Time in units of \
                                        fine scale clock ticks between \
                                        samples of fast data \
                                        that are saved. */ \
DGEN   (          int previous_fastdatainterval;          ,INTTYPE,  1) \
                                        /* Previous value of \
                                        fast sample save interval. */ \
DGEN   (          unsigned int timetostop;                ,INTTYPE,  1) \
                                        /* Absolute\
                                         time to stop feedback\
                                         cycles, in units of fine scale clock\
                                         ticks. */\
DGEN   (          int csrinit;                            ,INTTYPE,  1) \
                                        /* Value to load into the DAD-1 CSR.*/\
DGEN   (          int combufcount;                        ,INTTYPE,  1) \
                                        /* The number of entries in the\
                                        communications buffer \
                                        (actually ((number/8)-1) so it\
                                        is all set to be a loop counter\
                                        in a loop in which 8 entries are\
                                        processed at a time.).\
                                        (This count includes the entries that\
                                        are used as shot phase seq. desc.\
                                        areas for each category.) Note that\
                                        the total number of entries must be\
                                        a multiple of 8. */\
DGEN   (          int saveroutine;                        ,INTTYPE,  1) \
                                        /* Index of routine allowed to\
                                        write into the alldata structure. */\
DGEN   (	  float maggain;                          ,FLOATTYPE,1) \
                                        /* Magnetics gain multiplier */\
DGEN   (	  int savebasedata;                       ,INTTYPE,  1) \
                                        /* Set to 1 if the baseline\
                                        data should be archived. */\
DGEN   (	  int dmabuffer_stride;                   ,INTTYPE,  1) \
                                        /* Length in bytes of a\
                                        buffer for dma input from the\
                                        data acquisition system. */\
/*\
There is an integer command vector and a floating point\
command vector which could be different sizes.\
*/\
DGEN   (	  int fpcommand_stride;                   ,INTTYPE,  1) \
                                        /* Length in bytes of the \
                                        fp command storage vector. */\
DGEN   (	  int intcommand_stride;                  ,INTTYPE,  1) \
                                        /* Length in bytes of the \
                                        the int command storage vector. */\
/*\
If the P and pidtau vectors are used for PID calculations,\
then the sizes of these vectors MUST be the same as the\
size of the error vector.  If not, then the stride for the\
pvector could be different.  The pidtau vector is not archived\
so no stride is required for it.\
*/\
DGEN   (	  int errorvector_stride;                 ,INTTYPE,  1) \
                                        /* Length in bytes of the\
                                        error vector. */\
DGEN   (	  int pvector_stride;                     ,INTTYPE,  1) \
                                        /* Length in bytes of the\
                                        P vector. */\
DGEN   (	  int shapevector_stride;                 ,INTTYPE,  1) \
                                        /* Length in bytes of\
                                        the shape vector. */\
DGEN   (	  int fastdata_stride;                    ,INTTYPE,  1) \
                                        /* Length in bytes of\
                                        the storage structure\
                                        for fast data. */\
DGEN   (          int functionvector_count;               ,INTTYPE,  1) \
                                        /* Number of elements\
                                        in the function vector minus 1\
                                        (so it is ready to be a loop counter\
                                        */\
/*\
Pointers to the standard data arrays:\

Arrays with one element per continuous target vector element:\
   target vector\
   target vector deltat\
   target vector slope/intercept \
   target vector vertextime\
\
Inter cpu communication\
   communication buffer\
   communication buffer for control loop\
      Note: immediately following commbuffer in memory is next_phs_seq\
      and this is read into next_phs_seq_loop, which immediately follows\
      commbufloop in memory, at the same time that commbufloop is read.\
\
DMA buffer\
\
shape,error,p,intcommand,fpcommand vectors\
alldata arrays\
pidtable vector (two pointers, one for each pid calculation group)\
pidstorage storage array\
pidgains locations in the target vector\
*/\
DGEN   (          float *adtarget;                        ,POINTERTYPE,  1) \
                                        /* pointer to the target vector */\
DGEN   (          void **pointer_target;                  ,POINTERTYPE,  1) \
                                        /* pointer to the target vector \
					   containing pointers*/\
DGEN   (          float *addeltat;                        ,POINTERTYPE,  1) \
                                        /* ptr to the deltat array for\
                                              target vector computation */\
DGEN   (	  struct mbdata *admb;                    ,POINTERTYPE,  1) \
                                        /* ptr to the slope/intercept\
                                           array that provides data \
                                           for continuous target vector\
                                           element computation. */\
DGEN   (	  int *vertextime;                        ,POINTERTYPE,  1) \
                                        /* ptr to the array holding the\
				          time wrt start of the shot phase\
                                          in units of shot phase ticks\
					  of the last target vector vertex. */\
DGEN   (          float *adcombuffer;                     ,POINTERTYPE,  1) \
                                        /* pointer to an array of\
                                         floats that can be written by\
                                         any cpu for communication between\
                                         cpus.  This buffer is always read\
                                         with pfld instructions.\
                                         This array should be written, but\
                                         never read except by specific code\
                                         that executes the pfld instructions.\
                                         */\
DGEN   (          float *adcombufloop;                    ,POINTERTYPE,  1) \
                                        /* pointer to an array of\
                                         floats into which the content\
                                         of combuffer is read on each control\
                                         cycle. This is the array that should\
                                         be read by code needing the content\
                                         of combuffer. */\
DGEN   (          DMA_REGION *dmabuffer;                  ,POINTERTYPE,  1) \
                                        /* Address of \
                                               the buffer that\
                                               will receive the data from the\
				               data acquisition board. This\
                                               value can be incremented \
                                               periodically throughout\
                                               the shot.\
                                            */\
DGEN   (          float *baseline;                        ,POINTERTYPE,  1) \
                                        /* Address of buffer that holds\
                                              the baseline values for the\
                                              input data channels. */\
DGEN   (          float *datavector;                      ,POINTERTYPE,  1) \
                                        /* Address of the vector that\
                                                holds the current set of data\
                                                after the baseline is \
                                                subtracted. */\
DGEN   (          float *physicsvector;                   ,POINTERTYPE,  1) \
                                        /* Address of the vector that\
                                                holds the current set of data\
                                                after the baseline is \
                                                subtracted and converted \
                                                to physics units. */ \
DGEN   (	  float *shapevector;                     ,POINTERTYPE,  1) \
                                        /* Address of the shape\
                                                 vector */\
DGEN   (          float *errorvector;                     ,POINTERTYPE,  1) \
                                        /* Address of the error vector.\
                                              */\
DGEN   (          float *fpcommand;                       ,POINTERTYPE,  1) \
                                        /* Address of the float command\
                                               vector. */\
DGEN   (          int *intcommand;                        ,POINTERTYPE,  1) \
                                        /* Address of the int command\
                                              vector. */\
DGEN   (	  float *pvector;                         ,POINTERTYPE,  1) \
                                        /* Address of the P vector. */\
DGEN   (          struct pid_lookup *pidlookup;           ,POINTERTYPE,  1) \
                                        /* Address of \
                                                   the first pid\
                                                   lookup table. */\
DGEN   (          struct pid_lookup **pidtauvector;       ,POINTERTYPE,  1) \
                                        /* Address of \
                                                   the pidtau\
                                                   vector. Each element in\
                                                   this vector points to\
                                                   a pid lookup table. */\
DGEN   (          void (**functionvector)(struct rt_heap_misc *);,POINTERTYPE,1)\
                                        /* Pointer to an array\
                                        of pointers to the functions to be\
                                        executed on each control cycle. */\
DGEN   (	  float *previous_shapevector;            ,POINTERTYPE,  1) \
                                        /* Address of the shape\
                                                 vector on the previous cycle\
                                                 */\
DGEN   (          float *previous_errorvector;            ,POINTERTYPE,  1) \
                                        /* Address of the \
                                        error vector on the previous cycle. */\
DGEN   (          float *previous_fpcommand;              ,POINTERTYPE,  1) \
                                        /* Address of the float \
                                        command vector on the previous cycle.\
                                        */\
DGEN   (          int *previous_intcommand;               ,POINTERTYPE,  1) \
                                        /* Address of the int \
                                        command vector on the previous cycle */\
DGEN   (	  float *previous_pvector;                ,POINTERTYPE,  1) \
                                        /* Address of the P \
                                        vector on the previous cycle. */\
DGEN   (	  struct highfreqdata *alldata;           ,POINTERTYPE,  1) \
                                        /* Pointer to the\
                                        current storage location for fast \
					sampled data. */\
DGEN   (          float *install_buffer;                  ,POINTERTYPE,  1) \
                                        /* Pointer to the buffer for\
                                        installation dependent data.  The\
                                        master rt cpu (the cpu that acquires\
                                        the baseline data) writes the\
                                        baseline data here.  The buffer is\
                                        one element longer than the number\
                                        of data channels.  The extra space\
                                        in the buffer is used as a flag to\
                                        indicate when the buffer has been\
                                        filled. */\
DGEN   (          int *last_int;                          ,POINTERTYPE,  1) \
                                        /* Address of the last int in\
                                        the rt cpu memory that is used. A\
                                        check value is written at that\
                                        location. */\
DGEN   (          struct alterphsseq *phsseq_stack;       ,POINTERTYPE,  1) \
                                        /* Space to\
                                        store a record of all changes made\
                                        in the phase sequence. During the\
                                        real time execution, this address\
                                        is updated to point to the next\
                                        available location in the stack. */\
DGEN   (          int phsseq_stack_size;                  ,INTTYPE,  1) \
                                        /* Count of the number\
                                        of structs of type alterphsseq that\
                                        can fit into the area allocated for\
                                        the phase sequence stack. During\
                                        the real time execution this is\
                                        updated to hold the count of stack\
                                        entries for which there is still \
                                        space. */\
DGEN   (          int phsseq_stack_entries;               ,INTTYPE,  1) \
                                        /* Number of \
                                        structs of type alterphsseq that have\
                                        been written into the phase sequence\
                                        change stack. Updated in real time\
                                        each time that there is a phase\
                                        sequence change. */\
DGEN   (          struct alterphase *phase_stack;         ,POINTERTYPE,  1) \
                                        /* Space to\
                                        store a record of all changes made\
                                        in the shot phase. During the\
                                        real time execution, this address\
                                        is updated to point to the next\
                                        available location in the stack. */\
DGEN   (          int phase_stack_size;                   ,INTTYPE,  1) \
                                        /* Count of the number\
                                        of structs of type alterphase that\
                                        can fit into the area allocated for\
                                        the phase change stack. During\
                                        the real time execution this is\
                                        updated to hold the count of stack\
                                        entries for which there is still \
                                        space. */\
DGEN   (          int phase_stack_entries;                ,INTTYPE,  1) \
                                        /* Number of \
                                        structs of type alterphase that have\
                                        been written into the phase \
                                        change stack. Updated in real time\
                                        each time that there is a phase\
                                        change. */\
/*\
Pointers to arrays that have one element for each category\
controlled by the cpu.\
*/\
DGEN   (          struct phs_tmg_seq **current_phs_seq;   ,POINTERTYPE,  1) \
                                        /* pointer\
                                         to an array of pointers to phase\
                                         timing sequence structures. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array points to the current\
                                         phase timing sequence descriptor for\
                                         each category. */\
DGEN   (          int *current_phs_seq_number;            ,POINTERTYPE,  1) \
                                        /* pointer\
                                         to an array of integers,\
                                         one for\
                                         each category controlled on the cpu.\
                                         This array gives the number \
					 (based at 1) of the\
                                         phase timing sequence currently in use\
                                         for each category. */\
DGEN   (          struct rtshotphase **current_phase;   ,POINTERTYPE,  1) \
                                        /* pointer\
                                         to an array of pointers to phase\
                                         descriptors. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array points to the descriptor\
					 for the phase currently in use\
					 for each\
					 category.*/\
DGEN   (          void (**phase_init_function)(struct rtshotphase *,\
					struct rt_heap_misc *);    \
                                                    ,POINTERTYPE, 1)\
                                        /* pointer\
                                         to an array of pointers to phase\
                                         initialize functions. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array points to the function\
					 which gets executed when a phase\
					 sequence change occurs.*/\
DGEN   (          void ***current_parameter_data;       ,POINTERTYPE,  1) \
                                        /* pointer to an array of pointers\
					   to  parameter data storage areas.\
                                         There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
					 The parameter data area indicated\
					 by each pointer is the one belonging\
					 to the phase currently in use for \
					 the corresponding category.\
					 At the beginning of\
					 each parameter data area is an array\
					 of pointers to the individual\
					 parameter data blocks stored in\
					 that area.*/\
DGEN   (           int *next_phs_seq_loop;                ,POINTERTYPE,  1) \
                                        /* pointer\
                                         to an array of integers giving the\
                                         "numbers" (based at 1) of phase\
                                         timing sequences. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array of integers is filled on\
                                         each control cycle from the data in\
                                         the next_phs_seq buffer.  This array,\
                                         which is a subset of the combufloop,\
                                         should be read but not written. */\
DGEN   (          int *next_phs_seq;                      ,POINTERTYPE,  1) \
                                        /* pointer\
                                         to an array of integers giving the\
                                         "numbers" (based at 1) of phase\
                                         timing sequences. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array of integers is part of\
                                         the combuffer.\
                                         This array is updated by any routine\
                                         that wants to trigger a category to\
                                         jump to a new phase timing sequence.\
                                         (on the i860:)\
                                         This array should be written, but\
                                         never read except by specific code\
                                         that executes the pfld instructions.\
                                         */\
DGEN   (          float *change_cache;                    ,POINTERTYPE,  1) \
                                        /* Pointer to an array with\
                                        space for 4 floats from each category.\
                                        This space is used to store the next\
					4 data values from the change list,\
				        values that have already been read\
					in with pfld. */\
DGEN   (          int **targetvector_map;                 ,POINTERTYPE,  1) \
                                        /* Pointer to an array\
                                        of pointers, one for each category,\
                                        to arrays which contain the\
                                        offsets in bytes\
                                        into the target vector of all\
                                        of the continuous target vector\
                                        elements used by each category. */\
DGEN   (          int *cattargetcount;                    ,POINTERTYPE,  1) \
                                        /* Pointer to an array of\
                                        integers, one for each category,\
                                        specifying the number of continuous\
                                        target vector\
                                        elements used by each category\
                                        minus 1 (so it is all set to be\
                                        a loop counter). */\
DGEN   (          int *phsseq_count;                      ,POINTERTYPE,  1) \
                                        /* Pointer to an array that\
                                        gives the number of phase sequence\
                                        descriptors for each category. */\
DGEN   (          int *phase_count;                       ,POINTERTYPE,  1) \
                                        /* Pointer to an array that\
                                        gives the number of shot phase\
                                        descriptors for each category. */\
DGEN   (          struct phs_tmg_seq **phsseqentries;     ,POINTERTYPE,  1) \
                                        /* Pointer to\
                                        an array of pointers to arrays of\
                                        phase sequence descriptors.  There\
                                        is one array of descriptors for each\
                                        category. */\
DGEN   (          struct rtshotphase **phaseentries;      ,POINTERTYPE,  1) \
                                        /* Pointer to\
                                        an array of pointers to arrays of\
                                        shot phase descriptors.  There\
                                        is one array of descriptors for each\
                                        category. */\
DGEN   (          struct rt_memory_region *memory_regions; ,POINTERTYPE, 1) \
                                        /* Pointer to an array of \
                                        memory region descriptors.  There\
                                        is one descriptor for each\
                                        memory region. */\
DGEN   (          int *memory_ints;                       ,POINTERTYPE, 1) \
                                        /* Pointer to an array of \
                                        ints that are addresses of memory\
                                        regions on all cpus. */\
DGEN   (          struct rt_message *rt_messages;         ,POINTERTYPE, 1) \
                                        /* Pointer to an array of \
                                        rt_message descriptors.  There\
                                        is one descriptor for each\
                                        rtmessage. */\
/*\
Others\
*/\
DGEN   (          struct cpu_rt_heap *cpuspecific;        ,POINTERTYPE,  1) \
                                        /* Pointer to the structure\
                                        that contains rt heap pointers used\
					by assembly language routines.  This\
					is a structure that really isn't\
					part of the infrastructure.  It will\
					be removed eventually.\
                                        */\
DGEN   (	  int return_status;                      ,INTTYPE,  1) \
                                        /* Code indicating success\
                                        or some sort of failure mode\
                                        to be returned to the host routine\
                                        after a shot is over. */\
DGEN   (          int baseline_count;                     ,INTTYPE,  1) \
                                        /* Number of data samples\
                                        that should be averaged to get a\
                                        baseline value. */\
DGEN   (          int toggle_mask;                        ,INTTYPE,  1) \
                                        /* zero except for a 1 in the\
                                        bit corresponding to the digital i/o\
                                        board bit that corresponds to the\
                                        watchdog toggle bit for the cpu. */\
DGEN   (          int start_data_acq_offset;              ,INTTYPE,  1) \
                                        /* Offset into the\
                                        function vector of the pointer to\
                                        the routine to be used to start\
                                        data acquisition on this rtcpu. */\
DGEN   (          DMA_REGION *new_dmabuffer;              ,POINTERTYPE,  1) \
                                        /* Location for\
                                        temporary storage of either zero to\
                                        indicate that data structure pointers\
                                        should not be incremented (on the\
                                        current cycle) or non-zero value\
                                        equal to the address of the new dma\
                                        buffer to indicate that data structure\
                                        pointers should be incremented on the\
                                        current cycle to save a set of data.\
                                        */\
DGEN   (          char *rtmsg_log;                        ,POINTERTYPE,  1) \
                                        /* Pointer to a char array\
                                        used to buffer messages from the\
                                        realtime code which are sent to\
                                        the message server by the host\
                                        realtime after the shot. */\
DGEN   (	  unsigned int rtmsg_log_size;            ,INTTYPE,  1) \
                                        /* Size of rtmsg_log region. */\
DGEN   (	  unsigned int rtmsg_log_used;            ,INTTYPE,  1) \
                                        /* Number of bytes used in \
                                        rtmsg_log region. */\
DGEN   (          struct other_cpu_info *other_cpus;      ,POINTERTYPE,  1) \
                                        /* Pointer to \
                                        an array of structures containing\
                                        information on the\
                                        other rt cpus in the system.  The\
                                        structure contains VME bus addresses\
                                        of various locations in the memory\
                                        of the other cpus, etc. */\
DGEN   (          int other_cpu_count;                    ,INTTYPE,  1) \
                                        /* The number of rt cpus in\
                                        the system other than the one\
                                        on which this particular structure\
                                        is located. */\
DGEN   (          int shot_state;                         ,INTTYPE,  1) \
                                        /* The current state of the shot */\
DGEN   (          int *timetostop_flag;                   ,POINTERTYPE,  1) \
                                        /* Pointer to an integer\
                                        held in non-cacheable memory that\
                                        will be set non-zero when it is\
                                        time to stop the feedback control.\
                                        Used by the master to send a flag\
                                        to data acq. slaves. */\
DGEN   (	  unsigned int dtoffset;                  ,INTTYPE,  1) \
                                        /* Offset into the PID \
				         lookup table appropriate for the\
                                         time difference between the current\
                                         cycle and the previous cycle. */\
DGEN   (	  unsigned int duration;                  ,INTTYPE,  1) \
                                        /* The simulated duration\
                                         of a cycle in simulation mode.  Each\
                                         algorithm that wants to force the\
                                         cycle time to be a certain value\
                                         should add the desired time value to\
                                         this value.  This location is then\
                                         used by the data acquisition code to\
                                         decide which data to accept.\
					 */\
DGEN   (	  int simtrigger;                         ,INTTYPE,  1) \
                                        /* Simulated trigger\
					 values used in software test mode. */\
DGEN   (          int datasample_flag;                    ,INTTYPE,  1) \
                                        /* This can be set to 1\
                                         by application code to force the\
                                         data to be sampled, independent\
                                         of whether the specified data sample\
                                         time has been reached. */\
DGEN   (          int piddtmin;                           ,INTTYPE,  1) \
                                        /* The minimum value of dT\
                                         for which data is provided in a\
                                         pid lookup table. */\
DGEN   (          int piddtmax;                           ,INTTYPE,  1) \
                                        /* The maximum value of dT\
                                         for which data is provided in a\
                                         pid lookup table. */\
DGEN   (          int piddtinc;                           ,INTTYPE,  1) \
                                        /* Derived from\
                                         the increment in dT between values\
                                         in a pid lookup table. The value\
                                         here is actually the number of bits\
                                         to shift right in order to divide\
                                         by the increment in dT.  Thus the\
                                         increment in dT is always a power\
                                         of 2 fine scale clock ticks. */\
DGEN   (          int diodata;                            ,INTTYPE,  1) \
                                        /* The most recent data acquired\
                                         by reading the digital input data\
                                         register on the digital i/o board. */\
DGEN   (          int data_valid;                         ,INTTYPE,  1) \
                                        /* Set to 1 when the input data\
                                         is useable. The data is not useable\
                                         before the baseline data is acquired.\
                                         This flag is initialized to zero at\
                                         the beginning of the shot and\
                                         the baseline acquisition algorithm\
                                         sets this flag during the first\
                                         cycle when the valid baseline data\
                                         has been used to generate the values\
                                         in "datavector" & "physicsvector." */\
/*\
The following variables store the numbers, offsets,\
and sizes of the fields in the dma_region.\
*/\
DGEN   (          int offset_times;                       ,INTTYPE,  1) \
                                         /* offset to the times array */\
DGEN   (          int offset_diodata;                     ,INTTYPE,  1) \
                                         /* offset to the digital array */\
DGEN   (          int offset_install;                     ,INTTYPE,  1) \
                                         /* offset to other fields */\
DGEN   (          int numrawdata;                         ,INTTYPE,  1) \
                                         /* number of raw data channels */\
DGEN   (          int numtimes;                           ,INTTYPE,  1) \
                                         /* number of times */\
DGEN   (          int numdiodata;                         ,INTTYPE,  1) \
                                         /* number of digital channels */\
DGEN   (          int size_of_rawdata;                    ,INTTYPE,  1) \
                                         /* size in bytes of a raw value */\
DGEN   (          int size_of_times;                      ,INTTYPE,  1) \
                                         /* size in bytes of a time value */\
DGEN   (          int size_of_diodata;                    ,INTTYPE,  1) \
                                         /* size of a digital value */\
SDEND  (          };     ) /* end of the definition of struct rt_heap_misc */
GEN_rt_heap_misc

/*
----------------------------------------------------------------------
STRUCTURE: rtshotphase
Shot phase descriptor.
----------------------------------------------------------------------
*/
#define GEN_rtshotphase \
SDSTART(   struct rtshotphase {,D_rtshotphase)\
DGEN   (          int index;                        ,INTTYPE, 1)\
                                        /* Phase index */\
DGEN   (          int category;                     ,INTTYPE, 1)\
                                        /* category code for this phase */\
DGEN   (          int algorithm;                    ,INTTYPE, 1)\
                                        /* algorithm code for this phase */\
DGEN   (          int entrybehavior;                ,INTTYPE, 1)\
                                        /* How this phase \
                                           behaves when we start using it */\
DGEN   (          int entry_count;                ,INTTYPE, 1)\
                                        /* The number of times this phase\
					   has been entered.*/\
DGEN   (          unsigned int first_entry_time;    ,INTTYPE, 1)\
                                        /* Absolute time in\
                                           units of phase clock ticks that\
                                           the phase was\
                                           first started during the shot. */\
DGEN   (          unsigned int current_time;        ,INTTYPE, 1)\
                                        /* For a phase \
                                           currently in use, the current time\
                                           with respect to the start of the\
                                           shot phase in units of\
                                           phase clock ticks. */\
DGEN   (          union change_list_entry *change_list_head;\
                                                     ,POINTERTYPE, 1)\
                                        /* Pointer \
                                           to the first entry in \
                                           the change list for this phase */\
DGEN   (          union change_list_entry *change_list_next; \
                                                     ,POINTERTYPE, 1)\
                                        /* Pointer \
                                           to the next data entry in \
                                           the change list for this phase \
                                           that should be read when working\
                                           on the change list.  \
				           (Assembly language version only:\
				           After the\
                                           shot phase has been started, this\
                                           points to a location 16 bytes\
                                           after the start of the next change\
                                           list entry to process.  Those 16\
                                           bytes are cached in the array\
                                           pointed to by change_cache in the\
                                           rt_heap_misc struct.)\
                                           */\
DGEN   (          int change_list_entry_count;        ,INTTYPE, 1)\
					/* The count of unions of type \
					   change_list_entry that are in the \
					   change list. \
					 */ \
DGEN   (          struct mbdata *initial_mb;        ,POINTERTYPE, 1)\
                                        /* Pointer to the array\
                                           of slope/intercept pairs for\
                                           target vector computation that\
                                           should be loaded at the start\
                                           of a shot phase. */\
DGEN   (          float *initial_ct;                ,POINTERTYPE, 1)\
                                        /* Pointer to the array\
                                           of initial values for the\
                                           continuous target vector that\
                                           should be loaded at the start\
                                           of a shot phase. */\
DGEN   (          void *parameter_data;             ,POINTERTYPE, 1)\
                                        /* pointer to the parameter \
                                        data storage area for this shot\
					phase. */\
DGEN   (          int parameter_data_size;          ,INTTYPE, 1)\
                                        /* Size of parameter\
                                           data storage area in bytes. */\
DGEN   (          int maximum_block_order_index;   ,INTTYPE, 1)\
                                        /* The maximum index used for a \
					   parameter data block in this phase.\
					   Note that a block order index is a \
					   value based at 1.  So, \
					   maximum_block_order_index is also \
					   the count of pointers to parameter \
					   data blocks stored at the beginning\
					   of the parameter data area.  If \
					   maximum_block_order_index = 0 \
					   and parameter_data_size > 0 \
					   then the parameter data for this \
					   phase is not block structured. \
                                        */\
DGEN   (          void *scratch_space;              ,POINTERTYPE, 1)\
                                        /* pointer to the scratch \
                                        memory for this shot phase. */\
DGEN   (          int scratch_space_size;           ,INTTYPE, 1)\
                                        /* Size of scratch\
                                           memory in bytes. */\
DGEN   (          void (*phasebeginfcn)(struct rtshotphase *,\
					struct rt_heap_misc *);    \
                                                    ,POINTERTYPE, 1)\
                                              /* Pointer to the function\
                                         called in real time each time this\
                                         shot phase is started at the\
                                         beginning of the phase. Note that\
                                         the second argument will actually\
                                         be the pointer to rtheap but the\
                                         compiler complains about this\
                                         if it is here explicitly because\
                                         rt_heap_misc is not defined yet\
                                         at this point in the file. */\
DGEN   (          void (*phaseenterfcn)(struct rtshotphase *,\
					struct rt_heap_misc *);  \
                                                      ,POINTERTYPE, 1)\
                                              /* Pointer to the function\
                                         called in real time each time this\
                                         shot phase is started at a time\
                                         other than the beginning of the\
                                         phase. Note that  \
                                         the second argument will actually  \
                                         be the pointer to rtheap but the  \
                                         compiler complains about this\
                                         if it is here explicitly because\
                                         rt_heap_misc is not defined yet\
                                         at this point in the file.*/\
SDEND  (                      };                                   )
GEN_rtshotphase

/*
-------------------------------------------------------------------------------
STRUCTURE: cpu_rt_heap:
This structure contains information used by the various assembly language
routines.

Historically, this structure was meant to be unique to each CPU.  It was
provided to allow flexibility to meet unanticipated needs for data that is
specific to each CPU.  As the PCS infrastructure has evolved, it turns out
that data specific to each CPU is better provided as parameter data blocks
in a phase of a particular category.

This structure has ended up being used only to provide input parameters
to assembly language routines.  These assembly language routines are not really
infrastructure, but so far that is where they are located.  Eventually, it
would be better to move these assembly language routines into the
installation specific code and call them from C language routines.  The
information provided in this structure would then the provided as arguments
to the function call.  The information for the arguments would be obtained from
parameter data blocks.

Also, as it turns out, this structure is the same on each CPU.

So, for the time being we will simply treat this structure has part
of the infrastructure and make it the same for each CPU.

This structure contains pointers.  Therefore, there must be a companion
structure that contains offsets to locations within the real time heap.
That structure is defined below.  The structure containing offsets is
initialized in the waveform server and this structure is initialized in
the sc_fill_rtheap routines.

Note: the structure definition here is repeated by the definitions
of a series of offsets in asm_symbols.s so the definitions here must
exactly agree with the offsets defined there.
-------------------------------------------------------------------------------
*/
#define GEN_cpu_rt_heap \
SDSTART(struct cpu_rt_heap {                 ,D_cpu_rt_heap      ) \
DGEN   (	  int pidoffset1;                   ,INTTYPE,  1)  \
	                             /* Offset into the error and P\
                                        vectors of the values for pid \
                                        calculation group 1.  The value here\
                                        should be the actual offset minus\
                                        sizeof(real)\
                                        for the convenience of the pid\
                                        routine. */\
DGEN   (	  int pidoffset2;                   ,INTTYPE,  1)  \
                                     /* Offset into the error and P\
                                        vectors of the values for pid \
                                        calculation group 2.  The value here\
                                        should be the actual offset minus\
                                        sizeof(real)\
                                        for the convenience of the pid\
                                        routine. */\
DGEN   (	  int numpidcalcs1;                 ,INTTYPE,  1)  \
                                     /* Number of elements of the\
                                        error vector passed through the\
                                        pid calculation in group 1. The\
                                        value here should be the actual\
                                        number of elements minus 1 for the\
                                        convenience of the pid routine. */\
DGEN   (	  int numpidcalcs2;                 ,INTTYPE,  1)  \
                                     /* Number of elements of the\
                                        error vector passed through the\
                                        pid calculation in group 2. The\
                                        value here should be the actual\
                                        number of elements minus 1 for the\
                                        convenience of the pid routine. */\
DGEN   (          int numpoutsfast;                 ,INTTYPE,  1)  \
                                     /* Number of "fast" standard\
                                        outputs (with 1 subtracted to make\
                                        it ready to be a loop counter). */\
DGEN   (	  int comoutoffsets;                ,INTTYPE,  1)  \
                                     /* Offset into the intcommand\
                                        and fpcommand vectors of the first\
                                        location for standard outputs. */\
DGEN   (          int numslowpercycle;              ,INTTYPE,  1)  \
                                     /* Number of "slow" standard\
                                        outputs to be processed on each\
                                        control cycle loop\
                                        (with 1 subtracted to make\
                                        it ready to be a loop counter). */\
DGEN   (	  int rvectoroffset;                ,INTTYPE,  1)  \
                                     /* Offset into the error vector\
                                        vector of the first element used to\
                                        hold the error for an F supply (the\
                                        r vector). */\
DGEN   (          pidstore *adpidstorage;           ,POINTERTYPE,  1) \
                                     /* address of array\
                                                     of temporary storage\
                                                     for pid calculations.\
                                                     Used when referencing\
                                                     the full pid storage \
                                                     block. */\
DGEN   (          pidstore *adpidstorage1;          ,POINTERTYPE,  1) \
                                     /* address of temporary \
                                                     storage for pid\
                                                     calculations group 1.\
                                        The value here is the actual value\
                                        minus \
                                        sizeof(pidstore)\
                                        for the convenience of the\
                                        pid routine. */\
DGEN   (          pidstore *adpidstorage2;          ,POINTERTYPE,  1) \
                                     /* address of temporary \
                                                     storage for pid\
                                                     calculations group 2.\
                                        The value here is the actual value\
                                        minus sizeof(pidstore)\
                                        for the convenience of the\
                                        pid routine. */\
DGEN   (          struct pid_lookup **pidtables1;   ,POINTERTYPE,  1) \
                                     /* Pointer to the\
                                        array of pointers for pid lookup\
                                        tables for calculation group 1. */\
DGEN   (          struct pid_lookup **pidtables2;   ,POINTERTYPE,  1) \
                                     /* Pointer to the\
                                        array of pointers for pid lookup\
                                        tables for calculation group 2. */\
DGEN   (	  float *pidgains1;                 ,POINTERTYPE,  1)\
                                     /* Address in the target vector\
                                        of the first of the pid gain values\
                                        for pid calculation group 1. The \
                                        value here should be the address\
                                        minus sizeof(real)\
                                        for the convenience of the\
                                        pid routine. */\
DGEN   (	  float *pidgains2;                 ,POINTERTYPE,  1) \
                                     /* Address in the target vector\
                                        of the first of the pid gain values\
                                        for pid calculation group 2. The \
                                        value here should be the address\
                                        minus \
                                        sizeof(real)\
                                        for the convenience of the\
                                        pid routine. */\
DGEN   (	  float *saturation_values;         ,POINTERTYPE,  1)\
                                     /* Address of the block\
                                        in the float step portion of the\
                                        target vector that contains the\
                                        saturation values for the standard\
                                        outputs. */\
DGEN   (	  unsigned int poutoffset;          ,INTTYPE,  1)  \
                                     /* Offset from the\
                                        beginning of the block of elements\
                                        in the various vectors (target, P,\
                                        fpcommand, intcommand,\
                                        etc.) used for slow \
                                        standard outputs to the next element\
                                        to be processed for output. \
                                        Initialized to point to the last\
                                        element used as a standard output\
                                        (the value counts down during \
                                        processing).*/\
DGEN   (	  unsigned int maxslowoffset;       ,INTTYPE,  1)  \
                                     /* Offset into area of\
                                        P vector used for slow standard \
                                        outputs of the last element\
                                        in the group of slow outputs. \
                                        poutoffset is initialized to this\
                                        value when the processing loop\
                                        gets to the first standard output\
                                        element. */\
DGEN   (          float *target_stdout;             ,POINTERTYPE,  1)\
                                     /* Pointer to the first\
                                        element in the target vector to be\
                                        used for standard outputs. */\
DGEN   (          int p_stdout;                     ,INTTYPE,  1)  \
                                     /* Offset in bytes\
                                        into the the P vector\
                                        of the first element to be\
                                        used for standard outputs. */\
DGEN   (	  int ist_shape_mmatrix;            ,INTTYPE,  1)  \
                                     /* Offset in bytes into\
                                        the target vector (points to a \
                                        location in the integer step portion\
                                        of the target vector) of the location\
                                        that holds the address of the M\
                                        matrix that is currently in use. */\
DGEN   (	  int ist_shape_rmatrix;            ,INTTYPE,  1)  \
                                     /* Offset in bytes into\
                                        the target vector (points to a \
                                        location in the integer step portion\
                                        of the target vector) of the location\
                                        that holds the address of the R\
                                        matrix that is currently in use. */\
DGEN   (          int fst_shape_operating_point;    ,INTTYPE,  1)  \
                                     /* Offset in bytes\
                                        into the target vector (minus 4)\
                                        (points to a\
                                        location in the integer step portion\
                                        of the target vector) of the block\
                                        of locations that hold the chopper\
                                        operating\
                                        point values for each F coil. */\
SDEND  (             };   ) /* end of definition of GEN_cpu_rt_misc */
GEN_cpu_rt_heap

#endif /* end of #ifndef _REALTIME_DATA_ */

#ifdef OFFSET_STRUCTURE /* Included in serverdefs.h after rtheap_offset
			   is defined. */
/*
The structures defined here are companions to other structures defined in
this file but which contain offsets rather than pointers into the real time
possessor heap.  These structures are defined here so that they can be
included into serverdefs.h after the data type rtheap_offset is defined.
*/
/*
----------------------------------------------------------------------
STRUCTURE: rt_heap_misc_offsets
This structure is the analog to the rt_heap_misc structure defined above.
Except, this structure contains offsets into the area of memory allocated for
the PCS real time processor memory heap rather than pointers in the address
space of the real time processor.  This structure is used in the waveform
server to hold the layout of the real time processor memory.  The offsets
contained in this structure are converted into pointers in the rt_heap_misc
structure by the host_realtime process.
----------------------------------------------------------------------
*/
#define GEN_rt_heap_misc_offsets \
SDSTART(struct rt_heap_misc_offsets {           ,D_rt_heap_misc_offsets   ) \
/* \
Misc. important data values. \
*/ \
DGEN   (         int numtargets;                ,INTTYPE,1) \
                                     /* number of elements in the \
                                        "continuous" portion of the \
                                        target vector. For the \
                                        convenience of the code this value is \
                                        (number of elements/8 - 2) */ \
DGEN   (         unsigned int next_shtphs_tick; ,INTTYPE,1) \
                                     /* absolute time \
                                        of the next shot phase clock tick \
                                        in units of fine scale clock ticks. */\
DGEN   (         unsigned int next_target_calc_time; ,INTTYPE,1) \
                                     /* absolute time \
                                        in units of fine scale clock ticks \
                                        at which the target vector should be \
                                        recalculated. */ \
DGEN   (         unsigned int shtphs_tick_interval; ,INTTYPE,1) \
                                     /* length (in \
                                        fine scale clock ticks) of one shot \
                                        phase clock tick. */ \
DGEN   (         unsigned int shtphs_tick_count; ,INTTYPE,1) \
                                     /* The total number \
                                        of shot phase ticks that have passed \
                                        in the shot = current absolute \
                                        time in units of shot phase clock \
                                        ticks. */ \
DGEN   (	 unsigned int category_count_c;          ,INTTYPE,   1) \
                                        /* The number of\
                                         control categories executed on the\
                                         cpu.*/\
DGEN   (         unsigned int category_count_a;   ,INTTYPE,1) \
                                     /* The number of \
                                        control categories executed on the \
                                        cpu minus 1 (so it is all ready to \
                                        be a loop counter for assembly \
					language functions) */ \
DGEN   (         unsigned int sampleincrement;  ,INTTYPE,1) \
                                     /* Time in units of \
                                        fine scale clock ticks between \
                                        samples of raw and calculated data \
                                        that are saved. */ \
DGEN   (         unsigned int numrawsets_left;  ,INTTYPE,1) \
                                     /* Number of samples \
                                        of raw and calculated data left \
                                        to save.  At the beginning of the \
                                        shot this is initialized to the \
                                        number of sets of data to save. \
                                        This value is not decremented \
                                        for the branch test because it is \
                                        necessary to advance the pointers \
                                        one more time than the number of \
                                        sets of samples that are saved. \
                                        There must be memory allocated for \
                                        one more set of data than the number \
                                        written here at the beginning of the \
                                        shot (except for the raw data \
                                        buffer where there must be two more \
                                        buffer locations than the number \
                                        here). */ \
DGEN   (         unsigned int nextsample;       ,INTTYPE,1) \
                                     /* Absolute \
                                        time in units of \
                                        fine scale clock ticks to save \
                                        the next sample of raw and \
                                        calculated data. At the beginning \
                                        of the shot this is initialized \
                                        to the time to save the first \
                                        sample. */ \
DGEN   (         unsigned int software_test_mode; ,INTTYPE,1) \
                                     /* Set to non-zero \
                                        if we are running software test mode. \
                                        */ \
DGEN   (         unsigned int hardware_test_mode; ,INTTYPE,1) \
                                     /* Set to non-zero \
                                        if we are running hardware test mode. \
                                        */ \
DGEN   (         int numchannels;               ,INTTYPE,  1) \
                                     /* The number of input\
					data channels.*/\
DGEN   (         int datasetcount;              ,INTTYPE,1) \
                                     /* The number of input \
                                        data channels adjusted for the \
                                        convenience of the getdata routine: \
                                        value = ((# channels/8) - 2).  Note \
                                        the number of channels must be a \
                                        multiple of 8 for this value to be\
					useful.  In fact, this value is \
					really only relevant to DIII-D and \
					should be considered obsolete at\
					other installations.. */ \
DGEN   (         unsigned int num_memory_regions; ,INTTYPE, 1)\
                                        /* Number of memory regions */\
DGEN   (         unsigned int total_memory_ints;  ,INTTYPE, 1)\
                                        /* Number of memory ints used\
                                           for memory region addresses\
                                           for all cpus */\
DGEN   (         unsigned int num_rt_messages;  ,INTTYPE, 1)\
                                        /* Number of rtmessages */\
DGEN   (         unsigned int fastdatastart;    ,INTTYPE,1) \
                                     /* Absolute \
                                        time to start saving \
                                        the fast sampled data in units of \
                                        fine scale clock ticks. */ \
DGEN   (         unsigned int fastdatasets;     ,INTTYPE,1) \
                                     /* Number of fast data \
                                        sets to save. Memory must be \
                                        allocated for one more entry than \
                                        the value here.*/ \
DGEN   (         int fastdatainterval;          ,INTTYPE,  1) \
                                        /* Time in units of \
                                        fine scale clock ticks between \
                                        samples of fast data \
                                        that are saved. */ \
DGEN   (         int previous_fastdatainterval; ,INTTYPE,  1) \
                                        /* Previous value of \
                                        fast sample save interval. */ \
DGEN   (         unsigned int timetostop;       ,INTTYPE,1) \
                                     /* Absolute \
                                        time to stop feedback \
                                        cycles, in units of fine scale clock \
                                        ticks. */ \
DGEN   (         int csrinit;                   ,INTTYPE,1) \
                                     /* Value to load into the DAD-1 CSR.*/ \
DGEN   (         int combufcount;               ,INTTYPE,1) \
                                     /* The number of entries in the \
                                        communications buffer \
                                        (actually ((number/8)-1) so it \
                                        is all set to be a loop counter \
                                        in a loop in which 8 entries are \
                                        processed at a time.). \
                                        (This count includes the entries that \
                                        are used as shot phase seq. desc. \
                                        areas for each category.) Note that \
                                        the total number of entries must be \
                                        a multiple of 8. */ \
DGEN   (         int saveroutine;               ,INTTYPE,1) \
                                     /* Index of routine allowed to \
                                        write into the alldata structure. */ \
DGEN   (         float maggain;                 ,FLOATTYPE,1) \
                                     /* Magnetics gain multiplier */ \
DGEN   (         int savebasedata;              ,INTTYPE,1) \
                                     /* Set to 1 if the baseline \
                                        data should be archived. */ \
DGEN   (         int dmabuffer_stride;          ,INTTYPE,1) \
                                     /* Length in bytes of a \
                                        buffer for dma input from the \
                                        data acquisition system. */ \
DGEN   (         int fpcommand_stride;          ,INTTYPE,1) \
                                     /* Length in bytes of \
                                        the fp command storage vector. */ \
DGEN   (	 int intcommand_stride;         ,INTTYPE,  1) \
                                        /* Length in bytes of the \
                                        the int command storage vector. */\
DGEN   (         int errorvector_stride;        ,INTTYPE,1) \
                                        /* Length in bytes of the\
                                        error vector. */\
DGEN   (	 int pvector_stride;            ,INTTYPE,1) \
                                        /* Length in bytes of the\
                                        P vector. */\
DGEN   (         int shapevector_stride;        ,INTTYPE,1) \
                                     /* Length in bytes of \
                                        the shape vector. */ \
DGEN   (         int fastdata_stride;           ,INTTYPE,1) \
                                     /* Length in bytes of \
                                        the storage structure \
                                        for fast data. */ \
DGEN   (         int functionvector_count;      ,INTTYPE,1) \
                                     /* Number of elements \
                                        in the function vector minus 1 \
                                        (so it is ready to be a loop counter \
                                        */ \
NDGEN  (         rtheap_offset adtarget;        ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the target vector */ \
NDGEN  (         rtheap_offset pointer_target;  ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* offset to the target vector \
					   containing pointers*/\
NDGEN  (         rtheap_offset addeltat;        ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the deltat array for \
                                        target vector computation */ \
NDGEN  (         rtheap_offset admb;            ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the slope/intercept \
                                        array that provides data \
                                        for continuous target vector \
                                        element computation. */ \
NDGEN  (         rtheap_offset vertextime;      ,NESTEDTYPE,1,D_rtheap_offset)\
                                    /* Offset to the array holding the \
                                        time wrt start of the shot phase \
                                        in units of shot phase ticks \
                                        of the last target vector vertex. */ \
                                         \
NDGEN  (         rtheap_offset adcombuffer;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to an array of \
                                        floats that can be written by \
                                        any cpu for communication between \
                                        cpus.  This buffer is always read \
                                        with pfld instructions. \
                                        This array should be written, but \
                                        never read except by specific code \
                                        that executes the pfld instructions. \
                                        */ \
NDGEN  (         rtheap_offset adcombufloop;    ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to an array of \
                                        floats into which the content \
                                        of combuffer is read on each control \
                                        cycle. This is the array that should \
                                        be read by code needing the content \
                                        of combuffer. */ \
NDGEN  (         rtheap_offset dmabuffer;       ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to \
                                        the buffer that \
                                        will receive the data from the \
                                        data acquisition board. \
                                        */ \
NDGEN  (         rtheap_offset baseline;        ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the buffer that holds \
                                        the baseline values for the \
                                        input data channels. */ \
NDGEN  (         rtheap_offset datavector;      ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the vector that \
                                        holds the current set of data \
                                        after the baseline is \
                                        subtracted. */ \
NDGEN  (         rtheap_offset physicsvector;   ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the vector that \
                                        holds the current set of data \
                                        after the baseline is subtracted and \
                                        converted to physics units. */ \
NDGEN  (         rtheap_offset shapevector;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the shape \
                                        vector */ \
NDGEN  (         rtheap_offset errorvector;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the error vector. \
                                        */ \
NDGEN  (         rtheap_offset fpcommand;       ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the float command \
                                        vector. */ \
NDGEN  (         rtheap_offset intcommand;      ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the int command \
                                        vector. */ \
NDGEN  (         rtheap_offset pvector;         ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the P vector. */ \
NDGEN  (         rtheap_offset pidlookup;       ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the the first pid\
                                        lookup table. */\
NDGEN  (         rtheap_offset pidtauvector;    ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        the pidtau \
                                        vector. Each element in \
                                        this vector points to \
                                        a pid lookup table. */ \
NDGEN  (         rtheap_offset functionvector;  ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an array \
                                        of pointers to the functions to be \
                                        executed on each control cycle. */ \
                                         \
NDGEN  (         rtheap_offset previous_shapevector; ,NESTEDTYPE,1,\
                                                              D_rtheap_offset) \
                                     /* Offset to the shape \
                                        vector on the previous cycle \
                                        */ \
NDGEN  (         rtheap_offset previous_errorvector; ,NESTEDTYPE,1,\
                                                              D_rtheap_offset) \
                                     /* Offset to the \
                                        error vector on the previous cycle. */ \
NDGEN  (         rtheap_offset previous_fpcommand; ,NESTEDTYPE,1,\
                                                              D_rtheap_offset) \
                                     /* Offset to the float \
                                        command vector on the previous cycle. \
                                        */ \
NDGEN   (        rtheap_offset previous_intcommand; ,NESTEDTYPE,1,\
                                                              D_rtheap_offset) \
                                     /* Offset to the int \
                                        command vector on the previous cycle */\
NDGEN  (         rtheap_offset previous_pvector; ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the P \
                                        vector on the previous cycle. */ \
NDGEN  (         rtheap_offset alldata;         ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the \
                                        current storage location for fast \
                                        sampled data. */ \
NDGEN  (         rtheap_offset install_buffer;  ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to the buffer for \
                                        installation dependent data.  This \
                                        is a space that the installation \
                                        provides (by setting the macro \
                                        INSTALL_BUFFER_SIZE) for a cpu to \
                                        write to that is installation \
                                        dependent. */ \
NDGEN   (         rtheap_offset last_int;        ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to the last int in \
                                        the rt cpu memory that is used. A \
                                        check value is written at that \
                                        location. */ \
NDGEN  (         rtheap_offset phsseq_stack;    ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        space to \
                                        store a record of all changes made \
                                        in the phase sequence. \
                                        */ \
DGEN   (         int phsseq_stack_size;         ,INTTYPE,1) \
                                     /* Count of the number \
                                        of structs of type alterphsseq that \
                                        can fit into the area allocated for \
                                        the phase sequence stack. \
                                        */ \
NDGEN  (         rtheap_offset phase_stack;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        space to \
                                        store a record of all changes made \
                                        in the shot phase. \
                                        */ \
DGEN   (         int phase_stack_size;          ,INTTYPE,1) \
                                     /* Count of the number \
                                        of structs of type alterphase that \
                                        can fit into the area allocated for \
                                        the phase change stack. \
                                        */ \
/* \
Pointers to arrays that have one element for each category \
controlled by the cpu. \
*/ \
NDGEN  (         rtheap_offset current_phs_seq; ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset \
                                        to an array of pointers to phase \
                                        timing sequence structures. There \
                                        is one element in the array for \
                                        each category controlled on the cpu. \
                                        This array points to the current \
                                        phase timing sequence descriptor for \
                                        each category. */ \
NDGEN   (        rtheap_offset current_phs_seq_number; \
                                                ,NESTEDTYPE,1,D_rtheap_offset) \
                                        /* Offset\
                                         to an array of integers,\
                                         one for\
                                         each category controlled on the cpu.\
                                         This array gives the number \
					 (based at 1) of the\
                                         phase timing sequence currently in use\
                                         for each category. */\
NDGEN  (         rtheap_offset current_phase;   ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* offset\
                                         to an array of pointers to phase\
                                         descriptors. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array points to the descriptor\
					 for the phase currently in use\
					 for each\
					 category.*/\
NDGEN   (        rtheap_offset phase_init_function;\
                                                ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* offset\
                                         to an array of pointers to phase\
                                         initialize functions. There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
                                         This array points to the function\
					 which gets executed when a phase\
					 sequence change occurs.*/\
NDGEN  (         rtheap_offset current_parameter_data;\
                                                ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* offset to an array of pointers\
					   to  parameter data storage areas.\
                                         There\
                                         is one element in the array for\
                                         each category controlled on the cpu.\
					 The parameter data area indicated\
					 by each pointer is the one belonging\
					 to the phase currently in use for \
					 the corresponding category.\
					 At the beginning of\
					 each parameter data area is an array\
					 of pointers to the individual\
					 parameter data blocks stored in\
					 that area.*/\
NDGEN  (         rtheap_offset change_cache;    ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an array with \
                                        space for 4 floats from each category. \
                                        This space is used to store the next \
                                        4 data values from the change list, \
                                        values that have already been read \
                                        in with pfld. */ \
NDGEN  (         rtheap_offset targetvector_map; ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Offset to an array \
                                        of pointers, one for each category, \
                                        to arrays which contain the \
                                        offsets in bytes \
                                        into the target vector of all \
                                        of the continuous target vector \
                                        elements used by each category. */ \
NDGEN  (         rtheap_offset cattargetcount;  ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an array of \
                                        integers, one for each category, \
                                        specifying the number of continuous \
                                        target vector \
                                        elements used by each category \
                                        minus 1 (so it is all set to be \
                                        a loop counter). */ \
NDGEN  (         rtheap_offset phsseq_count;    ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an array that \
                                        gives the number of phase sequence \
                                        descriptors for each category. */ \
NDGEN  (         rtheap_offset phase_count;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an array that \
                                        gives the number of shot phase \
                                        descriptors for each category. */ \
NDGEN  (         rtheap_offset phsseqentries;   ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        an array of pointers to arrays of \
                                        phase sequence descriptors.  There \
                                        is one array of descriptors for each \
                                        category. */ \
NDGEN  (         rtheap_offset phaseentries;    ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        an array of pointers to arrays of \
                                        shot phase descriptors.  There \
                                        is one array of descriptors for each \
                                        category. */ \
NDGEN  (         rtheap_offset memory_regions;  ,NESTEDTYPE,1,D_rtheap_offset) \
                                        /* Offset to an array of \
                                        memory region descriptors.  There\
                                        is one descriptor for each\
                                        memory region. */\
NDGEN   (        rtheap_offset memory_ints;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                        /* Offset to an array of \
                                        ints that are addresses of memory\
                                        regions on all cpus. */\
NDGEN   (        rtheap_offset rt_messages;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                        /* Offset to an array of \
                                        rt_message descriptors.  There\
                                        is one descriptor for each\
                                        rtmessage. */\
                                     /* \
                                        Others \
                                        */ \
NDGEN  (         rtheap_offset cpuspecific;     ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Pointer to the structure \
                                        that contains rt heap pointers used\
                                        by assembly language routines.  This\
                                        is a structure that really isn't\
                                        part of the infrastructure.  It will\
                                        be removed eventually.\
                                        */\
DGEN   (         int baseline_count;            ,INTTYPE,1) \
                                     /* Number of data samples \
                                        that should be averaged to get a \
                                        baseline value. */ \
DGEN   (         int toggle_mask;               ,INTTYPE,1) \
                                     /* zero except for a 1 in the \
                                        bit corresponding to the digital i/o \
                                        board bit that corresponds to the \
                                        watchdog toggle bit for the cpu. */ \
DGEN   (         int start_data_acq_offset;     ,INTTYPE,1) \
                                     /* Offset into the \
                                        function vector of the pointer to \
                                        the routine to be used to start \
                                        data acquisition on this rtcpu. */ \
DGEN   (         rtheap_offset rtmsg_log;       ,INTTYPE,  1) \
                                        /* Offset to a char array\
                                        used to buffer messages from the\
                                        realtime code which are sent to\
                                        the message server by the host\
                                        realtime after the shot. */\
DGEN   (	 unsigned int rtmsg_log_size;   ,INTTYPE,  1) \
                                        /* Size of rtmsg_log region. */\
DGEN   (	 unsigned int rtmsg_log_used;   ,INTTYPE,  1) \
                                        /* Number of bytes used in \
                                        rtmsg_log region. */\
NDGEN  (         rtheap_offset other_cpus;      ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to \
                                        an array of structures containing \
                                        information on the \
                                        rt cpus in the system.  The \
                                        structure contains VME bus addresses \
                                        of various locations in the memory \
                                        of the other cpus, etc. */ \
DGEN   (         int other_cpu_count;           ,INTTYPE,1) \
                                     /* The number of rt cpus in \
                                        the system. \
                                        */ \
DGEN   (         int shot_state;                ,INTTYPE,  1) \
                                        /* The current state of the shot */\
NDGEN  (         rtheap_offset timetostop_flag; ,NESTEDTYPE,1,D_rtheap_offset) \
                                     /* Offset to an integer \
                                        held in non-cacheable memory that \
                                        will be set non-zero when it is \
                                        time to stop the feedback control. \
                                        Used by the master to send a flag \
                                        to data acq. slaves. */ \
DGEN   (         int piddtmin;                  ,INTTYPE,1) \
                                     /* The minimum value of dT \
                                        for which data is provided in a \
                                        pid lookup table. */ \
DGEN   (         int piddtmax;                  ,INTTYPE,1) \
                                     /* The maximum value of dT \
                                        for which data is provided in a \
                                        pid lookup table. */ \
DGEN   (         int piddtinc;                  ,INTTYPE,1) \
                                     /* Derived from \
                                        the increment in dT between values \
                                        in a pid lookup table. The value \
                                        here is actually the number of bits \
                                        to shift right in order to divide \
                                        by the increment in dT.  Thus the \
                                        increment in dT is always a power \
                                        of 2 fine scale clock ticks. */ \
DGEN   (         int bs_c_phsseq;               ,INTTYPE,1) \
                                     /* The index in the communication buffer \
                               of the first element containing the list of \
                               pointers to phase sequence descriptors. \
                               This list specifies for each category the phase \
                               sequence to be used on the next cycle. \
                               Note that the communication buffer is an array \
                               of 4 byte values so that this index must take \
                               account the alignment requirement for a \
                               pointer if the pointer is to be on a \
                               64 bit machine. */ \
/*\
The following variables store the numbers, offsets,\
and sizes of the fields in the dma_region.\
*/\
DGEN   (          int offset_times;                       ,INTTYPE,  1) \
                                         /* offset to the times array */\
DGEN   (          int offset_diodata;                     ,INTTYPE,  1) \
                                         /* offset to the digital array */\
DGEN   (          int offset_install;                     ,INTTYPE,  1) \
                                         /* offset to other fields */\
DGEN   (          int numrawdata;                         ,INTTYPE,  1) \
                                         /* number of raw data channels */\
DGEN   (          int numtimes;                           ,INTTYPE,  1) \
                                         /* number of times */\
DGEN   (          int numdiodata;                         ,INTTYPE,  1) \
                                         /* number of digital channels */\
DGEN   (          int size_of_rawdata;                    ,INTTYPE,  1) \
                                         /* size in bytes of a raw value */\
DGEN   (          int size_of_times;                      ,INTTYPE,  1) \
                                         /* size in bytes of a time value */\
DGEN   (          int size_of_diodata;                    ,INTTYPE,  1) \
                                         /* size of a digital value */\
SDEND  (                                        }; )
GEN_rt_heap_misc_offsets
/*
-------------------------------------------------------------------------------
STRUCTURE: cpu_rt_heap_offsets:
This structure is the analog to the cpu_rt_heap structure defined above.
Except, this structure contains offsets into the area of memory allocated for
the PCS real time processor memory heap rather than pointers in the address
space of the real time processor.  This structure is used in the waveform
server.  The offsets
contained in this structure are converted into pointers
by the host_realtime process.
-------------------------------------------------------------------------------
*/
#define GEN_cpu_rt_heap_offsets \
SDSTART(struct cpu_rt_heap_offsets {            ,D_cpu_rt_heap_offsets ) \
DGEN   (	  int pidoffset1;                   ,INTTYPE,  1)  \
	                             /* Offset into the error and P\
                                        vectors of the values for pid \
                                        calculation group 1.  The value here\
                                        should be the actual offset minus\
                                        sizeof(real)\
                                        for the convenience of the pid\
                                        routine. */\
DGEN   (	  int pidoffset2;                   ,INTTYPE,  1)  \
                                     /* Offset into the error and P\
                                        vectors of the values for pid \
                                        calculation group 2.  The value here\
                                        should be the actual offset minus\
                                        sizeof(real)\
                                        for the convenience of the pid\
                                        routine. */\
DGEN   (	  int numpidcalcs1;                 ,INTTYPE,  1)  \
                                     /* Number of elements of the\
                                        error vector passed through the\
                                        pid calculation in group 1. The\
                                        value here should be the actual\
                                        number of elements minus 1 for the\
                                        convenience of the pid routine. */\
DGEN   (	  int numpidcalcs2;                 ,INTTYPE,  1)  \
                                     /* Number of elements of the\
                                        error vector passed through the\
                                        pid calculation in group 2. The\
                                        value here should be the actual\
                                        number of elements minus 1 for the\
                                        convenience of the pid routine. */\
DGEN   (          int numpoutsfast;                 ,INTTYPE,  1)  \
                                     /* Number of "fast" standard\
                                        outputs (with 1 subtracted to make\
                                        it ready to be a loop counter). */\
DGEN   (	  int comoutoffsets;                ,INTTYPE,  1)  \
                                     /* Offset into the intcommand\
                                        and fpcommand vectors of the first\
                                        location for standard outputs. */\
DGEN   (          int numslowpercycle;              ,INTTYPE,  1)  \
                                     /* Number of "slow" standard\
                                        outputs to be processed on each\
                                        control cycle loop\
                                        (with 1 subtracted to make\
                                        it ready to be a loop counter). */\
DGEN   (	  int rvectoroffset;                ,INTTYPE,  1)  \
                                     /* Offset into the error vector\
                                        vector of the first element used to\
                                        hold the error for an F supply (the\
                                        r vector). */\
NDGEN  (          rtheap_offset adpidstorage;   ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* address of array\
                                                     of temporary storage\
                                                     for pid calculations.\
                                                     Used when referencing\
                                                     the full pid storage \
                                                     block. */\
NDGEN  (          rtheap_offset adpidstorage1;  ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* address of temporary \
                                                     storage for pid\
                                                     calculations group 1.\
                                        The value here is the actual value\
                                        minus \
                                        sizeof(pidstore)\
                                        for the convenience of the\
                                        pid routine. */\
NDGEN  (          rtheap_offset adpidstorage2;  ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* address of temporary \
                                                     storage for pid\
                                                     calculations group 2.\
                                        The value here is the actual value\
                                        minus sizeof(pidstore)\
                                        for the convenience of the\
                                        pid routine. */\
NDGEN  (          rtheap_offset pidtables1;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Pointer to the\
                                        array of pointers for pid lookup\
                                        tables for calculation group 1. */\
NDGEN  (          rtheap_offset pidtables2;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Pointer to the\
                                        array of pointers for pid lookup\
                                        tables for calculation group 2. */\
NDGEN  (	  rtheap_offset pidgains1;      ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Address in the target vector\
                                        of the first of the pid gain values\
                                        for pid calculation group 1. The \
                                        value here should be the address\
                                        minus sizeof(real)\
                                        for the convenience of the\
                                        pid routine. */\
NDGEN  (	  rtheap_offset pidgains2;      ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Address in the target vector\
                                        of the first of the pid gain values\
                                        for pid calculation group 2. The \
                                        value here should be the address\
                                        minus \
                                        sizeof(real)\
                                        for the convenience of the\
                                        pid routine. */\
NDGEN   (      rtheap_offset saturation_values; ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Address of the block\
                                        in the float step portion of the\
                                        target vector that contains the\
                                        saturation values for the standard\
                                        outputs. */\
DGEN   (	  unsigned int poutoffset;          ,INTTYPE,  1)  \
                                     /* Offset from the\
                                        beginning of the block of elements\
                                        in the various vectors (target, P,\
                                        fpcommand, intcommand,\
                                        etc.) used for slow \
                                        standard outputs to the next element\
                                        to be processed for output. \
                                        Initialized to point to the last\
                                        element used as a standard output\
                                        (the value counts down during \
                                        processing).*/\
DGEN   (	  unsigned int maxslowoffset;       ,INTTYPE,  1)  \
                                     /* Offset into area of\
                                        P vector used for slow standard \
                                        outputs of the last element\
                                        in the group of slow outputs. \
                                        poutoffset is initialized to this\
                                        value when the processing loop\
                                        gets to the first standard output\
                                        element. */\
NDGEN  (          rtheap_offset target_stdout;  ,NESTEDTYPE,1,D_rtheap_offset)\
                                     /* Pointer to the first\
                                        element in the target vector to be\
                                        used for standard outputs. */\
DGEN   (          int p_stdout;                     ,INTTYPE,  1)  \
                                     /* Offset in bytes\
                                        into the the P vector\
                                        of the first element to be\
                                        used for standard outputs. */\
DGEN   (	  int ist_shape_mmatrix;            ,INTTYPE,  1)  \
                                     /* Offset in bytes into\
                                        the target vector (points to a \
                                        location in the integer step portion\
                                        of the target vector) of the location\
                                        that holds the address of the M\
                                        matrix that is currently in use. */\
DGEN   (	  int ist_shape_rmatrix;            ,INTTYPE,  1)  \
                                     /* Offset in bytes into\
                                        the target vector (points to a \
                                        location in the integer step portion\
                                        of the target vector) of the location\
                                        that holds the address of the R\
                                        matrix that is currently in use. */\
DGEN   (          int fst_shape_operating_point;    ,INTTYPE,  1)  \
                                     /* Offset in bytes\
                                        into the target vector (minus 4)\
                                        (points to a\
                                        location in the integer step portion\
                                        of the target vector) of the block\
                                        of locations that hold the chopper\
                                        operating\
                                        point values for each F coil. */\
SDEND  (             };   ) /* end of definition of GEN_cpu_rt_misc_offsets */
GEN_cpu_rt_heap_offsets

/*
----------------------------------------------------------------------
STRUCTURE: phsseq_change_offsets
This structure is the companion to the structure of type phsseq_change
that was defined above. This structure defines a 
phase sequence change list entry.
This is the structure transferred from
the waveform server.  It contains the index of the shot phase among all of
the phases actually used for a particular category
rather than a pointer, as is the case for the structure
of type phsseq_change defined above.
----------------------------------------------------------------------
*/
#define GEN_phsseq_change_offsets \
SDSTART(struct phsseq_change_offsets {          ,D_phsseq_change_offsets)\
DGEN   (          unsigned int time;            ,INTTYPE,       1) \
                                     /* Time with respect to the start\
                                        of the phase sequence that the change\
                                        is to take place in units of phase\
                                        clock ticks. */\
DGEN  (           int phase;                    , INTTYPE,1)\
                                     /* Index of the shot phase, among all of \
					the phases actually used for a \
					particular category, \
                                        to change to at the specified\
                                        time */\
SDEND  (                        };                                      )
GEN_phsseq_change_offsets


/*
----------------------------------------------------------------------
STRUCTURE: rtshotphase_offsets
This structure is the companion to the structure of type rtshotphase
that was defined above.
This is the structure used to transfer data from
the waveform server.  It contains offsets into the real time processor heap
rather than pointers, as is the case for the structure
of type rtshotphase defined above.
----------------------------------------------------------------------
*/
#define GEN_rtshotphase_offsets \
SDSTART(   struct rtshotphase_offsets {,D_rtshotphase_offsets)\
DGEN   (          int index;                        ,INTTYPE, 1)\
                                        /* Phase index */\
DGEN   (          int category;                     ,INTTYPE, 1)\
                                        /* category code for this phase */\
DGEN   (          int algorithm;                    ,INTTYPE, 1)\
                                        /* algorithm code for this phase */\
DGEN   (          int entrybehavior;                ,INTTYPE, 1)\
                                        /* How this phase \
                                           behaves when we start using it */\
DGEN   (          float anchor_time;                ,FLOATTYPE, 1)\
                                        /* Time in seconds that the phase \
                                           is anchored */\
NDGEN  (          rtheap_offset parameter_data;     ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* pointer to the parameter \
                                        data for this shot phase. */\
DGEN   (          int parameter_data_size;          ,INTTYPE, 1)\
                                        /* Size of parameter\
                                           data in bytes. */\
NDGEN  (          rtheap_offset scratch_space;      ,NESTEDTYPE,1,D_rtheap_offset)\
                                        /* pointer to the scratch \
                                        memory for this shot phase. */\
DGEN   (          int scratch_space_size;           ,INTTYPE, 1)\
                                        /* Size of scratch\
                                           memory in bytes. */\
SDEND  (                      };                                   )
GEN_rtshotphase_offsets

#endif /* End of #ifdef OFFSET_STRUCTURE */
