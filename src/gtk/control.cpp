/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

#include "wx/control.h"

//-----------------------------------------------------------------------------
// wxControl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxControl,wxWindow)

wxControl::wxControl(void)
{
  m_label = "";
  m_needParent = TRUE;
};

wxControl::wxControl( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      const long style, const wxString &name ) :
  wxWindow( parent, id, pos, size, style, name )
{
};

void wxControl::Command( wxCommandEvent &WXUNUSED(event) )
{
};

void wxControl::SetLabel( const wxString &label )
{
  m_label = label;
};

wxString wxControl::GetLabel(void) const
{
  return m_label;
};



