/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

#include "wx/statbox.h"

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox,wxControl)

wxStaticBox::wxStaticBox(void)
{
}

wxStaticBox::wxStaticBox( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_needParent = TRUE;

  PreCreation( parent, id, pos, size, style, name );

  m_widget = gtk_frame_new(m_label);

  PostCreation();

  SetLabel(label);
  
  Show( TRUE );

  return TRUE;
}

void wxStaticBox::SetLabel( const wxString &label )
{
  wxControl::SetLabel( label );
  GtkFrame *frame = GTK_FRAME( m_widget );
  gtk_frame_set_label( frame, GetLabel() );
}
