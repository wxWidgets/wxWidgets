///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/stdstream.cpp
// Purpose:     Test wxStdInputStreamBuffer/wxStdOutputStreamBuffer
// Author:      Jonathan Liu <net147@gmail.com>
// Copyright:   (c) 2009 Jonathan Liu
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
// Test class
// ==========================================================================

class StdStreamTestCase : public CppUnit::TestCase
{
public:
    StdStreamTestCase();

private:
    CPPUNIT_TEST_SUITE( StdStreamTestCase );
        // Input buffer management and positioning
        CPPUNIT_TEST( InputBuffer_pubsetbuf );
        CPPUNIT_TEST( InputBuffer_pubseekoff );
        CPPUNIT_TEST( InputBuffer_pubseekpos );
        CPPUNIT_TEST( InputBuffer_pubsync );

        // Input functions
        CPPUNIT_TEST( InputBuffer_in_avail );
        CPPUNIT_TEST( InputBuffer_snextc );
        CPPUNIT_TEST( InputBuffer_sbumpc );
        CPPUNIT_TEST( InputBuffer_sgetc );
        CPPUNIT_TEST( InputBuffer_sgetn );
        CPPUNIT_TEST( InputBuffer_sputbackc );
        CPPUNIT_TEST( InputBuffer_sungetc );

        // Output buffer management and positioning
        CPPUNIT_TEST( OutputBuffer_pubsetbuf );
        CPPUNIT_TEST( OutputBuffer_pubseekoff );
        CPPUNIT_TEST( OutputBuffer_pubseekpos );
        CPPUNIT_TEST( OutputBuffer_pubsync );

        // Output functions
        CPPUNIT_TEST( OutputBuffer_sputc );
        CPPUNIT_TEST( OutputBuffer_sputn );
    CPPUNIT_TEST_SUITE_END();

    // Input buffer management and positioning
    void InputBuffer_pubsetbuf();
    void InputBuffer_pubseekoff();
    void InputBuffer_pubseekpos();
    void InputBuffer_pubsync();

    // Input functions
    void InputBuffer_in_avail();
    void InputBuffer_snextc();
    void InputBuffer_sbumpc();
    void InputBuffer_sgetc();
    void InputBuffer_sgetn();
    void InputBuffer_sputbackc();
    void InputBuffer_sungetc();

    // Output buffer management and positioning
    void OutputBuffer_pubsetbuf();
    void OutputBuffer_pubseekoff();
    void OutputBuffer_pubseekpos();
    void OutputBuffer_pubsync();

    // Output functions
    void OutputBuffer_sputc();
    void OutputBuffer_sputn();

    char m_testData[TEST_SIZE];

    wxDECLARE_NO_COPY_CLASS(StdStreamTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StdStreamTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StdStreamTestCase,
                                       "StdStreamTestCase" );

// ==========================================================================
// Implementation
// ==========================================================================

StdStreamTestCase::StdStreamTestCase()
{
    for (int i = 0; i < TEST_SIZE; ++i)
        m_testData[i] = (i & 0xFF);
}

// --------------------------------------------------------------------------
// Input buffer management and positioning
// --------------------------------------------------------------------------

void StdStreamTestCase::InputBuffer_pubsetbuf()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);
    char testBuffer[TEST_SIZE];

    CPPUNIT_ASSERT(buffer.pubsetbuf(testBuffer, TEST_SIZE) == NULL);
}

void StdStreamTestCase::InputBuffer_pubseekoff()
{
    const char *testData = "0123456789";
    wxMemoryInputStream stream(testData, 10);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT_EQUAL(2,
                         buffer.pubseekoff(2, std::ios_base::beg,
                                           std::ios_base::in));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(2, std::ios_base::beg,
                                           std::ios_base::out));

    CPPUNIT_ASSERT_EQUAL(4,
                         buffer.pubseekoff(2, std::ios_base::cur));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(2, std::ios_base::cur,
                                           std::ios_base::out));

    CPPUNIT_ASSERT_EQUAL(8,
                         buffer.pubseekoff(-2, std::ios_base::end));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(-2, std::ios_base::end,
                                           std::ios_base::out));

    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(3, std::ios_base::cur));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(3, std::ios_base::cur,
                                           std::ios_base::out));
}

void StdStreamTestCase::InputBuffer_pubseekpos()
{
    const char *testData = "0123456789";
    wxMemoryInputStream stream(testData, 10);
    wxStdInputStreamBuffer buffer(stream);

    for (int i = 9; i >= 0; --i)
    {
        if (i % 2 == 0)
            CPPUNIT_ASSERT_EQUAL(i, buffer.pubseekpos(i));
        else
            CPPUNIT_ASSERT_EQUAL(i, buffer.pubseekpos(i, std::ios_base::in));

        CPPUNIT_ASSERT_EQUAL('0' + i, buffer.sgetc());
    }
}

void StdStreamTestCase::InputBuffer_pubsync()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.pubsync() == 0);
}

// --------------------------------------------------------------------------
// Input functions
// --------------------------------------------------------------------------

void StdStreamTestCase::InputBuffer_in_avail()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE / 2];

    buffer.sgetn(data, TEST_SIZE / 2);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE - TEST_SIZE / 2, buffer.in_avail());
}

