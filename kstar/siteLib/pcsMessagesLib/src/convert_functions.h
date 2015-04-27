/* @(#)convert_functions.h	1.5 09/26/02
******************************************************************************
FILE: convert_functions.h

Definitions used by the PCS of routines for converting data between formats
used by different processors.
------------------------------------------------------------------------------
Modifications:
5/17/2000: created by J. Ferron
******************************************************************************
*/
#include "descriptors.h"
#include "processor_type.h"
#include "swap_functions.h"

#ifndef _CONVERT_FUNCTIONS_
#define _CONVERT_FUNCTIONS_
/*
------------------------------------------------------------------------------
Definitions for use with the convert_functions.c routines.
------------------------------------------------------------------------------
*/
#define TASK_ALL 0
#define TASK_STRUCTURE 1
#define TASK_UNION 2
#define SOURCE_OFFSET 0
#define TARGET_OFFSET 1
/*
-----------------------------------------------------------
Flags for use with the "setalignment" function.  The value here specifies
the number of least significant bits that should be zero in order to
obtain the desired alignment.
-----------------------------------------------------------
*/
#define ALIGN1  0      /* byte alignment */
#define ALIGN2  1      /* 2 byte alignment */
#define ALIGN4  2      /* 4 byte alignment */
#define ALIGN8  3      /* 8 byte alignment */
#define ALIGN16 4      /* 16 byte alignment */
#define ALIGN32 5      /* 32 byte alignment */
#define ALIGN64 6      /* 64 byte alignment */
#define ALIGN4K 12     /* 4KByte alignment */

/*
------------------------------------------------------------------------------
MACRO: CONVERT_SIZE
Macro to simplify the call to convert_size for the way it is used  by most
functions.

To use this macro, the function must have defined:
int object_size, object_alignment;
processor_type_mask processor;

processor must be set to the processor type mask.  Object_size and
object_alignment are used as return value variables.
------------------------------------------------------------------------------
*/

#define CONVERT_SIZE(descriptor) convert_size(descriptor, 0,\
					      processor, \
					      TASK_ALL, TARGET_OFFSET,\
					      NULL, \
					      &object_size, &object_alignment);
/*
-----------------------------------------------------------
Function prototypes
-----------------------------------------------------------
*/
extern int convert_get_size(int type, processor_type_mask target);

extern int convert_offsets(STRUCT_DESCRIPTORS *descriptor_in,
			   DESCRIPTOR_OFFSETS **offsets_out);

extern char *convert_data(char *data, STRUCT_DESCRIPTORS *descriptor_in, 
			  int count,
			  processor_type_mask source, 
			  processor_type_mask target, 
			  char *output_in,
			  int task,
			  DESCRIPTOR_OFFSETS **offset_in,
			  int *size_output, int *alignment_output,
			  int *source_size_output);

extern void convert_size(STRUCT_DESCRIPTORS *descriptor_in, int count,
			 processor_type_mask processor, 
			 int task, int offset_index,
			 DESCRIPTOR_OFFSETS **offset_output,
			 int *size_output, int *alignment_output);

#endif /* end of #ifndef _CONVERT_FUNCTIONS_ */
