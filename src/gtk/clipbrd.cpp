/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/clipbrd.cpp
// Purpose:     wxClipboard implementation for wxGTK
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/dataobj.h"
#endif

#include "wx/scopeguard.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

static GdkAtom  g_clipboardAtom   = 0;
static GdkAtom  g_targetsAtom     = 0;
static GdkAtom  g_timestampAtom   = 0;

#if wxUSE_UNICODE
extern GdkAtom g_altTextAtom;
#endif

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
#define TRACE_CLIPBOARD _T("clipboard")

// ----------------------------------------------------------------------------
// wxClipboardSync: used to perform clipboard operations synchronously
// ----------------------------------------------------------------------------

// constructing this object on stack will wait wait until the latest clipboard
// operation is finished on block exit
//
// notice that there can be no more than one such object alive at any moment,
// i.e. reentrancies are not allowed
class wxClipboardSync
{
public:
    wxClipboardSync(wxClipboard& clipboard)
    {
        wxASSERT_MSG( !ms_clipboard, _T("reentrancy in clipboard code") );
        ms_clipboard = &clipboard;
    }

    ~wxClipboardSync()
    {
        while ( ms_clipboard )
            gtk_main_iteration();
    }

    // this method must be called by GTK+ callbacks to indicate that we got the
    // result for our clipboard operation
    static void OnDone(wxClipboard *clipboard)
    {
        wxASSERT_MSG( clipboard == ms_clipboard,
                        _T("got notification for alien clipboard") );

        ms_clipboard = NULL;
    }

private:
    static wxClipboard *ms_clipboard;

    DECLARE_NO_COPY_CLASS(wxClipboardSync)
};

wxClipboard *wxClipboardSync::ms_clipboard = NULL;

//-----------------------------------------------------------------------------
// "selection_received" for targets
//-----------------------------------------------------------------------------

