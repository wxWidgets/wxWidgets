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
    const wxChar *fullname;
    const wxChar *volume;
    const wxChar *path;
    const wxChar *name;
    const wxChar *ext;
    bool isAbsolute;
    wxPathFormat format;
} filenames[] =
{
    // the empty string
    { _T(""), _T(""), _T(""), _T(""), _T(""), false, wxPATH_UNIX },
    { _T(""), _T(""), _T(""), _T(""), _T(""), false, wxPATH_DOS },
    { _T(""), _T(""), _T(""), _T(""), _T(""), false, wxPATH_VMS },

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

#if 0
    // NB: when using the wxFileName::GetLongPath() function on these two
    //     strings, the program will hang for several seconds blocking inside
    //     Win32 GetLongPathName() function
    { _T("\\\\server\\foo.bar"), _T("server"), _T("\\"), _T("foo"), _T("bar"), true, wxPATH_DOS },
    { _T("\\\\server\\dir\\foo.bar"), _T("server"), _T("\\dir"), _T("foo"), _T("bar"), true, wxPATH_DOS },
#endif

    // consecutive [back]slashes should be treated as single occurrences of
    // them and not interpreted as share names if there is a volume name
    { _T("c:\\aaa\\bbb\\ccc"), _T("c"), _T("\\aaa\\bbb"), _T("ccc"), _T(""), true, wxPATH_DOS },
    { _T("c:\\\\aaa\\bbb\\ccc"), _T("c"), _T("\\\\aaa\\bbb"), _T("ccc"), _T(""), true, wxPATH_DOS },

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

#if 0
    // VMS file names
    // NB: on Windows they have the same effect of the \\server\\ strings
    //     (see the note above)
    { _T("device:[dir1.dir2.dir3]file.txt"), _T("device"), _T("dir1.dir2.dir3"), _T("file"), _T("txt"), true, wxPATH_VMS },
#endif
    { _T("file.txt"), _T(""), _T(""), _T("file"), _T("txt"), false, wxPATH_VMS },
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
#if WXWIN_COMPATIBILITY_2_8
        CPPUNIT_TEST( TestStrip );
#endif
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
#if WXWIN_COMPATIBILITY_2_8
    void TestStrip();
#endif
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
            const wxChar *p = fni.fullname;
            while ( *p == _T('\\') )
                fullnameOrig += *p++;

            // replace consecutive slashes with single ones in the rest
            for ( wxChar chPrev = _T('\0'); *p; p++ )
            {
                if ( *p == _T('\\') && chPrev == _T('\\') )
                    continue;

                chPrev = *p;
                fullnameOrig += chPrev;
            }
        }
        else // !wxPATH_DOS
        {
            fullnameOrig = fni.fullname;
        }

        fullnameOrig.Replace(_T("//"), _T("/"));


        wxString fullname = fn.GetFullPath(fni.format);
        CPPUNIT_ASSERT_EQUAL( fullnameOrig, fullname );

        // notice that we use a dummy working directory to ensure that paths
        // with "../.." in them could be normalized, otherwise this would fail
        // if the test is run from root directory or its direct subdirectory
        CPPUNIT_ASSERT_MESSAGE
        (
            (const char *)wxString::Format(_T("Normalize(%s) failed"), fni.fullname).mb_str(),
            fn.Normalize(wxPATH_NORM_ALL, _T("/foo/bar/baz"), fni.format)
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
    wxFileName fn(_T("foo."));
    CPPUNIT_ASSERT_EQUAL( wxString(_T("foo.")), fn.GetFullPath() );
}

void FileNameTestCase::TestSetPath()
{
    wxFileName fn(_T("d:\\test\\foo.bar"), wxPATH_DOS);
    fn.SetPath(_T("c:\\temp"), wxPATH_DOS);
    CPPUNIT_ASSERT( fn.SameAs(wxFileName(_T("c:\\temp\\foo.bar"), wxPATH_DOS)) );

    fn = wxFileName(_T("/usr/bin/ls"), wxPATH_UNIX);
    fn.SetPath(_T("/usr/local/bin"), wxPATH_UNIX);
    CPPUNIT_ASSERT( fn.SameAs(wxFileName(_T("/usr/local/bin/ls"), wxPATH_UNIX)) );
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

        // test wxPATH_NORM_TILDE
        // NB: do the tilde expansion also under Windows to test if it works there too
        { "/a/b/~", wxPATH_NORM_TILDE, "/a/b/~", wxPATH_UNIX },
        { "/~/a/b", wxPATH_NORM_TILDE, "HOME/a/b", wxPATH_UNIX },
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
#ifdef __WXMSW__
        { "..\\MKINST~1", wxPATH_NORM_LONG, "..\\mkinstalldirs", wxPATH_DOS },
#endif
    };

    // set the env var ABCDEF
    wxSetEnv(_T("ABCDEF"), _T("abcdef"));

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
        expected.Replace(_T("HOME/"), home);
        expected.Replace(_T("CWD/"), cwd);
        WX_ASSERT_EQUAL_MESSAGE
        (
            ("array element #%d", (int)i),
            expected, fn.GetFullPath(fnt.fmt)
        );
    }
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
        wxSetEnv(_T("TEST_VAR"), fnt.env_contents);

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

    CPPUNIT_ASSERT_EQUAL( wxString(_T("~/test1/test2/test3/some file")),
                          fn.GetFullPath(wxPATH_UNIX) );
}

#if WXWIN_COMPATIBILITY_2_8
wxString wxTestStripExtension(wxString szFile)
{
    wxStripExtension(szFile);
    return szFile;
}

void FileNameTestCase::TestStrip()
{
    //test a crash
    CPPUNIT_ASSERT_EQUAL( wxString(_T("")), wxTestStripExtension(_T("")) );

    //others
    CPPUNIT_ASSERT_EQUAL( wxString(_T("")), wxTestStripExtension(_T(".")) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("")), wxTestStripExtension(_T(".wav")) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("good")), wxTestStripExtension(_T("good.wav")) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("good.wav")), wxTestStripExtension(_T("good.wav.wav")) );
}
#endif

#ifdef __WINDOWS__

void FileNameTestCase::TestShortLongPath()
{
    wxFileName fn(_T("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe"));

    // incredibly enough, GetLongPath() used to return different results during
    // the first and subsequent runs, test for this
    CPPUNIT_ASSERT_EQUAL( fn.GetLongPath(), fn.GetLongPath() );
    CPPUNIT_ASSERT_EQUAL( fn.GetShortPath(), fn.GetShortPath() );
}

#endif // __WINDOWS__
