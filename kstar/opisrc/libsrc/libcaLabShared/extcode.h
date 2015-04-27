#ifndef _extcode_H
#define _extcode_H
/**

	© Copyright 1990-2001 by National Instruments Corp.
	All rights reserved.

	@file	extcode.h
	@brief	This document reflects what is published in the CIN manual.
		DO NOT CHANGE
*/

#define rcsid_extcode "$Id: //lvdist/branches/Mercury/dev/plat/win/cintools/extcode.h#3 $"

/* define this to keep C++ isms from littering the code */
#ifdef __cplusplus
	/* single instance extern C functions: */
	#define EXTERNC extern "C"
	/* single instance extern C VARIABLES (needed because MSVC unnecessarily
	   mangles global variable names): */
	#define EEXTERNC extern "C"
	/* begin of extern c block: */
	#define BEGINEXTERNC extern "C" {
	/* end of externc block */
	#define ENDEXTERNC }
	/* default function argument value */
	#define DEFAULTARG(a)	= a
#else
	#define EXTERNC
	#define EEXTERNC extern
	#define BEGINEXTERNC
	#define ENDEXTERNC
	#define DEFAULTARG(a)
#endif


#include "platdefines.h"
#include "fundtypes.h"

BEGINEXTERNC

#ifdef CIN_VERS
	#if MSWin && (ProcessorType == kX86)
		#pragma pack(1)
	#endif
#endif

/* Multi-threaded categorization macros */
#ifndef TH_NOTNEEDED
/* Function tags */
#define TH_REENTRANT  /* function is completely reentrant and calls only same */
#define TH_PROTECTED  /* function uses mutex calls protect itself */

#define TH_SINGLE_UI  /* function is known to be used only from UI thread */

#define TH_UNSAFE	  /* function is NOT thread safe and needs to be made so */

/* Variable tags */
#define TH_READONLY	  /* (global) variable is never written to after startup */
#endif


/* manager.h *=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*/
#if Mac
	#if MacOSX
		/*
		// 2002.06.11: Because the gcc compiler and the Metrowerks compiler don't exactly
		//		agree on the powerpc alignment rules (which are complicated when scalars
		//		larger than 4 bytes are involved), we have decided to use "native" (or
		//		"natural") alignment, which uses the same simple rules as are used on other 
		//		platforms. Both gcc and Metrowerks seems to agree on these rules.
		//		(You can read the rules in "MachORuntime.pdf" available at Apple's Developer
		//		web site http://developer.apple.com/techpubs/. Search for "machoruntime".)
		//
		//		A simple struct that they disagree on is as follows:
		//		(Tests done with gcc 3.1 and MWerks Pro 7)
		//		typedef struct {
		//			char charZero;
		//			struct {
		//				double doubleInStruct;	// gcc aligns this to 4, MWerks to 8
		//				} embeddedStruct;
		//			} FunnyStruct;
		//		
		//		Consequences:
		//		-	A LabVIEW defined struct can be used as an alias for a system defined
		//			struct only when no members are larger than 4 bytes. The LabVIEW source
		//			code does not currently do this. Users writing CINs and DLLs should avoid
		//			this as well, or at least be aware of the potential problems.
		//		-	Neither of the compilers mentioned above have a command line or GUI option
		//			for setting alignment to "native". So we set it via a pragma here.
		//		-	Definitions of structs that may be affected by the difference in alignment
		//			should never be made before this point in the include files.
		//		-	Any other setting of the aligment options pragma must be temporary (should
		//			always be matched by a #pragma options align = reset).
		*/ 
		#if (Compiler == kGCC) || (Compiler == kMetroWerks)
			#pragma options align = natural
		#else
			#error "Unsupported compiler. Figure out how to set alignment to native/natural"
		#endif
	#endif
	/* these must be defined before we start including Mac headers */
	#ifndef ACCESSOR_CALLS_ARE_FUNCTIONS
		#define ACCESSOR_CALLS_ARE_FUNCTIONS 1
	#endif
	#ifndef OPAQUE_TOOLBOX_STRUCTS
		#define OPAQUE_TOOLBOX_STRUCTS 1
	#endif
	#include <MacTypes.h>
	typedef const uChar	ConstStr255[256];
#else
	/* All of these types are provided by Types.h on the Mac */
	typedef uChar		Str255[256], Str31[32], *StringPtr, **StringHandle;
	typedef const uChar	ConstStr255[256], *ConstStringPtr;
	typedef uInt32		ResType;
#endif

/*
Manager Error Codes are the error codes defined in labview-errors.txt
ranging from Error Code 1 to Error Code mgErrSentinel-1 (defined below). 
These codes are globally defined in the NI Error Codes Database. 
THE labview-errors.txt FILE IS AUTOMAGICALLY GENERATED FROM THE
DATABASE... DO NOT EDIT THEM DIRECTLY. When a new manager error
code is created there, this enum of values is updated to permit the
manager errors from being generated from DLLs and other external
blocks of code.  
Any changes to this list must also be reflected in resource
MLbl 6042, which defines the Error Code Ring Constant on the
Functions>>Numeric>>Additional Numeric Constants palette.
*/

enum {					/* Manager Error Codes */
#if !Mac
	noErr,
#endif
	mgArgErr=1,
	mFullErr,			/* Memory Mgr	2-3		*/
	mZoneErr,

