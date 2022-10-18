///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/sstream.cpp
// Purpose:     Test wxStringInputStream/wxStringOutputStream
// Author:      Vadim Zeitlin
// Copyright:   (c) 2004 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
#endif

#include "wx/sstream.h"

#include "bstream.h"

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxStringInputStream and wxStringOutputStream

class strStream :
        public BaseStreamTestCase<wxStringInputStream, wxStringOutputStream>
{
public:
    strStream();
    virtual ~strStream();

    CPPUNIT_TEST_SUITE(strStream);
        // Base class stream tests the strStream supports.
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
        // seeking currently not supported by output string stream
        //CPPUNIT_TEST(Output_SeekO);
        //CPPUNIT_TEST(Output_TellO);

        // Other test specific for String stream test case.
        CPPUNIT_TEST(Output_Check);
    CPPUNIT_TEST_SUITE_END();

protected:
    void Output_Check();

private:
    // Implement base class functions.
    virtual wxStringInputStream  *DoCreateInStream() override;
    virtual wxStringOutputStream *DoCreateOutStream() override;

    // output the given string to wxStringOutputStream and check that its
    // contents is exactly the same string
    void CheckString(const wxString& text);

    wxString m_str;
};

strStream::strStream()
{
    static const size_t LEN = 256;
    m_str.reserve(LEN);
    for ( size_t n = 0; n < LEN; n++ )
    {
        m_str += wxChar(wxT('A') + n % (wxT('Z') - wxT('A') + 1));
    }
}

strStream::~strStream()
{
}

wxStringInputStream *strStream::DoCreateInStream()
{
    wxStringInputStream *pStrInStream = new wxStringInputStream(m_str);
    CPPUNIT_ASSERT(pStrInStream->IsOk());
    return pStrInStream;
}

wxStringOutputStream *strStream::DoCreateOutStream()
{
    wxStringOutputStream *pStrOutStream = new wxStringOutputStream();
    CPPUNIT_ASSERT(pStrOutStream->IsOk());
    return pStrOutStream;
}

void strStream::CheckString(const wxString& text)
{
    wxStringOutputStream sos;

    const wxCharBuffer buf(text.To8BitData());
    sos.Write(buf, buf.length());

    CPPUNIT_ASSERT_EQUAL( text, sos.GetString() );
}

void strStream::Output_Check()
{
    CheckString("Hello world!");
    CheckString(wxString("hi\0dden", 8));
}

// Register the stream sub suite, by using some stream helper macro.
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(strStream)

TEST_CASE("wxStringOutputStream::Tell", "[stream]")
{
    wxStringOutputStream ss;
    CHECK( ss.TellO() == 0 );

    const char* const s = "Hello world";
    const wxFileOffset len = strlen(s);

    ss.Write(s, len);
    CHECK( ss.TellO() == len );

    wxString str(s);
    CHECK( wxStringOutputStream(&str).TellO() == len );

#if wxUSE_UNICODE
    wxMBConvUTF16 convUTF16;
    wxStringOutputStream ss16(nullptr, convUTF16);
    CHECK( ss16.TellO() == 0 );

    const wxCharBuffer s16 = convUTF16.cWC2MB(wxWCharBuffer(str.wc_str()));
    ss16.Write(s16, s16.length());
    CHECK( ss16.TellO() == 2*len );
    CHECK( wxStringOutputStream(&str, convUTF16).TellO() == 2*len );

    // The U+2070D character is represented by a surrogate pair in UTF-16.
    wxString u2070D = wxString::FromUTF8("\xF0\xA0\x9C\x8D");
    CHECK( wxStringOutputStream(&u2070D, convUTF16).TellO() == 4 );
#endif // wxUSE_UNICODE
}
