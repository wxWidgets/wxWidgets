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

#include <stdio.h>

#include <wx/string.h>
#include <wx/file.h>
#include <wx/app.h>

// without this pragma, the stupid compiler precompiles #defines below so that
// changing them doesn't "take place" later!
#ifdef __VISUALC__
    #pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// what to test (in alphabetic order)?

//#define TEST_ARRAYS
//#define TEST_CMDLINE
//#define TEST_DATETIME
//#define TEST_DIR
//#define TEST_DLLLOADER
//#define TEST_ENVIRON
//#define TEST_EXECUTE
//#define TEST_FILE
//#define TEST_FILECONF
#define TEST_FILENAME
//#define TEST_FTP
//#define TEST_HASH
//#define TEST_LIST
//#define TEST_LOG
//#define TEST_LONGLONG
//#define TEST_MIME
//#define TEST_INFO_FUNCTIONS
//#define TEST_REGISTRY
//#define TEST_SOCKETS
//#define TEST_STREAMS
//#define TEST_STRINGS
//#define TEST_THREADS
//#define TEST_TIMER
//#define TEST_VCARD            -- don't enable this (VZ)
//#define TEST_WCHAR
//#define TEST_ZIP
//#define TEST_ZLIB

// ----------------------------------------------------------------------------
// test class for container objects
// ----------------------------------------------------------------------------

#if defined(TEST_ARRAYS) || defined(TEST_LIST)

class Bar // Foo is already taken in the hash test
{
public:
    Bar(const wxString& name) : m_name(name) { ms_bars++; }
   ~Bar() { ms_bars--; }

   static size_t GetNumber() { return ms_bars; }

   const char *GetName() const { return m_name; }

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
// wxCmdLineParser
// ----------------------------------------------------------------------------

#ifdef TEST_CMDLINE

#include <wx/cmdline.h>
#include <wx/datetime.h>

static void ShowCmdLine(const wxCmdLineParser& parser)
{
    wxString s = "Input files: ";

    size_t count = parser.GetParamCount();
    for ( size_t param = 0; param < count; param++ )
    {
        s << parser.GetParam(param) << ' ';
    }

    s << '\n'
      << "Verbose:\t" << (parser.Found("v") ? "yes" : "no") << '\n'
      << "Quiet:\t" << (parser.Found("q") ? "yes" : "no") << '\n';

    wxString strVal;
    long lVal;
    wxDateTime dt;
    if ( parser.Found("o", &strVal) )
        s << "Output file:\t" << strVal << '\n';
    if ( parser.Found("i", &strVal) )
        s << "Input dir:\t" << strVal << '\n';
    if ( parser.Found("s", &lVal) )
        s << "Size:\t" << lVal << '\n';
    if ( parser.Found("d", &dt) )
        s << "Date:\t" << dt.FormatISODate() << '\n';
    if ( parser.Found("project_name", &strVal) )
        s << "Project:\t" << strVal << '\n';

    wxLogMessage(s);
}

#endif // TEST_CMDLINE

// ----------------------------------------------------------------------------
// wxDir
// ----------------------------------------------------------------------------

#ifdef TEST_DIR

#include <wx/dir.h>

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
        printf("\t%s\n", filename.c_str());

        cont = dir.GetNext(&filename);
    }

    puts("");
}

static void TestDirEnum()
{
    wxDir dir(wxGetCwd());

    puts("Enumerating everything in current directory:");
    TestDirEnumHelper(dir);

    puts("Enumerating really everything in current directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT);

    puts("Enumerating object files in current directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT, "*.o");

    puts("Enumerating directories in current directory:");
    TestDirEnumHelper(dir, wxDIR_DIRS);

    puts("Enumerating files in current directory:");
    TestDirEnumHelper(dir, wxDIR_FILES);

    puts("Enumerating files including hidden in current directory:");
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

#ifdef __UNIX__
    dir.Open("/");
#elif defined(__WXMSW__)
    dir.Open("c:\\");
#else
    #error "don't know where the root directory is"
#endif

    puts("Enumerating everything in root directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT);

    puts("Enumerating directories in root directory:");
    TestDirEnumHelper(dir, wxDIR_DIRS);

    puts("Enumerating files in root directory:");
    TestDirEnumHelper(dir, wxDIR_FILES);

    puts("Enumerating files including hidden in root directory:");
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    puts("Enumerating files in non existing directory:");
    wxDir dirNo("nosuchdir");
    TestDirEnumHelper(dirNo);
}

#endif // TEST_DIR

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

#ifdef TEST_DLLLOADER

#include <wx/dynlib.h>

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

    puts("*** testing wxDllLoader ***\n");

    wxDllType dllHandle = wxDllLoader::LoadLibrary(LIB_NAME);
    if ( !dllHandle )
    {
        wxPrintf(_T("ERROR: failed to load '%s'.\n"), LIB_NAME);
    }
    else
    {
        typedef int (*strlenType)(char *);
        strlenType pfnStrlen = (strlenType)wxDllLoader::GetSymbol(dllHandle, FUNC_NAME);
        if ( !pfnStrlen )
        {
            wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"),
                     FUNC_NAME, LIB_NAME);
        }
        else
        {
            if ( pfnStrlen("foo") != 3 )
            {
                wxPrintf(_T("ERROR: loaded function is not strlen()!\n"));
            }
            else
            {
                puts("... ok");
            }
        }

        wxDllLoader::UnloadLibrary(dllHandle);
    }
}

#endif // TEST_DLLLOADER

// ----------------------------------------------------------------------------
// wxGet/SetEnv
// ----------------------------------------------------------------------------

#ifdef TEST_ENVIRON

#include <wx/utils.h>

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

    puts("*** testing environment access functions ***");

    printf("Initially getenv(%s) = %s\n", var, MyGetEnv(var).c_str());
    wxSetEnv(var, _T("value for wxTestVar"));
    printf("After wxSetEnv: getenv(%s) = %s\n",  var, MyGetEnv(var).c_str());
    wxSetEnv(var, _T("another value"));
    printf("After 2nd wxSetEnv: getenv(%s) = %s\n",  var, MyGetEnv(var).c_str());
    wxUnsetEnv(var);
    printf("After wxUnsetEnv: getenv(%s) = %s\n",  var, MyGetEnv(var).c_str());
    printf("PATH = %s\n",  MyGetEnv(_T("PATH")));
}

#endif // TEST_ENVIRON

// ----------------------------------------------------------------------------
// wxExecute
// ----------------------------------------------------------------------------

#ifdef TEST_EXECUTE

#include <wx/utils.h>

static void TestExecute()
{
    puts("*** testing wxExecute ***");

#ifdef __UNIX__
    #define COMMAND "cat -n ../../Makefile" // "echo hi"
    #define SHELL_COMMAND "echo hi from shell"
    #define REDIRECT_COMMAND COMMAND // "date"
#elif defined(__WXMSW__)
    #define COMMAND "command.com -c 'echo hi'"
    #define SHELL_COMMAND "echo hi"
    #define REDIRECT_COMMAND COMMAND
#else
    #error "no command to exec"
#endif // OS

    printf("Testing wxShell: ");
    fflush(stdout);
    if ( wxShell(SHELL_COMMAND) )
        puts("Ok.");
    else
        puts("ERROR.");

    printf("Testing wxExecute: ");
    fflush(stdout);
    if ( wxExecute(COMMAND, TRUE /* sync */) == 0 )
        puts("Ok.");
    else
        puts("ERROR.");

#if 0 // no, it doesn't work (yet?)
    printf("Testing async wxExecute: ");
    fflush(stdout);
    if ( wxExecute(COMMAND) != 0 )
        puts("Ok (command launched).");
    else
        puts("ERROR.");
#endif // 0

    printf("Testing wxExecute with redirection:\n");
    wxArrayString output;
    if ( wxExecute(REDIRECT_COMMAND, output) != 0 )
    {
        puts("ERROR.");
    }
    else
    {
        size_t count = output.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            printf("\t%s\n", output[n].c_str());
        }

        puts("Ok.");
    }
}

#endif // TEST_EXECUTE

// ----------------------------------------------------------------------------
// file
// ----------------------------------------------------------------------------

#ifdef TEST_FILE

#include <wx/file.h>
#include <wx/ffile.h>
#include <wx/textfile.h>

static void TestFileRead()
{
    puts("*** wxFile read test ***");

    wxFile file(_T("testdata.fc"));
    if ( file.IsOpened() )
    {
        printf("File length: %lu\n", file.Length());

        puts("File dump:\n----------");

        static const off_t len = 1024;
        char buf[len];
        for ( ;; )
        {
            off_t nRead = file.Read(buf, len);
            if ( nRead == wxInvalidOffset )
            {
                printf("Failed to read the file.");
                break;
            }

            fwrite(buf, nRead, 1, stdout);

            if ( nRead < len )
                break;
        }

        puts("----------");
    }
    else
    {
        printf("ERROR: can't open test file.\n");
    }

    puts("");
}

