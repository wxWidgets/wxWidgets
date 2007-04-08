/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/button.cpp
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/toplevel.h"
#endif

#include "wx/stockitem.h"

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)


bool wxButton::Create(wxWindow *parent,
    wxWindowID id,
    const wxString& lbl,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    wxString label(lbl);
    if (label.empty() && wxIsStockID(id))
        label = wxGetStockLabel(id);

    m_macIsUserPane = false ;

    if ( !wxButtonBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_label = label ;

    OSStatus err;
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_peer = new wxMacControl(this) ;
    if ( id == wxID_HELP )
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlContentIconRef ;
        GetIconRef(kOnSystemDisk, kSystemIconsCreator, kHelpIcon, &info.u.iconRef);
        err = CreateRoundButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
            &bounds, kControlRoundButtonNormalSize,
            &info, m_peer->GetControlRefAddr() );
    }
    else if ( label.Find('\n' ) == wxNOT_FOUND && label.Find('\r' ) == wxNOT_FOUND)
    {
#if TARGET_API_MAC_OSX
        // Button height is static in Mac, can't be changed, so we need to force it here
        if ( GetWindowVariant() == wxWINDOW_VARIANT_NORMAL || GetWindowVariant() == wxWINDOW_VARIANT_LARGE )
        {
            bounds.bottom = bounds.top + 20 ;
            m_maxHeight = 20 ;
        }
        else if ( GetWindowVariant() == wxWINDOW_VARIANT_SMALL )
        {
            bounds.bottom = bounds.top + 17 ;
            m_maxHeight = 17 ;
        }
        else if ( GetWindowVariant() == wxWINDOW_VARIANT_MINI )
        {
            bounds.bottom = bounds.top + 15 ;
            m_maxHeight = 15 ;
        }
#endif

        err = CreatePushButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
            &bounds, CFSTR(""), m_peer->GetControlRefAddr() );
    }
    else
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlNoContent ;
        err = CreateBevelButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds, CFSTR(""),
            kControlBevelButtonLargeBevel, kControlBehaviorPushbutton,
            &info, 0, 0, 0, m_peer->GetControlRefAddr() );
    }

    verify_noerr( err );
    wxASSERT_MSG( m_peer != NULL && m_peer->Ok() , wxT("No valid Mac control") ) ;

    MacPostControlCreate( pos, size );

    return true;
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *btnOldDefault = wxButtonBase::SetDefault();

    if ( btnOldDefault )
    {
        // cast needed to access the protected member
        btnOldDefault->GetPeer()->SetData(kControlButtonPart , kControlPushButtonDefaultTag , (Boolean) 0 ) ;
    }

    m_peer->SetData(kControlButtonPart , kControlPushButtonDefaultTag , (Boolean) 1 ) ;

    return btnOldDefault;
}

wxSize wxButton::DoGetBestSize() const
{
    if ( GetId() == wxID_HELP )
        return wxSize( 20 , 20 ) ;

    wxSize sz = GetDefaultSize() ;
    int charspace = 8 ;

    switch (GetWindowVariant())
    {
        case wxWINDOW_VARIANT_NORMAL:
        case wxWINDOW_VARIANT_LARGE:
            sz.y = 20 ;
            charspace = 10 ;
            break;

        case wxWINDOW_VARIANT_SMALL:
            sz.y = 17 ;
            break;

        case wxWINDOW_VARIANT_MINI:
            sz.y = 15 ;
            break;

        default:
            break;
    }

    Rect    bestsize = { 0 , 0 , 0 , 0 } ;
    m_peer->GetBestRect( &bestsize ) ;

    int wBtn;
    if ( EmptyRect( &bestsize ) )
    {
        wBtn = m_label.length() * charspace + 12 ;
    }
    else
    {
        wBtn = bestsize.right - bestsize.left ;
        sz.y = bestsize.bottom - bestsize.top ;
    }

    if ((wBtn > sz.x) || ( GetWindowStyle() & wxBU_EXACTFIT))
        sz.x = wBtn;

    return sz ;
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 70 ;
    int hBtn = 20 ;

    return wxSize(wBtn, hBtn);
}

void wxButton::Command (wxCommandEvent & event)
{
    m_peer->Flash(kControlButtonPart) ;
    ProcessCommand(event);
}

wxInt32 wxButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId);
    event.SetEventObject(this);
    ProcessCommand(event);

    return noErr;
}
