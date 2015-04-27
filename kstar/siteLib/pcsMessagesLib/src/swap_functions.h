/* @(#)swap_functions.h	1.1 08/02/01
******************************************************************************
FILE: swap_functions.h

Prototypes for swap functions.
------------------------------------------------------------------------------
Modifications:
8/2/2001: RDJ created
******************************************************************************
*/

#ifndef _SWAP_FUNCTIONS_
#define _SWAP_FUNCTIONS_

/*
******************************************************************************
SUBROUTINE: swap_i8

Do endian swaps in input buffer of 8 byte words.
******************************************************************************
*/
int swap_i8(char *buffer,int numbytes);

/*
******************************************************************************
SUBROUTINE: swap_i4

Do endian swaps in input buffer of 4 byte words.
******************************************************************************
*/
int swap_i4(char *buffer,int numbytes);

/*
******************************************************************************
SUBROUTINE: swap_i2

Do endian swaps in input buffer of 2 byte words.
******************************************************************************
*/
int swap_i2(char *buffer,int numbytes);

#endif
