/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/clipbrd.cpp
// Purpose:     wxClipboard implementation for wxGTK
// Author:      Robert Roebling, Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
//              (c) 2007 Vadim Zeitlin
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

#include "wx/ptr_scpd.h"
#include "wx/scopeguard.h"

#include "wx/gtk/private.h"

wxDECLARE_SCOPED_ARRAY(wxDataFormat, wxDataFormatArray)
wxDEFINE_SCOPED_ARRAY(wxDataFormat, wxDataFormatArray)

// ----------------------------------------------------------------------------
// data
// ----------------------------------------------------------------------------

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

    // this method should be called if it's possible that no async clipboard
    // operation is currently in progress (like it can be the case when the
    // clipboard is cleared but not because we asked about it), it should only
    // be called if such situation is expected -- otherwise call OnDone() which
    // would assert in this case
    static void OnDoneIfInProgress(wxClipboard *clipboard)
    {
        if ( ms_clipboard )
            OnDone(clipboard);
    }

private:
    static wxClipboard *ms_clipboard;

    DECLARE_NO_COPY_CLASS(wxClipboardSync)
};

wxClipboard *wxClipboardSync::ms_clipboard = NULL;

// ============================================================================
// clipboard ca;backs implementation
// ============================================================================

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
    if ( !clipboard )
        return;

    wxON_BLOCK_EXIT1(wxClipboardSync::OnDone, clipboard);

    if ( !selection_data || selection_data->length <= 0 )
        return;

    // make sure we got the data in the correct form
    GdkAtom type = selection_data->type;
    if ( type != GDK_SELECTION_TYPE_ATOM )
    {
        if ( strcmp(wxGtkString(gdk_atom_name(type)), "TARGETS") != 0 )
        {
            wxLogTrace( TRACE_CLIPBOARD,
                        _T("got unsupported clipboard target") );

            return;
        }
    }

#ifdef __WXDEBUG__
    // it's not really a format, of course, but we can reuse its GetId() method
    // to format this atom as string
    wxDataFormat clip(selection_data->selection);
    wxLogTrace( TRACE_CLIPBOARD,
                wxT("Received available formats for clipboard %s"),
                clip.GetId().c_str() );
#endif // __WXDEBUG__

    // the atoms we received, holding a list of targets (= formats)
    const GdkAtom * const atoms = (GdkAtom *)selection_data->data;
    for ( size_t i = 0; i < selection_data->length/sizeof(GdkAtom); i++ )
    {
        const wxDataFormat format(atoms[i]);

        wxLogTrace(TRACE_CLIPBOARD, wxT("\t%s"), format.GetId().c_str());

        if ( clipboard->GTKOnTargetReceived(format) )
            return;
    }
}
}

bool wxClipboard::GTKOnTargetReceived(const wxDataFormat& format)
{
    if ( format != m_targetRequested )
        return false;

    m_formatSupported = true;
    return true;
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
    if ( !clipboard )
        return;

    wxON_BLOCK_EXIT1(wxClipboardSync::OnDone, clipboard);

    if ( !selection_data || selection_data->length <= 0 )
        return;

    clipboard->GTKOnSelectionReceived(*selection_data);
}
}

//-----------------------------------------------------------------------------
// "selection_clear"
//-----------------------------------------------------------------------------

