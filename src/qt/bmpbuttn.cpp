/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBitmapButton;

//-----------------------------------------------------------------------------
// wxBitmapButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton,wxControl)

wxBitmapButton::wxBitmapButton(void)
{
};

wxBitmapButton::wxBitmapButton( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, bitmap, pos, size, style, name );
};

bool wxBitmapButton::Create(  wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  wxSize newSize = size;

  PreCreation( parent, id, pos, newSize, style, name );
  
  m_bitmap = bitmap;
  m_label = "";
  
  
  if (newSize.x == -1) newSize.x = m_bitmap.GetHeight()+10;
  if (newSize.y == -1) newSize.y = m_bitmap.GetWidth()+10;
  SetSize( newSize.x, newSize.y );
  

  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};
      
void wxBitmapButton::SetDefault(void)
{
};

void wxBitmapButton::SetLabel( const wxString &label )
{
  wxControl::SetLabel( label );
};

wxString wxBitmapButton::GetLabel(void) const
{
  return wxControl::GetLabel();
};
