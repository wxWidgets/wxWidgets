///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filename/filename.cpp
// Purpose:     wxFileName unit test
// Author:      Vadim Zeitlin
// Created:     2004-07-25
// Copyright:   (c) 2004 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/stdpaths.h"
#include "wx/scopeguard.h"

#ifdef __WINDOWS__
    #include "wx/msw/registry.h"
    #include "wx/msw/wrapshl.h"
    #include "wx/msw/ole/oleutils.h"
    #include "wx/msw/private/comptr.h"
#endif // __WINDOWS__

#ifdef __UNIX__
    #include <unistd.h>
#endif // __UNIX__

#include "testfile.h"
#include "testdate.h"

// ----------------------------------------------------------------------------
// test data
// ----------------------------------------------------------------------------

static struct TestFileNameInfo
{
    const char *fullname;
    const char *volume;
    const char *path;
    const char *name;
    const char *ext;
    bool isAbsolute;
    wxPathFormat format;
} filenames[] =
{
    // the empty string
    { "", "", "", "", "", false, wxPATH_UNIX },
    { "", "", "", "", "", false, wxPATH_DOS },
    { "", "", "", "", "", false, wxPATH_VMS },

    // Unix file names
    { "/usr/bin/ls", "", "/usr/bin", "ls", "", true, wxPATH_UNIX },
    { "/usr/bin/", "", "/usr/bin", "", "", true, wxPATH_UNIX },
    { "~/.zshrc", "", "~", ".zshrc", "", true, wxPATH_UNIX },
    { "../../foo", "", "../..", "foo", "", false, wxPATH_UNIX },
    { "foo.bar", "", "", "foo", "bar", false, wxPATH_UNIX },
    { "~/foo.bar", "", "~", "foo", "bar", true, wxPATH_UNIX },
    { "~user/foo.bar", "", "~user", "foo", "bar", true, wxPATH_UNIX },
    { "~user/", "", "~user", "", "", true, wxPATH_UNIX },
    { "/foo", "", "/", "foo", "", true, wxPATH_UNIX },
    { "Mahogany-0.60/foo.bar", "", "Mahogany-0.60", "foo", "bar", false, wxPATH_UNIX },
    { "/tmp/wxwin.tar.bz", "", "/tmp", "wxwin.tar", "bz", true, wxPATH_UNIX },

    // Windows file names
    { "foo.bar", "", "", "foo", "bar", false, wxPATH_DOS },
    { "\\foo.bar", "", "\\", "foo", "bar", false, wxPATH_DOS },
    { "c:foo.bar", "c", "", "foo", "bar", false, wxPATH_DOS },
    { "c:\\foo.bar", "c", "\\", "foo", "bar", true, wxPATH_DOS },
    { "c:\\Windows\\command.com", "c", "\\Windows", "command", "com", true, wxPATH_DOS },
    { "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\",
      "Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}", "\\", "", "", true, wxPATH_DOS },
    { "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\Program Files\\setup.exe",
      "Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}", "\\Program Files", "setup", "exe", true, wxPATH_DOS },

#if 0
    // NB: when using the wxFileName::GetLongPath() function on these two
    //     strings, the program will hang for several seconds blocking inside
    //     Win32 GetLongPathName() function
    { "\\\\server\\foo.bar", "server", "\\", "foo", "bar", true, wxPATH_DOS },
    { "\\\\server\\dir\\foo.bar", "server", "\\dir", "foo", "bar", true, wxPATH_DOS },
#endif

    // consecutive [back]slashes should be treated as single occurrences of
    // them and not interpreted as share names if there is a volume name
    { "c:\\aaa\\bbb\\ccc", "c", "\\aaa\\bbb", "ccc", "", true, wxPATH_DOS },
    { "c:\\\\aaa\\bbb\\ccc", "c", "\\\\aaa\\bbb", "ccc", "", true, wxPATH_DOS },

    // wxFileName support for Mac file names is broken currently
#if 0
    // Mac file names
    { "Volume:Dir:File", "Volume", "Dir", "File", "", true, wxPATH_MAC },
    { "Volume:Dir:Subdir:File", "Volume", "Dir:Subdir", "File", "", true, wxPATH_MAC },
    { "Volume:", "Volume", "", "", "", true, wxPATH_MAC },
    { ":Dir:File", "", "Dir", "File", "", false, wxPATH_MAC },
    { ":File.Ext", "", "", "File", ".Ext", false, wxPATH_MAC },
    { "File.Ext", "", "", "File", ".Ext", false, wxPATH_MAC },
#endif // 0

#if 0
    // VMS file names
    // NB: on Windows they have the same effect of the \\server\\ strings
    //     (see the note above)
    { "device:[dir1.dir2.dir3]file.txt", "device", "dir1.dir2.dir3", "file", "txt", true, wxPATH_VMS },
#endif
    { "file.txt", "", "", "file", "txt", false, wxPATH_VMS },
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileNameTestCase : public CppUnit::TestCase
{
public:
    FileNameTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FileNameTestCase );
        CPPUNIT_TEST( TestConstruction );
        CPPUNIT_TEST( TestComparison );
        CPPUNIT_TEST( TestSplit );
        CPPUNIT_TEST( TestSetPath );
        CPPUNIT_TEST( TestStrip );
        CPPUNIT_TEST( TestNormalize );
        CPPUNIT_TEST( TestRelative );
        CPPUNIT_TEST( TestReplace );
        CPPUNIT_TEST( TestGetHumanReadable );
#ifdef __WINDOWS__
        CPPUNIT_TEST( TestShortLongPath );
#endif // __WINDOWS__
        CPPUNIT_TEST( TestUNC );
        CPPUNIT_TEST( TestVolumeUniqueName );
        CPPUNIT_TEST( TestCreateTempFileName );
        CPPUNIT_TEST( TestGetTimes );
        CPPUNIT_TEST( TestSetTimes );
        CPPUNIT_TEST( TestExists );
        CPPUNIT_TEST( TestIsSame );
#if defined(__UNIX__)
        CPPUNIT_TEST( TestSymlinks );
#endif // __UNIX__
#ifdef __WINDOWS__
        CPPUNIT_TEST( TestShortcuts );
#endif // __WINDOWS__
    CPPUNIT_TEST_SUITE_END();

    void TestConstruction();
    void TestComparison();
    void TestSplit();
    void TestSetPath();
    void TestStrip();
    void TestRelative();
    void TestNormalize();
    void TestReplace();
    void TestGetHumanReadable();
#ifdef __WINDOWS__
    void TestShortLongPath();
#endif // __WINDOWS__
    void TestUNC();
    void TestVolumeUniqueName();
    void TestCreateTempFileName();
    void TestGetTimes();
    void TestSetTimes();
    void TestExists();
    void TestIsSame();
#if defined(__UNIX__)
    void TestSymlinks();
#endif // __UNIX__
#ifdef __WINDOWS__
    void TestShortcuts();
#endif // __WINDOWS__

    wxDECLARE_NO_COPY_CLASS(FileNameTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileNameTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileNameTestCase, "FileNameTestCase" );

void FileNameTestCase::TestConstruction()
{
    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const TestFileNameInfo& fni = filenames[n];

        wxFileName fn(fni.fullname, fni.format);

        // the original full name could contain consecutive [back]slashes,
        // squeeze them except for the double backslash in the beginning in
        // Windows filenames where it has special meaning
        wxString fullnameOrig;
        if ( fni.format == wxPATH_DOS )
        {
            // copy the backslashes at beginning unchanged
            const char *p = fni.fullname;
            while ( *p == '\\' )
                fullnameOrig += *p++;

            // replace consecutive slashes with single ones in the rest
            for ( char chPrev = '\0'; *p; p++ )
            {
                if ( *p == '\\' && chPrev == '\\' )
                    continue;

                chPrev = *p;
                fullnameOrig += chPrev;
            }
        }
        else // !wxPATH_DOS
        {
            fullnameOrig = fni.fullname;
        }

        fullnameOrig.Replace("//", "/");


        wxString fullname = fn.GetFullPath(fni.format);
        CPPUNIT_ASSERT_EQUAL( fullnameOrig, fullname );

        // notice that we use a dummy working directory to ensure that paths
        // with "../.." in them could be normalized, otherwise this would fail
        // if the test is run from root directory or its direct subdirectory
        CPPUNIT_ASSERT_MESSAGE
        (
            (const char *)wxString::Format("Normalize(%s) failed", fni.fullname).mb_str(),
            fn.Normalize(wxPATH_NORM_ALL, "/foo/bar/baz", fni.format)
        );

        if ( *fni.volume && *fni.path )
        {
            // check that specifying the volume separately or as part of the
            // path doesn't make any difference
            wxString pathWithVolume = fni.volume;
            pathWithVolume += wxFileName::GetVolumeSeparator(fni.format);
            pathWithVolume += fni.path;

            CPPUNIT_ASSERT_EQUAL( wxFileName(pathWithVolume,
                                             fni.name,
                                             fni.ext,
                                             fni.format), fn );
        }
    }

    wxFileName fn;

    // empty strings
    fn.AssignDir(wxEmptyString);
    CPPUNIT_ASSERT( !fn.IsOk() );

    fn.Assign(wxEmptyString);
    CPPUNIT_ASSERT( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString);
    CPPUNIT_ASSERT( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString, wxEmptyString);
    CPPUNIT_ASSERT( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
    CPPUNIT_ASSERT( !fn.IsOk() );
}

void FileNameTestCase::TestComparison()
{
    wxFileName fn1(wxT("/tmp/file1"));
    wxFileName fn2(wxT("/tmp/dir2/../file2"));
    fn1.Normalize();
    fn2.Normalize();
    CPPUNIT_ASSERT_EQUAL(fn1.GetPath(), fn2.GetPath());
}

void FileNameTestCase::TestSplit()
{
    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const TestFileNameInfo& fni = filenames[n];
        wxString volume, path, name, ext;
        wxFileName::SplitPath(fni.fullname,
                              &volume, &path, &name, &ext, fni.format);

        CPPUNIT_ASSERT_EQUAL( wxString(fni.volume), volume );
        CPPUNIT_ASSERT_EQUAL( wxString(fni.path), path );
        CPPUNIT_ASSERT_EQUAL( wxString(fni.name), name );
        CPPUNIT_ASSERT_EQUAL( wxString(fni.ext), ext );
    }

    // special case of empty extension
    wxFileName fn("foo.");
    CPPUNIT_ASSERT_EQUAL( wxString("foo."), fn.GetFullPath() );
}

