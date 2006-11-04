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
#include "wx/app.h"
#include "wx/log.h"
#include "wx/apptrait.h"
#include "wx/platinfo.h"

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
#define TEST_ALL 0


#if TEST_ALL
    #define TEST_CMDLINE
    #define TEST_DATETIME
    #define TEST_DIR
    #define TEST_DYNLIB
    #define TEST_ENVIRON
    #define TEST_EXECUTE
    #define TEST_FILE
    #define TEST_FILECONF
    #define TEST_FILENAME
    #define TEST_FILETIME
 //   #define TEST_FTP  --FIXME! (RN)
    #define TEST_INFO_FUNCTIONS
    #define TEST_LOCALE
    #define TEST_LOG
    #define TEST_MIME
    #define TEST_MODULE
    #define TEST_PATHLIST
    #define TEST_ODBC
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
    // #define TEST_VCARD            -- don't enable this (VZ)
//    #define TEST_VOLUME   --FIXME! (RN)
    #define TEST_WCHAR
    #define TEST_ZIP
#else // #if TEST_ALL
    #define TEST_FILE
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
    (void)str.Replace(_T("\t"), _T("\\t"));
    (void)str.Replace(_T("\n"), _T("\\n"));
    (void)str.Replace(_T("\r"), _T("\\r"));

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
    wxString s = _T("Command line parsed successfully:\nInput files: ");

    size_t count = parser.GetParamCount();
    for ( size_t param = 0; param < count; param++ )
    {
        s << parser.GetParam(param) << ' ';
    }

    s << '\n'
      << _T("Verbose:\t") << (parser.Found(_T("v")) ? _T("yes") : _T("no")) << '\n'
      << _T("Quiet:\t") << (parser.Found(_T("q")) ? _T("yes") : _T("no")) << '\n';

    wxString strVal;
    long lVal;
    wxDateTime dt;
    if ( parser.Found(_T("o"), &strVal) )
        s << _T("Output file:\t") << strVal << '\n';
    if ( parser.Found(_T("i"), &strVal) )
        s << _T("Input dir:\t") << strVal << '\n';
    if ( parser.Found(_T("s"), &lVal) )
        s << _T("Size:\t") << lVal << '\n';
    if ( parser.Found(_T("d"), &dt) )
        s << _T("Date:\t") << dt.FormatISODate() << '\n';
    if ( parser.Found(_T("project_name"), &strVal) )
        s << _T("Project:\t") << strVal << '\n';

    wxLogMessage(s);
}

#endif // wxUSE_CMDLINE_PARSER

