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

// get a random 64 bit number
#define RAND_LL()   wxLongLong(rand(), rand())

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

// Seed the random number generator used by RAND_LL() to use different values
// in the different runs.
static const struct SeedRand
{
    SeedRand() { srand((unsigned)time(nullptr)); }
} gs_seedRand;

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("LongLong::Conversion", "[longlong]")
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        wxLongLong a = RAND_LL();

        wxLongLong b(a.GetHi(), a.GetLo());
        CHECK( a == b );
    }
}

TEST_CASE("LongLong::Comparison", "[longlong]")
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
            CHECK( (lls[m] < testLongs[n]) == (ls[m] < testLongs[n]) );
            CHECK( (lls[m] > testLongs[n]) == (ls[m] > testLongs[n]) );
            CHECK( (lls[m] <= testLongs[n]) == (ls[m] <= testLongs[n]) );
            CHECK( (lls[m] >= testLongs[n]) == (ls[m] >= testLongs[n]) );
            CHECK( (lls[m] != testLongs[n]) == (ls[m] != testLongs[n]) );
            CHECK( (lls[m] == testLongs[n]) == (ls[m] == testLongs[n]) );
        }
    }
}

TEST_CASE("LongLong::Addition", "[longlong]")
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        // Avoid overflow by using numbers less than half of the maximum value.
        wxLongLong a = RAND_LL() / 2;
        wxLongLong b = RAND_LL() / 2;
        wxLongLong c = a + b;

        CHECK( c.GetValue() == a.GetValue() + b.GetValue() );
    }
}

TEST_CASE("LongLong::Multiplication", "[longlong]")
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        // Avoid signed integer overflow by multiplying unsigned numbers only.
        wxULongLong a = RAND_LL().GetValue();
        wxULongLong b = RAND_LL().GetValue();
        wxULongLong c = a*b;

        CHECK( c.GetValue() == a.GetValue() * b.GetValue() );

        wxULongLong a1(a.GetHi(), a.GetLo());
        wxULongLong b1(b.GetHi(), b.GetLo());
        wxULongLong c1 = a1*b1;
        CHECK( c1 == c );
    }
}

TEST_CASE("LongLong::Division", "[longlong]")
{
    for ( size_t n = 0; n < ITEMS; n++ )
    {
        wxLongLong a = RAND_LL();

        // get a random (but non null) long (not wxLongLong for now) divider
        long l;
        do
        {
           l = rand();
        }
        while ( !l );

        wxLongLong q = a / l;
        wxLongLong r = a % l;

        CHECK( a == ( q * l + r ) );
    }
}

TEST_CASE("LongLong::BitOperations", "[longlong]")
{
    for ( size_t m = 0; m < ITEMS; m++ )
    {
        wxLongLong a = RAND_LL();

        for ( size_t n = 0; n < 33; n++ )
        {
            wxLongLong b(a.GetHi(), a.GetLo()), c, d = b, e;
            d >>= n;
            c = b >> n;
            CHECK( c == d );
            d <<= n;
            e = c << n;
            CHECK( d == e );
        }
    }
}

TEST_CASE("LongLong::ToString", "[longlong]")
{
    wxString s1, s2;

    for ( size_t n = 0; n < WXSIZEOF(testLongs); n++ )
    {
        wxLongLong a = testLongs[n];
        s1 = wxString::Format(wxT("%ld"), testLongs[n]);
        s2 = a.ToString();
        CHECK( s1 == s2 );

        s2 = wxEmptyString;
        s2 << a;
        CHECK( s1 == s2 );
    }

    wxLongLong a(0x12345678, 0x87654321);
    CHECK( a.ToString() == wxT("1311768467139281697") );
    a.Negate();
    CHECK( a.ToString() == wxT("-1311768467139281697") );

    wxLongLong llMin(-2147483647L - 1L, 0);
    CHECK( llMin.ToString() == wxT("-9223372036854775808") );
}

TEST_CASE("LongLong::LoHi", "[longlong]")
{
    wxLongLong ll(123, 456);
    CHECK( ll.GetLo() == 456u );
    CHECK( ll.GetHi() == 123 );

    wxULongLong ull(987, 654);
    CHECK( ull.GetLo() == 654u );
    CHECK( ull.GetHi() == 987u );
}

TEST_CASE("LongLong::Limits", "[longlong]")
{
    CHECK( std::numeric_limits<wxLongLong>::is_specialized );
    CHECK( std::numeric_limits<wxULongLong>::is_specialized );

    wxULongLong maxval = std::numeric_limits<wxULongLong>::max();
    CHECK( maxval.ToDouble() > 0 );
}
