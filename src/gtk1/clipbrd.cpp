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

#if wxUSE_CLIPBOARD

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

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
  
    wxDataObject *data_object = clipboard->m_receivedData;
    
    if (!data_object) return;
    
    if (selection_data->length <= 0) return;
  
    // make sure we got the data in the correct format
    
    if (data_object->m_formatAtom != selection_data->target) return;
    
    // make sure we got the data in the correct form (selection type).
    // if so, copy data to target object
    
    switch (data_object->GetFormat())
    {
        case wxDF_TEXT:
	{
            if (selection_data->type != GDK_SELECTION_TYPE_STRING) return;
	    
	    wxTextDataObject *text_object = (wxTextDataObject *) data_object;
	    
	    wxString text = (const char*) selection_data->data;
	    
	    text_object->SetText( text );
	    
	    break;
	}
	
	case wxDF_BITMAP:
	{
            if (selection_data->type != GDK_SELECTION_TYPE_BITMAP) return;
	    
	    return;
	    
	    break;
	}
	
	case wxDF_PRIVATE:
	{
            if (selection_data->type != GDK_SELECTION_TYPE_STRING) return;
	    
	    wxPrivateDataObject *private_object = (wxPrivateDataObject *) data_object;
	    
	    private_object->SetData( (const char*) selection_data->data, (size_t) selection_data->length );
	    
	    break;
	}
	
	default:
	{
	    return;
	}
    }
    
    wxTheClipboard->m_formatSupported = TRUE;
}

//-----------------------------------------------------------------------------
// "selection_clear"
//-----------------------------------------------------------------------------

static gint
selection_clear_clip( GtkWidget *WXUNUSED(widget), GdkEventSelection *event )
{
    if (!wxTheClipboard) return TRUE;
    
    if (event->selection == GDK_SELECTION_PRIMARY)
    {
        wxTheClipboard->m_ownsPrimarySelection = FALSE;
    }
    else
    if (event->selection == g_clipboardAtom)
    {
        wxTheClipboard->m_ownsClipboard = FALSE;
    }
    else
    {
        return FALSE;
    }
    
    if ((!wxTheClipboard->m_ownsPrimarySelection) &&
        (!wxTheClipboard->m_ownsClipboard))
    {
        // the clipboard is no longer in our hands. we can the
        // clipboard data.
    
        wxTheClipboard->m_dataObjects.Clear();
    }
  
    return TRUE;
}

//-----------------------------------------------------------------------------
// selection handler for supplying data
//-----------------------------------------------------------------------------

