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
#if wxUSE_GRAPHICS_CONTEXT
#include "wx/dcgraph.h"
#endif // wxUSE_GRAPHICS_CONTEXT


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

static const wxSize s_dcSize(100, 120);
static const wxColour s_bgColour(*wxWHITE); // colour to draw outside clipping box
static const wxColour s_fgColour(*wxGREEN); // colour to draw inside clipping box

class ClippingBoxTestCaseBase : public CppUnit::TestCase
{
public:
    ClippingBoxTestCaseBase()
    {
        m_dc = NULL;
        m_bmp.Create(s_dcSize);
    }

    virtual ~ClippingBoxTestCaseBase()
    {
        m_bmp = wxNullBitmap;
    }

    virtual void setUp() { wxASSERT( m_dc ); }

private:
    void CheckBox(int x, int y, int width, int height);

protected:
    void InitialState();
    void InitialStateWithTransformedDC();
    void OneRegion();
    void OneLargeRegion();
    void OneOuterRegion();
    void OneRegionNegDim();
    void OneRegionAndReset();
    void OneRegionAndEmpty();
    void OneRegionWithTransformedDC();
    void TwoRegionsOverlapping();
    void TwoRegionsOverlappingNegDim();
    void TwoRegionsNonOverlapping();
    void TwoRegionsNonOverlappingNegDim();
    void OneDevRegion();
    void OneLargeDevRegion();
    void OneOuterDevRegion();
    void OneDevRegionNegDim();
    void OneDevRegionAndReset();
    void OneDevRegionAndEmpty();
    void TwoDevRegionsOverlapping();
    void TwoDevRegionsOverlappingNegDim();
    void TwoDevRegionsNonOverlapping();
    void TwoDevRegionsNonOverlappingNegDim();

    virtual void FlushDC() = 0;

    wxBitmap m_bmp;
    wxDC* m_dc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseBase);
};


// wxDC tests
class ClippingBoxTestCaseDC : public ClippingBoxTestCaseBase
{
public:
    ClippingBoxTestCaseDC()
    {
        m_mdc.SelectObject(m_bmp);
        m_dc = &m_mdc;
    }

    virtual ~ClippingBoxTestCaseDC()
    {
        m_mdc.SelectObject(wxNullBitmap);
    }

    virtual void setUp()
    {
        m_mdc.DestroyClippingRegion();
        wxBrush bgBrush(s_bgColour, wxBRUSHSTYLE_SOLID);
        m_mdc.SetBackground(bgBrush);
        m_mdc.Clear();
    }

    virtual void FlushDC() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseDC );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedDC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();
protected:
    wxMemoryDC m_mdc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseDC);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCaseDC );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCaseDC, "ClippingBoxTestCaseDC" );

// wxGCDC tests
#if wxUSE_GRAPHICS_CONTEXT

class ClippingBoxTestCaseGCDC : public ClippingBoxTestCaseDC
{
public:
    ClippingBoxTestCaseGCDC()
    {
        m_gcdc = new wxGCDC(m_mdc);
        m_dc = m_gcdc;
    }

    virtual ~ClippingBoxTestCaseGCDC()
    {
        delete m_gcdc;
    }

    virtual void setUp()
    {
        CPPUNIT_ASSERT_MESSAGE("Invalid wxGCDC", m_gcdc);

        wxGraphicsContext* ctx = m_gcdc->GetGraphicsContext();
        ctx->SetAntialiasMode(wxANTIALIAS_NONE);
        ctx->DisableOffset();
        m_gcdc->DestroyClippingRegion();
        wxBrush bgBrush(s_bgColour, wxBRUSHSTYLE_SOLID);
        m_gcdc->SetBackground(bgBrush);
        m_gcdc->Clear();
    }

    virtual void FlushDC()
    {
        m_gcdc->GetGraphicsContext()->Flush();
    }

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDC );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedDC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:
    wxGCDC* m_gcdc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCDC);
};

// For MSW we have individual test cases for each graphics renderer
// so we don't need to test wxGCDC with default renderer.
#ifndef __WXMSW__
// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCaseGCDC );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCaseGCDC, "ClippingBoxTestCaseGCDC" );
#endif // !__WXMSW__

