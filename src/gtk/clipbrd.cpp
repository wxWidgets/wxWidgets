/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/clipbrd.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "clipbrd.h"
#endif

#include "wx/clipbrd.h"

#if wxUSE_CLIPBOARD

#include "wx/dataobj.h"
#include "wx/utils.h"
#include "wx/log.h"

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// thread system
//-----------------------------------------------------------------------------

#if wxUSE_THREADS
extern void wxapp_install_thread_wakeup();
extern void wxapp_uninstall_thread_wakeup();
#endif

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

GdkAtom  g_clipboardAtom   = 0;
GdkAtom  g_targetsAtom     = 0;

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
static const wxChar *TRACE_CLIPBOARD = _T("clipboard");

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
  gint    format;
  guchar *data;
  gint    length;
};

*/

//-----------------------------------------------------------------------------
// "selection_received" for targets
//-----------------------------------------------------------------------------

static void
targets_selection_received( GtkWidget *WXUNUSED(widget),
                            GtkSelectionData *selection_data,
#if (GTK_MINOR_VERSION > 0)
                            guint32 WXUNUSED(time),
#endif
                            wxClipboard *clipboard )
{
    if ( wxTheClipboard && selection_data->length > 0 )
    {
        /* make sure we got the data in the correct form */
        GdkAtom type = selection_data->type;
        if ( type != GDK_SELECTION_TYPE_ATOM )
        {
            if ( strcmp(gdk_atom_name(type), "TARGETS") )
            {
                wxLogTrace( TRACE_CLIPBOARD,
                            _T("got unsupported clipboard target") );

                clipboard->m_waiting = FALSE;
                return;
            }
        }

#ifdef __WXDEBUG__
        wxDataFormat clip( selection_data->selection );
        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("selection received for targets, clipboard %s"),
                    clip.GetId().c_str() );
#endif // __WXDEBUG__

        // the atoms we received, holding a list of targets (= formats)
        GdkAtom *atoms = (GdkAtom *)selection_data->data;

        for (unsigned int i=0; i<selection_data->length/sizeof(GdkAtom); i++)
        {
            wxDataFormat format( atoms[i] );

            wxLogTrace( TRACE_CLIPBOARD,
                        wxT("selection received for targets, format %s"),
                        format.GetId().c_str() );

            if (format == clipboard->m_targetRequested)
            {
                clipboard->m_waiting = FALSE;
                clipboard->m_formatSupported = TRUE;
                return;
            }
        }
    }

    clipboard->m_waiting = FALSE;
}

//-----------------------------------------------------------------------------
// "selection_received" for the actual data
//-----------------------------------------------------------------------------

static void
selection_received( GtkWidget *WXUNUSED(widget),
                    GtkSelectionData *selection_data,
#if (GTK_MINOR_VERSION > 0)
                    guint32 WXUNUSED(time),
#endif
                    wxClipboard *clipboard )
{
    if (!wxTheClipboard)
    {
        clipboard->m_waiting = FALSE;
        return;
    }

    wxDataObject *data_object = clipboard->m_receivedData;

    if (!data_object)
    {
        clipboard->m_waiting = FALSE;
        return;
    }

    if (selection_data->length <= 0)
    {
        clipboard->m_waiting = FALSE;
        return;
    }

    wxDataFormat format( selection_data->target );

    /* make sure we got the data in the correct format */
    if (!data_object->IsSupportedFormat( format ) )
    {
        clipboard->m_waiting = FALSE;
        return;
    }

    /* make sure we got the data in the correct form (selection type).
       if so, copy data to target object */
    if (selection_data->type != GDK_SELECTION_TYPE_STRING)
    {
        clipboard->m_waiting = FALSE;
        return;
    }

    data_object->SetData( format, (size_t) selection_data->length, (const char*) selection_data->data );

    wxTheClipboard->m_formatSupported = TRUE;
    clipboard->m_waiting = FALSE;
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
        wxTheClipboard->m_waiting = FALSE;
        return FALSE;
    }

    if ((!wxTheClipboard->m_ownsPrimarySelection) &&
        (!wxTheClipboard->m_ownsClipboard))
    {
        /* the clipboard is no longer in our hands. we can the delete clipboard data. */
        if (wxTheClipboard->m_data)
        {
            wxLogDebug( wxT("wxClipboard will get cleared" ) );

            delete wxTheClipboard->m_data;
            wxTheClipboard->m_data = (wxDataObject*) NULL;
        }
    }

    wxTheClipboard->m_waiting = FALSE;
    return TRUE;
}

//-----------------------------------------------------------------------------
// selection handler for supplying data
//-----------------------------------------------------------------------------

static void
selection_handler( GtkWidget *WXUNUSED(widget),
                   GtkSelectionData *selection_data,
                   guint WXUNUSED(info),
                   guint WXUNUSED(time),
                   gpointer WXUNUSED(data) )
{
    if (!wxTheClipboard) return;

    if (!wxTheClipboard->m_data) return;

    wxDataObject *data = wxTheClipboard->m_data;

    wxDataFormat format( selection_data->target );

    if (!data->IsSupportedFormat( format )) return;

    /* this will fail for composite formats */
    if (format.GetType() == wxDF_TEXT)
    {
        wxTextDataObject *text_object = (wxTextDataObject*) data;
        wxString text( text_object->GetText() );

#if wxUSE_UNICODE
        const wxWX2MBbuf s = text.mbc_str();
        int len = strlen(s);
#else // more efficient in non-Unicode
        const char *s = text.c_str();
        int len = (int) text.Length();
#endif
        gtk_selection_data_set(
            selection_data,
            GDK_SELECTION_TYPE_STRING,
            8*sizeof(gchar),
            (unsigned char*) (const char*) s,
            len );

        return;
    }

    int size = data->GetDataSize( format );

    if (size == 0) return;

    char *d = new char[size];

    data->GetDataHere( selection_data->target, (void*) d );

    gtk_selection_data_set(
        selection_data,
        GDK_SELECTION_TYPE_STRING,
        8*sizeof(gchar),
        (unsigned char*) d,
        size );
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

    m_data = (wxDataObject*) NULL;
    m_receivedData = (wxDataObject*) NULL;

    /* we use m_targetsWidget to query what formats are available */

    m_targetsWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_targetsWidget );

    gtk_signal_connect( GTK_OBJECT(m_targetsWidget),
                        "selection_received",
                        GTK_SIGNAL_FUNC( targets_selection_received ),
                        (gpointer) this );

    /* we use m_clipboardWidget to get and to offer data */

    m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_clipboardWidget );

    gtk_signal_connect( GTK_OBJECT(m_clipboardWidget),
                        "selection_received",
                        GTK_SIGNAL_FUNC( selection_received ),
                        (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_clipboardWidget),
                        "selection_clear_event",
                        GTK_SIGNAL_FUNC( selection_clear_clip ),
                        (gpointer) NULL );

    if (!g_clipboardAtom) g_clipboardAtom = gdk_atom_intern( "CLIPBOARD", FALSE );
    if (!g_targetsAtom) g_targetsAtom = gdk_atom_intern ("TARGETS", FALSE);

    m_formatSupported = FALSE;
    m_targetRequested = 0;

    m_usePrimary = FALSE;
}

wxClipboard::~wxClipboard()
{
    Clear();

    if (m_clipboardWidget) gtk_widget_destroy( m_clipboardWidget );
    if (m_targetsWidget) gtk_widget_destroy( m_targetsWidget );
}

void wxClipboard::Clear()
{
    if (m_data)
    {
#if wxUSE_THREADS
        /* disable GUI threads */
        wxapp_uninstall_thread_wakeup();
#endif

        /*  As we have data we also own the clipboard. Once we no longer own
            it, clear_selection is called which will set m_data to zero */
        if (gdk_selection_owner_get( g_clipboardAtom ) == m_clipboardWidget->window)
        {
            m_waiting = TRUE;

            gtk_selection_owner_set( (GtkWidget*) NULL, g_clipboardAtom,
                                     (guint32) GDK_CURRENT_TIME );

            while (m_waiting) gtk_main_iteration();
        }

        if (gdk_selection_owner_get( GDK_SELECTION_PRIMARY ) == m_clipboardWidget->window)
        {
            m_waiting = TRUE;

            gtk_selection_owner_set( (GtkWidget*) NULL, GDK_SELECTION_PRIMARY,
                                     (guint32) GDK_CURRENT_TIME );

            while (m_waiting) gtk_main_iteration();
        }

        if (m_data)
        {
            delete m_data;
            m_data = (wxDataObject*) NULL;
        }

#if wxUSE_THREADS
        /* re-enable GUI threads */
        wxapp_install_thread_wakeup();
#endif
    }

    m_targetRequested = 0;
    m_formatSupported = FALSE;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, FALSE, wxT("clipboard already open") );

    m_open = TRUE;

    return TRUE;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

    /* we can only store one wxDataObject */
    Clear();

    m_data = data;

    /* get formats from wxDataObjects */
    wxDataFormat *array = new wxDataFormat[ m_data->GetFormatCount() ];
    m_data->GetAllFormats( array );

    /* primary selection or clipboard */
    GdkAtom clipboard = m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                     : g_clipboardAtom;


    for (size_t i = 0; i < m_data->GetFormatCount(); i++)
    {
        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("wxClipboard now supports atom %s"),
                    array[i].GetId().c_str() );

        gtk_selection_add_target( GTK_WIDGET(m_clipboardWidget),
                                  clipboard,
                                  array[i],
                                  0 );  /* what is info ? */
    }

    delete[] array;

    gtk_signal_connect( GTK_OBJECT(m_clipboardWidget),
                        "selection_get",
                        GTK_SIGNAL_FUNC(selection_handler),
                        (gpointer) NULL );

