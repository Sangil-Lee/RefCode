// Filename : fileHandleException.h
//
// ***************************************************************************
// 
// Copyright(C) 2005, BNF Technology, All rights reserved.
// Proprietary information, BNF Technology
// 
// ***************************************************************************
//
// ***** HIGH LEVEL DESCRIPTION **********************************************
//
// Module for handling the exception of FileHandle class
//
// **** REVISION HISTORY *****************************************************
//
// Revision 1 2003-12-23
// Author: ysw
// Initial revision
//
// ***************************************************************************

#ifndef _FILEHANDLE_EXCEPTION_H
#define _FILEHANDLE_EXCEPTION_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "rtException.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

class FileHandleException : public RtException 
{

public :

	// Constructor
	FileHandleException() : RtException () { }
	FileHandleException(const char *exceptionReason) : RtException (exceptionReason) { }
	FileHandleException(const char *exceptionReason, const char *comment) : RtException (exceptionReason, comment) { }
};

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// _FILEHANDLE_EXCEPTION_H

