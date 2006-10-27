///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/backfile.cpp
// Purpose:     Test wxBackingFile
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/mstream.h"
#include "wx/fileback.h"
#include "bstream.h"

#if wxUSE_STREAMS

const size_t TESTSIZE = 256;
const size_t BUFSIZE = 100;


///////////////////////////////////////////////////////////////////////////////
// Parent stream for testing

class TestStream : public wxMemoryInputStream
{
public:
    TestStream(const void *buf, size_t size)
        : wxMemoryInputStream(buf, size) { }

    wxFileOffset GetLength() const { return wxInvalidOffset; }
};


///////////////////////////////////////////////////////////////////////////////
// The test case

class backStream : public CppUnit::TestCase
{
public:
    backStream();

    CPPUNIT_TEST_SUITE(backStream);
        CPPUNIT_TEST(ReadLenSeek);
        CPPUNIT_TEST(LenSeekRead);
        CPPUNIT_TEST(SeekReadLen);
        CPPUNIT_TEST(ReadAll);
        CPPUNIT_TEST(ReadTooMuch);
        CPPUNIT_TEST(EmptyStream);
    CPPUNIT_TEST_SUITE_END();

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

backStream::backStream()
{
    for (unsigned i = 0; i < TESTSIZE; i++)
        m_testdata[i] = i;
}

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

    CPPUNIT_ASSERT_EQUAL(TESTSIZE, in.Read(buf, TESTSIZE * 2).LastRead());
    CPPUNIT_ASSERT(in.Eof());
    CPPUNIT_ASSERT(memcmp(buf, m_testdata, TESTSIZE) == 0);
}

void backStream::EmptyStream()
{
    wxBackingFile bf(new TestStream(m_testdata, 0), BUFSIZE);
    wxBackedInputStream in(bf);

    char buf[1];

    CPPUNIT_ASSERT_EQUAL(size_t(0), in.Read(buf, 1).LastRead());
    CPPUNIT_ASSERT(in.Eof());
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

    CPPUNIT_ASSERT_EQUAL(size1, in.Read(buf, size1).LastRead());
    CPPUNIT_ASSERT(in.IsOk());
    CPPUNIT_ASSERT(memcmp(buf, testdata, size1) == 0);
    testdata += size1;

    CPPUNIT_ASSERT_EQUAL(size2, in.Read(buf, size2).LastRead());
    CPPUNIT_ASSERT(in.IsOk());
    CPPUNIT_ASSERT(memcmp(buf, testdata, size2) == 0);
    testdata += size2;

    CPPUNIT_ASSERT_EQUAL(size3, in.Read(buf, size3).LastRead());
    CPPUNIT_ASSERT(in.IsOk());
    CPPUNIT_ASSERT(memcmp(buf, testdata, size3) == 0);
    testdata += size3;

    CPPUNIT_ASSERT_EQUAL(remainder, in.Read(buf, TESTSIZE).LastRead());
    CPPUNIT_ASSERT(in.Eof());
    CPPUNIT_ASSERT(memcmp(buf, testdata, remainder) == 0);

    CPPUNIT_ASSERT_EQUAL(size_t(0), in.Read(buf, TESTSIZE).LastRead());
    CPPUNIT_ASSERT(in.Eof());
}

void backStream::Len(wxBackedInputStream& in)
{
    CPPUNIT_ASSERT_EQUAL(wxFileOffset(TESTSIZE), in.FindLength());
}

void backStream::Seek(wxInputStream& in)
{
    CPPUNIT_ASSERT_EQUAL(wxFileOffset(TESTSIZE), in.SeekI(TESTSIZE));
    in.GetC();
    CPPUNIT_ASSERT_EQUAL(size_t(0), in.LastRead());
    CPPUNIT_ASSERT(in.Eof());

    for (wxFileOffset i = TESTSIZE - 1; i >= 0; i--) {
        CPPUNIT_ASSERT_EQUAL(i, in.SeekI(i));
        CPPUNIT_ASSERT_EQUAL(i, in.TellI());
        CPPUNIT_ASSERT_EQUAL(char(i), in.GetC());
        CPPUNIT_ASSERT_EQUAL(size_t(1), in.LastRead());
        CPPUNIT_ASSERT(in.IsOk());
    }
}

// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(backStream)

#endif // wxUSE_STREAMS
