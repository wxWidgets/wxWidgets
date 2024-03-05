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


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/sizer.h"
    #include "wx/vector.h"
#endif // WX_PRECOMP

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test fixtures
// ----------------------------------------------------------------------------

// Base class for the two fixtures below.
class GridSizerTestCaseBase
{
protected:
    explicit GridSizerTestCaseBase(wxGridSizer* sizer);
    ~GridSizerTestCaseBase();
    // Clear the current sizer contents and add the specified windows to it,
    // using the same flags for all of them.
    void SetChildren(const wxVector<wxWindow*>& children,
                     const wxSizerFlags& flags);

    wxWindow *m_win;
    wxGridSizer* const m_sizerBase;

    wxDECLARE_NO_COPY_CLASS(GridSizerTestCaseBase);
};

class GridSizerTestCase : public GridSizerTestCaseBase
{
protected:
    GridSizerTestCase()
        : GridSizerTestCaseBase(m_sizer = new wxGridSizer(2))
    {
    }

    wxGridSizer *m_sizer;
};

class FlexGridSizerTestCase : public GridSizerTestCaseBase
{
protected:
    FlexGridSizerTestCase()
        : GridSizerTestCaseBase(m_sizer = new wxFlexGridSizer(2))
    {
    }

    wxFlexGridSizer *m_sizer;
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

GridSizerTestCaseBase::GridSizerTestCaseBase(wxGridSizer* sizer)
    : m_sizerBase(sizer)
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    m_win->SetClientSize(127, 35);

    m_win->SetSizer(m_sizerBase);
}

