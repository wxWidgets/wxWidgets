/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/webview_webkit.mm
// Purpose:     wxWebViewWebKit - embeddable web kit control,
//                             OS X implementation of web view component
// Author:      Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Modified by:
// Created:     2004-4-16
// Copyright:   (c) Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// https://developer.apple.com/documentation/webkit/wkwebview

#include "wx/osx/webview_webkit.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT && defined(__WXOSX__)

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/osx/private.h"
#include "wx/osx/core/cfref.h"
#include "wx/osx/private/available.h"
#include "wx/private/jsscriptwrapper.h"

#include "wx/hashmap.h"
#include "wx/filesys.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"

#include <WebKit/WebKit.h>
#include <Foundation/NSURLError.h>

// using native types to get compile errors and warnings

#define DEBUG_WEBKIT_SIZING 0

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewWebKit, wxWebView);

wxBEGIN_EVENT_TABLE(wxWebViewWebKit, wxControl)
wxEND_EVENT_TABLE()

@interface WXWKWebView: WKWebView
{
    wxWebViewWebKit* m_webView;
}

- (void)setWebView:(wxWebViewWebKit*)webView;

- (void)doCommandBySelector:(SEL)aSelector;

@end

@interface WebViewNavigationDelegate : NSObject<WKNavigationDelegate>
{
    wxWebViewWebKit* webKitWindow;
}

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow;

@end

@interface WebViewUIDelegate : NSObject<WKUIDelegate>
{
    wxWebViewWebKit* webKitWindow;
}

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow;

@end

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_13
@interface WebViewCustomProtocol : NSObject<WKURLSchemeHandler>
{
    wxWebViewHandler* m_handler;
}

- (id)initWithHandler:(wxWebViewHandler*) handler;

@end
#endif // macOS 10.13+

@interface WebViewScriptMessageHandler: NSObject<WKScriptMessageHandler>
{
    wxWebViewWebKit* webKitWindow;
}

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow;

@end

//-----------------------------------------------------------------------------
// wxWebViewFactoryWebKit
//-----------------------------------------------------------------------------

wxVersionInfo wxWebViewFactoryWebKit::GetVersionInfo()
{
    int verMaj, verMin, verMicro;
    wxGetOsVersion(&verMaj, &verMin, &verMicro);
    return wxVersionInfo("WKWebView", verMaj, verMin, verMicro);
}

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxWebViewWebKit::Create(wxWindow *parent,
                                 wxWindowID winID,
                                 const wxString& strURL,
                                 const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxString& name)
{
    DontCreatePeer();
    wxControl::Create(parent, winID, pos, size, style, wxDefaultValidator, name);

    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    WKWebViewConfiguration* webViewConfig = [[WKWebViewConfiguration alloc] init];

    if (!m_handlers.empty())
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_13
        if ( WX_IS_MACOS_AVAILABLE(10, 13) )
        {
            for (wxStringToWebHandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); it++)
            {
                [webViewConfig setURLSchemeHandler:[[WebViewCustomProtocol alloc] initWithHandler:it->second.get()]
                                            forURLScheme:wxCFStringRef(it->first).AsNSString()];
            }
        }
        else
