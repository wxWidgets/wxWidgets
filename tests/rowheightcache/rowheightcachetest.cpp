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


#ifndef WX_PRECOMP
#endif

#include "wx/generic/private/rowheightcache.h"

// ----------------------------------------------------------------------------
// TestRowRangesAdd
// ----------------------------------------------------------------------------
TEST_CASE("RowHeightCacheTestCase::TestRowRangesSimple", "[dataview][heightcache]")
{
    RowRanges rr;

    CHECK(rr.CountAll() == 0);

    for (unsigned int i = 0; i <= 10; i++)
    {
        CHECK(rr.Has(i) == false);

        rr.Add(i);

        CHECK(rr.CountAll() == i+1);
        CHECK(rr.CountTo(i) == i);
        CHECK(rr.Has(i) == true);
    }

    CHECK(rr.GetSize() == 1); // every row is sorted in the same range, so count == 1
    CHECK(rr.CountAll() == 11); // 11 rows collected
    CHECK(rr.CountTo(10) == 10);

    rr.Add(5); // row 5 already contained -> does nothing

    CHECK(rr.GetSize() == 1); // every row is sorted in the same range, so count == 1
    CHECK(rr.CountAll() == 11); // 11 rows collected
    CHECK(rr.CountTo(10) == 10);

    for (int i = 10; i >= 0; i--)
    {
        CHECK(rr.CountAll() == (unsigned)i+1);
        CHECK(rr.CountTo((unsigned)i) == (unsigned)i);

        rr.Remove(i);

        CHECK(rr.CountAll() == (unsigned)i);
        CHECK(rr.CountTo((unsigned)i) == (unsigned)i);
    }

    CHECK(rr.CountAll() == 0); // everything removed, no row range is left behind
    for (int i = 10; i > 0; i--)
    {
        CHECK(rr.CountTo(i) == 0);
    }
    CHECK(rr.GetSize() == 0);
}

// ----------------------------------------------------------------------------
// TestRowRangesRemove
// ----------------------------------------------------------------------------
TEST_CASE("RowHeightCacheTestCase::TestRowRangesGapsMod2", "[dataview][heightcache]")
{
    RowRanges rr;
    for (int i = 0; i < 100; i++)
    {
        CHECK(rr.Has(i) == false);

        if (i % 2 == 0)
        {
            rr.Add(i);
        }
    }
    CHECK(rr.CountAll() == 50);
    CHECK(rr.CountTo(100) == 50);

    for (unsigned int i = 99; i > 0; i--)
    {
        if (i % 2 == 0)
        {
            CHECK(rr.Has(i) == true);
            rr.Remove(i);
            CHECK(rr.Has(i) == false);
        }
        else
        {
            CHECK(rr.Has(i) == false);
        }
    }

    // only row 0 is in the RowRanges, so remove 1 does nothing
    rr.Remove(1);

    CHECK(rr.CountAll() == 1);
    CHECK(rr.CountTo(0) == 0);
    CHECK(rr.CountTo(1) == 1);
    CHECK(rr.CountTo(100) == 1);
    CHECK(rr.GetSize() == 1);

    rr.Remove(0); // last row is beeing removed

    CHECK(rr.CountAll() == 0);
    CHECK(rr.CountTo(0) == 0);
    CHECK(rr.CountTo(1) == 0);
    CHECK(rr.CountTo(100) == 0);
    CHECK(rr.GetSize() == 0);

    rr.Add(10);
    CHECK(rr.GetSize() == 1);
    CHECK(rr.CountTo(1) == 0); // tests CountTo first break
    rr.Add(5); // inserts a range at the beginning
    CHECK(rr.GetSize() == 2);
    rr.Remove(10);
    rr.Remove(5);
    CHECK(rr.GetSize() == 0);
}

