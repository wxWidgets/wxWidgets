///////////////////////////////////////////////////////////////////////////////
// Name:        tests/arrays/arrays.cpp
// Purpose:     wxArray unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-01
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
    (( array.GetCount() == n ) && \
     ( array.Last() == array.Item( n - 1 ) ))

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

   const wxChar *GetName() const { return m_name.c_str(); }

private:
   wxString m_name;

   static size_t ms_bars;
};

size_t Bar::ms_bars = 0;

WX_DECLARE_OBJARRAY(Bar, ArrayBars);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayBars)

// ----------------------------------------------------------------------------
// another object array test
// ----------------------------------------------------------------------------

// This code doesn't make any sense, as object arrays should be used with
// objects, not pointers, but it used to work, so check that it continues to
// compile.
WX_DECLARE_OBJARRAY(Bar*, ArrayBarPtrs);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayBarPtrs)

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

DEFINE_COMPARE(Char, char)
DEFINE_COMPARE(UShort, ushort)
DEFINE_COMPARE(Int, int)

WX_DEFINE_ARRAY_CHAR(char, wxArrayChar);
WX_DEFINE_SORTED_ARRAY_CHAR(char, wxSortedArrayCharNoCmp);
WX_DEFINE_SORTED_ARRAY_CMP_CHAR(char, CharCompareValues, wxSortedArrayChar);

WX_DEFINE_ARRAY_SHORT(ushort, wxArrayUShort);
WX_DEFINE_SORTED_ARRAY_SHORT(ushort, wxSortedArrayUShortNoCmp);
WX_DEFINE_SORTED_ARRAY_CMP_SHORT(ushort, UShortCompareValues, wxSortedArrayUShort);

WX_DEFINE_SORTED_ARRAY_CMP_INT(int, IntCompareValues, wxSortedArrayInt);

struct Item
{
    Item(int n_ = 0) : n(n_) { }

    int n;
};

WX_DEFINE_ARRAY_PTR(Item *, ItemPtrArray);

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
        CPPUNIT_TEST( SortedArray );
        CPPUNIT_TEST( wxStringArraySplitTest );
        CPPUNIT_TEST( wxStringArrayJoinTest );
        CPPUNIT_TEST( wxStringArraySplitJoinTest );

        CPPUNIT_TEST( wxObjArrayTest );
        CPPUNIT_TEST( wxObjArrayPtrTest );
        CPPUNIT_TEST( wxArrayUShortTest );
        CPPUNIT_TEST( wxArrayIntTest );
        CPPUNIT_TEST( wxArrayCharTest );
        CPPUNIT_TEST( TestSTL );
        CPPUNIT_TEST( Alloc );
        CPPUNIT_TEST( Clear );
        CPPUNIT_TEST( Swap );
        CPPUNIT_TEST( IndexFromEnd );
    CPPUNIT_TEST_SUITE_END();

    void wxStringArrayTest();
    void SortedArray();
    void wxStringArraySplitTest();
    void wxStringArrayJoinTest();
    void wxStringArraySplitJoinTest();
    void wxObjArrayTest();
    void wxObjArrayPtrTest();
    void wxArrayUShortTest();
    void wxArrayIntTest();
    void wxArrayCharTest();
    void TestSTL();
    void Alloc();
    void Clear();
    void Swap();
    void IndexFromEnd();

    wxDECLARE_NO_COPY_CLASS(ArraysTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ArraysTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ArraysTestCase, "ArraysTestCase" );

