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


#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/dcgraph.h"
#include "wx/dcsvg.h"
#include "wx/app.h"
#include "wx/window.h"
#include "wx/dcclient.h"
#include "wx/scopedptr.h"

#include "testfile.h"
#include "waitforpaint.h"

static const wxSize s_dcSize(260, 300);
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

static inline bool IsCoordEqual(int pos1, int pos2, int posTolerance = 0)
{
    return abs(pos1 - pos2) <= posTolerance;
}

static void CheckBoxPosition(int cur_x, int cur_y, int cur_w, int cur_h,
                             int x, int y, int w, int h,
                             int posTolerance = 0)
{
    wxString msgPos;
    if ( !IsCoordEqual(x, cur_x, posTolerance) ||
         !IsCoordEqual(y, cur_y, posTolerance) )
    {
        msgPos =
            wxString::Format("Invalid position: Actual: (%i, %i)  Expected: (%i, %i)",
                                cur_x, cur_y, x, y);
    }
    wxString msgDim;
    if ( !IsCoordEqual(w, cur_w, 2*posTolerance) ||
         !IsCoordEqual(h, cur_h, 2*posTolerance) )
    {
        msgDim =
            wxString::Format("Invalid dimension: Actual: %i x %i  Expected: %i x %i",
                                cur_w, cur_h, w, h);
    }

    wxString msg;
    if ( !msgPos.empty() )
    {
        msg = msgPos;
        if ( !msgDim.empty() )
        {
            msg += "\n- ";
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
        FAIL( buffer.data() );
    }
}

// We check whether diagonal corners
// of the rectangular clipping box are actually
// drawn at the edge of the clipping region.
void CheckClipRect(const wxBitmap& bmp, int x, int y, int width, int height)
{
#if wxUSE_IMAGE
    // We check whether diagonal corners of the clipping box are actually
    // drawn at the edge of the clipping region.
    wxImage img = bmp.ConvertToImage();
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
                            wxString::Format("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s",
                                    px, py, col.GetAsString().mbc_str(), s_fgColour.GetAsString().mbc_str());

                    }
                }
                else
                {
                    // Pixel outside the box.
                    if ( col != s_bgColour )
                    {
                        msgColour =
                            wxString::Format("Invalid colour drawn at (%i, %i): Actual: %s  Expected: %s",
                                    px, py, col.GetAsString().mbc_str(), s_bgColour.GetAsString().mbc_str());
                    }
                }

                if ( !msgColour.empty() )
                {
                    if ( !msg.empty() )
                    {
                        msg += "\n- ";
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
        FAIL( buffer.data() );
    }
#endif // wxUSE_IMAGE
}

// Figures created by clipping and drawing procedures
// can be slightly different (shifted by few pixels) due
// to the different algorithms they can use so we need
// to accept some shift of pixels.
static void CheckClipWithBitmap(const wxBitmap& bmp, const wxBitmap& bmpRef, int posTolerance)
{
    if ( !bmp.IsOk() || !bmpRef.IsOk() )
        return;

#if wxUSE_IMAGE
    wxImage img = bmp.ConvertToImage();
    wxImage imgRef = bmpRef.ConvertToImage();
    // Figures created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms they can use so we need
    // to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    if ( !CompareImageFuzzy(img, imgRef, posTolerance) )
        FAIL( "Invalid shape of the clipping region" );
#endif // wxUSE_IMAGE
}

static void CheckClipPos(wxDC& dc, int x, int y, int width, int height, int posTolerance = 0)
{
    // Check clipping box boundaries.
    int clipX, clipY, clipW, clipH;
    dc.GetClippingBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(clipX, clipY, clipW, clipH, x, y, width, height, posTolerance);
}

static void FlushDC(wxDC& dc)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if ( gc )
    {
        gc->Flush();

#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
        // Apparently, flushing native Direct2D renderer
        // is not enough to update underlying DC (bitmap)
        // and therefore we have to destroy the renderer
        // to do so.
        wxGraphicsRenderer* rend = gc->GetRenderer();
        if ( rend == wxGraphicsRenderer::GetDirect2DRenderer() )
            dc.SetGraphicsContext(nullptr);
#endif // __WXMSW__ && wxUSE_GRAPHICS_DIRECT2D
    }
#else
    wxUnusedVar(dc);
#endif // wxUSE_GRAPHICS_CONTEXT
}

static void CheckClipBox(wxDC& dc, const wxBitmap& bmp,
                         int x, int y, int width, int height,
                         int xPh, int yPh, int wPh, int hPh,
                         int posTolerance = 0)
{
    // Check clipping box boundaries.
    CheckClipPos(dc, x, y, width, height, posTolerance);

    if ( bmp.IsOk() )
    {
        // Update wxDC contents.
        FlushDC(dc);

        // We will examine pixels directly in the underlying bitmap
        // so we need to use device coordinates of the examined area.
#if wxUSE_IMAGE
        // We check whether diagonal corners
        // of the rectangular clipping box are actually
        // drawn at the edge of the clipping region.
        CheckClipRect(bmp, xPh, yPh, wPh, hPh);
#endif // wxUSE_IMAGE
    }
}

static void CheckClipShape(wxDC& dc, const wxBitmap& bmp, const wxBitmap& bmpRef, int posTolerance)
{
    // Update wxDC contents.
    FlushDC(dc);

#if wxUSE_IMAGE
    // Figures created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms they can use so we need
    // to accept some shift of pixels.
    CheckClipWithBitmap(bmp, bmpRef, posTolerance);
#endif // wxUSE_IMAGE
}

// ====================
// wxDC / wxGCDC tests
// ====================

// Actual tests
static void InitialState(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Initial clipping box should be the same as the entire DC surface.
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void InitialStateWithTransformedDC(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix,
                                          const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Initial clipping box with transformed DC.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(0 - parentDcOrigin.x, 0 - parentDcOrigin.y);
        wxSize dim = dc.DeviceToLogicalRel(s_dcSize);
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(0 - parentDcOrigin.x), dc.DeviceToLogicalY(0 - parentDcOrigin.y),
                     dc.DeviceToLogicalXRel(s_dcSize.GetWidth()),
                     dc.DeviceToLogicalYRel(s_dcSize.GetHeight()),
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
}

static void InitialStateWithRotatedDC(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Initial clipping box with rotated DC.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( !dc.CanUseTransformMatrix() )
        return;

    // Apply rotation to DC.
    wxAffineMatrix2D m = dc.GetTransformMatrix();
    m.Rotate(wxDegToRad(6));
    dc.SetTransformMatrix(m);

    // Calculate expected clipping box.
    m.Invert();
    double x1, y1, x2, y2;
    double x, y;
    // Top-left corner
    x = 0.0 - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = x;
    y1 = y;
    x2 = x;
    y2 = y;
    // Top-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-left corner
    x = 0.0 - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);

    int clipX = wxRound(x1);
    int clipY = wxRound(y1);
    int clipW = wxRound(x2) - wxRound(x1);
    int clipH = wxRound(y2) - wxRound(y1);

    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 clipX, clipY, clipW, clipH,
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void SameRegionRepeatRotatedDC(wxDC& dc, const wxPoint& parentDcOrigin)
{
    // While setting the same clipping region several times
    // the same clipping box should be should be returned.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( !dc.CanUseTransformMatrix() )
        return;

    // Apply rotation to DC.
    wxAffineMatrix2D m = dc.GetTransformMatrix();
    m.Rotate(wxDegToRad(6));
    dc.SetTransformMatrix(m);

    // Calculate expected clipping box.
    m.Invert();
    double x1, y1, x2, y2;
    double x, y;
    // Top-left corner
    x = 0.0 - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = x;
    y1 = y;
    x2 = x;
    y2 = y;
    // Top-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-left corner
    x = 0.0 - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);

    int clipX = wxRound(x1);
    int clipY = wxRound(y1);
    int clipW = wxRound(x2) - wxRound(x1);
    int clipH = wxRound(y2) - wxRound(y1);

    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipPos(dc, clipX, clipY, clipW, clipH);
    dc.SetClippingRegion(clipX, clipY, clipW, clipH);
    CheckClipPos(dc, clipX, clipY, clipW, clipH, 1);
    dc.SetClippingRegion(clipX, clipY, clipW, clipH);
    CheckClipPos(dc, clipX, clipY, clipW, clipH, 1);
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void OneRegion(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 80;
    const int h = 75;

    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 x, y, w, h,
                 x + parentDcOrigin.x , y + parentDcOrigin.y, w, h);
}

