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
        CPPUNIT_TEST( Intersect );
    CPPUNIT_TEST_SUITE_END();

    void Validity();
    void Intersect();

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

    // Offsetting an invalid region doesn't make sense.
    WX_ASSERT_FAILS_WITH_ASSERT( r.Offset(1, 1) );

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
}
