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
    #ifdef _UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../../lib/vc_dll/mswud/wx/setup.h"
            #else
                #include "../../../lib/vc_dll/mswu/wx/setup.h"
            #endif
        #else
            #ifdef _DEBUG
                #include "../../../lib/vc_lib/mswud/wx/setup.h"
            #else
                #include "../../../lib/vc_lib/mswu/wx/setup.h"
            #endif
        #endif

        // if we're using MSLU, ensure that the first library we use is
        // unicows.lib
        #if wxUSE_UNICODE_MSLU
            // for this, first tell linker not to use the standard libraries
            #pragma comment(linker, "/nod:kernel32.lib /nod:advapi32.lib /nod:user32.lib /nod:gdi32.lib /nod:shell32.lib /nod:comdlg32.lib /nod:version.lib /nod:mpr.lib /nod:rasapi32.lib /nod:winmm.lib /nod:winspool.lib /nod:vfw32.lib /nod:secur32.lib /nod:oleacc.lib /nod:oledlg.lib /nod:sensapi.lib")

            // then tell it to link with unicows.lib
            #pragma comment(lib, "unicows.lib")

            // and only then readd all standard libraries back
            #pragma comment(lib, "kernel32.lib")
            #pragma comment(lib, "user32.lib")
            #pragma comment(lib, "gdi32.lib")
            #pragma comment(lib, "winspool.lib")
            #pragma comment(lib, "comdlg32.lib")
            #pragma comment(lib, "advapi32.lib")
            #pragma comment(lib, "shell32.lib")
            #pragma comment(lib, "oleacc.lib")
        #endif // wxUSE_UNICODE_MSLU

        #ifdef _DEBUG
            #if wxUSE_XML
                #pragma comment(lib,"wxexpatd")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpegd")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpngd")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexud")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiffd")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlibd")
            #endif
            #pragma comment(lib,"wxbase26ud")
            #pragma comment(lib,"wxbase26ud_net")
            #pragma comment(lib,"wxbase26ud_xml")
            #pragma comment(lib,"wxmsw26ud_adv")
            #pragma comment(lib,"wxmsw26ud_core")
            #pragma comment(lib,"wxmsw26ud_html")
            #pragma comment(lib,"wxmsw26ud_qa")
            #pragma comment(lib,"wxmsw26ud_xrc")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw26ud_media")
            #endif
        #else // release
            #if wxUSE_XML
                #pragma comment(lib,"wxexpat")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpeg")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpng")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexu")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiff")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlib")
            #endif
            #pragma comment(lib,"wxbase26u")
            #pragma comment(lib,"wxbase26u_net")
            #pragma comment(lib,"wxbase26u_xml")
            #pragma comment(lib,"wxmsw26u_adv")
            #pragma comment(lib,"wxmsw26u_core")
            #pragma comment(lib,"wxmsw26u_html")
            #pragma comment(lib,"wxmsw26u_qa")
            #pragma comment(lib,"wxmsw26u_xrc")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw26u_media")
            #endif
        #endif // debug/release
    #else // !_UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../../lib/vc_dll/mswd/wx/setup.h"
            #else
                #include "../../../lib/vc_dll/msw/wx/setup.h"
            #endif
        #else // static lib
            #ifdef _DEBUG
                #include "../../../lib/vc_lib/mswd/wx/setup.h"
            #else
                #include "../../../lib/vc_lib/msw/wx/setup.h"
            #endif
        #endif // shared/static

        #ifdef _DEBUG
            #if wxUSE_XML
                #pragma comment(lib,"wxexpatd")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpegd")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpngd")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexd")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiffd")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlibd")
            #endif
            #pragma comment(lib,"wxbase26d")
            #pragma comment(lib,"wxbase26d_net")
            #pragma comment(lib,"wxbase26d_xml")
            #pragma comment(lib,"wxmsw26d_adv")
            #pragma comment(lib,"wxmsw26d_core")
            #pragma comment(lib,"wxmsw26d_html")
            #pragma comment(lib,"wxmsw26d_qa")
            #pragma comment(lib,"wxmsw26d_xrc")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw26d_media")
            #endif
        #else // release
            #if wxUSE_XML
                #pragma comment(lib,"wxexpat")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpeg")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpng")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregex")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiff")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlib")
            #endif
            #pragma comment(lib,"wxbase26")
            #pragma comment(lib,"wxbase26_net")
            #pragma comment(lib,"wxbase26_xml")
            #pragma comment(lib,"wxmsw26_adv")
            #pragma comment(lib,"wxmsw26_core")
            #pragma comment(lib,"wxmsw26_html")
            #pragma comment(lib,"wxmsw26_qa")
            #pragma comment(lib,"wxmsw26_xrc")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw26_media")
            #endif
        #endif // debug/release
    #endif // _UNICODE/!_UNICODE
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

