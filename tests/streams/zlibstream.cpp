///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/zlibstream.cpp
// Purpose:     Test wxZlibInputStream/wxZlibOutputStream
// Author:      Hans Van Leemputten
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/cppunit.h"
#include "wx/zstream.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"

#include "bstream.h"

using namespace std;
using namespace CppUnit;

#define WXTEST_WITH_GZIP_CONDITION(testMethod) \
    WXTEST_WITH_CONDITION( COMPOSE_TEST_NAME(zlibStream), wxZlibInputStream::CanHandleGZip() && wxZlibOutputStream::CanHandleGZip(), testMethod )

#define DATABUFFER_SIZE 1024

static const wxString FILENAME_GZ = _T("zlibtest.gz");

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxZlibInputStream and wxZlibOutputStream

class zlibStream : public BaseStreamTestCase<wxZlibInputStream, wxZlibOutputStream>
{
public:
    zlibStream();
    virtual ~zlibStream();

    CPPUNIT_TEST_SUITE(zlibStream);
        // Base class stream tests the zlibstream supports.
        CPPUNIT_TEST_FAIL(Input_GetSize);
        CPPUNIT_TEST(Input_GetC);
        CPPUNIT_TEST(Input_Read);
        CPPUNIT_TEST(Input_Eof);
        CPPUNIT_TEST(Input_LastRead);
        CPPUNIT_TEST_FAIL(Input_SeekI);
        CPPUNIT_TEST(Input_TellI);
        CPPUNIT_TEST(Input_Peek);
        CPPUNIT_TEST(Input_Ungetch);

        CPPUNIT_TEST(Output_PutC);
        CPPUNIT_TEST(Output_Write);
        CPPUNIT_TEST(Output_LastWrite);
        CPPUNIT_TEST_FAIL(Output_SeekO);
        CPPUNIT_TEST(Output_TellO);

        // Other test specific for zlib stream test case.
        CPPUNIT_TEST(TestStream_NoHeader_Default);
        CPPUNIT_TEST(TestStream_NoHeader_NoComp);
        CPPUNIT_TEST(TestStream_NoHeader_SpeedComp);
        CPPUNIT_TEST(TestStream_NoHeader_BestComp);
        CPPUNIT_TEST(TestStream_ZLib_Default);
        CPPUNIT_TEST(TestStream_ZLib_NoComp);
        CPPUNIT_TEST(TestStream_ZLib_SpeedComp);
        CPPUNIT_TEST(TestStream_ZLib_BestComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_Default);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_NoComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_SpeedComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_BestComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_ZLibGZip);
        CPPUNIT_TEST(Decompress_BadData);
        CPPUNIT_TEST(Decompress_wx24Data);
    CPPUNIT_TEST_SUITE_END();

protected:
    // Test different stream construct settings.
    void TestStream_NoHeader_Default();
    void TestStream_NoHeader_NoComp();
    void TestStream_NoHeader_SpeedComp();
    void TestStream_NoHeader_BestComp();
    void TestStream_ZLib_Default();
    void TestStream_ZLib_NoComp();
    void TestStream_ZLib_SpeedComp();
    void TestStream_ZLib_BestComp();
    void TestStream_GZip_Default();
    void TestStream_GZip_NoComp();
    void TestStream_GZip_SpeedComp();
    void TestStream_GZip_BestComp();
    void TestStream_ZLibGZip();
    // Try to decompress bad data.
    void Decompress_BadData();
    // Decompress a data file created with wx 2.4.
    // Note: This test is limited in testing range!
    void Decompress_wx24Data();

private:
    const char *GetDataBuffer();
    const unsigned char *GetCompressedData();
    void doTestStreamData(int input_flag, int output_flag, int compress_level);

private:
    // Implement base class functions.
    virtual wxZlibInputStream  *DoCreateInStream();  
    virtual wxZlibOutputStream *DoCreateOutStream();
    virtual void DoDeleteInStream();
    virtual void DoDeleteOutStream();

