///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/textentry.cpp
// Purpose:     wxTextEntry implementation for wxGTK
// Author:      Vadim Zeitlin
// Created:     2007-09-24
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TEXTCTRL || wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/textentry.h"
    #include "wx/textctrl.h"
    #include "wx/window.h"
#endif //WX_PRECOMP

#include "wx/textcompleter.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/object.h"
#include "wx/gtk/private/string.h"

// ----------------------------------------------------------------------------
// wxTextCoalesceData
// ----------------------------------------------------------------------------

class wxTextCoalesceData
{
public:
    wxTextCoalesceData(GtkWidget* widget, gulong handlerAfterKeyPress)
        : m_handlerAfterKeyPress(handlerAfterKeyPress)
    {
        m_inKeyPress = false;
        m_pendingTextChanged = false;

        // This signal handler is unblocked in StartHandlingKeyPress(), so
        // we need to block it initially to compensate for this.
        g_signal_handler_block(widget, m_handlerAfterKeyPress);
    }

    void StartHandlingKeyPress(GtkWidget* widget)
    {
        m_inKeyPress = true;
        m_pendingTextChanged = false;

        g_signal_handler_unblock(widget, m_handlerAfterKeyPress);
    }

    bool SetPendingIfInKeyPress()
    {
        if ( !m_inKeyPress )
            return false;

        m_pendingTextChanged = true;

        return true;
    }

    bool EndHandlingKeyPressAndCheckIfPending(GtkWidget* widget)
    {
        g_signal_handler_block(widget, m_handlerAfterKeyPress);

        wxASSERT( m_inKeyPress );
        m_inKeyPress = false;

        if ( !m_pendingTextChanged )
            return false;

        m_pendingTextChanged = false;

        return true;
    }

private:
    bool m_inKeyPress;
    bool m_pendingTextChanged;
    const gulong m_handlerAfterKeyPress;

    wxDECLARE_NO_COPY_CLASS(wxTextCoalesceData);
};

//-----------------------------------------------------------------------------
//  helper function to get the length of the text
//-----------------------------------------------------------------------------

static int GetEntryTextLength(GtkEntry* entry)
{
#if GTK_CHECK_VERSION(2, 14, 0)
    if ( wx_is_at_least_gtk2(14) )
    {
        return gtk_entry_get_text_length(entry);
    }
#endif // GTK+ 2.14+

    return strlen(gtk_entry_get_text(entry));
}

// ============================================================================
// signal handlers implementation
// ============================================================================

