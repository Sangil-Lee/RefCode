/* @(#)descriptors.h	1.14 08/02/01
descriptors.h
This file contains macros that support the data object descriptor code.

First, there is a section of basic definitions.

Then, macros are defined that are useful for maintaining data 
object descriptors. These macros
can be used to simultaneously define a structure or
typedef and the corresponding descriptor. Also there are macros convenient
for printing information about descriptors.

See the comments in the file convert_functions.c for an example of the
use of these definitions.

Avoid the following typical mistakes in your macro definitions:
1.  Multiple line definitions have a backslash at the end of each line.
2.  No extra characters, particularly spaces, after the backslash.
3.  No blank lines.

Edit history:
2/22/00: created by J. Ferron
*/
#if !defined(_DESCRIPTORS_) && !defined(DESCRIPTORS_PART_1) && \
                               !defined(DESCRIPTORS_PART_2) && \
                               !defined(DESCRIPTORS_PART_3)
#define _DESCRIPTORS_

#include "processor_type.h"

/*
------------------------------------------------------------------------------
Definitions of fundamental data types contained in data objects.
These definitions are used in data object descriptors.
Data structure descriptors are used for describing parameter data
blocks and other structures for archiving and for transport between
the various processes.
------------------------------------------------------------------------------
*/
#define CHARTYPE      1 /* Types with numbers 1 through 5 have been used
			   for archived code.  Therefor the meaning of
			   these type codes shouldn't be changed. */
#define SHORTTYPE     2 /* |                                          */
#define INTTYPE       3 /* |                                          */
#define LONG32TYPE    4 /* |                                          */
#define FLOATTYPE     5 /* |                                          */
			/* ------------------------------------------ */
#define DOUBLETYPE    6
#define LONGTYPE      7
#define POINTERTYPE   8
#define STRUCTURETYPE 9
#define STRUCTUREEND  10
#define NESTEDTYPE    11
#define ENDOFDESCRIPTOR 12
#define UNIONTYPE     13
#define UNIONEND      14
#define UNKNOWNTYPE   15

/*
------------------------------------------------------------------------------
Type definitions: STRUCT_DESCRIPTORS, DESCRIPTOR_OFFSETS, ARCHIVE_DESCRIPTOR

Structures defining elements of a dataobject.  
An array of STRUCT_DESCRIPTORS
is used to form the descriptor for a data object.  
This descriptor is used
for archiving parameter data and for transporting data structures 
between processes. The other structures are used in manipulating 
data object descriptors.
------------------------------------------------------------------------------
*/
/*
This next descriptor structure is for use with unrolled descriptors used
by the parameter data archive and restore routines.
*/
struct data_descriptor_archive {
   int type;   /* One of the ...TYPE macros that specify the data type. */
   int size;   /* The count of data values of type "type". */
   int offset; /* Offset into a data region where the data object is located.*/
};

typedef struct data_descriptor_archive ARCHIVE_DESCRIPTOR;

struct data_descriptor {
   int type;   /* One of the ...TYPE macros that specify the data type. */
   int size;   /* The count of data values of type "type". */
   struct data_descriptor *next; /* Pointer to a "nested" descriptor. 
				Ignored if the type value isn't NESTEDTYPE. */
};

typedef struct data_descriptor STRUCT_DESCRIPTORS;

typedef struct {
   int offset[2]; /* The offset into the data object
		     of the data described by a
		  descriptor.  This offset is filled in by the data
		  conversion routine convert_size. The 2 elements of the array
		  are for the offsets on the source processor
		  (index = SOURCE_OFFSET) and the offsets on the target
		  processor (index = TARGET_OFFSET).
	       */
   int size[2]; /* The size of a particular element of a data object.
		  This value is filled in by the data
		  conversion routine convert_size. The 2 elements of the array
		  are for the size on the source processor
		  (index = SOURCE_OFFSET) and the size on the target
		  processor (index = TARGET_OFFSET).
	       */
} DESCRIPTOR_OFFSETS;  

/*
------------------------------------------------------------------------------
Macros for generating general usage descriptors for an array of values
of a particular data type.  These are meant to be used to generate a
descriptor, not for declaring the variable.  Thus the variable name is a
dummy.
------------------------------------------------------------------------------
*/

#define GEN_char \
DSTART (                                          D_char            )\
DGEN   (                     char char_dummy;     ,CHARTYPE,       1)\
DEND

#define GEN_short \
DSTART (                                          D_short           )\
DGEN   (                     short short_dummy;   ,SHORTTYPE,      1)\
DEND

#define GEN_int \
DSTART (                                          D_int             ) \
DGEN   (                     int int_dummy;       ,INTTYPE,        1)\
DEND

#define GEN_long \
DSTART (                                          D_long             ) \
DGEN   (                     long long_dummy;     ,LONGTYPE,        1)\
DEND

#define GEN_float \
DSTART (                                          D_float           )\
DGEN   (                     float float_dummy;   ,FLOATTYPE,      1)\
DEND

