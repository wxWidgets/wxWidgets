/*
 * File:  dummy.cc
 * Purpose:  See below
 * Author:  Julian Smart
 * Created:  1993
 * Updated:
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 *
 * If precompiling wx.h for wxWindows and derived apps,
 * link dummy.obj with your program (the MSC 7 linker complains otherwise).
 *
 * This is the only convenient way I found to use precompiled headers
 * under MSC 7.
 *
 * This will produce a big PCH file.
 */

#if defined(__BORLANDC__)
#if !(defined(__WIN32__) || defined(__NT__) || defined(__WIN32__))
#pragma hdrfile "c:\wx\src\msw\wx.pch"
#endif

#pragma hdrstart
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <windows.h>

#include "wx/msw/msvcrt.h"

// Foils optimizations in Visual C++ (see also app.cpp). Without it,
// dummy.obj isn't linked and we get a linker error.
#if defined(__VISUALC__)
    char wxDummyChar = 0;
#endif

// if wxWindows is in the DLL link our entry point with the application
#if defined(WXUSINGDLL)

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
    #define APIENTRY FAR PASCAL
#endif

int
#ifdef __WATCOMC__
    PASCAL
#else
    APIENTRY
#endif
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR m_lpCmdLine,
        int nCmdShow )
{
    wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

    return wxEntry((WXHINSTANCE) hInstance, (WXHINSTANCE) hPrevInstance,
                   m_lpCmdLine, nCmdShow);
}
#endif


