/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin, 2005 Mart Raudsepp
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textctrl.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/panel.h"
    #include "wx/settings.h"
    #include "wx/math.h"
#endif

#include "wx/strconv.h"
#include "wx/fontutil.h"        // for wxNativeFontInfo (GetNativeFontInfo())

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "wx/gtk/private.h"
#include <gdk/gdkkeysyms.h>

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

extern "C" {
static void wxGtkOnRemoveTag(GtkTextBuffer *buffer,
                             GtkTextTag *tag,
                             GtkTextIter *start,
                             GtkTextIter *end,
                             char *prefix)
{
    gchar *name;
    g_object_get (tag, "name", &name, NULL);

    if (!name || strncmp(name, prefix, strlen(prefix)))
        // anonymous tag or not starting with prefix - don't remove
        g_signal_stop_emission_by_name (buffer, "remove_tag");

    g_free(name);
}
}

extern "C" {
static void wxGtkTextApplyTagsFromAttr(GtkWidget *text,
                                       GtkTextBuffer *text_buffer,
                                       const wxTextAttr& attr,
                                       GtkTextIter *start,
                                       GtkTextIter *end)
{
    static gchar buf[1024];
    GtkTextTag *tag;

    gulong remove_handler_id = g_signal_connect (text_buffer, "remove_tag",
            G_CALLBACK (wxGtkOnRemoveTag), gpointer("WX"));
    gtk_text_buffer_remove_all_tags(text_buffer, start, end);
    g_signal_handler_disconnect (text_buffer, remove_handler_id);

    if (attr.HasFont())
    {
        PangoFontDescription *font_description = attr.GetFont().GetNativeFontInfo()->description;
        wxGtkString font_string(pango_font_description_to_string(font_description));
        g_snprintf(buf, sizeof(buf), "WXFONT %s", font_string.c_str());
        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                         buf );
        if (!tag)
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "font-desc", font_description,
                                              NULL );
        gtk_text_buffer_apply_tag (text_buffer, tag, start, end);

        if (attr.GetFont().GetUnderlined())
        {
            g_snprintf(buf, sizeof(buf), "WXFONTUNDERLINE");
            tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                             buf );
            if (!tag)
                tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                                  "underline-set", TRUE,
                                                  "underline", PANGO_UNDERLINE_SINGLE,
                                                  NULL );
            gtk_text_buffer_apply_tag (text_buffer, tag, start, end);
        }
    }

    if (attr.HasTextColour())
    {
        const GdkColor *colFg = attr.GetTextColour().GetColor();
        g_snprintf(buf, sizeof(buf), "WXFORECOLOR %d %d %d",
                   colFg->red, colFg->green, colFg->blue);
        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                         buf );
        if (!tag)
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "foreground-gdk", colFg, NULL );
        gtk_text_buffer_apply_tag (text_buffer, tag, start, end);
    }

    if (attr.HasBackgroundColour())
    {
        const GdkColor *colBg = attr.GetBackgroundColour().GetColor();
        g_snprintf(buf, sizeof(buf), "WXBACKCOLOR %d %d %d",
                   colBg->red, colBg->green, colBg->blue);
        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                         buf );
        if (!tag)
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "background-gdk", colBg, NULL );
        gtk_text_buffer_apply_tag (text_buffer, tag, start, end);
    }

    if (attr.HasAlignment())
    {
        GtkTextIter para_start, para_end = *end;
        gtk_text_buffer_get_iter_at_line( text_buffer,
                                          &para_start,
                                          gtk_text_iter_get_line(start) );
        gtk_text_iter_forward_line(&para_end);

        remove_handler_id = g_signal_connect (text_buffer, "remove_tag",
                                              G_CALLBACK(wxGtkOnRemoveTag),
                                              gpointer("WXALIGNMENT"));
        gtk_text_buffer_remove_all_tags( text_buffer, &para_start, &para_end );
        g_signal_handler_disconnect (text_buffer, remove_handler_id);

        GtkJustification align;
        switch (attr.GetAlignment())
        {
            default:
                align = GTK_JUSTIFY_LEFT;
                break;
            case wxTEXT_ALIGNMENT_RIGHT:
                align = GTK_JUSTIFY_RIGHT;
                break;
            case wxTEXT_ALIGNMENT_CENTER:
                align = GTK_JUSTIFY_CENTER;
                break;
            // gtk+ doesn't support justify as of gtk+-2.7.4
        }

        g_snprintf(buf, sizeof(buf), "WXALIGNMENT %d", align);
        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                         buf );
        if (!tag)
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "justification", align, NULL );
        gtk_text_buffer_apply_tag( text_buffer, tag, &para_start, &para_end );
    }

    if (attr.HasLeftIndent())
    {
        // Indentation attribute

        // Clear old indentation tags
        GtkTextIter para_start, para_end = *end;
        gtk_text_buffer_get_iter_at_line( text_buffer,
                                          &para_start,
                                          gtk_text_iter_get_line(start) );
        gtk_text_iter_forward_line(&para_end);

        remove_handler_id = g_signal_connect (text_buffer, "remove_tag",
                                              G_CALLBACK(wxGtkOnRemoveTag),
                                              gpointer("WXINDENT"));
        gtk_text_buffer_remove_all_tags( text_buffer, &para_start, &para_end );
        g_signal_handler_disconnect (text_buffer, remove_handler_id);

        // Convert indent from 1/10th of a mm into pixels
        float factor;
#if GTK_CHECK_VERSION(2,2,0)
        if (!gtk_check_version(2,2,0))
            factor = (float)gdk_screen_get_width(gtk_widget_get_screen(text)) /
                      gdk_screen_get_width_mm(gtk_widget_get_screen(text)) / 10;
        else
#endif
            factor = (float)gdk_screen_width() / gdk_screen_width_mm() / 10;

        const int indent = (int)(factor * attr.GetLeftIndent());
        const int subIndent = (int)(factor * attr.GetLeftSubIndent());

        gint gindent;
        gint gsubindent;

        if (subIndent >= 0)
        {
            gindent = indent;
            gsubindent = -subIndent;
        }
        else
        {
            gindent = -subIndent;
            gsubindent = indent;
        }

        g_snprintf(buf, sizeof(buf), "WXINDENT %d %d", gindent, gsubindent);
        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                        buf );
        if (!tag)
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "left-margin", gindent, "indent", gsubindent, NULL );
        gtk_text_buffer_apply_tag (text_buffer, tag, &para_start, &para_end);
    }

    if (attr.HasTabs())
    {
        // Set tab stops

        // Clear old tabs
        GtkTextIter para_start, para_end = *end;
        gtk_text_buffer_get_iter_at_line( text_buffer,
                                          &para_start,
                                          gtk_text_iter_get_line(start) );
        gtk_text_iter_forward_line(&para_end);

        remove_handler_id = g_signal_connect (text_buffer, "remove_tag",
                                              G_CALLBACK(wxGtkOnRemoveTag),
                                              gpointer("WXTABS"));
        gtk_text_buffer_remove_all_tags( text_buffer, &para_start, &para_end );
        g_signal_handler_disconnect (text_buffer, remove_handler_id);

        const wxArrayInt& tabs = attr.GetTabs();

        wxString tagname = _T("WXTABS");
        g_snprintf(buf, sizeof(buf), "WXTABS");
        for (size_t i = 0; i < tabs.GetCount(); i++)
            tagname += wxString::Format(_T(" %d"), tabs[i]);

        const wxWX2MBbuf buf = tagname.mb_str(wxConvUTF8);

        tag = gtk_text_tag_table_lookup( gtk_text_buffer_get_tag_table( text_buffer ),
                                        buf );
        if (!tag)
        {
            // Factor to convert from 1/10th of a mm into pixels
            float factor;
#if GTK_CHECK_VERSION(2,2,0)
            if (!gtk_check_version(2,2,0))
                factor = (float)gdk_screen_get_width(gtk_widget_get_screen(text)) /
                          gdk_screen_get_width_mm(gtk_widget_get_screen(text)) / 10;
            else
#endif
                factor = (float)gdk_screen_width() / gdk_screen_width_mm() / 10;

            PangoTabArray* tabArray = pango_tab_array_new(tabs.GetCount(), TRUE);
            for (size_t i = 0; i < tabs.GetCount(); i++)
                pango_tab_array_set_tab(tabArray, i, PANGO_TAB_LEFT, (gint)(tabs[i] * factor));
            tag = gtk_text_buffer_create_tag( text_buffer, buf,
                                              "tabs", tabArray, NULL );
            pango_tab_array_free(tabArray);
        }
        gtk_text_buffer_apply_tag (text_buffer, tag, &para_start, &para_end);
    }
}
}