static void TestTextFileRead()
{
    puts("*** wxTextFile read test ***");

    wxTextFile file(_T("testdata.fc"));
    if ( file.Open() )
    {
        printf("Number of lines: %u\n", file.GetLineCount());
        printf("Last line: '%s'\n", file.GetLastLine().c_str());

        wxString s;

        puts("\nDumping the entire file:");
        for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
        {
            printf("%6u: %s\n", file.GetCurrentLine() + 1, s.c_str());
        }
        printf("%6u: %s\n", file.GetCurrentLine() + 1, s.c_str());

        puts("\nAnd now backwards:");
        for ( s = file.GetLastLine();
              file.GetCurrentLine() != 0;
              s = file.GetPrevLine() )
        {
            printf("%6u: %s\n", file.GetCurrentLine() + 1, s.c_str());
        }
        printf("%6u: %s\n", file.GetCurrentLine() + 1, s.c_str());
    }
    else
    {
        printf("ERROR: can't open '%s'\n", file.GetName());
    }

    puts("");
}

static void TestFileCopy()
{
    puts("*** Testing wxCopyFile ***");

    static const wxChar *filename1 = _T("testdata.fc");
    static const wxChar *filename2 = _T("test2");
    if ( !wxCopyFile(filename1, filename2) )
    {
        puts("ERROR: failed to copy file");
    }
    else
    {
        wxFFile f1(filename1, "rb"),
                f2(filename2, "rb");

        if ( !f1.IsOpened() || !f2.IsOpened() )
        {
            puts("ERROR: failed to open file(s)");
        }
        else
        {
            wxString s1, s2;
            if ( !f1.ReadAll(&s1) || !f2.ReadAll(&s2) )
            {
                puts("ERROR: failed to read file(s)");
            }
            else
            {
                if ( (s1.length() != s2.length()) ||
                     (memcmp(s1.c_str(), s2.c_str(), s1.length()) != 0) )
                {
                    puts("ERROR: copy error!");
                }
                else
                {
                    puts("File was copied ok.");
                }
            }
        }
    }

    if ( !wxRemoveFile(filename2) )
    {
        puts("ERROR: failed to remove the file");
    }

    puts("");
}

#endif // TEST_FILE

// ----------------------------------------------------------------------------
// wxFileConfig
// ----------------------------------------------------------------------------

#ifdef TEST_FILECONF

#include <wx/confbase.h>
#include <wx/fileconf.h>

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
    puts("*** testing wxFileConfig loading/reading ***");

    wxFileConfig fileconf(_T("test"), wxEmptyString,
                          _T("testdata.fc"), wxEmptyString,
                          wxCONFIG_USE_RELATIVE_PATH);

    // test simple reading
    puts("\nReading config file:");
    wxString defValue(_T("default")), value;
    for ( size_t n = 0; n < WXSIZEOF(fcTestData); n++ )
    {
        const FileConfTestData& data = fcTestData[n];
        value = fileconf.Read(data.name, defValue);
        printf("\t%s = %s ", data.name, value.c_str());
        if ( value == data.value )
        {
            puts("(ok)");
        }
        else
        {
            printf("(ERROR: should be %s)\n", data.value);
        }
    }

    // test enumerating the entries
    puts("\nEnumerating all root entries:");
    long dummy;
    wxString name;
    bool cont = fileconf.GetFirstEntry(name, dummy);
    while ( cont )
    {
        printf("\t%s = %s\n",
               name.c_str(),
               fileconf.Read(name.c_str(), _T("ERROR")).c_str());

        cont = fileconf.GetNextEntry(name, dummy);
    }
}

#endif // TEST_FILECONF

// ----------------------------------------------------------------------------
// wxFileName
// ----------------------------------------------------------------------------

#ifdef TEST_FILENAME

#include <wx/filename.h>

static void TestFileNameConstruction()
{
    puts("*** testing wxFileName construction ***");

    static const wxChar *filenames[] =
    {
        _T("/usr/bin/ls"),
        _T("/usr/bin/"),
        _T("~/.zshrc"),
        _T("../../foo"),
    };

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        wxFileName fn(filenames[n], wxPATH_UNIX);

        printf("Filename: '%s'\t", fn.GetFullPath().c_str());
        if ( !fn.Normalize(wxPATH_NORM_ALL, _T(""), wxPATH_UNIX) )
        {
            puts("ERROR (couldn't be normalized)");
        }
        else
        {
            printf("normalized: '%s'\n", fn.GetFullPath().c_str());
        }
    }

    puts("");
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
// wxHashTable
// ----------------------------------------------------------------------------

#ifdef TEST_HASH

#include <wx/hash.h>

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

#include <wx/listimpl.cpp>

WX_DEFINE_LIST(wxListFoos);

static void TestHash()
{
    puts("*** Testing wxHashTable ***\n");

    {
        wxHashFoos hash;
        hash.DeleteContents(TRUE);

        printf("Hash created: %u foos in hash, %u foos totally\n",
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

        printf("Hash filled: %u foos in hash, %u foos totally\n",
               hash.GetCount(), Foo::count);

        puts("Hash access test:");
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            printf("\tGetting element with key %d, value %d: ",
                   hashTestData[n], n);
            Foo *foo = hash.Get(hashTestData[n], n);
            if ( !foo )
            {
                printf("ERROR, not found.\n");
            }
            else
            {
                printf("%d (%s)\n", foo->n,
                       (size_t)foo->n == n ? "ok" : "ERROR");
            }
        }

        printf("\nTrying to get an element not in hash: ");

        if ( hash.Get(1234) || hash.Get(1, 0) )
        {
            puts("ERROR: found!");
        }
        else
        {
            puts("ok (not found)");
        }
    }

    printf("Hash destroyed: %u foos left\n", Foo::count);
}

#endif // TEST_HASH

// ----------------------------------------------------------------------------
// wxList
// ----------------------------------------------------------------------------

#ifdef TEST_LIST

#include <wx/list.h>

WX_DECLARE_LIST(Bar, wxListBars);
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxListBars);

static void TestListCtor()
{
    puts("*** Testing wxList construction ***\n");

    {
        wxListBars list1;
        list1.Append(new Bar(_T("first")));
        list1.Append(new Bar(_T("second")));

        printf("After 1st list creation: %u objects in the list, %u objects total.\n",
               list1.GetCount(), Bar::GetNumber());

        wxListBars list2;
        list2 = list1;

        printf("After 2nd list creation: %u and %u objects in the lists, %u objects total.\n",
               list1.GetCount(), list2.GetCount(), Bar::GetNumber());

        list1.DeleteContents(TRUE);
    }

    printf("After list destruction: %u objects left.\n", Bar::GetNumber());
}

#endif // TEST_LIST

// ----------------------------------------------------------------------------
// MIME types
// ----------------------------------------------------------------------------

#ifdef TEST_MIME

#include <wx/mimetype.h>

static wxMimeTypesManager g_mimeManager;

static void TestMimeEnum()
{
    wxArrayString mimetypes;

    size_t count = g_mimeManager.EnumAllFileTypes(mimetypes);

    printf("*** All %u known filetypes: ***\n", count);

    wxArrayString exts;
    wxString desc;

    for ( size_t n = 0; n < count; n++ )
    {
        wxFileType *filetype = g_mimeManager.GetFileTypeFromMimeType(mimetypes[n]);
        if ( !filetype )
        {
            printf("nothing known about the filetype '%s'!\n",
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

        printf("\t%s: %s (%s)\n",
               mimetypes[n].c_str(), desc.c_str(), extsAll.c_str());
    }
}

static void TestMimeOverride()
{
    wxPuts(_T("*** Testing wxMimeTypesManager additional files loading ***\n"));

    wxString mailcap = _T("/tmp/mailcap"),
             mimetypes = _T("/tmp/mime.types");

    wxPrintf(_T("Loading mailcap from '%s': %s\n"),
             mailcap.c_str(),
             g_mimeManager.ReadMailcap(mailcap) ? _T("ok") : _T("ERROR"));
    wxPrintf(_T("Loading mime.types from '%s': %s\n"),
             mimetypes.c_str(),
             g_mimeManager.ReadMimeTypes(mimetypes) ? _T("ok") : _T("ERROR"));
}

static void TestMimeFilename()
{
    wxPuts(_T("*** Testing MIME type from filename query ***\n"));

    static const wxChar *filenames[] =
    {
        _T("readme.txt"),
        _T("document.pdf"),
        _T("image.gif"),
    };

    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const wxString fname = filenames[n];
        wxString ext = fname.AfterLast(_T('.'));
        wxFileType *ft = g_mimeManager.GetFileTypeFromExtension(ext);
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

            wxPrintf(_T("To open %s (%s) do '%s'.\n"),
                     fname.c_str(), desc.c_str(), cmd.c_str());

            delete ft;
        }
    }
}

static void TestMimeAssociate()
{
    wxPuts(_T("*** Testing creation of filetype association ***\n"));

    wxFileType *ft = g_mimeManager.Associate
                     (
                        _T(".xyz"),
                        _T("application/x-xyz"),
                        _T("XYZFile"), // filetype (MSW only)
                        _T("XYZ File") // description (Unix only)
                     );
    if ( !ft )
    {
        wxPuts(_T("ERROR: failed to create association!"));
    }
    else
    {
        if ( !ft->SetOpenCommand(_T("myprogram")) )
        {
            wxPuts(_T("ERROR: failed to set open command!"));
        }

        delete ft;
    }
}

#endif // TEST_MIME

