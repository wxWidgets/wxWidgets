///////////////////////////////////////////////////////////////////////////////
// Name:        tests/geometry/region.cpp
// Purpose:     wxRegion unit test
// Author:      Vadim Zeitlin
// Created:     2011-10-12
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/region.h"
#endif // WX_PRECOMP

#include "wx/iosfwrap.h"

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{

// This function could be easily added to wxRegionIterator itself, where it
// could be implemented far more efficiently as all major platforms store the
// number of rectangles anyhow, but as we only use it for debugging purposes,
// just keep it here for now.
unsigned GetRectsCount(const wxRegion& rgn)
{
    unsigned count = 0;
    for ( wxRegionIterator iter(rgn); iter.HaveRects(); ++iter )
        count++;
    return count;
}

} // anonymous namespace

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxRegions
std::ostream& operator<<(std::ostream& os, const wxRegion& rgn)
{
    wxRect r = rgn.GetBox();
    os << "# rects = " << GetRectsCount(rgn)
       << "; bounding box {"
       << r.x << ", " << r.y << ", " << r.width << ", " << r.height
       << "}";
    return os;
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class RegionTestCase : public CppUnit::TestCase
{
public:
    RegionTestCase() { }

private:
    CPPUNIT_TEST_SUITE( RegionTestCase );
        CPPUNIT_TEST( Validity );
        CPPUNIT_TEST( Union );
        CPPUNIT_TEST( Intersect );
        CPPUNIT_TEST( Clear );
        CPPUNIT_TEST( Subtract );
        CPPUNIT_TEST( Xor );
        CPPUNIT_TEST( Contains );
        CPPUNIT_TEST( IsEqual );
    CPPUNIT_TEST_SUITE_END();

    void Validity();
    void Union();
    void Intersect();
    void Clear();
    void Subtract();
    void Xor();
    void Contains();
    void IsEqual();

    wxDECLARE_NO_COPY_CLASS(RegionTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RegionTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RegionTestCase, "RegionTestCase" );

void RegionTestCase::Validity()
{
    wxRegion r;

    CPPUNIT_ASSERT_MESSAGE
    (
        "Default constructed region must be invalid",
        !r.IsOk()
    );

    CPPUNIT_ASSERT_MESSAGE
    (
        "Invalid region should be empty",
        r.IsEmpty()
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "GetBox of invalid region should return zero-sized",
        r.GetBox(),
        wxRect(0, 0, 0, 0)
    );

    int x, y, w, h;
    r.GetBox(x, y, w, h);
    CPPUNIT_ASSERT_MESSAGE
    (
        "GetBox for invalid region should return zero-sized box values",
        (x == 0 && y == 0 && w == -1 && h == -1)
    );

    // Offsetting an invalid region doesn't make sense.
    WX_ASSERT_FAILS_WITH_ASSERT( r.Offset(1, 1) );
}

void RegionTestCase::Union()
{
    wxRegion r;

    CPPUNIT_ASSERT_MESSAGE
    (
        "Combining with a valid region should create a valid region",
        r.Union(0, 0, 10, 10)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Union() with invalid region should give the same region",
        wxRegion(0, 0, 10, 10),
        r
    );

    r.Clear();

    wxRect rect(0, 0, 2, 2);
    CPPUNIT_ASSERT_MESSAGE
    (
        "Combining invalid region with a rectangle should create a valid region",
        r.Union(rect)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Union() with rectangle should make region equal to the rectangle",
        wxRegion(0, 0, 2, 2),
        r
    );

    wxRegion invalid;

    CPPUNIT_ASSERT_MESSAGE
    (
        "Combining invalid region with valid region should create valid region",
        invalid.Union(r)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Union() of invalid region with valid region should match valid region",
        wxRegion(0, 0, 2, 2),
        invalid
    );
}

void RegionTestCase::Intersect()
{
    const wxPoint points1[] = {
        wxPoint(310, 392),
        wxPoint(270, 392),
        wxPoint(270, 421),
        wxPoint(310, 421)
    };

    wxRegion region1(WXSIZEOF(points1), points1);

    const wxPoint points2[] = {
        wxPoint(54, 104),
        wxPoint(85,  82),
        wxPoint(68,  58),
        wxPoint(37,  80)
    };

    wxRegion region2(4,points2);

    CPPUNIT_ASSERT( region1.Intersect(region2) );
    CPPUNIT_ASSERT( region1.IsEmpty() );

    wxRegion invalid;

    CPPUNIT_ASSERT_MESSAGE(
        "Intersecting an invalid region fails safely",
        !invalid.Intersect(region2)
    );
}

void RegionTestCase::Clear()
{
    const wxPoint points[] = {
        wxPoint(1, 2),
        wxPoint(5, 2),
        wxPoint(3, 3)
    };

    wxRegion region(WXSIZEOF(points), points);
    CPPUNIT_ASSERT(region.IsOk());

    region.Clear();

    CPPUNIT_ASSERT(!region.IsOk());
}

void RegionTestCase::Subtract()
{
    wxRegion region;
    wxRect rect(0, 0, 3, 3);

    CPPUNIT_ASSERT_MESSAGE(
        "Subtract rect from invalid region should fail safely.",
        !region.Subtract(rect)
    );

    wxRegion rectRegion(rect);
    CPPUNIT_ASSERT_MESSAGE(
        "Subtract region from invalid region should fail safely.",
        !region.Subtract(rectRegion)
    );
}

void RegionTestCase::Xor()
{
    wxRegion region;
    CPPUNIT_ASSERT_MESSAGE(
        "Xor on invalid region should succeed.",
        region.Xor(1, 3, 2, 2)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Xor on invalid region should set invalid region to rectangle.",
        wxRegion(1, 3, 2, 2),
        region
    );

    region.Clear();

    CPPUNIT_ASSERT_MESSAGE(
        "Xor rectangle on invalid region should succeed.",
        region.Xor(wxRect(1, 3, 2, 2))
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Xor on invalid region should set invalid region to rectangle.",
        wxRegion(1, 3, 2, 2),
        region
    );

    region.Clear();
    wxRegion valid_region(wxRect(1, 3, 2, 2));

    CPPUNIT_ASSERT_MESSAGE(
        "Xor region on invalid region should succeed.",
        region.Xor(valid_region)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Xor on invalid region should set invalid region to region.",
        wxRegion(1, 3, 2, 2),
        region
    );
}

void RegionTestCase::Contains()
{
    wxRegion region;

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Contains on an invalid region should return wxOutRegion.",
        wxOutRegion,
        region.Contains(1, 3)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Contains on an invalid region should return wxOutRegion.",
        wxOutRegion,
        region.Contains(wxPoint(1, 3))
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Contains on an invalid region should return wxOutRegion.",
        wxOutRegion,
        region.Contains(1, 3, 2, 2)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Contains on an invalid region should return wxOutRegion.",
        wxOutRegion,
        region.Contains(wxRect(1, 3, 2, 2))
    );
}

void RegionTestCase::IsEqual()
{
    wxRegion r1, r2;
    CPPUNIT_ASSERT_MESSAGE(
        "Two invalid regions are equal.",
        r1.IsEqual(r2)
    );

    r2.Union(0, 0, 1, 1);
    CPPUNIT_ASSERT_MESSAGE(
        "Invalid region is not equal to valid.",
        !r1.IsEqual(r2)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "Valid region is not equal to invalid.",
        !r2.IsEqual(r1)
    );

    r1.Union(r2);
    CPPUNIT_ASSERT_MESSAGE(
        "Valid regions can be equal.",
        r2.IsEqual(r1)
    );
}
