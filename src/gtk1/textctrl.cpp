/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/textctrl.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/settings.h"
#include "wx/panel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

static void
gtk_text_changed_callback( GtkWidget *WXUNUSED(widget), wxTextCtrl *win )
{
    if (!win->m_hasVMT) return;

    if (g_isIdle)
        wxapp_install_idle_handler();

    win->SetModified();
    win->UpdateFontIfNeeded();

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, win->GetId() );
    event.SetString( win->GetValue() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "changed" from vertical scrollbar
//-----------------------------------------------------------------------------

static void
gtk_scrollbar_changed_callback( GtkWidget *WXUNUSED(widget), wxTextCtrl *win )
{
    if (!win->m_hasVMT) return;

    if (g_isIdle)
        wxapp_install_idle_handler();

    win->CalculateScrollbar();
}

//-----------------------------------------------------------------------------
// "focus_in_event"
//-----------------------------------------------------------------------------

wxWindow *FindFocusedChild(wxWindow *win);
extern wxWindow  *g_focusWindow;
extern bool       g_blockEventsOnDrag;
// extern bool g_isIdle;

static gint gtk_text_focus_in_callback( GtkWidget *widget, GdkEvent *WXUNUSED(event), wxWindow *win )
{
    // Necessary?
#if 0
    if (g_isIdle)
        wxapp_install_idle_handler();
#endif
    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

    g_focusWindow = win;

    wxPanel *panel = wxDynamicCast(win->GetParent(), wxPanel);
    if (panel)
    {
        panel->SetLastFocus(win);
    }

#ifdef HAVE_XIM
    if (win->m_ic)
        gdk_im_begin(win->m_ic, win->m_wxwindow->window);
#endif

#if 0
#ifdef wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = win->GetCaret();
    if ( caret )
    {
        caret->OnSetFocus();
    }
#endif // wxUSE_CARET
#endif

    wxFocusEvent event( wxEVT_SET_FOCUS, win->GetId() );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "focus_in_event" );
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "focus_out_event"
//-----------------------------------------------------------------------------

static gint gtk_text_focus_out_callback( GtkWidget *widget, GdkEvent *WXUNUSED(event), wxWindow *win )
{
#if 0
    if (g_isIdle)
        wxapp_install_idle_handler();
#endif

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

#if 0
    // if the focus goes out of our app alltogether, OnIdle() will send
    // wxActivateEvent, otherwise gtk_window_focus_in_callback() will reset
    // g_sendActivateEvent to -1
    g_sendActivateEvent = 0;
#endif

    wxWindow *winFocus = FindFocusedChild(win);
    if ( winFocus )
        win = winFocus;

    g_focusWindow = (wxWindow *)NULL;

#ifdef HAVE_XIM
    if (win->m_ic)
        gdk_im_end();
#endif

#if 0
#ifdef wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = win->GetCaret();
    if ( caret )
    {
        caret->OnKillFocus();
    }
#endif // wxUSE_CARET
#endif

    wxFocusEvent event( wxEVT_KILL_FOCUS, win->GetId() );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "focus_out_event" );
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl,wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(wxTextCtrl::OnChar)

    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()

void wxTextCtrl::Init()
{
    m_modified = FALSE;
    m_updateFont = FALSE;
    m_text =
    m_vScrollbar = (GtkWidget *)NULL;
}

wxTextCtrl::wxTextCtrl( wxWindow *parent,
                        wxWindowID id,
                        const wxString &value,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name )
{
    Init();

    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create( wxWindow *parent,
                         wxWindowID id,
                         const wxString &value,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxValidator& validator,
                         const wxString &name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxTextCtrl creation failed") );
        return FALSE;
    }


    m_vScrollbarVisible = FALSE;

    bool multi_line = (style & wxTE_MULTILINE) != 0;
    if (multi_line)
    {
#if (GTK_MINOR_VERSION > 2)
        /* a multi-line edit control: create a vertical scrollbar by default and
           horizontal if requested */
        bool bHasHScrollbar = (style & wxHSCROLL) != 0;
#else
        bool bHasHScrollbar = FALSE;
#endif

        /* create our control ... */
        m_text = gtk_text_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );

        /* ... and put into the upper left hand corner of the table */
        m_widget = gtk_table_new(bHasHScrollbar ? 2 : 1, 2, FALSE);
        GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );
        gtk_table_attach( GTK_TABLE(m_widget), m_text, 0, 1, 0, 1,
                      (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                      (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                       0, 0);

        /* always wrap words */
        gtk_text_set_word_wrap( GTK_TEXT(m_text), TRUE );

#if (GTK_MINOR_VERSION > 2)
        /* put the horizontal scrollbar in the lower left hand corner */
        if (bHasHScrollbar)
        {
            GtkWidget *hscrollbar = gtk_hscrollbar_new(GTK_TEXT(m_text)->hadj);
            GTK_WIDGET_UNSET_FLAGS( hscrollbar, GTK_CAN_FOCUS );
            gtk_table_attach(GTK_TABLE(m_widget), hscrollbar, 0, 1, 1, 2,
                       (GtkAttachOptions)(GTK_EXPAND | GTK_FILL | GTK_SHRINK),
                       GTK_FILL,
                       0, 0);
            gtk_widget_show(hscrollbar);

            /* don't wrap lines, otherwise we wouldn't need the scrollbar */
            gtk_text_set_line_wrap( GTK_TEXT(m_text), FALSE );
        }
#endif

        /* finally, put the vertical scrollbar in the upper right corner */
        m_vScrollbar = gtk_vscrollbar_new( GTK_TEXT(m_text)->vadj );
        GTK_WIDGET_UNSET_FLAGS( m_vScrollbar, GTK_CAN_FOCUS );
        gtk_table_attach(GTK_TABLE(m_widget), m_vScrollbar, 1, 2, 0, 1,
                     GTK_FILL,
                     (GtkAttachOptions)(GTK_EXPAND | GTK_FILL | GTK_SHRINK),
                     0, 0);
    }
    else
    {
        /* a single-line text control: no need for scrollbars */
        m_widget =
          m_text = gtk_entry_new();
    }

    m_parent->DoAddChild( this );

    PostCreation();

    SetFont( parent->GetFont() );

    wxSize size_best( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = size_best.x;
    if (new_size.y == -1)
        new_size.y = size_best.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );

    if (multi_line)
        gtk_widget_show(m_text);

    if (multi_line)
    {
        gtk_signal_connect(GTK_OBJECT(GTK_TEXT(m_text)->vadj), "changed",
          (GtkSignalFunc) gtk_scrollbar_changed_callback, (gpointer) this );

        gtk_signal_connect( GTK_OBJECT(GTK_TEXT(m_text)), "focus_in_event",
              GTK_SIGNAL_FUNC(gtk_text_focus_in_callback), (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(GTK_TEXT(m_text)), "focus_out_event",
			    GTK_SIGNAL_FUNC(gtk_text_focus_out_callback), (gpointer)this );
    }
    else
    {
        gtk_signal_connect( GTK_OBJECT(m_text), "focus_in_event",
              GTK_SIGNAL_FUNC(gtk_text_focus_in_callback), (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(m_text), "focus_out_event",
			    GTK_SIGNAL_FUNC(gtk_text_focus_out_callback), (gpointer)this );
    }

    if (!value.IsEmpty())
    {
        gint tmp = 0;

#if GTK_MINOR_VERSION == 0
        // if we don't realize it, GTK 1.0.6 dies with a SIGSEGV in
        // gtk_editable_insert_text()
        gtk_widget_realize(m_text);
#endif // GTK 1.0

#if wxUSE_UNICODE
        wxWX2MBbuf val = value.mbc_str();
        gtk_editable_insert_text( GTK_EDITABLE(m_text), val, strlen(val), &tmp );
#else // !Unicode
        gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &tmp );
#endif // Unicode/!Unicode

        if (multi_line)
        {
            /* bring editable's cursor uptodate. bug in GTK. */

            GTK_EDITABLE(m_text)->current_pos = gtk_text_get_point( GTK_TEXT(m_text) );
        }
    }

    if (style & wxTE_PASSWORD)
    {
        if (!multi_line)
            gtk_entry_set_visibility( GTK_ENTRY(m_text), FALSE );
    }

    if (style & wxTE_READONLY)
    {
        if (!multi_line)
            gtk_entry_set_editable( GTK_ENTRY(m_text), FALSE );
    }
    else
    {
        if (multi_line)
            gtk_text_set_editable( GTK_TEXT(m_text), 1 );
    }

    /* we want to be notified about text changes */
    gtk_signal_connect( GTK_OBJECT(m_text), "changed",
      GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this);

    SetBackgroundColour( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) );
    SetForegroundColour( parent->GetForegroundColour() );

    m_cursor = wxCursor( wxCURSOR_IBEAM );

    Show( TRUE );

    return TRUE;
}

void wxTextCtrl::CalculateScrollbar()
{
    if ((m_windowStyle & wxTE_MULTILINE) == 0) return;

    GtkAdjustment *adj = GTK_TEXT(m_text)->vadj;

    if (adj->upper - adj->page_size < 0.8)
    {
        if (m_vScrollbarVisible)
        {
            gtk_widget_hide( m_vScrollbar );
            m_vScrollbarVisible = FALSE;
        }
    }
    else
    {
        if (!m_vScrollbarVisible)
        {
            gtk_widget_show( m_vScrollbar );
            m_vScrollbarVisible = TRUE;
        }
    }
}

wxString wxTextCtrl::GetValue() const
{
    wxCHECK_MSG( m_text != NULL, wxT(""), wxT("invalid text ctrl") );

    wxString tmp;
    if (m_windowStyle & wxTE_MULTILINE)
    {
        gint len = gtk_text_get_length( GTK_TEXT(m_text) );
        char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );
        tmp = wxString(text,*wxConvCurrent);
        g_free( text );
    }
    else
    {
        tmp = wxString(gtk_entry_get_text( GTK_ENTRY(m_text) ),*wxConvCurrent);
    }
    return tmp;
}

