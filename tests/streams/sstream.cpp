///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/sstream.cpp
// Purpose:     Test wxStringInputStream/wxStringOutputStream
// Author:      Vadim Zeitlin
// Copyright:   (c) 2004 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
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
    CHECK(pStrInStream->IsOk());
    return pStrInStream;
}

wxStringOutputStream *strStream::DoCreateOutStream()
{
    wxStringOutputStream *pStrOutStream = new wxStringOutputStream();
    CHECK(pStrOutStream->IsOk());
    return pStrOutStream;
}

void strStream::CheckString(const wxString& text)
{
    wxStringOutputStream sos;

    const wxCharBuffer buf(text.To8BitData());
    sos.Write(buf, buf.length());

    CHECK( sos.GetString() == text );
}

void strStream::Output_Check()
{
    CheckString("Hello world!");
    CheckString(wxString("hi\0dden", 8));
}

// Base class stream tests the strStream supports.
WX_STREAM_TEST_CASE(strStream, Input_GetSize)
WX_STREAM_TEST_CASE(strStream, Input_GetC)
WX_STREAM_TEST_CASE(strStream, Input_Read)
WX_STREAM_TEST_CASE(strStream, Input_Eof)
WX_STREAM_TEST_CASE(strStream, Input_LastRead)
WX_STREAM_TEST_CASE(strStream, Input_CanRead)
WX_STREAM_TEST_CASE(strStream, Input_SeekI)
WX_STREAM_TEST_CASE(strStream, Input_TellI)
WX_STREAM_TEST_CASE(strStream, Input_Peek)
WX_STREAM_TEST_CASE(strStream, Input_Ungetch)

WX_STREAM_TEST_CASE(strStream, Output_PutC)
WX_STREAM_TEST_CASE(strStream, Output_Write)
WX_STREAM_TEST_CASE(strStream, Output_LastWrite)
// seeking currently not supported by output string stream
//WX_STREAM_TEST_CASE(strStream, Output_SeekO)
//WX_STREAM_TEST_CASE(strStream, Output_TellO)

// Other test specific for String stream test case.
WX_STREAM_TEST_CASE(strStream, Output_Check)

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
}
