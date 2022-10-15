///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/crt.cpp
// Purpose:     Test for wx C runtime functions wrappers
// Author:      Vaclav Slavik
// Created:     2004-06-03
// Copyright:   (c) 2004 Vaclav Slavik
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


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
// tests
// ----------------------------------------------------------------------------

TEST_CASE("CRT::SetGetEnv", "[crt][getenv][setenv]")
{
#define TESTVAR_NAME wxT("WXTESTVAR")

    wxString val;
    wxSetEnv(TESTVAR_NAME, wxT("value"));
    CHECK( wxGetEnv(TESTVAR_NAME, &val) );
    CHECK( val == "value" );
    CHECK( wxString(wxGetenv(TESTVAR_NAME)) == "value" );

    wxSetEnv(TESTVAR_NAME, wxT("something else"));
    CHECK( wxGetEnv(TESTVAR_NAME, &val) );
    CHECK( val == "something else" );
    CHECK( wxString(wxGetenv(TESTVAR_NAME)) == "something else" );

    CHECK( wxUnsetEnv(TESTVAR_NAME) );
    CHECK( !wxGetEnv(TESTVAR_NAME, nullptr) );
    CHECK( !wxGetenv(TESTVAR_NAME) );

#undef TESTVAR_NAME
}

#if wxUSE_UNICODE
TEST_CASE("CRT::Strchr", "[crt][strchr]")
{
    // test that searching for a wide character in a narrow string simply
    // doesn't find it but doesn't fail with an assert (#11487)
    const wxUniChar smiley = *wxString::FromUTF8("\xe2\x98\xba").begin();

    CHECK( !wxStrchr("hello", smiley) );

    // but searching for an explicitly wide character does find it
    CHECK( wxStrchr(wxString::FromUTF8(":-) == \xe2\x98\xba"),
                    static_cast<wchar_t>(smiley)) );
}
#endif // wxUSE_UNICODE

TEST_CASE("CRT::Strcmp", "[crt][strcmp]")
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

    CHECK( wxStrcmp(char1, char2) < 0 );
    CHECK( wxStrcmp(char1, wchar2) < 0 );
    CHECK( wxStrcmp(char1, str2) < 0 );
    CHECK( wxStrcmp(char1, cstr2) < 0 );
    CHECK( wxStrcmp(char1, charbuf2) < 0 );
    CHECK( wxStrcmp(char1, wcharbuf2) < 0 );

    CHECK( wxStrcmp(wchar1, char2) < 0 );
    CHECK( wxStrcmp(wchar1, wchar2) < 0 );
    CHECK( wxStrcmp(wchar1, str2) < 0 );
    CHECK( wxStrcmp(wchar1, cstr2) < 0 );
    CHECK( wxStrcmp(wchar1, charbuf2) < 0 );
    CHECK( wxStrcmp(wchar1, wcharbuf2) < 0 );

    CHECK( wxStrcmp(str1, char2) < 0 );
    CHECK( wxStrcmp(str1, wchar2) < 0 );
    CHECK( wxStrcmp(str1, str2) < 0 );
    CHECK( wxStrcmp(str1, cstr2) < 0 );
    CHECK( wxStrcmp(str1, charbuf2) < 0 );
    CHECK( wxStrcmp(str1, wcharbuf2) < 0 );

    CHECK( wxStrcmp(cstr1, char2) < 0 );
    CHECK( wxStrcmp(cstr1, wchar2) < 0 );
    CHECK( wxStrcmp(cstr1, str2) < 0 );
    CHECK( wxStrcmp(cstr1, cstr2) < 0 );
    CHECK( wxStrcmp(cstr1, charbuf2) < 0 );
    CHECK( wxStrcmp(cstr1, wcharbuf2) < 0 );

    CHECK( wxStrcmp(charbuf1, char2) < 0 );
    CHECK( wxStrcmp(charbuf1, wchar2) < 0 );
    CHECK( wxStrcmp(charbuf1, str2) < 0 );
    CHECK( wxStrcmp(charbuf1, cstr2) < 0 );
    CHECK( wxStrcmp(charbuf1, charbuf2) < 0 );
    CHECK( wxStrcmp(charbuf1, wcharbuf2) < 0 );

    CHECK( wxStrcmp(wcharbuf1, char2) < 0 );
    CHECK( wxStrcmp(wcharbuf1, wchar2) < 0 );
    CHECK( wxStrcmp(wcharbuf1, str2) < 0 );
    CHECK( wxStrcmp(wcharbuf1, cstr2) < 0 );
    CHECK( wxStrcmp(wcharbuf1, charbuf2) < 0 );
    CHECK( wxStrcmp(wcharbuf1, wcharbuf2) < 0 );
}

TEST_CASE("CRT::Strspn", "[crt][strspn]")
{
    CHECK( wxStrspn(strMB, "xyz") == 0 );
    CHECK( wxStrspn(strWC, "xyz") == 0 );
    CHECK( wxStrspn(strWX, "xyz") == 0 );
    CHECK( wxStrspn(strMB, L"xyz") == 0 );
    CHECK( wxStrspn(strWC, L"xyz") == 0 );
    CHECK( wxStrspn(strWX, L"xyz") == 0 );

    CHECK( wxStrspn(strMB, "hleo") == 5 );
    CHECK( wxStrspn(strWC, "hleo") == 5 );
    CHECK( wxStrspn(strWX, "hleo") == 5 );

    CHECK( wxStrspn(strMB, "ld") == 0 );
    CHECK( wxStrspn(strWC, "ld") == 0 );
    CHECK( wxStrspn(strWX, "ld") == 0 );

    CHECK( wxStrspn(strMB, strWC) == strWX.length() );
    CHECK( wxStrspn(strWC, strWX) == strWX.length() );
    CHECK( wxStrspn(strWX, strMB) == strWX.length() );
}

