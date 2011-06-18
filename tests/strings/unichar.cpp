///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/unichar.cpp
// Purpose:     Test for wxUniChar class
// Author:      Vaclav Slavik
// Created:     2007-03-11
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
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

#include "wx/string.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class UniCharTestCase : public CppUnit::TestCase
{
public:
    UniCharTestCase() {}

private:
    CPPUNIT_TEST_SUITE( UniCharTestCase );
        CPPUNIT_TEST( CharCompare );
        CPPUNIT_TEST( CharCompareIntl );
        CPPUNIT_TEST( StringCompare );
#if wxUSE_UNICODE
        CPPUNIT_TEST( StringCompareIntl );
#endif // wxUSE_UNICODE
    CPPUNIT_TEST_SUITE_END();

    void CharCompare();
    void CharCompareIntl();
    void StringCompare();
    void StringCompareIntl();

    DECLARE_NO_COPY_CLASS(UniCharTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( UniCharTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( UniCharTestCase, "UniCharTestCase" );

void UniCharTestCase::CharCompare()
{
    wxUniChar a('a');
    wxUniChar b('b');

    CPPUNIT_ASSERT( a == a );
    CPPUNIT_ASSERT( a == 'a' );
    CPPUNIT_ASSERT( a == wxT('a') );
    CPPUNIT_ASSERT( a == wxUniChar('a') );
    CPPUNIT_ASSERT( a == wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( a != b );
    CPPUNIT_ASSERT( a != 'b' );
    CPPUNIT_ASSERT( a != wxT('b') );
    CPPUNIT_ASSERT( a != wxUniChar('b') );
    CPPUNIT_ASSERT( a != wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( a < b );
    CPPUNIT_ASSERT( a < 'b' );
    CPPUNIT_ASSERT( a < wxT('b') );
    CPPUNIT_ASSERT( a < wxUniChar('b') );
    CPPUNIT_ASSERT( a < wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( a <= b );
    CPPUNIT_ASSERT( a <= 'b' );
    CPPUNIT_ASSERT( a <= wxT('b') );
    CPPUNIT_ASSERT( a <= wxUniChar('b') );
    CPPUNIT_ASSERT( a <= wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( a <= a );
    CPPUNIT_ASSERT( a <= 'a' );
    CPPUNIT_ASSERT( a <= wxT('a') );
    CPPUNIT_ASSERT( a <= wxUniChar('a') );
    CPPUNIT_ASSERT( a <= wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( b > a );
    CPPUNIT_ASSERT( b > 'a' );
    CPPUNIT_ASSERT( b > wxT('a') );
    CPPUNIT_ASSERT( b > wxUniChar('a') );
    CPPUNIT_ASSERT( b > wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( b >= a );
    CPPUNIT_ASSERT( b >= 'a' );
    CPPUNIT_ASSERT( b >= wxT('a') );
    CPPUNIT_ASSERT( b >= wxUniChar('a') );
    CPPUNIT_ASSERT( b >= wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( b >= b );
    CPPUNIT_ASSERT( b >= 'b' );
    CPPUNIT_ASSERT( b >= wxT('b') );
    CPPUNIT_ASSERT( b >= wxUniChar('b') );
    CPPUNIT_ASSERT( b >= wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( b - a == 1 );
    CPPUNIT_ASSERT( a - b == -1 );
}


#define CYRILLIC_SMALL_LETTER_YU  ((wchar_t)0x044E)
#define ARABIC_LETTER_NOON        ((wchar_t)0x0646)

void UniCharTestCase::CharCompareIntl()
{
    wxUniChar a(CYRILLIC_SMALL_LETTER_YU);
    wxUniChar b(ARABIC_LETTER_NOON);

    CPPUNIT_ASSERT( a == a );
    CPPUNIT_ASSERT( a == CYRILLIC_SMALL_LETTER_YU );
    CPPUNIT_ASSERT( a == wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CPPUNIT_ASSERT( a != b );
    CPPUNIT_ASSERT( a != ARABIC_LETTER_NOON );
    CPPUNIT_ASSERT( a != wxUniChar(ARABIC_LETTER_NOON) );

    CPPUNIT_ASSERT( a < b );
    CPPUNIT_ASSERT( a < ARABIC_LETTER_NOON );
    CPPUNIT_ASSERT( a < wxUniChar(ARABIC_LETTER_NOON) );

    CPPUNIT_ASSERT( a <= b );
    CPPUNIT_ASSERT( a <= ARABIC_LETTER_NOON );
    CPPUNIT_ASSERT( a <= wxUniChar(ARABIC_LETTER_NOON) );

    CPPUNIT_ASSERT( a <= a );
    CPPUNIT_ASSERT( a <= CYRILLIC_SMALL_LETTER_YU );
    CPPUNIT_ASSERT( a <= wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CPPUNIT_ASSERT( b > a );
    CPPUNIT_ASSERT( b > CYRILLIC_SMALL_LETTER_YU );
    CPPUNIT_ASSERT( b > wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CPPUNIT_ASSERT( b >= a );
    CPPUNIT_ASSERT( b >= CYRILLIC_SMALL_LETTER_YU );
    CPPUNIT_ASSERT( b >= wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CPPUNIT_ASSERT( b >= b );
    CPPUNIT_ASSERT( b >= ARABIC_LETTER_NOON );
    CPPUNIT_ASSERT( b >= wxUniChar(ARABIC_LETTER_NOON) );

    CPPUNIT_ASSERT( b - a == 504 );
    CPPUNIT_ASSERT( a - b == -504 );
}

void UniCharTestCase::StringCompare()
{
    // test string comparison with chars
    wxString sa = "a";
    const wxString sb = "b";
    char c1 = 'a';
    wchar_t c2 = wxT('a');
    wxUniChar c3 = 'a';

    CPPUNIT_ASSERT( sa == 'a');
    CPPUNIT_ASSERT( 'a' == sa);
    CPPUNIT_ASSERT( sb != 'a');
    CPPUNIT_ASSERT( 'a' != sb);

    CPPUNIT_ASSERT( sa == c1);
    CPPUNIT_ASSERT( c1 == sa);
    CPPUNIT_ASSERT( sb != c1);
    CPPUNIT_ASSERT( c1 != sb);

    CPPUNIT_ASSERT( sa == c2);
    CPPUNIT_ASSERT( c2 == sa);
    CPPUNIT_ASSERT( sb != c2);
    CPPUNIT_ASSERT( c2 != sb);

    CPPUNIT_ASSERT( sa == c3);
    CPPUNIT_ASSERT( c3 == sa);
    CPPUNIT_ASSERT( sb != c3);
    CPPUNIT_ASSERT( c3 != sb);

    // test wxUniCharRef:
    CPPUNIT_ASSERT( sa == sa[0]);
    CPPUNIT_ASSERT( sa[0] == sa);
    CPPUNIT_ASSERT( sb != sa[0]);
    CPPUNIT_ASSERT( sa[0] != sb);

    // test const version of operator[] (returns wxUniChar, not wxUniCharRef):
    CPPUNIT_ASSERT( sb == sb[0]);
    CPPUNIT_ASSERT( sb[0] == sb);
    CPPUNIT_ASSERT( sa != sb[0]);
    CPPUNIT_ASSERT( sb[0] != sa);
}

#if wxUSE_UNICODE
void UniCharTestCase::StringCompareIntl()
{
    // test string comparison with chars
    wxString sa = CYRILLIC_SMALL_LETTER_YU;
    const wxString sb = ARABIC_LETTER_NOON;
    wchar_t c2 = CYRILLIC_SMALL_LETTER_YU;
    wxUniChar c3 = CYRILLIC_SMALL_LETTER_YU;

    CPPUNIT_ASSERT( sa == CYRILLIC_SMALL_LETTER_YU);
    CPPUNIT_ASSERT( CYRILLIC_SMALL_LETTER_YU == sa);
    CPPUNIT_ASSERT( sb != CYRILLIC_SMALL_LETTER_YU);
    CPPUNIT_ASSERT( CYRILLIC_SMALL_LETTER_YU != sb);

    CPPUNIT_ASSERT( sa == c2);
    CPPUNIT_ASSERT( c2 == sa);
    CPPUNIT_ASSERT( sb != c2);
    CPPUNIT_ASSERT( c2 != sb);

    CPPUNIT_ASSERT( sa == c3);
    CPPUNIT_ASSERT( c3 == sa);
    CPPUNIT_ASSERT( sb != c3);
    CPPUNIT_ASSERT( c3 != sb);

    // test wxUniCharRef:
    CPPUNIT_ASSERT( sa == sa[0]);
    CPPUNIT_ASSERT( sa[0] == sa);
    CPPUNIT_ASSERT( sb != sa[0]);
    CPPUNIT_ASSERT( sa[0] != sb);

    // test const version of operator[] (returns wxUniChar, not wxUniCharRef):
    CPPUNIT_ASSERT( sb == sb[0]);
    CPPUNIT_ASSERT( sb[0] == sb);
    CPPUNIT_ASSERT( sa != sb[0]);
    CPPUNIT_ASSERT( sb[0] != sa);
}
#endif // wxUSE_UNICODE
