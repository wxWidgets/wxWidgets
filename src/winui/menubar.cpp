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
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/msw/private.h"

#include "private.h"

#include <memory>
#include <winrt/Windows.Foundation.h>

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
        m_barHeight = frame->FromDIP(40);

        if ( !wxControl::Create(frame, wxID_ANY, wxPoint(0, 0),
                                wxSize(wxDefaultCoord, m_barHeight),
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
        //
        // The height must be our fixed bar height, never GetSize().y: when the
        // menu bar is a frame's only child (e.g. minimal.exe), the frame's
        // single-child auto-layout (wxTopLevelWindowBase::Layout) transiently
        // resizes us to fill the whole client area before m_winuiMenuBarWin is
        // recognised as a bar.  Reading GetSize().y back then would make us
        // cover the entire window.
        HWND hwnd = static_cast<HWND>(GetHWND());
        if ( !hwnd )
            return;

        const int width = m_frame->GetClientSize().x;
        ::SetWindowPos(hwnd, nullptr, 0, 0, width, m_barHeight,
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

    // --- dynamic status-bar help, mirroring the classic Win32 behaviour ------

    void SendMenuEvent(wxEventType type, int id)
    {
        if ( !m_frame )
            return;
        wxMenuEvent event(type, id);
        event.SetEventObject(m_frame);
        m_frame->GetEventHandler()->ProcessEvent(event);
    }

    // Show the help string of the highlighted item in the status bar (the frame
    // looks it up from the id); wxID_NONE clears it (e.g. on a submenu title).
    void HighlightItem(int id) { SendMenuEvent(wxEVT_MENU_HIGHLIGHT, id); }

    // Track how many flyout items are currently realised so we can detect when
    // any menu is open (text is cleared, saving the previous status text) and
    // when everything is closed again (the previous status text is restored).
    void NoteItemLoaded()
    {
        if ( m_openItems++ == 0 )
        {
            SendMenuEvent(wxEVT_MENU_OPEN, wxID_ANY);
            HighlightItem(wxID_NONE);
        }
    }

    void NoteItemUnloaded()
    {
        if ( m_openItems > 0 && --m_openItems == 0 )
            SendMenuEvent(wxEVT_MENU_CLOSE, wxID_ANY);
    }

    // Wire the help/open/close behaviour onto a flyout item (id is wxID_NONE for
    // submenu titles, which have no command help).
    void AttachItemHelp(MUXC::MenuFlyoutItemBase const& item, int id)
    {
        item.PointerEntered(
            [this, id](winrt::Windows::Foundation::IInspectable const&,
                       winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&)
            {
                HighlightItem(id);
            });
        item.Loaded(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                NoteItemLoaded();
            });
        item.Unloaded(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                NoteItemUnloaded();
            });
    }

    void Populate(
        winrt::Windows::Foundation::Collections::IVector<MUXC::MenuFlyoutItemBase> const& items,
        wxMenu *menu)
    {
        const size_t count = menu->GetMenuItemCount();

        // wx groups *consecutive* radio items together; mirror that with a new
        // WinUI radio group name each time a run of radio items starts so that
        // mutually-exclusive radio items are shown (and behave) distinctly from
        // independently-checkable items.
        int radioGroup = 0;
        bool inRadioRun = false;

        for ( size_t i = 0; i < count; ++i )
        {
            wxMenuItem *item = menu->FindItemByPosition(i);
            if ( !item )
                continue;

            if ( item->IsSeparator() )
            {
                inRadioRun = false;
                items.Append(MUXC::MenuFlyoutSeparator());
                continue;
            }

            if ( item->IsSubMenu() )
            {
                inRadioRun = false;
                MUXC::MenuFlyoutSubItem sub;
                sub.Text(wxWinUIToHString(item->GetItemLabelText()));
                Populate(sub.Items(), item->GetSubMenu());
                AttachItemHelp(sub, wxID_NONE);
                items.Append(sub);
                continue;
            }

            const int id = item->GetId();

            if ( item->GetKind() == wxITEM_RADIO )
            {
                if ( !inRadioRun )
                {
                    ++radioGroup;
                    inRadioRun = true;
                }

                MUXC::RadioMenuFlyoutItem radio;
                radio.Text(wxWinUIToHString(item->GetItemLabelText()));
                radio.GroupName(wxWinUIToHString(
                    wxString::Format("wxRadioGroup_%p_%d", menu, radioGroup)));
                radio.IsEnabled(item->IsEnabled());
                radio.IsChecked(item->IsChecked());
                radio.Click(
                    [this, id](winrt::Windows::Foundation::IInspectable const& sender,
                               winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        bool checked = true;
                        if ( auto r = sender.try_as<MUXC::RadioMenuFlyoutItem>() )
                            checked = r.IsChecked();
                        FireMenu(id, true, checked);
                    });
                // A RadioMenuFlyoutItem does not render its initial selection
                // dot when IsChecked is set before the item is realised (it is
                // only realised when its submenu is first opened).  Re-apply the
                // current wx state from its Loaded event so the default choice
                // shows its dot without needing a manual click.
                radio.Loaded(
                    [this, id](winrt::Windows::Foundation::IInspectable const& sender,
                               winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        auto r = sender.try_as<MUXC::RadioMenuFlyoutItem>();
                        wxMenuItem *mi = m_menubar ? m_menubar->FindItem(id) : nullptr;
                        if ( r && mi )
                            r.IsChecked(mi->IsChecked());
                    });
                AttachItemHelp(radio, id);
                items.Append(radio);
                continue;
            }

            inRadioRun = false;

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
                AttachItemHelp(toggle, id);
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
                AttachItemHelp(entry, id);
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
    int m_barHeight = 0;
    int m_openItems = 0;   // realised flyout items, for menu open/close tracking
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

// ----------------------------------------------------------------------------
// wxWinUIPopupMenu: show a wxMenu as a WinUI MenuFlyout
// ----------------------------------------------------------------------------

namespace
{

// The item picked by the user, recorded from the Click handlers and acted
// upon only after the flyout has closed (mirroring TrackPopupMenu semantics:
// the command is processed once the menu is gone).
struct wxWinUIPopupSelection
{
    int id = wxID_NONE;
    bool checkable = false;
    bool checked = false;
};

void wxWinUIPopulatePopupItems(
    winrt::Windows::Foundation::Collections::IVector<MUXC::MenuFlyoutItemBase> const& items,
    wxMenu *menu,
    std::shared_ptr<wxWinUIPopupSelection> const& sel)
{
    const size_t count = menu->GetMenuItemCount();

    // Same consecutive-run grouping as in wxWinUIMenuBarWindow::Populate().
    int radioGroup = 0;
    bool inRadioRun = false;

    for ( size_t i = 0; i < count; ++i )
    {
        wxMenuItem *item = menu->FindItemByPosition(i);
        if ( !item )
            continue;

        if ( item->IsSeparator() )
        {
            inRadioRun = false;
            items.Append(MUXC::MenuFlyoutSeparator());
            continue;
        }

        if ( item->IsSubMenu() )
        {
            inRadioRun = false;
            MUXC::MenuFlyoutSubItem sub;
            sub.Text(wxWinUIToHString(item->GetItemLabelText()));
            sub.IsEnabled(item->IsEnabled());
            wxWinUIPopulatePopupItems(sub.Items(), item->GetSubMenu(), sel);
            items.Append(sub);
            continue;
        }

        const int id = item->GetId();

        if ( item->GetKind() == wxITEM_RADIO )
        {
            if ( !inRadioRun )
            {
                ++radioGroup;
                inRadioRun = true;
            }

            MUXC::RadioMenuFlyoutItem radio;
            radio.Text(wxWinUIToHString(item->GetItemLabelText()));
            radio.GroupName(wxWinUIToHString(
                wxString::Format("wxPopupRadioGroup_%p_%d", menu, radioGroup)));
            radio.IsEnabled(item->IsEnabled());
            radio.IsChecked(item->IsChecked());
            radio.Click(
                [sel, id](winrt::Windows::Foundation::IInspectable const& sender,
                          winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    sel->id = id;
                    sel->checkable = true;
                    sel->checked = true;
                    if ( auto r = sender.try_as<MUXC::RadioMenuFlyoutItem>() )
                        sel->checked = r.IsChecked();
                });
            // Re-apply the checked state once realised (see the identical
            // workaround in wxWinUIMenuBarWindow::Populate()).
            radio.Loaded(
                [menu, id](winrt::Windows::Foundation::IInspectable const& sender,
                           winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    auto r = sender.try_as<MUXC::RadioMenuFlyoutItem>();
                    wxMenuItem *mi = menu->FindItem(id);
                    if ( r && mi )
                        r.IsChecked(mi->IsChecked());
                });
            items.Append(radio);
            continue;
        }

        inRadioRun = false;

        if ( item->IsCheckable() )
        {
            MUXC::ToggleMenuFlyoutItem toggle;
            toggle.Text(wxWinUIToHString(item->GetItemLabelText()));
            toggle.IsChecked(item->IsChecked());
            toggle.IsEnabled(item->IsEnabled());
            toggle.Click(
                [sel, id](winrt::Windows::Foundation::IInspectable const& sender,
                          winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    sel->id = id;
                    sel->checkable = true;
                    if ( auto t = sender.try_as<MUXC::ToggleMenuFlyoutItem>() )
                        sel->checked = t.IsChecked();
                });
            items.Append(toggle);
        }
        else
        {
            MUXC::MenuFlyoutItem entry;
            entry.Text(wxWinUIToHString(item->GetItemLabelText()));
            entry.IsEnabled(item->IsEnabled());
            entry.Click(
                [sel, id](winrt::Windows::Foundation::IInspectable const&,
                          winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    sel->id = id;
                });
            items.Append(entry);
        }
    }
}

} // anonymous namespace

// Called from wxWindowMSW::DoPopupMenu(); returns false to fall back to the
// classic Win32 TrackPopupMenu().
bool wxWinUIPopupMenu(wxWindow *win, wxMenu *menu, int x, int y)
{
    if ( !win || !menu || !wxWinUI3Initialize() )
        return false;

    HWND hwnd = GetHwndOf(win);
    if ( !hwnd )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI;
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;
        using namespace winrt::Windows::Foundation;

        DesktopWindowXamlSource source;
        source.Initialize(GetWindowIdFromWindow(hwnd));
        source.SiteBridge().ResizePolicy(ContentSizePolicy::ResizeContentToParentWindow);

        const HWND hwndBridge = GetWindowFromWindowId(source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            hwndBridge,
            GWL_STYLE,
            ::GetWindowLongPtr(hwndBridge, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
        );
        ::SetWindowPos(hwndBridge, HWND_TOP, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

        Grid root;
        root.RequestedTheme(wxWinUIGetCurrentElementTheme());
        source.Content(root);

        if ( !root.XamlRoot() )
        {
            source.Close();
            return false;
        }

        auto sel = std::make_shared<wxWinUIPopupSelection>();

        MUXC::MenuFlyout flyout;
        // The island is only as large as the window: let the flyout become a
        // windowed popup instead of being clipped to the island bounds.
        flyout.ShouldConstrainToRootBounds(false);
        wxWinUIPopulatePopupItems(flyout.Items(), menu, sel);

        wxPoint pos;
        if ( x == wxDefaultCoord && y == wxDefaultCoord )
            pos = win->ScreenToClient(wxGetMousePosition());
        else
            pos = wxPoint(x, y);
        const wxPoint dip = win->ToDIP(pos);

        bool done = false;
        bool loopIsRunning = false;
        wxEventLoop* loopRunning = nullptr;

        flyout.Closed(
            [&](IInspectable const&, IInspectable const&)
            {
                done = true;
                if ( loopRunning && loopIsRunning )
                    loopRunning->Exit();
            });

        {
            wxMenuEvent event(wxEVT_MENU_OPEN, wxID_ANY, menu);
            event.SetEventObject(menu);
            win->GetEventHandler()->ProcessEvent(event);
        }

        flyout.ShowAt(root, Point{ static_cast<float>(dip.x),
                                   static_cast<float>(dip.y) });

        wxEventLoop loop;
        loopRunning = &loop;
        if ( !done )
        {
            loopIsRunning = true;
            loop.Run();
            loopIsRunning = false;
        }
        loopRunning = nullptr;

        // The item Click callback may still be queued behind the Closed
        // notification: run the dispatcher for one more turn so any pending
        // click is delivered before we act on the selection.
        if ( auto queue = root.DispatcherQueue() )
        {
            bool drained = false;
            bool drainIsRunning = false;
            wxEventLoop drainLoop;
            wxEventLoop* drainRunning = &drainLoop;
            queue.TryEnqueue([&]()
            {
                drained = true;
                if ( drainRunning && drainIsRunning )
                    drainRunning->Exit();
            });
            if ( !drained )
            {
                drainIsRunning = true;
                drainLoop.Run();
                drainIsRunning = false;
            }
        }

        {
            wxMenuEvent event(wxEVT_MENU_CLOSE, wxID_ANY, menu);
            event.SetEventObject(menu);
            win->GetEventHandler()->ProcessEvent(event);
        }

        source.Close();

        if ( sel->id != wxID_NONE )
        {
            wxMenuItem * const item = menu->FindItem(sel->id);
            if ( item && sel->checkable )
                item->Check(sel->checked);

            // Process the command immediately (the menu may be a local
            // variable in the caller), routing it through the menu so that
            // handlers bound to the menu itself and to the invoking window
            // both work.
            wxMenuInvokingWindowSetter setInvokingWin(*menu, win);
            wxCommandEvent event(wxEVT_MENU, sel->id);
            event.SetEventObject(menu);
            if ( sel->checkable )
                event.SetInt(sel->checked ? 1 : 0);
            menu->ProcessEvent(event);
        }

        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI popup menu", e);
    }

    return false;
}

#endif // wxUSE_WINUI3 && wxUSE_MENUS
