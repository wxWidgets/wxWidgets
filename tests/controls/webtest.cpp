///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/webtest.cpp
// Purpose:     wxWebView unit test
// Author:      Steven Lamerton
// Created:     2011-07-08
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_WEB

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/webview.h"
#include "asserthelper.h"

class WebTestCase : public CppUnit::TestCase
{
public:
    WebTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( WebTestCase );
        CPPUNIT_TEST( Title );
        CPPUNIT_TEST( Url );
        CPPUNIT_TEST( History );
        CPPUNIT_TEST( HistoryEnable );
        CPPUNIT_TEST( HistoryClear );
        CPPUNIT_TEST( HistoryList );
    CPPUNIT_TEST_SUITE_END();

    void Title();
    void Url();
    void History();
    void HistoryEnable();
    void HistoryClear();
    void HistoryList();
    void LoadUrl(const wxString& url, int times = 1);

    wxWebView* m_browser;

    DECLARE_NO_COPY_CLASS(WebTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WebTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WebTestCase, "WebTestCase" );

void WebTestCase::setUp()
{
    m_browser = wxWebView::New(wxTheApp->GetTopWindow(), wxID_ANY);
}

void WebTestCase::tearDown()
{
    wxDELETE(m_browser);
}

void WebTestCase::LoadUrl(const wxString& url, int times)
{
    for(int i = 0; i < times; i++)
    {
        m_browser->LoadUrl(url);
        wxYield();
    }
}

void WebTestCase::Title()
{
    CPPUNIT_ASSERT_EQUAL("", m_browser->GetCurrentTitle());

    //Test title after loading raw html
    m_browser->SetPage("<html><title>Title</title></html>", "");
    CPPUNIT_ASSERT_EQUAL("Title", m_browser->GetCurrentTitle());

    //Test title after loading a url, we yield to let events process
    LoadUrl("about:blank");
    CPPUNIT_ASSERT_EQUAL("", m_browser->GetCurrentTitle());
}

void WebTestCase::Url()
{
    CPPUNIT_ASSERT_EQUAL("", m_browser->GetCurrentURL());

    LoadUrl("about:blank");
    CPPUNIT_ASSERT_EQUAL("about:blank", m_browser->GetCurrentURL());
}

void WebTestCase::History()
{
    //We use about:blank to remove the need for a network connection
    LoadUrl("about:blank", 3);

    CPPUNIT_ASSERT(m_browser->CanGoBack());
    CPPUNIT_ASSERT(!m_browser->CanGoForward());

    m_browser->GoBack();

    CPPUNIT_ASSERT(m_browser->CanGoBack());
    CPPUNIT_ASSERT(m_browser->CanGoForward());

    m_browser->GoBack();
    m_browser->GoBack();

    //We should now be at the start of the history
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
    CPPUNIT_ASSERT(m_browser->CanGoForward());
}

void WebTestCase::HistoryEnable()
{
    LoadUrl("about:blank");
    m_browser->EnableHistory(false);

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());

    LoadUrl("about:blank");

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
}

void WebTestCase::HistoryClear()
{
    LoadUrl("about:blank", 2);

    //Now we are in the 'middle' of the history
    m_browser->GoBack();

    CPPUNIT_ASSERT(m_browser->CanGoForward());
    CPPUNIT_ASSERT(m_browser->CanGoBack());

    m_browser->ClearHistory();

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
}

void WebTestCase::HistoryList()
{
    LoadUrl("about:blank", 2);
    m_browser->GoBack();

    CPPUNIT_ASSERT_EQUAL(1, m_browser->GetBackwardHistory().size());
    CPPUNIT_ASSERT_EQUAL(1, m_browser->GetForwardHistory().size());

    m_browser->LoadHistoryItem(m_browser->GetForwardHistory()[0]);

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT_EQUAL(2, m_browser->GetBackwardHistory().size());
}

#endif //wxUSE_WEB
