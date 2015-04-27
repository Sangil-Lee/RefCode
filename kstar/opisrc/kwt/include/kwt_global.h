/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Kwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Kwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef KWT_GLOBAL_H
#define KWT_GLOBAL_H

#include <qglobal.h>
#if QT_VERSION < 0x040000
#include <qmodules.h>
#endif

#define KWT_VERSION       0x010000
#define KWT_VERSION_STR   "1.0.0cvs"

//
// Create Kwt DLL if KWT_DLL is defined (Windows only)
//

#if defined(Q_WS_WIN)

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
#endif

#if defined(KWT_NODLL)
#undef KWT_MAKEDLL
#undef KWT_DLL
#undef KWT_TEMPLATEDLL
#endif

#ifdef KWT_DLL
#if defined(KWT_MAKEDLL)     /* create a Kwt DLL library */
#undef KWT_DLL
#define KWT_EXPORT  __declspec(dllexport)
#define KWT_TEMPLATEDLL
#endif
#endif

#if defined(KWT_DLL)     /* use a Kwt DLL library */
#define KWT_EXPORT  __declspec(dllimport)
#define KWT_TEMPLATEDLL
#endif

#else // ! Q_WS_WIN
#undef KWT_MAKEDLL       /* ignore these for other platforms */
#undef KWT_DLL
#undef KWT_TEMPLATEDLL
#endif

#ifndef KWT_EXPORT
#define KWT_EXPORT
#endif

// #define KWT_NO_COMPAT 1 // disable withdrawn functionality

#endif // KWT_GLOBAL_H