#endif // macOS 10.13+
            wxLogDebug("Registering custom wxWebView handlers is not supported under macOS < 10.13");
    }

    m_webView = [[WXWKWebView alloc] initWithFrame:r configuration:webViewConfig];
    [(WXWKWebView*)m_webView setWebView:this];
    SetPeer(new wxWidgetCocoaImpl( this, m_webView ));

    MacPostControlCreate(pos, size);

    if (!m_customUserAgent.empty())
        SetUserAgent(m_customUserAgent);

    [m_webView setHidden:false];


    // Register event listener interfaces
    WebViewNavigationDelegate* navDelegate =
            [[WebViewNavigationDelegate alloc] initWithWxWindow: this];
    [m_webView addObserver:navDelegate forKeyPath:@"title" options:0 context:this];

    [m_webView setNavigationDelegate:navDelegate];

    m_navigationDelegate = navDelegate;

    WebViewUIDelegate* uiDelegate =
            [[WebViewUIDelegate alloc] initWithWxWindow: this];

    [m_webView setUIDelegate:uiDelegate];

    // Implement javascript fullscreen interface with user script and message handler
    AddUserScript("\
        document.__wxToggleFullscreen = function (elem) { \
            if (!document.__wxStylesAdded) { \
                function createClass(name,rules) { \
                    var style= document.createElement('style'); style.type = 'text/css'; \
                    document.getElementsByTagName('head')[0].appendChild(style); \
                    style.sheet.addRule(name, rules); \
                } \
                createClass(\"body.wxfullscreen\", \"padding: 0; margin: 0; height: 100%;\"); \
                createClass(\".wxfullscreen\", \"position: fixed; overflow: hidden; z-index: 1000; left: 0; top: 0; bottom: 0; right: 0;\"); \
                createClass(\".wxfullscreenelem\", \"width: 100% !important; height: 100% !important; padding-top: 0 !important;\"); \
                document.__wxStylesAdded = true; \
            } \
            if (elem) { \
                elem.classList.add(\"wxfullscreen\"); \
                elem.classList.add(\"wxfullscreenelem\"); \
                document.body.classList.add(\"wxfullscreen\"); \
            } else if (document.webkitFullscreenElement) { \
                document.webkitFullscreenElement.classList.remove(\"wxfullscreen\"); \
                document.webkitFullscreenElement.classList.remove(\"wxfullscreenelem\"); \
                document.body.classList.remove(\"wxfullscreen\"); \
            } \
            document.webkitFullscreenElement = elem; \
            window.webkit.messageHandlers.__wxfullscreen.postMessage((elem) ? 1: 0); \
            document.dispatchEvent(new Event('webkitfullscreenchange')); \
            if (document.onwebkitfullscreenchange) document.onwebkitfullscreenchange(); \
        }; \
        Element.prototype.webkitRequestFullscreen = function() {document.__wxToggleFullscreen(this);}; \
        document.webkitExitFullscreen = function() {document.__wxToggleFullscreen(undefined);}; \
        document.onwebkitfullscreenchange = null; \
        document.webkitFullscreenEnabled = true; \
    ");
    [m_webView.configuration.userContentController addScriptMessageHandler:
        [[WebViewScriptMessageHandler alloc] initWithWxWindow:this] name:@"__wxfullscreen"];

    m_UIDelegate = uiDelegate;

    LoadURL(strURL);
    return true;
}

wxWebViewWebKit::~wxWebViewWebKit()
{
    [m_webView removeObserver:m_navigationDelegate forKeyPath:@"title" context:this];
    [m_webView setNavigationDelegate: nil];
    [m_webView setUIDelegate: nil];

    [m_navigationDelegate release];
    [m_UIDelegate release];
}

// ----------------------------------------------------------------------------
// public methods
// ----------------------------------------------------------------------------

bool wxWebViewWebKit::CanGoBack() const
{
    if ( !m_webView )
        return false;

    return [m_webView canGoBack];
}

bool wxWebViewWebKit::CanGoForward() const
{
    if ( !m_webView )
        return false;

    return [m_webView canGoForward];
}

void wxWebViewWebKit::GoBack()
{
    if ( !m_webView )
        return;

    [m_webView goBack];
}

void wxWebViewWebKit::GoForward()
{
    if ( !m_webView )
        return;

    [m_webView goForward];
}

bool wxWebViewWebKit::IsBusy() const
{
    return m_webView.loading ? true : false;
}

void wxWebViewWebKit::Reload(wxWebViewReloadFlags flags)
{
    if ( !m_webView )
        return;

    if (flags & wxWEBVIEW_RELOAD_NO_CACHE)
    {
        [m_webView reloadFromOrigin];
    }
    else
    {
        [m_webView reload];
    }
}

void wxWebViewWebKit::Stop()
{
    if ( !m_webView )
        return;

    [m_webView stopLoading];
}