extern "C" {
static void wxGtkTextInsert(GtkWidget *text,
                            GtkTextBuffer *text_buffer,
                            const wxTextAttr& attr,
                            const wxCharBuffer& buffer)

{
    gint start_offset;
    GtkTextIter iter, start;

    gtk_text_buffer_get_iter_at_mark( text_buffer, &iter,
                                     gtk_text_buffer_get_insert (text_buffer) );
    start_offset = gtk_text_iter_get_offset (&iter);
    gtk_text_buffer_insert( text_buffer, &iter, buffer, strlen(buffer) );

    gtk_text_buffer_get_iter_at_offset (text_buffer, &start, start_offset);

    wxGtkTextApplyTagsFromAttr(text, text_buffer, attr, &start, &iter);
}
}

// ----------------------------------------------------------------------------
// "insert_text" for GtkEntry
// ----------------------------------------------------------------------------

extern "C" {
static void
gtk_insert_text_callback(GtkEditable *editable,
                         const gchar *new_text,
                         gint new_text_length,
                         gint *position,
                         wxTextCtrl *win)
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // we should only be called if we have a max len limit at all
    GtkEntry *entry = GTK_ENTRY (editable);

    wxCHECK_RET( entry->text_max_length, _T("shouldn't be called") );

    // check that we don't overflow the max length limit
    //
    // FIXME: this doesn't work when we paste a string which is going to be
    //        truncated
    if ( entry->text_length == entry->text_max_length )
    {
        // we don't need to run the base class version at all
        g_signal_stop_emission_by_name (editable, "insert_text");

        // remember that the next changed signal is to be ignored to avoid
        // generating a dummy wxEVT_COMMAND_TEXT_UPDATED event
        win->IgnoreNextTextUpdate();

        // and generate the correct one ourselves
        wxCommandEvent event(wxEVT_COMMAND_TEXT_MAXLEN, win->GetId());
        event.SetEventObject(win);
        event.SetString(win->GetValue());
        win->GetEventHandler()->ProcessEvent( event );
    }
}
}

// Implementation of wxTE_AUTO_URL for wxGTK2 by Mart Raudsepp,

extern "C" {
static void
au_apply_tag_callback(GtkTextBuffer *buffer,
                      GtkTextTag *tag,
                      GtkTextIter *start,
                      GtkTextIter *end,
                      gpointer textctrl)
{
    if(tag == gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), "wxUrl"))
        g_signal_stop_emission_by_name (buffer, "apply_tag");
}
}

//-----------------------------------------------------------------------------
//  GtkTextCharPredicates for gtk_text_iter_*_find_char
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
pred_whitespace (gunichar ch, gpointer user_data)
{
    return g_unichar_isspace(ch);
}
}

extern "C" {
static gboolean
pred_non_whitespace (gunichar ch, gpointer user_data)
{
    return !g_unichar_isspace(ch);
}
}

extern "C" {
static gboolean
pred_nonpunct (gunichar ch, gpointer user_data)
{
    return !g_unichar_ispunct(ch);
}
}

extern "C" {
static gboolean
pred_nonpunct_or_slash (gunichar ch, gpointer user_data)
{
    return !g_unichar_ispunct(ch) || ch == '/';
}
}

//-----------------------------------------------------------------------------
//  Check for links between s and e and correct tags as necessary
//-----------------------------------------------------------------------------

// This function should be made match better while being efficient at one point.
// Most probably with a row of regular expressions.
extern "C" {
static void
au_check_word( GtkTextIter *s, GtkTextIter *e )
{
    static const char *URIPrefixes[] =
    {
        "http://",
        "ftp://",
        "www.",
        "ftp.",
        "mailto://",
        "https://",
        "file://",
        "nntp://",
        "news://",
        "telnet://",
        "mms://",
        "gopher://",
        "prospero://",
        "wais://",
    };

    GtkTextIter start = *s, end = *e;
    GtkTextBuffer *buffer = gtk_text_iter_get_buffer(s);

    // Get our special link tag
    GtkTextTag *tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), "wxUrl");

    // Get rid of punctuation from beginning and end.
    // Might want to move this to au_check_range if an improved link checking doesn't
    // use some intelligent punctuation checking itself (beware of undesired iter modifications).
    if(g_unichar_ispunct( gtk_text_iter_get_char( &start ) ) )
        gtk_text_iter_forward_find_char( &start, pred_nonpunct, NULL, e );

    gtk_text_iter_backward_find_char( &end, pred_nonpunct_or_slash, NULL, &start );
    gtk_text_iter_forward_char(&end);

    gchar* text = gtk_text_iter_get_text( &start, &end );
    size_t len = strlen(text), prefix_len;
    size_t n;

    for( n = 0; n < WXSIZEOF(URIPrefixes); ++n )
    {
        prefix_len = strlen(URIPrefixes[n]);
        if((len > prefix_len) && !strncasecmp(text, URIPrefixes[n], prefix_len))
            break;
    }

    if(n < WXSIZEOF(URIPrefixes))
    {
        gulong signal_id = g_signal_handler_find (buffer,
                                                  (GSignalMatchType) (G_SIGNAL_MATCH_FUNC),
                                                  0, 0, NULL,
                                                  (gpointer)au_apply_tag_callback, NULL);

        g_signal_handler_block (buffer, signal_id);
        gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
        g_signal_handler_unblock (buffer, signal_id);
    }
}
}

