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
#include "wx/dcsvg.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

static const wxSize s_dcSize(100, 120);
static const wxColour s_bgColour(*wxWHITE); // colour to draw outside clipping box
static const wxColour s_fgColour(*wxGREEN); // colour to draw inside clipping box
static const wxColour s_tmpColour(*wxBLUE);

#if wxUSE_IMAGE
static bool CompareImageFuzzy(const wxImage& img1, const wxImage& img2, int posTolerance = 0)
{
    // For each (x,y) pixel in the image1 we check
    // if there is a pixel of the same value in the square
    // area (x-d..x+d, y-d..y+d) around the pixel (x,y)
    // in the image2. If not, we consider pixels
    // and images as different.

    if ( img1.GetWidth() != img2.GetWidth() )
        return false;

    if ( img1.GetHeight() != img2.GetHeight() )
        return false;

    const int w = img1.GetWidth();
    const int h = img1.GetHeight();
    for ( int y1 = 0; y1 < h; y1++ )
    {
        int y2min = wxMax(y1 - posTolerance, 0);
        int y2max = wxMin(y1 + posTolerance, h);
        for( int x1 = 0; x1 < w; x1++ )
        {
            int x2min = wxMax(x1 - posTolerance, 0);
            int x2max = wxMin(x1 + posTolerance, w);

            unsigned char r1 = img1.GetRed(x1, y1);
            unsigned char g1 = img1.GetGreen(x1, y1);
            unsigned char b1 = img1.GetBlue(x1, y1);

            bool pix2Found = false;
            for( int y2 = y2min; y2 <= y2max && !pix2Found; y2++ )
                for( int x2 = x2min; x2 <= x2max; x2++ )
                {
                    if( r1 == img2.GetRed(x2, y2) &&
                        g1 == img2.GetGreen(x2, y2) &&
                        b1 == img2.GetBlue(x2, y2))
                    {
                        pix2Found = true;
                        break;
                    }
                }

            if ( !pix2Found )
                return false;
        }
    }

    return true;
}
#endif // wxUSE_IMAGE

class ClippingBoxTestCaseBase : public CppUnit::TestCase
{
public:
    ClippingBoxTestCaseBase()
    {
        m_bmp.Create(s_dcSize);
    }

    virtual ~ClippingBoxTestCaseBase()
    {
        m_bmp = wxNullBitmap;
    }

protected:
    void CheckBoxPosition(int cur_x, int cur_y, int cur_w, int cur_h,
                          int act_x, int act_y, int act_w, int act_h,
                          int posTolerance = 0);
    void CheckClipRect(int x, int y, int width, int height);
    void CheckClipWithBitmap(const wxBitmap& bmpRef, int posTolerance = 0);

    wxBitmap m_bmp;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseBase);
};

// ====================
// wxDC / wxGCDC tests
// ====================

class ClippingBoxTestCaseDCBase : public ClippingBoxTestCaseBase
{
public:
    ClippingBoxTestCaseDCBase()
    {
        m_dc = NULL;
    }

    virtual ~ClippingBoxTestCaseDCBase()
    {
    }

    virtual void setUp() wxOVERRIDE { wxASSERT( m_dc ); }
    virtual wxDC* GetDC(wxMemoryDC* dc) = 0;

protected:
    void CheckClipPos(int x, int y, int width, int height, int posTolerance = 0);
    void CheckClipBox(int x, int y, int width, int height,
                      int devX, int devY, int devWidth, int devHeight);
    void CheckClipShape(const wxBitmap& bmpRef, int posTolerance);

    void InitialState();
    void InitialStateWithTransformedDC();
    void InitialStateWithRotatedDC();
    void OneRegion();
    void OneLargeRegion();
    void OneOuterRegion();
    void OneRegionNegDim();
    void OneRegionAndReset();
    void OneRegionAndEmpty();
    void OneRegionOverTransformedDC();
    void OneRegionAndDCTransformation();
    void TwoRegionsOverlapping();
    void TwoRegionsOverlappingNegDim();
    void TwoRegionsNonOverlapping();
    void TwoRegionsNonOverlappingNegDim();
    void OneDevRegion();
    void OneLargeDevRegion();
    void OneOuterDevRegion();
    void OneDevRegionNegDim();
    void OneDevRegionNonRect();
    void OneDevRegionAndReset();
    void OneDevRegionAndEmpty();
    void OneDevRegionOverTransformedDC();
    void OneDevRegionAndDCTransformation();
    void TwoDevRegionsOverlapping();
    void TwoDevRegionsOverlappingNegDim();
    void TwoDevRegionsNonOverlapping();
    void TwoDevRegionsNonOverlappingNegDim();

    virtual void FlushDC() = 0;

    wxDC* m_dc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseDCBase);
};

// ===========
// wxDC tests
// ===========

class ClippingBoxTestCaseDC : public ClippingBoxTestCaseDCBase
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

    virtual void setUp() wxOVERRIDE
    {
        m_mdc.DestroyClippingRegion();
        wxBrush bgBrush(s_bgColour, wxBRUSHSTYLE_SOLID);
        m_mdc.SetBackground(bgBrush);
        m_mdc.Clear();
    }

    virtual wxDC* GetDC(wxMemoryDC* dc) wxOVERRIDE
    {
        return dc;
    }

protected:
    virtual void FlushDC() wxOVERRIDE {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseDC );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( InitialStateWithRotatedDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionOverTransformedDC );
        CPPUNIT_TEST( OneRegionAndDCTransformation );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionNonRect );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( OneDevRegionOverTransformedDC);
        CPPUNIT_TEST( OneDevRegionAndDCTransformation );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();
protected:
    wxMemoryDC m_mdc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseDC);
};

// For GTK+ 3 wxDC is equivalent to wxGCDC
// and hence doesn't need to be tested individually.
#if !defined(__WXGTK3__)
wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseDC, "[clip][dc]");
#endif // !__WXGTK3__

