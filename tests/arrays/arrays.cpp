///////////////////////////////////////////////////////////////////////////////
// Name:        tests/arrays/arrays.cpp
// Purpose:     wxArray unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-01
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
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

#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// helpers for testing values and sizes
// ----------------------------------------------------------------------------

#define COMPARE_VALUE( array , index , value ) ( array.Item( index ) == value )

#define COMPARE_2_VALUES( array , p0 , p1 ) \
    COMPARE_VALUE( array , 0 , p0 ) && \
    COMPARE_VALUE( array , 1 , p1 )

#define COMPARE_3_VALUES( array , p0 , p1 , p2 ) \
    COMPARE_2_VALUES( array , p0 , p1 ) && \
    COMPARE_VALUE( array , 2 , p2 )

#define COMPARE_4_VALUES( array , p0 , p1 , p2 , p3 ) \
    COMPARE_3_VALUES( array , p0 , p1 , p2 ) && \
    COMPARE_VALUE( array , 3 , p3 )

#define COMPARE_5_VALUES( array , p0 , p1 , p2 , p3 , p4 ) \
    COMPARE_4_VALUES( array , p0 , p1 , p2 , p3 ) && \
    COMPARE_VALUE( array , 4 , p4 )

#define COMPARE_6_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 ) \
    COMPARE_5_VALUES( array , p0 , p1 , p2 , p3 , p4 ) && \
    COMPARE_VALUE( array , 5 , p5 )

#define COMPARE_7_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 ) \
    COMPARE_6_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 ) && \
    COMPARE_VALUE( array , 6 , p6 )

#define COMPARE_8_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7 ) \
    COMPARE_7_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 ) && \
    COMPARE_VALUE( array , 7 , p7 )

#define COMPARE_9_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 ) \
    COMPARE_8_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7 ) && \
    COMPARE_VALUE( array , 8 , p8 )

#define COMPARE_10_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 , p9 ) \
    COMPARE_9_VALUES( array , p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7 , p8 ) && \
    COMPARE_VALUE( array , 9 , p9 )

#define COMPARE_COUNT( array , n ) \
    ( array.GetCount() == n ) && \
    ( array.Last() == array.Item( n - 1 ) )

// ----------------------------------------------------------------------------
// helpers for testing wxObjArray
// ----------------------------------------------------------------------------

class Bar
{
public:
    Bar(const wxString& name) : m_name(name) { ms_bars++; }
    Bar(const Bar& bar) : m_name(bar.m_name) { ms_bars++; }
   ~Bar() { ms_bars--; }

   static size_t GetNumber() { return ms_bars; }

   const wxChar *GetName() const { return m_name; }

private:
   wxString m_name;

   static size_t ms_bars;
};

size_t Bar::ms_bars = 0;

WX_DECLARE_OBJARRAY(Bar, ArrayBars);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayBars);

// ----------------------------------------------------------------------------
// helpers for sorting arrays and comparing items
// ----------------------------------------------------------------------------

int wxCMPFUNC_CONV StringLenCompare(const wxString& first,
                                    const wxString& second)
{
    return first.length() - second.length();
}

#define DEFINE_COMPARE(name, T)                                               \
                                                                              \
int wxCMPFUNC_CONV name ## CompareValues(T first, T second)                   \
{                                                                             \
    return first - second;                                                    \
}                                                                             \
                                                                              \
int wxCMPFUNC_CONV name ## Compare(T* first, T* second)                       \
{                                                                             \
    return *first - *second;                                                  \
}                                                                             \
                                                                              \
int wxCMPFUNC_CONV name ## RevCompare(T* first, T* second)                    \
{                                                                             \
    return *second - *first;                                                  \
}                                                                             \

typedef unsigned short ushort;

DEFINE_COMPARE(Char, char);
DEFINE_COMPARE(UShort, ushort);
DEFINE_COMPARE(Int, int);

WX_DEFINE_ARRAY_CHAR(char, wxArrayChar);
WX_DEFINE_SORTED_ARRAY_CHAR(char, wxSortedArrayCharNoCmp);
WX_DEFINE_SORTED_ARRAY_CMP_CHAR(char, CharCompareValues, wxSortedArrayChar);

