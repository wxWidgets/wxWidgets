/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "clipbrd.h"
#endif

#include "wx/clipbrd.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

wxClipboard *wxTheClipboard = (wxClipboard*) NULL;

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

void wxInitClipboard()
{
  if (wxTheClipboard) delete wxTheClipboard;
  wxTheClipboard = new wxClipboard();
}

void wxDoneClipboard()
{
  if (wxTheClipboard) delete wxTheClipboard;
  wxTheClipboard = (wxClipboard*) NULL;
}

//-----------------------------------------------------------------------------
// "selection_received"
//-----------------------------------------------------------------------------

/*
static void selection_received( GtkWidget *widget, GtkSelectionData *selection_data, gpointer data )
{
}
*/

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
  m_data = (wxDataObject*)NULL;
  m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
  gtk_widget_realize( m_clipboardWidget );
}

wxClipboard::~wxClipboard()
{
  if (m_data) delete m_data;
  if (m_clipboardWidget) gtk_widget_destroy( m_clipboardWidget );
}

void wxClipboard::SetData( wxDataObject *data )
{
  if (m_data) delete m_data;
  m_data = data;
}

void *wxClipboard::GetData( wxDataFormat format, size_t *length )
{
  if (!IsAvailable(format))
  {
    if (length) *length = 0;
    return NULL;
  }
  return NULL;
}

bool wxClipboard::IsAvailable( wxDataFormat WXUNUSED(format) )
{
  return FALSE;
}