extern "C" {
static void
targets_selection_received( GtkWidget *WXUNUSED(widget),
                            GtkSelectionData *selection_data,
                            guint32 WXUNUSED(time),
                            wxClipboard *clipboard )
{
    wxON_BLOCK_EXIT1(wxClipboardSync::OnDone, clipboard);

    if ( wxTheClipboard && selection_data->length > 0 )
    {
        // make sure we got the data in the correct form
        GdkAtom type = selection_data->type;
        if ( type != GDK_SELECTION_TYPE_ATOM )
        {
            if ( strcmp(wxGtkString(gdk_atom_name(type)), "TARGETS") )
            {
                wxLogTrace( TRACE_CLIPBOARD,
                            _T("got unsupported clipboard target") );

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

//            printf( "format %s requested %s\n",
//                    gdk_atom_name( atoms[i] ),
//                    gdk_atom_name( clipboard->m_targetRequested ) );

            if (format == clipboard->m_targetRequested)
            {
                clipboard->m_formatSupported = true;
                return;
            }
        }
    }
}
}

//-----------------------------------------------------------------------------
// "selection_received" for the actual data
//-----------------------------------------------------------------------------

extern "C" {
static void
selection_received( GtkWidget *WXUNUSED(widget),
                    GtkSelectionData *selection_data,
                    guint32 WXUNUSED(time),
                    wxClipboard *clipboard )
{
    wxON_BLOCK_EXIT1(wxClipboardSync::OnDone, clipboard);

    if (!wxTheClipboard)
        return;

    wxDataObject *data_object = clipboard->m_receivedData;

    if (!data_object)
        return;

    if (selection_data->length <= 0)
        return;

    wxDataFormat format( selection_data->target );

    // make sure we got the data in the correct format
    if (!data_object->IsSupportedFormat( format ) )
        return;

    data_object->SetData( format, (size_t) selection_data->length, (const char*) selection_data->data );

    wxTheClipboard->m_formatSupported = true;
}
}

//-----------------------------------------------------------------------------
// "selection_clear"
//-----------------------------------------------------------------------------

extern "C" {
static gint
selection_clear_clip( GtkWidget *WXUNUSED(widget), GdkEventSelection *event )
{
    wxON_BLOCK_EXIT1(wxClipboardSync::OnDone, wxTheClipboard);

    if (!wxTheClipboard) return true;

    if (event->selection == GDK_SELECTION_PRIMARY)
    {
        wxTheClipboard->m_ownsPrimarySelection = false;
    }
    else
    if (event->selection == g_clipboardAtom)
    {
        wxTheClipboard->m_ownsClipboard = false;
    }
    else
    {
        return FALSE;
    }

    if ((!wxTheClipboard->m_ownsPrimarySelection) &&
        (!wxTheClipboard->m_ownsClipboard))
    {
        /* the clipboard is no longer in our hands. we can the delete clipboard data. */
        if (wxTheClipboard->m_data)
        {
            wxLogTrace(TRACE_CLIPBOARD, wxT("wxClipboard will get cleared" ));

            delete wxTheClipboard->m_data;
            wxTheClipboard->m_data = (wxDataObject*) NULL;
        }
    }

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// selection handler for supplying data
//-----------------------------------------------------------------------------

extern "C" {
static void
selection_handler( GtkWidget *WXUNUSED(widget),
                   GtkSelectionData *selection_data,
                   guint WXUNUSED(info),
                   guint WXUNUSED(time),
                   gpointer signal_data )
{
    if (!wxTheClipboard) return;

    if (!wxTheClipboard->m_data) return;

    wxDataObject *data = wxTheClipboard->m_data;

    // ICCCM says that TIMESTAMP is a required atom.
    // In particular, it satisfies Klipper, which polls
    // TIMESTAMP to see if the clipboards content has changed.
    // It shall return the time which was used to set the data.
    if (selection_data->target == g_timestampAtom)
    {
        guint timestamp = GPOINTER_TO_UINT (signal_data);
        gtk_selection_data_set(selection_data,
                               GDK_SELECTION_TYPE_INTEGER,
                               32,
                               (guchar*)&(timestamp),
                               sizeof(timestamp));
        wxLogTrace(TRACE_CLIPBOARD,
                   _T("Clipboard TIMESTAMP requested, returning timestamp=%u"),
                   timestamp);
        return;
    }

    wxDataFormat format( selection_data->target );

#ifdef __WXDEBUG__
    wxLogTrace(TRACE_CLIPBOARD,
               _T("clipboard data in format %s, GtkSelectionData is target=%s type=%s selection=%s timestamp=%u"),
               format.GetId().c_str(),
               wxString::FromAscii(wxGtkString(gdk_atom_name(selection_data->target))).c_str(),
               wxString::FromAscii(wxGtkString(gdk_atom_name(selection_data->type))).c_str(),
               wxString::FromAscii(wxGtkString(gdk_atom_name(selection_data->selection))).c_str(),
               GPOINTER_TO_UINT( signal_data )
               );
#endif

    if (!data->IsSupportedFormat( format )) return;

    int size = data->GetDataSize( format );

    if (size == 0) return;

    void *d = malloc(size);

    // Text data will be in UTF8 in Unicode mode.
    data->GetDataHere( selection_data->target, d );

    // NB: GTK+ requires special treatment of UTF8_STRING data, the text
    //     would show as UTF-8 data interpreted as latin1 (?) in other
    //     GTK+ apps if we used gtk_selection_data_set()
    if (format == wxDataFormat(wxDF_UNICODETEXT))
    {
        gtk_selection_data_set_text(
            selection_data,
            (const gchar*)d,
            size );
    }
    else
    {
        gtk_selection_data_set(
            selection_data,
            GDK_SELECTION_TYPE_STRING,
            8*sizeof(gchar),
            (unsigned char*) d,
            size );
    }

    free(d);
}
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
    m_open = false;

    m_ownsClipboard = false;
    m_ownsPrimarySelection = false;

    m_data = (wxDataObject*) NULL;
    m_receivedData = (wxDataObject*) NULL;

    /* we use m_targetsWidget to query what formats are available */

    m_targetsWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_targetsWidget );

    g_signal_connect (m_targetsWidget, "selection_received",
                      G_CALLBACK (targets_selection_received), this);

    /* we use m_clipboardWidget to get and to offer data */

    m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_clipboardWidget );

    g_signal_connect (m_clipboardWidget, "selection_received",
                      G_CALLBACK (selection_received), this);

    g_signal_connect (m_clipboardWidget, "selection_clear_event",
                      G_CALLBACK (selection_clear_clip), NULL);

    if (!g_clipboardAtom) g_clipboardAtom = gdk_atom_intern( "CLIPBOARD", FALSE );
    if (!g_targetsAtom) g_targetsAtom = gdk_atom_intern ("TARGETS", FALSE);
    if (!g_timestampAtom) g_timestampAtom = gdk_atom_intern ("TIMESTAMP", FALSE);

    m_formatSupported = false;
    m_targetRequested = 0;

    m_usePrimary = false;
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
        //  As we have data we also own the clipboard. Once we no longer own
        //  it, clear_selection is called which will set m_data to zero
        if (gdk_selection_owner_get( g_clipboardAtom ) == m_clipboardWidget->window)
        {
            wxClipboardSync sync(*this);

            gtk_selection_owner_set( (GtkWidget*) NULL, g_clipboardAtom,
                                     (guint32) GDK_CURRENT_TIME );
        }

        if (gdk_selection_owner_get( GDK_SELECTION_PRIMARY ) == m_clipboardWidget->window)
        {
            wxClipboardSync sync(*this);

            gtk_selection_owner_set( (GtkWidget*) NULL, GDK_SELECTION_PRIMARY,
                                     (guint32) GDK_CURRENT_TIME );
        }

        delete m_data;
        m_data = NULL;
    }

    m_targetRequested = 0;
    m_formatSupported = false;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    return true;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    // we can only store one wxDataObject
    Clear();

    m_data = data;

    // get formats from wxDataObjects
    wxDataFormat *array = new wxDataFormat[ m_data->GetFormatCount() ];
    m_data->GetAllFormats( array );

    // primary selection or clipboard
    GdkAtom clipboard = m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                     : g_clipboardAtom;

    // by default provide TIMESTAMP as a target
    gtk_selection_add_target( GTK_WIDGET(m_clipboardWidget),
                              clipboard,
                              g_timestampAtom,
                              0 );

    for (size_t i = 0; i < m_data->GetFormatCount(); i++)
    {
        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("wxClipboard now supports atom %s"),
                    array[i].GetId().c_str() );

//        printf( "added %s\n",
//                    gdk_atom_name( array[i].GetFormatId() ) );

        gtk_selection_add_target( GTK_WIDGET(m_clipboardWidget),
                                  clipboard,
                                  array[i],
                                  0 );  /* what is info ? */
    }

    delete[] array;

    g_signal_connect (m_clipboardWidget, "selection_get",
                      G_CALLBACK (selection_handler),
                      GUINT_TO_POINTER (gtk_get_current_event_time()) );