extern "C" {

// "event-after" handler is only connected when we get a "key-press-event", so
// it's effectively called after the end of processing of this event and used
// to send a single wxEVT_TEXT even if we received several (typically two, when
// the selected text in the control is replaced by new text) "changed" signals.
static gboolean
wx_gtk_text_after_key_press(GtkWidget* widget,
                            GdkEventKey* WXUNUSED(gdk_event),
                            wxTextEntry* entry)
{
    wxTextCoalesceData* const data = entry->GTKGetCoalesceData();
    wxCHECK_MSG( data, FALSE, "must be non-null if this handler is called" );

    if ( data->EndHandlingKeyPressAndCheckIfPending(widget) )
    {
        entry->GTKOnTextChanged();
    }

    return FALSE;
}

// "changed" handler for GtkEntry
static void
wx_gtk_text_changed_callback(GtkWidget* WXUNUSED(widget), wxTextEntry* entry)
{
    if ( wxTextCoalesceData* const data = entry->GTKGetCoalesceData() )
    {
        if ( data->SetPendingIfInKeyPress() )
        {
            // Don't send the event right now as more might be coming.
            return;
        }
    }

    entry->GTKOnTextChanged();
}

// "insert_text" handler for GtkEntry
static void
wx_gtk_insert_text_callback(GtkEditable *editable,
                            const gchar * new_text,
                            gint new_text_length,
                            gint * position,
                            wxTextEntry *text)
{
    GtkEntry *entry = GTK_ENTRY (editable);

#if GTK_CHECK_VERSION(3,0,0) || defined(GSEAL_ENABLE)
    const int text_max_length = gtk_entry_buffer_get_max_length(gtk_entry_get_buffer(entry));
#else
    const int text_max_length = entry->text_max_length;
#endif

    bool handled = false;

    // check that we don't overflow the max length limit if we have it
    if ( text_max_length )
    {
        const int text_length = GetEntryTextLength(entry);

        // We can't use new_text_length as it is in bytes while we want to count
        // characters (in first approximation, anyhow...).
        if ( text_length + g_utf8_strlen(new_text, -1) > text_max_length )
        {
            // Prevent the new text from being inserted.
            handled = true;

            // Currently we don't insert anything at all, but it would be better to
            // insert as many characters as would fit into the text control and
            // only discard the rest.

            // Notify the user code about overflow.
            text->SendMaxLenEvent();
        }
    }

    // Check if we have to convert all input to upper-case
    if ( !handled && text->GTKIsUpperCase() )
    {
        const wxGtkString upper(g_utf8_strup(new_text, new_text_length));

        // Use the converted text to generate events
        if ( !text->GTKEntryOnInsertText(upper) )
        {
            // Event not handled, so do insert the text: we have to do it
            // ourselves to use the upper-case version of it

            // Prevent recursive call to this handler again
            g_signal_handlers_block_by_func
            (
                editable,
                (gpointer)wx_gtk_insert_text_callback,
                text
            );

            gtk_editable_insert_text(editable, upper, strlen(upper), position);

            g_signal_handlers_unblock_by_func
            (
                editable,
                (gpointer)wx_gtk_insert_text_callback,
                text
            );
        }

        // Don't call the default handler in any case, either the event was
        // handled in the user code or we've already inserted the text.
        handled = true;
    }

    if ( !handled && text->GTKEntryOnInsertText(new_text) )
    {
        // If we already handled the new text insertion, don't do it again.
        handled = true;
    }

    if ( handled )
    {
        // We must update the position to point after the newly inserted text,
        // as expected by GTK+.
        *position = text->GetInsertionPoint();

        g_signal_stop_emission_by_name (editable, "insert_text");
    }
}

// GTK+ does not expose any mechanism that we can really rely on to detect if/when
// the completion popup is shown or hidden. And the sole reliable way (for now) to
// know its state is to connect to the "grab-notify" signal and be notified then
// for its state. this is the best we can do for now than any other alternative.
// (GtkEntryCompletion grabs/ungrabs keyboard and mouse events on popups/popdowns).

static void
wx_gtk_entry_parent_grab_notify (GtkWidget *widget,
                                 gboolean was_grabbed,
                                 wxTextAutoCompleteData *data);

} // extern "C"

//-----------------------------------------------------------------------------
//  clipboard events: "copy-clipboard", "cut-clipboard", "paste-clipboard"
//-----------------------------------------------------------------------------

// common part of the event handlers below
static void
DoHandleClipboardCallback( GtkWidget *widget,
                           wxWindow *win,
                           wxEventType eventType,
                           const gchar* signal_name)
{
    wxClipboardTextEvent event( eventType, win->GetId() );
    event.SetEventObject( win );
    if ( win->HandleWindowEvent( event ) )
    {
        // don't let the default processing to take place if we did something
        // ourselves in the event handler
        g_signal_stop_emission_by_name (widget, signal_name);
    }
}

extern "C"
{

static void
wx_gtk_copy_clipboard_callback( GtkWidget *widget, wxWindow *win )
{
    DoHandleClipboardCallback(
        widget, win, wxEVT_TEXT_COPY, "copy-clipboard" );
}

static void
wx_gtk_cut_clipboard_callback( GtkWidget *widget, wxWindow *win )
{
    DoHandleClipboardCallback(
        widget, win, wxEVT_TEXT_CUT, "cut-clipboard" );
}

static void
wx_gtk_paste_clipboard_callback( GtkWidget *widget, wxWindow *win )
{
    DoHandleClipboardCallback(
        widget, win, wxEVT_TEXT_PASTE, "paste-clipboard" );
}

} // extern "C"

