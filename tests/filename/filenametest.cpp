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


#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/stdpaths.h"
#include "wx/scopeguard.h"
#include "wx/sckipc.h"

#include "wx/private/localeset.h"

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

// Use a hack to keep using wxPATH_NORM_ALL in this test code without getting
// deprecation warnings for it.
#define wxPATH_NORM_ALL wxPATH_NORM_DEPR_OLD_DEFAULT

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
      "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}", "\\", "", "", true, wxPATH_DOS },
    { "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\Program Files\\setup.exe",
      "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}", "\\Program Files", "setup", "exe", true, wxPATH_DOS },

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
// tests
// ----------------------------------------------------------------------------

TEST_CASE("wxFileName::Construction", "[filename]")
{
    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const TestFileNameInfo& fni = filenames[n];

        INFO("Test #" << n << ": \"" << fni.fullname << "\"");

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


        CHECK( fn.GetFullPath(fni.format) == fullnameOrig );

        // notice that we use a dummy working directory to ensure that paths
        // with "../.." in them could be normalized, otherwise this would fail
        // if the test is run from root directory or its direct subdirectory
        CHECK( fn.Normalize(wxPATH_NORM_ALL, "/foo/bar/baz", fni.format) );

        // restrict this check to drive letter volumes as UNC and GUID volumes
        // can't be combined with the path using the volume separator
        if ( strlen(fni.volume) == 1 && *fni.path )
        {
            // check that specifying the volume separately or as part of the
            // path doesn't make any difference
            wxString pathWithVolume = fni.volume;
            pathWithVolume += wxFileName::GetVolumeSeparator(fni.format);
            pathWithVolume += fni.path;

            CHECK( wxFileName(pathWithVolume,
                              fni.name,
                              fni.ext,
                              fni.format) == fn );
        }
    }

    wxFileName fn;

    // empty strings
    fn.AssignDir(wxEmptyString);
    CHECK( !fn.IsOk() );

    fn.Assign(wxEmptyString);
    CHECK( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString);
    CHECK( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString, wxEmptyString);
    CHECK( !fn.IsOk() );

    fn.Assign(wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
    CHECK( !fn.IsOk() );
}

TEST_CASE("wxFileName::Comparison", "[filename]")
{
    wxFileName fn1(wxT("/tmp/file1"));
    wxFileName fn2(wxT("/tmp/dir2/../file2"));
    fn1.MakeAbsolute();
    fn2.MakeAbsolute();
    CHECK(fn1.GetPath() == fn2.GetPath());
}

TEST_CASE("wxFileName::Split", "[filename]")
{
    for ( size_t n = 0; n < WXSIZEOF(filenames); n++ )
    {
        const TestFileNameInfo& fni = filenames[n];
        wxString volume, path, name, ext;
        wxFileName::SplitPath(fni.fullname,
                              &volume, &path, &name, &ext, fni.format);

        CHECK( volume == fni.volume );
        CHECK( path == fni.path );
        CHECK( name == fni.name );
        CHECK( ext == fni.ext );
    }

    // special case of empty extension
    wxFileName fn("foo.");
    CHECK( fn.GetFullPath() == "foo." );
}

TEST_CASE("wxFileName::SetPath", "[filename]")
{
    wxFileName fn("d:\\test\\foo.bar", wxPATH_DOS);
    fn.SetPath("c:\\temp", wxPATH_DOS);
    CHECK( fn.SameAs(wxFileName("c:\\temp\\foo.bar", wxPATH_DOS)) );

    fn = wxFileName("/usr/bin/ls", wxPATH_UNIX);
    fn.SetPath("/usr/local/bin", wxPATH_UNIX);
    CHECK( fn.SameAs(wxFileName("/usr/local/bin/ls", wxPATH_UNIX)) );
}

TEST_CASE("wxFileName::Normalize", "[filename]")
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

    static const char* pathWithEnvVar =
#ifdef __WINDOWS__
        "%ABCDEF%/g/h/i"
#else
        "$(ABCDEF)/g/h/i"
