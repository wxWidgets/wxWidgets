/////////////////////////////////////////////////////////////////////////////
// Name:        samples/console/console.cpp
// Purpose:     A sample console (as opposed to GUI) program using wxWidgets
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.10.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// IMPORTANT NOTE FOR WXWIDGETS USERS:
// If you're a wxWidgets user and you're looking at this file to learn how to
// structure a wxWidgets console application, then you don't have much to learn.
// This application is used more for testing rather than as sample but
// basically the following simple block is enough for you to start your
// own console application:

/*
    int main(int argc, char **argv)
    {
        wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

        wxInitializer initializer;
        if ( !initializer )
        {
            fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
            return -1;
        }

        static const wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
                wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
            // ... your other command line options here...

            { wxCMD_LINE_NONE }
        };

        wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);
        switch ( parser.Parse() )
        {
            case -1:
                wxLogMessage(wxT("Help was given, terminating."));
                break;

            case 0:
                // everything is ok; proceed
                break;

            default:
                wxLogMessage(wxT("Syntax error detected, aborting."));
                break;
        }

        // do something useful here

        return 0;
    }
*/


// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#include <stdio.h>

#include "wx/string.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/apptrait.h"
#include "wx/platinfo.h"
#include "wx/wxchar.h"

// without this pragma, the stupid compiler precompiles #defines below so that
// changing them doesn't "take place" later!
#ifdef __VISUALC__
    #pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

/*
   A note about all these conditional compilation macros: this file is used
   both as a test suite for various non-GUI wxWidgets classes and as a
   scratchpad for quick tests. So there are two compilation modes: if you
   define TEST_ALL all tests are run, otherwise you may enable the individual
   tests individually in the "#else" branch below.
 */

// what to test (in alphabetic order)? Define TEST_ALL to 0 to do a single
// test, define it to 1 to do all tests.
#define TEST_ALL 1

#if TEST_ALL
    #define TEST_DATETIME
    #define TEST_VOLUME
    #define TEST_STDPATHS
    #define TEST_STACKWALKER
    #define TEST_FTP
    #define TEST_SNGLINST
    #define TEST_REGEX
    #define TEST_INFO_FUNCTIONS
    #define TEST_MIME
    #define TEST_DYNLIB
#else // #if TEST_ALL
#endif

// some tests are interactive, define this to run them
#ifdef TEST_INTERACTIVE
    #undef TEST_INTERACTIVE

    #define TEST_INTERACTIVE 1
#else
    #define TEST_INTERACTIVE 1
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

#ifdef TEST_DYNLIB

#include "wx/dynlib.h"

#if defined(__WXMSW__) || defined(__UNIX__)

static void TestDllListLoaded()
{
    wxPuts(wxT("*** testing wxDynamicLibrary::ListLoaded() ***\n"));

    wxPuts("Loaded modules:");
    wxDynamicLibraryDetailsArray dlls = wxDynamicLibrary::ListLoaded();
    const size_t count = dlls.GetCount();
    for ( size_t n = 0; n < count; ++n )
    {
        const wxDynamicLibraryDetails& details = dlls[n];
        printf("%-45s", (const char *)details.GetPath().mb_str());

        void *addr wxDUMMY_INITIALIZE(NULL);
        size_t len wxDUMMY_INITIALIZE(0);
        if ( details.GetAddress(&addr, &len) )
        {
            printf(" %08lx:%08lx",
                   (unsigned long)addr, (unsigned long)((char *)addr + len));
        }

        printf(" %s\n", (const char *)details.GetVersion().mb_str());
    }
    
    wxPuts(wxEmptyString);
}

#endif

#endif // TEST_DYNLIB

// ----------------------------------------------------------------------------
// MIME types
// ----------------------------------------------------------------------------

#ifdef TEST_MIME

#include "wx/mimetype.h"

static void TestMimeEnum()
{
    wxPuts(wxT("*** Testing wxMimeTypesManager::EnumAllFileTypes() ***\n"));

    wxArrayString mimetypes;

    size_t count = wxTheMimeTypesManager->EnumAllFileTypes(mimetypes);

    wxPrintf(wxT("*** All %u known filetypes: ***\n"), count);

    wxArrayString exts;
    wxString desc;

    for ( size_t n = 0; n < count; n++ )
    {
        wxFileType *filetype =
            wxTheMimeTypesManager->GetFileTypeFromMimeType(mimetypes[n]);
        if ( !filetype )
        {
            wxPrintf(wxT("   nothing known about the filetype '%s'!\n"),
                   mimetypes[n].c_str());
            continue;
        }

        filetype->GetDescription(&desc);
        filetype->GetExtensions(exts);

        filetype->GetIcon(NULL);

        wxString extsAll;
        for ( size_t e = 0; e < exts.GetCount(); e++ )
        {
            if ( e > 0 )
                extsAll << wxT(", ");
            extsAll += exts[e];
        }

        wxPrintf(wxT("   %s: %s (%s)\n"),
               mimetypes[n].c_str(), desc.c_str(), extsAll.c_str());
    }

    wxPuts(wxEmptyString);
}

static void TestMimeFilename()
{
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
            wxPrintf(wxT("WARNING: extension '%s' is unknown.\n"), ext.c_str());
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

            wxPrintf(wxT("To open %s (%s) run:\n   %s\n"),
                     fname.c_str(), desc.c_str(), cmd.c_str());

            delete ft;
        }
    }

    wxPuts(wxEmptyString);
}

static void TestMimeAssociate()
{
    wxPuts(wxT("*** Testing creation of filetype association ***\n"));

    wxFileTypeInfo ftInfo(
                            wxT("application/x-xyz"),
                            wxT("xyzview '%s'"), // open cmd
                            wxT(""),             // print cmd
                            wxT("XYZ File"),     // description
                            wxT(".xyz"),         // extensions
                            wxNullPtr           // end of extensions
                         );
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
}

#endif // TEST_MIME


// ----------------------------------------------------------------------------
// misc information functions
// ----------------------------------------------------------------------------

#ifdef TEST_INFO_FUNCTIONS

#include "wx/utils.h"

#if TEST_INTERACTIVE
static void TestDiskInfo()
{
    wxPuts(wxT("*** Testing wxGetDiskSpace() ***"));

    for ( ;; )
    {
        wxChar pathname[128];
        wxPrintf(wxT("\nEnter a directory name (or 'quit' to escape): "));
        if ( !wxFgets(pathname, WXSIZEOF(pathname), stdin) )
            break;

        // kill the last '\n'
        pathname[wxStrlen(pathname) - 1] = 0;
        
        if (wxStrcmp(pathname, "quit") == 0)
            break;

        wxLongLong total, free;
        if ( !wxGetDiskSpace(pathname, &total, &free) )
        {
            wxPuts(wxT("ERROR: wxGetDiskSpace failed."));
        }
        else
        {
            wxPrintf(wxT("%sKb total, %sKb free on '%s'.\n"),
                    (total / 1024).ToString().c_str(),
                    (free / 1024).ToString().c_str(),
                    pathname);
        }
    }
}
#endif // TEST_INTERACTIVE

static void TestOsInfo()
{
    wxPuts(wxT("*** Testing OS info functions ***\n"));

    int major, minor;
    wxGetOsVersion(&major, &minor);
    wxPrintf(wxT("Running under: %s, version %d.%d\n"),
            wxGetOsDescription().c_str(), major, minor);

    wxPrintf(wxT("%ld free bytes of memory left.\n"), wxGetFreeMemory().ToLong());

    wxPrintf(wxT("Host name is %s (%s).\n"),
           wxGetHostName().c_str(), wxGetFullHostName().c_str());

    wxPuts(wxEmptyString);
}

