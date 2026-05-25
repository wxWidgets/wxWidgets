///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/drawing.cpp
// Purpose:     Tests for wxGraphicsContent general drawing
// Author:      Armel Asselin
// Created:     2014-02-21
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "drawing.h"
#include "wx/dcmemory.h"
#include "wx/ffile.h"
#include "wx/filename.h"

#if wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT
#include "wx/svggc.h"
#endif

//// ORGANIZATION /////////////////////////////////////////////////////////////

// This test suite is organized around Catch2 TEST_CASE and SECTIONs.
// The SVG tests verify the XML structure directly. This approach is:
// - Cross-platform: XML output is deterministic regardless of the OS.
// - Self-contained: No external reference files are required.
// - Robust: Uses string matching to verify drawing commands and attributes.

//// WRITING NEW TEST CASES

// - To add a new drawing test:
//   1. Add a SECTION to the relevant TEST_CASE (e.g., SVGGraphicsContext).
//   2. Create a wxSVGGraphicsContext instance.
//   3. Use wxGraphicsContext drawing primitives (or helpers in GraphicsContextDrawingTestCase).
//   4. Use CHECK(svg.Contains(...)) to verify the generated XML.

// ----------------------------------------------------------------------------
// GraphicsContextDrawingTestCase implementation
// ----------------------------------------------------------------------------

void GraphicsContextDrawingTestCase::DoFullDrawings (wxGraphicsContext *gc)
{
    // Background
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->DrawRectangle(0, 0, 800, 600);

    // Some shapes
    gc->SetPen(wxPen(*wxRED, 2));
    gc->SetBrush(*wxBLUE_BRUSH);
    gc->DrawRectangle(50, 50, 200, 100);

    gc->SetPen(wxPen(*wxGREEN, 5));
    gc->SetBrush(wxBrush(*wxYELLOW, wxBRUSHSTYLE_CROSSDIAG_HATCH));
    gc->DrawEllipse(300, 50, 200, 100);

    gc->SetBrush(*wxCYAN_BRUSH);
    gc->DrawRoundedRectangle(50, 250, 150, 80, 15);

    // Gradients
    wxGraphicsGradientStops stops;
    stops.Add(*wxRED, 0.0);
    stops.Add(*wxBLUE, 1.0);
    wxGraphicsBrush grad = gc->CreateLinearGradientBrush(300, 250, 500, 250, stops);
    gc->SetBrush(grad);
    gc->DrawRectangle(300, 250, 200, 100);

    // Text
    gc->SetFont(*wxNORMAL_FONT, *wxBLACK);
    gc->DrawText(wxASCII_STR("wxSVGGraphicsContext Full Test"), 50, 400);
}

// ----------------------------------------------------------------------------
// SVG Test Cases
// ----------------------------------------------------------------------------

#if wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

