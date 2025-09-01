///////////////////////////////////////////////////////////////////////////////
// Name:        tests/base64/base64.cpp
// Purpose:     wxBase64Encode/Decode unit test
// Author:      Charles Reimers
// Created:     2007-06-22
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#if wxUSE_BASE64

#include "wx/base64.h"

static const char encoded0to255[] =
    "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIj"
    "JCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZH"
    "SElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWpr"
    "bG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
    "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKz"
    "tLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX"
    "2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7"
    "/P3+/w==";

static void
generatePatternedData(void* buff, size_t len, unsigned char startVal,
                      unsigned char addVal, unsigned char multVal = 1,
                      unsigned char xorMask = 0, unsigned char andMask = 255)
{
    unsigned char *cbuff = (unsigned char *)buff;
    unsigned char curval = startVal;
    while(len--)
    {
        *(cbuff++) = curval;
        curval = (((curval + addVal) * multVal) ^ xorMask) & andMask;
    }
}

static void generateRandomData(void* buff, size_t len)
{
    unsigned char *cbuff = (unsigned char *)buff;
    while(len--)
    {
        *(cbuff++) = (unsigned char)((rand() / (float)RAND_MAX * 255.) + 1);
    }
}

static void generateGibberish(void* buff, size_t len)
{
    static const unsigned char cb64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char *cbuff = (unsigned char *)buff;
    while(len--)
    {
        *(cbuff++) = cb64[(size_t)((rand() / (float)RAND_MAX * 64.) + 1)];
    }
}

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

TEST_CASE("Encode Decode Empty", "[base64]")
{
    char shouldBeEmpty[10];
    shouldBeEmpty[0] = '\0';
    size_t len = 10;

    CHECK( wxBase64Encode(shouldBeEmpty, len, "", 0) != wxCONV_FAILED );
    CHECK( shouldBeEmpty[0] == '\0' );

    CHECK( wxBase64Decode(shouldBeEmpty, len, "") != wxCONV_FAILED );
    CHECK( shouldBeEmpty[0] == '\0' );

    wxMemoryBuffer bufmt;
    wxString resultEmpty = wxBase64Encode(bufmt);
    CHECK( resultEmpty.empty() );

    bufmt = wxBase64Decode(resultEmpty);
    CHECK( bufmt.GetDataLen() == 0 );
}

TEST_CASE("Encode Decode A", "[base64]")
{
    const wxString str = wxBase64Encode("A", 1);
    CHECK( str == wxString("QQ==") );

    wxMemoryBuffer buf = wxBase64Decode(str);
    CHECK(buf.GetDataLen() == 1);
    CHECK( *(char*)buf.GetData() == 'A' );

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CHECK(wxBase64Decode(cbuf, 1, str) == 1);
    CHECK( cbuf[0] == 'A' );
    CHECK( cbuf[1] == (char)-1 );
    CHECK( cbuf[2] == (char)-1 );
}

TEST_CASE("Encode Decode AB", "[base64]")
{
    const wxString str = wxBase64Encode("AB", 2);
    CHECK( str == wxString("QUI=") );

    wxMemoryBuffer buf = wxBase64Decode(str);
    CHECK( buf.GetDataLen() == 2 );
    CHECK( buf[0] == 'A' );
    CHECK( buf[1] == 'B' );

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CHECK( wxBase64Decode(cbuf, 1, str) == wxCONV_FAILED);
    CHECK( wxBase64Decode(cbuf, 2, str) == 2 );
    CHECK( cbuf[0] == 'A' );
    CHECK( cbuf[1] == 'B' );
    CHECK( cbuf[2] == (char)-1 );
    CHECK( cbuf[3] == (char)-1 );
}

TEST_CASE("Encode Decode ABC", "[base64]")
{
    const wxString str = wxBase64Encode("ABC", 3);
    CHECK( str == wxString("QUJD") );

    wxMemoryBuffer buf = wxBase64Decode(str);
    CHECK( buf.GetDataLen() == 3 );
    CHECK( buf[0] == 'A' );
    CHECK( buf[1] == 'B' );
    CHECK( buf[2] == 'C' );

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CHECK( wxBase64Decode(cbuf, 2, str) == wxCONV_FAILED );
    CHECK( wxBase64Decode(cbuf, 3, str) == 3 );
    CHECK( cbuf[0] == 'A' );
    CHECK( cbuf[1] == 'B' );
    CHECK( cbuf[2] == 'C' );
    CHECK( cbuf[3] == (char)-1 );
    CHECK( cbuf[4] == (char)-1 );
}

