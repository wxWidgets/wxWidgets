///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for Mac
// Author:      Stefan Csomor
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2004 Stefan Csomor
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/strconv.h"
    #include "wx/control.h"
#endif //WX_PRECOMP

#include "wx/mac/uma.h"
#include "wx/mac/aga.h"
#include "wx/tooltip.h"
#include "wx/dnd.h"

#if wxUSE_SYSTEM_OPTIONS
    #include "wx/sysopt.h"
#endif

#ifndef __DARWIN__
#include <ToolUtils.h>
#endif

// for targeting OSX
#include "wx/mac/private.h"

// ============================================================================
// wxTopLevelWindowMac implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxTopLevelWindowMac, wxTopLevelWindowBase)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac creation
// ----------------------------------------------------------------------------

typedef struct
{
    wxPoint m_position ;
    wxSize m_size ;
    bool m_wasResizable ;
} FullScreenData ;

void wxTopLevelWindowMac::Init()
{
    m_iconized =
    m_maximizeOnShow = false;
    m_macFullScreenData = NULL ;
}

bool wxTopLevelWindowMac::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    if ( !wxNonOwnedWindow::Create(parent, id, pos, size, style, name) )
        return false;

    wxWindow::SetLabel( title ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title , m_font.GetEncoding() ) ;
    wxTopLevelWindows.Append(this);

    return true;
}

bool wxTopLevelWindowMac::Destroy()
{
    // NB: this will get called during destruction if we don't do it now,
    // and may fire a kill focus event on a control being destroyed
    if (m_macWindow)
        ClearKeyboardFocus( (WindowRef)m_macWindow );
        
    return wxTopLevelWindowBase::Destroy();
}

wxTopLevelWindowMac::~wxTopLevelWindowMac()
{
    FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
    delete data ;
    m_macFullScreenData = NULL ;
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::Maximize(bool maximize)
{
    Point idealSize = { 0 , 0 } ;
    if ( maximize )
    {
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
        HIRect bounds ;
        HIWindowGetAvailablePositioningBounds(kCGNullDirectDisplay,kHICoordSpace72DPIGlobal,
            &bounds);
        idealSize.h = bounds.size.width;
        idealSize.v = bounds.size.height;
#else
        Rect rect ;
        GetAvailableWindowPositioningBounds(GetMainDevice(),&rect) ;
        idealSize.h = rect.right - rect.left ;
        idealSize.v = rect.bottom - rect.top ;
#endif
    }
    ZoomWindowIdeal( (WindowRef)m_macWindow , maximize ? inZoomOut : inZoomIn , &idealSize ) ;
}

bool wxTopLevelWindowMac::IsMaximized() const
{
    return IsWindowInStandardState( (WindowRef)m_macWindow , NULL , NULL ) ;
}

void wxTopLevelWindowMac::Iconize(bool iconize)
{
    if ( IsWindowCollapsable( (WindowRef)m_macWindow) )
        CollapseWindow( (WindowRef)m_macWindow , iconize ) ;
}

bool wxTopLevelWindowMac::IsIconized() const
{
    return IsWindowCollapsed((WindowRef)m_macWindow ) ;
}

void wxTopLevelWindowMac::Restore()
{
    if ( IsMaximized() )
        Maximize(false);
    else if ( IsIconized() )
        Iconize(false);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac misc
// ----------------------------------------------------------------------------

wxPoint wxTopLevelWindowMac::GetClientAreaOrigin() const
{
    return wxPoint(0, 0) ;
}


void wxTopLevelWindowMac::SetTitle(const wxString& title)
{
    SetLabel( title ) ;
}

void wxTopLevelWindowMac::SetLabel(const wxString& title)
{
    wxWindow::SetLabel( title ) ;
    UMASetWTitle( (WindowRef)m_macWindow , title , GetFont().GetEncoding() ) ;
}

wxString wxTopLevelWindowMac::GetTitle() const
{
    return wxWindow::GetLabel();
}

bool wxTopLevelWindowMac::ShowFullScreen(bool show, long style)
{
    if ( show )
    {
        FullScreenData *data = (FullScreenData *)m_macFullScreenData ;
        delete data ;
        data = new FullScreenData() ;

        m_macFullScreenData = data ;
        data->m_position = GetPosition() ;
        data->m_size = GetSize() ;
        data->m_wasResizable = MacGetWindowAttributes() & kWindowResizableAttribute ;

        if ( style & wxFULLSCREEN_NOMENUBAR )
            HideMenuBar() ;

        wxRect client = wxGetClientDisplayRect() ;

        int left , top , right , bottom ;
        int x, y, w, h ;

        x = client.x ;
        y = client.y ;
        w = client.width ;
        h = client.height ;

        MacGetContentAreaInset( left , top , right , bottom ) ;

        if ( style & wxFULLSCREEN_NOCAPTION )
        {
            y -= top ;
            h += top ;
        }

        if ( style & wxFULLSCREEN_NOBORDER )
        {
            x -= left ;
            w += left + right ;
            h += bottom ;
        }

        if ( style & wxFULLSCREEN_NOTOOLBAR )
        {
            // TODO
        }

        if ( style & wxFULLSCREEN_NOSTATUSBAR )
        {
            // TODO
        }

        SetSize( x , y , w, h ) ;
        if ( data->m_wasResizable )
            MacChangeWindowAttributes( kWindowNoAttributes , kWindowResizableAttribute ) ;
    }
    else if ( m_macFullScreenData != NULL )
    {
        ShowMenuBar() ;
        FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
        if ( data->m_wasResizable )
            MacChangeWindowAttributes( kWindowResizableAttribute ,  kWindowNoAttributes ) ;
        SetPosition( data->m_position ) ;
        SetSize( data->m_size ) ;

        delete data ;
        m_macFullScreenData = NULL ;
    }

    return false;
}

bool wxTopLevelWindowMac::IsFullScreen() const
{
    return m_macFullScreenData != NULL ;
}

// Attracts the users attention to this window if the application is
// inactive (should be called when a background event occurs)

static pascal void wxMacNMResponse( NMRecPtr ptr )
{
    NMRemove( ptr ) ;
    DisposePtr( (Ptr)ptr ) ;
}

void wxTopLevelWindowMac::RequestUserAttention(int WXUNUSED(flags))
{
    NMRecPtr notificationRequest = (NMRecPtr) NewPtr( sizeof( NMRec) ) ;
    static wxMacNMUPP nmupp( wxMacNMResponse );

    memset( notificationRequest , 0 , sizeof(*notificationRequest) ) ;
    notificationRequest->qType = nmType ;
    notificationRequest->nmMark = 1 ;
    notificationRequest->nmIcon = 0 ;
    notificationRequest->nmSound = 0 ;
    notificationRequest->nmStr = NULL ;
    notificationRequest->nmResp = nmupp ;

    verify_noerr( NMInstall( notificationRequest ) ) ;
}