static void TestPlatformInfo()
{
    wxPuts(wxT("*** Testing wxPlatformInfo functions ***\n"));

    // get this platform
    wxPlatformInfo plat;

    wxPrintf(wxT("Operating system family name is: %s\n"), plat.GetOperatingSystemFamilyName().c_str());
    wxPrintf(wxT("Operating system name is: %s\n"), plat.GetOperatingSystemIdName().c_str());
    wxPrintf(wxT("Port ID name is: %s\n"), plat.GetPortIdName().c_str());
    wxPrintf(wxT("Port ID short name is: %s\n"), plat.GetPortIdShortName().c_str());
    wxPrintf(wxT("Architecture is: %s\n"), plat.GetArchName().c_str());
    wxPrintf(wxT("Endianness is: %s\n"), plat.GetEndiannessName().c_str());

    wxPuts(wxEmptyString);
}

static void TestUserInfo()
{
    wxPuts(wxT("*** Testing user info functions ***\n"));

    wxPrintf(wxT("User id is:\t%s\n"), wxGetUserId().c_str());
    wxPrintf(wxT("User name is:\t%s\n"), wxGetUserName().c_str());
    wxPrintf(wxT("Home dir is:\t%s\n"), wxGetHomeDir().c_str());
    wxPrintf(wxT("Email address:\t%s\n"), wxGetEmailAddress().c_str());

    wxPuts(wxEmptyString);
}

#endif // TEST_INFO_FUNCTIONS

// ----------------------------------------------------------------------------
// regular expressions
// ----------------------------------------------------------------------------

#if defined TEST_REGEX && TEST_INTERACTIVE

#include "wx/regex.h"

static void TestRegExInteractive()
{
    wxPuts(wxT("*** Testing RE interactively ***"));

    for ( ;; )
    {
        wxChar pattern[128];
        wxPrintf(wxT("\nEnter a pattern (or 'quit' to escape): "));
        if ( !wxFgets(pattern, WXSIZEOF(pattern), stdin) )
            break;

        // kill the last '\n'
        pattern[wxStrlen(pattern) - 1] = 0;

        if (wxStrcmp(pattern, "quit") == 0)
            break;
            
        wxRegEx re;
        if ( !re.Compile(pattern) )
        {
            continue;
        }

        wxChar text[128];
        for ( ;; )
        {
            wxPrintf(wxT("Enter text to match: "));
            if ( !wxFgets(text, WXSIZEOF(text), stdin) )
                break;

            // kill the last '\n'
            text[wxStrlen(text) - 1] = 0;

            if ( !re.Matches(text) )
            {
                wxPrintf(wxT("No match.\n"));
            }
            else
            {
                wxPrintf(wxT("Pattern matches at '%s'\n"), re.GetMatch(text).c_str());

                size_t start, len;
                for ( size_t n = 1; ; n++ )
                {
                    if ( !re.GetMatch(&start, &len, n) )
                    {
                        break;
                    }

                    wxPrintf(wxT("Subexpr %u matched '%s'\n"),
                             n, wxString(text + start, len).c_str());
                }
            }
        }
    }
}

#endif // TEST_REGEX

// ----------------------------------------------------------------------------
// FTP
// ----------------------------------------------------------------------------

#ifdef TEST_FTP

#include "wx/protocol/ftp.h"
#include "wx/protocol/log.h"

#define FTP_ANONYMOUS

static wxFTP *ftp;

#ifdef FTP_ANONYMOUS
    static const wxChar *hostname = wxT("ftp.wxwidgets.org");
#else
    static const wxChar *hostname = "localhost";
#endif

