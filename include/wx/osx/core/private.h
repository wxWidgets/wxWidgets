/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/private.h
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

#ifndef _WX_PRIVATE_CORE_H_
#define _WX_PRIVATE_CORE_H_

#include "wx/defs.h"

#include <CoreFoundation/CoreFoundation.h>

#include "wx/osx/core/cfstring.h"
#include "wx/osx/core/cfdataref.h"

#if wxUSE_GUI

#if wxOSX_USE_IPHONE
#include <CoreGraphics/CoreGraphics.h>
#else
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "wx/bitmap.h"
#include "wx/window.h"

class WXDLLIMPEXP_CORE wxMacCGContextStateSaver
{
    DECLARE_NO_COPY_CLASS(wxMacCGContextStateSaver)

public:
    wxMacCGContextStateSaver( CGContextRef cg )
    {
        m_cg = cg;
        CGContextSaveGState( cg );
    }
    ~wxMacCGContextStateSaver()
    {
        CGContextRestoreGState( m_cg );
    }
private:
    CGContextRef m_cg;
};

class WXDLLIMPEXP_CORE wxDeferredObjectDeleter : public wxObject
{
public :
    wxDeferredObjectDeleter( wxObject* obj ) : m_obj(obj) 
    {
    }
    virtual ~wxDeferredObjectDeleter()
    {
        delete m_obj;
    }
protected :
    wxObject* m_obj ;
} ;

// Quartz

WXDLLIMPEXP_CORE CGImageRef wxMacCreateCGImageFromBitmap( const wxBitmap& bitmap );

WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithCFData( CFDataRef data );
WXDLLIMPEXP_CORE CGDataConsumerRef wxMacCGDataConsumerCreateWithCFData( CFMutableDataRef data );
WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithMemoryBuffer( const wxMemoryBuffer& buf );

CGColorSpaceRef WXDLLIMPEXP_CORE wxMacGetGenericRGBColorSpace(void);

class wxWindowMac;
// to
extern wxWindow* g_MacLastWindow;
class wxNonOwnedWindow;

class WXDLLIMPEXP_CORE wxWidgetImpl : public wxObject
{
public :
    wxWidgetImpl( wxWindowMac* peer , bool isRootControl = false );
    wxWidgetImpl();
    virtual ~wxWidgetImpl();

    void Init();

    virtual void        Destroy();
    
    bool                IsRootControl() const { return m_isRootControl; }

    wxWindowMac*        GetWXPeer() const { return m_wxPeer; }
    
    bool IsOk() const { return GetWXWidget() != NULL; }

    // not only the control itself, but also all its parents must be visible
    // in order for this function to return true
    virtual bool        IsVisible() const = 0;
    // set the visibility of this widget (maybe latent)
    virtual void        SetVisibility( bool visible ) = 0;

    virtual void        Raise() = 0;
    
    virtual void        Lower() = 0;

    virtual void        ScrollRect( const wxRect *rect, int dx, int dy ) = 0;

    virtual WXWidget    GetWXWidget() const = 0;
    
    virtual void        SetBackgroundColour( const wxColour& col ) = 0;

    // all coordinates in native parent widget relative coordinates
    virtual void        GetContentArea( int &left , int &top , int &width , int &height ) const = 0;
    virtual void        Move(int x, int y, int width, int height) = 0;
    virtual void        GetPosition( int &x, int &y ) const = 0;
    virtual void        GetSize( int &width, int &height ) const = 0;

    virtual void        SetNeedsDisplay( const wxRect* where = NULL ) = 0;
    virtual bool        GetNeedsDisplay() const = 0;

    virtual bool        NeedsFocusRect() const;
    virtual void        SetNeedsFocusRect( bool needs );

    virtual bool        CanFocus() const = 0;
    // return true if successful
    virtual bool        SetFocus() = 0;
    virtual bool        HasFocus() const = 0;
    