extern "C" {
static void
au_check_range(GtkTextIter *s,
               GtkTextIter *range_end)
{
    GtkTextIter range_start = *s;
    GtkTextIter word_end;
    GtkTextBuffer *buffer = gtk_text_iter_get_buffer(s);
    GtkTextTag *tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), "wxUrl");

    gtk_text_buffer_remove_tag(buffer, tag, s, range_end);

    if(g_unichar_isspace(gtk_text_iter_get_char(&range_start)))
        gtk_text_iter_forward_find_char(&range_start, pred_non_whitespace, NULL, range_end);

    while(!gtk_text_iter_equal(&range_start, range_end))
    {
        word_end = range_start;
        gtk_text_iter_forward_find_char(&word_end, pred_whitespace, NULL, range_end);

        // Now we should have a word delimited by range_start and word_end, correct link tags
        au_check_word(&range_start, &word_end);

        range_start = word_end;
        gtk_text_iter_forward_find_char(&range_start, pred_non_whitespace, NULL, range_end);
    }
}
}

//-----------------------------------------------------------------------------
//  "insert-text" for GtkTextBuffer
//-----------------------------------------------------------------------------

extern "C" {
static void
au_insert_text_callback(GtkTextBuffer *buffer,
                        GtkTextIter *end,
                        gchar *text,
                        gint len,
                        wxTextCtrl *win)
{
    if (!len || !(win->GetWindowStyleFlag() & wxTE_AUTO_URL) )
        return;

    GtkTextIter start = *end;
    gtk_text_iter_backward_chars(&start, g_utf8_strlen(text, len));

    GtkTextIter line_start = start;
    GtkTextIter line_end = *end;
    GtkTextIter words_start = start;
    GtkTextIter words_end = *end;

    gtk_text_iter_set_line(&line_start, gtk_text_iter_get_line(&start));
    gtk_text_iter_forward_to_line_end(&line_end);
    gtk_text_iter_backward_find_char(&words_start, pred_whitespace, NULL, &line_start);
    gtk_text_iter_forward_find_char(&words_end, pred_whitespace, NULL, &line_end);

    au_check_range(&words_start, &words_end);
}
}

//-----------------------------------------------------------------------------
//  "delete-range" for GtkTextBuffer
//-----------------------------------------------------------------------------

extern "C" {
static void
au_delete_range_callback(GtkTextBuffer *buffer,
                         GtkTextIter *start,
                         GtkTextIter *end,
                         wxTextCtrl *win)
{
    if( !(win->GetWindowStyleFlag() & wxTE_AUTO_URL) )
        return;

    GtkTextIter line_start = *start, line_end = *end;

    gtk_text_iter_set_line(&line_start, gtk_text_iter_get_line(start));
    gtk_text_iter_forward_to_line_end(&line_end);
    gtk_text_iter_backward_find_char(start, pred_whitespace, NULL, &line_start);
    gtk_text_iter_forward_find_char(end, pred_whitespace, NULL, &line_end);

    au_check_range(start, end);
}
}


//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_text_changed_callback( GtkWidget *widget, wxTextCtrl *win )
{
    if ( win->IgnoreTextUpdate() )
        return;

    if (!win->m_hasVMT) return;

    if (g_isIdle)
        wxapp_install_idle_handler();

    if ( win->MarkDirtyOnChange() )
        win->MarkDirty();

    win->SendTextUpdatedEvent();
}
}

//-----------------------------------------------------------------------------
//  clipboard events: "copy-clipboard", "cut-clipboard", "paste-clipboard"
//-----------------------------------------------------------------------------

// common part of the event handlers below
static void
handle_text_clipboard_callback( GtkWidget *widget, wxTextCtrl *win,
                                wxEventType eventType, const gchar * signal_name)
{
    wxClipboardTextEvent event( eventType, win->GetId() );
    event.SetEventObject( win );
    if ( win->GetEventHandler()->ProcessEvent( event ) )
    {
        // don't let the default processing to take place if we did something
        // ourselves in the event handler
        g_signal_stop_emission_by_name (widget, signal_name);
    }
}

extern "C" {
static void
gtk_copy_clipboard_callback( GtkWidget *widget, wxTextCtrl *win )
{
    handle_text_clipboard_callback(
        widget, win, wxEVT_COMMAND_TEXT_COPY, "copy-clipboard" );
}

static void
gtk_cut_clipboard_callback( GtkWidget *widget, wxTextCtrl *win )
{
    handle_text_clipboard_callback(
        widget, win, wxEVT_COMMAND_TEXT_CUT, "cut-clipboard" );
}

static void
gtk_paste_clipboard_callback( GtkWidget *widget, wxTextCtrl *win )
{
    handle_text_clipboard_callback(
        widget, win, wxEVT_COMMAND_TEXT_PASTE, "paste-clipboard" );
}
}

//-----------------------------------------------------------------------------
// "expose_event" from scrolled window and textview
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_text_exposed_callback( GtkWidget *widget, GdkEventExpose *event, wxTextCtrl *win )
{
    return TRUE;
}
}


//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxTextCtrlBase)

BEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
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

    // wxTE_AUTO_URL wxTextUrl support. Currently only creates
    // wxTextUrlEvent in the same cases as wxMSW, more can be added here.
    EVT_MOTION      (wxTextCtrl::OnUrlMouseEvent)
    EVT_LEFT_DOWN   (wxTextCtrl::OnUrlMouseEvent)
    EVT_LEFT_UP     (wxTextCtrl::OnUrlMouseEvent)
    EVT_LEFT_DCLICK (wxTextCtrl::OnUrlMouseEvent)
    EVT_RIGHT_DOWN  (wxTextCtrl::OnUrlMouseEvent)
    EVT_RIGHT_UP    (wxTextCtrl::OnUrlMouseEvent)
    EVT_RIGHT_DCLICK(wxTextCtrl::OnUrlMouseEvent)
END_EVENT_TABLE()

