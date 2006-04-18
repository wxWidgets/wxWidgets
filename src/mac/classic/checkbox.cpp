/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/checkbox.h"

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)

#include "wx/mac/uma.h"

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    if ( !wxCheckBoxBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

    SInt16 maxValue = 1 /* kControlCheckboxCheckedValue */;
    if (style & wxCHK_3STATE)
    {
        maxValue = 2 /* kControlCheckboxMixedValue */;
    }

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , maxValue,
          kControlCheckBoxProc , (long) this ) ;

    MacPostControlCreate() ;

    return true;
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
    return (wxCheckBoxState) ::GetControl32BitValue( (ControlHandle) m_macControl );
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState val)
{
    ::SetControl32BitValue( (ControlHandle) m_macControl , (int) val) ;
    MacRedrawControl() ;
}

void wxCheckBox::MacHandleControlClick( WXWidget WXUNUSED(control), wxInt16 WXUNUSED(controlpart) , bool WXUNUSED(mouseStillDown) )
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