// Base class for wxTextAutoCompleteFixed and wxTextAutoCompleteDynamic below.
class wxTextAutoCompleteData
{
public:
    // This method is only implemented by wxTextAutoCompleteFixed and will just
    // return false for wxTextAutoCompleteDynamic.
    virtual bool ChangeStrings(const wxArrayString& strings) = 0;

    // Conversely, this one is only implemented for wxTextAutoCompleteDynamic
    // and will just return false (without taking ownership of the argument!)
    // for wxTextAutoCompleteFixed.
    virtual bool ChangeCompleter(wxTextCompleter* completer) = 0;

    // We need to turn off wxTE_PROCESS_ENTER flag of our wxTextEntry while
    // the completion popup is shown to let it see Enter event and process it
    // on its own (e.g. to dismiss itself). This is done by "grab-notify" signal
    // see wxTextCtrl::OnChar()
    void GTKOnPopupShown(bool shown)
    {
        wxWindow* const win = GetEditableWindow(m_entry);

        if ( shown )
        {
            // If this is not the first call showing the popup, nothing to do
            // other than updating the count.
            if ( m_popupShownCount++ )
                return;

            // We're showing the popup for the first time, remember if we have
            // wxTE_PROCESS_ENTER flag.
            m_hadProcessEnterFlag = win->HasFlag(wxTE_PROCESS_ENTER);

            // And don't do anything if we don't.
            if ( !m_hadProcessEnterFlag )
                return;
        }
        else
        {
            // If popup is still shown, nothing to do.
            if ( --m_popupShownCount )
                return;

            // Popup was hidden, restore the flag below if we had it.
            if ( !m_hadProcessEnterFlag )
                return;
        }

        win->ToggleWindowStyle(wxTE_PROCESS_ENTER);
    }

    virtual ~wxTextAutoCompleteData()
    {
        // Note that we must not use m_entry here because this could result in
        // using an already half-destroyed wxTextEntry when we're destroyed
        // from its dtor (which is executed after wxTextCtrl dtor, which had
        // already destroyed the actual entry). So use the stored widget
        // instead and only after checking that it is still valid.
        if ( GTK_IS_ENTRY(m_widgetEntry) )
        {
            gtk_entry_set_completion(m_widgetEntry, nullptr);

            g_signal_handlers_disconnect_by_data(m_widgetEntry, this);
        }
    }

protected:
    // Check if completion can be used with this entry.
    static bool CanComplete(wxTextEntry* entry)
    {
        // If this check fails, this is probably a multiline wxTextCtrl which
        // doesn't have any associated GtkEntry.
        return GTK_IS_ENTRY(entry->GetEntry());
    }

    explicit wxTextAutoCompleteData(wxTextEntry* entry)
        : m_entry(entry),
          m_widgetEntry(entry->GetEntry())
    {
        GtkEntryCompletion* const completion = gtk_entry_completion_new();

        gtk_entry_completion_set_text_column (completion, 0);
        gtk_entry_set_completion(m_widgetEntry, completion);

        g_signal_connect (m_widgetEntry, "grab-notify",
                          G_CALLBACK (wx_gtk_entry_parent_grab_notify),
                          this);
    }

    // Provide access to wxTextEntry::GetEditableWindow() to the derived
    // classes: we can call it because this class is a friend of wxTextEntry,
    // but the derived classes can't do it directly.
    static wxWindow* GetEditableWindow(wxTextEntry* entry)
    {
        return entry->GetEditableWindow();
    }

    // Helper function for appending a string to GtkListStore.
    void AppendToStore(GtkListStore* store, const wxString& s)
    {
        GtkTreeIter iter;
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter, 0, (const gchar *)s.utf8_str(), -1);
    }

    // Really change the completion model (which may be null).
    void UseModel(GtkListStore* store)
    {
        GtkEntryCompletion* const c = gtk_entry_get_completion(m_widgetEntry);
        gtk_entry_completion_set_model (c, GTK_TREE_MODEL(store));
        gtk_entry_completion_complete (c);
    }


    // The text entry we're associated with.
    wxTextEntry * const m_entry;

    // And its GTK widget.
    GtkEntry* const m_widgetEntry;

    // Number of times GTKOnPopupShown() was called with true argument minus
    // the number of times it was called with false argument.
    int m_popupShownCount = 0;

    // True if the window had wxTE_PROCESS_ENTER flag before we turned it off
    // in GTKOnPopupShown().
    bool m_hadProcessEnterFlag = false;

    wxDECLARE_NO_COPY_CLASS(wxTextAutoCompleteData);
};

