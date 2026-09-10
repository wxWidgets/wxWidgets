///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/filetest.cpp
// Purpose:     wxFile unit test
// Author:      Vadim Zeitlin
// Created:     2009-09-12
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_FILE

#include "wx/file.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/scopeguard.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// tests implementation
// ----------------------------------------------------------------------------

TEST_CASE("wxFile::ReadAll", "[file]")
{
    TestFile tf;

    const char* text = "Ream\nde";

    {
        wxFile fout(tf.GetName(), wxFile::write);
        CHECK( fout.IsOpened() );
        fout.Write(text, strlen(text));
        CHECK( fout.Close() );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CHECK( fin.IsOpened() );

        wxString s;
        CHECK( fin.ReadAll(&s) );
        CHECK( s == text );
    }
}

TEST_CASE("wxFile::RoundTrip", "[file]")
{
    std::unique_ptr<wxMBConv> conv;

    SECTION("UTF-8") { conv.reset(new wxMBConvStrictUTF8); }
    SECTION("UTF-16") { conv.reset(new wxMBConvUTF16); }
    SECTION("UTF-32") { conv.reset(new wxMBConvUTF32); }

    TestFile tf;

    // Explicit length is needed because of the embedded NUL.
    const wxString data("Hello\0UTF!", 10);

    {
        wxFile fout(tf.GetName(), wxFile::write);
        CHECK( fout.IsOpened() );

        CHECK( fout.Write(data, *conv) );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CHECK( fin.IsOpened() );

        const ssize_t len = fin.Length();
        wxCharBuffer buf(len);
        CHECK( fin.Read(buf.data(), len) == len );

        wxString dataReadBack(buf, *conv, len);
        CHECK( dataReadBack == data );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CHECK( fin.IsOpened() );

        wxString dataReadBack;
        CHECK( fin.ReadAll(&dataReadBack, *conv) );

        CHECK( dataReadBack == data );
    }
}

static void CheckFileContents(const wxString& name, const wxString& data)
{
    // Check that the file exists with the expected contents.
    wxFile f(name);
    REQUIRE( f.IsOpened() );

    wxString s;
    CHECK( f.ReadAll(&s) );
    CHECK( s == data );
}

static bool RemoveTempPath(const wxString& path)
{
    if ( wxDirExists(path) )
        return wxFileName::Rmdir(path, wxPATH_RMDIR_RECURSIVE);

    if ( wxFileExists(path) )
        return wxRemoveFile(path);

    return true;
}

TEST_CASE("wxTempFile", "[file][temp]")
{
    constexpr const char* name = "wxtemp_test";
    const wxString dataOld("what is the meaning of life?");
    const wxString dataNew("the answer is 42");

    // Ensure that it will be removed at the end of the test in any case.
    TempFile tf(name);

    bool hasOldFile = false;

    SECTION("New")
    {
        wxRemoveFile(name);
    }

    SECTION("Existing")
    {
        wxFile f(name, wxFile::write);
        CHECK( f.IsOpened() );
        CHECK( f.Write(dataOld) );
        CHECK( f.Close() );

        hasOldFile = true;
    }

    // First check that not committing the file doesn't do anything.
    {
        wxTempFile discarded(name);
        CHECK( discarded.IsOpened() );
        CHECK( discarded.Write(dataNew) );
    }

    if ( !hasOldFile )
    {
        // The file shouldn't have been created.
        CHECK( !wxFile::Exists(name) );
    }
    else
    {
        // Check that the old file is still there with the old contents.
        CheckFileContents(name, dataOld);
    }

    // Next check that committing it does.
    wxTempFile tmpFile;
    CHECK( tmpFile.Open(name) );
    CHECK( tmpFile.Write(dataNew) );
    CHECK( tmpFile.Commit() );

    CheckFileContents(name, dataNew);
}