void wxWebViewWebKit::Print()
{

    // TODO: allow specifying the "show prompt" parameter in Print() ?
    bool showPrompt = true;

    if ( !m_webView )
        return;

    // As of macOS SDK 10.15 no offical printing API is available for WKWebView
    // Try if the undocumented printOperationWithPrintInfo: is available and use it
    // to create a printing operation
    // https://bugs.webkit.org/show_bug.cgi?id=151276
    SEL printSelector = @selector(printOperationWithPrintInfo:);
    if (![m_webView respondsToSelector:printSelector])
        printSelector = nil;

    if (!printSelector)
    {
        wxLogError(_("Printing is not supported by the system web control"));
        return;
    }

    NSPrintOperation* op = (NSPrintOperation*)[m_webView
                                               performSelector:printSelector
                                               withObject:[NSPrintInfo sharedPrintInfo]];
    if (!op)
    {
        wxLogError(_("Print operation could not be initialized"));
        return;
    }

    op.view.frame = m_webView.frame;
    if (showPrompt)
    {
        [op setShowsPrintPanel: showPrompt];
        // in my tests, the progress bar always freezes and it stops the whole
        // print operation. do not turn this to true unless there is a
        // workaround for the bug.
        [op setShowsProgressPanel: false];
    }
    // Print it.
    [op runOperationModalForWindow:m_webView.window
                          delegate:nil didRunSelector:nil contextInfo:nil];
}

void wxWebViewWebKit::SetEditable(bool WXUNUSED(enable))
{
}

bool wxWebViewWebKit::IsEditable() const
{
    return false;
}

bool wxWebViewWebKit::IsAccessToDevToolsEnabled() const
{
    // WebKit API available since macOS 10.11 and iOS 9.0
    WKPreferences* prefs = m_webView.configuration.preferences;
    SEL devToolsSelector = @selector(_developerExtrasEnabled);
    id val = nil;
    if ([prefs respondsToSelector:devToolsSelector])
         val = [prefs performSelector:devToolsSelector];
    return (val != nil);
}

void wxWebViewWebKit::EnableAccessToDevTools(bool enable)
{
    // WebKit API available since macOS 10.11 and iOS 9.0
    WKPreferences* prefs = m_webView.configuration.preferences;
    SEL devToolsSelector = @selector(_setDeveloperExtrasEnabled:);
    if ([prefs respondsToSelector:devToolsSelector])
        [prefs performSelector:devToolsSelector withObject:(id)enable];
}

bool wxWebViewWebKit::SetUserAgent(const wxString& userAgent)
{
    if (WX_IS_MACOS_AVAILABLE(10, 11))
    {
        if (m_webView)
            m_webView.customUserAgent = wxCFStringRef(userAgent).AsNSString();
        else
            m_customUserAgent = userAgent;

        return true;
    }
    else
        return false;
}

void wxWebViewWebKit::SetZoomType(wxWebViewZoomType zoomType)
{
    // there is only one supported zoom type at the moment so this setter
    // does nothing beyond checking sanity
    wxASSERT(zoomType == wxWEBVIEW_ZOOM_TYPE_LAYOUT);
}

wxWebViewZoomType wxWebViewWebKit::GetZoomType() const
{
    return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewWebKit::CanSetZoomType(wxWebViewZoomType type) const
{
    switch (type)
    {
        // for now that's the only one that is supported
        case wxWEBVIEW_ZOOM_TYPE_LAYOUT:
            return true;

        default:
            return false;
    }
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, void* clientData) const
{
    wxJSScriptWrapper wrapJS(javascript, wxJSScriptWrapper::JS_OUTPUT_STRING);

    // Start script execution
    [m_webView evaluateJavaScript:wxCFStringRef(wrapJS.GetWrappedCode()).AsNSString()
                completionHandler:^(id _Nullable obj, NSError * _Nullable error) {
        if (error)
        {
            SendScriptResult(clientData, false, wxCFStringRef(error.localizedDescription).AsString());
        }
        else
        {
            wxString scriptResult;
            if (obj)
                scriptResult = wxCFStringRef::AsString([NSString stringWithFormat:@"%@", obj]);
            wxString scriptOutput;
            bool success = wxJSScriptWrapper::ExtractOutput(scriptResult, &scriptOutput);

            SendScriptResult(clientData, success, scriptOutput);
        }
    }];
}

bool wxWebViewWebKit::AddScriptMessageHandler(const wxString& name)
{
    [m_webView.configuration.userContentController addScriptMessageHandler:
        [[WebViewScriptMessageHandler alloc] initWithWxWindow:this] name:wxCFStringRef(name).AsNSString()];
    // Make webkit message handler available under common name
    wxString js = wxString::Format("window.%s = window.webkit.messageHandlers.%s;",
            name, name);
    AddUserScript(js);
    RunScript(js);
    return true;
}

bool wxWebViewWebKit::RemoveScriptMessageHandler(const wxString& name)
{
    [m_webView.configuration.userContentController removeScriptMessageHandlerForName:wxCFStringRef(name).AsNSString()];
    return true;
}

bool wxWebViewWebKit::AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime)
{
    WKUserScript* userScript =
        [[WKUserScript alloc] initWithSource:wxCFStringRef(javascript).AsNSString()
            injectionTime:(injectionTime == wxWEBVIEW_INJECT_AT_DOCUMENT_START) ?
                WKUserScriptInjectionTimeAtDocumentStart : WKUserScriptInjectionTimeAtDocumentEnd
            forMainFrameOnly:NO];
    [m_webView.configuration.userContentController addUserScript:userScript];
    return true;
}

