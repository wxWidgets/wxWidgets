/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/colordlg.cpp
// Purpose:     wxColourDialog using WinUI ContentDialog + ColorPicker
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_COLOURDLG && wxUSE_WINUI3

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/colordlg.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/msw/private.h"
#include "wx/stockitem.h"

#include <winrt/Windows.Foundation.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog);

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data)
{
    m_winuiParent = GetParentForModalDialog(parent, 0);

    if ( data )
        m_colourData = *data;

    return true;
}

void wxColourDialog::SetTitle(const wxString& title)
{
    m_title = title;
}

wxString wxColourDialog::GetTitle() const
{
    return m_title;
}

int wxColourDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = m_winuiParent
        ? m_winuiParent
        : GetParentForModalDialog(nullptr, 0);
    HWND hwndParent = parent ? GetHwndOf(parent) : nullptr;

    if ( !hwndParent || !wxWinUI3Initialize() )
        return wxID_CANCEL;

    try
    {
        using namespace winrt::Microsoft::UI;
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;
        using namespace winrt::Windows::Foundation;

        DesktopWindowXamlSource source;
        const auto windowId = GetWindowIdFromWindow(hwndParent);
        source.Initialize(windowId);
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
            return wxID_CANCEL;
        }

        ContentDialog dialog;
        dialog.XamlRoot(root.XamlRoot());
        dialog.RequestedTheme(wxWinUIGetCurrentElementTheme());
        dialog.Title(winrt::box_value(wxWinUIToHString(
            m_title.empty() ? wxString(_("Choose colour")) : m_title)));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.PrimaryButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(
            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON))));
        dialog.CloseButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(
            wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON))));

        ColorPicker picker;
        picker.IsAlphaEnabled(m_colourData.GetChooseAlpha());
        picker.IsMoreButtonVisible(true);

        const wxColour initial = m_colourData.GetColour();
        if ( initial.IsOk() )
        {
            winrt::Windows::UI::Color color{};
            color.A = initial.Alpha();
            color.R = initial.Red();
            color.G = initial.Green();
            color.B = initial.Blue();
            picker.Color(color);
        }

        winrt::event_token colorChangedToken = picker.ColorChanged(
            [this](ColorPicker const&, ColorChangedEventArgs const& args)
            {
                const auto c = args.NewColor();
                wxColourDialogEvent event(wxEVT_COLOUR_CHANGED, this,
                                          wxColour(c.R, c.G, c.B, c.A));
                ProcessWindowEvent(event);
            });
        wxUnusedVar(colorChangedToken);

        dialog.Content(picker);

        ContentDialogResult dialogResult = ContentDialogResult::None;
        bool done = false;
        bool loopIsRunning = false;
        wxEventLoop* loopRunning = nullptr;

        // Behave app-modally: block the other top-level windows while the
        // dialog is up (the parent hosts the island and must stay enabled).
        wxWindowDisabler disabler(parent);

        auto operation = dialog.ShowAsync();
        operation.Completed(
            [&](IAsyncOperation<ContentDialogResult> const& async,
                AsyncStatus status)
            {
                if ( status == AsyncStatus::Completed )
                    dialogResult = async.GetResults();

                done = true;
                if ( loopRunning && loopIsRunning )
                    loopRunning->Exit();
            });

        wxEventLoop loop;
        loopRunning = &loop;
        if ( !done )
        {
            loopIsRunning = true;
            loop.Run();
            loopIsRunning = false;
        }
        loopRunning = nullptr;

        wxColour chosen;
        if ( dialogResult == ContentDialogResult::Primary )
        {
            const auto c = picker.Color();
            chosen = wxColour(c.R, c.G, c.B, c.A);
        }

        source.Close();

        if ( chosen.IsOk() )
        {
            m_colourData.SetColour(chosen);
            return wxID_OK;
        }

        return wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ColourDialog ContentDialog", e);
    }

    return wxID_CANCEL;
}

#endif // wxUSE_COLOURDLG && wxUSE_WINUI3
