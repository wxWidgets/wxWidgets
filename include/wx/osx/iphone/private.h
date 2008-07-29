/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: private.h 53819 2008-05-29 14:11:45Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_IPHONE_H_
#define _WX_PRIVATE_IPHONE_H_

#include "wx/osx/core/private.h"

#ifdef __OBJC__
    #import <UIKit/UIKit.h>
#endif

#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxWidgetIPhoneImpl : public wxWidgetImpl
{
public :
    wxWidgetIPhoneImpl( wxWindowMac* peer , WXWidget w, bool isRootControl = false ) ;    
    wxWidgetIPhoneImpl() ;    
    ~wxWidgetIPhoneImpl();    
    
    void Init();
    
    virtual bool        IsVisible() const ;
    virtual void        SetVisibility( bool visible );
    
    virtual void        Raise();
    
    virtual void        Lower();

    virtual void        ScrollRect( const wxRect *rect, int dx, int dy );

    virtual WXWidget    GetWXWidget() const { return m_osxView; }

    virtual void        SetBackgroundColour( const wxColour& col ) ;

    virtual void        GetContentArea( int &left , int &top , int &width , int &height ) const;
    virtual void        Move(int x, int y, int width, int height);
    virtual void        GetPosition( int &x, int &y ) const;
    virtual void        GetSize( int &width, int &height ) const;

    virtual void        SetNeedsDisplay( const wxRect* where = NULL );
    virtual bool        GetNeedsDisplay() const;

    virtual bool        CanFocus() const;
    // return true if successful
    virtual bool        SetFocus();
    virtual bool        HasFocus() const;

    void                RemoveFromParent();
    void                Embed( wxWidgetImpl *parent );
protected:
    WXWidget m_osxView;
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxWidgetIPhoneImpl)
};

class wxNonOwnedWindowIPhoneImpl : public wxNonOwnedWindowImpl
{
public :
    wxNonOwnedWindowIPhoneImpl( wxNonOwnedWindow* nonownedwnd) ;
    wxNonOwnedWindowIPhoneImpl();
    
    virtual ~wxNonOwnedWindowIPhoneImpl();
    
    virtual void Destroy() ;
    void Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name ) ;
    
    WXWindow GetWXWindow() const;
    void Raise();    
    void Lower();
    bool Show(bool show);    
    bool ShowWithEffect(bool show, wxShowEffect effect, unsigned timeout);
        
    void Update();
    bool SetTransparent(wxByte alpha);
    bool SetBackgroundColour(const wxColour& col );
    void SetExtraStyle( long exStyle );    
    bool SetBackgroundStyle(wxBackgroundStyle style);    
    bool CanSetTransparent();

    void MoveWindow(int x, int y, int width, int height);
    void GetPosition( int &x, int &y ) const;
    void GetSize( int &width, int &height ) const;

    void GetContentArea( int &left , int &top , int &width , int &height ) const;    
    bool SetShape(const wxRegion& region);
    
    virtual void SetTitle( const wxString& title, wxFontEncoding encoding ) ;
    
    virtual bool IsMaximized() const;
    
    virtual bool IsIconized() const;
    
    virtual void Iconize( bool iconize );
    
    virtual void Maximize(bool maximize);
    
    virtual bool IsFullScreen() const;
    
    virtual bool ShowFullScreen(bool show, long style);

    virtual void RequestUserAttention(int flags);
    
    virtual void ScreenToWindow( int *x, int *y );
    
    virtual void WindowToScreen( int *x, int *y );
    
    wxNonOwnedWindow*   GetWXPeer() { return m_wxPeer; }
protected :
    WX_UIWindow          m_macWindow;
    void *              m_macFullScreenData ;
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxNonOwnedWindowIPhoneImpl)
};    

#endif // wxUSE_GUI

#endif
    // _WX_PRIVATE_IPHONE_H_
