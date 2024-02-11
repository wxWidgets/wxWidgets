/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/srchctrl.cpp
// Purpose:     wxSearchCtrl implementation - native
// Author:      Kettab Ali
// Created:     2019-12-23
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


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
    #define wxHAS_GTK_SEARCH_ENTRY
#endif // GTK >= 3.6

namespace // anonymous
{

// A more readable way to check for GtkSearchEntry availability.
inline bool HasGtkSearchEntry()
{
#ifdef wxHAS_GTK_SEARCH_ENTRY
    return wx_is_at_least_gtk3(6);
#else
    return false;
#endif
}

inline GtkWidget* CreateGtkSearchEntryIfAvailable()
{
#ifdef wxHAS_GTK_SEARCH_ENTRY
    if ( wx_is_at_least_gtk3(6) )
    {
        return gtk_search_entry_new();
    }
#endif // wxHAS_GTK_SEARCH_ENTRY

    // No GtkSearchEntry! fallback to the plain GtkEntry.
    return gtk_entry_new();
}

}

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
        if ( !HasGtkSearchEntry() )
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
wxBEGIN_EVENT_TABLE(wxSearchCtrl, wxControl)
    EVT_CHAR(wxSearchCtrl::OnChar)
    EVT_TEXT(wxID_ANY, wxSearchCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, wxSearchCtrl::OnTextEnter)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxControl);

// ----------------------------------------------------------------------------
//  creation/destruction
// ----------------------------------------------------------------------------

// destruction
// -----------
wxSearchCtrl::~wxSearchCtrl()
{
#if wxUSE_MENUS
    delete m_menu;
#endif // wxUSE_MENUS
}

// creation
// --------

void wxSearchCtrl::Init()
{
    m_entry = nullptr;

#if wxUSE_MENUS
    m_menu = nullptr;
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
         !CreateBase(parent, id, pos, size, style | wxTE_PROCESS_ENTER,
                     validator, name) )
    {
        wxFAIL_MSG( "wxSearchCtrl creation failed" );
        return false;
    }

    GTKCreateSearchEntryWidget();

    if ( HasFlag(wxBORDER_NONE) )
    {
        g_object_set (m_widget, "has-frame", FALSE, nullptr);
    }

    GtkEntry * const entry = GetEntry();

    // Theoretically m_entry cannot be null, and the test here
    // is just for safety reasons.
    if ( !entry )
        return false;

    // Set it up to trigger default item on enter key press
    gtk_entry_set_activates_default(entry, !HasFlag(wxTE_PROCESS_ENTER));

    gtk_editable_set_editable(GTK_EDITABLE(entry), true);
#ifdef __WXGTK3__
    gtk_entry_set_width_chars(entry, 1);
#endif

    m_parent->DoAddChild(this);

    m_focusWidget = GTK_WIDGET(entry);

    PostCreation(size);

    gtk_entry_set_text(entry, value.utf8_str());

    SetHint(_("Search"));

    GTKConnectChangedSignal();
    GTKConnectInsertTextSignal(entry);
    GTKConnectClipboardSignals(GTK_WIDGET(entry));

    return true;
}

void wxSearchCtrl::GTKCreateSearchEntryWidget()
{
    m_widget = CreateGtkSearchEntryIfAvailable();

    g_object_ref(m_widget);

    m_entry = GTK_ENTRY(m_widget);

    if ( !HasGtkSearchEntry() )
    {
        // Add the search icon and make it looks as native as one would expect
        // (i.e. GtkSearchEntry).
        gtk_entry_set_icon_from_icon_name(m_entry,
                                          GTK_ENTRY_ICON_PRIMARY,
                                          "edit-find-symbolic");

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

    const bool hasMenu = m_menu != nullptr;

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
    // Search button is always shown in the native control.
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    // Search button is always shown in the native control.
    return true;
}

void wxSearchCtrl::ShowCancelButton(bool show)
{
    // The cancel button is shown/hidden automatically by the GtkSearchEntry.
    if ( HasGtkSearchEntry() )
        return;

    if ( show == IsCancelButtonVisible() )
    {
        // no change
        return;
    }

    gtk_entry_set_icon_from_icon_name(m_entry,
                                      GTK_ENTRY_ICON_SECONDARY,
                                      show ? "edit-clear-symbolic" : nullptr);

    m_cancelButtonVisible = show;
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
    if ( HasGtkSearchEntry() )
    {
        return !IsEmpty();
    }

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
    wxCHECK_RET( m_entry != nullptr, "invalid search ctrl" );

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

GdkWindow* wxSearchCtrl::GTKGetWindow(wxArrayGdkWindows& windows) const
{
#ifdef __WXGTK3__
    GTKFindWindow(m_widget, windows);
    return nullptr;
#else
    wxUnusedVar(windows);
    return gtk_entry_get_text_window(GTK_ENTRY(m_widget));
#endif
}
#endif // wxUSE_SEARCHCTRL
