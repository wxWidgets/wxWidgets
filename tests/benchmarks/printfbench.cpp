/////////////////////////////////////////////////////////////////////////////
// Name:        printfbench.cpp
// Purpose:     A sample console app which benchmarks wxPrintf*() functions
// Author:      Francesco Montorsi
// Modified by:
// Created:     27/3/2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/string.h>
#include <wx/stopwatch.h>
#include <wx/utils.h>
#include <wx/cmdline.h>
#include <wx/app.h>


// ----------------------------------------------------------------------------
// command line
// ----------------------------------------------------------------------------

#define HELP_SWITCH              wxT("h")
#define NUMBENCHMARK_OPTION           wxT("n")

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, HELP_SWITCH, wxT("help"),
      wxT("displays help on the command line parameters") },

    { wxCMD_LINE_OPTION, NUMBENCHMARK_OPTION, wxT("numtest"),
      wxT("the number of wxPrintf() calls to benchmark"), wxCMD_LINE_VAL_NUMBER },

    { wxCMD_LINE_NONE }
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define DEFAULT_NUMBENCHMARKS    100000
#define BUFSIZE                  10000

// set wxTEST_WX_ONLY to 1 when you want to profile wx's implementation only.
// A little reminder about profiling under Linux:
//
//   1) configure wxWidgets in release mode
//   2) make sure that HAVE_UNIX98_PRINTF is undefined (just #defining it to zero
//      does not work; you must comment out the entire #define) in your setup.h;
//      and also that wxUSE_PRINTF_POS_PARAMS is set to 1; this will force the
//      use of wx's own implementation of wxVsnprintf()
//   3) compile wx
//   4) set wxTEST_WX_ONLY to 1 and compile tests as well
//   5) run "callgrind ./printfbench"
//   6) run "kcachegrind dump_file_generated_by_callgrind"
//
#define wxTEST_WX_ONLY           1


const wxString g_verylongString =
    wxT("very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very ")
    wxT("very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very long string!\n\n\n");


// ----------------------------------------------------------------------------
// benchmarking helpers
// ----------------------------------------------------------------------------

#define DO_LONG_BENCHMARK(fnc) \
    fnc(buffer, BUFSIZE, \
        wxT("This is a reasonably long string with various %s arguments, exactly %d, ") \
        wxT("and is used as benchmark for %s - %% %.2f %d %s"), \
        wxT("(many!!)"), 6, wxT("this program"), 23.342f, 999, \
        g_verylongString.c_str());

#define DO_LONG_POSITIONAL_BENCHMARK(fnc) \
    fnc(buffer, BUFSIZE, \
        wxT("This is a %2$s and thus is harder to parse... let's %1$s ") \
        wxT("for our benchmarking aims - %% %3$f %5$d %4$s"), \
        wxT("test it"), wxT("string with positional arguments"), 23.342f, \
        g_verylongString.c_str(), 999);

#define DO_BENCHMARK(fnc) \
    fnc(buffer, BUFSIZE, \
        wxT("This is a short %s string with very few words"), wxT("test"));

#define DO_POSITIONAL_BENCHMARK(fnc) \
    fnc(buffer, BUFSIZE, \
        wxT("This is a %2$s and thus is harder to parse... nonetheless, %1$s !"), \
        wxT("test it"), wxT("string with positional arguments"));

// the configure script of wxWidgets will define HAVE_UNIX98_PRINTF on those
// system with a *printf() family of functions conformant to Unix 98 standard;
// systems without the configure script as build system (e.g. Windows) do not
// have positional support anyway
#ifdef HAVE_UNIX98_PRINTF
    #define wxSYSTEM_HAS_POSPARAM_SUPPORT   1
#else
    #define wxSYSTEM_HAS_POSPARAM_SUPPORT   1
#endif

// we need to avoid the use of wxPrintf() here since it could have been mapped
// to wxWidgets' implementation of wxVsnPrintf() !
#if wxUSE_UNICODE
    #define sys_printf swprintf
#else
    #define sys_printf snprintf
#endif

// the given stopwatch returns a time delta in milliseconds thus this macro
// returns the number of microseconds required for a single *printf() call
#define wxFMT(sw)      ((double)((sw.Time()*1000.0)/tests))




// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");

        return -1;
    }

    // parse the command line
    // ----------------------

    wxCmdLineParser cmdParser(g_cmdLineDesc, argc, argv);
    if (cmdParser.Parse() != 0)
        return false;

    if (cmdParser.Found(HELP_SWITCH))
    {
        cmdParser.Usage();
        return false;
    }

    long tests;
    if (!cmdParser.Found(NUMBENCHMARK_OPTION, &tests))
        tests = DEFAULT_NUMBENCHMARKS;



    // print some info useful to compare different benchmarks
    // -------------------------------------------------------

    wxPrintf(wxT("\nRunning on %s\n"), wxGetOsDescription().c_str());
    wxPrintf(wxT("Compiled in %s-%s-%s mode...\n\n"),
#if wxUSE_UNICODE
    wxT("unicode"),
#else
    wxT("ansi"),
#endif
#ifdef __WXDEBUG__
    wxT("debug"),
#else
    wxT("release"),
#endif
#ifdef WXUSINGDLL
    wxT("shared")
#else
    wxT("static")
#endif
            );
    wxPrintf(wxT("Running %ld tests, for each configuration/implementation\n\n"), tests);



    // start!
    // ----------------------

    wxChar buffer[BUFSIZE];

#if !wxTEST_WX_ONLY

#if wxUSE_PRINTF_POS_PARAMS
    wxStopWatch wxPos;
    for (int i=0; i < tests; i++)
    {
        DO_LONG_POSITIONAL_BENCHMARK(wxSnprintf)
        DO_POSITIONAL_BENCHMARK(wxSnprintf)
    }
    wxPos.Pause();
#endif

    // benchmark system implementation of snprintf()
    wxStopWatch sys;
    for (int i=0; i < tests; i++)
    {
        DO_LONG_BENCHMARK(sys_printf)
        DO_BENCHMARK(sys_printf)
    }
    sys.Pause();

#if wxSYSTEM_HAS_POSPARAM_SUPPORT
    wxStopWatch sysPos;
    for (int i=0; i < tests; i++)
    {
        DO_LONG_POSITIONAL_BENCHMARK(wxSnprintf)
        DO_POSITIONAL_BENCHMARK(wxSnprintf)
    }
    sysPos.Pause();
#endif

#else       // !wxTEST_WX_ONLY

    // fake stopwatches
    wxStopWatch wxPos, sys, sysPos;
    wxPos.Pause();
    sys.Pause();
    sysPos.Pause();

#endif      // !wxTEST_WX_ONLY

    // benchmark wxWidgets implementation of wxSnprintf()
    wxStopWatch wx;
    for (int i=0; i < tests; i++)
    {
        DO_LONG_BENCHMARK(wxSnprintf)
        DO_BENCHMARK(wxSnprintf)
    }
    wx.Pause();

    // print results
    // ----------------------

    wxPrintf(wxT("\n ============================== RESULTS ==============================\n"));
    wxPrintf(wxT(" => Time for the system's snprintf():                  %.5f microsec\n"), wxFMT(sys));
#if wxSYSTEM_HAS_POSPARAM_SUPPORT
    wxPrintf(wxT(" => Time for the system's snprintf() with positionals: %.5f microsec\n"), wxFMT(sysPos));
#endif
    wxPrintf(wxT(" => Time for wxSnprintf():                             %.5f microsec\n"), wxFMT(wx));
#if wxUSE_PRINTF_POS_PARAMS
    wxPrintf(wxT(" => Time for wxSnprintf() with positionals:            %.5f microsec\n"), wxFMT(wxPos));
#endif

    double medium;
#if wxSYSTEM_HAS_POSPARAM_SUPPORT && wxUSE_PRINTF_POS_PARAMS
    medium = ((double)wx.Time() / (double)sys.Time() + (double)wxPos.Time() / (double)sysPos.Time()) / 2;
#else
    medium = (double)wx.Time() / (double)sys.Time();
#endif

    if (medium > 0.0)
    {
        // sometimes it happens that with a small number of tests, medium results zero;
        // in that case doing the 1.0/medium will not be a wise thing!
        wxPrintf(wxT("\nwxWidgets implementation is %.3f times slower\n")
                 wxT("(i.e. %.3f times faster) than system implementation.\n"), medium, 1.0/medium);
    }

    wxPrintf(wxT("\n\n"));
    return 0;
}

