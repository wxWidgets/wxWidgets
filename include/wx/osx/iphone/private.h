/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_IPHONE_H_
#define _WX_PRIVATE_IPHONE_H_

#ifdef __OBJC__
    #import <UIKit/UIKit.h>
#endif

#include <CoreText/CTFont.h>
#include <CoreText/CTStringAttributes.h>
#include <CoreText/CTLine.h>


#if wxUSE_GUI

#include "wx/bmpbndl.h"

typedef CGRect WXRect;

OSStatus WXDLLIMPEXP_CORE wxMacDrawCGImage(
                               CGContextRef    inContext,
                               const CGRect *  inBounds,
                               CGImageRef      inImage) ;

WX_UIImage WXDLLIMPEXP_CORE wxOSXGetUIImageFromCGImage( CGImageRef image );
wxBitmapBundle WXDLLIMPEXP_CORE wxOSXCreateSystemBitmapBundle(const wxString& id, const wxString &client, const wxSize& size);

class WXDLLIMPEXP_CORE wxWidgetIPhoneImpl : public wxWidgetImpl
{
public :
    wxWidgetIPhoneImpl( wxWindowMac* peer , WXWidget w, int flags = 0 ) ;
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
    virtual bool        SetBackgroundStyle(wxBackgroundStyle style) ;
    virtual void        SetForegroundColour( const wxColour& col ) ;

    virtual void        GetContentArea( int &left , int &top , int &width , int &height ) const;
    virtual void        Move(int x, int y, int width, int height);
    virtual void        GetPosition( int &x, int &y ) const;
    virtual void        GetSize( int &width, int &height ) const;
    virtual void        SetControlSize( wxWindowVariant variant );
    virtual double      GetContentScaleFactor() const ;

    virtual void        SetNeedsDisplay( const wxRect* where = nullptr );
    virtual bool        GetNeedsDisplay() const;

    virtual bool        CanFocus() const;
    // return true if successful
    virtual bool        SetFocus();
    virtual bool        HasFocus() const;

    void                RemoveFromParent();
    void                Embed( wxWidgetImpl *parent );

    void                SetDefaultButton( bool isDefault );
    void                PerformClick();
    virtual void        SetLabel(const wxString& title);

    void                SetCursor( const wxCursor & cursor );
    void                CaptureMouse();
    void                ReleaseMouse();

    wxInt32             GetValue() const;
    void                SetValue( wxInt32 v );

    virtual wxBitmap    GetBitmap() const;
    virtual void        SetBitmap( const wxBitmapBundle& bitmap );
    virtual void        SetBitmapPosition( wxDirection dir );

    void                SetupTabs( const wxNotebook &notebook );
    void                GetBestRect( wxRect *r ) const;
    bool                IsEnabled() const;
    void                Enable( bool enable );
    bool                ButtonClickDidStateChange() { return true ;}
    void                SetMinimum( wxInt32 v );
    void                SetMaximum( wxInt32 v );
    void                SetIncrement(int WXUNUSED(value)) { }
    wxInt32             GetMinimum() const;
    wxInt32             GetMaximum() const;
    int                 GetIncrement() const { return 1; }
    void                PulseGauge();
    void                SetScrollThumb( wxInt32 value, wxInt32 thumbSize );

    void                SetFont(const wxFont & font);

    void                InstallEventHandler( WXWidget control = nullptr );
    bool                EnableTouchEvents(int WXUNUSED(eventsMask)) { return false; }

    virtual void        DoNotifyFocusEvent(bool receivedFocus, wxWidgetImpl* otherWindow);

    // thunk connected calls

    virtual void        drawRect(CGRect* rect, WXWidget slf, void* _cmd);
    virtual void        touchEvent(WX_NSSet touches, WX_UIEvent event, WXWidget slf, void* _cmd);
    virtual bool        becomeFirstResponder(WXWidget slf, void* _cmd);
    virtual bool        resignFirstResponder(WXWidget slf, void* _cmd);

    // action

    virtual void        controlAction(void* sender, wxUint32 controlEvent, WX_UIEvent rawEvent);
    virtual void         controlTextDidChange();
protected:
    WXWidget m_osxView;
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWidgetIPhoneImpl);
};

