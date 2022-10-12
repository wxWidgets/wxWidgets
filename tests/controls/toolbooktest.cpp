///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/toolbooktest.cpp
// Purpose:     wxToolbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TOOLBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/toolbook.h"
#include "wx/toolbar.h"
#include "bookctrlbasetest.h"

class ToolbookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    ToolbookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_toolbook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGING; }

    virtual void Realize() override { m_toolbook->GetToolBar()->Realize(); }

    CPPUNIT_TEST_SUITE( ToolbookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( ToolBar );
    CPPUNIT_TEST_SUITE_END();

    void ToolBar();

    wxToolbook *m_toolbook;

    wxDECLARE_NO_COPY_CLASS(ToolbookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ToolbookTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ToolbookTestCase, "ToolbookTestCase" );

void ToolbookTestCase::setUp()
{
    m_toolbook = new wxToolbook(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}

void ToolbookTestCase::tearDown()
{
    wxDELETE(m_toolbook);
}

void ToolbookTestCase::ToolBar()
{
    wxToolBar* toolbar = static_cast<wxToolBar*>(m_toolbook->GetToolBar());

    CPPUNIT_ASSERT(toolbar);
    CPPUNIT_ASSERT_EQUAL(3, toolbar->GetToolsCount());
}

#endif //wxUSE_TOOLBOOK