TEST_CASE("CRT::Strcspn", "[crt][strcspn]")
{
    CHECK( wxStrcspn(strMB, strWX) == 0 );
    CHECK( wxStrcspn(strWC, strMB) == 0 );
    CHECK( wxStrcspn(strWX, strWC) == 0 );

    CHECK( wxStrcspn(strMB, ", ") == 5 );
    CHECK( wxStrcspn(strWC, ", ") == 5 );
    CHECK( wxStrcspn(strWX, ", ") == 5 );

    CHECK( wxStrcspn(strMB, "hel") == 0 );
    CHECK( wxStrcspn(strWC, "hel") == 0 );
    CHECK( wxStrcspn(strWX, "hel") == 0 );

    CHECK( wxStrcspn(strMB, "xy") == strWX.length() );
    CHECK( wxStrcspn(strWC, "xy") == strWX.length() );
    CHECK( wxStrcspn(strWX, "xy") == strWX.length() );
}

TEST_CASE("CRT::Strpbrk", "[crt][strpbrk]")
{
    const wxString s(", ");

    CHECK( *wxStrpbrk(strMB,  ", ") ==  ',' );
    CHECK( *wxStrpbrk(strWC, L", ") == L',' );
    CHECK( *wxStrpbrk(strWX,  ", ") ==  ',' );
    CHECK( *wxStrpbrk(strWX, L", ") == L',' );

    CHECK( *wxStrpbrk(strMB, s) ==  ',' );
    CHECK( *wxStrpbrk(strWC, s) == L',' );
    CHECK( *wxStrpbrk(strWX, s) ==  ',' );
    CHECK( *wxStrpbrk(strWX.c_str(), s) == ',' );

    CHECK( *wxStrpbrk(strMB, s.c_str()) ==  ',' );
    CHECK( *wxStrpbrk(strWC, s.c_str()) == L',' );
    CHECK( *wxStrpbrk(strWX, s.c_str()) ==  ',' );
    CHECK( *wxStrpbrk(strWX.c_str(), s.c_str()) == ',' );

    CHECK( *wxStrpbrk(strMB, s.mb_str()) ==  ',' );
    CHECK( *wxStrpbrk(strWC, s.wc_str()) == L',' );
    CHECK( *wxStrpbrk(strWX, s.mb_str()) ==  ',' );
    CHECK( *wxStrpbrk(strWX, s.wc_str()) == L',' );
    CHECK( *wxStrpbrk(strWX.c_str(), s.mb_str()) ==  ',' );
    CHECK( *wxStrpbrk(strWX.c_str(), s.wc_str()) == L',' );

    CHECK( !wxStrpbrk(strWX, "xyz") );
    CHECK( !wxStrpbrk(strWX.c_str(), L"xyz") );
}

TEST_CASE("CRT::Strnlen", "[crt][strnlen]")
{
    // other misc tests for wxStrnlen(const char*, size_t)

    CHECK( wxStrnlen("", 0) == 0 );
    CHECK( wxStrnlen("", 8) == 0 );
    CHECK( wxStrnlen("1234", 0) == 0 );
    CHECK( wxStrnlen("1234", 2) == 2 );
    CHECK( wxStrnlen("1234", 8) == 4 );
    CHECK( wxStrnlen("1234567", 8) == 7 );
    CHECK( wxStrnlen("12345678", 8) == 8 );
    CHECK( wxStrnlen("123456789", 8) == 8 );
    CHECK( wxStrnlen("123456789", 12) == 9 );

    // other misc tests for wxStrnlen(const wchar_t*, size_t)

    CHECK( wxStrnlen(L"", 0) == 0 );
    CHECK( wxStrnlen(L"", 8) == 0 );
    CHECK( wxStrnlen(L"1234", 0) == 0 );
    CHECK( wxStrnlen(L"1234", 2) == 2 );
    CHECK( wxStrnlen(L"1234", 8) == 4 );
    CHECK( wxStrnlen(L"1234567", 8) == 7 );
    CHECK( wxStrnlen(L"12345678", 8) == 8 );
    CHECK( wxStrnlen(L"123456789", 8) == 8 );
    CHECK( wxStrnlen(L"123456789", 12) == 9 );

    // wxStrlen() is only for null-terminated strings:
    CHECK( wxStrnlen("1234" "\0" "78", 12) == 4 );
    CHECK( wxStrnlen(L"1234" L"\0" L"5678", 12) == 4 );
}

TEST_CASE("CRT::Strtox", "[crt][strtod][strtol]")
{
    const wxString s = "123@";
    const double d = 123.0;
    const long l = 123;

    SECTION("char")
    {
        char* end = nullptr;
        CHECK( wxStrtod(s, &end) == d );
        REQUIRE( end );
        CHECK( *end == '@' );

        CHECK( wxStrtol(s, &end, 10) == l );
        REQUIRE( end );
        CHECK( *end == '@' );
    }

    SECTION("wchar_t")
    {
        wchar_t* end = nullptr;
        CHECK( wxStrtod(s, &end) == d );
        REQUIRE( end );
        CHECK( *end == L'@' );

        CHECK( wxStrtol(s, &end, 10) == l );
        REQUIRE( end );
        CHECK( *end == L'@' );
    }

    SECTION("other")
    {
        CHECK( wxStrtod(s, 0) == d );
#ifdef wxHAS_NULLPTR_T
        CHECK( wxStrtod(s, nullptr) == d );
        CHECK( wxStrtol(s, nullptr, 10) == l );
#endif
    }
}
