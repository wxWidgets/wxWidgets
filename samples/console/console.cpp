/////////////////////////////////////////////////////////////////////////////
// Name:        samples/console/console.cpp
// Purpose:     A sample console (as opposed to GUI) program using wxWidgets
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.10.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/app.h>
#include <wx/cmdline.h>

// ============================================================================
// implementation
// ============================================================================

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "d", "dummy", "a dummy switch" },
    // ... your other command line options here...

    { wxCMD_LINE_NONE }
};

int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return -1;
    }

    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    switch ( parser.Parse() )
    {
        case -1:
            // help was given, terminating
            break;

        case 0:
            // everything is ok; proceed
            if (parser.Found("d"))
            {
                wxPrintf("Dummy switch was given...\n");

                while (1)
                {
                    wxChar input[128];
                    wxPrintf("Try to guess the magic number (type 'quit' to escape): ");
                    if ( !wxFgets(input, WXSIZEOF(input), stdin) )
                        break;

                    // kill the last '\n'
                    input[wxStrlen(input) - 1] = 0;

                    if (wxStrcmp(input, "quit") == 0)
                        break;

                    long val;
                    if (!wxString(input).ToLong(&val))
                    {
                        wxPrintf("Invalid number...\n");
                        continue;
                    }

                    if (val == 42)
                        wxPrintf("You guessed!\n");
                    else
                        wxPrintf("Bad luck!\n");
                }
            }
            break;

        default:
            break;
    }

    // do something useful here

    return 0;
}
