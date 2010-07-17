/////////////////////////////////////////////////////////////////////////////
// Name:        printfbench.cpp
// Purpose:     benchmarks for wx*Printf*() functions
// Author:      Francesco Montorsi
// Modified by:
// Created:     27/3/2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006-2009 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    TODO: revise the benchmarking functions below to allow valid comparisons
          between the wx implementation and the system's implementation of
          the tested functions (e.g. adding tests which use the wxS macro to
          avoid runtime encoding conversions, etc etc).
*/

//
// Profiling under Linux:
// =====================
//
//   1) configure wxWidgets in release mode
//   2) make sure that HAVE_UNIX98_PRINTF is undefined (just #defining it to zero
//      does not work; you must comment out the entire #define) in your setup.h;
//      and also that wxUSE_PRINTF_POS_PARAMS is set to 1; this will force the
//      use of wx's own implementation of wxVsnprintf()
//   3) compile wx
//   4) set wxTEST_WX_ONLY to 1 and compile tests as well
//
// Now you have two main choices:
//
//  - using gprof:
//      5) add to the Makefile of this test program the -pg option both to
//         CXXFLAGS and to LDFLAGS
//      6) run the test
//      7) look at the gmon.out file with gprof utility
//
//  - using valgrind:
//      4) run "valgrind --tool=callgrind ./printfbench"
//      5) run "kcachegrind dump_file_generated_by_callgrind"
//

#include "wx/string.h"
#include "bench.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define BUFSIZE                  10000

const wxString g_verylongString =
    "very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very "
    "very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very long string!\n\n\n";


// ----------------------------------------------------------------------------
// benchmarking helpers
// ----------------------------------------------------------------------------

#define DO_LONG_BENCHMARK(fnc, prefix)                                                     \
    fnc(buffer, BUFSIZE,                                                                   \
        prefix##"This is a reasonably long string with various %s arguments, exactly %d, " \
        prefix##"and is used as benchmark for %s - %% %.2f %d %s",                                 \
        prefix##"(many!!)", 6, "this program", 23.342f, 999,                                       \
        (const char*)g_verylongString.c_str());

#define DO_LONG_POSITIONAL_BENCHMARK(fnc, prefix)                                          \
    fnc(buffer, BUFSIZE,                                                                   \
        prefix##"This is a %2$s and thus is harder to parse... let's %1$s "                \
        prefix##"for our benchmarking aims - %% %3$f %5$d %4$s",                                   \
        prefix##"test it", "string with positional arguments", 23.342f,                            \
        (const char*)g_verylongString.c_str(), 999);

#define DO_BENCHMARK(fnc, prefix)                                                          \
    fnc(buffer, BUFSIZE, prefix##"This is a short %s string with very few words", "test");

#define DO_POSITIONAL_BENCHMARK(fnc, prefix)                                               \
    fnc(buffer, BUFSIZE,                                                                   \
        prefix##"This is a %2$s and thus is harder to parse... nonetheless, %1$s !",       \
        "test it", "string with positional arguments");

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


// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(SnprintfWithPositionals)
{
    wxChar buffer[BUFSIZE];
#if wxUSE_PRINTF_POS_PARAMS
    DO_LONG_POSITIONAL_BENCHMARK(wxSnprintf, )
    DO_POSITIONAL_BENCHMARK(wxSnprintf, )
#endif
    return true;
}

BENCHMARK_FUNC(Snprintf)
{
    wxChar buffer[BUFSIZE];
    DO_LONG_BENCHMARK(wxSnprintf, )
    DO_BENCHMARK(wxSnprintf, )
    return true;
}

BENCHMARK_FUNC(SystemSnprintfWithPositionals)
{
    wxChar buffer[BUFSIZE];
    DO_LONG_POSITIONAL_BENCHMARK(sys_printf, L)
    DO_POSITIONAL_BENCHMARK(sys_printf, L)
    return true;
}

BENCHMARK_FUNC(SystemSnprintf)
{
    wxChar buffer[BUFSIZE];
    DO_LONG_BENCHMARK(sys_printf, L)
    DO_BENCHMARK(sys_printf, L)
    return true;
}