static void OneLargeRegion(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region larger then DC surface.
    // Final clipping box should be limited to the DC extents.
    dc.SetClippingRegion(-15, -25,
                         s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void OneOuterRegion(wxDC& dc, const wxBitmap& bmp)
{
    // Setting one clipping region entirely outside DC surface.
    // Final clipping box should be empty.
    dc.SetClippingRegion(-100, -80, 20, 40);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneRegionNegDim(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 10;
    const int y = 20;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h);
    wxRect r2(0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                 r.GetWidth(), r.GetHeight());
}

static void OneRegionAndReset(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region and next destroy it.
    // Final clipping box should be the same as DC surface.
    dc.SetClippingRegion(10, 20, 80, 75);
    dc.DestroyClippingRegion();
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void OneRegionAndEmpty(wxDC& dc, const wxBitmap& bmp)
{
    // Setting one clipping region and next an empty box.
    // Final clipping box should empty.
    dc.SetClippingRegion(10, 20, 80, 75);
    dc.SetClippingRegion(0, 0, 0, 0);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneRegionOverTransformedDC(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region inside DC area
    // with applied some transformations.
    wxRect r1(-5, -10, 80, 75);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    dc.SetClippingRegion(r1);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    wxRect r2;
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(0 - parentDcOrigin.x, 0 - parentDcOrigin.y);
        wxSize dim = dc.DeviceToLogicalRel(s_dcSize);
        r2 = wxRect(pos.x, pos.y, dim.x, dim.y);
    }
    else
    {
        r2 = wxRect(dc.DeviceToLogicalX(0 - parentDcOrigin.x),
                    dc.DeviceToLogicalY(0 - parentDcOrigin.y),
                    dc.DeviceToLogicalXRel(s_dcSize.GetWidth()),
                    dc.DeviceToLogicalYRel(s_dcSize.GetHeight()));
    }
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );
    if ( checkExtCoords )
    {
        wxPoint pos = dc.LogicalToDevice(r.GetLeft(), r.GetTop());
        wxSize dim = dc.LogicalToDeviceRel(r.GetWidth(), r.GetHeight());
        CheckClipBox(dc, bmp,
                     r.GetLeft(), r.GetTop(),
                     r.GetWidth(), r.GetHeight(),
                     pos.x + parentDcOrigin.x, pos.y + parentDcOrigin.y, dim.x, dim.y);
    }
    else
    {
        CheckClipBox(dc, bmp,
                     r.GetLeft(), r.GetTop(),
                     r.GetWidth(), r.GetHeight(),
                     dc.LogicalToDeviceX(r.GetLeft()) + parentDcOrigin.x,
                     dc.LogicalToDeviceY(r.GetTop()) + parentDcOrigin.y,
                     dc.LogicalToDeviceXRel(r.GetWidth()),
                     dc.LogicalToDeviceYRel(r.GetHeight()));
    }
}

static void OneRegionOverRotatedDC(wxDC& dc)
{
    // Setting one clipping region inside DC area
    // with applied rotation transformation.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( !dc.CanUseTransformMatrix() )
        return;

    // Setting one clipping region inside DC area.
    const int x = 0;
    const int y = 0;
    const int w = 40;
    const int h = 30;

    // Apply transformation to DC.
    wxAffineMatrix2D m;
    m.Translate(30, 20);
    m.Scale(1.5, 1.5);
    m.Rotate(wxDegToRad(15));
    dc.SetTransformMatrix(m);

    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    // Currently only wxGCDC with Direct2D renderer returns
    // exact clipping box (+/- 1 pixel) for rotated DC.
    // For other DCs returned clipping box is not the smallest one.
#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if ( gc && gc->GetRenderer() == wxGraphicsRenderer::GetDirect2DRenderer() )
    {
        CheckClipPos(dc, x, y, w, h, 1);
    }
    else
#endif // __WXMSW__ && wxUSE_GRAPHICS_DIRECT2D
    {
        wxRect clipRect;
        dc.GetClippingBox(clipRect);
        wxRect minRect(x, y, w, h);
        CHECK(clipRect.Contains(minRect));
    }

    // Check clipping box coordinates in unrotated coordinate system
    dc.ResetTransformMatrix();

    // Calculate expected clipping box.
    double x1, y1, x2, y2;
    double xx, yy;
    // Top-left corner
    xx = x;
    yy = y;
    m.TransformPoint(&xx, &yy);
    x1 = xx;
    y1 = yy;
    x2 = xx;
    y2 = yy;
    // Top-right corner
    xx = x + w;
    yy = y;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);
    // Bottom-right corner
    xx = x + w;
    yy = y + h;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);
    // Bottom-left corner
    xx = x;
    yy = y + h;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);

    int clipX = wxRound(x1);
    int clipY = wxRound(y1);
    int clipW = wxRound(x2) - wxRound(x1);
    int clipH = wxRound(y2) - wxRound(y1);
    CheckClipPos(dc, clipX, clipY, clipW, clipH, 1);
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void OneRegionAndDCTransformation(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region inside DC area
    // and applying DC transformation afterwards.
    wxRect r(-5, -7, 26, 21);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(52, 87);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(20, 24);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-16, -21);
    }
    dc.SetClippingRegion(r);
    dc.SetBackground(wxBrush(s_tmpColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(48, 87);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(16, 24);
    }

    // This should entirely overpaint previous clipping area.
    wxCoord x, y, w, h;
    dc.GetClippingBox(&x, &y, &w, &h);
    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    const int clipX = -3;
    const int clipY = -7;
    const int clipW = 26;
    const int clipH = 21;
    if ( checkExtCoords )
    {
        wxPoint pos = dc.LogicalToDevice(clipX, clipY);
        wxSize dim = dc.LogicalToDeviceRel(clipW, clipH);
        CheckClipBox(dc, bmp,
                     clipX, clipY, clipW, clipH,
                     pos.x + parentDcOrigin.x, pos.y + parentDcOrigin.y, dim.x, dim.y);
    }
    else
    {
        CheckClipBox(dc, bmp,
                     clipX, clipY, clipW, clipH,
                     dc.LogicalToDeviceX(clipX) + parentDcOrigin.x,
                     dc.LogicalToDeviceY(clipY) + parentDcOrigin.y,
                     dc.LogicalToDeviceXRel(clipW),
                     dc.LogicalToDeviceYRel(clipH));
    }
}

static void OneRegionRTL(wxDC& dc, const wxBitmap& bmp)
{
    dc.SetLayoutDirection(wxLayout_RightToLeft);
    if ( dc.GetLayoutDirection() != wxLayout_RightToLeft )
    {
        // Skipping test because RTL layout direction is not supported
        return;
    }

#ifdef __WXGTK__
    wxUnusedVar(bmp);
    WARN("Skipping test known to fail in wxGTK");
#else
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 60;
    const int h = 75;

    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    int x2 = x + w - 1; // right physical edge becomes left logical edge
    CheckClipBox(dc, bmp,
        x, y, w, h,
        (s_dcSize.x-1)-x2, y, w, h);
#endif
}

static void TwoRegionsOverlapping(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region and next another region (partially overlapping).
    // Final clipping box should be an intersection of these two boxes.
    wxRect r1(10, 20, 80, 75);
    wxRect r2(50, 60, 50, 40);
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

    dc.SetClippingRegion(r1);
    dc.SetClippingRegion(r2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                 r.GetWidth(), r.GetHeight());
}

static void TwoRegionsOverlappingNegDim(wxDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
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

    dc.SetClippingRegion(x1, y1, w1, h1);
    dc.SetClippingRegion(x2, y2, w2, h2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 r.GetLeft(), r.GetTop(),
                 r.GetWidth(), r.GetHeight(),
                 r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                 r.GetWidth(), r.GetHeight());
}

static void TwoRegionsNonOverlapping(wxDC& dc, const wxBitmap& bmp)
{
    // Setting one clipping region and next another region (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    REQUIRE( !r1.Intersects(r2) );

    dc.SetClippingRegion(r1);
    dc.SetClippingRegion(r2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void TwoRegionsNonOverlappingNegDim(wxDC& dc, const wxBitmap& bmp)
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
    REQUIRE( !r1.Intersects(r2) );

    dc.SetClippingRegion(x1, y1, w1, h1);
    dc.SetClippingRegion(x2, y2, w2, h2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneDevRegion(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // inside transformed DC area.
    const int x = 10;
    const int y = 21;
    const int w = 80;
    const int h = 75;

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(x, y, w, h);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(x, y);
        wxSize dim = dc.DeviceToLogicalRel(w, h);
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     x + parentDcOrigin.x, y + parentDcOrigin.y, w, h);
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(x),
                     dc.DeviceToLogicalY(y),
                     dc.DeviceToLogicalXRel(w),
                     dc.DeviceToLogicalYRel(h),
                     x + parentDcOrigin.x, y + parentDcOrigin.y, w, h);
    }
}