static void TestCmdLineConvert()
{
    static const wxChar *cmdlines[] =
    {
        _T("arg1 arg2"),
        _T("-a \"-bstring 1\" -c\"string 2\" \"string 3\""),
        _T("literal \\\" and \"\""),
    };

    for ( size_t n = 0; n < WXSIZEOF(cmdlines); n++ )
    {
        const wxChar *cmdline = cmdlines[n];
        wxPrintf(_T("Parsing: %s\n"), cmdline);
        wxArrayString args = wxCmdLineParser::ConvertStringToArgs(cmdline);

        size_t count = args.GetCount();
        wxPrintf(_T("\targc = %u\n"), count);
        for ( size_t arg = 0; arg < count; arg++ )
        {
            wxPrintf(_T("\targv[%u] = %s\n"), arg, args[arg].c_str());
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
    static const wxChar *ROOTDIR = _T("/");
    static const wxChar *TESTDIR = _T("/usr/local/share");
#elif defined(__WXMSW__) || defined(__DOS__) || defined(__OS2__)
    static const wxChar *ROOTDIR = _T("c:\\");
    static const wxChar *TESTDIR = _T("d:\\");
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
        wxPrintf(_T("\t%s\n"), filename.c_str());

        cont = dir.GetNext(&filename);
    }

    wxPuts(wxEmptyString);
}

#if TEST_ALL

static void TestDirEnum()
{
    wxPuts(_T("*** Testing wxDir::GetFirst/GetNext ***"));

    wxString cwd = wxGetCwd();
    if ( !wxDir::Exists(cwd) )
    {
        wxPrintf(_T("ERROR: current directory '%s' doesn't exist?\n"), cwd.c_str());
        return;
    }

    wxDir dir(cwd);
    if ( !dir.IsOpened() )
    {
        wxPrintf(_T("ERROR: failed to open current directory '%s'.\n"), cwd.c_str());
        return;
    }

    wxPuts(_T("Enumerating everything in current directory:"));
    TestDirEnumHelper(dir);

    wxPuts(_T("Enumerating really everything in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT);

    wxPuts(_T("Enumerating object files in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT, _T("*.o*"));

    wxPuts(_T("Enumerating directories in current directory:"));
    TestDirEnumHelper(dir, wxDIR_DIRS);

    wxPuts(_T("Enumerating files in current directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES);

    wxPuts(_T("Enumerating files including hidden in current directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    dir.Open(ROOTDIR);

    wxPuts(_T("Enumerating everything in root directory:"));
    TestDirEnumHelper(dir, wxDIR_DEFAULT);

    wxPuts(_T("Enumerating directories in root directory:"));
    TestDirEnumHelper(dir, wxDIR_DIRS);

    wxPuts(_T("Enumerating files in root directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES);

    wxPuts(_T("Enumerating files including hidden in root directory:"));
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    wxPuts(_T("Enumerating files in non existing directory:"));
    wxDir dirNo(_T("nosuchdir"));
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
        wxSplitPath(dirname, &path, &name, &ext);

        if ( !ext.empty() )
            name << _T('.') << ext;

        wxString indent;
        for ( const wxChar *p = path.c_str(); *p; p++ )
        {
            if ( wxIsPathSeparator(*p) )
                indent += _T("    ");
        }

        wxPrintf(_T("%s%s\n"), indent.c_str(), name.c_str());

        return wxDIR_CONTINUE;
    }
};

static void TestDirTraverse()
{
    wxPuts(_T("*** Testing wxDir::Traverse() ***"));

    // enum all files
    wxArrayString files;
    size_t n = wxDir::GetAllFiles(TESTDIR, &files);
    wxPrintf(_T("There are %u files under '%s'\n"), n, TESTDIR);
    if ( n > 1 )
    {
        wxPrintf(_T("First one is '%s'\n"), files[0u].c_str());
        wxPrintf(_T(" last one is '%s'\n"), files[n - 1].c_str());
    }

    // enum again with custom traverser
    wxPuts(_T("Now enumerating directories:"));
    wxDir dir(TESTDIR);
    DirPrintTraverser traverser;
    dir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
}

#if TEST_ALL

static void TestDirExists()
{
    wxPuts(_T("*** Testing wxDir::Exists() ***"));

    static const wxChar *dirnames[] =
    {
        _T("."),
#if defined(__WXMSW__)
        _T("c:"),
        _T("c:\\"),
        _T("\\\\share\\file"),
        _T("c:\\dos"),
        _T("c:\\dos\\"),
        _T("c:\\dos\\\\"),
        _T("c:\\autoexec.bat"),
#elif defined(__UNIX__)
        _T("/"),
        _T("//"),
        _T("/usr/bin"),
        _T("/usr//bin"),
        _T("/usr///bin"),
#endif
    };

    for ( size_t n = 0; n < WXSIZEOF(dirnames); n++ )
    {
        wxPrintf(_T("%-40s: %s\n"),
                 dirnames[n],
                 wxDir::Exists(dirnames[n]) ? _T("exists")
                                            : _T("doesn't exist"));
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
    static const wxChar *LIB_NAME = _T("kernel32.dll");
    static const wxChar *FUNC_NAME = _T("lstrlenA");
#elif defined(__UNIX__)
    // weird: using just libc.so does *not* work!
    static const wxChar *LIB_NAME = _T("/lib/libc.so.6");
    static const wxChar *FUNC_NAME = _T("strlen");
#else
    #error "don't know how to test wxDllLoader on this platform"
#endif

    wxPuts(_T("*** testing basic wxDynamicLibrary functions ***\n"));

    wxDynamicLibrary lib(LIB_NAME);
    if ( !lib.IsLoaded() )
    {
        wxPrintf(_T("ERROR: failed to load '%s'.\n"), LIB_NAME);
    }
    else
    {
        typedef int (wxSTDCALL *wxStrlenType)(const char *);
        wxStrlenType pfnStrlen = (wxStrlenType)lib.GetSymbol(FUNC_NAME);
        if ( !pfnStrlen )
        {
            wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME, LIB_NAME);
        }
        else
        {
            wxPrintf(_T("Calling %s dynamically loaded from %s "),
                     FUNC_NAME, LIB_NAME);

            if ( pfnStrlen("foo") != 3 )
            {
                wxPrintf(_T("ERROR: loaded function is not wxStrlen()!\n"));
            }
            else
            {
                wxPuts(_T("... ok"));
            }
        }

#ifdef __WXMSW__
        static const wxChar *FUNC_NAME_AW = _T("lstrlen");

        typedef int (wxSTDCALL *wxStrlenTypeAorW)(const wxChar *);
        wxStrlenTypeAorW
            pfnStrlenAorW = (wxStrlenTypeAorW)lib.GetSymbolAorW(FUNC_NAME_AW);
        if ( !pfnStrlenAorW )
        {
            wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME_AW, LIB_NAME);
        }
        else
        {
            if ( pfnStrlenAorW(_T("foobar")) != 6 )
            {
                wxPrintf(_T("ERROR: loaded function is not wxStrlen()!\n"));
            }
        }
#endif // __WXMSW__
    }
}

#if defined(__WXMSW__) || defined(__UNIX__)

static void TestDllListLoaded()
{
    wxPuts(_T("*** testing wxDynamicLibrary::ListLoaded() ***\n"));

    puts("\nLoaded modules:");
    wxDynamicLibraryDetailsArray dlls = wxDynamicLibrary::ListLoaded();
    const size_t count = dlls.GetCount();
    for ( size_t n = 0; n < count; ++n )
    {
        const wxDynamicLibraryDetails& details = dlls[n];
        printf("%-45s", details.GetPath().mb_str());

        void *addr;
        size_t len;
        if ( details.GetAddress(&addr, &len) )
        {
            printf(" %08lx:%08lx",
                   (unsigned long)addr, (unsigned long)((char *)addr + len));
        }

        printf(" %s\n", details.GetVersion().mb_str());
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
        val = _T("<empty>");
    else
        val = wxString(_T('\'')) + val + _T('\'');

    return val;
}

static void TestEnvironment()
{
    const wxChar *var = _T("wxTestVar");

    wxPuts(_T("*** testing environment access functions ***"));

    wxPrintf(_T("Initially getenv(%s) = %s\n"), var, MyGetEnv(var).c_str());
    wxSetEnv(var, _T("value for wxTestVar"));
    wxPrintf(_T("After wxSetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxSetEnv(var, _T("another value"));
    wxPrintf(_T("After 2nd wxSetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxUnsetEnv(var);
    wxPrintf(_T("After wxUnsetEnv: getenv(%s) = %s\n"),  var, MyGetEnv(var).c_str());
    wxPrintf(_T("PATH = %s\n"),  MyGetEnv(_T("PATH")).c_str());
}

#endif // TEST_ENVIRON

// ----------------------------------------------------------------------------
// wxExecute
// ----------------------------------------------------------------------------

#ifdef TEST_EXECUTE

#include "wx/utils.h"

static void TestExecute()
{
    wxPuts(_T("*** testing wxExecute ***"));

#ifdef __UNIX__
    #define COMMAND "cat -n ../../Makefile" // "echo hi"
    #define SHELL_COMMAND "echo hi from shell"
    #define REDIRECT_COMMAND COMMAND // "date"
#elif defined(__WXMSW__)
    #define COMMAND "command.com /c echo hi"
    #define SHELL_COMMAND "echo hi"
    #define REDIRECT_COMMAND COMMAND
#else
    #error "no command to exec"
#endif // OS

    wxPrintf(_T("Testing wxShell: "));
    fflush(stdout);
    if ( wxShell(_T(SHELL_COMMAND)) )
        wxPuts(_T("Ok."));
    else
        wxPuts(_T("ERROR."));

    wxPrintf(_T("Testing wxExecute: "));
    fflush(stdout);
    if ( wxExecute(_T(COMMAND), true /* sync */) == 0 )
        wxPuts(_T("Ok."));
    else
        wxPuts(_T("ERROR."));

#if 0 // no, it doesn't work (yet?)
    wxPrintf(_T("Testing async wxExecute: "));
    fflush(stdout);
    if ( wxExecute(COMMAND) != 0 )
        wxPuts(_T("Ok (command launched)."));
    else
        wxPuts(_T("ERROR."));
#endif // 0

    wxPrintf(_T("Testing wxExecute with redirection:\n"));
    wxArrayString output;
    if ( wxExecute(_T(REDIRECT_COMMAND), output) != 0 )
    {
        wxPuts(_T("ERROR."));
    }
    else
    {
        size_t count = output.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxPrintf(_T("\t%s\n"), output[n].c_str());
        }

        wxPuts(_T("Ok."));
    }
}

#endif // TEST_EXECUTE

// ----------------------------------------------------------------------------
// file
// ----------------------------------------------------------------------------

#ifdef TEST_FILE

#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/textfile.h"

static void TestFileRead()
{
    wxPuts(_T("*** wxFile read test ***"));

    wxFile file(_T("testdata.fc"));
    if ( file.IsOpened() )
    {
        wxPrintf(_T("File length: %lu\n"), file.Length());

        wxPuts(_T("File dump:\n----------"));

        static const size_t len = 1024;
        wxChar buf[len];
        for ( ;; )
        {
            size_t nRead = file.Read(buf, len);
            if ( nRead == (size_t)wxInvalidOffset )
            {
                wxPrintf(_T("Failed to read the file."));
                break;
            }

            fwrite(buf, nRead, 1, stdout);

            if ( nRead < len )
                break;
        }

        wxPuts(_T("----------"));
    }
    else
    {
        wxPrintf(_T("ERROR: can't open test file.\n"));
    }

    wxPuts(wxEmptyString);
}

static void TestTextFileRead()
{
    wxPuts(_T("*** wxTextFile read test ***"));

    wxTextFile file(_T("testdata.fc"));
    if ( file.Open() )
    {
        wxPrintf(_T("Number of lines: %u\n"), file.GetLineCount());
        wxPrintf(_T("Last line: '%s'\n"), file.GetLastLine().c_str());

        wxString s;

        wxPuts(_T("\nDumping the entire file:"));
        for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
        {
            wxPrintf(_T("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
        }
        wxPrintf(_T("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());

        wxPuts(_T("\nAnd now backwards:"));
        for ( s = file.GetLastLine();
              file.GetCurrentLine() != 0;
              s = file.GetPrevLine() )
        {
            wxPrintf(_T("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
        }
        wxPrintf(_T("%6u: %s\n"), file.GetCurrentLine() + 1, s.c_str());
    }
    else
    {
        wxPrintf(_T("ERROR: can't open '%s'\n"), file.GetName());
    }

    wxPuts(wxEmptyString);
}

static void TestFileCopy()
{
    wxPuts(_T("*** Testing wxCopyFile ***"));

    static const wxChar *filename1 = _T("testdata.fc");
    static const wxChar *filename2 = _T("test2");
    if ( !wxCopyFile(filename1, filename2) )
    {
        wxPuts(_T("ERROR: failed to copy file"));
    }
    else
    {
        wxFFile f1(filename1, _T("rb")),
                f2(filename2, _T("rb"));

        if ( !f1.IsOpened() || !f2.IsOpened() )
        {
            wxPuts(_T("ERROR: failed to open file(s)"));
        }
        else
        {
            wxString s1, s2;
            if ( !f1.ReadAll(&s1) || !f2.ReadAll(&s2) )
            {
                wxPuts(_T("ERROR: failed to read file(s)"));
            }
            else
            {
                if ( (s1.length() != s2.length()) ||
                     (memcmp(s1.c_str(), s2.c_str(), s1.length()) != 0) )
                {
                    wxPuts(_T("ERROR: copy error!"));
                }
                else
                {
                    wxPuts(_T("File was copied ok."));
                }
            }
        }
    }

    if ( !wxRemoveFile(filename2) )
    {
        wxPuts(_T("ERROR: failed to remove the file"));
    }

    wxPuts(wxEmptyString);
}

static void TestTempFile()
{
    wxPuts(_T("*** wxTempFile test ***"));

    wxTempFile tmpFile;
    if ( tmpFile.Open(_T("test2")) && tmpFile.Write(_T("the answer is 42")) )
    {
        if ( tmpFile.Commit() )
            wxPuts(_T("File committed."));
        else
            wxPuts(_T("ERROR: could't commit temp file."));

        wxRemoveFile(_T("test2"));
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
    { _T("value1"),                       _T("one") },
    { _T("value2"),                       _T("two") },
    { _T("novalue"),                      _T("default") },
};

static void TestFileConfRead()
{
    wxPuts(_T("*** testing wxFileConfig loading/reading ***"));

    wxFileConfig fileconf(_T("test"), wxEmptyString,
                          _T("testdata.fc"), wxEmptyString,
                          wxCONFIG_USE_RELATIVE_PATH);

    // test simple reading
    wxPuts(_T("\nReading config file:"));
    wxString defValue(_T("default")), value;
    for ( size_t n = 0; n < WXSIZEOF(fcTestData); n++ )
    {
        const FileConfTestData& data = fcTestData[n];
        value = fileconf.Read(data.name, defValue);
        wxPrintf(_T("\t%s = %s "), data.name, value.c_str());
        if ( value == data.value )
        {
            wxPuts(_T("(ok)"));
        }
        else
        {
            wxPrintf(_T("(ERROR: should be %s)\n"), data.value);
        }
    }

    // test enumerating the entries
    wxPuts(_T("\nEnumerating all root entries:"));
    long dummy;
    wxString name;
    bool cont = fileconf.GetFirstEntry(name, dummy);
    while ( cont )
    {
        wxPrintf(_T("\t%s = %s\n"),
               name.c_str(),
               fileconf.Read(name.c_str(), _T("ERROR")).c_str());

        cont = fileconf.GetNextEntry(name, dummy);
    }

    static const wxChar *testEntry = _T("TestEntry");
    wxPrintf(_T("\nTesting deletion of newly created \"Test\" entry: "));
    fileconf.Write(testEntry, _T("A value"));
    fileconf.DeleteEntry(testEntry);
    wxPrintf(fileconf.HasEntry(testEntry) ? _T("ERROR\n") : _T("ok\n"));
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

    wxPrintf(_T("'%s'-> vol '%s', path '%s', name '%s', ext '%s'\n"),
             full.c_str(), vol.c_str(), path.c_str(), name.c_str(), ext.c_str());

    wxFileName::SplitPath(full, &path, &name, &ext);
    wxPrintf(_T("or\t\t-> path '%s', name '%s', ext '%s'\n"),
             path.c_str(), name.c_str(), ext.c_str());

    wxPrintf(_T("path is also:\t'%s'\n"), fn.GetPath().c_str());
    wxPrintf(_T("with volume: \t'%s'\n"),
             fn.GetPath(wxPATH_GET_VOLUME).c_str());
    wxPrintf(_T("with separator:\t'%s'\n"),
             fn.GetPath(wxPATH_GET_SEPARATOR).c_str());
    wxPrintf(_T("with both:   \t'%s'\n"),
             fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).c_str());

    wxPuts(_T("The directories in the path are:"));
    wxArrayString dirs = fn.GetDirs();
    size_t count = dirs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxPrintf(_T("\t%u: %s\n"), n, dirs[n].c_str());
    }
}
#endif

static void TestFileNameTemp()
{
    wxPuts(_T("*** testing wxFileName temp file creation ***"));

    static const wxChar *tmpprefixes[] =
    {
        _T(""),
        _T("foo"),
        _T(".."),
        _T("../bar"),
#ifdef __UNIX__
        _T("/tmp/foo"),
        _T("/tmp/foo/bar"), // this one must be an error
#endif // __UNIX__
    };

    for ( size_t n = 0; n < WXSIZEOF(tmpprefixes); n++ )
    {
        wxString path = wxFileName::CreateTempFileName(tmpprefixes[n]);
        if ( path.empty() )
        {
            // "error" is not in upper case because it may be ok
            wxPrintf(_T("Prefix '%s'\t-> error\n"), tmpprefixes[n]);
        }
        else
        {
            wxPrintf(_T("Prefix '%s'\t-> temp file '%s'\n"),
                   tmpprefixes[n], path.c_str());

            if ( !wxRemoveFile(path) )
            {
                wxLogWarning(_T("Failed to remove temp file '%s'"),
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
    wxFileName fn(_T("testdata.fc"));

    wxDateTime dtAccess, dtMod, dtCreate;
    if ( !fn.GetTimes(&dtAccess, &dtMod, &dtCreate) )
    {
        wxPrintf(_T("ERROR: GetTimes() failed.\n"));
    }
    else
    {
        static const wxChar *fmt = _T("%Y-%b-%d %H:%M:%S");

        wxPrintf(_T("File times for '%s':\n"), fn.GetFullPath().c_str());
        wxPrintf(_T("Creation:    \t%s\n"), dtCreate.Format(fmt).c_str());
        wxPrintf(_T("Last read:   \t%s\n"), dtAccess.Format(fmt).c_str());
        wxPrintf(_T("Last write:  \t%s\n"), dtMod.Format(fmt).c_str());
    }
}

#if 0
static void TestFileSetTimes()
{
    wxFileName fn(_T("testdata.fc"));

    if ( !fn.Touch() )
    {
        wxPrintf(_T("ERROR: Touch() failed.\n"));
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

static wxLocale gs_localeDefault(wxLANGUAGE_ENGLISH);

// find the name of the language from its value
static const wxChar *GetLangName(int lang)
{
    static const wxChar *languageNames[] =
    {
        _T("DEFAULT"),
        _T("UNKNOWN"),
        _T("ABKHAZIAN"),
        _T("AFAR"),
        _T("AFRIKAANS"),
        _T("ALBANIAN"),
        _T("AMHARIC"),
        _T("ARABIC"),
        _T("ARABIC_ALGERIA"),
        _T("ARABIC_BAHRAIN"),
        _T("ARABIC_EGYPT"),
        _T("ARABIC_IRAQ"),
        _T("ARABIC_JORDAN"),
        _T("ARABIC_KUWAIT"),
        _T("ARABIC_LEBANON"),
        _T("ARABIC_LIBYA"),
        _T("ARABIC_MOROCCO"),
        _T("ARABIC_OMAN"),
        _T("ARABIC_QATAR"),
        _T("ARABIC_SAUDI_ARABIA"),
        _T("ARABIC_SUDAN"),
        _T("ARABIC_SYRIA"),
        _T("ARABIC_TUNISIA"),
        _T("ARABIC_UAE"),
        _T("ARABIC_YEMEN"),
        _T("ARMENIAN"),
        _T("ASSAMESE"),
        _T("AYMARA"),
        _T("AZERI"),
        _T("AZERI_CYRILLIC"),
        _T("AZERI_LATIN"),
        _T("BASHKIR"),
        _T("BASQUE"),
        _T("BELARUSIAN"),
        _T("BENGALI"),
        _T("BHUTANI"),
        _T("BIHARI"),
        _T("BISLAMA"),
        _T("BRETON"),
        _T("BULGARIAN"),
        _T("BURMESE"),
        _T("CAMBODIAN"),
        _T("CATALAN"),
        _T("CHINESE"),
        _T("CHINESE_SIMPLIFIED"),
        _T("CHINESE_TRADITIONAL"),
        _T("CHINESE_HONGKONG"),
        _T("CHINESE_MACAU"),
        _T("CHINESE_SINGAPORE"),
        _T("CHINESE_TAIWAN"),
        _T("CORSICAN"),
        _T("CROATIAN"),
        _T("CZECH"),
        _T("DANISH"),
        _T("DUTCH"),
        _T("DUTCH_BELGIAN"),
        _T("ENGLISH"),
        _T("ENGLISH_UK"),
        _T("ENGLISH_US"),
        _T("ENGLISH_AUSTRALIA"),
        _T("ENGLISH_BELIZE"),
        _T("ENGLISH_BOTSWANA"),
        _T("ENGLISH_CANADA"),
        _T("ENGLISH_CARIBBEAN"),
        _T("ENGLISH_DENMARK"),
        _T("ENGLISH_EIRE"),
        _T("ENGLISH_JAMAICA"),
        _T("ENGLISH_NEW_ZEALAND"),
        _T("ENGLISH_PHILIPPINES"),
        _T("ENGLISH_SOUTH_AFRICA"),
        _T("ENGLISH_TRINIDAD"),
        _T("ENGLISH_ZIMBABWE"),
        _T("ESPERANTO"),
        _T("ESTONIAN"),
        _T("FAEROESE"),
        _T("FARSI"),
        _T("FIJI"),
        _T("FINNISH"),
        _T("FRENCH"),
        _T("FRENCH_BELGIAN"),
        _T("FRENCH_CANADIAN"),
        _T("FRENCH_LUXEMBOURG"),
        _T("FRENCH_MONACO"),
        _T("FRENCH_SWISS"),
        _T("FRISIAN"),
        _T("GALICIAN"),
        _T("GEORGIAN"),
        _T("GERMAN"),
        _T("GERMAN_AUSTRIAN"),
        _T("GERMAN_BELGIUM"),
        _T("GERMAN_LIECHTENSTEIN"),
        _T("GERMAN_LUXEMBOURG"),
        _T("GERMAN_SWISS"),
        _T("GREEK"),
        _T("GREENLANDIC"),
        _T("GUARANI"),
        _T("GUJARATI"),
        _T("HAUSA"),
        _T("HEBREW"),
        _T("HINDI"),
        _T("HUNGARIAN"),
        _T("ICELANDIC"),
        _T("INDONESIAN"),
        _T("INTERLINGUA"),
        _T("INTERLINGUE"),
        _T("INUKTITUT"),
        _T("INUPIAK"),
        _T("IRISH"),
        _T("ITALIAN"),
        _T("ITALIAN_SWISS"),
        _T("JAPANESE"),
        _T("JAVANESE"),
        _T("KANNADA"),
        _T("KASHMIRI"),
        _T("KASHMIRI_INDIA"),
        _T("KAZAKH"),
        _T("KERNEWEK"),
        _T("KINYARWANDA"),
        _T("KIRGHIZ"),
        _T("KIRUNDI"),
        _T("KONKANI"),
        _T("KOREAN"),
        _T("KURDISH"),
        _T("LAOTHIAN"),
        _T("LATIN"),
        _T("LATVIAN"),
        _T("LINGALA"),
        _T("LITHUANIAN"),
        _T("MACEDONIAN"),
        _T("MALAGASY"),
        _T("MALAY"),
        _T("MALAYALAM"),
        _T("MALAY_BRUNEI_DARUSSALAM"),
        _T("MALAY_MALAYSIA"),
        _T("MALTESE"),
        _T("MANIPURI"),
        _T("MAORI"),
        _T("MARATHI"),
        _T("MOLDAVIAN"),
        _T("MONGOLIAN"),
        _T("NAURU"),
        _T("NEPALI"),
        _T("NEPALI_INDIA"),
        _T("NORWEGIAN_BOKMAL"),
        _T("NORWEGIAN_NYNORSK"),
        _T("OCCITAN"),
        _T("ORIYA"),
        _T("OROMO"),
        _T("PASHTO"),
        _T("POLISH"),
        _T("PORTUGUESE"),
        _T("PORTUGUESE_BRAZILIAN"),
        _T("PUNJABI"),
        _T("QUECHUA"),
        _T("RHAETO_ROMANCE"),
        _T("ROMANIAN"),
        _T("RUSSIAN"),
        _T("RUSSIAN_UKRAINE"),
        _T("SAMOAN"),
        _T("SANGHO"),
        _T("SANSKRIT"),
        _T("SCOTS_GAELIC"),
        _T("SERBIAN"),
        _T("SERBIAN_CYRILLIC"),
        _T("SERBIAN_LATIN"),
        _T("SERBO_CROATIAN"),
        _T("SESOTHO"),
        _T("SETSWANA"),
        _T("SHONA"),
        _T("SINDHI"),
        _T("SINHALESE"),
        _T("SISWATI"),
        _T("SLOVAK"),
        _T("SLOVENIAN"),
        _T("SOMALI"),
        _T("SPANISH"),
        _T("SPANISH_ARGENTINA"),
        _T("SPANISH_BOLIVIA"),
        _T("SPANISH_CHILE"),
        _T("SPANISH_COLOMBIA"),
        _T("SPANISH_COSTA_RICA"),
        _T("SPANISH_DOMINICAN_REPUBLIC"),
        _T("SPANISH_ECUADOR"),
        _T("SPANISH_EL_SALVADOR"),
        _T("SPANISH_GUATEMALA"),
        _T("SPANISH_HONDURAS"),
        _T("SPANISH_MEXICAN"),
        _T("SPANISH_MODERN"),
        _T("SPANISH_NICARAGUA"),
        _T("SPANISH_PANAMA"),
        _T("SPANISH_PARAGUAY"),
        _T("SPANISH_PERU"),
        _T("SPANISH_PUERTO_RICO"),
        _T("SPANISH_URUGUAY"),
        _T("SPANISH_US"),
        _T("SPANISH_VENEZUELA"),
        _T("SUNDANESE"),
        _T("SWAHILI"),
        _T("SWEDISH"),
        _T("SWEDISH_FINLAND"),
        _T("TAGALOG"),
        _T("TAJIK"),
        _T("TAMIL"),
        _T("TATAR"),
        _T("TELUGU"),
        _T("THAI"),
        _T("TIBETAN"),
        _T("TIGRINYA"),
        _T("TONGA"),
        _T("TSONGA"),
        _T("TURKISH"),
        _T("TURKMEN"),
        _T("TWI"),
        _T("UIGHUR"),
        _T("UKRAINIAN"),
        _T("URDU"),
        _T("URDU_INDIA"),
        _T("URDU_PAKISTAN"),
        _T("UZBEK"),
        _T("UZBEK_CYRILLIC"),
        _T("UZBEK_LATIN"),
        _T("VIETNAMESE"),
        _T("VOLAPUK"),
        _T("WELSH"),
        _T("WOLOF"),
        _T("XHOSA"),
        _T("YIDDISH"),
        _T("YORUBA"),
        _T("ZHUANG"),
        _T("ZULU"),
    };

    if ( (size_t)lang < WXSIZEOF(languageNames) )
        return languageNames[lang];
    else
        return _T("INVALID");
}

static void TestDefaultLang()
{
    wxPuts(_T("*** Testing wxLocale::GetSystemLanguage ***"));

    static const wxChar *langStrings[] =
    {
        NULL,               // system default
        _T("C"),
        _T("fr"),
        _T("fr_FR"),
        _T("en"),
        _T("en_GB"),
        _T("en_US"),
        _T("de_DE.iso88591"),
        _T("german"),
        _T("?"),            // invalid lang spec
        _T("klingonese"),   // I bet on some systems it does exist...
    };

    wxPrintf(_T("The default system encoding is %s (%d)\n"),
             wxLocale::GetSystemEncodingName().c_str(),
             wxLocale::GetSystemEncoding());

    for ( size_t n = 0; n < WXSIZEOF(langStrings); n++ )
    {
        const wxChar *langStr = langStrings[n];
        if ( langStr )
        {
            // FIXME: this doesn't do anything at all under Windows, we need
            //        to create a new wxLocale!
            wxSetEnv(_T("LC_ALL"), langStr);
        }

        int lang = gs_localeDefault.GetSystemLanguage();
        wxPrintf(_T("Locale for '%s' is %s.\n"),
                 langStr ? langStr : _T("system default"), GetLangName(lang));
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
    wxPuts(_T("*** Testing wxMimeTypesManager::EnumAllFileTypes() ***\n"));

    wxArrayString mimetypes;

    size_t count = wxTheMimeTypesManager->EnumAllFileTypes(mimetypes);

    wxPrintf(_T("*** All %u known filetypes: ***\n"), count);

    wxArrayString exts;
    wxString desc;

    for ( size_t n = 0; n < count; n++ )
    {
        wxFileType *filetype =
            wxTheMimeTypesManager->GetFileTypeFromMimeType(mimetypes[n]);
        if ( !filetype )
        {
            wxPrintf(_T("nothing known about the filetype '%s'!\n"),
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
                extsAll << _T(", ");
            extsAll += exts[e];
        }

        wxPrintf(_T("\t%s: %s (%s)\n"),
               mimetypes[n].c_str(), desc.c_str(), extsAll.c_str());
    }

    wxPuts(wxEmptyString);
}

static void TestMimeOverride()
{
    wxPuts(_T("*** Testing wxMimeTypesManager additional files loading ***\n"));

    static const wxChar *mailcap = _T("/tmp/mailcap");
    static const wxChar *mimetypes = _T("/tmp/mime.types");

    if ( wxFile::Exists(mailcap) )
        wxPrintf(_T("Loading mailcap from '%s': %s\n"),
                 mailcap,
                 wxTheMimeTypesManager->ReadMailcap(mailcap) ? _T("ok") : _T("ERROR"));
    else
        wxPrintf(_T("WARN: mailcap file '%s' doesn't exist, not loaded.\n"),
                 mailcap);

    if ( wxFile::Exists(mimetypes) )
        wxPrintf(_T("Loading mime.types from '%s': %s\n"),
                 mimetypes,
                 wxTheMimeTypesManager->ReadMimeTypes(mimetypes) ? _T("ok") : _T("ERROR"));
    else
        wxPrintf(_T("WARN: mime.types file '%s' doesn't exist, not loaded.\n"),
                 mimetypes);

    wxPuts(wxEmptyString);
}

static void TestMimeFilename()
{
    wxPuts(_T("*** Testing MIME type from filename query ***\n"));

    static const wxChar *filenames[] =
    {
        _T("readme.txt"),
        _T("document.pdf"),
        _T("image.gif"),
        _T("picture.jpeg"),
    };

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const wxString fname = filenames[n];
        wxString ext = fname.AfterLast(_T('.'));
        wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
        if ( !ft )
        {
            wxPrintf(_T("WARNING: extension '%s' is unknown.\n"), ext.c_str());
        }
        else
        {
            wxString desc;
            if ( !ft->GetDescription(&desc) )
                desc = _T("<no description>");

            wxString cmd;
            if ( !ft->GetOpenCommand(&cmd,
                                     wxFileType::MessageParameters(fname, wxEmptyString)) )
                cmd = _T("<no command available>");
            else
                cmd = wxString(_T('"')) + cmd + _T('"');

            wxPrintf(_T("To open %s (%s) do %s.\n"),
                     fname.c_str(), desc.c_str(), cmd.c_str());

            delete ft;
        }
    }

    wxPuts(wxEmptyString);
}

static void TestMimeAssociate()
{
    wxPuts(_T("*** Testing creation of filetype association ***\n"));

    wxFileTypeInfo ftInfo(
                            _T("application/x-xyz"),
                            _T("xyzview '%s'"), // open cmd
                            _T(""),             // print cmd
                            _T("XYZ File"),     // description
                            _T(".xyz"),         // extensions
                            NULL                // end of extensions
                         );
    ftInfo.SetShortDesc(_T("XYZFile")); // used under Win32 only

    wxFileType *ft = wxTheMimeTypesManager->Associate(ftInfo);
    if ( !ft )
    {
        wxPuts(_T("ERROR: failed to create association!"));
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
// module dependencies feature
// ----------------------------------------------------------------------------

#ifdef TEST_MODULE

#include "wx/module.h"

class wxTestModule : public wxModule
{
protected:
    virtual bool OnInit() { wxPrintf(_T("Load module: %s\n"), GetClassInfo()->GetClassName()); return true; }
    virtual void OnExit() { wxPrintf(_T("Unload module: %s\n"), GetClassInfo()->GetClassName()); }
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
    wxPuts(_T("*** Testing wxGetDiskSpace() ***"));

    for ( ;; )
    {
        wxChar pathname[128];
        wxPrintf(_T("\nEnter a directory name: "));
        if ( !wxFgets(pathname, WXSIZEOF(pathname), stdin) )
            break;

        // kill the last '\n'
        pathname[wxStrlen(pathname) - 1] = 0;

        wxLongLong total, free;
        if ( !wxGetDiskSpace(pathname, &total, &free) )
        {
            wxPuts(_T("ERROR: wxGetDiskSpace failed."));
        }
        else
        {
            wxPrintf(_T("%sKb total, %sKb free on '%s'.\n"),
                    (total / 1024).ToString().c_str(),
                    (free / 1024).ToString().c_str(),
                    pathname);
        }
    }
}
#endif // TEST_INTERACTIVE

static void TestOsInfo()
{
    wxPuts(_T("*** Testing OS info functions ***\n"));

    int major, minor;
    wxGetOsVersion(&major, &minor);
    wxPrintf(_T("Running under: %s, version %d.%d\n"),
            wxGetOsDescription().c_str(), major, minor);

    wxPrintf(_T("%ld free bytes of memory left.\n"), wxGetFreeMemory().ToLong());

    wxPrintf(_T("Host name is %s (%s).\n"),
           wxGetHostName().c_str(), wxGetFullHostName().c_str());

    wxPuts(wxEmptyString);
}

static void TestPlatformInfo()
{
    wxPuts(_T("*** Testing wxPlatformInfo functions ***\n"));

    // get this platform
    wxPlatformInfo plat;

    wxPrintf(_T("Operating system family name is: %s\n"), plat.GetOperatingSystemFamilyName().c_str());
    wxPrintf(_T("Operating system name is: %s\n"), plat.GetOperatingSystemIdName().c_str());
    wxPrintf(_T("Port ID name is: %s\n"), plat.GetPortIdName().c_str());
    wxPrintf(_T("Port ID short name is: %s\n"), plat.GetPortIdShortName().c_str());
    wxPrintf(_T("Architecture is: %s\n"), plat.GetArchName().c_str());
    wxPrintf(_T("Endianness is: %s\n"), plat.GetEndiannessName().c_str());

    wxPuts(wxEmptyString);
}

static void TestUserInfo()
{
    wxPuts(_T("*** Testing user info functions ***\n"));

    wxPrintf(_T("User id is:\t%s\n"), wxGetUserId().c_str());
    wxPrintf(_T("User name is:\t%s\n"), wxGetUserName().c_str());
    wxPrintf(_T("Home dir is:\t%s\n"), wxGetHomeDir().c_str());
    wxPrintf(_T("Email address:\t%s\n"), wxGetEmailAddress().c_str());

    wxPuts(wxEmptyString);
}

#endif // TEST_INFO_FUNCTIONS

// ----------------------------------------------------------------------------
// path list
// ----------------------------------------------------------------------------

#ifdef TEST_PATHLIST

#ifdef __UNIX__
    #define CMD_IN_PATH _T("ls")
#else
    #define CMD_IN_PATH _T("command.com")
#endif

static void TestPathList()
{
    wxPuts(_T("*** Testing wxPathList ***\n"));

    wxPathList pathlist;
    pathlist.AddEnvList(_T("PATH"));
    wxString path = pathlist.FindValidPath(CMD_IN_PATH);
    if ( path.empty() )
    {
        wxPrintf(_T("ERROR: command not found in the path.\n"));
    }
    else
    {
        wxPrintf(_T("Command found in the path as '%s'.\n"), path.c_str());
    }
}

#endif // TEST_PATHLIST

// ----------------------------------------------------------------------------
// regular expressions
// ----------------------------------------------------------------------------

#ifdef TEST_REGEX

#include "wx/regex.h"

static void TestRegExInteractive()
{
    wxPuts(_T("*** Testing RE interactively ***"));

    for ( ;; )
    {
        wxChar pattern[128];
        wxPrintf(_T("\nEnter a pattern: "));
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
            wxPrintf(_T("Enter text to match: "));
            if ( !wxFgets(text, WXSIZEOF(text), stdin) )
                break;

            // kill the last '\n'
            text[wxStrlen(text) - 1] = 0;

            if ( !re.Matches(text) )
            {
                wxPrintf(_T("No match.\n"));
            }
            else
            {
                wxPrintf(_T("Pattern matches at '%s'\n"), re.GetMatch(text).c_str());

                size_t start, len;
                for ( size_t n = 1; ; n++ )
                {
                    if ( !re.GetMatch(&start, &len, n) )
                    {
                        break;
                    }

                    wxPrintf(_T("Subexpr %u matched '%s'\n"),
                             n, wxString(text + start, len).c_str());
                }
            }
        }
    }
}

#endif // TEST_REGEX

// ----------------------------------------------------------------------------
// database
// ----------------------------------------------------------------------------

#if !wxUSE_ODBC
    #undef TEST_ODBC
#endif

#ifdef TEST_ODBC

#include "wx/db.h"

static void TestDbOpen()
{
    HENV henv;
    wxDb db(henv);
}

#endif // TEST_ODBC

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

    // NB: do _not_ use ATTRIBUTE_PRINTF here, we have some invalid formats
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
  (void) wxPrintf(_T("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 0x12);
  (void) wxPrintf(_T("'\n"));
}

static void
fmtst1chk (const wxChar *fmt)
{
  (void) wxPrintf(_T("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 4, 0x12);
  (void) wxPrintf(_T("'\n"));
}

static void
fmtst2chk (const wxChar *fmt)
{
  (void) wxPrintf(_T("%s:\t`"), fmt);
  (void) wxPrintf(fmt, 4, 4, 0x12);
  (void) wxPrintf(_T("'\n"));
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

  wxPuts(_T("\nFormatted output test"));
  wxPrintf(_T("prefix  6d      6o      6x      6X      6u\n"));
  wxStrcpy(prefix, _T("%"));
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      for (k = 0; k < 2; k++) {
        for (l = 0; l < 2; l++) {
          wxStrcpy(prefix, _T("%"));
          if (i == 0) wxStrcat(prefix, _T("-"));
          if (j == 0) wxStrcat(prefix, _T("+"));
          if (k == 0) wxStrcat(prefix, _T("#"));
          if (l == 0) wxStrcat(prefix, _T("0"));
          wxPrintf(_T("%5s |"), prefix);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, _T("6d |"));
          wxPrintf(tp, DEC);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, _T("6o |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, _T("6x |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, _T("6X |"));
          wxPrintf(tp, INT);
          wxStrcpy(tp, prefix);
          wxStrcat(tp, _T("6u |"));
          wxPrintf(tp, UNS);
          wxPrintf(_T("\n"));
        }
      }
    }
  }
  wxPrintf(_T("%10s\n"), PointerNull);
  wxPrintf(_T("%-10s\n"), PointerNull);
}

static void TestPrintf()
{
  static wxChar shortstr[] = _T("Hi, Z.");
  static wxChar longstr[] = _T("Good morning, Doctor Chandra.  This is Hal.  \
I am ready for my first lesson today.");
  int result = 0;
  wxString test_format;

  fmtchk(_T("%.4x"));
  fmtchk(_T("%04x"));
  fmtchk(_T("%4.4x"));
  fmtchk(_T("%04.4x"));
  fmtchk(_T("%4.3x"));
  fmtchk(_T("%04.3x"));

  fmtst1chk(_T("%.*x"));
  fmtst1chk(_T("%0*x"));
  fmtst2chk(_T("%*.*x"));
  fmtst2chk(_T("%0*.*x"));

  wxString bad_format = _T("bad format:\t\"%b\"\n");
  wxPrintf(bad_format.c_str());
  wxPrintf(_T("nil pointer (padded):\t\"%10p\"\n"), (void *) NULL);

  wxPrintf(_T("decimal negative:\t\"%d\"\n"), -2345);
  wxPrintf(_T("octal negative:\t\"%o\"\n"), -2345);
  wxPrintf(_T("hex negative:\t\"%x\"\n"), -2345);
  wxPrintf(_T("long decimal number:\t\"%ld\"\n"), -123456L);
  wxPrintf(_T("long octal negative:\t\"%lo\"\n"), -2345L);
  wxPrintf(_T("long unsigned decimal number:\t\"%lu\"\n"), -123456L);
  wxPrintf(_T("zero-padded LDN:\t\"%010ld\"\n"), -123456L);
  test_format = _T("left-adjusted ZLDN:\t\"%-010ld\"\n");
  wxPrintf(test_format.c_str(), -123456);
  wxPrintf(_T("space-padded LDN:\t\"%10ld\"\n"), -123456L);
  wxPrintf(_T("left-adjusted SLDN:\t\"%-10ld\"\n"), -123456L);

  test_format = _T("zero-padded string:\t\"%010s\"\n");
  wxPrintf(test_format.c_str(), shortstr);
  test_format = _T("left-adjusted Z string:\t\"%-010s\"\n");
  wxPrintf(test_format.c_str(), shortstr);
  wxPrintf(_T("space-padded string:\t\"%10s\"\n"), shortstr);
  wxPrintf(_T("left-adjusted S string:\t\"%-10s\"\n"), shortstr);
  wxPrintf(_T("null string:\t\"%s\"\n"), PointerNull);
  wxPrintf(_T("limited string:\t\"%.22s\"\n"), longstr);

  wxPrintf(_T("e-style >= 1:\t\"%e\"\n"), 12.34);
  wxPrintf(_T("e-style >= .1:\t\"%e\"\n"), 0.1234);
  wxPrintf(_T("e-style < .1:\t\"%e\"\n"), 0.001234);
  wxPrintf(_T("e-style big:\t\"%.60e\"\n"), 1e20);
  wxPrintf(_T("e-style == .1:\t\"%e\"\n"), 0.1);
  wxPrintf(_T("f-style >= 1:\t\"%f\"\n"), 12.34);
  wxPrintf(_T("f-style >= .1:\t\"%f\"\n"), 0.1234);
  wxPrintf(_T("f-style < .1:\t\"%f\"\n"), 0.001234);
  wxPrintf(_T("g-style >= 1:\t\"%g\"\n"), 12.34);
  wxPrintf(_T("g-style >= .1:\t\"%g\"\n"), 0.1234);
  wxPrintf(_T("g-style < .1:\t\"%g\"\n"), 0.001234);
  wxPrintf(_T("g-style big:\t\"%.60g\"\n"), 1e20);

  wxPrintf (_T(" %6.5f\n"), .099999999860301614);
  wxPrintf (_T(" %6.5f\n"), .1);
  wxPrintf (_T("x%5.4fx\n"), .5);

  wxPrintf (_T("%#03x\n"), 1);

  //wxPrintf (_T("something really insane: %.10000f\n"), 1.0);

  {
    double d = FLT_MIN;
    int niter = 17;

    while (niter-- != 0)
      wxPrintf (_T("%.17e\n"), d / 2);
    fflush (stdout);
  }

#ifndef __WATCOMC__
  // Open Watcom cause compiler error here
  // Error! E173: col(24) floating-point constant too small to represent
  wxPrintf (_T("%15.5e\n"), 4.9406564584124654e-324);
#endif

#define FORMAT _T("|%12.4f|%12.4e|%12.4g|\n")
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
    int rc = wxSnprintf (buf, WXSIZEOF(buf), _T("%30s"), _T("foo"));

    wxPrintf(_T("snprintf (\"%%30s\", \"foo\") == %d, \"%.*s\"\n"),
             rc, WXSIZEOF(buf), buf);
#if 0
    wxChar buf2[512];
    wxPrintf ("snprintf (\"%%.999999u\", 10)\n",
            wxSnprintf(buf2, WXSIZEOFbuf2), "%.999999u", 10));
#endif
  }

  fp_test ();

  wxPrintf (_T("%e should be 1.234568e+06\n"), 1234567.8);
  wxPrintf (_T("%f should be 1234567.800000\n"), 1234567.8);
  wxPrintf (_T("%g should be 1.23457e+06\n"), 1234567.8);
  wxPrintf (_T("%g should be 123.456\n"), 123.456);
  wxPrintf (_T("%g should be 1e+06\n"), 1000000.0);
  wxPrintf (_T("%g should be 10\n"), 10.0);
  wxPrintf (_T("%g should be 0.02\n"), 0.02);

  {
    double x=1.0;
    wxPrintf(_T("%.17f\n"),(1.0/x/10.0+1.0)*x-x);
  }

  {
    wxChar buf[200];

    wxSprintf(buf,_T("%*s%*s%*s"),-1,_T("one"),-20,_T("two"),-30,_T("three"));

    result |= wxStrcmp (buf,
                      _T("onetwo                 three                         "));

    wxPuts (result != 0 ? _T("Test failed!") : _T("Test ok."));
  }

#ifdef wxLongLong_t
  {
      wxChar buf[200];

      wxSprintf(buf, _T("%07") wxLongLongFmtSpec _T("o"), wxLL(040000000000));
      #if 0
        // for some reason below line fails under Borland
      wxPrintf (_T("sprintf (buf, \"%%07Lo\", 040000000000ll) = %s"), buf);
      #endif

      if (wxStrcmp (buf, _T("40000000000")) != 0)
      {
          result = 1;
          wxPuts (_T("\tFAILED"));
      }
      wxUnusedVar(result);
      wxPuts (wxEmptyString);
  }
#endif // wxLongLong_t

  wxPrintf (_T("printf (\"%%hhu\", %u) = %hhu\n"), UCHAR_MAX + 2, UCHAR_MAX + 2);
  wxPrintf (_T("printf (\"%%hu\", %u) = %hu\n"), USHRT_MAX + 2, USHRT_MAX + 2);

  wxPuts (_T("--- Should be no further output. ---"));
  rfg1 ();
  rfg2 ();

#if 0
  {
    wxChar bytes[7];
    wxChar buf[20];

    memset (bytes, '\xff', sizeof bytes);
    wxSprintf (buf, _T("foo%hhn\n"), &bytes[3]);
    if (bytes[0] != '\xff' || bytes[1] != '\xff' || bytes[2] != '\xff'
        || bytes[4] != '\xff' || bytes[5] != '\xff' || bytes[6] != '\xff')
      {
        wxPuts (_T("%hhn overwrite more bytes"));
        result = 1;
      }
    if (bytes[3] != 3)
      {
        wxPuts (_T("%hhn wrote incorrect value"));
        result = 1;
      }
  }
#endif
}

static void
rfg1 (void)
{
  wxChar buf[100];

  wxSprintf (buf, _T("%5.s"), _T("xyz"));
  if (wxStrcmp (buf, _T("     ")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("     "));
  wxSprintf (buf, _T("%5.f"), 33.3);
  if (wxStrcmp (buf, _T("   33")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("   33"));
  wxSprintf (buf, _T("%8.e"), 33.3e7);
  if (wxStrcmp (buf, _T("   3e+08")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("   3e+08"));
  wxSprintf (buf, _T("%8.E"), 33.3e7);
  if (wxStrcmp (buf, _T("   3E+08")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("   3E+08"));
  wxSprintf (buf, _T("%.g"), 33.3);
  if (wxStrcmp (buf, _T("3e+01")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("3e+01"));
  wxSprintf (buf, _T("%.G"), 33.3);
  if (wxStrcmp (buf, _T("3E+01")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("3E+01"));
}

static void
rfg2 (void)
{
  int prec;
  wxChar buf[100];
  wxString test_format;

  prec = 0;
  wxSprintf (buf, _T("%.*g"), prec, 3.3);
  if (wxStrcmp (buf, _T("3")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("3"));
  prec = 0;
  wxSprintf (buf, _T("%.*G"), prec, 3.3);
  if (wxStrcmp (buf, _T("3")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("3"));
  prec = 0;
  wxSprintf (buf, _T("%7.*G"), prec, 3.33);
  if (wxStrcmp (buf, _T("      3")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("      3"));
  prec = 3;
  test_format = _T("%04.*o");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, _T(" 041")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T(" 041"));
  prec = 7;
  test_format = _T("%09.*u");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, _T("  0000033")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("  0000033"));
  prec = 3;
  test_format = _T("%04.*x");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, _T(" 021")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T(" 021"));
  prec = 3;
  test_format = _T("%04.*X");
  wxSprintf (buf, test_format.c_str(), prec, 33);
  if (wxStrcmp (buf, _T(" 021")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T(" 021"));
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
    wxConfig *config = new wxConfig(_T("myapp"));
    config->SetPath(_T("/group1"));
    config->Write(_T("entry1"), _T("foo"));
    config->SetPath(_T("/group2"));
    config->Write(_T("entry1"), _T("bar"));
}
#endif

static void TestRegConfRead()
{
    wxConfig *config = new wxConfig(_T("myapp"));

    wxString str;
    long dummy;
    config->SetPath(_T("/"));
    wxPuts(_T("Enumerating / subgroups:"));
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
    _T("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\CrashControl");

static void TestRegistryRead()
{
    wxPuts(_T("*** testing registry reading ***"));

    wxRegKey key(TESTKEY);
    wxPrintf(_T("The test key name is '%s'.\n"), key.GetName().c_str());
    if ( !key.Open() )
    {
        wxPuts(_T("ERROR: test key can't be opened, aborting test."));

        return;
    }

    size_t nSubKeys, nValues;
    if ( key.GetKeyInfo(&nSubKeys, NULL, &nValues, NULL) )
    {
        wxPrintf(_T("It has %u subkeys and %u values.\n"), nSubKeys, nValues);
    }

    wxPrintf(_T("Enumerating values:\n"));

    long dummy;
    wxString value;
    bool cont = key.GetFirstValue(value, dummy);
    while ( cont )
    {
        wxPrintf(_T("Value '%s': type "), value.c_str());
        switch ( key.GetValueType(value) )
        {
            case wxRegKey::Type_None:   wxPrintf(_T("ERROR (none)")); break;
            case wxRegKey::Type_String: wxPrintf(_T("SZ")); break;
            case wxRegKey::Type_Expand_String: wxPrintf(_T("EXPAND_SZ")); break;
            case wxRegKey::Type_Binary: wxPrintf(_T("BINARY")); break;
            case wxRegKey::Type_Dword: wxPrintf(_T("DWORD")); break;
            case wxRegKey::Type_Multi_String: wxPrintf(_T("MULTI_SZ")); break;
            default: wxPrintf(_T("other (unknown)")); break;
        }

        wxPrintf(_T(", value = "));
        if ( key.IsNumericValue(value) )
        {
            long val;
            key.QueryValue(value, &val);
            wxPrintf(_T("%ld"), val);
        }
        else // string
        {
            wxString val;
            key.QueryValue(value, val);
            wxPrintf(_T("'%s'"), val.c_str());

            key.QueryRawValue(value, val);
            wxPrintf(_T(" (raw value '%s')"), val.c_str());
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

    key.SetName(_T("HKEY_CLASSES_ROOT\\.ddf") );
    key.Create();
    key = _T("ddxf_auto_file") ;
    key.SetName(_T("HKEY_CLASSES_ROOT\\.flo") );
    key.Create();
    key = _T("ddxf_auto_file") ;
    key.SetName(_T("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon"));
    key.Create();
    key = _T("program,0") ;
    key.SetName(_T("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command"));
    key.Create();
    key = _T("program \"%1\"") ;

    key.SetName(_T("HKEY_CLASSES_ROOT\\.ddf") );
    key.DeleteSelf();
    key.SetName(_T("HKEY_CLASSES_ROOT\\.flo") );
    key.DeleteSelf();
    key.SetName(_T("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon"));
    key.DeleteSelf();
    key.SetName(_T("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command"));
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
    wxPuts(_T("*** Testing wxSocketServer ***\n"));

    static const int PORT = 3000;

    wxIPV4address addr;
    addr.Service(PORT);

    wxSocketServer *server = new wxSocketServer(addr);
    if ( !server->Ok() )
    {
        wxPuts(_T("ERROR: failed to bind"));

        return;
    }

    bool quit = false;
    while ( !quit )
    {
        wxPrintf(_T("Server: waiting for connection on port %d...\n"), PORT);

        wxSocketBase *socket = server->Accept();
        if ( !socket )
        {
            wxPuts(_T("ERROR: wxSocketServer::Accept() failed."));
            break;
        }

        wxPuts(_T("Server: got a client."));

        server->SetTimeout(60); // 1 min

        bool close = false;
        while ( !close && socket->IsConnected() )
        {
            wxString s;
            wxChar ch = _T('\0');
            for ( ;; )
            {
                if ( socket->Read(&ch, sizeof(ch)).Error() )
                {
                    // don't log error if the client just close the connection
                    if ( socket->IsConnected() )
                    {
                        wxPuts(_T("ERROR: in wxSocket::Read."));
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

            wxPrintf(_T("Server: got '%s'.\n"), s.c_str());
            if ( s == _T("close") )
            {
                wxPuts(_T("Closing connection"));

                close = true;
            }
            else if ( s == _T("quit") )
            {
                close =
                quit = true;

                wxPuts(_T("Shutting down the server"));
            }
            else // not a special command
            {
                socket->Write(s.MakeUpper().c_str(), s.length());
                socket->Write("\r\n", 2);
                wxPrintf(_T("Server: wrote '%s'.\n"), s.c_str());
            }
        }

        if ( !close )
        {
            wxPuts(_T("Server: lost a client unexpectedly."));
        }

        socket->Destroy();
    }

    // same as "delete server" but is consistent with GUI programs
    server->Destroy();
}

static void TestSocketClient()
{
    wxPuts(_T("*** Testing wxSocketClient ***\n"));

    static const wxChar *hostname = _T("www.wxwidgets.org");

    wxIPV4address addr;
    addr.Hostname(hostname);
    addr.Service(80);

    wxPrintf(_T("--- Attempting to connect to %s:80...\n"), hostname);

    wxSocketClient client;
    if ( !client.Connect(addr) )
    {
        wxPrintf(_T("ERROR: failed to connect to %s\n"), hostname);
    }
    else
    {
        wxPrintf(_T("--- Connected to %s:%u...\n"),
               addr.Hostname().c_str(), addr.Service());

        wxChar buf[8192];

        // could use simply "GET" here I suppose
        wxString cmdGet =
            wxString::Format(_T("GET http://%s/\r\n"), hostname);
        client.Write(cmdGet, cmdGet.length());
        wxPrintf(_T("--- Sent command '%s' to the server\n"),
               MakePrintable(cmdGet).c_str());
        client.Read(buf, WXSIZEOF(buf));
        wxPrintf(_T("--- Server replied:\n%s"), buf);
    }
}

#endif // TEST_SOCKETS

// ----------------------------------------------------------------------------
// FTP
// ----------------------------------------------------------------------------

#ifdef TEST_FTP

#include "wx/protocol/ftp.h"

static wxFTP ftp;

#define FTP_ANONYMOUS

#ifdef FTP_ANONYMOUS
    static const wxChar *directory = _T("/pub");
    static const wxChar *filename = _T("welcome.msg");
#else
    static const wxChar *directory = _T("/etc");
    static const wxChar *filename = _T("issue");
#endif

static bool TestFtpConnect()
{
    wxPuts(_T("*** Testing FTP connect ***"));

#ifdef FTP_ANONYMOUS
    static const wxChar *hostname = _T("ftp.wxwidgets.org");

    wxPrintf(_T("--- Attempting to connect to %s:21 anonymously...\n"), hostname);
#else // !FTP_ANONYMOUS
    static const wxChar *hostname = "localhost";

    wxChar user[256];
    wxFgets(user, WXSIZEOF(user), stdin);
    user[wxStrlen(user) - 1] = '\0'; // chop off '\n'
    ftp.SetUser(user);

    wxChar password[256];
    wxPrintf(_T("Password for %s: "), password);
    wxFgets(password, WXSIZEOF(password), stdin);
    password[wxStrlen(password) - 1] = '\0'; // chop off '\n'
    ftp.SetPassword(password);

    wxPrintf(_T("--- Attempting to connect to %s:21 as %s...\n"), hostname, user);
#endif // FTP_ANONYMOUS/!FTP_ANONYMOUS

    if ( !ftp.Connect(hostname) )
    {
        wxPrintf(_T("ERROR: failed to connect to %s\n"), hostname);

        return false;
    }
    else
    {
        wxPrintf(_T("--- Connected to %s, current directory is '%s'\n"),
                 hostname, ftp.Pwd().c_str());
        ftp.Close();
    }

    return true;
}

// test (fixed?) wxFTP bug with wu-ftpd >= 2.6.0?
static void TestFtpWuFtpd()
{
    wxFTP ftp;
    static const wxChar *hostname = _T("ftp.eudora.com");
    if ( !ftp.Connect(hostname) )
    {
        wxPrintf(_T("ERROR: failed to connect to %s\n"), hostname);
    }
    else
    {
        static const wxChar *filename = _T("eudora/pubs/draft-gellens-submit-09.txt");
        wxInputStream *in = ftp.GetInputStream(filename);
        if ( !in )
        {
            wxPrintf(_T("ERROR: couldn't get input stream for %s\n"), filename);
        }
        else
        {
            size_t size = in->GetSize();
            wxPrintf(_T("Reading file %s (%u bytes)..."), filename, size);

            wxChar *data = new wxChar[size];
            if ( !in->Read(data, size) )
            {
                wxPuts(_T("ERROR: read error"));
            }
            else
            {
                wxPrintf(_T("Successfully retrieved the file.\n"));
            }

            delete [] data;
            delete in;
        }
    }
}

static void TestFtpList()
{
    wxPuts(_T("*** Testing wxFTP file listing ***\n"));

    // test CWD
    if ( !ftp.ChDir(directory) )
    {
        wxPrintf(_T("ERROR: failed to cd to %s\n"), directory);
    }

    wxPrintf(_T("Current directory is '%s'\n"), ftp.Pwd().c_str());

    // test NLIST and LIST
    wxArrayString files;
    if ( !ftp.GetFilesList(files) )
    {
        wxPuts(_T("ERROR: failed to get NLIST of files"));
    }
    else
    {
        wxPrintf(_T("Brief list of files under '%s':\n"), ftp.Pwd().c_str());
        size_t count = files.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxPrintf(_T("\t%s\n"), files[n].c_str());
        }
        wxPuts(_T("End of the file list"));
    }

    if ( !ftp.GetDirList(files) )
    {
        wxPuts(_T("ERROR: failed to get LIST of files"));
    }
    else
    {
        wxPrintf(_T("Detailed list of files under '%s':\n"), ftp.Pwd().c_str());
        size_t count = files.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxPrintf(_T("\t%s\n"), files[n].c_str());
        }
        wxPuts(_T("End of the file list"));
    }

    if ( !ftp.ChDir(_T("..")) )
    {
        wxPuts(_T("ERROR: failed to cd to .."));
    }

    wxPrintf(_T("Current directory is '%s'\n"), ftp.Pwd().c_str());
}

static void TestFtpDownload()
{
    wxPuts(_T("*** Testing wxFTP download ***\n"));

    // test RETR
    wxInputStream *in = ftp.GetInputStream(filename);
    if ( !in )
    {
        wxPrintf(_T("ERROR: couldn't get input stream for %s\n"), filename);
    }
    else
    {
        size_t size = in->GetSize();
        wxPrintf(_T("Reading file %s (%u bytes)..."), filename, size);
        fflush(stdout);

        wxChar *data = new wxChar[size];
        if ( !in->Read(data, size) )
        {
            wxPuts(_T("ERROR: read error"));
        }
        else
        {
            wxPrintf(_T("\nContents of %s:\n%s\n"), filename, data);
        }

        delete [] data;
        delete in;
    }
}

static void TestFtpFileSize()
{
    wxPuts(_T("*** Testing FTP SIZE command ***"));

    if ( !ftp.ChDir(directory) )
    {
        wxPrintf(_T("ERROR: failed to cd to %s\n"), directory);
    }

    wxPrintf(_T("Current directory is '%s'\n"), ftp.Pwd().c_str());

    if ( ftp.FileExists(filename) )
    {
        int size = ftp.GetFileSize(filename);
        if ( size == -1 )
            wxPrintf(_T("ERROR: couldn't get size of '%s'\n"), filename);
        else
            wxPrintf(_T("Size of '%s' is %d bytes.\n"), filename, size);
    }
    else
    {
        wxPrintf(_T("ERROR: '%s' doesn't exist\n"), filename);
    }
}

static void TestFtpMisc()
{
    wxPuts(_T("*** Testing miscellaneous wxFTP functions ***"));

    if ( ftp.SendCommand(_T("STAT")) != '2' )
    {
        wxPuts(_T("ERROR: STAT failed"));
    }
    else
    {
        wxPrintf(_T("STAT returned:\n\n%s\n"), ftp.GetLastResult().c_str());
    }

    if ( ftp.SendCommand(_T("HELP SITE")) != '2' )
    {
        wxPuts(_T("ERROR: HELP SITE failed"));
    }
    else
    {
        wxPrintf(_T("The list of site-specific commands:\n\n%s\n"),
               ftp.GetLastResult().c_str());
    }
}

static void TestFtpInteractive()
{
    wxPuts(_T("\n*** Interactive wxFTP test ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(_T("Enter FTP command: "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

        // special handling of LIST and NLST as they require data connection
        wxString start(buf, 4);
        start.MakeUpper();
        if ( start == _T("LIST") || start == _T("NLST") )
        {
            wxString wildcard;
            if ( wxStrlen(buf) > 4 )
                wildcard = buf + 5;

            wxArrayString files;
            if ( !ftp.GetList(files, wildcard, start == _T("LIST")) )
            {
                wxPrintf(_T("ERROR: failed to get %s of files\n"), start.c_str());
            }
            else
            {
                wxPrintf(_T("--- %s of '%s' under '%s':\n"),
                       start.c_str(), wildcard.c_str(), ftp.Pwd().c_str());
                size_t count = files.GetCount();
                for ( size_t n = 0; n < count; n++ )
                {
                    wxPrintf(_T("\t%s\n"), files[n].c_str());
                }
                wxPuts(_T("--- End of the file list"));
            }
        }
        else // !list
        {
            wxChar ch = ftp.SendCommand(buf);
            wxPrintf(_T("Command %s"), ch ? _T("succeeded") : _T("failed"));
            if ( ch )
            {
                wxPrintf(_T(" (return code %c)"), ch);
            }

            wxPrintf(_T(", server reply:\n%s\n\n"), ftp.GetLastResult().c_str());
        }
    }

    wxPuts(_T("\n*** done ***"));
}

static void TestFtpUpload()
{
    wxPuts(_T("*** Testing wxFTP uploading ***\n"));

    // upload a file
    static const wxChar *file1 = _T("test1");
    static const wxChar *file2 = _T("test2");
    wxOutputStream *out = ftp.GetOutputStream(file1);
    if ( out )
    {
        wxPrintf(_T("--- Uploading to %s ---\n"), file1);
        out->Write("First hello", 11);
        delete out;
    }

    // send a command to check the remote file
    if ( ftp.SendCommand(wxString(_T("STAT ")) + file1) != '2' )
    {
        wxPrintf(_T("ERROR: STAT %s failed\n"), file1);
    }
    else
    {
        wxPrintf(_T("STAT %s returned:\n\n%s\n"),
               file1, ftp.GetLastResult().c_str());
    }

    out = ftp.GetOutputStream(file2);
    if ( out )
    {
        wxPrintf(_T("--- Uploading to %s ---\n"), file1);
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

    virtual void Walk(size_t skip = 1)
    {
        wxPuts(_T("Stack dump:"));

        wxStackWalker::Walk(skip);
    }

protected:
    virtual void OnStackFrame(const wxStackFrame& frame)
    {
        printf("[%2d] ", frame.GetLevel());

        wxString name = frame.GetName();
        if ( !name.empty() )
        {
            printf("%-20.40s", name.mb_str());
        }
        else
        {
            printf("0x%08lx", (unsigned long)frame.GetAddress());
        }

        if ( frame.HasSourceLocation() )
        {
            printf("\t%s:%d",
                   frame.GetFileName().mb_str(),
                   frame.GetLine());
        }

        puts("");

        wxString type, val;
        for ( size_t n = 0; frame.GetParam(n, &type, &name, &val); n++ )
        {
            printf("\t%s %s = %s\n", type.mb_str(), name.mb_str(), val.mb_str());
        }
    }
};

static void TestStackWalk(const char *argv0)
{
    wxPuts(_T("*** Testing wxStackWalker ***\n"));

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

static void TestStandardPaths()
{
    wxPuts(_T("*** Testing wxStandardPaths ***\n"));

    wxTheApp->SetAppName(_T("console"));

    wxStandardPathsBase& stdp = wxStandardPaths::Get();
    wxPrintf(_T("Config dir (sys):\t%s\n"), stdp.GetConfigDir().c_str());
    wxPrintf(_T("Config dir (user):\t%s\n"), stdp.GetUserConfigDir().c_str());
    wxPrintf(_T("Data dir (sys):\t\t%s\n"), stdp.GetDataDir().c_str());
    wxPrintf(_T("Data dir (sys local):\t%s\n"), stdp.GetLocalDataDir().c_str());
    wxPrintf(_T("Data dir (user):\t%s\n"), stdp.GetUserDataDir().c_str());
    wxPrintf(_T("Data dir (user local):\t%s\n"), stdp.GetUserLocalDataDir().c_str());
    wxPrintf(_T("Documents dir:\t\t%s\n"), stdp.GetDocumentsDir().c_str());
    wxPrintf(_T("Plugins dir:\t\t%s\n"), stdp.GetPluginsDir().c_str());
    wxPrintf(_T("Resources dir:\t\t%s\n"), stdp.GetResourcesDir().c_str());
    wxPrintf(_T("Localized res. dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir(_T("fr")).c_str());
    wxPrintf(_T("Message catalogs dir:\t%s\n"),
             stdp.GetLocalizedResourcesDir
                  (
                    _T("fr"),
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
    wxPuts(_T("*** Testing wxFileInputStream ***"));

    static const wxString filename = _T("testdata.fs");
    {
        wxFileOutputStream fsOut(filename);
        fsOut.Write("foo", 3);
    }

    {
        wxFileInputStream fsIn(filename);
        wxPrintf(_T("File stream size: %u\n"), fsIn.GetSize());
        while ( !fsIn.Eof() )
        {
            wxPutchar(fsIn.GetC());
        }
    }

    if ( !wxRemoveFile(filename) )
    {
        wxPrintf(_T("ERROR: failed to remove the file '%s'.\n"), filename.c_str());
    }

    wxPuts(_T("\n*** wxFileInputStream test done ***"));
}

static void TestMemoryStream()
{
    wxPuts(_T("*** Testing wxMemoryOutputStream ***"));

    wxMemoryOutputStream memOutStream;
    wxPrintf(_T("Initially out stream offset: %lu\n"),
             (unsigned long)memOutStream.TellO());

    for ( const wxChar *p = _T("Hello, stream!"); *p; p++ )
    {
        memOutStream.PutC(*p);
    }

    wxPrintf(_T("Final out stream offset: %lu\n"),
             (unsigned long)memOutStream.TellO());

    wxPuts(_T("*** Testing wxMemoryInputStream ***"));

    wxChar buf[1024];
    size_t len = memOutStream.CopyTo(buf, WXSIZEOF(buf));

    wxMemoryInputStream memInpStream(buf, len);
    wxPrintf(_T("Memory stream size: %u\n"), memInpStream.GetSize());
    while ( !memInpStream.Eof() )
    {
        wxPutchar(memInpStream.GetC());
    }

    wxPuts(_T("\n*** wxMemoryInputStream test done ***"));
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
    wxPuts(_T("*** Testing wxStopWatch ***\n"));

    wxStopWatch sw;
    sw.Pause();
    wxPrintf(_T("Initially paused, after 2 seconds time is..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(_T("\t%ldms\n"), sw.Time());

    wxPrintf(_T("Resuming stopwatch and sleeping 3 seconds..."));
    fflush(stdout);
    sw.Resume();
    wxSleep(3);
    wxPrintf(_T("\telapsed time: %ldms\n"), sw.Time());

    sw.Pause();
    wxPrintf(_T("Pausing agan and sleeping 2 more seconds..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(_T("\telapsed time: %ldms\n"), sw.Time());

    sw.Resume();
    wxPrintf(_T("Finally resuming and sleeping 2 more seconds..."));
    fflush(stdout);
    wxSleep(2);
    wxPrintf(_T("\telapsed time: %ldms\n"), sw.Time());

    wxStopWatch sw2;
    wxPuts(_T("\nChecking for 'backwards clock' bug..."));
    for ( size_t n = 0; n < 70; n++ )
    {
        sw2.Start();

        for ( size_t m = 0; m < 100000; m++ )
        {
            if ( sw.Time() < 0 || sw2.Time() < 0 )
            {
                wxPuts(_T("\ntime is negative - ERROR!"));
            }
        }

        wxPutchar('.');
        fflush(stdout);
    }

    wxPuts(_T(", ok."));
}

#endif // TEST_TIMER

// ----------------------------------------------------------------------------
// vCard support
// ----------------------------------------------------------------------------

#ifdef TEST_VCARD

#include "wx/vcard.h"

static void DumpVObject(size_t level, const wxVCardObject& vcard)
{
    void *cookie;
    wxVCardObject *vcObj = vcard.GetFirstProp(&cookie);
    while ( vcObj )
    {
        wxPrintf(_T("%s%s"),
               wxString(_T('\t'), level).c_str(),
               vcObj->GetName().c_str());

        wxString value;
        switch ( vcObj->GetType() )
        {
            case wxVCardObject::String:
            case wxVCardObject::UString:
                {
                    wxString val;
                    vcObj->GetValue(&val);
                    value << _T('"') << val << _T('"');
                }
                break;

            case wxVCardObject::Int:
                {
                    unsigned int i;
                    vcObj->GetValue(&i);
                    value.Printf(_T("%u"), i);
                }
                break;

            case wxVCardObject::Long:
                {
                    unsigned long l;
                    vcObj->GetValue(&l);
                    value.Printf(_T("%lu"), l);
                }
                break;

            case wxVCardObject::None:
                break;

            case wxVCardObject::Object:
                value = _T("<node>");
                break;

            default:
                value = _T("<unknown value type>");
        }

        if ( !!value )
            wxPrintf(_T(" = %s"), value.c_str());
        wxPutchar('\n');

        DumpVObject(level + 1, *vcObj);

        delete vcObj;
        vcObj = vcard.GetNextProp(&cookie);
    }
}

static void DumpVCardAddresses(const wxVCard& vcard)
{
    wxPuts(_T("\nShowing all addresses from vCard:\n"));

    size_t nAdr = 0;
    void *cookie;
    wxVCardAddress *addr = vcard.GetFirstAddress(&cookie);
    while ( addr )
    {
        wxString flagsStr;
        int flags = addr->GetFlags();
        if ( flags & wxVCardAddress::Domestic )
        {
            flagsStr << _T("domestic ");
        }
        if ( flags & wxVCardAddress::Intl )
        {
            flagsStr << _T("international ");
        }
        if ( flags & wxVCardAddress::Postal )
        {
            flagsStr << _T("postal ");
        }
        if ( flags & wxVCardAddress::Parcel )
        {
            flagsStr << _T("parcel ");
        }
        if ( flags & wxVCardAddress::Home )
        {
            flagsStr << _T("home ");
        }
        if ( flags & wxVCardAddress::Work )
        {
            flagsStr << _T("work ");
        }

        wxPrintf(_T("Address %u:\n")
               "\tflags = %s\n"
               "\tvalue = %s;%s;%s;%s;%s;%s;%s\n",
               ++nAdr,
               flagsStr.c_str(),
               addr->GetPostOffice().c_str(),
               addr->GetExtAddress().c_str(),
               addr->GetStreet().c_str(),
               addr->GetLocality().c_str(),
               addr->GetRegion().c_str(),
               addr->GetPostalCode().c_str(),
               addr->GetCountry().c_str()
               );

        delete addr;
        addr = vcard.GetNextAddress(&cookie);
    }
}

static void DumpVCardPhoneNumbers(const wxVCard& vcard)
{
    wxPuts(_T("\nShowing all phone numbers from vCard:\n"));

    size_t nPhone = 0;
    void *cookie;
    wxVCardPhoneNumber *phone = vcard.GetFirstPhoneNumber(&cookie);
    while ( phone )
    {
        wxString flagsStr;
        int flags = phone->GetFlags();
        if ( flags & wxVCardPhoneNumber::Voice )
        {
            flagsStr << _T("voice ");
        }
        if ( flags & wxVCardPhoneNumber::Fax )
        {
            flagsStr << _T("fax ");
        }
        if ( flags & wxVCardPhoneNumber::Cellular )
        {
            flagsStr << _T("cellular ");
        }
        if ( flags & wxVCardPhoneNumber::Modem )
        {
            flagsStr << _T("modem ");
        }
        if ( flags & wxVCardPhoneNumber::Home )
        {
            flagsStr << _T("home ");
        }
        if ( flags & wxVCardPhoneNumber::Work )
        {
            flagsStr << _T("work ");
        }

        wxPrintf(_T("Phone number %u:\n")
               "\tflags = %s\n"
               "\tvalue = %s\n",
               ++nPhone,
               flagsStr.c_str(),
               phone->GetNumber().c_str()
               );

        delete phone;
        phone = vcard.GetNextPhoneNumber(&cookie);
    }
}

static void TestVCardRead()
{
    wxPuts(_T("*** Testing wxVCard reading ***\n"));

    wxVCard vcard(_T("vcard.vcf"));
    if ( !vcard.IsOk() )
    {
        wxPuts(_T("ERROR: couldn't load vCard."));
    }
    else
    {
        // read individual vCard properties
        wxVCardObject *vcObj = vcard.GetProperty("FN");
        wxString value;
        if ( vcObj )
        {
            vcObj->GetValue(&value);
            delete vcObj;
        }
        else
        {
            value = _T("<none>");
        }

        wxPrintf(_T("Full name retrieved directly: %s\n"), value.c_str());


        if ( !vcard.GetFullName(&value) )
        {
            value = _T("<none>");
        }

        wxPrintf(_T("Full name from wxVCard API: %s\n"), value.c_str());

        // now show how to deal with multiply occurring properties
        DumpVCardAddresses(vcard);
        DumpVCardPhoneNumbers(vcard);

        // and finally show all
        wxPuts(_T("\nNow dumping the entire vCard:\n")
             "-----------------------------\n");

        DumpVObject(0, vcard);
    }
}

static void TestVCardWrite()
{
    wxPuts(_T("*** Testing wxVCard writing ***\n"));

    wxVCard vcard;
    if ( !vcard.IsOk() )
    {
        wxPuts(_T("ERROR: couldn't create vCard."));
    }
    else
    {
        // set some fields
        vcard.SetName("Zeitlin", "Vadim");
        vcard.SetFullName("Vadim Zeitlin");
        vcard.SetOrganization("wxWidgets", "R&D");

        // just dump the vCard back
        wxPuts(_T("Entire vCard follows:\n"));
        wxPuts(vcard.Write());
    }
}

#endif // TEST_VCARD

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
    _T("floppy"),
    _T("hard disk"),
    _T("CD-ROM"),
    _T("DVD-ROM"),
    _T("network volume"),
    _T("other volume"),
};

static void TestFSVolume()
{
    wxPuts(_T("*** Testing wxFSVolume class ***"));

    wxArrayString volumes = wxFSVolume::GetVolumes();
    size_t count = volumes.GetCount();

    if ( !count )
    {
        wxPuts(_T("ERROR: no mounted volumes?"));
        return;
    }

    wxPrintf(_T("%u mounted volumes found:\n"), count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxFSVolume vol(volumes[n]);
        if ( !vol.IsOk() )
        {
            wxPuts(_T("ERROR: couldn't create volume"));
            continue;
        }

        wxPrintf(_T("%u: %s (%s), %s, %s, %s\n"),
                 n + 1,
                 vol.GetDisplayName().c_str(),
                 vol.GetName().c_str(),
                 volumeKinds[vol.GetKind()],
                 vol.IsWritable() ? _T("rw") : _T("ro"),
                 vol.GetFlags() & wxFS_VOL_REMOVABLE ? _T("removable")
                                                     : _T("fixed"));
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
    { utf8Invalid, WXSIZEOF(utf8Invalid), _T("iso8859-1"), wxFONTENCODING_ISO8859_1 },
    { utf8koi8r, WXSIZEOF(utf8koi8r), _T("koi8-r"), wxFONTENCODING_KOI8 },
    { utf8iso8859_1, WXSIZEOF(utf8iso8859_1), _T("iso8859-1"), wxFONTENCODING_ISO8859_1 },
};

static void TestUtf8()
{
    wxPuts(_T("*** Testing UTF8 support ***\n"));

    char buf[1024];
    wchar_t wbuf[1024];

    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const Utf8Data& u8d = utf8data[n];
        if ( wxConvUTF8.MB2WC(wbuf, (const char *)u8d.text,
                              WXSIZEOF(wbuf)) == (size_t)-1 )
        {
            wxPuts(_T("ERROR: UTF-8 decoding failed."));
        }
        else
        {
            wxCSConv conv(u8d.charset);
            if ( conv.WC2MB(buf, wbuf, WXSIZEOF(buf)) == (size_t)-1 )
            {
                wxPrintf(_T("ERROR: conversion to %s failed.\n"), u8d.charset);
            }
            else
            {
                wxPrintf(_T("String in %s: %s\n"), u8d.charset, buf);
            }
        }

        wxString s(wxConvUTF8.cMB2WC((const char *)u8d.text));
        if ( s.empty() )
            s = _T("<< conversion failed >>");
        wxPrintf(_T("String in current cset: %s\n"), s.c_str());

    }

    wxPuts(wxEmptyString);
}

static void TestEncodingConverter()
{
    wxPuts(_T("*** Testing wxEncodingConverter ***\n"));

    // using wxEncodingConverter should give the same result as above
    char buf[1024];
    wchar_t wbuf[1024];
    if ( wxConvUTF8.MB2WC(wbuf, (const char *)utf8koi8r,
                          WXSIZEOF(utf8koi8r)) == (size_t)-1 )
    {
        wxPuts(_T("ERROR: UTF-8 decoding failed."));
    }
    else
    {
        wxEncodingConverter ec;
        ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_KOI8);
        ec.Convert(wbuf, buf);
        wxPrintf(_T("The same KOI8-R string using wxEC: %s\n"), buf);
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

static const wxChar *TESTFILE_ZIP = _T("testdata.zip");

static void TestZipStreamRead()
{
    wxPuts(_T("*** Testing ZIP reading ***\n"));

    static const wxString filename = _T("foo");
    wxZipInputStream istr(TESTFILE_ZIP, filename);
    wxPrintf(_T("Archive size: %u\n"), istr.GetSize());

    wxPrintf(_T("Dumping the file '%s':\n"), filename.c_str());
    while ( !istr.Eof() )
    {
        wxPutchar(istr.GetC());
        fflush(stdout);
    }

    wxPuts(_T("\n----- done ------"));
}

static void DumpZipDirectory(wxFileSystem& fs,
                             const wxString& dir,
                             const wxString& indent)
{
    wxString prefix = wxString::Format(_T("%s#zip:%s"),
                                         TESTFILE_ZIP, dir.c_str());
    wxString wildcard = prefix + _T("/*");

    wxString dirname = fs.FindFirst(wildcard, wxDIR);
    while ( !dirname.empty() )
    {
        if ( !dirname.StartsWith(prefix + _T('/'), &dirname) )
        {
            wxPrintf(_T("ERROR: unexpected wxFileSystem::FindNext result\n"));

            break;
        }

        wxPrintf(_T("%s%s\n"), indent.c_str(), dirname.c_str());

        DumpZipDirectory(fs, dirname,
                         indent + wxString(_T(' '), 4));

        dirname = fs.FindNext();
    }

    wxString filename = fs.FindFirst(wildcard, wxFILE);
    while ( !filename.empty() )
    {
        if ( !filename.StartsWith(prefix, &filename) )
        {
            wxPrintf(_T("ERROR: unexpected wxFileSystem::FindNext result\n"));

            break;
        }

        wxPrintf(_T("%s%s\n"), indent.c_str(), filename.c_str());

        filename = fs.FindNext();
    }
}

static void TestZipFileSystem()
{
    wxPuts(_T("*** Testing ZIP file system ***\n"));

    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem fs;
    wxPrintf(_T("Dumping all files in the archive %s:\n"), TESTFILE_ZIP);

    DumpZipDirectory(fs, _T(""), wxString(_T(' '), 4));
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
    wxPuts(_T("\n*** wxDateTime static methods test ***"));

    // some info about the current date
    int year = wxDateTime::GetCurrentYear();
    wxPrintf(_T("Current year %d is %sa leap one and has %d days.\n"),
           year,
           wxDateTime::IsLeapYear(year) ? "" : "not ",
           wxDateTime::GetNumberOfDays(year));

    wxDateTime::Month month = wxDateTime::GetCurrentMonth();
    wxPrintf(_T("Current month is '%s' ('%s') and it has %d days\n"),
           wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
           wxDateTime::GetMonthName(month).c_str(),
           wxDateTime::GetNumberOfDays(month));
}

// test time zones stuff
static void TestTimeZones()
{
    wxPuts(_T("\n*** wxDateTime timezone test ***"));

    wxDateTime now = wxDateTime::Now();

    wxPrintf(_T("Current GMT time:\t%s\n"), now.Format(_T("%c"), wxDateTime::GMT0).c_str());
    wxPrintf(_T("Unix epoch (GMT):\t%s\n"), wxDateTime((time_t)0).Format(_T("%c"), wxDateTime::GMT0).c_str());
    wxPrintf(_T("Unix epoch (EST):\t%s\n"), wxDateTime((time_t)0).Format(_T("%c"), wxDateTime::EST).c_str());
    wxPrintf(_T("Current time in Paris:\t%s\n"), now.Format(_T("%c"), wxDateTime::CET).c_str());
    wxPrintf(_T("               Moscow:\t%s\n"), now.Format(_T("%c"), wxDateTime::MSK).c_str());
    wxPrintf(_T("             New York:\t%s\n"), now.Format(_T("%c"), wxDateTime::EST).c_str());

    wxPrintf(_T("%s\n"), wxDateTime::Now().Format(_T("Our timezone is %Z")).c_str());

    wxDateTime::Tm tm = now.GetTm();
    if ( wxDateTime(tm) != now )
    {
        wxPrintf(_T("ERROR: got %s instead of %s\n"),
                 wxDateTime(tm).Format().c_str(), now.Format().c_str());
    }
}

// test some minimal support for the dates outside the standard range
static void TestTimeRange()
{
    wxPuts(_T("\n*** wxDateTime out-of-standard-range dates test ***"));

    static const wxChar *fmt = _T("%d-%b-%Y %H:%M:%S");

    wxPrintf(_T("Unix epoch:\t%s\n"),
             wxDateTime(2440587.5).Format(fmt).c_str());
    wxPrintf(_T("Feb 29, 0: \t%s\n"),
             wxDateTime(29, wxDateTime::Feb, 0).Format(fmt).c_str());
    wxPrintf(_T("JDN 0:     \t%s\n"),
             wxDateTime(0.0).Format(fmt).c_str());
    wxPrintf(_T("Jan 1, 1AD:\t%s\n"),
             wxDateTime(1, wxDateTime::Jan, 1).Format(fmt).c_str());
    wxPrintf(_T("May 29, 2099:\t%s\n"),
             wxDateTime(29, wxDateTime::May, 2099).Format(fmt).c_str());
}

// test DST calculations
static void TestTimeDST()
{
    wxPuts(_T("\n*** wxDateTime DST test ***"));

    wxPrintf(_T("DST is%s in effect now.\n\n"),
             wxDateTime::Now().IsDST() ? wxEmptyString : _T(" not"));

    for ( int year = 1990; year < 2005; year++ )
    {
        wxPrintf(_T("DST period in Europe for year %d: from %s to %s\n"),
                 year,
                 wxDateTime::GetBeginDST(year, wxDateTime::Country_EEC).Format().c_str(),
                 wxDateTime::GetEndDST(year, wxDateTime::Country_EEC).Format().c_str());
    }
}

#endif // TEST_ALL

#if TEST_INTERACTIVE

static void TestDateTimeInteractive()
{
    wxPuts(_T("\n*** interactive wxDateTime tests ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(_T("Enter a date: "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

        wxDateTime dt;
        const wxChar *p = dt.ParseDate(buf);
        if ( !p )
        {
            wxPrintf(_T("ERROR: failed to parse the date '%s'.\n"), buf);

            continue;
        }
        else if ( *p )
        {
            wxPrintf(_T("WARNING: parsed only first %u characters.\n"), p - buf);
        }

        wxPrintf(_T("%s: day %u, week of month %u/%u, week of year %u\n"),
                 dt.Format(_T("%b %d, %Y")).c_str(),
                 dt.GetDayOfYear(),
                 dt.GetWeekOfMonth(wxDateTime::Monday_First),
                 dt.GetWeekOfMonth(wxDateTime::Sunday_First),
                 dt.GetWeekOfYear(wxDateTime::Monday_First));
    }

    wxPuts(_T("\n*** done ***"));
}

#endif // TEST_INTERACTIVE

#if TEST_ALL

static void TestTimeMS()
{
    wxPuts(_T("*** testing millisecond-resolution support in wxDateTime ***"));

    wxDateTime dt1 = wxDateTime::Now(),
               dt2 = wxDateTime::UNow();

    wxPrintf(_T("Now = %s\n"), dt1.Format(_T("%H:%M:%S:%l")).c_str());
    wxPrintf(_T("UNow = %s\n"), dt2.Format(_T("%H:%M:%S:%l")).c_str());
    wxPrintf(_T("Dummy loop: "));
    for ( int i = 0; i < 6000; i++ )
    {
        //for ( int j = 0; j < 10; j++ )
        {
            wxString s;
            s.Printf(_T("%g"), sqrt((float)i));
        }

        if ( !(i % 100) )
            wxPutchar('.');
    }
    wxPuts(_T(", done"));

    dt1 = dt2;
    dt2 = wxDateTime::UNow();
    wxPrintf(_T("UNow = %s\n"), dt2.Format(_T("%H:%M:%S:%l")).c_str());

    wxPrintf(_T("Loop executed in %s ms\n"), (dt2 - dt1).Format(_T("%l")).c_str());

    wxPuts(_T("\n*** done ***"));
}

static void TestTimeHolidays()
{
    wxPuts(_T("\n*** testing wxDateTimeHolidayAuthority ***\n"));

    wxDateTime::Tm tm = wxDateTime(29, wxDateTime::May, 2000).GetTm();
    wxDateTime dtStart(1, tm.mon, tm.year),
               dtEnd = dtStart.GetLastMonthDay();

    wxDateTimeArray hol;
    wxDateTimeHolidayAuthority::GetHolidaysInRange(dtStart, dtEnd, hol);

    const wxChar *format = _T("%d-%b-%Y (%a)");

    wxPrintf(_T("All holidays between %s and %s:\n"),
           dtStart.Format(format).c_str(), dtEnd.Format(format).c_str());

    size_t count = hol.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxPrintf(_T("\t%s\n"), hol[n].Format(format).c_str());
    }

    wxPuts(wxEmptyString);
}

static void TestTimeZoneBug()
{
    wxPuts(_T("\n*** testing for DST/timezone bug ***\n"));

    wxDateTime date = wxDateTime(1, wxDateTime::Mar, 2000);
    for ( int i = 0; i < 31; i++ )
    {
        wxPrintf(_T("Date %s: week day %s.\n"),
               date.Format(_T("%d-%m-%Y")).c_str(),
               date.GetWeekDayName(date.GetWeekDay()).c_str());

        date += wxDateSpan::Day();
    }

    wxPuts(wxEmptyString);
}

static void TestTimeSpanFormat()
{
    wxPuts(_T("\n*** wxTimeSpan tests ***"));

    static const wxChar *formats[] =
    {
        _T("(default) %H:%M:%S"),
        _T("%E weeks and %D days"),
        _T("%l milliseconds"),
        _T("(with ms) %H:%M:%S:%l"),
        _T("100%% of minutes is %M"),       // test "%%"
        _T("%D days and %H hours"),
        _T("or also %S seconds"),
    };

    wxTimeSpan ts1(1, 2, 3, 4),
                ts2(111, 222, 333);
    for ( size_t n = 0; n < WXSIZEOF(formats); n++ )
    {
        wxPrintf(_T("ts1 = %s\tts2 = %s\n"),
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
    wxPuts(_T("\n*** wxTextInputStream test ***"));

    wxString filename = _T("testdata.fc");
    wxFileInputStream fsIn(filename);
    if ( !fsIn.Ok() )
    {
        wxPuts(_T("ERROR: couldn't open file."));
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

            wxPrintf(_T("Line %d: %s\n"), line++, s.c_str());
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
    wxLogTrace(_T("thread"), _T("Thread %ld is in OnExit"), GetId());

    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter && !m_cancelled )
        gs_cond.Post();
}

static void TestDetachedThreads()
{
    wxPuts(_T("\n*** Testing detached threads ***"));

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
    wxPuts(_T("\n*** Testing a joinable thread (a loooong calculation...) ***"));

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    wxPrintf(_T("\nThread terminated with exit code %lu.\n"),
             (unsigned long)thread.Wait());
}

static void TestThreadSuspend()
{
    wxPuts(_T("\n*** Testing thread suspend/resume functions ***"));

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

        wxPuts(_T("\nThread suspended"));
        if ( n > 0 )
        {
            // don't sleep but resume immediately the first time
            wxThread::Sleep(300);
        }
        wxPuts(_T("Going to resume the thread"));

        thread->Resume();
    }

    wxPuts(_T("Waiting until it terminates now"));

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

    wxPuts(_T("\n*** Testing thread delete function ***"));

    MyDetachedThread *thread0 = new MyDetachedThread(30, 'W');

    thread0->Delete();

    wxPuts(_T("\nDeleted a thread which didn't start to run yet."));

    MyDetachedThread *thread1 = new MyDetachedThread(30, 'Y');

    thread1->Run();

    wxThread::Sleep(300);

    thread1->Delete();

    wxPuts(_T("\nDeleted a running thread."));

    MyDetachedThread *thread2 = new MyDetachedThread(30, 'Z');

    thread2->Run();

    wxThread::Sleep(300);

    thread2->Pause();

    thread2->Delete();

    wxPuts(_T("\nDeleted a sleeping thread."));

    MyJoinableThread thread3(20);
    thread3.Run();

    thread3.Delete();

    wxPuts(_T("\nDeleted a joinable thread."));

    MyJoinableThread thread4(2);
    thread4.Run();

    wxThread::Sleep(300);

    thread4.Delete();

    wxPuts(_T("\nDeleted a joinable thread which already terminated."));

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
        wxPrintf(_T("Thread %lu has started running.\n"), GetId());
        fflush(stdout);

        gs_cond.Post();

        wxPrintf(_T("Thread %lu starts to wait...\n"), GetId());
        fflush(stdout);

        m_mutex->Lock();
        m_condition->Wait();
        m_mutex->Unlock();

        wxPrintf(_T("Thread %lu finished to wait, exiting.\n"), GetId());
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
    //wxLogTrace(_T("thread"), _T("Local condition var is %08x, gs_cond = %08x"),
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
    wxPuts(_T("Main thread is waiting for the other threads to start"));
    fflush(stdout);

    size_t nRunning = 0;
    while ( nRunning < WXSIZEOF(threads) )
    {
        gs_cond.Wait();

        nRunning++;

        wxPrintf(_T("Main thread: %u already running\n"), nRunning);
        fflush(stdout);
    }

    wxPuts(_T("Main thread: all threads started up."));
    fflush(stdout);

    wxThread::Sleep(500);

#if 1
    // now wake one of them up
    wxPrintf(_T("Main thread: about to signal the condition.\n"));
    fflush(stdout);
    condition.Signal();
#endif

    wxThread::Sleep(200);

    // wake all the (remaining) threads up, so that they can exit
    wxPrintf(_T("Main thread: about to broadcast the condition.\n"));
    fflush(stdout);
    condition.Broadcast();

    // give them time to terminate (dirty!)
    wxThread::Sleep(500);
}

#include "wx/utils.h"

class MyExecThread : public wxThread
{
public:
    MyExecThread(const wxString& command) : wxThread(wxTHREAD_JOINABLE),
                                            m_command(command)
    {
        Create();
    }

    virtual ExitCode Entry()
    {
        return (ExitCode)wxExecute(m_command, wxEXEC_SYNC);
    }

private:
    wxString m_command;
};

static void TestThreadExec()
{
    wxPuts(_T("*** Testing wxExecute interaction with threads ***\n"));

    MyExecThread thread(_T("true"));
    thread.Run();

    wxPrintf(_T("Main program exit code: %ld.\n"),
             wxExecute(_T("false"), wxEXEC_SYNC));

    wxPrintf(_T("Thread exit code: %ld.\n"), (long)thread.Wait());
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
        wxPrintf(_T("%s: Thread #%d (%ld) starting to wait for semaphore...\n"),
                 wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        m_sem->Wait();

        wxPrintf(_T("%s: Thread #%d (%ld) acquired the semaphore.\n"),
                 wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        Sleep(1000);

        wxPrintf(_T("%s: Thread #%d (%ld) releasing the semaphore.\n"),
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
    wxPuts(_T("*** Testing wxSemaphore class. ***"));

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
    if ( checker.Create(_T(".wxconsole.lock")) )
    {
        if ( checker.IsAnotherRunning() )
        {
            wxPrintf(_T("Another instance of the program is running, exiting.\n"));

            return 1;
        }

        // wait some time to give time to launch another instance
        wxPrintf(_T("Press \"Enter\" to continue..."));
        wxFgetc(stdin);
    }
    else // failed to create
    {
        wxPrintf(_T("Failed to init wxSingleInstanceChecker.\n"));
    }
#endif // TEST_SNGLINST

#ifdef TEST_CMDLINE
    TestCmdLineConvert();

#if wxUSE_CMDLINE_PARSER
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
        { wxCMD_LINE_SWITCH, _T("q"), _T("quiet"),   _T("be quiet") },

        { wxCMD_LINE_OPTION, _T("o"), _T("output"),  _T("output file") },
        { wxCMD_LINE_OPTION, _T("i"), _T("input"),   _T("input dir") },
        { wxCMD_LINE_OPTION, _T("s"), _T("size"),    _T("output block size"),
            wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, _T("d"), _T("date"),    _T("output file date"),
            wxCMD_LINE_VAL_DATE },

        { wxCMD_LINE_PARAM,  NULL, NULL, _T("input file"),
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);

    parser.AddOption(_T("project_name"), _T(""), _T("full path to project file"),
                     wxCMD_LINE_VAL_STRING,
                     wxCMD_LINE_OPTION_MANDATORY | wxCMD_LINE_NEEDS_SEPARATOR);

    switch ( parser.Parse() )
    {
        case -1:
            wxLogMessage(_T("Help was given, terminating."));
            break;

        case 0:
            ShowCmdLine(parser);
            break;

        default:
            wxLogMessage(_T("Syntax error detected, aborting."));
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

#ifdef TEST_EXECUTE
    TestExecute();
#endif // TEST_EXECUTE

#ifdef TEST_FILECONF
    TestFileConfRead();
#endif // TEST_FILECONF

#ifdef TEST_LOCALE
    TestDefaultLang();
#endif // TEST_LOCALE

#ifdef TEST_LOG
    wxPuts(_T("*** Testing wxLog ***"));

    wxString s;
    for ( size_t n = 0; n < 8000; n++ )
    {
        s << (wxChar)(_T('A') + (n % 26));
    }

    wxLogWarning(_T("The length of the string is %lu"),
                 (unsigned long)s.length());

    wxString msg;
    msg.Printf(_T("A very very long message: '%s', the end!\n"), s.c_str());

    // this one shouldn't be truncated
    wxPrintf(msg);

    // but this one will because log functions use fixed size buffer
    // (note that it doesn't need '\n' at the end neither - will be added
    //  by wxLog anyhow)
    wxLogMessage(_T("A very very long message 2: '%s', the end!"), s.c_str());
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
            TestFtpInteractive();
        #endif
    }
    //else: connecting to the FTP server failed

    #if 0
        TestFtpWuFtpd();
    #endif
#endif // TEST_FTP

#ifdef TEST_MIME
    wxLog::AddTraceMask(_T("mime"));
    #if TEST_ALL
        TestMimeEnum();
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

#ifdef TEST_ODBC
    TestDbOpen();
#endif // TEST_ODBC

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
    wxPrintf(_T("This system has %d CPUs\n"), nCPUs);
    if ( nCPUs != -1 )
        wxThread::SetConcurrency(nCPUs);

        TestJoinableThreads();

    #if TEST_ALL
        TestJoinableThreads();
        TestDetachedThreads();
        TestThreadSuspend();
        TestThreadDelete();
        TestThreadConditions();
        TestThreadExec();
        TestSemaphore();
    #endif
#endif // TEST_THREADS

#ifdef TEST_TIMER
    TestStopWatch();
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
    wxPuts(_T("Sleeping for 3 seconds... z-z-z-z-z..."));
    wxUsleep(3000);
#endif // TEST_USLEEP

#ifdef TEST_VCARD
    TestVCardRead();
    TestVCardWrite();
#endif // TEST_VCARD

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
