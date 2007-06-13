///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/popupwin.cpp
// Purpose:     implements wxPopupWindow for wxMac
// Author:      Stefan Csomor
// Modified by:
// Created:     
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Stefan Csomor
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// CAUTION : This is only experimental stuff right now

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_POPUPWIN

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/popupwin.h"
#include "wx/tooltip.h"

#include "wx/mac/private.h"    

// ============================================================================
// implementation
// ============================================================================

wxPopupWindow::~wxPopupWindow()
{
    if ( m_popupWindowRef )
    {
#if wxUSE_TOOLTIPS
        wxToolTip::NotifyWindowDelete(m_popupWindowRef) ;
#endif
        wxPendingDelete.Append( new wxMacDeferredWindowDeleter( (WindowRef) m_popupWindowRef ) ) ;
    }
}

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    m_macIsUserPane = false ;

    // popup windows are created hidden by default
    Hide();

    if ( ! wxPopupWindowBase::Create(parent) )
        return false;

    WindowClass wclass = kHelpWindowClass;
    WindowAttributes attr = kWindowCompositingAttribute ;
    WindowRef parentWindow =(WindowRef) parent->MacGetTopLevelWindowRef();

    Rect bounds = { 0,0,0,0 };
    OSStatus err = ::CreateNewWindow( wclass , attr , &bounds , (WindowRef*)&m_popupWindowRef ) ;
    if ( err == noErr )
    {
//        SetWindowGroup( (WindowRef) m_popupWindowRef, GetWindowGroup(parentWindow));    //  Put them in the same group so that their window layers are consistent
    }
    
    m_peer = new wxMacControl(this , true /*isRootControl*/) ;

    HIViewFindByID( HIViewGetRoot( (WindowRef) m_popupWindowRef ) , kHIViewWindowContentID ,
        m_peer->GetControlRefAddr() ) ;
    if ( !m_peer->Ok() )
    {
        // compatibility mode fallback
        GetRootControl( (WindowRef) m_popupWindowRef , m_peer->GetControlRefAddr() ) ;
        if ( !m_peer->Ok() )
            CreateRootControl( (WindowRef) m_popupWindowRef , m_peer->GetControlRefAddr() ) ;
    }

    // the root control level handler
    MacInstallEventHandler( (WXWidget) m_peer->GetControlRef() ) ;

    // the frame window event handler
    InstallStandardEventHandler( GetWindowEventTarget(MAC_WXHWND(m_popupWindowRef)) ) ;
    // MacInstallTopLevelWindowEventHandler() ;
        
    if ( parent )
        parent->AddChild(this);

    return true;
}

void wxPopupWindow::DoMoveWindow(int x, int y, int width, int height)
{
    Rect bounds = { y , x , y + height , x + width } ;
    verify_noerr(SetWindowBounds( (WindowRef) m_popupWindowRef, kWindowStructureRgn , &bounds )) ;
    wxWindowMac::MacSuperChangedPosition() ; // like this only children will be notified
}

void wxPopupWindow::DoGetPosition( int *x, int *y ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_popupWindowRef, kWindowStructureRgn , &bounds )) ;

    if (x)
       *x = bounds.left ;
    if (y)
       *y = bounds.top ;
}

void wxPopupWindow::DoGetSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_popupWindowRef, kWindowStructureRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

void wxPopupWindow::DoGetClientSize( int *width, int *height ) const
{
    Rect bounds ;

    verify_noerr(GetWindowBounds((WindowRef) m_popupWindowRef, kWindowContentRgn , &bounds )) ;

    if (width)
       *width = bounds.right - bounds.left ;
    if (height)
       *height = bounds.bottom - bounds.top ;
}

bool wxPopupWindow::Show(bool show)
{
    if ( !wxWindowMac::Show(show) )
        return false;

    if (show)
    {
        ::ShowWindow( (WindowRef)m_popupWindowRef );
        ::SelectWindow( (WindowRef)m_popupWindowRef ) ;

        // because apps expect a size event to occur at this moment
        wxSizeEvent event(GetSize() , m_windowId);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        ::HideWindow( (WindowRef)m_popupWindowRef );
    }
    return true;
}

WXWindow wxPopupWindow::MacGetPopupWindowRef() const
{
    return m_popupWindowRef;
}

#endif // #if wxUSE_POPUPWIN
