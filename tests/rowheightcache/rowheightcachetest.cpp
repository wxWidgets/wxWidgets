///////////////////////////////////////////////////////////////////////////////
// Name:        tests/rowheightcache/rowheightcachetest.cpp
// Purpose:     unit tests for the row height cache of a dataview
// Author:      Jens Goepfert (mail@jensgoepfert.de)
// Created:     2018-03-06
// Copyright:   (c) wxWidgets team
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
#endif

#include "wx/generic/private/rowheightcache.h"

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class RowHeightCacheTestCase : public CppUnit::TestCase
{
public:
    RowHeightCacheTestCase() { }

    virtual void setUp();
    virtual void tearDown();

protected:

private:
    CPPUNIT_TEST_SUITE( RowHeightCacheTestCase );
        CPPUNIT_TEST(TestRowRangesSimple);
        CPPUNIT_TEST(TestRowRangesGapsMod2);
        CPPUNIT_TEST(TestRowRangesCleanUp1);
        CPPUNIT_TEST(TestRowRangesCleanUp2);
        CPPUNIT_TEST(TestHeightCache);
    CPPUNIT_TEST_SUITE_END();

    void TestRowRangesSimple();
    void TestRowRangesGapsMod2();
    void TestHeightCache();
    void TestRowRangesCleanUp1();
    void TestRowRangesCleanUp2();

    wxDECLARE_NO_COPY_CLASS(RowHeightCacheTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RowHeightCacheTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RowHeightCacheTestCase,
                                        "RowHeightCacheTestCase" );

void RowHeightCacheTestCase::setUp()
{
}

void RowHeightCacheTestCase::tearDown()
{
}

// ----------------------------------------------------------------------------
// TestRowRangesAdd
// ----------------------------------------------------------------------------
void RowHeightCacheTestCase::TestRowRangesSimple()
{
    RowRanges *rr = new RowRanges();

    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 0);

    for (unsigned int i = 0; i <= 10; i++)
    {
        CPPUNIT_ASSERT_EQUAL(rr->Has(i), false);

        rr->Add(i);

        CPPUNIT_ASSERT_EQUAL(rr->CountAll(), i+1);
        CPPUNIT_ASSERT_EQUAL(rr->CountTo(i), i);
        CPPUNIT_ASSERT_EQUAL(rr->Has(i), true);
    }

    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1); // every row is sorted in the same range, so count == 1
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 11); // 11 rows collected
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(10), 10);

    rr->Add(5); // row 5 already contained -> does nothing

    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1); // every row is sorted in the same range, so count == 1
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 11); // 11 rows collected
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(10), 10);

    for (int i = 10; i >= 0; i--)
    {
        CPPUNIT_ASSERT_EQUAL(rr->CountAll(), (unsigned)i+1);
        CPPUNIT_ASSERT_EQUAL(rr->CountTo((unsigned)i), (unsigned)i);

        rr->Remove(i);

        CPPUNIT_ASSERT_EQUAL(rr->CountAll(), (unsigned)i);
        CPPUNIT_ASSERT_EQUAL(rr->CountTo((unsigned)i), (unsigned)i);
    }

    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 0); // everything removed, no row range is left behind
    for (int i = 10; i > 0; i--)
    {
        CPPUNIT_ASSERT_EQUAL(rr->CountTo(i), 0);
    }
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 0);
}

// ----------------------------------------------------------------------------
// TestRowRangesRemove
// ----------------------------------------------------------------------------
void RowHeightCacheTestCase::TestRowRangesGapsMod2()
{
    RowRanges *rr = new RowRanges();
    for (int i = 0; i < 100; i++)
    {
        CPPUNIT_ASSERT_EQUAL(rr->Has(i), false);

        if (i % 2 == 0)
        {
            rr->Add(i);
        }
    }
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 50);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 50);

    for (unsigned int i = 99; i > 0; i--)
    {
        if (i % 2 == 0)
        {
            CPPUNIT_ASSERT_EQUAL(rr->Has(i), true);
            rr->Remove(i);
            CPPUNIT_ASSERT_EQUAL(rr->Has(i), false);
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(rr->Has(i), false);
        }
    }

    // only row 0 is in the RowRanges, so remove 1 does nothing
    rr->Remove(1);

    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 1);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(0), 0);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(1), 1);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 1);
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1);

    rr->Remove(0); // last row is beeing removed

    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 0);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(0), 0);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(1), 0);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 0);
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 0);

    rr->Add(10);
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(1), 0); // tests CountTo first break
    rr->Add(5); // inserts a range at the beginning
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 2);
    rr->Remove(10);
    rr->Remove(5);
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 0);
}