#ifdef __WXMSW__
// GDI+ and Direct2D are available only under MSW.

#if wxUSE_GRAPHICS_GDIPLUS
class ClippingBoxTestCaseGDIPlus : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseGDIPlus()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseGDIPlus() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGDIPlus );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedDC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGDIPlus);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCaseGDIPlus );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCaseGDIPlus, "ClippingBoxTestCaseGDIPlus" );

#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
class ClippingBoxTestCaseDirect2D : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseDirect2D()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseDirect2D() {}

    virtual void FlushDC()
    {
        // Apparently, flushing native Direct2D renderer
        // is not enough to update underlying DC (bitmap)
        // and therefore we have to destroy the renderer
        // to do so.
        ClippingBoxTestCaseGCDC::FlushDC();
        m_gcdc->SetGraphicsContext(NULL);
    }

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseDirect2D );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedDC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseDirect2D);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCaseDirect2D );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCaseDirect2D, "ClippingBoxTestCaseDirect2D" );

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__

#if wxUSE_CAIRO
class ClippingBoxTestCaseCairo : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseCairo()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseCairo() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseCairo );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedDC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseCairo);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ClippingBoxTestCaseCairo );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ClippingBoxTestCaseCairo, "ClippingBoxTestCaseCairo" );

#endif // wxUSE_CAIRO

#endif //  wxUSE_GRAPHICS_CONTEXT

// Helper class to hold rectangle
// which size is guaranteed to be >= 0
class wxClipRect : public wxRect
{
public:
    wxClipRect(int xx, int yy, int w, int h)
    {
        if ( w < 0 )
        {
            w = -w;
            xx -= (w - 1);
        }
        if ( h < 0 )
        {
            h = -h;
            yy -= (h - 1);
        }
        x = xx;
        y = yy;
        width = w;
        height = h;
    }
};

// =====  Implementation  =====

