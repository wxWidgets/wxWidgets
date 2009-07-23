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
    #define TEST_CMDLINE
    #define TEST_DATETIME
    #define TEST_DIR
    #define TEST_DYNLIB
    #define TEST_ENVIRON
    #define TEST_FILE
    #define TEST_FILECONF
    #define TEST_FILENAME
    #define TEST_FILETIME
    #define TEST_FTP
    #define TEST_INFO_FUNCTIONS
    #define TEST_LOCALE
    #define TEST_LOG
    #define TEST_MIME
    #define TEST_MODULE
    #define TEST_PATHLIST
    #define TEST_PRINTF
    #define TEST_REGCONF
    #define TEST_REGEX
    #define TEST_REGISTRY
    #define TEST_SCOPEGUARD
    #define TEST_SNGLINST
//    #define TEST_SOCKETS  --FIXME! (RN)
    #define TEST_STACKWALKER
    #define TEST_STDPATHS
    #define TEST_STREAMS
    #define TEST_TEXTSTREAM
    #define TEST_THREADS
    #define TEST_TIMER
//    #define TEST_VOLUME   --FIXME! (RN)
    #define TEST_WCHAR
    #define TEST_ZIP
#else // #if TEST_ALL
    #define TEST_FTP
#endif

// some tests are interactive, define this to run them
#ifdef TEST_INTERACTIVE
    #undef TEST_INTERACTIVE

    #define TEST_INTERACTIVE 1
#else
    #define TEST_INTERACTIVE 0
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

#if defined(TEST_SOCKETS)

// replace TABs with \t and CRs with \n
static wxString MakePrintable(const wxChar *s)
{
    wxString str(s);
    (void)str.Replace(wxT("\t"), wxT("\\t"));
    (void)str.Replace(wxT("\n"), wxT("\\n"));
    (void)str.Replace(wxT("\r"), wxT("\\r"));

    return str;
}

#endif // MakePrintable() is used

// ----------------------------------------------------------------------------
// wxCmdLineParser
// ----------------------------------------------------------------------------

#ifdef TEST_CMDLINE

#include "wx/cmdline.h"
#include "wx/datetime.h"

#if wxUSE_CMDLINE_PARSER

static void ShowCmdLine(const wxCmdLineParser& parser)
{
    wxString s = wxT("Command line parsed successfully:\nInput files: ");

    size_t count = parser.GetParamCount();
    for ( size_t param = 0; param < count; param++ )
    {
        s << parser.GetParam(param) << ' ';
    }

    s << '\n'
      << wxT("Verbose:\t") << (parser.Found(wxT("v")) ? wxT("yes") : wxT("no")) << '\n'
      << wxT("Quiet:\t") << (parser.Found(wxT("q")) ? wxT("yes") : wxT("no")) << '\n';

    wxString strVal;
    long lVal;
    double dVal;
    wxDateTime dt;
    if ( parser.Found(wxT("o"), &strVal) )
        s << wxT("Output file:\t") << strVal << '\n';
    if ( parser.Found(wxT("i"), &strVal) )
        s << wxT("Input dir:\t") << strVal << '\n';
    if ( parser.Found(wxT("s"), &lVal) )
        s << wxT("Size:\t") << lVal << '\n';
    if ( parser.Found(wxT("f"), &dVal) )
        s << wxT("Double:\t") << dVal << '\n';
    if ( parser.Found(wxT("d"), &dt) )
        s << wxT("Date:\t") << dt.FormatISODate() << '\n';
    if ( parser.Found(wxT("project_name"), &strVal) )
        s << wxT("Project:\t") << strVal << '\n';

    wxLogMessage(s);
}

#endif // wxUSE_CMDLINE_PARSER

static void TestCmdLineConvert()
{
    static const wxChar *cmdlines[] =
    {
        wxT("arg1 arg2"),
        wxT("-a \"-bstring 1\" -c\"string 2\" \"string 3\""),
        wxT("literal \\\" and \"\""),
    };

    for ( size_t n = 0; n < WXSIZEOF(cmdlines); n++ )
    {
        const wxChar *cmdline = cmdlines[n];
        wxPrintf(wxT("Parsing: %s\n"), cmdline);
        wxArrayString args = wxCmdLineParser::ConvertStringToArgs(cmdline);

        size_t count = args.GetCount();
        wxPrintf(wxT("\targc = %u\n"), count);
        for ( size_t arg = 0; arg < count; arg++ )
        {
            wxPrintf(wxT("\targv[%u] = %s\n"), arg, args[arg].c_str());
        }
    }
}

#endif // TEST_CMDLINE

// ----------------------------------------------------------------------------
// wxDir
// ----------------------------------------------------------------------------

#ifdef TEST_DIR

#include "wx/dir.h"

#ifdef __UNIX__
    static const wxChar *ROOTDIR = wxT("/");
    static const wxChar *TESTDIR = wxT("/usr/local/share");
#elif defined(__WXMSW__) || defined(__DOS__) || defined(__OS2__)
    static const wxChar *ROOTDIR = wxT("c:\\");
    static const wxChar *TESTDIR = wxT("d:\\");
#else
    #error "don't know where the root directory is"
#endif

static void TestDirEnumHelper(wxDir& dir,
                              int flags = wxDIR_DEFAULT,
                              const wxString& filespec = wxEmptyString)
{
    wxString filename;

    if ( !dir.IsOpened() )
        return;

    bool cont = dir.GetFirst(&filename, filespec, flags);
    while ( cont )
    {
        wxPrintf(wxT("\t%s\n"), filename.c_str());

        cont = dir.GetNext(&filename);
    }

    wxPuts(wxEmptyString);
}

#if TEST_ALL