static bool TestFtpConnect()
{
    wxPuts(wxT("*** Testing FTP connect ***"));

#ifdef FTP_ANONYMOUS
    wxPrintf(wxT("--- Attempting to connect to %s:21 anonymously...\n"), hostname);
#else // !FTP_ANONYMOUS
    wxChar user[256];
    wxFgets(user, WXSIZEOF(user), stdin);
    user[wxStrlen(user) - 1] = '\0'; // chop off '\n'
    ftp->SetUser(user);

    wxChar password[256];
    wxPrintf(wxT("Password for %s: "), password);
    wxFgets(password, WXSIZEOF(password), stdin);
    password[wxStrlen(password) - 1] = '\0'; // chop off '\n'
    ftp->SetPassword(password);

    wxPrintf(wxT("--- Attempting to connect to %s:21 as %s...\n"), hostname, user);
#endif // FTP_ANONYMOUS/!FTP_ANONYMOUS

    if ( !ftp->Connect(hostname) )
    {
        wxPrintf(wxT("ERROR: failed to connect to %s\n"), hostname);

        return false;
    }
    else
    {
        wxPrintf(wxT("--- Connected to %s, current directory is '%s'\n"),
                 hostname, ftp->Pwd().c_str());
        ftp->Close();
    }

    return true;
}

