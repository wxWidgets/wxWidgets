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

GdkAtom  g_textAtom        = 0;
GdkAtom  g_clipboardAtom   = 0;

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

static void 
selection_received( GtkWidget *widget, GtkSelectionData *selection_data, gpointer data )
{
}

//-----------------------------------------------------------------------------
// "selection_clear"
//-----------------------------------------------------------------------------

static gint
selection_clear( GtkWidget *widget, GdkEventSelection *event )
{
  /* The clipboard is no longer in our hands. We can delete the
   * clipboard data. I hope I got that one right... */
    
  if (!wxTheClipboard) return TRUE;
  
  wxTheClipboard->SetData( (wxDataObject*) NULL );
  
  return TRUE;
}

//-----------------------------------------------------------------------------
// selection handler for supplying data
//-----------------------------------------------------------------------------

static void
selection_handler( GtkWidget *WXUNUSED(widget), GtkSelectionData *selection_data, gpointer WXUNUSED(data) )
{
  if (!wxTheClipboard) return;
  
  wxDataObject *data_object = wxTheClipboard->m_data;
  
  if (!data_object) return;
  
  if (data_object->GetDataSize() == 0) return;  
  
  gint len = data_object->GetDataSize();
  guchar *bin_data = (guchar*) malloc( len );
  data_object->GetDataHere( (void*)bin_data );
  
  if (selection_data->target == GDK_SELECTION_TYPE_STRING)
  {
    gtk_selection_data_set( 
      selection_data, GDK_SELECTION_TYPE_STRING, 8*sizeof(gchar), bin_data, len );
  }
  else if (selection_data->target == g_textAtom)
  {
    gtk_selection_data_set( 
      selection_data, g_textAtom, 8*sizeof(gchar), bin_data, len );
  }
  free( bin_data );
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
  m_data = (wxDataObject*) NULL;
  m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
  gtk_widget_realize( m_clipboardWidget );
  
  gtk_signal_connect( GTK_OBJECT(m_clipboardWidget), 
                      "selection_clear_event",
		      GTK_SIGNAL_FUNC( selection_clear ), 
		      (gpointer) NULL );
		      
  if (!g_clipboardAtom) g_clipboardAtom = gdk_atom_intern( "CLIPBOARD", FALSE );
  if (!g_textAtom) g_textAtom = gdk_atom_intern( "TEXT", FALSE );
}

wxClipboard::~wxClipboard()
{
  /* As we have data we also own the clipboard. Once we no longer own
     it, clear_selection is called which will set m_data to zero */
     
  if (m_data)
  { 
    delete m_data;
    gtk_selection_owner_set( (GtkWidget*) NULL, GDK_SELECTION_PRIMARY, GDK_CURRENT_TIME );
  }
  if (m_clipboardWidget) gtk_widget_destroy( m_clipboardWidget );
}

void wxClipboard::SetData( wxDataObject *data )
{
  if (m_data) delete m_data;
  m_data = data;
  if (!m_data) return;
 
  if (!gtk_selection_owner_set( m_clipboardWidget, 
                                g_clipboardAtom,
				GDK_CURRENT_TIME))
  {
    delete m_data;
    m_data = (wxDataObject*) NULL;
    return;
  }
  
  switch (m_data->GetPreferredFormat())
  {
/*
    case wxDF_STRING:
      gtk_selection_add_handler( m_clipboardWidget, 
                                 g_clipboardAtom, 
				 GDK_TARGET_STRING,
				 selection_handler,
				 NULL );
      break;
    case wxDF_TEXT:
      gtk_selection_add_handler( m_clipboardWidget, 
                                 g_clipboardAtom, 
				 g_textAtom,
				 selection_handler,
				 NULL );
      break;
*/
    default:
      break;
  }
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
