/////////////////////////////////////////////////////////////////////////////
// Name:        samples/console/console.cpp
// Purpose:     a sample console (as opposed to GUI) progam using wxWindows
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

#if wxUSE_GUI
    #error "This sample can't be compiled in GUI mode."
#endif // wxUSE_GUI

#include <stdio.h>

#include "wx/string.h"
#include "wx/file.h"
#include "wx/app.h"
#include "wx/log.h"

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
   both as a test suite for various non-GUI wxWindows classes and as a
   scratchpad for quick tests. So there are two compilation modes: if you
   define TEST_ALL all tests are run, otherwise you may enable the individual
   tests individually in the "#else" branch below.
 */

// what to test (in alphabetic order)? uncomment the line below to do all tests
//#define TEST_ALL
#ifdef TEST_ALL
    #define TEST_ARRAYS
    #define TEST_CHARSET
    #define TEST_CMDLINE
    #define TEST_DATETIME
    #define TEST_DIR
    #define TEST_DLLLOADER
    #define TEST_ENVIRON
    #define TEST_EXECUTE
    #define TEST_FILE
    #define TEST_FILECONF
    #define TEST_FILENAME
    #define TEST_FILETIME
    #define TEST_FTP
    #define TEST_HASH
    #define TEST_HASHMAP
    #define TEST_INFO_FUNCTIONS
    #define TEST_LIST
    #define TEST_LOCALE
    #define TEST_LOG
    #define TEST_LONGLONG
    #define TEST_MIME
    #define TEST_PATHLIST
    #define TEST_ODBC
    #define TEST_PRINTF
    #define TEST_REGCONF
    #define TEST_REGEX
    #define TEST_REGISTRY
    #define TEST_SNGLINST
    #define TEST_SOCKETS
    #define TEST_STREAMS
    #define TEST_STRINGS
    #define TEST_TEXTSTREAM
    #define TEST_THREADS
    #define TEST_TIMER
    #define TEST_UNICODE
    // #define TEST_VCARD            -- don't enable this (VZ)
    #define TEST_VOLUME
    #define TEST_WCHAR
    #define TEST_ZIP
    #define TEST_ZLIB

    #undef TEST_ALL
    static const bool TEST_ALL = true;
#else
    #define TEST_LOG

    static const bool TEST_ALL = false;
#endif

// some tests are interactive, define this to run them
#ifdef TEST_INTERACTIVE
    #undef TEST_INTERACTIVE

    static const bool TEST_INTERACTIVE = true;
#else
    static const bool TEST_INTERACTIVE = false;
#endif

// ----------------------------------------------------------------------------
// test class for container objects
// ----------------------------------------------------------------------------

#if defined(TEST_ARRAYS) || defined(TEST_LIST)

class Bar // Foo is already taken in the hash test
{
public:
    Bar(const wxString& name) : m_name(name) { ms_bars++; }
    Bar(const Bar& bar) : m_name(bar.m_name) { ms_bars++; }
   ~Bar() { ms_bars--; }

   static size_t GetNumber() { return ms_bars; }

   const wxChar *GetName() const { return m_name; }

private:
   wxString m_name;

   static size_t ms_bars;
};

size_t Bar::ms_bars = 0;

#endif // defined(TEST_ARRAYS) || defined(TEST_LIST)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

#if defined(TEST_STRINGS) || defined(TEST_SOCKETS)

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
// wxFontMapper::CharsetToEncoding
// ----------------------------------------------------------------------------

#ifdef TEST_CHARSET

#include "wx/fontmap.h"

static void TestCharset()
{
    static const wxChar *charsets[] =
    {
        // some vali charsets
        _T("us-ascii    "),
        _T("iso8859-1   "),
        _T("iso-8859-12 "),
        _T("koi8-r      "),
        _T("utf-7       "),
        _T("cp1250      "),
        _T("windows-1252"),

        // and now some bogus ones
        _T("            "),
        _T("cp1249      "),
        _T("iso--8859-1 "),
        _T("iso-8859-19 "),
    };

    for ( size_t n = 0; n < WXSIZEOF(charsets); n++ )
    {
        wxFontEncoding enc = wxFontMapper::Get()->CharsetToEncoding(charsets[n]);
        wxPrintf(_T("Charset: %s\tEncoding: %s (%s)\n"),
                 charsets[n],
                 wxFontMapper::Get()->GetEncodingName(enc).c_str(),
                 wxFontMapper::Get()->GetEncodingDescription(enc).c_str());
    }
}

#endif // TEST_CHARSET

// ----------------------------------------------------------------------------
// wxCmdLineParser
// ----------------------------------------------------------------------------

#ifdef TEST_CMDLINE

#include "wx/cmdline.h"
#include "wx/datetime.h"

#if wxUSE_CMDLINE_PARSER

static void ShowCmdLine(const wxCmdLineParser& parser)
{
    wxString s = _T("Input files: ");

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
#elif defined(__WXMSW__)
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

    wxPuts(_T(""));
}

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
    TestDirEnumHelper(dir, wxDIR_DEFAULT, "*.o*");

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
    wxDir dirNo("nosuchdir");
    TestDirEnumHelper(dirNo);
}

class DirPrintTraverser : public wxDirTraverser
{
public:
    virtual wxDirTraverseResult OnFile(const wxString& filename)
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
    dir.Traverse(traverser, _T(""), wxDIR_DIRS | wxDIR_HIDDEN);
}

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

#endif // TEST_DIR

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

#ifdef TEST_DLLLOADER

#include "wx/dynlib.h"

static void TestDllLoad()
{
#if defined(__WXMSW__)
    static const wxChar *LIB_NAME = _T("kernel32.dll");
    static const wxChar *FUNC_NAME = _T("lstrlenA");
#elif defined(__UNIX__)
    // weird: using just libc.so does *not* work!
    static const wxChar *LIB_NAME = _T("/lib/libc-2.0.7.so");
    static const wxChar *FUNC_NAME = _T("strlen");
#else
    #error "don't know how to test wxDllLoader on this platform"
#endif

    wxPuts(_T("*** testing wxDllLoader ***\n"));

    wxDynamicLibrary lib(LIB_NAME);
    if ( !lib.IsLoaded() )
    {
        wxPrintf(_T("ERROR: failed to load '%s'.\n"), LIB_NAME);
    }
    else
    {
        typedef int (*wxStrlenType)(const char *);
        wxStrlenType pfnStrlen = (wxStrlenType)lib.GetSymbol(FUNC_NAME);
        if ( !pfnStrlen )
        {
            wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME, LIB_NAME);
        }
        else
        {
            if ( pfnStrlen("foo") != 3 )
            {
                wxPrintf(_T("ERROR: loaded function is not wxStrlen()!\n"));
            }
            else
            {
                wxPuts(_T("... ok"));
            }
        }
    }
}

#endif // TEST_DLLLOADER

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
    if ( wxShell(SHELL_COMMAND) )
        wxPuts(_T("Ok."));
    else
        wxPuts(_T("ERROR."));

    wxPrintf(_T("Testing wxExecute: "));
    fflush(stdout);
    if ( wxExecute(COMMAND, true /* sync */) == 0 )
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
    if ( wxExecute(REDIRECT_COMMAND, output) != 0 )
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

        static const off_t len = 1024;
        wxChar buf[len];
        for ( ;; )
        {
            off_t nRead = file.Read(buf, len);
            if ( nRead == wxInvalidOffset )
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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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
        wxFFile f1(filename1, "rb"),
                f2(filename2, "rb");

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

    wxPuts(_T(""));
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

static void DumpFileName(const wxFileName& fn)
{
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

static struct FileNameInfo
{
    const wxChar *fullname;
    const wxChar *volume;
    const wxChar *path;
    const wxChar *name;
    const wxChar *ext;
    bool isAbsolute;
    wxPathFormat format;
} filenames[] =
{
    // Unix file names
    { _T("/usr/bin/ls"), _T(""), _T("/usr/bin"), _T("ls"), _T(""), true, wxPATH_UNIX },
    { _T("/usr/bin/"), _T(""), _T("/usr/bin"), _T(""), _T(""), true, wxPATH_UNIX },
    { _T("~/.zshrc"), _T(""), _T("~"), _T(".zshrc"), _T(""), true, wxPATH_UNIX },
    { _T("../../foo"), _T(""), _T("../.."), _T("foo"), _T(""), false, wxPATH_UNIX },
    { _T("foo.bar"), _T(""), _T(""), _T("foo"), _T("bar"), false, wxPATH_UNIX },
    { _T("~/foo.bar"), _T(""), _T("~"), _T("foo"), _T("bar"), true, wxPATH_UNIX },
    { _T("/foo"), _T(""), _T("/"), _T("foo"), _T(""), true, wxPATH_UNIX },
    { _T("Mahogany-0.60/foo.bar"), _T(""), _T("Mahogany-0.60"), _T("foo"), _T("bar"), false, wxPATH_UNIX },
    { _T("/tmp/wxwin.tar.bz"), _T(""), _T("/tmp"), _T("wxwin.tar"), _T("bz"), true, wxPATH_UNIX },

    // Windows file names
    { _T("foo.bar"), _T(""), _T(""), _T("foo"), _T("bar"), false, wxPATH_DOS },
    { _T("\\foo.bar"), _T(""), _T("\\"), _T("foo"), _T("bar"), false, wxPATH_DOS },
    { _T("c:foo.bar"), _T("c"), _T(""), _T("foo"), _T("bar"), false, wxPATH_DOS },
    { _T("c:\\foo.bar"), _T("c"), _T("\\"), _T("foo"), _T("bar"), true, wxPATH_DOS },
    { _T("c:\\Windows\\command.com"), _T("c"), _T("\\Windows"), _T("command"), _T("com"), true, wxPATH_DOS },
    { _T("\\\\server\\foo.bar"), _T("server"), _T("\\"), _T("foo"), _T("bar"), true, wxPATH_DOS },
    { _T("\\\\server\\dir\\foo.bar"), _T("server"), _T("\\dir"), _T("foo"), _T("bar"), true, wxPATH_DOS },

    // wxFileName support for Mac file names is broken currently
#if 0
    // Mac file names
    { _T("Volume:Dir:File"), _T("Volume"), _T("Dir"), _T("File"), _T(""), true, wxPATH_MAC },
    { _T("Volume:Dir:Subdir:File"), _T("Volume"), _T("Dir:Subdir"), _T("File"), _T(""), true, wxPATH_MAC },
    { _T("Volume:"), _T("Volume"), _T(""), _T(""), _T(""), true, wxPATH_MAC },
    { _T(":Dir:File"), _T(""), _T("Dir"), _T("File"), _T(""), false, wxPATH_MAC },
    { _T(":File.Ext"), _T(""), _T(""), _T("File"), _T(".Ext"), false, wxPATH_MAC },
    { _T("File.Ext"), _T(""), _T(""), _T("File"), _T(".Ext"), false, wxPATH_MAC },
#endif // 0

    // VMS file names
    { _T("device:[dir1.dir2.dir3]file.txt"), _T("device"), _T("dir1.dir2.dir3"), _T("file"), _T("txt"), true, wxPATH_VMS },
    { _T("file.txt"), _T(""), _T(""), _T("file"), _T("txt"), false, wxPATH_VMS },
};

static void TestFileNameConstruction()
{
    wxPuts(_T("*** testing wxFileName construction ***"));

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const FileNameInfo& fni = filenames[n];

        wxFileName fn(fni.fullname, fni.format);

        wxString fullname = fn.GetFullPath(fni.format);
        if ( fullname != fni.fullname )
        {
            wxPrintf(_T("ERROR: fullname should be '%s'\n"), fni.fullname);
        }

        bool isAbsolute = fn.IsAbsolute(fni.format);
        wxPrintf(_T("'%s' is %s (%s)\n\t"),
               fullname.c_str(),
               isAbsolute ? "absolute" : "relative",
               isAbsolute == fni.isAbsolute ? "ok" : "ERROR");

        if ( !fn.Normalize(wxPATH_NORM_ALL, _T(""), fni.format) )
        {
            wxPuts(_T("ERROR (couldn't be normalized)"));
        }
        else
        {
            wxPrintf(_T("normalized: '%s'\n"), fn.GetFullPath(fni.format).c_str());
        }
    }

    wxPuts(_T(""));
}

static void TestFileNameSplit()
{
    wxPuts(_T("*** testing wxFileName splitting ***"));

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const FileNameInfo& fni = filenames[n];
        wxString volume, path, name, ext;
        wxFileName::SplitPath(fni.fullname,
                              &volume, &path, &name, &ext, fni.format);

        wxPrintf(_T("%s -> volume = '%s', path = '%s', name = '%s', ext = '%s'"),
               fni.fullname,
               volume.c_str(), path.c_str(), name.c_str(), ext.c_str());

        if ( volume != fni.volume )
            wxPrintf(_T(" (ERROR: volume = '%s')"), fni.volume);
        if ( path != fni.path )
            wxPrintf(_T(" (ERROR: path = '%s')"), fni.path);
        if ( name != fni.name )
            wxPrintf(_T(" (ERROR: name = '%s')"), fni.name);
        if ( ext != fni.ext )
            wxPrintf(_T(" (ERROR: ext = '%s')"), fni.ext);

        wxPuts(_T(""));
    }
}

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

