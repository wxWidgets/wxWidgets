/////////////////////////////////////////////////////////////////////////////
// Name:        webkit.mm
// Purpose:     wxWebKitCtrl - embeddable web kit control
// Author:      Jethro Grassie / Kevin Ollivier
// Modified by:
// Created:     2004-4-16
// RCS-ID:      $Id$
// Copyright:   (c) Jethro Grassie / Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "webkit.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXCOCOA__
#include "wx/cocoa/autorelease.h"
#else
#include "wx/mac/uma.h"
#include <Carbon/Carbon.h>
#include <WebKit/HIWebView.h>
#include <WebKit/CarbonUtils.h>
#endif

#include "wx/html/webkit.h"
//#include "wx/html/wklisten.h"


// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWebKitCtrl, wxControl)
#endif

BEGIN_EVENT_TABLE(wxWebKitCtrl, wxControl)
    EVT_SIZE(wxWebKitCtrl::OnSize)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxWebKit Events
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxWebKitStateChangedEvent, wxCommandEvent )

DEFINE_EVENT_TYPE( wxEVT_WEBKIT_STATE_CHANGED )

wxWebKitStateChangedEvent::wxWebKitStateChangedEvent( wxWindow* win )
{
    SetEventType( wxEVT_WEBKIT_STATE_CHANGED);
    SetEventObject( win );
    SetId(win->GetId());
}

//---------------------------------------------------------
// helper functions for NSString<->wxString conversion
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

@interface MyFrameLoadMonitor : NSObject
{
    wxWindow* webKitWindow;
}

- initWithWxWindow: (wxWindow*)inWindow;

@end

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxWebKitCtrl::Create(wxWindow *parent,
                                 wxWindowID winID,
                                 const wxString& strURL,
                                 const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxValidator& validator,
                                 const wxString& name)
{

    m_currentURL = strURL;
    m_pageTitle = wxT("");
 
 //still needed for wxCocoa??
/*
    int width, height;
    wxSize sizeInstance;
    if (size.x == -1 || size.y == -1)
    {
        m_parent->GetClientSize(&width, &height);
        sizeInstance.x = width;
        sizeInstance.y = height;
    }
    else
    {
        sizeInstance.x = size.x;
        sizeInstance.y = size.y;
    }
*/  
    // now create and attach WebKit view...
#ifdef __WXCOCOA__
    wxControl::Create(parent, m_windowID, pos, sizeInstance, style , validator , name);
    SetSize(pos.x, pos.y, sizeInstance.x, sizeInstance.y);
    
    wxTopLevelWindowCocoa *topWin = wxDynamicCast(this, wxTopLevelWindowCocoa);
    NSWindow* nsWin = topWin->GetNSWindow();
    NSRect rect;
    rect.origin.x = pos.x;
    rect.origin.y = pos.y;
    rect.size.width = sizeInstance.x;
    rect.size.height = sizeInstance.y;
    m_webView = (WebView*)[[WebView alloc] initWithFrame:rect frameName:@"webkitFrame" groupName:@"webkitGroup"];
    SetNSView(m_webView);
    [m_cocoaNSView release];

    if(m_parent) m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,sizeInstance);
#else
    m_macIsUserPane = false;
    wxControl::Create(parent, m_windowID, pos, size, style , validator , name);
    WebInitForCarbon();
    HIWebViewCreate( (HIViewRef*) &m_macControl );
    MacPostControlCreate(pos, size);

    HIViewSetVisible( (HIViewRef) m_macControl, true );
    
    m_webView = (WebView*) HIWebViewGetWebView( (HIViewRef) m_macControl );
#endif

    // Register event listener interfaces
    MyFrameLoadMonitor* myFrameLoadMonitor = [[MyFrameLoadMonitor alloc] initWithWxWindow: (wxWindow*)this];
    [m_webView setFrameLoadDelegate:myFrameLoadMonitor];
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
    if( !m_webView )
        return;
        
    [[m_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:wxNSStringWithWxString(url)]]];

    m_currentURL = url;
}

bool wxWebKitCtrl::CanGoBack(){
    if ( !m_webView )
        return false;
        
    return [m_webView canGoBack];
}

bool wxWebKitCtrl::CanGoForward(){
    if ( !m_webView )
        return false;
        
    return [m_webView canGoForward];
}

bool wxWebKitCtrl::GoBack(){
    if ( !m_webView )
        return false;
        
    [m_webView goBack];
    return true;
}

bool wxWebKitCtrl::GoForward(){ 
    if ( !m_webView )
        return false;
        
    [m_webView goForward];
    return true;
}

void wxWebKitCtrl::Reload(){ 
    if ( !m_webView )
        return;
        
    [[m_webView mainFrame] reload];
}

void wxWebKitCtrl::Stop(){
    if ( !m_webView )
        return;
        
    [[m_webView mainFrame] stopLoading];
}

bool wxWebKitCtrl::CanGetPageSource(){
    if ( !m_webView )
        return;
        
    WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
    return ( [[dataSource representation] canProvideDocumentSource] );
}

wxString wxWebKitCtrl::GetPageSource(){
    if ( !m_webView )
        return;
    
    if (CanGetPageSource()){
        WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
        return wxStringWithNSString( [[dataSource representation] documentSource] );
    }
    
}

void wxWebKitCtrl::SetPageSource(wxString& source, const wxString& baseUrl){
    if ( !m_webView )
        return;
    
    if (CanGetPageSource()){
        [[m_webView mainFrame] loadHTMLString:(NSString*)wxNSStringWithWxString( source ) baseURL:[NSURL URLWithString:wxNSStringWithWxString( baseUrl )]];
    }

}

void wxWebKitCtrl::OnSize(wxSizeEvent &event){
    [m_webView display];
    event.Skip();
}

//------------------------------------------------------------
// Listener interfaces
//------------------------------------------------------------

@implementation MyFrameLoadMonitor

- initWithWxWindow: (wxWindow*)inWindow
{
    [super init];
    webKitWindow = inWindow;    // non retained
    return self;
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_NEGOTIATING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_TRANSFERRING);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_STOP);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame dataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError*) error forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        NSString *url = [[[[frame provisionalDataSource] request] URL] absoluteString];
        wxWebKitStateChangedEvent thisEvent(webKitWindow);
        thisEvent.SetState(wxWEBKIT_STATE_FAILED);
        thisEvent.SetURL( wxStringWithNSString( url ) );
        webKitWindow->GetEventHandler()->ProcessEvent( thisEvent );
    }
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
    if (frame == [sender mainFrame]){
        webKitWindow->SetTitle(wxStringWithNSString( title ));
    }
}
@end
