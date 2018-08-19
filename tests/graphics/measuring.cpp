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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MeasuringTextTestCase : public CppUnit::TestCase
{
public:
    MeasuringTextTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MeasuringTextTestCase );
        CPPUNIT_TEST( DCGetTextExtent );
        CPPUNIT_TEST( LeadingAndDescent );
        CPPUNIT_TEST( WindowGetTextExtent );
        CPPUNIT_TEST( GetPartialTextExtent );
#ifdef TEST_GC
        CPPUNIT_TEST( GraphicsGetTextExtent );
#endif // TEST_GC
    CPPUNIT_TEST_SUITE_END();

    void DCGetTextExtent();
    void LeadingAndDescent();
    void WindowGetTextExtent();

    void GetPartialTextExtent();

#ifdef TEST_GC
    void GraphicsGetTextExtent();
#endif // TEST_GC

    wxDECLARE_NO_COPY_CLASS(MeasuringTextTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MeasuringTextTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MeasuringTextTestCase, "MeasuringTextTestCase" );

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

        CPPUNIT_ASSERT( y > 1 );

        wxSize size = obj.GetTextExtent("Hello");
        CPPUNIT_ASSERT( size.x > 1 );
        CPPUNIT_ASSERT_EQUAL( y, size.y );
    }
};

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void MeasuringTextTestCase::DCGetTextExtent()
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    GetTextExtentTester<wxClientDC> testDC(dc);

    int w;
    dc.GetMultiLineTextExtent("Good\nbye", &w, NULL);
    const wxSize sz = dc.GetTextExtent("Good");
    CPPUNIT_ASSERT_EQUAL( sz.x, w );

    CPPUNIT_ASSERT( dc.GetMultiLineTextExtent("Good\nbye").y >= 2*sz.y );

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

void MeasuringTextTestCase::LeadingAndDescent()
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    // Retrieving just the descent should work.
    int descent = -17;
    dc.GetTextExtent("foo", NULL, NULL, &descent);
    CPPUNIT_ASSERT( descent != -17 );

    // Same for external leading.
    int leading = -289;
    dc.GetTextExtent("foo", NULL, NULL, NULL, &leading);
    CPPUNIT_ASSERT( leading != -289 );

    // And both should also work for the empty string as they retrieve the
    // values valid for the entire font and not just this string.
    int descent2,
        leading2;
    dc.GetTextExtent("", NULL, NULL, &descent2, &leading2);

    CPPUNIT_ASSERT_EQUAL( descent, descent2 );
    CPPUNIT_ASSERT_EQUAL( leading, leading2 );
}

void MeasuringTextTestCase::WindowGetTextExtent()
{
    wxWindow* const win = wxTheApp->GetTopWindow();

    GetTextExtentTester<wxWindow> testWin(*win);
}

void MeasuringTextTestCase::GetPartialTextExtent()
{
    wxClientDC dc(wxTheApp->GetTopWindow());

    wxArrayInt widths;
    CPPUNIT_ASSERT( dc.GetPartialTextExtents("Hello", widths) );
    CPPUNIT_ASSERT_EQUAL( 5, widths.size() );
    CPPUNIT_ASSERT_EQUAL( widths[0], dc.GetTextExtent("H").x );
    CPPUNIT_ASSERT_EQUAL( widths[4], dc.GetTextExtent("Hello").x );
}

#ifdef TEST_GC

void MeasuringTextTestCase::GraphicsGetTextExtent()
{
    wxGraphicsRenderer* renderer = wxGraphicsRenderer::GetDefaultRenderer();
    CPPUNIT_ASSERT(renderer);
    wxGraphicsContext* context = renderer->CreateMeasuringContext();
    CPPUNIT_ASSERT(context);
    wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    CPPUNIT_ASSERT(font.IsOk());
    context->SetFont(font, *wxBLACK);
    double width, height, descent, externalLeading = 0.0;
    context->GetTextExtent("x", &width, &height, &descent, &externalLeading);
    delete context;

    // TODO: Determine a way to make these tests more robust.
    CPPUNIT_ASSERT(width > 0.0);
    CPPUNIT_ASSERT(height > 0.0);

}

#endif // TEST_GC
