/* @(#)processor_type.h	1.3 08/08/00
******************************************************************************
FILE: processor_type.h

Definitions specifying the type of a processor.
------------------------------------------------------------------------------
Modifications:
5/17/2000: created by J. Ferron
******************************************************************************
*/
#ifndef _PROCESSOR_TYPE_
#define _PROCESSOR_TYPE_
/*
------------------------------------------------------------------------------
Define the macros for descriptor generation.  This first definition
defines the macros so that they simply generate the variable
definition code.
------------------------------------------------------------------------------
*/
#define DESCRIPTORS_PART_1
#include "descriptors.h"
#undef DESCRIPTORS_PART_1

/*
------------------------------------------------------------------------------
Processor type attributes
Each is a mask for a bit indicating a particular property.  If the bit
is set the processor has the specified property.
------------------------------------------------------------------------------
*/
#define LITTLEENDIAN 1 /* Processor is little endian when bit is set. */
#define BITS64 2       /* Processor has 64 bit attributes if bit is set. */
#define ALIGN8ON8 4    /* 8 byte quantities are aligned on 8 byte boundaries
			  if bit is set. */
/*
------------------------------------------------------------------------------
TYPEDEF: processor_type_mask

The data type chosen to represent the processor type must be chosen as
an integer type that has identical length on each of the possible types
of processor.

Here we define the type for a processor type mask to be an int.
This definition must be coordinated with conversion macros defined in
socketcom.h.
------------------------------------------------------------------------------
*/
#define GEN_processor_type_mask \
DSTART(                                              D_processor_type_mask)\
DGEN(typedef int processor_type_mask;             ,INTTYPE, 1)\
DEND
GEN_processor_type_mask
/*
------------------------------------------------------------------------------
Function prototypes.
------------------------------------------------------------------------------
*/
extern void get_processor (processor_type_mask *typ);


#endif /* end of #ifndef _PROCESSOR_TYPE_ */
