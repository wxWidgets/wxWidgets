/////////////////////////////////////////////////////////////////////////////
// Name:        msw/wince/wxbase.cpp
// Purpose:     wxBase
// Author:      Marco Cavallini 
// Modified by:
// Created:     05/12/2002
// RCS-ID:      
// Copyright:   (c) KOAN SAS ( www.koansoftware.com )
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////// 

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

/* I don't remember why This does not work under emulation
#ifdef _WIN32_WCE_EMULATION
#error 'This does not work under emulation'
#endif
*/

/////////////////////////////////
#if 0
    The hardware is identified by platform specific defines; usually of the form
    WCE_PLATFORM_XXXX where XXXX is the name of the platform. Pocket PCs use the
    older Palm Size PC define WIN32_PLATFORM_PSPC which has a value for the
    version number of the platform. A Pocket PC 2002 device build can be
    detected as follows:

    #if defined(WIN32_PLATFORM_PSPC) && WIN32_PLATFORM_PSPC == 310
       // Put Pocket PC 2002 specific code here...
    #endif
    // WINCE
    #if defined(_WIN32_WCE)
	    #pragma message("\t wxWindows Target: WINDOWS CE")
    //	#include <windows.h>
    #endif
#endif

#if defined (_WIN32_WCE) 
    #if _WIN32_WCE == 300
	    #pragma message("\t WINDOWS-CE 3.00")
    #elif _WIN32_WCE == 310
	    #pragma message("\t WINDOWS-CE 3.10")
    #endif
#endif


// WINCE
#if defined(_WIN32_WCE)
	#pragma message("\t wxWindows Target: WINDOWS CE")
#endif

// WIN32
#if !defined(_WIN32_WCE) && defined(_WIN32)
	#pragma message("\t wxWindows Target: WINDOWS 32")
#endif

// LINUX
#if defined(__GNUG__)
	#pragma message("\t wxWindows Target: LINUX GNU")
#endif



/////////////////////////////////
// Print out the actual platform
/////////////////////////////////

#if wxUSE_GUI
    #if defined(__WXMSW__) && !defined(__WXWINCE__)
		#pragma message("\t CE : __WXMSW__")
    #elif defined(__WXWINCE__)
		#pragma message("\t CE : __WXWINCE__")
    #elif defined(__WXMOTIF__)
		#pragma message("\t CE : __WXMOTIF__")
    #elif defined(__WXMGL__)
		#pragma message("\t CE : __WXMGL__")
    #elif defined(__WXGTK__)
		#pragma message("\t CE : __WXGTK__")
    #elif defined(__WXX11__)
		#pragma message("\t CE : __WXX11__")
    #elif defined(__WXMAC__)
		#pragma message("\t CE : __WXMAC__")
    #elif defined(__WXPM__)
		#pragma message("\t CE : __WXPM__")
    #elif defined(__WXSTUBS__)
		#pragma message("\t CE : __WXSTUBS__")
    #endif
#endif // !GUI
