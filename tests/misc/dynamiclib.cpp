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
    #include "wx/dir.h"
    #include "wx/filename.h"
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("DynamicLibrary::Load", "[dynlib]")
{
#if defined(__WINDOWS__)
    static const char* const LIB_NAME = "kernel32.dll";
    static const char* const FUNC_NAME = "lstrlenA";
#elif defined(__DARWIN__)
    // Under macOS 12+ we can actually load the libc dylib even though the
    // corresponding file doesn't exist on disk, so we have to handle it
    // differently.
    static const char* const LIB_NAME = "/usr/lib/libc.dylib";
    static const char* const FUNC_NAME = "strlen";
#else // other Unix
    static const char* const candidateDirs[] =
    {
        "/lib/x86_64-linux-gnu",
        "/lib",
        "/lib64",
        "/usr/lib",
    };

    static const char* const candidateVersions[] = { "6", "7", };

    wxString LIB_NAME;
    wxArrayString allMatches;
    for ( size_t n = 0; n < WXSIZEOF(candidateDirs); ++n )
    {
        const wxString dir(candidateDirs[n]);

        if ( !wxDir::Exists(dir) )
            continue;

        for ( size_t m = 0; m < WXSIZEOF(candidateVersions); ++m )
        {
            const wxString candidate = wxString::Format
                                       (
                                            "%s/libc.so.%s",
                                            dir, candidateVersions[m]
                                       );

            if ( wxFileName::Exists(candidate) )
            {
                LIB_NAME = candidate;
                break;
            }
        }

        if ( !LIB_NAME.empty() )
            break;

        wxDir::GetAllFiles(dir, &allMatches, "libc.*", wxDIR_FILES);
    }

    if ( LIB_NAME.empty() )
    {
        WARN("Couldn't find libc.so, skipping DynamicLibrary::Load() test.");

        if ( !allMatches.empty() )
        {
            WARN("Possible candidates:\n" << wxJoin(allMatches, '\n'));
        }

        return;
    }

    static const char* const FUNC_NAME = "strlen";
#endif // OS

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
        static const char* const FUNC_NAME_AW = "lstrlen";

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
