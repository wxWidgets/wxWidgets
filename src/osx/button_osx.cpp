/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/button_osx.cpp
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: button.cpp 54845 2008-07-30 14:52:41Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/toplevel.h"
    #include "wx/dcclient.h"
#endif

#include "wx/stockitem.h"

#include "wx/osx/private.h"

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

    m_labelOrig = m_label = label ;
    
    m_peer = wxWidgetImpl::CreateButton( this, parent, id, label, pos, size, style, GetExtraStyle() );

    MacPostControlCreate( pos, size );

    return true;
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *btnOldDefault = wxButtonBase::SetDefault();

    if ( btnOldDefault )
    {
        btnOldDefault->GetPeer()->SetDefaultButton( false );
    }

    m_peer->SetDefaultButton( true );

    return btnOldDefault;
}

void wxButton::Command (wxCommandEvent & event)
{
    m_peer->PerformClick() ;
    // ProcessCommand(event);
}

bool wxButton::OSXHandleClicked( double timestampsec )
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId);
    event.SetEventObject(this);
    ProcessCommand(event);
    return true;
}

//-------------------------------------------------------
// wxDisclosureTriangle
//-------------------------------------------------------

bool wxDisclosureTriangle::Create(wxWindow *parent, wxWindowID id, const wxString& label,
   const wxPoint& pos, const wxSize& size, long style,const wxValidator& validator, const wxString& name )
{
    m_macIsUserPane = false ;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_peer = wxWidgetImpl::CreateDisclosureTriangle(this, parent, id, label, pos, size, style, GetExtraStyle() );

    MacPostControlCreate( pos, size );
    // passing the text in the param doesn't seem to work, so lets do if again
    SetLabel( label );
    
    return true;
}

void wxDisclosureTriangle::SetOpen( bool open )
{
    m_peer->SetValue( open ? 1 : 0 );
}

bool wxDisclosureTriangle::IsOpen() const
{
   return m_peer->GetValue() == 1;
}

bool wxDisclosureTriangle::OSXHandleClicked( double timestampsec )
{
    // Just emit button event for now
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId);
    event.SetEventObject(this);
    ProcessCommand(event);

    return true;
}

wxSize wxDisclosureTriangle::DoGetBestSize() const
{
    return wxSize(16,16);
}