	fEOF,				/* File Mgr		4-12	*/
	fIsOpen,
	fIOErr,
	fNotFound,
	fNoPerm,
	fDiskFull,
	fDupPath,
	fTMFOpen,
	fNotEnabled,

	rFNotFound,			/* Resource Mgr 13-15	*/
	rAddFailed,
	rNotFound,

	iNotFound,			/* Image Mgr	16-17	*/
	iMemoryErr,

	dPenNotExist,		/* Draw Mgr		18		*/

	cfgBadType,			/* Config Mgr	19-22	*/
	cfgTokenNotFound,
	cfgParseError,
	cfgAllocError,

	ecLVSBFormatError,	/* extCode Mgr	23		*/
	ecLVSBOffsetError,	/* extCode Mgr	24		*/
	ecLVSBNoCodeError,	/* extCode Mgr	25		*/

	wNullWindow,		/* Window Mgr	26-27	*/
	wDestroyMixup,

	menuNullMenu,		/* Menu Mgr		28		*/

	pAbortJob,			/* Print Mgr	29-35	*/
	pBadPrintRecord,
	pDriverError,
	pWindowsError,
	pMemoryError,
	pDialogError,
	pMiscError,

	dvInvalidRefnum,	/* Device Mgr	36-41	*/
	dvDeviceNotFound,
	dvParamErr,
	dvUnitErr,
	dvOpenErr,
	dvAbortErr,

	bogusError,			/* generic error 42 */
	cancelError,		/* cancelled by user 43 */

	OMObjLowErr,		/* object message dispatcher errors 44-52 */
	OMObjHiErr,
	OMObjNotInHeapErr,
	OMOHeapNotKnownErr,
	OMBadDPIdErr,
	OMNoDPinTabErr,
	OMMsgOutOfRangeErr,
	OMMethodNullErr,
	OMUnknownMsgErr,

	mgNotSupported,

	ncBadAddressErr,		/* Net Connection errors 54-66 */
	ncInProgress,
	ncTimeOutErr,
	ncBusyErr,
	ncNotSupportedErr,
	ncNetErr,
	ncAddrInUseErr,
	ncSysOutOfMem,
	ncSysConnAbortedErr,	/* 62 */
	ncConnRefusedErr,
	ncNotConnectedErr,
	ncAlreadyConnectedErr,
	ncConnClosedErr,		/* 66 */

	amInitErr,				/* (Inter-)Application Message Manager 67- */

	occBadOccurrenceErr,	/* 68  Occurrence Mgr errors */
	occWaitOnUnBoundHdlrErr,
	occFunnyQOverFlowErr,

	fDataLogTypeConflict,	/* 71 */
	ecLVSBCannotBeCalledFromThread, /* ExtCode Mgr	72*/
	amUnrecognizedType,
	mCorruptErr,
	ecLVSBErrorMakingTempDLL,
	ecLVSBOldCIN,			/* ExtCode Mgr	76*/

	dragSktNotFound,		/* Drag Manager 77 - 80*/
	dropLoadErr,
	oleRegisterErr,
	oleReleaseErr,

	fmtTypeMismatch,		/* String processing (printf, scanf) errors */
	fmtUnknownConversion,
	fmtTooFew,
	fmtTooMany,
	fmtScanError,

	ecLVSBFutureCIN,		/* ExtCode Mgr	86*/

	lvOLEConvertErr,
	rtMenuErr,
	pwdTampered,			/* Password processing */
	LvVariantAttrNotFound,		/* LvVariant attribute not found 90-91*/
	LvVariantTypeMismatch,		/* Cannot convert to/from type */

	axEventDataNotAvailable,	/* Event Data Not Available 92-96*/
	axEventStoreNotPresent,		/* Event Store Not Present */
	axOccurrenceNotFound,		/* Occurence Not Found */
	axEventQueueNotCreated,		/* Event Queue not created */
	axEventInfoNotAvailable,	/* Event Info is not available */
	
	oleNullRefnumPassed,		/* Refnum Passed is Null */

	omidGetClassGUIDErr,		/* Error retrieving Class GUID from OMId 98-100*/
	omidGetCoClassGUIDErr,		/* Error retrieving CoClass GUID from OMId */
	omidGetTypeLibGUIDErr,		/* Error retrieving TypeLib GUID from OMId */
	
	appMagicBad,				/* bad built application magic bytes */

	iviInvalidDowncast,         /* IVI Invalid downcast*/
	iviInvalidClassSesn,		/* IVI No Class Session Opened */
	
	maxErr,						/* max manager 104-107 */
	maxConfigErr,				/* something not right with config objects */
	maxConfigLoadErr,			/* could not load configuration */
	maxGroupNotSupported,		

	ncSockNotMulticast,			/* net connection multicast specific errors 108-112 */
	ncSockNotSinglecast,
	ncBadMulticastAddr,
	ncMcastSockReadOnly,
	ncMcastSockWriteOnly,

	ncDatagramMsgSzErr,			/* net connection Datagram message size error 113 */

	bufferEmpty,				/* CircularLVDataBuffer (queues/notifiers) */
	bufferFull,					/* CircularLVDataBuffer (queues/notifiers) */
	dataCorruptErr,				/* error unflattening data */

