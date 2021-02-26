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
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("wxArrayString", "[dynarray]")
{
    wxArrayString a1;
    a1.Add(wxT("thermit"));
    a1.Add(wxT("condor"));
    a1.Add(wxT("lion"), 3);
    a1.Add(wxT("dog"));
    a1.Add(wxT("human"));
    a1.Add(wxT("alligator"));

    CHECK((COMPARE_8_VALUES( a1 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a1 , 8 ) );

    wxArrayString a2(a1);

    CHECK((COMPARE_8_VALUES( a2 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a2 , 8 ) );

    wxSortedArrayString a3(a1);

    CHECK((COMPARE_8_VALUES( a3 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("thermit") )));
    CHECK( COMPARE_COUNT( a3 , 8 ) );

    wxSortedArrayString a4;
    for (wxArrayString::iterator it = a1.begin(), en = a1.end(); it != en; ++it)
        a4.Add(*it);

    CHECK((COMPARE_8_VALUES( a4 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("lion") ,
                                           wxT("thermit") )));
    CHECK( COMPARE_COUNT( a4 , 8 ) );

    a1.RemoveAt(2,3);

    CHECK((COMPARE_5_VALUES( a1 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a1 , 5 ) );

    a2 = a1;

    CHECK((COMPARE_5_VALUES( a2 , wxT("thermit") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a2 , 5 ) );

    a1.Sort(false);

    CHECK((COMPARE_5_VALUES( a1 , wxT("alligator") ,
                                           wxT("condor") ,
                                           wxT("dog") ,
                                           wxT("human") ,
                                           wxT("thermit") )));
    CHECK( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(true);

    CHECK((COMPARE_5_VALUES( a1 , wxT("thermit") ,
                                           wxT("human") ,
                                           wxT("dog") ,
                                           wxT("condor") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a1 , 5 ) );

    a1.Sort(&StringLenCompare);

    CHECK((COMPARE_5_VALUES( a1 , wxT("dog") ,
                                           wxT("human") ,
                                           wxT("condor") ,
                                           wxT("thermit") ,
                                           wxT("alligator") )));
    CHECK( COMPARE_COUNT( a1 , 5 ) );
    CHECK( a1.Index( wxT("dog") ) == 0 );
    CHECK( a1.Index( wxT("human") ) == 1 );
    CHECK( a1.Index( wxT("humann") ) == wxNOT_FOUND );
    CHECK( a1.Index( wxT("condor") ) == 2 );
    CHECK( a1.Index( wxT("thermit") ) == 3 );
    CHECK( a1.Index( wxT("alligator") ) == 4 );

    CHECK( a1.Index( wxT("dog"), /*bCase=*/true, /*fromEnd=*/true ) == 0 );
    CHECK( a1.Index( wxT("human"), /*bCase=*/true, /*fromEnd=*/true ) == 1 );
    CHECK( a1.Index( wxT("humann"), /*bCase=*/true, /*fromEnd=*/true ) == wxNOT_FOUND );
    CHECK( a1.Index( wxT("condor"), /*bCase=*/true, /*fromEnd=*/true ) == 2 );
    CHECK( a1.Index( wxT("thermit"), /*bCase=*/true, /*fromEnd=*/true ) == 3 );
    CHECK( a1.Index( wxT("alligator"), /*bCase=*/true, /*fromEnd=*/true ) == 4 );

    wxArrayString a5;

    CHECK( a5.Add( wxT("x"), 1 ) == 0 );
    CHECK( a5.Add( wxT("a"), 3 ) == 1 );

    CHECK((COMPARE_4_VALUES( a5, wxT("x") ,
                                          wxT("a") ,
                                          wxT("a") ,
                                          wxT("a") )));

    a5.assign(a1.end(), a1.end());
    CHECK( a5.empty() );

    a5.assign(a1.begin(), a1.end());
    CHECK( a5 == a1 );

    const wxString months[] = { "Jan", "Feb", "Mar" };
    a5.assign(months, months + WXSIZEOF(months));
    CHECK( a5.size() == WXSIZEOF(months) );
    CHECK((COMPARE_3_VALUES(a5, "Jan", "Feb", "Mar")));

    a5.clear();
    CHECK( a5.size() == 0 );

    a5.resize(7, "Foo");
    CHECK( a5.size() == 7 );
    CHECK( a5[3] == "Foo" );

    a5.resize(3);
    CHECK( a5.size() == 3 );
    CHECK( a5[2] == "Foo" );

    wxArrayString a6;
    a6.Add("Foo");
    a6.Insert(a6[0], 1, 100);

    // The whole point of this code is to test self-assignment, so suppress
    // clang warning about it.
    wxCLANG_WARNING_SUPPRESS(self-assign-overloaded)

    wxArrayString a7;
    a7 = a7;
    CHECK( a7.size() == 0 );
    a7.Add("Bar");
    a7 = a7;
    CHECK( a7.size() == 1 );

    wxCLANG_WARNING_RESTORE(self-assign-overloaded)
}

TEST_CASE("wxSortedArrayString", "[dynarray]")
{
    wxSortedArrayString a;
    a.Add("d");
    a.Add("c");
    CHECK( a.Index("c") == 0 );

    a.push_back("b");
    a.push_back("a");
    CHECK( a.Index("a") == 0 );


    wxSortedArrayString ar(wxStringSortDescending);
    ar.Add("a");
    ar.Add("b");
    CHECK( ar[0] == "b" );
    CHECK( ar[1] == "a" );

    wxSortedArrayString ad(wxDictionaryStringSortAscending);
    ad.Add("AB");
    ad.Add("a");
    ad.Add("Aa");
    CHECK( ad[0] == "a" );
    CHECK( ad[1] == "Aa" );
    CHECK( ad.Index("a") == 0 );
    CHECK( ad.Index("Aa") == 1 );
    CHECK( ad.Index("AB") == 2 );
    CHECK( ad.Index("A") == wxNOT_FOUND );
    CHECK( ad.Index("z") == wxNOT_FOUND );
}

TEST_CASE("Arrays::Split", "[dynarray]")
{
    // test wxSplit:

    {
        wxString str = wxT(",,,,first,second,third,,");
        const wxChar *expected[] =
            { wxT(""), wxT(""), wxT(""), wxT(""), wxT("first"),
              wxT("second"), wxT("third"), wxT(""), wxT("") };

        wxArrayString exparr(WXSIZEOF(expected), expected);
        wxArrayString realarr(wxSplit(str, wxT(',')));
        CHECK( exparr == realarr );
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
        CHECK( exparr == realarr );

        // escaping turned off:
        wxArrayString exparr2(WXSIZEOF(expected2), expected2);
        wxArrayString realarr2(wxSplit(str, wxT(','), wxT('\0')));
        CHECK( exparr2 == realarr2 );
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
        CHECK( exparr == realarr );

        // escaping turned off:
        wxArrayString exparr2(WXSIZEOF(expected2), expected2);
        wxArrayString realarr2(wxSplit(str, wxT(','), wxT('\0')));
        CHECK( exparr2 == realarr2 );
    }
}

TEST_CASE("Arrays::Join", "[dynarray]")
{
    // test wxJoin:

    {
        const wxChar *arr[] = { wxT("first"), wxT(""), wxT("second"), wxT("third") };
        wxString expected = wxT("first,,second,third");

        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','));
        CHECK( expected == result );
    }

    {
        const wxChar *arr[] = { wxT("first, word"), wxT(",,second"), wxT("third,,") };
        wxString expected = wxT("first\\, word,\\,\\,second,third\\,\\,");
        wxString expected2 = wxT("first, word,,,second,third,,");

        // escaping on:
        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','), wxT('\\'));
        CHECK( expected == result );

        // escaping turned off:
        wxString result2 = wxJoin(arrstr, wxT(','), wxT('\0'));
        CHECK( expected2 == result2 );
    }

    {
        // test is escape characters placed in the original array are just ignored as they should:
        const wxChar *arr[] = { wxT("first\\, wo\\rd"), wxT("seco\\nd"), wxT("\\third\\") };
        wxString expected = wxT("first\\\\, wo\\rd,seco\\nd,\\third\\");
        wxString expected2 = wxT("first\\, wo\\rd,seco\\nd,\\third\\");

        // escaping on:
        wxArrayString arrstr(WXSIZEOF(arr), arr);
        wxString result = wxJoin(arrstr, wxT(','), wxT('\\'));
        CHECK( expected == result );

        // escaping turned off:
        wxString result2 = wxJoin(arrstr, wxT(','), wxT('\0'));
        CHECK( expected2 == result2 );
    }
}

TEST_CASE("Arrays::SplitJoin", "[dynarray]")
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
        CHECK( str == wxJoin(arr, separators[i]) );
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
        CHECK( theArr == wxSplit(string, separators[i]) );
    }

    wxArrayString emptyArray;
    wxString string = wxJoin(emptyArray, wxT(';'));
    CHECK( string.empty() );

    CHECK( wxSplit(string, wxT(';')).empty() );

    CHECK( wxSplit(wxT(";"), wxT(';')).size() == 2 );
}

