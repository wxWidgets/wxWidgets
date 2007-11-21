/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/main.cpp
// Purpose:     wxEnter for Palm OS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/07/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/cmdline.h"

#if wxUSE_GUI

// ============================================================================
// implementation: various entry points
// ============================================================================

// ----------------------------------------------------------------------------
// Platform-specific wxEntry
// ----------------------------------------------------------------------------

int wxEntry()
{
    /* There is no command line in PalmOS.  For now generate a fake arument
     * list.  Later this may be changed to reflect the application launch code
     */
    wxArrayString args;
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

#endif // wxUSE_GUI