WX_DEFINE_ARRAY_SHORT(ushort, wxArrayUShort);
WX_DEFINE_SORTED_ARRAY_SHORT(ushort, wxSortedArrayUShortNoCmp);
WX_DEFINE_SORTED_ARRAY_CMP_SHORT(ushort, UShortCompareValues, wxSortedArrayUShort);

WX_DEFINE_SORTED_ARRAY_CMP_INT(int, IntCompareValues, wxSortedArrayInt);

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ArraysTestCase : public CppUnit::TestCase
{
public:
    ArraysTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ArraysTestCase );
        CPPUNIT_TEST( wxStringArrayTest );
        CPPUNIT_TEST( wxStringArraySplitTest );
        CPPUNIT_TEST( wxStringArrayJoinTest );
        CPPUNIT_TEST( wxStringArraySplitJoinTest );

        CPPUNIT_TEST( wxObjArrayTest );
        CPPUNIT_TEST( wxArrayUShortTest );
        CPPUNIT_TEST( wxArrayIntTest );
        CPPUNIT_TEST( wxArrayCharTest );
        CPPUNIT_TEST( TestSTL );
        CPPUNIT_TEST( Alloc );
    CPPUNIT_TEST_SUITE_END();

    void wxStringArrayTest();
    void wxStringArraySplitTest();
    void wxStringArrayJoinTest();
    void wxStringArraySplitJoinTest();
    void wxObjArrayTest();
    void wxArrayUShortTest();
    void wxArrayIntTest();
    void wxArrayCharTest();
    void TestSTL();
    void Alloc();

    DECLARE_NO_COPY_CLASS(ArraysTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ArraysTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ArraysTestCase, "ArraysTestCase" );