#if TEST_INTERACTIVE
static void TestFtpInteractive()
{
    wxPuts(wxT("\n*** Interactive wxFTP test ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(wxT("Enter FTP command (or 'quit' to escape): "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

        // special handling of LIST and NLST as they require data connection
        wxString start(buf, 4);
        start.MakeUpper();
        if ( start == wxT("LIST") || start == wxT("NLST") )
        {
            wxString wildcard;
            if ( wxStrlen(buf) > 4 )
                wildcard = buf + 5;

            wxArrayString files;
            if ( !ftp->GetList(files, wildcard, start == wxT("LIST")) )
            {
                wxPrintf(wxT("ERROR: failed to get %s of files\n"), start.c_str());
            }
            else
            {
                wxPrintf(wxT("--- %s of '%s' under '%s':\n"),
                       start.c_str(), wildcard.c_str(), ftp->Pwd().c_str());
                size_t count = files.GetCount();
                for ( size_t n = 0; n < count; n++ )
                {
                    wxPrintf(wxT("\t%s\n"), files[n].c_str());
                }
                wxPuts(wxT("--- End of the file list"));
            }
        }
        else if ( start == wxT("QUIT") )
        {
            break;      // get out of here!
        }
        else // !list
        {
            wxChar ch = ftp->SendCommand(buf);
            wxPrintf(wxT("Command %s"), ch ? wxT("succeeded") : wxT("failed"));
            if ( ch )
            {
                wxPrintf(wxT(" (return code %c)"), ch);
            }

            wxPrintf(wxT(", server reply:\n%s\n\n"), ftp->GetLastResult().c_str());
        }
    }

    wxPuts(wxT("\n"));
}
#endif // TEST_INTERACTIVE
#endif // TEST_FTP

// ----------------------------------------------------------------------------
// stack backtrace
// ----------------------------------------------------------------------------

#ifdef TEST_STACKWALKER

#if wxUSE_STACKWALKER

#include "wx/stackwalk.h"

class StackDump : public wxStackWalker
{
public:
    StackDump(const char *argv0)
        : wxStackWalker(argv0)
    {
    }

    virtual void Walk(size_t skip = 1, size_t maxdepth = wxSTACKWALKER_MAX_DEPTH)
    {
        wxPuts(wxT("Stack dump:"));

        wxStackWalker::Walk(skip, maxdepth);
    }

protected:
    virtual void OnStackFrame(const wxStackFrame& frame)
    {
        printf("[%2d] ", (int) frame.GetLevel());

        wxString name = frame.GetName();
        if ( !name.empty() )
        {
            printf("%-20.40s", (const char*)name.mb_str());
        }
        else
        {
            printf("0x%08lx", (unsigned long)frame.GetAddress());
        }

        if ( frame.HasSourceLocation() )
        {
            printf("\t%s:%d",
                   (const char*)frame.GetFileName().mb_str(),
                   (int)frame.GetLine());
        }

        puts("");

        wxString type, val;
        for ( size_t n = 0; frame.GetParam(n, &type, &name, &val); n++ )
        {
            printf("\t%s %s = %s\n", (const char*)type.mb_str(),
                                     (const char*)name.mb_str(),
                                     (const char*)val.mb_str());
        }
    }
};

static void TestStackWalk(const char *argv0)
{
    wxPuts(wxT("*** Testing wxStackWalker ***"));

    StackDump dump(argv0);
    dump.Walk();
    
    wxPuts("\n");
}

#endif // wxUSE_STACKWALKER

#endif // TEST_STACKWALKER

// ----------------------------------------------------------------------------
// standard paths
// ----------------------------------------------------------------------------

#ifdef TEST_STDPATHS

#include "wx/stdpaths.h"
#include "wx/wxchar.h"      // wxPrintf

static void TestStandardPaths()
{
    wxPuts(wxT("*** Testing wxStandardPaths ***"));

    wxTheApp->SetAppName(wxT("console"));

    wxStandardPathsBase& stdp = wxStandardPaths::Get();
    wxPrintf(wxT("Config dir (sys):\t%s\n"), stdp.GetConfigDir().c_str());
    wxPrintf(wxT("Config dir (user):\t%s\n"), stdp.GetUserConfigDir().c_str());
    wxPrintf(wxT("Data dir (sys):\t\t%s\n"), stdp.GetDataDir().c_str());
    wxPrintf(wxT("Data dir (sys local):\t%s\n"), stdp.GetLocalDataDir().c_str());
    wxPrintf(wxT("Data dir (user):\t%s\n"), stdp.GetUserDataDir().c_str());
    wxPrintf(wxT("Data dir (user local):\t%s\n"), stdp.GetUserLocalDataDir().c_str());
    wxPrintf(wxT("Documents dir:\t\t%s\n"), stdp.GetDocumentsDir().c_str());
    wxPrintf(wxT("Executable path:\t%s\n"), stdp.GetExecutablePath().c_str());
    wxPrintf(wxT("Plugins dir:\t\t%s\n"), stdp.GetPluginsDir().c_str());
    wxPrintf(wxT("Resources dir:\t\t%s\n"), stdp.GetResourcesDir().c_str());
    wxPrintf(wxT("Localized res. dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir(wxT("fr")).c_str());
    wxPrintf(wxT("Message catalogs dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir
                  (
                    wxT("fr"),
                    wxStandardPaths::ResourceCat_Messages
                  ).c_str());
                  
    wxPuts("\n");
}

#endif // TEST_STDPATHS

// ----------------------------------------------------------------------------
// wxVolume tests
// ----------------------------------------------------------------------------

#if !defined(__WIN32__) || !wxUSE_FSVOLUME
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

static void TestFSVolume()
{
    wxPuts(wxT("*** Testing wxFSVolume class ***"));

    wxArrayString volumes = wxFSVolume::GetVolumes();
    size_t count = volumes.GetCount();

    if ( !count )
    {
        wxPuts(wxT("ERROR: no mounted volumes?"));
        return;
    }

    wxPrintf(wxT("%u mounted volumes found:\n"), count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxFSVolume vol(volumes[n]);
        if ( !vol.IsOk() )
        {
            wxPuts(wxT("ERROR: couldn't create volume"));
            continue;
        }

        wxPrintf(wxT("%u: %s (%s), %s, %s, %s\n"),
                 n + 1,
                 vol.GetDisplayName().c_str(),
                 vol.GetName().c_str(),
                 volumeKinds[vol.GetKind()],
                 vol.IsWritable() ? wxT("rw") : wxT("ro"),
                 vol.GetFlags() & wxFS_VOL_REMOVABLE ? wxT("removable")
                                                     : wxT("fixed"));
    }
    
    wxPuts("\n");
}

#endif // TEST_VOLUME

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_DATETIME

#include "wx/math.h"
#include "wx/datetime.h"

#if TEST_INTERACTIVE

static void TestDateTimeInteractive()
{
    wxPuts(wxT("\n*** interactive wxDateTime tests ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(wxT("Enter a date (or 'quit' to escape): "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;
        
        if ( wxString(buf).CmpNoCase("quit") == 0 )
            break;

        wxDateTime dt;
        const wxChar *p = dt.ParseDate(buf);
        if ( !p )
        {
            wxPrintf(wxT("ERROR: failed to parse the date '%s'.\n"), buf);

            continue;
        }
        else if ( *p )
        {
            wxPrintf(wxT("WARNING: parsed only first %u characters.\n"), p - buf);
        }

        wxPrintf(wxT("%s: day %u, week of month %u/%u, week of year %u\n"),
                 dt.Format(wxT("%b %d, %Y")).c_str(),
                 dt.GetDayOfYear(),
                 dt.GetWeekOfMonth(wxDateTime::Monday_First),
                 dt.GetWeekOfMonth(wxDateTime::Sunday_First),
                 dt.GetWeekOfYear(wxDateTime::Monday_First));
    }
    
    wxPuts("\n");
}

#endif // TEST_INTERACTIVE
#endif // TEST_DATETIME

// ----------------------------------------------------------------------------
// single instance
// ----------------------------------------------------------------------------

#ifdef TEST_SNGLINST

#include "wx/snglinst.h"

static bool TestSingleIstance()
{
    wxPuts(wxT("\n*** Testing wxSingleInstanceChecker ***"));

    wxSingleInstanceChecker checker;
    if ( checker.Create(wxT(".wxconsole.lock")) )
    {
        if ( checker.IsAnotherRunning() )
        {
            wxPrintf(wxT("Another instance of the program is running, exiting.\n"));

            return false;
        }

        // wait some time to give time to launch another instance
        wxPuts(wxT("If you try to run another instance of this program now, it won't start."));
        wxPrintf(wxT("Press \"Enter\" to exit wxSingleInstanceChecker test and proceed..."));
        wxFgetc(stdin);
    }
    else // failed to create
    {
        wxPrintf(wxT("Failed to init wxSingleInstanceChecker.\n"));
    }
    
    wxPuts("\n");
    
    return true;
}
#endif // TEST_SNGLINST


// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];

    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else // !wxUSE_UNICODE
    #define wxArgv argv
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");

        return -1;
    }

#ifdef TEST_SNGLINST
    if (!TestSingleIstance())
        return 1;
#endif // TEST_SNGLINST

#ifdef TEST_DYNLIB
    TestDllListLoaded();
#endif // TEST_DYNLIB

#ifdef TEST_FTP
    wxLog::AddTraceMask(FTP_TRACE_MASK);

    // wxFTP cannot be a static variable as its ctor needs to access
    // wxWidgets internals after it has been initialized
    ftp = new wxFTP;
    ftp->SetLog(new wxProtocolLog(FTP_TRACE_MASK));
    if ( TestFtpConnect() )
        TestFtpInteractive();
    //else: connecting to the FTP server failed

    delete ftp;
#endif // TEST_FTP

#ifdef TEST_MIME
    TestMimeEnum();
    TestMimeAssociate();
    TestMimeFilename();
#endif // TEST_MIME

#ifdef TEST_INFO_FUNCTIONS
    TestOsInfo();
    TestPlatformInfo();
    TestUserInfo();

    #if TEST_INTERACTIVE
        TestDiskInfo();
    #endif
#endif // TEST_INFO_FUNCTIONS

#ifdef TEST_PRINTF
    TestPrintf();
#endif // TEST_PRINTF

#if defined TEST_REGEX && TEST_INTERACTIVE
    TestRegExInteractive();
#endif // defined TEST_REGEX && TEST_INTERACTIVE

#ifdef TEST_DATETIME
    #if TEST_INTERACTIVE
        TestDateTimeInteractive();
    #endif
#endif // TEST_DATETIME

#ifdef TEST_STACKWALKER
#if wxUSE_STACKWALKER
    TestStackWalk(argv[0]);
#endif
#endif // TEST_STACKWALKER

#ifdef TEST_STDPATHS
    TestStandardPaths();
#endif

#ifdef TEST_VOLUME
    TestFSVolume();
#endif // TEST_VOLUME

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wxArgv[n]);

        delete [] wxArgv;
    }
#endif // wxUSE_UNICODE

    wxUnusedVar(argc);
    wxUnusedVar(argv);
    return 0;
}