void wxTextCtrl::Init()
{
    m_dontMarkDirty =
    m_modified = false;

    m_countUpdatesToIgnore = 0;

    SetUpdateFont(false);

    m_text = NULL;
    m_frozenness = 0;
    m_gdkHandCursor = NULL;
    m_gdkXTermCursor = NULL;
}

wxTextCtrl::~wxTextCtrl()
{
    if(m_gdkHandCursor)
        gdk_cursor_unref(m_gdkHandCursor);
    if(m_gdkXTermCursor)
        gdk_cursor_unref(m_gdkXTermCursor);
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
    m_needParent = true;
    m_acceptsFocus = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxTextCtrl creation failed") );
        return false;
    }

    bool multi_line = (style & wxTE_MULTILINE) != 0;

    if (multi_line)
    {
        // Create view
        m_text = gtk_text_view_new();

        m_buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(m_text) );

        // create scrolled window
        m_widget = gtk_scrolled_window_new( NULL, NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( m_widget ),
                                        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        // for ScrollLines/Pages
        m_scrollBar[1] = (GtkRange*)((GtkScrolledWindow*)m_widget)->vscrollbar;

        // Insert view into scrolled window
        gtk_container_add( GTK_CONTAINER(m_widget), m_text );

        // translate wx wrapping style to GTK+
        GtkWrapMode wrap;
        if ( HasFlag( wxTE_DONTWRAP ) )
            wrap = GTK_WRAP_NONE;
        else if ( HasFlag( wxTE_CHARWRAP ) )
            wrap = GTK_WRAP_CHAR;
        else if ( HasFlag( wxTE_WORDWRAP ) )
            wrap = GTK_WRAP_WORD;
        else // HasFlag(wxTE_BESTWRAP) always true as wxTE_BESTWRAP == 0
        {
            // GTK_WRAP_WORD_CHAR seems to be new in GTK+ 2.4
#ifdef __WXGTK24__
            if ( !gtk_check_version(2,4,0) )
            {
                wrap = GTK_WRAP_WORD_CHAR;
            }
            else
#endif
            wrap = GTK_WRAP_WORD;
        }

        gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( m_text ), wrap );

        GtkScrolledWindowSetBorder(m_widget, style);

        gtk_widget_add_events( GTK_WIDGET(m_text), GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );

        GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );
    }
    else
    {
        // a single-line text control: no need for scrollbars
        m_widget =
        m_text = gtk_entry_new();

        if (style & wxNO_BORDER)
            g_object_set (m_text, "has-frame", FALSE, NULL);
    }

    m_parent->DoAddChild( this );

    m_focusWidget = m_text;

    PostCreation(size);

    if (multi_line)
    {
        gtk_widget_show(m_text);
    }

    if (!value.empty())
    {
        SetValue( value );
    }

    if (style & wxTE_PASSWORD)
    {
        if (!multi_line)
            gtk_entry_set_visibility( GTK_ENTRY(m_text), FALSE );
    }

    if (style & wxTE_READONLY)
    {
        if (!multi_line)
            gtk_editable_set_editable( GTK_EDITABLE(m_text), FALSE );
        else
            gtk_text_view_set_editable( GTK_TEXT_VIEW( m_text), FALSE);
    }

    if (multi_line)
    {
        if (style & wxTE_RIGHT)
            gtk_text_view_set_justification( GTK_TEXT_VIEW(m_text), GTK_JUSTIFY_RIGHT );
        else if (style & wxTE_CENTRE)
            gtk_text_view_set_justification( GTK_TEXT_VIEW(m_text), GTK_JUSTIFY_CENTER );
        // Left justify (alignment) is the default and we don't need to apply GTK_JUSTIFY_LEFT
    }
    else
    {
#ifdef __WXGTK24__
        // gtk_entry_set_alignment was introduced in gtk+-2.3.5
        if (!gtk_check_version(2,4,0))
        {
            if (style & wxTE_RIGHT)
                gtk_entry_set_alignment( GTK_ENTRY(m_text), 1.0 );
            else if (style & wxTE_CENTRE)
                gtk_entry_set_alignment( GTK_ENTRY(m_text), 0.5 );
        }
#endif
    }

    // We want to be notified about text changes.
    if (multi_line)
    {
        g_signal_connect (m_buffer, "changed",
                          G_CALLBACK (gtk_text_changed_callback), this);

        // .. and handle URLs on multi-line controls with wxTE_AUTO_URL style
        if (style & wxTE_AUTO_URL)
        {
            GtkTextIter start, end;
            m_gdkHandCursor = gdk_cursor_new(GDK_HAND2);
            m_gdkXTermCursor = gdk_cursor_new(GDK_XTERM);

            // We create our wxUrl tag here for slight efficiency gain - we
            // don't have to check for the tag existance in callbacks,
            // hereby it's guaranteed to exist.
            gtk_text_buffer_create_tag(m_buffer, "wxUrl",
                                       "foreground", "blue",
                                       "underline", PANGO_UNDERLINE_SINGLE,
                                       NULL);

            // Check for URLs after each text change
            g_signal_connect_after (m_buffer, "insert_text",
                                    G_CALLBACK (au_insert_text_callback), this);
            g_signal_connect_after (m_buffer, "delete_range",
                                    G_CALLBACK (au_delete_range_callback), this);

            // Block all wxUrl tag applying unless we do it ourselves, in which case we
            // block this callback temporarily. This takes care of gtk+ internal
            // gtk_text_buffer_insert_range* calls that would copy our URL tag otherwise,
            // which is undesired because only a part of the URL might be copied.
            // The insert-text signal emitted inside it will take care of newly formed
            // or wholly copied URLs.
            g_signal_connect (m_buffer, "apply_tag",
                              G_CALLBACK (au_apply_tag_callback), NULL);

            // Check for URLs in the initial string passed to Create
            gtk_text_buffer_get_start_iter(m_buffer, &start);
            gtk_text_buffer_get_end_iter(m_buffer, &end);
            au_check_range(&start, &end);
        }
    }
    else
    {
        g_signal_connect (m_text, "changed",
                          G_CALLBACK (gtk_text_changed_callback), this);
    }

    g_signal_connect (m_text, "copy-clipboard",
                      G_CALLBACK (gtk_copy_clipboard_callback), this);
    g_signal_connect (m_text, "cut-clipboard",
                      G_CALLBACK (gtk_cut_clipboard_callback), this);
    g_signal_connect (m_text, "paste-clipboard",
                      G_CALLBACK (gtk_paste_clipboard_callback), this);

    m_cursor = wxCursor( wxCURSOR_IBEAM );

    wxTextAttr attrDef(GetForegroundColour(), GetBackgroundColour(), GetFont());
    SetDefaultStyle( attrDef );

    return true;
}


void wxTextCtrl::CalculateScrollbar()
{
}