static void TestFileNameMakeRelative()
{
    wxPuts(_T("*** testing wxFileName::MakeRelativeTo() ***"));

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const FileNameInfo& fni = filenames[n];

        wxFileName fn(fni.fullname, fni.format);

        // choose the base dir of the same format
        wxString base;
        switch ( fni.format )
        {
            case wxPATH_UNIX:
                base = "/usr/bin/";
                break;

            case wxPATH_DOS:
                base = "c:\\";
                break;

            case wxPATH_MAC:
            case wxPATH_VMS:
                // TODO: I don't know how this is supposed to work there
                continue;

            case wxPATH_NATIVE: // make gcc happy
            default:
                wxFAIL_MSG( "unexpected path format" );
        }

        wxPrintf(_T("'%s' relative to '%s': "),
               fn.GetFullPath(fni.format).c_str(), base.c_str());

        if ( !fn.MakeRelativeTo(base, fni.format) )
        {
            wxPuts(_T("unchanged"));
        }
        else
        {
            wxPrintf(_T("'%s'\n"), fn.GetFullPath(fni.format).c_str());
        }
    }
}

static void TestFileNameMakeAbsolute()
{
    wxPuts(_T("*** testing wxFileName::MakeAbsolute() ***"));

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const FileNameInfo& fni = filenames[n];
        wxFileName fn(fni.fullname, fni.format);

        wxPrintf(_T("'%s' absolutized: "),
               fn.GetFullPath(fni.format).c_str());
        fn.MakeAbsolute();
        wxPrintf(_T("'%s'\n"), fn.GetFullPath(fni.format).c_str());
    }

    wxPuts(_T(""));
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

#include <wx/filename.h>
#include <wx/datetime.h>

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

static void TestFileSetTimes()
{
    wxFileName fn(_T("testdata.fc"));

    if ( !fn.Touch() )
    {
        wxPrintf(_T("ERROR: Touch() failed.\n"));
    }
}

#endif // TEST_FILETIME

// ----------------------------------------------------------------------------
// wxHashTable
// ----------------------------------------------------------------------------

#ifdef TEST_HASH

#include "wx/hash.h"

struct Foo
{
    Foo(int n_) { n = n_; count++; }
    ~Foo() { count--; }

    int n;

    static size_t count;
};

size_t Foo::count = 0;

WX_DECLARE_LIST(Foo, wxListFoos);
WX_DECLARE_HASH(Foo, wxListFoos, wxHashFoos);

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxListFoos);

static void TestHash()
{
    wxPuts(_T("*** Testing wxHashTable ***\n"));

    {
        wxHashFoos hash;
        hash.DeleteContents(true);

        wxPrintf(_T("Hash created: %u foos in hash, %u foos totally\n"),
               hash.GetCount(), Foo::count);

        static const int hashTestData[] =
        {
            0, 1, 17, -2, 2, 4, -4, 345, 3, 3, 2, 1,
        };

        size_t n;
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            hash.Put(hashTestData[n], n, new Foo(n));
        }

        wxPrintf(_T("Hash filled: %u foos in hash, %u foos totally\n"),
               hash.GetCount(), Foo::count);

        wxPuts(_T("Hash access test:"));
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            wxPrintf(_T("\tGetting element with key %d, value %d: "),
                   hashTestData[n], n);
            Foo *foo = hash.Get(hashTestData[n], n);
            if ( !foo )
            {
                wxPrintf(_T("ERROR, not found.\n"));
            }
            else
            {
                wxPrintf(_T("%d (%s)\n"), foo->n,
                       (size_t)foo->n == n ? "ok" : "ERROR");
            }
        }

        wxPrintf(_T("\nTrying to get an element not in hash: "));

        if ( hash.Get(1234) || hash.Get(1, 0) )
        {
            wxPuts(_T("ERROR: found!"));
        }
        else
        {
            wxPuts(_T("ok (not found)"));
        }
    }

    wxPrintf(_T("Hash destroyed: %u foos left\n"), Foo::count);
}

#endif // TEST_HASH

// ----------------------------------------------------------------------------
// wxHashMap
// ----------------------------------------------------------------------------

#ifdef TEST_HASHMAP

#include "wx/hashmap.h"

// test compilation of basic map types
WX_DECLARE_HASH_MAP( int*, int*, wxPointerHash, wxPointerEqual, myPtrHashMap );
WX_DECLARE_HASH_MAP( long, long, wxIntegerHash, wxIntegerEqual, myLongHashMap );
WX_DECLARE_HASH_MAP( unsigned long, unsigned, wxIntegerHash, wxIntegerEqual,
                     myUnsignedHashMap );
WX_DECLARE_HASH_MAP( unsigned int, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap1 );
WX_DECLARE_HASH_MAP( int, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap2 );
WX_DECLARE_HASH_MAP( short, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap3 );
WX_DECLARE_HASH_MAP( unsigned short, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap4 );

// same as:
// WX_DECLARE_HASH_MAP( wxString, wxString, wxStringHash, wxStringEqual,
//                      myStringHashMap );
WX_DECLARE_STRING_HASH_MAP(wxString, myStringHashMap);

typedef myStringHashMap::iterator Itor;

static void TestHashMap()
{
    wxPuts(_T("*** Testing wxHashMap ***\n"));
    myStringHashMap sh(0); // as small as possible
    wxString buf;
    size_t i;
    const size_t count = 10000;

    // init with some data
    for( i = 0; i < count; ++i )
    {
        buf.Printf(wxT("%d"), i );
        sh[buf] = wxT("A") + buf + wxT("C");
    }

    // test that insertion worked
    if( sh.size() != count )
    {
        wxPrintf(_T("*** ERROR: %u ELEMENTS, SHOULD BE %u ***\n"), sh.size(), count);
    }

    for( i = 0; i < count; ++i )
    {
        buf.Printf(wxT("%d"), i );
        if( sh[buf] != wxT("A") + buf + wxT("C") )
        {
            wxPrintf(_T("*** ERROR INSERTION BROKEN! STOPPING NOW! ***\n"));
            return;
        }
    }

    // check that iterators work
    Itor it;
    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        if( i == count )
        {
            wxPrintf(_T("*** ERROR ITERATORS DO NOT TERMINATE! STOPPING NOW! ***\n"));
            return;
        }

        if( it->second != sh[it->first] )
        {
            wxPrintf(_T("*** ERROR ITERATORS BROKEN! STOPPING NOW! ***\n"));
            return;
        }
    }

    if( sh.size() != i )
    {
        wxPrintf(_T("*** ERROR: %u ELEMENTS ITERATED, SHOULD BE %u ***\n"), i, count);
    }

    // test copy ctor, assignment operator
    myStringHashMap h1( sh ), h2( 0 );
    h2 = sh;

    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        if( h1[it->first] != it->second )
        {
            wxPrintf(_T("*** ERROR: COPY CTOR BROKEN %s ***\n"), it->first.c_str());
        }

        if( h2[it->first] != it->second )
        {
            wxPrintf(_T("*** ERROR: OPERATOR= BROKEN %s ***\n"), it->first.c_str());
        }
    }

    // other tests
    for( i = 0; i < count; ++i )
    {
        buf.Printf(wxT("%d"), i );
        size_t sz = sh.size();

        // test find() and erase(it)
        if( i < 100 )
        {
            it = sh.find( buf );
            if( it != sh.end() )
            {
                sh.erase( it );

                if( sh.find( buf ) != sh.end() )
                {
                    wxPrintf(_T("*** ERROR: FOUND DELETED ELEMENT %u ***\n"), i);
                }
            }
            else
                wxPrintf(_T("*** ERROR: CANT FIND ELEMENT %u ***\n"), i);
        }
        else
        // test erase(key)
        {
            size_t c = sh.erase( buf );
            if( c != 1 )
                wxPrintf(_T("*** ERROR: SHOULD RETURN 1 ***\n"));

            if( sh.find( buf ) != sh.end() )
            {
                wxPrintf(_T("*** ERROR: FOUND DELETED ELEMENT %u ***\n"), i);
            }
        }

        // count should decrease
        if( sh.size() != sz - 1 )
        {
            wxPrintf(_T("*** ERROR: COUNT DID NOT DECREASE ***\n"));
        }
    }

    wxPrintf(_T("*** Finished testing wxHashMap ***\n"));
}

#endif // TEST_HASHMAP

// ----------------------------------------------------------------------------
// wxList
// ----------------------------------------------------------------------------

#ifdef TEST_LIST

#include "wx/list.h"

WX_DECLARE_LIST(Bar, wxListBars);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListBars);

static void TestListCtor()
{
    wxPuts(_T("*** Testing wxList construction ***\n"));

    {
        wxListBars list1;
        list1.Append(new Bar(_T("first")));
        list1.Append(new Bar(_T("second")));

        wxPrintf(_T("After 1st list creation: %u objects in the list, %u objects total.\n"),
               list1.GetCount(), Bar::GetNumber());

        wxListBars list2;
        list2 = list1;

        wxPrintf(_T("After 2nd list creation: %u and %u objects in the lists, %u objects total.\n"),
               list1.GetCount(), list2.GetCount(), Bar::GetNumber());

        list1.DeleteContents(true);
    }

    wxPrintf(_T("After list destruction: %u objects left.\n"), Bar::GetNumber());
}

#endif // TEST_LIST

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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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
                                     wxFileType::MessageParameters(fname, _T(""))) )
                cmd = _T("<no command available>");
            else
                cmd = wxString(_T('"')) + cmd + _T('"');

            wxPrintf(_T("To open %s (%s) do %s.\n"),
                     fname.c_str(), desc.c_str(), cmd.c_str());

            delete ft;
        }
    }

    wxPuts(_T(""));
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

    wxPuts(_T(""));
}

#endif // TEST_MIME

// ----------------------------------------------------------------------------
// misc information functions
// ----------------------------------------------------------------------------

#ifdef TEST_INFO_FUNCTIONS

#include "wx/utils.h"

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

static void TestOsInfo()
{
    wxPuts(_T("*** Testing OS info functions ***\n"));

    int major, minor;
    wxGetOsVersion(&major, &minor);
    wxPrintf(_T("Running under: %s, version %d.%d\n"),
            wxGetOsDescription().c_str(), major, minor);

    wxPrintf(_T("%ld free bytes of memory left.\n"), wxGetFreeMemory());

    wxPrintf(_T("Host name is %s (%s).\n"),
           wxGetHostName().c_str(), wxGetFullHostName().c_str());

    wxPuts(_T(""));
}

static void TestUserInfo()
{
    wxPuts(_T("*** Testing user info functions ***\n"));

    wxPrintf(_T("User id is:\t%s\n"), wxGetUserId().c_str());
    wxPrintf(_T("User name is:\t%s\n"), wxGetUserName().c_str());
    wxPrintf(_T("Home dir is:\t%s\n"), wxGetHomeDir().c_str());
    wxPrintf(_T("Email address:\t%s\n"), wxGetEmailAddress().c_str());

    wxPuts(_T(""));
}

#endif // TEST_INFO_FUNCTIONS

// ----------------------------------------------------------------------------
// long long
// ----------------------------------------------------------------------------

#ifdef TEST_LONGLONG

#include "wx/longlong.h"
#include "wx/timer.h"

// make a 64 bit number from 4 16 bit ones
#define MAKE_LL(x1, x2, x3, x4) wxLongLong((x1 << 16) | x2, (x3 << 16) | x3)

// get a random 64 bit number
#define RAND_LL()   MAKE_LL(rand(), rand(), rand(), rand())

static const long testLongs[] =
{
    0,
    1,
    -1,
    LONG_MAX,
    LONG_MIN,
    0x1234,
    -0x1234
};

#if wxUSE_LONGLONG_WX
inline bool operator==(const wxLongLongWx& a, const wxLongLongNative& b)
    { return a.GetHi() == b.GetHi() && a.GetLo() == b.GetLo(); }
inline bool operator==(const wxLongLongNative& a, const wxLongLongWx& b)
    { return a.GetHi() == b.GetHi() && a.GetLo() == b.GetLo(); }
#endif // wxUSE_LONGLONG_WX

static void TestSpeed()
{
    static const long max = 100000000;
    long n;

    {
        wxStopWatch sw;

        long l = 0;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        wxPrintf(_T("Summing longs took %ld milliseconds.\n"), sw.Time());
    }

#if wxUSE_LONGLONG_NATIVE
    {
        wxStopWatch sw;

        wxLongLong_t l = 0;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        wxPrintf(_T("Summing wxLongLong_t took %ld milliseconds.\n"), sw.Time());
    }
#endif // wxUSE_LONGLONG_NATIVE

    {
        wxStopWatch sw;

        wxLongLong l;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        wxPrintf(_T("Summing wxLongLongs took %ld milliseconds.\n"), sw.Time());
    }
}

static void TestLongLongConversion()
{
    wxPuts(_T("*** Testing wxLongLong conversions ***\n"));

    wxLongLong a;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        a = RAND_LL();

#if wxUSE_LONGLONG_NATIVE
        wxLongLongNative b(a.GetHi(), a.GetLo());

        wxASSERT_MSG( a == b, "conversions failure" );
#else
        wxPuts(_T("Can't do it without native long long type, test skipped."));

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    wxPuts(_T(" done!"));
}

static void TestMultiplication()
{
    wxPuts(_T("*** Testing wxLongLong multiplication ***\n"));

    wxLongLong a, b;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        a = RAND_LL();
        b = RAND_LL();

#if wxUSE_LONGLONG_NATIVE
        wxLongLongNative aa(a.GetHi(), a.GetLo());
        wxLongLongNative bb(b.GetHi(), b.GetLo());

        wxASSERT_MSG( a*b == aa*bb, "multiplication failure" );
#else // !wxUSE_LONGLONG_NATIVE
        wxPuts(_T("Can't do it without native long long type, test skipped."));

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    wxPuts(_T(" done!"));
}

static void TestDivision()
{
    wxPuts(_T("*** Testing wxLongLong division ***\n"));

    wxLongLong q, r;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        // get a random wxLongLong (shifting by 12 the MSB ensures that the
        // multiplication will not overflow)
        wxLongLong ll = MAKE_LL((rand() >> 12), rand(), rand(), rand());

        // get a random (but non null) long (not wxLongLong for now) to divide
        // it with
        long l;
        do
        {
           l = rand();
        }
        while ( !l );

        q = ll / l;
        r = ll % l;

#if wxUSE_LONGLONG_NATIVE
        wxLongLongNative m(ll.GetHi(), ll.GetLo());

        wxLongLongNative p = m / l, s = m % l;
        wxASSERT_MSG( q == p && r == s, "division failure" );
#else // !wxUSE_LONGLONG_NATIVE
        // verify the result
        wxASSERT_MSG( ll == q*l + r, "division failure" );
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    wxPuts(_T(" done!"));
}

static void TestAddition()
{
    wxPuts(_T("*** Testing wxLongLong addition ***\n"));

    wxLongLong a, b, c;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        a = RAND_LL();
        b = RAND_LL();
        c = a + b;

#if wxUSE_LONGLONG_NATIVE
        wxASSERT_MSG( c == wxLongLongNative(a.GetHi(), a.GetLo()) +
                           wxLongLongNative(b.GetHi(), b.GetLo()),
                      "addition failure" );
#else // !wxUSE_LONGLONG_NATIVE
        wxASSERT_MSG( c - b == a, "addition failure" );
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    wxPuts(_T(" done!"));
}

static void TestBitOperations()
{
    wxPuts(_T("*** Testing wxLongLong bit operation ***\n"));

    wxLongLong ll;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        ll = RAND_LL();

#if wxUSE_LONGLONG_NATIVE
        for ( size_t n = 0; n < 33; n++ )
        {
        }
#else // !wxUSE_LONGLONG_NATIVE
        wxPuts(_T("Can't do it without native long long type, test skipped."));

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    wxPuts(_T(" done!"));
}

static void TestLongLongComparison()
{
#if wxUSE_LONGLONG_WX
    wxPuts(_T("*** Testing wxLongLong comparison ***\n"));

    static const long ls[2] =
    {
        0x1234,
       -0x1234,
    };

    wxLongLongWx lls[2];
    lls[0] = ls[0];
    lls[1] = ls[1];

    for ( size_t n = 0; n < WXSIZEOF(testLongs); n++ )
    {
        bool res;

        for ( size_t m = 0; m < WXSIZEOF(lls); m++ )
        {
            res = lls[m] > testLongs[n];
            wxPrintf(_T("0x%lx > 0x%lx is %s (%s)\n"),
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] > testLongs[n]) ? "ok" : "ERROR");

            res = lls[m] < testLongs[n];
            wxPrintf(_T("0x%lx < 0x%lx is %s (%s)\n"),
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] < testLongs[n]) ? "ok" : "ERROR");

            res = lls[m] == testLongs[n];
            wxPrintf(_T("0x%lx == 0x%lx is %s (%s)\n"),
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] == testLongs[n]) ? "ok" : "ERROR");
        }
    }
#endif // wxUSE_LONGLONG_WX
}

static void TestLongLongToString()
{
    wxPuts(_T("*** Testing wxLongLong::ToString() ***\n"));

    for ( size_t n = 0; n < WXSIZEOF(testLongs); n++ )
    {
        wxLongLong ll = testLongs[n];
        wxPrintf(_T("%ld == %s\n"), testLongs[n], ll.ToString().c_str());
    }

    wxLongLong ll(0x12345678, 0x87654321);
    wxPrintf(_T("0x1234567887654321 = %s\n"), ll.ToString().c_str());

    ll.Negate();
    wxPrintf(_T("-0x1234567887654321 = %s\n"), ll.ToString().c_str());
}

static void TestLongLongPrintf()
{
    wxPuts(_T("*** Testing wxLongLong printing ***\n"));

#ifdef wxLongLongFmtSpec
    wxLongLong ll = wxLL(0x1234567890abcdef);
    wxString s = wxString::Format(_T("%") wxLongLongFmtSpec _T("x"), ll);
    wxPrintf(_T("0x1234567890abcdef -> %s (%s)\n"),
             s.c_str(), s == _T("1234567890abcdef") ? _T("ok") : _T("ERROR"));
#else // !wxLongLongFmtSpec
    #error "wxLongLongFmtSpec not defined for this compiler/platform"
#endif
}

#undef MAKE_LL
#undef RAND_LL

#endif // TEST_LONGLONG

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

static void TestRegExCompile()
{
    wxPuts(_T("*** Testing RE compilation ***\n"));

    static struct RegExCompTestData
    {
        const wxChar *pattern;
        bool correct;
    } regExCompTestData[] =
    {
        { _T("foo"), true },
        { _T("foo("), false },
        { _T("foo(bar"), false },
        { _T("foo(bar)"), true },
        { _T("foo["), false },
        { _T("foo[bar"), false },
        { _T("foo[bar]"), true },
        { _T("foo{"), true },
        { _T("foo{1"), false },
        { _T("foo{bar"), true },
        { _T("foo{1}"), true },
        { _T("foo{1,2}"), true },
        { _T("foo{bar}"), true },
        { _T("foo*"), true },
        { _T("foo**"), false },
        { _T("foo+"), true },
        { _T("foo++"), false },
        { _T("foo?"), true },
        { _T("foo??"), false },
        { _T("foo?+"), false },
    };

    wxRegEx re;
    for ( size_t n = 0; n < WXSIZEOF(regExCompTestData); n++ )
    {
        const RegExCompTestData& data = regExCompTestData[n];
        bool ok = re.Compile(data.pattern);

        wxPrintf(_T("'%s' is %sa valid RE (%s)\n"),
                 data.pattern,
                 ok ? _T("") : _T("not "),
                 ok == data.correct ? _T("ok") : _T("ERROR"));
    }
}

static void TestRegExMatch()
{
    wxPuts(_T("*** Testing RE matching ***\n"));

    static struct RegExMatchTestData
    {
        const wxChar *pattern;
        const wxChar *text;
        bool correct;
    } regExMatchTestData[] =
    {
        { _T("foo"), _T("bar"), false },
        { _T("foo"), _T("foobar"), true },
        { _T("^foo"), _T("foobar"), true },
        { _T("^foo"), _T("barfoo"), false },
        { _T("bar$"), _T("barbar"), true },
        { _T("bar$"), _T("barbar "), false },
    };

    for ( size_t n = 0; n < WXSIZEOF(regExMatchTestData); n++ )
    {
        const RegExMatchTestData& data = regExMatchTestData[n];

        wxRegEx re(data.pattern);
        bool ok = re.Matches(data.text);

        wxPrintf(_T("'%s' %s %s (%s)\n"),
                 data.pattern,
                 ok ? _T("matches") : _T("doesn't match"),
                 data.text,
                 ok == data.correct ? _T("ok") : _T("ERROR"));
    }
}

static void TestRegExSubmatch()
{
    wxPuts(_T("*** Testing RE subexpressions ***\n"));

    wxRegEx re(_T("([[:alpha:]]+) ([[:alpha:]]+) ([[:digit:]]+).*([[:digit:]]+)$"));
    if ( !re.IsValid() )
    {
        wxPuts(_T("ERROR: compilation failed."));
        return;
    }

    wxString text = _T("Fri Jul 13 18:37:52 CEST 2001");

    if ( !re.Matches(text) )
    {
        wxPuts(_T("ERROR: match expected."));
    }
    else
    {
        wxPrintf(_T("Entire match: %s\n"), re.GetMatch(text).c_str());

        wxPrintf(_T("Date: %s/%s/%s, wday: %s\n"),
                 re.GetMatch(text, 3).c_str(),
                 re.GetMatch(text, 2).c_str(),
                 re.GetMatch(text, 4).c_str(),
                 re.GetMatch(text, 1).c_str());
    }
}

static void TestRegExReplacement()
{
    wxPuts(_T("*** Testing RE replacement ***"));

    static struct RegExReplTestData
    {
        const wxChar *text;
        const wxChar *repl;
        const wxChar *result;
        size_t count;
    } regExReplTestData[] =
    {
        { _T("foo123"), _T("bar"), _T("bar"), 1 },
        { _T("foo123"), _T("\\2\\1"), _T("123foo"), 1 },
        { _T("foo_123"), _T("\\2\\1"), _T("123foo"), 1 },
        { _T("123foo"), _T("bar"), _T("123foo"), 0 },
        { _T("123foo456foo"), _T("&&"), _T("123foo456foo456foo"), 1 },
        { _T("foo123foo123"), _T("bar"), _T("barbar"), 2 },
        { _T("foo123_foo456_foo789"), _T("bar"), _T("bar_bar_bar"), 3 },
    };

    const wxChar *pattern = _T("([a-z]+)[^0-9]*([0-9]+)");
    wxRegEx re(pattern);

    wxPrintf(_T("Using pattern '%s' for replacement.\n"), pattern);

    for ( size_t n = 0; n < WXSIZEOF(regExReplTestData); n++ )
    {
        const RegExReplTestData& data = regExReplTestData[n];

        wxString text = data.text;
        size_t nRepl = re.Replace(&text, data.repl);

        wxPrintf(_T("%s =~ s/RE/%s/g: %u match%s, result = '%s' ("),
                 data.text, data.repl,
                 nRepl, nRepl == 1 ? _T("") : _T("es"),
                 text.c_str());
        if ( text == data.result && nRepl == data.count )
        {
            wxPuts(_T("ok)"));
        }
        else
        {
            wxPrintf(_T("ERROR: should be %u and '%s')\n"),
                     data.count, data.result);
        }
    }
}

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

#include <wx/db.h>

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
       in wxWindows: if I read the copyright below properly, this shouldn't
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
  wxPrintf(_T("%10s\n"), (wxChar *) NULL);
  wxPrintf(_T("%-10s\n"), (wxChar *) NULL);
}

