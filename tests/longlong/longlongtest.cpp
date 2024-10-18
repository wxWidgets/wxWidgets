///////////////////////////////////////////////////////////////////////////////
// Name:        tests/longlong/longlong.cpp
// Purpose:     wxLongLong unit test
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

#include "wx/longlong.h"
#include "wx/timer.h"

// ----------------------------------------------------------------------------
// helpers for testing
// ----------------------------------------------------------------------------

// number of iterations in loops
#define ITEMS 1000

// make a 64 bit number from 4 16 bit ones
#define MAKE_LL(x1, x2, x3, x4) wxLongLong((x1 << 16) | x2, (x3 << 16) | x3)

// get a random 64 bit number
#define RAND_LL()   MAKE_LL(rand(), rand(), rand(), rand())

static const long testLongs[] =
{
    0,
    1,
    -1,
    LONG_MAX,
    LONG_MIN,
    0x1234,
    -0x1234
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class LongLongTestCase : public CppUnit::TestCase
{
public:
    LongLongTestCase();

private:
    CPPUNIT_TEST_SUITE( LongLongTestCase );
        CPPUNIT_TEST( Conversion );
        CPPUNIT_TEST( Comparison );
        CPPUNIT_TEST( Addition );
        CPPUNIT_TEST( Multiplication );
        CPPUNIT_TEST( Division );
        CPPUNIT_TEST( BitOperations );
        CPPUNIT_TEST( ToString );
        CPPUNIT_TEST( LoHi );
        CPPUNIT_TEST( Limits );
    CPPUNIT_TEST_SUITE_END();

    void Conversion();
    void Comparison();
    void Addition();
    void Multiplication();
    void Division();
    void BitOperations();
    void ToString();
    void LoHi();
    void Limits();

    wxDECLARE_NO_COPY_CLASS(LongLongTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( LongLongTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( LongLongTestCase, "LongLongTestCase" );

LongLongTestCase::LongLongTestCase()
{
    srand((unsigned)time(nullptr));
}

void LongLongTestCase::Conversion()
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        wxLongLong a = RAND_LL();

        wxLongLong b(a.GetHi(), a.GetLo());
        CPPUNIT_ASSERT( a == b );
    }
}

void LongLongTestCase::Comparison()
{
    static const long ls[2] =
    {
        0x1234,
       -0x1234,
    };

    wxLongLong lls[2];
    lls[0] = ls[0];
    lls[1] = ls[1];

    for ( size_t n = 0; n < WXSIZEOF(testLongs); n++ )
    {
        for ( size_t m = 0; m < WXSIZEOF(lls); m++ )
        {
            CPPUNIT_ASSERT( (lls[m] < testLongs[n]) == (ls[m] < testLongs[n]) );
            CPPUNIT_ASSERT( (lls[m] > testLongs[n]) == (ls[m] > testLongs[n]) );
            CPPUNIT_ASSERT( (lls[m] <= testLongs[n]) == (ls[m] <= testLongs[n]) );
            CPPUNIT_ASSERT( (lls[m] >= testLongs[n]) == (ls[m] >= testLongs[n]) );
            CPPUNIT_ASSERT( (lls[m] != testLongs[n]) == (ls[m] != testLongs[n]) );
            CPPUNIT_ASSERT( (lls[m] == testLongs[n]) == (ls[m] == testLongs[n]) );
        }
    }
}

void LongLongTestCase::Addition()
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        wxLongLong a = RAND_LL();
        wxLongLong b = RAND_LL();
        wxLongLong c = a + b;

        CPPUNIT_ASSERT( c.GetValue() == a.GetValue() + b.GetValue() );
    }
}

void LongLongTestCase::Multiplication()
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        wxLongLong a = RAND_LL();
        wxLongLong b = RAND_LL();
        wxLongLong c = a*b;

        CPPUNIT_ASSERT( c.GetValue() == a.GetValue() * b.GetValue() );

        wxLongLong a1(a.GetHi(), a.GetLo());
        wxLongLong b1(b.GetHi(), b.GetLo());
        wxLongLong c1 = a1*b1;
        CPPUNIT_ASSERT( c1 == c );
    }
}

void LongLongTestCase::Division()
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        // get a random wxLongLong (shifting by 12 the MSB ensures that the
        // multiplication will not overflow)
        wxLongLong a = MAKE_LL((rand() >> 12), rand(), rand(), rand());

        // get a random (but non null) long (not wxLongLong for now) divider
        long l;
        do
        {
           l = rand();
        }
        while ( !l );

        wxLongLong q = a / l;
        wxLongLong r = a % l;

        CPPUNIT_ASSERT( a == ( q * l + r ) );
    }
}

void LongLongTestCase::BitOperations()
{
    for ( size_t m = 0; m < ITEMS; m++ )
    {
        wxLongLong a = RAND_LL();

        for ( size_t n = 0; n < 33; n++ )
        {
            wxLongLong b(a.GetHi(), a.GetLo()), c, d = b, e;
            d >>= n;
            c = b >> n;
            CPPUNIT_ASSERT( c == d );
            d <<= n;
            e = c << n;
            CPPUNIT_ASSERT( d == e );
        }
    }
}

void LongLongTestCase::ToString()
{
    wxString s1, s2;

    for ( size_t n = 0; n < WXSIZEOF(testLongs); n++ )
    {
        wxLongLong a = testLongs[n];
        s1 = wxString::Format(wxT("%ld"), testLongs[n]);
        s2 = a.ToString();
        CPPUNIT_ASSERT( s1 == s2 );

        s2 = wxEmptyString;
        s2 << a;
        CPPUNIT_ASSERT( s1 == s2 );
    }

    wxLongLong a(0x12345678, 0x87654321);
    CPPUNIT_ASSERT( a.ToString() == wxT("1311768467139281697") );
    a.Negate();
    CPPUNIT_ASSERT( a.ToString() == wxT("-1311768467139281697") );

    wxLongLong llMin(-2147483647L - 1L, 0);
    CPPUNIT_ASSERT( llMin.ToString() == wxT("-9223372036854775808") );
}

void LongLongTestCase::LoHi()
{
    wxLongLong ll(123, 456);
    CPPUNIT_ASSERT_EQUAL( 456u, ll.GetLo() );
    CPPUNIT_ASSERT_EQUAL( 123, ll.GetHi() );

    wxULongLong ull(987, 654);
    CPPUNIT_ASSERT_EQUAL( 654u, ull.GetLo() );
    CPPUNIT_ASSERT_EQUAL( 987u, ull.GetHi() );
}

void LongLongTestCase::Limits()
{
    CPPUNIT_ASSERT( std::numeric_limits<wxLongLong>::is_specialized );
    CPPUNIT_ASSERT( std::numeric_limits<wxULongLong>::is_specialized );

    wxULongLong maxval = std::numeric_limits<wxULongLong>::max();
    CPPUNIT_ASSERT( maxval.ToDouble() > 0 );
}
