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
            #pragma comment(lib,"wxbase290ud")
            #pragma comment(lib,"wxbase290ud_net")
            #pragma comment(lib,"wxbase290ud_xml")
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
                #pragma comment(lib,"wxmsw290ud_adv")
                #pragma comment(lib,"wxmsw290ud_core")
                #pragma comment(lib,"wxmsw290ud_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw290ud_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw290ud_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw290ud_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw290ud_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw290ud_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw290ud_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase290ud_odbc")
                #endif
            #endif // wxUSE_GUI
        #else // release
            #pragma comment(lib,"wxbase290u")
            #pragma comment(lib,"wxbase290u_net")
            #pragma comment(lib,"wxbase290u_xml")
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
                #pragma comment(lib,"wxmsw290u_adv")
                #pragma comment(lib,"wxmsw290u_core")
                #pragma comment(lib,"wxmsw290u_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw290u_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw290u_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw290u_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw290u_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw290u_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw290u_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase290u_odbc")
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
            #pragma comment(lib,"wxbase290d")
            #pragma comment(lib,"wxbase290d_net")
            #pragma comment(lib,"wxbase290d_xml")
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
                #pragma comment(lib,"wxmsw290d_adv")
                #pragma comment(lib,"wxmsw290d_core")
                #pragma comment(lib,"wxmsw290d_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw290d_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw290d_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw290d_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw290d_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw290d_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw290d_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase290d_odbc")
                #endif
            #endif // wxUSE_GUI
        #else // release
            #pragma comment(lib,"wxbase290")
            #pragma comment(lib,"wxbase290_net")
            #pragma comment(lib,"wxbase290_xml")
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
                #pragma comment(lib,"wxmsw290_adv")
                #pragma comment(lib,"wxmsw290_core")
                #pragma comment(lib,"wxmsw290_html")
                #if wxUSE_GLCANVAS
                    #pragma comment(lib,"wxmsw290_gl")
                #endif
                #if wxUSE_DEBUGREPORT
                    #pragma comment(lib,"wxmsw290_qa")
                #endif
                #if wxUSE_XRC
                    #pragma comment(lib,"wxmsw290_xrc")
                #endif
                #if wxUSE_AUI
                    #pragma comment(lib,"wxmsw290_aui")
                #endif
                #if wxUSE_RICHTEXT
                    #pragma comment(lib,"wxmsw290_richtext")
                #endif
                #if wxUSE_MEDIACTRL
                    #pragma comment(lib,"wxmsw290_media")
                #endif
                #if wxUSE_ODBC
                    #pragma comment(lib,"wxbase290_odbc")
                #endif
            #endif // wxUSE_GUI
        #endif // debug/release
    #endif // _UNICODE/!_UNICODE
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

