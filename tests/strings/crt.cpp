///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/crt.cpp
// Purpose:     Test for wx C runtime functions wrappers
// Author:      Vaclav Slavik
// Created:     2004-06-03
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vaclav Slavik
// Licence:     wxWindows licence
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

#include "wx/textfile.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const char *strMB = "hello, world";
static const wchar_t *strWC = L"hello, world";
static const wxString strWX("hello, world");

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class CrtTestCase : public CppUnit::TestCase
{
public:
    CrtTestCase() {}

private:
    CPPUNIT_TEST_SUITE( CrtTestCase );
        CPPUNIT_TEST( SetGetEnv );
#if wxUSE_UNICODE
        CPPUNIT_TEST( Strchr );
#endif // wxUSE_UNICODE
        CPPUNIT_TEST( Strcmp );
        CPPUNIT_TEST( Strspn );
        CPPUNIT_TEST( Strcspn );
        CPPUNIT_TEST( Strpbrk );
        CPPUNIT_TEST( Strnlen );
    CPPUNIT_TEST_SUITE_END();

    void SetGetEnv();
#if wxUSE_UNICODE
    void Strchr();
#endif // wxUSE_UNICODE
    void Strcmp();
    void Strspn();
    void Strcspn();
    void Strpbrk();
    void Strnlen();

    DECLARE_NO_COPY_CLASS(CrtTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CrtTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CrtTestCase, "CrtTestCase" );

void CrtTestCase::SetGetEnv()
{
#define TESTVAR_NAME wxT("WXTESTVAR")

    wxString val;
    wxSetEnv(TESTVAR_NAME, wxT("value"));
    CPPUNIT_ASSERT( wxGetEnv(TESTVAR_NAME, &val) );
    CPPUNIT_ASSERT_EQUAL( "value", val );
    CPPUNIT_ASSERT_EQUAL( "value", wxString(wxGetenv(TESTVAR_NAME)) );

    wxSetEnv(TESTVAR_NAME, wxT("something else"));
    CPPUNIT_ASSERT( wxGetEnv(TESTVAR_NAME, &val) );
    CPPUNIT_ASSERT_EQUAL( "something else", val );
    CPPUNIT_ASSERT_EQUAL( "something else", wxString(wxGetenv(TESTVAR_NAME)) );

    CPPUNIT_ASSERT( wxUnsetEnv(TESTVAR_NAME) );
    CPPUNIT_ASSERT( !wxGetEnv(TESTVAR_NAME, NULL) );
    CPPUNIT_ASSERT( !wxGetenv(TESTVAR_NAME) );

#undef TESTVAR_NAME
}

#if wxUSE_UNICODE
void CrtTestCase::Strchr()
{
    // test that searching for a wide character in a narrow string simply
    // doesn't find it but doesn't fail with an assert (#11487)
    const wxUniChar smiley = *wxString::FromUTF8("\xe2\x98\xba").begin();

    CPPUNIT_ASSERT( !wxStrchr("hello", smiley) );

    // but searching for an explicitly wide character does find it
    CPPUNIT_ASSERT( wxStrchr(wxString::FromUTF8(":-) == \xe2\x98\xba"),
                    static_cast<wchar_t>(smiley)) );
}
#endif // wxUSE_UNICODE

void CrtTestCase::Strcmp()
{
    // this code tests if all possible ways of calling wxStrcmp() compile:
    const char * const char1 = "first";
    const wchar_t * const wchar1 = L"first";
    wxString str1("first");
    wxCStrData cstr1(str1.c_str());
    wxCharBuffer charbuf1(char1);
    wxWCharBuffer wcharbuf1(wchar1);

    const char * const char2 = "last";
    const wchar_t * const wchar2 = L"last";
    wxString str2("last");
    wxCStrData cstr2(str2.c_str());
    wxCharBuffer charbuf2(char2);
    wxWCharBuffer wcharbuf2(wchar2);

    CPPUNIT_ASSERT( wxStrcmp(char1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(char1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(char1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(char1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(char1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(char1, wcharbuf2) < 0 );

    CPPUNIT_ASSERT( wxStrcmp(wchar1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wchar1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wchar1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wchar1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wchar1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wchar1, wcharbuf2) < 0 );

    CPPUNIT_ASSERT( wxStrcmp(str1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(str1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(str1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(str1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(str1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(str1, wcharbuf2) < 0 );

    CPPUNIT_ASSERT( wxStrcmp(cstr1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(cstr1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(cstr1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(cstr1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(cstr1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(cstr1, wcharbuf2) < 0 );

    CPPUNIT_ASSERT( wxStrcmp(charbuf1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(charbuf1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(charbuf1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(charbuf1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(charbuf1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(charbuf1, wcharbuf2) < 0 );

    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, char2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, wchar2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, str2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, cstr2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, charbuf2) < 0 );
    CPPUNIT_ASSERT( wxStrcmp(wcharbuf1, wcharbuf2) < 0 );
}

void CrtTestCase::Strspn()
{
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strMB, "xyz") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWC, "xyz") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWX, "xyz") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strMB, L"xyz") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWC, L"xyz") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWX, L"xyz") );

    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrspn(strMB, "hleo") );
    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrspn(strWC, "hleo") );
    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrspn(strWX, "hleo") );

    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strMB, "ld") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWC, "ld") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrspn(strWX, "ld") );

    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrspn(strMB, strWC) );
    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrspn(strWC, strWX) );
    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrspn(strWX, strMB) );
}

void CrtTestCase::Strcspn()
{
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strMB, strWX) );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strWC, strMB) );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strWX, strWC) );

    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrcspn(strMB, ", ") );
    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrcspn(strWC, ", ") );
    CPPUNIT_ASSERT_EQUAL( (size_t)5, wxStrcspn(strWX, ", ") );

    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strMB, "hel") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strWC, "hel") );
    CPPUNIT_ASSERT_EQUAL( (size_t)0, wxStrcspn(strWX, "hel") );

    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrcspn(strMB, "xy") );
    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrcspn(strWC, "xy") );
    CPPUNIT_ASSERT_EQUAL( strWX.length(), wxStrcspn(strWX, "xy") );
}

