///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/unichar.cpp
// Purpose:     Test for wxUniChar class
// Author:      Vaclav Slavik
// Created:     2007-03-11
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/string.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("UniChar::CharCompare", "[unichar]")
{
    wxUniChar a('a');
    wxUniChar b('b');

    CHECK( a == a );
    CHECK( a == 'a' );
    CHECK( a == wxT('a') );
    CHECK( a == wxUniChar('a') );
    CHECK( a == wxUniChar(wxT('a')) );

    CHECK( a == a );
    CHECK( 'a' == a );
    CHECK( wxT('a') == a );
    CHECK( wxUniChar('a') == a );
    CHECK( wxUniChar(wxT('a')) == a );

    CHECK( a != b );
    CHECK( a != 'b' );
    CHECK( a != wxT('b') );
    CHECK( a != wxUniChar('b') );
    CHECK( a != wxUniChar(wxT('b')) );

    CHECK( b != a );
    CHECK( 'b' != a );
    CHECK( wxT('b') != a );
    CHECK( wxUniChar('b') != a );
    CHECK( wxUniChar(wxT('b')) != a );

    CHECK( a < b );
    CHECK( a < 'b' );
    CHECK( a < wxT('b') );
    CHECK( a < wxUniChar('b') );
    CHECK( a < wxUniChar(wxT('b')) );

    CHECK( b > a );
    CHECK( 'b' > a );
    CHECK( wxT('b') > a );
    CHECK( wxUniChar('b') > a );
    CHECK( wxUniChar(wxT('b')) > a );

    CHECK( a <= b );
    CHECK( a <= 'b' );
    CHECK( a <= wxT('b') );
    CHECK( a <= wxUniChar('b') );
    CHECK( a <= wxUniChar(wxT('b')) );

    CHECK( b >= a );
    CHECK( 'b' >= a );
    CHECK( wxT('b') >= a );
    CHECK( wxUniChar('b') >= a );
    CHECK( wxUniChar(wxT('b')) >= a );

    CHECK( a <= a );
    CHECK( a <= 'a' );
    CHECK( a <= wxT('a') );
    CHECK( a <= wxUniChar('a') );
    CHECK( a <= wxUniChar(wxT('a')) );

    CHECK( a >= a );
    CHECK( 'a' >= a );
    CHECK( wxT('a') >= a );
    CHECK( wxUniChar('a') >= a );
    CHECK( wxUniChar(wxT('a')) >= a );

    CHECK( b > a );
    CHECK( b > 'a' );
    CHECK( b > wxT('a') );
    CHECK( b > wxUniChar('a') );
    CHECK( b > wxUniChar(wxT('a')) );

    CHECK( a < b );
    CHECK( 'a' < b );
    CHECK( wxT('a') < b );
    CHECK( wxUniChar('a') < b );
    CHECK( wxUniChar(wxT('a')) < b );

    CHECK( b >= a );
    CHECK( b >= 'a' );
    CHECK( b >= wxT('a') );
    CHECK( b >= wxUniChar('a') );
    CHECK( b >= wxUniChar(wxT('a')) );

    CHECK( a <= b );
    CHECK( 'a' <= b );
    CHECK( wxT('a') <= b );
    CHECK( wxUniChar('a') <= b );
    CHECK( wxUniChar(wxT('a')) <= b );

    CHECK( b >= b );
    CHECK( b >= 'b' );
    CHECK( b >= wxT('b') );
    CHECK( b >= wxUniChar('b') );
    CHECK( b >= wxUniChar(wxT('b')) );

    CHECK( b <= b );
    CHECK( 'b' <= b );
    CHECK( wxT('b') <= b );
    CHECK( wxUniChar('b') <= b );
    CHECK( wxUniChar(wxT('b')) <= b );

    CHECK( b - a == 1 );
    CHECK( a - b == -1 );
}


#define CYRILLIC_SMALL_LETTER_YU  ((wchar_t)0x044E)
#define ARABIC_LETTER_NOON        ((wchar_t)0x0646)