wxString wxTextCtrl::GetValue() const
{
    wxCHECK_MSG( m_text != NULL, wxEmptyString, wxT("invalid text ctrl") );

    wxString tmp;
    if ( IsMultiLine() )
    {
        GtkTextIter start;
        gtk_text_buffer_get_start_iter( m_buffer, &start );
        GtkTextIter end;
        gtk_text_buffer_get_end_iter( m_buffer, &end );
        wxGtkString text(gtk_text_buffer_get_text(m_buffer, &start, &end, true));

        const wxWxCharBuffer buf = wxGTK_CONV_BACK(text);
        if ( buf )
            tmp = buf;
    }
    else
    {
        const gchar *text = gtk_entry_get_text( GTK_ENTRY(m_text) );
        const wxWxCharBuffer buf = wxGTK_CONV_BACK( text );
        if ( buf )
            tmp = buf;
    }

    return tmp;
}

wxFontEncoding wxTextCtrl::GetTextEncoding() const
{
    // GTK+ uses UTF-8 internally, we need to convert to it but from which
    // encoding?

    // first check the default text style (we intentionally don't check the
    // style for the current position as it doesn't make sense for SetValue())
    const wxTextAttr& style = GetDefaultStyle();
    wxFontEncoding enc = style.HasFont() ? style.GetFont().GetEncoding()
                                         : wxFONTENCODING_SYSTEM;

    // fall back to the controls font if no style
    if ( enc == wxFONTENCODING_SYSTEM && m_hasFont )
        enc = GetFont().GetEncoding();

    return enc;
}

bool wxTextCtrl::IsEmpty() const
{
    if ( IsMultiLine() )
        return gtk_text_buffer_get_char_count(m_buffer) != 0;

    return wxTextCtrlBase::IsEmpty();
}

void wxTextCtrl::DoSetValue( const wxString &value, int flags )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    // the control won't be modified any more as we programmatically replace
    // all the existing text, so reset the flag and don't set it again (and do
    // it now, before the text event handler is ran so that IsModified() called
    // from there returns the expected value)
    m_modified = false;
    DontMarkDirtyOnNextChange();

    const wxCharBuffer buffer(wxGTK_CONV_ENC(value, GetTextEncoding()));
    if ( !buffer )
    {
        // see comment in WriteText() as to why we must warn the user about
        // this
        wxLogWarning(_("Failed to set text in the text control."));
        return;
    }

    // if the control is not empty, two "changed" signals are emitted,
    // otherwise only one and we need to ignore either both or one of them
    int ignore = flags & SetValue_SendEvent ? 0 : 1;
    if ( !IsEmpty() )
        ignore++;

    if ( ignore )
        IgnoreNextTextUpdate(ignore);

    if ( IsMultiLine() )
    {
        gtk_text_buffer_set_text( m_buffer, buffer, strlen(buffer) );
    }
    else // single line
    {
        gtk_entry_set_text( GTK_ENTRY(m_text), buffer );
    }

    // GRG, Jun/2000: Changed this after a lot of discussion in
    //   the lists. wxWidgets 2.2 will have a set of flags to
    //   customize this behaviour.
    SetInsertionPoint(0);
}

void wxTextCtrl::WriteText( const wxString &text )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( text.empty() )
        return;

    // check if we have a specific style for the current position
    wxFontEncoding enc = wxFONTENCODING_SYSTEM;
    wxTextAttr style;
    if ( GetStyle(GetInsertionPoint(), style) && style.HasFont() )
    {
        enc = style.GetFont().GetEncoding();
    }

    if ( enc == wxFONTENCODING_SYSTEM )
        enc = GetTextEncoding();

    const wxCharBuffer buffer(wxGTK_CONV_ENC(text, enc));
    if ( !buffer )
    {
        // we must log an error here as losing the text like this can be a
        // serious problem (e.g. imagine the document edited by user being
        // empty instead of containing the correct text)
        wxLogWarning(_("Failed to insert text in the control."));
        return;
    }

    // we're changing the text programmatically
    DontMarkDirtyOnNextChange();

    if ( IsMultiLine() )
    {
        // First remove the selection if there is one
        // TODO:  Is there an easier GTK specific way to do this?
        long from, to;
        GetSelection(&from, &to);
        if (from != to)
            Remove(from, to);

        // Insert the text
        wxGtkTextInsert( m_text, m_buffer, m_defaultStyle, buffer );

        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment( GTK_SCROLLED_WINDOW(m_widget) );
        // Scroll to cursor, but only if scrollbar thumb is at the very bottom
        if ( wxIsSameDouble(adj->value, adj->upper - adj->page_size) )
        {
            gtk_text_view_scroll_to_mark( GTK_TEXT_VIEW(m_text),
                    gtk_text_buffer_get_insert( m_buffer ), 0.0, FALSE, 0.0, 1.0 );
        }
    }
    else // single line
    {
        // First remove the selection if there is one
        gtk_editable_delete_selection( GTK_EDITABLE(m_text) );

        // This moves the cursor pos to behind the inserted text.
        gint len = gtk_editable_get_position(GTK_EDITABLE(m_text));

        gtk_editable_insert_text( GTK_EDITABLE(m_text), buffer, strlen(buffer), &len );

        // Bring entry's cursor uptodate.
        gtk_editable_set_position( GTK_EDITABLE(m_text), len );
    }
}

void wxTextCtrl::AppendText( const wxString &text )
{
    SetInsertionPointEnd();
    WriteText( text );
}

wxString wxTextCtrl::GetLineText( long lineNo ) const
{
    wxString result;
    if ( IsMultiLine() )
    {
        GtkTextIter line;
        gtk_text_buffer_get_iter_at_line(m_buffer,&line,lineNo);
        GtkTextIter end = line;
        gtk_text_iter_forward_to_line_end(&end);
        wxGtkString text(gtk_text_buffer_get_text(m_buffer, &line, &end, true));
        result = wxGTK_CONV_BACK(text);
    }
    else
    {
        if (lineNo == 0)
            result = GetValue();
    }
    return result;
}

void wxTextCtrl::OnDropFiles( wxDropFilesEvent &WXUNUSED(event) )
{
  /* If you implement this, don't forget to update the documentation!
   * (file docs/latex/wx/text.tex) */
    wxFAIL_MSG( wxT("wxTextCtrl::OnDropFiles not implemented") );
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y ) const
{
    if ( IsMultiLine() )
    {
        GtkTextIter iter;

        if (pos > GetLastPosition())
            return false;

        gtk_text_buffer_get_iter_at_offset(m_buffer, &iter, pos);

        if ( y )
            *y = gtk_text_iter_get_line(&iter);
        if ( x )
            *x = gtk_text_iter_get_line_offset(&iter);
    }
    else // single line control
    {
        if ( pos <= GTK_ENTRY(m_text)->text_length )
        {
            if ( y )
                *y = 0;
            if ( x )
                *x = pos;
        }
        else
        {
            // index out of bounds
            return false;
        }
    }

    return true;
}