#if wxUSE_GRAPHICS_CONTEXT
// =============
// wxGCDC tests
// =============

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

    virtual void setUp() wxOVERRIDE
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

    virtual wxDC* GetDC(wxMemoryDC* dc) wxOVERRIDE
    {
        wxGraphicsRenderer* rend = m_gcdc->GetGraphicsContext()->GetRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(*dc);
        ctx->SetAntialiasMode(wxANTIALIAS_NONE);
        ctx->DisableOffset();
        return new wxGCDC(ctx);
    }

protected:
    virtual void FlushDC() wxOVERRIDE
    {
        m_gcdc->GetGraphicsContext()->Flush();
    }

    void InitialStateWithRotatedGCForDC();

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDC );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( InitialStateWithRotatedDC );
        CPPUNIT_TEST( InitialStateWithRotatedGCForDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionOverTransformedDC );
        CPPUNIT_TEST( OneRegionAndDCTransformation );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionNonRect );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( OneDevRegionOverTransformedDC);
        CPPUNIT_TEST( OneDevRegionAndDCTransformation );
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
wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCDC, "[clip][gcdc]");
#endif // !__WXMSW__

#ifdef __WXMSW__
// GDI+ and Direct2D are available only under MSW.

#if wxUSE_GRAPHICS_GDIPLUS
class ClippingBoxTestCaseGCDCGDIPlus : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseGCDCGDIPlus()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseGCDCGDIPlus() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDCGDIPlus );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( InitialStateWithRotatedDC );
        CPPUNIT_TEST( InitialStateWithRotatedGCForDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionOverTransformedDC );
        CPPUNIT_TEST( OneRegionAndDCTransformation );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionNonRect );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( OneDevRegionOverTransformedDC);
        CPPUNIT_TEST( OneDevRegionAndDCTransformation );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCDCGDIPlus);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCDCGDIPlus, "[clip][gcdc][gdi+]");

#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
class ClippingBoxTestCaseGCDCDirect2D : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseGCDCDirect2D()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseGCDCDirect2D() {}

    virtual void FlushDC() wxOVERRIDE
    {
        // Apparently, flushing native Direct2D renderer
        // is not enough to update underlying DC (bitmap)
        // and therefore we have to destroy the renderer
        // to do so.
        ClippingBoxTestCaseGCDC::FlushDC();
        m_gcdc->SetGraphicsContext(NULL);
    }

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDCDirect2D );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( InitialStateWithRotatedDC );
        CPPUNIT_TEST( InitialStateWithRotatedGCForDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionOverTransformedDC );
        CPPUNIT_TEST( OneRegionAndDCTransformation );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionNonRect );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( OneDevRegionOverTransformedDC);
        CPPUNIT_TEST( OneDevRegionAndDCTransformation );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCDCDirect2D);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCDCDirect2D, "[clip][gcdc][d2d]");

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__

#if wxUSE_CAIRO
class ClippingBoxTestCaseGCDCCairo : public ClippingBoxTestCaseGCDC
{
public:
    ClippingBoxTestCaseGCDCCairo()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~ClippingBoxTestCaseGCDCCairo() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDCCairo );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedDC );
        CPPUNIT_TEST( InitialStateWithRotatedDC );
        CPPUNIT_TEST( InitialStateWithRotatedGCForDC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionOverTransformedDC );
        CPPUNIT_TEST( OneRegionAndDCTransformation );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( OneDevRegion);
        CPPUNIT_TEST( OneLargeDevRegion );
        CPPUNIT_TEST( OneOuterDevRegion );
        CPPUNIT_TEST( OneDevRegionNegDim );
        CPPUNIT_TEST( OneDevRegionNonRect );
        CPPUNIT_TEST( OneDevRegionAndReset);
        CPPUNIT_TEST( OneDevRegionAndEmpty );
        CPPUNIT_TEST( OneDevRegionOverTransformedDC);
        CPPUNIT_TEST( OneDevRegionAndDCTransformation );
        CPPUNIT_TEST( TwoDevRegionsOverlapping );
        CPPUNIT_TEST( TwoDevRegionsOverlappingNegDim);
        CPPUNIT_TEST( TwoDevRegionsNonOverlapping );
        CPPUNIT_TEST( TwoDevRegionsNonOverlappingNegDim );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCDCCairo);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCDCCairo, "[clip][gc][cairo]");

#endif // wxUSE_CAIRO

#endif //  wxUSE_GRAPHICS_CONTEXT

// Helper class to hold rectangle
// which size is guaranteed to be >= 0
class wxClipRect : public wxRect
{
public:
    wxClipRect(int xx, int yy, int w, int h, int boundWidth = 1)
    {
        if ( w < 0 )
        {
            w = -w;
            xx -= (w - boundWidth);
        }
        if ( h < 0 )
        {
            h = -h;
            yy -= (h - boundWidth);
        }
        x = xx;
        y = yy;
        width = w;
        height = h;
    }
};

// =====  Implementation  =====

static bool inline IsCoordEqual(int pos1, int pos2, int posTolerance = 0)
{
    return abs(pos1 - pos2) <= posTolerance;
}

