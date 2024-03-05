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

    CPPUNIT_TEST_SUITE(zlibStream);
        // Base class stream tests.
        CPPUNIT_TEST(Input_GetSizeFail);
        CPPUNIT_TEST(Input_GetC);
        CPPUNIT_TEST(Input_Read);
        CPPUNIT_TEST(Input_Eof);
        CPPUNIT_TEST(Input_LastRead);
        CPPUNIT_TEST(Input_CanRead);
        CPPUNIT_TEST(Input_SeekIFail);
        CPPUNIT_TEST(Input_TellI);
        CPPUNIT_TEST(Input_Peek);
        CPPUNIT_TEST(Input_Ungetch);

        CPPUNIT_TEST(Output_PutC);
        CPPUNIT_TEST(Output_Write);
        CPPUNIT_TEST(Output_LastWrite);
        CPPUNIT_TEST(Output_SeekOFail);
        CPPUNIT_TEST(Output_TellO);
    CPPUNIT_TEST_SUITE_END();

protected:
    wxLZMAInputStream *DoCreateInStream() override;
    wxLZMAOutputStream *DoCreateOutStream() override;

private:
    wxDECLARE_NO_COPY_CLASS(LZMAStream);
};

STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(LZMAStream)

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

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS
