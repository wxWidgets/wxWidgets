///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treebooktest.cpp
// Purpose:     wxtreebook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TREEBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/treebook.h"
#include "bookctrlbasetest.h"

#include <memory>

class TreebookTestCase : public BookCtrlBaseTestCase
{
public:
    TreebookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_treebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TREEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TREEBOOK_PAGE_CHANGING; }

    std::unique_ptr<wxTreebook> m_treebook;

    wxDECLARE_NO_COPY_CLASS(TreebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(TreebookTestCase, "Treebook",
                       "[treebook][book]");

// wxTreebook supports images, unlike most of the other book controls.
wxBOOK_CTRL_BASE_TEST_CASE(TreebookTestCase, "Treebook", Image,
                           "[treebook][book]");

TreebookTestCase::TreebookTestCase()
{
    m_treebook = make_unique<wxTreebook>(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


TEST_CASE_METHOD(TreebookTestCase, "Treebook::SubPages", "[treebook]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);

    CHECK(m_treebook->GetPageParent(3) == 2);

    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);

    CHECK(m_treebook->GetPageParent(2) == 1);

    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(m_treebook->GetPageParent(5) == 3);
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::ContainerPage", "[treebook]")
{
    // Get rid of the pages added in setUp().
    m_treebook->DeleteAllPages();
    CHECK( m_treebook->GetPageCount() == 0 );

    // Adding a page without the associated window should be allowed.
    REQUIRE_NOTHROW( m_treebook->AddPage(nullptr, "Container page") );
    CHECK( m_treebook->GetPageParent(0) == -1 );

    m_treebook->AddSubPage(new wxPanel(m_treebook.get()), "Child page");
    CHECK( m_treebook->GetPageParent(1) == 0 );
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::Expand", "[treebook]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(!m_treebook->IsNodeExpanded(1));
    CHECK(!m_treebook->IsNodeExpanded(3));

    m_treebook->CollapseNode(1);

    CHECK(!m_treebook->IsNodeExpanded(1));

    m_treebook->ExpandNode(3, false);

    CHECK(!m_treebook->IsNodeExpanded(3));

    m_treebook->ExpandNode(1);

    CHECK(m_treebook->IsNodeExpanded(1));
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::Delete", "[treebook]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(m_treebook->GetPageCount() == 6);

    m_treebook->DeletePage(3);

    CHECK(m_treebook->GetPageCount() == 3);

    m_treebook->DeletePage(1);

    CHECK(m_treebook->GetPageCount() == 1);

    m_treebook->DeletePage(0);

    CHECK(m_treebook->GetPageCount() == 0);
}

#endif // wxUSE_TREEBOOK