TEST_CASE("SVGGraphicsContext", "[svggc][graphics][svg]")
{
    const int width = 400;
    const int height = 300;

    SECTION("FileOutput")
    {
        const wxString filename = wxFileName::CreateTempFileName("wx_svggc_");
        REQUIRE(!filename.empty());

        struct FileGuard
        {
            wxString path;
            ~FileGuard() { wxRemoveFile(path); }
        } guard{ filename };

        {
            std::unique_ptr<wxSVGGraphicsContext> gcSvg(
                wxSVGGraphicsContext::Create(filename, width, height)
            );
            wxGraphicsContext* gc = gcSvg.get();
            REQUIRE(gc);

            // Background
            gc->SetBrush(*wxWHITE_BRUSH);
            gc->DrawRectangle(0, 0, width, height);

            // Shapes
            gc->SetPen(wxPen(*wxRED, 2));
            gc->SetBrush(*wxBLUE_BRUSH);
            gc->DrawRectangle(10, 10, 100, 50);

            gc->SetPen(wxPen(*wxGREEN, 1, wxPENSTYLE_DOT));
            gc->DrawEllipse(150, 10, 100, 50);

            gc->SetPen(wxPen(*wxBLACK, 3));
            gc->StrokeLine(10, 100, 390, 100);

            // Gradients
            wxGraphicsGradientStops stops;
            stops.Add(wxColour(255, 0, 0), 0.0);
            stops.Add(wxColour(0, 0, 255), 1.0);
            wxGraphicsBrush grad = gc->CreateLinearGradientBrush(10, 150, 110, 150, stops);
            gc->SetBrush(grad);
            gc->DrawRectangle(10, 150, 100, 50);

            // Text
            gc->SetFont(*wxNORMAL_FONT, *wxBLACK);
            gc->DrawText(wxASCII_STR("SVG Graphics Context"), 150, 170);

            // Transformations
            gc->PushState();
            gc->Translate(200, 250);
            gc->Rotate(M_PI / 4);
            gc->SetBrush(*wxRED_BRUSH);
            gc->DrawRectangle(-25, -25, 50, 50);
            gc->PopState();
        }

        // Check if file exists and contains basic SVG tags
        wxString content;
        REQUIRE(wxFFile(filename, "r").ReadAll(&content));

        CHECK(content.Contains("<svg"));
        CHECK(content.Contains("viewBox=\"0 0 400 300\""));
        CHECK(content.Contains("<path"));
        CHECK(content.Contains("fill=\"#0000FF\"")); // Blue brush
        CHECK(content.Contains("stroke=\"#FF0000\"")); // Red pen
        CHECK(content.Contains("<linearGradient"));
        CHECK(content.Contains("<text"));
        CHECK(content.Contains("SVG Graphics Context"));
        CHECK(content.Contains("transform=\"matrix"));
    }

    SECTION("StringOutput")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), width, height)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        gc->SetBrush(*wxCYAN_BRUSH);
        gc->DrawRoundedRectangle(10, 10, 100, 100, 10);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<path"));
        CHECK(svg.Contains("fill=\"#00FFFF\""));
    }

    SECTION("FullDrawing")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 800, 600)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        // Exercise the "Full" drawing logic
        GraphicsContextDrawingTestCase::DoFullDrawings(gc);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("viewBox=\"0 0 800 600\""));
        CHECK(svg.Contains("fill=\"#0000FF\"")); // Blue
        CHECK(svg.Contains("stroke=\"#FF0000\"")); // Red
        CHECK(svg.Contains("stroke=\"#00FF00\"")); // Green
        CHECK(svg.Contains("fill=\"#00FFFF\"")); // Cyan
        CHECK(svg.Contains("wxSVGGraphicsContext Full Test"));
    }

    SECTION("BasicDrawing")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 800, 600)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        GraphicsContextDrawingTestCase::DoBasicDrawings(gc);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("fill=\"#FFFFFF\""));
    }

    SECTION("FontDrawing")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 800, 600)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        GraphicsContextDrawingTestCase::DoFontDrawings(gc);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<text"));
        CHECK(svg.Contains("This is text"));
        CHECK(svg.Contains("Another visible text"));
        // Multiline text is emitted as one <text> element per line.
        CHECK(svg.Contains(">And<"));
        CHECK(svg.Contains(">more<"));
        // Rotated multiline text emits a rotate() transform per line.
        CHECK(svg.Contains(">Rotated text<"));
        CHECK(svg.Contains("rotate("));
    }

    SECTION("Clipping")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        gc->Clip(10, 10, 100, 100);
        gc->SetBrush(*wxRED_BRUSH);
        gc->DrawRectangle(0, 0, 400, 300);
        gc->ResetClip();

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<clipPath"));
        CHECK(svg.Contains("clip-path=\"url(#"));
        CHECK(svg.Contains("fill=\"#FF0000\""));
    }

    SECTION("Layers")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        gc->SetBrush(*wxBLUE_BRUSH);
        gc->BeginLayer(0.5);
        gc->DrawRectangle(10, 10, 100, 100);
        gc->EndLayer();

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<g opacity=\"0.50\">"));
        CHECK(svg.Contains("fill=\"#0000FF\""));
    }

    SECTION("Bitmaps")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        // Explicitly set the embed handler to ensure data URLs are generated
        gcSvg->SetBitmapHandler(new wxSVGBitmapEmbedHandler());

        wxBitmap bmp(32, 32);
        {
            wxMemoryDC mdc(bmp);
            mdc.SetBackground(*wxRED_BRUSH);
            mdc.Clear();
        }

        gc->DrawBitmap(bmp, 10, 10, 32, 32);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<image"));
        CHECK(svg.Contains("xlink:href=\"data:image/png;base64,"));
    }

    SECTION("RadialGradients")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        wxGraphicsGradientStops stops;
        stops.Add(*wxWHITE, 0.0);
        stops.Add(*wxBLACK, 1.0);
        wxGraphicsBrush grad = gc->CreateRadialGradientBrush(200, 150, 200, 150, 100, stops);
        gc->SetBrush(grad);
        gc->DrawRectangle(100, 50, 200, 200);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<radialGradient"));
        CHECK(svg.Contains("cx=\"200.00\""));
        CHECK(svg.Contains("cy=\"150.00\""));
        CHECK(svg.Contains("r=\"100.00\""));
    }

    SECTION("ComplexPaths")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(10, 10);
        path.AddLineToPoint(100, 10);
        path.AddQuadCurveToPoint(150, 50, 100, 100);
        path.AddCurveToPoint(50, 150, 0, 50, 10, 10);
        path.CloseSubpath();

        gc->SetPen(*wxBLACK_PEN);
        gc->StrokePath(path);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("<path d=\"M 10.00 10.00 L 100.00 10.00 Q 150.00 50.00 100.00 100.00 C 50.00 150.00 0.00 50.00 10.00 10.00 Z \""));
    }

    SECTION("Antialiasing")
    {
        std::unique_ptr<wxSVGGraphicsContext> gcSvg(
            wxSVGGraphicsContext::Create(wxString(), 400, 300)
        );
        wxGraphicsContext* gc = gcSvg.get();
        REQUIRE(gc);

        gc->SetAntialiasMode(wxANTIALIAS_NONE);
        gc->SetBrush(*wxRED_BRUSH);
        gc->DrawRectangle(10, 10, 100, 100);

        wxString svg = gcSvg->GetSVGDocument();
        CHECK(svg.Contains("shape-rendering=\"crispEdges\""));
    }

    SECTION("wxSVGFileDC")
    {
        // Verify that the underlying DC also works correctly
        wxSVGFileDC dc(wxString(), 400, 300);
        dc.SetBrush(*wxRED_BRUSH);
        dc.DrawRectangle(10, 10, 100, 100);
        dc.SetPen(wxPen(*wxBLUE, 2));
        dc.DrawLine(0, 0, 400, 300);

        wxString svg = dc.GetSVGDocument();
        CHECK(svg.Contains("<rect"));
        CHECK(svg.Contains("fill=\"#FF0000\""));
        CHECK(svg.Contains("<path"));
        CHECK(svg.Contains("stroke=\"#0000FF\""));
    }
}

#endif // wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT
