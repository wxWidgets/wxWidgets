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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
        *(cbuff++) = (unsigned char)(((rand() * 255) / RAND_MAX));
    }
}

static void generateGibberish(void* buff, size_t len)
{
    static const unsigned char cb64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char *cbuff = (unsigned char *)buff;
    while(len--)
    {
        *(cbuff++) = cb64[((rand() * 64) / RAND_MAX)];
    }
}

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class Base64TestCase : public CppUnit::TestCase
{
public:
    Base64TestCase() { }

private:
    CPPUNIT_TEST_SUITE( Base64TestCase );
        CPPUNIT_TEST( EncodeDecodeEmpty );
        CPPUNIT_TEST( EncodeDecodeA );
        CPPUNIT_TEST( EncodeDecodeAB );
        CPPUNIT_TEST( EncodeDecodeABC );
        CPPUNIT_TEST( EncodeDecodeABCD );
        CPPUNIT_TEST( EncodeDecode0to255 );
        CPPUNIT_TEST( EncodeDecodePatternA );
        CPPUNIT_TEST( EncodeDecodePatternB );
        CPPUNIT_TEST( EncodeDecodePatternC );
        CPPUNIT_TEST( EncodeDecodeRandom );
        CPPUNIT_TEST( DecodeInvalid );
    CPPUNIT_TEST_SUITE_END();

    void EncodeDecodeEmpty();
    void EncodeDecodeA();
    void EncodeDecodeAB();
    void EncodeDecodeABC();
    void EncodeDecodeABCD();
    void EncodeDecode0to255();
    void EncodeDecodePatternA();
    void EncodeDecodePatternB();
    void EncodeDecodePatternC();
    void EncodeDecodeRandom();
    void DecodeInvalid();

    wxDECLARE_NO_COPY_CLASS(Base64TestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( Base64TestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( Base64TestCase, "Base64TestCase" );

void Base64TestCase::EncodeDecodeEmpty()
{
    char shouldBeEmpty[10];
    shouldBeEmpty[0] = '\0';
    size_t len = 10;

    CPPUNIT_ASSERT(wxBase64Encode(shouldBeEmpty, len, "", 0) != wxCONV_FAILED);
    CPPUNIT_ASSERT_EQUAL('\0', shouldBeEmpty[0]);

    CPPUNIT_ASSERT(wxBase64Decode(shouldBeEmpty, len, "") != wxCONV_FAILED);
    CPPUNIT_ASSERT_EQUAL('\0', shouldBeEmpty[0]);

    wxMemoryBuffer bufmt;
    wxString resultEmpty = wxBase64Encode(bufmt);
    CPPUNIT_ASSERT(resultEmpty.empty());

    bufmt = wxBase64Decode(resultEmpty);
    CPPUNIT_ASSERT_EQUAL(0, bufmt.GetDataLen());
}

void Base64TestCase::EncodeDecodeA()
{
    const wxString str = wxBase64Encode("A", 1);
    CPPUNIT_ASSERT_EQUAL(wxString("QQ=="), str);

    wxMemoryBuffer buf = wxBase64Decode(str);
    CPPUNIT_ASSERT_EQUAL(1, buf.GetDataLen());
    CPPUNIT_ASSERT_EQUAL('A', *(char *)buf.GetData());

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CPPUNIT_ASSERT_EQUAL( 1, wxBase64Decode(cbuf, 1, str) );
    CPPUNIT_ASSERT_EQUAL( 'A', cbuf[0] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[1] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[2] );
}

void Base64TestCase::EncodeDecodeAB()
{
    const wxString str = wxBase64Encode("AB", 2);
    CPPUNIT_ASSERT_EQUAL(wxString("QUI="), str);

    wxMemoryBuffer buf = wxBase64Decode(str);
    CPPUNIT_ASSERT_EQUAL(2, buf.GetDataLen());
    CPPUNIT_ASSERT_EQUAL('A', buf[0]);
    CPPUNIT_ASSERT_EQUAL('B', buf[1]);

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, wxBase64Decode(cbuf, 1, str) );
    CPPUNIT_ASSERT_EQUAL( 2, wxBase64Decode(cbuf, 2, str) );
    CPPUNIT_ASSERT_EQUAL( 'A', cbuf[0] );
    CPPUNIT_ASSERT_EQUAL( 'B', cbuf[1] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[2] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[3] );
}

void Base64TestCase::EncodeDecodeABC()
{
    const wxString str = wxBase64Encode("ABC", 3);
    CPPUNIT_ASSERT_EQUAL(wxString("QUJD"), str);

    wxMemoryBuffer buf = wxBase64Decode(str);
    CPPUNIT_ASSERT_EQUAL(3, buf.GetDataLen());
    CPPUNIT_ASSERT_EQUAL('A', buf[0]);
    CPPUNIT_ASSERT_EQUAL('B', buf[1]);
    CPPUNIT_ASSERT_EQUAL('C', buf[2]);

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, wxBase64Decode(cbuf, 2, str) );
    CPPUNIT_ASSERT_EQUAL( 3, wxBase64Decode(cbuf, 3, str) );
    CPPUNIT_ASSERT_EQUAL( 'A', cbuf[0] );
    CPPUNIT_ASSERT_EQUAL( 'B', cbuf[1] );
    CPPUNIT_ASSERT_EQUAL( 'C', cbuf[2] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[3] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[4] );
}

void Base64TestCase::EncodeDecodeABCD()
{
    const wxString str = wxBase64Encode("ABCD", 4);
    CPPUNIT_ASSERT_EQUAL(wxString("QUJDRA=="), str);

    wxMemoryBuffer buf = wxBase64Decode(str);
    CPPUNIT_ASSERT_EQUAL(4, buf.GetDataLen());
    CPPUNIT_ASSERT_EQUAL('A', buf[0]);
    CPPUNIT_ASSERT_EQUAL('B', buf[1]);
    CPPUNIT_ASSERT_EQUAL('C', buf[2]);
    CPPUNIT_ASSERT_EQUAL('D', buf[3]);

    char cbuf[10];
    memset(cbuf, (char)-1, sizeof(cbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, wxBase64Decode(cbuf, 3, str) );
    CPPUNIT_ASSERT_EQUAL( 4, wxBase64Decode(cbuf, 4, str) );
    CPPUNIT_ASSERT_EQUAL( 'A', cbuf[0] );
    CPPUNIT_ASSERT_EQUAL( 'B', cbuf[1] );
    CPPUNIT_ASSERT_EQUAL( 'C', cbuf[2] );
    CPPUNIT_ASSERT_EQUAL( 'D', cbuf[3] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[4] );
    CPPUNIT_ASSERT_EQUAL( (char)-1, cbuf[5] );
}

void Base64TestCase::EncodeDecode0to255()
{
    unsigned char buff[256];
    generatePatternedData(buff, 256, 0, 1);
    wxString str = wxBase64Encode(buff, 256);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);

    mbuff = wxBase64Decode(encoded0to255);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

void Base64TestCase::EncodeDecodePatternA()
{
    unsigned char buff[350];
    generatePatternedData(buff, 350, 24, 5, 3);
    wxString str = wxBase64Encode(buff, 350);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

void Base64TestCase::EncodeDecodePatternB()
{
    unsigned char buff[350];
    generatePatternedData(buff, 350, 0, 1, 1, 0xAA);
    wxString str = wxBase64Encode(buff, 350);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

void Base64TestCase::EncodeDecodePatternC()
{
    unsigned char buff[11];
    generatePatternedData(buff, 11, 1, 0, 2);
    wxString str = wxBase64Encode(buff, 11);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);
}

void Base64TestCase::EncodeDecodeRandom()
{
    size_t size = rand() * 3000 / RAND_MAX + 11;
    unsigned char *buff = new unsigned char[size];
    generateRandomData(buff, size);
    wxString str = wxBase64Encode(buff, size);
    wxMemoryBuffer mbuff = wxBase64Decode(str);
    CPPUNIT_ASSERT(memcmp(mbuff.GetData(), buff, mbuff.GetDataLen()) == 0);

    generateGibberish(buff, size);
    char *buff2 = new char[size];
    size_t realsize = size;
    CPPUNIT_ASSERT(wxBase64Decode(buff2, realsize, (char *)buff, size));
    CPPUNIT_ASSERT(wxBase64Encode(buff2, size, buff2, realsize));
    delete[] buff2;
    delete[] buff;
}

void Base64TestCase::DecodeInvalid()
{
    size_t rc, posErr;
    rc = wxBase64Decode(NULL, 0, "one two!", wxNO_LEN,
                        wxBase64DecodeMode_Strict, &posErr);
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, rc);
    CPPUNIT_ASSERT_EQUAL( 3, posErr );

    rc = wxBase64Decode(NULL, 0, "one two!", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, rc);
    CPPUNIT_ASSERT_EQUAL( 7, posErr );

    rc = wxBase64Decode(NULL, 0, "? QQ==", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, rc);
    CPPUNIT_ASSERT_EQUAL( 0, posErr );

    const size_t POS_INVALID = (size_t)-1;
    posErr = POS_INVALID;
    rc = wxBase64Decode(NULL, 0, " QQ==", wxNO_LEN,
                        wxBase64DecodeMode_SkipWS, &posErr);
    CPPUNIT_ASSERT_EQUAL( 1, rc );
    CPPUNIT_ASSERT_EQUAL( POS_INVALID, posErr );

    rc = wxBase64Decode(NULL, 0, "? QQ==", wxNO_LEN,
                        wxBase64DecodeMode_Relaxed, &posErr);
    CPPUNIT_ASSERT_EQUAL( 1, rc );
    CPPUNIT_ASSERT_EQUAL( POS_INVALID, posErr );

    CPPUNIT_ASSERT( !wxBase64Decode("wxGetApp()").GetDataLen() );
}

#endif // wxUSE_BASE64
