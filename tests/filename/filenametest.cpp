///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filename/filename.cpp
// Purpose:     wxFileName unit test
// Author:      Vadim Zeitlin
// Created:     2004-07-25
// RCS-ID:      $Id$
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

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
#endif // __WXMSW__

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

// define stream inserter for wxFileName to use it in CPPUNIT_ASSERT_EQUAL()
inline std::ostream& operator<<(std::ostream& o, const wxFileName& fn)
{
    return o << fn.GetFullPath();
}

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
        CPPUNIT_TEST( TestReplace );
#ifdef __WINDOWS__
        CPPUNIT_TEST( TestShortLongPath );
#endif // __WINDOWS__
    CPPUNIT_TEST_SUITE_END();

    void TestConstruction();
    void TestComparison();
    void TestSplit();
    void TestSetPath();
    void TestStrip();
    void TestNormalize();
    void TestReplace();
#ifdef __WINDOWS__
    void TestShortLongPath();
#endif // __WINDOWS__

    DECLARE_NO_COPY_CLASS(FileNameTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileNameTestCase );

// also include in it's own registry so that these tests can be run alone
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
#ifdef __WXMSW__
        { "%ABCDEF%/g/h/i", wxPATH_NORM_ENV_VARS, "abcdef/g/h/i", wxPATH_UNIX },
#else
        { "$(ABCDEF)/g/h/i", wxPATH_NORM_ENV_VARS, "abcdef/g/h/i", wxPATH_UNIX },
#endif

        // test wxPATH_NORM_DOTS
        { "a/.././b/c/../../", wxPATH_NORM_DOTS, "", wxPATH_UNIX },

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
#ifdef __WXMSW__
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
#endif // __WXMSW__
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