// ----------------------------------------------------------------------------
// misc information functions
// ----------------------------------------------------------------------------

#ifdef TEST_INFO_FUNCTIONS

#include <wx/utils.h>

static void TestOsInfo()
{
    puts("*** Testing OS info functions ***\n");

    int major, minor;
    wxGetOsVersion(&major, &minor);
    printf("Running under: %s, version %d.%d\n",
            wxGetOsDescription().c_str(), major, minor);

    printf("%ld free bytes of memory left.\n", wxGetFreeMemory());

    printf("Host name is %s (%s).\n",
           wxGetHostName().c_str(), wxGetFullHostName().c_str());

    puts("");
}

static void TestUserInfo()
{
    puts("*** Testing user info functions ***\n");

    printf("User id is:\t%s\n", wxGetUserId().c_str());
    printf("User name is:\t%s\n", wxGetUserName().c_str());
    printf("Home dir is:\t%s\n", wxGetHomeDir().c_str());
    printf("Email address:\t%s\n", wxGetEmailAddress().c_str());

    puts("");
}

#endif // TEST_INFO_FUNCTIONS

// ----------------------------------------------------------------------------
// long long
// ----------------------------------------------------------------------------

#ifdef TEST_LONGLONG

#include <wx/longlong.h>
#include <wx/timer.h>

// make a 64 bit number from 4 16 bit ones
#define MAKE_LL(x1, x2, x3, x4) wxLongLong((x1 << 16) | x2, (x3 << 16) | x3)

// get a random 64 bit number
#define RAND_LL()   MAKE_LL(rand(), rand(), rand(), rand())

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

        printf("Summing longs took %ld milliseconds.\n", sw.Time());
    }

#if wxUSE_LONGLONG_NATIVE
    {
        wxStopWatch sw;

        wxLongLong_t l = 0;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        printf("Summing wxLongLong_t took %ld milliseconds.\n", sw.Time());
    }
#endif // wxUSE_LONGLONG_NATIVE

    {
        wxStopWatch sw;

        wxLongLong l;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        printf("Summing wxLongLongs took %ld milliseconds.\n", sw.Time());
    }
}

static void TestLongLongConversion()
{
    puts("*** Testing wxLongLong conversions ***\n");

    wxLongLong a;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        a = RAND_LL();

#if wxUSE_LONGLONG_NATIVE
        wxLongLongNative b(a.GetHi(), a.GetLo());

        wxASSERT_MSG( a == b, "conversions failure" );
#else
        puts("Can't do it without native long long type, test skipped.");

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    puts(" done!");
}

static void TestMultiplication()
{
    puts("*** Testing wxLongLong multiplication ***\n");

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
        puts("Can't do it without native long long type, test skipped.");

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    puts(" done!");
}

static void TestDivision()
{
    puts("*** Testing wxLongLong division ***\n");

    wxLongLong q, r;
    size_t nTested = 0;
    for ( size_t n = 0; n < 100000; n++ )
    {
        // get a random wxLongLong (shifting by 12 the MSB ensures that the
        // multiplication will not overflow)
        wxLongLong ll = MAKE_LL((rand() >> 12), rand(), rand(), rand());

        // get a random long (not wxLongLong for now) to divide it with
        long l = rand();
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

    puts(" done!");
}

static void TestAddition()
{
    puts("*** Testing wxLongLong addition ***\n");

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

    puts(" done!");
}

static void TestBitOperations()
{
    puts("*** Testing wxLongLong bit operation ***\n");

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
        puts("Can't do it without native long long type, test skipped.");

        return;
#endif // wxUSE_LONGLONG_NATIVE

        if ( !(nTested % 1000) )
        {
            putchar('.');
            fflush(stdout);
        }

        nTested++;
    }

    puts(" done!");
}

static void TestLongLongComparison()
{
    puts("*** Testing wxLongLong comparison ***\n");

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
            printf("0x%lx > 0x%lx is %s (%s)\n",
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] > testLongs[n]) ? "ok" : "ERROR");

            res = lls[m] < testLongs[n];
            printf("0x%lx < 0x%lx is %s (%s)\n",
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] < testLongs[n]) ? "ok" : "ERROR");

            res = lls[m] == testLongs[n];
            printf("0x%lx == 0x%lx is %s (%s)\n",
                   ls[m], testLongs[n], res ? "true" : "false",
                   res == (ls[m] == testLongs[n]) ? "ok" : "ERROR");
        }
    }
}

#undef MAKE_LL
#undef RAND_LL

#endif // TEST_LONGLONG

// ----------------------------------------------------------------------------
// registry
// ----------------------------------------------------------------------------

// this is for MSW only
#ifndef __WXMSW__
    #undef TEST_REGISTRY
#endif

#ifdef TEST_REGISTRY

#include <wx/msw/registry.h>

// I chose this one because I liked its name, but it probably only exists under
// NT
static const wxChar *TESTKEY =
    _T("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\CrashControl");

