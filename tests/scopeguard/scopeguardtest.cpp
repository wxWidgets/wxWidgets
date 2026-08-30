///////////////////////////////////////////////////////////////////////////////
// Name:        tests/scopeguard/scopeguardtest.cpp
// Purpose:     Test wxScopeGuard and related macros
// Author:      Vadim Zeitlin
// Copyright:   (c) 2005 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/string.h"
#include "wx/scopeguard.h"

// ----------------------------------------------------------------------------
// helper stuff: something to do on scope exit
// ----------------------------------------------------------------------------

static int gs_count = 0;

static void IncGlobal() { gs_count++; }
static void Inc(int *n) { (*n)++; }
static void IncBy(int *n, int m) { (*n) += m; }

class Counter
{
public:
    Counter(int n) : m_count(n) { }

    void Zero() { m_count = 0; }
    void Set(int n) { m_count = n; }
    void Sum(int n, int m) { m_count = n + m; }

    int GetCount() const { return m_count; }

private:
    int m_count;
};

// Fixture used by the tests of the macros operating on "this" below.
class CounterFixture
{
public:
    void Zero() { m_count = 0; }
    void Set(int n) { m_count = n; }
    void Sum(int n, int m) { m_count = n + m; }

    int m_count = 0;
};

// ============================================================================
// tests
// ============================================================================

TEST_CASE("ScopeGuard::Normal", "[scopeguard]")
{
    int n = 1,
        m = 2;

    {
        gs_count = 1;
        wxScopeGuard incGlobal = wxMakeGuard(IncGlobal),
                     incN = wxMakeGuard(Inc, &n),
                     incMby15 = wxMakeGuard(IncBy, &m, 15);

        wxUnusedVar(incGlobal);
        wxUnusedVar(incN);
        wxUnusedVar(incMby15);

        CHECK( gs_count == 1 );
        CHECK( n == 1 );
        CHECK( m == 2 );
    }

    CHECK( gs_count == 2 );
    CHECK( n == 2 );
    CHECK( m == 17 );
}

TEST_CASE("ScopeGuard::Dismiss", "[scopeguard]")
{
    int n = 1,
        m = 2;

    {
        gs_count = 1;
        wxScopeGuard incGlobal = wxMakeGuard(IncGlobal),
                     incN = wxMakeGuard(Inc, &n),
                     incMby15 = wxMakeGuard(IncBy, &m, 15);

        incGlobal.Dismiss();
        incN.Dismiss();
        incMby15.Dismiss();

        CHECK( gs_count == 1 );
        CHECK( n == 1 );
        CHECK( m == 2 );
    }

    CHECK( gs_count == 1 );
    CHECK( n == 1 );
    CHECK( m == 2 );
}

TEST_CASE("ScopeGuard::BlockExit", "[scopeguard]")
{
    int n = 1,
        m = 2;

    {
        gs_count = 1;

        wxON_BLOCK_EXIT0(IncGlobal);
        wxON_BLOCK_EXIT1(Inc, &n);
        wxON_BLOCK_EXIT2(IncBy, &m, 15);

        CHECK( gs_count == 1 );
        CHECK( n == 1 );
        CHECK( m == 2 );
    }

    CHECK( gs_count == 2 );
    CHECK( n == 2 );
    CHECK( m == 17 );
}

TEST_CASE("ScopeGuard::BlockExitObj", "[scopeguard]")
{
    Counter count0(1),
            count1(2),
            count2(3);

    {
        wxON_BLOCK_EXIT_OBJ0(count0, Counter::Zero);
        wxON_BLOCK_EXIT_OBJ1(count1, Counter::Set, 17);
        wxON_BLOCK_EXIT_OBJ2(count2, Counter::Sum, 2, 3);

        CHECK( count0.GetCount() == 1 );
        CHECK( count1.GetCount() == 2 );
        CHECK( count2.GetCount() == 3 );
    }

    CHECK( count0.GetCount() == 0 );
    CHECK( count1.GetCount() == 17 );
    CHECK( count2.GetCount() == 5 );
}

TEST_CASE_METHOD(CounterFixture, "ScopeGuard::BlockExitThis", "[scopeguard]")
{
    m_count = 1;

    {
        wxON_BLOCK_EXIT_THIS0(CounterFixture::Zero);

        CHECK( m_count == 1 );
    }
    CHECK( m_count == 0 );

    {
        wxON_BLOCK_EXIT_THIS1(CounterFixture::Set, 17);

        CHECK( m_count == 0 );
    }
    CHECK( m_count == 17 );

    {
        wxON_BLOCK_EXIT_THIS2(CounterFixture::Sum, 2, 3);
        CHECK( m_count == 17 );
    }
    CHECK( m_count == 5 );
}

TEST_CASE_METHOD(CounterFixture, "ScopeGuard::BlockExitSetVar", "[scopeguard]")
{
    m_count = 1;
    {
        wxON_BLOCK_EXIT_SET(m_count, 17);

        CHECK( m_count == 1 );
    }
    CHECK( m_count == 17 );


    int count = 1;
    {
        wxON_BLOCK_EXIT_SET(count, 17);

        CHECK( count == 1 );
    }
    CHECK( count == 17 );


    wxString s("hi");
    {
        wxON_BLOCK_EXIT_SET(s, "bye");

        CHECK( s == "hi" );
    }
    CHECK( s == "bye" );

    CounterFixture *p = this;
    {
        wxON_BLOCK_EXIT_NULL(p);

        CHECK( p );
    }
    CHECK( !p );
}
