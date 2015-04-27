/* @(#)get_processor.c	1.6 07/07/00
get_processor.c

This routine determines what type of processor this machine is.

A "processor type" value is an integer containing a bit mask.  The
bits are:
LITTLEENDIAN:  Set if little endian
BITS64:        Set if processor has pointers and longs that are 64-bits
ALIGN8ON8:     Set if alignment of 8 byte values (type double) is always
               on an 8-byte boundary.
*/

#include <stdio.h>
#include "processor_type.h"

/*
*******************************************************************************
MACRO: get_processor

Returns the processor type as an integer.
*******************************************************************************
*/

void	get_processor (processor_type_mask *typ)
{

*typ = 0;
if (sizeof(long) == 8) *typ = *typ | BITS64;

/*
This was taken from a module called bytestuff.c written by LLNL.  A few
changes have been made to adapt it to our purposes.
*/

/***************************************************/
/*      Copyright (c) 1988-1994                    */
/*      Regents of the University of California    */
/*      Lawrence Livermore National Laboratory     */
/*      All rights reserved                        */
/***************************************************/

/*  File : bytestuff.c

    Two routines to automatically determine byte ordering
    and integer sizing in a computer.  The routines should be
    run as part of the initialization for a netmailer, so that
    netmailers which send incompatible byte orders or integer
    sizes can tell that from the NM_protocol_header.

    Authors:    Gary Preckshot
                Dave Butner

                March 20, 1987
*/


/*	Byte-order identifiers : a byte which identifies the byte ordering
	of integers in the originating computer.  For the moment, we define
	two, big_endian and little_endian from an article of some note in
	an issue of "IEEE Computer".  big_endian computers store integers
	with the MSByte first in memory order (68000's) while little_endian
	computers store integers LSByte first (VAXen, PDP-11).
*/

#define unsupported_order 0
#define big_endian 1
#define little_endian 2
#define order_check_word 0x08040201
#define hi_check_word 0x0804
#define lo_check_word 0x0201

/***********************************************************/
/*  byte_order_check                                       */
/***********************************************************/
/*
    NOTE: This won't compile on computers having long ints
    less than four bytes.  You'll have to change the value
    of order_check_word to fall within the range of allowable
    long integer sizes.
*/


{
union byte_order_union 
    {
    unsigned char   bytes[8];
    short           shorts[4];
    int             integers[2];
    long            long_integer;
    };

    union byte_order_union  b;
    int	order = 0;

    switch (sizeof(int))
    {
    case 2:
        switch (sizeof(long))
        {
        case 2:
            b.integers[0] = lo_check_word;
            if ((b.bytes[0] & 0xFF) == 0x01) order = little_endian;
            else order = big_endian;
            break;
        case 4:
            b.long_integer = order_check_word;
            if ((b.integers[0] & 0xFFFF) == lo_check_word)
            {
                if ((b.bytes[0] & 0xFF)== 0x01) order = little_endian;
            }
            else if ((b.integers[0] & 0xFFFF) == hi_check_word)
            {
                if ((b.bytes[0] & 0xFF) == 0x08) order = big_endian;
            }
            break;
        default:
            break;
        }
        break;
    case 4:
        b.integers[0] = order_check_word;
        if ((b.shorts[0] & 0xFFFF) == lo_check_word)
        {
            if ((b.bytes[0] & 0xFF) == 0x01) order = little_endian;
        }
        else if ((b.shorts[0] & 0xFFFF) == hi_check_word)
        {
            if ((b.bytes[0] & 0xFF) == 0x08) order = big_endian;
        }
        break;
    default:
        break;
    }
    if (order == little_endian) *typ = *typ | LITTLEENDIAN;
}


{
struct test_struct {
   double real1;
   int integer;
   double real2;
} test;

if (((unsigned long)(&test.real1)/8*8 == (unsigned long) (&test.real1)) &&
    ((unsigned long)(&test.real2)/8*8 == (unsigned long) (&test.real2))) 
    *typ = *typ | ALIGN8ON8;
}

}