#endif
        ;

    static const struct FileNameTest
    {
        const char *original;
        int flags;
        const char *expected;
        wxPathFormat fmt;
    } tests[] =
    {
        // test wxPATH_NORM_ENV_VARS
        { pathWithEnvVar, wxPATH_NORM_ENV_VARS, "abcdef/g/h/i", wxPATH_UNIX },

        // test wxPATH_NORM_DOTS
        { "a/.././b/c/../../", wxPATH_NORM_DOTS, "", wxPATH_UNIX },
        { "", wxPATH_NORM_DOTS, "", wxPATH_UNIX },
        { "./foo", wxPATH_NORM_DOTS, "foo", wxPATH_UNIX },
        { "foo/./bar", wxPATH_NORM_DOTS, "foo/bar", wxPATH_UNIX },
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

        // test default behaviour with deprecated wxPATH_NORM_ALL
        { pathWithEnvVar, wxPATH_NORM_ALL, "CWD/abcdef/g/h/i", wxPATH_UNIX },
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

    // Check that paths are made absolute, but environment variables are not
    // expanded in them.
    CHECK( wxFileName(pathWithEnvVar).GetAbsolutePath()
            == wxFileName(wxGetCwd() + "/" + pathWithEnvVar).GetFullPath() );

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
            shortNamesDisabled != 1 )
    {
        wxFileName fn("TESTDA~1.CON");
        CHECK( fn.Normalize(wxPATH_NORM_LONG, cwd) );
        CHECK( fn.GetFullPath() == "testdata.conf" );
    }
    //else: when in doubt, don't run the test
#endif // __WINDOWS__
}

TEST_CASE("wxFileName::Relative", "[filename]")
{
    const wxString pathSep = wxFileName::GetPathSeparator();

    wxFileName fn("a" + pathSep + "b.cpp");
    fn.MakeRelativeTo("a");
    CHECK( fn.GetFullPath() == "b.cpp" );

    fn.AssignDir("a" + pathSep + "b");
    fn.MakeRelativeTo("a");

    CHECK( fn.GetFullPath() == "b" + pathSep );

    fn.AssignDir("a");
    fn.MakeRelativeTo("a");
    CHECK( fn.GetFullPath() == "." + pathSep );
}

TEST_CASE("wxFileName::Replace", "[filename]")
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

    CHECK( fn.GetFullPath(wxPATH_UNIX) == "~/test1/test2/test3/some file" );
}

TEST_CASE("wxFileName::GetHumanReadable", "[filename]")
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

    wxCLocaleSetter loc;    // we want to use "C" locale for LC_NUMERIC
                            // so that regardless of the system's locale
                            // the decimal point used by GetHumanReadableSize()
                            // is always '.'
    for ( unsigned n = 0; n < WXSIZEOF(testData); n++ )
    {
        const TestData& td = testData[n];

        // take care of using the decimal point for the current locale before
        // the actual comparison
        CHECK
        (
            wxFileName::GetHumanReadableSize(td.size, "NA", td.prec, td.conv)
            ==
            td.result
        );
    }

    // also test the default convention value
    CHECK( wxFileName::GetHumanReadableSize(1512993, "") == "1.4 MB" );
}

TEST_CASE("wxFileName::Strip", "[filename]")
{
    CHECK( wxFileName::StripExtension("") == "" );
    CHECK( wxFileName::StripExtension(".") == "." );
    CHECK( wxFileName::StripExtension(".vimrc") == ".vimrc" );
    CHECK( wxFileName::StripExtension("bad") == "bad" );
    CHECK( wxFileName::StripExtension("good.wav") == "good" );
    CHECK( wxFileName::StripExtension("good.wav.wav") == "good.wav" );
}

#ifdef __WINDOWS__

TEST_CASE("wxFileName::ShortLongPath", "[filename]")
{
    wxFileName fn("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");

    // incredibly enough, GetLongPath() used to return different results during
    // the first and subsequent runs, test for this
    CHECK( fn.GetLongPath() == fn.GetLongPath() );
    CHECK( fn.GetShortPath() == fn.GetShortPath() );
}

#endif // __WINDOWS__

// Small helper to make things slightly less verbose in the tests below.
static wxString GetDOSPath(const wxFileName& fn)
{
    return fn.GetPath(wxPATH_NO_SEPARATOR, wxPATH_DOS);
}

