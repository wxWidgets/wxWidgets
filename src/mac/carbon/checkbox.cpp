/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "checkbox.h"
#endif

#include "wx/wxprec.h"

#include "wx/checkbox.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

#include "wx/mac/uma.h"

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    if ( !wxCheckBoxBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_label = label ;

    SInt32 maxValue = 1 /* kControlCheckboxCheckedValue */;
    if (style & wxCHK_3STATE)
        maxValue = 2 /* kControlCheckboxMixedValue */;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_peer = new wxMacControl() ;
    verify_noerr( CreateCheckBoxControl(MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds ,
        CFSTR("") , 0 , false , m_peer->GetControlRefAddr() ) );
    
    m_peer->SetMaximum( maxValue ) ;
    
    MacPostControlCreate(pos,size) ;

  return TRUE;
}

void wxCheckBox::SetValue(bool val)
{
    if (val)
    {
        Set3StateValue(wxCHK_CHECKED);
    }
    else
    {
        Set3StateValue(wxCHK_UNCHECKED);
    }
}

bool wxCheckBox::GetValue() const
{
    return (DoGet3StateValue() != 0);
}

void wxCheckBox::Command (wxCommandEvent & event)
{
    int state = event.GetInt();

    wxCHECK_RET( (state == wxCHK_UNCHECKED) || (state == wxCHK_CHECKED)
        || (state == wxCHK_UNDETERMINED),
        wxT("event.GetInt() returned an invalid checkbox state") );

    Set3StateValue((wxCheckBoxState) state);

    ProcessCommand(event);
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return (wxCheckBoxState) m_peer->GetValue() ;
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState val)
{
    m_peer->SetValue( val ) ;
}

wxInt32 wxCheckBox::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId );
    wxCheckBoxState state = Get3StateValue();

    if (state == wxCHK_UNCHECKED)
    {
        state = wxCHK_CHECKED;
    }
    else if (state == wxCHK_CHECKED)
    {
        // If the style flag to allow the user setting the undetermined state
        // is set, then set the state to undetermined. Otherwise set state to
        // unchecked.
        if ( Is3rdStateAllowedForUser() )
        {
            state = wxCHK_UNDETERMINED;
        }
        else
        {
            state = wxCHK_UNCHECKED;
        }
    }
    else if (state == wxCHK_UNDETERMINED)
    {
        state = wxCHK_UNCHECKED;
    }
    Set3StateValue(state);

    event.SetInt(state);
    event.SetEventObject(this);
    ProcessCommand(event);

    return noErr ;
}

// Bitmap checkbox
bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id,
                              const wxBitmap *label,
                              const wxPoint& pos,
                              const wxSize& size, long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: Create the bitmap checkbox

    return FALSE;
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
    return FALSE;
}


