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

#include "wx/artprov.h"

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
    CHECK( wxBitmapBundle().GetPreferredBitmapSizeAtScale(1) == wxDefaultSize );

    const wxSize normal(32, 32);
    const wxSize bigger(64, 64);

    const wxBitmapBundle
        b = wxBitmapBundle::FromBitmaps(wxBitmap(normal), wxBitmap(bigger));

    // Check that the existing bitmaps are used without scaling for most of the
    // typical scaling values.
    CHECK( b.GetPreferredBitmapSizeAtScale(0   ) == normal );
    CHECK( b.GetPreferredBitmapSizeAtScale(1   ) == normal );
    CHECK( b.GetPreferredBitmapSizeAtScale(1.25) == normal );
    CHECK( b.GetPreferredBitmapSizeAtScale(1.4 ) == normal );
    CHECK( b.GetPreferredBitmapSizeAtScale(1.5 ) == bigger );
    CHECK( b.GetPreferredBitmapSizeAtScale(1.75) == bigger );
    CHECK( b.GetPreferredBitmapSizeAtScale(2   ) == bigger );
    CHECK( b.GetPreferredBitmapSizeAtScale(2.5 ) == bigger );

    // This scale is too big to use any of the existing bitmaps, so they will
    // be scaled.
    CHECK( b.GetPreferredBitmapSizeAtScale(3   ) == 3*normal );
}

#ifdef wxHAS_DPI_INDEPENDENT_PIXELS

TEST_CASE("BitmapBundle::Scaled", "[bmpbundle]")
{
    // Adding a bitmap with scale factor > 1 should create the bundle using the
    // scaled size as default size.
    wxBitmap scaled2x(64, 64);
    scaled2x.SetScaleFactor(2);
    CHECK( scaled2x.GetLogicalSize() == wxSize(32, 32) );

    wxBitmapBundle b(scaled2x);
    CHECK( b.GetDefaultSize() == wxSize(32, 32) );

    // Retrieving this bitmap back from the bundle should preserve its scale.
    scaled2x = b.GetBitmap(wxSize(64, 64));
    CHECK( scaled2x.GetSize() == wxSize(64, 64) );
    CHECK( scaled2x.GetScaleFactor() == 2 );

    // And retrieving the bitmap from the bundle should set scale factor for it
    // even if it hadn't originally been added with it.
    b = wxBitmapBundle::FromBitmaps(wxBitmap(32, 32), wxBitmap(64, 64));
    scaled2x = b.GetBitmap(wxSize(64, 64));
    CHECK( scaled2x.GetSize() == wxSize(64, 64) );
    CHECK( scaled2x.GetScaleFactor() == 2 );

    // Using scaled bitmaps when there is more than one of them is a bad idea
    // in general, as only physical size matters, but the default size should
    // still be the scaled size of the smallest one.
    b = wxBitmapBundle::FromBitmaps(scaled2x, wxBitmap(64, 64));
    CHECK( b.GetDefaultSize() == wxSize(32, 32) );
}

#endif // wxHAS_DPI_INDEPENDENT_PIXELS

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

TEST_CASE("BitmapBundle::FromSVGFile", "[bmpbundle][svg][file]")
{
    const wxSize size(20, 20); // completely arbitrary

    CHECK( !wxBitmapBundle::FromSVGFile("horse.bmp", size).IsOk() );

    wxBitmapBundle b = wxBitmapBundle::FromSVGFile("horse.svg", size);
    REQUIRE( b.IsOk() );
    CHECK( b.GetDefaultSize() == size );
}

#endif // wxHAS_SVG

TEST_CASE("BitmapBundle::ArtProvider", "[bmpbundle][art]")
{
    // Check that creating a bogus bundle fails as expected.
    wxBitmapBundle b = wxArtProvider::GetBitmapBundle("bloordyblop");
    CHECK( !b.IsOk() );

    // And that creating a bundle using a standard icon works.
    const wxSize size(16, 16);
    b = wxArtProvider::GetBitmapBundle(wxART_INFORMATION, wxART_MENU, size);
    CHECK( b.IsOk() );
    CHECK( b.GetDefaultSize() == size );

#if wxUSE_ARTPROVIDER_TANGO
    // Tango art provider is supposed to use 16px for the default size of the
    // menu and button images and 24px for all the other ones, but we need to
    // choose the client kind for which the current platform doesn't define its
    // own default/fallback size to be able to test for it, i.e. this is the
    // client for which GetNativeSizeHint() of the native art provider returns
    // wxDefaultSize.
    const wxArtClient artClient =
#ifdef __WXMSW__
        wxART_TOOLBAR
#else
        wxART_LIST
#endif
        ;

    // We also need to use an image provided by Tango but not by the native art
    // provider, but here we can at least avoid the platform checks by using an
    // image not provided by any native providers.
    b = wxArtProvider::GetBitmapBundle(wxART_REFRESH, artClient);

    CHECK( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(24, 24) );
#endif // wxUSE_ARTPROVIDER_TANGO
}

// This test only makes sense for the ports that actually support scaled
// bitmaps, which is the case for the ports using real logical pixels (they
// have to support bitmap scale for things to work) and MSW, which doesn't, but
// still implements support for at least storing and retrieving bitmap scale in
// its wxBitmap.
#if defined(wxHAS_DPI_INDEPENDENT_PIXELS) || defined(__WXMSW__)

TEST_CASE("BitmapBundle::Scale", "[bmpbundle][scale]")
{
    // This is not a wxBitmapBundle test, strictly speaking, but check that
    // setting scale factor works correctly for bitmaps, as wxBitmapBundle does
    // this internally.
    wxBitmap bmp;
    bmp.CreateWithDIPSize(8, 8, 2);
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    CHECK( bmp.GetLogicalSize() == wxSize(8, 8) );
#endif
    CHECK( bmp.GetDIPSize() == wxSize(8, 8) );
    CHECK( bmp.GetSize() == wxSize(16, 16) );
    CHECK( bmp.GetScaleFactor() == 2 );

    wxBitmap bmp2(bmp);
    bmp.SetScaleFactor(3);
    CHECK( bmp2.GetScaleFactor() == 2 );
    CHECK( bmp.GetScaleFactor() == 3 );

    // Check that creating bitmap bundle from a bitmap with a scale factor
    // works as expected.
    wxBitmapBundle b = bmp2;
    CHECK( b.GetDefaultSize() == wxSize(8, 8) );
}

#endif // ports with scaled bitmaps support
