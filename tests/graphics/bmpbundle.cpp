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
#include "wx/dcmemory.h"

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

TEST_CASE("BitmapBundle::GetBitmap", "[bmpbundle]")
{
    const wxBitmapBundle b = wxBitmapBundle::FromBitmap(wxBitmap(16, 16));

    CHECK( b.GetBitmap(wxSize(16, 16)).GetSize() == wxSize(16, 16) );
    CHECK( b.GetBitmap(wxSize(32, 32)).GetSize() == wxSize(32, 32) );
    CHECK( b.GetBitmap(wxSize(24, 24)).GetSize() == wxSize(24, 24) );
}

// Helper functions for the test below.
namespace
{

// Default size here doesn't really matter.
const wxSize BITMAP_SIZE(16, 16);

// The choice of colours here is arbitrary too, but they need to be all
// different to allow identifying which bitmap got scaled.
struct ColourAtScale
{
    double scale;
    wxUint32 rgb;
};

const ColourAtScale colours[] =
{
    { 1.0, 0x000000ff },
    { 1.5, 0x0000ff00 },
    { 2.0, 0x00ff0000 },
};

// Return the colour used for the (original) bitmap at the given scale.
wxColour GetColourForScale(double scale)
{
    wxColour col;
    for ( size_t n = 0; n < WXSIZEOF(colours); ++n )
    {
        if ( colours[n].scale == scale )
        {
            col.SetRGB(colours[n].rgb);
            return col;
        }
    }

    wxFAIL_MSG("no colour for this scale");

    return col;
}

double GetScaleFromColour(const wxColour& col)
{
    const wxUint32 rgb = col.GetRGB();
    for ( size_t n = 0; n < WXSIZEOF(colours); ++n )
    {
        if ( colours[n].rgb == rgb )
            return colours[n].scale;
    }

    wxFAIL_MSG(wxString::Format("no scale for colour %s", col.GetAsString()));

    return 0.0;
}

double SizeToScale(const wxSize& size)
{
    return static_cast<double>(size.y) / BITMAP_SIZE.y;
}

wxBitmap MakeSolidBitmap(double scale)
{
    wxBitmap bmp(BITMAP_SIZE*scale);

    wxMemoryDC dc(bmp);
    dc.SetBackground(GetColourForScale(scale));
    dc.Clear();

    return bmp;
}

wxColour GetBitmapColour(const wxBitmap& bmp)
{
    const wxImage img = bmp.ConvertToImage();

    // We just assume the bitmap is solid colour, we could check it, but it
    // doesn't seem really useful to do it.
    return wxColour(img.GetRed(0, 0), img.GetGreen(0, 0), img.GetBlue(0, 0));
}

// This struct exists just to allow using it conveniently in CHECK_THAT().
struct BitmapAtScale
{
    BitmapAtScale(const wxBitmapBundle& b, double scale)
        : size(b.GetPreferredBitmapSizeAtScale(scale)),
          bitmap(b.GetBitmap(size))
    {
    }

    const wxSize size;
    const wxBitmap bitmap;
};

class BitmapAtScaleMatcher : public Catch::MatcherBase<BitmapAtScale>
{
public:
    explicit BitmapAtScaleMatcher(double scale, double scaleOrig)
        : m_scale(scale),
          m_scaleOrig(scaleOrig)
    {
    }

    bool match(const BitmapAtScale& bitmapAtScale) const wxOVERRIDE
    {
        const wxBitmap& bmp = bitmapAtScale.bitmap;

        if ( SizeToScale(bitmapAtScale.size) != m_scale ||
                SizeToScale(bmp.GetSize()) != m_scale )
        {
            m_diffDesc.Printf("should have scale %.1f", m_scale);
        }

        if ( GetBitmapColour(bmp) != GetColourForScale(m_scaleOrig) )
        {
            if ( m_diffDesc.empty() )
                m_diffDesc = "should be ";
            else
                m_diffDesc += " and be ";

            m_diffDesc += wxString::Format("created from x%.1f", m_scaleOrig);
        }

        return m_diffDesc.empty();
    }

    std::string describe() const wxOVERRIDE
    {
        return m_diffDesc.utf8_string();
    }

private:
    const double m_scale;
    const double m_scaleOrig;
    mutable wxString m_diffDesc;
};

// The first parameter here determines the size of the expected bitmap and the
// second one, which defaults to the first one if it's not specified, the size
// of the bitmap which must have been scaled to create the bitmap of the right
// size.
BitmapAtScaleMatcher SameAs(double scale, double scaleOrig = 0.0)
{
    if ( scaleOrig == 0.0 )
        scaleOrig = scale;

    return BitmapAtScaleMatcher(scale, scaleOrig);
}

} // anonymous namespace

namespace Catch
{
    template <>
    struct StringMaker<BitmapAtScale>
    {
        static std::string convert(const BitmapAtScale& bitmapAtScale)
        {
            const wxBitmap& bmp = bitmapAtScale.bitmap;

            wxString scaleError;
            if ( bmp.GetSize() != bitmapAtScale.size )
            {
                scaleError.Printf(" (DIFFERENT from expected %.1f)",
                                  SizeToScale(bitmapAtScale.size));
            }

            return wxString::Format
                   (
                        "x%.1f bitmap%s created from x%.1f",
                        SizeToScale(bmp.GetSize()),
                        scaleError,
                        GetScaleFromColour(GetBitmapColour(bmp))
                   ).utf8_string();
        }
    };
}