void wxWebViewWebKit::RemoveAllUserScripts()
{
    [m_webView.configuration.userContentController removeAllUserScripts];
}

void wxWebViewWebKit::LoadURL(const wxString& url)
{
    [m_webView loadRequest:[NSURLRequest requestWithURL:
            [NSURL URLWithString:wxCFStringRef(url).AsNSString()]]];
}

wxString wxWebViewWebKit::GetCurrentURL() const
{
    return wxCFStringRef::AsString(m_webView.URL.absoluteString);
}

wxString wxWebViewWebKit::GetCurrentTitle() const
{
    return wxCFStringRef::AsString(m_webView.title);
}

float wxWebViewWebKit::GetZoomFactor() const
{
    return m_webView.magnification;
}

void wxWebViewWebKit::SetZoomFactor(float zoom)
{
    m_webView.magnification = zoom;
}

void wxWebViewWebKit::DoSetPage(const wxString& src, const wxString& baseUrl)
{
   if ( !m_webView )
        return;

    [m_webView loadHTMLString:wxCFStringRef( src ).AsNSString()
                                  baseURL:[NSURL URLWithString:
                                    wxCFStringRef( baseUrl ).AsNSString()]];
}

void wxWebViewWebKit::EnableHistory(bool WXUNUSED(enable))
{
    if ( !m_webView )
        return;

    // TODO: implement
}

void wxWebViewWebKit::ClearHistory()
{
    // TODO: implement
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWebKit::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    WKBackForwardList* history = [m_webView backForwardList];
    int count = history.backList.count;
    for(int i = -count; i < 0; i++)
    {
        WKBackForwardListItem* item = [history itemAtIndex:i];
        wxString url = wxCFStringRef::AsString(item.URL.absoluteString);
        wxString title = wxCFStringRef::AsString([item title]);
        wxWebViewHistoryItem* wxitem = new wxWebViewHistoryItem(url, title);
        wxitem->m_histItem = item;
        wxSharedPtr<wxWebViewHistoryItem> itemptr(wxitem);
        backhist.push_back(itemptr);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWebKit::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    WKBackForwardList* history = [m_webView backForwardList];
    int count = history.forwardList.count;
    for(int i = 1; i <= count; i++)
    {
        WKBackForwardListItem* item = [history itemAtIndex:i];
        wxString url = wxCFStringRef::AsString(item.URL.absoluteString);
        wxString title = wxCFStringRef::AsString([item title]);
        wxWebViewHistoryItem* wxitem = new wxWebViewHistoryItem(url, title);
        wxitem->m_histItem = item;
        wxSharedPtr<wxWebViewHistoryItem> itemptr(wxitem);
        forwardhist.push_back(itemptr);
    }
    return forwardhist;
}

void wxWebViewWebKit::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    [m_webView goToBackForwardListItem:item->m_histItem];
}