static void TestRegistryRead()
{
    puts("*** testing registry reading ***");

    wxRegKey key(TESTKEY);
    printf("The test key name is '%s'.\n", key.GetName().c_str());
    if ( !key.Open() )
    {
        puts("ERROR: test key can't be opened, aborting test.");

        return;
    }

    size_t nSubKeys, nValues;
    if ( key.GetKeyInfo(&nSubKeys, NULL, &nValues, NULL) )
    {
        printf("It has %u subkeys and %u values.\n", nSubKeys, nValues);
    }

    printf("Enumerating values:\n");

    long dummy;
    wxString value;
    bool cont = key.GetFirstValue(value, dummy);
    while ( cont )
    {
        printf("Value '%s': type ", value.c_str());
        switch ( key.GetValueType(value) )
        {
            case wxRegKey::Type_None:   printf("ERROR (none)"); break;
            case wxRegKey::Type_String: printf("SZ"); break;
            case wxRegKey::Type_Expand_String: printf("EXPAND_SZ"); break;
            case wxRegKey::Type_Binary: printf("BINARY"); break;
            case wxRegKey::Type_Dword: printf("DWORD"); break;
            case wxRegKey::Type_Multi_String: printf("MULTI_SZ"); break;
            default: printf("other (unknown)"); break;
        }

        printf(", value = ");
        if ( key.IsNumericValue(value) )
        {
            long val;
            key.QueryValue(value, &val);
            printf("%ld", val);
        }
        else // string
        {
            wxString val;
            key.QueryValue(value, val);
            printf("'%s'", val.c_str());

            key.QueryRawValue(value, val);
            printf(" (raw value '%s')", val.c_str());
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
// sockets
// ----------------------------------------------------------------------------

#ifdef TEST_SOCKETS

#include <wx/socket.h>
#include <wx/protocol/protocol.h>
#include <wx/protocol/http.h>

static void TestSocketServer()
{
    puts("*** Testing wxSocketServer ***\n");

    static const int PORT = 3000;

    wxIPV4address addr;
    addr.Service(PORT);

    wxSocketServer *server = new wxSocketServer(addr);
    if ( !server->Ok() )
    {
        puts("ERROR: failed to bind");

        return;
    }

    for ( ;; )
    {
        printf("Server: waiting for connection on port %d...\n", PORT);

        wxSocketBase *socket = server->Accept();
        if ( !socket )
        {
            puts("ERROR: wxSocketServer::Accept() failed.");
            break;
        }

        puts("Server: got a client.");

        server->SetTimeout(60); // 1 min

        while ( socket->IsConnected() )
        {
            wxString s;
            char ch = '\0';
            for ( ;; )
            {
                if ( socket->Read(&ch, sizeof(ch)).Error() )
                {
                    // don't log error if the client just close the connection
                    if ( socket->IsConnected() )
                    {
                        puts("ERROR: in wxSocket::Read.");
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

            printf("Server: got '%s'.\n", s.c_str());
            if ( s == _T("bye") )
            {
                delete socket;

                break;
            }

            socket->Write(s.MakeUpper().c_str(), s.length());
            socket->Write("\r\n", 2);
            printf("Server: wrote '%s'.\n", s.c_str());
        }

        puts("Server: lost a client.");

        socket->Destroy();
    }

    // same as "delete server" but is consistent with GUI programs
    server->Destroy();
}

static void TestSocketClient()
{
    puts("*** Testing wxSocketClient ***\n");

    static const char *hostname = "www.wxwindows.org";

    wxIPV4address addr;
    addr.Hostname(hostname);
    addr.Service(80);

    printf("--- Attempting to connect to %s:80...\n", hostname);

    wxSocketClient client;
    if ( !client.Connect(addr) )
    {
        printf("ERROR: failed to connect to %s\n", hostname);
    }
    else
    {
        printf("--- Connected to %s:%u...\n",
               addr.Hostname().c_str(), addr.Service());

        char buf[8192];

        // could use simply "GET" here I suppose
        wxString cmdGet =
            wxString::Format("GET http://%s/\r\n", hostname);
        client.Write(cmdGet, cmdGet.length());
        printf("--- Sent command '%s' to the server\n",
               MakePrintable(cmdGet).c_str());
        client.Read(buf, WXSIZEOF(buf));
        printf("--- Server replied:\n%s", buf);
    }
}

#endif // TEST_SOCKETS

#ifdef TEST_FTP

#include <wx/protocol/ftp.h>

static void TestProtocolFtp()
{
    puts("*** Testing wxFTP download ***\n");

    wxFTP ftp;
#if 1
    static const char *hostname = "ftp.wxwindows.org";
    static const char *directory = "pub";

    printf("--- Attempting to connect to %s:21 anonymously...\n", hostname);
#else
    static const char *hostname = "localhost";
    static const char *user = "zeitlin";
    static const char *directory = "/tmp";

    ftp.SetUser(user);
    ftp.SetPassword("password");

    printf("--- Attempting to connect to %s:21 as %s...\n", hostname, user);
#endif

    if ( !ftp.Connect(hostname) )
    {
        printf("ERROR: failed to connect to %s\n", hostname);
    }
    else
    {
        printf("--- Connected to %s, current directory is '%s'\n",
               hostname, ftp.Pwd().c_str());

        // test CWD
        if ( !ftp.ChDir(directory) )
        {
            printf("ERROR: failed to cd to %s\n", directory);
        }

        // test NLIST and LIST
        wxArrayString files;
        if ( !ftp.GetFilesList(files) )
        {
            puts("ERROR: failed to get NLIST of files");
        }
        else
        {
            printf("Brief list of files under '%s':\n", ftp.Pwd().c_str());
            size_t count = files.GetCount();
            for ( size_t n = 0; n < count; n++ )
            {
                printf("\t%s\n", files[n].c_str());
            }
            puts("End of the file list");
        }

        if ( !ftp.GetDirList(files) )
        {
            puts("ERROR: failed to get LIST of files");
        }
        else
        {
            printf("Detailed list of files under '%s':\n", ftp.Pwd().c_str());
            size_t count = files.GetCount();
            for ( size_t n = 0; n < count; n++ )
            {
                printf("\t%s\n", files[n].c_str());
            }
            puts("End of the file list");
        }

        if ( !ftp.ChDir(_T("..")) )
        {
            puts("ERROR: failed to cd to ..");
        }

        // test RETR
        static const char *filename = "welcome.msg";
        wxInputStream *in = ftp.GetInputStream(filename);
        if ( !in )
        {
            printf("ERROR: couldn't get input stream for %s\n", filename);
        }
        else
        {
            size_t size = in->StreamSize();
            printf("Reading file %s (%u bytes)...", filename, size);

            char *data = new char[size];
            if ( !in->Read(data, size) )
            {
                puts("ERROR: read error");
            }
            else
            {
                printf("\nContents of %s:\n%s\n", filename, data);
            }

            delete [] data;
            delete in;
        }

        // test some other FTP commands
        if ( ftp.SendCommand("STAT") != '2' )
        {
            puts("ERROR: STAT failed");
        }
        else
        {
            printf("STAT returned:\n\n%s\n", ftp.GetLastResult().c_str());
        }

        if ( ftp.SendCommand("HELP SITE") != '2' )
        {
            puts("ERROR: HELP SITE failed");
        }
        else
        {
            printf("The list of site-specific commands:\n\n%s\n",
                   ftp.GetLastResult().c_str());
        }
    }
}

static void TestProtocolFtpUpload()
{
    puts("*** Testing wxFTP uploading ***\n");

    static const char *hostname = "localhost";

    printf("--- Attempting to connect to %s:21...\n", hostname);

    wxFTP ftp;
    ftp.SetUser("zeitlin");
    ftp.SetPassword("password");
    if ( !ftp.Connect(hostname) )
    {
        printf("ERROR: failed to connect to %s\n", hostname);
    }
    else
    {
        printf("--- Connected to %s, current directory is '%s'\n",
               hostname, ftp.Pwd().c_str());

        // upload a file
        static const char *file1 = "test1";
        static const char *file2 = "test2";
        wxOutputStream *out = ftp.GetOutputStream(file1);
        if ( out )
        {
            printf("--- Uploading to %s ---\n", file1);
            out->Write("First hello", 11);
            delete out;
        }

        // send a command to check the remote file
        if ( ftp.SendCommand(wxString("STAT ") + file1) != '2' )
        {
            printf("ERROR: STAT %s failed\n", file1);
        }
        else
        {
            printf("STAT %s returned:\n\n%s\n",
                   file1, ftp.GetLastResult().c_str());
        }

        out = ftp.GetOutputStream(file2);
        if ( out )
        {
            printf("--- Uploading to %s ---\n", file1);
            out->Write("Second hello", 12);
            delete out;
        }
    }
}

#endif // TEST_FTP

// ----------------------------------------------------------------------------
// streams
// ----------------------------------------------------------------------------

#ifdef TEST_STREAMS

#include <wx/mstream.h>

static void TestMemoryStream()
{
    puts("*** Testing wxMemoryInputStream ***");

    wxChar buf[1024];
    wxStrncpy(buf, _T("Hello, stream!"), WXSIZEOF(buf));

    wxMemoryInputStream memInpStream(buf, wxStrlen(buf));
    printf(_T("Memory stream size: %u\n"), memInpStream.GetSize());
    while ( !memInpStream.Eof() )
    {
        putchar(memInpStream.GetC());
    }

    puts("\n*** wxMemoryInputStream test done ***");
}

#endif // TEST_STREAMS

// ----------------------------------------------------------------------------
// timers
// ----------------------------------------------------------------------------

#ifdef TEST_TIMER

#include <wx/timer.h>
#include <wx/utils.h>

static void TestStopWatch()
{
    puts("*** Testing wxStopWatch ***\n");

    wxStopWatch sw;
    printf("Sleeping 3 seconds...");
    wxSleep(3);
    printf("\telapsed time: %ldms\n", sw.Time());

    sw.Pause();
    printf("Sleeping 2 more seconds...");
    wxSleep(2);
    printf("\telapsed time: %ldms\n", sw.Time());

    sw.Resume();
    printf("And 3 more seconds...");
    wxSleep(3);
    printf("\telapsed time: %ldms\n", sw.Time());

    wxStopWatch sw2;
    puts("\nChecking for 'backwards clock' bug...");
    for ( size_t n = 0; n < 70; n++ )
    {
        sw2.Start();

        for ( size_t m = 0; m < 100000; m++ )
        {
            if ( sw.Time() < 0 || sw2.Time() < 0 )
            {
                puts("\ntime is negative - ERROR!");
            }
        }

        putchar('.');
    }

    puts(", ok.");
}

#endif // TEST_TIMER

// ----------------------------------------------------------------------------
// vCard support
// ----------------------------------------------------------------------------

#ifdef TEST_VCARD

#include <wx/vcard.h>

static void DumpVObject(size_t level, const wxVCardObject& vcard)
{
    void *cookie;
    wxVCardObject *vcObj = vcard.GetFirstProp(&cookie);
    while ( vcObj )
    {
        printf("%s%s",
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
            printf(" = %s", value.c_str());
        putchar('\n');

        DumpVObject(level + 1, *vcObj);

        delete vcObj;
        vcObj = vcard.GetNextProp(&cookie);
    }
}

static void DumpVCardAddresses(const wxVCard& vcard)
{
    puts("\nShowing all addresses from vCard:\n");

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

        printf("Address %u:\n"
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
    puts("\nShowing all phone numbers from vCard:\n");

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

        printf("Phone number %u:\n"
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
    puts("*** Testing wxVCard reading ***\n");

    wxVCard vcard(_T("vcard.vcf"));
    if ( !vcard.IsOk() )
    {
        puts("ERROR: couldn't load vCard.");
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

        printf("Full name retrieved directly: %s\n", value.c_str());


        if ( !vcard.GetFullName(&value) )
        {
            value = _T("<none>");
        }

        printf("Full name from wxVCard API: %s\n", value.c_str());

        // now show how to deal with multiply occuring properties
        DumpVCardAddresses(vcard);
        DumpVCardPhoneNumbers(vcard);

        // and finally show all
        puts("\nNow dumping the entire vCard:\n"
             "-----------------------------\n");

        DumpVObject(0, vcard);
    }
}

static void TestVCardWrite()
{
    puts("*** Testing wxVCard writing ***\n");

    wxVCard vcard;
    if ( !vcard.IsOk() )
    {
        puts("ERROR: couldn't create vCard.");
    }
    else
    {
        // set some fields
        vcard.SetName("Zeitlin", "Vadim");
        vcard.SetFullName("Vadim Zeitlin");
        vcard.SetOrganization("wxWindows", "R&D");

        // just dump the vCard back
        puts("Entire vCard follows:\n");
        puts(vcard.Write());
    }
}

#endif // TEST_VCARD

// ----------------------------------------------------------------------------
// wide char (Unicode) support
// ----------------------------------------------------------------------------

#ifdef TEST_WCHAR

#include <wx/strconv.h>
#include <wx/buffer.h>

static void TestUtf8()
{
    puts("*** Testing UTF8 support ***\n");

    wxString testString = "français";
#if 0
"************ French - Français ****************"
"Juste un petit exemple pour dire que les français aussi"
"ont à cœur de pouvoir utiliser tous leurs caractères ! :)";
#endif

    wxWCharBuffer wchBuf = testString.wc_str(wxConvUTF8);
    const wchar_t *pwz = (const wchar_t *)wchBuf;
    wxString testString2(pwz, wxConvLocal);

    printf("Decoding '%s' => '%s'\n", testString.c_str(), testString2.c_str());

    char *psz = "fran" "\xe7" "ais";
    size_t len = strlen(psz);
    wchar_t *pwz2 = new wchar_t[len + 1];
    for ( size_t n = 0; n <= len; n++ )
    {
        pwz2[n] = (wchar_t)(unsigned char)psz[n];
    }

    wxString testString3(pwz2, wxConvUTF8);
    delete [] pwz2;

    printf("Encoding '%s' -> '%s'\n", psz, testString3.c_str());
}

#endif // TEST_WCHAR

// ----------------------------------------------------------------------------
// ZIP stream
// ----------------------------------------------------------------------------

#ifdef TEST_ZIP

#include "wx/zipstrm.h"

static void TestZipStreamRead()
{
    puts("*** Testing ZIP reading ***\n");

    wxZipInputStream istr(_T("idx.zip"), _T("IDX.txt"));
    printf("Archive size: %u\n", istr.GetSize());

    puts("Dumping the file:");
    while ( !istr.Eof() )
    {
        putchar(istr.GetC());
        fflush(stdout);
    }

    puts("\n----- done ------");
}

#endif // TEST_ZIP

// ----------------------------------------------------------------------------
// ZLIB stream
// ----------------------------------------------------------------------------

#ifdef TEST_ZLIB

#include <wx/zstream.h>
#include <wx/wfstream.h>

static const wxChar *FILENAME_GZ = _T("test.gz");
static const char *TEST_DATA = "hello and hello again";

static void TestZlibStreamWrite()
{
    puts("*** Testing Zlib stream reading ***\n");

    wxFileOutputStream fileOutStream(FILENAME_GZ);
    wxZlibOutputStream ostr(fileOutStream, 0);
    printf("Compressing the test string... ");
    ostr.Write(TEST_DATA, sizeof(TEST_DATA));
    if ( !ostr )
    {
        puts("(ERROR: failed)");
    }
    else
    {
        puts("(ok)");
    }

    puts("\n----- done ------");
}

static void TestZlibStreamRead()
{
    puts("*** Testing Zlib stream reading ***\n");

    wxFileInputStream fileInStream(FILENAME_GZ);
    wxZlibInputStream istr(fileInStream);
    printf("Archive size: %u\n", istr.GetSize());

    puts("Dumping the file:");
    while ( !istr.Eof() )
    {
        putchar(istr.GetC());
        fflush(stdout);
    }

    puts("\n----- done ------");
}

#endif // TEST_ZLIB

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_DATETIME

#include <wx/date.h>

#include <wx/datetime.h>

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
        s.Printf("%02d:%02d:%02d %10s %02d, %4d%s",
                 hour, min, sec,
                 wxDateTime::GetMonthName(month).c_str(),
                 day,
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? "AD" : "BC");
        return s;
    }

    wxString FormatDate() const
    {
        wxString s;
        s.Printf("%02d-%s-%4d%s",
                 day,
                 wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? "AD" : "BC");
        return s;
    }
};

static const Date testDates[] =
{
    {  1, wxDateTime::Jan,  1970, 00, 00, 00, 2440587.5, wxDateTime::Thu,         0,     -3600 },
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
    puts("\n*** wxDateTime static methods test ***");

    // some info about the current date
    int year = wxDateTime::GetCurrentYear();
    printf("Current year %d is %sa leap one and has %d days.\n",
           year,
           wxDateTime::IsLeapYear(year) ? "" : "not ",
           wxDateTime::GetNumberOfDays(year));

    wxDateTime::Month month = wxDateTime::GetCurrentMonth();
    printf("Current month is '%s' ('%s') and it has %d days\n",
           wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
           wxDateTime::GetMonthName(month).c_str(),
           wxDateTime::GetNumberOfDays(month));

    // leap year logic
    static const size_t nYears = 5;
    static const size_t years[2][nYears] =
    {
        // first line: the years to test
        { 1990, 1976, 2000, 2030, 1984, },

        // second line: TRUE if leap, FALSE otherwise
        { FALSE, TRUE, TRUE, FALSE, TRUE }
    };

    for ( size_t n = 0; n < nYears; n++ )
    {
        int year = years[0][n];
        bool should = years[1][n] != 0,
             is = wxDateTime::IsLeapYear(year);

        printf("Year %d is %sa leap year (%s)\n",
               year,
               is ? "" : "not ",
               should == is ? "ok" : "ERROR");

        wxASSERT( should == wxDateTime::IsLeapYear(year) );
    }
}

// test constructing wxDateTime objects
static void TestTimeSet()
{
    puts("\n*** wxDateTime construction test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d1 = testDates[n];
        wxDateTime dt = d1.DT();

        Date d2;
        d2.Init(dt.GetTm());

        wxString s1 = d1.Format(),
                 s2 = d2.Format();

        printf("Date: %s == %s (%s)\n",
               s1.c_str(), s2.c_str(),
               s1 == s2 ? "ok" : "ERROR");
    }
}

// test time zones stuff
static void TestTimeZones()
{
    puts("\n*** wxDateTime timezone test ***");

    wxDateTime now = wxDateTime::Now();

    printf("Current GMT time:\t%s\n", now.Format("%c", wxDateTime::GMT0).c_str());
    printf("Unix epoch (GMT):\t%s\n", wxDateTime((time_t)0).Format("%c", wxDateTime::GMT0).c_str());
    printf("Unix epoch (EST):\t%s\n", wxDateTime((time_t)0).Format("%c", wxDateTime::EST).c_str());
    printf("Current time in Paris:\t%s\n", now.Format("%c", wxDateTime::CET).c_str());
    printf("               Moscow:\t%s\n", now.Format("%c", wxDateTime::MSK).c_str());
    printf("             New York:\t%s\n", now.Format("%c", wxDateTime::EST).c_str());

    wxDateTime::Tm tm = now.GetTm();
    if ( wxDateTime(tm) != now )
    {
        printf("ERROR: got %s instead of %s\n",
               wxDateTime(tm).Format().c_str(), now.Format().c_str());
    }
}

// test some minimal support for the dates outside the standard range
static void TestTimeRange()
{
    puts("\n*** wxDateTime out-of-standard-range dates test ***");

    static const char *fmt = "%d-%b-%Y %H:%M:%S";

    printf("Unix epoch:\t%s\n",
           wxDateTime(2440587.5).Format(fmt).c_str());
    printf("Feb 29, 0: \t%s\n",
            wxDateTime(29, wxDateTime::Feb, 0).Format(fmt).c_str());
    printf("JDN 0:     \t%s\n",
            wxDateTime(0.0).Format(fmt).c_str());
    printf("Jan 1, 1AD:\t%s\n",
            wxDateTime(1, wxDateTime::Jan, 1).Format(fmt).c_str());
    printf("May 29, 2099:\t%s\n",
            wxDateTime(29, wxDateTime::May, 2099).Format(fmt).c_str());
}

static void TestTimeTicks()
{
    puts("\n*** wxDateTime ticks test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        if ( d.ticks == -1 )
            continue;

        wxDateTime dt = d.DT();
        long ticks = (dt.GetValue() / 1000).ToLong();
        printf("Ticks of %s:\t% 10ld", d.Format().c_str(), ticks);
        if ( ticks == d.ticks )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %ld, delta = %ld)\n",
                   d.ticks, ticks - d.ticks);
        }

        dt = d.DT().ToTimezone(wxDateTime::GMT0);
        ticks = (dt.GetValue() / 1000).ToLong();
        printf("GMtks of %s:\t% 10ld", d.Format().c_str(), ticks);
        if ( ticks == d.gmticks )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %ld, delta = %ld)\n",
                   d.gmticks, ticks - d.gmticks);
        }
    }

    puts("");
}

// test conversions to JDN &c
static void TestTimeJDN()
{
    puts("\n*** wxDateTime to JDN test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);
        double jdn = dt.GetJulianDayNumber();

        printf("JDN of %s is:\t% 15.6f", d.Format().c_str(), jdn);
        if ( jdn == d.jdn )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %f, delta = %f)\n",
                   d.jdn, jdn - d.jdn);
        }
    }
}