#if wxUSE_THREADS
    /* disable GUI threads */
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
#endif

    return res;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    /* store requested format to be asked for by callbacks */
    m_targetRequested = format;

    wxLogTrace( TRACE_CLIPBOARD,
                wxT("wxClipboard:IsSupported: requested format: %s"),
                format.GetId().c_str() );

    wxCHECK_MSG( m_targetRequested, false, wxT("invalid clipboard format") );

    m_formatSupported = false;

    // block until m_formatSupported is set from targets_selection_received
    // callback
    {
        wxClipboardSync sync(*this);

        gtk_selection_convert( m_targetsWidget,
                               m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                            : g_clipboardAtom,
                               g_targetsAtom,
                               (guint32) GDK_CURRENT_TIME );
    } // wait until we get the results

#if wxUSE_UNICODE
    if (!m_formatSupported && format == wxDataFormat(wxDF_UNICODETEXT))
    {
        // Another try with plain STRING format
        extern GdkAtom g_altTextAtom;
        return IsSupported(g_altTextAtom);
    }
#endif

    return m_formatSupported;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

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

        wxCHECK_MSG( m_targetRequested, false, wxT("invalid clipboard format") );

        m_formatSupported = false;

        // block until m_formatSupported is set by targets_selection_received
        {
            wxClipboardSync sync(*this);

            gtk_selection_convert( m_targetsWidget,
                               m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                            : g_clipboardAtom,
                               g_targetsAtom,
                               (guint32) GDK_CURRENT_TIME );
        } // wait until we get the results

        if (!m_formatSupported) continue;

        /* store pointer to data object to be filled up by callbacks */
        m_receivedData = &data;

        /* store requested format to be asked for by callbacks */
        m_targetRequested = format;

        wxCHECK_MSG( m_targetRequested, false, wxT("invalid clipboard format") );

        /* start query */
        m_formatSupported = false;

        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("wxClipboard::GetData: format found, start convert") );

        {
            wxClipboardSync sync(*this);

            gtk_selection_convert( m_clipboardWidget,
                                   m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                                                : g_clipboardAtom,
                                   m_targetRequested,
                                   (guint32) GDK_CURRENT_TIME );
        } // wait until we get the results

        /*
           Normally this is a true error as we checked for the presence of such
           data before, but there are applications that may return an empty
           string (e.g. Gnumeric-1.6.1 on Linux if an empty cell is copied)
           which would produce a false error message here, so we check for the
           size of the string first. In ansi, GetDataSize returns an extra
           value (for the closing null?), with unicode, the exact number of
           tokens is given (that is more than 1 for special characters)
           (tested with Gnumeric-1.6.1 and OpenOffice.org-2.0.2)
         */
#if wxUSE_UNICODE
        if ( format != wxDF_UNICODETEXT || data.GetDataSize(format) > 0 )
#else // !UNICODE
        if ( format != wxDF_TEXT || data.GetDataSize(format) > 1 )
#endif // UNICODE / !UNICODE
        {
            wxCHECK_MSG( m_formatSupported, false,
                         wxT("error retrieving data from clipboard") );
        }

        /* return success */
        delete[] array;
        return true;
    }

    wxLogTrace( TRACE_CLIPBOARD,
                wxT("wxClipboard::GetData: format not found") );

    /* return failure */
    delete[] array;
    return false;
}

#endif
  // wxUSE_CLIPBOARD