TEST_CASE("Encode Decode ABCD", "[base64]")
{
    const wxString str = wxBase64Encode("ABCD", 4);
    CHECK( str == wxString("QUJDRA==") );

    wxMemoryBuffer buf = wxBase64Decode(str);
    CHECK( buf.GetDataLen() == 4 );
    CHECK( buf[0] == 'A' );
    CHECK( buf[1] == 'B' );
    CHECK( buf[2] == 'C' );
    CHECK( buf[3] == 'D' );

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CHECK( wxBase64Decode(cbuf, 3, str) == wxCONV_FAILED );
    CHECK( wxBase64Decode(cbuf, 4, str) == 4 );
    CHECK( cbuf[0] == 'A' );
    CHECK( cbuf[1] == 'B' );
    CHECK( cbuf[2] == 'C' );
    CHECK( cbuf[3] == 'D' );
    CHECK( cbuf[4] == (char)-1 );
    CHECK( cbuf[5] == (char)-1 );
}

TEST_CASE("Encode Decode 0 to 255", "[base64]")
{
    unsigned char buff[256];
    generatePatternedData(buff, 256, 0, 1);
    wxString str = wxBase64Encode(buff, 256);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);

    mbuff = wxBase64Decode(encoded0to255);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

TEST_CASE("Encode Decode Pattern A", "[base64]")
{
    unsigned char buff[350];
    generatePatternedData(buff, 350, 24, 5, 3);
    wxString str = wxBase64Encode(buff, 350);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

TEST_CASE("Encode Decode Pattern B", "[base64]")
{
    unsigned char buff[350];
    generatePatternedData(buff, 350, 0, 1, 1, 0xAA);
    wxString str = wxBase64Encode(buff, 350);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

TEST_CASE("Encode Decode PatternC", "[base64]")
{
    unsigned char buff[11];
    generatePatternedData(buff, 11, 1, 0, 2);
    wxString str = wxBase64Encode(buff, 11);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

TEST_CASE("Encode Decode Random", "[base64]")
{
    size_t size = (size_t)(rand() / (float)RAND_MAX * 3000. + 11);
    unsigned char *buff = new unsigned char[size];
    generateRandomData(buff, size);
    wxString str = wxBase64Encode(buff, size);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CHECK(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);

    generateGibberish(buff, size);
    char *buff2 = new char[size];
    size_t realsize = size;
    CHECK(wxBase64Decode(buff2, realsize, (char *)buff, size));
    CHECK(wxBase64Encode(buff2, size, buff2, realsize));
    delete[] buff2;
    delete[] buff;
}

TEST_CASE("Decode Invalid", "[base64]")
{
    size_t rc, posErr;
    rc = wxBase64Decode(nullptr, 0, "one two!", wxNO_LEN,
                        wxBase64DecodeMode_Strict, &posErr);
    CHECK( rc == wxCONV_FAILED );
    CHECK( posErr == 3 );

    rc = wxBase64Decode(nullptr, 0, "one two!", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CHECK( rc == wxCONV_FAILED );
    CHECK( posErr == 7 );

    rc = wxBase64Decode(nullptr, 0, "? QQ==", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CHECK( rc == wxCONV_FAILED );
    CHECK( posErr == 0 );

    const size_t POS_INVALID = (size_t)-1;
    posErr = POS_INVALID;
    rc = wxBase64Decode(nullptr, 0, " QQ==", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CHECK( rc == 1 );
    CHECK( posErr == POS_INVALID );

    rc = wxBase64Decode(nullptr, 0, "? QQ==", wxNO_LEN,
                        wxBase64DecodeMode_Relaxed, &posErr);
    CHECK( rc == 1);
    CHECK( posErr == POS_INVALID );

    CHECK( !wxBase64Decode("wxGetApp()").GetDataLen() );
}

#endif // wxUSE_BASE64