private:
    char            m_DataBuffer[DATABUFFER_SIZE];
    size_t          m_SizeCompressedData;
    unsigned char  *m_pCompressedData;

    // Used by the base Creat[In|Out]Stream and Delete[In|Out]Stream.
    wxMemoryInputStream  *m_pTmpMemInStream;
    wxMemoryOutputStream *m_pTmpMemOutStream;
};

zlibStream::zlibStream()
    :m_SizeCompressedData(0),
     m_pCompressedData(NULL),
     m_pTmpMemInStream(NULL),
     m_pTmpMemOutStream(NULL)
{
    // Init the data buffer.
    for (size_t i = 0; i < DATABUFFER_SIZE; i++)
        m_DataBuffer[i] = (i % 0xFF);

    // Set extra base config settings.
    m_bSimpleTellITest = true;
    m_bSimpleTellOTest = true;
}

zlibStream::~zlibStream()
{
    delete m_pCompressedData;

    delete m_pTmpMemInStream;
    delete m_pTmpMemOutStream;
}

void zlibStream::TestStream_NoHeader_Default()
{
    doTestStreamData(wxZLIB_NO_HEADER, wxZLIB_NO_HEADER, wxZ_DEFAULT_COMPRESSION);
}
void zlibStream::TestStream_NoHeader_NoComp()
{
    doTestStreamData(wxZLIB_NO_HEADER, wxZLIB_NO_HEADER, wxZ_NO_COMPRESSION);
}
void zlibStream::TestStream_NoHeader_SpeedComp()
{
    doTestStreamData(wxZLIB_NO_HEADER, wxZLIB_NO_HEADER, wxZ_BEST_SPEED);
}
void zlibStream::TestStream_NoHeader_BestComp()
{
    doTestStreamData(wxZLIB_NO_HEADER, wxZLIB_NO_HEADER, wxZ_BEST_COMPRESSION);
}

void zlibStream::TestStream_ZLib_Default()
{
    doTestStreamData(wxZLIB_ZLIB, wxZLIB_ZLIB, wxZ_DEFAULT_COMPRESSION);
}
void zlibStream::TestStream_ZLib_NoComp()
{
    doTestStreamData(wxZLIB_ZLIB, wxZLIB_ZLIB, wxZ_NO_COMPRESSION);
}
void zlibStream::TestStream_ZLib_SpeedComp()
{
    doTestStreamData(wxZLIB_ZLIB, wxZLIB_ZLIB, wxZ_BEST_SPEED);
}
void zlibStream::TestStream_ZLib_BestComp()
{
    doTestStreamData(wxZLIB_ZLIB, wxZLIB_ZLIB, wxZ_BEST_COMPRESSION);
}

void zlibStream::TestStream_GZip_Default()
{
    doTestStreamData(wxZLIB_GZIP, wxZLIB_GZIP, wxZ_DEFAULT_COMPRESSION);
}
void zlibStream::TestStream_GZip_NoComp()
{
    doTestStreamData(wxZLIB_GZIP, wxZLIB_GZIP, wxZ_NO_COMPRESSION);
}
void zlibStream::TestStream_GZip_SpeedComp()
{
    doTestStreamData(wxZLIB_GZIP, wxZLIB_GZIP, wxZ_BEST_SPEED);
}
void zlibStream::TestStream_GZip_BestComp()
{
    doTestStreamData(wxZLIB_GZIP, wxZLIB_GZIP, wxZ_BEST_COMPRESSION);
}

void zlibStream::TestStream_ZLibGZip()
{
    // Only use default compression level, as this test is 
    // for testing if the streams can determine the stream type info them self...
    doTestStreamData(wxZLIB_ZLIB|wxZLIB_GZIP, wxZLIB_ZLIB, wxZ_DEFAULT_COMPRESSION);
    doTestStreamData(wxZLIB_ZLIB|wxZLIB_GZIP, wxZLIB_GZIP, wxZ_DEFAULT_COMPRESSION);
}