void CrtTestCase::Strpbrk()
{
    const wxString s(", ");

    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strMB,  ", ") );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWC, L", ") );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX,  ", ") );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWX, L", ") );

    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strMB, s) );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWC, s) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX, s) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX.c_str(), s) );

    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strMB, s.c_str()) );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWC, s.c_str()) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX, s.c_str()) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX.c_str(), s.c_str()) );

    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strMB, s.mb_str()) );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWC, s.wc_str()) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX, s.mb_str()) );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWX, s.wc_str()) );
    CPPUNIT_ASSERT_EQUAL(  ',', *wxStrpbrk(strWX.c_str(), s.mb_str()) );
    CPPUNIT_ASSERT_EQUAL( L',', *wxStrpbrk(strWX.c_str(), s.wc_str()) );

    CPPUNIT_ASSERT( !wxStrpbrk(strWX, "xyz") );
    CPPUNIT_ASSERT( !wxStrpbrk(strWX.c_str(), L"xyz") );
}

void CrtTestCase::Strnlen()
{
    // other misc tests for wxStrnlen(const char*, size_t)

    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen("", 0) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen("", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen("1234", 0) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)2, wxStrnlen("1234", 2) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, wxStrnlen("1234", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)7, wxStrnlen("1234567", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, wxStrnlen("12345678", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, wxStrnlen("123456789", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)9, wxStrnlen("123456789", 12) );

    // other misc tests for wxStrnlen(const wchar_t*, size_t)

    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen(L"", 0) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen(L"", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, wxStrnlen(L"1234", 0) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)2, wxStrnlen(L"1234", 2) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, wxStrnlen(L"1234", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)7, wxStrnlen(L"1234567", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, wxStrnlen(L"12345678", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, wxStrnlen(L"123456789", 8) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)9, wxStrnlen(L"123456789", 12) );

    // wxStrlen() is only for NULL-terminated strings:
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, wxStrnlen("1234" "\0" "78", 12) );
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, wxStrnlen(L"1234" L"\0" L"5678", 12) );
}



