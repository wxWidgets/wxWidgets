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
#include "wx/dcps.h"
#include "wx/metafile.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// helper for XXXTextExtent() methods
// ----------------------------------------------------------------------------

template <typename T>
struct GetTextExtentTester
{
    // Constructor runs a couple of simple tests for GetTextExtent().
    GetTextExtentTester(const T& obj)
    {
        // Test that getting the height only doesn't crash.
        int y;
        obj.GetTextExtent("H", NULL, &y);

        CHECK( y > 1 );

        wxSize size = obj.GetTextExtent("Hello");
        CHECK( size.x > 1 );
        CHECK( size.y == y );

        // Test that getting text extent of an empty string returns (0, 0).
        CHECK( obj.GetTextExtent(wxString()) == wxSize() );
    }
};

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxDC::GetTextExtent", "[dc][text-extent]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    GetTextExtentTester<wxClientDC> testDC(dc);

    int w;
    dc.GetMultiLineTextExtent("Good\nbye", &w, NULL);
    const wxSize sz = dc.GetTextExtent("Good");
    CHECK( w == sz.x );

    CHECK( dc.GetMultiLineTextExtent("Good\nbye").y >= 2*sz.y );

    // Check that empty lines get counted
    CHECK( dc.GetMultiLineTextExtent("\n\n\n").y >= 3*sz.y );

    // And even empty strings count like one line.
    CHECK( dc.GetMultiLineTextExtent(wxString()) == wxSize(0, sz.y) );

    // Test the functions with some other DC kinds also.
#if wxUSE_PRINTING_ARCHITECTURE && wxUSE_POSTSCRIPT
    wxPostScriptDC psdc;
    // wxPostScriptDC doesn't have any font set by default but its
    // GetTextExtent() requires one to be set. This is probably a bug and we
    // should set the default font in it implicitly but for now just work
    // around it.
    psdc.SetFont(*wxNORMAL_FONT);
    GetTextExtentTester<wxPostScriptDC> testPS(psdc);
#endif

#if wxUSE_ENH_METAFILE
    wxEnhMetaFileDC metadc;
    GetTextExtentTester<wxEnhMetaFileDC> testMF(metadc);
#endif
}

TEST_CASE("wxDC::LeadingAndDescent", "[dc][text-extent]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    // Retrieving just the descent should work.
    int descent = -17;
    dc.GetTextExtent("foo", NULL, NULL, &descent);
    CHECK( descent != -17 );

    // Same for external leading.
    int leading = -289;
    dc.GetTextExtent("foo", NULL, NULL, NULL, &leading);
    CHECK( leading != -289 );

    // And both should also work for the empty string as they retrieve the
    // values valid for the entire font and not just this string.
    int descent2,
        leading2;
    dc.GetTextExtent("", NULL, NULL, &descent2, &leading2);

    CHECK( descent2 == descent );
    CHECK( leading2 == leading );
}

TEST_CASE("wxWindow::GetTextExtent", "[window][text-extent]")
{
    wxWindow* const win = wxTheApp->GetTopWindow();

    GetTextExtentTester<wxWindow> testWin(*win);
}

TEST_CASE("wxDC::GetPartialTextExtent", "[dc][text-extent][partial]")
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    wxArrayInt widths;
    REQUIRE( dc.GetPartialTextExtents("Hello", widths) );
    REQUIRE( widths.size() == 5 );
    CHECK( widths[0] == dc.GetTextExtent("H").x );
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
