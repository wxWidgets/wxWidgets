///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/stdstream.cpp
// Purpose:     Test wxStdInputStreamBuffer/wxStdOutputStreamBuffer
// Author:      Jonathan Liu <net147@gmail.com>
// Copyright:   (c) 2009 Jonathan Liu
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STD_IOSTREAM

#include "wx/stdstream.h"

#include <string.h>
#include "wx/mstream.h"

// ==========================================================================
// Definitions
// ==========================================================================

const int TEST_SIZE = 384;

// ==========================================================================
// Test fixture
// ==========================================================================

namespace
{

class StdStreamTestCase
{
public:
    StdStreamTestCase()
    {
        for (int i = 0; i < TEST_SIZE; ++i)
            m_testData[i] = (i & 0xFF);
    }

protected:
    char m_testData[TEST_SIZE];

    wxDECLARE_NO_COPY_CLASS(StdStreamTestCase);
};

} // anonymous namespace

// MSVS 2015 can't compile CHECK()s in this file without this disambiguation.
#ifdef __VISUALC__
    #if __VISUALC__ < 1910
        inline bool operator==(const std::fpos<_Mbstatet> lhs, const int rhs)
        {
            return static_cast<int>(lhs) == rhs;
        }
    #endif // MSVS 2015
#endif // __VISUALC__

// ==========================================================================
// Tests
// ==========================================================================

// --------------------------------------------------------------------------
// Input buffer management and positioning
// --------------------------------------------------------------------------

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_pubsetbuf", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);
    char testBuffer[TEST_SIZE];

    CHECK(buffer.pubsetbuf(testBuffer, TEST_SIZE) == nullptr);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_pubseekoff", "[stdstream][stream]")
{
    const char *testData = "0123456789";
    wxMemoryInputStream stream(testData, 10);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.pubseekoff(2, std::ios_base::beg, std::ios_base::in) == 2);
    CHECK(buffer.pubseekoff(2, std::ios_base::beg, std::ios_base::out) == -1);

    CHECK(buffer.pubseekoff(2, std::ios_base::cur) == 4);
    CHECK(buffer.pubseekoff(2, std::ios_base::cur, std::ios_base::out) == -1);

    CHECK(buffer.pubseekoff(-2, std::ios_base::end) == 8);
    CHECK(buffer.pubseekoff(-2, std::ios_base::end, std::ios_base::out) == -1);

    CHECK(buffer.pubseekoff(3, std::ios_base::cur) == -1);
    CHECK(buffer.pubseekoff(3, std::ios_base::cur, std::ios_base::out) == -1);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_pubseekpos", "[stdstream][stream]")
{
    const char *testData = "0123456789";
    wxMemoryInputStream stream(testData, 10);
    wxStdInputStreamBuffer buffer(stream);

    for (int i = 9; i >= 0; --i)
    {
        if (i % 2 == 0)
            CHECK(buffer.pubseekpos(i) == i);
        else
            CHECK(buffer.pubseekpos(i, std::ios_base::in) == i);

        CHECK(buffer.sgetc() == '0' + i);
    }
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_pubsync", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.pubsync() == 0);
}

