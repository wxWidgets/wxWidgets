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
#include "wx/splitter.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_WEBKIT

#ifdef __WXCOCOA__
#include "wx/cocoa/autorelease.h"
#else
#include "wx/mac/uma.h"
#include <Carbon/Carbon.h>
#include <WebKit/WebKit.h>
#include <WebKit/HIWebView.h>
#include <WebKit/CarbonUtils.h>
#endif

#include "wx/html/webkit.h"
#include "wx/notebook.h"


// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWebKitCtrl, wxControl)

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
    //m_pageTitle = _("Untitled Page");

 //still needed for wxCocoa??
/*
    int width, height;
    wxSize sizeInstance;
    if (size.x == wxDefaultCoord || size.y == wxDefaultCoord)
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
    wxControl::Create(parent, winID, pos, size, style , validator , name);
    m_peer = new wxMacControl(this);
    WebInitForCarbon();
    HIWebViewCreate( m_peer->GetControlRefAddr() );

    m_webView = (WebView*) HIWebViewGetWebView( m_peer->GetControlRef() );
    MacPostControlCreate(pos, size);
    HIViewSetVisible( m_peer->GetControlRef(), true );
    [m_webView setHidden:false];
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    if ( UMAGetSystemVersion() >= 0x1030 )
    HIViewChangeFeatures( m_peer->GetControlRef() , kHIViewIsOpaque , 0 ) ;
#endif
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

    bool result = [(WebView*)m_webView goBack];
    return result;
}

bool wxWebKitCtrl::GoForward(){
    if ( !m_webView )
        return false;

    bool result = [(WebView*)m_webView goForward];
    return result;
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
        return false;

    WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
    return ( [[dataSource representation] canProvideDocumentSource] );
}

wxString wxWebKitCtrl::GetPageSource(){

    if (CanGetPageSource()){
        WebDataSource* dataSource = [[m_webView mainFrame] dataSource];
        return wxStringWithNSString( [[dataSource representation] documentSource] );
    }

    return wxEmptyString;
}

void wxWebKitCtrl::SetPageSource(wxString& source, const wxString& baseUrl){
    if ( !m_webView )
        return;

    if (CanGetPageSource()){
        [[m_webView mainFrame] loadHTMLString:(NSString*)wxNSStringWithWxString( source ) baseURL:[NSURL URLWithString:wxNSStringWithWxString( baseUrl )]];
    }

}

void wxWebKitCtrl::OnSize(wxSizeEvent &event){
    // This is a nasty hack because WebKit seems to lose its position when it is embedded
    // in a control that is not itself the content view for a TLW.
    // I put it in OnSize because these calcs are not perfect, and in fact are basically
    // guesses based on reverse engineering, so it's best to give people the option of
    // overriding OnSize with their own calcs if need be.
    // I also left some test debugging print statements as a convenience if a(nother)
    // problem crops up.

    // Let's hope that Tiger fixes this mess...

    int x, y;
    x = 0;
    y = 0;

    wxWindow* parent = GetParent();

    wxWindow* tlw = MacGetTopLevelWindow();

    // This must be the case that Apple tested with, because well, in this one case
    // we don't need to do anything! It just works. ;)
    if (parent == tlw){
        return;
    }

    while(parent != NULL)
    {
        if ( parent->IsKindOf( CLASSINFO( wxSplitterWindow ) ) && GetParent()->IsKindOf( CLASSINFO( wxSplitterWindow ) ) ){
                // When parent is not a wxSplitterWindow, we can rely on it's GetPosition() to give us the correct
                // coordinates, but when the parent is a wxSplitterWindow, we need to manually calculate
                // the sash position of it and any parent wxSplitterWindows into the webkit's position.
                wxSplitterWindow* splitter;
                splitter = dynamic_cast<wxSplitterWindow*>(parent);
                if (splitter->GetSplitMode() == wxSPLIT_HORIZONTAL){
                    if (splitter->GetPosition().y > 0)
                        y += splitter->GetPosition().y;

                    if (splitter->GetSashSize() > 0)
                        y += splitter->GetSashSize();

                    if (splitter->GetSashPosition() > 0)
                        y += splitter->GetSashPosition();
                }
                else{
                    if (splitter->GetPosition().x > 0)
                        x += splitter->GetPosition().x;

                    if (splitter->GetSashSize() > 0)
                        x += splitter->GetSashSize();

                    if (splitter->GetSashPosition() > 0)
                        x += splitter->GetSashPosition();
                }
        }
        else{
            if (!parent->IsTopLevel()) {
                //printf("Parent: %s\n", parent->GetClassInfo()->GetClassName());
                int plusx = 0;
                plusx = parent->GetClientAreaOrigin().x + parent->GetPosition().x;
                if (plusx > 0){
                    x += plusx;
                    //printf("Parent: %s Added x: %d\n", parent->GetClassInfo()->GetClassName(), parent->GetClientAreaOrigin().x + parent->GetPosition().x);
                }

                int plusy = 0;
                plusy = parent->GetClientAreaOrigin().y + parent->GetPosition().y;
                if (plusy > 0){
                    y += plusy;
                    //printf("Parent: %s Added y: %d\n", parent->GetClassInfo()->GetClassName(), parent->GetClientAreaOrigin().y + parent->GetPosition().y);
                }
                else{
                    //printf("Parent: %s Origin: %d Position:%d\n", parent->GetClassInfo()->GetClassName(), parent->GetClientAreaOrigin().y, parent->GetPosition().y);
                }

            }
            else{
                //
                x += parent->GetClientAreaOrigin().x;
                // calculate the title bar height (26 pixels) into the top offset.
                // This becomes important later when we must flip the y coordinate
                // to convert to Cocoa's coordinate system.
                y += parent->GetClientAreaOrigin().y += 26;
                //printf("x: %d, y:%d\n", x, y);
            }
            //we still need to add the y, because we have to convert/flip coordinates for Cocoa

            if ( parent->IsKindOf( CLASSINFO( wxNotebook ) )  ){
                //Not sure why calcs are off in this one scenario...
                y -= 4;
                //printf("x: %d, y:%d\n", x, y);
            }

            if (parent->IsKindOf( CLASSINFO( wxPanel ) ) ){
                // Another strange case. Adding a wxPanel to the parent heirarchy
                // causes wxWebKitCtrl's Cocoa y origin to be 4 pixels off
                // for some reason, even if the panel has a position and origin of 0.
                // This corrects that. Man, I wish I could debug Carbon/HIWebView!! ;)
                y -= 4;
            }
        }

        parent = parent->GetParent();
    }

    // Tried using MacWindowToRootWindow both for wxWebKitCtrl and its parent,
    // but coordinates were off by a significant amount.
    // Am leaving the code here if anyone wants to play with it.

    //int x2, y2 = 0;
    //if (GetParent())
    //    GetParent()->MacWindowToRootWindow(&x2, &y2);
    //printf("x = %d, y = %d\n", x, y);
    //printf("x2 = %d, y2 = %d\n", x2, y2);
    //x = x2;
    //y = y2;

    if (tlw){
        //flip the y coordinate to convert to Cocoa coordinates
        //printf("tlw y: %d, y: %d\n", tlw->GetSize().y, (GetSize().y + y));
        y = tlw->GetSize().y - ((GetSize().y) + y);
    }

    //printf("Added to bounds x=%d, y=%d\n", x, y);
    NSRect bounds = [m_webView frame];
    bounds.origin.x = x;
    bounds.origin.y = y;
    [m_webView setFrame:bounds];

    //printf("Carbon position x=%d, y=%d\n", GetPosition().x, GetPosition().y);
    if (IsShown())
        [(WebView*)m_webView display];
    event.Skip();
}

void wxWebKitCtrl::MacVisibilityChanged(){
    bool isHidden = !IsControlVisible( m_peer->GetControlRef());
    if (!isHidden)
        [(WebView*)m_webView display];

    [m_webView setHidden:isHidden];
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

#endif //wxUSE_WEBKIT
