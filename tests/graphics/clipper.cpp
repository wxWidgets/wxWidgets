///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/clipper.cpp
// Purpose:     wxDCClipper unit tests
// Author:      Artur Wieczorek
// Created:     2022-12-27
// Copyright:   (c) 2022 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include <memory>

#include "wx/bitmap.h"
#include "wx/dcclient.h"
#include "wx/dcgraph.h"
#include "wx/dcmemory.h"
#include "wx/dcsvg.h"
#include "wx/app.h"
#include "wx/window.h"

#include "testfile.h"
#include "waitforpaint.h"

static const wxSize s_dcSize(260, 300);

static inline wxRect DeviceToLogical(wxDC& dc, const wxRect& r)
{
    return wxRect(dc.DeviceToLogical(r.GetPosition()), dc.DeviceToLogicalRel(r.GetSize()));
}

static void NoTransform(wxDC& dc)
{
    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r(10, 20, 30, 40);
        wxDCClipper clipper(dc, r);

        wxRect clipBox;
        dc.GetClippingBox(clipBox);
        CHECK(r == clipBox);
    }
    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipBox == clipBox);
}

static void ExternalTransform(wxDC& dc, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

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

    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r(10, 20, 30, 40);
        wxDCClipper clipper(dc, r);

        wxRect clipBox;
        dc.GetClippingBox(clipBox);
        CHECK(r == clipBox);
    }
    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipBox == clipBox);
}

static void InternalTransform(wxDC& dc, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r(10, 20, 30, 40);
        wxDCClipper clipper(dc, r);

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

        wxRect clipExpected = DeviceToLogical(dc, r);

        wxRect clipBox;
        dc.GetClippingBox(clipBox);
        CHECK(clipExpected == clipBox);
    }
    wxRect initClipExpected = DeviceToLogical(dc, initClipBox);

    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipExpected == clipBox);
}

static void SpecificClipping(wxDC& dc)
{
    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r(10, 20, 30, 40);
        wxDCClipper clipper(dc, r);

        const wxRect r1(16, 25, 20, 30);
        dc.SetClippingRegion(r1);

        wxRect clipExpected = DeviceToLogical(dc, r1);

        wxRect clipBox;
        dc.GetClippingBox(clipBox);
        CHECK(clipExpected == clipBox);
    }
    wxRect initClipExpected = DeviceToLogical(dc, initClipBox);

    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipExpected == clipBox);
}

static void InternalTransformSpecificClipping(wxDC& dc, bool useTransformMatrix)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( useTransformMatrix && !dc.CanUseTransformMatrix() )
        return;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r(10, 20, 30, 40);
        wxDCClipper clipper(dc, r);

        const wxRect r1(16, 25, 20, 30);
        dc.SetClippingRegion(r1);

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

        wxRect clipExpected = DeviceToLogical(dc, r1);

        wxRect clipBox;
        dc.GetClippingBox(clipBox);
        CHECK(clipExpected == clipBox);
    }
    wxRect initClipExpected = DeviceToLogical(dc, initClipBox);

    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipExpected == clipBox);
}

static void NoTransformEmbeddedClip(wxDC& dc)
{
    wxRect initClipBox;
    dc.GetClippingBox(initClipBox);
    {
        const wxRect r1(10, 20, 30, 40);
        wxDCClipper clipper1(dc, r1);
        {
            const wxRect r2(15, 25, 20, 30);
            wxDCClipper clipper2(dc, r2);

            wxRect clipBox2;
            dc.GetClippingBox(clipBox2);
            CHECK(r2 == clipBox2);
        }

        wxRect clipBox1;
        dc.GetClippingBox(clipBox1);
        CHECK(r1 == clipBox1);
    }
    wxRect clipBox;
    dc.GetClippingBox(clipBox);
    CHECK(initClipBox == clipBox);
}

