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

class TreebookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    TreebookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_treebook; }

    virtual wxEventType GetChangedEvent() const override
        { return wxEVT_TREEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
        { return wxEVT_TREEBOOK_PAGE_CHANGING; }

    CPPUNIT_TEST_SUITE( TreebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( Image );
        CPPUNIT_TEST( SubPages );
        CPPUNIT_TEST( ContainerPage );
        CPPUNIT_TEST( Expand );
        CPPUNIT_TEST( Delete );
    CPPUNIT_TEST_SUITE_END();

    void SubPages();
    void ContainerPage();
    void Expand();
    void Delete();

    wxTreebook *m_treebook;

    wxDECLARE_NO_COPY_CLASS(TreebookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreebookTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreebookTestCase, "TreebookTestCase" );

void TreebookTestCase::setUp()
{
    m_treebook = new wxTreebook(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}

void TreebookTestCase::tearDown()
{
    wxDELETE(m_treebook);
}

void TreebookTestCase::SubPages()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);

    CPPUNIT_ASSERT_EQUAL(2, m_treebook->GetPageParent(3));

    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);

    CPPUNIT_ASSERT_EQUAL(1, m_treebook->GetPageParent(2));

    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT_EQUAL(3, m_treebook->GetPageParent(5));
}

void TreebookTestCase::ContainerPage()
{
    // Get rid of the pages added in setUp().
    m_treebook->DeleteAllPages();
    CHECK( m_treebook->GetPageCount() == 0 );

    // Adding a page without the associated window should be allowed.
    REQUIRE_NOTHROW( m_treebook->AddPage(nullptr, "Container page") );
    CHECK( m_treebook->GetPageParent(0) == -1 );

    m_treebook->AddSubPage(new wxPanel(m_treebook), "Child page");
    CHECK( m_treebook->GetPageParent(1) == 0 );
}

void TreebookTestCase::Expand()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(1));
    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(3));

    m_treebook->CollapseNode(1);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(1));

    m_treebook->ExpandNode(3, false);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(3));

    m_treebook->ExpandNode(1);

    CPPUNIT_ASSERT(m_treebook->IsNodeExpanded(1));
}

void TreebookTestCase::Delete()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT_EQUAL(6, m_treebook->GetPageCount());

    m_treebook->DeletePage(3);

    CPPUNIT_ASSERT_EQUAL(3, m_treebook->GetPageCount());

    m_treebook->DeletePage(1);

    CPPUNIT_ASSERT_EQUAL(1, m_treebook->GetPageCount());

    m_treebook->DeletePage(0);

    CPPUNIT_ASSERT_EQUAL(0, m_treebook->GetPageCount());
}

#endif // wxUSE_TREEBOOK
