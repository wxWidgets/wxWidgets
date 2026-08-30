///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/backfile.cpp
// Purpose:     Test wxBackingFile
// Author:      Mike Wetherell
// Copyright:   (c) 2006 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/mstream.h"
#include "wx/private/fileback.h"
#include "bstream.h"

const size_t TESTSIZE = 256;
const size_t BUFSIZE = 100;

///////////////////////////////////////////////////////////////////////////////
// Parent stream for testing

class TestStream : public wxMemoryInputStream
{
public:
    TestStream(const void *buf, size_t size)
        : wxMemoryInputStream(buf, size) { }

    wxFileOffset GetLength() const override { return wxInvalidOffset; }
};


///////////////////////////////////////////////////////////////////////////////
// The test case

class backStream
{
public:
    backStream()
    {
        for (unsigned i = 0; i < TESTSIZE; i++)
            m_testdata[i] = i;
    }

protected:
    void ReadLenSeek();
    void LenSeekRead();
    void SeekReadLen();
    void ReadAll();
    void ReadTooMuch();
    void EmptyStream();

private:
    void Read(wxInputStream& in, size_t size1, size_t size2, size_t size3);
    void Len(wxBackedInputStream& in);
    void Seek(wxInputStream& in);

    char m_testdata[TESTSIZE];
};

void backStream::ReadLenSeek()
{
    wxBackingFile bf(new TestStream(m_testdata, TESTSIZE), BUFSIZE);
    wxBackedInputStream in(bf);

    Read(in, BUFSIZE, BUFSIZE / 2, 2 * BUFSIZE / 3);
    Len(in);
    Seek(in);
}

void backStream::LenSeekRead()
{
    wxBackingFile bf(new TestStream(m_testdata, TESTSIZE), BUFSIZE);
    wxBackedInputStream in(bf);

    Len(in);
    Seek(in);
    Read(in, BUFSIZE, BUFSIZE / 2, 2 * BUFSIZE / 3);
}

void backStream::SeekReadLen()
{
    wxBackingFile bf(new TestStream(m_testdata, TESTSIZE), BUFSIZE);
    wxBackedInputStream in(bf);

    Seek(in);
    Read(in, BUFSIZE, BUFSIZE / 2, 2 * BUFSIZE / 3);
    Len(in);
}

void backStream::ReadAll()
{
    wxBackingFile bf(new TestStream(m_testdata, TESTSIZE), BUFSIZE);
    wxBackedInputStream in(bf);

    Read(in, TESTSIZE, 0, 0);
}

void backStream::ReadTooMuch()
{
    wxBackingFile bf(new TestStream(m_testdata, TESTSIZE), BUFSIZE);
    wxBackedInputStream in(bf);

    char buf[TESTSIZE * 2];

    CHECK(in.Read(buf, TESTSIZE * 2).LastRead() == TESTSIZE);
    CHECK(in.Eof());
    CHECK(memcmp(buf, m_testdata, TESTSIZE) == 0);
}

void backStream::EmptyStream()
{
    wxBackingFile bf(new TestStream(m_testdata, 0), BUFSIZE);
    wxBackedInputStream in(bf);

    char buf[1];

    CHECK(in.Read(buf, 1).LastRead() == size_t(0));
    CHECK(in.Eof());
}

void backStream::Read(wxInputStream& in,
                      size_t size1,
                      size_t size2,
                      size_t size3)
{
    const size_t remainder = TESTSIZE - size1 - size2 - size3;
    char buf[TESTSIZE];
    char *testdata = m_testdata;

    in.SeekI(0);

    CHECK(in.Read(buf, size1).LastRead() == size1);
    CHECK(in.IsOk());
    CHECK(memcmp(buf, testdata, size1) == 0);
    testdata += size1;

    CHECK(in.Read(buf, size2).LastRead() == size2);
    CHECK(in.IsOk());
    CHECK(memcmp(buf, testdata, size2) == 0);
    testdata += size2;

    CHECK(in.Read(buf, size3).LastRead() == size3);
    CHECK(in.IsOk());
    CHECK(memcmp(buf, testdata, size3) == 0);
    testdata += size3;

    CHECK(in.Read(buf, TESTSIZE).LastRead() == remainder);
    CHECK(in.Eof());
    CHECK(memcmp(buf, testdata, remainder) == 0);

    CHECK(in.Read(buf, TESTSIZE).LastRead() == size_t(0));
    CHECK(in.Eof());
}

void backStream::Len(wxBackedInputStream& in)
{
    CHECK(in.FindLength() == wxFileOffset(TESTSIZE));
}

void backStream::Seek(wxInputStream& in)
{
    CHECK(in.SeekI(TESTSIZE) == wxFileOffset(TESTSIZE));
    in.GetC();
    CHECK(in.LastRead() == size_t(0));
    CHECK(in.Eof());

    for (wxFileOffset i = TESTSIZE - 1; i >= 0; i--) {
        CHECK(in.SeekI(i) == i);
        CHECK(in.TellI() == i);
        CHECK(in.GetC() == int(i));
        CHECK(in.LastRead() == size_t(1));
        CHECK(in.IsOk());
    }
}

WX_STREAM_TEST_CASE(backStream, ReadLenSeek)
WX_STREAM_TEST_CASE(backStream, LenSeekRead)
WX_STREAM_TEST_CASE(backStream, SeekReadLen)
WX_STREAM_TEST_CASE(backStream, ReadAll)
WX_STREAM_TEST_CASE(backStream, ReadTooMuch)
WX_STREAM_TEST_CASE(backStream, EmptyStream)
