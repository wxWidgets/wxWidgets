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

#ifndef _MSC_VER
    #error "This file should only be included when using Microsoft Visual C++"
#endif

// VC++ IDE predefines _DEBUG and _UNICODE for the new projects itself, but
// the other symbols (WXUSINGDLL, __WXUNIVERSAL__, ...) should be defined
// explicitly!

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

#ifndef wxNO_NET_LIB
    #pragma comment(lib, wxBASE_LIB_NAME("net"))
#endif
#ifndef wxNO_XML_LIB
    #pragma comment(lib, wxBASE_LIB_NAME("xml"))
#endif
#if wxUSE_REGEX && !defined(wxNO_REGEX_LIB)
    #pragma comment(lib, wx3RD_PARTY_LIB_NAME_U("regex"))
#endif

#if wxUSE_GUI
    #if wxUSE_XML && !defined(wxNO_EXPAT_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("expat"))
    #endif
    #if wxUSE_LIBJPEG && !defined(wxNO_JPEG_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("jpeg"))
    #endif
    #if wxUSE_LIBPNG && !defined(wxNO_PNG_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("png"))
    #endif
    #if wxUSE_LIBTIFF && !defined(wxNO_TIFF_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("tiff"))
    #endif
    #if wxUSE_ZLIB && !defined(wxNO_ZLIB_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("zlib"))
    #endif

    #pragma comment(lib, wxMSW_LIB_NAME("core"))

    #ifndef wxNO_ADV_LIB
        #pragma comment(lib, wxMSW_LIB_NAME("adv"))
    #endif

    #ifndef wxNO_HTML_LIB
        #pragma comment(lib, wxMSW_LIB_NAME("html"))
    #endif
    #if wxUSE_GLCANVAS && !defined(wxNO_GL_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("gl"))
    #endif
    #if wxUSE_DEBUGREPORT && !defined(wxNO_QA_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("qa"))
    #endif
    #if wxUSE_XRC && !defined(wxNO_XRC_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("xrc"))
    #endif
    #if wxUSE_AUI && !defined(wxNO_AUI_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("aui"))
    #endif
    #if wxUSE_PROPGRID && !defined(wxNO_PROPGRID_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("propgrid"))
    #endif
    #if wxUSE_RICHTEXT && !defined(wxNO_RICHTEXT_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("richtext"))
    #endif
    #if wxUSE_MEDIACTRL && !defined(wxNO_MEDIA_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("media"))
    #endif
    #if wxUSE_STC && !defined(wxNO_STC_LIB)
        #pragma comment(lib, wxMSW_LIB_NAME("stc"))
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("scintilla"))
    #endif
#endif // wxUSE_GUI
