#ifndef _fundtypes_H
#define _fundtypes_H
/*
	NI CONFIDENTIAL
	© Copyright 1990-2000 by National Instruments Corp.
	All rights reserved.

	Owners: brian.powell, greg.richardson, paul.austin

	fundtypes.h - Fundamental data types.

	Header intended for use by C and C++. No // comments.
*/

#ifdef DefineHeaderRCSId
static char rcsid_fundtypes[] = "$Id: //lvdist/branches/Mercury/dev/plat/win/cintools/fundtypes.h#3 $";
#endif

#include "platdefines.h"

#if Mac || Palm/*##############################################################*/

	typedef char				int8;
	typedef unsigned char		uInt8;
	typedef uInt8				uChar;
	typedef short				int16;
	typedef unsigned short		uInt16;
	typedef long				int32;
	typedef unsigned long		uInt32;
	typedef float				float32;
	#if (Compiler==kMPW) || (ProcessorType == kPPC)
		typedef double			float64;
	#else /* Think C and MetroWerks */
		typedef short double	float64;
	#endif

	#if (ProcessorType == kM68000) || ((ProcessorType == kPPC) && (Compiler == kMPW))
		typedef long double floatExt;
		#define ExtHiLo 0
	#elif MacOSX
		typedef double floatExt;
	#else
		typedef struct {
			double hi;
			double lo;
			} floatExt;
		#define ExtHiLo 1
	#endif	

	#define CompilerSupportsInt64 1
	typedef long long			int64;
	typedef unsigned long long	uInt64;

#elif Unix /*###########################################################*/

	#if OpSystem != kBeOS
		typedef signed char			int8;
	#endif
	typedef unsigned char		uInt8;
	typedef uInt8				uChar;
	typedef short int			int16;
	typedef unsigned short int	uInt16;
	#if OpSystem != kBeOS
		typedef int					int32;
	#endif
	typedef unsigned int		uInt32;
	typedef float				float32;
	typedef double				float64;
	#define ExtHiLo 0
	#if ProcessorType==kPARISC || ProcessorType==kMIPS || ProcessorType==kDECAlpha || OpSystem == kBeOS || OpSystem == kAIX || (Linux && PowerPC)
		typedef double				floatExt;
	#elif ProcessorType==kX86
		#if (Compiler==kGCC)
			typedef struct { /* force GCC to make this struct 10 bytes */
				int32	mlo __attribute__((packed,aligned(2)));
				int32	mhi __attribute__((packed,aligned(2)));
				int16	e __attribute__((packed,aligned(2)));
				} floatExt;
		#else
			typedef struct {
				int32	mlo, mhi;
				int16	e;
				} floatExt;
		#endif
	#else
		typedef long double			floatExt;
	#endif

	#if OpSystem==kPowerUnix || (OpSystem == kHPUX && defined(__cplusplus)) || OpSystem == kAIX
		#define CompilerSupportsInt64 0
		typedef struct {
			int32	hi, lo;
			} int64, uInt64;
	#else
		#define CompilerSupportsInt64 1
		typedef long long		int64;
		typedef unsigned long long	uInt64;
	#endif

#elif MSWin /*##########################################################*/

	typedef char				int8;
	typedef unsigned char		uInt8;
	typedef uInt8				uChar;
	typedef short int			int16;
	typedef unsigned short int	uInt16;
	typedef long				int32;
	typedef unsigned long		uInt32;
	typedef float				float32;
	typedef double				float64;

	#if defined(_NI_VC_) || defined(_NI_SC_) || defined(_NI_BC_)
		#define CompilerSupportsInt64 0
		typedef struct {
			int32	hi, lo;
			} int64, uInt64;
	#else
		#define CompilerSupportsInt64 1
		#if Compiler==kMetroWerks
		typedef long long				int64;
		typedef unsigned long long		uInt64;
		#else		
		typedef __int64				int64;
		typedef unsigned __int64	uInt64;
		#endif
	#endif

	#define ExtHiLo 0
	#if ProcessorType == kX86
		typedef struct {
			int32	mlo, mhi;
			int16	e;
			} floatExt;
	#else
		typedef double floatExt;
	#endif
	
#endif

/* Bool32 is based on our own int32 type. */
typedef int32		Bool32;
#undef	TRUE
#undef	FALSE
#define TRUE		1L
#define FALSE		0L

/* For making void* pts that are distinguishable. */
#define NIPrivatePtr(x)	typedef struct x ## _t { void *p; } *x

/* Limits for fundamental types. */
#define uInt8_MAX		0xff
#define uInt16_MAX		0xffff
#define uInt32_MAX		0xffffffff

#define int8_MIN		(-128)
#define int8_MAX		(127)
#define int16_MIN		(-32768)
#define int16_MAX		32767
#define int32_MIN		(-2147483647L - 1)
#define int32_MAX		2147483647L

#define float32_MAX     3.402823466e+38F       
#define float64_MAX     1.7976931348623158e+308 

#if CompilerSupportsInt64
	#if MSWin
		/* VC has its own way for adorning constants */
		#define int64_MIN   (-9223372036854775807i64 - 1)
		#define int64_MAX   9223372036854775807i64
		#define uInt64_MAX   0xffffffffffffffffui64
	#else
		#define int64_MIN   (-9223372036854775807LL - 1)
		#define int64_MAX   9223372036854775807LL
		#define uInt64_MAX   0xffffffffffffffffULL
	#endif
#endif

#ifndef _NI_HRESULT_DEFINED_
	typedef int32 HRESULT;
	typedef int32 SCODE;
#endif

/* The macro Unused can be used to avoid compiler warnings for
	unused parameters or locals. */
#ifdef __cplusplus
	/* This implementation of Unused is safe for const parameters. */
	#define Unused(var_or_param)	_Unused((const void *)&var_or_param)
	inline void _Unused(const void *) {}
#elif MSWin
	/* This implementation of Unused is not safe for const parameters. */
	#define Unused(var_or_param)	var_or_param=var_or_param
#else
	#define Unused(var_or_param)
#endif

/* Get the system definitions for 
	size_t, NULL, ptrdiff_t, offsetof, wchar_t */

#if !WinCE /* no stddef.h on WinCE */
	#if AlphaPtr32
		#pragma pointer_size 64
	#endif
	#include <stddef.h>
	#if AlphaPtr32
		#pragma pointer_size 32
	#endif
#else
	// DS: #error Need to find out where to get stddef defines for WinCE
	#include <stddef.h>
#endif /* WinCE */

#endif /* _fundtypes_H */
