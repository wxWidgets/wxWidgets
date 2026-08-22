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

class SimplebookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    SimplebookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_simplebook; }

    virtual wxEventType GetChangedEvent() const override
        { return wxEVT_BOOKCTRL_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
        { return wxEVT_BOOKCTRL_PAGE_CHANGING; }

    CPPUNIT_TEST_SUITE( SimplebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( InsertPreservesSelectedPageIdentity );
        CPPUNIT_TEST( RemoveHidesTransferredPage );
    CPPUNIT_TEST_SUITE_END();

    void InsertPreservesSelectedPageIdentity();
    void RemoveHidesTransferredPage();

    wxSimplebook *m_simplebook;

    wxDECLARE_NO_COPY_CLASS(SimplebookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SimplebookTestCase );

// also include in its own registry so that these tests can be run alone
wxREGISTER_UNIT_TEST_WITH_TAGS(
    SimplebookTestCase,
    "[SimplebookTestCase][winui-v0-supported]");

void SimplebookTestCase::setUp()
{
    m_simplebook = new wxSimplebook(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}

void SimplebookTestCase::tearDown()
{
    wxDELETE(m_simplebook);
}

void SimplebookTestCase::InsertPreservesSelectedPageIdentity()
{
    CPPUNIT_ASSERT_EQUAL(0, m_simplebook->SetSelection(1));
    wxWindow* const selected = m_simplebook->GetPage(1);
    CPPUNIT_ASSERT(selected->IsShown());

    wxPanel* const unselectedInsertion = new wxPanel(m_simplebook);
    CPPUNIT_ASSERT(m_simplebook->InsertPage(
        0, unselectedInsertion, "Unselected insertion", false));
    CPPUNIT_ASSERT_EQUAL(2, m_simplebook->GetSelection());
    CPPUNIT_ASSERT(m_simplebook->GetCurrentPage() == selected);
    CPPUNIT_ASSERT(selected->IsShown());
    CPPUNIT_ASSERT(!unselectedInsertion->IsShown());

    wxPanel* const selectedInsertion = new wxPanel(m_simplebook);
    CPPUNIT_ASSERT(m_simplebook->InsertPage(
        0, selectedInsertion, "Selected insertion", true));
    CPPUNIT_ASSERT_EQUAL(0, m_simplebook->GetSelection());
    CPPUNIT_ASSERT(m_simplebook->GetCurrentPage() == selectedInsertion);
    CPPUNIT_ASSERT(selectedInsertion->IsShown());
    CPPUNIT_ASSERT(!selected->IsShown());

    size_t shown = 0;
    for ( size_t i = 0; i < m_simplebook->GetPageCount(); ++i )
    {
        if ( m_simplebook->GetPage(i)->IsShown() )
            ++shown;
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), shown);
}

void SimplebookTestCase::RemoveHidesTransferredPage()
{
    CPPUNIT_ASSERT_EQUAL(0, m_simplebook->SetSelection(1));
    wxWindow* const removed = m_simplebook->GetPage(1);
    CPPUNIT_ASSERT(removed->IsShown());

    CPPUNIT_ASSERT(m_simplebook->RemovePage(1));
    CPPUNIT_ASSERT(!removed->IsShown());
    CPPUNIT_ASSERT(m_simplebook->FindPage(removed) == wxNOT_FOUND);

    delete removed;
}

#endif // wxUSE_BOOKCTRL

