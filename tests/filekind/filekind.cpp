///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filetype/filetype.cpp
// Purpose:     Test wxGetFileKind and wxStreamBase::IsSeekable
// Author:      Mike Wetherell
// Copyright:   (c) 2005 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STREAMS

#ifdef __UNIX__
    #include <sys/socket.h>
#endif

#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/wfstream.h"
#include "wx/filename.h"
#include "wx/socket.h"
#include "wx/sckstrm.h"
#include "wx/mstream.h"

#ifdef __VISUALC__
    #define isatty _isatty
    #define fdopen _fdopen
    #define fileno _fileno
#endif

#include "testfile.h"

///////////////////////////////////////////////////////////////////////////////
// Helpers

// test a wxFFile and wxFFileInput/OutputStreams of a known type
//
static void TestFILE(wxFFile& file, bool expected)
{
    CHECK(file.IsOpened());
    CHECK((wxGetFileKind(file.fp()) == wxFILE_KIND_DISK) == expected);
    CHECK((file.GetKind() == wxFILE_KIND_DISK) == expected);

    wxFFileInputStream inStream(file);
    CHECK(inStream.IsSeekable() == expected);

    wxFFileOutputStream outStream(file);
    CHECK(outStream.IsSeekable() == expected);
}

// test a wxFile and wxFileInput/OutputStreams of a known type
//
static void TestFd(wxFile& file, bool expected)
{
    CHECK(file.IsOpened());
    CHECK((wxGetFileKind(file.fd()) == wxFILE_KIND_DISK) == expected);
    CHECK((file.GetKind() == wxFILE_KIND_DISK) == expected);

    wxFileInputStream inStream(file);
    CHECK(inStream.IsSeekable() == expected);

    wxFileOutputStream outStream(file);
    CHECK(outStream.IsSeekable() == expected);
}

///////////////////////////////////////////////////////////////////////////////
// The tests

// test with an ordinary file
//
TEST_CASE("FileKind::File", "[filekind]")
{
    TempFile tmp; // put first
    wxFile file;
    tmp.Assign(wxFileName::CreateTempFileName(wxT("wxft"), &file));
    TestFd(file, true);
    file.Close();

    wxFFile ffile(tmp.GetName());
    TestFILE(ffile, true);
}

// test with a pipe
//
#if defined __UNIX__ || defined _MSC_VER || defined __MINGW32__
TEST_CASE("FileKind::Pipe", "[filekind]")
{
    int afd[2];
    int rc;
#ifdef __UNIX__
    rc = pipe(afd);
#else
    rc = _pipe(afd, 256, O_BINARY);
#endif
    INFO("Failed to create pipe");
    REQUIRE( rc == 0 );

    wxFile file0(afd[0]);
    wxFile file1(afd[1]);
    TestFd(file0, false);
    file0.Detach();

    wxFFile ffile(fdopen(afd[0], "r"));
    TestFILE(ffile, false);
}
#endif

// test with a socket
//
#if defined __UNIX__
TEST_CASE("FileKind::Socket", "[filekind]")
{
    int s = socket(PF_INET, SOCK_STREAM, 0);

    wxFile file(s);
    TestFd(file, false);
    file.Detach();

    wxFFile ffile(fdopen(s, "r"));
    TestFILE(ffile, false);
}
#endif

// Socket streams should be non-seekable
//
#if wxUSE_SOCKETS
TEST_CASE("FileKind::SocketStream", "[filekind]")
{
    wxSocketClient client;
    wxSocketInputStream inStream(client);
    CHECK(!inStream.IsSeekable());
    wxSocketOutputStream outStream(client);
    CHECK(!outStream.IsSeekable());

    wxBufferedInputStream nonSeekableBufferedInput(inStream);
    CHECK(!nonSeekableBufferedInput.IsSeekable());
    wxBufferedOutputStream nonSeekableBufferedOutput(outStream);
    CHECK(!nonSeekableBufferedOutput.IsSeekable());
}
#endif

// Memory streams should be seekable
//
TEST_CASE("FileKind::MemoryStream", "[filekind]")
{
    char buf[20] = { 0 };
    wxMemoryInputStream inStream(buf, sizeof(buf));
    CHECK(inStream.IsSeekable());
    wxMemoryOutputStream outStream(buf, sizeof(buf));
    CHECK(outStream.IsSeekable());

    wxBufferedInputStream seekableBufferedInput(inStream);
    CHECK(seekableBufferedInput.IsSeekable());
    wxBufferedOutputStream seekableBufferedOutput(outStream);
    CHECK(seekableBufferedOutput.IsSeekable());
}

// Stdin will usually be a terminal, if so then test it
//
TEST_CASE("FileKind::Stdin", "[filekind]")
{
    if (isatty(0))
        CHECK(wxGetFileKind(0) == wxFILE_KIND_TERMINAL);
    if (isatty(fileno(stdin)))
        CHECK(wxGetFileKind(stdin) == wxFILE_KIND_TERMINAL);
}

#endif // wxUSE_STREAMS