// test week days computation
static void TestTimeWDays()
{
    puts("\n*** wxDateTime weekday test ***");

    // test GetWeekDay()
    size_t n;
    for ( n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        printf("%s is: %s",
               d.Format().c_str(),
               wxDateTime::GetWeekDayName(wday).c_str());
        if ( wday == d.wday )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n",
                   wxDateTime::GetWeekDayName(d.wday).c_str());
        }
    }

    puts("");

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
                case 1: which = "first"; break;
                case 2: which = "second"; break;
                case 3: which = "third"; break;
                case 4: which = "fourth"; break;
                case 5: which = "fifth"; break;

                case -1: which = "last"; break;
            }

            if ( nWeek < -1 )
            {
                which += " from end";
            }

            s.Printf("The %s %s of %s in %d",
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

    static const char *fmt = "%d-%b-%Y";

    wxDateTime dt;
    for ( n = 0; n < WXSIZEOF(weekDatesTestData); n++ )
    {
        const WeekDateTestData& wd = weekDatesTestData[n];

        dt.SetToWeekDay(wd.wday, wd.nWeek, wd.month, wd.year);

        printf("%s is %s", wd.Format().c_str(), dt.Format(fmt).c_str());

        const Date& d = wd.date;
        if ( d.SameDay(dt.GetTm()) )
        {
            puts(" (ok)");
        }
        else
        {
            dt.Set(d.day, d.month, d.year);

            printf(" (ERROR: should be %s)\n", dt.Format(fmt).c_str());
        }
    }
}

