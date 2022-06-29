///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/dynamiclib.cpp
// Purpose:     Test wxDynamicLibrary
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/dynlib.h"

#ifndef __WINDOWS__
    #include "wx/filename.h"
    #include "wx/log.h"
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("DynamicLibrary::Load", "[dynlib]")
{
#if defined(__WINDOWS__)
    static const char* const LIB_NAME = "kernel32.dll";
    static const char* const FUNC_NAME = "lstrlenA";
#else // !__WINDOWS__
#if defined(__DARWIN__)
    static const char* const LIB_NAME = "/usr/lib/libc.dylib";
#elif defined(__LINUX__)
    #ifdef __x86_64__
        static const char* const LIB_NAME = "/lib/x86_64-linux-gnu/libc.so.6";
    #else
        static const char* const LIB_NAME = "/lib/libc.so.6";
    #endif
#else
    // Try some generic fallback.
    static const char* const LIB_NAME = "/usr/lib/libc.so";
#endif
    static const char* const FUNC_NAME = "strlen";

    // Under macOS 12+ we can actually load the libc dylib even though the
    // corresponding file doesn't exist on disk, so skip this check there.
#ifndef __DARWIN__
    if ( !wxFileName::Exists(LIB_NAME) )
    {
        WARN("Shared library \"" << LIB_NAME << "\" doesn't exist, "
             "skipping DynamicLibraryTestCase::Load() test.");
        return;
    }
#endif // !__DARWIN__
#endif // __WINDOWS__/!__WINDOWS__

    wxDynamicLibrary lib(LIB_NAME);
    REQUIRE( lib.IsLoaded() );

    SECTION("strlen")
    {
        typedef int (wxSTDCALL *wxStrlenType)(const char *);
        wxStrlenType pfnStrlen = (wxStrlenType)lib.GetSymbol(FUNC_NAME);

        if ( pfnStrlen )
        {
            // Call the function dynamically loaded
            CHECK( pfnStrlen("foo") == 3 );
        }
        else
        {
            FAIL(FUNC_NAME << " wasn't found in " << LIB_NAME);
        }
    }

#ifdef __WINDOWS__
    SECTION("A/W")
    {
        static const wxChar *FUNC_NAME_AW = wxT("lstrlen");

        typedef int (wxSTDCALL *wxStrlenTypeAorW)(const wxChar *);
        wxStrlenTypeAorW
            pfnStrlenAorW = (wxStrlenTypeAorW)lib.GetSymbolAorW(FUNC_NAME_AW);

        if ( pfnStrlenAorW )
        {
            CHECK( pfnStrlenAorW(wxT("foobar")) == 6 );
        }
        else
        {
            FAIL(FUNC_NAME_AW << " wasn't found in " << LIB_NAME);
        }
    }
#endif // __WINDOWS__
}
