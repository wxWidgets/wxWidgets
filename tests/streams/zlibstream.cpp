///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/zlibstream.cpp
// Purpose:     Test wxZlibInputStream/wxZlibOutputStream
// Author:      Hans Van Leemputten
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Hans Van Leemputten
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

#include "wx/zstream.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"
#include "wx/txtstrm.h"
#include "wx/buffer.h"

#include "bstream.h"

using std::string;

#define WXTEST_WITH_GZIP_CONDITION(testMethod) \
    WXTEST_WITH_CONDITION( COMPOSE_TEST_NAME(zlibStream), wxZlibInputStream::CanHandleGZip() && wxZlibOutputStream::CanHandleGZip(), testMethod )

#define DATABUFFER_SIZE 1024

static const wxString FILENAME_GZ = wxT("zlibtest.gz");

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
        CPPUNIT_TEST(Input_CanRead);
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
        CPPUNIT_TEST(TestStream_NoHeader_Dictionary);
        CPPUNIT_TEST(TestStream_ZLib_Default);
        CPPUNIT_TEST(TestStream_ZLib_NoComp);
        CPPUNIT_TEST(TestStream_ZLib_SpeedComp);
        CPPUNIT_TEST(TestStream_ZLib_BestComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_Default);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_NoComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_SpeedComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_BestComp);
        WXTEST_WITH_GZIP_CONDITION(TestStream_GZip_Dictionary);
        WXTEST_WITH_GZIP_CONDITION(TestStream_ZLibGZip);
        CPPUNIT_TEST(Decompress_BadData);
        CPPUNIT_TEST(Decompress_wx251_zlib114_Data_NoHeader);
        CPPUNIT_TEST(Decompress_wx251_zlib114_Data_ZLib);
        WXTEST_WITH_GZIP_CONDITION(Decompress_gzip135Data);
    CPPUNIT_TEST_SUITE_END();

protected:
    // Test different stream construct settings.
    void TestStream_NoHeader_Default();
    void TestStream_NoHeader_NoComp();
    void TestStream_NoHeader_SpeedComp();
    void TestStream_NoHeader_BestComp();
    void TestStream_NoHeader_Dictionary();
    void TestStream_ZLib_Default();
    void TestStream_ZLib_NoComp();
    void TestStream_ZLib_SpeedComp();
    void TestStream_ZLib_BestComp();
    void TestStream_GZip_Default();
    void TestStream_GZip_NoComp();
    void TestStream_GZip_SpeedComp();
    void TestStream_GZip_BestComp();
    void TestStream_GZip_Dictionary();
    void TestStream_ZLibGZip();
    // Try to decompress bad data.
    void Decompress_BadData();
    // Decompress data that was compress by an external app.
    // (like test wx 2.4.2, 2.5.1 and gzip data)
    // Note: This test is limited in testing range!
    void Decompress_wx251_zlib114_Data_NoHeader();
    void Decompress_wx251_zlib114_Data_ZLib();
    void Decompress_gzip135Data();

private:
    const char *GetDataBuffer();
    const unsigned char *GetCompressedData();
    void doTestStreamData(int input_flag, int output_flag, int compress_level, const wxMemoryBuffer *buf = NULL);
    void doDecompress_ExternalData(const unsigned char *data, const char *value, size_t data_size, size_t value_size, int flag = wxZLIB_AUTO);

private:
    // Implement base class functions.
    virtual wxZlibInputStream  *DoCreateInStream();
    virtual wxZlibOutputStream *DoCreateOutStream();
    virtual void DoDeleteInStream();
    virtual void DoDeleteOutStream();

    // Helper that can be used to create new wx compatibility tests...
    // Otherwise not used by the tests.
    void genExtTestData(wxTextOutputStream &out, const char *buf, int flag);

