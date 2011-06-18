///////////////////////////////////////////////////////////////////////////////
// Name:        tests/sizers/wrapsizer.cpp
// Purpose:     Unit tests for wxWrapSizer
// Author:      Catalin Raceanu
// Created:     2010-10-23
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets development team
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
#endif // WX_PRECOMP

#include "wx/wrapsizer.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class WrapSizerTestCase : public CppUnit::TestCase
{
public:
    WrapSizerTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( WrapSizerTestCase );
        CPPUNIT_TEST( CalcMin );
    CPPUNIT_TEST_SUITE_END();

    void CalcMin();

    wxWindow *m_win;
    wxSizer *m_sizer;

    DECLARE_NO_COPY_CLASS(WrapSizerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WrapSizerTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WrapSizerTestCase, "WrapSizerTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void WrapSizerTestCase::setUp()
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    m_win->SetClientSize(180, 240);

    m_sizer = new wxWrapSizer(wxHORIZONTAL);
    m_win->SetSizer(m_sizer);
}

void WrapSizerTestCase::tearDown()
{
    delete m_win;
    m_win = NULL;

    m_sizer = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void WrapSizerTestCase::CalcMin()
{
    const wxSize sizeTotal = m_win->GetClientSize();
    wxSize sizeMinExpected;

    // With a single child the min size must be the same as child size.
    const wxSize sizeChild1 = wxSize(sizeTotal.x/2 - 10, sizeTotal.y/4);
    sizeMinExpected = sizeChild1;

    wxWindow * const
        child1 = new wxWindow(m_win, wxID_ANY, wxDefaultPosition, sizeChild1);
    child1->SetBackgroundColour(*wxRED);
    m_sizer->Add(child1);
    m_win->Layout();

    CPPUNIT_ASSERT_EQUAL( sizeMinExpected, m_sizer->CalcMin() );

    // If both children can fit in the same row, the minimal size of the sizer
    // is determined by the sum of their minimal horizontal dimensions and
    // the maximum of their minimal vertical dimensions.
    const wxSize sizeChild2 = wxSize(sizeTotal.x/2 + 10, sizeTotal.y/3);
    sizeMinExpected.x += sizeChild2.x;
    sizeMinExpected.y = wxMax(sizeChild1.y, sizeChild2.y);

    wxWindow * const
        child2 = new wxWindow(m_win, wxID_ANY, wxDefaultPosition, sizeChild2);
    child2->SetBackgroundColour(*wxYELLOW);
    m_sizer->Add(child2);
    m_win->Layout();

    CPPUNIT_ASSERT_EQUAL( sizeMinExpected, m_sizer->CalcMin() );

    // Three children will take at least two rows so the minimal size in
    // vertical direction must increase.
    const wxSize sizeChild3 = wxSize(sizeTotal.x/2, sizeTotal.y/5);
    sizeMinExpected.y += sizeChild3.y;

    wxWindow * const
        child3 = new wxWindow(m_win, wxID_ANY, wxDefaultPosition, sizeChild3);
    child3->SetBackgroundColour(*wxGREEN);
    m_sizer->Add(child3);
    m_win->Layout();

    CPPUNIT_ASSERT_EQUAL( sizeMinExpected, m_sizer->CalcMin() );
}