static void OneDevRegionRTL(wxDC& dc, const wxBitmap& bmp, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    dc.SetLayoutDirection(wxLayout_RightToLeft);
    if ( dc.GetLayoutDirection() != wxLayout_RightToLeft )
    {
        // Skipping test because RTL layout direction is not supported
        return;
    }

#ifdef __WXGTK__
    wxUnusedVar(bmp);
    WARN("Skipping test known to fail in wxGTK");
#else
    // Setting one clipping region in device coordinates
    // inside transformed DC area.
    const int x = 10;
    const int y = 21;
    const int w = 79;
    const int h = 75;

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(x, y, w, h);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    wxPoint pos = dc.DeviceToLogical(x+w-1, y); // right physical edge becomes left logical edge
    wxSize dim = dc.DeviceToLogicalRel(-w, h);
    CheckClipBox(dc, bmp,
                 pos.x, pos.y, dim.x, dim.y,
                 x, y, w, h);
#endif
}

static void OneLargeDevRegion(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates larger
    // then transformed DC surface.
    // Final clipping box should be limited to the DC extents.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(-20, -30, s_dcSize.GetWidth()+30, s_dcSize.GetHeight()+50);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(0 - parentDcOrigin.x, 0 - parentDcOrigin.y);
        wxSize dim = dc.DeviceToLogicalRel(s_dcSize);
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(0 - parentDcOrigin.x),
                     dc.DeviceToLogicalY(0 - parentDcOrigin.y),
                     dc.DeviceToLogicalXRel(s_dcSize.GetWidth()),
                     dc.DeviceToLogicalYRel(s_dcSize.GetHeight()),
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
}


static void OneOuterDevRegion(wxDC& dc, const wxBitmap& bmp, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // entirely outside transformed DC surface.
    // Final clipping box should be empty.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(300, 80, 20, 40);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc,bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneDevRegionNegDim(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 19;
    const int y = 23;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h);
    wxRect r2(0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(x, y, w, h);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(r.GetPosition());
        wxSize dim = dc.DeviceToLogicalRel(r.GetSize());
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(r.GetLeft()),
                     dc.DeviceToLogicalY(r.GetTop()),
                     dc.DeviceToLogicalXRel(r.GetWidth()),
                     dc.DeviceToLogicalYRel(r.GetHeight()),
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
}

static void DrawFigure(wxDC& dc, int n, const wxPoint points[], const wxPoint& ofs)
{
    wxVector<wxPoint> pts;
    for (int i = 0; i < n; i++)
    {
        pts.push_back(points[i] + ofs);
    }
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    dc.SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.SetPen(wxPen(s_fgColour));
    dc.DrawPolygon(n, &pts[0]);
}

static void OneDevRegionNonRect(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords,  bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one triangular clipping region in device coordinates.
    const wxPoint poly[3] =
    {
        wxPoint(13, 25),
        wxPoint(68, 38),
        wxPoint(40, 92)
    };

    // Expected clipping box in device coordinates.
    const int clipX = 14;
    const int clipY = 26;
    const int clipW = 54;
    const int clipH = 66;

    // Draw image with reference triangle.
    wxBitmap bmpRef(s_dcSize);
    wxMemoryDC memDC(bmpRef);
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if ( gc )
    {
        wxGraphicsRenderer* rend = gc->GetRenderer();
        gc = rend->CreateContext(memDC);
        gc->SetAntialiasMode(wxANTIALIAS_NONE);
        gc->DisableOffset();
        wxGCDC gdc(gc);
        DrawFigure(gdc, WXSIZEOF(poly), poly, parentDcOrigin);
    }
    else
#endif // wxUSE_GRAPHICS_CONTEXT
    {
        DrawFigure(memDC, WXSIZEOF(poly), poly, parentDcOrigin);
    }

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion r(WXSIZEOF(poly), poly);
    dc.SetDeviceClippingRegion(r);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    // Check clipping box parameters and compare
    // filled in clipping region with reference triangle.
    // Triangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(clipX, clipY);
        wxSize dim = dc.DeviceToLogicalRel(clipW, clipH);
        CheckClipPos(dc, pos.x, pos.y, dim.x, dim.y, 1);
    }
    else
    {
        CheckClipPos(dc,
                     dc.DeviceToLogicalX(clipX),
                     dc.DeviceToLogicalY(clipY),
                     dc.DeviceToLogicalXRel(clipW),
                     dc.DeviceToLogicalYRel(clipH), 1);
    }
    CheckClipShape(dc, bmp, bmpRef, 1);
}

static void OneDevRegionAndReset(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // and next destroy it.
    // Final clipping box should be the same as DC surface.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(10, 20, 80, 75);
    dc.SetDeviceClippingRegion(reg);
    dc.DestroyClippingRegion();
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(0 - parentDcOrigin.x, 0 - parentDcOrigin.y);
        wxSize dim = dc.DeviceToLogicalRel(s_dcSize);
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(0 - parentDcOrigin.x),
                     dc.DeviceToLogicalY(0 - parentDcOrigin.y),
                     dc.DeviceToLogicalXRel(s_dcSize.GetWidth()),
                     dc.DeviceToLogicalYRel(s_dcSize.GetHeight()),
                     0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    }
}

static void OneDevRegionAndEmpty(wxDC& dc, const wxBitmap& bmp, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // and next an empty region.
    // Final clipping box should empty.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg1(10, 20, 80, 75);
    dc.SetDeviceClippingRegion(reg1);
    wxRegion reg2(0, 0, 0, 0);
    dc.SetDeviceClippingRegion(reg2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneDevRegionOverTransformedDC(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Set one clipping region in device coordinates inside
    // DC area with applied some transformations.
    wxRect r1(18, 35, 60, 75);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg(r1);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(r.GetPosition());
        wxSize dim = dc.DeviceToLogicalRel(r.GetSize());
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                     r.GetWidth(), r.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(r.GetLeft()),
                     dc.DeviceToLogicalY(r.GetTop()),
                     dc.DeviceToLogicalXRel(r.GetWidth()),
                     dc.DeviceToLogicalYRel(r.GetHeight()),
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                     r.GetWidth(), r.GetHeight());
    }
}

static void OneDevRegionOverRotatedDC(wxDC& dc)
{
    // Set one clipping region in device coordinates inside
    // DC area with applied rotation transformation.
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( !dc.CanUseTransformMatrix() )
        return;

    // Setting one clipping region inside DC area.
    const int x = 28;
    const int y = 35;
    const int w = 60;
    const int h = 75;

    wxAffineMatrix2D m;
    m.Translate(30, 20);
    m.Scale(1.5, 1.5);
    m.Rotate(wxDegToRad(15));
    dc.SetTransformMatrix(m);

    wxRegion reg(wxRect(x, y, w, h));
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    // Calculate expected clipping box.
    m.Invert();
    double x1, y1, x2, y2;
    double xx, yy;
    // Top-left corner
    xx = x;
    yy = y;
    m.TransformPoint(&xx, &yy);
    x1 = xx;
    y1 = yy;
    x2 = xx;
    y2 = yy;
    // Top-right corner
    xx = x + w;
    yy = y;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);
    // Bottom-right corner
    xx = x + w;
    yy = y + h;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);
    // Bottom-left corner
    xx = x;
    yy = y + h;
    m.TransformPoint(&xx, &yy);
    x1 = wxMin(x1, xx);
    y1 = wxMin(y1, yy);
    x2 = wxMax(x2, xx);
    y2 = wxMax(y2, yy);

    int clipX = wxRound(x1);
    int clipY = wxRound(y1);
    int clipW = wxRound(x2) - wxRound(x1);
    int clipH = wxRound(y2) - wxRound(y1);
    CheckClipPos(dc, clipX, clipY, clipW, clipH, 1);
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void OneDevRegionAndDCTransformation(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Set one clipping region in device coordinates inside
    // DC area and apply DC transformation afterwards.
    wxRect r1(28, 60, 60, 75);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(42, 78);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-16, -21);
    }

    wxRegion reg(r1);
    dc.SetDeviceClippingRegion(reg);
    dc.SetBackground(wxBrush(s_tmpColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(48, 87);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(16, 24);
    }

    // This should entirely overpaint previous clipping area.
    wxCoord x, y, w, h;
    dc.GetClippingBox(&x, &y, &w, &h);
    dc.SetClippingRegion(x, y, w, h);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    wxRect r2(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(r.GetPosition());
        wxSize dim = dc.DeviceToLogicalRel(r.GetSize());
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                     r.GetWidth(), r.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(r.GetLeft()),
                     dc.DeviceToLogicalY(r.GetTop()),
                     dc.DeviceToLogicalXRel(r.GetWidth()),
                     dc.DeviceToLogicalYRel(r.GetHeight()),
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y,
                     r.GetWidth(), r.GetHeight());
    }
}

