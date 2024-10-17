/////////////////////////////////////////////////////////////////////////////
// Name:        webview_chromium_impl.h
// Purpose:     Documentation for wxWebViewChromium implementation helpers
// Author:      Vadim Zeitlin
// Created:     2024-02-17
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Helper class to simplify creating custom CefClient subclasses.

    Please note that the application including this header must set up the
    compiler options to include the CEF directory as one of the include paths,
    as this header -- unlike wx/webview_chromium.h -- includes CEF headers.

    Here is a simple example of using this class to customize processing the
    messages received from the other CEF processes:

    @code
    #include <wx/webview_chromium_impl.h>

    struct CustomClient : wxDelegatingCefClient
    {
        using wxDelegatingCefClient::wxDelegatingCefClient;

        static CefClient* Create(CefClient* client, void* WXUNUSED(data)
        {
            return new CustomClient(client);
        }

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) override
        {
            // Handle the message here.

            return true; // or false if not handled
        }
    };

    void MyFunction()
    {
        wxWebViewConfiguration config = wxWebView::NewConfiguration(wxWebViewBackendChromium);

        auto configChrome =
            static_cast<wxWebViewConfigurationChromium*>(config.GetNativeConfiguration());
        configChrome->m_clientCreate = &CustomClient::Create;

        auto webview = new wxWebViewChromium(config);
        if ( !webview->Create(this, wxID_ANY, url) )
        {
            // Handle error.
        }
    }
    @endcode

    @since 3.3.0
    @category{webview}
 */
class wxDelegatingCefClient : CefClient
{
};