void ClippingBoxTestCaseBase::CheckBox(int x, int y, int width, int height)
{
    // Check clipping box boundaries.
    int clipX, clipY, clipW, clipH;
    m_dc->GetClippingBox(&clipX, &clipY, &clipW, &clipH);

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

    // We will examine pixels directly in the underlying bitmap
    // so we need to get device coordinates of examined area.
    x = m_dc->LogicalToDeviceX(x);
    y = m_dc->LogicalToDeviceY(y);
    width = m_dc->LogicalToDeviceXRel(width);
    height = m_dc->LogicalToDeviceYRel(height);

    // Update wxDC contents.
    FlushDC();

    // Check whether diagonal corners of the clipping box
    // are actually drawn at the edge of the clipping region.
#if wxUSE_IMAGE
    // For some renderers it's not possible to get pixels
    // value from wxDC so we would have to examine pixels
    // in the underlying bitmap.
    wxImage img;
    img = m_bmp.ConvertToImage();
#else
    return;
#endif // wxUSE_IMAGE

    // Check area near the top-left corner
    int ymin = y-1;
    int xmin = x-1;
    int ymax = y+1;
    int xmax = x+1;
    ymin = wxMin(wxMax(ymin, 0), s_dcSize.GetHeight()-1);
    xmin = wxMin(wxMax(xmin, 0), s_dcSize.GetWidth()-1);
    ymax = wxMin(wxMax(ymax, 0), s_dcSize.GetHeight()-1);
    xmax = wxMin(wxMax(xmax, 0), s_dcSize.GetWidth()-1);

    for( int py = ymin; py <= ymax; py++ )
        for( int px = xmin; px <= xmax; px++ )
        {
            wxColour c;
#if wxUSE_IMAGE
            unsigned char r = img.GetRed(px, py);
            unsigned char g = img.GetGreen(px, py);
            unsigned char b = img.GetBlue(px, py);
            c.Set(r, g, b);
#else
            // We cannot get pixel value
            break;
#endif // wxUSE_IMAGE / !wxUSE_IMAGE

            wxString msgColour;
            if ( px >= x && px <= x + (width-1) &&
                 py >= y && py <= y + (height-1) )
            {
                // Pixel inside the box.
                if ( c != s_fgColour )
                {
                    msgColour =
                        wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                px, py, c.GetAsString().mbc_str(), s_fgColour.GetAsString().mbc_str());

                }
            }
            else
            {
                // Pixel outside the box.
                if ( c != s_bgColour )
                {
                    msgColour =
                        wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                px, py, c.GetAsString().mbc_str(), s_bgColour.GetAsString().mbc_str());
                }
            }

            if ( !msgColour.empty() )
            {
                if ( !msg.empty() )
                {
                    msg += wxS("\n- ");
                    msg += msgColour;
                }
                else
                {
                    msg = msgColour;
                }
            }
        }

    // Check area near the bottom-right corner
    ymin = y+(height-1)-1;
    xmin = x+(width-1)-1;
    ymax = y+(height-1)+1;
    xmax = x+(width-1)+1;
    ymin = wxMin(wxMax(ymin, 0), s_dcSize.GetHeight()-1);
    xmin = wxMin(wxMax(xmin, 0), s_dcSize.GetWidth()-1);
    ymax = wxMin(wxMax(ymax, 0), s_dcSize.GetHeight()-1);
    xmax = wxMin(wxMax(ymax, 0), s_dcSize.GetWidth()-1);
    for( int py = ymin; py <= ymax; py++ )
        for( int px = xmin; px <= xmax; px++ )
        {
            wxColour c;
#if wxUSE_IMAGE
            unsigned char r = img.GetRed(px, py);
            unsigned char g = img.GetGreen(px, py);
            unsigned char b = img.GetBlue(px, py);
            c.Set(r, g, b);
#else
            // We cannot get pixel value
            break;
#endif // wxUSE_IMAGE / !wxUSE_IMAGE

            wxString msgColour;
            if ( px >= x && px <= x + (width-1) &&
                 py >= y && py <= y + (height-1) )
            {
                // Pixel inside the box.
                if ( c != s_fgColour )
                {
                    msgColour =
                        wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                px, py, c.GetAsString().mbc_str(), s_fgColour.GetAsString().mbc_str());
                }
            }
            else
            {
                // Pixel outside the box.
                if ( c != s_bgColour )
                {
                    msgColour =
                        wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                px, py, c.GetAsString().mbc_str(), s_bgColour.GetAsString().mbc_str());
                }
            }

            if ( !msgColour.empty() )
            {
                if ( !msg.empty() )
                {
                    msg += wxS("\n- ");
                    msg += msgColour;
                }
                else
                {
                    msg = msgColour;
                }
            }
        }

    if( !msg.empty() )
    {
        wxCharBuffer buffer = msg.ToUTF8();
        CPPUNIT_FAIL( buffer.data() );
    }
}

// Actual tests

void ClippingBoxTestCaseBase::InitialState()
{
    // Initial clipping box should be the same as the entire DC surface.
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseBase::InitialStateWithTransformedDC()
{
    // Initial clipping box with transformed DC.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(0),
             m_dc->DeviceToLogicalY(0),
             m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
             m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()));
}

void ClippingBoxTestCaseBase::OneRegion()
{
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 80;
    const int h = 75;

    m_dc->SetClippingRegion(x, y, w, h);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(x, y, w, h);
}