static void TestPrintf()
{
  static wxChar shortstr[] = _T("Hi, Z.");
  static wxChar longstr[] = _T("Good morning, Doctor Chandra.  This is Hal.  \
I am ready for my first lesson today.");
  int result = 0;

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

  wxPrintf(_T("bad format:\t\"%b\"\n"));
  wxPrintf(_T("nil pointer (padded):\t\"%10p\"\n"), (void *) NULL);

  wxPrintf(_T("decimal negative:\t\"%d\"\n"), -2345);
  wxPrintf(_T("octal negative:\t\"%o\"\n"), -2345);
  wxPrintf(_T("hex negative:\t\"%x\"\n"), -2345);
  wxPrintf(_T("long decimal number:\t\"%ld\"\n"), -123456L);
  wxPrintf(_T("long octal negative:\t\"%lo\"\n"), -2345L);
  wxPrintf(_T("long unsigned decimal number:\t\"%lu\"\n"), -123456L);
  wxPrintf(_T("zero-padded LDN:\t\"%010ld\"\n"), -123456L);
  wxPrintf(_T("left-adjusted ZLDN:\t\"%-010ld\"\n"), -123456);
  wxPrintf(_T("space-padded LDN:\t\"%10ld\"\n"), -123456L);
  wxPrintf(_T("left-adjusted SLDN:\t\"%-10ld\"\n"), -123456L);

  wxPrintf(_T("zero-padded string:\t\"%010s\"\n"), shortstr);
  wxPrintf(_T("left-adjusted Z string:\t\"%-010s\"\n"), shortstr);
  wxPrintf(_T("space-padded string:\t\"%10s\"\n"), shortstr);
  wxPrintf(_T("left-adjusted S string:\t\"%-10s\"\n"), shortstr);
  wxPrintf(_T("null string:\t\"%s\"\n"), (wxChar *)NULL);
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

  wxPrintf (_T("%15.5e\n"), 4.9406564584124654e-324);

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
      wxPrintf (_T("sprintf (buf, \"%%07Lo\", 040000000000ll) = %s"), buf);

      if (wxStrcmp (buf, _T("40000000000")) != 0)
      {
          result = 1;
          wxPuts (_T("\tFAILED"));
      }
      wxPuts (_T(""));
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
  wxSprintf (buf, _T("%04.*o"), prec, 33);
  if (wxStrcmp (buf, _T(" 041")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T(" 041"));
  prec = 7;
  wxSprintf (buf, _T("%09.*u"), prec, 33);
  if (wxStrcmp (buf, _T("  0000033")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T("  0000033"));
  prec = 3;
  wxSprintf (buf, _T("%04.*x"), prec, 33);
  if (wxStrcmp (buf, _T(" 021")) != 0)
    wxPrintf (_T("got: '%s', expected: '%s'\n"), buf, _T(" 021"));
  prec = 3;
  wxSprintf (buf, _T("%04.*X"), prec, 33);
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

static void TestRegConfWrite()
{
    wxRegConfig regconf(_T("console"), _T("wxwindows"));
    regconf.Write(_T("Hello"), wxString(_T("world")));
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

        putchar('\n');

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

    key.SetName("HKEY_CLASSES_ROOT\\.ddf" );
    key.Create();
    key = "ddxf_auto_file" ;
    key.SetName("HKEY_CLASSES_ROOT\\.flo" );
    key.Create();
    key = "ddxf_auto_file" ;
    key.SetName("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon");
    key.Create();
    key = "program,0" ;
    key.SetName("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command");
    key.Create();
    key = "program \"%1\"" ;

    key.SetName("HKEY_CLASSES_ROOT\\.ddf" );
    key.DeleteSelf();
    key.SetName("HKEY_CLASSES_ROOT\\.flo" );
    key.DeleteSelf();
    key.SetName("HKEY_CLASSES_ROOT\\ddxf_auto_file\\DefaultIcon");
    key.DeleteSelf();
    key.SetName("HKEY_CLASSES_ROOT\\ddxf_auto_file\\shell\\open\\command");
    key.DeleteSelf();
}

#endif // TEST_REGISTRY

// ----------------------------------------------------------------------------
// scope guard
// ----------------------------------------------------------------------------

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
    ON_BLOCK_EXIT0(function0);
    ON_BLOCK_EXIT1(function1, 17);
    ON_BLOCK_EXIT2(function2, 3.14, 'p');

    Object obj;
    ON_BLOCK_EXIT_OBJ0(obj, Object::method0);
    ON_BLOCK_EXIT_OBJ1(obj, Object::method1, 7);
    ON_BLOCK_EXIT_OBJ2(obj, Object::method2, 2.71, 'e');

    wxScopeGuard dismissed = wxMakeGuard(function0);
    dismissed.Dismiss();
}

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

    static const wxChar *hostname = _T("www.wxwindows.org");

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
    static const wxChar *hostname = _T("ftp.wxwindows.org");

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

    if ( ftp.SendCommand("STAT") != '2' )
    {
        wxPuts(_T("ERROR: STAT failed"));
    }
    else
    {
        wxPrintf(_T("STAT returned:\n\n%s\n"), ftp.GetLastResult().c_str());
    }

    if ( ftp.SendCommand("HELP SITE") != '2' )
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
        if ( start == "LIST" || start == "NLST" )
        {
            wxString wildcard;
            if ( wxStrlen(buf) > 4 )
                wildcard = buf + 5;

            wxArrayString files;
            if ( !ftp.GetList(files, wildcard, start == "LIST") )
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
    if ( ftp.SendCommand(wxString("STAT ") + file1) != '2' )
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
// streams
// ----------------------------------------------------------------------------

#ifdef TEST_STREAMS

#include "wx/wfstream.h"
#include "wx/mstream.h"

static void TestFileStream()
{
    wxPuts(_T("*** Testing wxFileInputStream ***"));

    static const wxChar *filename = _T("testdata.fs");
    {
        wxFileOutputStream fsOut(filename);
        fsOut.Write("foo", 3);
    }

    wxFileInputStream fsIn(filename);
    wxPrintf(_T("File stream size: %u\n"), fsIn.GetSize());
    while ( !fsIn.Eof() )
    {
        putchar(fsIn.GetC());
    }

    if ( !wxRemoveFile(filename) )
    {
        wxPrintf(_T("ERROR: failed to remove the file '%s'.\n"), filename);
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
        putchar(memInpStream.GetC());
    }

    wxPuts(_T("\n*** wxMemoryInputStream test done ***"));
}

#endif // TEST_STREAMS

// ----------------------------------------------------------------------------
// timers
// ----------------------------------------------------------------------------

#ifdef TEST_TIMER

#include "wx/timer.h"
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

        putchar('.');
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
        putchar('\n');

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

        // now show how to deal with multiply occuring properties
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
        vcard.SetOrganization("wxWindows", "R&D");

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

#ifdef TEST_UNICODE

static void TestUnicodeToFromAscii()
{
    wxPuts(_T("Testing wxString::To/FromAscii()\n"));

    static const char *msg = "Hello, world!";
    wxString s = wxString::FromAscii(msg);

    wxPrintf(_T("Message in Unicode: %s\n"), s.c_str());
    printf("Message in ASCII: %s\n", (const char *)s.ToAscii());

    wxPutchar(_T('\n'));
}

#endif // TEST_UNICODE

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

        wxString s(wxConvUTF8.cMB2WC((const char *)u8d.text), *wxConvCurrent);
        if ( s.empty() )
            s = _T("<< conversion failed >>");
        wxPrintf(_T("String in current cset: %s\n"), s.c_str());

    }

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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

    static const wxChar *filename = _T("foo");
    wxZipInputStream istr(TESTFILE_ZIP, filename);
    wxPrintf(_T("Archive size: %u\n"), istr.GetSize());

    wxPrintf(_T("Dumping the file '%s':\n"), filename);
    while ( !istr.Eof() )
    {
        putchar(istr.GetC());
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
// ZLIB stream
// ----------------------------------------------------------------------------

#ifdef TEST_ZLIB

#include "wx/zstream.h"
#include "wx/wfstream.h"

static const wxChar *FILENAME_GZ = _T("test.gz");
static const wxChar *TEST_DATA = _T("hello and hello and hello and hello and hello");

static void TestZlibStreamWrite()
{
    wxPuts(_T("*** Testing Zlib stream reading ***\n"));

    wxFileOutputStream fileOutStream(FILENAME_GZ);
    wxZlibOutputStream ostr(fileOutStream);
    wxPrintf(_T("Compressing the test string... "));
    ostr.Write(TEST_DATA, wxStrlen(TEST_DATA) + 1);
    if ( !ostr )
    {
        wxPuts(_T("(ERROR: failed)"));
    }
    else
    {
        wxPuts(_T("(ok)"));
    }

    wxPuts(_T("\n----- done ------"));
}

static void TestZlibStreamRead()
{
    wxPuts(_T("*** Testing Zlib stream reading ***\n"));

    wxFileInputStream fileInStream(FILENAME_GZ);
    wxZlibInputStream istr(fileInStream);
    wxPrintf(_T("Archive size: %u\n"), istr.GetSize());

    wxPuts(_T("Dumping the file:"));
    while ( !istr.Eof() )
    {
        putchar(istr.GetC());
        fflush(stdout);
    }

    wxPuts(_T("\n----- done ------"));
}

#endif // TEST_ZLIB

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_DATETIME

#include <math.h>

#include "wx/datetime.h"

// the test data
struct Date
{
    wxDateTime::wxDateTime_t day;
    wxDateTime::Month month;
    int year;
    wxDateTime::wxDateTime_t hour, min, sec;
    double jdn;
    wxDateTime::WeekDay wday;
    time_t gmticks, ticks;

    void Init(const wxDateTime::Tm& tm)
    {
        day = tm.mday;
        month = tm.mon;
        year = tm.year;
        hour = tm.hour;
        min = tm.min;
        sec = tm.sec;
        jdn = 0.0;
        gmticks = ticks = -1;
    }

    wxDateTime DT() const
        { return wxDateTime(day, month, year, hour, min, sec); }

    bool SameDay(const wxDateTime::Tm& tm) const
    {
        return day == tm.mday && month == tm.mon && year == tm.year;
    }

    wxString Format() const
    {
        wxString s;
        s.Printf(_T("%02d:%02d:%02d %10s %02d, %4d%s"),
                 hour, min, sec,
                 wxDateTime::GetMonthName(month).c_str(),
                 day,
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? _T("AD") : _T("BC"));
        return s;
    }

    wxString FormatDate() const
    {
        wxString s;
        s.Printf(_T("%02d-%s-%4d%s"),
                 day,
                 wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? _T("AD") : _T("BC"));
        return s;
    }
};

static const Date testDates[] =
{
    {  1, wxDateTime::Jan,  1970, 00, 00, 00, 2440587.5, wxDateTime::Thu,         0,     -3600 },
    {  7, wxDateTime::Feb,  2036, 00, 00, 00, 2464730.5, wxDateTime::Thu,        -1,        -1 },
    {  8, wxDateTime::Feb,  2036, 00, 00, 00, 2464731.5, wxDateTime::Fri,        -1,        -1 },
    {  1, wxDateTime::Jan,  2037, 00, 00, 00, 2465059.5, wxDateTime::Thu,        -1,        -1 },
    {  1, wxDateTime::Jan,  2038, 00, 00, 00, 2465424.5, wxDateTime::Fri,        -1,        -1 },
    { 21, wxDateTime::Jan,  2222, 00, 00, 00, 2532648.5, wxDateTime::Mon,        -1,        -1 },
    { 29, wxDateTime::May,  1976, 12, 00, 00, 2442928.0, wxDateTime::Sat, 202219200, 202212000 },
    { 29, wxDateTime::Feb,  1976, 00, 00, 00, 2442837.5, wxDateTime::Sun, 194400000, 194396400 },
    {  1, wxDateTime::Jan,  1900, 12, 00, 00, 2415021.0, wxDateTime::Mon,        -1,        -1 },
    {  1, wxDateTime::Jan,  1900, 00, 00, 00, 2415020.5, wxDateTime::Mon,        -1,        -1 },
    { 15, wxDateTime::Oct,  1582, 00, 00, 00, 2299160.5, wxDateTime::Fri,        -1,        -1 },
    {  4, wxDateTime::Oct,  1582, 00, 00, 00, 2299149.5, wxDateTime::Mon,        -1,        -1 },
    {  1, wxDateTime::Mar,     1, 00, 00, 00, 1721484.5, wxDateTime::Thu,        -1,        -1 },
    {  1, wxDateTime::Jan,     1, 00, 00, 00, 1721425.5, wxDateTime::Mon,        -1,        -1 },
    { 31, wxDateTime::Dec,     0, 00, 00, 00, 1721424.5, wxDateTime::Sun,        -1,        -1 },
    {  1, wxDateTime::Jan,     0, 00, 00, 00, 1721059.5, wxDateTime::Sat,        -1,        -1 },
    { 12, wxDateTime::Aug, -1234, 00, 00, 00, 1270573.5, wxDateTime::Fri,        -1,        -1 },
    { 12, wxDateTime::Aug, -4000, 00, 00, 00,  260313.5, wxDateTime::Sat,        -1,        -1 },
    { 24, wxDateTime::Nov, -4713, 00, 00, 00,      -0.5, wxDateTime::Mon,        -1,        -1 },
};

// this test miscellaneous static wxDateTime functions
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

    // leap year logic
    static const size_t nYears = 5;
    static const size_t years[2][nYears] =
    {
        // first line: the years to test
        { 1990, 1976, 2000, 2030, 1984, },

        // second line: true if leap, false otherwise
        { false, true, true, false, true }
    };

    for ( size_t n = 0; n < nYears; n++ )
    {
        int year = years[0][n];
        bool should = years[1][n] != 0,
             is = wxDateTime::IsLeapYear(year);

        wxPrintf(_T("Year %d is %sa leap year (%s)\n"),
               year,
               is ? "" : "not ",
               should == is ? "ok" : "ERROR");

        wxASSERT( should == wxDateTime::IsLeapYear(year) );
    }
}

// test constructing wxDateTime objects
static void TestTimeSet()
{
    wxPuts(_T("\n*** wxDateTime construction test ***"));

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d1 = testDates[n];
        wxDateTime dt = d1.DT();

        Date d2;
        d2.Init(dt.GetTm());

        wxString s1 = d1.Format(),
                 s2 = d2.Format();

        wxPrintf(_T("Date: %s == %s (%s)\n"),
                 s1.c_str(), s2.c_str(),
                 s1 == s2 ? _T("ok") : _T("ERROR"));
    }
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

static void TestTimeTicks()
{
    wxPuts(_T("\n*** wxDateTime ticks test ***"));

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        if ( d.ticks == -1 )
            continue;

        wxDateTime dt = d.DT();
        long ticks = (dt.GetValue() / 1000).ToLong();
        wxPrintf(_T("Ticks of %s:\t% 10ld"), d.Format().c_str(), ticks);
        if ( ticks == d.ticks )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %ld, delta = %ld)\n"),
                     (long)d.ticks, (long)(ticks - d.ticks));
        }

        dt = d.DT().ToTimezone(wxDateTime::GMT0);
        ticks = (dt.GetValue() / 1000).ToLong();
        wxPrintf(_T("GMtks of %s:\t% 10ld"), d.Format().c_str(), ticks);
        if ( ticks == d.gmticks )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %ld, delta = %ld)\n"),
                     (long)d.gmticks, (long)(ticks - d.gmticks));
        }
    }

    wxPuts(_T(""));
}