// ----------------------------------------------------------------------------
// TestRowRangesCleanUp1
// ----------------------------------------------------------------------------
TEST_CASE("RowHeightCacheTestCase::TestRowRangesCleanUp1", "[dataview][heightcache]")
{
    RowRanges rr;
    for (unsigned int i = 0; i < 100; i++)
    {
        CHECK(rr.Has(i) == false);

        if (i % 2 == 0)
        {
            rr.Add(i);
        }
    }
    CHECK(rr.GetSize() == 50); // adding 50 rows (only even) results in 50 range objects
    CHECK(rr.CountAll() == 50);
    CHECK(rr.CountTo(100) == 50);

    for (unsigned int i = 0; i < 100; i++)
    {
        if (i % 2 == 1)
        {
            rr.Add(i);
        }
    }

    CHECK(rr.GetSize() == 1); // adding 50 rows (only odd) should combined to 1 range object
    CHECK(rr.CountAll() == 100);
    CHECK(rr.CountTo(100) == 100);
}

// ----------------------------------------------------------------------------
// TestRowRangesCleanUp2
// ----------------------------------------------------------------------------
TEST_CASE("RowHeightCacheTestCase::TestRowRangesCleanUp2", "[dataview][heightcache]")
{
    RowRanges rr;
    for (unsigned int i = 0; i < 10; i++)
    {
        rr.Add(i);
    }
    CHECK(rr.GetSize() == 1); // adding 10 sequent rows results in 1 range objects
    CHECK(rr.CountAll() == 10);
    CHECK(rr.CountTo(100) == 10);

    for (unsigned int i = 12; i < 20; i++)
    {
        rr.Add(i);
    }

    CHECK(rr.GetSize() == 2);
    rr.Add(11); // tests extending a range at the beginning (to the left)
    CHECK(rr.GetSize() == 2);
    rr.Add(10); // extends a range and reduces them
    CHECK(rr.GetSize() == 1);
}

// ----------------------------------------------------------------------------
// TestHeightCache
// ----------------------------------------------------------------------------
TEST_CASE("RowHeightCacheTestCase::TestHeightCache", "[dataview][heightcache]")
{
    HeightCache hc;

    for (unsigned int i = 0; i <= 10; i++)
    {
        hc.Put(i, 22);
    }
    for (unsigned int i = 15; i <= 17; i++)
    {
        hc.Put(i, 22);
    }
    for (unsigned int i = 20; i <= 2000; i++)
    {
        hc.Put(i, 22);
    }

    hc.Put(11, 42);
    hc.Put(12, 42);
    hc.Put(18, 42);

    hc.Put(13, 62);
    hc.Put(14, 62);
    hc.Put(19, 62);

    int start = 0;
    int height = 0;
    unsigned int row = 666;

    CHECK(hc.GetLineStart(1000, start) == true);
    CHECK(start == 22180);

    CHECK(hc.GetLineHeight(1000, height) == true);
    CHECK(height == 22);

    CHECK(hc.GetLineHeight(5000, start) == false);

    // test invalid y
    CHECK(hc.GetLineAt(-1, row) == false);
    CHECK(row == 666);

    // test start of first row
    CHECK(hc.GetLineAt(0, row) == true);
    CHECK(row == 0);

    // test end of first row
    CHECK(hc.GetLineAt(21, row) == true);
    CHECK(row == 0);

    // test start of second row
    CHECK(hc.GetLineAt(22, row) == true);
    CHECK(row == 1);

    hc.Remove(1000); // Delete row 1000 and everything behind

    CHECK(hc.GetLineAt(22179, row) == true);
    CHECK(row == 999);
    CHECK(hc.GetLineHeight(999, height) == true);
    CHECK(height == 22);

    row = 666;
    height = 666;
    CHECK(hc.GetLineAt(22180, row) == false);
    CHECK(row == 666);
    CHECK(hc.GetLineHeight(1000, height) == false);
    CHECK(height == 666);

    hc.Clear(); // Clear all items
    for (int i = 20; i <= 2000; i++)
    {
        height = 666;
        CHECK(hc.GetLineHeight(i, height) == false);
        CHECK(height == 666);
    }

    hc.Clear(); // Clear twice should not crash

    row = 666;
    height = 666;
    CHECK(hc.GetLineAt(22180, row) == false);
    CHECK(row == 666);
}
