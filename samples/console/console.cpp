/////////////////////////////////////////////////////////////////////////////
// Name:        samples/console/console.cpp
// Purpose:     A sample console (as opposed to GUI) program using wxWidgets
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.10.99
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
    { wxCMD_LINE_SWITCH, "d", "dummy", "a dummy switch",
        wxCMD_LINE_VAL_NONE, 0 },
    { wxCMD_LINE_SWITCH, "s", "secret", "a secret switch",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_HIDDEN },
    // ... your other command line options here...

    wxCMD_LINE_DESC_END
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
            if (parser.Found("s"))
            {
                wxPrintf("Secret switch was given...\n");
            }

            break;

        default:
            break;
    }

    if ( argc == 1 )
    {
        // If there were no command-line options supplied, emit a message
        // otherwise it's not obvious that the sample ran successfully
        wxPrintf("Welcome to the wxWidgets 'console' sample!\n");
        wxPrintf("For more information, run it again with the --help option\n");
    }

    // do something useful here

    return 0;
}
