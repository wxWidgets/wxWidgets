/////////////////////////////////////////////////////////////////////////////
// Name:        msvc/wx/msw/setup.h
// Purpose:     wrapper around the real wx/setup.h for Visual C++
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-12-12
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// VC++ IDE predefines _DEBUG and _UNICODE for the new projects itself, but
// the other symbols (WXUSINGDLL, __WXUNIVERSAL__, ...) should be defined
// explicitly!

#ifdef _MSC_VER
    #include "wx/version.h"
    #include "wx/cpp.h"

    // notice that wxSUFFIX_DEBUG is a string but wxSUFFIX itself must be an
    // identifier as string concatenation is not done inside #include where we
    // need it
    #ifdef _DEBUG
        #define wxSUFFIX_DEBUG "d"
        #ifdef _UNICODE
            #define wxSUFFIX ud
        #else // !_UNICODE
            #define wxSUFFIX d
        #endif // _UNICODE/!_UNICODE
    #else
        #define wxSUFFIX_DEBUG ""
        #ifdef _UNICODE
            #define wxSUFFIX u
        #else // !_UNICODE
            #define wxSUFFIX
        #endif // _UNICODE/!_UNICODE
    #endif

    #ifdef WXUSINGDLL
        #define wxLIB_SUBDIR vc_dll
    #else // !DLL
        #define wxLIB_SUBDIR vc_lib
    #endif // DLL/!DLL


    // the real setup.h header file we need is in the build-specific directory,
    // construct the path to it
    #define wxSETUPH_PATH \
        wxCONCAT5(../../../lib/, wxLIB_SUBDIR, /msw, wxSUFFIX, /wx/setup.h)
    #define wxSETUPH_PATH_STR wxSTRINGIZE(wxSETUPH_PATH)

    #include wxSETUPH_PATH_STR


    // the library names depend on the build, these macro builds the correct
    // library name for the given base name
    #define wxSUFFIX_STR wxSTRINGIZE(wxSUFFIX)
    #define wxSHORT_VERSION_STRING \
        wxSTRINGIZE(wxMAJOR_VERSION) wxSTRINGIZE(wxMINOR_VERSION)

    #define wxWX_LIB_NAME(name, subname) \
        "wx" name wxSHORT_VERSION_STRING wxSUFFIX_STR subname

    #define wxBASE_LIB_NAME(name) wxWX_LIB_NAME("base", "_" name)
    #define wxMSW_LIB_NAME(name) wxWX_LIB_NAME("msw", "_" name)

    // this one is for 3rd party libraries: they don't have the version number
    // in their names and usually exist in ANSI version only (except for regex)
    #define wx3RD_PARTY_LIB_NAME(name) "wx" name wxSUFFIX_DEBUG

    // special version for regex as it does have a Unicode version
    #define wx3RD_PARTY_LIB_NAME_U(name) "wx" name wxSUFFIX_STR

    #pragma comment(lib, wxWX_LIB_NAME("base", ""))
    #pragma comment(lib, wxBASE_LIB_NAME("net"))
    #pragma comment(lib, wxBASE_LIB_NAME("xml"))
    #if wxUSE_REGEX
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME_U("regex"))
    #endif

    #if wxUSE_GUI
        #if wxUSE_XML
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("expat"))
        #endif
        #if wxUSE_LIBJPEG
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("jpeg"))
        #endif
        #if wxUSE_LIBPNG
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("png"))
        #endif
        #if wxUSE_LIBTIFF
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("tiff"))
        #endif
        #if wxUSE_ZLIB
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("zlib"))
        #endif

        #pragma comment(lib, wxMSW_LIB_NAME("adv"))
        #pragma comment(lib, wxMSW_LIB_NAME("core"))
        #pragma comment(lib, wxMSW_LIB_NAME("html"))
        #if wxUSE_GLCANVAS
            #pragma comment(lib, wxMSW_LIB_NAME("gl"))
        #endif
        #if wxUSE_DEBUGREPORT
            #pragma comment(lib, wxMSW_LIB_NAME("qa"))
        #endif
        #if wxUSE_XRC
            #pragma comment(lib, wxMSW_LIB_NAME("xrc"))
        #endif
        #if wxUSE_AUI
            #pragma comment(lib, wxMSW_LIB_NAME("aui"))
        #endif
        #if wxUSE_RICHTEXT
            #pragma comment(lib, wxMSW_LIB_NAME("richtext"))
        #endif
        #if wxUSE_MEDIACTRL
            #pragma comment(lib, wxMSW_LIB_NAME("media"))
        #endif
        #if wxUSE_ODBC
            #pragma comment(lib, wxMSW_LIB_NAME("odbc"))
        #endif
    #endif // wxUSE_GUI
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

