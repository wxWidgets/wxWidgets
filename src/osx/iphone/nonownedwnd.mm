/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/nonownedwnd.mm
// Purpose:     non owned window for iphone
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-06-20
// RCS-ID:      $Id: nonownedwnd.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/osx/private.h"

#include "wx/nonownedwnd.h"
#include "wx/frame.h"

IMPLEMENT_DYNAMIC_CLASS( wxNonOwnedWindowIPhoneImpl , wxNonOwnedWindowImpl )

wxNonOwnedWindowIPhoneImpl::wxNonOwnedWindowIPhoneImpl( wxNonOwnedWindow* nonownedwnd) : 
    wxNonOwnedWindowImpl(nonownedwnd)
{
    m_macWindow = NULL;
    m_macFullScreenData = NULL;
}
    
wxNonOwnedWindowIPhoneImpl::wxNonOwnedWindowIPhoneImpl() 
{
    m_macWindow = NULL;
    m_macFullScreenData = NULL;
}
    
wxNonOwnedWindowIPhoneImpl::~wxNonOwnedWindowIPhoneImpl()
{
    [m_macWindow release];
}

void wxNonOwnedWindowIPhoneImpl::Destroy()
{
    wxPendingDelete.Append( new wxDeferredObjectDeleter( this ) );
}

void wxNonOwnedWindowIPhoneImpl::Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
long style, long extraStyle, const wxString& name )
{
    m_macWindow = [UIWindow alloc];
    
    UIWindowLevel level = UIWindowLevelNormal;
    
    // most styles are not supported on the iphone
    
    if ( style & wxFRAME_TOOL_WINDOW )
    {
        level = UIWindowLevelAlert; ;
    }
    else if ( ( style & wxPOPUP_WINDOW ) )
    {
        level = UIWindowLevelAlert;;
    }
    else if ( ( style & wxCAPTION ) )
    {
    }
    else if ( ( style & wxFRAME_DRAWER ) )
    {
    }
    else
    {
    }

    if ( ( style & wxSTAY_ON_TOP ) )
        level = UIWindowLevelAlert;    
    CGRect r = CGRectMake( 0, 0, size.x, size.y) ;
    
    [m_macWindow initWithFrame:r ];
        
    [m_macWindow setWindowLevel:level];
    // [m_macWindow makeKeyAndOrderFront:nil];
}


WXWindow wxNonOwnedWindowIPhoneImpl::GetWXWindow() const
{
    return m_macWindow;
}

void wxNonOwnedWindowIPhoneImpl::Raise()
{
}
    
void wxNonOwnedWindowIPhoneImpl::Lower()
{
}

bool wxNonOwnedWindowIPhoneImpl::Show(bool show)
{
    [m_macWindow setHidden:(show ? NO : YES)];
    if ( show )
    {
        //[m_macWindow orderFront: self];
        [m_macWindow makeKeyWindow];
    }
    return true;
}
    
bool wxNonOwnedWindowIPhoneImpl::ShowWithEffect(bool show, wxShowEffect effect, unsigned timeout)
{
    return Show(show);
}

void wxNonOwnedWindowIPhoneImpl::Update()
{
//  TODO  [m_macWindow displayIfNeeded];
}

bool wxNonOwnedWindowIPhoneImpl::SetTransparent(wxByte alpha)
{
    [m_macWindow setAlpha:(CGFloat) alpha/255.0];
    return true;
}

bool wxNonOwnedWindowIPhoneImpl::SetBackgroundColour(const wxColour& col )
{
    return true;
}

void wxNonOwnedWindowIPhoneImpl::SetExtraStyle( long exStyle )
{
    // no special styles supported
}
    
bool wxNonOwnedWindowIPhoneImpl::SetBackgroundStyle(wxBackgroundStyle style)
{
    return true;
}
    
bool wxNonOwnedWindowIPhoneImpl::CanSetTransparent()
{
    return true;
}

void wxNonOwnedWindowIPhoneImpl::MoveWindow(int x, int y, int width, int height)
{
    CGRect r = CGRectMake( 0,0,width,height) ;
    [m_macWindow setFrame:r];
}

void wxNonOwnedWindowIPhoneImpl::GetPosition( int &x, int &y ) const
{
    CGRect r = [m_macWindow frame];
    x = r.origin.x;
    y = r.origin.y;
}

void wxNonOwnedWindowIPhoneImpl::GetSize( int &width, int &height ) const
{
    CGRect rect = [m_macWindow frame];
    width = rect.size.width;
    height = rect.size.height;
}

void wxNonOwnedWindowIPhoneImpl::GetContentArea( int& left, int &right, int &width, int &height ) const
{
    CGRect rect = [m_macWindow  frame];
    width = rect.size.width;
    height = rect.size.height;
    left = 0;
    right = 0;
}
    
bool wxNonOwnedWindowIPhoneImpl::SetShape(const wxRegion& region)
{
    return false;
}

void wxNonOwnedWindowIPhoneImpl::SetTitle( const wxString& title, wxFontEncoding encoding ) 
{
// TODO change title of app ?
}
    
bool wxNonOwnedWindowIPhoneImpl::IsMaximized() const
{
    return false;
}
    
bool wxNonOwnedWindowIPhoneImpl::IsIconized() const
{
    return false;
}
    
void wxNonOwnedWindowIPhoneImpl::Iconize( bool iconize )
{
}
    
void wxNonOwnedWindowIPhoneImpl::Maximize(bool maximize)
{
}

bool wxNonOwnedWindowIPhoneImpl::IsFullScreen() const
{
    return m_macFullScreenData != NULL ;
}
    
bool wxNonOwnedWindowIPhoneImpl::ShowFullScreen(bool show, long style)
{    
    return true;
}

void wxNonOwnedWindowIPhoneImpl::RequestUserAttention(int WXUNUSED(flags))
{
}

void wxNonOwnedWindowIPhoneImpl::ScreenToWindow( int *x, int *y )
{
    CGPoint p = CGPointMake(  (x ? *x : 0), (y ? *y : 0) );
    p = [m_macWindow convertPoint:p fromWindow:nil];
    if ( x )
        *x = p.x;
    if ( y )
        *y = p.y;
}

void wxNonOwnedWindowIPhoneImpl::WindowToScreen( int *x, int *y )
{
    CGPoint p = CGPointMake(  (x ? *x : 0), (y ? *y : 0) );
    p = [m_macWindow convertPoint:p toWindow:nil];
    if ( x )
        *x = p.x;
    if ( y )
        *y = p.y;
}

wxNonOwnedWindowImpl* wxNonOwnedWindowImpl::CreateNonOwnedWindow( wxNonOwnedWindow* wxpeer, wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name )
{
    wxNonOwnedWindowImpl* now = new wxNonOwnedWindowIPhoneImpl( wxpeer );
    now->Create( parent, pos, size, style , extraStyle, name );
    return now;
}
