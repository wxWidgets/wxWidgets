/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/stattext.h"

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticText,wxControl)

wxStaticText::wxStaticText(void)
{
};

wxStaticText::wxStaticText( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
};

bool wxStaticText::Create( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  return TRUE;
};

wxString wxStaticText::GetLabel(void) const
{
};

void wxStaticText::SetLabel( const wxString &label )
{
  wxControl::SetLabel(label);
};