private:
    char            m_DataBuffer[DATABUFFER_SIZE];
    size_t          m_SizeCompressedData;
    unsigned char  *m_pCompressedData;
    wxMemoryBuffer  m_Dictionary;

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

    m_Dictionary.AppendData(m_DataBuffer, sizeof(m_DataBuffer) / 2);

    // Set extra base config settings.
    m_bSimpleTellITest = true;
    m_bSimpleTellOTest = true;

/* Example code on how to produce test data...
    {
        wxFFileOutputStream fstream_out(wxT("gentest.cpp"));
        wxTextOutputStream out( fstream_out );

        genExtTestData(out, "zlib data created with wxWidgets 2.5.x [March 27], wxZLIB_NO_HEADER, zlib 1.1.4", wxZLIB_NO_HEADER);
        genExtTestData(out, "zlib data created with wxWidgets 2.5.x [March 27], wxZLIB_ZLIB, zlib 1.1.4", wxZLIB_ZLIB);
    }
*/
}

zlibStream::~zlibStream()
{
    delete[] m_pCompressedData;

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
void zlibStream::TestStream_NoHeader_Dictionary()
{
    doTestStreamData(wxZLIB_NO_HEADER, wxZLIB_NO_HEADER, wxZ_DEFAULT_COMPRESSION, &m_Dictionary);
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
void zlibStream::TestStream_GZip_Dictionary()
{
    doTestStreamData(wxZLIB_GZIP, wxZLIB_GZIP, wxZ_DEFAULT_COMPRESSION, &m_Dictionary);
}

void zlibStream::TestStream_ZLibGZip()
{
    // Only use default compression level, as this test is
    // for testing if the streams can determine the stream type info them self...
    doTestStreamData(wxZLIB_AUTO, wxZLIB_ZLIB, wxZ_DEFAULT_COMPRESSION);
    doTestStreamData(wxZLIB_AUTO, wxZLIB_GZIP, wxZ_DEFAULT_COMPRESSION);
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

void zlibStream::Decompress_wx251_zlib114_Data_NoHeader()
{
    const unsigned char data[] = {171,202,201,76,82,72,73,44,73,84,72,46,74,77,44,73,77,81,40,207,44,201,80,40,175,8,207,76,73,79,45,41,86,48,210,51,213,171,80,136,246,77,44,74,206,80,48,50,143,213,1,202,69,249,120,58,197,251,249,199,123,184,58,186,184,6,233,40,84,129,12,49,212,51,212,51,1,0,32};
    const char *value = "zlib data created with wxWidgets 2.5.x [March 27], wxZLIB_NO_HEADER, zlib 1.1.4";
    const size_t data_size = sizeof(data);
    const size_t value_size = strlen(value);
    // We need to specify wxZLIB_NO_HEADER because wxZLIB_AUTO can't find it his self.
    doDecompress_ExternalData(data, value, data_size, value_size, wxZLIB_NO_HEADER);
}

void zlibStream::Decompress_wx251_zlib114_Data_ZLib()
{
    const unsigned char data[] = {120,156,171,202,201,76,82,72,73,44,73,84,72,46,74,77,44,73,77,81,40,207,44,201,80,40,175,8,207,76,73,79,45,41,86,48,210,51,213,171,80,136,246,77,44,74,206,80,48,50,143,213,1,202,69,249,120,58,197,131,8,29,133,42,144,126,67,61,67,61,19,0,191,86,23,216};
    const char *value = "zlib data created with wxWidgets 2.5.x [March 27], wxZLIB_ZLIB, zlib 1.1.4";
    const size_t data_size = sizeof(data);
    const size_t value_size = strlen(value);
    doDecompress_ExternalData(data, value, data_size, value_size);
}

void zlibStream::Decompress_gzip135Data()
{
    // Compressed data was on the command line with gzip 1.3.5.
    const unsigned char gzip135_data[] = {31,139,8,0,177,248,112,64,4,3,115,206,207,45,40,74,45,46,78,77,81,72,73,44,73,84,72,46,74,77,44,1,114,202,51,75,50,20,220,253,66,21,210,171,50,11,20,12,245,140,245,76,185,0,1,107,16,80,44,0,0,0,0};
    const char *gzip135_value = "Compressed data created with GNU gzip 1.3.5\n";
    // Size of the value and date items.
    const size_t data_size = sizeof(gzip135_data);
    const size_t value_size = strlen(gzip135_value);

    // Perform a generic data test on the data.
    doDecompress_ExternalData(gzip135_data, gzip135_value, data_size, value_size);
}

const char *zlibStream::GetDataBuffer()
{
    return m_DataBuffer;
}

const unsigned char *zlibStream::GetCompressedData()
{
    if (!m_pCompressedData)
    {
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
    }

    CPPUNIT_ASSERT(m_pCompressedData != NULL);
    return m_pCompressedData;
}

void zlibStream::doTestStreamData(int input_flag, int output_flag, int compress_level, const wxMemoryBuffer *buf)
{
    size_t fail_pos;
    char last_value = 0;
    bool bWasEOF;

    {   // Part one: Create a compressed file.
        wxFileOutputStream fstream_out(FILENAME_GZ);
        CPPUNIT_ASSERT(fstream_out.IsOk());
        {
            wxZlibOutputStream zstream_out(fstream_out, compress_level, output_flag);
            CPPUNIT_ASSERT_MESSAGE("Could not create the output stream", zstream_out.IsOk());

            if (buf)
                zstream_out.SetDictionary(*buf);

            // Next: Compress some data so the file is containing something.
            zstream_out.Write(GetDataBuffer(), DATABUFFER_SIZE);
        }

        // Next thing is required by zlib versions pre 1.2.0.
        if (input_flag == wxZLIB_NO_HEADER)
            fstream_out.PutC(' ');
    }

    {   // Part two: Verify that the compressed data when uncompressed
        //           matches the original data.
        wxFileInputStream fstream_in(FILENAME_GZ);
        CPPUNIT_ASSERT(fstream_in.IsOk());
        wxZlibInputStream zstream_in(fstream_in, input_flag);
        CPPUNIT_ASSERT_MESSAGE("Could not create the input stream", zstream_in.IsOk());

        if (buf)
            zstream_in.SetDictionary(*buf);

        // Next: Check char per char if the returned data is valid.
        const char *pbuf = GetDataBuffer();
        for (fail_pos = 0; !zstream_in.Eof(); fail_pos++)
        {
            last_value = zstream_in.GetC();
            if (zstream_in.LastRead() != 1 ||
                last_value != pbuf[fail_pos])
                break;
        }

        bWasEOF = zstream_in.Eof();
    }

    // Remove the temp file...
    ::wxRemoveFile(FILENAME_GZ);

    // Check state of the verify action.
    if (fail_pos != DATABUFFER_SIZE || !bWasEOF)
    {
        wxString msg;
        msg << wxT("Wrong data item at pos ") << fail_pos
            << wxT(" (Org_val ") << GetDataBuffer()[fail_pos]
            << wxT(" != Zlib_val ") << last_value
            << wxT("), with compression level ") << compress_level;
        CPPUNIT_FAIL(string(msg.mb_str()));
    }
}

void zlibStream::doDecompress_ExternalData(const unsigned char *data, const char *value, size_t data_size, size_t value_size, int flag)
{
    // See that the input is ok.
    wxASSERT(data != NULL);
    wxASSERT(value != NULL);
    wxASSERT(data_size > 0);
    wxASSERT(value_size > 0);

    // Quickly try to see if the data is valid.
    switch (flag)
    {
    case wxZLIB_NO_HEADER:
        break;
    case wxZLIB_ZLIB:
        if (!(data_size >= 1 && data[0] == 0x78))
        {
            wxLogError(wxT("zlib data seems to not be zlib data!"));
        }
        break;
    case wxZLIB_GZIP:
        if (!(data_size >= 2 && data[0] == 0x1F && data[1] == 0x8B))
        {
            wxLogError(wxT("gzip data seems to not be gzip data!"));
        }
        break;
    case wxZLIB_AUTO:
        if (!(data_size >= 1 && data[0] == 0x78) ||
            !(data_size >= 2 && data[0] == 0x1F && data[1] == 0x8B))
        {
            wxLogError(wxT("Data seems to not be zlib or gzip data!"));
        }
    default:
        wxLogError(wxT("Unknown flag, skipping quick test."));
    };

    // Creat the needed streams.
    wxMemoryInputStream memstream_in(data, data_size);
    CPPUNIT_ASSERT(memstream_in.IsOk());
    wxZlibInputStream zstream_in(memstream_in, flag);
    CPPUNIT_ASSERT(zstream_in.IsOk());

    bool bValueEq = true;
    size_t i;
    for (i = 0; !zstream_in.Eof(); i++)
    {
        char last_value = zstream_in.GetC();

        // First check if it is a valid read.
        if (zstream_in.LastRead() == 1)
        {
            // Check the values
            if (last_value != value[i])
            {
                bValueEq = false;
                break;
            }
        }
        else
        {
            // If the read failed and turned the stream to Eof we stop reading.
            if (zstream_in.Eof())
                break;

            CPPUNIT_ASSERT_MESSAGE("Stream is no longer ok!", zstream_in.IsOk());
        }

        // Don't go over the end of the value buffer...
        if (i == value_size)
        {
            // And if we do then try to see how long the stream actually is.
            while (!zstream_in.Eof())
            {
                // Move one item along in the stream.
                (void)zstream_in.GetC();
                i++;

                // Check if we are in an infinite loop by multiplying value_size
                // by 5 to have a *much* bigger range then the real range.
                // Note: In case you ask yourself, why 5, the answer is no reason...
                //       it is not too big and not to small a size, nothing more
                //       nothing less to it.
                if (i > (value_size*5))
                {
                    // Note: Please make sure Input_Eof test passed.
                    CPPUNIT_FAIL("Infinite stream detected, breaking the infinite loop");
                    return;
                }
            }
        }
    }

    CPPUNIT_ASSERT_MESSAGE("Could not decompress the compressed data, original and restored value did not match.",
                           i == value_size && bValueEq);
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


void zlibStream::genExtTestData(wxTextOutputStream &out, const char *buf, int flag)
{
    unsigned char *data;
    size_t size;

    {   // Gen data
        wxMemoryOutputStream memstream_out;
        {
            wxZlibOutputStream zstream_out(memstream_out, wxZ_DEFAULT_COMPRESSION, flag);
            zstream_out.Write(buf, strlen(buf));
        }
        if (flag == wxZLIB_NO_HEADER)
            memstream_out.PutC(' ');

        size = memstream_out.GetSize();
        data = new unsigned char[size];
        memstream_out.CopyTo(data, size);
    }

    out << wxT("void zlibStream::Decompress_wxXXXData()") << wxT("\n");
    out << wxT("{") << wxT("\n") << wxT("    const unsigned char data[] = {");

    size_t i;
    for (i = 0; i < size; i++)
    {
        if (i+1 != size)
            out << wxString::Format(wxT("%d,"), data[i]);
        else
            out << wxString::Format(wxT("%d"), data[i]);
    }
    delete [] data;

    out << wxT("};") << wxT("\n");
    out << wxT("    const char *value = \"") << wxString(buf, wxConvUTF8) << wxT("\";") << wxT("\n");
    out << wxT("    const size_t data_size = sizeof(data);") << wxT("\n");
    out << wxT("    const size_t value_size = strlen(value);") << wxT("\n");
    out << wxT("    doDecompress_ExternalData(data, value, data_size, value_size);") << wxT("\n");
    out << wxT("}") << wxT("\n");
}


// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(zlibStream)