// ----------------------------------------------------------------------------
// TestRowRangesCleanUp1
// ----------------------------------------------------------------------------
void RowHeightCacheTestCase::TestRowRangesCleanUp1()
{
    RowRanges *rr = new RowRanges();
    for (unsigned int i = 0; i < 100; i++)
    {
        CPPUNIT_ASSERT_EQUAL(rr->Has(i), false);

        if (i % 2 == 0)
        {
            rr->Add(i);
        }
    }
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 50); // adding 50 rows (only even) results in 50 range objects
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 50);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 50);

    for (unsigned int i = 0; i < 100; i++)
    {
        if (i % 2 == 1)
        {
            rr->Add(i);
        }
    }

    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1); // adding 50 rows (only odd) should combined to 1 range object
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 100);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 100);
}

// ----------------------------------------------------------------------------
// TestRowRangesCleanUp2
// ----------------------------------------------------------------------------
void RowHeightCacheTestCase::TestRowRangesCleanUp2()
{
    RowRanges *rr = new RowRanges();
    for (unsigned int i = 0; i < 10; i++)
    {
        rr->Add(i);
    }
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1); // adding 10 sequent rows results in 1 range objects
    CPPUNIT_ASSERT_EQUAL(rr->CountAll(), 10);
    CPPUNIT_ASSERT_EQUAL(rr->CountTo(100), 10);

    for (unsigned int i = 12; i < 20; i++)
    {
        rr->Add(i);
    }

    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 2);
    rr->Add(11); // tests extending a range at the beginning (to the left)
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 2);
    rr->Add(10); // extends a range and reduces them
    CPPUNIT_ASSERT_EQUAL(rr->GetSize(), 1);
}

// ----------------------------------------------------------------------------
// TestHeightCache
// ----------------------------------------------------------------------------
void RowHeightCacheTestCase::TestHeightCache()
{
    HeightCache *hc = new HeightCache();

    for (unsigned int i = 0; i <= 10; i++)
    {
        hc->Put(i, 22);
    }
    for (unsigned int i = 15; i <= 17; i++)
    {
        hc->Put(i, 22);
    }
    for (unsigned int i = 20; i <= 2000; i++)
    {
        hc->Put(i, 22);
    }

    hc->Put(11, 42);
    hc->Put(12, 42);
    hc->Put(18, 42);

    hc->Put(13, 62);
    hc->Put(14, 62);
    hc->Put(19, 62);

    int start = 0;
    int height = 0;
    unsigned int row = 666;

    CPPUNIT_ASSERT_EQUAL(hc->GetLineStart(1000, start), true);
    CPPUNIT_ASSERT_EQUAL(start, 22180);

    CPPUNIT_ASSERT_EQUAL(hc->GetLineHeight(1000, height), true);
    CPPUNIT_ASSERT_EQUAL(height, 22);

    CPPUNIT_ASSERT_EQUAL(hc->GetLineHeight(5000, start), false);

    // test invalid y
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(-1, row), false);
    CPPUNIT_ASSERT_EQUAL(row, 666);

    // test start of first row
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(0, row), true);
    CPPUNIT_ASSERT_EQUAL(row, 0);

    // test end of first row
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(21, row), true);
    CPPUNIT_ASSERT_EQUAL(row, 0);

    // test start of second row
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(22, row), true);
    CPPUNIT_ASSERT_EQUAL(row, 1);

    hc->Remove(1000); // Delete row 1000 and everything behind

    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(22179, row), true);
    CPPUNIT_ASSERT_EQUAL(row, 999);
    CPPUNIT_ASSERT_EQUAL(hc->GetLineHeight(999, height), true);
    CPPUNIT_ASSERT_EQUAL(height, 22);

    row = 666;
    height = 666;
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(22180, row), false);
    CPPUNIT_ASSERT_EQUAL(row, 666);
    CPPUNIT_ASSERT_EQUAL(hc->GetLineHeight(1000, height), false);
    CPPUNIT_ASSERT_EQUAL(height, 666);

    hc->Clear(); // Clear all items
    for (int i = 20; i <= 2000; i++)
    {
        height = 666;
        CPPUNIT_ASSERT_EQUAL(hc->GetLineHeight(i, height), false);
        CPPUNIT_ASSERT_EQUAL(height, 666);
    }

    hc->Clear(); // Clear twice should not crash

    row = 666;
    height = 666;
    CPPUNIT_ASSERT_EQUAL(hc->GetLineAt(22180, row), false);
    CPPUNIT_ASSERT_EQUAL(row, 666);
}
