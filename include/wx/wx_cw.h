/////////////////////////////////////////////////////////////////////////////
// Name:        wx_cw.h
// Purpose:     wxWindows definitions for CodeWarrior builds
// Author:      Stefan Csomor
// Modified by:
// Created:     12/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CW__
#define _WX_CW__

#ifdef __cplusplus
	#if __POWERPC__
		#include <wx_PPC++.mch>
	#elif __INTEL__
		#include <wx_x86++.mch>
	#elif __CFM68K__
		#include <wx_cfm++.mch>
	#else
		#include <wx_68k++.mch>
	#endif
#else
	#if __POWERPC__
		#include <wx_PPC.mch>
	#elif __INTEL__
		#include <wx_x86.mch>
	#elif __CFM68K__
		#include <wx_cfm.mch>
	#else
		#include <wx_68k.mch>
	#endif
#endif

#endif
    // _WX_CW__