// test conversions to JDN &c
static void TestTimeJDN()
{
    wxPuts(_T("\n*** wxDateTime to JDN test ***"));

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);
        double jdn = dt.GetJulianDayNumber();

        wxPrintf(_T("JDN of %s is:\t% 15.6f"), d.Format().c_str(), jdn);
        if ( jdn == d.jdn )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %f, delta = %f)\n"),
                     d.jdn, jdn - d.jdn);
        }
    }
}

// test week days computation
static void TestTimeWDays()
{
    wxPuts(_T("\n*** wxDateTime weekday test ***"));

    // test GetWeekDay()
    size_t n;
    for ( n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        wxPrintf(_T("%s is: %s"),
                 d.Format().c_str(),
                 wxDateTime::GetWeekDayName(wday).c_str());
        if ( wday == d.wday )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %s)\n"),
                     wxDateTime::GetWeekDayName(d.wday).c_str());
        }
    }

    wxPuts(_T(""));

    // test SetToWeekDay()
    struct WeekDateTestData
    {
        Date date;                  // the real date (precomputed)
        int nWeek;                  // its week index in the month
        wxDateTime::WeekDay wday;   // the weekday
        wxDateTime::Month month;    // the month
        int year;                   // and the year

        wxString Format() const
        {
            wxString s, which;
            switch ( nWeek < -1 ? -nWeek : nWeek )
            {
                case 1: which = _T("first"); break;
                case 2: which = _T("second"); break;
                case 3: which = _T("third"); break;
                case 4: which = _T("fourth"); break;
                case 5: which = _T("fifth"); break;

                case -1: which = _T("last"); break;
            }

            if ( nWeek < -1 )
            {
                which += _T(" from end");
            }

            s.Printf(_T("The %s %s of %s in %d"),
                     which.c_str(),
                     wxDateTime::GetWeekDayName(wday).c_str(),
                     wxDateTime::GetMonthName(month).c_str(),
                     year);

            return s;
        }
    };

    // the array data was generated by the following python program
    /*
from DateTime import *
from whrandom import *
from string import *

monthNames = [ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
wdayNames = [ 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun' ]

week = DateTimeDelta(7)

for n in range(20):
    year = randint(1900, 2100)
    month = randint(1, 12)
    day = randint(1, 28)
    dt = DateTime(year, month, day)
    wday = dt.day_of_week

    countFromEnd = choice([-1, 1])
    weekNum = 0;

    while dt.month is month:
        dt = dt - countFromEnd * week
        weekNum = weekNum + countFromEnd

    data = { 'day': rjust(`day`, 2), 'month': monthNames[month - 1], 'year': year, 'weekNum': rjust(`weekNum`, 2), 'wday': wdayNames[wday] }

    print "{ { %(day)s, wxDateTime::%(month)s, %(year)d }, %(weekNum)d, "\
          "wxDateTime::%(wday)s, wxDateTime::%(month)s, %(year)d }," % data
    */

    static const WeekDateTestData weekDatesTestData[] =
    {
        { { 20, wxDateTime::Mar, 2045 },  3, wxDateTime::Mon, wxDateTime::Mar, 2045 },
        { {  5, wxDateTime::Jun, 1985 }, -4, wxDateTime::Wed, wxDateTime::Jun, 1985 },
        { { 12, wxDateTime::Nov, 1961 }, -3, wxDateTime::Sun, wxDateTime::Nov, 1961 },
        { { 27, wxDateTime::Feb, 2093 }, -1, wxDateTime::Fri, wxDateTime::Feb, 2093 },
        { {  4, wxDateTime::Jul, 2070 }, -4, wxDateTime::Fri, wxDateTime::Jul, 2070 },
        { {  2, wxDateTime::Apr, 1906 }, -5, wxDateTime::Mon, wxDateTime::Apr, 1906 },
        { { 19, wxDateTime::Jul, 2023 }, -2, wxDateTime::Wed, wxDateTime::Jul, 2023 },
        { {  5, wxDateTime::May, 1958 }, -4, wxDateTime::Mon, wxDateTime::May, 1958 },
        { { 11, wxDateTime::Aug, 1900 },  2, wxDateTime::Sat, wxDateTime::Aug, 1900 },
        { { 14, wxDateTime::Feb, 1945 },  2, wxDateTime::Wed, wxDateTime::Feb, 1945 },
        { { 25, wxDateTime::Jul, 1967 }, -1, wxDateTime::Tue, wxDateTime::Jul, 1967 },
        { {  9, wxDateTime::May, 1916 }, -4, wxDateTime::Tue, wxDateTime::May, 1916 },
        { { 20, wxDateTime::Jun, 1927 },  3, wxDateTime::Mon, wxDateTime::Jun, 1927 },
        { {  2, wxDateTime::Aug, 2000 },  1, wxDateTime::Wed, wxDateTime::Aug, 2000 },
        { { 20, wxDateTime::Apr, 2044 },  3, wxDateTime::Wed, wxDateTime::Apr, 2044 },
        { { 20, wxDateTime::Feb, 1932 }, -2, wxDateTime::Sat, wxDateTime::Feb, 1932 },
        { { 25, wxDateTime::Jul, 2069 },  4, wxDateTime::Thu, wxDateTime::Jul, 2069 },
        { {  3, wxDateTime::Apr, 1925 },  1, wxDateTime::Fri, wxDateTime::Apr, 1925 },
        { { 21, wxDateTime::Mar, 2093 },  3, wxDateTime::Sat, wxDateTime::Mar, 2093 },
        { {  3, wxDateTime::Dec, 2074 }, -5, wxDateTime::Mon, wxDateTime::Dec, 2074 },
    };

    static const wxChar *fmt = _T("%d-%b-%Y");

    wxDateTime dt;
    for ( n = 0; n < WXSIZEOF(weekDatesTestData); n++ )
    {
        const WeekDateTestData& wd = weekDatesTestData[n];

        dt.SetToWeekDay(wd.wday, wd.nWeek, wd.month, wd.year);

        wxPrintf(_T("%s is %s"), wd.Format().c_str(), dt.Format(fmt).c_str());

        const Date& d = wd.date;
        if ( d.SameDay(dt.GetTm()) )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            dt.Set(d.day, d.month, d.year);

            wxPrintf(_T(" (ERROR: should be %s)\n"), dt.Format(fmt).c_str());
        }
    }
}

// test the computation of (ISO) week numbers
static void TestTimeWNumber()
{
    wxPuts(_T("\n*** wxDateTime week number test ***"));

    struct WeekNumberTestData
    {
        Date date;                          // the date
        wxDateTime::wxDateTime_t week;      // the week number in the year
        wxDateTime::wxDateTime_t wmon;      // the week number in the month
        wxDateTime::wxDateTime_t wmon2;     // same but week starts with Sun
        wxDateTime::wxDateTime_t dnum;      // day number in the year
    };

    // data generated with the following python script:
    /*
from DateTime import *
from whrandom import *
from string import *

monthNames = [ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
wdayNames = [ 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun' ]

def GetMonthWeek(dt):
    weekNumMonth = dt.iso_week[1] - DateTime(dt.year, dt.month, 1).iso_week[1] + 1
    if weekNumMonth < 0:
        weekNumMonth = weekNumMonth + 53
    return weekNumMonth

def GetLastSundayBefore(dt):
    if dt.iso_week[2] == 7:
        return dt
    else:
        return dt - DateTimeDelta(dt.iso_week[2])

for n in range(20):
    year = randint(1900, 2100)
    month = randint(1, 12)
    day = randint(1, 28)
    dt = DateTime(year, month, day)
    dayNum = dt.day_of_year
    weekNum = dt.iso_week[1]
    weekNumMonth = GetMonthWeek(dt)

    weekNumMonth2 = 0
    dtSunday = GetLastSundayBefore(dt)

    while dtSunday >= GetLastSundayBefore(DateTime(dt.year, dt.month, 1)):
        weekNumMonth2 = weekNumMonth2 + 1
        dtSunday = dtSunday - DateTimeDelta(7)

    data = { 'day': rjust(`day`, 2), \
             'month': monthNames[month - 1], \
             'year': year, \
             'weekNum': rjust(`weekNum`, 2), \
             'weekNumMonth': weekNumMonth, \
             'weekNumMonth2': weekNumMonth2, \
             'dayNum': rjust(`dayNum`, 3) }

    print "        { { %(day)s, "\
          "wxDateTime::%(month)s, "\
          "%(year)d }, "\
          "%(weekNum)s, "\
          "%(weekNumMonth)s, "\
          "%(weekNumMonth2)s, "\
          "%(dayNum)s }," % data

    */
    static const WeekNumberTestData weekNumberTestDates[] =
    {
        { { 27, wxDateTime::Dec, 1966 }, 52, 5, 5, 361 },
        { { 22, wxDateTime::Jul, 1926 }, 29, 4, 4, 203 },
        { { 22, wxDateTime::Oct, 2076 }, 43, 4, 4, 296 },
        { {  1, wxDateTime::Jul, 1967 }, 26, 1, 1, 182 },
        { {  8, wxDateTime::Nov, 2004 }, 46, 2, 2, 313 },
        { { 21, wxDateTime::Mar, 1920 }, 12, 3, 4,  81 },
        { {  7, wxDateTime::Jan, 1965 },  1, 2, 2,   7 },
        { { 19, wxDateTime::Oct, 1999 }, 42, 4, 4, 292 },
        { { 13, wxDateTime::Aug, 1955 }, 32, 2, 2, 225 },
        { { 18, wxDateTime::Jul, 2087 }, 29, 3, 3, 199 },
        { {  2, wxDateTime::Sep, 2028 }, 35, 1, 1, 246 },
        { { 28, wxDateTime::Jul, 1945 }, 30, 5, 4, 209 },
        { { 15, wxDateTime::Jun, 1901 }, 24, 3, 3, 166 },
        { { 10, wxDateTime::Oct, 1939 }, 41, 3, 2, 283 },
        { {  3, wxDateTime::Dec, 1965 }, 48, 1, 1, 337 },
        { { 23, wxDateTime::Feb, 1940 },  8, 4, 4,  54 },
        { {  2, wxDateTime::Jan, 1987 },  1, 1, 1,   2 },
        { { 11, wxDateTime::Aug, 2079 }, 32, 2, 2, 223 },
        { {  2, wxDateTime::Feb, 2063 },  5, 1, 1,  33 },
        { { 16, wxDateTime::Oct, 1942 }, 42, 3, 3, 289 },
    };

    for ( size_t n = 0; n < WXSIZEOF(weekNumberTestDates); n++ )
    {
        const WeekNumberTestData& wn = weekNumberTestDates[n];
        const Date& d = wn.date;

        wxDateTime dt = d.DT();

        wxDateTime::wxDateTime_t
            week = dt.GetWeekOfYear(wxDateTime::Monday_First),
            wmon = dt.GetWeekOfMonth(wxDateTime::Monday_First),
            wmon2 = dt.GetWeekOfMonth(wxDateTime::Sunday_First),
            dnum = dt.GetDayOfYear();

        wxPrintf(_T("%s: the day number is %d"), d.FormatDate().c_str(), dnum);
        if ( dnum == wn.dnum )
        {
            wxPrintf(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %d)"), wn.dnum);
        }

        wxPrintf(_T(", week in month = %d"), wmon);
        if ( wmon != wn.wmon )
        {
            wxPrintf(_T(" (ERROR: should be %d)"), wn.wmon);
        }

        wxPrintf(_T(" or %d"), wmon2);
        if ( wmon2 == wn.wmon2 )
        {
            wxPrintf(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %d)"), wn.wmon2);
        }

        wxPrintf(_T(", week in year = %d"), week);
        if ( week != wn.week )
        {
            wxPrintf(_T(" (ERROR: should be %d)"), wn.week);
        }

        wxPutchar(_T('\n'));

        wxDateTime dt2(1, wxDateTime::Jan, d.year);
        dt2.SetToTheWeek(wn.week, dt.GetWeekDay());
        if ( dt2 != dt )
        {
            Date d2;
            d2.Init(dt2.GetTm());
            wxPrintf(_T("ERROR: SetToTheWeek() returned %s\n"),
                     d2.FormatDate().c_str());
        }
    }
}