static void TwoDevRegionsOverlapping(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // and next another region in device coordinates (partially overlapping).
    // Final clipping box should be an intersection of these two regions.
    wxRect r1(30, 39, 40, 30);
    wxRect r2(60, 51, 40, 30);
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg1(r1);
    dc.SetDeviceClippingRegion(reg1);
    wxRegion reg2(r2);
    dc.SetDeviceClippingRegion(reg2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(r.GetPosition());
        wxSize dim = dc.DeviceToLogicalRel(r.GetSize());
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(r.GetLeft()),
                     dc.DeviceToLogicalY(r.GetTop()),
                     dc.DeviceToLogicalXRel(r.GetWidth()),
                     dc.DeviceToLogicalYRel(r.GetHeight()),
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
}

static void TwoDevRegionsOverlappingNegDim(wxDC& dc, const wxBitmap& bmp, bool checkExtCoords, bool useTransformMatrix, const wxPoint& parentDcOrigin)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates with negative size values
    // and next another region in device coordinates (partially overlapping).
    // Final clipping box should be an intersection of these two regions
    // with positive size values.
    const int x1 = 31;
    const int y1 = 40;
    const int w1 = -80;
    const int h1 = -95;

    const int x2 = 20;
    const int y2 = 30;
    const int w2 = 40;
    const int h2 = 30;
    wxClipRect r1(x1, y1, w1, h1);
    wxRect r2(x2, y2, w2, h2);
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg1(x1, y1, w1, h1);
    dc.SetDeviceClippingRegion(reg1);
    wxRegion reg2(x2, y2, w2, h2);
    dc.SetDeviceClippingRegion(reg2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    if ( checkExtCoords )
    {
        wxPoint pos = dc.DeviceToLogical(r.GetPosition());
        wxSize dim = dc.DeviceToLogicalRel(r.GetSize());
        CheckClipBox(dc, bmp,
                     pos.x, pos.y, dim.x, dim.y,
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
    else
    {
        CheckClipBox(dc, bmp,
                     dc.DeviceToLogicalX(r.GetLeft()),
                     dc.DeviceToLogicalY(r.GetTop()),
                     dc.DeviceToLogicalXRel(r.GetWidth()),
                     dc.DeviceToLogicalYRel(r.GetHeight()),
                     r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
    }
}

static void TwoDevRegionsNonOverlapping(wxDC& dc, const wxBitmap& bmp, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // Setting one clipping region in device coordinates
    // and next another region in device coordinates (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    REQUIRE( !r1.Intersects(r2) );

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg1(r1);
    dc.SetDeviceClippingRegion(reg1);
    wxRegion reg2(r2);
    dc.SetDeviceClippingRegion(reg2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void TwoDevRegionsNonOverlappingNegDim(wxDC& dc, const wxBitmap& bmp, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

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
    REQUIRE( !r1.Intersects(r2) );

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix )
    {
        wxAffineMatrix2D m;
        m.Translate(40, 75);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);
    }
    else
#endif // wxUSE_DC_TRANSFORM_MATRIX
    {
        dc.SetDeviceOrigin(10, 15);
        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);
    }
    wxRegion reg1(x1, y1, w1, h1);
    dc.SetDeviceClippingRegion(reg1);
    wxRegion reg2(x2, y2, w2, h2);
    dc.SetDeviceClippingRegion(reg2);
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    CheckClipBox(dc, bmp,
                 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void DcAttributes(wxDC& dc)
{
    // Check if wxDC atrributes left unchanged
    wxFont font = dc.GetFont().Bold().Smaller();
    wxPen pen(*wxYELLOW, 2);
    wxBrush brush = *wxBLUE_BRUSH;

    wxDCFontChanger fontChanger(dc, font);
    wxDCPenChanger penChanger(dc, pen);
    wxDCBrushChanger brushChanger(dc, brush);
    wxCoord chWidth = dc.GetCharWidth();
    wxCoord chHeight = dc.GetCharHeight();
    wxFontMetrics fm = dc.GetFontMetrics();

    dc.SetClippingRegion(10, 20, 30, 40);
    dc.DestroyClippingRegion();

    CHECK(dc.GetFont() == font);
    CHECK(dc.GetPen() == pen);
    CHECK(dc.GetBrush() == brush);
    CHECK(dc.GetCharWidth() == chWidth);
    CHECK(dc.GetCharHeight() == chHeight);
    wxFontMetrics fm2 = dc.GetFontMetrics();
    CHECK(fm2.ascent == fm.ascent);
    CHECK(fm2.averageWidth == fm.averageWidth);
    CHECK(fm2.descent == fm.descent);
    CHECK(fm2.externalLeading == fm.externalLeading);
    CHECK(fm2.height == fm.height);
    CHECK(fm2.internalLeading == fm.internalLeading);
}

// Tests specific to wxGCDC
#if wxUSE_GRAPHICS_CONTEXT
static void InitialStateWithRotatedGCForDC(wxGCDC& dc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Initial wxGCDC clipping box with rotated wxGraphicsContext.
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    gc->Rotate(wxDegToRad(6));
    {
        // Flush cashed clipping region extents
        wxPoint org = dc.GetDeviceOrigin();
        dc.SetDeviceOrigin(org.x + 1, org.y);
        dc.SetDeviceOrigin(org.x, org.y);
    }
    dc.SetBackground(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    // Calculate expected clipping box.
    int clipX, clipY, clipW, clipH;
    wxGraphicsMatrix m = gc->GetTransform();
    m.Invert();
    double x1, y1, x2, y2;
    double x, y;
    // Top-left corner
    x = 0.0 - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = x;
    y1 = y;
    x2 = x;
    y2 = y;
    // Top-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = 0.0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-right corner
    x = s_dcSize.GetWidth() - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    x2 = wxMax(x2, x);
    y2 = wxMax(y2, y);
    // Bottom-left corner
    x = 0.0 - parentDcOrigin.x;
    y = s_dcSize.GetHeight() - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    x1 = wxMin(x1, x);
    y1 = wxMin(y1, y);
    y2 = wxMax(y2, y);

    clipX = wxRound(x1);
    clipY = wxRound(y1);
    clipW = wxRound(x2)-wxRound(x1);
    clipH = wxRound(y2)-wxRound(y1);

    CheckClipBox(dc, bmp,
                 clipX, clipY, clipW, clipH,
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}
#endif // wxUSE_GRAPHICS_CONTEXT

TEST_CASE("ClippingBoxTestCase::wxDC", "[clip][dc]")
{
    wxBitmap bmp(s_dcSize);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    SECTION("InitialState")
    {
        InitialState(dc, bmp, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, wxPoint());
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, wxPoint());
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, wxPoint());
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, wxPoint());
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, wxPoint());
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, wxPoint());
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, wxPoint());
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, wxPoint());
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }

    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionOverRotatedDC")
    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping (dc, bmp, true, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }
}

#if wxUSE_GRAPHICS_CONTEXT
TEST_CASE("ClippingBoxTestCase::wxGCDC", "[clip][dc][gcdc]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    mdc.Clear();
    mdc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    mdc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    mdc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -mdc.DeviceToLogical(0, 0);
    wxGCDC dc(mdc);
    dc.GetGraphicsContext()->SetAntialiasMode(wxANTIALIAS_NONE);
    dc.GetGraphicsContext()->DisableOffset();
    wxRect gdcClip;
    dc.GetClippingBox(gdcClip);
    REQUIRE(dcClip == gdcClip);
    REQUIRE(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(dc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, dcOrigin);
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }
    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionOverRotatedDC")
    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }

    SECTION("InitialStateWithRotatedGCForDC")
    {
        InitialStateWithRotatedGCForDC(dc, bmp, dcOrigin);
    }
}