// This class simply forwards to GtkListStore.
class wxTextAutoCompleteFixed : public wxTextAutoCompleteData
{
public:
    // Factory function, may return nullptr if entry is invalid.
    static wxTextAutoCompleteFixed* New(wxTextEntry *entry)
    {
        if ( !CanComplete(entry) )
            return nullptr;

        return new wxTextAutoCompleteFixed(entry);
    }

    virtual bool ChangeStrings(const wxArrayString& strings) override
    {
        wxGtkObject<GtkListStore> store(gtk_list_store_new (1, G_TYPE_STRING));

        for ( wxArrayString::const_iterator i = strings.begin();
              i != strings.end();
              ++i )
        {
            AppendToStore(store, *i);
        }

        UseModel(store);

        return true;
    }

    virtual bool ChangeCompleter(wxTextCompleter*) override
    {
        return false;
    }

private:
    // Ctor is private, use New() to create objects of this type.
    explicit wxTextAutoCompleteFixed(wxTextEntry *entry)
        : wxTextAutoCompleteData(entry)
    {
    }

    wxDECLARE_NO_COPY_CLASS(wxTextAutoCompleteFixed);
};

// Dynamic completion using the provided custom wxTextCompleter.
class wxTextAutoCompleteDynamic : public wxTextAutoCompleteData
{
public:
    static wxTextAutoCompleteDynamic* New(wxTextEntry *entry)
    {
        if ( !CanComplete(entry) )
            return nullptr;

        wxWindow * const win = GetEditableWindow(entry);
        if ( !win )
            return nullptr;

        return new wxTextAutoCompleteDynamic(entry, win);
    }

    virtual ~wxTextAutoCompleteDynamic()
    {
        delete m_completer;

        m_win->Unbind(wxEVT_TEXT, &wxTextAutoCompleteDynamic::OnEntryChanged, this);
    }

    virtual bool ChangeStrings(const wxArrayString&) override
    {
        return false;
    }

    // Takes ownership of the pointer which must be non-null.
    virtual bool ChangeCompleter(wxTextCompleter *completer) override
    {
        delete m_completer;
        m_completer = completer;

        DoUpdateCompletionModel();

        return true;
    }

private:
    // Ctor is private, use New() to create objects of this type.
    explicit wxTextAutoCompleteDynamic(wxTextEntry *entry, wxWindow *win)
        : wxTextAutoCompleteData(entry),
          m_win(win)
    {
        m_completer = nullptr;

        win->Bind(wxEVT_TEXT, &wxTextAutoCompleteDynamic::OnEntryChanged, this);
    }

    void OnEntryChanged(wxCommandEvent& event)
    {
        DoUpdateCompletionModel();

        event.Skip();
    }

    // Recreate the model to contain all completions for the current prefix.
    void DoUpdateCompletionModel()
    {
        const wxString& prefix = m_entry->GetValue();

        if ( m_completer->Start(prefix) )
        {
            wxGtkObject<GtkListStore> store(gtk_list_store_new (1, G_TYPE_STRING));

            for (;;)
            {
                const wxString s = m_completer->GetNext();
                if ( s.empty() )
                    break;

                AppendToStore(store, s);
            }

            UseModel(store);
        }
        else
        {
            UseModel(nullptr);
        }
    }


    // Custom completer.
    wxTextCompleter *m_completer;

    // The associated window, we need to store it to unbind our event handler.
    wxWindow* const m_win;

    wxDECLARE_NO_COPY_CLASS(wxTextAutoCompleteDynamic);
};

