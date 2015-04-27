#ifndef _platdefines_H
#define _platdefines_H
/*
	NI CONFIDENTIAL
	© Copyright 1990-2000 by National Instruments Corp.
	All rights reserved.

	Owners: brian.powell, greg.richardson, paul.austin

	platdefines.h - Defines that describe various platforms.

	The 5 main defines set up in this file are OpSystem, WindowSystem,
	Compiler, ProcessorType and BigEndian. The other platform defines are derived
	from these main defines and provide convenience for common tests.

	This file should not contain anything but #defines and no // comments.
	This is because it is used to preprocess many kinds of files.
*/

#ifdef DefineHeaderRCSId
static char rcsid_platdefines[] = "$Id: //lvdist/branches/Mercury/dev/plat/win/cintools/platdefines.h#3 $";
#endif

/*
LabVIEW system options: don't uncomment, the compiler defines these automatically
*/
/*	THINK_C			Macintosh */
/*	THINK_CPLUS		Macintosh integrated Symantec C++ translator */
/*	macintosh		Macintosh mpw 68K compiler */
/*	__powerc		Macintosh mpw Power PC compiler */
/*	__WATCOMC__		PC and MS Windows */
/*	WIN32			PC and NT */
/*	unix			UNIX */
/*	xwindows		X Windows */
/*	sparc			SUN Sparc station */
/*	__hpux			HP workstation */
/*	__sgi			Silicon Graphics */
/* NeXT				OpenStep */
/* _AIX				AIX */

/* Possible values for OpSystem */
	#define kMacOS		1
	#define kOpenStep	2
	#define kMacOSX		3
	#define kMSWin31	10
	#define kMSWin32	11
	#define kLinux		20
	#define kSunOS		21
	#define kSolaris	22
	#define kHPUX		23
	#define kPowerUnix	24
	#define kIrix		25
	#define kAIX		26
	#define kBeOS		27
	#define kOSF1		28
	#define kVxWorks	29
	#define kPalmOS	    30

/* Possible values for WindowSystem */
	#define kMacWM		1
	#define kOpenStepWM 2
	#define kMSWin31WM	10
	#define kMSWin32WM	11
	#define kXWindows	20
	#define kBeWS		30
	#define kPalmWM		40

/* Possible values for Compiler */
	#define kThinkC		1
	#define kSymanCpp	2
	#define kMPW		3
	#define kMetroWerks 4
	#define kWatcom		10
	#define kVisualC	11
	#define kMicrosoftC 12
	#define kSymanCWin	13
	#define kBorlandC	14
	#define kGCC		20
	#define kUnbundledC 21
	#define kMotorolaCC 22
	#define kSGIC		23
	#define kAIXC		24
	#define kOSFC		25

/* Possible values for ProcessorType */
	#define kM68000		1
	#define kX86		2
	#define kSparc		3
	#define kPPC		4
	#define kPARISC		5
	#define kMIPS		6
	#define kDECAlpha	7
	#define kPIC		8
	#define kARM		9
	
/* Possible values for Runtime (Mac only)*/
	#define kMachO		1 
	#define kCFM		2

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(_WIN32_WCE)
	#define OpSystem		kMSWin32
	#define WindowSystem	kMSWin32WM
	#if defined(__MWERKS__)
		#define Compiler		kMetroWerks
	#elif defined(_MSC_VER) || defined(_NI_VC_)
		#define Compiler		kVisualC
	#elif defined(__SC__) || defined(_NI_SC_)
		#define Compiler		kSymanCWin
	#elif defined(__BORLANDC__) || defined(__BCPLUSPLUS__) || defined(_NI_BC_)
		#define Compiler		kBorlandC
	#else
		#error "We don't know the Compiler"
	#endif
	#ifdef _M_PPC
		#define ProcessorType	kPPC
	#elif defined(_M_IX86)
		#define ProcessorType	kX86
	#elif defined(_M_ALPHA)
		#define ProcessorType	kDECAlpha
	#elif Compiler == kBorlandC
		#define ProcessorType	kX86
	#elif defined(_ARM_)
		#define ProcessorType 	kARM	
	#else
		#error "We don't know the ProcessorType architecture"
	#endif
	#define BigEndian		0
