///////////////////////////////////////////////////////////////////////////////
// Name:        tests/sizers/gridsizer.cpp
// Purpose:     Unit tests for wxGridSizer and wxFlexGridSizer.
// Author:      Vadim Zeitlin
// Created:     2015-04-03
// Copyright:   (c) 2015 Vadim Zeitlin <vadim@wxwidgets.org>
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
    #include "wx/vector.h"
#endif // WX_PRECOMP

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class GridSizerTestCase : public CppUnit::TestCase
{
public:
    GridSizerTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( GridSizerTestCase );
        CPPUNIT_TEST( Expand );
        CPPUNIT_TEST( IncompatibleFlags );
    CPPUNIT_TEST_SUITE_END();

    void Expand();
    void IncompatibleFlags();

    // Clear the current sizer contents and add the specified windows to it,
    // using the same flags for all of them.
    void SetChildren(const wxVector<wxWindow*>& children,
                     const wxSizerFlags& flags);

    wxWindow *m_win;
    wxFlexGridSizer *m_sizer;

    wxDECLARE_NO_COPY_CLASS(GridSizerTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GridSizerTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GridSizerTestCase, "GridSizerTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void GridSizerTestCase::setUp()
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    m_win->SetClientSize(127, 35);

    m_sizer = new wxFlexGridSizer(2);
    m_win->SetSizer(m_sizer);
}

void GridSizerTestCase::tearDown()
{
    delete m_win;
    m_win = NULL;

    m_sizer = NULL;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

void GridSizerTestCase::SetChildren(const wxVector<wxWindow*>& children,
                                    const wxSizerFlags& flags)
{
    m_sizer->Clear();
    for ( wxVector<wxWindow*>::const_iterator i = children.begin();
          i != children.end();
          ++i )
    {
        m_sizer->Add(*i, flags);
    }

    m_win->Layout();
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void GridSizerTestCase::Expand()
{
    const wxSize sizeTotal = m_win->GetClientSize();
    const wxSize sizeChild(sizeTotal.x / 4, sizeTotal.y / 4);
    const wxSize sizeRest(sizeTotal.x - sizeTotal.x / 4,
                          sizeTotal.y - sizeTotal.y / 4);

    wxVector<wxWindow*> children;
    for ( int n = 0; n < 4; n++ )
    {
        children.push_back(new wxWindow(m_win, wxID_ANY, wxDefaultPosition,
                                        sizeChild));
    }

    m_sizer->AddGrowableRow(1);
    m_sizer->AddGrowableCol(1);

    // Without Expand() windows have their initial size.
    SetChildren(children, wxSizerFlags());
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[0]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[1]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[2]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[3]->GetSize() );

    // With just expand, they expand to fill the entire column and the row
    // containing them (which may or not expand on its own).
    SetChildren(children, wxSizerFlags().Expand());
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[0]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeRest.x, sizeChild.y),
                          children[1]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeChild.x, sizeRest.y),
                          children[2]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeRest, children[3]->GetSize() );

    // With expand and another alignment flag, they should expand only in the
    // direction in which the alignment is not given.
    SetChildren(children, wxSizerFlags().Expand().CentreVertical());
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[0]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeRest.x, sizeChild.y),
                          children[1]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[2]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeRest.x, sizeChild.y),
                          children[3]->GetSize() );

    // Same as above but mirrored.
    SetChildren(children, wxSizerFlags().Expand().CentreHorizontal());
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[0]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( sizeChild, children[1]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeChild.x, sizeRest.y),
                          children[2]->GetSize() );
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeChild.x, sizeRest.y),
                          children[3]->GetSize() );
}

void GridSizerTestCase::IncompatibleFlags()
{
    WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE
    (
        "Combining wxEXPAND and wxCENTRE should assert",
        m_sizer->Add(10, 10, wxSizerFlags().Expand().Centre())
    );
}