extern "C"
{

static void
wx_gtk_entry_parent_grab_notify (GtkWidget *widget,
                                 gboolean was_grabbed,
                                 wxTextAutoCompleteData *data)
{
    g_return_if_fail (GTK_IS_ENTRY(widget));

    bool shown = false;

    if ( gtk_widget_has_focus(widget) )
    {
        // If was_grabbed is FALSE that means the topmost grab widget ancestor
        // of our GtkEntry becomes shadowed by a call to gtk_grab_add()
        // which means that the GtkEntryCompletion popup window is actually
        // shown on screen.

        if ( !was_grabbed )
            shown = true;
    }

    data->GTKOnPopupShown(shown);
}

} // extern "C"

// ============================================================================
// wxTextEntry implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization and destruction
// ----------------------------------------------------------------------------

wxTextEntry::wxTextEntry()
{
    m_autoCompleteData = nullptr;
    m_coalesceData = nullptr;
    m_isUpperCase = false;
}

wxTextEntry::~wxTextEntry()
{
    delete m_coalesceData;
    delete m_autoCompleteData;
}

// ----------------------------------------------------------------------------
// text operations
// ----------------------------------------------------------------------------

void wxTextEntry::WriteText(const wxString& value)
{
    GtkEditable * const edit = GetEditable();

    // remove the selection if there is one and suppress the text change event
    // generated by this: we only want to generate one event for this change,
    // not two
    {
        EventsSuppressor noevents(this);
        gtk_editable_delete_selection(edit);
    }

    // insert new text at the cursor position
    gint len = gtk_editable_get_position(edit);
    gtk_editable_insert_text
    (
        edit,
        value.utf8_str(),
        -1,     // text: length: compute it using strlen()
        &len    // will be updated to position after the text end
    );

    // and move cursor to the end of new text
    gtk_editable_set_position(edit, len);
}

void wxTextEntry::DoSetValue(const wxString& value, int flags)
{
    if (value != DoGetValue())
    {
        // Use Remove() rather than SelectAll() to avoid unnecessary clipboard
        // operations, and prevent triggering an apparent bug in GTK which
        // causes the subsequent WriteText() to append rather than overwrite.
        {
            EventsSuppressor noevents(this);
            Remove(0, -1);
        }

        // Testing whether value is empty here is more than just an
        // optimization: WriteText() always generates an explicit event in
        // wxGTK, which we need to avoid unless SetValue_SendEvent is given.
        if ( !value.empty() )
        {
            // Suppress events from here even if we do need them, it's simpler
            // to send the event below in all cases.
            EventsSuppressor noevents(this);
            WriteText(value);
        }

        // Changing the value is supposed to reset the insertion point. Note,
        // however, that this does not happen if the text doesn't really change.
        SetInsertionPoint(0);
    }

    // OTOH we must send the event even if the text didn't really change for
    // consistency.
    if ( flags & SetValue_SendEvent )
        SendTextUpdatedEvent(GetEditableWindow());
}

wxString wxTextEntry::DoGetValue() const
{
    const wxGtkString value(gtk_editable_get_chars(GetEditable(), 0, -1));

    return wxString::FromUTF8Unchecked(value);
}

void wxTextEntry::Remove(long from, long to)
{
    gtk_editable_delete_text(GetEditable(), from, to);
}

// static
int wxTextEntry::GTKGetEntryTextLength(GtkEntry* entry)
{
    return GetEntryTextLength(entry);
}

// ----------------------------------------------------------------------------
// clipboard operations
// ----------------------------------------------------------------------------

void wxTextEntry::GTKConnectClipboardSignals(GtkWidget* entry)
{
    g_signal_connect(entry, "copy-clipboard",
                     G_CALLBACK (wx_gtk_copy_clipboard_callback),
                     GetEditableWindow());
    g_signal_connect(entry, "cut-clipboard",
                     G_CALLBACK (wx_gtk_cut_clipboard_callback),
                     GetEditableWindow());
    g_signal_connect(entry, "paste-clipboard",
                     G_CALLBACK (wx_gtk_paste_clipboard_callback),
                     GetEditableWindow());
}

void wxTextEntry::Copy()
{
    gtk_editable_copy_clipboard(GetEditable());
}

void wxTextEntry::Cut()
{
    gtk_editable_cut_clipboard(GetEditable());
}

