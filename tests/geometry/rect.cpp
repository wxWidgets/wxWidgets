///////////////////////////////////////////////////////////////////////////////
// Name:        tests/geometry/rect.cpp
// Purpose:     wxRect unit test
// Author:      Vadim Zeitlin
// Created:     2004-12-11
// RCS-ID:      $Id$
// Copyright:   (c) 2004 wxWindows 
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class RectTestCase : public CppUnit::TestCase
{
public:
    RectTestCase() { }

private:
    CPPUNIT_TEST_SUITE( RectTestCase );
        CPPUNIT_TEST( Union );
    CPPUNIT_TEST_SUITE_END();

    void Union();

    DECLARE_NO_COPY_CLASS(RectTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RectTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RectTestCase, "RectTestCase" );

void RectTestCase::Union()
{
    static const struct RectData
    {
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        int x, y, w, h;

        wxRect GetFirst() const { return wxRect(x1, y1, w1, h1); }
        wxRect GetSecond() const { return wxRect(x2, y2, w2, h2); }
        wxRect GetResult() const { return wxRect(x, y, w, h); }
    } s_rects[] =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 1, 1, 2, 2, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 6, 6 },
        { 1, 1, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4 },
    };

    for ( size_t n = 0; n < WXSIZEOF(s_rects); n++ )
    {
        const RectData& data = s_rects[n];

        CPPUNIT_ASSERT(
            data.GetFirst().Union(data.GetSecond()) == data.GetResult()
        );

        CPPUNIT_ASSERT(
            data.GetSecond().Union(data.GetFirst()) == data.GetResult()
        );
    }
}

