///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/svgfiledc.cpp
// Purpose:     wxSVGFileDC unit tests
// Author:      Blake Madden
// Created:     2026-09-18
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SVG

#include "wx/dcsvg.h"
#include "wx/ffile.h"

#include "testfile.h"

namespace
{

wxSVGFileDC MakeDC(int width = 200, int height = 100)
{
    return wxSVGFileDC(wxString{}, width, height);
}

} // namespace

TEST_CASE("wxSVGFileDC::Ctor", "[svg][dcsvg]")
{
    wxSVGFileDC dc(wxString{}, 320, 240);

    CHECK( dc.IsOk() );

    wxSize size = dc.GetSize();
    CHECK( size.GetWidth() == 320 );
    CHECK( size.GetHeight() == 240 );

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.StartsWith("<?xml") );
    CHECK( svg.Contains("<svg ") );
    CHECK( svg.Contains("viewBox=\"0 0 320 240\"") );
    CHECK( svg.Contains("</svg>") );
}

TEST_CASE("wxSVGFileDC::Title", "[svg][dcsvg]")
{
    wxSVGFileDC dc(wxString{}, 100, 100, wxSVG_DEFAULT_DPI, "My Drawing");

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<title>My Drawing</title>") );
}

TEST_CASE("wxSVGFileDC::DrawLine", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();
    dc.SetPen(wxPen{ *wxRED, 2 });

    dc.DrawLine(10, 20, 110, 20);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<path d=\"M10 20 L110 20\"") );
    CHECK( svg.Contains("stroke=\"#FF0000\"") );
    CHECK( svg.Contains("stroke-width=\"2\"") );
}

TEST_CASE("wxSVGFileDC::DrawRectangle", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();
    dc.SetPen(wxPen{ *wxRED, 1 });
    dc.SetBrush(*wxBLUE_BRUSH);

    dc.DrawRectangle(10, 10, 50, 30);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<rect x=\"10\" y=\"10\" width=\"50\" height=\"30\"") );
    CHECK( svg.Contains("fill=\"#0000FF\"") );
    CHECK( svg.Contains("stroke=\"#FF0000\"") );
}

TEST_CASE("wxSVGFileDC::DrawEllipse", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();
    dc.SetBrush(*wxGREEN_BRUSH);

    dc.DrawEllipse(0, 0, 40, 20);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<ellipse cx=\"20.00\" cy=\"10.00\" rx=\"20.00\" ry=\"10.00\"") );
    CHECK( svg.Contains("fill=\"#00FF00\"") );
}

TEST_CASE("wxSVGFileDC::DrawPolygon", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();
    dc.SetBrush(*wxCYAN_BRUSH);

    const wxPoint points[3] = { wxPoint{0, 0}, wxPoint{10, 0}, wxPoint{5, 10} };
    dc.DrawPolygon(3, points);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<polygon points=\"0 0 10 0 5 10 \"") );
    CHECK( svg.Contains("fill=\"#00FFFF\"") );
    CHECK( svg.Contains("fill-rule=\"evenodd\"") );
}

TEST_CASE("wxSVGFileDC::DrawText", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();

    dc.DrawText("Hello SVG", 5, 5);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<text ") );
    CHECK( svg.Contains(">Hello SVG</text>") );
}

TEST_CASE("wxSVGFileDC::DrawRotatedText", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();

    dc.DrawRotatedText("Angled", 5, 5, 45.0);

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains(">Angled</text>") );
    CHECK( svg.Contains("rotate(-45.00") );
}

TEST_CASE("wxSVGFileDC::AccessibleGroup", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();

    wxSVGAttributes attr;
    attr.Role("img").AriaLabel("A red square");

    dc.BeginAccessibleGroup(attr, "Square", "A simple red square");
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawRectangle(0, 0, 10, 10);
    dc.EndAccessibleGroup();

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("role=\"img\"") );
    CHECK( svg.Contains("aria-label=\"A red square\"") );
    CHECK( svg.Contains("<title>Square</title>") );
    CHECK( svg.Contains("<desc>A simple red square</desc>") );
}

TEST_CASE("wxSVGFileDC::Layer", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();

    dc.BeginLayer(0.5);
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.DrawRectangle(0, 0, 10, 10);
    dc.EndLayer();

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<g opacity=\"0.50\">") );
    CHECK( svg.Contains("fill=\"#0000FF\"") );
}

TEST_CASE("wxSVGFileDC::Clipping", "[svg][dcsvg]")
{
    wxSVGFileDC dc = MakeDC();

    dc.SetClippingRegion(10, 10, 50, 50);
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawRectangle(0, 0, 100, 100);
    dc.DestroyClippingRegion();

    const wxString svg = dc.GetSVGDocument();
    CHECK( svg.Contains("<clipPath id=\"clip") );
    CHECK( svg.Contains("clip-path=\"url(#clip") );
    CHECK( svg.Contains("x=\"10\" y=\"10\" width=\"50\" height=\"50\"") );
}

TEST_CASE("wxSVGFileDC::Save", "[svg][dcsvg]")
{
    TestFile file;
    const wxString filename = file.GetName();

    {
        wxSVGFileDC dc{ filename, 50, 50 };
        dc.SetBrush(*wxYELLOW_BRUSH);
        dc.DrawRectangle(0, 0, 20, 20);

        REQUIRE( dc.Save() );
    }

    wxString content;
    REQUIRE(wxFFile{ filename, "r" }.ReadAll(&content));
    CHECK( content.Contains("<svg ") );
    CHECK( content.Contains("fill=\"#FFFF00\"") );
}

#endif // wxUSE_SVG