void ClippingBoxTestCaseBase::CheckBoxPosition(int cur_x, int cur_y, int cur_w, int cur_h,
                                               int x, int y, int w, int h,
                                               int posTolerance)
{
    wxString msgPos;
    if ( !IsCoordEqual(x, cur_x, posTolerance) ||
         !IsCoordEqual(y, cur_y, posTolerance) )
    {
        msgPos =
            wxString::Format(wxS("Invalid position: Actual: (%i, %i)  Expected: (%i, %i)"),
                                cur_x, cur_y, x, y);
    }
    wxString msgDim;
    if ( !IsCoordEqual(w, cur_w, 2*posTolerance) ||
         !IsCoordEqual(h, cur_h, 2*posTolerance) )
    {
        msgDim =
            wxString::Format(wxS("Invalid dimension: Actual: %i x %i  Expected: %i x %i"),
                                cur_w, cur_h, w, h);
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

void ClippingBoxTestCaseBase::CheckClipRect(int x, int y, int width, int height)
{
#if wxUSE_IMAGE
    // We check whether diagonal corners of the clipping box are actually
    // drawn at the edge of the clipping region.
    wxImage img = m_bmp.ConvertToImage();
    wxString msg;

    wxPoint corners[4];
    corners[0] = wxPoint(x, y); // top-left corner
    corners[1] = wxPoint(x+width-1, y); // top-right corner
    corners[2] = wxPoint(x, y+height-1); // bottom-left corner
    corners[3] = wxPoint(x+width-1, y+height-1); // bottom-right corner

    // Check area near every corner
    for ( unsigned int c = 0; c < WXSIZEOF(corners); c++ )
    {
        int ymin = corners[c].y-1;
        int xmin = corners[c].x-1;
        int ymax = corners[c].y+1;
        int xmax = corners[c].x+1;
        ymin = wxMin(wxMax(ymin, 0), s_dcSize.GetHeight()-1);
        xmin = wxMin(wxMax(xmin, 0), s_dcSize.GetWidth()-1);
        ymax = wxMin(wxMax(ymax, 0), s_dcSize.GetHeight()-1);
        xmax = wxMin(wxMax(xmax, 0), s_dcSize.GetWidth()-1);

        for( int py = ymin; py <= ymax; py++ )
            for( int px = xmin; px <= xmax; px++ )
            {
                unsigned char r = img.GetRed(px, py);
                unsigned char g = img.GetGreen(px, py);
                unsigned char b = img.GetBlue(px, py);
                const wxColour col(r, g, b);

                wxString msgColour;
                if ( px >= x && px <= x + (width-1) &&
                     py >= y && py <= y + (height-1) )
                {
                    // Pixel inside the box.
                    if ( col != s_fgColour )
                    {
                        msgColour =
                            wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                    px, py, col.GetAsString().mbc_str(), s_fgColour.GetAsString().mbc_str());

                    }
                }
                else
                {
                    // Pixel outside the box.
                    if ( col != s_bgColour )
                    {
                        msgColour =
                            wxString::Format(wxS("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s"),
                                    px, py, col.GetAsString().mbc_str(), s_bgColour.GetAsString().mbc_str());
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
    }

    if( !msg.empty() )
    {
        wxCharBuffer buffer = msg.ToUTF8();
        CPPUNIT_FAIL( buffer.data() );
    }
#endif // wxUSE_IMAGE
}

void ClippingBoxTestCaseBase::CheckClipWithBitmap(const wxBitmap& bmpRef, int posTolerance)
{
#if wxUSE_IMAGE
    wxImage img = m_bmp.ConvertToImage();

    // If reference bitmap is given then it has to be
    // compared with current bitmap.
    if ( bmpRef.IsOk() )
    {
        wxImage imgRef = bmpRef.ConvertToImage();
        // Figures created by clipping and drawing procedures
        // can be slightly different (shifted by few pixels) due
        // to the different algorithms they can use so we need
        // to perform a "fuzzy" comparison of the images,
        // tolerating some drift of the pixels.
        if ( !CompareImageFuzzy(img, imgRef, posTolerance) )
            CPPUNIT_FAIL( "Invalid shape of the clipping region" );
    }
#endif // wxUSE_IMAGE
}

void ClippingBoxTestCaseDCBase::CheckClipPos(int x, int y, int width, int height, int posTolerance)
{
    // Check clipping box boundaries.
    int clipX, clipY, clipW, clipH;
    m_dc->GetClippingBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(clipX, clipY, clipW, clipH, x, y, width, height, posTolerance);
}

void ClippingBoxTestCaseDCBase::CheckClipBox(int x, int y, int width, int height,
                                             int devX, int devY, int devWidth, int devHeight)
{
    // Check clipping box boundaries.
    int clipX, clipY, clipW, clipH;
    m_dc->GetClippingBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(clipX, clipY, clipW, clipH, x, y, width, height);

    // Update wxDC contents.
    FlushDC();

    // We will examine pixels directly in the underlying bitmap
    // so we need to use device coordinates of the examined area.
#if wxUSE_IMAGE
    // We check whether diagonal corners
    // of the rectangular clipping box are actually
    // drawn at the edge of the clipping region.
    CheckClipRect(devX, devY, devWidth, devHeight);
#endif // wxUSE_IMAGE
}

void ClippingBoxTestCaseDCBase::CheckClipShape(const wxBitmap& bmpRef, int posTolerance)
{
    // Update wxDC contents.
    FlushDC();

#if wxUSE_IMAGE
    // Figures created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms they can use so we need
    // to accept some shift of pixels.
    CheckClipWithBitmap(bmpRef, posTolerance);
#endif // wxUSE_IMAGE
}

// Actual tests

void ClippingBoxTestCaseDCBase::InitialState()
{
    // Initial clipping box should be the same as the entire DC surface.
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::InitialStateWithTransformedDC()
{
    // Initial clipping box with transformed DC.
    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(m_dc->DeviceToLogicalX(0), m_dc->DeviceToLogicalY(0),
             m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
             m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()),
             0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::InitialStateWithRotatedDC()
{
    // Initial clipping box with rotated DC.
#if wxUSE_DC_TRANSFORM_MATRIX
    int clipX, clipY, clipW, clipH;
    if ( m_dc->CanUseTransformMatrix() )
    {
        // Apply rotation to DC.
        wxAffineMatrix2D m = m_dc->GetTransformMatrix();
        m.Rotate(6*M_PI/180.0);
        m_dc->SetTransformMatrix(m);

        // Calculate expected clipping box.
        m.Invert();
        double x1, y1, x2, y2;
        double x, y;
        // Top-left corner
        x = 0.0;
        y = 0.0;
        m.TransformPoint(&x, &y);
        x1 = x;
        y1 = y;
        x2 = x;
        y2 = y;
        // Top-right corner
        x = s_dcSize.GetWidth();
        y = 0.0;
        m.TransformPoint(&x, &y);
        x1 = wxMin(x1, x);
        y1 = wxMin(y1, y);
        x2 = wxMax(x2, x);
        y2 = wxMax(y2, y);
        // Bottom-right corner
        x = s_dcSize.GetWidth();
        y = s_dcSize.GetHeight();
        m.TransformPoint(&x, &y);
        x1 = wxMin(x1, x);
        y1 = wxMin(y1, y);
        x2 = wxMax(x2, x);
        y2 = wxMax(y2, y);
        // Bottom-left corner
        x = 0.0;
        y = s_dcSize.GetHeight();
        m.TransformPoint(&x, &y);
        x1 = wxMin(x1, x);
        y1 = wxMin(y1, y);
        x2 = wxMax(x2, x);
        y2 = wxMax(y2, y);

        clipX = wxRound(x1);
        clipY = wxRound(y1);
        clipW = wxRound(x2)-wxRound(x1);
        clipH = wxRound(y2)-wxRound(y1);
    }
    else
    {
        clipX = 0;
        clipY = 0;
        clipW = s_dcSize.GetWidth();
        clipH = s_dcSize.GetHeight();
    }
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(clipX, clipY, clipW, clipH,
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

void ClippingBoxTestCaseDCBase::OneRegion()
{
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 80;
    const int h = 75;

    m_dc->SetClippingRegion(x, y, w, h);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(x, y, w, h,
                 x, y, w, h);
}

void ClippingBoxTestCaseDCBase::OneLargeRegion()
{
    // Setting one clipping region larger then DC surface.
    // Final clipping box should be limited to the DC extents.
    m_dc->SetClippingRegion(-10, -20,
                         s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneOuterRegion()
{
    // Setting one clipping region entirely outside DC surface.
    // Final clipping box should be empty.
    m_dc->SetClippingRegion(-100, -80, 20, 40);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::OneRegionNegDim()
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
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneRegionAndReset()
{
    // Setting one clipping region and next destroy it.
    // Final clipping box should be the same as DC surface.
    m_dc->SetClippingRegion(10, 20, 80, 75);
    m_dc->DestroyClippingRegion();
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneRegionAndEmpty()
{
    // Setting one clipping region and next an empty box.
    // Final clipping box should empty.
    m_dc->SetClippingRegion(10, 20, 80, 75);
    m_dc->SetClippingRegion(0, 0, 0, 0);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::OneRegionOverTransformedDC()
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
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 m_dc->LogicalToDeviceX(r.GetLeft()),
                 m_dc->LogicalToDeviceY(r.GetTop()),
                 m_dc->LogicalToDeviceXRel(r.GetWidth()),
                 m_dc->LogicalToDeviceYRel(r.GetHeight()));
}

void ClippingBoxTestCaseDCBase::OneRegionAndDCTransformation()
{
    // Setting one clipping region inside DC area
    // and applying DC transformation afterwards.
    wxRect r(-10, -21, 26, 21);

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-16, -21);

    m_dc->SetClippingRegion(r);
    m_dc->SetBackground(wxBrush(s_tmpColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();

    m_dc->SetDeviceOrigin(16, 24);

    // This should entirely overpaint previous clipping area.
    wxCoord x, y, w, h;
    m_dc->GetClippingBox(&x, &y, &w, &h);
    m_dc->SetClippingRegion(x, y, w, h);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    const int clipX = -13;
    const int clipY = -24;
    const int clipW = 26;
    const int clipH = 21;
    CheckClipBox(clipX, clipY, clipW, clipH,
                 m_dc->LogicalToDeviceX(clipX),
                 m_dc->LogicalToDeviceY(clipY),
                 m_dc->LogicalToDeviceXRel(clipW),
                 m_dc->LogicalToDeviceYRel(clipH));
}

void ClippingBoxTestCaseDCBase::TwoRegionsOverlapping()
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
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::TwoRegionsOverlappingNegDim()
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
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::TwoRegionsNonOverlapping()
{
    // Setting one clipping region and next another region (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    wxASSERT( !r1.Intersects(r2) );

    m_dc->SetClippingRegion(r1);
    m_dc->SetClippingRegion(r2);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::TwoRegionsNonOverlappingNegDim()
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
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::OneDevRegion()
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
    CheckClipBox(m_dc->DeviceToLogicalX(x),
                 m_dc->DeviceToLogicalY(y),
                 m_dc->DeviceToLogicalXRel(w),
                 m_dc->DeviceToLogicalYRel(h),
                 x, y, w, h);
}

void ClippingBoxTestCaseDCBase::OneLargeDevRegion()
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
    CheckClipBox(m_dc->DeviceToLogicalX(0),
                 m_dc->DeviceToLogicalY(0),
                 m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
                 m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneOuterDevRegion()
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
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::OneDevRegionNegDim()
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
    CheckClipBox(m_dc->DeviceToLogicalX(r.GetLeft()),
                 m_dc->DeviceToLogicalY(r.GetTop()),
                 m_dc->DeviceToLogicalXRel(r.GetWidth()),
                 m_dc->DeviceToLogicalYRel(r.GetHeight()),
                 r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneDevRegionNonRect()
{
    // Setting one triangular clipping region in device coordinates.
    const wxPoint poly[3] =
    {
        wxPoint(3, 5),
        wxPoint(68, 18),
        wxPoint(40, 72)
    };
    // Expected clipping box in device coordinates.
    const int clipX = 4;
    const int clipY = 6;
    const int clipW = 64;
    const int clipH = 66;

    // Draw image with reference triangle.
    wxBitmap bmpRef(s_dcSize);
    wxMemoryDC memDC(bmpRef);
    wxDC* dcRef = GetDC(&memDC);
    dcRef->SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dcRef->Clear();
    dcRef->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dcRef->SetPen(wxPen(s_fgColour));
    dcRef->DrawPolygon(WXSIZEOF(poly), poly);
    if ( dcRef != &memDC )
        delete dcRef;

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion r(WXSIZEOF(poly), poly);
    m_dc->SetDeviceClippingRegion(r);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    // Check clipping box parameters and compare
    // filled in clipping region with reference triangle.
    // Triangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    CheckClipPos(m_dc->DeviceToLogicalX(clipX),
                 m_dc->DeviceToLogicalY(clipY),
                 m_dc->DeviceToLogicalXRel(clipW),
                 m_dc->DeviceToLogicalYRel(clipH), 1);
    CheckClipShape(bmpRef, 1);
}

void ClippingBoxTestCaseDCBase::OneDevRegionAndReset()
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
    CheckClipBox(m_dc->DeviceToLogicalX(0),
                 m_dc->DeviceToLogicalY(0),
                 m_dc->DeviceToLogicalXRel(s_dcSize.GetWidth()),
                 m_dc->DeviceToLogicalYRel(s_dcSize.GetHeight()),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneDevRegionAndEmpty()
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
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::OneDevRegionOverTransformedDC()
{
    // Set one clipping region in device coordinates inside
    // DC area with applied some transformations.
    wxRect r1(8, 15, 60, 75);

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-15, -20);
    wxRegion reg(r1);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );
    CheckClipBox(m_dc->DeviceToLogicalX(r.GetLeft()),
                 m_dc->DeviceToLogicalY(r.GetTop()),
                 m_dc->DeviceToLogicalXRel(r.GetWidth()),
                 m_dc->DeviceToLogicalYRel(r.GetHeight()),
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::OneDevRegionAndDCTransformation()
{
    // Set one clipping region in device coordinates inside
    // DC area and apply DC transformation afterwards.
    wxRect r1(8, 15, 60, 75);

    m_dc->SetDeviceOrigin(10, 15);
    m_dc->SetUserScale(0.5, 1.5);
    m_dc->SetLogicalScale(4.0, 2.0);
    m_dc->SetLogicalOrigin(-16, -21);

    wxRegion reg(r1);
    m_dc->SetDeviceClippingRegion(reg);
    m_dc->SetBackground(wxBrush(s_tmpColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();

    m_dc->SetDeviceOrigin(16, 24);

    // This should entirely overpaint previous clipping area.
    wxCoord x, y, w, h;
    m_dc->GetClippingBox(&x, &y, &w, &h);
    m_dc->SetClippingRegion(x, y, w, h);
    m_dc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_dc->Clear();
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );
    CheckClipBox(m_dc->DeviceToLogicalX(r.GetLeft()),
                 m_dc->DeviceToLogicalY(r.GetTop()),
                 m_dc->DeviceToLogicalXRel(r.GetWidth()),
                 m_dc->DeviceToLogicalYRel(r.GetHeight()),
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::TwoDevRegionsOverlapping()
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
    CheckClipBox(m_dc->DeviceToLogicalX(r.GetLeft()),
                 m_dc->DeviceToLogicalY(r.GetTop()),
                 m_dc->DeviceToLogicalXRel(r.GetWidth()),
                 m_dc->DeviceToLogicalYRel(r.GetHeight()),
                 r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::TwoDevRegionsOverlappingNegDim()
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
    CheckClipBox(m_dc->DeviceToLogicalX(r.GetLeft()),
                 m_dc->DeviceToLogicalY(r.GetTop()),
                 m_dc->DeviceToLogicalXRel(r.GetWidth()),
                 m_dc->DeviceToLogicalYRel(r.GetHeight()),
                 r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseDCBase::TwoDevRegionsNonOverlapping()
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
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

void ClippingBoxTestCaseDCBase::TwoDevRegionsNonOverlappingNegDim()
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
    CheckClipBox(0, 0, 0, 0,
                 0, 0, 0, 0);
}

// Tests specific to wxGCDC
#if wxUSE_GRAPHICS_CONTEXT
void ClippingBoxTestCaseGCDC::InitialStateWithRotatedGCForDC()
{
    // Initial wxGCDC clipping box with rotated wxGraphicsContext.
    wxGraphicsContext* gc = m_gcdc->GetGraphicsContext();
    gc->Rotate(6*M_PI/180.0);

    m_gcdc->SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_gcdc->Clear();

    // Calculate expected clipping box.
    int clipX, clipY, clipW, clipH;
    wxGraphicsMatrix m = gc->GetTransform();
    m.Invert();
    double x1, y1, x2, y2;
    double x, y;
    // Top-left corner
    x = 0.0;
    y = 0.0;
    m.TransformPoint(&x, &y);
    x1 = x;
    y1 = y;
    x2 = x;
    y2 = y;
    // Top-right corner
    x = s_dcSize.GetWidth();
    y = 0.0;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-right corner
    x = s_dcSize.GetWidth();
    y = s_dcSize.GetHeight();
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-left corner
    x = 0.0;
    y = s_dcSize.GetHeight();
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);

    clipX = wxRound(x1);
    clipY = wxRound(y1);
    clipW = wxRound(x2)-wxRound(x1);
    clipH = wxRound(y2)-wxRound(y1);

    CheckClipBox(clipX, clipY, clipW, clipH,
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}
#endif // wxUSE_GRAPHICS_CONTEXT

// ========================
// wxGraphicsContext tests
// ========================
#if wxUSE_GRAPHICS_CONTEXT

class ClippingBoxTestCaseGCBase : public ClippingBoxTestCaseBase
{
public:
    ClippingBoxTestCaseGCBase()
    {
        m_rend = NULL;
        m_gc = NULL;
        m_mdc.SelectObject(m_bmp);
    }

    virtual ~ClippingBoxTestCaseGCBase()
    {
        m_mdc.SelectObject(wxNullBitmap);
        delete m_gc;
    }

    virtual void setUp() wxOVERRIDE
    {
        m_mdc.DestroyClippingRegion();
        wxBrush bgBrush(s_bgColour, wxBRUSHSTYLE_SOLID);
        m_mdc.SetBackground(bgBrush);
        m_mdc.Clear();

        CPPUNIT_ASSERT_MESSAGE("Invalid renderer", m_rend);
        m_gc = m_rend->CreateContext(m_mdc);
        m_gc->SetAntialiasMode(wxANTIALIAS_NONE);
        m_gc->DisableOffset();
        m_gc->ResetClip();
    }

protected:
    virtual void FlushGC()
    {
        m_gc->Flush();
    }

    void CheckClipPos(int x, int y, int width, int height, int posTolerance = 0);
    void CheckClipBox(int x, int y, int width, int height);
    void CheckClipShape(const wxBitmap& bmpRef, int posTolerance);
    void Clear();

    void InitialState();
    void InitialStateWithTransformedGC();
    void OneRegion();
    void OneLargeRegion();
    void OneOuterRegion();
    void OneRegionNegDim();
    void OneRegionAndReset();
    void OneRegionAndEmpty();
    void OneRegionWithTransformedGC();
    void OneRegionWithRotatedGC();
    void TwoRegionsOverlapping();
    void TwoRegionsOverlappingNegDim();
    void TwoRegionsNonOverlapping();
    void TwoRegionsNonOverlappingNegDim();
    void RegionsAndPushPopState();

protected:
    wxGraphicsRenderer* m_rend;
    wxGraphicsContext* m_gc;
    wxMemoryDC m_mdc;

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCBase);
};

#ifdef __WXMSW__
// GDI+ and Direct2D are available only under MSW.

#if wxUSE_GRAPHICS_GDIPLUS
class ClippingBoxTestCaseGCGDIPlus : public ClippingBoxTestCaseGCBase
{
public:
    ClippingBoxTestCaseGCGDIPlus()
    {
        m_rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    }

    virtual ~ClippingBoxTestCaseGCGDIPlus() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCGDIPlus );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedGC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedGC );
        CPPUNIT_TEST( OneRegionWithRotatedGC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( RegionsAndPushPopState );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCGDIPlus);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCGDIPlus, "[clip][gc][gdi+]");

#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
class ClippingBoxTestCaseGCDirect2D : public ClippingBoxTestCaseGCBase
{
public:
    ClippingBoxTestCaseGCDirect2D()
    {
        m_rend = wxGraphicsRenderer::GetDirect2DRenderer();
    }

    virtual ~ClippingBoxTestCaseGCDirect2D() {}

    virtual void FlushGC() wxOVERRIDE
    {
        // Apparently, flushing native Direct2D renderer
        // is not enough to update underlying DC (bitmap)
        // and therefore we have to destroy the renderer
        // to do so.
        ClippingBoxTestCaseGCBase::FlushGC();
        delete m_gc;
        m_gc = NULL;
    }

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCDirect2D );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedGC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedGC );
        CPPUNIT_TEST( OneRegionWithRotatedGC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( RegionsAndPushPopState );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCDirect2D);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCDirect2D, "[clip][gc][d2d]");

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__

#if wxUSE_CAIRO
class ClippingBoxTestCaseGCCairo : public ClippingBoxTestCaseGCBase
{
public:
    ClippingBoxTestCaseGCCairo()
    {
        m_rend = wxGraphicsRenderer::GetCairoRenderer();
    }

    virtual ~ClippingBoxTestCaseGCCairo() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCCairo );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedGC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedGC );
        CPPUNIT_TEST( OneRegionWithRotatedGC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( RegionsAndPushPopState );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCCairo);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCCairo, "[clip][gc][cairo]");

#endif // wxUSE_CAIRO

#ifdef __WXOSX__
class ClippingBoxTestCaseGCCoreGraphics : public ClippingBoxTestCaseGCBase
{
public:
    ClippingBoxTestCaseGCCoreGraphics()
    {
        m_rend = wxGraphicsRenderer::GetDefaultRenderer();
    }

    virtual ~ClippingBoxTestCaseGCCoreGraphics() {}

private:
    CPPUNIT_TEST_SUITE( ClippingBoxTestCaseGCCoreGraphics );
        CPPUNIT_TEST( InitialState );
        CPPUNIT_TEST( InitialStateWithTransformedGC );
        CPPUNIT_TEST( OneRegion );
        CPPUNIT_TEST( OneLargeRegion );
        CPPUNIT_TEST( OneOuterRegion );
        CPPUNIT_TEST( OneRegionNegDim );
        CPPUNIT_TEST( OneRegionAndReset );
        CPPUNIT_TEST( OneRegionAndEmpty );
        CPPUNIT_TEST( OneRegionWithTransformedGC );
        CPPUNIT_TEST( OneRegionWithRotatedGC );
        CPPUNIT_TEST( TwoRegionsOverlapping );
        CPPUNIT_TEST( TwoRegionsOverlappingNegDim );
        CPPUNIT_TEST( TwoRegionsNonOverlapping );
        CPPUNIT_TEST( TwoRegionsNonOverlappingNegDim );
        CPPUNIT_TEST( RegionsAndPushPopState );
    CPPUNIT_TEST_SUITE_END();

protected:

    wxDECLARE_NO_COPY_CLASS(ClippingBoxTestCaseGCCoreGraphics);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ClippingBoxTestCaseGCCoreGraphics, "[clip][gc][cg]");

#endif // __WXOSX__

void ClippingBoxTestCaseGCBase::CheckClipPos(int x, int y, int width, int height, int posTolerance)
{
    // Check clipping box boundaries.
    double clipX, clipY, clipW, clipH;
    m_gc->GetClipBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(wxRound(clipX), wxRound(clipY), wxRound(clipW), wxRound(clipH),
                     x, y, width, height, posTolerance);
}

void ClippingBoxTestCaseGCBase::CheckClipBox(int x, int y, int width, int height)
{
    // Check clipping box boundaries.
    double clipX, clipY, clipW, clipH;
    m_gc->GetClipBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(wxRound(clipX), wxRound(clipY), wxRound(clipW), wxRound(clipH),
                     x, y, width, height);

    // We will examine pixels directly in the underlying bitmap
    // so we need to get device coordinates of examined area.
    wxGraphicsMatrix m = m_gc->GetTransform();
    double xdev = x;
    double ydev = y;
    m.TransformPoint(&xdev, &ydev);
    double wdev = width;
    double hdev = height;
    m.TransformDistance(&wdev, &hdev);

    // Update wxGraphicsContext contents.
    FlushGC();
#if wxUSE_IMAGE
    // We check whether diagonal corners
    // of the rectangular clipping box are actually
    // drawn at the edge of the clipping region.
    CheckClipRect(wxRound(xdev), wxRound(ydev), wxRound(wdev), wxRound(hdev));
#endif // wxUSE_IMAGE
}

void ClippingBoxTestCaseGCBase::CheckClipShape(const wxBitmap& bmpRef, int posTolerance)
{
    // Update wxGraphicsContext contents.
    FlushGC();

#if wxUSE_IMAGE
    // Figures created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms they can use so we need
    // to accept some shift of pixels.
    CheckClipWithBitmap(bmpRef, posTolerance);
#endif // wxUSE_IMAGE
}

void ClippingBoxTestCaseGCBase::Clear()
{
    double x, y, w, h;
    m_gc->GetClipBox(&x, &y, &w, &h);
    m_gc->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_gc->SetPen(*wxTRANSPARENT_PEN);
    m_gc->SetCompositionMode(wxCOMPOSITION_SOURCE);
    m_gc->DrawRectangle(x, y, w, h);
}

// Actual tests

void ClippingBoxTestCaseGCBase::InitialState()
{
    // Initial clipping box should be the same as the entire GC surface.
    Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseGCBase::InitialStateWithTransformedGC()
{
    // Initial clipping box with transformed GC.
    wxGraphicsMatrix m = m_gc->CreateMatrix();
    m.Translate(10, 15);
    m.Scale(1/2.0, 1/3.0);
    m.Translate(20, 30);
    m_gc->SetTransform(m);
    Clear();
    m.Invert();
    double x = 0;
    double y = 0;
    m.TransformPoint(&x, &y);
    double w = s_dcSize.GetWidth();
    double h = s_dcSize.GetHeight();
    m.TransformDistance(&w, &h);
    CheckClipBox(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
}

void ClippingBoxTestCaseGCBase::OneRegion()
{
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 80;
    const int h = 75;

    m_gc->Clip(x, y, w, h);
    Clear();
    CheckClipBox(x, y, w, h);
}

void ClippingBoxTestCaseGCBase::OneLargeRegion()
{
    // Setting one clipping region larger then GC surface.
    // Final clipping box should be limited to the GC extents.
    m_gc->Clip(-10, -20,
               s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseGCBase::OneOuterRegion()
{
    // Setting one clipping region entirely outside GC surface.
    // Final clipping box should be empty.
    m_gc->Clip(-100, -80, 20, 40);
    Clear();
    CheckClipBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseGCBase::OneRegionNegDim()
{
    // Setting one clipping region with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 10;
    const int y = 20;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h, 0);
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_gc->Clip(x, y, w, h);
    Clear();
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseGCBase::OneRegionAndReset()
{
    // Setting one clipping region and next destroy it.
    // Final clipping box should be the same as GC surface.
    m_gc->Clip(10, 20, 80, 75);
    m_gc->ResetClip();
    Clear();
    CheckClipBox(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

void ClippingBoxTestCaseGCBase::OneRegionAndEmpty()
{
    // Setting one clipping region and next an empty box.
    // Final clipping box should empty.
    m_gc->Clip(10, 20, 80, 75);
    m_gc->Clip(0, 0, 0, 0);
    Clear();
    CheckClipBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseGCBase::OneRegionWithTransformedGC()
{
    // Setting one clipping region inside GC area
    // with applied some transformations.
    wxRect r1(-10, -21, 80, 75);

    wxGraphicsMatrix m = m_gc->CreateMatrix();
    m.Translate(10, 15);
    m.Scale(1/2.0, 1/3.0);
    m.Translate(20, 30);
    m_gc->SetTransform(m);
    m_gc->Clip(r1.x, r1.y, r1.width, r1.height);
    Clear();
    m.Invert();
    double x = 0;
    double y = 0;
    m.TransformPoint(&x, &y);
    double w = s_dcSize.GetWidth();
    double h = s_dcSize.GetHeight();
    m.TransformDistance(&w, &h);
    wxRect r2(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseGCBase::OneRegionWithRotatedGC()
{
    // Setting one rectangular clipping region for rotated graphics context.
    const double rotAngle = 1.0*M_PI/180.0;
    const int rectX = 16;
    const int rectY = 14;
    const int rectW = 60;
    const int rectH = 55;

    // Set clipping region for rotated wxGC.
    m_gc->Rotate(rotAngle);
    m_gc->Clip(rectX, rectY, rectW, rectH);
    // Fill in clipping region.
    Clear();

    // Draw reference image with rotated rectangle which
    // should look the same as rectangle drawn with Clip().
    wxBitmap bmpRef(s_dcSize);
    {
        wxMemoryDC memDC(bmpRef);
        wxGraphicsContext* gcRef = m_rend->CreateContext(memDC);
        gcRef->SetAntialiasMode(wxANTIALIAS_NONE);
        gcRef->DisableOffset();
        gcRef->SetBrush(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(*wxTRANSPARENT_PEN);
        gcRef->DrawRectangle(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
        gcRef->Rotate(rotAngle);
        gcRef->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(wxPen(s_fgColour));
        gcRef->DrawRectangle(rectX, rectY, rectW, rectH);
        delete gcRef;
    }

    // Compare filled in clipping region with reference rectangle.
    // Rotated rectangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    CheckClipShape(bmpRef, 1);
}

void ClippingBoxTestCaseGCBase::TwoRegionsOverlapping()
{
    // Setting one clipping region and next another region (partially overlapping).
    // Final clipping box should be an intersection of these two boxes.
    wxRect r1(10, 20, 80, 75);
    wxRect r2(50, 60, 50, 40);
    wxRect r = r1.Intersect(r2);
    wxASSERT( !r.IsEmpty() );

    m_gc->Clip(r1.x, r1.y, r1.width, r1.height);
    m_gc->Clip(r2.x, r2.y, r2.width, r2.height);
    Clear();
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseGCBase::TwoRegionsOverlappingNegDim()
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
    wxClipRect r1(x1, y1, w1, h1, 0);
    wxRect r2(x2, y2, w2, h2);
    wxRect r = r1.Intersect(r2);

    m_gc->Clip(x1, y1, w1, h1);
    m_gc->Clip(x2, y2, w2, h2);
    Clear();
    CheckClipBox(r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight());
}

void ClippingBoxTestCaseGCBase::TwoRegionsNonOverlapping()
{
    // Setting one clipping region and next another region (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    wxASSERT( !r1.Intersects(r2) );

    m_gc->Clip(r1.x, r1.y, r1.width, r1.height);
    m_gc->Clip(r2.x, r2.y, r2.width, r2.height);
    Clear();
    CheckClipBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseGCBase::TwoRegionsNonOverlappingNegDim()
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
    wxClipRect r1(x1, y1, w1, h1, 0);
    wxRect r2(x2, y2, w2, h2);
    wxASSERT( !r1.Intersects(r2) );

    m_gc->Clip(x1, y1, w1, h1);
    m_gc->Clip(x2, y2, w2, h2);
    Clear();
    CheckClipBox(0, 0, 0, 0);
}

void ClippingBoxTestCaseGCBase::RegionsAndPushPopState()
{
    // Setting muliple rectangular clipping regions
    // for transformed wxGC and store/restore them.

    // Get rectangle of the entire drawing area.
    double x, y, w, h;
    m_gc->GetClipBox(&x, &y, &w, &h);

    // Set clipping regions and store/restore them.
    m_gc->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    m_gc->SetPen(wxPen(s_fgColour));

    m_gc->Translate(5, 5);
    m_gc->Rotate(5*M_PI/180);
    m_gc->Clip(20, 15, 50, 45);
    m_gc->PushState();

    m_gc->Rotate(5*M_PI/180);
    m_gc->ResetClip();
    m_gc->Clip(10, 5, 60, 15);
    m_gc->PushState();

    m_gc->Rotate(-15*M_PI/180);
    m_gc->ResetClip();
    m_gc->Clip(5, 10, 30, 35);
    m_gc->DrawRectangle(x, y, w, h);

    m_gc->PopState();
    m_gc->DrawRectangle(x, y, w, h);

    m_gc->PopState();
    m_gc->DrawRectangle(x, y, w, h);

    // Draw reference image with rotated rectangles which
    // should look the same as rectangles drawn with Clip().
    wxBitmap bmpRef(s_dcSize);
    {
        wxMemoryDC memDC(bmpRef);
        wxGraphicsContext* gcRef = m_rend->CreateContext(memDC);
        gcRef->SetAntialiasMode(wxANTIALIAS_NONE);
        gcRef->DisableOffset();
        gcRef->SetBrush(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(*wxTRANSPARENT_PEN);
        gcRef->DrawRectangle(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());

        gcRef->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(wxPen(s_fgColour));

        gcRef->Translate(5, 5);
        gcRef->Rotate(5*M_PI/180);
        gcRef->PushState();

        gcRef->Rotate(5*M_PI/180);
        gcRef->PushState();

        gcRef->Rotate(-15*M_PI/180);
        gcRef->DrawRectangle(5, 10, 30, 35);

        gcRef->PopState();
        gcRef->DrawRectangle(10, 5, 60, 15);

        gcRef->PopState();
        gcRef->DrawRectangle(20, 15, 50, 45);

        delete gcRef;
    }

    // Compare filled in clipping regions with reference image.
    // Rotated rectangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    CheckClipShape(bmpRef, 1);
}

#endif // wxUSE_GRAPHICS_CONTEXT

#if wxUSE_SVG

// We can't reuse the existing tests for wxSVGFileDC as we can't check its
// output, but we can still at least check the behaviour of GetClippingBox().
TEST_CASE("ClippingBoxTestCaseSVGDC", "[clip][svgdc]")
{
    TestFile tf;
    wxSVGFileDC dc(tf.GetName(), s_dcSize.x, s_dcSize.y);

    wxRect rect;
    dc.GetClippingBox(rect);
    CHECK( rect == wxRect(s_dcSize) );

    const wxRect rectClip(10, 20, 80, 75);
    dc.SetClippingRegion(rectClip);

    dc.GetClippingBox(rect);
    CHECK( rect == rectClip );

    dc.DestroyClippingRegion();
    dc.GetClippingBox(rect);
    CHECK( rect == wxRect(s_dcSize) );
}

#endif // wxUSE_SVG
