/////////////////////////////////////////////////////////////////////////////
// Name:        msw/main.cpp
// Purpose:     Main/DllMain
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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

#if defined(__TWIN32__) || defined(__WXWINE__)

extern "C"
BOOL PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

#else

#ifdef __WATCOMC__
int PASCAL
#else
int APIENTRY
#endif

 WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
// __TWIN32__

{
  return wxEntry((WXHINSTANCE) hInstance, (WXHINSTANCE) hPrevInstance, lpCmdLine, nCmdShow);
}
#endif

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
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Only call wxEntry if the application itself is part of the DLL.
            // If only the wxWindows library is in the DLL, then the initialisation
            // will be called when the application implicitly calls WinMain.

#if !defined(WXMAKINGDLL)
            return wxEntry((WXHINSTANCE) hModule);
#endif
            break;

        case DLL_PROCESS_DETACH:
        default:
            break;
    }
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