void zlibStream::Decompress_BadData()
{
    // Setup the bad data stream and the zlib stream.
    wxMemoryInputStream memstream_in(GetDataBuffer(), DATABUFFER_SIZE);
    CPPUNIT_ASSERT(memstream_in.IsOk());
    wxZlibInputStream zstream_in(memstream_in);
    CPPUNIT_ASSERT(zstream_in.IsOk()); // We did not yet read from the stream 
                                       // so it should still be OK.
    // Try to force the stream to go to bad status.
    CPPUNIT_ASSERT(!zstream_in.Eof());
    if (zstream_in.IsOk())
        zstream_in.GetC();

    // Because of the bad data in the input stream the zlib
    // stream should be marked as NOT OK.
    CPPUNIT_ASSERT(!zstream_in.IsOk());
}

void zlibStream::Decompress_wx24Data()
{
    // The wx24_value was used in a wxWidgets 2.4(.2) 
    // application to produce wx24_data, using wxZlibOutputStream.
    const unsigned char wx24_data[] = {120,156,242,72,205,201,201,87,40,207,47,202,73,97,0,0,0,0,255,255,0};
    const char *wx24_value = "Hello world";
    // Size of the value and date items.
    const size_t data_size = sizeof(wx24_data);
    const size_t value_size = strlen(wx24_value);

    const unsigned char *buf = GetCompressedData();
    m_pTmpMemInStream = new wxMemoryInputStream(buf, m_SizeCompressedData);

    wxMemoryInputStream memstream_in(wx24_data, data_size);
    CPPUNIT_ASSERT(memstream_in.IsOk());
    wxZlibInputStream zstream_in(memstream_in);
    CPPUNIT_ASSERT(zstream_in.IsOk());

    size_t i;
    for (i = 0; !zstream_in.Eof(); i++)
    {
        char last_value = zstream_in.GetC();
        if (last_value != wx24_value[i])
            break;

        // Don't go over the end of the value buffer...
        if (wx24_value[i] == '\0')
        {
            // And if we do then try to see how long the stream actually is.
            for (/* nothing */; !zstream_in.Eof(); i++)
            {
                // Move one item along in the stream.
                (void)zstream_in.GetC();
                
                // Check if we are in an infinite loop by multiplying value_size
                // by 5 to have a *much* bigger range then the real range.
                // Note: Incase you ask your self, why 5, the answer is no reason... 
                //       it is not to big and not to small a size, nothing more 
                //       nothing less to it.
                if (i > (value_size*5)) 
                {
                    // Note: Please make sure Input_Eof test passed.
                    CPPUNIT_FAIL("Infinite stream detected, breaking the infinite loop");
                    return;
                }
            }

            break;
        }
    }

    CPPUNIT_ASSERT_MESSAGE("Could not decompress data that was compressed with wxWidgets 2.4.x", i == (value_size + 1));
}

const char *zlibStream::GetDataBuffer()
{
    return m_DataBuffer;
}

const unsigned char *zlibStream::GetCompressedData()
{
    if (!m_pCompressedData)
    {
#if 1
        // Construct the compressed data live.
        wxMemoryOutputStream memstream_out;
        {
            const char *buf = "01234567890123456789012345678901234567890123456789"; /* = 50 */
            wxZlibOutputStream zstream_out(memstream_out);
            zstream_out.Write(buf, strlen(buf));
        }

        // Copy the to the 
        m_SizeCompressedData = memstream_out.GetSize();
        m_pCompressedData = new unsigned char[m_SizeCompressedData];
        memstream_out.CopyTo(m_pCompressedData, m_SizeCompressedData);
#else
        // Or use recorded compressed data.
        const unsigned char tmp[] = {120,218,51,48,52,50,54,49,53,51,183,176,52,32,142,197,0,0,3,229,10,9,0};
        m_SizeCompressedData = sizeof(tmp);
        m_pCompressedData = new unsigned char[m_SizeCompressedData+1];
        memcpy(m_pCompressedData, tmp, m_SizeCompressedData);
#endif
    }

    CPPUNIT_ASSERT(m_pCompressedData != NULL);    
    return m_pCompressedData;
}

