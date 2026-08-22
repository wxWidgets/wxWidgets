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
#include "wx/msw/private/msgdlg.h"
#include "wx/scopeguard.h"
#include "wx/winui/private/dialogcontracts.h"

#include <winrt/Windows.Foundation.h>

wxIMPLEMENT_CLASS(wxMessageDialog, wxDialog);

namespace
{

int wxWinUIFallbackMessageBox(wxMessageDialog& dlg)
{
    const long wxStyle = dlg.GetMessageDialogStyle();
    wxWindow * const parent = dlg.GetParentForModalDialog();

    // Native TaskDialog/MessageBox only disable their HWND owner. wx modal
    // dialogs are application-modal, so also disable unrelated TLWs for the
    // exact duration of the fallback, matching the established MSW port.
    wxWindowDisabler disableOthers(&dlg, parent);

    // Preserve the complete wx button/label contract whenever comctl32 v6 is
    // available. In particular MessageBox cannot represent four buttons or
    // any Set*Label() customization, while the shared MSW task-dialog helper
    // supports both.
    if ( wxMSWMessageDialog::TaskDialogIndirect_t taskDialogIndirect =
             wxMSWMessageDialog::GetTaskDialogIndirectFunc() )
    {
        WinStruct<TASKDIALOGCONFIG> config;
        wxMSWMessageDialog::wxMSWTaskDialogConfig wxConfig(dlg);
        wxConfig.MSWCommonTaskDialogInit(config);

        int nativeResult = IDCANCEL;
        const HRESULT hr =
            taskDialogIndirect(&config, &nativeResult, nullptr, nullptr);
        if ( SUCCEEDED(hr) )
        {
            if ( nativeResult == IDCANCEL &&
                 !(wxStyle & (wxYES_NO | wxCANCEL)) )
            {
                nativeResult = IDOK;
            }
            return wxMSWMessageDialog::MSWTranslateReturnCode(
                nativeResult);
        }

        wxLogApiError("TaskDialogIndirect", hr);
    }

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
        case IDHELP:
            return wxID_HELP;
    }

    return wxID_CANCEL;
}

} // anonymous namespace

bool wxWinUIBuildMessageDialogButtons(
    const wxMessageDialog& dlg,
    wxWinUIMessageDialogButtons& buttons)
{
    buttons.layout =
        wxWinUIBuildMessageDialogLayout(dlg.GetMessageDialogStyle());
    if ( buttons.layout.requiresNativeFallback )
        return false;

    auto buttonForId = [&dlg](int id) -> wxWinUIMessageDialogButton
    {
        switch ( id )
        {
            case wxID_YES:
                return { dlg.GetYesLabel(), id };
            case wxID_NO:
                return { dlg.GetNoLabel(), id };
            case wxID_OK:
                return { dlg.GetOKLabel(), id };
            case wxID_CANCEL:
                return { dlg.GetCancelLabel(), id };
            case wxID_HELP:
                return { dlg.GetHelpLabel(), id };
        }

        return {};
    };

    for ( std::size_t i = 0; i < buttons.layout.buttonCount; ++i )
        buttons.values[i] = buttonForId(buttons.layout.buttonIds[i]);

    return true;
}

int wxMessageDialog::ShowModal()
{
    const wxWeakRef<wxDialog> externalLifetimeSelf(this);
    WinUIBeginExternalModalLifetime();
    wxScopeGuard externalLifetime = wxMakeGuard(
        [externalLifetimeSelf]()
        {
            if ( wxDialog * const live = externalLifetimeSelf.get() )
                live->WinUIEndExternalModalLifetime();
        });
    wxUnusedVar(externalLifetime);

    WX_HOOK_MODAL_DIALOG();
    WinUIArmExternalModalLifetime();
    if ( IsBeingDeleted() )
        return wxID_CANCEL;

    const wxWeakRef<wxWindow> weakSelf(this);

    wxWindow* const parent = GetParentForModalDialog();

    if ( !wxWinUI3Initialize() )
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
        presenter.SetLifetimeOwner(this);

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
        presenter.SetExternalDismissAllowed(
            buttons.layout.canDismissExternally);

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
            buttons.layout.buttonIds[buttons.layout.defaultIndex];

        for ( std::size_t i = 0;
              i < buttons.layout.buttonCount;
              ++i )
        {
            const wxWinUIMessageDialogButton& button =
                buttons.values[i];
            presenter.AddButton(
                button.id, button.label, button.id == defaultId);
        }

        return wxWinUIResolveMessageDialogDismissal(
            buttons.layout, presenter.ShowModal());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ContentDialog", e);
    }

    // A queued Destroy() is allowed to cancel the presenter while its nested
    // loop is active. Never dereference the dialog after that cancellation.
    wxMessageDialog * const live =
        static_cast<wxMessageDialog *>(weakSelf.get());
    return live && !live->IsBeingDeleted()
        ? wxWinUIFallbackMessageBox(*live)
        : wxID_CANCEL;
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