	requireFullPathErr,			/* supplied folder path where full file path is required  */
	folderNotExistErr,			/* folder doesn't exist */

	ncBtInvalidModeErr,			/* invalid Bluetooth mode 119 */
	ncBtSetModeErr,				/* error setting Bluetooth mode 120 */

	mgBtInvalidGUIDStrErr,		/* The GUID string is invalid 121 */

	mgErrSentinel,
	mgPrivErrBase = 500,	/* Start of Private Errors */
	mgPrivErrLast = 599,	/* Last allocated in Error DB */


	kAppErrorBase = 1000,	/* Start of application errors */
	kAppErrorLast = 1399	/* Last allocated in Error DB */
	};


typedef int32	MgErr;

typedef enum {	iB=1, iW, iL, iQ, uB, uW, uL, uQ, fS, fD, fX, cS, cD, cX } NumType;

#define Private(T)	typedef struct T##_t { void *p; } *T
#define PrivateH(T)	 struct T##_t; typedef struct T##_t **T
#define PrivateP(T)	 struct T##_t; typedef struct T##_t *T
#define PrivateFwd(T)	typedef struct T##_t *T /* for forward declarations */

/** @struct Path 
Opaque type used by the path manager. See pathmgr.cpp
declared here for use in constant table */
PrivateH(Path);

/* forward definitions of LvVariant for DLLs and CINs */
#ifdef __cplusplus
	class LvVariant;
#else
	typedef struct LVVARIANT LvVariant;
#endif
typedef LvVariant* LvVariantPtr;

/* forward definitions of HWAVEFORM and HWAVES for DLLs and CINs */
typedef struct IWaveform IWaveform;
typedef struct IWaves IWaves;

typedef IWaveform* HWAVEFORM;
typedef IWaves* HWAVES;

/* forward definitions of ATime128 (time stamp) for DLLs and CINs */
#ifdef __cplusplus
	class ATime128;
#else
	typedef struct ATIME128 ATime128;
#endif
typedef ATime128* ATime128Ptr;


typedef struct {
	float32 re, im;
	} cmplx64;

typedef struct {
	float64 re, im;
	} cmplx128;

typedef struct {
	floatExt	re, im;
	} cmplxExt;

typedef struct {
	//int32	cnt;		/* number of bytes that follow */
	size_t	cnt;		/* number of bytes that follow */
	uChar	str[1];		/* cnt bytes */
	} LStr, *LStrPtr, **LStrHandle;

typedef uChar		*UPtr, **UHandle;
typedef uChar		*PStr, **PStrHandle, *CStr;
typedef const uChar	*ConstPStr, *ConstCStr, *ConstUPtr, **ConstPStrHandle;
/*
	The following function pointer types match AZ and DS handle allocation
	prototypes.
*/
typedef TH_REENTRANT UHandle	(*AllocProcPtr)(int32);
typedef TH_REENTRANT MgErr		(*DeallocProcPtr)(void *);
typedef TH_REENTRANT MgErr		(*ReallocProcPtr)(void *, int32);
typedef TH_REENTRANT int32		(*AllocSizeProcPtr)(const void *);

typedef struct {
	int32	cnt;		/* number of pascal strings that follow */
	uChar	str[1];		/* cnt bytes of concatenated pascal strings */
	} CPStr, *CPStrPtr, **CPStrHandle;

/*** The Support Manager ***/

#define HiNibble(x)		(uInt8)(((x)>>4) & 0x0F)
#define LoNibble(x)		(uInt8)((x) & 0x0F)
#define HiByte(x)		((int8)((int16)(x)>>8))
#define LoByte(x)		((int8)(x))
#define Word(hi,lo)		(((int16)(hi)<<8) | ((int16)(uInt8)(lo)))
#define Hi16(x)			((int16)((int32)(x)>>16))
#define Lo16(x)			((int16)(x))
#define Long(hi,lo)		(((int32)(hi)<<16) | ((int32)(uInt16)(lo)))

#if OpSystem == kPowerUnix
/* the compiler cannot handle the casts when in an enum */
#define Cat4Chrs(c1,c2,c3,c4)	((((c1)&0xff)<<24)|(((c2)&0xff)<<16)|(((c3)&0xff)<<8)|((c4)&0xff))
#else
#define Cat4Chrs(c1,c2,c3,c4)	(((int32)(uInt8)(c1)<<24)|((int32)(uInt8)(c2)<<16)|((int32)(uInt8)(c3)<<8)|((int32)(uInt8)(c4)))
#endif
#define Cat2Chrs(c1,c2)			(((int16)(uInt8)(c1)<<8)|((int16)(uInt8)(c2)))

#if BigEndian
#define RTToL(c1,c2,c3,c4)	Cat4Chrs(c1,c2,c3,c4)
#define RTToW(c1,c2)		Cat2Chrs(c1,c2)
#else
#define RTToL(c1,c2,c3,c4)	Cat4Chrs(c4,c3,c2,c1)
#define RTToW(c1,c2)		Cat2Chrs(c2,c1)
#endif