TEST_CASE("wxFileName::UNC", "[filename]")
{
    wxFileName fn("//share/path/name.ext", wxPATH_DOS);
    CHECK( fn.GetVolume() == "\\\\share" );
    CHECK( GetDOSPath(fn) == "\\path" );

    fn.Assign("\\\\share2\\path2\\name.ext", wxPATH_DOS);
    CHECK( fn.GetVolume() == "\\\\share2" );
    CHECK( GetDOSPath(fn) == "\\path2" );

    fn.SetPath("\\\\server\\volume\\path", wxPATH_DOS);
    fn.AppendDir("subdir");
    CHECK( fn.GetFullPath(wxPATH_DOS) == "\\\\server\\volume\\path\\subdir\\name.ext" );

    // Check for a bug with normalization breaking the path (#22275).
    fn.Normalize(wxPATH_NORM_LONG);
    CHECK( fn.GetFullPath(wxPATH_DOS) == "\\\\server\\volume\\path\\subdir\\name.ext" );

#ifdef __WINDOWS__
    // Check that doubled backslashes in the beginning of the path are not
    // misinterpreted as UNC volume when we have a drive letter in the
    // beginning.
    fn.Assign("d:\\\\root\\directory\\file");
    CHECK( fn.GetFullPath() == "d:\\root\\directory\\file" );

    // Check that single letter UNC paths don't turn into drive letters, as
    // they used to do.
    fn.Assign("\\\\x\\dir\\file");
    CHECK( fn.GetFullPath() == "\\\\x\\dir\\file" );
#endif // __WINDOWS__
}

TEST_CASE("wxFileName::VolumeUniqueName", "[filename]")
{
    wxFileName fn("\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\",
                  wxPATH_DOS);
    CHECK( fn.GetVolume() == "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}" );
    CHECK( GetDOSPath(fn) == "\\" );
    CHECK( fn.GetFullPath(wxPATH_DOS) == "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\" );

    fn.Assign("\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\"
              "Program Files\\setup.exe", wxPATH_DOS);
    CHECK( fn.GetVolume() == "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}" );
    CHECK( GetDOSPath(fn) == "\\Program Files" );
    CHECK( fn.GetFullPath(wxPATH_DOS) == "\\\\?\\Volume{8089d7d7-d0ac-11db-9dd0-806d6172696f}\\Program Files\\setup.exe" );
}

TEST_CASE("wxFileName::CreateTempFileName", "[filename]")
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

        INFO("Prefix: " << prefix);

        wxString path = wxFileName::CreateTempFileName(prefix);
        CHECK( path.empty() == !testData[n].shouldSucceed );

        if (testData[n].shouldSucceed)
        {
            INFO("Path: " << path);

            // test the place where the temp file has been created
            wxString expected = testData[n].expectedFolder;
            expected.Replace("$SYSTEM_TEMP", wxStandardPaths::Get().GetTempDir());
            expected.Replace("$USER_DOCS_DIR", wxStandardPaths::Get().GetDocumentsDir());
            CHECK( wxFileName(path).GetPath() == expected );

            // the temporary file is created with full permissions for the current process
            // so we should always be able to remove it:
            CHECK( wxRemoveFile(path) );
        }
    }
}