// test the computation of (ISO) week numbers
static void TestTimeWNumber()
{
    puts("\n*** wxDateTime week number test ***");

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

        printf("%s: the day number is %d",
               d.FormatDate().c_str(), dnum);
        if ( dnum == wn.dnum )
        {
            printf(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)", wn.dnum);
        }

        printf(", week in month is %d", wmon);
        if ( wmon == wn.wmon )
        {
            printf(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)", wn.wmon);
        }

        printf(" or %d", wmon2);
        if ( wmon2 == wn.wmon2 )
        {
            printf(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)", wn.wmon2);
        }

        printf(", week in year is %d", week);
        if ( week == wn.week )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)\n", wn.week);
        }
    }
}

// test DST calculations
static void TestTimeDST()
{
    puts("\n*** wxDateTime DST test ***");

    printf("DST is%s in effect now.\n\n",
           wxDateTime::Now().IsDST() ? "" : " not");

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

        printf("DST period in the US for year %d: from %s to %s",
               year, dtBegin.Format().c_str(), dtEnd.Format().c_str());

        size_t n = year - 1990;
        const Date& dBegin = datesDST[0][n];
        const Date& dEnd = datesDST[1][n];

        if ( dBegin.SameDay(dtBegin.GetTm()) && dEnd.SameDay(dtEnd.GetTm()) )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s %d to %s %d)\n",
                    wxDateTime::GetMonthName(dBegin.month).c_str(), dBegin.day,
                    wxDateTime::GetMonthName(dEnd.month).c_str(), dEnd.day);
        }
    }

    puts("");

    for ( year = 1990; year < 2005; year++ )
    {
        printf("DST period in Europe for year %d: from %s to %s\n",
               year,
               wxDateTime::GetBeginDST(year, wxDateTime::Country_EEC).Format().c_str(),
               wxDateTime::GetEndDST(year, wxDateTime::Country_EEC).Format().c_str());
    }
}

// test wxDateTime -> text conversion
static void TestTimeFormat()
{
    puts("\n*** wxDateTime formatting test ***");

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
        const char *format;
    } formatTestFormats[] =
    {
       { CompareBoth, "---> %c" },
       { CompareDate, "Date is %A, %d of %B, in year %Y" },
       { CompareBoth, "Date is %x, time is %X" },
       { CompareTime, "Time is %H:%M:%S or %I:%M:%S %p" },
       { CompareNone, "The day of year: %j, the week of year: %W" },
       { CompareDate, "ISO date without separators: %4Y%2m%2d" },
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
    printf("%s\n", wxDateTime::Now().Format("Our timezone is %Z").c_str());

    for ( size_t d = 0; d < WXSIZEOF(formatTestDates) + 1; d++ )
    {
        puts("");

        wxDateTime dt = d == 0 ? wxDateTime::Now() : formatTestDates[d - 1].DT();
        for ( size_t n = 0; n < WXSIZEOF(formatTestFormats); n++ )
        {
            wxString s = dt.Format(formatTestFormats[n].format);
            printf("%s", s.c_str());

            // what can we recover?
            int kind = formatTestFormats[n].compareKind;

            // convert back
            wxDateTime dt2;
            const wxChar *result = dt2.ParseFormat(s, formatTestFormats[n].format);
            if ( !result )
            {
                // converion failed - should it have?
                if ( kind == CompareNone )
                    puts(" (ok)");
                else
                    puts(" (ERROR: conversion back failed)");
            }
            else if ( *result )
            {
                // should have parsed the entire string
                puts(" (ERROR: conversion back stopped too soon)");
            }
            else
            {
                bool equal = FALSE; // suppress compilaer warning
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
                    printf(" (ERROR: got back '%s' instead of '%s')\n",
                           dt2.Format().c_str(), dt.Format().c_str());
                }
                else
                {
                    puts(" (ok)");
                }
            }
        }
    }
}

// test text -> wxDateTime conversion
static void TestTimeParse()
{
    puts("\n*** wxDateTime parse test ***");

    struct ParseTestData
    {
        const char *format;
        Date date;
        bool good;
    };

    static const ParseTestData parseTestDates[] =
    {
        { "Sat, 18 Dec 1999 00:46:40 +0100", { 18, wxDateTime::Dec, 1999, 00, 46, 40 }, TRUE },
        { "Wed, 1 Dec 1999 05:17:20 +0300",  {  1, wxDateTime::Dec, 1999, 03, 17, 20 }, TRUE },
    };

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const char *format = parseTestDates[n].format;

        printf("%s => ", format);

        wxDateTime dt;
        if ( dt.ParseRfc822Date(format) )
        {
            printf("%s ", dt.Format().c_str());

            if ( parseTestDates[n].good )
            {
                wxDateTime dtReal = parseTestDates[n].date.DT();
                if ( dt == dtReal )
                {
                    puts("(ok)");
                }
                else
                {
                    printf("(ERROR: should be %s)\n", dtReal.Format().c_str());
                }
            }
            else
            {
                puts("(ERROR: bad format)");
            }
        }
        else
        {
            printf("bad format (%s)\n",
                   parseTestDates[n].good ? "ERROR" : "ok");
        }
    }
}

static void TestInteractive()
{
    puts("\n*** interactive wxDateTime tests ***");

    char buf[128];

    for ( ;; )
    {
        printf("Enter a date: ");
        if ( !fgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[strlen(buf) - 1] = 0;

        wxDateTime dt;
        const char *p = dt.ParseDate(buf);
        if ( !p )
        {
            printf("ERROR: failed to parse the date '%s'.\n", buf);

            continue;
        }
        else if ( *p )
        {
            printf("WARNING: parsed only first %u characters.\n", p - buf);
        }

        printf("%s: day %u, week of month %u/%u, week of year %u\n",
               dt.Format("%b %d, %Y").c_str(),
               dt.GetDayOfYear(),
               dt.GetWeekOfMonth(wxDateTime::Monday_First),
               dt.GetWeekOfMonth(wxDateTime::Sunday_First),
               dt.GetWeekOfYear(wxDateTime::Monday_First));
    }

    puts("\n*** done ***");
}

static void TestTimeMS()
{
    puts("*** testing millisecond-resolution support in wxDateTime ***");

    wxDateTime dt1 = wxDateTime::Now(),
               dt2 = wxDateTime::UNow();

    printf("Now = %s\n", dt1.Format("%H:%M:%S:%l").c_str());
    printf("UNow = %s\n", dt2.Format("%H:%M:%S:%l").c_str());
    printf("Dummy loop: ");
    for ( int i = 0; i < 6000; i++ )
    {
        //for ( int j = 0; j < 10; j++ )
        {
            wxString s;
            s.Printf("%g", sqrt(i));
        }

        if ( !(i % 100) )
            putchar('.');
    }
    puts(", done");

    dt1 = dt2;
    dt2 = wxDateTime::UNow();
    printf("UNow = %s\n", dt2.Format("%H:%M:%S:%l").c_str());

    printf("Loop executed in %s ms\n", (dt2 - dt1).Format("%l").c_str());

    puts("\n*** done ***");
}

static void TestTimeArithmetics()
{
    puts("\n*** testing arithmetic operations on wxDateTime ***");

    static const struct ArithmData
    {
        ArithmData(const wxDateSpan& sp, const char *nam)
            : span(sp), name(nam) { }

        wxDateSpan span;
        const char *name;
    } testArithmData[] =
    {
        ArithmData(wxDateSpan::Day(), "day"),
        ArithmData(wxDateSpan::Week(), "week"),
        ArithmData(wxDateSpan::Month(), "month"),
        ArithmData(wxDateSpan::Year(), "year"),
        ArithmData(wxDateSpan(1, 2, 3, 4), "year, 2 months, 3 weeks, 4 days"),
    };

    wxDateTime dt(29, wxDateTime::Dec, 1999), dt1, dt2;

    for ( size_t n = 0; n < WXSIZEOF(testArithmData); n++ )
    {
        wxDateSpan span = testArithmData[n].span;
        dt1 = dt + span;
        dt2 = dt - span;

        const char *name = testArithmData[n].name;
        printf("%s + %s = %s, %s - %s = %s\n",
               dt.FormatISODate().c_str(), name, dt1.FormatISODate().c_str(),
               dt.FormatISODate().c_str(), name, dt2.FormatISODate().c_str());

        printf("Going back: %s", (dt1 - span).FormatISODate().c_str());
        if ( dt1 - span == dt )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n", dt.FormatISODate().c_str());
        }

        printf("Going forward: %s", (dt2 + span).FormatISODate().c_str());
        if ( dt2 + span == dt )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n", dt.FormatISODate().c_str());
        }

        printf("Double increment: %s", (dt2 + 2*span).FormatISODate().c_str());
        if ( dt2 + 2*span == dt1 )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n", dt2.FormatISODate().c_str());
        }

        puts("");
    }
}