void ArraysTestCase::wxStringArrayTest()
{
    wxArrayString a1;
    a1.Add(_T("thermit"));
    a1.Add(_T("condor"));
    a1.Add(_T("lion"), 3);
    a1.Add(_T("dog"));
    a1.Add(_T("human"));
    a1.Add(_T("alligator"));

    CPPUNIT_ASSERT( COMPARE_8_VALUES( a1 , _T("thermit") ,
                                           _T("condor") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 8 ) );

    wxArrayString a2(a1);

    CPPUNIT_ASSERT( COMPARE_8_VALUES( a2 , _T("thermit") ,
                                           _T("condor") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a2 , 8 ) );

    wxSortedArrayString a3(a1);

    CPPUNIT_ASSERT( COMPARE_8_VALUES( a3 , _T("alligator") ,
                                           _T("condor") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("thermit") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a3 , 8 ) );

    wxSortedArrayString a4;
    for (wxArrayString::iterator it = a1.begin(), en = a1.end(); it != en; ++it)
        a4.Add(*it);

    CPPUNIT_ASSERT( COMPARE_8_VALUES( a4 , _T("alligator") ,
                                           _T("condor") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("lion") ,
                                           _T("thermit") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a4 , 8 ) );

    a1.RemoveAt(2,3);

    CPPUNIT_ASSERT( COMPARE_5_VALUES( a1 , _T("thermit") ,
                                           _T("condor") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a2 = a1;

    CPPUNIT_ASSERT( COMPARE_5_VALUES( a2 , _T("thermit") ,
                                           _T("condor") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a2 , 5 ) );

    a1.Sort(false);

    CPPUNIT_ASSERT( COMPARE_5_VALUES( a1 , _T("alligator") ,
                                           _T("condor") ,
                                           _T("dog") ,
                                           _T("human") ,
                                           _T("thermit") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(true);

    CPPUNIT_ASSERT( COMPARE_5_VALUES( a1 , _T("thermit") ,
                                           _T("human") ,
                                           _T("dog") ,
                                           _T("condor") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(&StringLenCompare);

    CPPUNIT_ASSERT( COMPARE_5_VALUES( a1 , _T("dog") ,
                                           _T("human") ,
                                           _T("condor") ,
                                           _T("thermit") ,
                                           _T("alligator") ) );
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );
    CPPUNIT_ASSERT( a1.Index( _T("dog") ) == 0 );
    CPPUNIT_ASSERT( a1.Index( _T("human") ) == 1 );
    CPPUNIT_ASSERT( a1.Index( _T("humann") ) == wxNOT_FOUND );
    CPPUNIT_ASSERT( a1.Index( _T("condor") ) == 2 );
    CPPUNIT_ASSERT( a1.Index( _T("thermit") ) == 3 );
    CPPUNIT_ASSERT( a1.Index( _T("alligator") ) == 4 );

    wxArrayString a5;

    CPPUNIT_ASSERT( a5.Add( _T("x"), 1 ) == 0 );
    CPPUNIT_ASSERT( a5.Add( _T("a"), 3 ) == 1 );

    CPPUNIT_ASSERT( COMPARE_4_VALUES( a5, _T("x") ,
                                          _T("a") ,
                                          _T("a") ,
                                          _T("a") ) );
}

void ArraysTestCase::wxStringArraySplitTest()
{
    // test wxSplit:

    {
        wxString str = wxT(",,,,first,second,third,,");
        const wxChar *expected[] =
            { wxT(""), wxT(""), wxT(""), wxT(""), wxT("first"),
              wxT("second"), wxT("third"), wxT(""), wxT("") };

        wxArrayString exparr(WXSIZEOF(expected), expected);
        wxArrayString realarr(wxSplit(str, wxT(',')));
        CPPUNIT_ASSERT( exparr == realarr );
    }

    {
        wxString str = wxT(",\\,first,second,third,");
        const wxChar *expected[] =
            { wxT(""), wxT(",first"), wxT("second"), wxT("third"), wxT("") };
        const wxChar *expected2[] =
            { wxT(""), wxT("\\"), wxT("first"), wxT("second"), wxT("third"), wxT("") };

        // escaping on:
        wxArrayString exparr(WXSIZEOF(expected), expected);
        wxArrayString realarr(wxSplit(str, wxT(','), wxT('\\')));
        CPPUNIT_ASSERT( exparr == realarr );

        // escaping turned off:
        wxArrayString exparr2(WXSIZEOF(expected2), expected2);
        wxArrayString realarr2(wxSplit(str, wxT(','), wxT('\0')));
        CPPUNIT_ASSERT( exparr2 == realarr2 );
    }

    {
        // test is escape characters placed before non-separator character are
        // just ignored as they should:
        wxString str = wxT(",\\,,fir\\st,se\\cond\\,,\\third");
        const wxChar *expected[] =
            { wxT(""), wxT(","), wxT("fir\\st"), wxT("se\\cond,"), wxT("\\third") };
        const wxChar *expected2[] =
            { wxT(""), wxT("\\"), wxT(""), wxT("fir\\st"), wxT("se\\cond\\"),
              wxT(""), wxT("\\third") };

        // escaping on:
        wxArrayString exparr(WXSIZEOF(expected), expected);
        wxArrayString realarr(wxSplit(str, wxT(','), wxT('\\')));
        CPPUNIT_ASSERT( exparr == realarr );

        // escaping turned off:
        wxArrayString exparr2(WXSIZEOF(expected2), expected2);
        wxArrayString realarr2(wxSplit(str, wxT(','), wxT('\0')));
        CPPUNIT_ASSERT( exparr2 == realarr2 );
    }
}

void ArraysTestCase::wxStringArrayJoinTest()
{
    // test wxJoin:

    {
        const wxChar *arr[] = { wxT("first"), wxT(""), wxT("second"), wxT("third") };
        wxString expected = wxT("first,,second,third");

        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','));
        CPPUNIT_ASSERT( expected == result );
    }

    {
        const wxChar *arr[] = { wxT("first, word"), wxT(",,second"), wxT("third,,") };
        wxString expected = wxT("first\\, word,\\,\\,second,third\\,\\,");
        wxString expected2 = wxT("first, word,,,second,third,,");

        // escaping on:
        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','), wxT('\\'));
        CPPUNIT_ASSERT( expected == result );

        // escaping turned off:
        wxString result2 = wxJoin(arrstr, wxT(','), wxT('\0'));
        CPPUNIT_ASSERT( expected2 == result2 );
    }

    {
        // test is escape characters placed in the original array are just ignored as they should:
        const wxChar *arr[] = { wxT("first\\, wo\\rd"), wxT("seco\\nd"), wxT("\\third\\") };
        wxString expected = wxT("first\\\\, wo\\rd,seco\\nd,\\third\\");
        wxString expected2 = wxT("first\\, wo\\rd,seco\\nd,\\third\\");

        // escaping on:
        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','), wxT('\\'));
        CPPUNIT_ASSERT( expected == result );

        // escaping turned off:
        wxString result2 = wxJoin(arrstr, wxT(','), wxT('\0'));
        CPPUNIT_ASSERT( expected2 == result2 );
    }
}

