/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/webkit.mm
// Purpose:     wxWebKitCtrl - embeddable web kit control
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/splitter.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_WEBKIT

#include "wx/osx/private.h"
#include "wx/html/webkit.h"

// QuartzCore used for making webpage screenshots for AirPrint, so don't import unless needed
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_4_2
    #import <QuartzCore/QuartzCore.h>
#endif


// Default text size and upper/lower boundaries
enum {
    WX_IPHONE_WEBKIT_TEXTSIZE_DEFAULT = 100,
    WX_IPHONE_WEBKIT_TEXTSIZE_MAX = 160,
    WX_IPHONE_WEBKIT_TEXTSIZE_MIN = 50,
    WX_IPHONE_WEBKIT_TEXTSIZE_STEP = 5
};

extern WXDLLEXPORT_DATA(const char) wxWebKitCtrlNameStr[] = "webkitctrl";

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWebKitCtrl, wxControl)

BEGIN_EVENT_TABLE(wxWebKitCtrl, wxControl)
END_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxWebKit Events (from src/html/htmlctrl/webkit/webkit.mm)
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxWebKitStateChangedEvent, wxCommandEvent )

wxDEFINE_EVENT( wxEVT_WEBKIT_STATE_CHANGED, wxWebKitStateChangedEvent );

wxWebKitStateChangedEvent::wxWebKitStateChangedEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_STATE_CHANGED);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
}

IMPLEMENT_DYNAMIC_CLASS( wxWebKitBeforeLoadEvent, wxCommandEvent )

wxDEFINE_EVENT( wxEVT_WEBKIT_BEFORE_LOAD, wxWebKitBeforeLoadEvent );

wxWebKitBeforeLoadEvent::wxWebKitBeforeLoadEvent( wxWindow* win )
{
    m_cancelled = false;
    SetEventType( wxEVT_WEBKIT_BEFORE_LOAD);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
}


IMPLEMENT_DYNAMIC_CLASS( wxWebKitNewWindowEvent, wxCommandEvent )

wxDEFINE_EVENT( wxEVT_WEBKIT_NEW_WINDOW, wxWebKitNewWindowEvent );

wxWebKitNewWindowEvent::wxWebKitNewWindowEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_NEW_WINDOW);
    if ( win )
    {
        SetEventObject( win );
        SetId(win->GetId());
    }
}


//---------------------------------------------------------
// helper function for NSString <-> wxString conversion (from src/html/htmlctrl/webkit/webkit.mm)
//---------------------------------------------------------

inline wxString wxStringWithNSString(NSString *nsstring)
{
#if wxUSE_UNICODE
    return wxString([nsstring UTF8String], wxConvUTF8);
#else
    return wxString([nsstring lossyCString]);
#endif // wxUSE_UNICODE
}

inline NSString* wxNSStringWithWxString(const wxString &wxstring)
{
#if wxUSE_UNICODE
    return [NSString stringWithUTF8String: wxstring.mb_str(wxConvUTF8)];
#else
    return [NSString stringWithCString: wxstring.c_str() length:wxstring.Len()];
#endif // wxUSE_UNICODE
}



#pragma mark Cocoa

@interface wxUIWebView : UIWebView <UIWebViewDelegate>
{
    @private
    wxWebKitCtrl *webKitWindow;
}

- (id)initWithWxWindow:(wxWebKitCtrl *)inWindow;

- (BOOL)webView:(UIWebView *)webView
shouldStartLoadWithRequest:(NSURLRequest *)request
 navigationType:(UIWebViewNavigationType)navigationType;

- (void)webViewDidStartLoad:(UIWebView *)webView;

- (void)webViewDidFinishLoad:(UIWebView *)webView;

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;

@end

@implementation wxUIWebView

- (id)init {
    if ((self = [super init])) {
        webKitWindow = NULL;
    }
    
    return self;
}

- (id)initWithWxWindow:(wxWebKitCtrl *)inWindow {
    if ((self = [super init])) {
        webKitWindow = inWindow;    // non retained
    }
    
    return self;
}

- (BOOL)webView:(UIWebView *)webView
shouldStartLoadWithRequest:(NSURLRequest *)request
 navigationType:(UIWebViewNavigationType)navigationType {
    
    return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView {
    
    if (webKitWindow) {
        NSString *url = [[[webView request] URL] absoluteString];
        
        wxWebKitStateChangedEvent negotiatingEvent(webKitWindow);
        negotiatingEvent.SetState(wxWEBKIT_STATE_NEGOTIATING);
        negotiatingEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler()) {
            webKitWindow->GetEventHandler()->ProcessEvent( negotiatingEvent );
        }
        
        wxWebKitStateChangedEvent transferringEvent(webKitWindow);
        transferringEvent.SetState(wxWEBKIT_STATE_TRANSFERRING);
        transferringEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler()) {
            webKitWindow->GetEventHandler()->ProcessEvent( transferringEvent );
        }
    }
    
}