long wxTextCtrl::XYToPosition(long x, long y ) const
{
    if ( IsSingleLine() )
        return 0;

    GtkTextIter iter;
    if (y >= gtk_text_buffer_get_line_count (m_buffer))
        return -1;

    gtk_text_buffer_get_iter_at_line(m_buffer, &iter, y);
    if (x >= gtk_text_iter_get_chars_in_line (&iter))
        return -1;

    return gtk_text_iter_get_offset(&iter) + x;
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    if ( IsMultiLine() )
    {
        int last_line = gtk_text_buffer_get_line_count( m_buffer ) - 1;
        if (lineNo > last_line)
            return -1;

        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_line(m_buffer, &iter, lineNo);
        // get_chars_in_line return includes paragraph delimiters, so need to subtract 1 IF it is not the last line
        return gtk_text_iter_get_chars_in_line(&iter) - ((lineNo == last_line) ? 0 : 1);
    }
    else
    {
        wxString str = GetLineText (lineNo);
        return (int) str.length();
    }
}

int wxTextCtrl::GetNumberOfLines() const
{
    if ( IsMultiLine() )
    {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_offset( m_buffer, &iter, 0 );

        // move forward by one display line until the end is reached
        int lineCount = 1;
        while ( gtk_text_view_forward_display_line(GTK_TEXT_VIEW(m_text), &iter) )
        {
            lineCount++;
        }

        // If the last character in the text buffer is a newline,
        // gtk_text_view_forward_display_line() will return false without that
        // line being counted. Must add one manually in that case.
        GtkTextIter lastCharIter;
        gtk_text_buffer_get_iter_at_offset
        (
            m_buffer,
            &lastCharIter,
            gtk_text_buffer_get_char_count(m_buffer) - 1
        );
        gchar lastChar = gtk_text_iter_get_char( &lastCharIter );
        if ( lastChar == wxT('\n') )
            lineCount++;

        return lineCount;
    }
    else // single line
    {
        return 1;
    }
}

void wxTextCtrl::SetInsertionPoint( long pos )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_offset( m_buffer, &iter, pos );
        gtk_text_buffer_place_cursor( m_buffer, &iter );
        gtk_text_view_scroll_mark_onscreen
        (
            GTK_TEXT_VIEW(m_text),
            gtk_text_buffer_get_insert( m_buffer )
        );
    }
    else
    {
        // FIXME: Is the editable's cursor really uptodate without double set_position in GTK2?
        gtk_editable_set_position(GTK_EDITABLE(m_text), int(pos));
    }
}

void wxTextCtrl::SetInsertionPointEnd()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter( m_buffer, &end );
        gtk_text_buffer_place_cursor( m_buffer, &end );
    }
    else
    {
        gtk_editable_set_position( GTK_EDITABLE(m_text), -1 );
    }
}

void wxTextCtrl::SetEditable( bool editable )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        gtk_text_view_set_editable( GTK_TEXT_VIEW(m_text), editable );
    }
    else
    {
        gtk_editable_set_editable( GTK_EDITABLE(m_text), editable );
    }
}

bool wxTextCtrl::Enable( bool enable )
{
    if (!wxWindowBase::Enable(enable))
    {
        // nothing to do
        return false;
    }

    if ( IsMultiLine() )
    {
        SetEditable( enable );
    }
    else
    {
        gtk_widget_set_sensitive( m_text, enable );
    }

    return true;
}

// wxGTK-specific: called recursively by Enable,
// to give widgets an oppprtunity to correct their colours after they
// have been changed by Enable
void wxTextCtrl::OnParentEnable( bool enable )
{
    // If we have a custom background colour, we use this colour in both
    // disabled and enabled mode, or we end up with a different colour under the
    // text.
    wxColour oldColour = GetBackgroundColour();
    if (oldColour.Ok())
    {
        // Need to set twice or it'll optimize the useful stuff out
        if (oldColour == * wxWHITE)
            SetBackgroundColour(*wxBLACK);
        else
            SetBackgroundColour(*wxWHITE);
        SetBackgroundColour(oldColour);
    }
}

void wxTextCtrl::MarkDirty()
{
    m_modified = true;
}

void wxTextCtrl::DiscardEdits()
{
    m_modified = false;
}

// ----------------------------------------------------------------------------
// max text length support
// ----------------------------------------------------------------------------

bool wxTextCtrl::IgnoreTextUpdate()
{
    if ( m_countUpdatesToIgnore > 0 )
    {
        m_countUpdatesToIgnore--;

        return true;
    }

    return false;
}

bool wxTextCtrl::MarkDirtyOnChange()
{
    if ( m_dontMarkDirty )
    {
        m_dontMarkDirty = false;

        return false;
    }

    return true;
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    if ( !HasFlag(wxTE_MULTILINE) )
    {
        gtk_entry_set_max_length(GTK_ENTRY(m_text), len);

        // there is a bug in GTK+ 1.2.x: "changed" signal is emitted even if
        // we had tried to enter more text than allowed by max text length and
        // the text wasn't really changed
        //
        // to detect this and generate TEXT_MAXLEN event instead of
        // TEXT_CHANGED one in this case we also catch "insert_text" signal
        //
        // when max len is set to 0 we disconnect our handler as it means that
        // we shouldn't check anything any more
        if ( len )
        {
            g_signal_connect (m_text, "insert_text",
                              G_CALLBACK (gtk_insert_text_callback), this);
        }
        else // no checking
        {
            g_signal_handlers_disconnect_by_func (m_text,
                    (gpointer) gtk_insert_text_callback, this);
        }
    }
}

void wxTextCtrl::SetSelection( long from, long to )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if (from == -1 && to == -1)
    {
        from = 0;
        to = GetValue().length();
    }

    if ( IsMultiLine() )
    {
        GtkTextIter fromi, toi;
        gtk_text_buffer_get_iter_at_offset( m_buffer, &fromi, from );
        gtk_text_buffer_get_iter_at_offset( m_buffer, &toi, to );

        gtk_text_buffer_place_cursor( m_buffer, &toi );
        gtk_text_buffer_move_mark_by_name( m_buffer, "selection_bound", &fromi );
    }
    else
    {
        gtk_editable_select_region( GTK_EDITABLE(m_text), (gint)from, (gint)to );
    }
}

