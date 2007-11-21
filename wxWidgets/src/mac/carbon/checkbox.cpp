/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"
#include "wx/mac/uma.h"


IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)


// Single check box item
bool wxCheckBox::Create(wxWindow *parent,
    wxWindowID id,
    const wxString& label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    m_macIsUserPane = false ;

    if ( !wxCheckBoxBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_label = label ;

    SInt32 maxValue = 1 /* kControlCheckboxCheckedValue */;
    if (style & wxCHK_3STATE)
        maxValue = 2 /* kControlCheckboxMixedValue */;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_peer = new wxMacControl( this ) ;
    verify_noerr( CreateCheckBoxControl(MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds ,
        CFSTR("") , 0 , false , m_peer->GetControlRefAddr() ) );

    m_peer->SetMaximum( maxValue ) ;

    MacPostControlCreate(pos, size) ;

    return true;
}

void wxCheckBox::SetValue(bool val)
{
    if (val)
        Set3StateValue(wxCHK_CHECKED);
    else
        Set3StateValue(wxCHK_UNCHECKED);
}

bool wxCheckBox::GetValue() const
{
    return (DoGet3StateValue() != 0);
}

void wxCheckBox::Command(wxCommandEvent & event)
{
    int state = event.GetInt();

    wxCHECK_RET( (state == wxCHK_UNCHECKED) || (state == wxCHK_CHECKED)
        || (state == wxCHK_UNDETERMINED),
        wxT("event.GetInt() returned an invalid checkbox state") );

    Set3StateValue((wxCheckBoxState)state);

    ProcessCommand(event);
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return (wxCheckBoxState)m_peer->GetValue() ;
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState val)
{
    m_peer->SetValue( val ) ;
}

wxInt32 wxCheckBox::MacControlHit( WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCheckBoxState origState, newState;

    newState = origState = Get3StateValue();

    switch (origState)
    {
        case wxCHK_UNCHECKED:
            newState = wxCHK_CHECKED;
            break;

        case wxCHK_CHECKED:
            // If the style flag to allow the user setting the undetermined state is set,
            // then set the state to undetermined; otherwise set state to unchecked.
            newState = Is3rdStateAllowedForUser() ? wxCHK_UNDETERMINED : wxCHK_UNCHECKED;
            break;

        case wxCHK_UNDETERMINED:
            newState = wxCHK_UNCHECKED;
            break;

        default:
            break;
    }

    if (newState != origState)
    {
        Set3StateValue( newState );

        wxCommandEvent event( wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId );
        event.SetInt( newState );
        event.SetEventObject( this );
        ProcessCommand( event );
    }

    return noErr;
}

// Bitmap checkbox
bool wxBitmapCheckBox::Create(wxWindow *parent,
    wxWindowID id,
    const wxBitmap *label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& wxVALIDATOR_PARAM(validator),
    const wxString& name)
{
    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif
    m_windowStyle = style;

    if (parent)
        parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: Create the bitmap checkbox

    return false;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap *bitmap)
{
    // TODO
    wxFAIL_MSG(wxT("wxBitmapCheckBox::SetLabel() not yet implemented"));
}

void wxBitmapCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize( x , y , width , height , sizeFlags ) ;
}

void wxBitmapCheckBox::SetValue(bool val)
{
    // TODO
    wxFAIL_MSG(wxT("wxBitmapCheckBox::SetValue() not yet implemented"));
}

bool wxBitmapCheckBox::GetValue() const
{
    // TODO
    wxFAIL_MSG(wxT("wxBitmapCheckBox::GetValue() not yet implemented"));

    return false;
}

#endif