    virtual void        RemoveFromParent() = 0;
    virtual void        Embed( wxWidgetImpl *parent ) = 0;

    // static creation methods, must be implemented by all toolkits
    
    static wxWidgetImpl*    CreateUserPane( wxWindowMac* wxpeer, const wxPoint& pos, const wxSize& size,
                            long style, long extraStyle, const wxString& name) ;
    static wxWidgetImpl*    CreateContentView( wxNonOwnedWindow* now ) ;

    // converts from Toplevel-Content relative to local
    static void Convert( wxPoint *pt , wxWidgetImpl *from , wxWidgetImpl *to );
protected :
    bool                m_isRootControl;
    wxWindowMac*        m_wxPeer;
    bool                m_needsFocusRect;

    DECLARE_ABSTRACT_CLASS(wxWidgetImpl)
};

class wxNonOwnedWindowImpl : public wxObject
{
public :
    wxNonOwnedWindowImpl( wxNonOwnedWindow* nonownedwnd) : m_wxPeer(nonownedwnd)
    {
    }
    wxNonOwnedWindowImpl()
    {
    }
    virtual ~wxNonOwnedWindowImpl()
    {
    }
            
    virtual void Destroy() 
    {
    }

    virtual void Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name ) = 0;
    
    
    virtual WXWindow GetWXWindow() const = 0;
        
    virtual void Raise()
    {
    }
    
    virtual void Lower()
    {
    }

    virtual bool Show(bool show)
    {
        return false;
    }
    
    virtual bool ShowWithEffect(bool show, wxShowEffect effect, unsigned timeout)
    {
        return Show(show);
    }
        
    virtual void Update()
    {
    }

    virtual bool SetTransparent(wxByte alpha)
    {
        return false;
    }

    virtual bool SetBackgroundColour(const wxColour& col )
    {
        return false;
    }

    virtual void SetExtraStyle( long exStyle )
    {
    }
    
    virtual bool SetBackgroundStyle(wxBackgroundStyle style)
    {            
        return false ;
    }
    
    bool CanSetTransparent()
    {
        return false;
    }

    virtual void GetContentArea( int &left , int &top , int &width , int &height ) const = 0;    
    virtual void MoveWindow(int x, int y, int width, int height) = 0;
    virtual void GetPosition( int &x, int &y ) const = 0;
    virtual void GetSize( int &width, int &height ) const = 0;

    virtual bool SetShape(const wxRegion& region)
    {
        return false;
    }
        
    virtual void SetTitle( const wxString& title, wxFontEncoding encoding ) = 0;
    
    virtual bool IsMaximized() const = 0;
    
    virtual bool IsIconized() const= 0;
    
    virtual void Iconize( bool iconize )= 0;
    
    virtual void Maximize(bool maximize) = 0;
    
    virtual bool IsFullScreen() const= 0;
    
    virtual bool ShowFullScreen(bool show, long style)= 0;

    virtual void RequestUserAttention(int flags) = 0;
    
    virtual void ScreenToWindow( int *x, int *y ) = 0; 
    
    virtual void WindowToScreen( int *x, int *y ) = 0;
    
    wxNonOwnedWindow*   GetWXPeer() { return m_wxPeer; }
protected :
    wxNonOwnedWindow*   m_wxPeer;
    DECLARE_ABSTRACT_CLASS(wxNonOwnedWindowImpl)
};

#endif // wxUSE_GUI

//---------------------------------------------------------------------------
// cocoa bridging utilities
//---------------------------------------------------------------------------

bool wxMacInitCocoa();

class WXDLLIMPEXP_CORE wxMacAutoreleasePool
{
public :
    wxMacAutoreleasePool();
    ~wxMacAutoreleasePool();
private :
    void* m_pool;
};

// NSObject

void wxMacCocoaRelease( void* obj );
void wxMacCocoaAutorelease( void* obj );
void wxMacCocoaRetain( void* obj );


#endif
    // _WX_PRIVATE_CORE_H_
