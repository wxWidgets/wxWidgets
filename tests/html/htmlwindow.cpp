///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlwindow.cpp
// Purpose:     wxHtmlWindow tests
// Author:      Vaclav Slavik
// Created:     2008-10-15
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vaclav Slavik <vslavik@fastmail.fm>
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
#endif // WX_PRECOMP

#include "wx/html/htmlwin.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HtmlWindowTestCase : public CppUnit::TestCase
{
public:
    HtmlWindowTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( HtmlWindowTestCase );
        CPPUNIT_TEST( SelectionToText );
    CPPUNIT_TEST_SUITE_END();

    void SelectionToText();

    wxHtmlWindow *m_win;

    DECLARE_NO_COPY_CLASS(HtmlWindowTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HtmlWindowTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HtmlWindowTestCase, "HtmlWindowTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void HtmlWindowTestCase::setUp()
{
    m_win = new wxHtmlWindow(wxTheApp->GetTopWindow(), wxID_ANY);
}

void HtmlWindowTestCase::tearDown()
{
    m_win->Destroy();
    m_win = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

static const char *TEST_MARKUP =
    "<html><body>"
    "  Title<p>"
    "  A longer line<br>"
    "  and the last line."
    "</body></html>";

static const char *TEST_PLAIN_TEXT =
    "Title\nA longer line\nand the last line.";

void HtmlWindowTestCase::SelectionToText()
{
    m_win->SetPage(TEST_MARKUP);
    m_win->SelectAll();

    CPPUNIT_ASSERT_EQUAL( TEST_PLAIN_TEXT, m_win->SelectionToText() );
}
