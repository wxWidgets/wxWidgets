///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/webtest.cpp
// Purpose:     wxWebView unit test
// Author:      Steven Lamerton
// Created:     2011-07-08
// Copyright:   (c) 2011 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_WEBKIT2 || wxUSE_WEBVIEW_IE)

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/webview.h"
#include "asserthelper.h"
#if wxUSE_WEBVIEW_IE
    #include "wx/msw/webview_ie.h"
#endif

//Convenience macro
#define ENSURE_LOADED CHECK( m_loaded->WaitEvent() )

class WebViewTestCase
{
public:
    WebViewTestCase()
        : m_browser(wxWebView::New()),
          m_loaded(new EventCounter(m_browser, wxEVT_WEBVIEW_LOADED))
    {
    }

    ~WebViewTestCase()
    {
        delete m_loaded;
        delete m_browser;
    }

protected:
    void LoadUrl(int times = 1)
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

    wxWebView* const m_browser;
    EventCounter* const m_loaded;
};

TEST_CASE_METHOD(WebViewTestCase, "WebView", "[wxWebView]")
{
    m_browser -> Create(wxTheApp->GetTopWindow(), wxID_ANY);
    ENSURE_LOADED;

    SECTION("Title")
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

    SECTION("URL")
    {
        CPPUNIT_ASSERT_EQUAL("about:blank", m_browser->GetCurrentURL());

        //After first loading about:blank the next in the sequence is about:
        LoadUrl();
        CPPUNIT_ASSERT_EQUAL("about:", m_browser->GetCurrentURL());
    }

    SECTION("History")
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

    SECTION("HistoryEnable")
    {
        LoadUrl();
        m_browser->EnableHistory(false);

        CPPUNIT_ASSERT(!m_browser->CanGoForward());
        CPPUNIT_ASSERT(!m_browser->CanGoBack());

        LoadUrl();

        CPPUNIT_ASSERT(!m_browser->CanGoForward());
        CPPUNIT_ASSERT(!m_browser->CanGoBack());
    }

    SECTION("HistoryClear")
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

    SECTION("HistoryList")
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

    SECTION("Editable")
    {
        CPPUNIT_ASSERT(!m_browser->IsEditable());

        m_browser->SetEditable(true);

        CPPUNIT_ASSERT(m_browser->IsEditable());

        m_browser->SetEditable(false);

        CPPUNIT_ASSERT(!m_browser->IsEditable());
    }

    SECTION("Selection")
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

    SECTION("Zoom")
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

    SECTION("RunScript")
    {
        m_browser->
            SetPage("<html><head><script></script></head><body></body></html>", "");
        ENSURE_LOADED;

        wxString result;
    #if wxUSE_WEBVIEW_IE
        CPPUNIT_ASSERT(wxWebViewIE::MSWSetModernEmulationLevel());

        // Define a specialized scope guard ensuring that we reset the emulation
        // level to its default value even if any asserts below fail.
        class ResetEmulationLevel
        {
        public:
            ResetEmulationLevel()
            {
                m_reset = true;
            }

            bool DoReset()
            {
                m_reset = false;
                return wxWebViewIE::MSWSetModernEmulationLevel(false);
            }

            ~ResetEmulationLevel()
            {
                if ( m_reset )
                    DoReset();
            }

        private:
            bool m_reset;
        } resetEmulationLevel;

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){var person = new Object();person.name = 'Bar'; \
            person.lastName = 'Foo';return person;}f();", &result));
        CPPUNIT_ASSERT_EQUAL("{\"name\":\"Bar\",\"lastName\":\"Foo\"}", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){ return [\"foo\", \"bar\"]; }f();", &result));
        CPPUNIT_ASSERT_EQUAL("[\"foo\",\"bar\"]", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){var d = new Date('10/08/2017 21:30:40'); \
            var tzoffset = d.getTimezoneOffset() * 60000; return new Date(d.getTime() - tzoffset);}f();",
            &result));
        CPPUNIT_ASSERT_EQUAL("\"2017-10-08T21:30:40.000Z\"", result);

        CPPUNIT_ASSERT(resetEmulationLevel.DoReset());
    #endif // wxUSE_WEBVIEW_IE

        CPPUNIT_ASSERT(m_browser->RunScript("document.write(\"Hello World!\");"));
        CPPUNIT_ASSERT_EQUAL("Hello World!", m_browser->GetPageText());

        CPPUNIT_ASSERT(m_browser->RunScript("function f(a){return a;}f('Hello World!');", &result));
        CPPUNIT_ASSERT_EQUAL(_("Hello World!"), result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(a){return a;}f('a\\\'aa\\n\\rb\vb\\tb\\\\ccc\\\"ddd\\b\\fx');", &result));
        CPPUNIT_ASSERT_EQUAL(_("a\'aa\n\rb\vb\tb\\ccc\"ddd\b\fx"), result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(a){return a;}f(123);", &result));
        CPPUNIT_ASSERT_EQUAL(123, wxAtoi(result));

        CPPUNIT_ASSERT(m_browser->
            RunScript("function f(a){return a;}f(2.34);", &result));
        double value;
        result.ToDouble(&value);
        CPPUNIT_ASSERT_EQUAL(2.34, value);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(a){return a;}f(false);", &result));
        CPPUNIT_ASSERT_EQUAL("false", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){var person = new Object();person.name = 'Foo'; \
            person.lastName = 'Bar';return person;}f();", &result));
        CPPUNIT_ASSERT_EQUAL("{\"name\":\"Foo\",\"lastName\":\"Bar\"}", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){ return [\"foo\", \"bar\"]; }f();", &result));
        CPPUNIT_ASSERT_EQUAL("[\"foo\",\"bar\"]", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){var person = new Object();}f();", &result));
        CPPUNIT_ASSERT_EQUAL("undefined", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){return null;}f();", &result));
        CPPUNIT_ASSERT_EQUAL("null", result);

        result = "";
        CPPUNIT_ASSERT(!m_browser->RunScript("int main() { return 0; }", &result));
        CPPUNIT_ASSERT(!result);

        CPPUNIT_ASSERT(m_browser->RunScript("function a() { return eval(\"function b() { \
            return eval(\\\"function c() { return eval(\\\\\\\"function d() { \
            return \\\\\\\\\\\\\\\"test\\\\\\\\\\\\\\\"; } d();\\\\\\\"); } \
            c();\\\"); } b();\"); } a();", &result));
        CPPUNIT_ASSERT_EQUAL("test", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(a){return a;}f(\"This is a backslash: \\\\\");",
            &result));
        CPPUNIT_ASSERT_EQUAL("This is a backslash: \\", result);

        CPPUNIT_ASSERT(m_browser->RunScript("function f(){var d = new Date('10/08/2016 21:30:40'); \
            var tzoffset = d.getTimezoneOffset() * 60000; return new Date(d.getTime() - tzoffset);}f();",
            &result));
        CPPUNIT_ASSERT_EQUAL("\"2016-10-08T21:30:40.000Z\"", result);

        // Check for errors too.
        CPPUNIT_ASSERT(!m_browser->RunScript("syntax(error"));
        CPPUNIT_ASSERT(!m_browser->RunScript("syntax(error", &result));
        CPPUNIT_ASSERT(!m_browser->RunScript("x.y.z"));
    }

    SECTION("SetPage")
    {
        m_browser->SetPage("<html><body>text</body></html>", "");
        ENSURE_LOADED;
        CPPUNIT_ASSERT_EQUAL("text", m_browser->GetPageText());

        m_browser->SetPage("<html><body>other text</body></html>", "");
        ENSURE_LOADED;
        CPPUNIT_ASSERT_EQUAL("other text", m_browser->GetPageText());
    }
}

#endif //wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_WEBKIT2 || wxUSE_WEBVIEW_IE)