TEST_CASE("wxObjArray", "[dynarray]")
{
    {
        ArrayBars bars;
        Bar bar(wxT("first bar in general, second bar in array (two copies!)"));

        CHECK( bars.GetCount() == 0 );
        CHECK( Bar::GetNumber() == 1 );

        bars.Add(new Bar(wxT("first bar in array")));
        bars.Add(bar, 2);

        CHECK( bars.GetCount() == 3 );
        CHECK( Bar::GetNumber() == 4 );

        bars.RemoveAt(1, bars.GetCount() - 1);

        CHECK( bars.GetCount() == 1 );
        CHECK( Bar::GetNumber() == 2 );

        bars.Empty();

        CHECK( bars.GetCount() == 0 );
        CHECK( Bar::GetNumber() == 1 );
    }
    CHECK( Bar::GetNumber() == 0 );
}

TEST_CASE("wxObjArrayPtr", "[dynarray]")
{
    // Just check that instantiating this class compiles.
    ArrayBarPtrs barptrs;
    CHECK( barptrs.size() == 0 );
}

#define TestArrayOf(name)                                                     \
                                                                              \
TEST_CASE("wxDynArray::" #name, "[dynarray]")                                 \
{                                                                             \
    wxArray##name a;                                                          \
    a.Add(1);                                                                 \
    a.Add(17,2);                                                              \
    a.Add(5,3);                                                               \
    a.Add(3,4);                                                               \
                                                                              \
    CHECK((COMPARE_10_VALUES(a,1,17,17,5,5,5,3,3,3,3)));             \
    CHECK( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## Compare);                                                  \
                                                                              \
    CHECK((COMPARE_10_VALUES(a,1,3,3,3,3,5,5,5,17,17)));             \
    CHECK( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    a.Sort(name ## RevCompare);                                               \
                                                                              \
    CHECK((COMPARE_10_VALUES(a,17,17,5,5,5,3,3,3,3,1)));             \
    CHECK( COMPARE_COUNT( a , 10 ) );                                \
                                                                              \
    wxSortedArray##name b;                                                    \
                                                                              \
    b.Add(1);                                                                 \
    b.Add(17);                                                                \
    b.Add(5);                                                                 \
    b.Add(3);                                                                 \
                                                                              \
    CHECK((COMPARE_4_VALUES(b,1,3,5,17)));                           \
    CHECK( COMPARE_COUNT( b , 4 ) );                                 \
    CHECK( b.Index( 0 ) == wxNOT_FOUND );                            \
    CHECK( b.Index( 1 ) == 0 );                                      \
    CHECK( b.Index( 3 ) == 1 );                                      \
    CHECK( b.Index( 4 ) == wxNOT_FOUND );                            \
    CHECK( b.Index( 5 ) == 2 );                                      \
    CHECK( b.Index( 6 ) == wxNOT_FOUND );                            \
    CHECK( b.Index( 17 ) == 3 );                                     \
}

TestArrayOf(UShort)

TestArrayOf(Char)

TestArrayOf(Int)

TEST_CASE("wxDynArray::Alloc", "[dynarray]")
{
    wxArrayInt a;
    a.Add(17);
    a.Add(9);
    CHECK( a.GetCount() == 2 );

    a.Alloc(1000);

    CHECK( a.GetCount() == 2 );
    CHECK( a[0] == 17 );
    CHECK( a[1] == 9 );
}

TEST_CASE("wxDynArray::Clear", "[dynarray]")
{
    ItemPtrArray items;

    WX_CLEAR_ARRAY(items);
    CHECK( items.size() == 0 );

    items.push_back(new Item(17));
    items.push_back(new Item(71));
    CHECK( items.size() == 2 );

    WX_CLEAR_ARRAY(items);
    CHECK( items.size() == 0 );
}

namespace
{

template <typename A, typename T>
void DoTestSwap(T v1, T v2, T v3)
{
    A a1, a2;
    a1.swap(a2);
    CHECK( a1.empty() );
    CHECK( a2.empty() );

    a1.push_back(v1);
    a1.swap(a2);
    CHECK( a1.empty() );
    CHECK( a2.size() == 1 );

    a1.push_back(v2);
    a1.push_back(v3);
    a2.swap(a1);
    CHECK( a1.size() == 1 );
    CHECK( a2.size() == 2 );
    CHECK( a1[0] == v1 );
    CHECK( a2[1] == v3 );

    a1.swap(a2);
    CHECK( a1.size() == 2 );
    CHECK( a2.size() == 1 );
}

} // anonymous namespace

TEST_CASE("wxDynArray::Swap", "[dynarray]")
{
    DoTestSwap<wxArrayString>("Foo", "Bar", "Baz");

    DoTestSwap<wxArrayInt>(1, 10, 100);
    DoTestSwap<wxArrayLong>(6, 28, 496);
}

TEST_CASE("wxDynArray::TestSTL", "[dynarray]")
{
    wxArrayInt list1;
    wxArrayInt::iterator it, en;
    wxArrayInt::reverse_iterator rit, ren;
    int i;
    static const int COUNT = 5;

    for ( i = 0; i < COUNT; ++i )
        list1.push_back(i);

    CHECK( list1.capacity() >= (size_t)COUNT );
    CHECK( list1.size() == COUNT );

    for ( it = list1.begin(), en = list1.end(), i = 0;
          it != en; ++it, ++i )
    {
        CHECK( *it == i );
    }

    CHECK( i == COUNT );

    for ( rit = list1.rbegin(), ren = list1.rend(), i = COUNT;
          rit != ren; ++rit, --i )
    {
        CHECK( *rit == i-1 );
    }

    CHECK( i == 0 );

    CHECK( *list1.rbegin() == *(list1.end()-1) );
    CHECK( *list1.begin() == *(list1.rend()-1) );

    it = list1.begin()+1;
    rit = list1.rbegin()+1;
    CHECK( *list1.begin() == *(it-1) );
    CHECK( *list1.rbegin() == *(rit-1) );

    CHECK( list1.front() == 0 );
    CHECK( list1.back() == COUNT - 1 );

    list1.erase(list1.begin());
    list1.erase(list1.end()-1);

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CHECK( *it == i );
    }


    ItemPtrArray items;
    items.push_back(new Item(17));
    CHECK( (*(items.rbegin()))->n == 17 );
    CHECK( (**items.begin()).n == 17 );
    WX_CLEAR_ARRAY(items);
}

TEST_CASE("wxDynArray::IndexFromEnd", "[dynarray]")
{
    wxArrayInt a;
    a.push_back(10);
    a.push_back(1);
    a.push_back(42);

    CHECK( a.Index(10) == 0 );
    CHECK( a.Index(1) == 1 );
    CHECK( a.Index(42) == 2 );
    CHECK( a.Index(10, /*bFromEnd=*/true) == 0 );
    CHECK( a.Index( 1, /*bFromEnd=*/true) == 1 );
    CHECK( a.Index(42, /*bFromEnd=*/true) == 2 );
}


TEST_CASE("wxNaturalStringComparisonGeneric()", "[wxString][compare]")
{
    // simple string comparison
    CHECK(wxCmpNaturalGeneric("a", "a") == 0);
    CHECK(wxCmpNaturalGeneric("a", "z") < 0);
    CHECK(wxCmpNaturalGeneric("z", "a") > 0);

    // case insensitivity
    CHECK(wxCmpNaturalGeneric("a", "A") == 0);
    CHECK(wxCmpNaturalGeneric("A", "a") == 0);
    CHECK(wxCmpNaturalGeneric("AB", "a") > 0);
    CHECK(wxCmpNaturalGeneric("a", "AB") < 0);

    // empty strings sort before whitespace and punctiation
    CHECK(wxCmpNaturalGeneric("", " ") < 0);
    CHECK(wxCmpNaturalGeneric(" ", "") > 0);
    CHECK(wxCmpNaturalGeneric("", ",") < 0);
    CHECK(wxCmpNaturalGeneric(",", "") > 0);

    // empty strings sort before numbers
    CHECK(wxCmpNaturalGeneric("", "0") < 0);
    CHECK(wxCmpNaturalGeneric("0", "") > 0);

    // empty strings sort before letters and symbols
    CHECK(wxCmpNaturalGeneric("", "abc") < 0);
    CHECK(wxCmpNaturalGeneric("abc", "") > 0);

    // whitespace and punctiation sort before numbers
    CHECK(wxCmpNaturalGeneric(" ", "1") < 0);
    CHECK(wxCmpNaturalGeneric("1", " ") > 0);
    CHECK(wxCmpNaturalGeneric(",", "1") < 0);
    CHECK(wxCmpNaturalGeneric("1", ",") > 0);

    // strings containing numbers sort before letters and symbols
    CHECK(wxCmpNaturalGeneric("00", "a") < 0);
    CHECK(wxCmpNaturalGeneric("a", "00") > 0);

    // strings containing numbers are compared by their value
    CHECK(wxCmpNaturalGeneric("01", "1") == 0);
    CHECK(wxCmpNaturalGeneric("1", "01") == 0);
    CHECK(wxCmpNaturalGeneric("1", "05") < 0);
    CHECK(wxCmpNaturalGeneric("05", "1") > 0);
    CHECK(wxCmpNaturalGeneric("10", "5") > 0);
    CHECK(wxCmpNaturalGeneric("5", "10") < 0);
    CHECK(wxCmpNaturalGeneric("1", "9999999999999999999") < 0);
    CHECK(wxCmpNaturalGeneric("9999999999999999999", "1") > 0);

    // comparing strings composed from whitespace,
    //  punctuation, numbers, letters, and symbols
    CHECK(wxCmpNaturalGeneric("1st", " 1st") > 0);
    CHECK(wxCmpNaturalGeneric(" 1st", "1st") < 0);

    CHECK(wxCmpNaturalGeneric("1st", ",1st") > 0);
    CHECK(wxCmpNaturalGeneric(",1st", "1st") < 0);

    CHECK(wxCmpNaturalGeneric("1st", "01st") == 0);
    CHECK(wxCmpNaturalGeneric("01st", "1st") == 0);
    CHECK(wxCmpNaturalGeneric("10th", "5th") > 0);
    CHECK(wxCmpNaturalGeneric("5th", "10th") < 0);

    CHECK(wxCmpNaturalGeneric("a1st", "a01st") == 0);
    CHECK(wxCmpNaturalGeneric("a01st", "a1st") == 0);
    CHECK(wxCmpNaturalGeneric("a10th", "a5th") > 0);
    CHECK(wxCmpNaturalGeneric("a5th", "a10th") < 0);
    CHECK(wxCmpNaturalGeneric("a 10th", "a5th") < 0);
    CHECK(wxCmpNaturalGeneric("a5th", "a 10th") > 0);

    CHECK(wxCmpNaturalGeneric("a1st1", "a01st01") == 0);
    CHECK(wxCmpNaturalGeneric("a01st01", "a1st1") == 0);
    CHECK(wxCmpNaturalGeneric("a10th10", "a5th5") > 0);
    CHECK(wxCmpNaturalGeneric("a5th5", "a10th10") < 0);
    CHECK(wxCmpNaturalGeneric("a 10th 10", "a5th 5") < 0);
    CHECK(wxCmpNaturalGeneric("a5th 5", "a 10th 10") > 0);
}

