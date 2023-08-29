///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/memstream.cpp
// Purpose:     Test wxMemoryInputStream/wxMemoryOutputStream
// Author:      Hans Van Leemputten
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/mstream.h"

#include "bstream.h"

#define DATABUFFER_SIZE     256

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxMemoryInputStream and wxMemoryOutputStream

class memStream : public BaseStreamTestCase<wxMemoryInputStream, wxMemoryOutputStream>
{
public:
    memStream();
    virtual ~memStream();

    CPPUNIT_TEST_SUITE(memStream);
        // Base class stream tests the memStream supports.
        CPPUNIT_TEST(Input_GetSize);
        CPPUNIT_TEST(Input_GetC);
        CPPUNIT_TEST(Input_Read);
        CPPUNIT_TEST(Input_Eof);
        CPPUNIT_TEST(Input_LastRead);
        CPPUNIT_TEST(Input_CanRead);
        CPPUNIT_TEST(Input_SeekI);
        CPPUNIT_TEST(Input_TellI);
        CPPUNIT_TEST(Input_Peek);
        CPPUNIT_TEST(Input_Ungetch);

        CPPUNIT_TEST(Output_PutC);
        CPPUNIT_TEST(Output_Write);
        CPPUNIT_TEST(Output_LastWrite);
        CPPUNIT_TEST(Output_SeekO);
        CPPUNIT_TEST(Output_TellO);

        // Other test specific for Memory stream test case.
        CPPUNIT_TEST(Ctor_InFromIn);
        CPPUNIT_TEST(Ctor_InFromOut);
    CPPUNIT_TEST_SUITE_END();

protected:
    // Add own test here.
    void Ctor_InFromIn();
    void Ctor_InFromOut();

private:
    const char *GetDataBuffer();

private:
    // Implement base class functions.
    virtual wxMemoryInputStream  *DoCreateInStream() override;
    virtual wxMemoryOutputStream *DoCreateOutStream() override;

private:
    char    m_DataBuffer[DATABUFFER_SIZE];
};

memStream::memStream()
{
    // Init the data buffer.
    for (size_t i = 0; i < DATABUFFER_SIZE; i++)
        m_DataBuffer[i] = (i % 0xFF);
}

memStream::~memStream()
{
    /* Nothing extra for now. */
}

const char *memStream::GetDataBuffer()
{
    return m_DataBuffer;
}

wxMemoryInputStream *memStream::DoCreateInStream()
{
    wxMemoryInputStream *pMemInStream = new wxMemoryInputStream(GetDataBuffer(), DATABUFFER_SIZE);
    CPPUNIT_ASSERT(pMemInStream->IsOk());
    return pMemInStream;
}
wxMemoryOutputStream *memStream::DoCreateOutStream()
{
    wxMemoryOutputStream *pMemOutStream = new wxMemoryOutputStream();
    CPPUNIT_ASSERT(pMemOutStream->IsOk());
    return pMemOutStream;
}

void memStream::Ctor_InFromIn()
{
    wxMemoryInputStream *pMemInStream1 = DoCreateInStream();
    wxMemoryInputStream *pMemInStream2 = new wxMemoryInputStream(*pMemInStream1);
    CPPUNIT_ASSERT(pMemInStream2->IsOk());
    CPPUNIT_ASSERT_EQUAL(pMemInStream1->GetLength(), pMemInStream2->GetLength());
    wxFileOffset len = pMemInStream2->GetLength();
    char *dat = new char[len];
    pMemInStream2->Read(dat, len);
    CPPUNIT_ASSERT_EQUAL(len, (wxFileOffset)pMemInStream2->LastRead());
    wxStreamBuffer *buf = pMemInStream1->GetInputStreamBuffer();
    void *pIn = buf->GetBufferStart();
    CPPUNIT_ASSERT(memcmp(pIn, dat, len) == 0);
    delete pMemInStream2;

    wxFileOffset len2 = len / 2;
    CPPUNIT_ASSERT(len2);
    CPPUNIT_ASSERT(pMemInStream1->SeekI(-len2, wxFromCurrent) != wxInvalidOffset);
    pIn = buf->GetBufferPos();
    pMemInStream2 = new wxMemoryInputStream(*pMemInStream1, len2);
    CPPUNIT_ASSERT(pMemInStream2->IsOk());
    CPPUNIT_ASSERT_EQUAL((wxFileOffset)len2, pMemInStream2->GetLength());
    pMemInStream2->Read(dat, len2);
    CPPUNIT_ASSERT_EQUAL(len2, (wxFileOffset)pMemInStream2->LastRead());
    CPPUNIT_ASSERT(memcmp(pIn, dat, len2) == 0);

    delete[] dat;
    delete pMemInStream2;
    delete pMemInStream1;
}

void memStream::Ctor_InFromOut()
{
    wxMemoryOutputStream *pMemOutStream = DoCreateOutStream();
    pMemOutStream->Write(GetDataBuffer(), DATABUFFER_SIZE);
    wxMemoryInputStream *pMemInStream = new wxMemoryInputStream(*pMemOutStream);
    CPPUNIT_ASSERT(pMemInStream->IsOk());
    CPPUNIT_ASSERT_EQUAL(pMemInStream->GetLength(), pMemOutStream->GetLength());
    size_t len = pMemInStream->GetLength();
    wxStreamBuffer *in = pMemInStream->GetInputStreamBuffer();
    wxStreamBuffer *out = pMemOutStream->GetOutputStreamBuffer();
    void *pIn = in->GetBufferStart();
    void *pOut = out->GetBufferStart();
    CPPUNIT_ASSERT(pIn != pOut);
    CPPUNIT_ASSERT(memcmp(pIn, pOut, len) == 0);
    delete pMemInStream;
    delete pMemOutStream;
}

// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(memStream)