static void DCAttributes(wxDC& dc)
{
    // Check if wxDC atrributes left unchanged
    wxFont font = dc.GetFont().Bold().Smaller();
    wxPen pen(*wxYELLOW, 2);
    wxBrush brush = *wxBLUE_BRUSH;

    wxDCFontChanger fontChanger(dc, font);
    wxDCPenChanger penChanger(dc,pen);
    wxDCBrushChanger brushChanger(dc, brush);
    wxCoord chWidth = dc.GetCharWidth();
    wxCoord chHeight = dc.GetCharHeight();
    wxFontMetrics fm = dc.GetFontMetrics();
    {
        wxDCClipper clipper(dc, 10, 20, 30, 40);
    }
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

TEST_CASE("ClipperTestCase::wxDC", "[clipper][dc]")
{
    wxBitmap bmp(s_dcSize);
    wxMemoryDC dc(bmp);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("ExternalTransform 1")
    {
        ExternalTransform(dc, false);
    }

    SECTION("ExternalTransform 2")
    {
        ExternalTransform(dc, true);
    }

    SECTION("InternalTransform 1")
    {
        InternalTransform(dc, false);
    }

    SECTION("InternalTransform 2")
    {
        InternalTransform(dc, true);
    }

    SECTION("SpecificClipping")
    {
        SpecificClipping(dc);
    }

    SECTION("InternalTransformSpecificClipping 1")
    {
        InternalTransformSpecificClipping(dc, false);
    }

    SECTION("InternalTransformSpecificClipping 2")
    {
        InternalTransformSpecificClipping(dc, true);
    }

    SECTION("NoTransformEmbeddedClip")
    {
        NoTransformEmbeddedClip(dc);
    }

    SECTION("DCAttributes")
    {
        DCAttributes(dc);
    }
}

#if wxUSE_GRAPHICS_CONTEXT
TEST_CASE("ClipperTestCase::wxGCDC", "[clipper][dc][gcdc]")
{
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(*wxWHITE_BRUSH);
    mdc.Clear();
    wxGCDC dc(mdc);
    dc.GetGraphicsContext()->SetAntialiasMode(wxANTIALIAS_NONE);
    dc.GetGraphicsContext()->DisableOffset();

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("ExternalTransform 1")
    {
        ExternalTransform(dc, false);
    }

    SECTION("ExternalTransform 2")
    {
        ExternalTransform(dc, true);
    }

    SECTION("InternalTransform 1")
    {
        InternalTransform(dc, false);
    }

    SECTION("InternalTransform 2")
    {
        InternalTransform(dc, true);
    }

    SECTION("SpecificClipping")
    {
        SpecificClipping(dc);
    }

    SECTION("InternalTransformSpecificClipping 1")
    {
        InternalTransformSpecificClipping(dc, false);
    }

    SECTION("InternalTransformSpecificClipping 2")
    {
        InternalTransformSpecificClipping(dc, true);
    }

    SECTION("NoTransformEmbeddedClip")
    {
        NoTransformEmbeddedClip(dc);
    }

    SECTION("DCAttributes")
    {
        DCAttributes(dc);
    }
}

#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("ClipperTestCase::wxGCDC(GDI+)", "[clipper][dc][gcdc][gdiplus]")
{
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(*wxWHITE_BRUSH);
    mdc.Clear();
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    REQUIRE(rend);
    wxGraphicsContext* gc = rend->CreateContext(mdc);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    wxGCDC dc(gc);

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("ExternalTransform 1")
    {
        ExternalTransform(dc, false);
    }

    SECTION("ExternalTransform 2")
    {
        ExternalTransform(dc, true);
    }

    SECTION("InternalTransform 1")
    {
        InternalTransform(dc, false);
    }

    SECTION("InternalTransform 2")
    {
        InternalTransform(dc, true);
    }

    SECTION("SpecificClipping")
    {
        SpecificClipping(dc);
    }

    SECTION("InternalTransformSpecificClipping 1")
    {
        InternalTransformSpecificClipping(dc, false);
    }

    SECTION("InternalTransformSpecificClipping 2")
    {
        InternalTransformSpecificClipping(dc, true);
    }

    SECTION("NoTransformEmbeddedClip")
    {
        NoTransformEmbeddedClip(dc);
    }

    SECTION("DCAttributes")
    {
        DCAttributes(dc);
    }
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("ClipperTestCase::wxGCDC(Direct2D)", "[clipper][dc][gcdc][direct2d]")
{
    if ( wxIsRunningUnderWine() )
    {
        WARN("Skipping tests known to fail in Wine");
    }
    else
    {
        int depth = GENERATE(24, 32);

        wxBitmap bmp(s_dcSize, depth);
        wxMemoryDC mdc(bmp);
        mdc.SetBackground(*wxWHITE_BRUSH);
        mdc.Clear();
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
        REQUIRE(rend);
        wxGraphicsContext* gc = rend->CreateContext(mdc);
        gc->SetAntialiasMode(wxANTIALIAS_NONE);
        gc->DisableOffset();
        wxGCDC dc(gc);

        SECTION("NoTransform")
        {
            NoTransform(dc);
        }

        SECTION("ExternalTransform 1")
        {
            ExternalTransform(dc, false);
        }

        SECTION("ExternalTransform 2")
        {
            ExternalTransform(dc, true);
        }

        SECTION("InternalTransform 1")
        {
            InternalTransform(dc, false);
        }

        SECTION("InternalTransform 2")
        {
            InternalTransform(dc, true);
        }

        SECTION("SpecificClipping")
        {
            SpecificClipping(dc);
        }

        SECTION("InternalTransformSpecificClipping 1")
        {
            InternalTransformSpecificClipping(dc, false);
        }

        SECTION("InternalTransformSpecificClipping 2")
        {
            InternalTransformSpecificClipping(dc, true);
        }

        SECTION("NoTransformEmbeddedClip")
        {
            NoTransformEmbeddedClip(dc);
        }

        SECTION("DCAttributes")
        {
            DCAttributes(dc);
        }
    }
}
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
TEST_CASE("ClipperTestCase::wxGCDC(Cairo)", "[clipper][dc][gcdc][cairo]")
{
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC mdc(bmp);
    mdc.SetBackground(*wxWHITE_BRUSH);
    mdc.Clear();
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
    REQUIRE(rend);
    wxGraphicsContext* gc = rend->CreateContext(mdc);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->DisableOffset();
    wxGCDC dc(gc);

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("ExternalTransform 1")
    {
        ExternalTransform(dc, false);
    }

    SECTION("ExternalTransform 2")
    {
        ExternalTransform(dc, true);
    }

    SECTION("InternalTransform 1")
    {
        InternalTransform(dc, false);
    }

    SECTION("InternalTransform 2")
    {
        InternalTransform(dc, true);
    }

    SECTION("SpecificClipping")
    {
        SpecificClipping(dc);
    }

    SECTION("InternalTransformSpecificClipping 1")
    {
        InternalTransformSpecificClipping(dc, false);
    }

    SECTION("InternalTransformSpecificClipping 2")
    {
        InternalTransformSpecificClipping(dc, true);
    }

    SECTION("NoTransformEmbeddedClip")
    {
        NoTransformEmbeddedClip(dc);
    }

    SECTION("DCAttributes")
    {
        DCAttributes(dc);
    }
}
#endif // wxUSE_CAIRO

#endif // wxUSE_GRAPHICS_CONTEXT

#if wxUSE_SVG
TEST_CASE("ClipperTestCase::wxSVGFileDC", "[clipper][dc][svgdc]")
{
    TestFile tf;
    wxSVGFileDC dc(tf.GetName(), s_dcSize.x, s_dcSize.y);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("ExternalTransform 1")
    {
        ExternalTransform(dc, false);
    }

    SECTION("ExternalTransform 2")
    {
        ExternalTransform(dc, true);
    }

    SECTION("InternalTransform 1")
    {
        InternalTransform(dc, false);
    }

    SECTION("InternalTransform 2")
    {
        InternalTransform(dc, true);
    }

    SECTION("SpecificClipping")
    {
        SpecificClipping(dc);
    }

    SECTION("InternalTransformSpecificClipping 1")
    {
        InternalTransformSpecificClipping(dc, false);
    }

    SECTION("InternalTransformSpecificClipping 2")
    {
        InternalTransformSpecificClipping(dc, true);
    }

    SECTION("NoTransformEmbeddedClip")
    {
        NoTransformEmbeddedClip(dc);
    }

    SECTION("DCAttributes")
    {
        DCAttributes(dc);
    }
}
#endif // wxUSE_SVG

TEST_CASE("ClipperTestCase::wxPaintDC", "[clipper][dc][paintdc]")
{
#ifdef __WXOSX__
    WARN("Skipping tests known to fail in wxOSX");
#else
    // Ensure window is shown and large enough for testing
    wxTheApp->GetTopWindow()->Raise();
    REQUIRE(wxTheApp->GetTopWindow()->IsShown());
    wxSize winSize = wxTheApp->GetTopWindow()->GetSize();
    winSize.x = wxMax(winSize.x, s_dcSize.x + 50);
    winSize.y = wxMax(winSize.y, s_dcSize.y + 50);
    wxTheApp->GetTopWindow()->SetSize(winSize);
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    // Under wxGTK2 we need to have two children (at least) because if there
    // is exactly one child its size is set to fill the whole parent frame
    // and the window cannot be resized - see wxTopLevelWindowBase::Layout().
    std::unique_ptr<wxWindow> w0(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));
#endif // wxGTK 2
    std::unique_ptr<wxWindow> win(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY, wxPoint(0, 0)));
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
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        SECTION("NoTransform")
        {
            NoTransform(dc);
        }

        SECTION("ExternalTransform 1")
        {
            ExternalTransform(dc, false);
        }

        SECTION("ExternalTransform 2")
        {
            ExternalTransform(dc, true);
        }

        SECTION("InternalTransform 1")
        {
            InternalTransform(dc, false);
        }

        SECTION("InternalTransform 2")
        {
            InternalTransform(dc, true);
        }

        SECTION("SpecificClipping")
        {
            SpecificClipping(dc);
        }

        SECTION("InternalTransformSpecificClipping 1")
        {
            InternalTransformSpecificClipping(dc, false);
        }

        SECTION("InternalTransformSpecificClipping 2")
        {
            InternalTransformSpecificClipping(dc, true);
        }

        SECTION("NoTransformEmbeddedClip")
        {
            NoTransformEmbeddedClip(dc);
        }

        SECTION("DCAttributes")
        {
            DCAttributes(dc);
        }

        paintExecuted = true;
    });

    testWin->Refresh();
    testWin->Update();
    CHECK(paintExecuted == true);
#endif // !__WXOSX__
}
