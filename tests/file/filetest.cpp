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

TEST_CASE("wxTempFile", "[file][temp]")
{
    wxTempFile tmpFile;
    CHECK( tmpFile.Open(wxT("test2")) );
    CHECK( tmpFile.Write(wxT("the answer is 42")) );
    CHECK( tmpFile.Commit() );
    CHECK( wxRemoveFile(wxT("test2")) );
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