void wxTextCtrl::ShowPosition( long pos )
{
    if ( IsMultiLine() )
    {
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter( m_buffer, &iter );
        gtk_text_iter_set_offset( &iter, pos );
        GtkTextMark *mark = gtk_text_buffer_create_mark( m_buffer, NULL, &iter, TRUE );
        gtk_text_view_scroll_to_mark( GTK_TEXT_VIEW(m_text), mark, 0.0, FALSE, 0.0, 0.0 );
    }
}

wxTextCtrlHitTestResult
wxTextCtrl::HitTest(const wxPoint& pt, long *pos) const
{
    if ( !IsMultiLine() )
    {
        // not supported
        return wxTE_HT_UNKNOWN;
    }

    int x, y;
    gtk_text_view_window_to_buffer_coords
    (
        GTK_TEXT_VIEW(m_text),
        GTK_TEXT_WINDOW_TEXT,
        pt.x, pt.y,
        &x, &y
    );

    GtkTextIter iter;
    gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(m_text), &iter, x, y);
    if ( pos )
        *pos = gtk_text_iter_get_offset(&iter);

    return wxTE_HT_ON_TEXT;
}

long wxTextCtrl::GetInsertionPoint() const
{
    wxCHECK_MSG( m_text != NULL, 0, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        // There is no direct accessor for the cursor, but
        // internally, the cursor is the "mark" called
        // "insert" in the text view's btree structure.

        GtkTextMark *mark = gtk_text_buffer_get_insert( m_buffer );
        GtkTextIter cursor;
        gtk_text_buffer_get_iter_at_mark( m_buffer, &cursor, mark );

        return gtk_text_iter_get_offset( &cursor );
    }
    else
    {
        return (long) gtk_editable_get_position(GTK_EDITABLE(m_text));
    }
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    wxCHECK_MSG( m_text != NULL, 0, wxT("invalid text ctrl") );

    int pos = 0;

    if ( IsMultiLine() )
    {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter( m_buffer, &end );

        pos = gtk_text_iter_get_offset( &end );
    }
    else
    {
        pos = GTK_ENTRY(m_text)->text_length;
    }

    return (long)pos;
}

void wxTextCtrl::Remove( long from, long to )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        GtkTextIter fromi, toi;
        gtk_text_buffer_get_iter_at_offset( m_buffer, &fromi, from );
        gtk_text_buffer_get_iter_at_offset( m_buffer, &toi, to );

        gtk_text_buffer_delete( m_buffer, &fromi, &toi );
    }
    else // single line
        gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::Replace( long from, long to, const wxString &value )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    Remove( from, to );

    if (!value.empty())
    {
        SetInsertionPoint( from );
        WriteText( value );
    }
}

void wxTextCtrl::Cut()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
        g_signal_emit_by_name (m_text, "cut-clipboard");
    else
        gtk_editable_cut_clipboard(GTK_EDITABLE(m_text));
}

void wxTextCtrl::Copy()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
        g_signal_emit_by_name (m_text, "copy-clipboard");
    else
        gtk_editable_copy_clipboard(GTK_EDITABLE(m_text));
}

void wxTextCtrl::Paste()
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
        g_signal_emit_by_name (m_text, "paste-clipboard");
    else
        gtk_editable_paste_clipboard(GTK_EDITABLE(m_text));
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
    //wxFAIL_MSG( wxT("wxTextCtrl::CanUndo not implemented") );
    return false;
}

bool wxTextCtrl::CanRedo() const
{
    // TODO
    //wxFAIL_MSG( wxT("wxTextCtrl::CanRedo not implemented") );
    return false;
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* fromOut, long* toOut) const
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    gint from = -1;
    gint to = -1;
    bool haveSelection = false;

     if ( IsMultiLine() )
     {
         GtkTextIter ifrom, ito;
         if ( gtk_text_buffer_get_selection_bounds(m_buffer, &ifrom, &ito) )
         {
             haveSelection = true;
             from = gtk_text_iter_get_offset(&ifrom);
             to = gtk_text_iter_get_offset(&ito);
         }
     }
     else  // not multi-line
     {
         if ( gtk_editable_get_selection_bounds( GTK_EDITABLE(m_text),
                                                 &from, &to) )
         {
             haveSelection = true;
         }
     }

     if (! haveSelection )
          from = to = GetInsertionPoint();

     if ( from > to )
     {
         // exchange them to be compatible with wxMSW
         gint tmp = from;
         from = to;
         to = tmp;
     }

    if ( fromOut )
        *fromOut = from;
    if ( toOut )
        *toOut = to;
}


bool wxTextCtrl::IsEditable() const
{
    wxCHECK_MSG( m_text != NULL, false, wxT("invalid text ctrl") );

    if ( IsMultiLine() )
    {
        return gtk_text_view_get_editable(GTK_TEXT_VIEW(m_text));
    }
    else
    {
        return gtk_editable_get_editable(GTK_EDITABLE(m_text));
    }
}

bool wxTextCtrl::IsModified() const
{
    return m_modified;
}

void wxTextCtrl::Clear()
{
    SetValue( wxEmptyString );
}

void wxTextCtrl::OnChar( wxKeyEvent &key_event )
{
    wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );

    if ( key_event.GetKeyCode() == WXK_RETURN )
    {
        if ( HasFlag(wxTE_PROCESS_ENTER) )
        {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
            event.SetEventObject(this);
            event.SetString(GetValue());
            if ( GetEventHandler()->ProcessEvent(event) )
                return;
        }

        // FIXME: this is not the right place to do it, wxDialog::OnCharHook()
        //        probably is
        if ( IsSingleLine() )
        {
            // This will invoke the dialog default action, such
            // as the clicking the default button.

            wxWindow *top_frame = m_parent;
            while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
                top_frame = top_frame->GetParent();

            if (top_frame && GTK_IS_WINDOW(top_frame->m_widget))
            {
                GtkWindow *window = GTK_WINDOW(top_frame->m_widget);

                if (window->default_widget)
                {
                    gtk_widget_activate (window->default_widget);
                    return;
                }
            }
        }
    }

    key_event.Skip();
}

GtkWidget* wxTextCtrl::GetConnectWidget()
{
    return GTK_WIDGET(m_text);
}

GdkWindow *wxTextCtrl::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    if ( IsMultiLine() )
    {
        return gtk_text_view_get_window(GTK_TEXT_VIEW(m_text),
                                        GTK_TEXT_WINDOW_TEXT );
    }
    else
    {
        return GTK_ENTRY(m_text)->text_area;
    }
}

// the font will change for subsequent text insertiongs
bool wxTextCtrl::SetFont( const wxFont &font )
{
    wxCHECK_MSG( m_text != NULL, false, wxT("invalid text ctrl") );

    if ( !wxTextCtrlBase::SetFont(font) )
    {
        // font didn't change, nothing to do
        return false;
    }

    if ( IsMultiLine() )
    {
        SetUpdateFont(true);

        m_defaultStyle.SetFont(font);

        ChangeFontGlobally();
    }

    return true;
}

