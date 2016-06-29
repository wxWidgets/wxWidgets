///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/clippingbox.cpp
// Purpose:     clipping box unit tests
// Author:      Artur Wieczorek
// Created:     2016-06-29
// Copyright:   (c) 2016 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/dcgraph.h"


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ClippingBoxTestCase : public CppUnit::TestCase
{
public:
    ClippingBoxTestCase()
    {
        m_bmp.Create(100, 100);
        m_dc.SelectObject(m_bmp);
        m_gcdc = new wxGCDC(m_dc);
    }

    ~ClippingBoxTestCase()
    {
        delete m_gcdc;
        m_dc.SelectObject(wxNullBitmap);
        m_bmp = wxNullBitmap;
    }

    virtual void setUp()
    {
        m_dc.DestroyClippingRegion();
        m_gcdc->DestroyClippingRegion();
    }

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCase );
        CPPUNIT_TEST( InitialStateDC );
        CPPUNIT_TEST( OneRegionDC );
        CPPUNIT_TEST( OneLargeRegionDC );
        CPPUNIT_TEST( OneRegionNegDimDC );
        CPPUNIT_TEST( OneRegionAndResetDC );
        CPPUNIT_TEST( OneRegionAndEmptyDC );
        CPPUNIT_TEST( TwoRegionsOverlappingDC );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDimDC );
        CPPUNIT_TEST( TwoRegionsNonOverlappingDC );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDimDC );

        CPPUNIT_TEST( InitialStateGCDC );
        CPPUNIT_TEST( OneRegionGCDC );
        CPPUNIT_TEST( OneLargeRegionGCDC );
        CPPUNIT_TEST( OneRegionNegDimGCDC );
        CPPUNIT_TEST( OneRegionAndResetGCDC );
        CPPUNIT_TEST( OneRegionAndEmptyGCDC );
        CPPUNIT_TEST( TwoRegionsOverlappingGCDC );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDimGCDC );
        CPPUNIT_TEST( TwoRegionsNonOverlappingGCDC );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDimGCDC );
    CPPUNIT_TEST_SUITE_END();

    void CheckBox(const wxDC& dc, int x, int y, int width, int height);

    void InitialStateDC();
    void OneRegionDC();
    void OneLargeRegionDC();
    void OneRegionNegDimDC();
    void OneRegionAndResetDC();
    void OneRegionAndEmptyDC();
    void TwoRegionsOverlappingDC();
    void TwoRegionsOverlappingNegDimDC();
    void TwoRegionsNonOverlappingDC();
    void TwoRegionsNonOverlappingNegDimDC();

    void InitialStateGCDC();
    void OneRegionGCDC();
    void OneLargeRegionGCDC();
    void OneRegionNegDimGCDC();
    void OneRegionAndResetGCDC();
    void OneRegionAndEmptyGCDC();
    void TwoRegionsOverlappingGCDC();
    void TwoRegionsOverlappingNegDimGCDC();
    void TwoRegionsNonOverlappingGCDC();
    void TwoRegionsNonOverlappingNegDimGCDC();

    void InitialState(wxDC& dc);
    void OneRegion(wxDC& dc);
    void OneLargeRegion(wxDC& dc);
    void OneRegionNegDim(wxDC& dc);
    void OneRegionAndReset(wxDC& dc);
    void OneRegionAndEmpty(wxDC& dc);
    void TwoRegionsOverlapping(wxDC& dc);
    void TwoRegionsOverlappingNegDim(wxDC& dc);
    void TwoRegionsNonOverlapping(wxDC& dc);
    void TwoRegionsNonOverlappingNegDim(wxDC& dc);

    wxBitmap m_bmp;
    wxMemoryDC m_dc;
    wxGCDC *m_gcdc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCase, "ClippingBoxTestCase" );

void ClippingBoxTestCase::CheckBox(const wxDC& dc, int x, int y, int width, int height)
{
    int clipX, clipY, clipW, clipH;
    dc.GetClippingBox(&clipX, &clipY, &clipW, &clipH);

    wxString msgPos;
    if ( x != clipX || y != clipY )
    {
        msgPos =
            wxString::Format(wxS("Invalid position: Actual: (%i, %i)  Expected: (%i, %i)"),
                                clipX, clipY, x, y);
    }
    wxString msgDim;
    if ( width != clipW || height != clipH )
    {
        msgDim =
            wxString::Format(wxS("Invalid dimension: Actual: %i x %i  Expected: %i x %i"),
                                clipW, clipH, width, height);
    }

    wxString msg;
    if ( !msgPos.empty() )
    {
        msg = msgPos;
        if ( !msgDim.empty() )
        {
            msg += wxS("\n- ");
            msg += msgDim;
        }
    }
    else if ( !msgDim.empty() )
    {
        msg = msgDim;
    }

    if( !msg.empty() )
    {
        wxCharBuffer buffer = msg.ToUTF8();
        CPPUNIT_FAIL( buffer.data() );
    }
}