#if wxUSE_THREADS
    /* disable GUI threads */
    wxapp_uninstall_thread_wakeup();
#endif

    /* Tell the world we offer clipboard data */
    bool res = (gtk_selection_owner_set( m_clipboardWidget,
                                         clipboard,
                                         (guint32) GDK_CURRENT_TIME ));

    if (m_usePrimary)
        m_ownsPrimarySelection = res;
    else
        m_ownsClipboard = res;

#if wxUSE_THREADS
    /* re-enable GUI threads */
    wxapp_install_thread_wakeup();
#endif

    return res;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = FALSE;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    /* reentrance problems */
    if (m_waiting) return FALSE;

    /* store requested format to be asked for by callbacks */
    m_targetRequested = format;

#if 0
    wxLogTrace( TRACE_CLIPBOARD,
                wxT("wxClipboard:IsSupported: requested format: %s"),
                format.GetId().c_str() );
#endif

    wxCHECK_MSG( m_targetRequested, FALSE, wxT("invalid clipboard format") );

    m_formatSupported = FALSE;

    /* perform query. this will set m_formatSupported to
       TRUE if m_targetRequested is supported.
       also, we have to wait for the "answer" from the
       clipboard owner which is an asynchronous process.
       therefore we set m_waiting = TRUE here and wait
       until the callback "targets_selection_received"
       sets it to FALSE */

    m_waiting = TRUE;

    gtk_selection_convert( m_targetsWidget,
                           m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                        : g_clipboardAtom,
                           g_targetsAtom,
                           (guint32) GDK_CURRENT_TIME );

    while (m_waiting) gtk_main_iteration();

    if (!m_formatSupported) return FALSE;

    return TRUE;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    /* get formats from wxDataObjects */
    wxDataFormat *array = new wxDataFormat[ data.GetFormatCount() ];
    data.GetAllFormats( array );

    for (size_t i = 0; i < data.GetFormatCount(); i++)
    {
        wxDataFormat format( array[i] );

        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("wxClipboard::GetData: requested format: %s"),
                    format.GetId().c_str() );

        /* is data supported by clipboard ? */

        /* store requested format to be asked for by callbacks */
        m_targetRequested = format;

        wxCHECK_MSG( m_targetRequested, FALSE, wxT("invalid clipboard format") );

        m_formatSupported = FALSE;

       /* perform query. this will set m_formatSupported to
          TRUE if m_targetRequested is supported.
          also, we have to wait for the "answer" from the
          clipboard owner which is an asynchronous process.
          therefore we set m_waiting = TRUE here and wait
          until the callback "targets_selection_received"
          sets it to FALSE */

        m_waiting = TRUE;

        gtk_selection_convert( m_targetsWidget,
                           m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                        : g_clipboardAtom,
                           g_targetsAtom,
                           (guint32) GDK_CURRENT_TIME );

        while (m_waiting) gtk_main_iteration();

        if (!m_formatSupported) continue;

        /* store pointer to data object to be filled up by callbacks */
        m_receivedData = &data;

        /* store requested format to be asked for by callbacks */
        m_targetRequested = format;

        wxCHECK_MSG( m_targetRequested, FALSE, wxT("invalid clipboard format") );

        /* start query */
        m_formatSupported = FALSE;

        /* ask for clipboard contents.  this will set
           m_formatSupported to TRUE if m_targetRequested
           is supported.
           also, we have to wait for the "answer" from the
           clipboard owner which is an asynchronous process.
           therefore we set m_waiting = TRUE here and wait
           until the callback "targets_selection_received"
           sets it to FALSE */

        m_waiting = TRUE;

        wxLogDebug( wxT("wxClipboard::GetData: format found, start convert") );

        gtk_selection_convert( m_clipboardWidget,
                               m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                            : g_clipboardAtom,
                               m_targetRequested,
                               (guint32) GDK_CURRENT_TIME );

        while (m_waiting) gtk_main_iteration();

        /* this is a true error as we checked for the presence of such data before */
        wxCHECK_MSG( m_formatSupported, FALSE, wxT("error retrieving data from clipboard") );

        /* return success */
        delete[] array;
        return TRUE;
    }

    wxLogDebug( wxT("wxClipboard::GetData: format not found") );

    /* return failure */
    delete[] array;
    return FALSE;
}

#endif
  // wxUSE_CLIPBOARD

