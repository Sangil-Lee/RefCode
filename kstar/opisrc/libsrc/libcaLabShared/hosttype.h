#ifndef _hosttype_H
#define _hosttype_H
/**
	NI CONFIDENTIAL
	© Copyright 1990-2001 by National Instruments Corp.
	All rights reserved.

	@author	brian.powell, greg.richardson
	@file	hosttype.h
	@brief	Host specific definitions, etc.
*/

#ifdef DefineHeaderRCSId
static char rcsid_hosttype[] = "$Id: //lvdist/branches/Mercury/dev/plat/win/cintools/hosttype.h#2 $";
#endif

#if Mac
	#include <Memory.h>
	#include <Files.h>
	#include <TextEdit.h>
	#include <MacWindows.h>
	#include <Dialogs.h>
	#include <OSUtils.h>
	#include <Resources.h>
	#include <Events.h>
	#include <QDOffscreen.h>
	#include <Menus.h>
	#include <Palettes.h>
	#include <LowMem.h>
	#include <Gestalt.h>
	#include <MacErrors.h>

	#if OpSystem != kMacOSX
		#define QDG(qdField)	(qd.qdField)
		#include <Traps.h>
		#include <Packages.h>
		#include <Printing.h>
		#include <SegLoad.h>
	#endif

#endif	/* Mac */

#if OpenStep
#include <AppKit/NSWindow.h>
#endif /* OpenStep */

#if Unix
#if AlphaPtr32
	#pragma pointer_size 64
#endif
#include <stdio.h>
#include <string.h>
#if XWindows
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif /* XWindows */
#if AlphaPtr32
	#pragma pointer_size 32
#endif
#endif /* Unix */

#if WindowSystem == kBeWS
	#include <Window.h>
	#include <View.h>
	#include <Bitmap.h>
#endif /* kBeWS */

#if MSWin
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#define _WIN32_WINNT 0x0400
#include <windows.h>
#pragma warning (disable : 4001) /* nonstandard extension 'single line comment' was used */

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1L
#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0L
#ifdef VCI_RTX
	#if VCI_RTX
		#include <io.h>
		#include "rtxsyscall.h"
		#include <rtapi.h>
	#endif
#endif /* defined VCI_RTX */
#endif /* MSWin */

#endif /* _hosttype_H */