void wxWebViewWebKit::Paste()
{
#if defined(__WXOSX_IPHONE__)
    wxWebView::Paste();
#else
    // The default (javascript) implementation presents the user with a popup
    // menu containing a single 'Paste' menu item.
    // Send this action to directly paste as expected.
    [[NSApplication sharedApplication] sendAction:@selector(paste:) to:nil from:m_webView];
#endif
}

bool wxWebViewWebKit::CanUndo() const
{
    return [[m_webView undoManager] canUndo];
}

bool wxWebViewWebKit::CanRedo() const
{
    return [[m_webView undoManager] canRedo];
}

void wxWebViewWebKit::Undo()
{
    [[m_webView undoManager] undo];
}

void wxWebViewWebKit::Redo()
{
    [[m_webView undoManager] redo];
}

void wxWebViewWebKit::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    m_handlers[handler->GetName()] = handler;
}

//------------------------------------------------------------
// Listener interfaces
//------------------------------------------------------------

// NB: I'm still tracking this down, but it appears the Cocoa window
// still has these events fired on it while the Carbon control is being
// destroyed. Therefore, we must be careful to check both the existence
// of the Carbon control and the event handler before firing events.

@implementation WXWKWebView

- (void)setWebView:(wxWebViewWebKit *)webView
{
    m_webView = webView;
}

#if !defined(__WXOSX_IPHONE__)
- (void)willOpenMenu:(NSMenu *)menu withEvent:(NSEvent *)event
{
    if (m_webView && !m_webView->IsContextMenuEnabled())
        [menu removeAllItems];
}

-(id)validRequestorForSendType:(NSString*)sendType returnType:(NSString*)returnType
{
    if (m_webView && !m_webView->IsContextMenuEnabled())
        return nil;
    else
        return [super validRequestorForSendType:sendType returnType:returnType];
}

- (BOOL)performKeyEquivalent:(NSEvent *)event
{
    if ([event modifierFlags] & NSCommandKeyMask)
    {
        switch ([event.characters characterAtIndex:0])
        {
            case 'a':
                [self selectAll:nil];
                return YES;
            case 'c':
                m_webView->Copy();
                return YES;
            case 'v':
                m_webView->Paste();
                return YES;
            case 'x':
                m_webView->Cut();
                return YES;
        }
    }

    return [super performKeyEquivalent:event];
}

- (void)doCommandBySelector:(SEL)aSelector
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (aSelector != @selector(noop:))
        [super doCommandBySelector:aSelector];
    else if (impl)
        impl->doCommandBySelector(aSelector, self, _cmd);
}

#endif // !defined(__WXOSX_IPHONE__)

@end