extern "C" {
static gint
selection_clear_clip( GtkWidget *WXUNUSED(widget), GdkEventSelection *event )
{
    wxClipboard * const clipboard = wxTheClipboard;
    if ( !clipboard )
        return TRUE;

    // notice the use of OnDoneIfInProgress() here instead of just OnDone():
    // it's perfectly possible that we're receiving this notification from GTK+
    // even though we hadn't cleared the clipboard ourselves but because
    // another application (or even another window in the same program)
    // acquired it
    wxON_BLOCK_EXIT1(wxClipboardSync::OnDoneIfInProgress, clipboard);

    wxClipboard::Kind kind;
    if (event->selection == GDK_SELECTION_PRIMARY)
    {
        wxLogTrace(TRACE_CLIPBOARD, wxT("Lost primary selection" ));

        kind = wxClipboard::Primary;
    }
    else if (event->selection == g_clipboardAtom)
    {
        wxLogTrace(TRACE_CLIPBOARD, wxT("Lost clipboard" ));

        kind = wxClipboard::Clipboard;
    }
    else // some other selection, we're not concerned
    {
        return FALSE;
    }

    // the clipboard is no longer in our hands, we don't need data any more
    clipboard->GTKClearData(kind);

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
    wxClipboard * const clipboard = wxTheClipboard;
    if ( !clipboard )
        return;

    wxDataObject * const data = clipboard->GTKGetDataObject();
    if ( !data )
        return;

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
    wxON_BLOCK_EXIT1(free, d);

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
}
}

void wxClipboard::GTKOnSelectionReceived(const GtkSelectionData& sel)
{
    wxCHECK_RET( m_receivedData, _T("should be inside GetData()") );

    const wxDataFormat format(sel.target);
    wxLogTrace(TRACE_CLIPBOARD, _T("Received selection %s"),
               format.GetId().c_str());

    if ( !m_receivedData->IsSupportedFormat(format) )
        return;

    m_receivedData->SetData(format, sel.length, sel.data);
    m_formatSupported = true;
}

// ============================================================================
// wxClipboard implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxClipboard ctor/dtor
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
    m_open = false;

    m_dataPrimary =
    m_dataClipboard =
    m_receivedData = NULL;

    m_formatSupported = false;
    m_targetRequested = 0;

    // we use m_targetsWidget to query what formats are available
    m_targetsWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_targetsWidget );

    g_signal_connect (m_targetsWidget, "selection_received",
                      G_CALLBACK (targets_selection_received), this);

    // we use m_clipboardWidget to get and to offer data
    m_clipboardWidget = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_realize( m_clipboardWidget );

    g_signal_connect (m_clipboardWidget, "selection_received",
                      G_CALLBACK (selection_received), this);

    g_signal_connect (m_clipboardWidget, "selection_clear_event",
                      G_CALLBACK (selection_clear_clip), NULL);

    // initialize atoms we use if not done yet
    if ( !g_clipboardAtom )
        g_clipboardAtom = gdk_atom_intern( "CLIPBOARD", FALSE );
    if ( !g_targetsAtom )
        g_targetsAtom = gdk_atom_intern ("TARGETS", FALSE);
    if ( !g_timestampAtom )
        g_timestampAtom = gdk_atom_intern ("TIMESTAMP", FALSE);
}

wxClipboard::~wxClipboard()
{
    Clear();

    gtk_widget_destroy( m_clipboardWidget );
    gtk_widget_destroy( m_targetsWidget );
}

// ----------------------------------------------------------------------------
// wxClipboard helper functions
// ----------------------------------------------------------------------------

GdkAtom wxClipboard::GTKGetClipboardAtom() const
{
    return m_usePrimary ? (GdkAtom)GDK_SELECTION_PRIMARY
                        : g_clipboardAtom;
}

void wxClipboard::GTKClearData(Kind kind)
{
    wxDataObject *&data = Data();
    if ( data )
    {
        delete data;
        data = NULL;
    }
}

bool wxClipboard::SetSelectionOwner(bool set)
{
    bool rc = gtk_selection_owner_set
              (
                set ? m_clipboardWidget : NULL,
                GTKGetClipboardAtom(),
                (guint32)GDK_CURRENT_TIME
              );

    if ( !rc )
    {
        wxLogTrace(TRACE_CLIPBOARD, _T("Failed to %sset selection owner"),
                   set ? _T("") : _T("un"));
    }

    return rc;
}

void wxClipboard::AddSupportedTarget(GdkAtom atom)
{
    gtk_selection_add_target
    (
        GTK_WIDGET(m_clipboardWidget),
        GTKGetClipboardAtom(),
        atom,
        0 // info (same as client data) unused
    );
}

