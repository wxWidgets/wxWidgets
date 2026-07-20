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
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        wxWinUIDialogPresenter presenter;
        if ( !presenter.Create(parent, GetCaption()) )
            return wxWinUIFallbackMessageBox(*this);

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

        presenter.SetContent(content);

        // Wrap the message at a comfortable width and give the dialog enough
        // room for it: the XAML text can't be measured before it is realised.
        const int textWidth = 380;
        wxString allText = message;
        if ( !extended.empty() )
            allText << wxASCII_STR("\n\n") << extended;

        wxSize textSize = GetTextExtent(allText);
        int lines = 1 + static_cast<int>(allText.Freq('\n'));
        if ( textSize.x > 0 )
            lines += textSize.x / ToDIP(textWidth);
        presenter.SetContentSize(
            wxSize(textWidth, wxMax(48, lines * 22)));

        const int defaultId =
            buttons.defaultButton == ContentDialogButton::Secondary
                ? buttons.secondary.id
                : buttons.defaultButton == ContentDialogButton::Close
                    ? buttons.close.id
                    : buttons.primary.id;

        if ( buttons.primary.id != wxID_NONE )
        {
            presenter.AddButton(buttons.primary.id, buttons.primary.label,
                                buttons.primary.id == defaultId);
        }
        if ( buttons.secondary.id != wxID_NONE )
        {
            presenter.AddButton(buttons.secondary.id, buttons.secondary.label,
                                buttons.secondary.id == defaultId);
        }
        if ( buttons.close.id != wxID_NONE )
        {
            presenter.AddButton(buttons.close.id, buttons.close.label,
                                buttons.close.id == defaultId);
        }

        const int result = presenter.ShowModal();
        if ( result != wxID_CANCEL )
            return result;

        // Cancelled: report the dismissal the way the caller expects.
        if ( buttons.close.id != wxID_NONE )
            return buttons.close.id;
        if ( !(GetMessageDialogStyle() & (wxYES_NO | wxCANCEL)) )
            return wxID_OK;
        return wxID_CANCEL;
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
