///////////////////////////////////////////////////////////////////////////////
// Name:        wx/build.h
// Purpose:     Runtime build options checking
// Author:      Vadim Zeitlin, Vaclav Slavik
// Modified by:
// Created:     07.05.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUILD_H_
#define _WX_BUILD_H_

#include "wx/version.h"

// ----------------------------------------------------------------------------
// WX_BUILD_OPTIONS_SIGNATURE
// ----------------------------------------------------------------------------

#define __WX_BO_STRINGIZE(x)  #x

#if (wxMINOR_VERSION % 2) == 0
    #define __WX_BO_VERSION(x,y,z) \
        __WX_BO_STRINGIZE(x) "." __WX_BO_STRINGIZE(y)
#else
    #define __WX_BO_VERSION(x,y,z) \
        __WX_BO_STRINGIZE(x) "." __WX_BO_STRINGIZE(y) "." __WX_BO_STRINGIZE(z)
#endif

#ifdef __WXDEBUG__
    #define __WX_BO_DEBUG "debug"
#else
    #define __WX_BO_DEBUG "no debug"
#endif

#if wxUSE_UNICODE
    #define __WX_BO_UNICODE "Unicode"
#else
    #define __WX_BO_UNICODE "ANSI"
#endif
    
// This macro is passed as argument to wxConsoleApp::CheckBuildOptions()
#define WX_BUILD_OPTIONS_SIGNATURE \
    __WX_BO_VERSION(wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER) \
    " (" __WX_BO_DEBUG "," __WX_BO_UNICODE ")"



// Use this macro to check build options. Adding it to a file in DLL will
// ensure that the DLL checks build options in same way IMPLEMENT_APP() does.
#define WX_CHECK_BUILD_OPTIONS(libName)                                 \
    static bool wxCheckBuildOptions()                                   \
    {                                                                   \
        wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,     \
                                        libName);                       \
        return true;                                                    \
    };                                                                  \
    static bool gs_buildOptionsCheck = wxCheckBuildOptions();

#endif // _WX_BUILD_H_