TEST_CASE("wxFileName::GetTimes", "[filename]")
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    REQUIRE( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    wxDateTime dtAccess, dtMod, dtCreate;
    REQUIRE( fn.GetTimes(&dtAccess, &dtMod, &dtCreate) );

    // make sure all retrieved dates are equal to the current date&time
    // with an accuracy up to 1 minute
    CHECK(dtCreate.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
    CHECK(dtMod.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
    CHECK(dtAccess.IsEqualUpTo(wxDateTime::Now(), wxTimeSpan(0,1)));
}

TEST_CASE("wxFileName::SetTimes", "[filename]")
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    REQUIRE( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    const wxDateTime dtAccess(1, wxDateTime::Jan, 2013);
    const wxDateTime dtModify(1, wxDateTime::Feb, 2013);
    const wxDateTime dtCreate(1, wxDateTime::Mar, 2013);

    REQUIRE( fn.SetTimes(&dtAccess, &dtModify, &dtCreate) );

    wxDateTime dtAccess2,
               dtModify2,
               dtCreate2;
    REQUIRE( fn.GetTimes(&dtAccess2, &dtModify2, &dtCreate2) );
    CHECK( dtAccess2 == dtAccess );
    CHECK( dtModify2 == dtModify );

    // Under Unix the creation time can't be set.
#ifdef __WINDOWS__
    CHECK( dtCreate2 == dtCreate );
#endif // __WINDOWS__
}

TEST_CASE("wxFileName::Exists", "[filename]")
{
    wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
    REQUIRE( fn.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

    CHECK( fn.FileExists() );
    CHECK( !wxFileName::DirExists(fn.GetFullPath()) );

    CHECK( fn.Exists(wxFILE_EXISTS_REGULAR) );
    CHECK( !fn.Exists(wxFILE_EXISTS_DIR) );
    CHECK( fn.Exists() );

    const wxString& tempdir = wxFileName::GetTempDir();

    wxFileName fileInTempDir(tempdir, "bloordyblop");
    CHECK( !fileInTempDir.Exists() );
    CHECK( fileInTempDir.DirExists() );

    wxFileName dirTemp(wxFileName::DirName(tempdir));
    CHECK( !dirTemp.FileExists() );
    CHECK( dirTemp.DirExists() );

    CHECK( dirTemp.Exists(wxFILE_EXISTS_DIR) );
    CHECK( !dirTemp.Exists(wxFILE_EXISTS_REGULAR) );
    CHECK( dirTemp.Exists() );

#ifdef __UNIX__
    CHECK( !wxFileName::FileExists("/dev/null") );
    CHECK( !wxFileName::DirExists("/dev/null") );
    CHECK( wxFileName::Exists("/dev/null") );
    CHECK( wxFileName::Exists("/dev/null", wxFILE_EXISTS_DEVICE) );
#ifdef __LINUX__
    // These files are only guaranteed to exist under Linux.
    // No need for wxFILE_EXISTS_NO_FOLLOW here; wxFILE_EXISTS_SYMLINK implies it
    CHECK( wxFileName::Exists("/proc/self", wxFILE_EXISTS_SYMLINK) );
#endif // __LINUX__
#ifndef __VMS
   // OpenVMS does not have mkdtemp
    wxString name = dirTemp.GetPath() + "/socktmpdirXXXXXX";
    wxString socktempdir = wxString::From8BitData(mkdtemp(name.char_str()));
    wxON_BLOCK_EXIT2(wxRmdir, socktempdir, 0);
    wxString sockfile = socktempdir + "/socket";
    wxTCPServer server;
    server.Create(sockfile);
    CHECK( wxFileName::Exists(sockfile, wxFILE_EXISTS_SOCKET) );
    wxString fifo = dirTemp.GetPath() + "/fifo";
   if (mkfifo(fifo.c_str(), 0600) == 0)
    {
        wxON_BLOCK_EXIT1(wxRemoveFile, fifo);

        CHECK( wxFileName::Exists(fifo, wxFILE_EXISTS_FIFO) );
    }
#endif
#endif // __UNIX__
}

TEST_CASE("wxFileName::Mkdir", "[filename]")
{
    wxFileName fn;
    fn.AssignDir("/foo/bar");
    if ( fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
    {
        CHECK( fn.DirExists() );
        CHECK( fn.Rmdir() );
    }
    //else: creating the directory may fail because of permissions
}

TEST_CASE("wxFileName::SameAs", "[filename]")
{
    wxFileName fn1( wxFileName::CreateTempFileName( "filenametest1" ) );
    REQUIRE( fn1.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn1.GetFullPath() );

    wxFileName fn2( wxFileName::CreateTempFileName( "filenametest2" ) );
    REQUIRE( fn2.IsOk() );
    wxON_BLOCK_EXIT1( wxRemoveFile, fn2.GetFullPath() );

    CHECK( fn1.SameAs( fn1 ) );
    CHECK( !fn1.SameAs( fn2 ) );

#if defined(__UNIX__)
    // We need to create a temporary directory and a temporary link.
    // Unfortunately we can't use wxFileName::CreateTempFileName() for neither
    // as it creates plain files, so use tempnam() explicitly instead.
    char* tn = tempnam(NULL, "wxfn1");
    const wxString tempdir1 = wxString::From8BitData(tn);
    free(tn);

    REQUIRE( wxFileName::Mkdir(tempdir1) );
    // Unfortunately the casts are needed to select the overload we need here.
    wxON_BLOCK_EXIT2( static_cast<bool (*)(const wxString&, int)>(wxFileName::Rmdir),
                      tempdir1, static_cast<int>(wxPATH_RMDIR_RECURSIVE) );

    tn = tempnam(NULL, "wxfn2");
    const wxString tempdir2 = wxString::From8BitData(tn);
    free(tn);
    CHECK( symlink(tempdir1.c_str(), tempdir2.c_str()) == 0 );
    wxON_BLOCK_EXIT1( wxRemoveFile, tempdir2 );


    wxFileName fn3(tempdir1, "foo");
    wxFileName fn4(tempdir2, "foo");

    // These files have different paths, hence are different.
    CHECK( !fn3.SameAs(fn4) );

    // Create and close a file to trigger creating it.
    wxFile(fn3.GetFullPath(), wxFile::write);

    // Now that both files do exist we should be able to detect that they are
    // actually the same file.
    CHECK( fn3.SameAs(fn4) );
#endif // __UNIX__
}

#if defined(__UNIX__)

// Tests for functions that are changed by ShouldFollowLink()
TEST_CASE("wxFileName::Symlinks", "[filename]")
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
    CHECK(tempdirfn.DirExists());

    // Create a regular file in that dir, to act as a symlink target
    wxFileName targetfn(wxFileName::CreateTempFileName(tempdir));
    CHECK(targetfn.FileExists());

    // Resolving a non-symlink will just return the same thing
    INFO("Non-symlink didn't resolve to the same file");
    CHECK( targetfn.ResolveLink() == targetfn );

    // Create a symlink to that file
    wxFileName linktofile(tempdir, "linktofile");
    CHECK( symlink(targetfn.GetFullPath().c_str(), linktofile.GetFullPath().c_str()) == 0 );

    // Test resolving the filename to the symlink
    INFO("Failed to resolve symlink to file");
    CHECK( linktofile.ResolveLink() == targetfn );

    // Create a relative symlink to that file
    wxFileName relativelinktofile(tempdir, "relativelinktofile");
    wxString relativeFileName = "./" + targetfn.GetFullName();
    CHECK( symlink(relativeFileName.c_str(), relativelinktofile.GetFullPath().c_str()) == 0 );
    INFO("Failed to resolve relative symlink to absolute file path");
    CHECK( relativelinktofile.ResolveLink() == targetfn );

    // ... and another to the temporary directory
    const wxString linktodirName(tempdir + "/linktodir");
    wxFileName linktodirfn(linktodirName);
    wxFileName linktodir(wxFileName::DirName(linktodirName));
    CHECK( symlink(tmpfn.GetFullPath().c_str(), linktodirName.c_str()) == 0 );
    INFO("Failed to resolve symlink to directory");
    CHECK( linktodirfn.ResolveLink() == tmpfn );

    // And symlinks to both of those symlinks
    wxFileName linktofilelnk(tempdir, "linktofilelnk");
    CHECK( symlink(linktofile.GetFullPath().c_str(), linktofilelnk.GetFullPath().c_str()) == 0 );
    INFO("Failed to resolve symlink to file symlink");
    CHECK( linktofilelnk.ResolveLink() == targetfn );

    wxFileName linktodirlnk(tempdir, "linktodirlnk");
    CHECK( symlink(linktodir.GetFullPath().c_str(), linktodirlnk.GetFullPath().c_str()) == 0 );
    INFO("Failed to resolve symlink to directory symlink");
    CHECK( linktodirlnk.ResolveLink() == tmpfn );

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
        INFO("Comparison with file" + msg);
        CHECK( linktofile.SameAs(targetfn) == deref );

        INFO("Comparison with directory" + msg);
        CHECK( linktodir.SameAs(tmpfn) == deref );

        // A link-to-a-link should dereference through to the final target
        INFO("Comparison with link to a file" + msg);
        CHECK( linktofilelnk.SameAs(targetfn) == deref );
        INFO("Comparison with link to a directory" + msg);
        CHECK( linktodirlnk.SameAs(tmpfn) == deref );

        // Test GetTimes()
        INFO("Getting times of a directory" + msg);
        wxDateTime dtAccess, dtMod, dtCreate;
        CHECK( linktodir.GetTimes(&dtAccess, &dtMod, &dtCreate) );

        // Test (File|Dir)Exists()
        INFO("Testing file existence" + msg);
        CHECK( linktofile.FileExists() == deref );
        INFO("Testing directory existence" + msg);
        CHECK( linktodir.DirExists() == deref );

        // Test wxFileName::Exists
        // The wxFILE_EXISTS_NO_FOLLOW flag should override DontFollowLink()
        INFO("Testing file existence" + msg);
        CHECK_FALSE( linktofile.Exists(wxFILE_EXISTS_REGULAR | wxFILE_EXISTS_NO_FOLLOW) );

        INFO("Testing directory existence" + msg);
        CHECK_FALSE( linktodir.Exists(wxFILE_EXISTS_DIR | wxFILE_EXISTS_NO_FOLLOW) );

        // and the static versions
        INFO("Testing file existence" + msg);
        CHECK_FALSE( wxFileName::Exists(linktofile.GetFullPath(), wxFILE_EXISTS_REGULAR | wxFILE_EXISTS_NO_FOLLOW) );
        CHECK( wxFileName::Exists(linktofile.GetFullPath(), wxFILE_EXISTS_REGULAR) );

        INFO("Testing directory existence" + msg);
        CHECK_FALSE( wxFileName::Exists(linktodir.GetFullPath(), wxFILE_EXISTS_DIR | wxFILE_EXISTS_NO_FOLLOW) );
        CHECK( wxFileName::Exists(linktodir.GetFullPath(), wxFILE_EXISTS_DIR) );
    }

    // Finally test Exists() after removing the file.
    CHECK(wxRemoveFile(targetfn.GetFullPath()));

    // Resolving a file that doesn't exist returns empty
    INFO("Non-existent file didn't resolve correctly");
    CHECK( targetfn.ResolveLink() == wxFileName() );

    // This should succeed, as the symlink still exists and
    // the default wxFILE_EXISTS_ANY implies wxFILE_EXISTS_NO_FOLLOW
    CHECK(wxFileName(tempdir, "linktofile").Exists());
    // So should this one, as wxFILE_EXISTS_SYMLINK does too
    CHECK(wxFileName(tempdir, "linktofile").Exists(wxFILE_EXISTS_SYMLINK));
    // but not this one, as the now broken symlink is followed
    CHECK(!wxFileName(tempdir, "linktofile").Exists(wxFILE_EXISTS_REGULAR));
    CHECK(linktofile.Exists());

    // This is also a convenient place to test Rmdir() as we have things to
    // remove.

    // First, check that removing a symlink to a directory fails.
    CHECK( !wxFileName::Rmdir(linktodirName) );

    // And recursively removing it only removes the symlink itself, not the
    // directory.
    CHECK( wxFileName::Rmdir(linktodirName, wxPATH_RMDIR_RECURSIVE) );
    CHECK( tmpfn.Exists() );

    // Finally removing the directory itself does remove everything.
    CHECK(tempdirfn.Rmdir(wxPATH_RMDIR_RECURSIVE));
    CHECK( !tempdirfn.Exists() );
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
   REQUIRE( SUCCEEDED(hr) );

   wxCOMPtr<IPersistFile> pf;
   hr = sl->QueryInterface(IID_IPersistFile, (void **)&pf);
   REQUIRE( SUCCEEDED(hr) );

   hr = sl->SetPath(pathFile.t_str());
   CHECK( SUCCEEDED(hr) );

   hr = pf->Save(pathLink.wc_str(), TRUE);
   CHECK( SUCCEEDED(hr) );
}

} // anonymous namespace

TEST_CASE("wxFileName::Shortcuts", "[filename]")
{
   wxFileName fn(wxFileName::CreateTempFileName("filenametest"));
   REQUIRE( fn.IsOk() );
   wxON_BLOCK_EXIT1( wxRemoveFile, fn.GetFullPath() );

   wxFileName fnLink(fn.GetPath(), "sc_" + fn.GetName(), "lnk");
   REQUIRE( fnLink.IsOk() );
   wxON_BLOCK_EXIT1( wxRemoveFile, fnLink.GetFullPath() );

   CreateShortcut(fn.GetFullPath(), fnLink.GetFullPath());

   // MakeRelativeTo() is supposed to change only the path of the file, not its
   // name.
   wxFileName fnLinkRel(fnLink);
   fnLink.MakeRelativeTo(".");
   CHECK( fnLinkRel.GetFullName() == fnLink.GetFullName() );
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
