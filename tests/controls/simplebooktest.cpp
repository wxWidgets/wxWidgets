///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/simplebooktest.cpp
// Purpose:     wxSimplebook unit test
// Author:      Vadim Zeitlin
// Created:     2013-06-23
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BOOKCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/simplebook.h"
#include "bookctrlbasetest.h"

#include <memory>

class SimplebookTestCase : public BookCtrlBaseTestCase
{
public:
    SimplebookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_simplebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_BOOKCTRL_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_BOOKCTRL_PAGE_CHANGING; }

    std::unique_ptr<wxSimplebook> m_simplebook;

    wxDECLARE_NO_COPY_CLASS(SimplebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(SimplebookTestCase, "Simplebook",
                       "[simplebook][book][SimplebookTestCase][winui-v0-supported]");

SimplebookTestCase::SimplebookTestCase()
{
    m_simplebook = make_unique<wxSimplebook>(
        wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


TEST_CASE_METHOD(SimplebookTestCase, "Simplebook::InsertPreservesSelectedPageIdentity",
                 "[simplebook][SimplebookTestCase][winui-v0-supported]")
{
    REQUIRE(m_simplebook->SetSelection(1) == 0);
    wxWindow* const selected = m_simplebook->GetPage(1);
    REQUIRE(selected->IsShown());

    wxPanel* const unselectedInsertion = new wxPanel(m_simplebook.get());
    REQUIRE(m_simplebook->InsertPage(
        0, unselectedInsertion, "Unselected insertion", false));
    REQUIRE(m_simplebook->GetSelection() == 2);
    REQUIRE(m_simplebook->GetCurrentPage() == selected);
    REQUIRE(selected->IsShown());
    REQUIRE(!unselectedInsertion->IsShown());

    wxPanel* const selectedInsertion = new wxPanel(m_simplebook.get());
    REQUIRE(m_simplebook->InsertPage(
        0, selectedInsertion, "Selected insertion", true));
    REQUIRE(m_simplebook->GetSelection() == 0);
    REQUIRE(m_simplebook->GetCurrentPage() == selectedInsertion);
    REQUIRE(selectedInsertion->IsShown());
    REQUIRE(!selected->IsShown());

    size_t shown = 0;
    for ( size_t i = 0; i < m_simplebook->GetPageCount(); ++i )
    {
        if ( m_simplebook->GetPage(i)->IsShown() )
            ++shown;
    }
    REQUIRE(shown == static_cast<size_t>(1));
}

TEST_CASE_METHOD(SimplebookTestCase, "Simplebook::RemoveHidesTransferredPage",
                 "[simplebook][SimplebookTestCase][winui-v0-supported]")
{
    REQUIRE(m_simplebook->SetSelection(1) == 0);
    wxWindow* const removed = m_simplebook->GetPage(1);
    REQUIRE(removed->IsShown());

    REQUIRE(m_simplebook->RemovePage(1));
    REQUIRE(!removed->IsShown());
    REQUIRE(m_simplebook->FindPage(removed) == wxNOT_FOUND);

    delete removed;
}

#endif // wxUSE_BOOKCTRL