void ArraysTestCase::wxStringArraySplitJoinTest()
{
    wxChar separators[] = { wxT('a'), wxT(','), wxT('_'), wxT(' '), wxT('\\'),
                            wxT('&'), wxT('{'), wxT('A'), wxT('<'), wxT('>'),
                            wxT('\''), wxT('\n'), wxT('!'), wxT('-') };

    // test with a string: split it and then rejoin it:

    wxString str = wxT("This is a long, long test; if wxSplit and wxJoin do work ")
                   wxT("correctly, then splitting and joining this 'text' _multiple_ ")
                   wxT("times shouldn't cause any loss of content.\n")
                   wxT("This is some latex code: ")
                   wxT("\\func{wxString}{wxJoin}{")
                   wxT("\\param{const wxArray String\\&}{ arr}, ")
                   wxT("\\param{const wxChar}{ sep}, ")
                   wxT("\\param{const wxChar}{ escape = '\\'}}.\n")
                   wxT("This is some HTML code: ")
                   wxT("<html><head><meta http-equiv=\"content-type\">")
                   wxT("<title>Initial page of Mozilla Firefox</title>")
                   wxT("</meta></head></html>");

    for (size_t i=0; i < WXSIZEOF(separators); i++)
    {
        wxArrayString arr = wxSplit(str, separators[i]);
        CPPUNIT_ASSERT( str == wxJoin(arr, separators[i]) );
    }


    // test with an array: join it and then resplit it:

    const wxChar *arr[] =
        {
            wxT("first, second!"), wxT("this is the third!!"),
            wxT("\nThat's the fourth token\n\n"), wxT(" - fifth\ndummy\ntoken - "),
            wxT("_sixth__token__with_underscores"), wxT("The! Last! One!")
        };
    wxArrayString theArr(WXSIZEOF(arr), arr);

    for (size_t i=0; i < WXSIZEOF(separators); i++)
    {
        wxString string = wxJoin(theArr, separators[i]);
        CPPUNIT_ASSERT( theArr == wxSplit(string, separators[i]) );
    }

    wxArrayString emptyArray;
    wxString string = wxJoin(emptyArray, _T(';'));
    CPPUNIT_ASSERT( string.empty() );

    CPPUNIT_ASSERT( wxSplit(string, _T(';')).empty() );

    CPPUNIT_ASSERT_EQUAL( (size_t)2, wxSplit(_T(";"), _T(';')).size() );
}

void ArraysTestCase::wxObjArrayTest()
{
    {
        ArrayBars bars;
        Bar bar(_T("first bar in general, second bar in array (two copies!)"));

        CPPUNIT_ASSERT( bars.GetCount() == 0 );
        CPPUNIT_ASSERT( Bar::GetNumber() == 1 );

        bars.Add(new Bar(_T("first bar in array")));
        bars.Add(bar,2);

        CPPUNIT_ASSERT( bars.GetCount() == 3 );
        CPPUNIT_ASSERT( Bar::GetNumber() == 4 );

        bars.RemoveAt(1, bars.GetCount() - 1);

        CPPUNIT_ASSERT( bars.GetCount() == 1 );
        CPPUNIT_ASSERT( Bar::GetNumber() == 2 );

        bars.Empty();

        CPPUNIT_ASSERT( bars.GetCount() == 0 );
        CPPUNIT_ASSERT( Bar::GetNumber() == 1 );
    }
    CPPUNIT_ASSERT( Bar::GetNumber() == 0 );
}

