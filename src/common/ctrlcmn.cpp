/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlcmn.cpp
// Purpose:     wxControl common interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "controlbase.h"
    #pragma implementation "statbmpbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/control.h"
    #include "wx/log.h"
#endif

#if wxUSE_STATBMP
    #include "wx/bitmap.h"
    #include "wx/statbmp.h"
#endif // wxUSE_STATBMP

// ============================================================================
// implementation
// ============================================================================

bool wxControlBase::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxValidator& validator,
                           const wxString &name)
{
    bool ret = wxWindow::Create(parent, id, pos, size, style, name);

#if wxUSE_VALIDATORS
    if ( ret )
        SetValidator(validator);
#endif // wxUSE_VALIDATORS

    return ret;
}

bool wxControlBase::CreateControl(wxWindowBase *parent,
                                  wxWindowID id,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    // even if it's possible to create controls without parents in some port,
    // it should surely be discouraged because it doesn't work at all under
    // Windows
    wxCHECK_MSG( parent, FALSE, wxT("all controls must have parents") );

    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

    parent->AddChild(this);

    return TRUE;
}

// inherit colour and font settings from the parent window
void wxControlBase::InheritAttributes()
{
    SetBackgroundColour(GetParent()->GetBackgroundColour());
    SetForegroundColour(GetParent()->GetForegroundColour());
    SetFont(GetParent()->GetFont());
}

void wxControlBase::Command(wxCommandEvent& event)
{
    (void)GetEventHandler()->ProcessEvent(event);
}

void wxControlBase::InitCommandEvent(wxCommandEvent& event) const
{
    event.SetEventObject((wxControlBase *)this);    // const_cast

    // event.SetId(GetId()); -- this is usuall done in the event ctor

    switch ( m_clientDataType )
    {
        case ClientData_Void:
            event.SetClientData(GetClientData());
            break;

        case ClientData_Object:
            event.SetClientObject(GetClientObject());
            break;

        case ClientData_None:
            // nothing to do
            ;
    }
}

// ----------------------------------------------------------------------------
// wxStaticBitmap
// ----------------------------------------------------------------------------

#if wxUSE_STATBMP

wxSize wxStaticBitmapBase::DoGetBestClientSize() const
{
    wxBitmap bmp = GetBitmap();
    if ( bmp.Ok() )
        return wxSize(bmp.GetWidth(), bmp.GetHeight());

    // this is completely arbitrary
    return wxSize(16, 16);
}

#endif // wxUSE_STATBMP

#endif // wxUSE_CONTROLS

