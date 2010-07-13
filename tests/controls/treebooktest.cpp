///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treebooktest.cpp
// Purpose:     wxtreebook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxBookCtrlBase *GetBase() const { return m_treebook; }

    virtual wxEventType GetChangedEvent() const
    { return wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const
    { return wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING; }

    CPPUNIT_TEST_SUITE( TreebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( Image );
    CPPUNIT_TEST_SUITE_END();

    wxTreebook *m_treebook;

    DECLARE_NO_COPY_CLASS(TreebookTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreebookTestCase );

// also include in it's own registry so that these tests can be run alone
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
