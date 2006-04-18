/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/mac/uma.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent)

wxSpinButton::wxSpinButton()
   : wxSpinButtonBase()
{
}

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style, const wxString& name)
{
    if ( !wxSpinButtonBase::Create(parent, id, pos, size,
                                   style, wxDefaultValidator, name) )
        return false;

    m_min = 0;
    m_max = 100;

    if (!parent)
        return false;

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  wxEmptyString , pos , size ,style,*( (wxValidator*) NULL ) , name , &bounds , title ) ;

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 100,
        kControlLittleArrowsProc , (long) this ) ;

    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;

    MacPostControlCreate() ;

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

// Attributes
////////////////////////////////////////////////////////////////////////////

int wxSpinButton::GetMin() const
{
    return m_min;
}

int wxSpinButton::GetMax() const
{
    return m_max;
}

int wxSpinButton::GetValue() const
{
    int n = m_value;

    if (n < m_min) n = m_min;
    if (n > m_max) n = m_max;

    return n;
}

void wxSpinButton::SetValue(int val)
{
    m_value = val ;
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    SetControl32BitMaximum( (ControlHandle) m_macControl , maxVal ) ;
    SetControl32BitMinimum((ControlHandle) m_macControl , minVal ) ;
}

void wxSpinButton::MacHandleValueChanged( int inc )
{

    wxEventType scrollEvent = wxEVT_NULL;
    int oldValue = m_value ;

    m_value = oldValue + inc;

    if (m_value < m_min)
    {
        if ( m_windowStyle & wxSP_WRAP )
            m_value = m_max;
        else
            m_value = m_min;
    }

    if (m_value > m_max)
    {
        if ( m_windowStyle & wxSP_WRAP )
            m_value = m_min;
        else
            m_value = m_max;
    }

    if ( m_value - oldValue == -1 )
        scrollEvent = wxEVT_SCROLL_LINEDOWN ;
    else if ( m_value - oldValue == 1 )
        scrollEvent = wxEVT_SCROLL_LINEUP ;
    else
        scrollEvent = wxEVT_SCROLL_THUMBTRACK ;

    wxSpinEvent event(scrollEvent, m_windowId);

    event.SetPosition(m_value);
    event.SetEventObject( this );
    if ((GetEventHandler()->ProcessEvent( event )) &&
        !event.IsAllowed() )
    {
        m_value = oldValue ;
    }
    SetControl32BitValue( (ControlHandle) m_macControl , m_value ) ;

    /* always send a thumbtrack event */
    if (scrollEvent != wxEVT_SCROLL_THUMBTRACK)
    {
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
        wxSpinEvent event2( scrollEvent, GetId());
        event2.SetPosition( m_value );
        event2.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event2 );
    }
}

void wxSpinButton::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED(mouseStillDown))
{
    if ( (ControlHandle) m_macControl == NULL )
        return ;

    int nScrollInc = 0;

    switch( controlpart )
    {
    case kControlUpButtonPart :
        nScrollInc = 1;
        break ;
    case kControlDownButtonPart :
        nScrollInc = -1;
        break ;
    }
    MacHandleValueChanged( nScrollInc ) ;

}

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
    return wxSize(16,24);
}

#endif // wxUSE_SPINBTN
