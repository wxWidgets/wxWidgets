/////////////////////////////////////////////////////////////////////////////
// Name:        msw/main.cpp
// Purpose:     Main/DllMain
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/event.h"
#include "wx/app.h"

#include "wx/msw/private.h"

// Don't implement WinMain if we're building an MFC/wxWindows
// hybrid app.
#if wxUSE_MFC && !defined(NOMAIN)
#define NOMAIN 1
#endif

// from src/msw/app.cpp
extern void WXDLLEXPORT wxEntryCleanup();

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

HINSTANCE wxhInstance = 0;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// various entry points
// ----------------------------------------------------------------------------

// May wish not to have a DllMain or WinMain, e.g. if we're programming
// a Netscape plugin or if we're writing a console application
#if wxUSE_GUI && !defined(NOMAIN)

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
    #define APIENTRY FAR PASCAL
#endif

/////////////////////////////////////////////////////////////////////////////////
// WinMain
// Note that WinMain is also defined in dummy.obj, which is linked to
// an application that is using the DLL version of wxWindows.

#if !defined(_WINDLL)

#if defined(__WXMICROWIN__)
    #define HINSTANCE HANDLE

    extern "C"
#endif

int PASCAL WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    return wxEntry((WXHINSTANCE) hInstance, (WXHINSTANCE) hPrevInstance,
                   lpCmdLine, nCmdShow);
}

#endif // !defined(_WINDLL)

/////////////////////////////////////////////////////////////////////////////////
// DllMain

#if defined(_WINDLL)

// DLL entry point

extern "C"
#ifdef __BORLANDC__
// SCD: I don't know why, but also OWL uses this function
BOOL WINAPI DllEntryPoint (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
#else
BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
#endif
{
#ifndef WXMAKINGDLL
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Only call wxEntry if the application itself is part of the DLL.
            // If only the wxWindows library is in the DLL, then the
            // initialisation will be called when the application implicitly
            // calls WinMain.
            return wxEntry((WXHINSTANCE) hModule);

        case DLL_PROCESS_DETACH:
           if ( wxTheApp )
              wxTheApp->OnExit();
           wxEntryCleanup();
           break;
    }
#else
	(void)hModule;
	(void)fdwReason;
#endif // !WXMAKINGDLL
	(void)lpReserved;
    return TRUE;
}

#endif // _WINDLL

#endif // !NOMAIN

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

HINSTANCE wxGetInstance()
{
    return wxhInstance;
}

void wxSetInstance(HINSTANCE hInst)
{
    wxhInstance = hInst;
}