#define Offset(type, field)		((int32)&((type*)0)->field)
#if Compiler==kGCC
// This is a GCC compiler extension when used on non-static fields to get the
// offset of the field in the class.  (Using the pointer cast of 0 generates a
// warnings when used on a C++ class that uses inheritance, so I introduced
// this new macro to get rid of the warning. CS 5/16/02
#define COffset(type, field)		((int32)&type::field)
#else
#define COffset(type, field)		((int32)&((type*)0)->field)
#endif

#if (ProcessorType==kX86) || (ProcessorType==kM68000)
	#define UseGetSetIntMacros	1
#else
	#define UseGetSetIntMacros	0
#endif

#if UseGetSetIntMacros
	#define GetAWord(p)			(*(int16*)(p))
	#define SetAWord(p,x)		(*(int16*)(p)= x)
	#define GetALong(p)			(*(int32*)(p))
	#define SetALong(p,x)		(*(int32*)(p)= x)
#else
	TH_REENTRANT int32			GetALong(const void *);
	TH_REENTRANT int32			SetALong(void *, int32);
	TH_REENTRANT int16			GetAWord(const void *);
	TH_REENTRANT int16			SetAWord(void *, int16);
#endif

#if !Palm
TH_REENTRANT int32			Abs(int32);
TH_REENTRANT int32			Min(int32, int32);
TH_REENTRANT int32			Max(int32, int32);
TH_REENTRANT int32			Pin(int32, int32, int32);
TH_REENTRANT Bool32			IsDigit(uChar);
TH_REENTRANT Bool32			IsAlpha(uChar);
TH_REENTRANT Bool32			IsPunct(uChar);
TH_REENTRANT Bool32			IsLower(uChar);
TH_REENTRANT Bool32			IsUpper(uChar);
TH_REENTRANT int32			ToUpper(int32);
TH_REENTRANT int32			ToLower(int32);
TH_REENTRANT uChar			HexChar(int32);
TH_REENTRANT int32			StrLen(const uChar *);
TH_REENTRANT int32			StrCat(CStr, ConstCStr);
TH_REENTRANT CStr			StrCpy(CStr, ConstCStr);
TH_REENTRANT CStr			StrNCpy(CStr, ConstCStr, int32);
TH_REENTRANT int32			StrCmp(ConstCStr, ConstCStr);
TH_REENTRANT int32			StrNCmp(ConstCStr, ConstCStr, int32);
TH_REENTRANT int32			StrNCaseCmp(ConstCStr, ConstCStr, int32);
TH_REENTRANT int32			StrIStr(CStr s, CStr r);
#endif

/* RandomGen is not truly reentrant but safe enough for our purposes */
TH_REENTRANT void			RandomGen(float64*);

/* FileNameCmp compares two PStr's with the same case-sensitivity as */
/* the filesystem. */
/* FileNameNCmp compares two CStr's (to length n) with the same */
/* case-sensitivity as the filesystem. */
/* FileNameIndCmp compares two PStr's (passing pointers to the string */
/* pointers) with the same case-sensitivity as the filesystem. */
#if Mac || MSWin
#define FileNameCmp PStrCaseCmp
#define FileNameNCmp StrNCaseCmp
#define FileNameIndCmp PPStrCaseCmp
#elif Unix
#define FileNameCmp PStrCmp
#define FileNameNCmp StrNCmp
#define FileNameIndCmp PPStrCmp
#endif

#define PStrBuf(b)		(&((PStr)(b))[1])
#define PStrLen(b)		(((PStr)(b))[0])				/* # of chars in string */
#define PStrSize(b)		(PStrLen(b)+1)					/* # of bytes including length */

/* use LHStrBuf(h), LHStrLen(h) instead of passing a *h to LStrBuf or LStrLen for right hand side refs. */
#define LHStrBuf(sh)	(sh?(&(*sh)->str[0]):0)			  /* same as above, and take string handle.*/
#define LHStrLen(sh)	(sh?((*sh)->cnt):0)				/* same as above, and take string handle. */

#define LStrBuf(sp)		(&((sp))->str[0])
#define LStrLen(sp)		(((sp))->cnt)					/* # of chars in string */
#define LStrSize(sp)	(LStrLen(sp)+sizeof(int32))		/* # of bytes including length */

#define CPStrLen		LStrLen			/* concatenated Pascal vs. LabVIEW strings */
#define CPStrBuf		LStrBuf			/* concatenated Pascal vs. LabVIEW strings */