#define TestArrayOf(name)                                                     \
                                                                              \
void ArraysTestCase::wxArray ## name ## Test()                                \
{                                                                             \
    wxArray##name a;                                                          \
    a.Add(1);                                                                 \
    a.Add(17,2);                                                              \
    a.Add(5,3);                                                               \
    a.Add(3,4);                                                               \
                                                                              \
    CPPUNIT_ASSERT( COMPARE_10_VALUES(a,1,17,17,5,5,5,3,3,3,3) );             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## Compare);                                                  \
                                                                              \
    CPPUNIT_ASSERT( COMPARE_10_VALUES(a,1,3,3,3,3,5,5,5,17,17) );             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## RevCompare);                                               \
                                                                              \
    CPPUNIT_ASSERT( COMPARE_10_VALUES(a,17,17,5,5,5,3,3,3,3,1) );             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    wxSortedArray##name b;                                                    \
                                                                              \
    b.Add(1);                                                                 \
    b.Add(17);                                                                \
    b.Add(5);                                                                 \
    b.Add(3);                                                                 \
                                                                              \
    CPPUNIT_ASSERT( COMPARE_4_VALUES(b,1,3,5,17) );                           \
    CPPUNIT_ASSERT( COMPARE_COUNT( b , 4 ) );                                 \
    CPPUNIT_ASSERT( b.Index( 0 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 1 ) == 0 );                                      \
    CPPUNIT_ASSERT( b.Index( 3 ) == 1 );                                      \
    CPPUNIT_ASSERT( b.Index( 4 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 5 ) == 2 );                                      \
    CPPUNIT_ASSERT( b.Index( 6 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 17 ) == 3 );                                     \
}

TestArrayOf(UShort);

TestArrayOf(Char);

TestArrayOf(Int);

void ArraysTestCase::Alloc()
{
    wxArrayInt a;
    a.Add(17);
    a.Add(9);
    CPPUNIT_ASSERT_EQUAL( size_t(2), a.GetCount() );

    a.Alloc(1000);

    CPPUNIT_ASSERT_EQUAL( size_t(2), a.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 17, a[0] );
    CPPUNIT_ASSERT_EQUAL( 9, a[1] );
}

void ArraysTestCase::TestSTL()
{
    wxArrayInt list1;
    wxArrayInt::iterator it, en;
    wxArrayInt::reverse_iterator rit, ren;
    int i;
    static const int COUNT = 5;

    for ( i = 0; i < COUNT; ++i )
        list1.push_back(i);

    CPPUNIT_ASSERT( list1.capacity() >= (size_t)COUNT );
    CPPUNIT_ASSERT_EQUAL( (size_t)COUNT, list1.size() );

    for ( it = list1.begin(), en = list1.end(), i = 0;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i );
    }

    CPPUNIT_ASSERT_EQUAL( COUNT, i );

    for ( rit = list1.rbegin(), ren = list1.rend(), i = COUNT;
          rit != ren; ++rit, --i )
    {
        CPPUNIT_ASSERT( *rit == i-1 );
    }

    CPPUNIT_ASSERT_EQUAL( 0, i );

    CPPUNIT_ASSERT( *list1.rbegin() == *(list1.end()-1) &&
                    *list1.begin() == *(list1.rend()-1) );

    it = list1.begin()+1;
    rit = list1.rbegin()+1;
    CPPUNIT_ASSERT( *list1.begin() == *(it-1) &&
                    *list1.rbegin() == *(rit-1) );

    CPPUNIT_ASSERT( ( list1.front() == 0 ) && ( list1.back() == COUNT - 1 ) );

    list1.erase(list1.begin());
    list1.erase(list1.end()-1);

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i );
    }
}