void wxTextCtrl::SetValue( const wxString &value )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (m_windowStyle & wxTE_MULTILINE)
    {
        gint len = gtk_text_get_length( GTK_TEXT(m_text) );
        gtk_editable_delete_text( GTK_EDITABLE(m_text), 0, len );
        len = 0;
#if wxUSE_UNICODE
        wxWX2MBbuf tmpbuf = value.mbc_str();
        gtk_editable_insert_text( GTK_EDITABLE(m_text), tmpbuf, strlen(tmpbuf), &len );
#else
        gtk_editable_insert_text( GTK_EDITABLE(m_text), value.mbc_str(), value.Length(), &len );
#endif
    }
    else
    {
        gtk_entry_set_text( GTK_ENTRY(m_text), value.mbc_str() );
    }

    // GRG, Jun/2000: Changed this after a lot of discussion in
    //   the lists. wxWindows 2.2 will have a set of flags to
    //   customize this behaviour.
    SetInsertionPoint(0);

    m_modified = FALSE;
}

void wxTextCtrl::WriteText( const wxString &text )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (text.IsEmpty()) return;

    if (m_windowStyle & wxTE_MULTILINE)
    {
        /* this moves the cursor pos to behind the inserted text */
        gint len = GTK_EDITABLE(m_text)->current_pos;

#if wxUSE_UNICODE
        wxWX2MBbuf buf = text.mbc_str();
        gtk_editable_insert_text( GTK_EDITABLE(m_text), buf, strlen(buf), &len );
#else
        gtk_editable_insert_text( GTK_EDITABLE(m_text), text, text.Length(), &len );
#endif

        /* bring editable's cursor uptodate. bug in GTK. */
        GTK_EDITABLE(m_text)->current_pos = gtk_text_get_point( GTK_TEXT(m_text) );
    }
    else
    {
        /* this moves the cursor pos to behind the inserted text */
        gint len = GTK_EDITABLE(m_text)->current_pos;
#if wxUSE_UNICODE
        wxWX2MBbuf buf = text.mbc_str();
        gtk_editable_insert_text( GTK_EDITABLE(m_text), buf, strlen(buf), &len );
#else
        gtk_editable_insert_text( GTK_EDITABLE(m_text), text, text.Length(), &len );
#endif

        /* bring editable's cursor uptodate. bug in GTK. */
        GTK_EDITABLE(m_text)->current_pos += text.Len();

        /* bring entry's cursor uptodate. bug in GTK. */
        gtk_entry_set_position( GTK_ENTRY(m_text), GTK_EDITABLE(m_text)->current_pos );
    }
}

void wxTextCtrl::AppendText( const wxString &text )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (text.IsEmpty()) return;

    if (m_windowStyle & wxTE_MULTILINE)
    {
        bool hasSpecialAttributes = m_font.Ok() ||
                                    m_foregroundColour.Ok() ||
                                    m_backgroundColour.Ok();
        if ( hasSpecialAttributes )
        {
             gtk_text_insert( GTK_TEXT(m_text),
                              m_font.GetInternalFont(),
                              m_foregroundColour.GetColor(),
                              m_backgroundColour.GetColor(),
                              text.mbc_str(), text.length());

        }
        else
        {
            /* we'll insert at the last position */
            gint len = gtk_text_get_length( GTK_TEXT(m_text) );
#if wxUSE_UNICODE
            wxWX2MBbuf buf = text.mbc_str();
            gtk_editable_insert_text( GTK_EDITABLE(m_text), buf, strlen(buf), &len );
#else
            gtk_editable_insert_text( GTK_EDITABLE(m_text), text, text.Length(), &len );
#endif
        }

        /* bring editable's cursor uptodate. bug in GTK. */
        GTK_EDITABLE(m_text)->current_pos = gtk_text_get_point( GTK_TEXT(m_text) );
    }
    else
    {
        gtk_entry_append_text( GTK_ENTRY(m_text), text.mbc_str() );
    }
}