TH_REENTRANT int32			PStrCat(PStr, ConstPStr);
TH_REENTRANT PStr			PStrCpy(PStr, ConstPStr);
TH_REENTRANT PStr			PStrNCpy(PStr, ConstPStr, int32);
TH_REENTRANT int32			PStrCmp(ConstPStr, ConstPStr);
TH_REENTRANT int32			PPStrCmp(ConstPStr*, ConstPStr*);
TH_REENTRANT int32			PStrCaseCmp(ConstPStr, ConstPStr);
TH_REENTRANT int32			PPStrCaseCmp(ConstPStr*, ConstPStr*);
TH_REENTRANT int32			LStrCmp(LStrPtr, LStrPtr);
TH_REENTRANT int32			LStrCaseCmp(LStrPtr, LStrPtr);
TH_REENTRANT int32			PtrLenStrCmp(uChar*, int32, uChar*, int32);
TH_REENTRANT int32			PtrLenStrCaseCmp(uChar*, int32, uChar*, int32);
TH_REENTRANT int32			LHStrCmp(LStrHandle, LStrHandle);
TH_REENTRANT int32			LHStrCaseCmp(LStrHandle, LStrHandle);
TH_REENTRANT int32			CPStrSize(CPStrPtr);
TH_REENTRANT int32			CPStrCmp(CPStrPtr, CPStrPtr);
TH_REENTRANT MgErr			CPStrInsert(CPStrHandle, ConstPStr, int32);
TH_REENTRANT void			CPStrRemove(CPStrHandle, int32);
TH_REENTRANT PStr			CPStrIndex(CPStrHandle, int32);
TH_REENTRANT MgErr			CPStrReplace(CPStrHandle, PStr, int32);
TH_REENTRANT int32			BlockCmp(const void * p1, const void * p2, int32 n);
TH_REENTRANT int32			PToCStr(ConstPStr, CStr);
TH_REENTRANT int32			CToPStr(ConstCStr, PStr);
TH_REENTRANT int32			LToPStr(LStrPtr, PStr);
TH_REENTRANT int32			PToLStr(ConstPStr, LStrPtr);
TH_REENTRANT PStr			PStrDup(ConstPStr buf);
TH_REENTRANT MgErr			PStrToDSLStr(PStr buf, LStrHandle *lStr);
TH_REENTRANT MgErr			DbgPrintf(char *buf, ...);
TH_REENTRANT int32			SPrintf(CStr, ConstCStr, ...);
TH_REENTRANT int32			SPrintfp(CStr, ConstPStr, ...);
TH_REENTRANT int32			PPrintf(PStr, ConstCStr, ...);
TH_REENTRANT int32			PPrintfp(PStr, ConstPStr, ...);
TH_REENTRANT MgErr			LStrPrintf(LStrHandle t, CStr fmt, ...);
typedef		int32 (*CompareProcPtr)(const void*, const void*);
TH_REENTRANT void			QSort(void*, int32, int32, CompareProcPtr);
TH_REENTRANT int32			BinSearch(const void*, int32, int32, const void*, CompareProcPtr);
TH_REENTRANT uInt32			MilliSecs(void);
TH_REENTRANT uInt32			TimeInSecs(void);

/*** The Memory Manager ***/

typedef struct {
	int32 totFreeSize, maxFreeSize, nFreeBlocks;
	int32 totAllocSize, maxAllocSize;
	int32 nPointers, nUnlockedHdls, nLockedHdls;
	int32 reserved[4];
	} MemStatRec;

/*
For parameters to the memory manager functions below
p means pointer
h means handle
ph means pointer to handle
*/
#if !Palm
TH_REENTRANT MgErr		AZCheckHandle(const void *h);
TH_REENTRANT MgErr		AZCheckPtr(void *p);
TH_REENTRANT MgErr		AZDisposeHandle(void *h);
TH_REENTRANT MgErr		AZDisposePtr(void *p);
TH_REENTRANT int32		AZGetHandleSize(const void *h);
TH_REENTRANT MgErr		AZHLock(void *h);
TH_REENTRANT MgErr		AZHUnlock(void *h);
TH_REENTRANT void		AZHPurge(void *h);
TH_REENTRANT void		AZHNoPurge(void *h);
TH_REENTRANT UHandle	AZNewHandle(int32);
TH_REENTRANT UHandle	AZNewHClr(int32);
TH_REENTRANT UPtr		AZNewPtr(int32);
TH_REENTRANT UPtr		AZNewPClr(int32);
TH_REENTRANT UHandle	AZRecoverHandle(void *p);
TH_REENTRANT MgErr		AZSetHandleSize(void *h, int32);
TH_REENTRANT MgErr		AZSetHSzClr(void *h, int32);
TH_REENTRANT int32		AZHeapCheck(Bool32);
TH_REENTRANT int32		AZMaxMem(void);
TH_REENTRANT MgErr		AZMemStats(MemStatRec *msrp);
TH_REENTRANT MgErr		AZCopyHandle(void *ph, const void *hsrc);
TH_REENTRANT MgErr		AZSetHandleFromPtr(void *ph, const void *psrc, int32 n);

TH_REENTRANT MgErr		DSCheckHandle(const void *h);
TH_REENTRANT MgErr		DSCheckPtr(void *p);
TH_REENTRANT MgErr		DSDisposeHandle(void *h);
TH_REENTRANT MgErr		DSDisposePtr(void *p);
TH_REENTRANT int32		DSGetHandleSize(const void *h);
TH_REENTRANT UHandle	DSNewHandle(int32);
TH_REENTRANT UHandle	DSNewHClr(int32);
TH_REENTRANT UPtr		DSNewPtr(int32);
TH_REENTRANT UPtr		DSNewPClr(int32);
TH_REENTRANT UHandle	DSRecoverHandle(void *p);
TH_REENTRANT MgErr		DSSetHandleSize(void *h, int32);
TH_REENTRANT MgErr		DSSetHSzClr(void *h, int32);
TH_REENTRANT MgErr		DSCopyHandle(void *ph, const void *hsrc);
TH_REENTRANT MgErr		DSSetHandleFromPtr(void *ph, const void *psrc, int32 n);

