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
GdkAtom  g_targetsAtom     = 0;

//-----------------------------------------------------------------------------
// reminder
//-----------------------------------------------------------------------------

/* The contents of a selection are returned in a GtkSelectionData
   structure. selection/target identify the request. 
   type specifies the type of the return; if length < 0, and
   the data should be ignored. This structure has object semantics -
   no fields should be modified directly, they should not be created
   directly, and pointers to them should not be stored beyond the duration of
   a callback. (If the last is changed, we'll need to add reference
   counting)

struct _GtkSelectionData
{
  GdkAtom selection;
  GdkAtom target;
  GdkAtom type;
  gint	  format;
  guchar *data;
  gint	  length;
};

*/

//-----------------------------------------------------------------------------
// "selection_received" for targets
//-----------------------------------------------------------------------------

static void
targets_selection_received( GtkWidget *WXUNUSED(widget), 
                            GtkSelectionData *selection_data, 
		            wxClipboard *clipboard )
{
  if (!wxTheClipboard) return;
  
  if (selection_data->length <= 0) return;
  
  // make sure we got the data in the correct form 
  if (selection_data->type != GDK_SELECTION_TYPE_ATOM) return;
  
  // the atoms we received, holding a list of targets (= formats) 
  GdkAtom *atoms = (GdkAtom *)selection_data->data;

  for (unsigned int i=0; i<selection_data->length/sizeof(GdkAtom); i++)
  {
     if (atoms[i] == clipboard->m_targetRequested)
     {
       clipboard->m_formatSupported = TRUE;
       return;
     }
  }

  return;
}

//-----------------------------------------------------------------------------
// "selection_received" for the actual data
//-----------------------------------------------------------------------------

static void 
selection_received( GtkWidget *WXUNUSED(widget), 
                    GtkSelectionData *selection_data, 
		    wxClipboard *clipboard )
{
  if (!wxTheClipboard) return;
  
  if (selection_data->length <= 0) return;
  
  size_t size = (size_t) selection_data->length;
  
  // make sure we got the data in the correct form 
  if (selection_data->type != GDK_SELECTION_TYPE_STRING) return;
  
  clipboard->m_receivedSize = size;
  
  clipboard->m_receivedData = new char[size+1];
  
  memcpy( clipboard->m_receivedData, selection_data->data, size);  
}

//-----------------------------------------------------------------------------
// "selection_clear"
//-----------------------------------------------------------------------------

static gint
selection_clear( GtkWidget *WXUNUSED(widget), GdkEventSelection *WXUNUSED(event) )
{
  if (!wxTheClipboard) return TRUE;
  
  /* the clipboard is no longer in our hands. we can delete the
   * clipboard data. I hope I got that one right... */
    
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
  if (!g_targetsAtom) g_targetsAtom = gdk_atom_intern ("TARGETS", FALSE);
  
  m_receivedData = (char*)NULL;
  m_receivedSize = 0;
  m_formatSupported = FALSE;
  m_targetRequested = 0;
}

wxClipboard::~wxClipboard()
{
  Clear();  
  
  if (m_clipboardWidget) gtk_widget_destroy( m_clipboardWidget );
}

void wxClipboard::Clear()
{
  /* As we have data we also own the clipboard. Once we no longer own
     it, clear_selection is called which will set m_data to zero */
     
  if (m_data)
  { 
    delete m_data;
    gtk_selection_owner_set( (GtkWidget*) NULL, GDK_SELECTION_PRIMARY, GDK_CURRENT_TIME );
  }
  
  m_receivedSize = 0;
  
  if (m_receivedData)
  {
    delete[] m_receivedData;
    m_receivedData = (char*) NULL;
  }
  
  m_targetRequested = 0;
  
  m_formatSupported = FALSE;
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
    case wxDF_TEXT:
      gtk_selection_add_handler( m_clipboardWidget, 
                                 g_clipboardAtom, 
				 g_textAtom,
				 selection_handler,
				 NULL );
      break;
    default:
      break;
  }
}

bool wxClipboard::IsSupportedFormat( wxDataFormat format )
{
  m_targetRequested = 0;
  
  if (format == wxDF_TEXT)
  {
//     m_targetRequested = g_textAtom;
     m_targetRequested = GDK_TARGET_STRING;
  }
  
  if (m_targetRequested == 0) return FALSE;

  gtk_signal_connect( GTK_OBJECT(m_clipboardWidget), 
                      "selection_received",
		      GTK_SIGNAL_FUNC( targets_selection_received ), 
		      (gpointer) this );
  
  m_formatSupported = FALSE;
  
  gtk_selection_convert( m_clipboardWidget,
			 g_clipboardAtom, 
			 g_targetsAtom,
			 GDK_CURRENT_TIME );

  gtk_signal_disconnect_by_func( GTK_OBJECT(m_clipboardWidget), 
		                 GTK_SIGNAL_FUNC( targets_selection_received ),
		                 (gpointer) this );
  
  if (!m_formatSupported) return FALSE;
  
  return TRUE;
}

bool wxClipboard::ObtainData( wxDataFormat format )
{
  m_receivedSize = 0;
  
  if (m_receivedData)
  {
    delete[] m_receivedData;
    m_receivedData = (char*) NULL;
  }
  
  m_targetRequested = 0;
  
  if (format == wxDF_TEXT)
  {
//     m_targetRequested = g_textAtom;
     m_targetRequested = GDK_TARGET_STRING;
  }
  
  if (m_targetRequested == 0) return FALSE;

  gtk_signal_connect( GTK_OBJECT(m_clipboardWidget), 
                      "selection_received",
		      GTK_SIGNAL_FUNC( selection_received ), 
		      (gpointer) this );

  gtk_selection_convert( m_clipboardWidget,
			 g_clipboardAtom, 
			 m_targetRequested,
			 GDK_CURRENT_TIME );
  
  gtk_signal_disconnect_by_func( GTK_OBJECT(m_clipboardWidget), 
		                 GTK_SIGNAL_FUNC( selection_received ),
		                 (gpointer) this );
    
  if (m_receivedSize == 0) return FALSE;
  
  return TRUE;
}

size_t wxClipboard::GetDataSize() const
{
  return m_receivedSize;
}

void wxClipboard::GetDataHere( void *data ) const
{
  memcpy(data, m_receivedData, m_receivedSize );
}

//-----------------------------------------------------------------------------
// wxClipboardModule
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule,wxModule)

bool wxClipboardModule::OnInit()
{
  wxTheClipboard = new wxClipboard();
  
  return TRUE;
}

void wxClipboardModule::OnExit()
{
  if (wxTheClipboard) delete wxTheClipboard;
  wxTheClipboard = (wxClipboard*) NULL;
}
