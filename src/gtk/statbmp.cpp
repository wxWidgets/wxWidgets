/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/statbmp.h"

//-----------------------------------------------------------------------------
// wxStaticBitmap
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap,wxControl)

wxStaticBitmap::wxStaticBitmap(void)
{
};

wxStaticBitmap::wxStaticBitmap( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, bitmap, pos, size, style, name );
};

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  wxSize newSize = size;
  
  PreCreation( parent, id, pos, size, style, name );

  m_bitmap = bitmap;
    
  if (m_bitmap.Ok())
  {
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (m_bitmap.GetMask()) mask = m_bitmap.GetMask()->GetBitmap();
    m_widget = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );
    
    if (newSize.x == -1) newSize.x = m_bitmap.GetWidth();
    if (newSize.y == -1) newSize.y = m_bitmap.GetHeight();
    SetSize( newSize.x, newSize.y );
  }
  else
  {
    m_widget = gtk_label_new( "Bitmap" );
  }
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};

void wxStaticBitmap::SetBitmap( const wxBitmap &bitmap ) 
{
  m_bitmap = bitmap;
  
  if (m_bitmap.Ok())
  {
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (m_bitmap.GetMask()) mask = m_bitmap.GetMask()->GetBitmap();
    gtk_pixmap_set( GTK_PIXMAP(m_widget), m_bitmap.GetPixmap(), mask );
  }
};