void wxTextEntry::Paste()
{
    gtk_editable_paste_clipboard(GetEditable());
}

// ----------------------------------------------------------------------------
// undo/redo
// ----------------------------------------------------------------------------

void wxTextEntry::Undo()
{
    // TODO: not implemented
}

void wxTextEntry::Redo()
{
    // TODO: not implemented
}

bool wxTextEntry::CanUndo() const
{
    return false;
}

bool wxTextEntry::CanRedo() const
{
    return false;
}

// ----------------------------------------------------------------------------
// insertion point
// ----------------------------------------------------------------------------

void wxTextEntry::SetInsertionPoint(long pos)
{
    gtk_editable_set_position(GetEditable(), pos);
}

long wxTextEntry::GetInsertionPoint() const
{
    return gtk_editable_get_position(GetEditable());
}

long wxTextEntry::GetLastPosition() const
{
    // this can't be implemented for arbitrary GtkEditable so only do it for
    // GtkEntries
    long pos = -1;
    GtkEntry* entry = (GtkEntry*)GetEditable();
    if (GTK_IS_ENTRY(entry))
        pos = GetEntryTextLength(entry);

    return pos;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxTextEntry::SetSelection(long from, long to)
{
    // in wx convention, (-1, -1) means the entire range but GTK+ translates -1
    // (or any negative number for that matter) into last position so we need
    // to translate manually
    if ( from == -1 && to == -1 )
        from = 0;

    // for compatibility with MSW, exchange from and to parameters so that the
    // insertion point is set to the start of the selection and not its end as
    // GTK+ does by default
    gtk_editable_select_region(GetEditable(), to, from);

#ifndef __WXGTK3__
    // avoid reported problem with RHEL 5 GTK+ 2.10 where selection is reset by
    // a clipboard callback, see #13277
    if (!wx_is_at_least_gtk2(12))
    {
        GtkEntry* entry = GTK_ENTRY(GetEditable());
        if (to < 0)
            to = entry->text_length;
        entry->selection_bound = to;
    }
#endif
}

void wxTextEntry::GetSelection(long *from, long *to) const
{
    gint start, end;
    if ( gtk_editable_get_selection_bounds(GetEditable(), &start, &end) )
    {
        // the output must always be in order, although in GTK+ it isn't
        if ( start > end )
        {
            gint tmp = start;
            start = end;
            end = tmp;
        }
    }
    else // no selection
    {
        // for compatibility with MSW return the empty selection at cursor
        start =
        end = GetInsertionPoint();
    }

    if ( from )
        *from = start;

    if ( to )
        *to = end;
}

// ----------------------------------------------------------------------------
// auto completion
// ----------------------------------------------------------------------------

bool wxTextEntry::DoAutoCompleteStrings(const wxArrayString& choices)
{
    // Try to update the existing data first.
    if ( !m_autoCompleteData || !m_autoCompleteData->ChangeStrings(choices) )
    {
        delete m_autoCompleteData;
        m_autoCompleteData = nullptr;

        // If it failed, try creating a new object for fixed completion.
        wxTextAutoCompleteFixed* const ac = wxTextAutoCompleteFixed::New(this);
        if ( !ac )
            return false;

        ac->ChangeStrings(choices);

        m_autoCompleteData = ac;
    }

    return true;
}

bool wxTextEntry::DoAutoCompleteCustom(wxTextCompleter *completer)
{
    // First deal with the case when we just want to disable auto-completion.
    if ( !completer )
    {
        if ( m_autoCompleteData )
        {
            delete m_autoCompleteData;
            m_autoCompleteData = nullptr;
        }
        //else: Nothing to do, we hadn't used auto-completion even before.
    }
    else // Have a valid completer.
    {
        // As above, try to update the completer of the existing object first
        // and fall back on creating a new one.
        if ( !m_autoCompleteData ||
                !m_autoCompleteData->ChangeCompleter(completer) )
        {
            delete m_autoCompleteData;
            m_autoCompleteData = nullptr;

            wxTextAutoCompleteDynamic* const
                ac = wxTextAutoCompleteDynamic::New(this);
            if ( !ac )
                return false;

            ac->ChangeCompleter(completer);

            m_autoCompleteData = ac;
        }
    }

    return true;
}
// ----------------------------------------------------------------------------
// editable status
// ----------------------------------------------------------------------------

bool wxTextEntry::IsEditable() const
{
    return gtk_editable_get_editable(GetEditable()) != 0;
}

void wxTextEntry::SetEditable(bool editable)
{
    gtk_editable_set_editable(GetEditable(), editable);
}

// ----------------------------------------------------------------------------
// input restrictions
// ----------------------------------------------------------------------------

void wxTextEntry::SetMaxLength(unsigned long len)
{
    GtkEntry* const entry = (GtkEntry*)GetEditable();
    if (!GTK_IS_ENTRY(entry))
        return;

    gtk_entry_set_max_length(entry, len);
}

void wxTextEntry::SendMaxLenEvent()
{
    // remember that the next changed signal is to be ignored to avoid
    // generating a dummy wxEVT_TEXT event
    //IgnoreNextTextUpdate();

    wxWindow * const win = GetEditableWindow();
    wxCommandEvent event(wxEVT_TEXT_MAXLEN, win->GetId());
    event.SetEventObject(win);
    event.SetString(GetValue());
    win->HandleWindowEvent(event);
}

void wxTextEntry::ForceUpper()
{
    if ( !m_isUpperCase )
    {
        ConvertToUpperCase();

        m_isUpperCase = true;
    }
}

// ----------------------------------------------------------------------------
// IM handling
// ----------------------------------------------------------------------------

void wxTextEntry::GTKEntryOnKeypress(GtkWidget* widget) const
{
    // We coalesce possibly multiple events resulting from a single key press
    // (this always happens when there is a selection, as we always get a
    // "changed" event when the selection is removed and another one when the
    // new text is inserted) into a single wxEVT_TEXT and to do this we need
    // this extra handler.
    if ( !m_coalesceData )
    {
        // We can't use g_signal_connect_after("key-press-event") because the
        // emission of this signal is stopped by GtkEntry own key-press-event
        // handler, so we have to use the generic "event-after" instead to be
        // notified about the end of handling of this key press and to send any
        // pending events a.s.a.p.
        const gulong handler =  g_signal_connect
                                (
                                    widget,
                                    "event-after",
                                    G_CALLBACK(wx_gtk_text_after_key_press),
                                    const_cast<wxTextEntry*>(this)
                                );

        m_coalesceData = new wxTextCoalesceData(widget, handler);
    }

    m_coalesceData->StartHandlingKeyPress(widget);
}

int wxTextEntry::GTKEntryIMFilterKeypress(GdkEventKey* event) const
{
    GTKEntryOnKeypress(GTK_WIDGET(GetEntry()));

    int result = false;
#if GTK_CHECK_VERSION(2, 22, 0)
    if (wx_is_at_least_gtk2(22))
    {
        result = gtk_entry_im_context_filter_keypress(GetEntry(), event);
    }
#else // GTK+ < 2.22
    wxUnusedVar(event);
#endif // GTK+ 2.22+

    return result;
}

// ----------------------------------------------------------------------------
// signals and events
// ----------------------------------------------------------------------------

void wxTextEntry::EnableTextChangedEvents(bool enable)
{
    // Check that we have the associated text, as it may happen (for e.g.
    // read-only wxBitmapComboBox) and shouldn't result in any errors, we just
    // don't have any events to enable or disable in this case.
    void* const entry = GetTextObject();
    if ( !entry )
        return;

    if ( enable )
    {
        g_signal_handlers_unblock_by_func(entry,
            (gpointer)wx_gtk_text_changed_callback, this);
    }
    else // disable events
    {
        g_signal_handlers_block_by_func(entry,
            (gpointer)wx_gtk_text_changed_callback, this);
    }
}

void wxTextEntry::GTKConnectChangedSignal()
{
    g_signal_connect(GetTextObject(), "changed",
                     G_CALLBACK(wx_gtk_text_changed_callback), this);

}

void wxTextEntry::GTKConnectInsertTextSignal(GtkEntry* entry)
{
    g_signal_connect(entry, "insert_text",
                     G_CALLBACK(wx_gtk_insert_text_callback), this);
}

bool wxTextEntry::GTKEntryOnInsertText(const char* text)
{
    return GetEditableWindow()->GTKOnInsertText(text);
}

// ----------------------------------------------------------------------------
// margins support
// ----------------------------------------------------------------------------

bool wxTextEntry::DoSetMargins(const wxPoint& margins)
{
#if GTK_CHECK_VERSION(2,10,0) && !defined(__WXGTK4__)
    GtkEntry* entry = GetEntry();

    if ( !entry )
        return false;
    if ( !wx_is_at_least_gtk2(10) )
        return false;

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    const GtkBorder* oldBorder = gtk_entry_get_inner_border(entry);
    GtkBorder newBorder;

    if ( oldBorder )
        newBorder = *oldBorder;
    else
    {
        // Use some reasonable defaults for initial margins
        newBorder.left = 2;
        newBorder.right = 2;

        // These numbers seem to let the text remain vertically centered
        // in common use scenarios when margins.y == -1.
        newBorder.top = 3;
        newBorder.bottom = 3;
    }

    if ( margins.x != -1 )
        newBorder.left = margins.x;

    if ( margins.y != -1 )
        newBorder.top = margins.y;

    gtk_entry_set_inner_border(entry, &newBorder);
    wxGCC_WARNING_RESTORE()

    return true;
#else
    wxUnusedVar(margins);
    return false;
#endif
}

wxPoint wxTextEntry::DoGetMargins() const
{
    wxPoint point(-1, -1);
#if GTK_CHECK_VERSION(2,10,0) && !defined(__WXGTK4__)
    GtkEntry* entry = GetEntry();
    if (entry)
    {
        if (wx_is_at_least_gtk2(10))
        {
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            const GtkBorder* border = gtk_entry_get_inner_border(entry);
            wxGCC_WARNING_RESTORE()
            if (border)
            {
                point.x = border->left;
                point.y = border->top;
            }
        }
    }
#endif
    return point;
}

#ifdef __WXGTK3__
bool wxTextEntry::SetHint(const wxString& hint)
{
#if GTK_CHECK_VERSION(3,2,0)
    GtkEntry *entry = GetEntry();
    if (entry && gtk_check_version(3,2,0) == nullptr)
    {
        gtk_entry_set_placeholder_text(entry, hint.utf8_str());
        return true;
    }
#endif
    return wxTextEntryBase::SetHint(hint);
}

wxString wxTextEntry::GetHint() const
{
#if GTK_CHECK_VERSION(3,2,0)
    GtkEntry *entry = GetEntry();
    if (entry && gtk_check_version(3,2,0) == nullptr)
    {
        return wxString::FromUTF8(gtk_entry_get_placeholder_text(entry));
    }
#endif
    return wxTextEntryBase::GetHint();
}
#endif // __WXGTK3__

bool wxTextEntry::ClickDefaultButtonIfPossible()
{
    GtkWidget* const widget = GTK_WIDGET(GetEntry());
    if (widget == nullptr)
        return false;

    // This does the same thing as gtk_entry_real_activate() in GTK itself.
    //
    // Note: in GTK 4 we should probably just use gtk_widget_activate_default().
    GtkWidget* const toplevel = gtk_widget_get_toplevel(widget);
    if ( GTK_IS_WINDOW (toplevel) )
    {
        GtkWindow* const window = GTK_WINDOW(toplevel);

        if ( window )
        {
            GtkWidget* const default_widget = gtk_window_get_default_widget(window);
            GtkWidget* const focus_widget = gtk_window_get_focus(window);

            if ( widget != default_widget &&
                    !(widget == focus_widget &&
                        (!default_widget ||
                            !gtk_widget_get_sensitive(default_widget))) )
            {
                if ( gtk_window_activate_default(window) )
                    return true;
            }
        }
    }

    return false;
}

#endif // wxUSE_TEXTCTRL || wxUSE_COMBOBOX
