///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/measuring.cpp
// Purpose:     Tests for wxGraphicsRenderer::CreateMeasuringContext
// Author:      Kevin Ollivier, Vadim Zeitlin (non wxGC parts)
// Created:     2008-02-12
// Copyright:   (c) 2008 Kevin Ollivier <kevino@theolliviers.com>
//              (c) 2012 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/bitmap.h"
#include "wx/colour.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/font.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

// wxCairoRenderer::CreateMeasuringContext() is not implement for wxX11
#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)
    #include "wx/graphics.h"
    #define TEST_GC
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcps.h"
#include "wx/image.h"
#include "wx/metafile.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// helper for XXXTextExtent() methods
// ----------------------------------------------------------------------------

namespace
{

// Run a couple of simple tests for GetTextExtent().
template <typename T>
void GetTextExtentTester(const T& obj)
{
    // Test that getting the height only doesn't crash.
    int y;
    obj.GetTextExtent("H", nullptr, &y);

    CHECK( y > 1 );

    wxSize size = obj.GetTextExtent("Hello");
    CHECK( size.x > 1 );
    CHECK( size.y == y );

    // Test that getting text extent of an empty string returns (0, 0).
    CHECK( obj.GetTextExtent(wxString()) == wxSize() );
}

// Currently this is known to work in wxMSW, wxGTK3 and wxQt, to be checked
// (and enabled) for the other ports.
#if defined(__WXMSW__) || defined(__WXGTK3__) || defined(__WXQT__)
    #define wxHAS_ROTATED_TEXT_RIGHT_ANGLE_TEST
#endif

#ifdef wxHAS_ROTATED_TEXT_RIGHT_ANGLE_TEST

const wxSize ROTATED_TEXT_BITMAP_SIZE(240, 240);
const wxPoint ROTATED_TEXT_ORIGIN(120, 120);

wxRect GetRotatedTextBounds(double angle)
{
    wxBitmap bitmap(ROTATED_TEXT_BITMAP_SIZE);
    wxMemoryDC dc(bitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(wxFontInfo(24).Family(wxFONTFAMILY_SWISS)));
    dc.DrawRotatedText("TEST", ROTATED_TEXT_ORIGIN.x, ROTATED_TEXT_ORIGIN.y,
                       angle);
    dc.SelectObject(wxNullBitmap);

    const wxImage image = bitmap.ConvertToImage();

    int left = ROTATED_TEXT_BITMAP_SIZE.x;
    int top = ROTATED_TEXT_BITMAP_SIZE.y;
    int right = -1;
    int bottom = -1;

    for ( int y = 0; y < ROTATED_TEXT_BITMAP_SIZE.y; ++y )
    {
        for ( int x = 0; x < ROTATED_TEXT_BITMAP_SIZE.x; ++x )
        {
            if ( image.GetRed(x, y) < 250 || image.GetGreen(x, y) < 250 ||
                 image.GetBlue(x, y) < 250 )
            {
                left = wxMin(left, x);
                top = wxMin(top, y);
                right = wxMax(right, x);
                bottom = wxMax(bottom, y);
            }
        }
    }

    if ( right < left || bottom < top )
        return wxRect();

    return wxRect(left, top, right - left + 1, bottom - top + 1);
}

#endif // wxHAS_ROTATED_TEXT_RIGHT_ANGLE_TEST

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxDC::GetTextExtent", "[dc][text-extent]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    GetTextExtentTester(dc);

    int w;
    dc.GetMultiLineTextExtent("Good\nbye", &w, nullptr);
    const wxSize sz = dc.GetTextExtent("Good");
    CHECK( w == sz.x );

    CHECK( dc.GetMultiLineTextExtent("Good\nbye").y >= 2*sz.y );

    // Check that empty lines get counted
    CHECK( dc.GetMultiLineTextExtent("\n\n\n").y >= 3*sz.y );

    // And even empty strings count like one line.
    CHECK( dc.GetMultiLineTextExtent(wxString()) == wxSize(0, sz.y) );
}

TEST_CASE("wxMemoryDC::GetTextExtent", "[memdc][text-extent]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC memdc(bmp);
    GetTextExtentTester(memdc);

    // Under MSW, this wxDC should work even without any valid font -- but
    // this is not the case under wxGTK and probably neither elsewhere, so
    // restrict this test to that platform only.
#ifdef __WXMSW__
    memdc.SetFont(wxNullFont);
    GetTextExtentTester(memdc);
#endif // __WXMSW__
}

#ifdef wxHAS_ROTATED_TEXT_RIGHT_ANGLE_TEST

TEST_CASE("wxMemoryDC::DrawRotatedText cardinal angles", "[dc][text][msw]")
{
    const wxRect bounds90 = GetRotatedTextBounds(90);
    const wxRect bounds180 = GetRotatedTextBounds(180);
    const wxRect bounds270 = GetRotatedTextBounds(270);

    REQUIRE(!bounds90.IsEmpty());
    REQUIRE(!bounds180.IsEmpty());
    REQUIRE(!bounds270.IsEmpty());

    // Check the bitmap footprint to catch cardinal angles accidentally taking
    // the unrotated text drawing path.
    CHECK(bounds90.GetHeight() > bounds90.GetWidth());
    CHECK(bounds180.GetRight() <= ROTATED_TEXT_ORIGIN.x + 2);
    CHECK(bounds270.GetHeight() > bounds270.GetWidth());
}

