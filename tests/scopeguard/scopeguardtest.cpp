///////////////////////////////////////////////////////////////////////////////
// Name:        tests/scopeguard/scopeguardtest.cpp
// Purpose:     Test wxScopeGuard and related macros
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ScopeGuardTestCase : public CppUnit::TestCase
{
public:
    CPPUNIT_TEST_SUITE(ScopeGuardTestCase);
        CPPUNIT_TEST(Normal);
        CPPUNIT_TEST(Dismiss);
        CPPUNIT_TEST(BlockExit);
        CPPUNIT_TEST(BlockExitObj);
    CPPUNIT_TEST_SUITE_END();

    void Normal();
    void Dismiss();
    void BlockExit();
    void BlockExitObj();
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(ScopeGuardTestCase);

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ScopeGuardTestCase,
                                      "ScopeGuardTestCase");

// ============================================================================
// ScopeGuardTestCase implementation
// ============================================================================

void ScopeGuardTestCase::Normal()
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

        CPPUNIT_ASSERT_EQUAL( 1, gs_count );
        CPPUNIT_ASSERT_EQUAL( 1, n );
        CPPUNIT_ASSERT_EQUAL( 2, m );
    }

    CPPUNIT_ASSERT_EQUAL( 2, gs_count );
    CPPUNIT_ASSERT_EQUAL( 2, n );
    CPPUNIT_ASSERT_EQUAL( 17, m );
}

void ScopeGuardTestCase::Dismiss()
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

        CPPUNIT_ASSERT_EQUAL( 1, gs_count );
        CPPUNIT_ASSERT_EQUAL( 1, n );
        CPPUNIT_ASSERT_EQUAL( 2, m );
    }

    CPPUNIT_ASSERT_EQUAL( 1, gs_count );
    CPPUNIT_ASSERT_EQUAL( 1, n );
    CPPUNIT_ASSERT_EQUAL( 2, m );
}

void ScopeGuardTestCase::BlockExit()
{
    int n = 1,
        m = 2;

    {
        gs_count = 1;

        wxON_BLOCK_EXIT0(IncGlobal);
        wxON_BLOCK_EXIT1(Inc, &n);
        wxON_BLOCK_EXIT2(IncBy, &m, 15);

        CPPUNIT_ASSERT_EQUAL( 1, gs_count );
        CPPUNIT_ASSERT_EQUAL( 1, n );
        CPPUNIT_ASSERT_EQUAL( 2, m );
    }

    CPPUNIT_ASSERT_EQUAL( 2, gs_count );
    CPPUNIT_ASSERT_EQUAL( 2, n );
    CPPUNIT_ASSERT_EQUAL( 17, m );
}

void ScopeGuardTestCase::BlockExitObj()
{
    Counter count0(1),
            count1(2),
            count2(3);

    {
        wxON_BLOCK_EXIT_OBJ0(count0, Counter::Zero);
        wxON_BLOCK_EXIT_OBJ1(count1, Counter::Set, 17);
        wxON_BLOCK_EXIT_OBJ2(count2, Counter::Sum, 2, 3);

        CPPUNIT_ASSERT_EQUAL( 1, count0.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 2, count1.GetCount() );
        CPPUNIT_ASSERT_EQUAL( 3, count2.GetCount() );
    }

    CPPUNIT_ASSERT_EQUAL( 0, count0.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 17, count1.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 5, count2.GetCount() );
}