TH_REENTRANT int32		DSHeapCheck(Bool32);
TH_REENTRANT int32		DSMaxMem(void);
TH_REENTRANT MgErr		DSMemStats(MemStatRec *msrp);
TH_REENTRANT void		ClearMem(void*, int32);
TH_REENTRANT void		MoveBlock(const void *src, void *dest, int32); // accepts zero bytes without problem
TH_REENTRANT void		SwapBlock(void *src, void *dest, int32);
#else
TH_REENTRANT UHandle	DSNewHandle(int32);
TH_REENTRANT UHandle	DSNewHClr(int32);
TH_REENTRANT UPtr		DSNewPtr(int32);
TH_REENTRANT MgErr		DSDisposeHandle(void *h);
TH_REENTRANT MgErr		DSDisposePtr(void *p);
TH_REENTRANT MgErr		DSCopyHandle(void *ph, const void *hsrc);
TH_REENTRANT MgErr		DSSetHandleSize(void *h, int32);
TH_REENTRANT MgErr		DSSetHSzClr(void *h, int32);
TH_REENTRANT UPtr		DSNewPClr(int32);
#endif
/*** The Magic Cookie Manager ***/

/** @struct MagicCookie 
Opaque type used by the cookie manager. See cookie.cpp */
Private(MagicCookie);
#define kNotAMagicCookie ((MagicCookie)0L)	/* canonical invalid magic cookie */

/*** The File Manager ***/

/** open modes */
enum { openReadWrite, openReadOnly, openWriteOnly, openWriteOnlyTruncate }; 
/** deny modes */
enum { denyReadWrite, denyWriteOnly, denyNeither };		
/** seek modes */
enum { fStart=1, fEnd, fCurrent };											
/** For access, see fDefaultAccess */

/** Path type codes */
enum {	fAbsPath,
		fRelPath,
		fNotAPath,
		fUNCPath,								/**< uncfilename */
		nPathTypes };							

/** @struct File 
Opaque type used by the file manager. See filemgr.cpp */
Private(File);

/** Used for FGetInfo */
typedef struct {			/**< file/directory information record */
	int32	type;			/**< system specific file type-- 0 for directories */
	int32	creator;		/**< system specific file creator-- 0 for directories */
	int32	permissions;	/**< system specific file access rights */
	int32	size;			/**< file size in bytes (data fork on Mac) or entries in folder */
	int32	rfSize;			/**< resource fork size (on Mac only) */
	uInt32	cdate;			/**< creation date */
	uInt32	mdate;			/**< last modification date */
	Bool32	folder;			/**< indicates whether path refers to a folder */
	Bool32	isInvisible; /**< indicates whether the file is visible in File Dialog */
	struct {
		int16 v, h;
		} location;			/**< system specific geographical location */
	Str255	owner;			/**< owner (in pascal string form) of file or folder */
	Str255	group;			/**< group (in pascal string form) of file or folder */
	} FInfoRec, *FInfoPtr;

/** Used for FGetVolInfo */
typedef struct {
	uInt32	size;			/**< size in bytes of a volume */
	uInt32	used;			/**< number of bytes used on volume */
	uInt32	free;			/**< number of bytes available for use on volume */
	} VInfoRec;

/** Used with FListDir2 */
typedef struct {
	int32 flags, type;
	} FMFileType;

#if !Mac
/* For backward compatability with old CINs
-- There was a name collision with Navigation Services on MacOS */
#define FileType	FMFileType
#endif /* !Mac */

/** Type Flags used with FMFileType */
#define kIsFile				0x01
#define kRecognizedType		0x02
#define kIsLink				0x04
#define kFIsInvisible		0x08
#define kIsTopLevelVI		0x10	/**< Used only for VIs in archives */
#define kErrGettingType		0x20	/**< error occurred getting type info */
#if Mac
#define kFIsStationery		0x40
#endif

/** Used for converting from NICOM to different flavors of LV-WDT */
enum {
	kWDTUniform =0L,	/*< Uniform Flt64 WDT */
	kArrayWDTUniform	/*< Array of uniform flt64 WDT */
	};

/** Used with FExists */
enum {
	kFNotExist = 0L,
	kFIsFile,
	kFIsFolder
	};

TH_REENTRANT MgErr		FCreate(File *fdp, Path path, int32 access, int32 openMode, int32 denyMode, PStr group);
TH_REENTRANT MgErr		FCreateAlways(File *fdp, Path path, int32 access, int32 openMode, int32 denyMode, PStr group);
TH_REENTRANT MgErr		FMOpen(File *fdp, Path path, int32 openMode, int32 denyMode);
TH_REENTRANT MgErr		FMClose(File fd);
TH_REENTRANT MgErr		FMSeek(File fd, int32 ofst, int32 mode);
TH_REENTRANT MgErr		FMTell(File fd, int32 *ofstp);
TH_REENTRANT MgErr		FGetEOF(File fd, int32 *sizep);
TH_REENTRANT MgErr		FSetEOF(File fd, int32 size);
TH_REENTRANT MgErr		FMRead(File fd, int32 inCount, int32 *outCountp, UPtr buffer);
TH_REENTRANT MgErr		FMWrite(File fd, int32 inCount, int32 *outCountp, UPtr buffer);
TH_REENTRANT MgErr		FFlush(File fd);
TH_REENTRANT MgErr		FGetInfo(Path path, FInfoPtr infop);
TH_REENTRANT int32		FExists(Path path);
TH_REENTRANT MgErr		FGetAccessRights(Path path, PStr owner, PStr group, int32 *permPtr);
TH_REENTRANT MgErr		FSetInfo(Path path, FInfoPtr infop);
TH_REENTRANT MgErr		FSetAccessRights(Path path, PStr owner, PStr group, int32 *permPtr);
TH_REENTRANT MgErr		FMove(Path oldPath, Path newPath);
TH_REENTRANT MgErr		FCopy(Path oldPath, Path newPath);
TH_REENTRANT MgErr		FRemove(Path path);
TH_REENTRANT MgErr		FNewDir(Path path, int32 permissions);

