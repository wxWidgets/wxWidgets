/////////////////////////////////////////////////////////////////////////////
// Name:        wx_cw_d.h
// Purpose:     wxWindows definitions for CodeWarrior builds (Debug)
// Author:      Stefan Csomor
// Modified by:
// Created:     12/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CW__
#define _WX_CW__

#define MSL_USE_PRECOMPILED_HEADERS	1

#if !defined( __MWERKS__ )
	#error "this file is only for builds with Metrowerks CodeWarrior"
#endif 

#if (__MWERKS__ < 0x0900) || macintosh
	#define __MAC__
	#define __WXMAC__
	#define	USE_PRECOMPILED_MAC_HEADERS	1  /*Set to 0 if you don't want to use precompiled MacHeaders*/
	#include <ansi_prefix.mac.h>
	#define WX_PRECOMP
	// automatically includes MacHeaders
#elif (__MWERKS__ >= 0x0900) && __INTEL__
	#define __WXMSW__
	#define __WINDOWS__
	#define __WIN95__
	#define STRICT
	#define NOMINMAX
	#include <ansi_prefix.win32.h>
//		#include <Windows.h>
//		#include <COMMCTRL.H>
#elif __BEOS__
	#include <ansi_prefix.be.h>
	#include <Be.h>
#else	
	#error unknown MW compiler
#endif

#define __WXDEBUG__ 1
#define WXDEBUG 1

// in order to avoid problems further down in wxWindows

char *strdup(const char *s) ;

#endif
    // _WX_CW__