void zlibStream::doTestStreamData(int input_flag, int output_flag, int compress_level)
{
    size_t fail_pos = 0;
    char last_value = 0;
    bool bWasEOF = true;

    {   // Part one: Create a compressed file.
        wxFileOutputStream fstream_out(FILENAME_GZ);
        CPPUNIT_ASSERT(fstream_out.IsOk());
        wxZlibOutputStream zstream_out(fstream_out, compress_level, output_flag);
        if (!zstream_out.IsOk())
        {
            if (output_flag == wxZLIB_GZIP)
                CPPUNIT_FAIL("Could not create the gzip output stream. Note: gzip requires zlib 1.2+!");
            else
                CPPUNIT_FAIL("Could not create the output stream");
        }

        // Next: Compress some data so the file is containing something.
        zstream_out.Write(GetDataBuffer(), DATABUFFER_SIZE);
    }

    {   // Part two: Verify that the compressed data when uncompressed 
        //           matches the original data.
        wxFileInputStream fstream_in(FILENAME_GZ);
        CPPUNIT_ASSERT(fstream_in.IsOk());
        wxZlibInputStream zstream_in(fstream_in, input_flag);
        CPPUNIT_ASSERT_MESSAGE("Could not create the input stream", zstream_in.IsOk());

        // Next: Check char per char if the returned data is valid.
        const char *pbuf = GetDataBuffer();
        for (fail_pos = 0; !zstream_in.Eof(); fail_pos++)
        {
            last_value = zstream_in.GetC();
            if (last_value != pbuf[fail_pos])
                break;
        }

        bWasEOF = zstream_in.Eof();
    }

    // Remove the temp file...
    ::wxRemoveFile(FILENAME_GZ);

    // Check state of the verify action.
    if (fail_pos != DATABUFFER_SIZE || !bWasEOF)
    {
        wxString msg(wxString::Format(_T("Wrong data item at pos %d (Org_val %d != Zlib_val %d), with compression level %d"), 
                                            fail_pos, GetDataBuffer()[fail_pos], last_value, compress_level));
        CPPUNIT_FAIL(string(msg.mb_str()));
    }
}

wxZlibInputStream *zlibStream::DoCreateInStream()    
{ 
    const unsigned char *buf = GetCompressedData();
    m_pTmpMemInStream = new wxMemoryInputStream(buf, m_SizeCompressedData);
    CPPUNIT_ASSERT(m_pTmpMemInStream->IsOk());
    wxZlibInputStream *pzstream_in = new wxZlibInputStream(*m_pTmpMemInStream);
    CPPUNIT_ASSERT(pzstream_in->IsOk());
    return pzstream_in;
}
wxZlibOutputStream *zlibStream::DoCreateOutStream()
{ 
    m_pTmpMemOutStream = new wxMemoryOutputStream();
    CPPUNIT_ASSERT(m_pTmpMemOutStream->IsOk());
    wxZlibOutputStream *pzstream_out = new wxZlibOutputStream(*m_pTmpMemOutStream);
    CPPUNIT_ASSERT(pzstream_out->IsOk());
    return pzstream_out;
}
void zlibStream::DoDeleteInStream()  
{ 
    delete m_pTmpMemInStream;
    m_pTmpMemInStream = NULL;
}
void zlibStream::DoDeleteOutStream() 
{ 
    delete m_pTmpMemOutStream;
    m_pTmpMemOutStream = NULL;
}

// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(zlibStream)

