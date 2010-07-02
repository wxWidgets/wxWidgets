///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/notebooktest.cpp
// Purpose:     wxNotebook unit test
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
    #include "wx/notebook.h"
#endif // WX_PRECOMP

#include "bookctrlbasetest.h"

class NotebookTestCase : public BookCtrlBaseTestCase
{
public:
    NotebookTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxBookCtrlBase *GetBase() const { return m_notebook; }

    virtual wxEventType GetChangedEvent() const
    { return wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const
    { return wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING; }


    CPPUNIT_TEST_SUITE( NotebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxNotebook *m_notebook;

    DECLARE_NO_COPY_CLASS(NotebookTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( NotebookTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NotebookTestCase, "NotebookTestCase" );

void NotebookTestCase::setUp()
{
    m_notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}

void NotebookTestCase::tearDown()
{
    wxDELETE(m_notebook);
}
