///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/grsppsth.cpp
// Purpose:     graphics path unit tests
// Author:      Artur Wieczorek
// Created:     2018-07-01
// Copyright:   (c) 2018 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/dcgraph.h"
#include "wx/scopedptr.h"

static void DoAllTests(wxGraphicsContext* gc);

// For MSW we have individual test cases for each graphics renderer
// so we don't need to execute tests with default renderer.
#ifndef __WXMSW__

TEST_CASE("GraphicsPathTestCase", "[path]")
{
    wxBitmap bmp(500, 500);
    wxMemoryDC mdc(bmp);
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsRenderer::GetDefaultRenderer()->CreateContext(mdc));
    REQUIRE(gc);
    DoAllTests(gc.get());
}

#else

#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("GraphicsPathTestCaseGDIPlus", "[path][gdi+]")
{
    wxBitmap bmp(500, 500);
    wxMemoryDC mdc(bmp);
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsRenderer::GetGDIPlusRenderer()->CreateContext(mdc));
    REQUIRE(gc);
    DoAllTests(gc.get());
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("GraphicsPathTestCaseDirect2D", "[path][d2d]")
{
    wxBitmap bmp(500, 500);
    wxMemoryDC mdc(bmp);
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsRenderer::GetDirect2DRenderer()->CreateContext(mdc));
    REQUIRE(gc);
    DoAllTests(gc.get());
}
#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__ / !__WXMSW__

#if wxUSE_CAIRO
TEST_CASE("GraphicsPathTestCaseCairo", "[path][cairo]")
{
    wxBitmap bmp(500, 500);
    wxMemoryDC mdc(bmp);
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsRenderer::GetCairoRenderer()->CreateContext(mdc));
    REQUIRE(gc);
    DoAllTests(gc.get());
}
#endif // wxUSE_CAIRO

#define WX_CHECK_POINT(p1, p2, tolerance)      \
    CHECK(fabs(p1.m_x - p2.m_x) <= tolerance); \
    CHECK(fabs(p1.m_y - p2.m_y) <= tolerance)