TEST_CASE("TempDir", "[file][temp]")
{
    SECTION("Create")
    {
        TempDir dir("wxtest-tempdir");
        dir.RequireOk();
        CHECK( wxDirExists(dir.GetName()) );
        CHECK( dir.GetError().empty() );
    }

    SECTION("RetryExistingFile")
    {
        const wxString prefix = "wxtest-tempdir-file";
        const unsigned long nameId = TempDir::ReserveNameId();
        const wxString firstPath = TempDir::GetCandidateName(prefix, nameId, 0);
        const wxString secondPath =
            TempDir::GetCandidateName(prefix, nameId, 1);

        REQUIRE( RemoveTempPath(firstPath) );
        REQUIRE( RemoveTempPath(secondPath) );
        wxON_BLOCK_EXIT1(RemoveTempPath, firstPath);
        wxON_BLOCK_EXIT1(RemoveTempPath, secondPath);

        {
            wxFile file(firstPath, wxFile::write);
            REQUIRE( file.IsOpened() );
            REQUIRE( file.Close() );
        }

        TempDir dir(prefix, nameId);
        dir.RequireOk();
        CHECK( dir.GetName() == secondPath );
        CHECK( wxFileExists(firstPath) );
        CHECK( wxDirExists(secondPath) );
        CHECK( dir.GetError().empty() );
    }

    SECTION("RetryExistingDir")
    {
        const wxString prefix = "wxtest-tempdir-dir";
        const unsigned long nameId = TempDir::ReserveNameId();
        const wxString firstPath = TempDir::GetCandidateName(prefix, nameId, 0);
        const wxString secondPath =
            TempDir::GetCandidateName(prefix, nameId, 1);

        REQUIRE( RemoveTempPath(firstPath) );
        REQUIRE( RemoveTempPath(secondPath) );
        wxON_BLOCK_EXIT1(RemoveTempPath, firstPath);
        wxON_BLOCK_EXIT1(RemoveTempPath, secondPath);

        REQUIRE( wxMkdir(firstPath) );

        TempDir dir(prefix, nameId);
        dir.RequireOk();
        CHECK( dir.GetName() == secondPath );
        CHECK( wxDirExists(firstPath) );
        CHECK( wxDirExists(secondPath) );
        CHECK( dir.GetError().empty() );
    }

    SECTION("BadParent")
    {
        TempDir parent("wxtest-tempdir-parent");
        parent.RequireOk();

        const wxString parentFile =
            wxFileName(parent.GetName(), "file").GetFullPath();

        {
            wxFile file(parentFile, wxFile::write);
            REQUIRE( file.IsOpened() );
            REQUIRE( file.Close() );
        }

        TempDir dir(wxFileName(parentFile, "child").GetFullPath());
        CHECK_FALSE( dir.IsOk() );
        CHECK( dir.GetName().empty() );
        CHECK( dir.GetError().Contains("wxMkdir failed for") );
        CHECK( dir.GetError().Contains(parentFile) );
    }
}

#ifdef __LINUX__

// Check that GetSize() works correctly for special files.
TEST_CASE("wxFile::Special", "[file][linux][special-file]")
{
    // We can't test /proc/kcore here, unlike in the similar
    // wxFileName::GetSize() test, as wxFile must be able to open it (at least
    // for reading) and usually we don't have the permissions to do it.

    // This file is not seekable and has 0 size, but can still be read.
    wxFile fileProc("/proc/cpuinfo");
    CHECK( fileProc.IsOpened() );

    wxString s;
    CHECK( fileProc.ReadAll(&s) );
    CHECK( !s.empty() );

    // All files in /sys have the size of one kernel page, even if they don't
    // have that much data in them.
    const long pageSize = sysconf(_SC_PAGESIZE);

    wxFile fileSys("/sys/power/state");
    if ( !fileSys.IsOpened() )
    {
        WARN("/sys/power/state can't be opened, skipping test");
        return;
    }

    CHECK( fileSys.Length() == pageSize );
    CHECK( fileSys.ReadAll(&s) );
    CHECK( !s.empty() );
    CHECK( s.length() < static_cast<unsigned long>(pageSize) );
}

#endif // __LINUX__

#endif // wxUSE_FILE