void ClippingBoxTestCaseBase::OneLargeRegion()
{
    // Setting one clipping region larger then DC surface.
    // Final clipping box should be limited to the DC extents.
    m_dc->SetClippingRegion(-10, -20,
                         s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseBase::OneOuterRegion()
{
    // Setting one clipping region entirely outside DC surface.
    // Final clipping box should be empty.
    m_dc->SetClippingRegion(-100, -80, 20, 40);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::OneRegionNegDim()
{
    // Setting one clipping region with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 10;
    const int y = 20;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h);
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_dc->SetClippingRegion(x, y, w, h);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(r.GetLeft(),
             r.GetTop(),
             r.GetWidth(),
             r.GetHeight());
}

void ClippingBoxTestCaseBase::OneRegionAndReset()
{
    // Setting one clipping region and next destroy it.
    // Final clipping box should be the same as DC surface.
    m_dc->SetClippingRegion(10, 20, 80, 75);
    m_dc->DestroyClippingRegion();
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseBase::OneRegionAndEmpty()
{
    // Setting one clipping region and next an empty box.
    // Final clipping box should empty.
    m_dc->SetClippingRegion(10, 20, 80, 75);
    m_dc->SetClippingRegion(0, 0, 0, 0);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::OneRegionWithTransformedDC()
{
    // Setting one clipping region inside DC area
    // with applied some transformations.
    wxRect r1(-10, -20, 80, 75);

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    m_dc->SetClippingRegion(r1);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    wxRect r2(m_dc->DeviceToLogicalX(0),
              m_dc->DeviceToLogicalY(0),
              m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
              m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()));
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );
    CheckBox(r.GetLeft(),
             r.GetTop(),
             r.GetWidth(),
             r.GetHeight());
}

void ClippingBoxTestCaseBase::TwoRegionsOverlapping()
{
    // Setting one clipping region and next another region (partially overlapping).
    // Final clipping box should be an intersection of these two boxes.
    wxRect r1(10, 20, 80, 75);
    wxRect r2(50, 60, 50, 40);
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_dc->SetClippingRegion(r1);
    m_dc->SetClippingRegion(r2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(r.GetLeft(),
             r.GetTop(),
             r.GetWidth(),
             r.GetHeight());
}

void ClippingBoxTestCaseBase::TwoRegionsOverlappingNegDim()
{
    // Setting one clipping region with negative size values
    // and next another region (partially overlapping).
    // Final clipping box should be an intersection of these two boxes
    // with positive size values.
    const int x1 = 90;
    const int y1 = 95;
    const int w1 = -80;
    const int h1 = -75;

    const int x2 = 50;
    const int y2 = 60;
    const int w2 = 50;
    const int h2 = 40;
    wxClipRect r1(x1, y1, w1, h1);
    wxRect r2(x2, y2, w2, h2);
    wxRect r = r1.Intersect(r2);

    m_dc->SetClippingRegion(x1, y1, w1, h1);
    m_dc->SetClippingRegion(x2, y2, w2, h2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(r.GetLeft(),
             r.GetTop(),
             r.GetWidth(),
             r.GetHeight());
}

void ClippingBoxTestCaseBase::TwoRegionsNonOverlapping()
{
    // Setting one clipping region and next another rwgion (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    wxASSERT( !r1.Intersects(r2) );

    m_dc->SetClippingRegion(r1);
    m_dc->SetClippingRegion(r2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::TwoRegionsNonOverlappingNegDim()
{
    // Setting one clipping region with negative size values
    // and next another region (non-overlapping).
    // Final clipping box should be empty.
    const int x1 = 10;
    const int y1 = 20;
    const int w1 = -80;
    const int h1 = -75;

    const int x2 = 50;
    const int y2 = 60;
    const int w2 = 50;
    const int h2 = 40;
    wxClipRect r1(x1, y1, w1, h1);
    wxRect r2(x2, y2, w2, h2);
    wxASSERT( !r1.Intersects(r2) );

    m_dc->SetClippingRegion(x1, y1, w1, h1);
    m_dc->SetClippingRegion(x2, y2, w2, h2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::OneDevRegion()
{
    // Setting one clipping region in device coordinates
    // inside transformed DC area.
    const int x = 10;
    const int y = 21;
    const int w = 80;
    const int h = 75;

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(x, y, w, h);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(x),
             m_dc->DeviceToLogicalY(y),
             m_dc->DeviceToLogicalXRel(w),
             m_dc->DeviceToLogicalYRel(h));
}

void ClippingBoxTestCaseBase::OneLargeDevRegion()
{
    // Setting one clipping region in device coordinates larger
    // then transformed DC surface.
    // Final clipping box should be limited to the DC extents.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(-10, -20, s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(0),
             m_dc->DeviceToLogicalY(0),
             m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
             m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()));
}

void ClippingBoxTestCaseBase::OneOuterDevRegion()
{
    // Setting one clipping region in device coordinates
    // entirely outside transformed DC surface.
    // Final clipping box should be empty.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(200, 80, 20, 40);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::OneDevRegionNegDim()
{
    // Setting one clipping region in device coordinates
    // with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 19;
    const int y = 23;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h);
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(x, y, w, h);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(r.GetLeft()),
             m_dc->DeviceToLogicalY(r.GetTop()),
             m_dc->DeviceToLogicalXRel(r.GetWidth()),
             m_dc->DeviceToLogicalYRel(r.GetHeight()));
}

void ClippingBoxTestCaseBase::OneDevRegionAndReset()
{
    // Setting one clipping region in device coordinates
    // and next destroy it.
    // Final clipping box should be the same as DC surface.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(10, 20, 80, 75);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->DestroyClippingRegion();
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(0),
             m_dc->DeviceToLogicalY(0),
             m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
             m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()));
}

void ClippingBoxTestCaseBase::OneDevRegionAndEmpty()
{
    // Setting one clipping region in device coordinates
    // and next an empty region.
    // Final clipping box should empty.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg1(10, 20, 80, 75);
    m_dc->SetDeviceClippingRegion(reg1);
    wxRegion reg2(0, 0, 0, 0);
    m_dc->SetDeviceClippingRegion(reg2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::TwoDevRegionsOverlapping()
{
    // Setting one clipping region in device coordinates
    // and next another region in device coordinates (partially overlapping).
    // Final clipping box should be an intersection of these two regions.
    wxRect r1(30, 39, 40, 30);
    wxRect r2(60, 51, 40, 30);
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg1(r1);
    m_dc->SetDeviceClippingRegion(reg1);
    wxRegion reg2(r2);
    m_dc->SetDeviceClippingRegion(reg2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(r.GetLeft()),
             m_dc->DeviceToLogicalY(r.GetTop()),
             m_dc->DeviceToLogicalXRel(r.GetWidth()),
             m_dc->DeviceToLogicalYRel(r.GetHeight()));
}

void ClippingBoxTestCaseBase::TwoDevRegionsOverlappingNegDim()
{
    // Setting one clipping region in device coordinates with negative size values
    // and next another region in device coordinates (partially overlapping).
    // Final clipping box should be an intersection of these two regions
    // with positive size values.
    const int x1 = 31;
    const int y1 = 20;
    const int w1 = -80;
    const int h1 = -75;

    const int x2 = 20;
    const int y2 = 6;
    const int w2 = 40;
    const int h2 = 30;
    wxClipRect r1(x1, y1, w1, h1);
    wxRect r2(x2, y2, w2, h2);
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg1(x1, y1, w1, h1);
    m_dc->SetDeviceClippingRegion(reg1);
    wxRegion reg2(x2, y2, w2, h2);
    m_dc->SetDeviceClippingRegion(reg2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(m_dc->DeviceToLogicalX(r.GetLeft()),
             m_dc->DeviceToLogicalY(r.GetTop()),
             m_dc->DeviceToLogicalXRel(r.GetWidth()),
             m_dc->DeviceToLogicalYRel(r.GetHeight()));
}

void ClippingBoxTestCaseBase::TwoDevRegionsNonOverlapping()
{
    // Setting one clipping region in device coordinates
    // and next another region in device coordinates (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    wxASSERT( !r1.Intersects(r2) );

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg1(r1);
    m_dc->SetDeviceClippingRegion(reg1);
    wxRegion reg2(r2);
    m_dc->SetDeviceClippingRegion(reg2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseBase::TwoDevRegionsNonOverlappingNegDim()
{
    // Setting one clipping region in device coordinates with negative size values
    // and next another region (non-overlapping).
    // Final clipping box should be empty.
    const int x1 = 10;
    const int y1 = 20;
    const int w1 = -80;
    const int h1 = -75;

    const int x2 = 50;
    const int y2 = 60;
    const int w2 = 50;
    const int h2 = 40;
    wxClipRect r1(x1, y1, w1, h1);
    wxRect r2(x2, y2, w2, h2);
    wxASSERT( !r1.Intersects(r2) );

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg1(x1, y1, w1, h1);
    m_dc->SetDeviceClippingRegion(reg1);
    wxRegion reg2(x2, y2, w2, h2);
    m_dc->SetDeviceClippingRegion(reg2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckBox(0, 0, 0, 0);
}