static void TestCurrentPoint(wxGraphicsContext* gc)
{
    // No current point
    {
        wxGraphicsPath path = gc->CreatePath();
        // Should return (0, 0) if current point is not yet set.
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(0, 0), 0);
    }
    // MoveToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        wxPoint2DDouble pt(27, 35);
        path.MoveToPoint(pt);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, pt, 1E-3);
    }
    // AddLineToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        wxPoint2DDouble pt(27, 35);
        path.AddLineToPoint(pt);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, pt, 1E-3);
    }
    // AddLineToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(10, 18);
        wxPoint2DDouble pt(37, 45);
        path.AddLineToPoint(pt);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, pt, 1E-3);
    }
    // AddArc - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble r = 40;
        path.AddArc(x, y, r, 0, M_PI/2, true);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y + r), 1E-3);
    }
    // AddArc
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(20, 38);
        const wxDouble x = 200;
        const wxDouble y = 50;
        const wxDouble r = 40;
        path.AddArc(x, y, r, 0, M_PI / 2, true);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y + r), 1E-3);
    }
    // AddArcToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x1 = 80;
        const wxDouble y1 = 80;
        const wxDouble x2 = -30;
        const wxDouble y2 = y1;
        const wxDouble r = 20;
        wxASSERT(x1 == y1 && y2 == y1); // alpha = 45 deg
        double d = r / tan(45 / 180.0 * M_PI / 2.0);
        path.AddArcToPoint(x1, y1, x2, y2, r);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x1 - d, y2), 1E-3);
    }
    // AddArcToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 20;
        const wxDouble y0 = 20;
        path.MoveToPoint(x0, y0);
        const wxDouble x1 = 80;
        const wxDouble y1 = 80;
        const wxDouble x2 = 140;
        const wxDouble y2 = y1;
        const wxDouble r = 20;
        wxASSERT(x0 == y0 && x1 == y1 && y2 == y1); // alpha = 135 deg
        double d = r / tan(135 / 180.0 * M_PI / 2.0);
        path.AddArcToPoint(x1, y1, x2, y2, r);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x1 + d, y2), 1E-3);
    }
    // AddCurveToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x1 = 102;
        const wxDouble y1 = 230;
        const wxDouble x2 = 153;
        const wxDouble y2 = 25;
        const wxDouble x3 = 230;
        const wxDouble y3 = 128;
        path.AddCurveToPoint(x1, y1, x2, y2, x3, y3);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x3, y3), 1E-3);
    }
    // AddCurveToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 25;
        const wxDouble y0 = 128;
        path.MoveToPoint(x0, y0);
        const wxDouble x1 = 102;
        const wxDouble y1 = 230;
        const wxDouble x2 = 153;
        const wxDouble y2 = 25;
        const wxDouble x3 = 230;
        const wxDouble y3 = 128;
        path.AddCurveToPoint(x1, y1, x2, y2, x3, y3);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x3, y3), 1E-3);
    }
    // AddQuadCurveToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x1 = 200;
        const wxDouble y1 = 200;
        const wxDouble x2 = 300;
        const wxDouble y2 = 100;
        path.AddQuadCurveToPoint(x1, y1, x2, y2);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x2, y2), 1E-3);
    }
    // AddQuadCurveToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 20;
        const wxDouble y0 = 100;
        path.MoveToPoint(x0, y0);
        const wxDouble x1 = 200;
        const wxDouble y1 = 200;
        const wxDouble x2 = 300;
        const wxDouble y2 = 100;
        path.AddQuadCurveToPoint(x1, y1, x2, y2);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x2, y2), 1E-3);
    }
    // AddCircle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble r = 30;
        path.AddCircle(x, y, r);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + r, y), 1E-3);
    }
    // AddCircle
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        const wxDouble r = 40;
        path.AddCircle(x, y, r);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + r, y), 1E-3);
    }
    // AddEllipse - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddEllipse(x, y, w, h);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + w, y + h / 2), 1E-3);
    }
    // AddEllipse
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddEllipse(x, y, w, h);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + w, y + h / 2), 1E-3);
    }
    // AddRectangle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        path.AddRectangle(x, y, 40, 20);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y), 1E-3);
    }
    // AddRectangle
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        path.AddRectangle(x, y, 50, 30);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y), 1E-3);
    }
    // AddRoundedRectangle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 5);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + w, y + h / 2), 1E-3);
    }
    // AddRoundedRectangle - no current point, radius = 0
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        path.AddRoundedRectangle(x, y, 40, 20, 0); // Should behave like AddRectangle
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y), 1E-3);
    }
    // AddRoundedRectangle
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 5);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x + w, y + h / 2), 1E-3);
    }
    // AddRoundedRectangle - radius = 0
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 50;
        const wxDouble y0 = 60;
        path.MoveToPoint(x0, y0);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 0); // Should behave like AddRectangle
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x, y), 1E-3);
    }
    // CloseSubpath - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 50;
        const wxDouble y0 = 80;
        path.AddLineToPoint(x0, y0);
        path.AddArcToPoint(100, 160, 50, 200, 30);
        path.CloseSubpath();
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x0, y0), 1E-3);
    }
    // CloseSubpath
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 10;
        const wxDouble y0 = 20;
        path.MoveToPoint(x0, y0);
        path.AddLineToPoint(50, 80);
        path.AddArcToPoint(100, 160, 50, 200, 30);
        path.CloseSubpath();
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, wxPoint2DDouble(x0, y0), 1E-3);
    }
    // AddPath - no current point
    {
        // Path to be added
        wxGraphicsPath path2 = gc->CreatePath();
        path2.AddArcToPoint(100, 160, 50, 200, 30);
        path2.AddLineToPoint(50, 80);
        path2.CloseSubpath();
        wxPoint2DDouble cp2 = path2.GetCurrentPoint();
        // Main path
        wxGraphicsPath path = gc->CreatePath();
        path.AddLineToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        path.AddRectangle(x, y, 50, 200);
        path.AddPath(path2);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, cp2, 1E-3);
    }
    // AddPath
    {
        // Path to be added
        wxGraphicsPath path2 = gc->CreatePath();
        path2.AddArcToPoint(100, 160, 50, 200, 30);
        path2.AddLineToPoint(50, 80);
        path2.CloseSubpath();
        wxPoint2DDouble cp2 = path2.GetCurrentPoint();
        // Main path
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(15, 35);
        path.AddLineToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        const wxDouble r = 20;
        path.AddCircle(x, y, r);
        wxPoint2DDouble cp0 = path.GetCurrentPoint();
        path.AddPath(path2);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, cp2, 1E-3);
    }
}

static void DoAllTests(wxGraphicsContext* gc)
{
    gc->DisableOffset();
    TestCurrentPoint(gc);
}

#endif //  wxUSE_GRAPHICS_CONTEXT
