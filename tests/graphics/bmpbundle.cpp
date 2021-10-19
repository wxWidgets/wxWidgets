///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/bmpbundle.cpp
// Purpose:     wxBitmapBundle unit test
// Author:      Vadim Zeitlin
// Created:     2021-09-27
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/bmpbndl.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("BitmapBundle::Create", "[bmpbundle]")
{
    wxBitmapBundle b;
    CHECK( !b.IsOk() );
    CHECK( b.GetDefaultSize() == wxDefaultSize );

    b = wxBitmap(16, 16);
    CHECK( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(16, 16) );
}

TEST_CASE("BitmapBundle::FromBitmaps", "[bmpbundle]")
{
    wxVector<wxBitmap> bitmaps;
    bitmaps.push_back(wxBitmap(16, 16));
    bitmaps.push_back(wxBitmap(24, 24));

    wxBitmapBundle b = wxBitmapBundle::FromBitmaps(bitmaps);
    REQUIRE( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(16, 16) );

    CHECK( b.GetBitmap(wxDefaultSize ).GetSize() == wxSize(16, 16) );
    CHECK( b.GetBitmap(wxSize(16, 16)).GetSize() == wxSize(16, 16) );
    CHECK( b.GetBitmap(wxSize(20, 20)).GetSize() == wxSize(20, 20) );
    CHECK( b.GetBitmap(wxSize(24, 24)).GetSize() == wxSize(24, 24) );
}

TEST_CASE("BitmapBundle::GetPreferredSize", "[bmpbundle]")
{
    CHECK( wxBitmapBundle().GetPreferredSizeAtScale(1) == wxDefaultSize );

    const wxSize normal(32, 32);
    const wxSize bigger(64, 64);

    const wxBitmapBundle
        b = wxBitmapBundle::FromBitmaps(wxBitmap(normal), wxBitmap(bigger));

    CHECK( b.GetPreferredSizeAtScale(0   ) == normal );
    CHECK( b.GetPreferredSizeAtScale(1   ) == normal );
    CHECK( b.GetPreferredSizeAtScale(1.25) == normal );
    CHECK( b.GetPreferredSizeAtScale(1.4 ) == normal );
    CHECK( b.GetPreferredSizeAtScale(1.5 ) == bigger );
    CHECK( b.GetPreferredSizeAtScale(1.75) == bigger );
    CHECK( b.GetPreferredSizeAtScale(2   ) == bigger );
    CHECK( b.GetPreferredSizeAtScale(2.5 ) == bigger );
    CHECK( b.GetPreferredSizeAtScale(3   ) == bigger );
}

#ifdef wxHAS_SVG

TEST_CASE("BitmapBundle::FromSVG", "[bmpbundle][svg]")
{
    static const char svg_data[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
"<svg width=\"200\" height=\"200\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\">"
"<g>"
"<circle cx=\"100\" cy=\"100\" r=\"50\" fill=\"blue\"/>"
"</g>"
"</svg>"
    ;

    wxBitmapBundle b = wxBitmapBundle::FromSVG(svg_data, wxSize(20, 20));
    REQUIRE( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(20, 20) );

    CHECK( b.GetBitmap(wxSize(32, 32)).GetSize() == wxSize(32, 32) );

    // Check that not using XML header works too.
    const char* svg_tag_start = strstr(svg_data, "<svg");
    REQUIRE( svg_tag_start );

    b = wxBitmapBundle::FromSVG(svg_data, wxSize(20, 20));
    REQUIRE( b.IsOk() );
    CHECK( b.GetBitmap(wxSize(16, 16)).GetSize() == wxSize(16, 16) );
}

#endif // wxHAS_SVG