#elif defined( __PALMOS_H__ )
	#define OpSystem		kPalmOS
	#define WindowSystem	kPalmWM
	#define Compiler		kMetroWerks
	#define ProcessorType		kM68000
	#define BigEndian		1
#elif defined(__BEOS__)
	#define WindowSystem	kBeWS
	#define ProcessorType	kPPC
	#define OpSystem		kBeOS
	#define Compiler		kMetroWerks
	#define BigEndian		1
#elif defined(macintosh) || defined(__PPCC__) || defined(THINK_C) || defined(__SC__) || defined(__MWERKS__) || defined(__APPLE_CC__)
	#ifdef TARGET_API_MAC_CARBON
		#define OpSystem		kMacOSX
	#else
		#define OpSystem		kMacOS
	#endif
	#define WindowSystem	kMacWM
	#define OLDROUTINENAMES 0
	#define OLDROUTINELOCATIONS 0 
	#if defined(__MACH__)
		#define Runtime kMachO
	#else
		#define Runtime kCFM
	#endif 
	#if defined(__MOTO__)
		#define Compiler	kMotorolaCC
	#elif defined(__MWERKS__)
		#define Compiler	kMetroWerks
	#elif defined(__PPCC__) || defined(applec) || defined(__MRC__)
		#define Compiler	kMPW	/* could be one of PPCC, C (MPW's old 68K compiler), or SC, or MrC */
	#elif defined(THINK_C) || defined(__SC__)
		#if defined(THINK_CPLUS)
			#define Compiler	kSymanCpp	/* the integrated C++ compiler from TC6 or Sym C 7 */
		#else
			#define Compiler	kThinkC		/* TC 6 or Sym C 7 */
		#endif
   	#elif defined(__APPLE_CC__)
        #define Compiler kGCC
	#endif
	#if defined(__powerc) || defined(__ppc__)   
		#define ProcessorType	kPPC
	#else
		#define ProcessorType	kM68000
	#endif
	#define BigEndian 1
#elif defined(__WATCOMC__)
	#define OpSystem		kMSWin31
	#define WindowSystem	kMSWin31WM
	#define Compiler		kWatcom
	#define ProcessorType	kX86
	#define BigEndian		0
#elif defined(linux)
	#define WindowSystem	kXWindows
	#define OpSystem		kLinux
	#if defined(__GNUC__)
		#define Compiler	kGCC
	#elif defined(__ASSEMBLER__)
		#define Compiler	kGAS  /* assembly file */
	#else
		#error "Unknown Linux compiler"
	#endif
	#if defined(i386)
		#define ProcessorType	kX86
		#define BigEndian		0
	#elif defined(__alpha)
		#define ProcessorType	kDECAlpha
		#define BigEndian		0
	#elif defined(powerpc)
		#define ProcessorType	kPPC
		#define BigEndian		1
	#elif defined(sparc)
		#define ProcessorType	kSparc
		#define BigEndian		1
	#elif defined(mips)
		#define ProcessorType	kMIPS
		#define BigEndian		1
	#elif defined(arm) || defined(__arm__)
		#define ProcessorType	kARM
		#define BigEndian		0
	#else
		#error "Unknown Linux platform"
	#endif
#elif (defined(__i386) || defined(__i486)) && defined(__svr4__)
	#define WindowSystem	kXWindows
	#define ProcessorType	kX86
	#define OpSystem		kSolaris
	#ifdef __GNUC__
		#define Compiler	kGCC
	#else
		#define Compiler	kUnbundledC
	#endif
	#define BigEndian		0
#elif sparc || __sparc
	#define WindowSystem	kXWindows
	#define ProcessorType	kSparc
	#ifdef __SVR4
		#define OpSystem	kSolaris
	#else
		#define OpSystem	kSunOS
	#endif
	#ifdef __GNUC__
		#define Compiler	kGCC
	#else
		#define Compiler	kUnbundledC
	#endif
	#define BigEndian		1
#elif defined(__alpha)
	#define ProcessorType	kDECAlpha
	#define BigEndian		0
	#if defined(__osf__)
		#define WindowSystem	kXWindows
		#define OpSystem	kOSF1
		#ifdef __GNUC__
			#define Compiler kGCC
		#else
			#define Compiler kOSFC
		#endif
	#elif defined(linux)
		#define WindowSystem	kXWindows
		#define Compiler	kGCC
	#endif
#elif defined(__hpux)
	#define WindowSystem	kXWindows
	#define ProcessorType	kPARISC
	#define OpSystem		kHPUX
	#ifdef __GNUC__
		#define Compiler	kGCC
	#else
		#define Compiler	kUnbundledC
	#endif
	#define BigEndian		1
#elif defined(__HC__)
	#define OpSystem		kPowerUnix
	#define WindowSystem	kXWindows
	#define Compiler		kUnbundledC
	#define ProcessorType	kPPC
	#define BigEndian		1
#elif defined(__sgi)
	#define OpSystem		kIrix
	#define WindowSystem	kXWindows
	#define Compiler		kSGIC
	#define ProcessorType	kMIPS
	#define BigEndian		1
#elif defined(NeXT)
	#define OpSystem		kOpenStep
	#define WindowSystem	kOpenStepWM
	#define Compiler		kGCC
	#define ProcessorType	kX86
	#define BigEndian		0
#elif defined(_AIX)
	#define OpSystem		kAIX
	#define WindowSystem	kXWindows
	#define Compiler		kAIXC
	#define ProcessorType	kPPC
	#define BigEndian		1
#elif defined(__GNUC__) && defined(__embedded__)
	#define OpSystem		kVxWorks
	#define WindowSystem	kXWindows
	#define Compiler		kGCC
	#define ProcessorType	kPPC
	#define BigEndian		1
#elif defined(__GNUC__) && defined(__embedded_x86__)
	#define OpSystem		kVxWorks
	#define WindowSystem	kXWindows
	#define Compiler		kGCC
	#define ProcessorType	kX86
	#define BigEndian		0
#endif

#if !defined(OpSystem) || !defined(WindowSystem) || !defined(Compiler) || !defined(ProcessorType)
	#error
#endif

/* Other defines for convenience. */

#define Unix			(((OpSystem>=kLinux) && (OpSystem<=kVxWorks)) || (OpSystem==kOpenStep))
#define MSWin			((OpSystem>=kMSWin31) && (OpSystem<=kMSWin32))
#define Mac				(OpSystem==kMacOS || OpSystem==kMacOSX)
#define MacClassic		(OpSystem==kMacOS)
#define MacOSX			(OpSystem==kMacOSX)
#define MacOSXCFM		(OpSystem==kMacOSX && Runtime==kCFM)
#define MacOSXMachO		(OpSystem==kMacOSX && Runtime==kMachO)
#define OpenStep		(OpSystem==kOpenStep)
#define XWindows		(WindowSystem==kXWindows)
#define Think			(Compiler==kThinkC)
#define MWerks68K		(Compiler==kMetroWerks && ProcessorType==kM68000)
#define Palm		 	(Compiler==kMetroWerks && OpSystem==kPalmOS)
#define MWerksPPC		(Compiler==kMetroWerks && ProcessorType==kPPC)
#define MWerksX86		(Compiler==kMetroWerks && OpSystem==kMSWin32)
#define Sparc			(ProcessorType==kSparc)
#define Linux			(OpSystem==kLinux)
#define DECAlpha		(ProcessorType==kDECAlpha)
#define PowerPC			(ProcessorType==kPPC)
#define SVR4			((OpSystem==kSolaris) || (OpSystem==kPowerUnix) || (OpSystem==kIrix))
#define SVR4thread		(SVR4 && OpSystem!=kIrix)
#define VxWorks			(OpSystem==kVxWorks)

#define AlphaPtr32		((ProcessorType==kDECAlpha) && (OpSystem==kOSF1))

#if MWerks68K && !defined(MWerks68KPalm)
	#pragma d0_pointers on
#endif

#if defined(_WIN32_WCE)
	#define WinCE 1
#else
	#define WinCE 0
#endif

/* This should move to a LabVIEW header GR 1-13-00 ??? */
#if (ProcessorType==kX86)
	#define FPUASM 1
#else
	#define FPUASM 0
#endif


#if OpSystem==kPowerUnix || DECAlpha
	#define VarArgIsInt32 0
#else
	#define VarArgIsInt32 1
#endif

#endif /* _platdefines_H */