TEST_CASE("UniChar::CharCompareIntl", "[unichar]")
{
    wxUniChar a(CYRILLIC_SMALL_LETTER_YU);
    wxUniChar b(ARABIC_LETTER_NOON);

    CHECK( a == a );
    CHECK( a == CYRILLIC_SMALL_LETTER_YU );
    CHECK( a == wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CHECK( a != b );
    CHECK( a != ARABIC_LETTER_NOON );
    CHECK( a != wxUniChar(ARABIC_LETTER_NOON) );

    CHECK( a < b );
    CHECK( a < ARABIC_LETTER_NOON );
    CHECK( a < wxUniChar(ARABIC_LETTER_NOON) );

    CHECK( a <= b );
    CHECK( a <= ARABIC_LETTER_NOON );
    CHECK( a <= wxUniChar(ARABIC_LETTER_NOON) );

    CHECK( a <= a );
    CHECK( a <= CYRILLIC_SMALL_LETTER_YU );
    CHECK( a <= wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CHECK( b > a );
    CHECK( b > CYRILLIC_SMALL_LETTER_YU );
    CHECK( b > wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CHECK( b >= a );
    CHECK( b >= CYRILLIC_SMALL_LETTER_YU );
    CHECK( b >= wxUniChar(CYRILLIC_SMALL_LETTER_YU) );

    CHECK( b >= b );
    CHECK( b >= ARABIC_LETTER_NOON );
    CHECK( b >= wxUniChar(ARABIC_LETTER_NOON) );

    CHECK( b - a == 504 );
    CHECK( a - b == -504 );
}

TEST_CASE("UniChar::StringCompare", "[unichar]")
{
    // test string comparison with chars
    wxString sa = "a";
    const wxString sb = "b";
    char c1 = 'a';
    wchar_t c2 = wxT('a');
    wxUniChar c3 = 'a';

    CHECK( sa == 'a');
    CHECK( 'a' == sa);
    CHECK( sb != 'a');
    CHECK( 'a' != sb);

    CHECK( sa == c1);
    CHECK( c1 == sa);
    CHECK( sb != c1);
    CHECK( c1 != sb);

    CHECK( sa == c2);
    CHECK( c2 == sa);
    CHECK( sb != c2);
    CHECK( c2 != sb);

    CHECK( sa == c3);
    CHECK( c3 == sa);
    CHECK( sb != c3);
    CHECK( c3 != sb);

    // test wxUniCharRef:
    CHECK( sa == sa[0]);
    CHECK( sa[0] == sa);
    CHECK( sb != sa[0]);
    CHECK( sa[0] != sb);

    // test const version of operator[] (returns wxUniChar, not wxUniCharRef):
    CHECK( sb == sb[0]);
    CHECK( sb[0] == sb);
    CHECK( sa != sb[0]);
    CHECK( sb[0] != sa);
}

TEST_CASE("UniChar::StringCompareIntl", "[unichar]")
{
    // test string comparison with chars
    wxString sa = CYRILLIC_SMALL_LETTER_YU;
    const wxString sb = ARABIC_LETTER_NOON;
    wchar_t c2 = CYRILLIC_SMALL_LETTER_YU;
    wxUniChar c3 = CYRILLIC_SMALL_LETTER_YU;

    CHECK( sa == CYRILLIC_SMALL_LETTER_YU);
    CHECK( CYRILLIC_SMALL_LETTER_YU == sa);
    CHECK( sb != CYRILLIC_SMALL_LETTER_YU);
    CHECK( CYRILLIC_SMALL_LETTER_YU != sb);

    CHECK( sa == c2);
    CHECK( c2 == sa);
    CHECK( sb != c2);
    CHECK( c2 != sb);

    CHECK( sa == c3);
    CHECK( c3 == sa);
    CHECK( sb != c3);
    CHECK( c3 != sb);

    // test wxUniCharRef:
    CHECK( sa == sa[0]);
    CHECK( sa[0] == sa);
    CHECK( sb != sa[0]);
    CHECK( sa[0] != sb);

    // test const version of operator[] (returns wxUniChar, not wxUniCharRef):
    CHECK( sb == sb[0]);
    CHECK( sb[0] == sb);
    CHECK( sa != sb[0]);
    CHECK( sb[0] != sa);
}

#define wxUNICHAR_TEST_INT_COMPARE \
    wxUniChar a(aVal); \
    CHECK( a == aVal ); \
    CHECK( a != bVal ); \
    CHECK( a < bVal ); \
    CHECK( a <= bVal ); \
    CHECK( a > cVal ); \
    CHECK( a >= cVal );


TEST_CASE("UniChar::ShortCompare", "[unichar]")
{
    short aVal = 2;
    short bVal = 3;
    short cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::UnsignedShortCompare", "[unichar]")
{
    unsigned short aVal = 2;
    unsigned short bVal = 3;
    unsigned short cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::IntCompare", "[unichar]")
{
    int aVal = 2;
    int bVal = 3;
    int cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::UnsignedIntCompare", "[unichar]")
{
    unsigned int aVal = 2;
    unsigned int bVal = 3;
    unsigned int cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::LongCompare", "[unichar]")
{
    long aVal = 2;
    long bVal = 3;
    long cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::UnsignedLongCompare", "[unichar]")
{
    unsigned long aVal = 2;
    unsigned long bVal = 3;
    unsigned long cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::wxLongLongCompare", "[unichar]")
{
    wxLongLong_t aVal = 2;
    wxLongLong_t bVal = 3;
    wxLongLong_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

TEST_CASE("UniChar::wxULongLongCompare", "[unichar]")
{
    wxULongLong_t aVal = 2;
    wxULongLong_t bVal = 3;
    wxULongLong_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#endif

TEST_CASE("UniChar::WideCharCompare", "[unichar]")
{
    wchar_t aVal = 2;
    wchar_t bVal = 3;
    wchar_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#undef wxUNICHAR_TEST_INT_COMPARE

#define wxUNICHAR_TEST_ASSIGNMENT_OPERATOR \
    wxUniChar a; \
    wxUniChar b(bVal); \
    a = b; \
    CHECK(a == b);

TEST_CASE("UniChar::UniCharAssignmentOperator", "[unichar]")
{
    wxUniChar a;
    wxUniChar b('b');
    a = b;
    CHECK(a == b);
}

TEST_CASE("UniChar::UniCharRefAssignmentOperator", "[unichar]")
{
    wxUniChar a;
    wxUniChar b('b');
    wxString bStr('b');
    wxUniCharRef bRef = bStr[0];
    a = bRef;
    CHECK(a == b);
}

TEST_CASE("UniChar::CharAssignmentOperator", "[unichar]")
{
    char bVal = 'b';

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::UnsignedCharAssignmentOperator", "[unichar]")
{
    unsigned char bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::ShortAssignmentOperator", "[unichar]")
{
    short bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::UnsignedShortAssignmentOperator", "[unichar]")
{
    unsigned short bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::IntAssignmentOperator", "[unichar]")
{
    int bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::UnsignedIntAssignmentOperator", "[unichar]")
{
    unsigned int bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::LongAssignmentOperator", "[unichar]")
{
    long bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::UnsignedLongAssignmentOperator", "[unichar]")
{
    unsigned long bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::wxLongLongAssignmentOperator", "[unichar]")
{
    wxLongLong_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

TEST_CASE("UniChar::wxULongLongAssignmentOperator", "[unichar]")
{
    wxULongLong_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#endif

TEST_CASE("UniChar::WideCharAssignmentOperator", "[unichar]")
{
    wchar_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#undef wxUNICHAR_TEST_ASSIGNMENT_OPERATOR

TEST_CASE("UniChar::CharParenOperator", "[unichar]")
{
    char aVal;
    char bVal = 'b';
    wxUniChar b(bVal);

    aVal = (char) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::UnsignedCharParenOperator", "[unichar]")
{
    unsigned char aVal;
    unsigned char bVal = 'b';
    wxUniChar b(bVal);

    aVal = (unsigned char) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::ShortParenOperator", "[unichar]")
{
    short aVal;
    short bVal = 2;
    wxUniChar b(bVal);

    aVal = (short) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::UnsignedShortParenOperator", "[unichar]")
{
    unsigned short aVal;
    unsigned short bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned short) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::IntParenOperator", "[unichar]")
{
    int aVal;
    int bVal = 2;
    wxUniChar b(bVal);

    aVal = (int) b;
    CHECK(aVal == bVal);

}

TEST_CASE("UniChar::UnsignedIntParenOperator", "[unichar]")
{
    unsigned int aVal;
    unsigned int bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned int) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::LongParenOperator", "[unichar]")
{
    long aVal;
    long bVal = 2;
    wxUniChar b(bVal);

    aVal = (long) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::UnsignedLongParenOperator", "[unichar]")
{
    unsigned long aVal;
    unsigned long bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned long) b;
    CHECK(aVal == bVal);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::wxLongLongParenOperator", "[unichar]")
{
    wxLongLong_t aVal;
    wxLongLong_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wxLongLong_t) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::wxULongLongParenOperator", "[unichar]")
{
    wxULongLong_t aVal;
    wxULongLong_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wxULongLong_t) b;
    CHECK(aVal == bVal);
}

#endif

TEST_CASE("UniChar::WideCharParenOperator", "[unichar]")
{
    wchar_t aVal;
    wchar_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wchar_t) b;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefUniCharAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wxChar b = 'b';
    wxUniChar bVal(b);
    bRef = bVal;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefUniCharRefAssignmentOperator", "[unichar]")
{
    wxChar b = 'b';
    wxString testStr(b);
    wxUniCharRef testRef = testStr[0];

    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    bRef = testRef;

    CHECK(bRef == testRef);
}

TEST_CASE("UniChar::RefCharAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    char b = 'b';
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefUnsignedCharAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned char b = 'b';
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefShortAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    short b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefUnsignedShortAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned short b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefIntAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    int b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefUnsignedIntAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned int b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefLongAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    long b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefUnsignedLongAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned long b = 2;
    bRef = b;

    CHECK(bRef == b);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::RefwxLongLongAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wxLongLong_t b = 2;
    bRef = b;

    CHECK(bRef == b);
}
TEST_CASE("UniChar::RefwxULongLongAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned long b = 2;
    bRef = b;

    CHECK(bRef == b);
}

#endif

TEST_CASE("UniChar::RefWideCharAssignmentOperator", "[unichar]")
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wchar_t b = 2;
    bRef = b;

    CHECK(bRef == b);
}

TEST_CASE("UniChar::RefCharParenOperator", "[unichar]")
{
    char aVal;
    char bVal = 'b';
    wxString testStr(bVal);
    wxUniCharRef bRef = testStr[0];

    aVal = (char) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefUnsignedCharParenOperator", "[unichar]")
{
    unsigned char aVal;
    unsigned char bVal = 'b';
    wxString testStr{wxUniChar(bVal)};
    wxUniCharRef bRef = testStr[0];

    aVal = (unsigned char) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefShortParenOperator", "[unichar]")
{
    short aVal;
    short bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (short) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefUnsignedShortParenOperator", "[unichar]")
{
    unsigned short aVal;
    unsigned short bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned short) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefIntParenOperator", "[unichar]")
{
    int aVal;
    int bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (int) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefUnsignedIntParenOperator", "[unichar]")
{
    unsigned int aVal;
    unsigned int bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned int) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefLongParenOperator", "[unichar]")
{
    long aVal;
    long bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (long) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefUnsignedLongParenOperator", "[unichar]")
{
    unsigned long aVal;
    unsigned long bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned long) bRef;
    CHECK(aVal == bVal);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::RefwxLongLongParenOperator", "[unichar]")
{
    wxLongLong_t aVal;
    wxLongLong_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxLongLong_t) bRef;
    CHECK(aVal == bVal);
}

TEST_CASE("UniChar::RefwxULongLongParenOperator", "[unichar]")
{
    wxULongLong_t aVal;
    wxULongLong_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxULongLong_t) bRef;
    CHECK(aVal == bVal);
}

#endif

TEST_CASE("UniChar::RefWideCharParenOperator", "[unichar]")
{
    wchar_t aVal;
    wchar_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxLongLong_t) bRef;
    CHECK(aVal == bVal);
}

#define wxUNICHARREF_TEST_INT_COMPARE \
    wxUniChar a(aVal); \
    wxString dummyStr("0"); \
    wxUniCharRef aRef = dummyStr[0]; \
    aRef = a; \
    CHECK( aRef == aVal ); \
    CHECK( aRef != bVal ); \
    CHECK( aRef < bVal ); \
    CHECK( aRef <= bVal ); \
    CHECK( aRef > cVal ); \
    CHECK( aRef >= cVal );

TEST_CASE("UniChar::RefShortCompare", "[unichar]")
{
    short aVal = 2;
    short bVal = 3;
    short cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefUnsignedShortCompare", "[unichar]")
{
    unsigned short aVal = 2;
    unsigned short bVal = 3;
    unsigned short cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefIntCompare", "[unichar]")
{
    int aVal = 2;
    int bVal = 3;
    int cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefUnsignedIntCompare", "[unichar]")
{
    unsigned int aVal = 2;
    unsigned int bVal = 3;
    unsigned int cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefLongCompare", "[unichar]")
{
    long aVal = 2;
    long bVal = 3;
    long cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefUnsignedLongCompare", "[unichar]")
{
    unsigned long aVal = 2;
    unsigned long bVal = 3;
    unsigned long cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE("UniChar::RefwxLongLongCompare", "[unichar]")
{
    wxLongLong_t aVal = 2;
    wxLongLong_t bVal = 3;
    wxLongLong_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

TEST_CASE("UniChar::RefwxULongLongCompare", "[unichar]")
{
    wxULongLong_t aVal = 2;
    wxULongLong_t bVal = 3;
    wxULongLong_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

#endif

TEST_CASE("UniChar::RefWideCharCompare", "[unichar]")
{
    wchar_t aVal = 2;
    wchar_t bVal = 3;
    wchar_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}