wxString wxTextCtrl::GetLineText( long lineNo ) const
{
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_text) );
    char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );

    if (text)
    {
        wxString buf(wxT(""));
        long i;
        int currentLine = 0;
        for (i = 0; currentLine != lineNo && text[i]; i++ )
          if (text[i] == '\n')
            currentLine++;
        // Now get the text
        int j;
        for (j = 0; text[i] && text[i] != '\n'; i++, j++ )
            buf += text[i];

        g_free( text );
        return buf;
    }
    else
      return wxEmptyString;
  }
  else
  {
    if (lineNo == 0) return GetValue();
    return wxEmptyString;
  }
}

void wxTextCtrl::OnDropFiles( wxDropFilesEvent &WXUNUSED(event) )
{
  /* If you implement this, don't forget to update the documentation!
   * (file docs/latex/wx/text.tex) */
    wxFAIL_MSG( wxT("wxTextCtrl::OnDropFiles not implemented") );
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y ) const
{
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxString text = GetValue();

        // cast to prevent warning. But pos really should've been unsigned.
        if( (unsigned long)pos > text.Len()  )
            return FALSE;

        *x=0;   // First Col
        *y=0;   // First Line

        const wxChar* stop = text.c_str() + pos;
        for ( const wxChar *p = text.c_str(); p < stop; p++ )
        {
            if (*p == wxT('\n'))
            {
                (*y)++;
                *x=0;
            }
            else
                (*x)++;
        }
    }
    else // single line control
    {
        if ( pos <= GTK_ENTRY(m_text)->text_length )
        {
            *y = 0;
            *x = pos;
        }
        else
        {
            // index out of bounds
            return FALSE;
        }
    }

    return TRUE;
}