class wxNonOwnedWindowIPhoneImpl : public wxNonOwnedWindowImpl
{
public :
    wxNonOwnedWindowIPhoneImpl( wxNonOwnedWindow* nonownedwnd) ;
    wxNonOwnedWindowIPhoneImpl();

    virtual ~wxNonOwnedWindowIPhoneImpl();

    virtual void WillBeDestroyed() override;
    void Create(wxWindow* parent, const wxPoint& pos, const wxSize& size,
                long style, long extraStyle, const wxString& name ) override;
    void Create( wxWindow* parent, WXWindow nativeWindow );

    WXWindow GetWXWindow() const override;
    void Raise() override;
    void Lower() override;
    bool Show(bool show) override;
    bool ShowWithEffect(bool show, wxShowEffect effect, unsigned timeout) override;

    void Update() override;
    bool SetTransparent(wxByte alpha) override;
    bool SetBackgroundColour(const wxColour& col ) override;
    void SetExtraStyle( long exStyle ) override;
    bool SetBackgroundStyle(wxBackgroundStyle style) override;
    bool CanSetTransparent() override;

    void MoveWindow(int x, int y, int width, int height) override;
    void GetPosition( int &x, int &y ) const override;
    void GetSize( int &width, int &height ) const override;

    void GetContentArea( int &left , int &top , int &width , int &height ) const override;
    bool SetShape(const wxRegion& region) override;

    virtual void SetTitle( const wxString& title ) override;

    // Title bar buttons don't exist in iOS.
    virtual bool EnableCloseButton(bool WXUNUSED(enable)) override { return false; }
    virtual bool EnableMaximizeButton(bool WXUNUSED(enable)) override { return false; }
    virtual bool EnableMinimizeButton(bool WXUNUSED(enable)) override { return false; }

    virtual bool IsMaximized() const override;

    virtual bool IsIconized() const override;

    virtual void Iconize( bool iconize ) override;

    virtual void Maximize(bool maximize) override;

    virtual bool IsFullScreen() const override;

    virtual bool EnableFullScreenView(bool enable, long style) override;

    virtual bool ShowFullScreen(bool show, long style) override;

    virtual wxContentProtection GetContentProtection() const override
        {  return wxCONTENT_PROTECTION_NONE; }
    virtual bool SetContentProtection(wxContentProtection contentProtection) override
        { return false; }

    virtual void RequestUserAttention(int flags) override;

    virtual void ScreenToWindow( int *x, int *y ) override;

    virtual void WindowToScreen( int *x, int *y ) override;

    // FIXME: Does iPhone have a concept of inactive windows?
    virtual bool IsActive() override { return true; }

    wxNonOwnedWindow*   GetWXPeer() { return m_wxPeer; }

    virtual bool InitialShowEventSent() { return m_initialShowSent; }
protected :
    WX_UIWindow          m_macWindow;
    void *              m_macFullScreenData ;
    bool                m_initialShowSent;
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxNonOwnedWindowIPhoneImpl);
};

#ifdef __OBJC__

    WXDLLIMPEXP_CORE CGRect wxToNSRect( UIView* parent, const wxRect& r );
    WXDLLIMPEXP_CORE wxRect wxFromNSRect( UIView* parent, const CGRect& rect );
    WXDLLIMPEXP_CORE CGPoint wxToNSPoint( UIView* parent, const wxPoint& p );
    WXDLLIMPEXP_CORE wxPoint wxFromNSPoint( UIView* parent, const CGPoint& p );

    CGRect WXDLLIMPEXP_CORE wxOSXGetFrameForControl( wxWindowMac* window , const wxPoint& pos , const wxSize &size ,
        bool adjustForOrigin = true );

    @interface wxUIButton : UIButton
    {
    }

    @end

    @interface wxUIView : UIView
    {
    }

    @end // wxUIView


    void WXDLLIMPEXP_CORE wxOSXIPhoneClassAddWXMethods(Class c);

#endif

#endif // wxUSE_GUI

#endif
    // _WX_PRIVATE_IPHONE_H_
