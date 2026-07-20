/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/msgdlg.cpp
// Purpose:     wxMessageDialog using WinUI ContentDialog
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_MSGDLG && wxUSE_WINUI3

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/msgdlg.h"
    #include "wx/settings.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/msw/private.h"

#include <winrt/Windows.Foundation.h>

wxIMPLEMENT_CLASS(wxMessageDialog, wxDialog);

namespace
{

struct wxWinUIMessageDialogButton
{
    wxString label;
    int id = wxID_NONE;
};

struct wxWinUIMessageDialogButtons
{
    wxWinUIMessageDialogButton primary;
    wxWinUIMessageDialogButton secondary;
    wxWinUIMessageDialogButton close;
    winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton defaultButton =
        winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary;
};

bool wxWinUIBuildMessageDialogButtons(const wxMessageDialog& dlg,
                                      wxWinUIMessageDialogButtons& buttons)
{
    const long style = dlg.GetMessageDialogStyle();

    if ( (style & wxYES_NO) && (style & wxCANCEL) && (style & wxHELP) )
        return false;

    if ( style & wxYES_NO )
    {
        buttons.primary = { dlg.GetYesLabel(), wxID_YES };
        buttons.secondary = { dlg.GetNoLabel(), wxID_NO };

        if ( style & wxCANCEL )
            buttons.close = { dlg.GetCancelLabel(), wxID_CANCEL };
        else if ( style & wxHELP )
            buttons.close = { dlg.GetHelpLabel(), wxID_HELP };

        if ( style & wxNO_DEFAULT )
            buttons.defaultButton =
                winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Secondary;
        else if ( style & wxCANCEL_DEFAULT )
            buttons.defaultButton =
                winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Close;
    }
    else
    {
        buttons.primary = { dlg.GetOKLabel(), wxID_OK };

        if ( style & wxHELP )
            buttons.secondary = { dlg.GetHelpLabel(), wxID_HELP };

        if ( style & wxCANCEL )
            buttons.close = { dlg.GetCancelLabel(), wxID_CANCEL };

        if ( style & wxCANCEL_DEFAULT )
            buttons.defaultButton =
                winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Close;
    }

    return true;
}

int wxWinUIFallbackMessageBox(const wxMessageDialog& dlg)
{
    const long wxStyle = dlg.GetMessageDialogStyle();
    unsigned msStyle = 0;

    if ( wxStyle & wxYES_NO )
    {
        msStyle = (wxStyle & wxCANCEL) ? MB_YESNOCANCEL : MB_YESNO;

        if ( wxStyle & wxNO_DEFAULT )
            msStyle |= MB_DEFBUTTON2;
        else if ( wxStyle & wxCANCEL_DEFAULT )
            msStyle |= MB_DEFBUTTON3;
    }
    else
    {
        msStyle = (wxStyle & wxCANCEL) ? MB_OKCANCEL : MB_OK;

        if ( wxStyle & wxCANCEL_DEFAULT )
            msStyle |= MB_DEFBUTTON2;
    }

    if ( wxStyle & wxHELP )
        msStyle |= MB_HELP;

    switch ( dlg.GetEffectiveIcon() )
    {
        case wxICON_ERROR:
            msStyle |= MB_ICONERROR;
            break;
        case wxICON_WARNING:
            msStyle |= MB_ICONWARNING;
            break;
        case wxICON_QUESTION:
            msStyle |= MB_ICONQUESTION;
            break;
        case wxICON_INFORMATION:
            msStyle |= MB_ICONINFORMATION;
            break;
    }

    if ( wxStyle & wxSTAY_ON_TOP )
        msStyle |= MB_TOPMOST;

    wxWindow* const parent = dlg.GetParentForModalDialog();
    HWND hwndParent = parent ? GetHwndOf(parent) : nullptr;

    wxString fullMessage = dlg.GetMessage();
    if ( !dlg.GetExtendedMessage().empty() )
        fullMessage << wxASCII_STR("\n\n") << dlg.GetExtendedMessage();

    const int rc = ::MessageBox(hwndParent,
                                fullMessage.t_str(),
                                dlg.GetCaption().t_str(),
                                msStyle);

    switch ( rc )
    {
        case IDOK:
            return wxID_OK;
        case IDCANCEL:
            return wxID_CANCEL;
        case IDYES:
            return wxID_YES;
        case IDNO:
            return wxID_NO;
    }

    return wxID_CANCEL;
}

} // anonymous namespace

int wxMessageDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = GetParentForModalDialog();
    HWND hwndParent = parent ? GetHwndOf(parent) : nullptr;

    if ( !hwndParent || !wxWinUI3Initialize() )
        return wxWinUIFallbackMessageBox(*this);

    wxWinUIMessageDialogButtons buttons;
    if ( !wxWinUIBuildMessageDialogButtons(*this, buttons) )
        return wxWinUIFallbackMessageBox(*this);

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
            return wxWinUIFallbackMessageBox(*this);
        }

        ContentDialog dialog;
        dialog.XamlRoot(root.XamlRoot());
        dialog.RequestedTheme(wxWinUIGetCurrentElementTheme());
        dialog.Title(winrt::box_value(wxWinUIToHString(GetCaption())));
        dialog.DefaultButton(buttons.defaultButton);

        wxString message = GetMessage();
        wxString extended = GetExtendedMessage();
        if ( extended.empty() )
        {
            const size_t pos = message.find("\n\n");
            if ( pos != wxString::npos )
            {
                extended.assign(message, pos + 2, wxString::npos);
                message.erase(pos);
            }
        }

        StackPanel content;
        content.Spacing(8);

        TextBlock mainText;
        wxWinUISetDialogText(mainText, message);
        content.Children().Append(mainText);

        if ( !extended.empty() )
        {
            TextBlock extendedText;
            wxWinUISetDialogText(extendedText, extended);
            extendedText.Opacity(0.78);
            content.Children().Append(extendedText);
        }

        dialog.Content(content);

        if ( buttons.primary.id != wxID_NONE )
            dialog.PrimaryButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(buttons.primary.label)));
        if ( buttons.secondary.id != wxID_NONE )
            dialog.SecondaryButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(buttons.secondary.label)));
        if ( buttons.close.id != wxID_NONE )
            dialog.CloseButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(buttons.close.label)));

        ContentDialogResult dialogResult = ContentDialogResult::None;
        bool done = false;
        bool loopIsRunning = false;
        wxEventLoop* loopRunning = nullptr;

        // Behave app-modally: block the other top-level windows while the
        // dialog is up.  The parent itself must stay enabled since it hosts
        // the dialog's island (its client area is covered by the smoke layer).
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

        source.Close();

        switch ( dialogResult )
        {
            case ContentDialogResult::Primary:
                return buttons.primary.id;
            case ContentDialogResult::Secondary:
                return buttons.secondary.id;
            case ContentDialogResult::None:
                if ( buttons.close.id != wxID_NONE )
                    return buttons.close.id;
                if ( !(GetMessageDialogStyle() & (wxYES_NO | wxCANCEL)) )
                    return wxID_OK;
                return wxID_CANCEL;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ContentDialog", e);
    }

    return wxWinUIFallbackMessageBox(*this);
}

wxFont wxMessageDialog::GetMessageFont()
{
    return wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
}

void wxMessageDialog::DoCentre(int WXUNUSED(dir))
{
    // ContentDialog is positioned by WinUI relative to its XamlRoot.
}

#endif // wxUSE_MSGDLG && wxUSE_WINUI3
