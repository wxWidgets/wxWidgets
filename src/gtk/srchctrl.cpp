/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/srchctrl.cpp
// Purpose:     wxSearchCtrl implementation - native
// Author:      Kettab Ali
// Created:     2019-12-23
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif //WX_PRECOMP

#include "wx/utils.h"
#include "wx/gtk/private.h"
#include "wx/gtk/private/gtk3-compat.h"


#if GTK_CHECK_VERSION(3,6,0)
    // GtkSearchEntry is available only for GTK+ >= 3.6
    #define wxHAS_GTK_SEARCH_ENTRY 1
#else // GTK < 3.6
    #define wxHAS_GTK_SEARCH_ENTRY 0
#endif // GTK >= 3.6

// ============================================================================
// signal handlers implementation
// ============================================================================

extern "C" {

static void
wx_gtk_icon_press(GtkEntry* WXUNUSED(entry),
                  gint position,
                  GdkEventButton* WXUNUSED(event),
                  wxSearchCtrl* ctrl)
{
    if ( position == GTK_ENTRY_ICON_PRIMARY )
    {
        // 1- Notice that contrary to the generic version, we don't generate the
        //    wxEVT_SEARCH event here, which is the native behaviour of the
        //    GtkSearchEntry (the search icon is inactive for a GtkSearchEntry).
        //
        // 2- If the wxSearchCtrl has a menu associated with it, we explicitly
        //    make the search icon clickable as a way to display the menu.

        ctrl->PopupSearchMenu();
    }
    else // position == GTK_ENTRY_ICON_SECONDARY
    {
        if ( !wx_is_at_least_gtk3(6) )
        {
            // No need to call this for a GtkSearchEntry.
            ctrl->Clear();
        }

        wxCommandEvent event(wxEVT_SEARCH_CANCEL, ctrl->GetId());
        event.SetEventObject(ctrl);
        ctrl->HandleWindowEvent(event);
    }
}

}

// ============================================================================
// wxSearchCtrl implementation
// ============================================================================
wxBEGIN_EVENT_TABLE(wxSearchCtrl, wxSearchCtrlBase)
    EVT_CHAR(wxSearchCtrl::OnChar)
    EVT_TEXT(wxID_ANY, wxSearchCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, wxSearchCtrl::OnTextEnter)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase);

// ----------------------------------------------------------------------------
//  creation/destruction
// ----------------------------------------------------------------------------

// destruction
// -----------
wxSearchCtrl::~wxSearchCtrl()
{
    if ( m_entry )
    {
        GTKDisconnect(m_entry);
        g_object_remove_weak_pointer(G_OBJECT(m_entry), (void**)&m_entry);
    }

#if wxUSE_MENUS
    delete m_menu;
#endif // wxUSE_MENUS
}

// creation
// --------

void wxSearchCtrl::Init()
{
    m_entry = NULL;

#if wxUSE_MENUS
    m_menu = NULL;
#endif // wxUSE_MENUS

    m_cancelButtonVisible = false;
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !PreCreation(parent, pos, size) ||
         !CreateBase(parent, id, pos, size, (style | wxTE_PROCESS_ENTER),
                     validator, name) )
    {
        wxFAIL_MSG( "wxSearchCtrl creation failed" );
        return false;
    }

    GTKCreateSearchEntryWidget();

    if ( HasFlag(wxBORDER_NONE) )
    {
        g_object_set (m_widget, "has-frame", FALSE, NULL);
    }

    GtkEntry * const entry = GetEntry();

    if ( entry )
    {
        // Set it up to trigger default item on enter key press
        gtk_entry_set_activates_default(entry, !HasFlag(wxTE_PROCESS_ENTER));

        gtk_editable_set_editable(GTK_EDITABLE(entry), true);
#ifdef __WXGTK3__
        gtk_entry_set_width_chars(entry, 1);
#endif
    }

    m_parent->DoAddChild(this);

    if ( entry )
        m_focusWidget = GTK_WIDGET(entry);

    PostCreation(size);

    if ( entry )
    {
        gtk_entry_set_text(entry, wxGTK_CONV(value));

        SetHint(_("Search"));

        GTKConnectChangedSignal();
        GTKConnectInsertTextSignal(entry);
        GTKConnectClipboardSignals(GTK_WIDGET(entry));
    }

    return true;
}

void wxSearchCtrl::GTKCreateSearchEntryWidget()
{
#if wxHAS_GTK_SEARCH_ENTRY
    if ( wx_is_at_least_gtk3(6) )
    {
        m_widget = gtk_search_entry_new();
    }
#endif // wxHAS_GTK_SEARCH_ENTRY

    if ( !m_widget )
    {
        // No GtkSearchEntry! fallback to the plain GtkEntry.
        m_widget = gtk_entry_new();
    }

    g_object_ref(m_widget);

    m_entry = GTK_ENTRY(m_widget);
    g_object_add_weak_pointer(G_OBJECT(m_entry), (void**)&m_entry);

    if ( !wx_is_at_least_gtk3(6) )
    {
        gtk_entry_set_icon_from_icon_name(m_entry, 
                                          GTK_ENTRY_ICON_PRIMARY,
                                          "edit-find-symbolic");

        // Mimic the behaviour of the native GtkSearchEntry.
        gtk_entry_set_icon_sensitive(m_entry, GTK_ENTRY_ICON_PRIMARY, FALSE);
        gtk_entry_set_icon_activatable(m_entry, GTK_ENTRY_ICON_PRIMARY, FALSE);
    }

    g_signal_connect(m_entry, "icon-press", G_CALLBACK(wx_gtk_icon_press), this);
}

GtkEditable *wxSearchCtrl::GetEditable() const
{
    return GTK_EDITABLE(m_entry);
}

void wxSearchCtrl::Clear()
{
    wxTextEntry::Clear();
    ShowCancelButton(false);
}

bool wxSearchCtrl::IsModified() const
{
    return m_modified;
}

void wxSearchCtrl::MarkDirty()
{
    m_modified = true;
}

void wxSearchCtrl::DiscardEdits()
{
    m_modified = false;
}

bool wxSearchCtrl::PositionToXY(long pos, long *x, long *y ) const
{
    if (pos <= GTKGetEntryTextLength(GetEntry()))
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

    return true;
}

long wxSearchCtrl::XYToPosition(long x, long y ) const
{
    if ( y != 0 || x > GTKGetEntryTextLength(GetEntry()) )
        return -1;

    return x;
}

int wxSearchCtrl::GetLineLength(long lineNo) const
{
    const wxString str = GetLineText(lineNo);
    return (int) str.length();
}

int wxSearchCtrl::GetNumberOfLines() const 
{
    return 1;
}

wxString wxSearchCtrl::GetLineText( long lineNo ) const
{
    if ( lineNo == 0 )
        return GetValue();

    return wxString();
}

void wxSearchCtrl::ShowPosition( long pos )
{
    gtk_editable_set_position(GetEditable(), pos);
}

// search control specific interfaces
// ----------------------------------
#if wxUSE_MENUS

void wxSearchCtrl::SetMenu( wxMenu* menu )
{
    if ( menu == m_menu )
    {
        // no change
        return;
    }

    delete m_menu;
    m_menu = menu;

    const bool hasMenu = m_menu != NULL;

    gtk_entry_set_icon_sensitive(m_entry, GTK_ENTRY_ICON_PRIMARY, hasMenu);
    gtk_entry_set_icon_activatable(m_entry, GTK_ENTRY_ICON_PRIMARY, hasMenu);
}

wxMenu* wxSearchCtrl::GetMenu()
{
    return m_menu;
}

#endif // wxUSE_MENUS

void wxSearchCtrl::ShowSearchButton(bool WXUNUSED(show))
{
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    return HasMenu();
}

void wxSearchCtrl::ShowCancelButton(bool show)
{
#if wxHAS_GTK_SEARCH_ENTRY
    // The cancel button is shown/hidden automatically by the GtkSearchEntry.
    if ( wx_is_at_least_gtk3(6) )
        return;
#endif // wxHAS_GTK_SEARCH_ENTRY

    if ( show == IsCancelButtonVisible() )
    {
        // no change
        return;
    }

    if ( show && !m_cancelButtonVisible )
    {
        gtk_entry_set_icon_from_icon_name(m_entry, 
                                          GTK_ENTRY_ICON_SECONDARY,
                                          "edit-clear-symbolic");
    }
    else // !show && m_cancelButtonVisible
    {
        gtk_entry_set_icon_from_icon_name(m_entry, GTK_ENTRY_ICON_SECONDARY, NULL);
    }

    m_cancelButtonVisible = show;
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
#if wxHAS_GTK_SEARCH_ENTRY
    return !IsEmpty();
#endif // wxHAS_GTK_SEARCH_ENTRY

    return m_cancelButtonVisible;
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    wxTextEntry::SetHint(text);
}

wxString wxSearchCtrl::GetDescriptiveText() const
{
    return wxTextEntry::GetHint();
}

// Events
// ----------

void wxSearchCtrl::OnChar(wxKeyEvent& key_event)
{
    wxCHECK_RET( m_entry != NULL, "invalid search ctrl" );

    if ( key_event.GetKeyCode() == WXK_RETURN )
    {
        if ( HasFlag(wxTE_PROCESS_ENTER) )
        {
            wxCommandEvent event(wxEVT_TEXT_ENTER, m_windowId);
            event.SetEventObject(this);
            event.SetString(GetValue());
            if ( HandleWindowEvent(event) )
                return;

            // We disable built-in default button activation when
            // wxTE_PROCESS_ENTER is used, but we still should activate it
            // if the event wasn't handled, so do it from here.
            if ( ClickDefaultButtonIfPossible() )
                return;
        }
    }

    key_event.Skip();
}

void wxSearchCtrl::OnText(wxCommandEvent& event)
{
    ShowCancelButton(!IsEmpty());
    event.Skip();
}

void wxSearchCtrl::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
    if ( !IsEmpty() )
    {
        wxCommandEvent evt(wxEVT_SEARCH, GetId());
        evt.SetEventObject(this);
        evt.SetString(GetValue());

        ProcessWindowEvent(evt);
    }
}

#if wxUSE_MENUS

void wxSearchCtrl::PopupSearchMenu()
{
    if ( m_menu )
    {
        const wxSize size = GetSize();
        PopupMenu(m_menu, 0, size.y);
    }
}

#endif // wxUSE_MENUS

#endif // wxUSE_SEARCHCTRL