// wxDC tests

void ClippingBoxTestCase::InitialStateDC()
{
    InitialState(m_dc);
}

void ClippingBoxTestCase::OneRegionDC()
{
    OneRegion(m_dc);
}

void ClippingBoxTestCase::OneLargeRegionDC()
{
    OneLargeRegion(m_dc);
}

void ClippingBoxTestCase::OneRegionNegDimDC()
{
    OneRegionNegDim(m_dc);
}

void ClippingBoxTestCase::OneRegionAndResetDC()
{
    OneRegionAndReset(m_dc);
}

void ClippingBoxTestCase::OneRegionAndEmptyDC()
{
    OneRegionAndEmpty(m_dc);
}

void ClippingBoxTestCase::TwoRegionsOverlappingDC()
{
    TwoRegionsOverlapping(m_dc);
}

void ClippingBoxTestCase::TwoRegionsOverlappingNegDimDC()
{
    TwoRegionsOverlappingNegDim(m_dc);
}

void ClippingBoxTestCase::TwoRegionsNonOverlappingDC()
{
    TwoRegionsNonOverlapping(m_dc);
}

void ClippingBoxTestCase::TwoRegionsNonOverlappingNegDimDC()
{
    TwoRegionsNonOverlappingNegDim(m_dc);
}

// wxGCDC tests

void ClippingBoxTestCase::InitialStateGCDC()
{
    InitialState(*m_gcdc);
}

void ClippingBoxTestCase::OneRegionGCDC()
{
    OneRegion(*m_gcdc);
}

void ClippingBoxTestCase::OneLargeRegionGCDC()
{
    OneLargeRegion(*m_gcdc);
}

void ClippingBoxTestCase::OneRegionNegDimGCDC()
{
    OneRegionNegDim(*m_gcdc);
}

void ClippingBoxTestCase::OneRegionAndResetGCDC()
{
    OneRegionAndReset(*m_gcdc);
}

void ClippingBoxTestCase::OneRegionAndEmptyGCDC()
{
    OneRegionAndEmpty(*m_gcdc);
}

void ClippingBoxTestCase::TwoRegionsOverlappingGCDC()
{
    TwoRegionsOverlapping(*m_gcdc);
}

void ClippingBoxTestCase::TwoRegionsOverlappingNegDimGCDC()
{
    TwoRegionsOverlappingNegDim(*m_gcdc);
}

void ClippingBoxTestCase::TwoRegionsNonOverlappingGCDC()
{
    TwoRegionsNonOverlapping(*m_gcdc);
}

void ClippingBoxTestCase::TwoRegionsNonOverlappingNegDimGCDC()
{
    TwoRegionsNonOverlappingNegDim(*m_gcdc);
}

// Actual tests

void ClippingBoxTestCase::InitialState(wxDC& dc)
{
    CheckBox(dc, 0, 0, 0, 0);
}

void ClippingBoxTestCase::OneRegion(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, 80, 75);
    CheckBox(dc, 10, 20, 80, 75);
}

void ClippingBoxTestCase::OneLargeRegion(wxDC& dc)
{
    dc.SetClippingRegion(-10, -20, 130, 150);
    CheckBox(dc, 0, 0, 100, 100);
}

void ClippingBoxTestCase::OneRegionNegDim(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, -80, -75);
    CheckBox(dc, 0, 0, 11, 21);
}

void ClippingBoxTestCase::OneRegionAndReset(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, 80, 75);
    dc.DestroyClippingRegion();
    CheckBox(dc, 0, 0, 0, 0);
}

void ClippingBoxTestCase::OneRegionAndEmpty(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, 80, 75);
    dc.SetClippingRegion(0, 0, 0, 0);
    CheckBox(dc, 0, 0, 0, 0);
}

void ClippingBoxTestCase::TwoRegionsOverlapping(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, 80, 75);
    dc.SetClippingRegion(50, 60, 50, 40);
    CheckBox(dc, 50, 60, 40, 35);
}

void ClippingBoxTestCase::TwoRegionsOverlappingNegDim(wxDC& dc)
{
    dc.SetClippingRegion(90, 95, -80, -75);
    dc.SetClippingRegion(50, 60, 50, 40);
    CheckBox(dc, 50, 60, 41, 36);
}

void ClippingBoxTestCase::TwoRegionsNonOverlapping(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, 30, 30);
    dc.SetClippingRegion(50, 60, 50, 40);
    CheckBox(dc, 0, 0, 0, 0);
}

void ClippingBoxTestCase::TwoRegionsNonOverlappingNegDim(wxDC& dc)
{
    dc.SetClippingRegion(10, 20, -80, -75);
    dc.SetClippingRegion(50, 60, 50, 40);
    CheckBox(dc, 0, 0, 0, 0);
}