- (void)webViewDidFinishLoad:(UIWebView *)webView {
    
    if (webKitWindow) {
        
        // "Finished loading" event
        NSString *url = [[[webView request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_STOP);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler()) {
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
        }
        
        // Title
        NSString *title = [webView stringByEvaluatingJavaScriptFromString:@"document.title"];
        webKitWindow->SetPageTitle(wxStringWithNSString( title ));
    }
    
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error {
    
    wxUnusedVar(error);
    
    if (webKitWindow) {
        NSString *url = [[[webView request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        if (webKitWindow->GetEventHandler())
            webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
    
}

@end


#pragma mark wx

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxWebKitCtrl::Create(wxWindow *parent,
                          wxWindowID winID,
                          const wxString& strURL,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxValidator& validator,
                          const wxString& name)
{
    m_currTextSize = 100;
    m_currentURL = strURL;
    
    DontCreatePeer();

    if (! wxControl::Create(parent, winID, pos, size, style , validator , name)) {
        return false;
    }
    
    CGRect r = wxOSXGetFrameForControl(this, pos , size) ;
    
    m_webView = [[wxUIWebView alloc] initWithWxWindow:this];
    [m_webView setDelegate:m_webView];
    
    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl(this, m_webView );
    SetPeer(c);
    
    MacPostControlCreate(pos, size);
    
    LoadURL(m_currentURL);

    return true;
}

wxWebKitCtrl::~wxWebKitCtrl()
{

}

// ----------------------------------------------------------------------------
// public methods
// ----------------------------------------------------------------------------

void wxWebKitCtrl::LoadURL(const wxString &url)
{
    if( !m_webView ) {
        return;
    }
    
    NSString *stringURL = [NSString stringWithString:wxCFStringRef(url).AsNSString()];
    if ( !stringURL ) {
        return;
    }
    if ([stringURL isEqualToString:@""]) {
        return;
    }
    
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:stringURL]];
    [m_webView loadRequest:request];
    
    m_currentURL = url;
}

bool wxWebKitCtrl::CanGoBack()
{    
    if ( !m_webView ) {
        return false;
    }
    
    return [m_webView canGoBack];
}

bool wxWebKitCtrl::CanGoForward()
{    
    if ( !m_webView ) {
        return false;
    }
    
    return [m_webView canGoForward];
}

bool wxWebKitCtrl::GoBack()
{

    if ( !m_webView ) {
        return false;
    }

    [m_webView goBack];
    return true;
}

bool wxWebKitCtrl::GoForward()
{
    
    if ( !m_webView ) {
        return false;
    }
    
    [m_webView goForward];
    return true;
}

void wxWebKitCtrl::Reload()
{
    if ( !m_webView ) {
        return;
    }
    
    [m_webView reload];
}

void wxWebKitCtrl::Stop()
{
    if ( !m_webView ) {
        return;
    }
    
    [m_webView stopLoading];
}

bool wxWebKitCtrl::CanGetPageSource()
{    
    if ( !m_webView ) {
        return false;
    }
    
    if ( m_webView.loading ) {
        return false;
    }
    
    return true;
}

wxString wxWebKitCtrl::GetPageSource()
{    
    if ( !m_webView ) {
        return wxEmptyString;
    }
    
    if ( !CanGetPageSource() ) {
        return wxEmptyString;
    }
        
    NSString *source = [m_webView stringByEvaluatingJavaScriptFromString:
                        @"document.getElementsByTagName('html')[0].outerHTML"];
    if ( !source ) {
        return wxEmptyString;
    } else {
        return wxStringWithNSString(source);
    }
}

wxString wxWebKitCtrl::GetSelection()
{    
    if ( !m_webView ) {
        return wxEmptyString;
    }
    
    NSString *selection = [m_webView stringByEvaluatingJavaScriptFromString:@"window.getSelection().toString()"];
    if ( !selection ) {
        return wxEmptyString;
    } else {
        return wxStringWithNSString(selection);
    }    
}

void wxWebKitCtrl::AdjustTextSize()
{
    NSString *adjustString = [NSString stringWithFormat:@"document.getElementsByTagName('body')[0].style.webkitTextSizeAdjust= '%d%%'", 
                              m_currTextSize];
    [m_webView stringByEvaluatingJavaScriptFromString:adjustString];
}

bool wxWebKitCtrl::CanIncreaseTextSize()
{    
    if ( !m_webView ) {
        return false;
    }
    
    return (m_currTextSize < WX_IPHONE_WEBKIT_TEXTSIZE_MAX);
}

void wxWebKitCtrl::IncreaseTextSize()
{    
    if ( !m_webView ) {
        return;
    }
    
    if (CanIncreaseTextSize()) {
        m_currTextSize += WX_IPHONE_WEBKIT_TEXTSIZE_STEP;
        AdjustTextSize();
    }
}

bool wxWebKitCtrl::CanDecreaseTextSize()
{    
    if ( !m_webView ) {
        return false;
    }
    
    return (m_currTextSize > WX_IPHONE_WEBKIT_TEXTSIZE_MIN);
}

void wxWebKitCtrl::DecreaseTextSize()
{    
    if ( !m_webView ) {
        return;
    }
    
    if (CanDecreaseTextSize()) {
        m_currTextSize -= WX_IPHONE_WEBKIT_TEXTSIZE_STEP;
        AdjustTextSize();
    }
}

void wxWebKitCtrl::SetPageSource(const wxString& source,
                                 const wxString& baseUrl)
{    
    if ( !m_webView ) {
        return;
    }
    
    [m_webView loadHTMLString:(NSString *)wxNSStringWithWxString(source)
                      baseURL:[NSURL URLWithString:wxNSStringWithWxString(baseUrl)]];
}

void wxWebKitCtrl::Print(bool showPrompt)
{
    
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_4_2
    
    // No AirPrint since 4.2
    return;
    
#else

    if ( !m_webView ) {
        return;
    }
    
    // Take screenshot
    CGRect myRect = [m_webView bounds];
    
    UIGraphicsBeginImageContext(myRect.size);   
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [[UIColor blackColor] set];
    CGContextFillRect(ctx, myRect);
    
    [m_webView.layer renderInContext:ctx];

    UIImage *viewImage = UIGraphicsGetImageFromCurrentImageContext();
    NSData *imageData = UIImageJPEGRepresentation(viewImage, 1.0);
    UIGraphicsEndImageContext();
    
    // Send to AirPrint
    UIPrintInteractionController *pic = [UIPrintInteractionController sharedPrintController];
    if ( !pic ) {
        return;
    }
    
    if ( ![UIPrintInteractionController canPrintData:imageData] ) {
        return;
    }
    
    UIPrintInfo *printInfo = [UIPrintInfo printInfo];
    [printInfo setOutputType:UIPrintInfoOutputGeneral];
    [printInfo setJobName:[NSString stringWithString:wxNSStringWithWxString(m_pageTitle)]];
    [printInfo setDuplex:UIPrintInfoDuplexLongEdge];
    
    [pic setPrintInfo:printInfo];
    [pic setShowsPageRange:YES];
    
    [pic setPrintingItem:imageData];
    
    void (^completionHandler)(UIPrintInteractionController *, BOOL, NSError *) = ^(UIPrintInteractionController *pic,
                                                                                   BOOL completed,
                                                                                   NSError *error) {
        if (!completed && error) {
            NSLog(@"AirPrint error; domain: %@; error code: %u", error.domain, error.code);
        }
    };
    
    [pic presentAnimated:YES
       completionHandler:completionHandler];
    
#endif  // __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_4_2
}

void wxWebKitCtrl::MakeEditable(bool enable)
{    
    // Do nothing
}

bool wxWebKitCtrl::IsEditable()
{    
    // Well, I guess.
    return true;
}

int wxWebKitCtrl::GetScrollPos()
{
    if ( !m_webView ) {
        return 0;
    }
    
    NSString *scrollPos = [m_webView stringByEvaluatingJavaScriptFromString:@"document.body.scrollTop"];
    if ( scrollPos ) {
        return [scrollPos intValue];
    } else {
        return 0;
    }
}

void wxWebKitCtrl::SetScrollPos(int pos)
{    
    if ( !m_webView ) {
        return;
    }
    
    [m_webView stringByEvaluatingJavaScriptFromString:[NSString stringWithFormat:@"document.body.scrollTop = %d;", pos]];
}

void wxWebKitCtrl::SetUserScaling(bool scaling)
{
    if (! m_webView ) {
        return;
    }
    
    [m_webView setScalesPageToFit:(scaling ? YES : NO)];
}

/// Returns true if the user can adjust the zoom.
bool wxWebKitCtrl::GetUserScaling() const
{
    if (! m_webView ) {
        return false;
    }
    
    return (m_webView.scalesPageToFit == YES);
}

wxString wxWebKitCtrl::RunScript(const wxString& javascript)
{    
    if ( !m_webView ) {
        return wxEmptyString;
    }
    
    
    NSString *scriptResult = [m_webView stringByEvaluatingJavaScriptFromString:wxNSStringWithWxString(javascript)];
    if (scriptResult) {
        return wxStringWithNSString(scriptResult);
    } else {
        return wxEmptyString;
    }
}

void wxWebKitCtrl::OnSize(wxSizeEvent &event)
{
    event.Skip();
}

void wxWebKitCtrl::MacVisibilityChanged()
{

}

#endif  //wxUSE_WEBKIT
