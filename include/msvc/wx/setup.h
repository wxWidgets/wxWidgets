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

        #ifdef _DEBUG
            #pragma comment(lib,"wxbase29ud")
            #pragma comment(lib,"wxbase29ud_net")
            #pragma comment(lib,"wxbase29ud_xml")
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexud")
            #endif

            #if wxUSE_GUI
                #if wxUSE_XML
                    #pragma comment(lib,"wxexpatd")
                #endif
                #if wxUSE_LIBJPEG
                    #pragma comment(lib,"wxjpegd")
                #endif
                #if wxUSE_LIBPNG
                    #pragma comment(lib,"wxpngd")
                #endif
                #if wxUSE_LIBTIFF
                    #pragma comment(lib,"wxtiffd")
                #endif
                #if wxUSE_ZLIB
                    #pragma comment(lib,"wxzlibd")
                #endif
                #pragma comment(lib,"wxmsw29ud_adv")
                #pragma comment(lib,"wxmsw29ud_core")
                #pragma comment(lib,"wxmsw29ud_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw29ud_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw29ud_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw29ud_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw29ud_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw29ud_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw29ud_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase29ud_odbc")
                #endif
            #endif // wxUSE_GUI
        #else // release
            #pragma comment(lib,"wxbase29u")
            #pragma comment(lib,"wxbase29u_net")
            #pragma comment(lib,"wxbase29u_xml")
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexu")
            #endif

            #if wxUSE_GUI
                #if wxUSE_XML
                    #pragma comment(lib,"wxexpat")
                #endif
                #if wxUSE_LIBJPEG
                    #pragma comment(lib,"wxjpeg")
                #endif
                #if wxUSE_LIBPNG
                    #pragma comment(lib,"wxpng")
                #endif
                #if wxUSE_LIBTIFF
                    #pragma comment(lib,"wxtiff")
                #endif
                #if wxUSE_ZLIB
                    #pragma comment(lib,"wxzlib")
                #endif
                #pragma comment(lib,"wxmsw29u_adv")
                #pragma comment(lib,"wxmsw29u_core")
                #pragma comment(lib,"wxmsw29u_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw29u_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw29u_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw29u_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw29u_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw29u_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw29u_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase29u_odbc")
                #endif
            #endif // wxUSE_GUI
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
            #pragma comment(lib,"wxbase29d")
            #pragma comment(lib,"wxbase29d_net")
            #pragma comment(lib,"wxbase29d_xml")
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexd")
            #endif

            #if wxUSE_GUI
                #if wxUSE_XML
                    #pragma comment(lib,"wxexpatd")
                #endif
                #if wxUSE_LIBJPEG
                    #pragma comment(lib,"wxjpegd")
                #endif
                #if wxUSE_LIBPNG
                    #pragma comment(lib,"wxpngd")
                #endif
                #if wxUSE_LIBTIFF
                    #pragma comment(lib,"wxtiffd")
                #endif
                #if wxUSE_ZLIB
                    #pragma comment(lib,"wxzlibd")
                #endif
                #pragma comment(lib,"wxmsw29d_adv")
                #pragma comment(lib,"wxmsw29d_core")
                #pragma comment(lib,"wxmsw29d_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw29d_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw29d_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw29d_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw29d_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw29d_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw29d_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase29d_odbc")
                #endif
            #endif // wxUSE_GUI
        #else // release
            #pragma comment(lib,"wxbase29")
            #pragma comment(lib,"wxbase29_net")
            #pragma comment(lib,"wxbase29_xml")
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregex")
            #endif

            #if wxUSE_GUI
                #if wxUSE_XML
                    #pragma comment(lib,"wxexpat")
                #endif
                #if wxUSE_LIBJPEG
                    #pragma comment(lib,"wxjpeg")
                #endif
                #if wxUSE_LIBPNG
                    #pragma comment(lib,"wxpng")
                #endif
                #if wxUSE_LIBTIFF
                    #pragma comment(lib,"wxtiff")
                #endif
                #if wxUSE_ZLIB
                    #pragma comment(lib,"wxzlib")
                #endif
                #pragma comment(lib,"wxmsw29_adv")
                #pragma comment(lib,"wxmsw29_core")
                #pragma comment(lib,"wxmsw29_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw29_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw29_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw29_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw29_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw29_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw29_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase29_odbc")
                #endif
            #endif // wxUSE_GUI
        #endif // debug/release
    #endif // _UNICODE/!_UNICODE
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

