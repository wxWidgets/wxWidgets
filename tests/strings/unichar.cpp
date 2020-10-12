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
        CPPUNIT_TEST( ShortCompare );
        CPPUNIT_TEST( UnsignedShortCompare );
        CPPUNIT_TEST( IntCompare );
        CPPUNIT_TEST( UnsignedIntCompare );
        CPPUNIT_TEST( LongCompare );
        CPPUNIT_TEST( UnsignedLongCompare );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxLongLongCompare ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxULongLongCompare ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( WideCharCompare ); )
        CPPUNIT_TEST( UniCharAssignmentOperator );
        CPPUNIT_TEST( UniCharRefAssignmentOperator );
        CPPUNIT_TEST( CharAssignmentOperator );
        CPPUNIT_TEST( UnsignedCharAssignmentOperator );
        CPPUNIT_TEST( ShortAssignmentOperator );
        CPPUNIT_TEST( UnsignedShortAssignmentOperator );
        CPPUNIT_TEST( IntAssignmentOperator );
        CPPUNIT_TEST( UnsignedIntAssignmentOperator );
        CPPUNIT_TEST( LongAssignmentOperator );
        CPPUNIT_TEST( UnsignedLongAssignmentOperator );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxLongLongAssignmentOperator ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxULongLongAssignmentOperator ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( WideCharAssignmentOperator ); )
        CPPUNIT_TEST( CharParenOperator );
        CPPUNIT_TEST( UnsignedCharParenOperator );
        CPPUNIT_TEST( ShortParenOperator );
        CPPUNIT_TEST( UnsignedShortParenOperator );
        CPPUNIT_TEST( IntParenOperator );
        CPPUNIT_TEST( UnsignedIntParenOperator );
        CPPUNIT_TEST( LongParenOperator );
        CPPUNIT_TEST( UnsignedLongParenOperator );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxLongLongParenOperator ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( wxULongLongParenOperator ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( WideCharParenOperator ); )
        CPPUNIT_TEST(RefUniCharAssignmentOperator);
        CPPUNIT_TEST(RefUniCharRefAssignmentOperator);
        CPPUNIT_TEST(RefCharAssignmentOperator);
        CPPUNIT_TEST( RefShortAssignmentOperator );
        CPPUNIT_TEST( RefUnsignedShortAssignmentOperator );
        CPPUNIT_TEST( RefIntAssignmentOperator );
        CPPUNIT_TEST( RefUnsignedIntAssignmentOperator );
        CPPUNIT_TEST( RefLongAssignmentOperator );
        CPPUNIT_TEST( RefUnsignedLongAssignmentOperator );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxLongLongAssignmentOperator ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxULongLongAssignmentOperator ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( RefWideCharAssignmentOperator ); )
        CPPUNIT_TEST(RefCharParenOperator);
        CPPUNIT_TEST( RefShortParenOperator );
        CPPUNIT_TEST( RefUnsignedShortParenOperator );
        CPPUNIT_TEST( RefIntParenOperator );
        CPPUNIT_TEST( RefUnsignedIntParenOperator );
        CPPUNIT_TEST( RefLongParenOperator );
        CPPUNIT_TEST( RefUnsignedLongParenOperator );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxLongLongParenOperator ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxULongLongParenOperator ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( RefWideCharParenOperator ); )
        CPPUNIT_TEST( RefShortCompare );
        CPPUNIT_TEST( RefUnsignedShortCompare );
        CPPUNIT_TEST( RefIntCompare );
        CPPUNIT_TEST( RefUnsignedIntCompare );
        CPPUNIT_TEST( RefLongCompare );
        CPPUNIT_TEST( RefUnsignedLongCompare );
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxLongLongCompare ); )
        wxIF_LONG_LONG_TYPE( CPPUNIT_TEST( RefwxULongLongCompare ); )
        wxIF_WCHAR_T_TYPE( CPPUNIT_TEST( RefWideCharCompare ); )
#if wxUSE_UNICODE
        CPPUNIT_TEST( StringCompareIntl );
#endif // wxUSE_UNICODE
    CPPUNIT_TEST_SUITE_END();

    void CharCompare();
    void CharCompareIntl();
    void StringCompare();
    void StringCompareIntl();
    void ShortCompare();
    void UnsignedShortCompare();
    void IntCompare();
    void UnsignedIntCompare();
    void LongCompare();
    void UnsignedLongCompare();
    wxIF_LONG_LONG_TYPE( void wxLongLongCompare(); )
    wxIF_LONG_LONG_TYPE( void wxULongLongCompare(); )
    wxIF_WCHAR_T_TYPE( void WideCharCompare(); )
    void UniCharAssignmentOperator();
    void UniCharRefAssignmentOperator();
    void CharAssignmentOperator();
    void UnsignedCharAssignmentOperator();
    void ShortAssignmentOperator();
    void UnsignedShortAssignmentOperator();
    void IntAssignmentOperator();
    void UnsignedIntAssignmentOperator();
    void LongAssignmentOperator();
    void UnsignedLongAssignmentOperator();
    wxIF_LONG_LONG_TYPE( void wxLongLongAssignmentOperator(); )
    wxIF_LONG_LONG_TYPE( void wxULongLongAssignmentOperator(); )
    wxIF_WCHAR_T_TYPE( void WideCharAssignmentOperator(); )
    void CharParenOperator();
    void UnsignedCharParenOperator();
    void ShortParenOperator();
    void UnsignedShortParenOperator();
    void IntParenOperator();
    void UnsignedIntParenOperator();
    void LongParenOperator();
    void UnsignedLongParenOperator();
    wxIF_LONG_LONG_TYPE( void wxLongLongParenOperator(); )
    wxIF_LONG_LONG_TYPE( void wxULongLongParenOperator(); )
    wxIF_WCHAR_T_TYPE( void WideCharParenOperator(); )
    void RefUniCharAssignmentOperator();
    void RefUniCharRefAssignmentOperator();
    void RefCharAssignmentOperator();
    void RefUnsignedCharAssignmentOperator();
    void RefShortAssignmentOperator();
    void RefUnsignedShortAssignmentOperator();
    void RefIntAssignmentOperator();
    void RefUnsignedIntAssignmentOperator();
    void RefLongAssignmentOperator();
    void RefUnsignedLongAssignmentOperator();
    wxIF_LONG_LONG_TYPE( void RefwxLongLongAssignmentOperator(); )
    wxIF_LONG_LONG_TYPE( void RefwxULongLongAssignmentOperator(); )
    wxIF_WCHAR_T_TYPE( void RefWideCharAssignmentOperator(); )
    void RefCharParenOperator();
    void RefUnsignedCharParenOperator();
    void RefShortParenOperator();
    void RefUnsignedShortParenOperator();
    void RefIntParenOperator();
    void RefUnsignedIntParenOperator();
    void RefLongParenOperator();
    void RefUnsignedLongParenOperator();
    wxIF_LONG_LONG_TYPE( void RefwxLongLongParenOperator(); )
    wxIF_LONG_LONG_TYPE( void RefwxULongLongParenOperator(); )
    wxIF_WCHAR_T_TYPE( void RefWideCharParenOperator(); )
    void RefShortCompare();
    void RefUnsignedShortCompare();
    void RefIntCompare();
    void RefUnsignedIntCompare();
    void RefLongCompare();
    void RefUnsignedLongCompare();
    wxIF_LONG_LONG_TYPE( void RefwxLongLongCompare(); )
    wxIF_LONG_LONG_TYPE( void RefwxULongLongCompare(); )
    wxIF_WCHAR_T_TYPE( void RefWideCharCompare(); )

    wxDECLARE_NO_COPY_CLASS(UniCharTestCase);
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

    CPPUNIT_ASSERT( a == a );
    CPPUNIT_ASSERT( 'a' == a );
    CPPUNIT_ASSERT( wxT('a') == a );
    CPPUNIT_ASSERT( wxUniChar('a') == a );
    CPPUNIT_ASSERT( wxUniChar(wxT('a')) == a );

    CPPUNIT_ASSERT( a != b );
    CPPUNIT_ASSERT( a != 'b' );
    CPPUNIT_ASSERT( a != wxT('b') );
    CPPUNIT_ASSERT( a != wxUniChar('b') );
    CPPUNIT_ASSERT( a != wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( b != a );
    CPPUNIT_ASSERT( 'b' != a );
    CPPUNIT_ASSERT( wxT('b') != a );
    CPPUNIT_ASSERT( wxUniChar('b') != a );
    CPPUNIT_ASSERT( wxUniChar(wxT('b')) != a );

    CPPUNIT_ASSERT( a < b );
    CPPUNIT_ASSERT( a < 'b' );
    CPPUNIT_ASSERT( a < wxT('b') );
    CPPUNIT_ASSERT( a < wxUniChar('b') );
    CPPUNIT_ASSERT( a < wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( b > a );
    CPPUNIT_ASSERT( 'b' > a );
    CPPUNIT_ASSERT( wxT('b') > a );
    CPPUNIT_ASSERT( wxUniChar('b') > a );
    CPPUNIT_ASSERT( wxUniChar(wxT('b')) > a );

    CPPUNIT_ASSERT( a <= b );
    CPPUNIT_ASSERT( a <= 'b' );
    CPPUNIT_ASSERT( a <= wxT('b') );
    CPPUNIT_ASSERT( a <= wxUniChar('b') );
    CPPUNIT_ASSERT( a <= wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( b >= a );
    CPPUNIT_ASSERT( 'b' >= a );
    CPPUNIT_ASSERT( wxT('b') >= a );
    CPPUNIT_ASSERT( wxUniChar('b') >= a );
    CPPUNIT_ASSERT( wxUniChar(wxT('b')) >= a );

    CPPUNIT_ASSERT( a <= a );
    CPPUNIT_ASSERT( a <= 'a' );
    CPPUNIT_ASSERT( a <= wxT('a') );
    CPPUNIT_ASSERT( a <= wxUniChar('a') );
    CPPUNIT_ASSERT( a <= wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( a >= a );
    CPPUNIT_ASSERT( 'a' >= a );
    CPPUNIT_ASSERT( wxT('a') >= a );
    CPPUNIT_ASSERT( wxUniChar('a') >= a );
    CPPUNIT_ASSERT( wxUniChar(wxT('a')) >= a );

    CPPUNIT_ASSERT( b > a );
    CPPUNIT_ASSERT( b > 'a' );
    CPPUNIT_ASSERT( b > wxT('a') );
    CPPUNIT_ASSERT( b > wxUniChar('a') );
    CPPUNIT_ASSERT( b > wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( a < b );
    CPPUNIT_ASSERT( 'a' < b );
    CPPUNIT_ASSERT( wxT('a') < b );
    CPPUNIT_ASSERT( wxUniChar('a') < b );
    CPPUNIT_ASSERT( wxUniChar(wxT('a')) < b );

    CPPUNIT_ASSERT( b >= a );
    CPPUNIT_ASSERT( b >= 'a' );
    CPPUNIT_ASSERT( b >= wxT('a') );
    CPPUNIT_ASSERT( b >= wxUniChar('a') );
    CPPUNIT_ASSERT( b >= wxUniChar(wxT('a')) );

    CPPUNIT_ASSERT( a <= b );
    CPPUNIT_ASSERT( 'a' <= b );
    CPPUNIT_ASSERT( wxT('a') <= b );
    CPPUNIT_ASSERT( wxUniChar('a') <= b );
    CPPUNIT_ASSERT( wxUniChar(wxT('a')) <= b );

    CPPUNIT_ASSERT( b >= b );
    CPPUNIT_ASSERT( b >= 'b' );
    CPPUNIT_ASSERT( b >= wxT('b') );
    CPPUNIT_ASSERT( b >= wxUniChar('b') );
    CPPUNIT_ASSERT( b >= wxUniChar(wxT('b')) );

    CPPUNIT_ASSERT( b <= b );
    CPPUNIT_ASSERT( 'b' <= b );
    CPPUNIT_ASSERT( wxT('b') <= b );
    CPPUNIT_ASSERT( wxUniChar('b') <= b );
    CPPUNIT_ASSERT( wxUniChar(wxT('b')) <= b );

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

#define wxUNICHAR_TEST_INT_COMPARE \
    wxUniChar a(aVal); \
    CPPUNIT_ASSERT( a == aVal ); \
    CPPUNIT_ASSERT( a != bVal ); \
    CPPUNIT_ASSERT( a < bVal ); \
    CPPUNIT_ASSERT( a <= bVal ); \
    CPPUNIT_ASSERT( a > cVal ); \
    CPPUNIT_ASSERT( a >= cVal );


void UniCharTestCase::ShortCompare()
{
    short aVal = 2;
    short bVal = 3;
    short cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::UnsignedShortCompare()
{
    unsigned short aVal = 2;
    unsigned short bVal = 3;
    unsigned short cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::IntCompare()
{
    int aVal = 2;
    int bVal = 3;
    int cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::UnsignedIntCompare()
{
    unsigned int aVal = 2;
    unsigned int bVal = 3;
    unsigned int cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::LongCompare()
{
    long aVal = 2;
    long bVal = 3;
    long cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::UnsignedLongCompare()
{
    unsigned long aVal = 2;
    unsigned long bVal = 3;
    unsigned long cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::wxLongLongCompare()
{
    wxLongLong_t aVal = 2;
    wxLongLong_t bVal = 3;
    wxLongLong_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

void UniCharTestCase::wxULongLongCompare()
{
    wxULongLong_t aVal = 2;
    wxULongLong_t bVal = 3;
    wxULongLong_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::WideCharCompare()
{
    wchar_t aVal = 2;
    wchar_t bVal = 3;
    wchar_t cVal = 1;

    wxUNICHAR_TEST_INT_COMPARE
}

#endif

#undef wxUNICHAR_TEST_INT_COMPARE

#define wxUNICHAR_TEST_ASSIGNMENT_OPERATOR \
    wxUniChar a; \
    wxUniChar b(bVal); \
    a = b; \
    CPPUNIT_ASSERT(a == b);

void UniCharTestCase::UniCharAssignmentOperator()
{
    wxUniChar a;
    wxUniChar b('b');
    a = b;
    CPPUNIT_ASSERT(a == b);
}

void UniCharTestCase::UniCharRefAssignmentOperator()
{
    wxUniChar a;
    wxUniChar b('b');
    wxString bStr('b');
    wxUniCharRef bRef = bStr[0];
    a = bRef;
    CPPUNIT_ASSERT(a == b);
}

void UniCharTestCase::CharAssignmentOperator()
{
    char bVal = 'b';

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::UnsignedCharAssignmentOperator()
{
    unsigned char bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::ShortAssignmentOperator()
{
    short bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::UnsignedShortAssignmentOperator()
{
    unsigned short bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::IntAssignmentOperator()
{
    int bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::UnsignedIntAssignmentOperator()
{
    unsigned int bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::LongAssignmentOperator()
{
    long bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::UnsignedLongAssignmentOperator()
{
    unsigned long bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::wxLongLongAssignmentOperator()
{
    wxLongLong_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

void UniCharTestCase::wxULongLongAssignmentOperator()
{
    wxULongLong_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::WideCharAssignmentOperator()
{
    wchar_t bVal = 2;

    wxUNICHAR_TEST_ASSIGNMENT_OPERATOR
}

#endif

#undef wxUNICHAR_TEST_ASSIGNMENT_OPERATOR

void UniCharTestCase::CharParenOperator()
{
    char aVal;
    char bVal = 'b';
    wxUniChar b(bVal);

    aVal = (char) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::UnsignedCharParenOperator()
{
    unsigned char aVal;
    unsigned char bVal = 'b';
    wxUniChar b(bVal);

    aVal = (unsigned char) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::ShortParenOperator()
{
    short aVal;
    short bVal = 2;
    wxUniChar b(bVal);

    aVal = (short) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::UnsignedShortParenOperator()
{
    unsigned short aVal;
    unsigned short bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned short) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::IntParenOperator()
{
    int aVal;
    int bVal = 2;
    wxUniChar b(bVal);

    aVal = (int) b;
    CPPUNIT_ASSERT(aVal == bVal);

}

void UniCharTestCase::UnsignedIntParenOperator()
{
    unsigned int aVal;
    unsigned int bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned int) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::LongParenOperator()
{
    long aVal;
    long bVal = 2;
    wxUniChar b(bVal);

    aVal = (long) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::UnsignedLongParenOperator()
{
    unsigned long aVal;
    unsigned long bVal = 2;
    wxUniChar b(bVal);

    aVal = (unsigned long) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::wxLongLongParenOperator()
{
    wxLongLong_t aVal;
    wxLongLong_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wxLongLong_t) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::wxULongLongParenOperator()
{
    wxULongLong_t aVal;
    wxULongLong_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wxULongLong_t) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::WideCharParenOperator()
{
    wchar_t aVal;
    wchar_t bVal = 2;
    wxUniChar b(bVal);

    aVal = (wchar_t) b;
    CPPUNIT_ASSERT(aVal == bVal);
}

#endif

void UniCharTestCase::RefUniCharAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wxChar b = 'b';
    wxUniChar bVal(b);
    bRef = bVal;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefUniCharRefAssignmentOperator()
{
    wxChar b = 'b';
    wxString testStr(b);
    wxUniCharRef testRef = testStr[0];

    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    bRef = testRef;

    CPPUNIT_ASSERT(bRef == testRef);
}

void UniCharTestCase::RefCharAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    char b = 'b';
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefUnsignedCharAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned char b = 'b';
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefShortAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    short b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefUnsignedShortAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned short b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefIntAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    int b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefUnsignedIntAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned int b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefLongAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    long b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

void UniCharTestCase::RefUnsignedLongAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned long b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::RefwxLongLongAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wxLongLong_t b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}
void UniCharTestCase::RefwxULongLongAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    unsigned long b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::RefWideCharAssignmentOperator()
{
    wxString dummyStr('0');
    wxUniCharRef bRef = dummyStr[0];

    wchar_t b = 2;
    bRef = b;

    CPPUNIT_ASSERT(bRef == b);
}

#endif

void UniCharTestCase::RefCharParenOperator()
{
    char aVal;
    char bVal = 'b';
    wxString testStr(bVal);
    wxUniCharRef bRef = testStr[0];

    aVal = (char) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefUnsignedCharParenOperator()
{
    unsigned char aVal;
    unsigned char bVal = 'b';
    wxString testStr = wxString::Format(wxT("%u"), bVal);
    wxUniCharRef bRef = testStr[0];

    aVal = (unsigned char) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefShortParenOperator()
{
    short aVal;
    short bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (short) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefUnsignedShortParenOperator()
{
    unsigned short aVal;
    unsigned short bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned short) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefIntParenOperator()
{
    int aVal;
    int bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (int) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefUnsignedIntParenOperator()
{
    unsigned int aVal;
    unsigned int bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned int) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefLongParenOperator()
{
    long aVal;
    long bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (long) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefUnsignedLongParenOperator()
{
    unsigned long aVal;
    unsigned long bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (unsigned long) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::RefwxLongLongParenOperator()
{
    wxLongLong_t aVal;
    wxLongLong_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxLongLong_t) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

void UniCharTestCase::RefwxULongLongParenOperator()
{
    wxULongLong_t aVal;
    wxULongLong_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxULongLong_t) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::RefWideCharParenOperator()
{
    wchar_t aVal;
    wchar_t bVal = 2;
    wxUniChar b(bVal);

    wxString dummyStr("0");
    wxUniCharRef bRef = dummyStr[0];
    bRef = b;

    aVal = (wxLongLong_t) bRef;
    CPPUNIT_ASSERT(aVal == bVal);
}

#endif

#define wxUNICHARREF_TEST_INT_COMPARE \
    wxUniChar a(aVal); \
    wxString dummyStr("0"); \
    wxUniCharRef aRef = dummyStr[0]; \
    aRef = a; \
    CPPUNIT_ASSERT( aRef == aVal ); \
    CPPUNIT_ASSERT( aRef != bVal ); \
    CPPUNIT_ASSERT( aRef < bVal ); \
    CPPUNIT_ASSERT( aRef <= bVal ); \
    CPPUNIT_ASSERT( aRef > cVal ); \
    CPPUNIT_ASSERT( aRef >= cVal );

void UniCharTestCase::RefShortCompare()
{
    short aVal = 2;
    short bVal = 3;
    short cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefUnsignedShortCompare()
{
    unsigned short aVal = 2;
    unsigned short bVal = 3;
    unsigned short cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefIntCompare()
{
    int aVal = 2;
    int bVal = 3;
    int cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefUnsignedIntCompare()
{
    unsigned int aVal = 2;
    unsigned int bVal = 3;
    unsigned int cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefLongCompare()
{
    long aVal = 2;
    long bVal = 3;
    long cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefUnsignedLongCompare()
{
    unsigned long aVal = 2;
    unsigned long bVal = 3;
    unsigned long cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

void UniCharTestCase::RefwxLongLongCompare()
{
    wxLongLong_t aVal = 2;
    wxLongLong_t bVal = 3;
    wxLongLong_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

void UniCharTestCase::RefwxULongLongCompare()
{
    wxULongLong_t aVal = 2;
    wxULongLong_t bVal = 3;
    wxULongLong_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

#endif

#if wxWCHAR_T_IS_REAL_TYPE

void UniCharTestCase::RefWideCharCompare()
{
    wchar_t aVal = 2;
    wchar_t bVal = 3;
    wchar_t cVal = 1;

    wxUNICHARREF_TEST_INT_COMPARE
}

#endif
