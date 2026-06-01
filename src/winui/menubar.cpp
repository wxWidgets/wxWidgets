/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/menubar.cpp
// Purpose:     wxWinUI frame menu bar (WinUI MenuBar hosted at the frame top)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3 && wxUSE_MENUS

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/frame.h"
    #include "wx/event.h"
#endif

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

// ----------------------------------------------------------------------------
// wxWinUIMenuBarWindow: a thin wxControl hosting a WinUI MenuBar island across
// the top of its parent frame.
// ----------------------------------------------------------------------------

class wxWinUIMenuBarWindow : public wxControl
{
public:
    bool Create(wxWindow *frame, wxMenuBar *menubar)
    {
        m_frame = frame;
        m_menubar = menubar;

        if ( !wxControl::Create(frame, wxID_ANY, wxPoint(0, 0),
                                wxSize(wxDefaultCoord, frame->FromDIP(40)),
                                wxBORDER_NONE) )
            return false;

        if ( !m_host.Initialize(this) )
            return false;

        Build();

        m_frame->Bind(wxEVT_SIZE, &wxWinUIMenuBarWindow::OnFrameSize, this);
        Reposition();
        return true;
    }

    ~wxWinUIMenuBarWindow() override
    {
        if ( m_frame )
            m_frame->Unbind(wxEVT_SIZE, &wxWinUIMenuBarWindow::OnFrameSize, this);
    }

    // Rebuild the WinUI MenuBar contents from the (possibly changed) wxMenuBar.
    void Rebuild() { Build(); }

private:
    void OnFrameSize(wxSizeEvent& event)
    {
        Reposition();
        event.Skip();
    }

    void Reposition()
    {
        if ( !m_frame )
            return;

        // Position the bar at the *native* top-left of the frame client area.
        // wx SetSize() can't be used: child coordinates are offset by
        // GetClientAreaOrigin() (which already includes our own height), so it
        // would push us down by our height.  The frame reserves our height in
        // GetClientAreaOrigin() so the real content sits below us.
        HWND hwnd = static_cast<HWND>(GetHWND());
        if ( !hwnd )
            return;

        const int width = m_frame->GetClientSize().x;
        const int height = GetSize().y;
        ::SetWindowPos(hwnd, nullptr, 0, 0, width, height,
                       SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void FireMenu(int id, bool checkable, bool checked)
    {
        if ( checkable )
        {
            if ( wxMenuItem *item = m_menubar->FindItem(id) )
                item->Check(checked);
        }

        wxCommandEvent event(wxEVT_MENU, id);
        event.SetEventObject(m_frame);
        if ( checkable )
            event.SetInt(checked ? 1 : 0);
        m_frame->GetEventHandler()->ProcessEvent(event);
    }

    void Populate(
        winrt::Windows::Foundation::Collections::IVector<MUXC::MenuFlyoutItemBase> const& items,
        wxMenu *menu)
    {
        const size_t count = menu->GetMenuItemCount();
        for ( size_t i = 0; i < count; ++i )
        {
            wxMenuItem *item = menu->FindItemByPosition(i);
            if ( !item )
                continue;

            if ( item->IsSeparator() )
            {
                items.Append(MUXC::MenuFlyoutSeparator());
                continue;
            }

            if ( item->IsSubMenu() )
            {
                MUXC::MenuFlyoutSubItem sub;
                sub.Text(wxWinUIToHString(item->GetItemLabelText()));
                Populate(sub.Items(), item->GetSubMenu());
                items.Append(sub);
                continue;
            }

            const int id = item->GetId();

            if ( item->IsCheckable() )
            {
                MUXC::ToggleMenuFlyoutItem toggle;
                toggle.Text(wxWinUIToHString(item->GetItemLabelText()));
                toggle.IsChecked(item->IsChecked());
                toggle.IsEnabled(item->IsEnabled());
                toggle.Click(
                    [this, id](winrt::Windows::Foundation::IInspectable const& sender,
                               winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        bool checked = false;
                        if ( auto t = sender.try_as<MUXC::ToggleMenuFlyoutItem>() )
                            checked = t.IsChecked();
                        FireMenu(id, true, checked);
                    });
                items.Append(toggle);
            }
            else
            {
                MUXC::MenuFlyoutItem entry;
                entry.Text(wxWinUIToHString(item->GetItemLabelText()));
                entry.IsEnabled(item->IsEnabled());
                entry.Click(
                    [this, id](winrt::Windows::Foundation::IInspectable const&,
                               winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        FireMenu(id, false, false);
                    });
                items.Append(entry);
            }
        }
    }

    void Build()
    {
        if ( !m_menubar )
            return;

        try
        {
            MUXC::MenuBar bar;
            const size_t count = m_menubar->GetMenuCount();
            for ( size_t i = 0; i < count; ++i )
            {
                MUXC::MenuBarItem barItem;
                barItem.Title(wxWinUIToHString(m_menubar->GetMenuLabelText(i)));
                Populate(barItem.Items(), m_menubar->GetMenu(i));
                bar.Items().Append(barItem);
            }

            m_host.SetContent(bar);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI MenuBar creation", e);
        }
    }

    wxWinUIControlHost m_host;
    wxWindow *m_frame = nullptr;
    wxMenuBar *m_menubar = nullptr;
};

// ----------------------------------------------------------------------------
// public entry point used by wxFrame
// ----------------------------------------------------------------------------

wxWindow* wxWinUIAttachFrameMenuBar(wxWindow *frame, wxMenuBar *menubar,
                                    wxWindow *existing)
{
    if ( existing )
        existing->Destroy();

    if ( !frame || !menubar )
        return nullptr;

    wxWinUIMenuBarWindow *win = new wxWinUIMenuBarWindow();
    if ( !win->Create(frame, menubar) )
    {
        win->Destroy();
        return nullptr;
    }

    return win;
}

void wxWinUIRefreshFrameMenuBar(wxWindow *menuBarWin)
{
    // menuBarWin is always a window previously returned by
    // wxWinUIAttachFrameMenuBar(), so this cast is safe.
    if ( menuBarWin )
        static_cast<wxWinUIMenuBarWindow*>(menuBarWin)->Rebuild();
}

#endif // wxUSE_WINUI3 && wxUSE_MENUS
