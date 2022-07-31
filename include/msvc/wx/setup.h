/////////////////////////////////////////////////////////////////////////////
// Name:        msvc/wx/setup.h
// Purpose:     wrapper around the real wx/setup.h for Visual C++
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-12-12
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwidgets.org>
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
        // don't define wxSUFFIX at all as preprocessor operations don't work
        // with empty values so we need to check for this case specially below
    #endif // _UNICODE/!_UNICODE
#endif

// compiler-specific prefix: by default it's always just "vc" for compatibility
// reasons but if you use multiple MSVC versions you probably build them with
// COMPILER_PREFIX=vcXX and in this case you may want to either predefine
// wxMSVC_VERSION as "XX" or define wxMSVC_VERSION_AUTO to use the appropriate
// version depending on the compiler used
//
// There is an additional complication with MSVC 14.0, 14.1 and 14.2 versions
// (a.k.a. MSVS 2015, 2017 and 2019): as they're all ABI-compatible with each
// other, it is convenient to use the same "vc14x" compiler prefix for all of
// them, but this is not how wxMSVC_VERSION_AUTO behaves by default, so you
// need to additionally define wxMSVC_VERSION_ABI_COMPAT to opt in into using
// this "vc14x" prefix.
#ifdef wxMSVC_VERSION
    #define wxCOMPILER_PREFIX wxCONCAT(vc, wxMSVC_VERSION)
#elif defined(wxMSVC_VERSION_AUTO) || defined(wxMSVC_VERSION_ABI_COMPAT)
    #if _MSC_VER == 1300
        #define wxCOMPILER_PREFIX vc70
    #elif _MSC_VER == 1310
        #define wxCOMPILER_PREFIX vc71
    #elif _MSC_VER == 1400
        #define wxCOMPILER_PREFIX vc80
    #elif _MSC_VER == 1500
        #define wxCOMPILER_PREFIX vc90
    #elif _MSC_VER == 1600
        #define wxCOMPILER_PREFIX vc100
    #elif _MSC_VER == 1700
        #define wxCOMPILER_PREFIX vc110
    #elif _MSC_VER == 1800
        #define wxCOMPILER_PREFIX vc120
    #elif _MSC_VER >= 1900 && _MSC_VER < 2000
        #ifdef wxMSVC_VERSION_ABI_COMPAT
            #define wxCOMPILER_PREFIX vc14x
        #else
            #if _MSC_VER < 1910
                #define wxCOMPILER_PREFIX vc140
            #elif _MSC_VER >= 1910 && _MSC_VER < 1920
                #define wxCOMPILER_PREFIX vc141
            #elif _MSC_VER >= 1920 && _MSC_VER < 1930
                #define wxCOMPILER_PREFIX vc142
            #elif _MSC_VER >= 1930 && _MSC_VER < 2000
                #define wxCOMPILER_PREFIX vc143
            #else
                #error "Unknown MSVC 14.x compiler version, please report to wx-dev."
            #endif
        #endif
    #else
        #error "Unknown MSVC compiler version, please report to wx-dev."
    #endif
#else
    #define wxCOMPILER_PREFIX vc
#endif

// architecture-specific part: not used (again, for compatibility), for x86
#if defined(_M_X64)
    #define wxARCH_SUFFIX _x64
#elif defined(_M_ARM64)
    #define wxARCH_SUFFIX _arm64
#elif defined(_M_IA64)
    #define wxARCH_SUFFIX _ia64
#else // assume _M_IX86
    #define wxARCH_SUFFIX
#endif

// Ensure the library configuration is defined
#ifndef wxCFG
    #define wxCFG
#endif

// Construct the path for the subdirectory under /lib/ that the included setup.h
// will be used from
#ifdef WXUSINGDLL
    #define wxLIB_SUBDIR \
        wxCONCAT4(wxCOMPILER_PREFIX, wxARCH_SUFFIX, _dll, wxCFG)
#else // !DLL
    #define wxLIB_SUBDIR \
        wxCONCAT4(wxCOMPILER_PREFIX, wxARCH_SUFFIX, _lib, wxCFG)
#endif // DLL/!DLL

// The user can predefine a different prefix if not using the default MSW port
// with MSVC.
#ifndef wxTOOLKIT_PREFIX
    #if defined(__WXGTK__)
        #define wxTOOLKIT_PREFIX gtk2
    #else
        #define wxTOOLKIT_PREFIX msw
    #endif
#endif // wxTOOLKIT_PREFIX

#ifdef wxSUFFIX
    #define wxTOOLKIT_FULL wxCONCAT(wxTOOLKIT_PREFIX, wxSUFFIX)
#else // suffix is empty
    #define wxTOOLKIT_FULL wxTOOLKIT_PREFIX
#endif

// the real setup.h header file we need is in the build-specific directory,
// construct the path to it
#define wxSETUPH_PATH ../../../lib/wxLIB_SUBDIR/wxTOOLKIT_FULL/wx/setup.h

#define wxSETUPH_PATH_STR wxSTRINGIZE(wxSETUPH_PATH)

#include wxSETUPH_PATH_STR


// the library names depend on the build, these macro builds the correct
// library name for the given base name
#ifdef wxSUFFIX
    #define wxSUFFIX_STR wxSTRINGIZE(wxSUFFIX)
#else // suffix is empty
    #define wxSUFFIX_STR ""
#endif
#define wxSHORT_VERSION_STRING \
    wxSTRINGIZE(wxMAJOR_VERSION) wxSTRINGIZE(wxMINOR_VERSION)

#define wxWX_LIB_NAME(name, subname) \
    "wx" name wxSHORT_VERSION_STRING wxSUFFIX_STR subname

#define wxBASE_LIB_NAME(name) wxWX_LIB_NAME("base", "_" name)
#define wxTOOLKIT_LIB_NAME(name) wxWX_LIB_NAME(wxSTRINGIZE(wxTOOLKIT_PREFIX), "_" name)

// This one is for 3rd party libraries: they don't have the version number
// in their names and usually exist in ANSI version only (except for regex)
//
// 3rd party libraries are also are not linked in when using DLLs as they're
// embedded inside our own DLLs and don't need to be linked with the user code.
#define wx3RD_PARTY_LIB_NAME(name) "wx" name wxSUFFIX_DEBUG

// special version for regex as it does have a Unicode version
#define wx3RD_PARTY_LIB_NAME_U(name) "wx" name wxSUFFIX_STR

#if defined(wxMONOLITHIC) && wxMONOLITHIC == 1
    #pragma comment(lib, wxWX_LIB_NAME("msw", ""))
#else
    #pragma comment(lib, wxWX_LIB_NAME("base", ""))

    #ifndef wxNO_NET_LIB
        #pragma comment(lib, wxBASE_LIB_NAME("net"))
    #endif
    #if wxUSE_XML && !defined(wxNO_XML_LIB)
        #pragma comment(lib, wxBASE_LIB_NAME("xml"))
    #endif
#endif // defined(wxMONOLITHIC) && wxMONOLITHIC == 1

#if !defined(WXUSINGDLL)
    #if !defined(wxNO_NET_LIB)
        #pragma comment(lib, "ws2_32")
    #endif

    #if wxUSE_XML && !defined(wxNO_XML_LIB) && !defined(wxNO_EXPAT_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("expat"))
    #endif

    #if wxUSE_REGEX && !defined(wxNO_REGEX_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME_U("regex"))
    #endif
    #if wxUSE_ZLIB && !defined(wxNO_ZLIB_LIB)
        #pragma comment(lib, wx3RD_PARTY_LIB_NAME("zlib"))
    #endif
#endif // !defined(WXUSINGDLL)

#if wxUSE_GUI
    #if !defined(WXUSINGDLL)
        #if wxUSE_LIBJPEG && !defined(wxNO_JPEG_LIB)
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("jpeg"))
        #endif
        #if wxUSE_LIBPNG && !defined(wxNO_PNG_LIB)
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("png"))
        #endif
        #if wxUSE_LIBTIFF && !defined(wxNO_TIFF_LIB)
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("tiff"))
        #endif
        #if wxUSE_STC && !defined(wxNO_STC_LIB)
            #pragma comment(lib, wx3RD_PARTY_LIB_NAME("scintilla"))
        #endif
    #endif // !defined(WXUSINGDLL)


    #if !defined(wxMONOLITHIC) || wxMONOLITHIC == 0

        #pragma comment(lib, wxTOOLKIT_LIB_NAME("core"))

        #if wxUSE_HTML && !defined(wxNO_HTML_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("html"))
        #endif
        #if wxUSE_DEBUGREPORT && !defined(wxNO_QA_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("qa"))
        #endif
        #if wxUSE_XRC && !defined(wxNO_XRC_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("xrc"))
        #endif
        #if wxUSE_AUI && !defined(wxNO_AUI_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("aui"))
        #endif
        #if wxUSE_PROPGRID && !defined(wxNO_PROPGRID_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("propgrid"))
        #endif
        #if wxUSE_RIBBON && !defined(wxNO_RIBBON_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("ribbon"))
        #endif
        #if wxUSE_RICHTEXT && !defined(wxNO_RICHTEXT_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("richtext"))
        #endif
        #if wxUSE_MEDIACTRL && !defined(wxNO_MEDIA_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("media"))
        #endif
        #if wxUSE_STC && !defined(wxNO_STC_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("stc"))
        #endif
        #if wxUSE_WEBVIEW && !defined(wxNO_WEBVIEW_LIB)
            #pragma comment(lib, wxTOOLKIT_LIB_NAME("webview"))
        #endif
    #endif // !defined(wxMONOLITHIC) || wxMONOLITHIC == 0

    #if wxUSE_GLCANVAS && !defined(wxNO_GL_LIB)
        #pragma comment(lib, wxTOOLKIT_LIB_NAME("gl"))
    #endif

#endif // wxUSE_GUI


#ifndef WXUSINGDLL
    // Make sure all required system libraries are added to the linker too when
    // using static libraries.
    #pragma comment(lib, "kernel32")
    #pragma comment(lib, "user32")
    #pragma comment(lib, "gdi32")
    #pragma comment(lib, "comdlg32")
    #pragma comment(lib, "winspool")
    #pragma comment(lib, "winmm")
    #pragma comment(lib, "shell32")
    #pragma comment(lib, "comctl32")
    #pragma comment(lib, "ole32")
    #pragma comment(lib, "oleaut32")
    #pragma comment(lib, "uuid")
    #pragma comment(lib, "rpcrt4")
    #pragma comment(lib, "advapi32")
    #if wxUSE_URL_NATIVE
        #pragma comment(lib, "wininet")
    #endif

    #ifdef __WXGTK__
        #ifdef __WXGTK3__
            #pragma comment(lib, "libgtk-3.dll.a")
            #pragma comment(lib, "libgdk-3.dll.a")
        #else
            #pragma comment(lib, "gtk-win32-2.0.lib")
            #pragma comment(lib, "gdk-win32-2.0.lib")
        #endif
        #pragma comment(lib, "pangocairo-1.0.lib")
        #pragma comment(lib, "gdk_pixbuf-2.0.lib")
        #pragma comment(lib, "cairo.lib")
        #pragma comment(lib, "pango-1.0.lib")
        #pragma comment(lib, "gobject-2.0.lib")
        #pragma comment(lib, "gthread-2.0.lib")
        #pragma comment(lib, "glib-2.0.lib")
    #endif
#endif // !WXUSINGDLL