void StdStreamTestCase::InputBuffer_snextc()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE];

    data[0] = buffer.sgetc();

    for (int i = 1; i < TEST_SIZE; ++i)
        data[i] = buffer.snextc();

    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT_EQUAL((int)(unsigned char) (m_testData[TEST_SIZE - 1]),
                         buffer.sbumpc());
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

void StdStreamTestCase::InputBuffer_sbumpc()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; ++i)
        data[i] = buffer.sbumpc();

    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

void StdStreamTestCase::InputBuffer_sgetc()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; ++i) {
        data[i] = buffer.sgetc();
        buffer.sbumpc();
    }

    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

void StdStreamTestCase::InputBuffer_sgetn()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE * 2];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE * 2);

    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, read);
    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

void StdStreamTestCase::InputBuffer_sputbackc()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE);

    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, read);
    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);

    char putBackChar = m_testData[TEST_SIZE - 1] + 147;

    CPPUNIT_ASSERT_EQUAL((int) putBackChar, buffer.sputbackc(putBackChar));
    CPPUNIT_ASSERT_EQUAL((int) putBackChar, buffer.sgetc());
    CPPUNIT_ASSERT_EQUAL((int) putBackChar, buffer.sbumpc());
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

void StdStreamTestCase::InputBuffer_sungetc()
{
    wxMemoryInputStream stream(m_testData, TEST_SIZE);
    wxStdInputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.sgetc() != EOF);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, buffer.in_avail());

    char data[TEST_SIZE];
    std::streamsize read = buffer.sgetn(data, TEST_SIZE);

    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, read);
    CPPUNIT_ASSERT(memcmp(data, m_testData, TEST_SIZE) == 0);
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);

    CPPUNIT_ASSERT_EQUAL((int) m_testData[TEST_SIZE - 1], buffer.sungetc());
    CPPUNIT_ASSERT_EQUAL((int) m_testData[TEST_SIZE - 1], buffer.sgetc());
    CPPUNIT_ASSERT_EQUAL((int) m_testData[TEST_SIZE - 1], buffer.sbumpc());
    CPPUNIT_ASSERT(buffer.sgetc() == EOF);
}

// --------------------------------------------------------------------------
// Output buffer management and positioning
// --------------------------------------------------------------------------

void StdStreamTestCase::OutputBuffer_pubsetbuf()
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);
    char testBuffer[TEST_SIZE];

    CPPUNIT_ASSERT(buffer.pubsetbuf(testBuffer, TEST_SIZE) == NULL);
}

void StdStreamTestCase::OutputBuffer_pubseekoff()
{
    char testData[] = "0123456789";
    wxMemoryOutputStream stream(testData, 10);
    wxStdOutputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT_EQUAL(2,
                         buffer.pubseekoff(2, std::ios_base::beg,
                                           std::ios_base::out));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(2, std::ios_base::beg,
                                           std::ios_base::in));

    CPPUNIT_ASSERT_EQUAL(4,
                         buffer.pubseekoff(2, std::ios_base::cur));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(2, std::ios_base::cur,
                                           std::ios_base::in));

    CPPUNIT_ASSERT_EQUAL(8,
                         buffer.pubseekoff(-2, std::ios_base::end));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(-2, std::ios_base::end,
                                           std::ios_base::in));

    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(3, std::ios_base::cur));
    CPPUNIT_ASSERT_EQUAL(-1,
                         buffer.pubseekoff(3, std::ios_base::cur,
                                           std::ios_base::in));
}

void StdStreamTestCase::OutputBuffer_pubseekpos()
{
    char testData[] = "0123456789";
    wxMemoryOutputStream stream(testData, 10);
    wxStdOutputStreamBuffer buffer(stream);

    for (int i = 9; i >= 0; --i)
    {
        if (i % 2 == 0)
        {
            CPPUNIT_ASSERT_EQUAL(i, buffer.pubseekpos(i));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(i,
                                 buffer.pubseekpos(i, std::ios_base::out));
        }

        CPPUNIT_ASSERT_EQUAL('0' + (9 - i), buffer.sputc('0' + (9 - i)));
    }

    CPPUNIT_ASSERT(memcmp(testData, "9876543210", 10) == 0);

    CPPUNIT_ASSERT_EQUAL(-1, buffer.pubseekpos(5, std::ios_base::in));
}

void StdStreamTestCase::OutputBuffer_pubsync()
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    CPPUNIT_ASSERT(buffer.pubsync() == 0);
}

// --------------------------------------------------------------------------
// Output functions
// --------------------------------------------------------------------------

void StdStreamTestCase::OutputBuffer_sputc()
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    for (int i = 0; i < TEST_SIZE; ++i)
        buffer.sputc(m_testData[i]);

    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, stream.GetSize());

    char result[TEST_SIZE];

    stream.CopyTo(result, TEST_SIZE);
    CPPUNIT_ASSERT(memcmp(result, m_testData, TEST_SIZE) == 0);
}

void StdStreamTestCase::OutputBuffer_sputn()
{
    wxMemoryOutputStream stream;
    wxStdOutputStreamBuffer buffer(stream);

    buffer.sputn(m_testData, TEST_SIZE);
    CPPUNIT_ASSERT_EQUAL(TEST_SIZE, stream.GetSize());

    char result[TEST_SIZE];

    stream.CopyTo(result, TEST_SIZE);
    CPPUNIT_ASSERT(memcmp(result, m_testData, TEST_SIZE) == 0);
}

#endif // wxUSE_STD_IOSTREAM