void ArraysTestCase::wxStringArrayTest()
{
    wxArrayString a1;
    a1.Add(wxT("thermit"));
    a1.Add(wxT("condor"));
    a1.Add(wxT("lion"), 3);
    a1.Add(wxT("dog"));
    a1.Add(wxT("human"));
    a1.Add(wxT("alligator"));

    CPPUNIT_ASSERT((COMPARE_8_VALUES( a1 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 8 ) );

    wxArrayString a2(a1);

    CPPUNIT_ASSERT((COMPARE_8_VALUES( a2 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a2 , 8 ) );

    wxSortedArrayString a3(a1);

    CPPUNIT_ASSERT((COMPARE_8_VALUES( a3 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("thermit") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a3 , 8 ) );

    wxSortedArrayString a4;
    for (wxArrayString::iterator it = a1.begin(), en = a1.end(); it != en; ++it)
        a4.Add(*it);

    CPPUNIT_ASSERT((COMPARE_8_VALUES( a4 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("thermit") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a4 , 8 ) );

    a1.RemoveAt(2,3);

    CPPUNIT_ASSERT((COMPARE_5_VALUES( a1 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a2 = a1;

    CPPUNIT_ASSERT((COMPARE_5_VALUES( a2 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a2 , 5 ) );

    a1.Sort(false);

    CPPUNIT_ASSERT((COMPARE_5_VALUES( a1 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("thermit") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(true);

    CPPUNIT_ASSERT((COMPARE_5_VALUES( a1 , wxT("thermit") ,
                                           wxT("human") ,
                                           wxT("dog") ,
                                           wxT("condor") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(&StringLenCompare);

    CPPUNIT_ASSERT((COMPARE_5_VALUES( a1 , wxT("dog") ,
                                           wxT("human") ,
                                           wxT("condor") ,
                                           wxT("thermit") ,
                                           wxT("alligator") )));
    CPPUNIT_ASSERT( COMPARE_COUNT( a1 , 5 ) );
    CPPUNIT_ASSERT( a1.Index( wxT("dog") ) == 0 );
    CPPUNIT_ASSERT( a1.Index( wxT("human") ) == 1 );
    CPPUNIT_ASSERT( a1.Index( wxT("humann") ) == wxNOT_FOUND );
    CPPUNIT_ASSERT( a1.Index( wxT("condor") ) == 2 );
    CPPUNIT_ASSERT( a1.Index( wxT("thermit") ) == 3 );
    CPPUNIT_ASSERT( a1.Index( wxT("alligator") ) == 4 );

    CPPUNIT_ASSERT( a1.Index( wxT("dog"), /*bCase=*/true, /*fromEnd=*/true ) == 0 );
    CPPUNIT_ASSERT( a1.Index( wxT("human"), /*bCase=*/true, /*fromEnd=*/true ) == 1 );
    CPPUNIT_ASSERT( a1.Index( wxT("humann"), /*bCase=*/true, /*fromEnd=*/true ) == wxNOT_FOUND );
    CPPUNIT_ASSERT( a1.Index( wxT("condor"), /*bCase=*/true, /*fromEnd=*/true ) == 2 );
    CPPUNIT_ASSERT( a1.Index( wxT("thermit"), /*bCase=*/true, /*fromEnd=*/true ) == 3 );
    CPPUNIT_ASSERT( a1.Index( wxT("alligator"), /*bCase=*/true, /*fromEnd=*/true ) == 4 );

    wxArrayString a5;

    CPPUNIT_ASSERT( a5.Add( wxT("x"), 1 ) == 0 );
    CPPUNIT_ASSERT( a5.Add( wxT("a"), 3 ) == 1 );

    CPPUNIT_ASSERT((COMPARE_4_VALUES( a5, wxT("x") ,
                                          wxT("a") ,
                                          wxT("a") ,
                                          wxT("a") )));

    a5.assign(a1.end(), a1.end());
    CPPUNIT_ASSERT( a5.empty() );

    a5.assign(a1.begin(), a1.end());
    CPPUNIT_ASSERT( a5 == a1 );

    const wxString months[] = { "Jan", "Feb", "Mar" };
    a5.assign(months, months + WXSIZEOF(months));
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(months), a5.size() );
    CPPUNIT_ASSERT((COMPARE_3_VALUES(a5, "Jan", "Feb", "Mar")));

    a5.clear();
    CPPUNIT_ASSERT_EQUAL( 0, a5.size() );

    a5.resize(7, "Foo");
    CPPUNIT_ASSERT_EQUAL( 7, a5.size() );
    CPPUNIT_ASSERT_EQUAL( "Foo", a5[3] );

    a5.resize(3);
    CPPUNIT_ASSERT_EQUAL( 3, a5.size() );
    CPPUNIT_ASSERT_EQUAL( "Foo", a5[2] );

    wxArrayString a6;
    a6.Add("Foo");
    a6.Insert(a6[0], 1, 100);

    // The whole point of this code is to test self-assignment, so suppress
    // clang warning about it.
    wxCLANG_WARNING_SUPPRESS(self-assign-overloaded)

    wxArrayString a7;
    a7 = a7;
    CPPUNIT_ASSERT_EQUAL( 0, a7.size() );
    a7.Add("Bar");
    a7 = a7;
    CPPUNIT_ASSERT_EQUAL( 1, a7.size() );

    wxCLANG_WARNING_RESTORE(self-assign-overloaded)
}

void ArraysTestCase::SortedArray()
{
    wxSortedArrayString a;
    a.Add("d");
    a.Add("c");
    CPPUNIT_ASSERT_EQUAL( 0, a.Index("c") );

    a.push_back("b");
    a.push_back("a");
    CPPUNIT_ASSERT_EQUAL( 0, a.Index("a") );


    wxSortedArrayString ar(wxStringSortDescending);
    ar.Add("a");
    ar.Add("b");
    CPPUNIT_ASSERT_EQUAL( "b", ar[0] );
    CPPUNIT_ASSERT_EQUAL( "a", ar[1] );

    wxSortedArrayString ad(wxDictionaryStringSortAscending);
    ad.Add("AB");
    ad.Add("a");
    ad.Add("Aa");
    CPPUNIT_ASSERT_EQUAL( "a", ad[0] );
    CPPUNIT_ASSERT_EQUAL( "Aa", ad[1] );
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

    size_t i;
    for (i = 0; i < WXSIZEOF(separators); i++)
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

    for (i = 0; i < WXSIZEOF(separators); i++)
    {
        wxString string = wxJoin(theArr, separators[i]);
        CPPUNIT_ASSERT( theArr == wxSplit(string, separators[i]) );
    }

    wxArrayString emptyArray;
    wxString string = wxJoin(emptyArray, wxT(';'));
    CPPUNIT_ASSERT( string.empty() );

    CPPUNIT_ASSERT( wxSplit(string, wxT(';')).empty() );

    CPPUNIT_ASSERT_EQUAL( 2, wxSplit(wxT(";"), wxT(';')).size() );
}

void ArraysTestCase::wxObjArrayTest()
{
    {
        ArrayBars bars;
        Bar bar(wxT("first bar in general, second bar in array (two copies!)"));

        CPPUNIT_ASSERT_EQUAL( 0, bars.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 1, Bar::GetNumber() );

        bars.Add(new Bar(wxT("first bar in array")));
        bars.Add(bar, 2);

        CPPUNIT_ASSERT_EQUAL( 3, bars.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 4, Bar::GetNumber() );

        bars.RemoveAt(1, bars.GetCount() - 1);

        CPPUNIT_ASSERT_EQUAL( 1, bars.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 2, Bar::GetNumber() );

        bars.Empty();

        CPPUNIT_ASSERT_EQUAL( 0, bars.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 1, Bar::GetNumber() );
    }
    CPPUNIT_ASSERT_EQUAL( 0, Bar::GetNumber() );
}

void ArraysTestCase::wxObjArrayPtrTest()
{
    // Just check that instantiating this class compiles.
    ArrayBarPtrs barptrs;
    CPPUNIT_ASSERT_EQUAL( 0, barptrs.size() );
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
    CPPUNIT_ASSERT((COMPARE_10_VALUES(a,1,17,17,5,5,5,3,3,3,3)));             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## Compare);                                                  \
                                                                              \
    CPPUNIT_ASSERT((COMPARE_10_VALUES(a,1,3,3,3,3,5,5,5,17,17)));             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## RevCompare);                                               \
                                                                              \
    CPPUNIT_ASSERT((COMPARE_10_VALUES(a,17,17,5,5,5,3,3,3,3,1)));             \
    CPPUNIT_ASSERT( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    wxSortedArray##name b;                                                    \
                                                                              \
    b.Add(1);                                                                 \
    b.Add(17);                                                                \
    b.Add(5);                                                                 \
    b.Add(3);                                                                 \
                                                                              \
    CPPUNIT_ASSERT((COMPARE_4_VALUES(b,1,3,5,17)));                           \
    CPPUNIT_ASSERT( COMPARE_COUNT( b , 4 ) );                                 \
    CPPUNIT_ASSERT( b.Index( 0 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 1 ) == 0 );                                      \
    CPPUNIT_ASSERT( b.Index( 3 ) == 1 );                                      \
    CPPUNIT_ASSERT( b.Index( 4 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 5 ) == 2 );                                      \
    CPPUNIT_ASSERT( b.Index( 6 ) == wxNOT_FOUND );                            \
    CPPUNIT_ASSERT( b.Index( 17 ) == 3 );                                     \
}

TestArrayOf(UShort)

TestArrayOf(Char)

TestArrayOf(Int)

void ArraysTestCase::Alloc()
{
    wxArrayInt a;
    a.Add(17);
    a.Add(9);
    CPPUNIT_ASSERT_EQUAL( 2, a.GetCount() );

    a.Alloc(1000);

    CPPUNIT_ASSERT_EQUAL( 2, a.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 17, a[0] );
    CPPUNIT_ASSERT_EQUAL( 9, a[1] );
}

void ArraysTestCase::Clear()
{
    ItemPtrArray items;

    WX_CLEAR_ARRAY(items);
    CPPUNIT_ASSERT_EQUAL( 0, items.size() );

    items.push_back(new Item(17));
    items.push_back(new Item(71));
    CPPUNIT_ASSERT_EQUAL( 2, items.size() );

    WX_CLEAR_ARRAY(items);
    CPPUNIT_ASSERT_EQUAL( 0, items.size() );
}

namespace
{

template <typename A, typename T>
void DoTestSwap(T v1, T v2, T v3)
{
    A a1, a2;
    a1.swap(a2);
    CPPUNIT_ASSERT( a1.empty() );
    CPPUNIT_ASSERT( a2.empty() );

    a1.push_back(v1);
    a1.swap(a2);
    CPPUNIT_ASSERT( a1.empty() );
    CPPUNIT_ASSERT_EQUAL( 1, a2.size() );

    a1.push_back(v2);
    a1.push_back(v3);
    a2.swap(a1);
    CPPUNIT_ASSERT_EQUAL( 1, a1.size() );
    CPPUNIT_ASSERT_EQUAL( 2, a2.size() );
    CPPUNIT_ASSERT_EQUAL( v1, a1[0] );
    CPPUNIT_ASSERT_EQUAL( v3, a2[1] );

    a1.swap(a2);
    CPPUNIT_ASSERT_EQUAL( 2, a1.size() );
    CPPUNIT_ASSERT_EQUAL( 1, a2.size() );
}

} // anonymous namespace

void ArraysTestCase::Swap()
{
    DoTestSwap<wxArrayString>("Foo", "Bar", "Baz");

    DoTestSwap<wxArrayInt>(1, 10, 100);
    DoTestSwap<wxArrayLong>(6, 28, 496);
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
    CPPUNIT_ASSERT_EQUAL( COUNT, list1.size() );

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

    CPPUNIT_ASSERT( *list1.rbegin() == *(list1.end()-1) );
    CPPUNIT_ASSERT( *list1.begin() == *(list1.rend()-1) );

    it = list1.begin()+1;
    rit = list1.rbegin()+1;
    CPPUNIT_ASSERT( *list1.begin() == *(it-1) );
    CPPUNIT_ASSERT( *list1.rbegin() == *(rit-1) );

    CPPUNIT_ASSERT( list1.front() == 0 );
    CPPUNIT_ASSERT( list1.back() == COUNT - 1 );

    list1.erase(list1.begin());
    list1.erase(list1.end()-1);

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i );
    }


    ItemPtrArray items;
    items.push_back(new Item(17));
    CPPUNIT_ASSERT_EQUAL( 17, (*(items.rbegin()))->n );
    CPPUNIT_ASSERT_EQUAL( 17, (**items.begin()).n );
    WX_CLEAR_ARRAY(items);
}

void ArraysTestCase::IndexFromEnd()
{
    wxArrayInt a;
    a.push_back(10);
    a.push_back(1);
    a.push_back(42);

    CPPUNIT_ASSERT_EQUAL( 0, a.Index(10) );
    CPPUNIT_ASSERT_EQUAL( 1, a.Index(1) );
    CPPUNIT_ASSERT_EQUAL( 2, a.Index(42) );
    CPPUNIT_ASSERT_EQUAL( 0, a.Index(10, /*bFromEnd=*/true) );
    CPPUNIT_ASSERT_EQUAL( 1, a.Index(1, /*bFromEnd=*/true) );
    CPPUNIT_ASSERT_EQUAL( 2, a.Index(42, /*bFromEnd=*/true) );
}
