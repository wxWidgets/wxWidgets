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
// explicitely!

#ifdef _MSC_VER
    #ifdef _UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../lib/vc_dll/mswud/wx/setup.h"
            #else
                #include "../../lib/vc_dll/mswu/wx/setup.h"
            #endif
        #else
            #ifdef _DEBUG
                #include "../../lib/vc_lib/mswud/wx/setup.h"
            #else
                #include "../../lib/vc_lib/mswu/wx/setup.h"
            #endif
        #endif

        #ifdef _DEBUG
            #pragma comment(lib,"wxexpatd")
            #pragma comment(lib,"wxjpegd")
            #pragma comment(lib,"wxpngd")
            #pragma comment(lib,"wxregexud")
            #pragma comment(lib,"wxtiffd")
            #pragma comment(lib,"wxzlibd")
            #pragma comment(lib,"wxbase25ud")
            #pragma comment(lib,"wxbase25ud_net")
            #pragma comment(lib,"wxbase25ud_xml")
            #pragma comment(lib,"wxmsw25ud_adv")
            #pragma comment(lib,"wxmsw25ud_core")
            #pragma comment(lib,"wxmsw25ud_html")
            #pragma comment(lib,"wxmsw25ud_xrc")
        #else // release
            #pragma comment(lib,"wxexpat")
            #pragma comment(lib,"wxjpeg")
            #pragma comment(lib,"wxpng")
            #pragma comment(lib,"wxregexu")
            #pragma comment(lib,"wxtiff")
            #pragma comment(lib,"wxzlib")
            #pragma comment(lib,"wxbase25u")
            #pragma comment(lib,"wxbase25u_net")
            #pragma comment(lib,"wxbase25u_xml")
            #pragma comment(lib,"wxmsw25u_adv")
            #pragma comment(lib,"wxmsw25u_core")
            #pragma comment(lib,"wxmsw25u_html")
            #pragma comment(lib,"wxmsw25u_xrc")
        #endif // debug/release
    #else // !_UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../lib/vc_dll/mswd/wx/setup.h"
            #else
                #include "../../lib/vc_dll/msw/wx/setup.h"
            #endif
        #else // static lib
            #ifdef _DEBUG
                #include "../../lib/vc_lib/mswd/wx/setup.h"
            #else
                #include "../../lib/vc_lib/msw/wx/setup.h"
            #endif
        #endif // shared/static

        #ifdef _DEBUG
            #pragma comment(lib,"wxexpatd")
            #pragma comment(lib,"wxjpegd")
            #pragma comment(lib,"wxpngd")
            #pragma comment(lib,"wxregexd")
            #pragma comment(lib,"wxtiffd")
            #pragma comment(lib,"wxzlibd")
            #pragma comment(lib,"wxbase25d")
            #pragma comment(lib,"wxbase25d_net")
            #pragma comment(lib,"wxbase25d_xml")
            #pragma comment(lib,"wxmsw25d_adv")
            #pragma comment(lib,"wxmsw25d_core")
            #pragma comment(lib,"wxmsw25d_html")
            #pragma comment(lib,"wxmsw25d_xrc")
        #else // release
            #pragma comment(lib,"wxexpat")
            #pragma comment(lib,"wxjpeg")
            #pragma comment(lib,"wxpng")
            #pragma comment(lib,"wxregex")
            #pragma comment(lib,"wxtiff")
            #pragma comment(lib,"wxzlib")
            #pragma comment(lib,"wxbase25")
            #pragma comment(lib,"wxbase25_net")
            #pragma comment(lib,"wxbase25_xml")
            #pragma comment(lib,"wxmsw25_adv")
            #pragma comment(lib,"wxmsw25_core")
            #pragma comment(lib,"wxmsw25_html")
            #pragma comment(lib,"wxmsw25_xrc")
        #endif // debug/release
    #endif // _UNICODE/!_UNICODE
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

