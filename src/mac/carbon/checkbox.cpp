/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#include "wx/checkbox.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

#include <wx/mac/uma.h>

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 1, 
	  	kControlCheckBoxProc , (long) this ) ;
	
	MacPostControlCreate() ;

  return TRUE;
}

void wxCheckBox::SetValue(bool val)
{
   ::SetControlValue( m_macControl , val ) ;
}

bool wxCheckBox::GetValue() const
{
    return ::GetControlValue( m_macControl ) ;
}

void wxCheckBox::Command (wxCommandEvent & event)
{
    SetValue ((event.GetInt() != 0));
    ProcessCommand (event);
}

void wxCheckBox::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	SetValue( !GetValue() ) ;
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId );
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
}

// Bitmap checkbox
bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *label,
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
}

void wxBitmapCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize( x , y , width , height , sizeFlags ) ;
}

void wxBitmapCheckBox::SetValue(bool val)
{
    // TODO
}

bool wxBitmapCheckBox::GetValue() const
{
    // TODOD
    return FALSE;
}


