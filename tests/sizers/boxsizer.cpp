///////////////////////////////////////////////////////////////////////////////
// Name:        tests/sizers/boxsizer.cpp
// Purpose:     Unit tests for wxBoxSizer
// Author:      Vadim Zeitlin
// Created:     2010-03-06
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
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
    #include "wx/sizer.h"
#endif // WX_PRECOMP

inline std::ostream& operator<<(std::ostream& o, const wxSize& s)
{
    return o << s.x << 'x' << s.y;
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class BoxSizerTestCase : public CppUnit::TestCase
{
public:
    BoxSizerTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( BoxSizerTestCase );
        CPPUNIT_TEST( Size1 );
    CPPUNIT_TEST_SUITE_END();

    void Size1();

    wxWindow *m_win;
    wxSizer *m_sizer;

    DECLARE_NO_COPY_CLASS(BoxSizerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BoxSizerTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BoxSizerTestCase, "BoxSizerTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void BoxSizerTestCase::setUp()
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    m_win->SetClientSize(127, 35);

    m_sizer = new wxBoxSizer(wxHORIZONTAL);
    m_win->SetSizer(m_sizer);
}

void BoxSizerTestCase::tearDown()
{
    delete m_win;
    m_win = NULL;

    m_sizer = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void BoxSizerTestCase::Size1()
{
    const wxSize sizeTotal = m_win->GetClientSize();
    const wxSize sizeChild = sizeTotal / 2;

    wxWindow * const
        child = new wxWindow(m_win, wxID_ANY, wxDefaultPosition, sizeChild);
    m_sizer->Add(child);
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeChild, child->GetSize() );
;
    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags(1));
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeTotal.x, sizeChild.y), child->GetSize() );

    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags(1).Expand());
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeTotal, child->GetSize() );
}