static void TestTimeHolidays()
{
    puts("\n*** testing wxDateTimeHolidayAuthority ***\n");

    wxDateTime::Tm tm = wxDateTime(29, wxDateTime::May, 2000).GetTm();
    wxDateTime dtStart(1, tm.mon, tm.year),
               dtEnd = dtStart.GetLastMonthDay();

    wxDateTimeArray hol;
    wxDateTimeHolidayAuthority::GetHolidaysInRange(dtStart, dtEnd, hol);

    const wxChar *format = "%d-%b-%Y (%a)";

    printf("All holidays between %s and %s:\n",
           dtStart.Format(format).c_str(), dtEnd.Format(format).c_str());

    size_t count = hol.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        printf("\t%s\n", hol[n].Format(format).c_str());
    }

    puts("");
}

static void TestTimeZoneBug()
{
    puts("\n*** testing for DST/timezone bug ***\n");

    wxDateTime date = wxDateTime(1, wxDateTime::Mar, 2000);
    for ( int i = 0; i < 31; i++ )
    {
        printf("Date %s: week day %s.\n",
               date.Format(_T("%d-%m-%Y")).c_str(),
               date.GetWeekDayName(date.GetWeekDay()).c_str());

        date += wxDateSpan::Day();
    }

    puts("");
}

#if 0

// test compatibility with the old wxDate/wxTime classes
static void TestTimeCompatibility()
{
    puts("\n*** wxDateTime compatibility test ***");

    printf("wxDate for JDN 0: %s\n", wxDate(0l).FormatDate().c_str());
    printf("wxDate for MJD 0: %s\n", wxDate(2400000).FormatDate().c_str());

    double jdnNow = wxDateTime::Now().GetJDN();
    long jdnMidnight = (long)(jdnNow - 0.5);
    printf("wxDate for today: %s\n", wxDate(jdnMidnight).FormatDate().c_str());

    jdnMidnight = wxDate().Set().GetJulianDate();
    printf("wxDateTime for today: %s\n",
            wxDateTime((double)(jdnMidnight + 0.5)).Format("%c", wxDateTime::GMT0).c_str());

    int flags = wxEUROPEAN;//wxFULL;
    wxDate date;
    date.Set();
    printf("Today is %s\n", date.FormatDate(flags).c_str());
    for ( int n = 0; n < 7; n++ )
    {
        printf("Previous %s is %s\n",
               wxDateTime::GetWeekDayName((wxDateTime::WeekDay)n),
               date.Previous(n + 1).FormatDate(flags).c_str());
    }
}

#endif // 0

#endif // TEST_DATETIME

// ----------------------------------------------------------------------------
// threads
// ----------------------------------------------------------------------------

#ifdef TEST_THREADS

#include <wx/thread.h>

static size_t gs_counter = (size_t)-1;
static wxCriticalSection gs_critsect;
static wxCondition gs_cond;

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
    MyDetachedThread(size_t n, char ch)
    {
        m_n = n;
        m_ch = ch;
        m_cancelled = FALSE;

        Create();
    }

    // thread execution starts here
    virtual ExitCode Entry();

    // and stops here
    virtual void OnExit();

private:
    size_t m_n; // number of characters to write
    char m_ch;  // character to write

    bool m_cancelled;   // FALSE if we exit normally
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
            m_cancelled = TRUE;

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
    wxLogTrace("thread", "Thread %ld is in OnExit", GetId());

    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter && !m_cancelled )
        gs_cond.Signal();
}

void TestDetachedThreads()
{
    puts("\n*** Testing detached threads ***");

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

    puts("");
}

void TestJoinableThreads()
{
    puts("\n*** Testing a joinable thread (a loooong calculation...) ***");

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    printf("\nThread terminated with exit code %lu.\n",
           (unsigned long)thread.Wait());
}

void TestThreadSuspend()
{
    puts("\n*** Testing thread suspend/resume functions ***");

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

        puts("\nThread suspended");
        if ( n > 0 )
        {
            // don't sleep but resume immediately the first time
            wxThread::Sleep(300);
        }
        puts("Going to resume the thread");

        thread->Resume();
    }

    puts("Waiting until it terminates now");

    // wait until the thread terminates
    gs_cond.Wait();

    puts("");
}

void TestThreadDelete()
{
    // As above, using Sleep() is only for testing here - we must use some
    // synchronisation object instead to ensure that the thread is still
    // running when we delete it - deleting a detached thread which already
    // terminated will lead to a crash!

    puts("\n*** Testing thread delete function ***");

    MyDetachedThread *thread0 = new MyDetachedThread(30, 'W');

    thread0->Delete();

    puts("\nDeleted a thread which didn't start to run yet.");

    MyDetachedThread *thread1 = new MyDetachedThread(30, 'Y');

    thread1->Run();

    wxThread::Sleep(300);

    thread1->Delete();

    puts("\nDeleted a running thread.");

    MyDetachedThread *thread2 = new MyDetachedThread(30, 'Z');

    thread2->Run();

    wxThread::Sleep(300);

    thread2->Pause();

    thread2->Delete();

    puts("\nDeleted a sleeping thread.");

    MyJoinableThread thread3(20);
    thread3.Run();

    thread3.Delete();

    puts("\nDeleted a joinable thread.");

    MyJoinableThread thread4(2);
    thread4.Run();

    wxThread::Sleep(300);

    thread4.Delete();

    puts("\nDeleted a joinable thread which already terminated.");

    puts("");
}

#endif // TEST_THREADS

// ----------------------------------------------------------------------------
// arrays
// ----------------------------------------------------------------------------

#ifdef TEST_ARRAYS

static void PrintArray(const char* name, const wxArrayString& array)
{
    printf("Dump of the array '%s'\n", name);

    size_t nCount = array.GetCount();
    for ( size_t n = 0; n < nCount; n++ )
    {
        printf("\t%s[%u] = '%s'\n", name, n, array[n].c_str());
    }
}

static void PrintArray(const char* name, const wxArrayInt& array)
{
    printf("Dump of the array '%s'\n", name);

    size_t nCount = array.GetCount();
    for ( size_t n = 0; n < nCount; n++ )
    {
        printf("\t%s[%u] = %d\n", name, n, array[n]);
    }
}

int wxCMPFUNC_CONV StringLenCompare(const wxString& first,
                                    const wxString& second)
{
    return first.length() - second.length();
}

int wxCMPFUNC_CONV IntCompare(int *first,
                              int *second)
{
    return *first - *second;
}

int wxCMPFUNC_CONV IntRevCompare(int *first,
                              int *second)
{
    return *second - *first;
}

static void TestArrayOfInts()
{
    puts("*** Testing wxArrayInt ***\n");

    wxArrayInt a;
    a.Add(1);
    a.Add(17);
    a.Add(5);
    a.Add(3);

    puts("Initially:");
    PrintArray("a", a);

    puts("After sort:");
    a.Sort(IntCompare);
    PrintArray("a", a);

    puts("After reverse sort:");
    a.Sort(IntRevCompare);
    PrintArray("a", a);
}

#include "wx/dynarray.h"

WX_DECLARE_OBJARRAY(Bar, ArrayBars);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayBars);

