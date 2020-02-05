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
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    wxString value;
    if ( !wxGetEnv("wxTEST_WEBVIEW_GTK2", &value) || value != "1" )
    {
        WARN("Skipping WebView tests known to fail with wxGTK 2, set "
             "wxTEST_WEBVIEW_GTK2=1 to force running them.");
        return;
    }
#endif

    m_browser -> Create(wxTheApp->GetTopWindow(), wxID_ANY);
    ENSURE_LOADED;

    SECTION("Title")
    {
        CHECK(m_browser->GetCurrentTitle() == "");

        //Test title after loading raw html
        m_browser->SetPage("<html><title>Title</title><body>Text</body></html>", "");
        ENSURE_LOADED;
        CHECK(m_browser->GetCurrentTitle() == "Title");

        //Test title after loading a url, we yield to let events process
        LoadUrl();
        CHECK(m_browser->GetCurrentTitle() == "");
    }

    SECTION("URL")
    {
        CHECK(m_browser->GetCurrentURL() == "about:blank");

        //After first loading about:blank the next in the sequence is about:
        LoadUrl();
        CHECK(m_browser->GetCurrentURL() == "about:");
    }

    SECTION("History")
    {
        LoadUrl(3);

        CHECK(m_browser->CanGoBack());
        CHECK(!m_browser->CanGoForward());

        m_browser->GoBack();
        ENSURE_LOADED;

        CHECK(m_browser->CanGoBack());
        CHECK(m_browser->CanGoForward());

        m_browser->GoBack();
        ENSURE_LOADED;
        m_browser->GoBack();
        ENSURE_LOADED;

        //We should now be at the start of the history
        CHECK(!m_browser->CanGoBack());
        CHECK(m_browser->CanGoForward());
    }

    SECTION("HistoryEnable")
    {
        LoadUrl();
        m_browser->EnableHistory(false);

        CHECK(!m_browser->CanGoForward());
        CHECK(!m_browser->CanGoBack());

        LoadUrl();

        CHECK(!m_browser->CanGoForward());
        CHECK(!m_browser->CanGoBack());
    }

    SECTION("HistoryClear")
    {
        LoadUrl(2);

        //Now we are in the 'middle' of the history
        m_browser->GoBack();
        ENSURE_LOADED;

        CHECK(m_browser->CanGoForward());
        CHECK(m_browser->CanGoBack());

        m_browser->ClearHistory();

        CHECK(!m_browser->CanGoForward());
        CHECK(!m_browser->CanGoBack());
    }

    SECTION("HistoryList")
    {
        LoadUrl(2);
        m_browser->GoBack();
        ENSURE_LOADED;

        CHECK(m_browser->GetBackwardHistory().size() == 1);
        CHECK(m_browser->GetForwardHistory().size() == 1);

        m_browser->LoadHistoryItem(m_browser->GetForwardHistory()[0]);
        ENSURE_LOADED;

        CHECK(!m_browser->CanGoForward());
        CHECK(m_browser->GetBackwardHistory().size() == 2);
    }

    SECTION("Editable")
    {
        CHECK(!m_browser->IsEditable());

        m_browser->SetEditable(true);

        CHECK(m_browser->IsEditable());

        m_browser->SetEditable(false);

        CHECK(!m_browser->IsEditable());
    }

    SECTION("Selection")
    {
        m_browser->SetPage("<html><body>Some <strong>strong</strong> text</body></html>", "");
        ENSURE_LOADED;
        CHECK(!m_browser->HasSelection());

        m_browser->SelectAll();

        CHECK(m_browser->HasSelection());
        CHECK(m_browser->GetSelectedText() == "Some strong text");

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
        CHECK(!m_browser->HasSelection());
    }

    SECTION("Zoom")
    {
        if(m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT))
        {
            m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
            CHECK(m_browser->GetZoomType() == wxWEBVIEW_ZOOM_TYPE_LAYOUT);

            m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
            CHECK(m_browser->GetZoom() == wxWEBVIEW_ZOOM_TINY);
        }

        //Reset the zoom level
        m_browser->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);

        if(m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT))
        {
            m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT);
            CHECK(m_browser->GetZoomType() == wxWEBVIEW_ZOOM_TYPE_TEXT);

            m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
            CHECK(m_browser->GetZoom() == wxWEBVIEW_ZOOM_TINY);
        }
    }

    SECTION("RunScript")
    {
        m_browser->
            SetPage("<html><head><script></script></head><body></body></html>", "");
        ENSURE_LOADED;

        wxString result;
    #if wxUSE_WEBVIEW_IE
        CHECK(wxWebViewIE::MSWSetModernEmulationLevel());

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

        CHECK(m_browser->RunScript("function f(){var person = new Object();person.name = 'Bar'; \
            person.lastName = 'Foo';return person;}f();", &result));
        CHECK(result == "{\"name\":\"Bar\",\"lastName\":\"Foo\"}");

        CHECK(m_browser->RunScript("function f(){ return [\"foo\", \"bar\"]; }f();", &result));
        CHECK(result == "[\"foo\",\"bar\"]");

        CHECK(m_browser->RunScript("function f(){var d = new Date('10/08/2017 21:30:40'); \
            var tzoffset = d.getTimezoneOffset() * 60000; return new Date(d.getTime() - tzoffset);}f();",
            &result));
        CHECK(result == "\"2017-10-08T21:30:40.000Z\"");

        CHECK(resetEmulationLevel.DoReset());
    #endif // wxUSE_WEBVIEW_IE

        CHECK(m_browser->RunScript("document.write(\"Hello World!\");"));
        CHECK(m_browser->GetPageText() == "Hello World!");

        CHECK(m_browser->RunScript("function f(a){return a;}f('Hello World!');", &result));
        CHECK(result == _("Hello World!"));

        CHECK(m_browser->RunScript("function f(a){return a;}f('a\\\'aa\\n\\rb\\tb\\\\ccc\\\"ddd\\b\\fx');", &result));
        CHECK(result == _("a\'aa\n\rb\tb\\ccc\"ddd\b\fx"));

        CHECK(m_browser->RunScript("function f(a){return a;}f(123);", &result));
        CHECK(wxAtoi(result) == 123);

        CHECK(m_browser->
            RunScript("function f(a){return a;}f(2.34);", &result));
        double value;
        result.ToDouble(&value);
        CHECK(value == 2.34);

        CHECK(m_browser->RunScript("function f(a){return a;}f(false);", &result));
        CHECK(result == "false");

        CHECK(m_browser->RunScript("function f(){var person = new Object();person.name = 'Foo'; \
            person.lastName = 'Bar';return person;}f();", &result));
        CHECK(result == "{\"name\":\"Foo\",\"lastName\":\"Bar\"}");

        CHECK(m_browser->RunScript("function f(){ return [\"foo\", \"bar\"]; }f();", &result));
        CHECK(result == "[\"foo\",\"bar\"]");

        CHECK(m_browser->RunScript("function f(){var person = new Object();}f();", &result));
        CHECK(result == "undefined");

        CHECK(m_browser->RunScript("function f(){return null;}f();", &result));
        CHECK(result == "null");

        result = "";
        CHECK(!m_browser->RunScript("int main() { return 0; }", &result));
        CHECK(!result);

        CHECK(m_browser->RunScript("function a() { return eval(\"function b() { \
            return eval(\\\"function c() { return eval(\\\\\\\"function d() { \
            return \\\\\\\\\\\\\\\"test\\\\\\\\\\\\\\\"; } d();\\\\\\\"); } \
            c();\\\"); } b();\"); } a();", &result));
        CHECK(result == "test");

        CHECK(m_browser->RunScript("function f(a){return a;}f(\"This is a backslash: \\\\\");",
            &result));
        CHECK(result == "This is a backslash: \\");

        CHECK(m_browser->RunScript("function f(){var d = new Date('10/08/2016 21:30:40'); \
            var tzoffset = d.getTimezoneOffset() * 60000; return new Date(d.getTime() - tzoffset);}f();",
            &result));
        CHECK(result == "\"2016-10-08T21:30:40.000Z\"");

        // Check for C++-style comments which used to be broken.
        CHECK(m_browser->RunScript("function f() {\n"
                                   "    // A C++ style comment\n"
                                   "    return 17;\n"
                                   "}f();", &result));
        CHECK(result == "17");

        // Check for errors too.
        CHECK(!m_browser->RunScript("syntax(error"));
        CHECK(!m_browser->RunScript("syntax(error", &result));
        CHECK(!m_browser->RunScript("x.y.z"));
    }

    SECTION("SetPage")
    {
        m_browser->SetPage("<html><body>text</body></html>", "");
        ENSURE_LOADED;
        CHECK(m_browser->GetPageText() == "text");

        m_browser->SetPage("<html><body>other text</body></html>", "");
        ENSURE_LOADED;
        CHECK(m_browser->GetPageText() == "other text");
    }
}

#endif //wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_WEBKIT2 || wxUSE_WEBVIEW_IE)