static void TestDirEnum()
{
    wxPuts(wxT("*** Testing wxDir::GetFirst/GetNext ***"));

    wxString cwd = wxGetCwd();
    if ( !wxDir::Exists(cwd) )
    {
        wxPrintf(wxT("ERROR: current directory '%s' doesn't exist?\n"), cwd.c_str());
        return;
    }

    wxDir dir(cwd);
    if ( !dir.IsOpened() )
    {
        wxPrintf(wxT("ERROR: failed to open current directory '%s'.\n"), cwd.c_str());
        return;
    }

    wxPuts(wxT("Enumerating everything in current directory:"));
    TestDirEnumHelper(dir);

    wxPuts(wxT("Enumerating really everything in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT);

    wxPuts(wxT("Enumerating object files in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT, wxT("*.o*"));

    wxPuts(wxT("Enumerating directories in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DIRS);

    wxPuts(wxT("Enumerating files in current directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES);

    wxPuts(wxT("Enumerating files including hidden in current directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    dir.Open(ROOTDIR);

    wxPuts(wxT("Enumerating everything in root directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT);

    wxPuts(wxT("Enumerating directories in root directory:"));
    TestDirEnumHelper(dir, wxDIR_DIRS);

    wxPuts(wxT("Enumerating files in root directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES);

    wxPuts(wxT("Enumerating files including hidden in root directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    wxPuts(wxT("Enumerating files in non existing directory:"));
    wxDir dirNo(wxT("nosuchdir"));
    TestDirEnumHelper(dirNo);
}

#endif // TEST_ALL

class DirPrintTraverser : public wxDirTraverser
{
public:
    virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
    {
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& dirname)
    {
        wxString path, name, ext;
        wxFileName::SplitPath(dirname, &path, &name, &ext);

        if ( !ext.empty() )
            name << wxT('.') << ext;

        wxString indent;
        for ( const wxChar *p = path.c_str(); *p; p++ )
        {
            if ( wxIsPathSeparator(*p) )
                indent += wxT("    ");
        }

        wxPrintf(wxT("%s%s\n"), indent.c_str(), name.c_str());

        return wxDIR_CONTINUE;
    }
};

static void TestDirTraverse()
{
    wxPuts(wxT("*** Testing wxDir::Traverse() ***"));

    // enum all files
    wxArrayString files;
    size_t n = wxDir::GetAllFiles(TESTDIR, &files);
    wxPrintf(wxT("There are %u files under '%s'\n"), n, TESTDIR);
    if ( n > 1 )
    {
        wxPrintf(wxT("First one is '%s'\n"), files[0u].c_str());
        wxPrintf(wxT(" last one is '%s'\n"), files[n - 1].c_str());
    }

    // enum again with custom traverser
    wxPuts(wxT("Now enumerating directories:"));
    wxDir dir(TESTDIR);
    DirPrintTraverser traverser;
    dir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
}

#if TEST_ALL

static void TestDirExists()
{
    wxPuts(wxT("*** Testing wxDir::Exists() ***"));

    static const wxChar *dirnames[] =
    {
        wxT("."),
#if defined(__WXMSW__)
        wxT("c:"),
        wxT("c:\\"),
        wxT("\\\\share\\file"),
        wxT("c:\\dos"),
        wxT("c:\\dos\\"),
        wxT("c:\\dos\\\\"),
        wxT("c:\\autoexec.bat"),
#elif defined(__UNIX__)
        wxT("/"),
        wxT("//"),
        wxT("/usr/bin"),
        wxT("/usr//bin"),
        wxT("/usr///bin"),
#endif
    };

    for ( size_t n = 0; n < WXSIZEOF(dirnames); n++ )
    {
        wxPrintf(wxT("%-40s: %s\n"),
                 dirnames[n],
                 wxDir::Exists(dirnames[n]) ? wxT("exists")
                                            : wxT("doesn't exist"));
    }
}

#endif // TEST_ALL

#endif // TEST_DIR

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

#ifdef TEST_DYNLIB

#include "wx/dynlib.h"

static void TestDllLoad()
{
#if defined(__WXMSW__)
    static const wxChar *LIB_NAME = wxT("kernel32.dll");
    static const wxChar *FUNC_NAME = wxT("lstrlenA");
#elif defined(__UNIX__)
    // weird: using just libc.so does *not* work!
    static const wxChar *LIB_NAME = wxT("/lib/libc.so.6");
    static const wxChar *FUNC_NAME = wxT("strlen");
#else
    #error "don't know how to test wxDllLoader on this platform"
#endif

    wxPuts(wxT("*** testing basic wxDynamicLibrary functions ***\n"));

    wxDynamicLibrary lib(LIB_NAME);
    if ( !lib.IsLoaded() )
    {
        wxPrintf(wxT("ERROR: failed to load '%s'.\n"), LIB_NAME);
    }
    else
    {
        typedef int (wxSTDCALL *wxStrlenType)(const char *);
        wxStrlenType pfnStrlen = (wxStrlenType)lib.GetSymbol(FUNC_NAME);
        if ( !pfnStrlen )
        {
            wxPrintf(wxT("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME, LIB_NAME);
        }
        else
        {
            wxPrintf(wxT("Calling %s dynamically loaded from %s "),
                     FUNC_NAME, LIB_NAME);

            if ( pfnStrlen("foo") != 3 )
            {
                wxPrintf(wxT("ERROR: loaded function is not wxStrlen()!\n"));
            }
            else
            {
                wxPuts(wxT("... ok"));
            }
        }

#ifdef __WXMSW__
        static const wxChar *FUNC_NAME_AW = wxT("lstrlen");

        typedef int (wxSTDCALL *wxStrlenTypeAorW)(const wxChar *);
        wxStrlenTypeAorW
            pfnStrlenAorW = (wxStrlenTypeAorW)lib.GetSymbolAorW(FUNC_NAME_AW);
        if ( !pfnStrlenAorW )
        {
            wxPrintf(wxT("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME_AW, LIB_NAME);
        }
        else
        {
            if ( pfnStrlenAorW(wxT("foobar")) != 6 )
            {
                wxPrintf(wxT("ERROR: loaded function is not wxStrlen()!\n"));
            }
        }
#endif // __WXMSW__
    }
}

#if defined(__WXMSW__) || defined(__UNIX__)

static void TestDllListLoaded()
{
    wxPuts(wxT("*** testing wxDynamicLibrary::ListLoaded() ***\n"));

    puts("\nLoaded modules:");
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
}

#endif

#endif // TEST_DYNLIB

// ----------------------------------------------------------------------------
// wxGet/SetEnv
// ----------------------------------------------------------------------------

#ifdef TEST_ENVIRON

#include "wx/utils.h"

static wxString MyGetEnv(const wxString& var)
{
    wxString val;
    if ( !wxGetEnv(var, &val) )
        val = wxT("<empty>");
    else
        val = wxString(wxT('\'')) + val + wxT('\'');

    return val;
}

static void TestEnvironment()
{
    const wxChar *var = wxT("wxTestVar");

    wxPuts(wxT("*** testing environment access functions ***"));

    wxPrintf(wxT("Initially getenv(%s) = %s\n"), var, MyGetEnv(var).c_str());
    wxSetEnv(var, wxT("value for wxTestVar"));
    wxPrintf(wxT("After wxSetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxSetEnv(var, wxT("another value"));
    wxPrintf(wxT("After 2nd wxSetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxUnsetEnv(var);
    wxPrintf(wxT("After wxUnsetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxPrintf(wxT("PATH = %s\n"),  MyGetEnv(wxT("PATH")).c_str());
}

#endif // TEST_ENVIRON

// ----------------------------------------------------------------------------
// file
// ----------------------------------------------------------------------------

#ifdef TEST_FILE

#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/textfile.h"

static void TestFileRead()
{
    wxPuts(wxT("*** wxFile read test ***"));

    wxFile file(wxT("testdata.fc"));
    if ( file.IsOpened() )
    {
        wxPrintf(wxT("File length: %lu\n"), file.Length());

        wxPuts(wxT("File dump:\n----------"));

        static const size_t len = 1024;
        wxChar buf[len];
        for ( ;; )
        {
            size_t nRead = file.Read(buf, len);
            if ( nRead == (size_t)wxInvalidOffset )
            {
                wxPrintf(wxT("Failed to read the file."));
                break;
            }

            fwrite(buf, nRead, 1, stdout);

            if ( nRead < len )
                break;
        }

        wxPuts(wxT("----------"));
    }
    else
    {
        wxPrintf(wxT("ERROR: can't open test file.\n"));
    }

    wxPuts(wxEmptyString);
}

static void TestTextFileRead()
{
    wxPuts(wxT("*** wxTextFile read test ***"));

    wxTextFile file(wxT("testdata.fc"));
    if ( file.Open() )
    {
        wxPrintf(wxT("Number of lines: %u\n"), file.GetLineCount());
        wxPrintf(wxT("Last line: '%s'\n"), file.GetLastLine().c_str());

        wxString s;

        wxPuts(wxT("\nDumping the entire file:"));
        for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
        {
            wxPrintf(wxT("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
        }
        wxPrintf(wxT("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());

        wxPuts(wxT("\nAnd now backwards:"));
        for ( s = file.GetLastLine();
              file.GetCurrentLine() != 0;
              s = file.GetPrevLine() )
        {
            wxPrintf(wxT("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
        }
        wxPrintf(wxT("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
    }
    else
    {
        wxPrintf(wxT("ERROR: can't open '%s'\n"), file.GetName());
    }

    wxPuts(wxEmptyString);
}

static void TestFileCopy()
{
    wxPuts(wxT("*** Testing wxCopyFile ***"));

    static const wxChar *filename1 = wxT("testdata.fc");
    static const wxChar *filename2 = wxT("test2");
    if ( !wxCopyFile(filename1, filename2) )
    {
        wxPuts(wxT("ERROR: failed to copy file"));
    }
    else
    {
        wxFFile f1(filename1, wxT("rb")),
                f2(filename2, wxT("rb"));

        if ( !f1.IsOpened() || !f2.IsOpened() )
        {
            wxPuts(wxT("ERROR: failed to open file(s)"));
        }
        else
        {
            wxString s1, s2;
            if ( !f1.ReadAll(&s1) || !f2.ReadAll(&s2) )
            {
                wxPuts(wxT("ERROR: failed to read file(s)"));
            }
            else
            {
                if ( (s1.length() != s2.length()) ||
                     (memcmp(s1.c_str(), s2.c_str(), s1.length()) != 0) )
                {
                    wxPuts(wxT("ERROR: copy error!"));
                }
                else
                {
                    wxPuts(wxT("File was copied ok."));
                }
            }
        }
    }

    if ( !wxRemoveFile(filename2) )
    {
        wxPuts(wxT("ERROR: failed to remove the file"));
    }

    wxPuts(wxEmptyString);
}

static void TestTempFile()
{
    wxPuts(wxT("*** wxTempFile test ***"));

    wxTempFile tmpFile;
    if ( tmpFile.Open(wxT("test2")) && tmpFile.Write(wxT("the answer is 42")) )
    {
        if ( tmpFile.Commit() )
            wxPuts(wxT("File committed."));
        else
            wxPuts(wxT("ERROR: could't commit temp file."));

        wxRemoveFile(wxT("test2"));
    }

    wxPuts(wxEmptyString);
}

#endif // TEST_FILE

// ----------------------------------------------------------------------------
// wxFileConfig
// ----------------------------------------------------------------------------

#ifdef TEST_FILECONF

#include "wx/confbase.h"
#include "wx/fileconf.h"

static const struct FileConfTestData
{
    const wxChar *name;      // value name
    const wxChar *value;     // the value from the file
} fcTestData[] =
{
    { wxT("value1"),                       wxT("one") },
    { wxT("value2"),                       wxT("two") },
    { wxT("novalue"),                      wxT("default") },
};

static void TestFileConfRead()
{
    wxPuts(wxT("*** testing wxFileConfig loading/reading ***"));

    wxFileConfig fileconf(wxT("test"), wxEmptyString,
                          wxT("testdata.fc"), wxEmptyString,
                          wxCONFIG_USE_RELATIVE_PATH);

    // test simple reading
    wxPuts(wxT("\nReading config file:"));
    wxString defValue(wxT("default")), value;
    for ( size_t n = 0; n < WXSIZEOF(fcTestData); n++ )
    {
        const FileConfTestData& data = fcTestData[n];
        value = fileconf.Read(data.name, defValue);
        wxPrintf(wxT("\t%s = %s "), data.name, value.c_str());
        if ( value == data.value )
        {
            wxPuts(wxT("(ok)"));
        }
        else
        {
            wxPrintf(wxT("(ERROR: should be %s)\n"), data.value);
        }
    }

    // test enumerating the entries
    wxPuts(wxT("\nEnumerating all root entries:"));
    long dummy;
    wxString name;
    bool cont = fileconf.GetFirstEntry(name, dummy);
    while ( cont )
    {
        wxPrintf(wxT("\t%s = %s\n"),
               name.c_str(),
               fileconf.Read(name.c_str(), wxT("ERROR")).c_str());

        cont = fileconf.GetNextEntry(name, dummy);
    }

    static const wxChar *testEntry = wxT("TestEntry");
    wxPrintf(wxT("\nTesting deletion of newly created \"Test\" entry: "));
    fileconf.Write(testEntry, wxT("A value"));
    fileconf.DeleteEntry(testEntry);
    wxPrintf(fileconf.HasEntry(testEntry) ? wxT("ERROR\n") : wxT("ok\n"));
}

#endif // TEST_FILECONF

// ----------------------------------------------------------------------------
// wxFileName
// ----------------------------------------------------------------------------

#ifdef TEST_FILENAME

#include "wx/filename.h"

#if 0
static void DumpFileName(const wxChar *desc, const wxFileName& fn)
{
    wxPuts(desc);

    wxString full = fn.GetFullPath();

    wxString vol, path, name, ext;
    wxFileName::SplitPath(full, &vol, &path, &name, &ext);

    wxPrintf(wxT("'%s'-> vol '%s', path '%s', name '%s', ext '%s'\n"),
             full.c_str(), vol.c_str(), path.c_str(), name.c_str(), ext.c_str());

    wxFileName::SplitPath(full, &path, &name, &ext);
    wxPrintf(wxT("or\t\t-> path '%s', name '%s', ext '%s'\n"),
             path.c_str(), name.c_str(), ext.c_str());

    wxPrintf(wxT("path is also:\t'%s'\n"), fn.GetPath().c_str());
    wxPrintf(wxT("with volume: \t'%s'\n"),
             fn.GetPath(wxPATH_GET_VOLUME).c_str());
    wxPrintf(wxT("with separator:\t'%s'\n"),
             fn.GetPath(wxPATH_GET_SEPARATOR).c_str());
    wxPrintf(wxT("with both:   \t'%s'\n"),
             fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).c_str());

    wxPuts(wxT("The directories in the path are:"));
    wxArrayString dirs = fn.GetDirs();
    size_t count = dirs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxPrintf(wxT("\t%u: %s\n"), n, dirs[n].c_str());
    }
}
#endif

static void TestFileNameTemp()
{
    wxPuts(wxT("*** testing wxFileName temp file creation ***"));

    static const wxChar *tmpprefixes[] =
    {
        wxT(""),
        wxT("foo"),
        wxT(".."),
        wxT("../bar"),
#ifdef __UNIX__
        wxT("/tmp/foo"),
        wxT("/tmp/foo/bar"), // this one must be an error
#endif // __UNIX__
    };

    for ( size_t n = 0; n < WXSIZEOF(tmpprefixes); n++ )
    {
        wxString path = wxFileName::CreateTempFileName(tmpprefixes[n]);
        if ( path.empty() )
        {
            // "error" is not in upper case because it may be ok
            wxPrintf(wxT("Prefix '%s'\t-> error\n"), tmpprefixes[n]);
        }
        else
        {
            wxPrintf(wxT("Prefix '%s'\t-> temp file '%s'\n"),
                   tmpprefixes[n], path.c_str());

            if ( !wxRemoveFile(path) )
            {
                wxLogWarning(wxT("Failed to remove temp file '%s'"),
                             path.c_str());
            }
        }
    }
}

static void TestFileNameDirManip()
{
    // TODO: test AppendDir(), RemoveDir(), ...
}

static void TestFileNameComparison()
{
    // TODO!
}

static void TestFileNameOperations()
{
    // TODO!
}

static void TestFileNameCwd()
{
    // TODO!
}

#endif // TEST_FILENAME

// ----------------------------------------------------------------------------
// wxFileName time functions
// ----------------------------------------------------------------------------

#ifdef TEST_FILETIME

#include "wx/filename.h"
#include "wx/datetime.h"

static void TestFileGetTimes()
{
    wxFileName fn(wxT("testdata.fc"));

    wxDateTime dtAccess, dtMod, dtCreate;
    if ( !fn.GetTimes(&dtAccess, &dtMod, &dtCreate) )
    {
        wxPrintf(wxT("ERROR: GetTimes() failed.\n"));
    }
    else
    {
        static const wxChar *fmt = wxT("%Y-%b-%d %H:%M:%S");

        wxPrintf(wxT("File times for '%s':\n"), fn.GetFullPath().c_str());
        wxPrintf(wxT("Creation:    \t%s\n"), dtCreate.Format(fmt).c_str());
        wxPrintf(wxT("Last read:   \t%s\n"), dtAccess.Format(fmt).c_str());
        wxPrintf(wxT("Last write:  \t%s\n"), dtMod.Format(fmt).c_str());
    }
}

#if 0
static void TestFileSetTimes()
{
    wxFileName fn(wxT("testdata.fc"));

    if ( !fn.Touch() )
    {
        wxPrintf(wxT("ERROR: Touch() failed.\n"));
    }
}
#endif

#endif // TEST_FILETIME

// ----------------------------------------------------------------------------
// wxLocale
// ----------------------------------------------------------------------------

#ifdef TEST_LOCALE

#include "wx/intl.h"
#include "wx/utils.h"   // for wxSetEnv

static wxLocale gs_localeDefault;
    // NOTE: don't init it here as it needs a wxAppTraits object
    //       and thus must be init-ed after creation of the wxInitializer
    //       class in the main()

// find the name of the language from its value
static const wxChar *GetLangName(int lang)
{
    static const wxChar *languageNames[] =
    {
        wxT("DEFAULT"),
        wxT("UNKNOWN"),
        wxT("ABKHAZIAN"),
        wxT("AFAR"),
        wxT("AFRIKAANS"),
        wxT("ALBANIAN"),
        wxT("AMHARIC"),
        wxT("ARABIC"),
        wxT("ARABIC_ALGERIA"),
        wxT("ARABIC_BAHRAIN"),
        wxT("ARABIC_EGYPT"),
        wxT("ARABIC_IRAQ"),
        wxT("ARABIC_JORDAN"),
        wxT("ARABIC_KUWAIT"),
        wxT("ARABIC_LEBANON"),
        wxT("ARABIC_LIBYA"),
        wxT("ARABIC_MOROCCO"),
        wxT("ARABIC_OMAN"),
        wxT("ARABIC_QATAR"),
        wxT("ARABIC_SAUDI_ARABIA"),
        wxT("ARABIC_SUDAN"),
        wxT("ARABIC_SYRIA"),
        wxT("ARABIC_TUNISIA"),
        wxT("ARABIC_UAE"),
        wxT("ARABIC_YEMEN"),
        wxT("ARMENIAN"),
        wxT("ASSAMESE"),
        wxT("AYMARA"),
        wxT("AZERI"),
        wxT("AZERI_CYRILLIC"),
        wxT("AZERI_LATIN"),
        wxT("BASHKIR"),
        wxT("BASQUE"),
        wxT("BELARUSIAN"),
        wxT("BENGALI"),
        wxT("BHUTANI"),
        wxT("BIHARI"),
        wxT("BISLAMA"),
        wxT("BRETON"),
        wxT("BULGARIAN"),
        wxT("BURMESE"),
        wxT("CAMBODIAN"),
        wxT("CATALAN"),
        wxT("CHINESE"),
        wxT("CHINESE_SIMPLIFIED"),
        wxT("CHINESE_TRADITIONAL"),
        wxT("CHINESE_HONGKONG"),
        wxT("CHINESE_MACAU"),
        wxT("CHINESE_SINGAPORE"),
        wxT("CHINESE_TAIWAN"),
        wxT("CORSICAN"),
        wxT("CROATIAN"),
        wxT("CZECH"),
        wxT("DANISH"),
        wxT("DUTCH"),
        wxT("DUTCH_BELGIAN"),
        wxT("ENGLISH"),
        wxT("ENGLISH_UK"),
        wxT("ENGLISH_US"),
        wxT("ENGLISH_AUSTRALIA"),
        wxT("ENGLISH_BELIZE"),
        wxT("ENGLISH_BOTSWANA"),
        wxT("ENGLISH_CANADA"),
        wxT("ENGLISH_CARIBBEAN"),
        wxT("ENGLISH_DENMARK"),
        wxT("ENGLISH_EIRE"),
        wxT("ENGLISH_JAMAICA"),
        wxT("ENGLISH_NEW_ZEALAND"),
        wxT("ENGLISH_PHILIPPINES"),
        wxT("ENGLISH_SOUTH_AFRICA"),
        wxT("ENGLISH_TRINIDAD"),
        wxT("ENGLISH_ZIMBABWE"),
        wxT("ESPERANTO"),
        wxT("ESTONIAN"),
        wxT("FAEROESE"),
        wxT("FARSI"),
        wxT("FIJI"),
        wxT("FINNISH"),
        wxT("FRENCH"),
        wxT("FRENCH_BELGIAN"),
        wxT("FRENCH_CANADIAN"),
        wxT("FRENCH_LUXEMBOURG"),
        wxT("FRENCH_MONACO"),
        wxT("FRENCH_SWISS"),
        wxT("FRISIAN"),
        wxT("GALICIAN"),
        wxT("GEORGIAN"),
        wxT("GERMAN"),
        wxT("GERMAN_AUSTRIAN"),
        wxT("GERMAN_BELGIUM"),
        wxT("GERMAN_LIECHTENSTEIN"),
        wxT("GERMAN_LUXEMBOURG"),
        wxT("GERMAN_SWISS"),
        wxT("GREEK"),
        wxT("GREENLANDIC"),
        wxT("GUARANI"),
        wxT("GUJARATI"),
        wxT("HAUSA"),
        wxT("HEBREW"),
        wxT("HINDI"),
        wxT("HUNGARIAN"),
        wxT("ICELANDIC"),
        wxT("INDONESIAN"),
        wxT("INTERLINGUA"),
        wxT("INTERLINGUE"),
        wxT("INUKTITUT"),
        wxT("INUPIAK"),
        wxT("IRISH"),
        wxT("ITALIAN"),
        wxT("ITALIAN_SWISS"),
        wxT("JAPANESE"),
        wxT("JAVANESE"),
        wxT("KANNADA"),
        wxT("KASHMIRI"),
        wxT("KASHMIRI_INDIA"),
        wxT("KAZAKH"),
        wxT("KERNEWEK"),
        wxT("KINYARWANDA"),
        wxT("KIRGHIZ"),
        wxT("KIRUNDI"),
        wxT("KONKANI"),
        wxT("KOREAN"),
        wxT("KURDISH"),
        wxT("LAOTHIAN"),
        wxT("LATIN"),
        wxT("LATVIAN"),
        wxT("LINGALA"),
        wxT("LITHUANIAN"),
        wxT("MACEDONIAN"),
        wxT("MALAGASY"),
        wxT("MALAY"),
        wxT("MALAYALAM"),
        wxT("MALAY_BRUNEI_DARUSSALAM"),
        wxT("MALAY_MALAYSIA"),
        wxT("MALTESE"),
        wxT("MANIPURI"),
        wxT("MAORI"),
        wxT("MARATHI"),
        wxT("MOLDAVIAN"),
        wxT("MONGOLIAN"),
        wxT("NAURU"),
        wxT("NEPALI"),
        wxT("NEPALI_INDIA"),
        wxT("NORWEGIAN_BOKMAL"),
        wxT("NORWEGIAN_NYNORSK"),
        wxT("OCCITAN"),
        wxT("ORIYA"),
        wxT("OROMO"),
        wxT("PASHTO"),
        wxT("POLISH"),
        wxT("PORTUGUESE"),
        wxT("PORTUGUESE_BRAZILIAN"),
        wxT("PUNJABI"),
        wxT("QUECHUA"),
        wxT("RHAETO_ROMANCE"),
        wxT("ROMANIAN"),
        wxT("RUSSIAN"),
        wxT("RUSSIAN_UKRAINE"),
        wxT("SAMOAN"),
        wxT("SANGHO"),
        wxT("SANSKRIT"),
        wxT("SCOTS_GAELIC"),
        wxT("SERBIAN"),
        wxT("SERBIAN_CYRILLIC"),
        wxT("SERBIAN_LATIN"),
        wxT("SERBO_CROATIAN"),
        wxT("SESOTHO"),
        wxT("SETSWANA"),
        wxT("SHONA"),
        wxT("SINDHI"),
        wxT("SINHALESE"),
        wxT("SISWATI"),
        wxT("SLOVAK"),
        wxT("SLOVENIAN"),
        wxT("SOMALI"),
        wxT("SPANISH"),
        wxT("SPANISH_ARGENTINA"),
        wxT("SPANISH_BOLIVIA"),
        wxT("SPANISH_CHILE"),
        wxT("SPANISH_COLOMBIA"),
        wxT("SPANISH_COSTA_RICA"),
        wxT("SPANISH_DOMINICAN_REPUBLIC"),
        wxT("SPANISH_ECUADOR"),
        wxT("SPANISH_EL_SALVADOR"),
        wxT("SPANISH_GUATEMALA"),
        wxT("SPANISH_HONDURAS"),
        wxT("SPANISH_MEXICAN"),
        wxT("SPANISH_MODERN"),
        wxT("SPANISH_NICARAGUA"),
        wxT("SPANISH_PANAMA"),
        wxT("SPANISH_PARAGUAY"),
        wxT("SPANISH_PERU"),
        wxT("SPANISH_PUERTO_RICO"),
        wxT("SPANISH_URUGUAY"),
        wxT("SPANISH_US"),
        wxT("SPANISH_VENEZUELA"),
        wxT("SUNDANESE"),
        wxT("SWAHILI"),
        wxT("SWEDISH"),
        wxT("SWEDISH_FINLAND"),
        wxT("TAGALOG"),
        wxT("TAJIK"),
        wxT("TAMIL"),
        wxT("TATAR"),
        wxT("TELUGU"),
        wxT("THAI"),
        wxT("TIBETAN"),
        wxT("TIGRINYA"),
        wxT("TONGA"),
        wxT("TSONGA"),
        wxT("TURKISH"),
        wxT("TURKMEN"),
        wxT("TWI"),
        wxT("UIGHUR"),
        wxT("UKRAINIAN"),
        wxT("URDU"),
        wxT("URDU_INDIA"),
        wxT("URDU_PAKISTAN"),
        wxT("UZBEK"),
        wxT("UZBEK_CYRILLIC"),
        wxT("UZBEK_LATIN"),
        wxT("VIETNAMESE"),
        wxT("VOLAPUK"),
        wxT("WELSH"),
        wxT("WOLOF"),
        wxT("XHOSA"),
        wxT("YIDDISH"),
        wxT("YORUBA"),
        wxT("ZHUANG"),
        wxT("ZULU"),
    };

    if ( (size_t)lang < WXSIZEOF(languageNames) )
        return languageNames[lang];
    else
        return wxT("INVALID");
}

static void TestDefaultLang()
{
    wxPuts(wxT("*** Testing wxLocale::GetSystemLanguage ***"));

    gs_localeDefault.Init(wxLANGUAGE_ENGLISH);

    static const wxChar *langStrings[] =
    {
        NULL,               // system default
        wxT("C"),
        wxT("fr"),
        wxT("fr_FR"),
        wxT("en"),
        wxT("en_GB"),
        wxT("en_US"),
        wxT("de_DE.iso88591"),
        wxT("german"),
        wxT("?"),            // invalid lang spec
        wxT("klingonese"),   // I bet on some systems it does exist...
    };

    wxPrintf(wxT("The default system encoding is %s (%d)\n"),
             wxLocale::GetSystemEncodingName().c_str(),
             wxLocale::GetSystemEncoding());

    for ( size_t n = 0; n < WXSIZEOF(langStrings); n++ )
    {
        const wxChar *langStr = langStrings[n];
        if ( langStr )
        {
            // FIXME: this doesn't do anything at all under Windows, we need
            //        to create a new wxLocale!
            wxSetEnv(wxT("LC_ALL"), langStr);
        }

        int lang = gs_localeDefault.GetSystemLanguage();
        wxPrintf(wxT("Locale for '%s' is %s.\n"),
                 langStr ? langStr : wxT("system default"), GetLangName(lang));
    }
}

#endif // TEST_LOCALE

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
            wxPrintf(wxT("nothing known about the filetype '%s'!\n"),
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

        wxPrintf(wxT("\t%s: %s (%s)\n"),
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

            wxPrintf(wxT("To open %s (%s) do %s.\n"),
                     fname.c_str(), desc.c_str(), cmd.c_str());

            delete ft;
        }
    }

    wxPuts(wxEmptyString);
}

// these tests were broken by wxMimeTypesManager changes, temporarily disabling
#if 0

static void TestMimeOverride()
{
    wxPuts(wxT("*** Testing wxMimeTypesManager additional files loading ***\n"));

    static const wxChar *mailcap = wxT("/tmp/mailcap");
    static const wxChar *mimetypes = wxT("/tmp/mime.types");

    if ( wxFile::Exists(mailcap) )
        wxPrintf(wxT("Loading mailcap from '%s': %s\n"),
                 mailcap,
                 wxTheMimeTypesManager->ReadMailcap(mailcap) ? wxT("ok") : wxT("ERROR"));
    else
        wxPrintf(wxT("WARN: mailcap file '%s' doesn't exist, not loaded.\n"),
                 mailcap);

    if ( wxFile::Exists(mimetypes) )
        wxPrintf(wxT("Loading mime.types from '%s': %s\n"),
                 mimetypes,
                 wxTheMimeTypesManager->ReadMimeTypes(mimetypes) ? wxT("ok") : wxT("ERROR"));
    else
        wxPrintf(wxT("WARN: mime.types file '%s' doesn't exist, not loaded.\n"),
                 mimetypes);

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

#endif // 0

#endif // TEST_MIME

// ----------------------------------------------------------------------------
// module dependencies feature
// ----------------------------------------------------------------------------

#ifdef TEST_MODULE

#include "wx/module.h"

class wxTestModule : public wxModule
{
protected:
    virtual bool OnInit() { wxPrintf(wxT("Load module: %s\n"), GetClassInfo()->GetClassName()); return true; }
    virtual void OnExit() { wxPrintf(wxT("Unload module: %s\n"), GetClassInfo()->GetClassName()); }
};

class wxTestModuleA : public wxTestModule
{
public:
    wxTestModuleA();
private:
    DECLARE_DYNAMIC_CLASS(wxTestModuleA)
};

class wxTestModuleB : public wxTestModule
{
public:
    wxTestModuleB();
private:
    DECLARE_DYNAMIC_CLASS(wxTestModuleB)
};

class wxTestModuleC : public wxTestModule
{
public:
    wxTestModuleC();
private:
    DECLARE_DYNAMIC_CLASS(wxTestModuleC)
};

class wxTestModuleD : public wxTestModule
{
public:
    wxTestModuleD();
private:
    DECLARE_DYNAMIC_CLASS(wxTestModuleD)
};

IMPLEMENT_DYNAMIC_CLASS(wxTestModuleC, wxModule)
wxTestModuleC::wxTestModuleC()
{
    AddDependency(CLASSINFO(wxTestModuleD));
}

IMPLEMENT_DYNAMIC_CLASS(wxTestModuleA, wxModule)
wxTestModuleA::wxTestModuleA()
{
    AddDependency(CLASSINFO(wxTestModuleB));
    AddDependency(CLASSINFO(wxTestModuleD));
}

IMPLEMENT_DYNAMIC_CLASS(wxTestModuleD, wxModule)
wxTestModuleD::wxTestModuleD()
{
}

IMPLEMENT_DYNAMIC_CLASS(wxTestModuleB, wxModule)
wxTestModuleB::wxTestModuleB()
{
    AddDependency(CLASSINFO(wxTestModuleD));
    AddDependency(CLASSINFO(wxTestModuleC));
}

#endif // TEST_MODULE

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
        wxPrintf(wxT("\nEnter a directory name: "));
        if ( !wxFgets(pathname, WXSIZEOF(pathname), stdin) )
            break;

        // kill the last '\n'
        pathname[wxStrlen(pathname) - 1] = 0;

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
// path list
// ----------------------------------------------------------------------------

#ifdef TEST_PATHLIST

#ifdef __UNIX__
    #define CMD_IN_PATH wxT("ls")
#else
    #define CMD_IN_PATH wxT("command.com")
#endif

static void TestPathList()
{
    wxPuts(wxT("*** Testing wxPathList ***\n"));

    wxPathList pathlist;
    pathlist.AddEnvList(wxT("PATH"));
    wxString path = pathlist.FindValidPath(CMD_IN_PATH);
    if ( path.empty() )
    {
        wxPrintf(wxT("ERROR: command not found in the path.\n"));
    }
    else
    {
        wxPrintf(wxT("Command found in the path as '%s'.\n"), path.c_str());
    }
}

#endif // TEST_PATHLIST

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
        wxPrintf(wxT("\nEnter a pattern: "));
        if ( !wxFgets(pattern, WXSIZEOF(pattern), stdin) )
            break;

        // kill the last '\n'
        pattern[wxStrlen(pattern) - 1] = 0;

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
// printf() tests
// ----------------------------------------------------------------------------

/*
   NB: this stuff was taken from the glibc test suite and modified to build
       in wxWidgets: if I read the copyright below properly, this shouldn't
       be a problem
 */

#ifdef TEST_PRINTF

#ifdef wxTEST_PRINTF
    // use our functions from wxchar.cpp
    #undef wxPrintf
    #undef wxSprintf

    // NB: do _not_ use WX_ATTRIBUTE_PRINTF here, we have some invalid formats
    //     in the tests below
    int wxPrintf( const wxChar *format, ... );
    int wxSprintf( wxChar *str, const wxChar *format, ... );
#endif

#include "wx/longlong.h"

#include <float.h>

static void rfg1 (void);
static void rfg2 (void);


static void
fmtchk (const wxChar *fmt)
{
  (void) wxPrintf(wxT("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 0x12);
  (void) wxPrintf(wxT("'\n"));
}

static void
fmtst1chk (const wxChar *fmt)
{
  (void) wxPrintf(wxT("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 4, 0x12);
  (void) wxPrintf(wxT("'\n"));
}

static void
fmtst2chk (const wxChar *fmt)
{
  (void) wxPrintf(wxT("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 4, 4, 0x12);
  (void) wxPrintf(wxT("'\n"));
}

/* This page is covered by the following copyright: */

/* (C) Copyright C E Chew
 *
 * Feel free to copy, use and distribute this software provided:
 *
 *        1. you do not pretend that you wrote it
 *        2. you leave this copyright notice intact.
 */

/*
 * Extracted from exercise.c for glibc-1.05 bug report by Bruce Evans.
 */

#define DEC -123
#define INT 255
#define UNS (~0)

/* Formatted Output Test
 *
 * This exercises the output formatting code.
 */

wxChar *PointerNull = NULL;

static void
fp_test (void)
{
  int i, j, k, l;
  wxChar buf[7];
  wxChar *prefix = buf;
  wxChar tp[20];

  wxPuts(wxT("\nFormatted output test"));
  wxPrintf(wxT("prefix  6d      6o      6x      6X      6u\n"));
  wxStrcpy(prefix, wxT("%"));
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      for (k = 0; k < 2; k++) {
        for (l = 0; l < 2; l++) {
          wxStrcpy(prefix, wxT("%"));
          if (i == 0) wxStrcat(prefix, wxT("-"));
          if (j == 0) wxStrcat(prefix, wxT("+"));
          if (k == 0) wxStrcat(prefix, wxT("#"));
          if (l == 0) wxStrcat(prefix, wxT("0"));
          wxPrintf(wxT("%5s |"), prefix);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, wxT("6d |"));
          wxPrintf(tp, DEC);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, wxT("6o |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, wxT("6x |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, wxT("6X |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, wxT("6u |"));
          wxPrintf(tp, UNS);
          wxPrintf(wxT("\n"));
        }
      }
    }
  }
  wxPrintf(wxT("%10s\n"), PointerNull);
  wxPrintf(wxT("%-10s\n"), PointerNull);
}

static void TestPrintf()
{
  static wxChar shortstr[] = wxT("Hi, Z.");
  static wxChar longstr[] = wxT("Good morning, Doctor Chandra.  This is Hal.  \
I am ready for my first lesson today.");
  int result = 0;
  wxString test_format;

  fmtchk(wxT("%.4x"));
  fmtchk(wxT("%04x"));
  fmtchk(wxT("%4.4x"));
  fmtchk(wxT("%04.4x"));
  fmtchk(wxT("%4.3x"));
  fmtchk(wxT("%04.3x"));

  fmtst1chk(wxT("%.*x"));
  fmtst1chk(wxT("%0*x"));
  fmtst2chk(wxT("%*.*x"));
  fmtst2chk(wxT("%0*.*x"));

  wxString bad_format = wxT("bad format:\t\"%b\"\n");
  wxPrintf(bad_format.c_str());
  wxPrintf(wxT("nil pointer (padded):\t\"%10p\"\n"), (void *) NULL);

  wxPrintf(wxT("decimal negative:\t\"%d\"\n"), -2345);
  wxPrintf(wxT("octal negative:\t\"%o\"\n"), -2345);
  wxPrintf(wxT("hex negative:\t\"%x\"\n"), -2345);
  wxPrintf(wxT("long decimal number:\t\"%ld\"\n"), -123456L);
  wxPrintf(wxT("long octal negative:\t\"%lo\"\n"), -2345L);
  wxPrintf(wxT("long unsigned decimal number:\t\"%lu\"\n"), -123456L);
  wxPrintf(wxT("zero-padded LDN:\t\"%010ld\"\n"), -123456L);
  test_format = wxT("left-adjusted ZLDN:\t\"%-010ld\"\n");
  wxPrintf(test_format.c_str(), -123456);
  wxPrintf(wxT("space-padded LDN:\t\"%10ld\"\n"), -123456L);
  wxPrintf(wxT("left-adjusted SLDN:\t\"%-10ld\"\n"), -123456L);

  test_format = wxT("zero-padded string:\t\"%010s\"\n");
  wxPrintf(test_format.c_str(), shortstr);
  test_format = wxT("left-adjusted Z string:\t\"%-010s\"\n");
  wxPrintf(test_format.c_str(), shortstr);
  wxPrintf(wxT("space-padded string:\t\"%10s\"\n"), shortstr);
  wxPrintf(wxT("left-adjusted S string:\t\"%-10s\"\n"), shortstr);
  wxPrintf(wxT("null string:\t\"%s\"\n"), PointerNull);
  wxPrintf(wxT("limited string:\t\"%.22s\"\n"), longstr);

  wxPrintf(wxT("e-style >= 1:\t\"%e\"\n"), 12.34);
  wxPrintf(wxT("e-style >= .1:\t\"%e\"\n"), 0.1234);
  wxPrintf(wxT("e-style < .1:\t\"%e\"\n"), 0.001234);
  wxPrintf(wxT("e-style big:\t\"%.60e\"\n"), 1e20);
  wxPrintf(wxT("e-style == .1:\t\"%e\"\n"), 0.1);
  wxPrintf(wxT("f-style >= 1:\t\"%f\"\n"), 12.34);
  wxPrintf(wxT("f-style >= .1:\t\"%f\"\n"), 0.1234);
  wxPrintf(wxT("f-style < .1:\t\"%f\"\n"), 0.001234);
  wxPrintf(wxT("g-style >= 1:\t\"%g\"\n"), 12.34);
  wxPrintf(wxT("g-style >= .1:\t\"%g\"\n"), 0.1234);
  wxPrintf(wxT("g-style < .1:\t\"%g\"\n"), 0.001234);
  wxPrintf(wxT("g-style big:\t\"%.60g\"\n"), 1e20);

  wxPrintf (wxT(" %6.5f\n"), .099999999860301614);
  wxPrintf (wxT(" %6.5f\n"), .1);
  wxPrintf (wxT("x%5.4fx\n"), .5);

  wxPrintf (wxT("%#03x\n"), 1);

  //wxPrintf (wxT("something really insane: %.10000f\n"), 1.0);

  {
    double d = FLT_MIN;
    int niter = 17;

    while (niter-- != 0)
      wxPrintf (wxT("%.17e\n"), d / 2);
    fflush (stdout);
  }

#ifndef __WATCOMC__
  // Open Watcom cause compiler error here
  // Error! E173: col(24) floating-point constant too small to represent
  wxPrintf (wxT("%15.5e\n"), 4.9406564584124654e-324);
#endif

#define FORMAT wxT("|%12.4f|%12.4e|%12.4g|\n")
  wxPrintf (FORMAT, 0.0, 0.0, 0.0);
  wxPrintf (FORMAT, 1.0, 1.0, 1.0);
  wxPrintf (FORMAT, -1.0, -1.0, -1.0);
  wxPrintf (FORMAT, 100.0, 100.0, 100.0);
  wxPrintf (FORMAT, 1000.0, 1000.0, 1000.0);
  wxPrintf (FORMAT, 10000.0, 10000.0, 10000.0);
  wxPrintf (FORMAT, 12345.0, 12345.0, 12345.0);
  wxPrintf (FORMAT, 100000.0, 100000.0, 100000.0);
  wxPrintf (FORMAT, 123456.0, 123456.0, 123456.0);
#undef        FORMAT

  {
    wxChar buf[20];
    int rc = wxSnprintf (buf, WXSIZEOF(buf), wxT("%30s"), wxT("foo"));

    wxPrintf(wxT("snprintf (\"%%30s\", \"foo\") == %d, \"%.*s\"\n"),
             rc, WXSIZEOF(buf), buf);
#if 0
    wxChar buf2[512];
    wxPrintf ("snprintf (\"%%.999999u\", 10)\n",
            wxSnprintf(buf2, WXSIZEOFbuf2), "%.999999u", 10));
#endif
  }

  fp_test ();

  wxPrintf (wxT("%e should be 1.234568e+06\n"), 1234567.8);
  wxPrintf (wxT("%f should be 1234567.800000\n"), 1234567.8);
  wxPrintf (wxT("%g should be 1.23457e+06\n"), 1234567.8);
  wxPrintf (wxT("%g should be 123.456\n"), 123.456);
  wxPrintf (wxT("%g should be 1e+06\n"), 1000000.0);
  wxPrintf (wxT("%g should be 10\n"), 10.0);
  wxPrintf (wxT("%g should be 0.02\n"), 0.02);

  {
    double x=1.0;
    wxPrintf(wxT("%.17f\n"),(1.0/x/10.0+1.0)*x-x);
  }

  {
    wxChar buf[200];

    wxSprintf(buf,wxT("%*s%*s%*s"),-1,wxT("one"),-20,wxT("two"),-30,wxT("three"));

    result |= wxStrcmp (buf,
                      wxT("onetwo                 three                         "));

    wxPuts (result != 0 ? wxT("Test failed!") : wxT("Test ok."));
  }

#ifdef wxLongLong_t
  {
      wxChar buf[200];

      wxSprintf(buf, wxT("%07") wxLongLongFmtSpec wxT("o"), wxLL(040000000000));
      #if 0
        // for some reason below line fails under Borland
      wxPrintf (wxT("sprintf (buf, \"%%07Lo\", 040000000000ll) = %s"), buf);
      #endif

      if (wxStrcmp (buf, wxT("40000000000")) != 0)
      {
          result = 1;
          wxPuts (wxT("\tFAILED"));
      }
      wxUnusedVar(result);
      wxPuts (wxEmptyString);
  }
#endif // wxLongLong_t

  wxPrintf (wxT("printf (\"%%hhu\", %u) = %hhu\n"), UCHAR_MAX + 2, UCHAR_MAX + 2);
  wxPrintf (wxT("printf (\"%%hu\", %u) = %hu\n"), USHRT_MAX + 2, USHRT_MAX + 2);

  wxPuts (wxT("--- Should be no further output. ---"));
  rfg1 ();
  rfg2 ();

#if 0
  {
    wxChar bytes[7];
    wxChar buf[20];

    memset (bytes, '\xff', sizeof bytes);
    wxSprintf (buf, wxT("foo%hhn\n"), &bytes[3]);
    if (bytes[0] != '\xff' || bytes[1] != '\xff' || bytes[2] != '\xff'
        || bytes[4] != '\xff' || bytes[5] != '\xff' || bytes[6] != '\xff')
      {
        wxPuts (wxT("%hhn overwrite more bytes"));
        result = 1;
      }
    if (bytes[3] != 3)
      {
        wxPuts (wxT("%hhn wrote incorrect value"));
        result = 1;
      }
  }
#endif
}

static void
rfg1 (void)
{
  wxChar buf[100];

  wxSprintf (buf, wxT("%5.s"), wxT("xyz"));
  if (wxStrcmp (buf, wxT("     ")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("     "));
  wxSprintf (buf, wxT("%5.f"), 33.3);
  if (wxStrcmp (buf, wxT("   33")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("   33"));
  wxSprintf (buf, wxT("%8.e"), 33.3e7);
  if (wxStrcmp (buf, wxT("   3e+08")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("   3e+08"));
  wxSprintf (buf, wxT("%8.E"), 33.3e7);
  if (wxStrcmp (buf, wxT("   3E+08")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("   3E+08"));
  wxSprintf (buf, wxT("%.g"), 33.3);
  if (wxStrcmp (buf, wxT("3e+01")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("3e+01"));
  wxSprintf (buf, wxT("%.G"), 33.3);
  if (wxStrcmp (buf, wxT("3E+01")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("3E+01"));
}

static void
rfg2 (void)
{
  int prec;
  wxChar buf[100];
  wxString test_format;

  prec = 0;
  wxSprintf (buf, wxT("%.*g"), prec, 3.3);
  if (wxStrcmp (buf, wxT("3")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("3"));
  prec = 0;
  wxSprintf (buf, wxT("%.*G"), prec, 3.3);
  if (wxStrcmp (buf, wxT("3")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("3"));
  prec = 0;
  wxSprintf (buf, wxT("%7.*G"), prec, 3.33);
  if (wxStrcmp (buf, wxT("      3")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("      3"));
  prec = 3;
  test_format = wxT("%04.*o");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, wxT(" 041")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT(" 041"));
  prec = 7;
  test_format = wxT("%09.*u");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, wxT("  0000033")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT("  0000033"));
  prec = 3;
  test_format = wxT("%04.*x");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, wxT(" 021")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT(" 021"));
  prec = 3;
  test_format = wxT("%04.*X");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, wxT(" 021")) != 0)
    wxPrintf (wxT("got: '%s', expected: '%s'\n"), buf, wxT(" 021"));
}

#endif // TEST_PRINTF

// ----------------------------------------------------------------------------
// registry and related stuff
// ----------------------------------------------------------------------------

// this is for MSW only
#ifndef __WXMSW__
    #undef TEST_REGCONF
    #undef TEST_REGISTRY
#endif

#ifdef TEST_REGCONF

#include "wx/confbase.h"
#include "wx/msw/regconf.h"

#if 0
static void TestRegConfWrite()
{
    wxConfig *config = new wxConfig(wxT("myapp"));
    config->SetPath(wxT("/group1"));
    config->Write(wxT("entry1"), wxT("foo"));
    config->SetPath(wxT("/group2"));
    config->Write(wxT("entry1"), wxT("bar"));
}
#endif

static void TestRegConfRead()
{
    wxRegConfig *config = new wxRegConfig(wxT("myapp"));

    wxString str;
    long dummy;
    config->SetPath(wxT("/"));
    wxPuts(wxT("Enumerating / subgroups:"));
    bool bCont = config->GetFirstGroup(str, dummy);
    while(bCont)
    {
        wxPuts(str);
        bCont = config->GetNextGroup(str, dummy);
    }
}

#endif // TEST_REGCONF

#ifdef TEST_REGISTRY

#include "wx/msw/registry.h"

// I chose this one because I liked its name, but it probably only exists under
// NT
static const wxChar *TESTKEY =
    wxT("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\CrashControl");

static void TestRegistryRead()
{
    wxPuts(wxT("*** testing registry reading ***"));

    wxRegKey key(TESTKEY);
    wxPrintf(wxT("The test key name is '%s'.\n"), key.GetName().c_str());
    if ( !key.Open() )
    {
        wxPuts(wxT("ERROR: test key can't be opened, aborting test."));

        return;
    }

    size_t nSubKeys, nValues;
    if ( key.GetKeyInfo(&nSubKeys, NULL, &nValues, NULL) )
    {
        wxPrintf(wxT("It has %u subkeys and %u values.\n"), nSubKeys, nValues);
    }

    wxPrintf(wxT("Enumerating values:\n"));

    long dummy;
    wxString value;
    bool cont = key.GetFirstValue(value, dummy);
    while ( cont )
    {
        wxPrintf(wxT("Value '%s': type "), value.c_str());
        switch ( key.GetValueType(value) )
        {
            case wxRegKey::Type_None:   wxPrintf(wxT("ERROR (none)")); break;
            case wxRegKey::Type_String: wxPrintf(wxT("SZ")); break;
            case wxRegKey::Type_Expand_String: wxPrintf(wxT("EXPAND_SZ")); break;
            case wxRegKey::Type_Binary: wxPrintf(wxT("BINARY")); break;
            case wxRegKey::Type_Dword: wxPrintf(wxT("DWORD")); break;
            case wxRegKey::Type_Multi_String: wxPrintf(wxT("MULTI_SZ")); break;
            default: wxPrintf(wxT("other (unknown)")); break;
        }

        wxPrintf(wxT(", value = "));
        if ( key.IsNumericValue(value) )
        {
            long val;
            key.QueryValue(value, &val);
            wxPrintf(wxT("%ld"), val);
        }
        else // string
        {
            wxString val;
            key.QueryValue(value, val);
            wxPrintf(wxT("'%s'"), val.c_str());

            key.QueryRawValue(value, val);
            wxPrintf(wxT(" (raw value '%s')"), val.c_str());
        }

        wxPutchar('\n');

        cont = key.GetNextValue(value, dummy);
    }
}

static void TestRegistryAssociation()
{
    /*
       The second call to deleteself genertaes an error message, with a
       messagebox saying .flo is crucial to system operation, while the .ddf
       call also fails, but with no error message
    */

    wxRegKey key;

    key.SetName(wxT("HKEY_CLASSES_ROOT\\.ddf") );
    key.Create();
    key = wxT("ddxf_auto_file") ;
    key.SetName(wxT("HKEY_CLASSES_ROOT\\.flo") );
    key.Create();
    key = wxT("ddxf_auto_file") ;
    key.SetName(wxT("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon"));
    key.Create();
    key = wxT("program,0") ;
    key.SetName(wxT("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command"));
    key.Create();
    key = wxT("program \"%1\"") ;

    key.SetName(wxT("HKEY_CLASSES_ROOT\\.ddf") );
    key.DeleteSelf();
    key.SetName(wxT("HKEY_CLASSES_ROOT\\.flo") );
    key.DeleteSelf();
    key.SetName(wxT("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon"));
    key.DeleteSelf();
    key.SetName(wxT("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command"));
    key.DeleteSelf();
}

#endif // TEST_REGISTRY

// ----------------------------------------------------------------------------
// scope guard
// ----------------------------------------------------------------------------

#ifdef TEST_SCOPEGUARD

#include "wx/scopeguard.h"

static void function0() { puts("function0()"); }
static void function1(int n) { printf("function1(%d)\n", n); }
static void function2(double x, char c) { printf("function2(%g, %c)\n", x, c); }

struct Object
{
    void method0() { printf("method0()\n"); }
    void method1(int n) { printf("method1(%d)\n", n); }
    void method2(double x, char c) { printf("method2(%g, %c)\n", x, c); }
};

static void TestScopeGuard()
{
    wxON_BLOCK_EXIT0(function0);
    wxON_BLOCK_EXIT1(function1, 17);
    wxON_BLOCK_EXIT2(function2, 3.14, 'p');

    Object obj;
    wxON_BLOCK_EXIT_OBJ0(obj, Object::method0);
    wxON_BLOCK_EXIT_OBJ1(obj, Object::method1, 7);
    wxON_BLOCK_EXIT_OBJ2(obj, Object::method2, 2.71, 'e');

    wxScopeGuard dismissed = wxMakeGuard(function0);
    dismissed.Dismiss();
}

#endif

// ----------------------------------------------------------------------------
// sockets
// ----------------------------------------------------------------------------

#ifdef TEST_SOCKETS

#include "wx/socket.h"
#include "wx/protocol/protocol.h"
#include "wx/protocol/http.h"

static void TestSocketServer()
{
    wxPuts(wxT("*** Testing wxSocketServer ***\n"));

    static const int PORT = 3000;

    wxIPV4address addr;
    addr.Service(PORT);

    wxSocketServer *server = new wxSocketServer(addr);
    if ( !server->Ok() )
    {
        wxPuts(wxT("ERROR: failed to bind"));

        return;
    }

    bool quit = false;
    while ( !quit )
    {
        wxPrintf(wxT("Server: waiting for connection on port %d...\n"), PORT);

        wxSocketBase *socket = server->Accept();
        if ( !socket )
        {
            wxPuts(wxT("ERROR: wxSocketServer::Accept() failed."));
            break;
        }

        wxPuts(wxT("Server: got a client."));

        server->SetTimeout(60); // 1 min

        bool close = false;
        while ( !close && socket->IsConnected() )
        {
            wxString s;
            wxChar ch = wxT('\0');
            for ( ;; )
            {
                if ( socket->Read(&ch, sizeof(ch)).Error() )
                {
                    // don't log error if the client just close the connection
                    if ( socket->IsConnected() )
                    {
                        wxPuts(wxT("ERROR: in wxSocket::Read."));
                    }

                    break;
                }

                if ( ch == '\r' )
                    continue;

                if ( ch == '\n' )
                    break;

                s += ch;
            }

            if ( ch != '\n' )
            {
                break;
            }

            wxPrintf(wxT("Server: got '%s'.\n"), s.c_str());
            if ( s == wxT("close") )
            {
                wxPuts(wxT("Closing connection"));

                close = true;
            }
            else if ( s == wxT("quit") )
            {
                close =
                quit = true;

                wxPuts(wxT("Shutting down the server"));
            }
            else // not a special command
            {
                socket->Write(s.MakeUpper().c_str(), s.length());
                socket->Write("\r\n", 2);
                wxPrintf(wxT("Server: wrote '%s'.\n"), s.c_str());
            }
        }

        if ( !close )
        {
            wxPuts(wxT("Server: lost a client unexpectedly."));
        }

        socket->Destroy();
    }

    // same as "delete server" but is consistent with GUI programs
    server->Destroy();
}

static void TestSocketClient()
{
    wxPuts(wxT("*** Testing wxSocketClient ***\n"));

    static const wxChar *hostname = wxT("www.wxwidgets.org");

    wxIPV4address addr;
    addr.Hostname(hostname);
    addr.Service(80);

    wxPrintf(wxT("--- Attempting to connect to %s:80...\n"), hostname);

    wxSocketClient client;
    if ( !client.Connect(addr) )
    {
        wxPrintf(wxT("ERROR: failed to connect to %s\n"), hostname);
    }
    else
    {
        wxPrintf(wxT("--- Connected to %s:%u...\n"),
               addr.Hostname().c_str(), addr.Service());

        wxChar buf[8192];

        // could use simply "GET" here I suppose
        wxString cmdGet =
            wxString::Format(wxT("GET http://%s/\r\n"), hostname);
        client.Write(cmdGet, cmdGet.length());
        wxPrintf(wxT("--- Sent command '%s' to the server\n"),
               MakePrintable(cmdGet).c_str());
        client.Read(buf, WXSIZEOF(buf));
        wxPrintf(wxT("--- Server replied:\n%s"), buf);
    }
}

#endif // TEST_SOCKETS

// ----------------------------------------------------------------------------
// FTP
// ----------------------------------------------------------------------------

#ifdef TEST_FTP

#include "wx/protocol/ftp.h"
#include "wx/protocol/log.h"

#define FTP_ANONYMOUS

static wxFTP *ftp;

#ifdef FTP_ANONYMOUS
    static const wxChar *directory = wxT("/pub");
    static const wxChar *filename = wxT("welcome.msg");
#else
    static const wxChar *directory = wxT("/etc");
    static const wxChar *filename = wxT("issue");
#endif

static bool TestFtpConnect()
{
    wxPuts(wxT("*** Testing FTP connect ***"));

#ifdef FTP_ANONYMOUS
    static const wxChar *hostname = wxT("ftp.wxwidgets.org");

    wxPrintf(wxT("--- Attempting to connect to %s:21 anonymously...\n"), hostname);
#else // !FTP_ANONYMOUS
    static const wxChar *hostname = "localhost";

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

static void TestFtpList()
{
    wxPuts(wxT("*** Testing wxFTP file listing ***\n"));

    // test CWD
    if ( !ftp->ChDir(directory) )
    {
        wxPrintf(wxT("ERROR: failed to cd to %s\n"), directory);
    }

    wxPrintf(wxT("Current directory is '%s'\n"), ftp->Pwd().c_str());

    // test NLIST and LIST
    wxArrayString files;
    if ( !ftp->GetFilesList(files) )
    {
        wxPuts(wxT("ERROR: failed to get NLIST of files"));
    }
    else
    {
        wxPrintf(wxT("Brief list of files under '%s':\n"), ftp->Pwd().c_str());
        size_t count = files.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxPrintf(wxT("\t%s\n"), files[n].c_str());
        }
        wxPuts(wxT("End of the file list"));
    }

    if ( !ftp->GetDirList(files) )
    {
        wxPuts(wxT("ERROR: failed to get LIST of files"));
    }
    else
    {
        wxPrintf(wxT("Detailed list of files under '%s':\n"), ftp->Pwd().c_str());
        size_t count = files.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxPrintf(wxT("\t%s\n"), files[n].c_str());
        }
        wxPuts(wxT("End of the file list"));
    }

    if ( !ftp->ChDir(wxT("..")) )
    {
        wxPuts(wxT("ERROR: failed to cd to .."));
    }

    wxPrintf(wxT("Current directory is '%s'\n"), ftp->Pwd().c_str());
}

static void TestFtpDownload()
{
    wxPuts(wxT("*** Testing wxFTP download ***\n"));

    // test RETR
    wxInputStream *in = ftp->GetInputStream(filename);
    if ( !in )
    {
        wxPrintf(wxT("ERROR: couldn't get input stream for %s\n"), filename);
    }
    else
    {
        size_t size = in->GetSize();
        wxPrintf(wxT("Reading file %s (%u bytes)..."), filename, size);
        fflush(stdout);

        wxChar *data = new wxChar[size];
        if ( !in->Read(data, size) )
        {
            wxPuts(wxT("ERROR: read error"));
        }
        else
        {
            wxPrintf(wxT("\nContents of %s:\n%s\n"), filename, data);
        }

        delete [] data;
        delete in;
    }
}

static void TestFtpFileSize()
{
    wxPuts(wxT("*** Testing FTP SIZE command ***"));

    if ( !ftp->ChDir(directory) )
    {
        wxPrintf(wxT("ERROR: failed to cd to %s\n"), directory);
    }

    wxPrintf(wxT("Current directory is '%s'\n"), ftp->Pwd().c_str());

    if ( ftp->FileExists(filename) )
    {
        int size = ftp->GetFileSize(filename);
        if ( size == -1 )
            wxPrintf(wxT("ERROR: couldn't get size of '%s'\n"), filename);
        else
            wxPrintf(wxT("Size of '%s' is %d bytes.\n"), filename, size);
    }
    else
    {
        wxPrintf(wxT("ERROR: '%s' doesn't exist\n"), filename);
    }
}

static void TestFtpMisc()
{
    wxPuts(wxT("*** Testing miscellaneous wxFTP functions ***"));

    if ( ftp->SendCommand(wxT("STAT")) != '2' )
    {
        wxPuts(wxT("ERROR: STAT failed"));
    }
    else
    {
        wxPrintf(wxT("STAT returned:\n\n%s\n"), ftp->GetLastResult().c_str());
    }

    if ( ftp->SendCommand(wxT("HELP SITE")) != '2' )
    {
        wxPuts(wxT("ERROR: HELP SITE failed"));
    }
    else
    {
        wxPrintf(wxT("The list of site-specific commands:\n\n%s\n"),
               ftp->GetLastResult().c_str());
    }
}

#if TEST_INTERACTIVE

static void TestFtpInteractive()
{
    wxPuts(wxT("\n*** Interactive wxFTP test ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(wxT("Enter FTP command: "));
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

    wxPuts(wxT("\n*** done ***"));
}

#endif // TEST_INTERACTIVE

static void TestFtpUpload()
{
    wxPuts(wxT("*** Testing wxFTP uploading ***\n"));

    // upload a file
    static const wxChar *file1 = wxT("test1");
    static const wxChar *file2 = wxT("test2");
    wxOutputStream *out = ftp->GetOutputStream(file1);
    if ( out )
    {
        wxPrintf(wxT("--- Uploading to %s ---\n"), file1);
        out->Write("First hello", 11);
        delete out;
    }

    // send a command to check the remote file
    if ( ftp->SendCommand(wxString(wxT("STAT ")) + file1) != '2' )
    {
        wxPrintf(wxT("ERROR: STAT %s failed\n"), file1);
    }
    else
    {
        wxPrintf(wxT("STAT %s returned:\n\n%s\n"),
               file1, ftp->GetLastResult().c_str());
    }

    out = ftp->GetOutputStream(file2);
    if ( out )
    {
        wxPrintf(wxT("--- Uploading to %s ---\n"), file1);
        out->Write("Second hello", 12);
        delete out;
    }
}

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
    wxPuts(wxT("*** Testing wxStackWalker ***\n"));

    StackDump dump(argv0);
    dump.Walk();
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
    wxPuts(wxT("*** Testing wxStandardPaths ***\n"));

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
}

#endif // TEST_STDPATHS

// ----------------------------------------------------------------------------
// streams
// ----------------------------------------------------------------------------

#ifdef TEST_STREAMS

#include "wx/wfstream.h"
#include "wx/mstream.h"

static void TestFileStream()
{
    wxPuts(wxT("*** Testing wxFileInputStream ***"));

    static const wxString filename = wxT("testdata.fs");
    {
        wxFileOutputStream fsOut(filename);
        fsOut.Write("foo", 3);
    }

    {
        wxFileInputStream fsIn(filename);
        wxPrintf(wxT("File stream size: %u\n"), fsIn.GetSize());
        int c;
        while ( (c=fsIn.GetC()) != wxEOF  )
        {
            wxPutchar(c);
        }
    }

    if ( !wxRemoveFile(filename) )
    {
        wxPrintf(wxT("ERROR: failed to remove the file '%s'.\n"), filename.c_str());
    }

    wxPuts(wxT("\n*** wxFileInputStream test done ***"));
}

static void TestMemoryStream()
{
    wxPuts(wxT("*** Testing wxMemoryOutputStream ***"));

    wxMemoryOutputStream memOutStream;
    wxPrintf(wxT("Initially out stream offset: %lu\n"),
             (unsigned long)memOutStream.TellO());

    for ( const wxChar *p = wxT("Hello, stream!"); *p; p++ )
    {
        memOutStream.PutC(*p);
    }

    wxPrintf(wxT("Final out stream offset: %lu\n"),
             (unsigned long)memOutStream.TellO());

    wxPuts(wxT("*** Testing wxMemoryInputStream ***"));

    wxChar buf[1024];
    size_t len = memOutStream.CopyTo(buf, WXSIZEOF(buf));

    wxMemoryInputStream memInpStream(buf, len);
    wxPrintf(wxT("Memory stream size: %u\n"), memInpStream.GetSize());
    int c;
    while ( (c=memInpStream.GetC()) != wxEOF )
    {
        wxPutchar(c);
    }

    wxPuts(wxT("\n*** wxMemoryInputStream test done ***"));
}

#endif // TEST_STREAMS

// ----------------------------------------------------------------------------
// timers
// ----------------------------------------------------------------------------

#ifdef TEST_TIMER

#include "wx/stopwatch.h"
#include "wx/utils.h"

static void TestStopWatch()
{
    wxPuts(wxT("*** Testing wxStopWatch ***\n"));

    wxStopWatch sw;
    sw.Pause();
    wxPrintf(wxT("Initially paused, after 2 seconds time is..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(wxT("\t%ldms\n"), sw.Time());

    wxPrintf(wxT("Resuming stopwatch and sleeping 3 seconds..."));
    fflush(stdout);
    sw.Resume();
    wxSleep(3);
    wxPrintf(wxT("\telapsed time: %ldms\n"), sw.Time());

    sw.Pause();
    wxPrintf(wxT("Pausing agan and sleeping 2 more seconds..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(wxT("\telapsed time: %ldms\n"), sw.Time());

    sw.Resume();
    wxPrintf(wxT("Finally resuming and sleeping 2 more seconds..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(wxT("\telapsed time: %ldms\n"), sw.Time());

    wxStopWatch sw2;
    wxPuts(wxT("\nChecking for 'backwards clock' bug..."));
    for ( size_t n = 0; n < 70; n++ )
    {
        sw2.Start();

        for ( size_t m = 0; m < 100000; m++ )
        {
            if ( sw.Time() < 0 || sw2.Time() < 0 )
            {
                wxPuts(wxT("\ntime is negative - ERROR!"));
            }
        }

        wxPutchar('.');
        fflush(stdout);
    }

    wxPuts(wxT(", ok."));
}

#include "wx/timer.h"
#include "wx/evtloop.h"

void TestTimer()
{
    wxPuts(wxT("*** Testing wxTimer ***\n"));

    class MyTimer : public wxTimer
    {
    public:
        MyTimer() : wxTimer() { m_num = 0; }

        virtual void Notify()
        {
            wxPrintf(wxT("%d"), m_num++);
            fflush(stdout);

            if ( m_num == 10 )
            {
                wxPrintf(wxT("... exiting the event loop"));
                Stop();

                wxEventLoop::GetActive()->Exit(0);
                wxPuts(wxT(", ok."));
            }

            fflush(stdout);
        }

    private:
        int m_num;
    };

    wxEventLoop loop;

    wxTimer timer1;
    timer1.Start(100, true /* one shot */);
    timer1.Stop();
    timer1.Start(100, true /* one shot */);

    MyTimer timer;
    timer.Start(500);

    loop.Run();
}

#endif // TEST_TIMER

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
}

#endif // TEST_VOLUME

// ----------------------------------------------------------------------------
// wide char and Unicode support
// ----------------------------------------------------------------------------

#ifdef TEST_WCHAR

#include "wx/strconv.h"
#include "wx/fontenc.h"
#include "wx/encconv.h"
#include "wx/buffer.h"

static const unsigned char utf8koi8r[] =
{
    208, 157, 208, 181, 209, 129, 208, 186, 208, 176, 208, 183, 208, 176,
    208, 189, 208, 189, 208, 190, 32, 208, 191, 208, 190, 209, 128, 208,
    176, 208, 180, 208, 190, 208, 178, 208, 176, 208, 187, 32, 208, 188,
    208, 181, 208, 189, 209, 143, 32, 209, 129, 208, 178, 208, 190, 208,
    181, 208, 185, 32, 208, 186, 209, 128, 209, 131, 209, 130, 208, 181,
    208, 185, 209, 136, 208, 181, 208, 185, 32, 208, 189, 208, 190, 208,
    178, 208, 190, 209, 129, 209, 130, 209, 140, 209, 142, 0
};

static const unsigned char utf8iso8859_1[] =
{
    0x53, 0x79, 0x73, 0x74, 0xc3, 0xa8, 0x6d, 0x65, 0x73, 0x20, 0x49, 0x6e,
    0x74, 0xc3, 0xa9, 0x67, 0x72, 0x61, 0x62, 0x6c, 0x65, 0x73, 0x20, 0x65,
    0x6e, 0x20, 0x4d, 0xc3, 0xa9, 0x63, 0x61, 0x6e, 0x69, 0x71, 0x75, 0x65,
    0x20, 0x43, 0x6c, 0x61, 0x73, 0x73, 0x69, 0x71, 0x75, 0x65, 0x20, 0x65,
    0x74, 0x20, 0x51, 0x75, 0x61, 0x6e, 0x74, 0x69, 0x71, 0x75, 0x65, 0
};

static const unsigned char utf8Invalid[] =
{
    0x3c, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x3e, 0x32, 0x30, 0x30,
    0x32, 0xe5, 0xb9, 0xb4, 0x30, 0x39, 0xe6, 0x9c, 0x88, 0x32, 0x35, 0xe6,
    0x97, 0xa5, 0x20, 0x30, 0x37, 0xe6, 0x99, 0x82, 0x33, 0x39, 0xe5, 0x88,
    0x86, 0x35, 0x37, 0xe7, 0xa7, 0x92, 0x3c, 0x2f, 0x64, 0x69, 0x73, 0x70,
    0x6c, 0x61, 0x79, 0
};

static const struct Utf8Data
{
    const unsigned char *text;
    size_t len;
    const wxChar *charset;
    wxFontEncoding encoding;
} utf8data[] =
{
    { utf8Invalid, WXSIZEOF(utf8Invalid), wxT("iso8859-1"), wxFONTENCODING_ISO8859_1 },
    { utf8koi8r, WXSIZEOF(utf8koi8r), wxT("koi8-r"), wxFONTENCODING_KOI8 },
    { utf8iso8859_1, WXSIZEOF(utf8iso8859_1), wxT("iso8859-1"), wxFONTENCODING_ISO8859_1 },
};

static void TestUtf8()
{
    wxPuts(wxT("*** Testing UTF8 support ***\n"));

    char buf[1024];
    wchar_t wbuf[1024];

    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const Utf8Data& u8d = utf8data[n];
        if ( wxConvUTF8.MB2WC(wbuf, (const char *)u8d.text,
                              WXSIZEOF(wbuf)) == (size_t)-1 )
        {
            wxPuts(wxT("ERROR: UTF-8 decoding failed."));
        }
        else
        {
            wxCSConv conv(u8d.charset);
            if ( conv.WC2MB(buf, wbuf, WXSIZEOF(buf)) == (size_t)-1 )
            {
                wxPrintf(wxT("ERROR: conversion to %s failed.\n"), u8d.charset);
            }
            else
            {
                wxPrintf(wxT("String in %s: %s\n"), u8d.charset, buf);
            }
        }

        wxString s(wxConvUTF8.cMB2WC((const char *)u8d.text));
        if ( s.empty() )
            s = wxT("<< conversion failed >>");
        wxPrintf(wxT("String in current cset: %s\n"), s.c_str());

    }

    wxPuts(wxEmptyString);
}

static void TestEncodingConverter()
{
    wxPuts(wxT("*** Testing wxEncodingConverter ***\n"));

    // using wxEncodingConverter should give the same result as above
    char buf[1024];
    wchar_t wbuf[1024];
    if ( wxConvUTF8.MB2WC(wbuf, (const char *)utf8koi8r,
                          WXSIZEOF(utf8koi8r)) == (size_t)-1 )
    {
        wxPuts(wxT("ERROR: UTF-8 decoding failed."));
    }
    else
    {
        wxEncodingConverter ec;
        ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_KOI8);
        ec.Convert(wbuf, buf);
        wxPrintf(wxT("The same KOI8-R string using wxEC: %s\n"), buf);
    }

    wxPuts(wxEmptyString);
}

#endif // TEST_WCHAR

// ----------------------------------------------------------------------------
// ZIP stream
// ----------------------------------------------------------------------------

#ifdef TEST_ZIP

#include "wx/filesys.h"
#include "wx/fs_zip.h"
#include "wx/zipstrm.h"

static const wxChar *TESTFILE_ZIP = wxT("testdata.zip");

static void TestZipStreamRead()
{
    wxPuts(wxT("*** Testing ZIP reading ***\n"));

    static const wxString filename = wxT("foo");
    wxFFileInputStream in(TESTFILE_ZIP);
    wxZipInputStream istr(in);
    wxZipEntry entry(filename);
    istr.OpenEntry(entry);

    wxPrintf(wxT("Archive size: %u\n"), istr.GetSize());

    wxPrintf(wxT("Dumping the file '%s':\n"), filename.c_str());
    int c;
    while ( (c=istr.GetC()) != wxEOF )
    {
        wxPutchar(c);
        fflush(stdout);
    }

    wxPuts(wxT("\n----- done ------"));
}

static void DumpZipDirectory(wxFileSystem& fs,
                             const wxString& dir,
                             const wxString& indent)
{
    wxString prefix = wxString::Format(wxT("%s#zip:%s"),
                                         TESTFILE_ZIP, dir.c_str());
    wxString wildcard = prefix + wxT("/*");

    wxString dirname = fs.FindFirst(wildcard, wxDIR);
    while ( !dirname.empty() )
    {
        if ( !dirname.StartsWith(prefix + wxT('/'), &dirname) )
        {
            wxPrintf(wxT("ERROR: unexpected wxFileSystem::FindNext result\n"));

            break;
        }

        wxPrintf(wxT("%s%s\n"), indent.c_str(), dirname.c_str());

        DumpZipDirectory(fs, dirname,
                         indent + wxString(wxT(' '), 4));

        dirname = fs.FindNext();
    }

    wxString filename = fs.FindFirst(wildcard, wxFILE);
    while ( !filename.empty() )
    {
        if ( !filename.StartsWith(prefix, &filename) )
        {
            wxPrintf(wxT("ERROR: unexpected wxFileSystem::FindNext result\n"));

            break;
        }

        wxPrintf(wxT("%s%s\n"), indent.c_str(), filename.c_str());

        filename = fs.FindNext();
    }
}

static void TestZipFileSystem()
{
    wxPuts(wxT("*** Testing ZIP file system ***\n"));

    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem fs;
    wxPrintf(wxT("Dumping all files in the archive %s:\n"), TESTFILE_ZIP);

    DumpZipDirectory(fs, wxT(""), wxString(wxT(' '), 4));
}

#endif // TEST_ZIP

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_DATETIME

#include "wx/math.h"
#include "wx/datetime.h"

// this test miscellaneous static wxDateTime functions

#if TEST_ALL

static void TestTimeStatic()
{
    wxPuts(wxT("\n*** wxDateTime static methods test ***"));

    // some info about the current date
    int year = wxDateTime::GetCurrentYear();
    wxPrintf(wxT("Current year %d is %sa leap one and has %d days.\n"),
           year,
           wxDateTime::IsLeapYear(year) ? "" : "not ",
           wxDateTime::GetNumberOfDays(year));

    wxDateTime::Month month = wxDateTime::GetCurrentMonth();
    wxPrintf(wxT("Current month is '%s' ('%s') and it has %d days\n"),
           wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
           wxDateTime::GetMonthName(month).c_str(),
           wxDateTime::GetNumberOfDays(month));
}

// test time zones stuff
static void TestTimeZones()
{
    wxPuts(wxT("\n*** wxDateTime timezone test ***"));

    wxDateTime now = wxDateTime::Now();

    wxPrintf(wxT("Current GMT time:\t%s\n"), now.Format(wxT("%c"), wxDateTime::GMT0).c_str());
    wxPrintf(wxT("Unix epoch (GMT):\t%s\n"), wxDateTime((time_t)0).Format(wxT("%c"), wxDateTime::GMT0).c_str());
    wxPrintf(wxT("Unix epoch (EST):\t%s\n"), wxDateTime((time_t)0).Format(wxT("%c"), wxDateTime::EST).c_str());
    wxPrintf(wxT("Current time in Paris:\t%s\n"), now.Format(wxT("%c"), wxDateTime::CET).c_str());
    wxPrintf(wxT("               Moscow:\t%s\n"), now.Format(wxT("%c"), wxDateTime::MSK).c_str());
    wxPrintf(wxT("             New York:\t%s\n"), now.Format(wxT("%c"), wxDateTime::EST).c_str());

    wxPrintf(wxT("%s\n"), wxDateTime::Now().Format(wxT("Our timezone is %Z")).c_str());

    wxDateTime::Tm tm = now.GetTm();
    if ( wxDateTime(tm) != now )
    {
        wxPrintf(wxT("ERROR: got %s instead of %s\n"),
                 wxDateTime(tm).Format().c_str(), now.Format().c_str());
    }
}

// test some minimal support for the dates outside the standard range
static void TestTimeRange()
{
    wxPuts(wxT("\n*** wxDateTime out-of-standard-range dates test ***"));

    static const wxChar *fmt = wxT("%d-%b-%Y %H:%M:%S");

    wxPrintf(wxT("Unix epoch:\t%s\n"),
             wxDateTime(2440587.5).Format(fmt).c_str());
    wxPrintf(wxT("Feb 29, 0: \t%s\n"),
             wxDateTime(29, wxDateTime::Feb, 0).Format(fmt).c_str());
    wxPrintf(wxT("JDN 0:     \t%s\n"),
             wxDateTime(0.0).Format(fmt).c_str());
    wxPrintf(wxT("Jan 1, 1AD:\t%s\n"),
             wxDateTime(1, wxDateTime::Jan, 1).Format(fmt).c_str());
    wxPrintf(wxT("May 29, 2099:\t%s\n"),
             wxDateTime(29, wxDateTime::May, 2099).Format(fmt).c_str());
}

// test DST calculations
static void TestTimeDST()
{
    wxPuts(wxT("\n*** wxDateTime DST test ***"));

    wxPrintf(wxT("DST is%s in effect now.\n\n"),
             wxDateTime::Now().IsDST() ? wxEmptyString : wxT(" not"));

    for ( int year = 1990; year < 2005; year++ )
    {
        wxPrintf(wxT("DST period in Europe for year %d: from %s to %s\n"),
                 year,
                 wxDateTime::GetBeginDST(year, wxDateTime::Country_EEC).Format().c_str(),
                 wxDateTime::GetEndDST(year, wxDateTime::Country_EEC).Format().c_str());
    }
}

#endif // TEST_ALL

#if TEST_INTERACTIVE

static void TestDateTimeInteractive()
{
    wxPuts(wxT("\n*** interactive wxDateTime tests ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(wxT("Enter a date: "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

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

    wxPuts(wxT("\n*** done ***"));
}

#endif // TEST_INTERACTIVE

#if TEST_ALL

static void TestTimeMS()
{
    wxPuts(wxT("*** testing millisecond-resolution support in wxDateTime ***"));

    wxDateTime dt1 = wxDateTime::Now(),
               dt2 = wxDateTime::UNow();

    wxPrintf(wxT("Now = %s\n"), dt1.Format(wxT("%H:%M:%S:%l")).c_str());
    wxPrintf(wxT("UNow = %s\n"), dt2.Format(wxT("%H:%M:%S:%l")).c_str());
    wxPrintf(wxT("Dummy loop: "));
    for ( int i = 0; i < 6000; i++ )
    {
        //for ( int j = 0; j < 10; j++ )
        {
            wxString s;
            s.Printf(wxT("%g"), sqrt((float)i));
        }

        if ( !(i % 100) )
            wxPutchar('.');
    }
    wxPuts(wxT(", done"));

    dt1 = dt2;
    dt2 = wxDateTime::UNow();
    wxPrintf(wxT("UNow = %s\n"), dt2.Format(wxT("%H:%M:%S:%l")).c_str());

    wxPrintf(wxT("Loop executed in %s ms\n"), (dt2 - dt1).Format(wxT("%l")).c_str());

    wxPuts(wxT("\n*** done ***"));
}

static void TestTimeHolidays()
{
    wxPuts(wxT("\n*** testing wxDateTimeHolidayAuthority ***\n"));

    wxDateTime::Tm tm = wxDateTime(29, wxDateTime::May, 2000).GetTm();
    wxDateTime dtStart(1, tm.mon, tm.year),
               dtEnd = dtStart.GetLastMonthDay();

    wxDateTimeArray hol;
    wxDateTimeHolidayAuthority::GetHolidaysInRange(dtStart, dtEnd, hol);

    const wxChar *format = wxT("%d-%b-%Y (%a)");

    wxPrintf(wxT("All holidays between %s and %s:\n"),
           dtStart.Format(format).c_str(), dtEnd.Format(format).c_str());

    size_t count = hol.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxPrintf(wxT("\t%s\n"), hol[n].Format(format).c_str());
    }

    wxPuts(wxEmptyString);
}

static void TestTimeZoneBug()
{
    wxPuts(wxT("\n*** testing for DST/timezone bug ***\n"));

    wxDateTime date = wxDateTime(1, wxDateTime::Mar, 2000);
    for ( int i = 0; i < 31; i++ )
    {
        wxPrintf(wxT("Date %s: week day %s.\n"),
               date.Format(wxT("%d-%m-%Y")).c_str(),
               date.GetWeekDayName(date.GetWeekDay()).c_str());

        date += wxDateSpan::Day();
    }

    wxPuts(wxEmptyString);
}

static void TestTimeSpanFormat()
{
    wxPuts(wxT("\n*** wxTimeSpan tests ***"));

    static const wxChar *formats[] =
    {
        wxT("(default) %H:%M:%S"),
        wxT("%E weeks and %D days"),
        wxT("%l milliseconds"),
        wxT("(with ms) %H:%M:%S:%l"),
        wxT("100%% of minutes is %M"),       // test "%%"
        wxT("%D days and %H hours"),
        wxT("or also %S seconds"),
    };

    wxTimeSpan ts1(1, 2, 3, 4),
                ts2(111, 222, 333);
    for ( size_t n = 0; n < WXSIZEOF(formats); n++ )
    {
        wxPrintf(wxT("ts1 = %s\tts2 = %s\n"),
               ts1.Format(formats[n]).c_str(),
               ts2.Format(formats[n]).c_str());
    }

    wxPuts(wxEmptyString);
}

#endif // TEST_ALL

#endif // TEST_DATETIME

// ----------------------------------------------------------------------------
// wxTextInput/OutputStream
// ----------------------------------------------------------------------------

#ifdef TEST_TEXTSTREAM

#include "wx/txtstrm.h"
#include "wx/wfstream.h"

static void TestTextInputStream()
{
    wxPuts(wxT("\n*** wxTextInputStream test ***"));

    wxString filename = wxT("testdata.fc");
    wxFileInputStream fsIn(filename);
    if ( !fsIn.Ok() )
    {
        wxPuts(wxT("ERROR: couldn't open file."));
    }
    else
    {
        wxTextInputStream tis(fsIn);

        size_t line = 1;
        for ( ;; )
        {
            const wxString s = tis.ReadLine();

            // line could be non empty if the last line of the file isn't
            // terminated with EOL
            if ( fsIn.Eof() && s.empty() )
                break;

            wxPrintf(wxT("Line %d: %s\n"), line++, s.c_str());
        }
    }
}

#endif // TEST_TEXTSTREAM

// ----------------------------------------------------------------------------
// threads
// ----------------------------------------------------------------------------

#ifdef TEST_THREADS

#include "wx/thread.h"

static size_t gs_counter = (size_t)-1;
static wxCriticalSection gs_critsect;
static wxSemaphore gs_cond;

class MyJoinableThread : public wxThread
{
public:
    MyJoinableThread(size_t n) : wxThread(wxTHREAD_JOINABLE)
        { m_n = n; Create(); }

    // thread execution starts here
    virtual ExitCode Entry();

private:
    size_t m_n;
};

wxThread::ExitCode MyJoinableThread::Entry()
{
    unsigned long res = 1;
    for ( size_t n = 1; n < m_n; n++ )
    {
        res *= n;

        // it's a loooong calculation :-)
        Sleep(100);
    }

    return (ExitCode)res;
}

class MyDetachedThread : public wxThread
{
public:
    MyDetachedThread(size_t n, wxChar ch)
    {
        m_n = n;
        m_ch = ch;
        m_cancelled = false;

        Create();
    }

    // thread execution starts here
    virtual ExitCode Entry();

    // and stops here
    virtual void OnExit();

private:
    size_t m_n; // number of characters to write
    wxChar m_ch;  // character to write

    bool m_cancelled;   // false if we exit normally
};

wxThread::ExitCode MyDetachedThread::Entry()
{
    {
        wxCriticalSectionLocker lock(gs_critsect);
        if ( gs_counter == (size_t)-1 )
            gs_counter = 1;
        else
            gs_counter++;
    }

    for ( size_t n = 0; n < m_n; n++ )
    {
        if ( TestDestroy() )
        {
            m_cancelled = true;

            break;
        }

        wxPutchar(m_ch);
        fflush(stdout);

        wxThread::Sleep(100);
    }

    return 0;
}

void MyDetachedThread::OnExit()
{
    wxLogTrace(wxT("thread"), wxT("Thread %ld is in OnExit"), GetId());

    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter && !m_cancelled )
        gs_cond.Post();
}

static void TestDetachedThreads()
{
    wxPuts(wxT("\n*** Testing detached threads ***"));

    static const size_t nThreads = 3;
    MyDetachedThread *threads[nThreads];
    size_t n;
    for ( n = 0; n < nThreads; n++ )
    {
        threads[n] = new MyDetachedThread(10, 'A' + n);
    }

    threads[0]->SetPriority(WXTHREAD_MIN_PRIORITY);
    threads[1]->SetPriority(WXTHREAD_MAX_PRIORITY);

    for ( n = 0; n < nThreads; n++ )
    {
        threads[n]->Run();
    }

    // wait until all threads terminate
    gs_cond.Wait();

    wxPuts(wxEmptyString);
}

static void TestJoinableThreads()
{
    wxPuts(wxT("\n*** Testing a joinable thread (a loooong calculation...) ***"));

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    wxPrintf(wxT("\nThread terminated with exit code %lu.\n"),
             (unsigned long)thread.Wait());
}

static void TestThreadSuspend()
{
    wxPuts(wxT("\n*** Testing thread suspend/resume functions ***"));

    MyDetachedThread *thread = new MyDetachedThread(15, 'X');

    thread->Run();

    // this is for this demo only, in a real life program we'd use another
    // condition variable which would be signaled from wxThread::Entry() to
    // tell us that the thread really started running - but here just wait a
    // bit and hope that it will be enough (the problem is, of course, that
    // the thread might still not run when we call Pause() which will result
    // in an error)
    wxThread::Sleep(300);

    for ( size_t n = 0; n < 3; n++ )
    {
        thread->Pause();

        wxPuts(wxT("\nThread suspended"));
        if ( n > 0 )
        {
            // don't sleep but resume immediately the first time
            wxThread::Sleep(300);
        }
        wxPuts(wxT("Going to resume the thread"));

        thread->Resume();
    }

    wxPuts(wxT("Waiting until it terminates now"));

    // wait until the thread terminates
    gs_cond.Wait();

    wxPuts(wxEmptyString);
}

static void TestThreadDelete()
{
    // As above, using Sleep() is only for testing here - we must use some
    // synchronisation object instead to ensure that the thread is still
    // running when we delete it - deleting a detached thread which already
    // terminated will lead to a crash!

    wxPuts(wxT("\n*** Testing thread delete function ***"));

    MyDetachedThread *thread0 = new MyDetachedThread(30, 'W');

    thread0->Delete();

    wxPuts(wxT("\nDeleted a thread which didn't start to run yet."));

    MyDetachedThread *thread1 = new MyDetachedThread(30, 'Y');

    thread1->Run();

    wxThread::Sleep(300);

    thread1->Delete();

    wxPuts(wxT("\nDeleted a running thread."));

    MyDetachedThread *thread2 = new MyDetachedThread(30, 'Z');

    thread2->Run();

    wxThread::Sleep(300);

    thread2->Pause();

    thread2->Delete();

    wxPuts(wxT("\nDeleted a sleeping thread."));

    MyJoinableThread thread3(20);
    thread3.Run();

    thread3.Delete();

    wxPuts(wxT("\nDeleted a joinable thread."));

    MyJoinableThread thread4(2);
    thread4.Run();

    wxThread::Sleep(300);

    thread4.Delete();

    wxPuts(wxT("\nDeleted a joinable thread which already terminated."));

    wxPuts(wxEmptyString);
}

class MyWaitingThread : public wxThread
{
public:
    MyWaitingThread( wxMutex *mutex, wxCondition *condition )
    {
        m_mutex = mutex;
        m_condition = condition;

        Create();
    }

    virtual ExitCode Entry()
    {
        wxPrintf(wxT("Thread %lu has started running.\n"), GetId());
        fflush(stdout);

        gs_cond.Post();

        wxPrintf(wxT("Thread %lu starts to wait...\n"), GetId());
        fflush(stdout);

        m_mutex->Lock();
        m_condition->Wait();
        m_mutex->Unlock();

        wxPrintf(wxT("Thread %lu finished to wait, exiting.\n"), GetId());
        fflush(stdout);

        return 0;
    }

private:
    wxMutex *m_mutex;
    wxCondition *m_condition;
};

static void TestThreadConditions()
{
    wxMutex mutex;
    wxCondition condition(mutex);

    // otherwise its difficult to understand which log messages pertain to
    // which condition
    //wxLogTrace(wxT("thread"), wxT("Local condition var is %08x, gs_cond = %08x"),
    //           condition.GetId(), gs_cond.GetId());

    // create and launch threads
    MyWaitingThread *threads[10];

    size_t n;
    for ( n = 0; n < WXSIZEOF(threads); n++ )
    {
        threads[n] = new MyWaitingThread( &mutex, &condition );
    }

    for ( n = 0; n < WXSIZEOF(threads); n++ )
    {
        threads[n]->Run();
    }

    // wait until all threads run
    wxPuts(wxT("Main thread is waiting for the other threads to start"));
    fflush(stdout);

    size_t nRunning = 0;
    while ( nRunning < WXSIZEOF(threads) )
    {
        gs_cond.Wait();

        nRunning++;

        wxPrintf(wxT("Main thread: %u already running\n"), nRunning);
        fflush(stdout);
    }

    wxPuts(wxT("Main thread: all threads started up."));
    fflush(stdout);

    wxThread::Sleep(500);

#if 1
    // now wake one of them up
    wxPrintf(wxT("Main thread: about to signal the condition.\n"));
    fflush(stdout);
    condition.Signal();
#endif

    wxThread::Sleep(200);

    // wake all the (remaining) threads up, so that they can exit
    wxPrintf(wxT("Main thread: about to broadcast the condition.\n"));
    fflush(stdout);
    condition.Broadcast();

    // give them time to terminate (dirty!)
    wxThread::Sleep(500);
}

// semaphore tests
#include "wx/datetime.h"

class MySemaphoreThread : public wxThread
{
public:
    MySemaphoreThread(int i, wxSemaphore *sem)
        : wxThread(wxTHREAD_JOINABLE),
          m_sem(sem),
          m_i(i)
    {
        Create();
    }

    virtual ExitCode Entry()
    {
        wxPrintf(wxT("%s: Thread #%d (%ld) starting to wait for semaphore...\n"),
                 wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        m_sem->Wait();

        wxPrintf(wxT("%s: Thread #%d (%ld) acquired the semaphore.\n"),
                 wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        Sleep(1000);

        wxPrintf(wxT("%s: Thread #%d (%ld) releasing the semaphore.\n"),
                 wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        m_sem->Post();

        return 0;
    }

private:
    wxSemaphore *m_sem;
    int m_i;
};

WX_DEFINE_ARRAY_PTR(wxThread *, ArrayThreads);

static void TestSemaphore()
{
    wxPuts(wxT("*** Testing wxSemaphore class. ***"));

    static const int SEM_LIMIT = 3;

    wxSemaphore sem(SEM_LIMIT, SEM_LIMIT);
    ArrayThreads threads;

    for ( int i = 0; i < 3*SEM_LIMIT; i++ )
    {
        threads.Add(new MySemaphoreThread(i, &sem));
        threads.Last()->Run();
    }

    for ( size_t n = 0; n < threads.GetCount(); n++ )
    {
        threads[n]->Wait();
        delete threads[n];
    }
}

#endif // TEST_THREADS

// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

#ifdef TEST_SNGLINST
    #include "wx/snglinst.h"
#endif // TEST_SNGLINST

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
    wxSingleInstanceChecker checker;
    if ( checker.Create(wxT(".wxconsole.lock")) )
    {
        if ( checker.IsAnotherRunning() )
        {
            wxPrintf(wxT("Another instance of the program is running, exiting.\n"));

            return 1;
        }

        // wait some time to give time to launch another instance
        wxPrintf(wxT("Press \"Enter\" to continue..."));
        wxFgetc(stdin);
    }
    else // failed to create
    {
        wxPrintf(wxT("Failed to init wxSingleInstanceChecker.\n"));
    }
#endif // TEST_SNGLINST

#ifdef TEST_CMDLINE
    TestCmdLineConvert();

#if wxUSE_CMDLINE_PARSER
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },

        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "i", "input",   "input dir" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size",
            wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date",
            wxCMD_LINE_VAL_DATE },
        { wxCMD_LINE_OPTION, "f", "double",  "output double",
            wxCMD_LINE_VAL_DOUBLE },

        { wxCMD_LINE_PARAM,  NULL, NULL, "input file",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);

    parser.AddOption(wxT("project_name"), wxT(""), wxT("full path to project file"),
                     wxCMD_LINE_VAL_STRING,
                     wxCMD_LINE_OPTION_MANDATORY | wxCMD_LINE_NEEDS_SEPARATOR);

    switch ( parser.Parse() )
    {
        case -1:
            wxLogMessage(wxT("Help was given, terminating."));
            break;

        case 0:
            ShowCmdLine(parser);
            break;

        default:
            wxLogMessage(wxT("Syntax error detected, aborting."));
            break;
    }
#endif // wxUSE_CMDLINE_PARSER

#endif // TEST_CMDLINE

#ifdef TEST_DIR
    #if TEST_ALL
        TestDirExists();
        TestDirEnum();
    #endif
    TestDirTraverse();
#endif // TEST_DIR

#ifdef TEST_DYNLIB
    TestDllLoad();
    TestDllListLoaded();
#endif // TEST_DYNLIB

#ifdef TEST_ENVIRON
    TestEnvironment();
#endif // TEST_ENVIRON

#ifdef TEST_FILECONF
    TestFileConfRead();
#endif // TEST_FILECONF

#ifdef TEST_LOCALE
    TestDefaultLang();
#endif // TEST_LOCALE

#ifdef TEST_LOG
    wxPuts(wxT("*** Testing wxLog ***"));

    wxString s;
    for ( size_t n = 0; n < 8000; n++ )
    {
        s << (wxChar)(wxT('A') + (n % 26));
    }

    wxLogWarning(wxT("The length of the string is %lu"),
                 (unsigned long)s.length());

    wxString msg;
    msg.Printf(wxT("A very very long message: '%s', the end!\n"), s.c_str());

    // this one shouldn't be truncated
    wxPrintf(msg);

    // but this one will because log functions use fixed size buffer
    // (note that it doesn't need '\n' at the end neither - will be added
    //  by wxLog anyhow)
    wxLogMessage(wxT("A very very long message 2: '%s', the end!"), s.c_str());
#endif // TEST_LOG

#ifdef TEST_FILE
    TestFileRead();
    TestTextFileRead();
    TestFileCopy();
    TestTempFile();
#endif // TEST_FILE

#ifdef TEST_FILENAME
    TestFileNameTemp();
    TestFileNameCwd();
    TestFileNameDirManip();
    TestFileNameComparison();
    TestFileNameOperations();
#endif // TEST_FILENAME

#ifdef TEST_FILETIME
    TestFileGetTimes();
    #if 0
    TestFileSetTimes();
    #endif
#endif // TEST_FILETIME

#ifdef TEST_FTP
    wxLog::AddTraceMask(FTP_TRACE_MASK);

    // wxFTP cannot be a static variable as its ctor needs to access
    // wxWidgets internals after it has been initialized
    ftp = new wxFTP;
    ftp->SetLog(new wxProtocolLog(FTP_TRACE_MASK));

    if ( TestFtpConnect() )
    {
        #if TEST_ALL
            TestFtpList();
            TestFtpDownload();
            TestFtpMisc();
            TestFtpFileSize();
            TestFtpUpload();
        #endif // TEST_ALL

        #if TEST_INTERACTIVE
            //TestFtpInteractive();
        #endif
    }
    //else: connecting to the FTP server failed

    delete ftp;
#endif // TEST_FTP

#ifdef TEST_MIME
    //wxLog::AddTraceMask(wxT("mime"));
    TestMimeEnum();
#if 0
    TestMimeOverride();
    TestMimeAssociate();
#endif
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

#ifdef TEST_PATHLIST
    TestPathList();
#endif // TEST_PATHLIST

#ifdef TEST_PRINTF
    TestPrintf();
#endif // TEST_PRINTF

#ifdef TEST_REGCONF
    #if 0
    TestRegConfWrite();
    #endif
    TestRegConfRead();
#endif // TEST_REGCONF

#if defined TEST_REGEX && TEST_INTERACTIVE
    TestRegExInteractive();
#endif // defined TEST_REGEX && TEST_INTERACTIVE

#ifdef TEST_REGISTRY
    TestRegistryRead();
    TestRegistryAssociation();
#endif // TEST_REGISTRY

#ifdef TEST_SOCKETS
    TestSocketServer();
    TestSocketClient();
#endif // TEST_SOCKETS

#ifdef TEST_STREAMS
    #if TEST_ALL
        TestFileStream();
    #endif
        TestMemoryStream();
#endif // TEST_STREAMS

#ifdef TEST_TEXTSTREAM
    TestTextInputStream();
#endif // TEST_TEXTSTREAM

#ifdef TEST_THREADS
    int nCPUs = wxThread::GetCPUCount();
    wxPrintf(wxT("This system has %d CPUs\n"), nCPUs);
    if ( nCPUs != -1 )
        wxThread::SetConcurrency(nCPUs);

        TestJoinableThreads();

    #if TEST_ALL
        TestJoinableThreads();
        TestDetachedThreads();
        TestThreadSuspend();
        TestThreadDelete();
        TestThreadConditions();
        TestSemaphore();
    #endif
#endif // TEST_THREADS

#ifdef TEST_TIMER
    TestStopWatch();
    TestTimer();
#endif // TEST_TIMER

#ifdef TEST_DATETIME
    #if TEST_ALL
        TestTimeStatic();
        TestTimeRange();
        TestTimeZones();
        TestTimeDST();
        TestTimeHolidays();
        TestTimeSpanFormat();
        TestTimeMS();

        TestTimeZoneBug();
    #endif

    #if TEST_INTERACTIVE
        TestDateTimeInteractive();
    #endif
#endif // TEST_DATETIME

#ifdef TEST_SCOPEGUARD
    TestScopeGuard();
#endif

#ifdef TEST_STACKWALKER
#if wxUSE_STACKWALKER
    TestStackWalk(argv[0]);
#endif
#endif // TEST_STACKWALKER

#ifdef TEST_STDPATHS
    TestStandardPaths();
#endif

#ifdef TEST_USLEEP
    wxPuts(wxT("Sleeping for 3 seconds... z-z-z-z-z..."));
    wxUsleep(3000);
#endif // TEST_USLEEP

#ifdef TEST_VOLUME
    TestFSVolume();
#endif // TEST_VOLUME

#ifdef TEST_WCHAR
    TestUtf8();
    TestEncodingConverter();
#endif // TEST_WCHAR

#ifdef TEST_ZIP
    TestZipStreamRead();
    TestZipFileSystem();
#endif // TEST_ZIP

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