TEST_CASE("BitmapBundle::GetPreferredSize", "[bmpbundle]")
{
    // Check that empty bundle doesn't have any preferred size.
    wxBitmapBundle b;
    CHECK( b.GetPreferredBitmapSizeAtScale(1) == wxDefaultSize );

    const wxBitmap normal = MakeSolidBitmap(1.0);
    const wxBitmap middle = MakeSolidBitmap(1.5);
    const wxBitmap bigger = MakeSolidBitmap(2.0);


    // Then check what happens if there is only a single bitmap.
    b = wxBitmapBundle::FromBitmap(normal);

    // We should avoid scaling as long as the size is close enough to the
    // actual bitmap size.
    CHECK_THAT( BitmapAtScale(b, 0   ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1   ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.25), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.4 ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.5 ), SameAs(1) );

    // Once it becomes too big, we're going to need to scale, but we should be
    // scaling by an integer factor.
    CHECK_THAT( BitmapAtScale(b, 1.75), SameAs(2, 1) );
    CHECK_THAT( BitmapAtScale(b, 2   ), SameAs(2, 1) );
    CHECK_THAT( BitmapAtScale(b, 2.25), SameAs(2, 1) );
    CHECK_THAT( BitmapAtScale(b, 2.5 ), SameAs(3, 1) );


    // Now check what happens when there is also a double size bitmap.
    b = wxBitmapBundle::FromBitmaps(normal, bigger);

    // Check that the existing bitmaps are used without scaling for most of the
    // typical scaling values.
    CHECK_THAT( BitmapAtScale(b, 0   ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1   ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.25), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.4 ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.5 ), SameAs(1) );
    CHECK_THAT( BitmapAtScale(b, 1.75), SameAs(2) );
    CHECK_THAT( BitmapAtScale(b, 2   ), SameAs(2) );
    CHECK_THAT( BitmapAtScale(b, 2.5 ), SameAs(2) );
    CHECK_THAT( BitmapAtScale(b, 3   ), SameAs(2) );

    // This scale is too big to use any of the existing bitmaps, so they will
    // be scaled, but use integer factors and, importantly, scale the correct
    // bitmap using them: we need to scale the small bitmap by a factor of 3,
    // rather than scaling the larger bitmap by a factor of 1.5 here, but we
    // must also scale the larger one by a factor of 2 rather than scaling the
    // small one by a factor of 4.
    CHECK_THAT( BitmapAtScale(b, 3.33), SameAs(3, 1) );
    CHECK_THAT( BitmapAtScale(b, 4   ), SameAs(4, 2) );
    CHECK_THAT( BitmapAtScale(b, 5   ), SameAs(5, 1) );


    // Finally check that things work as expected when we have 3 versions.
    wxVector<wxBitmap> bitmaps;
    bitmaps.push_back(normal);
    bitmaps.push_back(middle);
    bitmaps.push_back(bigger);
    b = wxBitmapBundle::FromBitmaps(bitmaps);

    CHECK_THAT( BitmapAtScale(b, 0   ), SameAs(1.0) );
    CHECK_THAT( BitmapAtScale(b, 1   ), SameAs(1.0) );
    CHECK_THAT( BitmapAtScale(b, 1.25), SameAs(1.0) );
    CHECK_THAT( BitmapAtScale(b, 1.4 ), SameAs(1.5) );
    CHECK_THAT( BitmapAtScale(b, 1.5 ), SameAs(1.5) );
    CHECK_THAT( BitmapAtScale(b, 1.75), SameAs(1.5) );
    CHECK_THAT( BitmapAtScale(b, 2   ), SameAs(2.0) );
    CHECK_THAT( BitmapAtScale(b, 2.5 ), SameAs(2.0) );
    CHECK_THAT( BitmapAtScale(b, 3   ), SameAs(2.0) );

    CHECK_THAT( BitmapAtScale(b, 3.33), SameAs(3.0, 1.5) );
    CHECK_THAT( BitmapAtScale(b, 4.25), SameAs(4.0, 2.0) );
    CHECK_THAT( BitmapAtScale(b, 4.50), SameAs(4.5, 1.5) );
    CHECK_THAT( BitmapAtScale(b, 5   ), SameAs(5.0, 1.0) );
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

TEST_CASE("BitmapBundle::GetConsensusSize", "[bmpbundle]")
{
    // Just a trivial helper to make writing the tests below simpler.
    struct Bundles
    {
        wxVector<wxBitmapBundle> vec;

        void Add(int size)
        {
            vec.push_back(wxBitmapBundle::FromBitmap(wxSize(size, size)));
        }

        int GetConsensusSize(double scale) const
        {
            return wxBitmapBundle::GetConsensusSizeFor(scale, vec).y;
        }
    } bundles;

    // When there is a tie, a larger size is chosen by default.
    bundles.Add(16);
    bundles.Add(24);
    CHECK( bundles.GetConsensusSize(2) == 48 );

    // Breaking the tie results in the smaller size winning now.
    bundles.Add(16);
    CHECK( bundles.GetConsensusSize(2) == 32 );

    // Integer scaling factors should be preferred.
    CHECK( bundles.GetConsensusSize(1.5) == 16 );
}
