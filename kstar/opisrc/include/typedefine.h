#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>

// OK, NOK
#ifdef OK
#undef OK
#endif
#define OK      0

#ifdef NOK
#undef NOK
#endif
#define NOK     (-1)
// TRUE, FALSE
#ifdef TRUE
#undef TRUE
#endif
#define TRUE    1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE   0

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------
typedef unsigned char           boolean;                //TYPE_BOOL
typedef unsigned char           uint8;                  //TYPE_UINT8
typedef signed char             int8;                   //TYPE_INT8
typedef unsigned short          uint16;                 //TYPE_UINT16
typedef short                   int16;                  //TYPE_INT16
typedef unsigned int            uint32;                 //TYPE_UINT32
typedef int                     int32;                  //TYPE_INT32


#endif