@implementation WebViewNavigationDelegate

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow
{
    if (self = [super init])
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary*)change context:(void *)context
{
    if (context == webKitWindow)
    {
        wxWebViewEvent event(wxEVT_WEBVIEW_TITLE_CHANGED,
                             webKitWindow->GetId(),
                             webKitWindow->GetCurrentURL(),
                             "");

        event.SetString(webKitWindow->GetCurrentTitle());

        webKitWindow->ProcessWindowEvent(event);
    }
    else
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (void)webView:(WKWebView *)webView didCommitNavigation:(WKNavigation *)navigation
{
    if (webKitWindow){
        NSString *url = webView.URL.absoluteString;
        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATED,
                             webKitWindow->GetId(),
                             wxCFStringRef::AsString( url ),
                             "");

        if (webKitWindow && webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent(event);
    }
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation
{
    if (webKitWindow){
        NSString *url = webView.URL.absoluteString;

        wxWebViewEvent event(wxEVT_WEBVIEW_LOADED,
                             webKitWindow->GetId(),
                             wxCFStringRef::AsString( url ),
                             "");

        if (webKitWindow && webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent(event);
    }
}

wxString nsErrorToWxHtmlError(NSError* error, wxWebViewNavigationError* out)
{
    *out = wxWEBVIEW_NAV_ERR_OTHER;

    if ([[error domain] isEqualToString:NSURLErrorDomain])
    {
        switch ([error code])
        {
            case NSURLErrorCannotFindHost:
            case NSURLErrorFileDoesNotExist:
            case NSURLErrorRedirectToNonExistentLocation:
                *out = wxWEBVIEW_NAV_ERR_NOT_FOUND;
                break;

            case NSURLErrorResourceUnavailable:
            case NSURLErrorHTTPTooManyRedirects:
            case NSURLErrorDataLengthExceedsMaximum:
            case NSURLErrorBadURL:
            case NSURLErrorFileIsDirectory:
                *out = wxWEBVIEW_NAV_ERR_REQUEST;
                break;

            case NSURLErrorTimedOut:
            case NSURLErrorDNSLookupFailed:
            case NSURLErrorNetworkConnectionLost:
            case NSURLErrorCannotConnectToHost:
            case NSURLErrorNotConnectedToInternet:
            //case NSURLErrorInternationalRoamingOff:
            //case NSURLErrorCallIsActive:
            //case NSURLErrorDataNotAllowed:
                *out = wxWEBVIEW_NAV_ERR_CONNECTION;
                break;

            case NSURLErrorCancelled:
            case NSURLErrorUserCancelledAuthentication:
                *out = wxWEBVIEW_NAV_ERR_USER_CANCELLED;
                break;

            case NSURLErrorCannotDecodeRawData:
            case NSURLErrorCannotDecodeContentData:
            case NSURLErrorCannotParseResponse:
            case NSURLErrorBadServerResponse:
                *out = wxWEBVIEW_NAV_ERR_REQUEST;
                break;

            case NSURLErrorUserAuthenticationRequired:
            case NSURLErrorSecureConnectionFailed:
            case NSURLErrorClientCertificateRequired:
                *out = wxWEBVIEW_NAV_ERR_AUTH;
                break;

            case NSURLErrorNoPermissionsToReadFile:
                *out = wxWEBVIEW_NAV_ERR_SECURITY;
                break;

            case NSURLErrorServerCertificateHasBadDate:
            case NSURLErrorServerCertificateUntrusted:
            case NSURLErrorServerCertificateHasUnknownRoot:
            case NSURLErrorServerCertificateNotYetValid:
            case NSURLErrorClientCertificateRejected:
                *out = wxWEBVIEW_NAV_ERR_CERTIFICATE;
                break;
        }
    }

    wxString message = wxCFStringRef::AsString([error localizedDescription]);
    NSString* detail = [error localizedFailureReason];
    if (detail != NULL)
    {
        message = message + " (" + wxCFStringRef::AsString(detail) + ")";
    }
    return message;
}

- (void)webView:(WKWebView *)webView
    didFailNavigation:(WKNavigation *)navigation
            withError:(NSError *)error;
{
    if (webKitWindow){
        NSString *url = webView.URL.absoluteString;

        wxWebViewNavigationError type;
        wxString description = nsErrorToWxHtmlError(error, &type);
        wxWebViewEvent event(wxEVT_WEBVIEW_ERROR,
                             webKitWindow->GetId(),
                             wxCFStringRef::AsString( url ),
                             wxEmptyString);
        event.SetString(description);
        event.SetInt(type);

        if (webKitWindow && webKitWindow->GetEventHandler())
        {
            webKitWindow->GetEventHandler()->ProcessEvent(event);
        }
    }
}

- (void)webView:(WKWebView *)webView
    didFailProvisionalNavigation:(WKNavigation *)navigation
                       withError:(NSError *)error;
{
    if (webKitWindow){
        NSString *url = webView.URL.absoluteString;

        wxWebViewNavigationError type;
        wxString description = nsErrorToWxHtmlError(error, &type);
        wxWebViewEvent event(wxEVT_WEBVIEW_ERROR,
                             webKitWindow->GetId(),
                             wxCFStringRef::AsString( url ),
                             wxEmptyString);
        event.SetString(description);
        event.SetInt(type);

        if (webKitWindow && webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent(event);
    }
}

- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction
    decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler
{
    NSString *url = [[navigationAction.request URL] absoluteString];
    wxWebViewNavigationActionFlags navFlags =
        navigationAction.navigationType == WKNavigationTypeLinkActivated ?
        wxWEBVIEW_NAV_ACTION_USER :
        wxWEBVIEW_NAV_ACTION_OTHER;

    wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING,
                         webKitWindow->GetId(),
                         wxCFStringRef::AsString( url ), "", navFlags);

    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(event);

    decisionHandler(event.IsAllowed() ? WKNavigationActionPolicyAllow : WKNavigationActionPolicyCancel);
}

@end

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_13
@implementation WebViewCustomProtocol

- (id)initWithHandler:(wxWebViewHandler *)handler
{
    m_handler = handler;
    return self;
}

- (void)webView:(WKWebView *)webView startURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask
WX_API_AVAILABLE_MACOS(10, 13)
{
    NSURLRequest *request = urlSchemeTask.request;
    NSString* path = [[request URL] absoluteString];

    wxString wxpath = wxCFStringRef::AsString(path);

    wxFSFile* file = m_handler->GetFile(wxpath);

    if (!file)
    {
        NSError *error = [[NSError alloc] initWithDomain:NSURLErrorDomain
                            code:NSURLErrorFileDoesNotExist
                            userInfo:nil];

        [urlSchemeTask didFailWithError:error];

        [error release];

        return;
    }

    size_t length = file->GetStream()->GetLength();


    NSURLResponse *response =  [[NSURLResponse alloc] initWithURL:[request URL]
                               MIMEType:wxCFStringRef(file->GetMimeType()).AsNSString()
                               expectedContentLength:length
                               textEncodingName:nil];

    //Load the data, we malloc it so it is tidied up properly
    void* buffer = malloc(length);
    file->GetStream()->Read(buffer, length);
    NSData *data = [[NSData alloc] initWithBytesNoCopy:buffer length:length];

    //Set the data
    [urlSchemeTask didReceiveResponse:response];
    [urlSchemeTask didReceiveData:data];

    //Notify that we have finished
    [urlSchemeTask didFinish];

    [data release];

    [response release];
}

- (void)webView:(WKWebView *)webView stopURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask
WX_API_AVAILABLE_MACOS(10, 13)
{

}

@end
#endif // macOS 10.13+


@implementation WebViewUIDelegate

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow
{
    if (self = [super init])
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (WKWebView *)webView:(WKWebView *)webView
    createWebViewWithConfiguration:(WKWebViewConfiguration *)configuration
   forNavigationAction:(WKNavigationAction *)navigationAction
        windowFeatures:(WKWindowFeatures *)windowFeatures
{
    wxWebViewNavigationActionFlags navFlags =
        navigationAction.navigationType == WKNavigationTypeLinkActivated ?
        wxWEBVIEW_NAV_ACTION_USER :
        wxWEBVIEW_NAV_ACTION_OTHER;

    wxWebViewEvent event(wxEVT_WEBVIEW_NEWWINDOW,
                         webKitWindow->GetId(),
                         wxCFStringRef::AsString( navigationAction.request.URL.absoluteString ),
                         "", navFlags);

    if (webKitWindow && webKitWindow->GetEventHandler())
        webKitWindow->GetEventHandler()->ProcessEvent(event);

    return nil;
}

- (void)webView:(WKWebView *)webView runJavaScriptAlertPanelWithMessage:(NSString *)message
    initiatedByFrame:(WKFrameInfo *)frame
    completionHandler:(void (^)())completionHandler
{
    wxMessageDialog dlg(webKitWindow->GetParent(), wxCFStringRef::AsString(message));
    dlg.ShowModal();
    completionHandler();
}

- (void)webView:(WKWebView *)webView runJavaScriptConfirmPanelWithMessage:(NSString *)message
    initiatedByFrame:(WKFrameInfo *)frame
    completionHandler:(void (^)(BOOL))completionHandler
{
    wxMessageDialog dlg(webKitWindow->GetParent(), wxCFStringRef::AsString(message),
                        wxMessageBoxCaptionStr, wxOK|wxCANCEL|wxCENTRE);
    completionHandler(dlg.ShowModal() == wxID_OK);
}

- (void)webView:(WKWebView *)webView runJavaScriptTextInputPanelWithPrompt:(NSString *)prompt
    defaultText:(NSString *)defaultText
    initiatedByFrame:(WKFrameInfo *)frame
    completionHandler:(void (^)(NSString * _Nullable))completionHandler
{
    wxString resultText;
    wxTextEntryDialog dlg(webKitWindow->GetParent(), wxCFStringRef::AsString(prompt),
                          wxGetTextFromUserPromptStr, wxCFStringRef::AsString(defaultText));
    if (dlg.ShowModal() == wxID_OK)
        resultText = dlg.GetValue();

    completionHandler(wxCFStringRef(resultText).AsNSString());
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
- (void)webView:(WKWebView *)webView runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters
    initiatedByFrame:(WKFrameInfo *)frame
    completionHandler:(void (^)(NSArray<NSURL *> * _Nullable))completionHandler
WX_API_AVAILABLE_MACOS(10, 12)
{
    long style = wxFD_OPEN | wxFD_FILE_MUST_EXIST;
    if (parameters.allowsMultipleSelection)
        style |= wxFD_MULTIPLE;

    wxFileDialog dlg(webKitWindow->GetParent(), wxFileSelectorPromptStr, "", "",
                     wxFileSelectorDefaultWildcardStr, style);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString filePaths;
        dlg.GetPaths(filePaths);
        NSMutableArray* urls = [[NSMutableArray alloc] init];
        for (wxArrayString::iterator it = filePaths.begin(); it != filePaths.end(); it++)
            [urls addObject:[NSURL fileURLWithPath:wxCFStringRef(*it).AsNSString()]];
        completionHandler(urls);
        [urls release];
    }
    else
        completionHandler(nil);
}
#endif // __MAC_OS_X_VERSION_MAX_ALLOWED

// The following WKUIDelegateMethods are undocumented as of macOS SDK 11.0,
// but are documented in the WebKit cocoa interface headers:
// https://github.com/WebKit/WebKit/blob/main/Source/WebKit/UIProcess/API/Cocoa/WKUIDelegatePrivate.h

- (void)_webView:(WKWebView *)webView printFrame:(WKFrameInfo*)frame
{
    webKitWindow->Print();
}

@end

@implementation WebViewScriptMessageHandler

- (id)initWithWxWindow: (wxWebViewWebKit*)inWindow
{
    if (self = [super init])
    {
        webKitWindow = inWindow;    // non retained
    }
    return self;
}

- (void)userContentController:(nonnull WKUserContentController *)userContentController
      didReceiveScriptMessage:(nonnull WKScriptMessage *)message
{
    // Handle internal fullscreen message independent of user message handlers
    if ([message.name isEqualToString:@"__wxfullscreen"])
    {
        wxWebViewEvent event(wxEVT_WEBVIEW_FULLSCREEN_CHANGED, webKitWindow->GetId(),
            webKitWindow->GetCurrentURL(), wxString());
        event.SetEventObject(webKitWindow);
        event.SetInt(((NSNumber*)message.body).intValue);
        webKitWindow->HandleWindowEvent(event);
        return;
    }

    wxWebViewEvent event(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED,
                         webKitWindow->GetId(),
                         webKitWindow->GetCurrentURL(),
                         "",
                         wxWEBVIEW_NAV_ACTION_NONE,
                         wxCFStringRef::AsString(message.name));
    if ([message.body isKindOfClass:NSString.class])
        event.SetString(wxCFStringRef::AsString(message.body));
    else if ([message.body isKindOfClass:NSNumber.class])
        event.SetString(wxCFStringRef::AsString(((NSNumber*)message.body).stringValue));
    else if ([message.body isKindOfClass:NSDate.class])
        event.SetString(wxCFStringRef::AsString(((NSDate*)message.body).description));
    else if ([message.body isKindOfClass:NSNull.class])
        event.SetString("null");
    else if ([message.body isKindOfClass:NSDictionary.class] || [message.body isKindOfClass:NSArray.class])
    {
        NSError* error = nil;
        NSData* jsonData = [NSJSONSerialization dataWithJSONObject:message.body options:0 error:&error];
        NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        event.SetString(wxCFStringRef::AsString(jsonString));
    }

    webKitWindow->ProcessWindowEvent(event);
}

@end

#endif //wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT
