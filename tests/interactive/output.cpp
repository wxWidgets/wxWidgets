///////////////////////////////////////////////////////////////////////////////
// Name:        tests/interactive/output.cpp
// Purpose:     Miscellaneous tests NOT requiring user input, just user checks
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-21
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/app.h"
#include "wx/wxcrt.h"           // for wxPuts
#include "wx/wxcrtvararg.h"     // for wxPrintf

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#define TEST_DYNLIB
#if wxUSE_MIMETYPE
#define TEST_MIME
#endif
#define TEST_INFO_FUNCTIONS
#define TEST_STACKWALKER
#define TEST_STDPATHS
#define TEST_VOLUME

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class InteractiveOutputTestCase : public CppUnit::TestCase
{
public:
    InteractiveOutputTestCase() { }

private:
    CPPUNIT_TEST_SUITE( InteractiveOutputTestCase );
        CPPUNIT_TEST( TestDllListLoaded );
        CPPUNIT_TEST( TestMimeEnum );
        CPPUNIT_TEST( TestMimeAssociate );
        CPPUNIT_TEST( TestMimeFilename );
        CPPUNIT_TEST( TestOsInfo );
        CPPUNIT_TEST( TestPlatformInfo );
        CPPUNIT_TEST( TestUserInfo );
        CPPUNIT_TEST( TestStackWalk );
        CPPUNIT_TEST( TestStandardPaths );
        CPPUNIT_TEST( TestFSVolume );
    CPPUNIT_TEST_SUITE_END();

    void TestDllListLoaded();
    void TestMimeEnum();
    void TestMimeAssociate();
    void TestMimeFilename();
    void TestOsInfo();
    void TestPlatformInfo();
    void TestUserInfo();
    void TestStackWalk();
    void TestStandardPaths();
    void TestFSVolume();

    wxDECLARE_NO_COPY_CLASS(InteractiveOutputTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

wxREGISTER_UNIT_TEST_WITH_TAGS(InteractiveOutputTestCase,
                               "[!hide][interactive][output]");

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

#if !defined(__WINDOWS__) && !defined(__UNIX__)
    #undef TEST_DYNLIB
#endif

#include "wx/dynlib.h"

void InteractiveOutputTestCase::TestDllListLoaded()
{
#ifdef TEST_DYNLIB
    wxPuts(wxT("*** testing wxDynamicLibrary::ListLoaded() ***\n"));

    wxPuts("Loaded modules:");
    wxDynamicLibraryDetailsArray dlls = wxDynamicLibrary::ListLoaded();
    const size_t count = dlls.GetCount();
    for ( size_t n = 0; n < count; ++n )
    {
        const wxDynamicLibraryDetails& details = dlls[n];
        wxPrintf("%-45s", details.GetPath());

        void *addr wxDUMMY_INITIALIZE(nullptr);
        size_t len wxDUMMY_INITIALIZE(0);
        if ( details.GetAddress(&addr, &len) )
        {
            wxPrintf(" %p:%p", addr, (void *)((char *)addr + len));
        }

        wxPrintf(" %s\n", details.GetVersion());
    }

    wxPuts(wxEmptyString);
#endif // TEST_DYNLIB
}


// ----------------------------------------------------------------------------
// MIME types
// ----------------------------------------------------------------------------

#include "wx/mimetype.h"

void InteractiveOutputTestCase::TestMimeEnum()
{
#ifdef TEST_MIME
    wxPuts(wxT("*** Testing wxMimeTypesManager::EnumAllFileTypes() ***\n"));

    wxArrayString mimetypes;

    size_t count = wxTheMimeTypesManager->EnumAllFileTypes(mimetypes);

    wxPrintf(wxT("*** All %zu known filetypes: ***\n"), count);

    wxArrayString exts;
    wxString desc;

    for ( size_t n = 0; n < count; n++ )
    {
        wxFileType *filetype =
            wxTheMimeTypesManager->GetFileTypeFromMimeType(mimetypes[n]);
        if ( !filetype )
        {
            wxPrintf(wxT("   nothing known about the filetype '%s'!\n"),
                     mimetypes[n]);
            continue;
        }

        filetype->GetDescription(&desc);
        filetype->GetExtensions(exts);

        filetype->GetIcon(nullptr);

        wxString extsAll;
        for ( size_t e = 0; e < exts.GetCount(); e++ )
        {
            if ( e > 0 )
                extsAll << wxT(", ");
            extsAll += exts[e];
        }

        wxPrintf(wxT("   %s: %s (%s)\n"), mimetypes[n], desc, extsAll);
    }

    wxPuts(wxEmptyString);
#endif // TEST_MIME
}

void InteractiveOutputTestCase::TestMimeFilename()
{
#ifdef TEST_MIME
    wxPuts(wxT("*** Testing MIME type from filename query ***\n"));

    static const wxChar *filenames[] =
    {
        wxT("readme.txt"),
        wxT("document.pdf"),
        wxT("image.gif"),
        wxT("picture.jpeg"),
    };

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const wxString fname = filenames[n];
        wxString ext = fname.AfterLast(wxT('.'));
        wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
        if ( !ft )
        {
            wxPrintf(wxT("WARNING: extension '%s' is unknown.\n"), ext);
        }
        else
        {
            wxString desc;
            if ( !ft->GetDescription(&desc) )
                desc = wxT("<no description>");

            wxString cmd;
            if ( !ft->GetOpenCommand(&cmd,
                                     wxFileType::MessageParameters(fname, wxEmptyString)) )
                cmd = wxT("<no command available>");
            else
                cmd = wxString(wxT('"')) + cmd + wxT('"');

            wxPrintf(wxT("To open %s (%s) run:\n   %s\n"), fname, desc, cmd);

            delete ft;
        }
    }

    wxPuts(wxEmptyString);
#endif // TEST_MIME
}

void InteractiveOutputTestCase::TestMimeAssociate()
{
#ifdef TEST_MIME
    wxPuts(wxT("*** Testing creation of filetype association ***\n"));

    wxFileTypeInfo ftInfo("application/x-xyz");
    ftInfo.SetOpenCommand("xyzview '%s'");
    ftInfo.SetDescription("XYZ File");
    ftInfo.AddExtension(".xyz");
    ftInfo.SetShortDesc(wxT("XYZFile")); // used under Win32 only

    wxFileType *ft = wxTheMimeTypesManager->Associate(ftInfo);
    if ( !ft )
    {
        wxPuts(wxT("ERROR: failed to create association!"));
    }
    else
    {
        // TODO: read it back
        delete ft;
    }

    wxPuts(wxEmptyString);
#endif // TEST_MIME
}


// ----------------------------------------------------------------------------
// misc information functions
// ----------------------------------------------------------------------------

#include "wx/utils.h"

void InteractiveOutputTestCase::TestOsInfo()
{
#ifdef TEST_INFO_FUNCTIONS
    wxPuts(wxT("*** Testing OS info functions ***\n"));

    int major, minor;
    wxGetOsVersion(&major, &minor);
    wxPrintf(wxT("Running under: %s, version %d.%d\n"),
             wxGetOsDescription(), major, minor);

    wxPrintf(wxT("%lld free bytes of memory left.\n"), wxGetFreeMemory());

    wxPrintf(wxT("Host name is %s (%s).\n"),
             wxGetHostName(), wxGetFullHostName());

    wxPuts(wxEmptyString);
#endif // TEST_INFO_FUNCTIONS
}

void InteractiveOutputTestCase::TestPlatformInfo()
{
#ifdef TEST_INFO_FUNCTIONS
    wxPuts(wxT("*** Testing wxPlatformInfo functions ***\n"));

    // get this platform
    wxPlatformInfo plat;

    wxPrintf(wxT("Operating system family name is: %s\n"), plat.GetOperatingSystemFamilyName());
    wxPrintf(wxT("Operating system name is: %s\n"), plat.GetOperatingSystemIdName());
    wxPrintf(wxT("Port ID name is: %s\n"), plat.GetPortIdName());
    wxPrintf(wxT("Port ID short name is: %s\n"), plat.GetPortIdShortName());
    wxPrintf(wxT("Architecture bitness is: %s\n"), plat.GetBitnessName());
    wxPrintf(wxT("Endianness is: %s\n"), plat.GetEndiannessName());

    wxPuts(wxEmptyString);
#endif // TEST_INFO_FUNCTIONS
}

void InteractiveOutputTestCase::TestUserInfo()
{
#ifdef TEST_INFO_FUNCTIONS
    wxPuts(wxT("*** Testing user info functions ***\n"));

    wxPrintf(wxT("User id is:\t%s\n"), wxGetUserId());
    wxPrintf(wxT("User name is:\t%s\n"), wxGetUserName());
    wxPrintf(wxT("Home dir is:\t%s\n"), wxGetHomeDir());
    wxPrintf(wxT("Email address:\t%s\n"), wxGetEmailAddress());

    wxPuts(wxEmptyString);
#endif // TEST_INFO_FUNCTIONS
}


// ----------------------------------------------------------------------------
// stack backtrace
// ----------------------------------------------------------------------------

#if wxUSE_STACKWALKER

#include "wx/stackwalk.h"

class StackDump : public wxStackWalker
{
public:
    StackDump(const char *argv0)
        : wxStackWalker(argv0)
    {
    }

    virtual void Walk(size_t skip = 1, size_t maxdepth = wxSTACKWALKER_MAX_DEPTH) override
    {
        wxPuts(wxT("Stack dump:"));

        wxStackWalker::Walk(skip, maxdepth);
    }

protected:
    virtual void OnStackFrame(const wxStackFrame& frame) override
    {
        wxPrintf("[%2zu] ", frame.GetLevel());

        wxString name = frame.GetName();
        if ( !name.empty() )
        {
            wxPrintf("%-20.40s", name);
        }
        else
        {
            wxPrintf("0x%p", frame.GetAddress());
        }

        if ( frame.HasSourceLocation() )
        {
            wxPrintf("\t%s:%zu", frame.GetFileName(), frame.GetLine());
        }

        puts("");

        wxString type, val;
        for ( size_t n = 0; frame.GetParam(n, &type, &name, &val); n++ )
        {
            wxPrintf("\t%s %s = %s\n", type, name, val);
        }
    }
};
#endif

void InteractiveOutputTestCase::TestStackWalk()
{
#ifdef TEST_STACKWALKER
#if wxUSE_STACKWALKER
    wxPuts(wxT("*** Testing wxStackWalker ***"));

    wxString progname(wxTheApp->argv[0]);
    StackDump dump(progname.utf8_str());
    dump.Walk();

    wxPuts("\n");
#endif
#endif // TEST_STACKWALKER
}


// ----------------------------------------------------------------------------
// standard paths
// ----------------------------------------------------------------------------

#include "wx/stdpaths.h"
#include "wx/wxchar.h"      // wxPrintf

void InteractiveOutputTestCase::TestStandardPaths()
{
#ifdef TEST_STDPATHS
    wxPuts(wxT("*** Testing wxStandardPaths ***"));

    wxTheApp->SetAppName(wxT("console"));

    wxStandardPathsBase& stdp = wxStandardPaths::Get();
    wxPrintf(wxT("Config dir (sys):\t%s\n"), stdp.GetConfigDir());
    wxPrintf(wxT("Config dir (user):\t%s\n"), stdp.GetUserConfigDir());
    wxPrintf(wxT("Data dir (sys):\t\t%s\n"), stdp.GetDataDir());
    wxPrintf(wxT("Data dir (sys local):\t%s\n"), stdp.GetLocalDataDir());
    wxPrintf(wxT("Data dir (user):\t%s\n"), stdp.GetUserDataDir());
    wxPrintf(wxT("Data dir (user local):\t%s\n"), stdp.GetUserLocalDataDir());
    wxPrintf(wxT("Documents dir:\t\t%s\n"), stdp.GetDocumentsDir());
    wxPrintf(wxT("Cache dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Cache));
    wxPrintf(wxT("Desktop dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Desktop));
    wxPrintf(wxT("Downloads dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Downloads));
    wxPrintf(wxT("Music dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Music));
    wxPrintf(wxT("Pictures dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Pictures));
    wxPrintf(wxT("Videos dir:\t\t%s\n"), stdp.GetUserDir(wxStandardPaths::Dir_Videos));
    wxPrintf(wxT("Executable path:\t%s\n"), stdp.GetExecutablePath());
    wxPrintf(wxT("Plugins dir:\t\t%s\n"), stdp.GetPluginsDir());
    wxPrintf(wxT("Resources dir:\t\t%s\n"), stdp.GetResourcesDir());
    wxPrintf(wxT("Localized res. dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir(wxT("fr")));
    wxPrintf(wxT("Message catalogs dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir
                  (
                    wxT("fr"),
                    wxStandardPaths::ResourceCat_Messages
                  ));

    wxPuts("\n");
#endif // TEST_STDPATHS
}


// ----------------------------------------------------------------------------
// wxVolume tests
// ----------------------------------------------------------------------------

#if (!defined(__WIN32__) && !defined(__APPLE__)) || defined(__WXOSX_IPHONE__) || !wxUSE_FSVOLUME
    #undef TEST_VOLUME
#endif

#ifdef TEST_VOLUME

#include "wx/volume.h"
static const wxChar *volumeKinds[] =
{
    wxT("floppy"),
    wxT("hard disk"),
    wxT("CD-ROM"),
    wxT("DVD-ROM"),
    wxT("network volume"),
    wxT("other volume"),
};

#endif

void InteractiveOutputTestCase::TestFSVolume()
{
#ifdef TEST_VOLUME
    wxPuts(wxT("*** Testing wxFSVolume class ***"));

    wxArrayString volumes = wxFSVolume::GetVolumes();
    size_t count = volumes.GetCount();

    if ( !count )
    {
        wxPuts(wxT("ERROR: no mounted volumes?"));
        return;
    }

    wxPrintf(wxT("%zu mounted volumes found:\n"), count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxFSVolume vol(volumes[n]);
        if ( !vol.IsOk() )
        {
            wxPuts(wxT("ERROR: couldn't create volume"));
            continue;
        }

        wxPrintf(wxT("%zu: %s (%s), %s, %s, %s\n"),
                 n + 1,
                 vol.GetDisplayName(),
                 vol.GetName(),
                 volumeKinds[vol.GetKind()],
                 vol.IsWritable() ? wxT("rw") : wxT("ro"),
                 vol.GetFlags() & wxFS_VOL_REMOVABLE ? wxT("removable")
                                                     : wxT("fixed"));
    }

    wxPuts("\n");
#endif // TEST_VOLUME
}