#endif // wxHAS_ROTATED_TEXT_RIGHT_ANGLE_TEST

#if wxUSE_PRINTING_ARCHITECTURE && wxUSE_POSTSCRIPT
TEST_CASE("wxPostScriptDC::GetTextExtent", "[psdc][text-extent]")
{
    wxPostScriptDC psdc;
    // wxPostScriptDC doesn't have any font set by default but its
    // GetTextExtent() requires one to be set. This is probably a bug and we
    // should set the default font in it implicitly but for now just work
    // around it.
    psdc.SetFont(*wxNORMAL_FONT);
    GetTextExtentTester(psdc);
}
#endif // wxUSE_POSTSCRIPT

#if wxUSE_ENH_METAFILE
TEST_CASE("wxEnhMetaFileDC::GetTextExtent", "[emfdc][text-extent]")
{
    wxEnhMetaFileDC metadc;
    GetTextExtentTester(metadc);
}
#endif // wxUSE_ENH_METAFILE

TEST_CASE("wxDC::LeadingAndDescent", "[dc][text-extent]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    // Retrieving just the descent should work.
    int descent = -17;
    dc.GetTextExtent("foo", nullptr, nullptr, &descent);
    CHECK( descent != -17 );

    // Same for external leading.
    int leading = -289;
    dc.GetTextExtent("foo", nullptr, nullptr, nullptr, &leading);
    CHECK( leading != -289 );

    // And both should also work for the empty string as they retrieve the
    // values valid for the entire font and not just this string.
    int descent2,
        leading2;
    dc.GetTextExtent("", nullptr, nullptr, &descent2, &leading2);

    CHECK( descent2 == descent );
    CHECK( leading2 == leading );
}

TEST_CASE("wxWindow::GetTextExtent", "[window][text-extent]")
{
    wxWindow* const win = wxTheApp->GetTopWindow();

    GetTextExtentTester(*win);
}

TEST_CASE("wxDC::GetPartialTextExtent", "[dc][text-extent][partial]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    wxArrayInt widths;
    REQUIRE( dc.GetPartialTextExtents("Hello", widths) );
    REQUIRE( widths.size() == 5 );
    CHECK( widths[0] == dc.GetTextExtent("H").x );
#ifdef __WXQT__
    // Skip test which work locally, but not when run on GitHub CI
    if ( IsAutomaticTest() )
        return;
#endif
    CHECK( widths[4] == dc.GetTextExtent("Hello").x );
}

#ifdef TEST_GC

TEST_CASE("wxGC::GetTextExtent", "[dc][text-extent]")
{
    wxGraphicsRenderer* renderer = wxGraphicsRenderer::GetDefaultRenderer();
    REQUIRE(renderer);
    wxGraphicsContext* context = renderer->CreateMeasuringContext();
    REQUIRE(context);
    wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    REQUIRE(font.IsOk());
    context->SetFont(font, *wxBLACK);
    double width, height, descent, externalLeading = 0.0;
    context->GetTextExtent("x", &width, &height, &descent, &externalLeading);
    delete context;

    // TODO: Determine a way to make these tests more robust.
    CHECK(width > 0.0);
    CHECK(height > 0.0);
}

#endif // TEST_GC

namespace
{

int FindFirstNonWhitePixelX(const wxBitmap& bmp)
{
    const wxImage img = bmp.ConvertToImage();
    const int width = img.GetWidth();
    const int height = img.GetHeight();

    for ( int x = 0; x < width; ++x )
    {
        for ( int y = 0; y < height; ++y )
        {
            if ( img.GetRed(x, y) < 250 || img.GetGreen(x, y) < 250 ||
                 img.GetBlue(x, y) < 250 )
                return x;
        }
    }

    return width;
}

enum class DrawTextWithTabs
{
    Directly,
    AsLabel
};

int DrawTextWithTabAndFindFirstInk(DrawTextWithTabs mode, int* charWidth)
{
    wxBitmap bmp(200, 50, 24);
    wxMemoryDC dc(bmp);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(wxFontInfo(12).Family(wxFONTFAMILY_MODERN)));

    *charWidth = dc.GetCharWidth();

    const wxString text("\tX");
    switch ( mode )
    {
        case DrawTextWithTabs::Directly:
            dc.DrawText(text, 0, 0);
            break;

        case DrawTextWithTabs::AsLabel:
            dc.DrawLabel(text, bmp.GetSize());
            break;
    }

    dc.SelectObject(wxNullBitmap);

    return FindFirstNonWhitePixelX(bmp);
}

} // anonymous namespace

TEST_CASE("wxDC::DrawTextWithTabs", "[dc][text]")
{
    int charWidth = 0;
    int firstInkX =
        DrawTextWithTabAndFindFirstInk(DrawTextWithTabs::Directly, &charWidth);

    REQUIRE( firstInkX < 200 );
    CHECK( firstInkX > 2*charWidth );

    firstInkX =
        DrawTextWithTabAndFindFirstInk(DrawTextWithTabs::AsLabel, &charWidth);

    REQUIRE( firstInkX < 200 );
    CHECK( firstInkX > 2*charWidth );
}