void wxTextCtrl::ChangeFontGlobally()
{
    // this method is very inefficient and hence should be called as rarely as
    // possible!
    //
    // TODO: it can be implemented much more efficiently for GTK2
    wxASSERT_MSG( IsMultiLine(),
                  _T("shouldn't be called for single line controls") );

    wxString value = GetValue();
    if ( !value.empty() )
    {
        SetUpdateFont(false);

        Clear();
        AppendText(value);
    }
}

bool wxTextCtrl::SetForegroundColour(const wxColour& colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
        return false;

    // update default fg colour too
    m_defaultStyle.SetTextColour(colour);

    return true;
}

bool wxTextCtrl::SetBackgroundColour( const wxColour &colour )
{
    wxCHECK_MSG( m_text != NULL, false, wxT("invalid text ctrl") );

    if ( !wxControl::SetBackgroundColour( colour ) )
        return false;

    if (!m_backgroundColour.Ok())
        return false;

    // change active background color too
    m_defaultStyle.SetBackgroundColour( colour );

    return true;
}

bool wxTextCtrl::SetStyle( long start, long end, const wxTextAttr& style )
{
    if ( IsMultiLine() )
    {
        if ( style.IsDefault() )
        {
            // nothing to do
            return true;
        }

        gint l = gtk_text_buffer_get_char_count( m_buffer );

        wxCHECK_MSG( start >= 0 && end <= l, false,
                     _T("invalid range in wxTextCtrl::SetStyle") );

        GtkTextIter starti, endi;
        gtk_text_buffer_get_iter_at_offset( m_buffer, &starti, start );
        gtk_text_buffer_get_iter_at_offset( m_buffer, &endi, end );

        // use the attributes from style which are set in it and fall back
        // first to the default style and then to the text control default
        // colours for the others
        wxTextAttr attr = wxTextAttr::Combine(style, m_defaultStyle, this);

        wxGtkTextApplyTagsFromAttr( m_widget, m_buffer, attr, &starti, &endi );

        return true;
    }

    // else single line
    // cannot do this for GTK+'s Entry widget
    return false;
}

void wxTextCtrl::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_text, style);
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

wxSize wxTextCtrl::DoGetBestSize() const
{
    // FIXME should be different for multi-line controls...
    wxSize ret( wxControl::DoGetBestSize() );
    wxSize best(80, ret.y);
    CacheBestSize(best);
    return best;
}

// ----------------------------------------------------------------------------
// freeze/thaw
// ----------------------------------------------------------------------------

void wxTextCtrl::Freeze()
{
    if ( HasFlag(wxTE_MULTILINE) )
    {
        if ( !m_frozenness++ )
        {
            // freeze textview updates and remove buffer
            g_signal_connect (m_text, "expose_event",
                              G_CALLBACK (gtk_text_exposed_callback), this);
            g_signal_connect (m_widget, "expose_event",
                              G_CALLBACK (gtk_text_exposed_callback), this);
            gtk_widget_set_sensitive(m_widget, false);
            g_object_ref(m_buffer);
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(m_text), gtk_text_buffer_new(NULL));
        }
    }
}

void wxTextCtrl::Thaw()
{
    if ( HasFlag(wxTE_MULTILINE) )
    {
        wxASSERT_MSG( m_frozenness > 0, _T("Thaw() without matching Freeze()") );

        if ( !--m_frozenness )
        {
            // Reattach buffer and thaw textview updates
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(m_text), m_buffer);
            g_object_unref(m_buffer);
            gtk_widget_set_sensitive(m_widget, true);
            g_signal_handlers_disconnect_by_func (m_widget,
                    (gpointer) gtk_text_exposed_callback, this);
            g_signal_handlers_disconnect_by_func (m_text,
                    (gpointer) gtk_text_exposed_callback, this);
        }
    }
}

// ----------------------------------------------------------------------------
// wxTextUrlEvent passing if style & wxTE_AUTO_URL
// ----------------------------------------------------------------------------

// FIXME: when dragging on a link the sample gets an "Unknown event".
// This might be an excessive event from us or a buggy wxMouseEvent::Moving() or
// a buggy sample, or something else
void wxTextCtrl::OnUrlMouseEvent(wxMouseEvent& event)
{
    event.Skip();
    if( !HasFlag(wxTE_AUTO_URL) )
        return;

    gint x, y;
    GtkTextIter start, end;
    GtkTextTag *tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(m_buffer),
                                                "wxUrl");

    gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(m_text), GTK_TEXT_WINDOW_WIDGET,
                                          event.GetX(), event.GetY(), &x, &y);

    gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(m_text), &end, x, y);
    if (!gtk_text_iter_has_tag(&end, tag))
    {
        gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(m_text),
                              GTK_TEXT_WINDOW_TEXT), m_gdkXTermCursor);
        return;
    }

    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(m_text),
                          GTK_TEXT_WINDOW_TEXT), m_gdkHandCursor);

    start = end;
    if(!gtk_text_iter_begins_tag(&start, tag))
        gtk_text_iter_backward_to_tag_toggle(&start, tag);
    if(!gtk_text_iter_ends_tag(&end, tag))
        gtk_text_iter_forward_to_tag_toggle(&end, tag);

    // Native context menu is probably not desired on an URL.
    // Consider making this dependant on ProcessEvent(wxTextUrlEvent) return value
    if(event.GetEventType() == wxEVT_RIGHT_DOWN)
        event.Skip(false);

    wxTextUrlEvent url_event(m_windowId, event,
                             gtk_text_iter_get_offset(&start),
                             gtk_text_iter_get_offset(&end));

    InitCommandEvent(url_event);
    // Is that a good idea? Seems not (pleasure with gtk_text_view_start_selection_drag)
    //event.Skip(!GetEventHandler()->ProcessEvent(url_event));
    GetEventHandler()->ProcessEvent(url_event);
}

bool wxTextCtrl::GTKProcessEvent(wxEvent& event) const
{
    bool rc = wxTextCtrlBase::GTKProcessEvent(event);

    // GtkTextView starts a drag operation when left mouse button is pressed
    // and ends it when it is released and if it doesn't get the release event
    // the next click on a control results in an assertion failure inside
    // gtk_text_view_start_selection_drag() which simply *kills* the program
    // without anything we can do about it, so always let GTK+ have this event
    return rc && (IsSingleLine() || event.GetEventType() != wxEVT_LEFT_UP);
}

// static
wxVisualAttributes
wxTextCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_entry_new, true);
}