// test DST calculations
static void TestTimeDST()
{
    wxPuts(_T("\n*** wxDateTime DST test ***"));

    wxPrintf(_T("DST is%s in effect now.\n\n"),
             wxDateTime::Now().IsDST() ? _T("") : _T(" not"));

    // taken from http://www.energy.ca.gov/daylightsaving.html
    static const Date datesDST[2][2004 - 1900 + 1] =
    {
        {
            { 1, wxDateTime::Apr, 1990 },
            { 7, wxDateTime::Apr, 1991 },
            { 5, wxDateTime::Apr, 1992 },
            { 4, wxDateTime::Apr, 1993 },
            { 3, wxDateTime::Apr, 1994 },
            { 2, wxDateTime::Apr, 1995 },
            { 7, wxDateTime::Apr, 1996 },
            { 6, wxDateTime::Apr, 1997 },
            { 5, wxDateTime::Apr, 1998 },
            { 4, wxDateTime::Apr, 1999 },
            { 2, wxDateTime::Apr, 2000 },
            { 1, wxDateTime::Apr, 2001 },
            { 7, wxDateTime::Apr, 2002 },
            { 6, wxDateTime::Apr, 2003 },
            { 4, wxDateTime::Apr, 2004 },
        },
        {
            { 28, wxDateTime::Oct, 1990 },
            { 27, wxDateTime::Oct, 1991 },
            { 25, wxDateTime::Oct, 1992 },
            { 31, wxDateTime::Oct, 1993 },
            { 30, wxDateTime::Oct, 1994 },
            { 29, wxDateTime::Oct, 1995 },
            { 27, wxDateTime::Oct, 1996 },
            { 26, wxDateTime::Oct, 1997 },
            { 25, wxDateTime::Oct, 1998 },
            { 31, wxDateTime::Oct, 1999 },
            { 29, wxDateTime::Oct, 2000 },
            { 28, wxDateTime::Oct, 2001 },
            { 27, wxDateTime::Oct, 2002 },
            { 26, wxDateTime::Oct, 2003 },
            { 31, wxDateTime::Oct, 2004 },
        }
    };

    int year;
    for ( year = 1990; year < 2005; year++ )
    {
        wxDateTime dtBegin = wxDateTime::GetBeginDST(year, wxDateTime::USA),
                   dtEnd = wxDateTime::GetEndDST(year, wxDateTime::USA);

        wxPrintf(_T("DST period in the US for year %d: from %s to %s"),
                 year, dtBegin.Format().c_str(), dtEnd.Format().c_str());

        size_t n = year - 1990;
        const Date& dBegin = datesDST[0][n];
        const Date& dEnd = datesDST[1][n];

        if ( dBegin.SameDay(dtBegin.GetTm()) && dEnd.SameDay(dtEnd.GetTm()) )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %s %d to %s %d)\n"),
                     wxDateTime::GetMonthName(dBegin.month).c_str(), dBegin.day,
                     wxDateTime::GetMonthName(dEnd.month).c_str(), dEnd.day);
        }
    }

    wxPuts(_T(""));

    for ( year = 1990; year < 2005; year++ )
    {
        wxPrintf(_T("DST period in Europe for year %d: from %s to %s\n"),
                 year,
                 wxDateTime::GetBeginDST(year, wxDateTime::Country_EEC).Format().c_str(),
                 wxDateTime::GetEndDST(year, wxDateTime::Country_EEC).Format().c_str());
    }
}

// test wxDateTime -> text conversion
static void TestTimeFormat()
{
    wxPuts(_T("\n*** wxDateTime formatting test ***"));

    // some information may be lost during conversion, so store what kind
    // of info should we recover after a round trip
    enum CompareKind
    {
        CompareNone,        // don't try comparing
        CompareBoth,        // dates and times should be identical
        CompareDate,        // dates only
        CompareTime         // time only
    };

    static const struct
    {
        CompareKind compareKind;
        const wxChar *format;
    } formatTestFormats[] =
    {
       { CompareBoth, _T("---> %c") },
       { CompareDate, _T("Date is %A, %d of %B, in year %Y") },
       { CompareBoth, _T("Date is %x, time is %X") },
       { CompareTime, _T("Time is %H:%M:%S or %I:%M:%S %p") },
       { CompareNone, _T("The day of year: %j, the week of year: %W") },
       { CompareDate, _T("ISO date without separators: %Y%m%d") },
    };

    static const Date formatTestDates[] =
    {
        { 29, wxDateTime::May, 1976, 18, 30, 00 },
        { 31, wxDateTime::Dec, 1999, 23, 30, 00 },
#if 0
        // this test can't work for other centuries because it uses two digit
        // years in formats, so don't even try it
        { 29, wxDateTime::May, 2076, 18, 30, 00 },
        { 29, wxDateTime::Feb, 2400, 02, 15, 25 },
        { 01, wxDateTime::Jan,  -52, 03, 16, 47 },
#endif
    };

    // an extra test (as it doesn't depend on date, don't do it in the loop)
    wxPrintf(_T("%s\n"), wxDateTime::Now().Format(_T("Our timezone is %Z")).c_str());

    for ( size_t d = 0; d < WXSIZEOF(formatTestDates) + 1; d++ )
    {
        wxPuts(_T(""));

        wxDateTime dt = d == 0 ? wxDateTime::Now() : formatTestDates[d - 1].DT();
        for ( size_t n = 0; n < WXSIZEOF(formatTestFormats); n++ )
        {
            wxString s = dt.Format(formatTestFormats[n].format);
            wxPrintf(_T("%s"), s.c_str());

            // what can we recover?
            int kind = formatTestFormats[n].compareKind;

            // convert back
            wxDateTime dt2;
            const wxChar *result = dt2.ParseFormat(s, formatTestFormats[n].format);
            if ( !result )
            {
                // converion failed - should it have?
                if ( kind == CompareNone )
                    wxPuts(_T(" (ok)"));
                else
                    wxPuts(_T(" (ERROR: conversion back failed)"));
            }
            else if ( *result )
            {
                // should have parsed the entire string
                wxPuts(_T(" (ERROR: conversion back stopped too soon)"));
            }
            else
            {
                bool equal = false; // suppress compilaer warning
                switch ( kind )
                {
                    case CompareBoth:
                        equal = dt2 == dt;
                        break;

                    case CompareDate:
                        equal = dt.IsSameDate(dt2);
                        break;

                    case CompareTime:
                        equal = dt.IsSameTime(dt2);
                        break;
                }

                if ( !equal )
                {
                    wxPrintf(_T(" (ERROR: got back '%s' instead of '%s')\n"),
                           dt2.Format().c_str(), dt.Format().c_str());
                }
                else
                {
                    wxPuts(_T(" (ok)"));
                }
            }
        }
    }
}

// test text -> wxDateTime conversion
static void TestTimeParse()
{
    wxPuts(_T("\n*** wxDateTime parse test ***"));

    struct ParseTestData
    {
        const wxChar *format;
        Date date;
        bool good;
    };

    static const ParseTestData parseTestDates[] =
    {
        { _T("Sat, 18 Dec 1999 00:46:40 +0100"), { 18, wxDateTime::Dec, 1999, 00, 46, 40 }, true },
        { _T("Wed, 1 Dec 1999 05:17:20 +0300"),  {  1, wxDateTime::Dec, 1999, 03, 17, 20 }, true },
    };

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const wxChar *format = parseTestDates[n].format;

        wxPrintf(_T("%s => "), format);

        wxDateTime dt;
        if ( dt.ParseRfc822Date(format) )
        {
            wxPrintf(_T("%s "), dt.Format().c_str());

            if ( parseTestDates[n].good )
            {
                wxDateTime dtReal = parseTestDates[n].date.DT();
                if ( dt == dtReal )
                {
                    wxPuts(_T("(ok)"));
                }
                else
                {
                    wxPrintf(_T("(ERROR: should be %s)\n"), dtReal.Format().c_str());
                }
            }
            else
            {
                wxPuts(_T("(ERROR: bad format)"));
            }
        }
        else
        {
            wxPrintf(_T("bad format (%s)\n"),
                   parseTestDates[n].good ? "ERROR" : "ok");
        }
    }
}

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
            s.Printf(_T("%g"), sqrt(i));
        }

        if ( !(i % 100) )
            putchar('.');
    }
    wxPuts(_T(", done"));

    dt1 = dt2;
    dt2 = wxDateTime::UNow();
    wxPrintf(_T("UNow = %s\n"), dt2.Format(_T("%H:%M:%S:%l")).c_str());

    wxPrintf(_T("Loop executed in %s ms\n"), (dt2 - dt1).Format(_T("%l")).c_str());

    wxPuts(_T("\n*** done ***"));
}

static void TestTimeArithmetics()
{
    wxPuts(_T("\n*** testing arithmetic operations on wxDateTime ***"));

    static const struct ArithmData
    {
        ArithmData(const wxDateSpan& sp, const wxChar *nam)
            : span(sp), name(nam) { }

        wxDateSpan span;
        const wxChar *name;
    } testArithmData[] =
    {
        ArithmData(wxDateSpan::Day(), _T("day")),
        ArithmData(wxDateSpan::Week(), _T("week")),
        ArithmData(wxDateSpan::Month(), _T("month")),
        ArithmData(wxDateSpan::Year(), _T("year")),
        ArithmData(wxDateSpan(1, 2, 3, 4), _T("year, 2 months, 3 weeks, 4 days")),
    };

    wxDateTime dt(29, wxDateTime::Dec, 1999), dt1, dt2;

    for ( size_t n = 0; n < WXSIZEOF(testArithmData); n++ )
    {
        wxDateSpan span = testArithmData[n].span;
        dt1 = dt + span;
        dt2 = dt - span;

        const wxChar *name = testArithmData[n].name;
        wxPrintf(_T("%s + %s = %s, %s - %s = %s\n"),
               dt.FormatISODate().c_str(), name, dt1.FormatISODate().c_str(),
               dt.FormatISODate().c_str(), name, dt2.FormatISODate().c_str());

        wxPrintf(_T("Going back: %s"), (dt1 - span).FormatISODate().c_str());
        if ( dt1 - span == dt )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %s)\n"), dt.FormatISODate().c_str());
        }

        wxPrintf(_T("Going forward: %s"), (dt2 + span).FormatISODate().c_str());
        if ( dt2 + span == dt )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %s)\n"), dt.FormatISODate().c_str());
        }

        wxPrintf(_T("Double increment: %s"), (dt2 + 2*span).FormatISODate().c_str());
        if ( dt2 + 2*span == dt1 )
        {
            wxPuts(_T(" (ok)"));
        }
        else
        {
            wxPrintf(_T(" (ERROR: should be %s)\n"), dt2.FormatISODate().c_str());
        }

        wxPuts(_T(""));
    }
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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
}

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

    wxFileInputStream fsIn(_T("testdata.fc"));
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

        putchar(m_ch);
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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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

    wxPuts(_T(""));
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

WX_DEFINE_ARRAY(wxThread *, ArrayThreads);

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
// arrays
// ----------------------------------------------------------------------------

#ifdef TEST_ARRAYS

#include "wx/dynarray.h"

typedef unsigned short ushort;

#define DefineCompare(name, T)                                                \
                                                                              \
int wxCMPFUNC_CONV name ## CompareValues(T first, T second)                   \
{                                                                             \
    return first - second;                                                    \
}                                                                             \
                                                                              \