GridSizerTestCaseBase::~GridSizerTestCaseBase()
{
    delete m_win;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

void GridSizerTestCaseBase::SetChildren(const wxVector<wxWindow*>& children,
                                        const wxSizerFlags& flags)
{
    m_sizerBase->Clear();
    for ( wxVector<wxWindow*>::const_iterator i = children.begin();
          i != children.end();
          ++i )
    {
        m_sizerBase->Add(*i, flags);
    }

    m_win->Layout();
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(GridSizerTestCase,
                 "wxGridSizer::Layout",
                 "[grid-sizer][sizer]")
{
    const wxSize sizeTotal = m_win->GetClientSize();
    const wxSize sizeChild(sizeTotal.x / 2, sizeTotal.y / 2);

    wxVector<wxWindow*> children;
    for ( int n = 0; n < 3; n++ )
    {
        children.push_back(new wxWindow(m_win, wxID_ANY));
    }

    SetChildren(children, wxSizerFlags().Expand());
    CHECK( children[0]->GetRect() == wxRect(wxPoint(0, 0), sizeChild) );
    CHECK( children[1]->GetRect() == wxRect(wxPoint(sizeChild.x, 0), sizeChild) );
    CHECK( children[2]->GetRect() == wxRect(wxPoint(0, sizeChild.y), sizeChild) );
}

TEST_CASE_METHOD(FlexGridSizerTestCase,
                 "wxFlexGridSizer::Layout",
                 "[grid-sizer][sizer]")
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
    SECTION("No flags")
    {
        SetChildren(children, wxSizerFlags());
        CHECK( children[0]->GetSize() == sizeChild );
        CHECK( children[1]->GetSize() == sizeChild );
        CHECK( children[2]->GetSize() == sizeChild );
        CHECK( children[3]->GetSize() == sizeChild );
    }

    // With just expand, they expand to fill the entire column and the row
    // containing them (which may or not expand on its own).
    SECTION("Expand")
    {
        SetChildren(children, wxSizerFlags().Expand());
        CHECK( children[0]->GetSize() == sizeChild );
        CHECK( children[1]->GetSize() == wxSize(sizeRest.x, sizeChild.y) );
        CHECK( children[2]->GetSize() == wxSize(sizeChild.x, sizeRest.y) );
        CHECK( children[3]->GetSize() == sizeRest );
    }

    // With expand and another alignment flag, they should expand only in the
    // direction in which the alignment is not given.
    SECTION("Expand and center vertically")
    {
        SetChildren(children, wxSizerFlags().Expand().CentreVertical());
        CHECK( children[0]->GetSize() == sizeChild );
        CHECK( children[1]->GetSize() == wxSize(sizeRest.x, sizeChild.y) );
        CHECK( children[2]->GetSize() == sizeChild );
        CHECK( children[3]->GetSize() == wxSize(sizeRest.x, sizeChild.y) );
    }

    // Same as above but mirrored.
    SECTION("Expand and center horizontally")
    {
        SetChildren(children, wxSizerFlags().Expand().CentreHorizontal());
        CHECK( children[0]->GetSize() == sizeChild );
        CHECK( children[1]->GetSize() == sizeChild );
        CHECK( children[2]->GetSize() == wxSize(sizeChild.x, sizeRest.y) );
        CHECK( children[3]->GetSize() == wxSize(sizeChild.x, sizeRest.y) );
    }

    // Test alignment flag too.
    SECTION("Right align")
    {
        SetChildren(children, wxSizerFlags().Right());
        CHECK( children[0]->GetPosition() == wxPoint(         0,           0) );
        CHECK( children[1]->GetPosition() == wxPoint(sizeRest.x,           0) );
        CHECK( children[2]->GetPosition() == wxPoint(         0, sizeChild.y) );
        CHECK( children[3]->GetPosition() == wxPoint(sizeRest.x, sizeChild.y) );
    }

    // Also test combining centering in one direction and aligning in another.
    SECTION("Right align and center vertically")
    {
        SetChildren(children, wxSizerFlags().Right().CentreVertical());

        const int yMid = sizeChild.y + (sizeRest.y - sizeChild.y) / 2;

        CHECK( children[0]->GetPosition() == wxPoint(         0,    0) );
        CHECK( children[1]->GetPosition() == wxPoint(sizeRest.x,    0) );
        CHECK( children[2]->GetPosition() == wxPoint(         0, yMid) );
        CHECK( children[3]->GetPosition() == wxPoint(sizeRest.x, yMid) );
    }
}

TEST_CASE_METHOD(GridSizerTestCase,
                 "wxGridSizer::IncompatibleFlags",
                 "[grid-sizer][sizer]")
{
    WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE
    (
        "Combining wxEXPAND and wxCENTRE should assert",
        m_sizer->Add(10, 10, wxSizerFlags().Expand().Centre())
    );
}

TEST_CASE_METHOD(FlexGridSizerTestCase,
                 "wxFlexGridSizer::GrowMode",
                 "[grid-sizer][sizer]")
{
    wxVector<wxWindow*> children;
    for ( int n = 0; n < 4; n++ )
    {
        children.push_back(new wxWindow(m_win, wxID_ANY));
    }

    // Proportions of growable columns should be respected.
    m_sizer->AddGrowableCol(0, 1);
    m_sizer->AddGrowableCol(1, 4);

    // However proportions of growable rows should not because we use
    // wxFLEX_GROWMODE_ALL which tells the sizer to grow all rows uniformly.
    m_sizer->AddGrowableRow(0, 1);
    m_sizer->AddGrowableRow(1, 4);

    m_sizer->SetFlexibleDirection(wxHORIZONTAL);
    m_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

    // Make both dimensions divisible by 5 to avoid dealing with extra pixels.
    m_win->SetClientSize(100, 100);

    SetChildren(children, wxSizerFlags().Expand());

    // Check that we have different widths but same heights for all children.
    CHECK( children[0]->GetSize() == wxSize(20, 50) );
    CHECK( children[1]->GetSize() == wxSize(80, 50) );
    CHECK( children[2]->GetSize() == wxSize(20, 50) );
    CHECK( children[3]->GetSize() == wxSize(80, 50) );
}