typedef CPStr FDirEntRec, *FDirEntPtr, **FDirEntHandle; /**< directory list record */

TH_REENTRANT MgErr		FListDir(Path path, FDirEntHandle list, FMFileType **);
TH_REENTRANT MgErr		FAddPath(Path basePath, Path relPath, Path newPath);
TH_REENTRANT MgErr		FAppendName(Path path, ConstPStr name);
TH_REENTRANT MgErr		FRelPath(Path start, Path end, Path relPath);
TH_REENTRANT MgErr		FName(Path path, StringHandle name);
TH_REENTRANT MgErr		FNamePtr(Path path, PStr name);
TH_REENTRANT MgErr		FDirName(Path path, Path dir);
TH_REENTRANT MgErr		FVolName(Path path, Path vol);
TH_REENTRANT Path		FMakePath(Path path, int32 type, ...);
TH_REENTRANT Path		FEmptyPath(Path);
TH_REENTRANT Path		FNotAPath(Path);
TH_REENTRANT MgErr		FPathToPath(Path *p);
TH_REENTRANT MgErr		FPathCpy(Path dst, Path src);
TH_REENTRANT void		FDestroyPath(Path *pp); // FDestroyPath releases memory AND sets pointer to NULL.
TH_REENTRANT MgErr		FDisposePath(Path p);   // Use of FDestroyPath recommended over FDisposePath.
TH_REENTRANT int32		FUnFlattenPath(UPtr fp, Path *pPtr);
TH_REENTRANT int32		FFlattenPath(Path p, UPtr fp);
TH_REENTRANT int32		FDepth(Path path);
TH_REENTRANT LStrHandle FGetDefGroup(LStrHandle);
TH_REENTRANT Bool32		FStrFitsPat(uChar*, uChar*, int32, int32);
TH_REENTRANT int32		FPathCmp(Path, Path);
TH_REENTRANT int32		FPathCmpLexical(Path, Path);
TH_REENTRANT UHandle PathToCString(Path );
TH_REENTRANT MgErr		FPathToAZString(Path, LStrHandle*);
TH_REENTRANT MgErr		FPathToDSString(Path, LStrHandle*);
TH_REENTRANT MgErr		FStringToPath(LStrHandle, Path*);
TH_REENTRANT MgErr		FTextToPath(UPtr, int32, Path*);
TH_REENTRANT MgErr		FLockOrUnlockRange(File, int32, int32, int32, Bool32);
TH_REENTRANT MgErr		FGetVolInfo(Path, VInfoRec*);
TH_REENTRANT MgErr		FMGetVolInfo(Path, float64*, float64*);
TH_REENTRANT MgErr		FMGetVolPath(Path, Path*);
TH_REENTRANT MgErr		FSetPathType(Path, int32);
TH_REENTRANT MgErr		FGetPathType(Path, int32*);
TH_REENTRANT Bool32		FIsAPath(Path);
TH_REENTRANT Bool32		FIsEmptyPath(Path);
TH_REENTRANT Bool32		FIsAPathOrNotAPath(Path);
TH_REENTRANT Bool32		FIsAPathOfType(Path, int32);
TH_REENTRANT Bool32		FIsAbsPath(Path);
TH_REENTRANT MgErr		FAppPath(Path);

#define LVRefNum MagicCookie
typedef MagicCookie LVUserEventRef;
typedef MagicCookie Occurrence;

TH_REENTRANT MgErr		PostLVUserEvent(LVUserEventRef ref, void *data);
TH_PROTECTED MgErr		Occur(Occurrence o);

TH_REENTRANT MgErr		FNewRefNum(Path, File, LVRefNum*);
TH_REENTRANT Bool32		FIsARefNum(LVRefNum);
TH_REENTRANT MgErr		FDisposeRefNum(LVRefNum);
TH_REENTRANT MgErr		FRefNumToFD(LVRefNum, File*);
TH_REENTRANT MgErr		FRefNumToPath(LVRefNum, Path);
TH_REENTRANT MgErr		FArrToPath(UHandle, Bool32, Path);
TH_REENTRANT MgErr		FPathToArr(Path, Bool32*, UHandle);
TH_REENTRANT int32		FPrintf(File, CStr, ...);  /* moved from support manager area */
TH_REENTRANT MgErr FPrintfWErr(File fd, CStr fmt, ...);
TH_UNSAFE CStr DateCString(uInt32, int32);
TH_UNSAFE CStr TimeCString(uInt32, int32);
TH_UNSAFE CStr ASCIITime(uInt32);

