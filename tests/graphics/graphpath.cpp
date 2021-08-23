///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/grappath.cpp
// Purpose:     graphics path unit tests
// Author:      Artur Wieczorek
// Created:     2018-07-01
// Copyright:   (c) 2018 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


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
    if ( wxIsRunningUnderWine() )
        return;

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
        path.AddPath(path2);
        wxPoint2DDouble cp = path.GetCurrentPoint();
        WX_CHECK_POINT(cp, cp2, 1E-3);
    }
}

#define WX_CHECK_BOX(r1, r2, tolerance)                           \
    WX_CHECK_POINT(r1.GetLeftTop(), r2.GetLeftTop(), tolerance); \
    WX_CHECK_POINT(r1.GetRightBottom(), r2.GetRightBottom(), tolerance)

static void TestBox(wxGraphicsContext* gc)
{
    // No current point
    {
        wxGraphicsPath path = gc->CreatePath();
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(0, 0, 0, 0), 0);
    }
    // MoveToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(28, 38);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(0, 0, 0, 0), 0);
    }
    // AddLineToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        path.AddLineToPoint(28, 36);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(0, 0, 0, 0), 0);
    }
    // AddLineToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 10;
        const wxDouble y0 = 18;
        path.MoveToPoint(x0, y0);
        const wxDouble w = 20;
        const wxDouble h = 46;
        path.AddLineToPoint(x0 + w, y0 + h);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x0, y0, w, h), 0);
    }
    // AddArc - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble r = 40;
        path.AddArc(x, y, r, 0, M_PI / 2, true);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, r, r), 1E-3);
    }
    // AddArc
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 20;
        const wxDouble y0 = 20;
        path.MoveToPoint(x0, y0);
        const wxDouble x = 200;
        const wxDouble y = 50;
        const wxDouble r = 40;
        path.AddArc(x, y, r, 0, M_PI / 2, true);
        const wxDouble x2 = x + r;
        const wxDouble y2 = y + r;
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x0, y0, x2 - x0, y2 - y0), 1E-3);
    }
    // AddArcToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x1 = 80;
        const wxDouble y1 = 0;
        const wxDouble x2 = x1;
        const wxDouble y2 = 40;
        const wxDouble r = 20;
        wxASSERT(y1 == 0 && x2 == x1); // alpha = 90 deg
        path.AddArcToPoint(x1, y1, x2, y2, r);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(0, 0, x1, r), 1E-3);
    }
    // AddArcToPoint
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 20;
        const wxDouble y0 = 20;
        path.MoveToPoint(x0, y0);
        const wxDouble x1 = 80;
        const wxDouble y1 = y0;
        const wxDouble x2 = x1;
        const wxDouble y2 = 140;
        const wxDouble r = 20;
        wxASSERT(y1 == y0 && x2 == x1); // alpha = 90 deg
        path.AddArcToPoint(x1, y1, x2, y2, r);
        const wxDouble xe = x1;
        const wxDouble ye = y1 + r;
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x0, y0, xe - x0, ye - y0), 1E-3);
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
        const wxDouble xmin = wxMin(wxMin(x1, x2), x3);
        const wxDouble ymin = wxMin(wxMin(y1, y2), y3);
        const wxDouble xmax = wxMax(wxMax(x1, x2), x3);
        const wxDouble ymax = wxMax(wxMax(y1, y2), y3);
        wxRect2DDouble b = path.GetBox();
        const wxDouble tolerance = 1E-3;
        CHECK(xmin - tolerance <= b.GetLeft());
        CHECK(ymin - tolerance <= b.GetTop());
        CHECK(xmax + tolerance >= b.GetRight());
        CHECK(ymax + tolerance >= b.GetBottom());
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
        const wxDouble xmin = wxMin(wxMin(wxMin(x0, x1), x2), x3);
        const wxDouble ymin = wxMin(wxMin(wxMin(y0, y1), y2), y3);
        const wxDouble xmax = wxMax(wxMax(wxMax(x0, x1), x2), x3);
        const wxDouble ymax = wxMax(wxMax(wxMax(y0, y1), y2), y3);
        wxRect2DDouble b = path.GetBox();
        const wxDouble tolerance = 1E-3;
        CHECK(xmin - tolerance <= b.GetLeft());
        CHECK(ymin - tolerance <= b.GetTop());
        CHECK(xmax + tolerance >= b.GetRight());
        CHECK(ymax + tolerance >= b.GetBottom());
    }
    // AddQuadCurveToPoint - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x1 = 200;
        const wxDouble y1 = 200;
        const wxDouble x2 = 300;
        const wxDouble y2 = 100;
        path.AddQuadCurveToPoint(x1, y1, x2, y2);
        //        const wxDouble xmin = wxMin(x1, x2);
        const wxDouble xmin = 133;
        const wxDouble ymin = wxMin(y1, y2);
        const wxDouble xmax = wxMax(x1, x2);
        const wxDouble ymax = wxMax(y1, y2);
        wxRect2DDouble b = path.GetBox();
        const wxDouble tolerance = 1E-3;
        CHECK(xmin - tolerance <= b.GetLeft());
        CHECK(ymin - tolerance <= b.GetTop());
        CHECK(xmax + tolerance >= b.GetRight());
        CHECK(ymax + tolerance >= b.GetBottom());
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
        const wxDouble xmin = wxMin(wxMin(x0, x1), x2);
        const wxDouble ymin = wxMin(wxMin(y0, y1), y2);
        const wxDouble xmax = wxMax(wxMax(x0, x1), x2);
        const wxDouble ymax = wxMax(wxMax(y0, y1), y2);
        wxRect2DDouble b = path.GetBox();
        const wxDouble tolerance = 1E-3;
        CHECK(xmin - tolerance <= b.GetLeft());
        CHECK(ymin - tolerance <= b.GetTop());
        CHECK(xmax + tolerance >= b.GetRight());
        CHECK(ymax + tolerance >= b.GetBottom());
    }
    // AddCircle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble r = 30;
        path.AddCircle(x, y, r);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x - r, y - r, 2 * r, 2 * r), 1E-3);
    }
    // AddCircle
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        const wxDouble r = 40;
        path.AddCircle(x, y, r);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x - r, y - r, 2 * r, 2 * r), 1E-3);
    }
    // AddEllipse - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddEllipse(x, y, w, h);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
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
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // AddRectangle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRectangle(x, y, w, h);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // AddRectangle
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 50;
        const wxDouble h = 30;
        path.AddRectangle(x, y, w, h);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // AddRoundedRectangle - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 5);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // AddRoundedRectangle - no current point, radius = 0
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 0);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
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
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // AddRoundedRectangle - radius = 0
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 40;
        const wxDouble h = 20;
        path.AddRoundedRectangle(x, y, w, h, 0);
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
    // CloseSubpath - empty path
    {
        wxGraphicsPath path = gc->CreatePath();
        path.CloseSubpath();
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(0, 0, 0, 0), 0);
    }
    // CloseSubpath - no current point
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 50;
        const wxDouble y0 = 80;
        path.AddLineToPoint(x0, y0);
        const wxDouble x1 = 100;
        const wxDouble y1 = 160;
        path.AddLineToPoint(x1, y1);
        path.CloseSubpath();
        const wxDouble w = x1 - x0;
        const wxDouble h = y1 - y0;
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x0, y0, w, h), 1E-3);
    }
    // CloseSubpath
    {
        wxGraphicsPath path = gc->CreatePath();
        const wxDouble x0 = 10;
        const wxDouble y0 = 20;
        path.MoveToPoint(x0, y0);
        path.AddLineToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 160;
        path.AddLineToPoint(x, y);
        path.CloseSubpath();
        const wxDouble w = x - x0;
        const wxDouble h = y - y0;
        wxRect2DDouble b = path.GetBox();
        WX_CHECK_BOX(b, wxRect2DDouble(x0, y0, w, h), 1E-3);
    }
    // AddPath - no current point
    {
        // Path to be added
        wxGraphicsPath path2 = gc->CreatePath();
        path2.AddLineToPoint(100, 160);
        path2.AddLineToPoint(50, 80);
        path2.CloseSubpath();
        wxRect2DDouble b2 = path2.GetBox();
        // Main path
        wxGraphicsPath path = gc->CreatePath();
        path.AddLineToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        path.AddRectangle(x, y, 50, 200);
        wxRect2DDouble b0 = path.GetBox();
        b0.Union(b2);
        path.AddPath(path2);
        wxRect2DDouble b1 = path.GetBox();
        WX_CHECK_BOX(b0, b1, 1E-3);
    }
    // AddPath
    {
        // Path to be added
        wxGraphicsPath path2 = gc->CreatePath();
        path2.AddArcToPoint(100, 160, 50, 200, 30);
        path2.AddLineToPoint(50, 80);
        path2.CloseSubpath();
        wxRect2DDouble b2 = path2.GetBox();
        // Main path
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(15, 35);
        path.AddLineToPoint(50, 80);
        const wxDouble x = 100;
        const wxDouble y = 140;
        const wxDouble r = 20;
        path.AddCircle(x, y, r);
        wxRect2DDouble b0 = path.GetBox();
        b0.Union(b2);
        path.AddPath(path2);
        wxRect2DDouble b1 = path.GetBox();
        WX_CHECK_BOX(b0, b1, 1E-3);
    }
    // Overlapping figures
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble xr = 100;
        const wxDouble yr = 150;
        const wxDouble wr = 80;
        const wxDouble hr = 40;
        path.AddRectangle(xr, yr, wr, hr);
        const wxDouble xe = xr + wr / 4;
        const wxDouble ye = yr + hr / 4;
        const wxDouble we = wr / 2;
        const wxDouble he = hr / 2;
        path.AddEllipse(xe, ye, we, he);
        wxRect2DDouble b = path.GetBox();
        wxRect2DDouble r;
        wxRect2DDouble::Union(wxRect2DDouble(xe, ye, we, he), wxRect2DDouble(xr, yr, wr, hr), &r);
        WX_CHECK_BOX(b, r, 1E-3);
    }
    // Partially overlapping figures
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble xe = 100;
        const wxDouble ye = 150;
        const wxDouble we = 40;
        const wxDouble he = 20;
        path.AddEllipse(xe, ye, we, he);
        const wxDouble xr = xe + he / 2;
        const wxDouble yr = ye + we / 2;
        const wxDouble wr = we + 10;
        const wxDouble hr = he + 10;
        path.AddRectangle(xr, yr, wr, hr);
        wxRect2DDouble b = path.GetBox();
        wxRect2DDouble r;
        wxRect2DDouble::Union(wxRect2DDouble(xe, ye, we, he), wxRect2DDouble(xr, yr, wr, hr), &r);
        WX_CHECK_BOX(b, r, 1E-3);
    }
    // Non-overlapping figures
    {
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble xe = 100;
        const wxDouble ye = 150;
        const wxDouble we = 40;
        const wxDouble he = 20;
        path.AddEllipse(xe, ye, we, he);
        const wxDouble xr = xe + he + 10;
        const wxDouble yr = ye + we + 20;
        const wxDouble wr = 50;
        const wxDouble hr = 30;
        path.AddRectangle(xr, yr, wr, hr);
        wxRect2DDouble b = path.GetBox();
        wxRect2DDouble r;
        wxRect2DDouble::Union(wxRect2DDouble(xe, ye, we, he), wxRect2DDouble(xr, yr, wr, hr), &r);
        WX_CHECK_BOX(b, r, 1E-3);
    }
    // Path from transformed graphics context
    {
        gc->PushState();
        gc->Translate(5, 15);
        gc->Rotate(10 * M_PI / 180);
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(50, 60);
        const wxDouble x = 100;
        const wxDouble y = 150;
        const wxDouble w = 50;
        const wxDouble h = 30;
        path.AddRectangle(x, y, w, h);
        wxRect2DDouble b = path.GetBox();
        gc->PopState();
        WX_CHECK_BOX(b, wxRect2DDouble(x, y, w, h), 1E-3);
    }
}

static void DoAllTests(wxGraphicsContext* gc)
{
    gc->DisableOffset();
    TestCurrentPoint(gc);
    TestBox(gc);
}

#endif //  wxUSE_GRAPHICS_CONTEXT