#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("ClippingBoxTestCase::wxGCDC(GDI+)", "[clip][dc][gcdc][gdiplus]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    mdc.Clear();
    mdc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    mdc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    mdc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -mdc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);
    wxRect gdcClip;
    dc.GetClippingBox(gdcClip);
    REQUIRE(dcClip == gdcClip);
    REQUIRE(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(dc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, dcOrigin);
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }

    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionOverRotatedDC")

    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }

    SECTION("InitialStateWithRotatedGCForDC")
    {
        InitialStateWithRotatedGCForDC(dc, bmp, dcOrigin);
    }
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("ClippingBoxTestCase::wxGCDC(Direct2D)", "[clip][dc][gcdc][direct2d]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    mdc.Clear();
    mdc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    mdc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    mdc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -mdc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);
    wxRect gdcClip;
    dc.GetClippingBox(gdcClip);
    REQUIRE(dcClip == gdcClip);
    REQUIRE(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(dc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, dcOrigin);
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }

    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionOverRotatedDC")
    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }

    SECTION("InitialStateWithRotatedGCForDC")
    {
        InitialStateWithRotatedGCForDC(dc, bmp, dcOrigin);
    }
}
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
TEST_CASE("ClippingBoxTestCase::wxGCDC(Cairo)", "[clip][dc][gcdc][cairo]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    mdc.Clear();
    mdc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    mdc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    mdc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -mdc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);
    wxRect gdcClip;
    dc.GetClippingBox(gdcClip);
    REQUIRE(dcClip == gdcClip);
    REQUIRE(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(dc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, dcOrigin);
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }

    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, dcOrigin);
    }

    SECTION("OneDevRegionOverRotatedDC")
    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, dcOrigin);
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, dcOrigin);
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, dcOrigin);
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }

    SECTION("InitialStateWithRotatedGCForDC")
    {
        InitialStateWithRotatedGCForDC(dc, bmp, dcOrigin);
    }
}
#endif // wxUSE_CAIRO
#endif // wxUSE_GRAPHICS_CONTEXT