long wxTextCtrl::XYToPosition(long x, long y ) const
{
    if (!(m_windowStyle & wxTE_MULTILINE)) return 0;

    long pos=0;
    for( int i=0; i<y; i++ ) pos += GetLineLength(i) + 1; // one for '\n'

    pos += x;
    return pos;
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    wxString str = GetLineText (lineNo);
    return (int) str.Length();
}

int wxTextCtrl::GetNumberOfLines() const
{
    if (m_windowStyle & wxTE_MULTILINE)
    {
        gint len = gtk_text_get_length( GTK_TEXT(m_text) );
        char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );

        if (text)
        {
            int currentLine = 0;
            for (int i = 0; i < len; i++ )
            {
                if (text[i] == '\n')
                    currentLine++;
            }
            g_free( text );

            // currentLine is 0 based, add 1 to get number of lines
            return currentLine + 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }
}

void wxTextCtrl::SetInsertionPoint( long pos )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (m_windowStyle & wxTE_MULTILINE)
    {
        /* seems to be broken in GTK 1.0.X:
           gtk_text_set_point( GTK_TEXT(m_text), (int)pos ); */

        gtk_signal_disconnect_by_func( GTK_OBJECT(m_text),
          GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this);

        /* we fake a set_point by inserting and deleting. as the user
           isn't supposed to get to know about thos non-sense, we
           disconnect so that no events are sent to the user program. */

        gint tmp = (gint)pos;
        gtk_editable_insert_text( GTK_EDITABLE(m_text), " ", 1, &tmp );
        gtk_editable_delete_text( GTK_EDITABLE(m_text), tmp-1, tmp );

        gtk_signal_connect( GTK_OBJECT(m_text), "changed",
          GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this);

        /* bring editable's cursor uptodate. another bug in GTK. */

        GTK_EDITABLE(m_text)->current_pos = gtk_text_get_point( GTK_TEXT(m_text) );
    }
    else
    {
        gtk_entry_set_position( GTK_ENTRY(m_text), (int)pos );

        /* bring editable's cursor uptodate. bug in GTK. */

        GTK_EDITABLE(m_text)->current_pos = (guint32)pos;
    }
}

void wxTextCtrl::SetInsertionPointEnd()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (m_windowStyle & wxTE_MULTILINE)
        SetInsertionPoint(gtk_text_get_length(GTK_TEXT(m_text)));
    else
        gtk_entry_set_position( GTK_ENTRY(m_text), -1 );
}

void wxTextCtrl::SetEditable( bool editable )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (m_windowStyle & wxTE_MULTILINE)
        gtk_text_set_editable( GTK_TEXT(m_text), editable );
    else
        gtk_entry_set_editable( GTK_ENTRY(m_text), editable );
}

bool wxTextCtrl::Enable( bool enable )
{
    if (!wxWindowBase::Enable(enable))
    {
        // nothing to do
        return FALSE;
    }

    if (m_windowStyle & wxTE_MULTILINE)
    {
        gtk_text_set_editable( GTK_TEXT(m_text), enable );
    }
    else
    {
        gtk_widget_set_sensitive( m_text, enable );
    }

    return TRUE;
}

void wxTextCtrl::DiscardEdits()
{
    m_modified = FALSE;
}