#define GEN_double \
DSTART (                                          D_double           )\
DGEN   (                     double double_dummy; ,DOUBLETYPE,      1)\
DEND

#define GEN_pointer \
DSTART (                                          D_pointer         )\
DGEN   (                     int *pointer_dummy;  ,POINTERTYPE,    1)\
DEND

/*
Macro used to print a summary of a structure descriptor.
Given a structure defined with its descriptor, structure name "example",
this code:

#define DESCRIPTORS_PART_2
#include "descriptors.h"
#undef DESCRIPTORS_PART_2

GEN_example

#define DESCRIPTORS_PART_3
#include "descriptors.h"
#undef DESCRIPTORS_PART_3

structure_summary(example) 

prints a summary the structure and its
sizes on various machines.
*/
#define structure_summary(type) \
   printf("**************************************\n");\
   printf(#type ":\n"\
	  "Size on this host: %d\n",\
	  sizeof(struct type));\
   processor = ALIGN8ON8;\
   CONVERT_SIZE(D_##type)\
   printf("On Sun/i860: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = ALIGN8ON8 | BITS64 | LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Alpha: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Intel: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   GEN_##type

#define object_summary(type) \
   printf("**************************************\n");\
   printf(#type ":\n"\
	  "Size on this host: %d\n",\
	  sizeof(type));\
   processor = ALIGN8ON8;\
   CONVERT_SIZE(D_##type)\
   printf("On Sun/i860: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = ALIGN8ON8 | BITS64 | LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Alpha: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Intel: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   GEN_##type

#define special_summary(type) \
   printf("**************************************\n");\
   printf(#type ":\n"\
	  );\
   processor = ALIGN8ON8;\
   CONVERT_SIZE(D_##type)\
   printf("On Sun/i860: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = ALIGN8ON8 | BITS64 | LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Alpha: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   processor = LITTLEENDIAN;\
   CONVERT_SIZE(D_##type)\
   printf("On Intel: size=%d, alignment=%d\n",\
	  object_size,object_alignment);\
   GEN_##type

/*
Function prototypes.
*/
extern ARCHIVE_DESCRIPTOR *descriptor_unrolled_copy(
					    STRUCT_DESCRIPTORS *descriptor_in,
					    DESCRIPTOR_OFFSETS *offsets_in,
					    processor_type_mask processor,
					    ARCHIVE_DESCRIPTOR **output_in,
					    int task, int location_in,
					    int *count_out
					    );
extern void descriptor_unrolled_print(ARCHIVE_DESCRIPTOR *descriptor_in,
				      int count);

int descriptor_count(STRUCT_DESCRIPTORS *descriptor_in);
STRUCT_DESCRIPTORS *descriptor_copy(STRUCT_DESCRIPTORS *descriptor_in,
				    STRUCT_DESCRIPTORS **output_in,
				    int *count_out);
void descriptor_print(STRUCT_DESCRIPTORS *descriptor_in);

#endif /*  end of #ifndef _DESCRIPTORS_ */

/*
Include this first set of definitions at the beginning of the source
for a server process, before including any files with descriptor
macro definitions.
*/

#ifdef DESCRIPTORS_PART_1

#ifdef GENERATE_DESCRIPTORS_DEFINED
#undef DGEN
#undef NDGEN
#undef DSTART
#undef SDSTART
#undef UDSTART
#undef DEND
#undef SDEND
#undef UDEND
#endif

/* Descriptor Generate */
#define DGEN(element,type,size) element
/* Nested Descriptor Generate */
#define NDGEN(element,type,size,pointer)  element
/* Descriptor Start */
#define DSTART(name)
/* Structure Descriptor Start */
#define SDSTART(element,name)  element
/* Union Descriptor Start */
#define UDSTART(element,name)  element
/* Descriptor End */
#define DEND
/* Structure Descriptor End */
#define SDEND(element) element
/* Union Descriptor End */
#define UDEND(element) element

#define GENERATE_DESCRIPTORS_DEFINED
#endif /* #ifdef DESCRIPTORS_PART_1 */

/*
Include this second set of definitions at  in the source
for a server process,  after including  all files with descriptor
macro definitions and for including files with use of descriptor macros to
declare a descriptor array.
*/

#ifdef DESCRIPTORS_PART_2

#ifdef GENERATE_DESCRIPTORS_DEFINED
#undef DGEN
#undef NDGEN
#undef DSTART
#undef SDSTART
#undef UDSTART
#undef DEND
#undef SDEND
#undef UDEND
#endif

/* Descriptor Generate */
#define DGEN(element,type,size) {type,size,NULL},
/* Nested Descriptor Generate */
#define NDGEN(element,type,size,pointer) {type,size,pointer},
/* Descriptor Start */
#define DSTART(name)  STRUCT_DESCRIPTORS name[] = {
/* Structure Descriptor Start */
#define SDSTART(element,name) STRUCT_DESCRIPTORS name[] = {\
                              {STRUCTURETYPE, 1, NULL},
/* Union Descriptor Start */
#define UDSTART(element,name) STRUCT_DESCRIPTORS name[] = {\
                              {UNIONTYPE, 1, NULL},
/* Descriptor End */
#define DEND {ENDOFDESCRIPTOR,0,NULL} };
/* Structure Descriptor End */
#define SDEND(element) {STRUCTUREEND,0,NULL},{ENDOFDESCRIPTOR,0,NULL} };
/* Union Descriptor End */
#define UDEND(element) {UNIONEND,0,NULL},{ENDOFDESCRIPTOR,0,NULL} };

#define GENERATE_DESCRIPTORS_DEFINED

#endif /* #ifdef DESCRIPTORS_PART_2 */

/*
This third set of definitions can be used to print a list
of the descriptor entries to verify the code.
*/

#ifdef DESCRIPTORS_PART_3

#ifdef GENERATE_DESCRIPTORS_DEFINED
#undef DGEN
#undef NDGEN
#undef DSTART
#undef SDSTART
#undef UDSTART
#undef DEND
#undef SDEND
#undef UDEND
#endif

/* Descriptor Generate */
#define DGEN(element,type,size) \
{\
   int length,nblanks;\
   char blanks[] = "                                                       ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 50 - length - 12;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   switch(type){\
   case INTTYPE:\
      printf("       " #element "%s ,INTTYPE      ,%d\n",blanks,size);\
      break;\
   case CHARTYPE:\
      printf("       " #element "%s ,CHARTYPE     ,%d\n",blanks,size);\
      break;\
   case SHORTTYPE:\
      printf("       " #element "%s ,SHORTTYPE    ,%d\n",blanks,size);\
      break;\
   case POINTERTYPE:\
      printf("       " #element "%s ,POINTERTYPE  ,%d\n",blanks,size);\
      break;\
   case FLOATTYPE:\
      printf("       " #element "%s ,FLOATTYPE    ,%d\n",blanks,size);\
      break;\
   case DOUBLETYPE:\
      printf("       " #element "%s ,DOUBLETYPE   ,%d\n",blanks,size);\
      break;\
   case LONGTYPE:\
      printf("       " #element "%s ,LONGTYPE     ,%d\n",blanks,size);\
      break;\
   case STRUCTURETYPE:\
      printf("       " #element "%s ,STRUCTURETYPE,%d\n",blanks,size);\
      break;\
   case STRUCTUREEND:\
      printf("       " #element "%s ,STRUCTUREEND\n",blanks);\
      break;\
   } \
}
/* Nested Descriptor Generate */
#define NDGEN(element,type,size,pointer) \
{\
   int length,nblanks;\
   char blanks[] = "                                                       ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 50 - 12 - length;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("       " #element "%s ,NESTEDTYPE  ,%d," #pointer "\n",\
	  blanks,size);\
}
/* Descriptor Start */
#define DSTART(name)  \
{\
   int length,nblanks;\
   char blanks[] = "                                                      ";\
   nblanks = strlen(blanks);\
   length = 46;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("--------------------------------------\n"\
	  "%s" #name "\n",blanks);\
}
/* Structure Descriptor Start */
#define SDSTART(element,name) \
{\
   int length,nblanks;\
   char blanks[] = "                                                      ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 46 - length;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("--------------------------------------\n"\
	  #element "%s,STRUCTURETYPE, " #name "\n",blanks);\
}
/* Union Descriptor Start */
#define UDSTART(element,name) \
{\
   int length,nblanks;\
   char blanks[] = "                                                      ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 46 - length;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("--------------------------------------\n"\
	  #element "%s,UNIONTYPE, " #name "\n",blanks);\
}
/* Descriptor End */
#define DEND \
{\
   int length,nblanks;\
   char blanks[] = "                                                      ";\
   nblanks = strlen(blanks);\
   length = 46;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("%sENDOFDESCRIPTOR\n"\
	  "--------------------------------------\n", blanks);\
}
/* Structure Descriptor End */
#define SDEND(element)  \
{\
   int length,nblanks;\
   char blanks[] = "                                                     ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 39 - length;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("       " #element "%s,STRUCTUREEND   \n",blanks);\
   printf("                                              "\
          "ENDOFDESCRIPTOR\n"\
	  "--------------------------------------\n");\
}
/* Union Descriptor End */
#define UDEND(element)  \
{\
   int length,nblanks;\
   char blanks[] = "                                                     ";\
   length = strlen(#element);\
   nblanks = strlen(blanks);\
   length = 39 - length;\
   length = (length < nblanks)?length:nblanks;\
   length = (length > 0)?length:0;\
   blanks[length] = '\0';\
   printf("       " #element "%s,UNIONEND   \n",blanks);\
   printf("                                              "\
          "ENDOFDESCRIPTOR\n"\
	  "--------------------------------------\n");\
}

#define GENERATE_DESCRIPTORS_DEFINED

#endif /* #ifdef DESCRIPTORS_PART_3 */

