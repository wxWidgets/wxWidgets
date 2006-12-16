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
    // the empty string
    { _T(""), _(""), _(""), _(""), _(""), false, wxPATH_UNIX }, 
    { _T(""), _(""), _(""), _(""), _(""), false, wxPATH_DOS }, 
    { _T(""), _(""), _(""), _(""), _(""), false, wxPATH_VMS }, 

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

    // NB: when using the wxFileName::GetLongPath() function on these two strings,
    //     the program will hang various seconds. All those time is taken by the
    //     call to the win32 API GetLongPathName()...
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
    // NB: on Windows they have the same effect of the \\server\\ strings
    //     (see the note above)
    { _T("device:[dir1.dir2.dir3]file.txt"), _T("device"), _T("dir1.dir2.dir3"), _T("file"), _T("txt"), true, wxPATH_VMS },
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
        CPPUNIT_TEST( TestStrip );
        CPPUNIT_TEST( TestNormalize );
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
        const FileNameInfo& fni = filenames[n];

        wxFileName fn(fni.fullname, fni.format);

        wxString fullname = fn.GetFullPath(fni.format);
        CPPUNIT_ASSERT_EQUAL( wxString(fni.fullname), fullname );

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
        const FileNameInfo& fni = filenames[n];
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

    static struct FileNameTest
    {
        const wxChar *original;
        int flags;
        wxString expected;
    } tests[] =
    {
        // test wxPATH_NORM_ENV_VARS
#ifdef __WXMSW__
        { wxT("%ABCDEF%/g/h/i"), wxPATH_NORM_ENV_VARS, wxT("abcdef/g/h/i") },
#else
        { wxT("$(ABCDEF)/g/h/i"), wxPATH_NORM_ENV_VARS, wxT("abcdef/g/h/i") },
#endif

        // test wxPATH_NORM_DOTS
        { wxT("a/.././b/c/../../"), wxPATH_NORM_DOTS, wxT("") },

        // test wxPATH_NORM_TILDE
        // NB: do the tilde expansion also under Windows to test if it works there too
        { wxT("/a/b/~"), wxPATH_NORM_TILDE, wxT("/a/b/~") },
        { wxT("/~/a/b"), wxPATH_NORM_TILDE, home + wxT("a/b") },
        { wxT("~/a/b"), wxPATH_NORM_TILDE, home + wxT("a/b") },

        // test wxPATH_NORM_ABSOLUTE
        { wxT("a/b/"), wxPATH_NORM_ABSOLUTE, cwd + wxT("a/b/") },
        { wxT("a/b/c.ext"), wxPATH_NORM_ABSOLUTE, cwd + wxT("a/b/c.ext") },
        { wxT("/a"), wxPATH_NORM_ABSOLUTE, wxT("/a") },

        // test giving no flags at all to Normalize()
        { wxT("a/b/"), 0, wxT("a/b/") },
        { wxT("a/b/c.ext"), 0, wxT("a/b/c.ext") },
        { wxT("/a"), 0, wxT("/a") }
    };

    // set the env var ABCDEF
    wxSetEnv(_T("ABCDEF"), _T("abcdef"));

    for ( size_t i = 0; i < WXSIZEOF(tests); i++ )
    {
        wxFileName fn(tests[i].original, wxPATH_UNIX);

        // be sure this normalization does not fail
        CPPUNIT_ASSERT_MESSAGE
        (
            (const char *)wxString::Format(_T("Normalize(%s) failed"), tests[i].original).mb_str(),
            fn.Normalize(tests[i].flags, cwd, wxPATH_UNIX)
        );

        // compare result with expected string
        CPPUNIT_ASSERT_EQUAL( tests[i].expected, fn.GetFullPath(wxPATH_UNIX) );
    }
}

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