void FileNameTestCase::TestSetPath()
{
    wxFileName fn("d:\\test\\foo.bar", wxPATH_DOS);
    fn.SetPath("c:\\temp", wxPATH_DOS);
    CPPUNIT_ASSERT( fn.SameAs(wxFileName("c:\\temp\\foo.bar", wxPATH_DOS)) );

    fn = wxFileName("/usr/bin/ls", wxPATH_UNIX);
    fn.SetPath("/usr/local/bin", wxPATH_UNIX);
    CPPUNIT_ASSERT( fn.SameAs(wxFileName("/usr/local/bin/ls", wxPATH_UNIX)) );
}

void FileNameTestCase::TestNormalize()
{
    // prepare some data to be used later
    wxString sep = wxFileName::GetPathSeparator();
    wxString cwd = wxGetCwd();
    wxString home = wxGetUserHome();

    cwd.Replace(sep, wxT("/"));
    if (cwd.Last() != wxT('/'))
        cwd += wxT('/');
    home.Replace(sep, wxT("/"));
    if (home.Last() != wxT('/'))
        home += wxT('/');

    // since we will always be testing paths using the wxPATH_UNIX
    // format, we need to remove the volume, if present
    if (home.Contains(wxT(':')))
        home = home.AfterFirst(wxT(':'));
    if (cwd.Contains(wxT(':')))
        cwd = cwd.AfterFirst(wxT(':'));

    static const struct FileNameTest
    {
        const char *original;
        int flags;
        const char *expected;
        wxPathFormat fmt;
    } tests[] =
    {
        // test wxPATH_NORM_ENV_VARS
#ifdef __WINDOWS__
        { "%ABCDEF%/g/h/i", wxPATH_NORM_ENV_VARS, "abcdef/g/h/i", wxPATH_UNIX },
#else
        { "$(ABCDEF)/g/h/i", wxPATH_NORM_ENV_VARS, "abcdef/g/h/i", wxPATH_UNIX },
#endif

        // test wxPATH_NORM_DOTS
        { "a/.././b/c/../../", wxPATH_NORM_DOTS, "", wxPATH_UNIX },
        { "", wxPATH_NORM_DOTS, "", wxPATH_UNIX },
        { "./foo", wxPATH_NORM_DOTS, "foo", wxPATH_UNIX },
        { "b/../bar", wxPATH_NORM_DOTS, "bar", wxPATH_UNIX },
        { "c/../../quux", wxPATH_NORM_DOTS, "../quux", wxPATH_UNIX },
        { "/c/../../quux", wxPATH_NORM_DOTS, "/quux", wxPATH_UNIX },
        { "../../quux", wxPATH_NORM_DOTS, "../../quux", wxPATH_UNIX },

        // test wxPATH_NORM_TILDE: notice that ~ is only interpreted specially
        // when it is the first character in the file name
        { "/a/b/~", wxPATH_NORM_TILDE, "/a/b/~", wxPATH_UNIX },
        { "/~/a/b", wxPATH_NORM_TILDE, "/~/a/b", wxPATH_UNIX },
        { "~/a/b", wxPATH_NORM_TILDE, "HOME/a/b", wxPATH_UNIX },

        // test wxPATH_NORM_CASE
        { "Foo", wxPATH_NORM_CASE, "Foo", wxPATH_UNIX },
        { "Foo", wxPATH_NORM_CASE, "foo", wxPATH_DOS },
        { "C:\\Program Files\\wx", wxPATH_NORM_CASE,
          "c:\\program files\\wx", wxPATH_DOS },
        { "C:/Program Files/wx", wxPATH_NORM_ALL | wxPATH_NORM_CASE,
          "c:\\program files\\wx", wxPATH_DOS },
        { "C:\\Users\\zeitlin", wxPATH_NORM_ALL | wxPATH_NORM_CASE,
          "c:\\users\\zeitlin", wxPATH_DOS },

        // test wxPATH_NORM_ABSOLUTE
        { "a/b/", wxPATH_NORM_ABSOLUTE, "CWD/a/b/", wxPATH_UNIX },
        { "a/b/c.ext", wxPATH_NORM_ABSOLUTE, "CWD/a/b/c.ext", wxPATH_UNIX },
        { "/a", wxPATH_NORM_ABSOLUTE, "/a", wxPATH_UNIX },

        // test giving no flags at all to Normalize()
        { "a/b/", 0, "a/b/", wxPATH_UNIX },
        { "a/b/c.ext", 0, "a/b/c.ext", wxPATH_UNIX },
        { "/a", 0, "/a", wxPATH_UNIX },

        // test handling dots without wxPATH_NORM_DOTS and wxPATH_NORM_ABSOLUTE
        // for both existing and non-existent files (this is important under
        // MSW where GetLongPathName() works only for the former)
        { "./foo", wxPATH_NORM_LONG, "./foo", wxPATH_UNIX },
        { "../foo", wxPATH_NORM_LONG, "../foo", wxPATH_UNIX },
        { ".\\test.bkl", wxPATH_NORM_LONG, ".\\test.bkl", wxPATH_DOS },
        { ".\\foo", wxPATH_NORM_LONG, ".\\foo", wxPATH_DOS },
        { "..\\Makefile.in", wxPATH_NORM_LONG, "..\\Makefile.in", wxPATH_DOS },
        { "..\\foo", wxPATH_NORM_LONG, "..\\foo", wxPATH_DOS },
    };

    // set the env var ABCDEF
    wxSetEnv("ABCDEF", "abcdef");

    for ( size_t i = 0; i < WXSIZEOF(tests); i++ )
    {
        const FileNameTest& fnt = tests[i];
        wxFileName fn(fnt.original, fnt.fmt);

        // be sure this normalization does not fail
        WX_ASSERT_MESSAGE
        (
            ("#%d: Normalize(%s) failed", (int)i, fnt.original),
            fn.Normalize(fnt.flags, cwd, fnt.fmt)
        );

        // compare result with expected string
        wxString expected(tests[i].expected);
        expected.Replace("HOME/", home);
        expected.Replace("CWD/", cwd);
        WX_ASSERT_EQUAL_MESSAGE
        (
            ("array element #%d", (int)i),
            expected, fn.GetFullPath(fnt.fmt)
        );
    }

    // MSW-only test for wxPATH_NORM_LONG: notice that we only run it if short
    // names generation is not disabled for this system as otherwise the file
    // MKINST~1 doesn't exist at all and normalizing it fails (it's possible
    // that we're on a FAT partition in which case the test would still succeed
    // and also that the registry key was changed recently and didn't take
    // effect yet but these are marginal cases which we consciously choose to
    // ignore for now)
#ifdef __WINDOWS__
    long shortNamesDisabled;
    if ( wxRegKey
         (
            wxRegKey::HKLM,
            "SYSTEM\\CurrentControlSet\\Control\\FileSystem"
         ).QueryValue("NtfsDisable8dot3NameCreation", &shortNamesDisabled) &&
            !shortNamesDisabled )
    {
        wxFileName fn("..\\MKINST~1");
        CPPUNIT_ASSERT( fn.Normalize(wxPATH_NORM_LONG, cwd) );
        CPPUNIT_ASSERT_EQUAL( "..\\mkinstalldirs", fn.GetFullPath() );
    }
    //else: when in doubt, don't run the test
#endif // __WINDOWS__
}