typedef struct {	/* overlays ANSI definition for unix, watcom, think, mpw */
	int32	sec;	/* 0:59 */
	int32	min;	/* 0:59 */
	int32	hour;	/* 0:23 */
	int32	mday;	/* day of the month, 1:31 */
	int32	mon;	/* month of the year, 1:12 */
	int32	year;	/* year, 1904:2040 */
	int32	wday;	/* day of the week, 1:7 for Sun:Sat */
	int32	yday;	/* day of year (julian date), 1:366 */
	int32	isdst;	/* 1 if daylight savings time */
	} DateRec;

TH_REENTRANT void SecsToDate(uInt32, DateRec*);
TH_REENTRANT uInt32 DateToSecs(DateRec*);

// User Defined Refnum callback functions
TH_SINGLE_UI EXTERNC int32 OMEventFired(int32 session, LStrHandle typeName, LStrHandle className,
										int32 eventCode, void **data);
TH_SINGLE_UI EXTERNC MgErr	OMGetClassPropListIds(LStrHandle typeName, LStrHandle className, int32 ***propIDs);

TH_PROTECTED EXTERNC MgErr UDCookieToSesn(void* refObjIn, uInt32 *sesn);
TH_PROTECTED MgErr UDCookieToName(LStrHandle* name, void* refObj);
TH_PROTECTED EXTERNC MgErr UDRegisterSesn(LStrHandle name, uInt32 sesn, LStrHandle typeName, 
										  LStrHandle className, void* refObj, int32 cleanup,
										  LStrHandle libName, int32 calling, LStrHandle openFunc, 
										  LStrHandle closeFunc);
TH_PROTECTED EXTERNC MgErr UDUnregisterSesn(void* refObjIn);
TH_PROTECTED EXTERNC MgErr UDRemoveSesn(uInt32 sesn, LStrHandle typeName, LStrHandle className,
										Bool32 removeProc);

/*** The Resource Manager ***/

/** @struct RsrcFile 
The opaque type used by the resource manager. See resource.cpp */
Private(RsrcFile);

/*	Debugging ON section Begin	*/
#ifndef DBG
#define DBG 1
#endif
/*	Debugging ON section End	*/

/*	Debugging OFF section Begin
#undef DBG
#define DBG 0
	Debugging OFF section End	*/

/** Refer to SPrintfv() */
int32 DBPrintf(const char *fmt, ...);

/* LabVIEW Bool32 representation and values */
typedef uInt16 LVBooleanU16;
#define LVBooleanU16True	((LVBooleanU16)0x8000)
#define LVBooleanU16False	((LVBooleanU16)0x0000)
typedef uInt8 LVBoolean;
#define LVBooleanTrue		((LVBoolean)1)
#define LVBooleanFalse	((LVBoolean)0)
#define LVTRUE			LVBooleanTrue			/* for CIN users */
#define LVFALSE			LVBooleanFalse

typedef double floatNum;
#if MacOSX		// we want to make sure that any C code file we generate have their functions properly externed christina.dellas 9.25.02 
	#define CIN EXTERNC
#else
	#define CIN
#endif
#define ENTERLVSB
#define LEAVELVSB

#if Mac
	#include <fp.h>
#elif MSWin
	double log1p(double x);
#endif

TH_REENTRANT MgErr NumericArrayResize(int32, int32, UHandle*, int32);
TH_REENTRANT MgErr CallChain(UHandle);

/* CIN-specific prototypes */
int32 GetDSStorage(void);
int32 SetDSStorage(int32 newVal);	/* returns old value */
int16 *GetTDPtr(void);
UPtr  GetLVInternals(void);
MgErr SetCINArraySize(UHandle, int32, int32);


CIN MgErr CINInit(void);
CIN MgErr CINDispose(void);
CIN MgErr CINAbort(void);
CIN MgErr CINLoad(RsrcFile reserved);
CIN MgErr CINUnload(void);
CIN MgErr CINSave(RsrcFile reserved);
CIN MgErr CINProperties(int32 selector, void *arg);

/* selectors for CINProperties */
enum { kCINIsReentrant };

/* CINInit -- Called after the VI is loaded or recompiled. */
#define UseDefaultCINInit CIN MgErr CINInit() { return noErr; }

/* CINDispose -- Called before the VI is unloaded or recompiled.*/
#define UseDefaultCINDispose CIN MgErr CINDispose() \
	{ return noErr; }

/* CINAbort-- Called when the VI is aborted. */
#define UseDefaultCINAbort CIN MgErr CINAbort() { return noErr; }

/* CINLoad -- Called when the VI is loaded. */
#define UseDefaultCINLoad CIN MgErr CINLoad(RsrcFile reserved) \
	{ Unused(reserved); return noErr; }

/* CINUnload -- Called when the VI is unloaded. */
#define UseDefaultCINUnload CIN MgErr CINUnload() \
	{ return noErr; }

/* CINSave -- Called when the VI is saved. */
#define UseDefaultCINSave CIN MgErr CINSave(RsrcFile reserved) \
	{ Unused(reserved); return noErr; }

#if defined(CIN_VERS) && MSWin && (ProcessorType == kX86)
	#pragma pack()
#endif

ENDEXTERNC

#endif /* _extcode_H */
