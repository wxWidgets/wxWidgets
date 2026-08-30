///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/lzmastream.cpp
// Purpose:     Unit tests for LZMA stream classes
// Author:      Vadim Zeitlin
// Created:     2018-03-30
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#if wxUSE_LIBLZMA && wxUSE_STREAMS

#include "wx/mstream.h"
#include "wx/lzmastream.h"

#include "bstream.h"

class LZMAStream : public BaseStreamTestCase<wxLZMAInputStream, wxLZMAOutputStream>
{
public:
    LZMAStream();

protected:
    wxLZMAInputStream *DoCreateInStream() override;
    wxLZMAOutputStream *DoCreateOutStream() override;

private:
    wxDECLARE_NO_COPY_CLASS(LZMAStream);
};

LZMAStream::LZMAStream()
{
    // Disable TellI() and TellO() tests in the base class which don't work
    // with the compressed streams.
    m_bSimpleTellITest =
    m_bSimpleTellOTest = true;
}

wxLZMAInputStream *LZMAStream::DoCreateInStream()
{
    // Compress some data.
    const char data[] = "This is just some test data for LZMA streams unit test";
    const size_t len = sizeof(data);

    wxMemoryOutputStream outmem;
    wxLZMAOutputStream outz(outmem);
    outz.Write(data, len);
    REQUIRE( outz.LastWrite() == len );
    REQUIRE( outz.Close() );

    wxMemoryInputStream* const inmem = new wxMemoryInputStream(outmem);
    REQUIRE( inmem->IsOk() );

    // Give ownership of the memory input stream to the LZMA stream.
    return new wxLZMAInputStream(inmem);
}

wxLZMAOutputStream *LZMAStream::DoCreateOutStream()
{
    return new wxLZMAOutputStream(new wxMemoryOutputStream());
}

// Base class stream tests.
WX_STREAM_TEST_CASE(LZMAStream, Input_GetSizeFail)
WX_STREAM_TEST_CASE(LZMAStream, Input_GetC)
WX_STREAM_TEST_CASE(LZMAStream, Input_Read)
WX_STREAM_TEST_CASE(LZMAStream, Input_Eof)
WX_STREAM_TEST_CASE(LZMAStream, Input_LastRead)
WX_STREAM_TEST_CASE(LZMAStream, Input_CanRead)
WX_STREAM_TEST_CASE(LZMAStream, Input_SeekIFail)
WX_STREAM_TEST_CASE(LZMAStream, Input_TellI)
WX_STREAM_TEST_CASE(LZMAStream, Input_Peek)
WX_STREAM_TEST_CASE(LZMAStream, Input_Ungetch)

WX_STREAM_TEST_CASE(LZMAStream, Output_PutC)
WX_STREAM_TEST_CASE(LZMAStream, Output_Write)
WX_STREAM_TEST_CASE(LZMAStream, Output_LastWrite)
WX_STREAM_TEST_CASE(LZMAStream, Output_SeekOFail)
WX_STREAM_TEST_CASE(LZMAStream, Output_TellO)

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS
