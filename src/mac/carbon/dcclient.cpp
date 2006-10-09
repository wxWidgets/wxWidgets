/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
    #include "wx/toplevel.h"
    #include "wx/math.h"
    #include "wx/region.h"
#endif

#include "wx/mac/private.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)

/*
 * wxWindowDC
 */

#include "wx/mac/uma.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"


static wxBrush MacGetBackgroundBrush( wxWindow* window )
{
    wxBrush bkdBrush = window->MacGetBackgroundBrush() ;

#if !TARGET_API_MAC_OSX
    // transparency cannot be handled by the OS when not using composited windows
    wxWindow* parent = window->GetParent() ;

    // if we have some 'pseudo' transparency
    if ( ! bkdBrush.Ok() || bkdBrush.GetStyle() == wxTRANSPARENT || window->GetBackgroundColour() == wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE ) )
    {
        // walk up until we find something
        while ( parent != NULL )
        {
            if ( parent->GetBackgroundColour() != wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE ) )
            {
                // if we have any other colours in the hierarchy
                bkdBrush.SetColour( parent->GetBackgroundColour() ) ;
                break ;
            }

            if ( parent->IsKindOf( CLASSINFO(wxTopLevelWindow) ) )
            {
                bkdBrush = parent->MacGetBackgroundBrush() ;
                break ;
            }

            if ( parent->IsKindOf( CLASSINFO( wxNotebook ) )
#if wxUSE_TAB_DIALOG
                 || parent->IsKindOf( CLASSINFO( wxTabCtrl ) )
#endif // wxUSE_TAB_DIALOG
                )
            {
                Rect extent = { 0 , 0 , 0 , 0 } ;
                int x , y ;
                x = y = 0 ;
                wxSize size = parent->GetSize() ;
                parent->MacClientToRootWindow( &x , &y ) ;
                extent.left = x ;
                extent.top = y ;
                extent.top-- ;
                extent.right = x + size.x ;
                extent.bottom = y + size.y ;
                bkdBrush.MacSetThemeBackground( kThemeBackgroundTabPane , (WXRECTPTR) &extent ) ;
                break ;
            }

            parent = parent->GetParent() ;
        }
    }

    if ( !bkdBrush.Ok() || bkdBrush.GetStyle() == wxTRANSPARENT )
    {
        // if we did not find something, use a default
        bkdBrush.MacSetTheme( kThemeBrushDialogBackgroundActive ) ;
    }
#endif

    return bkdBrush ;
}

wxWindowDC::wxWindowDC()
{
    m_window = NULL ;
}

wxWindowDC::wxWindowDC(wxWindow *window)
{
    m_window = window ;
    wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
    if (!rootwindow)
        return;

    WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
    int x , y ;
    x = y = 0 ;
    window->MacWindowToRootWindow( &x , &y ) ;
    m_macPort = UMAGetWindowPort( windowref ) ;
    m_ok = true ;

#if wxMAC_USE_CORE_GRAPHICS
    m_macLocalOriginInPort.x = x ;
    m_macLocalOriginInPort.y = y ;

    if ( window->MacGetCGContextRef() )
    {
        m_graphicContext = new wxMacCGContext( (CGContextRef) window->MacGetCGContextRef() ) ;
        m_graphicContext->SetPen( m_pen ) ;
        m_graphicContext->SetBrush( m_brush ) ;
    }
    else
    {
        // as out of order redraw is not supported under CQ, we have to create a qd port for these
        // situations
        m_macLocalOrigin.x = x ;
        m_macLocalOrigin.y = y ;

        m_graphicContext = new wxMacCGContext( (CGrafPtr) m_macPort ) ;
        m_graphicContext->SetPen( m_pen ) ;
        m_graphicContext->SetBrush( m_brush ) ;
    }
    // there is no out-of-order drawing on OSX
#else
    m_macLocalOrigin.x = x ;
    m_macLocalOrigin.y = y ;
    CopyRgn( (RgnHandle) window->MacGetVisibleRegion(true).GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
#endif
    SetBackground(MacGetBackgroundBrush(window));

    SetFont( window->GetFont() ) ;
}

wxWindowDC::~wxWindowDC()
{
}

void wxWindowDC::DoGetSize( int* width, int* height ) const
{
    wxCHECK_RET( m_window, _T("GetSize() doesn't work without window") );

    m_window->GetSize(width, height);
}

/*
 * wxClientDC
 */

wxClientDC::wxClientDC()
{
    m_window = NULL ;
}

wxClientDC::wxClientDC(wxWindow *window)
{
    m_window = window ;
    wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
    if (!rootwindow)
        return;

    WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
    wxPoint origin = window->GetClientAreaOrigin() ;
    wxSize size = window->GetClientSize() ;
    int x , y ;
    x = origin.x ;
    y = origin.y ;
    window->MacWindowToRootWindow( &x , &y ) ;
    m_macPort = UMAGetWindowPort( windowref ) ;
    m_ok = true ;

#if wxMAC_USE_CORE_GRAPHICS
    m_macLocalOriginInPort.x = x ;
    m_macLocalOriginInPort.y = y ;
    if ( window->MacGetCGContextRef() )
    {
        m_graphicContext = new wxMacCGContext( (CGContextRef) window->MacGetCGContextRef() ) ;
        m_graphicContext->SetPen( m_pen ) ;
        m_graphicContext->SetBrush( m_brush ) ;
        SetClippingRegion( 0 , 0 , size.x , size.y ) ;
    }
    else
    {
        // as out of order redraw is not supported under CQ,
        // we have to create a QD port for these situations
        m_macLocalOrigin.x = x ;
        m_macLocalOrigin.y = y ;
        m_graphicContext = new wxMacCGContext( (CGrafPtr) m_macPort ) ;
        m_graphicContext->SetPen( m_pen ) ;
        m_graphicContext->SetBrush( m_brush ) ;
     }
#else
    m_macLocalOrigin.x = x ;
    m_macLocalOrigin.y = y ;
    SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , origin.x , origin.y , origin.x + size.x , origin.y + size.y ) ;
    SectRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) window->MacGetVisibleRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , -origin.x , -origin.y ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn ,(RgnHandle)  m_macCurrentClipRgn ) ;
#endif

    SetBackground(MacGetBackgroundBrush(window));
    SetFont( window->GetFont() ) ;
}

wxClientDC::~wxClientDC()
{
#if wxMAC_USE_CORE_GRAPHICS
/*
    if ( m_window->MacGetCGContextRef() == 0)
    {
        CGContextRef cgContext = (wxMacCGContext*)(m_graphicContext)->GetNativeContext() ;
        CGContextFlush( cgContext ) ;
    }
*/
#endif
}

void wxClientDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, _T("GetSize() doesn't work without window") );

    m_window->GetClientSize( width, height );
}

/*
 * wxPaintDC
 */

wxPaintDC::wxPaintDC()
{
    m_window = NULL ;
}

wxPaintDC::wxPaintDC(wxWindow *window)
{
    m_window = window ;
    wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
    WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
    wxPoint origin = window->GetClientAreaOrigin() ;
    wxSize size = window->GetClientSize() ;
    int x , y ;
    x = origin.x ;
    y = origin.y ;
    window->MacWindowToRootWindow( &x , &y ) ;
    m_macPort = UMAGetWindowPort( windowref ) ;
    m_ok = true ;

#if wxMAC_USE_CORE_GRAPHICS
    m_macLocalOriginInPort.x = x ;
    m_macLocalOriginInPort.y = y ;
    if ( window->MacGetCGContextRef() )
    {
        m_graphicContext = new wxMacCGContext( (CGContextRef) window->MacGetCGContextRef() ) ;
        m_graphicContext->SetPen( m_pen ) ;
        m_graphicContext->SetBrush( m_brush ) ;
        SetClippingRegion( 0 , 0 , size.x , size.y ) ;
        SetBackground(MacGetBackgroundBrush(window));
    }
    else
    {
        wxLogDebug(wxT("You cannot create a wxPaintDC outside an OS-draw event") ) ;
        m_graphicContext = NULL ;
    }
    // there is no out-of-order drawing on OSX
#else
    m_macLocalOrigin.x = x ;
    m_macLocalOrigin.y = y ;
    SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , origin.x , origin.y , origin.x + size.x , origin.y + size.y ) ;
    SectRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) window->MacGetVisibleRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , -origin.x , -origin.y ) ;
    SectRgn( (RgnHandle) m_macBoundaryClipRgn  , (RgnHandle) window->GetUpdateRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
    SetBackground(MacGetBackgroundBrush(window));
#endif

    SetFont( window->GetFont() ) ;
}

wxPaintDC::~wxPaintDC()
{
}

void wxPaintDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, _T("GetSize() doesn't work without window") );

    m_window->GetClientSize( width, height );
}