static void TestArrayOfObjects()
{
    puts("*** Testing wxObjArray ***\n");

    {
        ArrayBars bars;
        Bar bar("second bar");

        printf("Initially: %u objects in the array, %u objects total.\n",
               bars.GetCount(), Bar::GetNumber());

        bars.Add(new Bar("first bar"));
        bars.Add(bar);

        printf("Now: %u objects in the array, %u objects total.\n",
               bars.GetCount(), Bar::GetNumber());

        bars.Empty();

        printf("After Empty(): %u objects in the array, %u objects total.\n",
               bars.GetCount(), Bar::GetNumber());
    }

    printf("Finally: no more objects in the array, %u objects total.\n",
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
    puts("*** Testing wxString constructores ***");

    #define TEST_CTOR(args, res)                                               \
        {                                                                      \
            wxString s args ;                                                  \
            printf("wxString%s = %s ", #args, s.c_str());                      \
            if ( s == res )                                                    \
            {                                                                  \
                puts("(ok)");                                                  \
            }                                                                  \
            else                                                               \
            {                                                                  \
                printf("(ERROR: should be %s)\n", res);                        \
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

    puts("");
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

    printf ("TestString elapsed time: %ld\n", sw.Time());
}

static void TestPChar()
{
    wxStopWatch sw;

    char a [128];
    char b [128];
    char c [128];

    for (int i = 0; i < 1000000; ++i)
    {
        strcpy (a, "Hello");
        strcpy (b, " world");
        strcpy (c, "! How'ya doin'?");
        strcat (a, b);
        strcat (a, c);
        strcpy (c, "Hello world! What's up?");
        if (strcmp (c, a) == 0)
            strcpy (c, "Doh!");
    }

    printf ("TestPChar elapsed time: %ld\n", sw.Time());
}

static void TestStringSub()
{
    wxString s("Hello, world!");

    puts("*** Testing wxString substring extraction ***");

    printf("String = '%s'\n", s.c_str());
    printf("Left(5) = '%s'\n", s.Left(5).c_str());
    printf("Right(6) = '%s'\n", s.Right(6).c_str());
    printf("Mid(3, 5) = '%s'\n", s(3, 5).c_str());
    printf("Mid(3) = '%s'\n", s.Mid(3).c_str());
    printf("substr(3, 5) = '%s'\n", s.substr(3, 5).c_str());
    printf("substr(3) = '%s'\n", s.substr(3).c_str());

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
        printf("StartsWith('%s') = %s", prefix.c_str(), rc ? "TRUE" : "FALSE");
        if ( rc )
        {
            printf(" (the rest is '%s')\n", rest.c_str());
        }
        else
        {
            putchar('\n');
        }
    }

    puts("");
}

static void TestStringFormat()
{
    puts("*** Testing wxString formatting ***");

    wxString s;
    s.Printf("%03d", 18);

    printf("Number 18: %s\n", wxString::Format("%03d", 18).c_str());
    printf("Number 18: %s\n", s.c_str());

    puts("");
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
    puts("*** Testing wxString find() functions ***");

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

        printf(_T("Index of '%s' in '%s' starting from %u is %s "),
               strToFind, ft.str, ft.start, PosToString(res).c_str());

        size_t resTrue = ft.result;
        if ( res == resTrue )
        {
            puts(_T("(ok)"));
        }
        else
        {
            printf(_T("(ERROR: should be %s)\n"),
                   PosToString(resTrue).c_str());
        }
    }

    puts("");
}

static void TestStringTokenizer()
{
    puts("*** Testing wxStringTokenizer ***");

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
        printf(_T("String '%s' has %u tokens delimited by '%s' (mode = %s) "),
               MakePrintable(tt.str).c_str(),
               count,
               MakePrintable(tt.delims).c_str(),
               modeNames[tkz.GetMode()]);
        if ( count == tt.count )
        {
            puts(_T("(ok)"));
        }
        else
        {
            printf(_T("(ERROR: should be %u)\n"), tt.count);

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

            printf(_T("\ttoken %u: '%s'"),
                   ++count2,
                   MakePrintable(token).c_str());

            if ( buf )
            {
                if ( token == s )
                {
                    puts(" (ok)");
                }
                else
                {
                    printf(" (ERROR: should be %s)\n", s);
                }

                s = wxStrtok(NULL, tt.delims, &last);
            }
            else
            {
                // nothing to compare with
                puts("");
            }
        }

        if ( count2 != count )
        {
            puts(_T("\tERROR: token count mismatch"));
        }

        delete [] buf;
    }

    puts("");
}

static void TestStringReplace()
{
    puts("*** Testing wxString::replace ***");

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
            puts("(ok)");
        }
        else
        {
            wxPrintf(_T("(ERROR: should be '%s')\n"), data.result);
        }
    }

    puts("");
}

#endif // TEST_STRINGS

// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if ( !wxInitialize() )
    {
        fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");
    }

#ifdef TEST_USLEEP
    puts("Sleeping for 3 seconds... z-z-z-z-z...");
    wxUsleep(3000);
#endif // TEST_USLEEP

#ifdef TEST_CMDLINE
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },

        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "i", "input",   "input dir" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },

        { wxCMD_LINE_PARAM,  NULL, NULL, "input file",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, argv);

    parser.AddOption("project_name", "", "full path to project file",
                     wxCMD_LINE_VAL_STRING,
                     wxCMD_LINE_OPTION_MANDATORY | wxCMD_LINE_NEEDS_SEPARATOR);

    switch ( parser.Parse() )
    {
        case -1:
            wxLogMessage("Help was given, terminating.");
            break;

        case 0:
            ShowCmdLine(parser);
            break;

        default:
            wxLogMessage("Syntax error detected, aborting.");
            break;
    }
#endif // TEST_CMDLINE

#ifdef TEST_STRINGS
    if ( 0 )
    {
        TestPChar();
        TestString();
    }
        TestStringSub();
    if ( 0 )
    {
        TestStringConstruction();
        TestStringFormat();
        TestStringFind();
        TestStringTokenizer();
        TestStringReplace();
    }
#endif // TEST_STRINGS

#ifdef TEST_ARRAYS
    if ( 0 )
    {
    wxArrayString a1;
    a1.Add("tiger");
    a1.Add("cat");
    a1.Add("lion");
    a1.Add("dog");
    a1.Add("human");
    a1.Add("ape");

    puts("*** Initially:");

    PrintArray("a1", a1);

    wxArrayString a2(a1);
    PrintArray("a2", a2);

    wxSortedArrayString a3(a1);
    PrintArray("a3", a3);

    puts("*** After deleting a string from a1");
    a1.Remove(2);

    PrintArray("a1", a1);
    PrintArray("a2", a2);
    PrintArray("a3", a3);

    puts("*** After reassigning a1 to a2 and a3");
    a3 = a2 = a1;
    PrintArray("a2", a2);
    PrintArray("a3", a3);

    puts("*** After sorting a1");
    a1.Sort();
    PrintArray("a1", a1);

    puts("*** After sorting a1 in reverse order");
    a1.Sort(TRUE);
    PrintArray("a1", a1);

    puts("*** After sorting a1 by the string length");
    a1.Sort(StringLenCompare);
    PrintArray("a1", a1);

    TestArrayOfObjects();
    }
    TestArrayOfInts();
#endif // TEST_ARRAYS

#ifdef TEST_DIR
    TestDirEnum();
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

#ifdef TEST_LOG
    wxString s;
    for ( size_t n = 0; n < 8000; n++ )
    {
        s << (char)('A' + (n % 26));
    }

    wxString msg;
    msg.Printf("A very very long message: '%s', the end!\n", s.c_str());

    // this one shouldn't be truncated
    printf(msg);

    // but this one will because log functions use fixed size buffer
    // (note that it doesn't need '\n' at the end neither - will be added
    //  by wxLog anyhow)
    wxLogMessage("A very very long message 2: '%s', the end!", s.c_str());
#endif // TEST_LOG

#ifdef TEST_FILE
    if ( 0 )
    {
        TestFileRead();
        TestTextFileRead();
    }
    TestFileCopy();
#endif // TEST_FILE

#ifdef TEST_FILENAME
    TestFileNameConstruction();
    if ( 0 )
    {
        TestFileNameCwd();
        TestFileNameComparison();
        TestFileNameOperations();
    }
#endif // TEST_FILENAME

#ifdef TEST_THREADS
    int nCPUs = wxThread::GetCPUCount();
    printf("This system has %d CPUs\n", nCPUs);
    if ( nCPUs != -1 )
        wxThread::SetConcurrency(nCPUs);

    if ( argc > 1 && argv[1][0] == 't' )
        wxLog::AddTraceMask("thread");

    if ( 1 )
        TestDetachedThreads();
    if ( 1 )
        TestJoinableThreads();
    if ( 1 )
        TestThreadSuspend();
    if ( 1 )
        TestThreadDelete();

#endif // TEST_THREADS

#ifdef TEST_LONGLONG
    // seed pseudo random generator
    srand((unsigned)time(NULL));

    if ( 0 )
    {
        TestSpeed();
    }
    if ( 0 )
    {
        TestMultiplication();
        TestDivision();
        TestAddition();
        TestLongLongConversion();
        TestBitOperations();
    }
    TestLongLongComparison();
#endif // TEST_LONGLONG

#ifdef TEST_HASH
    TestHash();
#endif // TEST_HASH

#ifdef TEST_MIME
    wxLog::AddTraceMask(_T("mime"));
    if ( 0 )
    {
        TestMimeEnum();
        TestMimeOverride();
        TestMimeFilename();
    }
    TestMimeAssociate();
#endif // TEST_MIME

#ifdef TEST_INFO_FUNCTIONS
    TestOsInfo();
    TestUserInfo();
#endif // TEST_INFO_FUNCTIONS

#ifdef TEST_REGISTRY
    if ( 0 )
        TestRegistryRead();
    TestRegistryAssociation();
#endif // TEST_REGISTRY

#ifdef TEST_SOCKETS
    if ( 0 )
    {
        TestSocketServer();
    }
        TestSocketClient();
#endif // TEST_SOCKETS

#ifdef TEST_FTP
    wxLog::AddTraceMask(_T("ftp"));
    TestProtocolFtp();
    if ( 0 )
    TestProtocolFtpUpload();
#endif // TEST_FTP

#ifdef TEST_STREAMS
    TestMemoryStream();
#endif // TEST_STREAMS

#ifdef TEST_TIMER
    TestStopWatch();
#endif // TEST_TIMER

#ifdef TEST_DATETIME
    if ( 0 )
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
        TestTimeMS();

        TestTimeZoneBug();
    }
    if ( 0 )
        TestInteractive();
#endif // TEST_DATETIME

#ifdef TEST_VCARD
    if ( 0 )
    TestVCardRead();
    TestVCardWrite();
#endif // TEST_VCARD

#ifdef TEST_WCHAR
    TestUtf8();
#endif // TEST_WCHAR

#ifdef TEST_ZIP
    TestZipStreamRead();
#endif // TEST_ZIP

#ifdef TEST_ZLIB
    if ( 0 )
    TestZlibStreamWrite();
    TestZlibStreamRead();
#endif // TEST_ZLIB

    wxUninitialize();

    return 0;
}

