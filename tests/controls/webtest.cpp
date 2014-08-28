///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/webtest.cpp
// Purpose:     wxWebView unit test
// Author:      Steven Lamerton
// Created:     2011-07-08
// Copyright:   (c) 2011 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_IE)

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
        CPPUNIT_TEST( Editable );
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( Zoom );
        CPPUNIT_TEST( RunScript );
        CPPUNIT_TEST( SetPage );
    CPPUNIT_TEST_SUITE_END();

    void Title();
    void Url();
    void History();
    void HistoryEnable();
    void HistoryClear();
    void HistoryList();
    void Editable();
    void Selection();
    void Zoom();
    void RunScript();
    void SetPage();
    void LoadUrl(int times = 1);

    wxWebView* m_browser;
    EventCounter* m_loaded;

    DECLARE_NO_COPY_CLASS(WebTestCase)
};

//Convenience macro
#define ENSURE_LOADED WX_ASSERT_EVENT_OCCURS((*m_loaded), 1)

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WebTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WebTestCase, "WebTestCase" );

void WebTestCase::setUp()
{
    m_browser = wxWebView::New(wxTheApp->GetTopWindow(), wxID_ANY);

    m_loaded = new EventCounter(m_browser, wxEVT_WEBVIEW_LOADED);
    m_browser->LoadURL("about:blank");
    ENSURE_LOADED;
}

void WebTestCase::tearDown()
{
    wxDELETE(m_loaded);
    wxDELETE(m_browser);
}

void WebTestCase::LoadUrl(int times)
{
    //We alternate between urls as otherwise webkit merges them in the history
    //we use about and about blank to avoid the need for a network connection
    for(int i = 0; i < times; i++)
    {
        if(i % 2 == 1)
            m_browser->LoadURL("about:blank");
        else
            m_browser->LoadURL("about:");
        ENSURE_LOADED;
    }
}

void WebTestCase::Title()
{
    CPPUNIT_ASSERT_EQUAL("", m_browser->GetCurrentTitle());

    //Test title after loading raw html
    m_browser->SetPage("<html><title>Title</title><body>Text</body></html>", "");
    ENSURE_LOADED;
    CPPUNIT_ASSERT_EQUAL("Title", m_browser->GetCurrentTitle());

    //Test title after loading a url, we yield to let events process
    LoadUrl();
    CPPUNIT_ASSERT_EQUAL("", m_browser->GetCurrentTitle());
}

void WebTestCase::Url()
{
    CPPUNIT_ASSERT_EQUAL("about:blank", m_browser->GetCurrentURL());

    //After first loading about:blank the next in the sequence is about:
    LoadUrl();
    CPPUNIT_ASSERT_EQUAL("about:", m_browser->GetCurrentURL());
}

void WebTestCase::History()
{
    LoadUrl(3);

    CPPUNIT_ASSERT(m_browser->CanGoBack());
    CPPUNIT_ASSERT(!m_browser->CanGoForward());

    m_browser->GoBack();
    ENSURE_LOADED;

    CPPUNIT_ASSERT(m_browser->CanGoBack());
    CPPUNIT_ASSERT(m_browser->CanGoForward());

    m_browser->GoBack();
    ENSURE_LOADED;
    m_browser->GoBack();
    ENSURE_LOADED;

    //We should now be at the start of the history
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
    CPPUNIT_ASSERT(m_browser->CanGoForward());
}

void WebTestCase::HistoryEnable()
{
    LoadUrl();
    m_browser->EnableHistory(false);

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());

    LoadUrl();

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
}

void WebTestCase::HistoryClear()
{
    LoadUrl(2);

    //Now we are in the 'middle' of the history
    m_browser->GoBack();
    ENSURE_LOADED;

    CPPUNIT_ASSERT(m_browser->CanGoForward());
    CPPUNIT_ASSERT(m_browser->CanGoBack());

    m_browser->ClearHistory();

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT(!m_browser->CanGoBack());
}

void WebTestCase::HistoryList()
{
    LoadUrl(2);
    m_browser->GoBack();
    ENSURE_LOADED;

    CPPUNIT_ASSERT_EQUAL(1, m_browser->GetBackwardHistory().size());
    CPPUNIT_ASSERT_EQUAL(1, m_browser->GetForwardHistory().size());

    m_browser->LoadHistoryItem(m_browser->GetForwardHistory()[0]);
    ENSURE_LOADED;

    CPPUNIT_ASSERT(!m_browser->CanGoForward());
    CPPUNIT_ASSERT_EQUAL(2, m_browser->GetBackwardHistory().size());
}

void WebTestCase::Editable()
{
    CPPUNIT_ASSERT(!m_browser->IsEditable());

    m_browser->SetEditable(true);

    CPPUNIT_ASSERT(m_browser->IsEditable());

    m_browser->SetEditable(false);

    CPPUNIT_ASSERT(!m_browser->IsEditable());
}

void WebTestCase::Selection()
{
    m_browser->SetPage("<html><body>Some <strong>strong</strong> text</body></html>", "");
    ENSURE_LOADED;
    CPPUNIT_ASSERT(!m_browser->HasSelection());

    m_browser->SelectAll();

    CPPUNIT_ASSERT(m_browser->HasSelection());
    CPPUNIT_ASSERT_EQUAL("Some strong text", m_browser->GetSelectedText());

    // The web engine doesn't necessarily represent the HTML in the same way as
    // we used above, e.g. IE uses upper case for all the tags while WebKit
    // under OS X inserts plenty of its own <span> tags, so don't test for
    // equality and just check that the source contains things we'd expect it
    // to.
    const wxString selSource = m_browser->GetSelectedSource();
    WX_ASSERT_MESSAGE
    (
        ("Unexpected selection source: \"%s\"", selSource),
        selSource.Lower().Matches("*some*<strong*strong</strong>*text*")
    );

    m_browser->ClearSelection();
    CPPUNIT_ASSERT(!m_browser->HasSelection());
}

void WebTestCase::Zoom()
{
    if(m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT))
    {
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
        CPPUNIT_ASSERT_EQUAL(wxWEBVIEW_ZOOM_TYPE_LAYOUT, m_browser->GetZoomType());

        m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
        CPPUNIT_ASSERT_EQUAL(wxWEBVIEW_ZOOM_TINY, m_browser->GetZoom());
    }

    //Reset the zoom level
    m_browser->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);

    if(m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT))
    {
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT);
        CPPUNIT_ASSERT_EQUAL(wxWEBVIEW_ZOOM_TYPE_TEXT, m_browser->GetZoomType());

        m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
        CPPUNIT_ASSERT_EQUAL(wxWEBVIEW_ZOOM_TINY, m_browser->GetZoom());
    }
}

void WebTestCase::RunScript()
{
    m_browser->RunScript("document.write(\"Hello World!\");");
    CPPUNIT_ASSERT_EQUAL("Hello World!", m_browser->GetPageText());
}

void WebTestCase::SetPage()
{
    m_browser->SetPage("<html><body>text</body></html>", "");
    ENSURE_LOADED;
    CPPUNIT_ASSERT_EQUAL("text", m_browser->GetPageText());

    m_browser->SetPage("<html><body>other text</body></html>", "");
    ENSURE_LOADED;
    CPPUNIT_ASSERT_EQUAL("other text", m_browser->GetPageText());
}

#endif //wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_IE)