int wxCMPFUNC_CONV name ## Compare(T* first, T* second)                       \
{                                                                             \
    return *first - *second;                                                  \
}                                                                             \
                                                                              \
int wxCMPFUNC_CONV name ## RevCompare(T* first, T* second)                    \
{                                                                             \
    return *second - *first;                                                  \
}                                                                             \

DefineCompare(UShort, ushort);
DefineCompare(Int, int);

// test compilation of all macros
WX_DEFINE_ARRAY_SHORT(ushort, wxArrayUShort);
WX_DEFINE_SORTED_ARRAY_SHORT(ushort, wxSortedArrayUShortNoCmp);
WX_DEFINE_SORTED_ARRAY_CMP_SHORT(ushort, UShortCompareValues, wxSortedArrayUShort);
WX_DEFINE_SORTED_ARRAY_CMP_INT(int, IntCompareValues, wxSortedArrayInt);

WX_DECLARE_OBJARRAY(Bar, ArrayBars);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayBars);

static void PrintArray(const wxChar* name, const wxArrayString& array)
{
    wxPrintf(_T("Dump of the array '%s'\n"), name);

    size_t nCount = array.GetCount();
    for ( size_t n = 0; n < nCount; n++ )
    {
        wxPrintf(_T("\t%s[%u] = '%s'\n"), name, n, array[n].c_str());
    }
}

int wxCMPFUNC_CONV StringLenCompare(const wxString& first,
                                    const wxString& second)
{
    return first.length() - second.length();
}

#define TestArrayOf(name)                                                     \
                                                                              \
static void PrintArray(const wxChar* name, const wxSortedArray##name & array) \
{                                                                             \
    wxPrintf(_T("Dump of the array '%s'\n"), name);                           \
                                                                              \
    size_t nCount = array.GetCount();                                         \
    for ( size_t n = 0; n < nCount; n++ )                                     \
    {                                                                         \
        wxPrintf(_T("\t%s[%u] = %d\n"), name, n, array[n]);                   \
    }                                                                         \
}                                                                             \
                                                                              \
static void PrintArray(const wxChar* name, const wxArray##name & array)       \
{                                                                             \
    wxPrintf(_T("Dump of the array '%s'\n"), name);                           \
                                                                              \
    size_t nCount = array.GetCount();                                         \
    for ( size_t n = 0; n < nCount; n++ )                                     \
    {                                                                         \
        wxPrintf(_T("\t%s[%u] = %d\n"), name, n, array[n]);                   \
    }                                                                         \
}                                                                             \
                                                                              \
static void TestArrayOf ## name ## s()                                        \
{                                                                             \
    wxPrintf(_T("*** Testing wxArray%s ***\n"), #name);                       \
                                                                              \
    wxArray##name a;                                                          \
    a.Add(1);                                                                 \
    a.Add(17,2);                                                              \
    a.Add(5,3);                                                               \
    a.Add(3,4);                                                               \
                                                                              \
    wxPuts(_T("Initially:"));                                                 \
    PrintArray(_T("a"), a);                                                   \
                                                                              \
    wxPuts(_T("After sort:"));                                                \
    a.Sort(name ## Compare);                                                  \
    PrintArray(_T("a"), a);                                                   \
                                                                              \
    wxPuts(_T("After reverse sort:"));                                        \
    a.Sort(name ## RevCompare);                                               \
    PrintArray(_T("a"), a);                                                   \
                                                                              \
    wxSortedArray##name b;                                                    \
    b.Add(1);                                                                 \
    b.Add(17);                                                                \
    b.Add(5);                                                                 \
    b.Add(3);                                                                 \
                                                                              \
    wxPuts(_T("Sorted array initially:"));                                    \
    PrintArray(_T("b"), b);                                                       \
}

TestArrayOf(UShort);
TestArrayOf(Int);

static void TestArrayOfObjects()
{
    wxPuts(_T("*** Testing wxObjArray ***\n"));

    {
        ArrayBars bars;
        Bar bar("second bar (two copies!)");

        wxPrintf(_T("Initially: %u objects in the array, %u objects total.\n"),
               bars.GetCount(), Bar::GetNumber());

        bars.Add(new Bar("first bar"));
        bars.Add(bar,2);

        wxPrintf(_T("Now: %u objects in the array, %u objects total.\n"),
               bars.GetCount(), Bar::GetNumber());

        bars.RemoveAt(1, bars.GetCount() - 1);

        wxPrintf(_T("After removing all but first element: %u objects in the ")
                 _T("array, %u objects total.\n"),
               bars.GetCount(), Bar::GetNumber());

        bars.Empty();

        wxPrintf(_T("After Empty(): %u objects in the array, %u objects total.\n"),
               bars.GetCount(), Bar::GetNumber());
    }

    wxPrintf(_T("Finally: no more objects in the array, %u objects total.\n"),
           Bar::GetNumber());
}

#endif // TEST_ARRAYS

// ----------------------------------------------------------------------------
// strings
// ----------------------------------------------------------------------------

#ifdef TEST_STRINGS

#include "wx/timer.h"
#include "wx/tokenzr.h"

static void TestStringConstruction()
{
    wxPuts(_T("*** Testing wxString constructores ***"));

    #define TEST_CTOR(args, res)                                               \
        {                                                                      \
            wxString s args ;                                                  \
            wxPrintf(_T("wxString%s = %s "), #args, s.c_str());                      \
            if ( s == res )                                                    \
            {                                                                  \
                wxPuts(_T("(ok)"));                                                  \
            }                                                                  \
            else                                                               \
            {                                                                  \
                wxPrintf(_T("(ERROR: should be %s)\n"), res);                        \
            }                                                                  \
        }

    TEST_CTOR((_T('Z'), 4), _T("ZZZZ"));
    TEST_CTOR((_T("Hello"), 4), _T("Hell"));
    TEST_CTOR((_T("Hello"), 5), _T("Hello"));
    // TEST_CTOR((_T("Hello"), 6), _T("Hello")); -- should give assert failure

    static const wxChar *s = _T("?really!");
    const wxChar *start = wxStrchr(s, _T('r'));
    const wxChar *end = wxStrchr(s, _T('!'));
    TEST_CTOR((start, end), _T("really"));

    wxPuts(_T(""));
}

static void TestString()
{
    wxStopWatch sw;

    wxString a, b, c;

    a.reserve (128);
    b.reserve (128);
    c.reserve (128);

    for (int i = 0; i < 1000000; ++i)
    {
        a = "Hello";
        b = " world";
        c = "! How'ya doin'?";
        a += b;
        a += c;
        c = "Hello world! What's up?";
        if (c != a)
            c = "Doh!";
    }

    wxPrintf(_T("TestString elapsed time: %ld\n"), sw.Time());
}

static void TestPChar()
{
    wxStopWatch sw;

    wxChar a [128];
    wxChar b [128];
    wxChar c [128];

    for (int i = 0; i < 1000000; ++i)
    {
        wxStrcpy (a, _T("Hello"));
        wxStrcpy (b, _T(" world"));
        wxStrcpy (c, _T("! How'ya doin'?"));
        wxStrcat (a, b);
        wxStrcat (a, c);
        wxStrcpy (c, _T("Hello world! What's up?"));
        if (wxStrcmp (c, a) == 0)
            wxStrcpy (c, _T("Doh!"));
    }

    wxPrintf(_T("TestPChar elapsed time: %ld\n"), sw.Time());
}

static void TestStringSub()
{
    wxString s("Hello, world!");

    wxPuts(_T("*** Testing wxString substring extraction ***"));

    wxPrintf(_T("String = '%s'\n"), s.c_str());
    wxPrintf(_T("Left(5) = '%s'\n"), s.Left(5).c_str());
    wxPrintf(_T("Right(6) = '%s'\n"), s.Right(6).c_str());
    wxPrintf(_T("Mid(3, 5) = '%s'\n"), s(3, 5).c_str());
    wxPrintf(_T("Mid(3) = '%s'\n"), s.Mid(3).c_str());
    wxPrintf(_T("substr(3, 5) = '%s'\n"), s.substr(3, 5).c_str());
    wxPrintf(_T("substr(3) = '%s'\n"), s.substr(3).c_str());

    static const wxChar *prefixes[] =
    {
        _T("Hello"),
        _T("Hello, "),
        _T("Hello, world!"),
        _T("Hello, world!!!"),
        _T(""),
        _T("Goodbye"),
        _T("Hi"),
    };

    for ( size_t n = 0; n < WXSIZEOF(prefixes); n++ )
    {
        wxString prefix = prefixes[n], rest;
        bool rc = s.StartsWith(prefix, &rest);
        wxPrintf(_T("StartsWith('%s') = %s"), prefix.c_str(), rc ? _T("true") : _T("false"));
        if ( rc )
        {
            wxPrintf(_T(" (the rest is '%s')\n"), rest.c_str());
        }
        else
        {
            putchar('\n');
        }
    }

    wxPuts(_T(""));
}

static void TestStringFormat()
{
    wxPuts(_T("*** Testing wxString formatting ***"));

    wxString s;
    s.Printf(_T("%03d"), 18);

    wxPrintf(_T("Number 18: %s\n"), wxString::Format(_T("%03d"), 18).c_str());
    wxPrintf(_T("Number 18: %s\n"), s.c_str());

    wxPuts(_T(""));
}

// returns "not found" for npos, value for all others
static wxString PosToString(size_t res)
{
    wxString s = res == wxString::npos ? wxString(_T("not found"))
                                       : wxString::Format(_T("%u"), res);
    return s;
}

static void TestStringFind()
{
    wxPuts(_T("*** Testing wxString find() functions ***"));

    static const wxChar *strToFind = _T("ell");
    static const struct StringFindTest
    {
        const wxChar *str;
        size_t        start,
                      result;   // of searching "ell" in str
    } findTestData[] =
    {
        { _T("Well, hello world"),  0, 1 },
        { _T("Well, hello world"),  6, 7 },
        { _T("Well, hello world"),  9, wxString::npos },
    };

    for ( size_t n = 0; n < WXSIZEOF(findTestData); n++ )
    {
        const StringFindTest& ft = findTestData[n];
        size_t res = wxString(ft.str).find(strToFind, ft.start);

        wxPrintf(_T("Index of '%s' in '%s' starting from %u is %s "),
               strToFind, ft.str, ft.start, PosToString(res).c_str());

        size_t resTrue = ft.result;
        if ( res == resTrue )
        {
            wxPuts(_T("(ok)"));
        }
        else
        {
            wxPrintf(_T("(ERROR: should be %s)\n"),
                   PosToString(resTrue).c_str());
        }
    }

    wxPuts(_T(""));
}

static void TestStringTokenizer()
{
    wxPuts(_T("*** Testing wxStringTokenizer ***"));

    static const wxChar *modeNames[] =
    {
        _T("default"),
        _T("return empty"),
        _T("return all empty"),
        _T("with delims"),
        _T("like strtok"),
    };

    static const struct StringTokenizerTest
    {
        const wxChar *str;              // string to tokenize
        const wxChar *delims;           // delimiters to use
        size_t        count;            // count of token
        wxStringTokenizerMode mode;     // how should we tokenize it
    } tokenizerTestData[] =
    {
        { _T(""), _T(" "), 0 },
        { _T("Hello, world"), _T(" "), 2 },
        { _T("Hello,   world  "), _T(" "), 2 },
        { _T("Hello, world"), _T(","), 2 },
        { _T("Hello, world!"), _T(",!"), 2 },
        { _T("Hello,, world!"), _T(",!"), 3 },
        { _T("Hello, world!"), _T(",!"), 3, wxTOKEN_RET_EMPTY_ALL },
        { _T("username:password:uid:gid:gecos:home:shell"), _T(":"), 7 },
        { _T("1 \t3\t4  6   "), wxDEFAULT_DELIMITERS, 4 },
        { _T("1 \t3\t4  6   "), wxDEFAULT_DELIMITERS, 6, wxTOKEN_RET_EMPTY },
        { _T("1 \t3\t4  6   "), wxDEFAULT_DELIMITERS, 9, wxTOKEN_RET_EMPTY_ALL },
        { _T("01/02/99"), _T("/-"), 3 },
        { _T("01-02/99"), _T("/-"), 3, wxTOKEN_RET_DELIMS },
    };

    for ( size_t n = 0; n < WXSIZEOF(tokenizerTestData); n++ )
    {
        const StringTokenizerTest& tt = tokenizerTestData[n];
        wxStringTokenizer tkz(tt.str, tt.delims, tt.mode);

        size_t count = tkz.CountTokens();
        wxPrintf(_T("String '%s' has %u tokens delimited by '%s' (mode = %s) "),
               MakePrintable(tt.str).c_str(),
               count,
               MakePrintable(tt.delims).c_str(),
               modeNames[tkz.GetMode()]);
        if ( count == tt.count )
        {
            wxPuts(_T("(ok)"));
        }
        else
        {
            wxPrintf(_T("(ERROR: should be %u)\n"), tt.count);

            continue;
        }

        // if we emulate strtok(), check that we do it correctly
        wxChar *buf, *s = NULL, *last;

        if ( tkz.GetMode() == wxTOKEN_STRTOK )
        {
            buf = new wxChar[wxStrlen(tt.str) + 1];
            wxStrcpy(buf, tt.str);

            s = wxStrtok(buf, tt.delims, &last);
        }
        else
        {
            buf = NULL;
        }

        // now show the tokens themselves
        size_t count2 = 0;
        while ( tkz.HasMoreTokens() )
        {
            wxString token = tkz.GetNextToken();

            wxPrintf(_T("\ttoken %u: '%s'"),
                   ++count2,
                   MakePrintable(token).c_str());

            if ( buf )
            {
                if ( token == s )
                {
                    wxPuts(_T(" (ok)"));
                }
                else
                {
                    wxPrintf(_T(" (ERROR: should be %s)\n"), s);
                }

                s = wxStrtok(NULL, tt.delims, &last);
            }
            else
            {
                // nothing to compare with
                wxPuts(_T(""));
            }
        }

        if ( count2 != count )
        {
            wxPuts(_T("\tERROR: token count mismatch"));
        }

        delete [] buf;
    }

    wxPuts(_T(""));
}

static void TestStringReplace()
{
    wxPuts(_T("*** Testing wxString::replace ***"));

    static const struct StringReplaceTestData
    {
        const wxChar *original;     // original test string
        size_t start, len;          // the part to replace
        const wxChar *replacement;  // the replacement string
        const wxChar *result;       // and the expected result
    } stringReplaceTestData[] =
    {
        { _T("012-AWORD-XYZ"), 4, 5, _T("BWORD"), _T("012-BWORD-XYZ") },
        { _T("increase"), 0, 2, _T("de"), _T("decrease") },
        { _T("wxWindow"), 8, 0, _T("s"), _T("wxWindows") },
        { _T("foobar"), 3, 0, _T("-"), _T("foo-bar") },
        { _T("barfoo"), 0, 6, _T("foobar"), _T("foobar") },
    };

    for ( size_t n = 0; n < WXSIZEOF(stringReplaceTestData); n++ )
    {
        const StringReplaceTestData data = stringReplaceTestData[n];

        wxString original = data.original;
        original.replace(data.start, data.len, data.replacement);

        wxPrintf(_T("wxString(\"%s\").replace(%u, %u, %s) = %s "),
                 data.original, data.start, data.len, data.replacement,
                 original.c_str());

        if ( original == data.result )
        {
            wxPuts(_T("(ok)"));
        }
        else
        {
            wxPrintf(_T("(ERROR: should be '%s')\n"), data.result);
        }
    }

    wxPuts(_T(""));
}

static void TestStringMatch()
{
    wxPuts(_T("*** Testing wxString::Matches() ***"));

    static const struct StringMatchTestData
    {
        const wxChar *text;
        const wxChar *wildcard;
        bool matches;
    } stringMatchTestData[] =
    {
        { _T("foobar"), _T("foo*"), 1 },
        { _T("foobar"), _T("*oo*"), 1 },
        { _T("foobar"), _T("*bar"), 1 },
        { _T("foobar"), _T("??????"), 1 },
        { _T("foobar"), _T("f??b*"), 1 },
        { _T("foobar"), _T("f?b*"), 0 },
        { _T("foobar"), _T("*goo*"), 0 },
        { _T("foobar"), _T("*foo"), 0 },
        { _T("foobarfoo"), _T("*foo"), 1 },
        { _T(""), _T("*"), 1 },
        { _T(""), _T("?"), 0 },
    };

    for ( size_t n = 0; n < WXSIZEOF(stringMatchTestData); n++ )
    {
        const StringMatchTestData& data = stringMatchTestData[n];
        bool matches = wxString(data.text).Matches(data.wildcard);
        wxPrintf(_T("'%s' %s '%s' (%s)\n"),
                 data.wildcard,
                 matches ? _T("matches") : _T("doesn't match"),
                 data.text,
                 matches == data.matches ? _T("ok") : _T("ERROR"));
    }

    wxPuts(_T(""));
}

#endif // TEST_STRINGS

// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

#ifdef TEST_SNGLINST
    #include "wx/snglinst.h"
#endif // TEST_SNGLINST

int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(wxBuildOptions());

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");

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

#ifdef TEST_CHARSET
    TestCharset();
#endif // TEST_CHARSET

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

#if wxUSE_UNICODE
    wxChar **wargv = new wxChar *[argc + 1];

    {
        for ( int n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wargv[n] = wxStrdup(warg);
        }

        wargv[n] = NULL;
    }

    #define argv wargv
#endif // wxUSE_UNICODE

    wxCmdLineParser parser(cmdLineDesc, argc, argv);

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wargv[n]);

        delete [] wargv;
    }
#endif // wxUSE_UNICODE

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

#ifdef TEST_STRINGS
    if ( TEST_ALL )
    {
        TestPChar();
        TestString();
        TestStringSub();
        TestStringConstruction();
        TestStringFormat();
        TestStringFind();
        TestStringTokenizer();
        TestStringReplace();
    }
    else
    {
        TestStringMatch();
    }
#endif // TEST_STRINGS

#ifdef TEST_ARRAYS
    if ( TEST_ALL )
    {
        wxArrayString a1;
        a1.Add(_T("tiger"));
        a1.Add(_T("cat"));
        a1.Add(_T("lion"), 3);
        a1.Add(_T("dog"));
        a1.Add(_T("human"));
        a1.Add(_T("ape"));

        wxPuts(_T("*** Initially:"));

        PrintArray(_T("a1"), a1);

        wxArrayString a2(a1);
        PrintArray(_T("a2"), a2);

        wxSortedArrayString a3(a1);
        PrintArray(_T("a3"), a3);

        wxPuts(_T("*** After deleting three strings from a1"));
        a1.Remove(2,3);

        PrintArray(_T("a1"), a1);
        PrintArray(_T("a2"), a2);
        PrintArray(_T("a3"), a3);

        wxPuts(_T("*** After reassigning a1 to a2 and a3"));
        a3 = a2 = a1;
        PrintArray(_T("a2"), a2);
        PrintArray(_T("a3"), a3);

        wxPuts(_T("*** After sorting a1"));
        a1.Sort();
        PrintArray(_T("a1"), a1);

        wxPuts(_T("*** After sorting a1 in reverse order"));
        a1.Sort(true);
        PrintArray(_T("a1"), a1);

        wxPuts(_T("*** After sorting a1 by the string length"));
        a1.Sort(StringLenCompare);
        PrintArray(_T("a1"), a1);

        TestArrayOfObjects();
        TestArrayOfUShorts();
    }

    TestArrayOfInts();
#endif // TEST_ARRAYS

#ifdef TEST_DIR
    if ( TEST_ALL )
    {
        TestDirExists();
        TestDirEnum();
    }
    TestDirTraverse();
#endif // TEST_DIR

#ifdef TEST_DLLLOADER
    TestDllLoad();
#endif // TEST_DLLLOADER

#ifdef TEST_ENVIRON
    TestEnvironment();
#endif // TEST_ENVIRON

#ifdef TEST_EXECUTE
    TestExecute();
#endif // TEST_EXECUTE

#ifdef TEST_FILECONF
    TestFileConfRead();
#endif // TEST_FILECONF

#ifdef TEST_LIST
    TestListCtor();
#endif // TEST_LIST

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
    if ( TEST_ALL )
    {
        TestFileRead();
        TestTextFileRead();
        TestFileCopy();
    }
#endif // TEST_FILE

#ifdef TEST_FILENAME
    if ( 0 )
    {
        wxFileName fn;
        fn.Assign(_T("c:\\foo"), _T("bar.baz"));
        fn.Assign(_T("/u/os9-port/Viewer/tvision/WEI2HZ-3B3-14_05-04-00MSC1.asc"));

        DumpFileName(fn);
    }

    TestFileNameConstruction();
    if ( TEST_ALL )
    {
        TestFileNameConstruction();
        TestFileNameMakeRelative();
        TestFileNameMakeAbsolute();
        TestFileNameSplit();
        TestFileNameTemp();
        TestFileNameCwd();
        TestFileNameComparison();
        TestFileNameOperations();
    }
#endif // TEST_FILENAME

#ifdef TEST_FILETIME
    TestFileGetTimes();
    if ( 0 )
    TestFileSetTimes();
#endif // TEST_FILETIME

#ifdef TEST_FTP
    wxLog::AddTraceMask(FTP_TRACE_MASK);
    if ( TestFtpConnect() )
    {
        if ( TEST_ALL )
        {
            TestFtpList();
            TestFtpDownload();
            TestFtpMisc();
            TestFtpFileSize();
            TestFtpUpload();
        }

        if ( TEST_INTERACTIVE )
            TestFtpInteractive();
    }
    //else: connecting to the FTP server failed

    if ( 0 )
        TestFtpWuFtpd();
#endif // TEST_FTP

#ifdef TEST_LONGLONG
    // seed pseudo random generator
    srand((unsigned)time(NULL));

    if ( 0 )
    {
        TestSpeed();
    }

    if ( TEST_ALL )
    {
        TestMultiplication();
        TestDivision();
        TestAddition();
        TestLongLongConversion();
        TestBitOperations();
        TestLongLongComparison();
        TestLongLongToString();
        TestLongLongPrintf();
    }
#endif // TEST_LONGLONG

#ifdef TEST_HASH
    TestHash();
#endif // TEST_HASH

#ifdef TEST_HASHMAP
    TestHashMap();
#endif // TEST_HASHMAP

#ifdef TEST_MIME
    wxLog::AddTraceMask(_T("mime"));
    if ( TEST_ALL )
    {
        TestMimeEnum();
        TestMimeOverride();
        TestMimeAssociate();
    }
    TestMimeFilename();
#endif // TEST_MIME

#ifdef TEST_INFO_FUNCTIONS
    if ( TEST_ALL )
    {
        TestOsInfo();
        TestUserInfo();

        if ( TEST_INTERACTIVE )
            TestDiskInfo();
    }
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
    TestRegConfWrite();
#endif // TEST_REGCONF

#ifdef TEST_REGEX
    // TODO: write a real test using src/regex/tests file
    if ( TEST_ALL )
    {
        TestRegExCompile();
        TestRegExMatch();
        TestRegExSubmatch();
        TestRegExReplacement();

        if ( TEST_INTERACTIVE )
            TestRegExInteractive();
    }
#endif // TEST_REGEX

#ifdef TEST_REGISTRY
    TestRegistryRead();
    TestRegistryAssociation();
#endif // TEST_REGISTRY

#ifdef TEST_SOCKETS
    TestSocketServer();
    TestSocketClient();
#endif // TEST_SOCKETS

#ifdef TEST_STREAMS
    if ( TEST_ALL )
    {
        TestFileStream();
    }
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

        TestDetachedThreads();
    if ( TEST_ALL )
    {
        TestJoinableThreads();
        TestThreadSuspend();
        TestThreadDelete();
        TestThreadConditions();
        TestThreadExec();
        TestSemaphore();
    }
#endif // TEST_THREADS

#ifdef TEST_TIMER
    TestStopWatch();
#endif // TEST_TIMER

#ifdef TEST_DATETIME
    if ( TEST_ALL )
    {
        TestTimeSet();
        TestTimeStatic();
        TestTimeRange();
        TestTimeZones();
        TestTimeTicks();
        TestTimeJDN();
        TestTimeDST();
        TestTimeWDays();
        TestTimeWNumber();
        TestTimeParse();
        TestTimeArithmetics();
        TestTimeHolidays();
        TestTimeFormat();
        TestTimeSpanFormat();
        TestTimeMS();

        TestTimeZoneBug();
    }

    TestTimeWNumber();

    if ( TEST_INTERACTIVE )
        TestDateTimeInteractive();
#endif // TEST_DATETIME

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

#ifdef TEST_UNICODE
    TestUnicodeToFromAscii();
#endif // TEST_UNICODE

#ifdef TEST_WCHAR
    TestUtf8();
    TestEncodingConverter();
#endif // TEST_WCHAR

#ifdef TEST_ZIP
    TestZipStreamRead();
    TestZipFileSystem();
#endif // TEST_ZIP

#ifdef TEST_ZLIB
    TestZlibStreamWrite();
    TestZlibStreamRead();
#endif // TEST_ZLIB

    return 0;
}