// --------------------------------------------------------------------------
// Input functions
// --------------------------------------------------------------------------

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_in_avail", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE / 2];

    buffer.sgetn(data, TEST_SIZE / 2);
    CHECK(buffer.in_avail() == TEST_SIZE - TEST_SIZE / 2);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_snextc", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE];

    data[0] = buffer.sgetc();

    for (int i = 1; i < TEST_SIZE; ++i)
        data[i] = buffer.snextc();

    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sbumpc() == (int)(unsigned char) (m_testData[TEST_SIZE - 1]));
    CHECK(buffer.sgetc() == EOF);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_sbumpc", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; ++i)
        data[i] = buffer.sbumpc();

    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sgetc() == EOF);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_sgetc", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; ++i) {
        data[i] = buffer.sgetc();
        buffer.sbumpc();
    }

    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sgetc() == EOF);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_sgetn", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE * 2];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE * 2);

    CHECK(read == TEST_SIZE);
    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sgetc() == EOF);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_sputbackc", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE);

    CHECK(read == TEST_SIZE);
    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sgetc() == EOF);

    char putBackChar = m_testData[TEST_SIZE - 1] + 147;

    CHECK(buffer.sputbackc(putBackChar) == (int) putBackChar);
    CHECK(buffer.sgetc() == (int) putBackChar);
    CHECK(buffer.sbumpc() == (int) putBackChar);
    CHECK(buffer.sgetc() == EOF);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::InputBuffer_sungetc", "[stdstream][stream]")
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CHECK(buffer.sgetc() != EOF);
    CHECK(buffer.in_avail() == TEST_SIZE);

    char data[TEST_SIZE];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE);

    CHECK(read == TEST_SIZE);
    CHECK(memcmp(data, m_testData, TEST_SIZE) == 0);
    CHECK(buffer.sgetc() == EOF);

    CHECK(buffer.sungetc() == (int) m_testData[TEST_SIZE - 1]);
    CHECK(buffer.sgetc() == (int) m_testData[TEST_SIZE - 1]);
    CHECK(buffer.sbumpc() == (int) m_testData[TEST_SIZE - 1]);
    CHECK(buffer.sgetc() == EOF);
}

// --------------------------------------------------------------------------
// Output buffer management and positioning
// --------------------------------------------------------------------------

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_pubsetbuf", "[stdstream][stream]")
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);
    char testBuffer[TEST_SIZE];

    CHECK(buffer.pubsetbuf(testBuffer, TEST_SIZE) == nullptr);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_pubseekoff", "[stdstream][stream]")
{
    char testData[] = "0123456789";
    wxMemoryOutputStream stream(testData, 10);
    wxStdOutputStreamBuffer buffer(stream);

    CHECK(buffer.pubseekoff(2, std::ios_base::beg, std::ios_base::out) == 2);
    CHECK(buffer.pubseekoff(2, std::ios_base::beg, std::ios_base::in) == -1);

    CHECK(buffer.pubseekoff(2, std::ios_base::cur) == 4);
    CHECK(buffer.pubseekoff(2, std::ios_base::cur, std::ios_base::in) == -1);

    CHECK(buffer.pubseekoff(-2, std::ios_base::end) == 8);
    CHECK(buffer.pubseekoff(-2, std::ios_base::end, std::ios_base::in) == -1);

    CHECK(buffer.pubseekoff(3, std::ios_base::cur) == -1);
    CHECK(buffer.pubseekoff(3, std::ios_base::cur, std::ios_base::in) == -1);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_pubseekpos", "[stdstream][stream]")
{
    char testData[] = "0123456789";
    wxMemoryOutputStream stream(testData, 10);
    wxStdOutputStreamBuffer buffer(stream);

    for (int i = 9; i >= 0; --i)
    {
        if (i % 2 == 0)
        {
            CHECK(buffer.pubseekpos(i) == i);
        }
        else
        {
            CHECK(buffer.pubseekpos(i, std::ios_base::out) == i);
        }

        CHECK(buffer.sputc('0' + (9 - i)) == '0' + (9 - i));
    }

    CHECK(memcmp(testData, "9876543210", 10) == 0);

    CHECK(buffer.pubseekpos(5, std::ios_base::in) == -1);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_pubsync", "[stdstream][stream]")
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    CHECK(buffer.pubsync() == 0);
}

// --------------------------------------------------------------------------
// Output functions
// --------------------------------------------------------------------------

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_sputc", "[stdstream][stream]")
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    for (int i = 0; i < TEST_SIZE; ++i)
        buffer.sputc(m_testData[i]);

    CHECK(stream.GetSize() == TEST_SIZE);

    char result[TEST_SIZE];

    stream.CopyTo(result, TEST_SIZE);
    CHECK(memcmp(result, m_testData, TEST_SIZE) == 0);
}

TEST_CASE_METHOD(StdStreamTestCase, "StdStream::OutputBuffer_sputn", "[stdstream][stream]")
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    buffer.sputn(m_testData, TEST_SIZE);
    CHECK(stream.GetSize() == TEST_SIZE);

    char result[TEST_SIZE];

    stream.CopyTo(result, TEST_SIZE);
    CHECK(memcmp(result, m_testData, TEST_SIZE) == 0);
}

#endif // wxUSE_STD_IOSTREAM