#if wxUSE_SVG
TEST_CASE("ClippingBoxTestCase::wxSVGFileDC", "[clip][dc][svgdc]")
{
    wxBitmap bmp; // We need wxNullBitmap because we can't check the output
    TestFile tf;
    wxSVGFileDC dc(tf.GetName(), s_dcSize.x, s_dcSize.y);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();

    SECTION("InitialState")
    {
        InitialState(dc, bmp, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC 1")
    {
        InitialStateWithTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC 2")
    {
        InitialStateWithTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("InitialStateWithTransformedDC Transform Matrix")
    {
        InitialStateWithTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("InitialStateWithRotatedDC")
    {
        InitialStateWithRotatedDC(dc, bmp, wxPoint());
    }

    SECTION("SameRegionRepeatRotatedDC")
    {
        SameRegionRepeatRotatedDC(dc, wxPoint());
    }

    SECTION("OneRegion")
    {
        OneRegion(dc, bmp, wxPoint());
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(dc, bmp, wxPoint());
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(dc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(dc, bmp, wxPoint());
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(dc, bmp, wxPoint());
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(dc, bmp);
    }

    SECTION("OneRegionOverTransformedDC 1")
    {
        OneRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneRegionOverTransformedDC 2")
    {
        OneRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneRegionOverTransformedDC Transform Matrix")
    {
        OneRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneRegionOverRotatedDC")
    {
        OneRegionOverRotatedDC(dc);
    }

    SECTION("OneRegionAndDCTransformation 1")
    {
        OneRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneRegionAndDCTransformation 2")
    {
        OneRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneRegionAndDCTransformation Transform Matrix")
    {
        OneRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneRegionRTL")
    {
        OneRegionRTL(dc, bmp);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(dc, bmp, wxPoint());
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(dc, bmp, wxPoint());
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(dc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(dc, bmp);
    }

    SECTION("OneDevRegion 1")
    {
        OneDevRegion(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegion 2")
    {
        OneDevRegion(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegion Transform Matrix")
    {
        OneDevRegion(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionRTL")
    {
        OneDevRegionRTL(dc, bmp, false);
    }

    SECTION("OneDevRegionRTL TransformMatrix")
    {
        OneDevRegionRTL(dc, bmp, true);
    }

    SECTION("OneLargeDevRegion 1")
    {
        OneLargeDevRegion(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneLargeDevRegion 2")
    {
        OneLargeDevRegion(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneLargeDevRegion Transform Matrix")
    {
        OneLargeDevRegion(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneOuterDevRegion")
    {
        OneOuterDevRegion(dc, bmp, false);
    }

    SECTION("OneOuterDevRegion Transform Matrix")
    {
        OneOuterDevRegion(dc, bmp, true);
    }

    SECTION("OneDevRegionNegDim 1")
    {
        OneDevRegionNegDim(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionNegDim 2")
    {
        OneDevRegionNegDim(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionNegDim Transform Matrix")
    {
        OneDevRegionNegDim(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionNonRect 1")
    {
        OneDevRegionNonRect(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionNonRect 2")
    {
        OneDevRegionNonRect(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionNonRect Transform Matrix")
    {
        OneDevRegionNonRect(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionAndReset 1")
    {
        OneDevRegionAndReset(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionAndReset 2")
    {
        OneDevRegionAndReset(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionAndReset Transform Matrix")
    {
        OneDevRegionAndReset(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionAndEmpty")
    {
        OneDevRegionAndEmpty(dc, bmp, false);
    }

    SECTION("OneDevRegionAndEmpty Transform Matrix")
    {
        OneDevRegionAndEmpty(dc, bmp, true);
    }

    SECTION("OneDevRegionOverTransformedDC 1")
    {
        OneDevRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionOverTransformedDC 2")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionOverTransformedDC Transform Matrix")
    {
        OneDevRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
    }

    SECTION("OneDevRegionOverRotatedDC")
    {
        OneDevRegionOverRotatedDC(dc);
    }

    SECTION("OneDevRegionAndDCTransformation 1")
    {
        OneDevRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
    }

    SECTION("OneDevRegionAndDCTransformation 2")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
    }

    SECTION("OneDevRegionAndDCTransformation Transform Matrix")
    {
        OneDevRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping 1")
    {
        TwoDevRegionsOverlapping(dc, bmp, false, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping 2")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlapping Transform Matrix")
    {
        TwoDevRegionsOverlapping(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim 1")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim 2")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, wxPoint());
    }

    SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, wxPoint());
    }

    SECTION("TwoDevRegionsNonOverlapping")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
    {
        TwoDevRegionsNonOverlapping(dc, bmp, true);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
    }

    SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
    {
        TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
    }

    SECTION("DCAttributes")
    {
        DcAttributes(dc);
    }
}
#endif // wxUSE_SVG

TEST_CASE("ClippingBoxTestCase::wxPaintDC", "[clip][dc][paintdc]")
{
    wxBitmap bmp; // We need wxNullBitmap because we can't check the output
    // Ensure window is shown and large enough for testing
    wxTheApp->GetTopWindow()->Raise();
    REQUIRE(wxTheApp->GetTopWindow()->IsShown());
    wxSize winSize = wxTheApp->GetTopWindow()->GetSize();
    winSize.x = wxMax(winSize.x, s_dcSize.x + 50);
    winSize.y = wxMax(winSize.y, s_dcSize.y + 50);
    wxTheApp->GetTopWindow()->SetSize(winSize);
#if defined(__WXGTK__)
    // Under wxGTK we need to have two children (at least) because if there
    // is one child its paint area is set to fill the whole parent frame.
    wxScopedPtr<wxWindow> w0(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));
#endif // wxGTK
    wxScopedPtr<wxWindow> win(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY, wxPoint(0, 0)));
    win->SetClientSize(s_dcSize);

    // Wait for the first paint event to be sure
    // that window really has its final size.
    wxWindow* testWin = win.get();
    {
        WaitForPaint waitForPaint(testWin);
        testWin->Show();
        waitForPaint.YieldUntilPainted();
    }

    bool paintExecuted = false;
    testWin->Bind(wxEVT_PAINT, [=, &paintExecuted](wxPaintEvent&)
    {
        wxPaintDC dc(testWin);
        REQUIRE(dc.GetSize() == s_dcSize);
        dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        dc.Clear();

        SECTION("InitialState")
        {
            InitialState(dc, bmp, wxPoint());
        }

        SECTION("InitialStateWithTransformedDC 1")
        {
            InitialStateWithTransformedDC(dc, bmp, false, false, wxPoint());
        }

        SECTION("InitialStateWithTransformedDC 2")
        {
            InitialStateWithTransformedDC(dc, bmp, true, false, wxPoint());
        }

        SECTION("InitialStateWithTransformedDC Transform Matrix")
        {
            InitialStateWithTransformedDC(dc, bmp, true, true, wxPoint());
        }

        SECTION("InitialStateWithRotatedDC")
        {
            InitialStateWithRotatedDC(dc, bmp, wxPoint());
        }

        SECTION("SameRegionRepeatRotatedDC")
        {
            SameRegionRepeatRotatedDC(dc, wxPoint());
        }

        SECTION("OneRegion")
        {
            OneRegion(dc, bmp, wxPoint());
        }

        SECTION("OneLargeRegion")
        {
            OneLargeRegion(dc, bmp, wxPoint());
        }

        SECTION("OneOuterRegion")
        {
            OneOuterRegion(dc, bmp);
        }

        SECTION("OneRegionNegDim")
        {
            OneRegionNegDim(dc, bmp, wxPoint());
        }

        SECTION("OneRegionAndReset")
        {
            OneRegionAndReset(dc, bmp, wxPoint());
        }

        SECTION("OneRegionAndEmpty")
        {
            OneRegionAndEmpty(dc, bmp);
        }

        SECTION("OneRegionOverTransformedDC 1")
        {
            OneRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneRegionOverTransformedDC 2")
        {
            OneRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneRegionOverTransformedDC Transform Matrix")
        {
            OneRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneRegionOverRotatedDC")
        {
            OneRegionOverRotatedDC(dc);
        }

        SECTION("OneRegionAndDCTransformation 1")
        {
            OneRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneRegionAndDCTransformation 2")
        {
            OneRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneRegionAndDCTransformation Transform Matrix")
        {
            OneRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneRegionRTL")
        {
            OneRegionRTL(dc, bmp);
        }

        SECTION("TwoRegionsOverlapping")
        {
            TwoRegionsOverlapping(dc, bmp, wxPoint());
        }

        SECTION("TwoRegionsOverlappingNegDim")
        {
            TwoRegionsOverlappingNegDim(dc, bmp, wxPoint());
        }

        SECTION("TwoRegionsNonOverlapping")
        {
            TwoRegionsNonOverlapping(dc, bmp);
        }

        SECTION("TwoRegionsNonOverlappingNegDim")
        {
            TwoRegionsNonOverlappingNegDim(dc, bmp);
        }

        SECTION("OneDevRegion 1")
        {
            OneDevRegion(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegion 2")
        {
            OneDevRegion(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegion Transform Matrix")
        {
            OneDevRegion(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneDevRegionRTL")
        {
            OneDevRegionRTL(dc, bmp, false);
        }

        SECTION("OneDevRegionRTL TransformMatrix")
        {
            OneDevRegionRTL(dc, bmp, true);
        }

        SECTION("OneLargeDevRegion 1")
        {
            OneLargeDevRegion(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneLargeDevRegion 2")
        {
            OneLargeDevRegion(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneLargeDevRegion Transform Matrix")
        {
            OneLargeDevRegion(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneOuterDevRegion")
        {
            OneOuterDevRegion(dc, bmp, false);
        }

        SECTION("OneOuterDevRegion Transform Matrix")
        {
            OneOuterDevRegion(dc, bmp, true);
        }

        SECTION("OneDevRegionNegDim 1")
        {
            OneDevRegionNegDim(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegionNegDim 2")
        {
            OneDevRegionNegDim(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegionNegDim Transform Matrix")
        {
            OneDevRegionNegDim(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneDevRegionNonRect 1")
        {
            OneDevRegionNonRect(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegionNonRect 2")
        {
            OneDevRegionNonRect(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegionNonRect Transform Matrix")
        {
            OneDevRegionNonRect(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneDevRegionAndReset 1")
        {
            OneDevRegionAndReset(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegionAndReset 2")
        {
            OneDevRegionAndReset(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegionAndReset Transform Matrix")
        {
            OneDevRegionAndReset(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneDevRegionAndEmpty")
        {
            OneDevRegionAndEmpty(dc, bmp, false);
        }

        SECTION("OneDevRegionAndEmpty Transform Matrix")
        {
            OneDevRegionAndEmpty(dc, bmp, true);
        }

        SECTION("OneDevRegionOverTransformedDC 1")
        {
            OneDevRegionOverTransformedDC(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegionOverTransformedDC 2")
        {
            OneDevRegionOverTransformedDC(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegionOverTransformedDC Transform Matrix")
        {
            OneDevRegionOverTransformedDC(dc, bmp, true, true, wxPoint());
        }

        SECTION("OneDevRegionOverRotatedDC")
        {
            OneDevRegionOverRotatedDC(dc);
        }

        SECTION("OneDevRegionAndDCTransformation 1")
        {
            OneDevRegionAndDCTransformation(dc, bmp, false, false, wxPoint());
        }

        SECTION("OneDevRegionAndDCTransformation 2")
        {
            OneDevRegionAndDCTransformation(dc, bmp, true, false, wxPoint());
        }

        SECTION("OneDevRegionAndDCTransformation Transform Matrix")
        {
            OneDevRegionAndDCTransformation(dc, bmp, true, true, wxPoint());
        }

        SECTION("TwoDevRegionsOverlapping 1")
        {
            TwoDevRegionsOverlapping(dc, bmp, false, false, wxPoint());
        }

        SECTION("TwoDevRegionsOverlapping 2")
        {
            TwoDevRegionsOverlapping(dc, bmp, true, false, wxPoint());
        }

        SECTION("TwoDevRegionsOverlapping Transform Matrix")
        {
            TwoDevRegionsOverlapping(dc, bmp, true, true, wxPoint());
        }

        SECTION("TwoDevRegionsOverlappingNegDim 1")
        {
            TwoDevRegionsOverlappingNegDim(dc, bmp, false, false, wxPoint());
        }

        SECTION("TwoDevRegionsOverlappingNegDim 2")
        {
            TwoDevRegionsOverlappingNegDim(dc, bmp, true, false, wxPoint());
        }

        SECTION("TwoDevRegionsOverlappingNegDim Transform Matrix")
        {
            TwoDevRegionsOverlappingNegDim(dc, bmp, true, true, wxPoint());
        }

        SECTION("TwoDevRegionsNonOverlapping")
        {
            TwoDevRegionsNonOverlapping(dc, bmp, false);
        }

        SECTION("TwoDevRegionsNonOverlapping Transform Matrix")
        {
            TwoDevRegionsNonOverlapping(dc, bmp, true);
        }

        SECTION("TwoDevRegionsNonOverlappingNegDim")
        {
            TwoDevRegionsNonOverlappingNegDim(dc, bmp, false);
        }

        SECTION("TwoDevRegionsNonOverlappingNegDim Transform Matrix")
        {
            TwoDevRegionsNonOverlappingNegDim(dc, bmp, true);
        }

        SECTION("DCAttributes")
        {
            DcAttributes(dc);
        }

        paintExecuted = true;
    });

    testWin->Refresh();
    testWin->Update();
    // Wait for update to be done
    wxStopWatch sw;
    while( sw.Time() < 50 )
    {
         wxYield();
    }

    CHECK(paintExecuted == true);
}

#if wxUSE_GRAPHICS_CONTEXT
// ========================
// wxGraphicsContext tests
// ========================

// Helper functions

static inline void FlushGC(wxScopedPtr<wxGraphicsContext>& gc)
{
    gc->Flush();
#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
    // Apparently, flushing native Direct2D renderer
    // is not enough to update underlying DC (bitmap)
    // and therefore we have to destroy the renderer
    // to do so.
    wxGraphicsRenderer* rend = gc->GetRenderer();
    if ( rend == wxGraphicsRenderer::GetDirect2DRenderer() )
    {
        gc.reset();
    }
#endif // __WXMSW__ && wxUSE_GRAPHICS_DIRECT2D
}

static void CheckClipPos(wxGraphicsContext* gc,
                         int x, int y, int width, int height, int posTolerance = 0)
{
    // Check clipping box boundaries.
    double clipX, clipY, clipW, clipH;
    gc->GetClipBox(&clipX, &clipY, &clipW, &clipH);

    CheckBoxPosition(wxRound(clipX), wxRound(clipY), wxRound(clipW), wxRound(clipH),
                     x, y, width, height, posTolerance);
}

static void CheckClipBox(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp,
                         int x, int y, int w, int h,
                         int xPh, int yPh, int wPh, int hPh)
{
    CheckClipPos(gc.get(), x, y, w, h);

    if (bmp.IsOk())
    {
        // Update wxGraphicsContext contents.
        FlushGC(gc);
        // We check whether diagonal corners
        // of the rectangular clipping box are actually
        // drawn at the edge of the clipping region.
        CheckClipRect(bmp, xPh, yPh, wPh, hPh);
    }
}

static void CheckClipShape(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxBitmap& bmpRef, int posTolerance)
{
    // Update wxGraphicsContext contents.
    FlushGC(gc);

#if wxUSE_IMAGE
    // Figures created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms they can use so we need
    // to accept some shift of pixels.
    CheckClipWithBitmap(bmp, bmpRef, posTolerance);
#endif // wxUSE_IMAGE
}

void ClearGC(wxGraphicsContext* gc)
{
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);
    gc->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->SetCompositionMode(wxCOMPOSITION_SOURCE);
    gc->DrawRectangle(x, y, w, h);
}

// Actual tests

static void InitialState(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Initial clipping box should be the same as the entire GC surface.
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
             0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void InitialStateWithTransformedGC(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Initial clipping box with transformed GC.
    wxGraphicsMatrix m = gc->CreateMatrix();
    m.Translate(10, 15);
    m.Scale(1/2.0, 1/3.0);
    m.Translate(20, 30);
    gc->SetTransform(m);
    ClearGC(gc.get());
    m.Invert();
    double x = 0 - parentDcOrigin.x;
    double y = 0 - parentDcOrigin.y;
    m.TransformPoint(&x, &y);
    double w = s_dcSize.GetWidth();
    double h = s_dcSize.GetHeight();
    m.TransformDistance(&w, &h);
    CheckClipBox(gc, bmp,
                 wxRound(x), wxRound(y), wxRound(w), wxRound(h),
                 0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void OneRegion(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region inside DC area.
    const int x = 10;
    const int y = 20;
    const int w = 80;
    const int h = 75;

    gc->Clip(x, y, w, h);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, x, y, w, h,
                 x + parentDcOrigin.x, y + parentDcOrigin.y, w, h);
}

static void OneLargeRegion(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region larger then GC surface.
    // Final clipping box should be limited to the GC extents.
    gc->Clip(-20, -30,
             s_dcSize.GetWidth()+40, s_dcSize.GetHeight()+60);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
             0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void OneOuterRegion(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp)
{
    // Setting one clipping region entirely outside GC surface.
    // Final clipping box should be empty.
    gc->Clip(-100, -80, 20, 40);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0, 0, 0, 0,
             0, 0, 0, 0);
}

static void OneRegionNegDim(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region with negative sizes values.
    // Final clipping box should have standard positive size values.
    const int x = 20;
    const int y = 30;
    const int w = -80;
    const int h = -75;
    wxClipRect r1(x, y, w, h, 0);
    wxRect r2(0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight());
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

    gc->Clip(x, y, w, h);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
             r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
}

static void OneRegionAndReset(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region and next destroy it.
    // Final clipping box should be the same as GC surface.
    gc->Clip(10, 20, 80, 75);
    gc->ResetClip();
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0 - parentDcOrigin.x, 0 - parentDcOrigin.y, s_dcSize.GetWidth(), s_dcSize.GetHeight(),
                  0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
}

static void OneRegionAndEmpty(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp)
{
    // Setting one clipping region and next an empty box.
    // Final clipping box should empty.
    gc->Clip(10, 20, 80, 75);
    gc->Clip(0, 0, 0, 0);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void OneRegionWithTransformedGC(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region inside GC area
    // with applied some transformations.
    wxRect r1(-10, -12, 80, 75);

    wxGraphicsMatrix m = gc->CreateMatrix();
    m.Translate(10, 15);
    m.Scale(1/2.0, 1/3.0);
    m.Translate(20, 30);
    gc->SetTransform(m);
    gc->Clip(r1.x, r1.y, r1.width, r1.height);
    ClearGC(gc.get());
    m.Invert();
    double x = 0;
    double y = 0;
    m.TransformPoint(&x, &y);
    double w = s_dcSize.GetWidth();
    double h = s_dcSize.GetHeight();
    m.TransformDistance(&w, &h);
    wxRect r2(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );
    wxGraphicsMatrix m2 = gc->GetTransform();
    double xd = r.GetLeft();
    double yd = r.GetTop();
    m2.TransformPoint(&xd, &yd);
    double wd = r.GetWidth();
    double hd = r.GetHeight();
    m2.TransformDistance(&wd, &hd);
    CheckClipBox(gc, bmp, r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
                 wxRound(xd + parentDcOrigin.x), wxRound(yd + parentDcOrigin.y), wxRound(wd), wxRound(hd));
}

static void OneRegionWithRotatedGC(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one rectangular clipping region for rotated graphics context.
    const double rotAngle = wxDegToRad(1.0);
    const int rectX = 16;
    const int rectY = 14;
    const int rectW = 60;
    const int rectH = 55;

    // Set clipping region for rotated wxGC.
    gc->Rotate(rotAngle);
    gc->Clip(rectX, rectY, rectW, rectH);
    // Fill in clipping region.
    ClearGC(gc.get());

    // Draw reference image with rotated rectangle which
    // should look the same as rectangle drawn with Clip().
    wxBitmap bmpRef(s_dcSize);
    {
        wxMemoryDC memDC(bmpRef);
        memDC.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        memDC.Clear();
        memDC.SetDeviceOrigin(parentDcOrigin.x, parentDcOrigin.y);
        wxGraphicsRenderer* rend = gc->GetRenderer();
        wxScopedPtr<wxGraphicsContext> gcRef(rend->CreateContext(memDC));
        gcRef->SetAntialiasMode(wxANTIALIAS_NONE);
        gcRef->DisableOffset();
        gcRef->SetBrush(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(*wxTRANSPARENT_PEN);
        gcRef->DrawRectangle(0, 0, s_dcSize.GetWidth(), s_dcSize.GetHeight());
        gcRef->Rotate(rotAngle);
        gcRef->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(wxPen(s_fgColour));
        gcRef->DrawRectangle(rectX, rectY, rectW, rectH);
    }

    // Compare filled in clipping region with reference rectangle.
    // Rotated rectangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    CheckClipShape(gc, bmp, bmpRef, 1);
}

static void TwoRegionsOverlapping(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
{
    // Setting one clipping region and next another region (partially overlapping).
    // Final clipping box should be an intersection of these two boxes.
    wxRect r1(10, 20, 80, 75);
    wxRect r2(50, 60, 50, 40);
    wxRect r = r1.Intersect(r2);
    REQUIRE( !r.IsEmpty() );

    gc->Clip(r1.x, r1.y, r1.width, r1.height);
    gc->Clip(r2.x, r2.y, r2.width, r2.height);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
             r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
}

static void TwoRegionsOverlappingNegDim(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxPoint& parentDcOrigin)
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

    gc->Clip(x1, y1, w1, h1);
    gc->Clip(x2, y2, w2, h2);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
                 r.GetLeft() + parentDcOrigin.x, r.GetTop() + parentDcOrigin.y, r.GetWidth(), r.GetHeight());
}

static void TwoRegionsNonOverlapping(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp)
{
    // Setting one clipping region and next another region (non-overlapping).
    // Final clipping box should be empty.
    wxRect r1(10, 20, 30, 30);
    wxRect r2(50, 60, 50, 40);
    REQUIRE( !r1.Intersects(r2) );

    gc->Clip(r1.x, r1.y, r1.width, r1.height);
    gc->Clip(r2.x, r2.y, r2.width, r2.height);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void TwoRegionsNonOverlappingNegDim(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp)
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
    REQUIRE( !r1.Intersects(r2) );

    gc->Clip(x1, y1, w1, h1);
    gc->Clip(x2, y2, w2, h2);
    ClearGC(gc.get());
    CheckClipBox(gc, bmp, 0, 0, 0, 0,
                 0, 0, 0, 0);
}

static void RegionsAndPushPopState(wxScopedPtr<wxGraphicsContext>& gc, const wxBitmap& bmp, const wxDC& parentDc)
{
    // Setting muliple rectangular clipping regions
    // for transformed wxGC and store/restore them.
#ifdef __WXOSX__
    if ( !wxCheckOsVersion(10, 13) )
    {
        // Due to the bug in resetting clipping region when graphics state
        // is pushed on the stack we need to skip test on macOS < 10.13.
        WARN("Skipping test known not to work under macOS < 10.13");
        return;
    }
    else
    {
        // Due to the bug in resetting clipping region with
        // CGContextResetClip (https://github.com/wxWidgets/wxWidgets/issues/22629)
        // we need to skip this test because emulated resetting
        // implemented as a workaround doesn't work when PushState() is used.
        WARN("Skipping test known not to work under macOS");
        return;
    }
#endif
    
    // Get rectangle of the entire drawing area.
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);

    // Set clipping regions and store/restore them.
    gc->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
    gc->SetPen(wxPen(s_fgColour));

    gc->Translate(5, 5);
    gc->Rotate(wxDegToRad(5));
    gc->Clip(40, 35, 50, 45);
    gc->PushState();

    gc->Rotate(wxDegToRad(5));
    gc->ResetClip();
    gc->Clip(30, 25, 60, 15);
    gc->PushState();

    gc->Rotate(wxDegToRad(-15));
    gc->ResetClip();
    gc->Clip(25, 30, 30, 35);
    gc->DrawRectangle(x, y, w, h);

    gc->PopState();
    gc->DrawRectangle(x, y, w, h);

    gc->PopState();
    gc->DrawRectangle(x, y, w, h);

    // Draw reference image with rotated rectangles which
    // should look the same as rectangles drawn with Clip().
    wxBitmap bmpRef(s_dcSize);
    {
        wxMemoryDC memDC(bmpRef);
        memDC.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
        memDC.Clear();

        // Apply same basic transformations that are applied to the parent wxDC
        wxPoint org = parentDc.GetDeviceOrigin();
        memDC.SetDeviceOrigin(org.x, org.y);
        double sx, sy;
        parentDc.GetUserScale(&sx, &sy);
        memDC.SetUserScale(sx, sy);
        parentDc.GetLogicalScale(&sx, &sy);
        memDC.SetLogicalScale(sx, sy);
        org = parentDc.GetLogicalOrigin();
        memDC.SetLogicalOrigin(org.x, org.y);

        wxGraphicsRenderer* rend = gc->GetRenderer();
        wxScopedPtr<wxGraphicsContext> gcRef(rend->CreateContext(memDC));
        gcRef->SetAntialiasMode(wxANTIALIAS_NONE);
        gcRef->DisableOffset();

        gcRef->SetBrush(wxBrush(s_fgColour, wxBRUSHSTYLE_SOLID));
        gcRef->SetPen(wxPen(s_fgColour));

        gcRef->Translate(5, 5);
        gcRef->Rotate(wxDegToRad(5));
        gcRef->PushState();

        gcRef->Rotate(wxDegToRad(5));
        gcRef->PushState();

        gcRef->Rotate(wxDegToRad(-15));
        gcRef->DrawRectangle(25, 30, 30, 35);

        gcRef->PopState();
        gcRef->DrawRectangle(30, 25, 60, 15);

        gcRef->PopState();
        gcRef->DrawRectangle(40, 35, 50, 45);
    }

    // Compare filled in clipping regions with reference image.
    // Rotated rectangles created by clipping and drawing procedures
    // can be slightly different (shifted by few pixels) due
    // to the different algorithms used for different operations
    // so we need to perform a "fuzzy" comparison of the images,
    // tolerating some drift of the pixels.
    CheckClipShape(gc, bmp, bmpRef, 1);
}

TEST_CASE("ClippingBoxTestCaseGC::DefaultRenderer", "[clip][gc][default]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);
    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    dc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    dc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    dc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -dc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDefaultRenderer();
    REQUIRE(rend);
    wxScopedPtr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);
    wxRect gcClip(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    CHECK(dcClip == gcClip);
    CHECK(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(gc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedGC")
    {
        InitialStateWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(gc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(gc, bmp);
    }

    SECTION("OneRegionWithTransformedGC")
    {
        OneRegionWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionWithRotatedGC")
    {
        OneRegionWithRotatedGC(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(gc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(gc, bmp);
    }

    SECTION("RegionsAndPushPopState")
    {
        RegionsAndPushPopState(gc, bmp, dc);
    }
}

#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("ClippingBoxTestCaseGC::GDI+Renderer", "[clip][gc][gdiplus]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    dc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    dc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    dc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -dc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    REQUIRE(rend);
    wxScopedPtr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);
    wxRect gcClip(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    CHECK(dcClip == gcClip);
    CHECK(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(gc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedGC")
    {
        InitialStateWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(gc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(gc, bmp);
    }

    SECTION("OneRegionWithTransformedGC")
    {
        OneRegionWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionWithRotatedGC")
    {
        OneRegionWithRotatedGC(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(gc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(gc, bmp);
    }

    SECTION("RegionsAndPushPopState")
    {
        RegionsAndPushPopState(gc, bmp, dc);
    }
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("ClippingBoxTestCaseGC::Direct2DRenderer", "[clip][gc][direct2d]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    dc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    dc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    dc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -dc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
    REQUIRE(rend);
    wxScopedPtr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);
    wxRect gcClip(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    CHECK(dcClip == gcClip);
    CHECK(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(gc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedGC")
    {
        InitialStateWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(gc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(gc, bmp);
    }

    SECTION("OneRegionWithTransformedGC")
    {
        OneRegionWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionWithRotatedGC")
    {
        OneRegionWithRotatedGC(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(gc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(gc, bmp);
    }

    SECTION("RegionsAndPushPopState")
    {
        RegionsAndPushPopState(gc, bmp, dc);
    }
}
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
TEST_CASE("ClippingBoxTestCaseGC::CairoRenderer", "[clip][gc][cairo]")
{
    wxPoint parentDcDevOrigin = GENERATE(wxPoint(0, 0), wxPoint(4, 12), wxPoint(-6, -8));
    wxPoint parentDcLogOrigin = GENERATE(wxPoint(0, 0), wxPoint(-6, -8), wxPoint(4, 12));

#ifdef __WXMSW__
    int depth = GENERATE(24, 32);
    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(s_bgColour, wxBRUSHSTYLE_SOLID));
    dc.Clear();
    dc.SetDeviceOrigin(parentDcDevOrigin.x, parentDcDevOrigin.y);
    dc.SetLogicalOrigin(parentDcLogOrigin.x, parentDcLogOrigin.y);
    wxRect dcClip;
    dc.GetClippingBox(dcClip);
    wxPoint dcOrigin = -dc.DeviceToLogical(0, 0);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
    REQUIRE(rend);
    wxScopedPtr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    double x, y, w, h;
    gc->GetClipBox(&x, &y, &w, &h);
    wxRect gcClip(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
    CHECK(dcClip == gcClip);
    CHECK(-dcOrigin == dcClip.GetTopLeft());

    SECTION("InitialState")
    {
        InitialState(gc, bmp, dcOrigin);
    }

    SECTION("InitialStateWithTransformedGC")
    {
        InitialStateWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegion")
    {
        OneRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneLargeRegion")
    {
        OneLargeRegion(gc, bmp, dcOrigin);
    }

    SECTION("OneOuterRegion")
    {
        OneOuterRegion(gc, bmp);
    }

    SECTION("OneRegionNegDim")
    {
        OneRegionNegDim(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndReset")
    {
        OneRegionAndReset(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionAndEmpty")
    {
        OneRegionAndEmpty(gc, bmp);
    }

    SECTION("OneRegionWithTransformedGC")
    {
        OneRegionWithTransformedGC(gc, bmp, dcOrigin);
    }

    SECTION("OneRegionWithRotatedGC")
    {
        OneRegionWithRotatedGC(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlapping")
    {
        TwoRegionsOverlapping(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsOverlappingNegDim")
    {
        TwoRegionsOverlappingNegDim(gc, bmp, dcOrigin);
    }

    SECTION("TwoRegionsNonOverlapping")
    {
        TwoRegionsNonOverlapping(gc, bmp);
    }

    SECTION("TwoRegionsNonOverlappingNegDim")
    {
        TwoRegionsNonOverlappingNegDim(gc, bmp);
    }

    SECTION("RegionsAndPushPopState")
    {
        RegionsAndPushPopState(gc, bmp, dc);
    }
}
#endif // wxUSE_GRAPHICS_CAIRO

#endif // wxUSE_GRAPHICS_CONTEXT