void wxTextCtrl::SetSelection( long from, long to )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( (m_windowStyle & wxTE_MULTILINE) &&
         !GTK_TEXT(m_text)->line_start_cache )
    {
        // tell the programmer that it didn't work
        wxLogDebug(_T("Can't call SetSelection() before realizing the control"));
        return;
    }

    gtk_editable_select_region( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::ShowPosition( long WXUNUSED(pos) )
{
//    SetInsertionPoint( pos );
}

long wxTextCtrl::GetInsertionPoint() const
{
    wxCHECK_MSG( m_text != NULL, 0, wxT("invalid text ctrl") );

    return (long) GTK_EDITABLE(m_text)->current_pos;
}

long wxTextCtrl::GetLastPosition() const
{
    wxCHECK_MSG( m_text != NULL, 0, wxT("invalid text ctrl") );

    int pos = 0;
    if (m_windowStyle & wxTE_MULTILINE)
        pos = gtk_text_get_length( GTK_TEXT(m_text) );
    else
        pos = GTK_ENTRY(m_text)->text_length;

    return (long)pos;
}

void wxTextCtrl::Remove( long from, long to )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::Replace( long from, long to, const wxString &value )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );

    if (!value.IsEmpty())
    {
        gint pos = (gint)from;
#if wxUSE_UNICODE
        wxWX2MBbuf buf = value.mbc_str();
        gtk_editable_insert_text( GTK_EDITABLE(m_text), buf, strlen(buf), &pos );
#else
        gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &pos );
#endif
    }
}

void wxTextCtrl::Cut()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

#if (GTK_MINOR_VERSION > 0)
    gtk_editable_cut_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_cut_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

void wxTextCtrl::Copy()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

#if (GTK_MINOR_VERSION > 0)
    gtk_editable_copy_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_copy_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

void wxTextCtrl::Paste()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

#if (GTK_MINOR_VERSION > 0)
    gtk_editable_paste_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_paste_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to) ;
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to) && (IsEditable());
}

bool wxTextCtrl::CanPaste() const
{
    return IsEditable() ;
}

// Undo/redo
void wxTextCtrl::Undo()
{
    // TODO
    wxFAIL_MSG( wxT("wxTextCtrl::Undo not implemented") );
}

void wxTextCtrl::Redo()
{
    // TODO
    wxFAIL_MSG( wxT("wxTextCtrl::Redo not implemented") );
}

bool wxTextCtrl::CanUndo() const
{
    // TODO
    wxFAIL_MSG( wxT("wxTextCtrl::CanUndo not implemented") );
    return FALSE;
}

bool wxTextCtrl::CanRedo() const
{
    // TODO
    wxFAIL_MSG( wxT("wxTextCtrl::CanRedo not implemented") );
    return FALSE;
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (!(GTK_EDITABLE(m_text)->has_selection))
    {
        long i = GetInsertionPoint();
        if (from) *from = i;
        if (to)   *to = i;
        return;
    }

    if (from) *from = (long) GTK_EDITABLE(m_text)->selection_start_pos;
    if (to)   *to = (long) GTK_EDITABLE(m_text)->selection_end_pos;
}

bool wxTextCtrl::IsEditable() const
{
    wxCHECK_MSG( m_text != NULL, FALSE, wxT("invalid text ctrl") );

    return GTK_EDITABLE(m_text)->editable;
}

bool wxTextCtrl::IsModified() const
{
    return m_modified;
}

void wxTextCtrl::Clear()
{
    SetValue( wxT("") );
}

void wxTextCtrl::OnChar( wxKeyEvent &key_event )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ((key_event.KeyCode() == WXK_RETURN) && (m_windowStyle & wxPROCESS_ENTER))
    {
        wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
        event.SetEventObject(this);
        event.SetString(GetValue());
        if (GetEventHandler()->ProcessEvent(event)) return;
    }

    if ((key_event.KeyCode() == WXK_RETURN) && !(m_windowStyle & wxTE_MULTILINE))
    {
        wxWindow *top_frame = m_parent;
        while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
            top_frame = top_frame->GetParent();
        GtkWindow *window = GTK_WINDOW(top_frame->m_widget);

        if (window->default_widget)
        {
            gtk_widget_activate (window->default_widget);
            return;
        }
    }

    key_event.Skip();
}

