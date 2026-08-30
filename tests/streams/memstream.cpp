///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/memstream.cpp
// Purpose:     Test wxMemoryInputStream/wxMemoryOutputStream
// Author:      Hans Van Leemputten
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
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

protected:
    // Tests specific to this stream.
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
    CHECK(pMemInStream->IsOk());
    return pMemInStream;
}
wxMemoryOutputStream *memStream::DoCreateOutStream()
{
    wxMemoryOutputStream *pMemOutStream = new wxMemoryOutputStream();
    CHECK(pMemOutStream->IsOk());
    return pMemOutStream;
}

void memStream::Ctor_InFromIn()
{
    wxMemoryInputStream *pMemInStream1 = DoCreateInStream();
    wxMemoryInputStream *pMemInStream2 = new wxMemoryInputStream(*pMemInStream1);
    CHECK(pMemInStream2->IsOk());
    CHECK(pMemInStream2->GetLength() == pMemInStream1->GetLength());
    wxFileOffset len = pMemInStream2->GetLength();
    char *dat = new char[len];
    pMemInStream2->Read(dat, len);
    CHECK((wxFileOffset)pMemInStream2->LastRead() == len);
    wxStreamBuffer *buf = pMemInStream1->GetInputStreamBuffer();
    void *pIn = buf->GetBufferStart();
    CHECK(memcmp(pIn, dat, len) == 0);
    delete pMemInStream2;

    wxFileOffset len2 = len / 2;
    CHECK(len2);
    CHECK(pMemInStream1->SeekI(-len2, wxFromCurrent) != wxInvalidOffset);
    pIn = buf->GetBufferPos();
    pMemInStream2 = new wxMemoryInputStream(*pMemInStream1, len2);
    CHECK(pMemInStream2->IsOk());
    CHECK(pMemInStream2->GetLength() == (wxFileOffset)len2);
    pMemInStream2->Read(dat, len2);
    CHECK((wxFileOffset)pMemInStream2->LastRead() == len2);
    CHECK(memcmp(pIn, dat, len2) == 0);

    delete[] dat;
    delete pMemInStream2;
    delete pMemInStream1;
}

void memStream::Ctor_InFromOut()
{
    wxMemoryOutputStream *pMemOutStream = DoCreateOutStream();
    pMemOutStream->Write(GetDataBuffer(), DATABUFFER_SIZE);
    wxMemoryInputStream *pMemInStream = new wxMemoryInputStream(*pMemOutStream);
    CHECK(pMemInStream->IsOk());
    CHECK(pMemOutStream->GetLength() == pMemInStream->GetLength());
    size_t len = pMemInStream->GetLength();
    wxStreamBuffer *in = pMemInStream->GetInputStreamBuffer();
    wxStreamBuffer *out = pMemOutStream->GetOutputStreamBuffer();
    void *pIn = in->GetBufferStart();
    void *pOut = out->GetBufferStart();
    CHECK(pIn != pOut);
    CHECK(memcmp(pIn, pOut, len) == 0);
    delete pMemInStream;
    delete pMemOutStream;
}

// Base class stream tests the memStream supports.
WX_STREAM_TEST_CASE(memStream, Input_GetSize)
WX_STREAM_TEST_CASE(memStream, Input_GetC)
WX_STREAM_TEST_CASE(memStream, Input_Read)
WX_STREAM_TEST_CASE(memStream, Input_Eof)
WX_STREAM_TEST_CASE(memStream, Input_LastRead)
WX_STREAM_TEST_CASE(memStream, Input_CanRead)
WX_STREAM_TEST_CASE(memStream, Input_SeekI)
WX_STREAM_TEST_CASE(memStream, Input_TellI)
WX_STREAM_TEST_CASE(memStream, Input_Peek)
WX_STREAM_TEST_CASE(memStream, Input_Ungetch)

WX_STREAM_TEST_CASE(memStream, Output_PutC)
WX_STREAM_TEST_CASE(memStream, Output_Write)
WX_STREAM_TEST_CASE(memStream, Output_LastWrite)
WX_STREAM_TEST_CASE(memStream, Output_SeekO)
WX_STREAM_TEST_CASE(memStream, Output_TellO)

// Other test specific for Memory stream test case.
WX_STREAM_TEST_CASE(memStream, Ctor_InFromIn)
WX_STREAM_TEST_CASE(memStream, Ctor_InFromOut)