static void
selection_handler( GtkWidget *WXUNUSED(widget), GtkSelectionData *selection_data, gpointer WXUNUSED(data) )
{
    if (!wxTheClipboard) return;
  
    wxNode *node = wxTheClipboard->m_dataObjects.First();
    
    while (node)
    {
        wxDataObject *data_object = (wxDataObject *)node->Data();
    
	if (data_object->m_formatAtom != selection_data->target)
	{
	    node = node->Next();
	    break;
	}
	
	switch (data_object->GetFormat())
	{
	    case wxDF_TEXT:
	    {
	        wxTextDataObject *text_object = (wxTextDataObject*) data_object;
	    
	        wxString text = text_object->GetText();
	    
		char *s = WXSTRINGCAST text;
		int len = (int) text.Length();
		
                gtk_selection_data_set( 
                    selection_data, 
		    GDK_SELECTION_TYPE_STRING, 
		    8*sizeof(gchar),
		    (unsigned char*) s, 
		    len ); 
		    
		break;
	    }
	    
	    case wxDF_BITMAP:
	    {
	        // wxBitmapDataObject *private_object = (wxBitmapDataObject*) data_object;
	    
		// how do we do that ?
		
		break;
	    }
	    
	    case wxDF_PRIVATE:
            {
	        wxPrivateDataObject *private_object = (wxPrivateDataObject*) data_object;
	    
	        if (private_object->GetDataSize() == 0) return;
	    
                gtk_selection_data_set( 
                    selection_data, 
		    GDK_SELECTION_TYPE_STRING, 
		    8*sizeof(gchar), 
		    (unsigned char*) private_object->GetData(), 
		    (int) private_object->GetDataSize() );
	    }
	    
	    default:
	      break;
        }
	
	node = node->Next();
    }
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
    m_open = FALSE;

    m_ownsClipboard = FALSE;
    m_ownsPrimarySelection = FALSE;

    m_dataObjects.DeleteContents( TRUE );
  
    m_receivedData = (wxDataObject*) NULL;

    m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_clipboardWidget );

    gtk_signal_connect( GTK_OBJECT(m_clipboardWidget), 
                        "selection_clear_event",
		        GTK_SIGNAL_FUNC( selection_clear_clip ), 
		        (gpointer) NULL );
		      
    if (!g_clipboardAtom) g_clipboardAtom = gdk_atom_intern( "CLIPBOARD", FALSE );
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "TEXT", FALSE );
    if (!g_targetsAtom) g_targetsAtom = gdk_atom_intern ("TARGETS", FALSE);
  
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
    if (m_dataObjects.GetCount())
    { 
        /*  As we have data we also own the clipboard. Once we no longer own
            it, clear_selection is called which will set m_data to zero */
     
        if (gdk_selection_owner_get( g_clipboardAtom ) == m_clipboardWidget->window)
        {
            gtk_selection_owner_set( (GtkWidget*) NULL, g_clipboardAtom, GDK_CURRENT_TIME );
        }
    
        if (gdk_selection_owner_get( GDK_SELECTION_PRIMARY ) == m_clipboardWidget->window)
        {
            gtk_selection_owner_set( (GtkWidget*) NULL, GDK_SELECTION_PRIMARY, GDK_CURRENT_TIME );
        }
    
        m_dataObjects.Clear();
    }
  
    m_targetRequested = 0;
  
    m_formatSupported = FALSE;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, FALSE, "clipboard already open" );
  
    m_open = TRUE;
  
    return TRUE;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, "data is invalid" );
  
    wxNode *node = m_dataObjects.First();
    
    while (node)
    {
        wxDataObject *d = (wxDataObject*)node->Data();
	
	if (d->GetFormat() == data->GetFormat())
	{
	    m_dataObjects.DeleteNode( node );
	    
	    break;
	}
	
        node = node->Next();
    }
    
    m_dataObjects.Append( data );
  
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );
  
    if (data->GetFormat() == wxDF_PRIVATE)
    {
        wxPrivateDataObject* pd = (wxPrivateDataObject*) data;
	
        wxCHECK_MSG( !pd->GetId().IsEmpty(), FALSE, "private clipboard format requires ID string" );
	
        data->m_formatAtom = GetTargetAtom( data->GetFormat(), pd->GetId() );
    }
    else
    {
        data->m_formatAtom = GetTargetAtom( data->GetFormat() );
    }
    
    // This should happen automatically
      
    m_ownsClipboard = FALSE;
    m_ownsPrimarySelection = FALSE;
    
    // Add handlers if someone requests data
  
    gtk_selection_add_handler( m_clipboardWidget, 
                               g_clipboardAtom,
			       data->m_formatAtom,
			       selection_handler,
			       NULL );
			       
    gtk_selection_add_handler( m_clipboardWidget, 
                               GDK_SELECTION_PRIMARY,
			       data->m_formatAtom,
			       selection_handler,
			       NULL );
			       
    // Tell the world we offer clipboard data
  
    if (!gtk_selection_owner_set( m_clipboardWidget, 
                                  g_clipboardAtom,
				  GDK_CURRENT_TIME ))
    {
        return FALSE;
    }
    m_ownsClipboard = TRUE;
    
    if (!gtk_selection_owner_set( m_clipboardWidget, 
                                  GDK_SELECTION_PRIMARY,
				  GDK_CURRENT_TIME ))
    {
        return FALSE;
    }
    m_ownsPrimarySelection = TRUE;
			     
    return TRUE;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, "clipboard not open" );
    
    m_open = FALSE;
}

bool wxClipboard::IsSupportedFormat( wxDataFormat format, const wxString &id )
{
    m_targetRequested = GetTargetAtom( format, id );
  
    if (m_targetRequested == 0) return FALSE;
    
    // add handler for target (= format) query

    gtk_signal_connect( GTK_OBJECT(m_clipboardWidget), 
                        "selection_received",
		        GTK_SIGNAL_FUNC( targets_selection_received ), 
		        (gpointer) this );
  
    m_formatSupported = FALSE;
  
    // perform query. this will set m_formatSupported to 
    // TRUE if m_targetRequested is supported
    
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

bool wxClipboard::GetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );
    
    m_receivedData = data;
    
    wxCHECK_MSG( m_receivedData, FALSE, "invalid data object" );
    
    if (m_receivedData->GetFormat() == wxDF_PRIVATE)
    {
        wxPrivateDataObject* pd = (wxPrivateDataObject*) m_receivedData;
	
        wxCHECK_MSG( !pd->GetId().IsEmpty(), FALSE, "private clipboard format requires ID string" );
	
        m_targetRequested = GetTargetAtom( m_receivedData->GetFormat(), pd->GetId() );
    }
    else
    {
        m_targetRequested = GetTargetAtom( m_receivedData->GetFormat() );
    }
    
    data->m_formatAtom = m_targetRequested;
  
    wxCHECK_MSG( m_targetRequested, FALSE, "unsupported clipboard format" );
  
    m_formatSupported = FALSE;
  
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

    wxCHECK_MSG( m_formatSupported, FALSE, "error retrieving data from clipboard" );
  
    return TRUE;
}

GdkAtom wxClipboard::GetTargetAtom( wxDataFormat format, const wxString &id )
{
    // What is X representation of that format?
  
    switch (format)
    {
        case wxDF_TEXT:
        {
            return GDK_TARGET_STRING;
				// g_textAtom
        }
      
        case wxDF_BITMAP:
        {
            return GDK_TARGET_BITMAP;
            break;
        }
      
        case wxDF_PRIVATE:
        {
            // we create our own X representation
    
            return gdk_atom_intern( WXSTRINGCAST( id ), FALSE );
        }
	
	default:
	{
	    return (GdkAtom) 0;
	}
    }
	
    return (GdkAtom) 0;
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

#endif

  // wxUSE_CLIPBOARD

