/////////////////////////////////////////////////////////////////////////////
// Name:        msw/main.cpp
// Purpose:     WinMain/DllMain
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/event.h"
#include "wx/app.h"
#include "wx/cmdline.h"

#include "wx/msw/private.h"

#ifdef __BORLANDC__
    // BC++ has to be special: its run-time expects the DLL entry point to be
    // named DllEntryPoint instead of the (more) standard DllMain
    #define DllMain DllEntryPoint
#endif

#if defined(__WXMICROWIN__)
    #define HINSTANCE HANDLE
#endif

#if wxUSE_GUI

// ============================================================================
// implementation: various entry points
// ============================================================================

// ----------------------------------------------------------------------------
// Windows-specific wxEntry
// ----------------------------------------------------------------------------

WXDLLEXPORT int wxEntry(HINSTANCE hInstance,
                        HINSTANCE WXUNUSED(hPrevInstance),
                        wxCmdLineArgType WXUNUSED(pCmdLine),
                        int nCmdShow)
{
    // remember the parameters Windows gave us
    wxSetInstance(hInstance);
    wxApp::m_nCmdShow = nCmdShow;

    // parse the command line: we can't use pCmdLine in Unicode build so it is
    // simpler to never use it at all (this also results in a more correct
    // argv[0])

    // break the command line in words
    wxArrayString args;

    const wxChar *cmdLine = ::GetCommandLine();
    if ( cmdLine )
    {
        args = wxCmdLineParser::ConvertStringToArgs(cmdLine);
    }

#ifdef __WXWINCE__
    // WinCE doesn't insert the program itself, so do it ourselves.
    args.Insert(wxGetFullModuleName(), 0);
#endif

    int argc = args.GetCount();

    // +1 here for the terminating NULL
    wxChar **argv = new wxChar *[argc + 1];
    for ( int i = 0; i < argc; i++ )
    {
        argv[i] = wxStrdup(args[i]);
    }

    // argv[] must be NULL-terminated
    argv[argc] = NULL;

    return wxEntry(argc, argv);
}

// May wish not to have a DllMain or WinMain, e.g. if we're programming
// a Netscape plugin or if we're writing a console application
#if !defined(NOMAIN)

extern "C"
{

// ----------------------------------------------------------------------------
// WinMain
// ----------------------------------------------------------------------------

// Note that WinMain is also defined in dummy.obj, which is linked to
// an application that is using the DLL version of wxWidgets.

#if defined(_WINDLL)

// DLL entry point

BOOL WINAPI
DllMain(HANDLE hModule, DWORD fdwReason, LPVOID WXUNUSED(lpReserved))
{
    // Only call wxEntry if the application itself is part of the DLL.
    // If only the wxWidgets library is in the DLL, then the
    // initialisation will be called when the application implicitly
    // calls WinMain.
#ifndef WXMAKINGDLL
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            return wxEntry(hModule);

        case DLL_PROCESS_DETACH:
            wxEntryCleanup();
            break;
    }
#else
    (void)hModule;
    (void)fdwReason;
#endif // !WXMAKINGDLL

    return TRUE;
}

#endif // _WINDLL

} // extern "C"

#endif // !NOMAIN

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// global HINSTANCE
// ----------------------------------------------------------------------------

#if wxUSE_BASE

HINSTANCE wxhInstance = 0;

HINSTANCE wxGetInstance()
{
    return wxhInstance;
}

void wxSetInstance(HINSTANCE hInst)
{
    wxhInstance = hInst;
}

#endif // wxUSE_BASE