bool wxClipboard::DoIsSupported(const wxDataFormat& format)
{
    wxCHECK_MSG( format, false, wxT("invalid clipboard format") );

    wxLogTrace(TRACE_CLIPBOARD, wxT("Checking if format %s is available"),
               format.GetId().c_str());

    // these variables will be used by our GTKOnTargetReceived()
    m_targetRequested = format;
    m_formatSupported = false;

    // block until m_formatSupported is set from targets_selection_received
    // callback
    {
        wxClipboardSync sync(*this);

        gtk_selection_convert( m_targetsWidget,
                               GTKGetClipboardAtom(),
                               g_targetsAtom,
                               (guint32) GDK_CURRENT_TIME );
    }

    return m_formatSupported;
}

// ----------------------------------------------------------------------------
// wxClipboard public API implementation
// ----------------------------------------------------------------------------

void wxClipboard::Clear()
{
    if ( gdk_selection_owner_get(GTKGetClipboardAtom()) ==
            m_clipboardWidget->window )
    {
        wxClipboardSync sync(*this);

        // this will result in selection_clear_clip callback being called and
        // it will free our data
        SetSelectionOwner(false);
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

    // we can only store one wxDataObject so clear the old one
    Clear();

    Data() = data;

    // get formats from wxDataObjects
    const size_t count = data->GetFormatCount();
    wxDataFormatArray formats(new wxDataFormat[count]);
    data->GetAllFormats(formats.get());

    // always provide TIMESTAMP as a target, see comments in selection_handler
    // for explanation
    AddSupportedTarget(g_timestampAtom);

    for ( size_t i = 0; i < count; i++ )
    {
        const wxDataFormat format(formats[i]);

        wxLogTrace(TRACE_CLIPBOARD, wxT("Adding support for %s"),
                   format.GetId().c_str());

        AddSupportedTarget(format);
    }

    g_signal_connect (m_clipboardWidget, "selection_get",
                      G_CALLBACK (selection_handler),
                      GUINT_TO_POINTER (gtk_get_current_event_time()) );

    // tell the world we offer clipboard data
    return SetSelectionOwner();
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
    if ( DoIsSupported(format) )
        return true;

#if wxUSE_UNICODE
    if ( format == wxDF_UNICODETEXT )
    {
        // also with plain STRING format
        return DoIsSupported(g_altTextAtom);
    }
#endif // wxUSE_UNICODE

    return false;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    // get all supported formats from wxDataObjects
    const size_t count = data.GetFormatCount();
    wxDataFormatArray formats(new wxDataFormat[count]);
    data.GetAllFormats(formats.get());

    for ( size_t i = 0; i < count; i++ )
    {
        const wxDataFormat format(formats[i]);

        // is this format supported by clipboard ?
        if ( !DoIsSupported(format) )
            continue;

        wxLogTrace(TRACE_CLIPBOARD, wxT("Requesting format %s"),
                   format.GetId().c_str());

        // these variables will be used by our GTKOnSelectionReceived()
        m_receivedData = &data;
        m_formatSupported = false;

        {
            wxClipboardSync sync(*this);

            gtk_selection_convert(m_clipboardWidget,
                                  GTKGetClipboardAtom(),
                                  format,
                                  (guint32) GDK_CURRENT_TIME );
        } // wait until we get the results

        /*
           Normally this is a true error as we checked for the presence of such
           data before, but there are applications that may return an empty
           string (e.g. Gnumeric-1.6.1 on Linux if an empty cell is copied)
           which would produce a false error message here, so we check for the
           size of the string first. With ANSI, GetDataSize returns an extra
           value (for the closing null?), with unicode, the exact number of
           tokens is given (that is more than 1 for non-ASCII characters)
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

        return true;
    }

    wxLogTrace(TRACE_CLIPBOARD, wxT("GetData(): format not found"));

    return false;
}

#endif // wxUSE_CLIPBOARD