void FileNameTestCase::TestRelative()
{
    const wxString pathSep = wxFileName::GetPathSeparator();

    wxFileName fn("a" + pathSep + "b.cpp");
    fn.MakeRelativeTo("a");
    CPPUNIT_ASSERT_EQUAL( "b.cpp", fn.GetFullPath() );

    fn.AssignDir("a" + pathSep + "b");
    fn.MakeRelativeTo("a");

    CPPUNIT_ASSERT_EQUAL( "b" + pathSep, fn.GetFullPath() );

    fn.AssignDir("a");
    fn.MakeRelativeTo("a");
    CPPUNIT_ASSERT_EQUAL( "." + pathSep, fn.GetFullPath() );
}

void FileNameTestCase::TestReplace()
{
    static const struct FileNameTest
    {
        const char *original;
        const char *env_contents;
        const char *replace_fmtstring;
        const char *expected;
        wxPathFormat fmt;
    } tests[] =
    {
        { "/usr/a/strange path/lib/someFile.ext", "/usr/a/strange path", "$%s", "$TEST_VAR/lib/someFile.ext", wxPATH_UNIX },
        { "/usr/a/path/lib/someFile.ext", "/usr/a/path", "$%s", "$TEST_VAR/lib/someFile.ext", wxPATH_UNIX },
        { "/usr/a/path/lib/someFile", "/usr/a/path/", "$%s", "$TEST_VARlib/someFile", wxPATH_UNIX },
        { "/usr/a/path/lib/", "/usr/a/path/", "$(%s)", "$(TEST_VAR)lib/", wxPATH_UNIX },
        { "/usr/a/path/lib/", "/usr/a/path/", "${{%s}}", "${{TEST_VAR}}lib/", wxPATH_UNIX },
        { "/usr/a/path/lib/", "/usr/a/path/", "%s", "TEST_VARlib/", wxPATH_UNIX },
        { "/usr/a/path/lib/", "/usr/a/path/", "%s//", "TEST_VAR/lib/", wxPATH_UNIX },
            // note: empty directory components are automatically removed by wxFileName thus
            //       using // in the replace format string has no effect

        { "/usr/../a/path/lib/", "/usr/a/path/", "%s", "/usr/../a/path/lib/", wxPATH_UNIX },
        { "/usr/a/path/usr/usr", "/usr", "%s", "TEST_VAR/a/pathTEST_VAR/usr", wxPATH_UNIX },
        { "/usr/a/path/usr/usr", "/usr", "$%s", "$TEST_VAR/a/path$TEST_VAR/usr", wxPATH_UNIX },
        { "/a/b/c/d", "a/", "%s", "/TEST_VARb/c/d", wxPATH_UNIX },

        { "C:\\A\\Strange Path\\lib\\someFile", "C:\\A\\Strange Path", "%%%s%%", "%TEST_VAR%\\lib\\someFile", wxPATH_WIN },
        { "C:\\A\\Path\\lib\\someFile", "C:\\A\\Path", "%%%s%%", "%TEST_VAR%\\lib\\someFile", wxPATH_WIN },
        { "C:\\A\\Path\\lib\\someFile", "C:\\A\\Path", "$(%s)", "$(TEST_VAR)\\lib\\someFile", wxPATH_WIN }
    };

    for ( size_t i = 0; i < WXSIZEOF(tests); i++ )
    {
        const FileNameTest& fnt = tests[i];
        wxFileName fn(fnt.original, fnt.fmt);

        // set the environment variable
        wxSetEnv("TEST_VAR", fnt.env_contents);

        // be sure this ReplaceEnvVariable does not fail
        WX_ASSERT_MESSAGE
        (
            ("#%d: ReplaceEnvVariable(%s) failed", (int)i, fnt.replace_fmtstring),
            fn.ReplaceEnvVariable("TEST_VAR", fnt.replace_fmtstring, fnt.fmt)
        );

        // compare result with expected string
        wxString expected(fnt.expected);
        WX_ASSERT_EQUAL_MESSAGE
        (
            ("array element #%d", (int)i),
            expected, fn.GetFullPath(fnt.fmt)
        );
    }

    // now test ReplaceHomeDir

    wxFileName fn = wxFileName::DirName(wxGetHomeDir());
    fn.AppendDir("test1");
    fn.AppendDir("test2");
    fn.AppendDir("test3");
    fn.SetName("some file");

    WX_ASSERT_MESSAGE
    (
        ("ReplaceHomeDir(%s) failed", fn.GetFullPath()),
        fn.ReplaceHomeDir()
    );

    CPPUNIT_ASSERT_EQUAL( wxString("~/test1/test2/test3/some file"),
                          fn.GetFullPath(wxPATH_UNIX) );
}

