///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wince/chkconf.h
// Purpose:     WinCE-specific configuration options checks
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-03-07
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WINCE_CHKCONF_H_
#define _WX_MSW_WINCE_CHKCONF_H_

// Standard SDK lacks a few things, forcefully disable them
#ifdef WCE_PLATFORM_STANDARDSDK
    // no shell functions support
    #undef wxUSE_STDPATHS
    #define wxUSE_STDPATHS 0
#endif // WCE_PLATFORM_STANDARDSDK

#endif // _WX_MSW_WINCE_CHKCONF_H_

