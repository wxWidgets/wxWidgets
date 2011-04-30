///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/measuring.cpp
// Purpose:     Tests for wxGraphicsRenderer::CreateMeasuringContext
// Author:      Kevin Ollivier
// Created:     2008-02-12
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Kevin Ollivier <kevino@theolliviers.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

// wxCairoRenderer::CreateMeasuringContext() is not implement for wxX11
#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/font.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/graphics.h"
// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MeasuringContextTestCase : public CppUnit::TestCase
{
public:
    MeasuringContextTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( MeasuringContextTestCase );
        CPPUNIT_TEST( GetTextExtent );
    CPPUNIT_TEST_SUITE_END();

    void GetTextExtent();

    wxWindow *m_win;

    DECLARE_NO_COPY_CLASS(MeasuringContextTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MeasuringContextTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MeasuringContextTestCase, "MeasuringContextTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void MeasuringContextTestCase::setUp()
{
    m_win = wxTheApp->GetTopWindow();
}

void MeasuringContextTestCase::tearDown()
{
    m_win = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void MeasuringContextTestCase::GetTextExtent()
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

    // TODO: Determine a way to make these tests more robust.
    CPPUNIT_ASSERT(width > 0.0);
    CPPUNIT_ASSERT(height > 0.0);

}

#endif