void FileNameTestCase::TestGetHumanReadable()
{
    static const struct TestData
    {
        const char *result;
        int size;
        int prec;
        wxSizeConvention conv;
    } testData[] =
    {
        { "NA",             0, 1, wxSIZE_CONV_TRADITIONAL },
        { "2.0 KB",      2000, 1, wxSIZE_CONV_TRADITIONAL },
        { "1.953 KiB",   2000, 3, wxSIZE_CONV_IEC         },
        { "2.000 KB",    2000, 3, wxSIZE_CONV_SI          },
        { "297 KB",    304351, 0, wxSIZE_CONV_TRADITIONAL },
        { "304 KB",    304351, 0, wxSIZE_CONV_SI          },
    };

    CLocaleSetter loc;      // we want to use "C" locale for LC_NUMERIC
                            // so that regardless of the system's locale
                            // the decimal point used by GetHumanReadableSize()
                            // is always '.'
    for ( unsigned n = 0; n < WXSIZEOF(testData); n++ )
    {
        const TestData& td = testData[n];

        // take care of using the decimal point for the current locale before
        // the actual comparison
        CPPUNIT_ASSERT_EQUAL
        (
            td.result,
            wxFileName::GetHumanReadableSize(td.size, "NA", td.prec, td.conv)
        );
    }

    // also test the default convention value
    CPPUNIT_ASSERT_EQUAL( "1.4 MB", wxFileName::GetHumanReadableSize(1512993, "") );
}

void FileNameTestCase::TestStrip()
{
    CPPUNIT_ASSERT_EQUAL( "", wxFileName::StripExtension("") );
    CPPUNIT_ASSERT_EQUAL( ".", wxFileName::StripExtension(".") );
    CPPUNIT_ASSERT_EQUAL( ".vimrc", wxFileName::StripExtension(".vimrc") );
    CPPUNIT_ASSERT_EQUAL( "bad", wxFileName::StripExtension("bad") );
    CPPUNIT_ASSERT_EQUAL( "good", wxFileName::StripExtension("good.wav") );
    CPPUNIT_ASSERT_EQUAL( "good.wav", wxFileName::StripExtension("good.wav.wav") );
}

#ifdef __WINDOWS__

void FileNameTestCase::TestShortLongPath()
{
    wxFileName fn("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");

    // incredibly enough, GetLongPath() used to return different results during
    // the first and subsequent runs, test for this
    CPPUNIT_ASSERT_EQUAL( fn.GetLongPath(), fn.GetLongPath() );
    CPPUNIT_ASSERT_EQUAL( fn.GetShortPath(), fn.GetShortPath() );
}

#endif // __WINDOWS__

void FileNameTestCase::TestUNC()
{
    wxFileName fn("//share/path/name.ext", wxPATH_DOS);
    CPPUNIT_ASSERT_EQUAL( "share", fn.GetVolume() );
    CPPUNIT_ASSERT_EQUAL( "\\path", fn.GetPath(wxPATH_NO_SEPARATOR, wxPATH_DOS) );

    fn.Assign("\\\\share2\\path2\\name.ext", wxPATH_DOS);
    CPPUNIT_ASSERT_EQUAL( "share2", fn.GetVolume() );
    CPPUNIT_ASSERT_EQUAL( "\\path2", fn.GetPath(wxPATH_NO_SEPARATOR, wxPATH_DOS) );
}

void FileNameTestCase::TestVolumeUniqueName()
{
    wxFileName fn("\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\",
                  wxPATH_DOS);
    CPPUNIT_ASSERT_EQUAL( "Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}",
                          fn.GetVolume() );
    CPPUNIT_ASSERT_EQUAL( "\\", fn.GetPath(wxPATH_NO_SEPARATOR, wxPATH_DOS) );
    CPPUNIT_ASSERT_EQUAL( "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\",
                          fn.GetFullPath(wxPATH_DOS) );

    fn.Assign("\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\"
              "Program Files\\setup.exe", wxPATH_DOS);
    CPPUNIT_ASSERT_EQUAL( "Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}",
                          fn.GetVolume() );
    CPPUNIT_ASSERT_EQUAL( "\\Program Files",
                          fn.GetPath(wxPATH_NO_SEPARATOR, wxPATH_DOS) );
    CPPUNIT_ASSERT_EQUAL( "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\"
                          "Program Files\\setup.exe",
                          fn.GetFullPath(wxPATH_DOS) );
}

void FileNameTestCase::TestCreateTempFileName()
{
    static const struct TestData
    {
        const char *prefix;
        const char *expectedFolder;
        bool shouldSucceed;
    } testData[] =
    {
        { "", "$SYSTEM_TEMP", true },
        { "foo", "$SYSTEM_TEMP", true },
        { "..", "$SYSTEM_TEMP", true },
        { "../bar", "..", true },
#ifdef __WINDOWS__
        { "$USER_DOCS_DIR\\", "$USER_DOCS_DIR", true },
        { "c:\\a\\directory\\which\\does\\not\\exist", "", false },
#elif defined( __UNIX__ )
        { "$USER_DOCS_DIR/", "$USER_DOCS_DIR", true },
        { "/tmp/foo", "/tmp", true },
        { "/tmp/a/directory/which/does/not/exist", "", false },
#endif // __UNIX__
    };

    for ( size_t n = 0; n < WXSIZEOF(testData); n++ )
    {
        wxString prefix = testData[n].prefix;
        prefix.Replace("$USER_DOCS_DIR", wxStandardPaths::Get().GetDocumentsDir());

        std::string errDesc = wxString::Format("failed on prefix '%s'", prefix).ToStdString();

        wxString path = wxFileName::CreateTempFileName(prefix);
        CPPUNIT_ASSERT_EQUAL_MESSAGE( errDesc, !testData[n].shouldSucceed, path.empty() );

        if (testData[n].shouldSucceed)
        {
            errDesc += "; path is " + path.ToStdString();

            // test the place where the temp file has been created
            wxString expected = testData[n].expectedFolder;
            expected.Replace("$SYSTEM_TEMP", wxStandardPaths::Get().GetTempDir());
            expected.Replace("$USER_DOCS_DIR", wxStandardPaths::Get().GetDocumentsDir());
            CPPUNIT_ASSERT_EQUAL_MESSAGE( errDesc, expected, wxFileName(path).GetPath() );

            // the temporary file is created with full permissions for the current process
            // so we should always be able to remove it:
            CPPUNIT_ASSERT_MESSAGE( errDesc, wxRemoveFile(path) );
        }
    }
}

void FileNameTestCase::TestGetTimes()
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    CPPUNIT_ASSERT( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    wxDateTime dtAccess, dtMod, dtCreate;
    CPPUNIT_ASSERT( fn.GetTimes(&dtAccess, &dtMod, &dtCreate) );

    // make sure all retrieved dates are equal to the current date&time
    // with an accuracy up to 1 minute
    CPPUNIT_ASSERT(dtCreate.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
    CPPUNIT_ASSERT(dtMod.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
    CPPUNIT_ASSERT(dtAccess.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
}

void FileNameTestCase::TestSetTimes()
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    CPPUNIT_ASSERT( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    const wxDateTime dtAccess(1, wxDateTime::Jan, 2013);
    const wxDateTime dtModify(1, wxDateTime::Feb, 2013);
    const wxDateTime dtCreate(1, wxDateTime::Mar, 2013);

    CPPUNIT_ASSERT( fn.SetTimes(&dtAccess, &dtModify, &dtCreate) );

    wxDateTime dtAccess2,
               dtModify2,
               dtCreate2;
    CPPUNIT_ASSERT( fn.GetTimes(&dtAccess2, &dtModify2, &dtCreate2) );
    CPPUNIT_ASSERT_EQUAL( dtAccess, dtAccess2 );
    CPPUNIT_ASSERT_EQUAL( dtModify, dtModify2 );

    // Under Unix the creation time can't be set.
#ifdef __WINDOWS__
    CPPUNIT_ASSERT_EQUAL( dtCreate, dtCreate2 );
#endif // __WINDOWS__
}

void FileNameTestCase::TestExists()
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    CPPUNIT_ASSERT( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    CPPUNIT_ASSERT( fn.FileExists() );
    CPPUNIT_ASSERT( !wxFileName::DirExists(fn.GetFullPath()) );

    CPPUNIT_ASSERT( fn.Exists(wxFILE_EXISTS_REGULAR) );
    CPPUNIT_ASSERT( !fn.Exists(wxFILE_EXISTS_DIR) );
    CPPUNIT_ASSERT( fn.Exists() );

    const wxString& tempdir = wxFileName::GetTempDir();

    wxFileName fileInTempDir(tempdir, "bloordyblop");
    CPPUNIT_ASSERT( !fileInTempDir.Exists() );
    CPPUNIT_ASSERT( fileInTempDir.DirExists() );

    wxFileName dirTemp(wxFileName::DirName(tempdir));
    CPPUNIT_ASSERT( !dirTemp.FileExists() );
    CPPUNIT_ASSERT( dirTemp.DirExists() );

    CPPUNIT_ASSERT( dirTemp.Exists(wxFILE_EXISTS_DIR) );
    CPPUNIT_ASSERT( !dirTemp.Exists(wxFILE_EXISTS_REGULAR) );
    CPPUNIT_ASSERT( dirTemp.Exists() );

#ifdef __UNIX__
    CPPUNIT_ASSERT( !wxFileName::FileExists("/dev/null") );
    CPPUNIT_ASSERT( !wxFileName::DirExists("/dev/null") );
    CPPUNIT_ASSERT( wxFileName::Exists("/dev/null") );
    CPPUNIT_ASSERT( wxFileName::Exists("/dev/null", wxFILE_EXISTS_DEVICE) );
#ifdef __LINUX__
    // These files are only guaranteed to exist under Linux.
    // No need for wxFILE_EXISTS_NO_FOLLOW here; wxFILE_EXISTS_SYMLINK implies it
    CPPUNIT_ASSERT( wxFileName::Exists("/proc/self", wxFILE_EXISTS_SYMLINK) );
    CPPUNIT_ASSERT( wxFileName::Exists("/dev/log", wxFILE_EXISTS_SOCKET) );
#endif // __LINUX__
#ifndef __VMS
    wxString fifo = dirTemp.GetPath() + "/fifo";
   if (mkfifo(fifo.c_str(), 0600) == 0)
    {
        wxON_BLOCK_EXIT1(wxRemoveFile, fifo);

        CPPUNIT_ASSERT( wxFileName::Exists(fifo, wxFILE_EXISTS_FIFO) );
    }
#endif
#endif // __UNIX__
}

void FileNameTestCase::TestIsSame()
{
    wxFileName fn1( wxFileName::CreateTempFileName( "filenametest1" ) );
    CPPUNIT_ASSERT( fn1.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn1.GetFullPath() );

    wxFileName fn2( wxFileName::CreateTempFileName( "filenametest2" ) );
    CPPUNIT_ASSERT( fn2.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn2.GetFullPath() );

    CPPUNIT_ASSERT( fn1.SameAs( fn1 ) );
    CPPUNIT_ASSERT( !fn1.SameAs( fn2 ) );

#if defined(__UNIX__)
    // We need to create a temporary directory and a temporary link.
    // Unfortunately we can't use wxFileName::CreateTempFileName() for neither
    // as it creates plain files, so use tempnam() explicitly instead.
    char* tn = tempnam(NULL, "wxfn1");
    const wxString tempdir1 = wxString::From8BitData(tn);
    free(tn);

    CPPUNIT_ASSERT( wxFileName::Mkdir(tempdir1) );
    // Unfortunately the casts are needed to select the overload we need here.
    wxON_BLOCK_EXIT2( static_cast<bool (*)(const wxString&, int)>(wxFileName::Rmdir),
                      tempdir1, static_cast<int>(wxPATH_RMDIR_RECURSIVE) );

    tn = tempnam(NULL, "wxfn2");
    const wxString tempdir2 = wxString::From8BitData(tn);
    free(tn);
    CPPUNIT_ASSERT_EQUAL( 0, symlink(tempdir1.c_str(), tempdir2.c_str()) );
    wxON_BLOCK_EXIT1( wxRemoveFile, tempdir2 );


    wxFileName fn3(tempdir1, "foo");
    wxFileName fn4(tempdir2, "foo");

    // These files have different paths, hence are different.
    CPPUNIT_ASSERT( !fn3.SameAs(fn4) );

    // Create and close a file to trigger creating it.
    wxFile(fn3.GetFullPath(), wxFile::write);

    // Now that both files do exist we should be able to detect that they are
    // actually the same file.
    CPPUNIT_ASSERT( fn3.SameAs(fn4) );
#endif // __UNIX__
}

#if defined(__UNIX__)

// Tests for functions that are changed by ShouldFollowLink()
void FileNameTestCase::TestSymlinks()
{
    const wxString tmpdir(wxStandardPaths::Get().GetTempDir());

    wxFileName tmpfn(wxFileName::DirName(tmpdir));

    // Create a temporary directory
#ifdef __VMS
    wxString name = tmpdir + ".filenametestXXXXXX]";
    mkdir( name.char_str() , 0222 );
    wxString tempdir = name;
#else
    wxString name = tmpdir + "/filenametestXXXXXX";
    wxString tempdir = wxString::From8BitData(mkdtemp(name.char_str()));
    tempdir << wxFileName::GetPathSeparator();
#endif
    wxFileName tempdirfn(wxFileName::DirName(tempdir));
    CPPUNIT_ASSERT(tempdirfn.DirExists());

    // Create a regular file in that dir, to act as a symlink target
    wxFileName targetfn(wxFileName::CreateTempFileName(tempdir));
    CPPUNIT_ASSERT(targetfn.FileExists());

    // Create a symlink to that file
    wxFileName linktofile(tempdir, "linktofile");
    CPPUNIT_ASSERT_EQUAL(0, symlink(targetfn.GetFullPath().c_str(),
                                        linktofile.GetFullPath().c_str()));

    // ... and another to the temporary directory
    const wxString linktodirName(tempdir + "/linktodir");
    wxFileName linktodir(wxFileName::DirName(linktodirName));
    CPPUNIT_ASSERT_EQUAL(0, symlink(tmpfn.GetFullPath().c_str(),
                                        linktodirName.c_str()));

    // And symlinks to both of those symlinks
    wxFileName linktofilelnk(tempdir, "linktofilelnk");
    CPPUNIT_ASSERT_EQUAL(0, symlink(linktofile.GetFullPath().c_str(),
                                    linktofilelnk.GetFullPath().c_str()));
    wxFileName linktodirlnk(tempdir, "linktodirlnk");
    CPPUNIT_ASSERT_EQUAL(0, symlink(linktodir.GetFullPath().c_str(),
                                    linktodirlnk.GetFullPath().c_str()));

    // Run the tests twice: once in the default symlink following mode and the
    // second time without following symlinks.
    bool deref = true;
    for ( int n = 0; n < 2; ++n, deref = !deref )
    {
        const std::string msg(deref ? " failed for the link target"
                                    : " failed for the path itself");

        if ( !deref )
        {
            linktofile.DontFollowLink();
            linktodir.DontFollowLink();
            linktofilelnk.DontFollowLink();
            linktodirlnk.DontFollowLink();
        }

        // Test SameAs()
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Comparison with file" + msg,
            deref, linktofile.SameAs(targetfn)
        );

        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Comparison with directory" + msg,
            deref, linktodir.SameAs(tmpfn)
        );

        // A link-to-a-link should dereference through to the final target
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Comparison with link to a file" + msg,
            deref,
            linktofilelnk.SameAs(targetfn)
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Comparison with link to a directory" + msg,
            deref,
            linktodirlnk.SameAs(tmpfn)
        );

        // Test GetTimes()
        wxDateTime dtAccess, dtMod, dtCreate;
        CPPUNIT_ASSERT_MESSAGE
        (
            "Getting times of a directory" + msg,
            linktodir.GetTimes(&dtAccess, &dtMod, &dtCreate)
        );

        // Test (File|Dir)Exists()
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing file existence" + msg,
            deref,
            linktofile.FileExists()
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing directory existence" + msg,
            deref,
            linktodir.DirExists()
        );

        // Test wxFileName::Exists
        // The wxFILE_EXISTS_NO_FOLLOW flag should override DontFollowLink()
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing file existence" + msg,
            false,
            linktofile.Exists(wxFILE_EXISTS_REGULAR | wxFILE_EXISTS_NO_FOLLOW)
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing directory existence" + msg,
            false,
            linktodir.Exists(wxFILE_EXISTS_DIR | wxFILE_EXISTS_NO_FOLLOW)
        );
        // and the static versions
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing file existence" + msg,
            false,
            wxFileName::Exists(linktofile.GetFullPath(), wxFILE_EXISTS_REGULAR | wxFILE_EXISTS_NO_FOLLOW)
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing file existence" + msg,
            true,
            wxFileName::Exists(linktofile.GetFullPath(), wxFILE_EXISTS_REGULAR)
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing directory existence" + msg,
            false,
            wxFileName::Exists(linktodir.GetFullPath(), wxFILE_EXISTS_DIR | wxFILE_EXISTS_NO_FOLLOW)
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE
        (
            "Testing directory existence" + msg,
            true,
            wxFileName::Exists(linktodir.GetFullPath(), wxFILE_EXISTS_DIR)
        );
    }

    // Finally test Exists() after removing the file.
    CPPUNIT_ASSERT(wxRemoveFile(targetfn.GetFullPath()));
    // This should succeed, as the symlink still exists and
    // the default wxFILE_EXISTS_ANY implies wxFILE_EXISTS_NO_FOLLOW
    CPPUNIT_ASSERT(wxFileName(tempdir, "linktofile").Exists());
    // So should this one, as wxFILE_EXISTS_SYMLINK does too
    CPPUNIT_ASSERT(wxFileName(tempdir, "linktofile").
                                            Exists(wxFILE_EXISTS_SYMLINK));
    // but not this one, as the now broken symlink is followed
    CPPUNIT_ASSERT(!wxFileName(tempdir, "linktofile").
                                            Exists(wxFILE_EXISTS_REGULAR));
    CPPUNIT_ASSERT(linktofile.Exists());

    // This is also a convenient place to test Rmdir() as we have things to
    // remove.

    // First, check that removing a symlink to a directory fails.
    CPPUNIT_ASSERT( !wxFileName::Rmdir(linktodirName) );

    // And recursively removing it only removes the symlink itself, not the
    // directory.
    CPPUNIT_ASSERT( wxFileName::Rmdir(linktodirName, wxPATH_RMDIR_RECURSIVE) );
    CPPUNIT_ASSERT( tmpfn.Exists() );

    // Finally removing the directory itself does remove everything.
    CPPUNIT_ASSERT(tempdirfn.Rmdir(wxPATH_RMDIR_RECURSIVE));
    CPPUNIT_ASSERT( !tempdirfn.Exists() );
}

#endif // __UNIX__

#ifdef __WINDOWS__

namespace
{

void CreateShortcut(const wxString& pathFile, const wxString& pathLink)
{
   wxOleInitializer oleInit;

   HRESULT hr;

   wxCOMPtr<IShellLink> sl;
   hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                         IID_IShellLink, (void **)&sl);
   CPPUNIT_ASSERT( SUCCEEDED(hr) );

   wxCOMPtr<IPersistFile> pf;
   hr = sl->QueryInterface(IID_IPersistFile, (void **)&pf);
   CPPUNIT_ASSERT( SUCCEEDED(hr) );

   hr = sl->SetPath(pathFile.t_str());
   CPPUNIT_ASSERT( SUCCEEDED(hr) );

   hr = pf->Save(pathLink.wc_str(), TRUE);
   CPPUNIT_ASSERT( SUCCEEDED(hr) );
}

} // anonymous namespace

void FileNameTestCase::TestShortcuts()
{
   wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
   CPPUNIT_ASSERT( fn.IsOk() );
   wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

   wxFileName fnLink(fn.GetPath(), "sc_" + fn.GetName(), "lnk");
   CPPUNIT_ASSERT( fnLink.IsOk() );
   wxON_BLOCK_EXIT1( wxRemoveFile, fnLink.GetFullPath() );

   CreateShortcut(fn.GetFullPath(), fnLink.GetFullPath());

   // MakeRelativeTo() is supposed to change only the path of the file, not its
   // name.
   wxFileName fnLinkRel(fnLink);
   fnLink.MakeRelativeTo(".");
   CPPUNIT_ASSERT_EQUAL(fnLink.GetFullName(), fnLinkRel.GetFullName());
}

#endif // __WINDOWS__

#ifdef __LINUX__

// Check that GetSize() works correctly for special files.
TEST_CASE("wxFileName::GetSizeSpecial", "[filename][linux][special-file]")
{
    wxULongLong size = wxFileName::GetSize("/proc/kcore");
    INFO( "size of /proc/kcore=" << size );
    CHECK( size > 0 );

    // All files in /sys are one page in size, irrespectively of the size of
    // their actual contents.
    CHECK( wxFileName::GetSize("/sys/power/state") == sysconf(_SC_PAGESIZE) );
}

#endif // __LINUX__
