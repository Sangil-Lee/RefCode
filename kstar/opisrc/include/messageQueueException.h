// Filename : messageQueueException.h
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
// Module for handle the exception of MessageQueue class
//
// **** REVISION HISTORY *****************************************************
//
// Revision 1 2003-12-23
// Author: ysw
// Initial revision
//
// ***************************************************************************

#ifndef _MESSAGEQUEUE_EXCEPTION_H
#define _MESSAGEQUEUE_EXCEPTION_H

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

class MessageQueueException : public RtException 
{

public :

	// Constructor
	MessageQueueException() : RtException () { }
	MessageQueueException(const char *exceptionReason) : RtException (exceptionReason) { }
	MessageQueueException(const char *exceptionReason, const char *comment) : RtException (exceptionReason, comment) { }
};

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// _MESSAGEQUEUE_EXCEPTION_H