GtkWidget* wxTextCtrl::GetConnectWidget()
{
    return GTK_WIDGET(m_text);
}

bool wxTextCtrl::IsOwnGtkWindow( GdkWindow *window )
{
    if (m_windowStyle & wxTE_MULTILINE)
        return (window == GTK_TEXT(m_text)->text_area);
    else
        return (window == GTK_ENTRY(m_text)->text_area);
}

// the font will change for subsequent text insertiongs
bool wxTextCtrl::SetFont( const wxFont &font )
{
    wxCHECK_MSG( m_text != NULL, FALSE, wxT("invalid text ctrl") );

    if ( !wxWindowBase::SetFont(font) )
    {
        // font didn't change, nothing to do
        return FALSE;
    }

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        m_updateFont = TRUE;

        ChangeFontGlobally();
    }

    return TRUE;
}

void wxTextCtrl::ChangeFontGlobally()
{
    // this method is very inefficient and hence should be called as rarely as
    // possible!
    wxASSERT_MSG( (m_windowStyle & wxTE_MULTILINE) && m_updateFont,
                  _T("shouldn't be called for single line controls") );

    wxString value = GetValue();
    if ( !value.IsEmpty() )
    {
        Clear();
        AppendText(value);

        m_updateFont = FALSE;
    }
}

void wxTextCtrl::UpdateFontIfNeeded()
{
    if ( m_updateFont )
        ChangeFontGlobally();
}

bool wxTextCtrl::SetForegroundColour( const wxColour &WXUNUSED(colour) )
{
    wxCHECK_MSG( m_text != NULL, FALSE, wxT("invalid text ctrl") );

    // doesn't work
    return FALSE;
}

bool wxTextCtrl::SetBackgroundColour( const wxColour &colour )
{
    wxCHECK_MSG( m_text != NULL, FALSE, wxT("invalid text ctrl") );

    wxControl::SetBackgroundColour( colour );

    if (!m_widget->window)
        return FALSE;

    wxColour sysbg = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE );
    if (sysbg.Red() == colour.Red() &&
        sysbg.Green() == colour.Green() &&
        sysbg.Blue() == colour.Blue())
    {
        return FALSE; // FIXME or TRUE?
    }

    if (!m_backgroundColour.Ok())
        return FALSE;

    if (m_windowStyle & wxTE_MULTILINE)
    {
        GdkWindow *window = GTK_TEXT(m_text)->text_area;
        if (!window)
            return FALSE;
        m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
        gdk_window_set_background( window, m_backgroundColour.GetColor() );
        gdk_window_clear( window );
    }

    return TRUE;
}

void wxTextCtrl::ApplyWidgetStyle()
{
    if (m_windowStyle & wxTE_MULTILINE)
    {
        // how ?
    }
    else
    {
        SetWidgetStyle();
        gtk_widget_set_style( m_text, m_widgetStyle );
    }
}

void wxTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

void wxTextCtrl::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (cursor.Ok())
    {
        GdkWindow *window = (GdkWindow*) NULL;
        if (HasFlag(wxTE_MULTILINE))
            window = GTK_TEXT(m_text)->text_area;
        else
            window = GTK_ENTRY(m_text)->text_area;

        if (window)
            gdk_window_set_cursor( window, cursor.GetCursor() );

        if (!g_globalCursor.Ok())
            cursor = *wxSTANDARD_CURSOR;

        window = m_widget->window;
        if ((window) && !(GTK_WIDGET_NO_WINDOW(m_widget)))
            gdk_window_set_cursor( window, cursor.GetCursor() );
    }

    UpdateWindowUI();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    // FIXME should be different for multi-line controls...
    wxSize ret( wxControl::DoGetBestSize() );
    return wxSize(80, ret.y);
}
