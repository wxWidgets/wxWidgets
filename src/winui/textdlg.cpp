/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/textdlg.cpp
// Purpose:     wxTextEntryDialog using WinUI ContentDialog
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_TEXTDLG && wxUSE_WINUI3

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/textdlg.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/msw/private.h"
#include "wx/stockitem.h"

#include <climits>
#include <winrt/Windows.Foundation.h>

const char wxGetTextFromUserPromptStr[] = "Input Text";
const char wxGetPasswordFromUserPromptStr[] = "Enter Password";

wxIMPLEMENT_CLASS(wxTextEntryDialog, wxDialog);
wxIMPLEMENT_CLASS(wxPasswordEntryDialog, wxTextEntryDialog);

namespace
{

int wxWinUIContentDialogMaxLength(unsigned long len)
{
    if ( len == 0 )
        return 0;

    return len > static_cast<unsigned long>(INT_MAX)
        ? INT_MAX
        : static_cast<int>(len);
}

} // anonymous namespace

wxTextEntryDialog::wxTextEntryDialog()
{
    m_textctrl = nullptr;
    m_dialogStyle = 0;
    m_winuiParent = nullptr;
    m_maxLength = 0;
    m_forceUpper = false;
    m_isPassword = false;
#if wxUSE_VALIDATORS
    m_validator = nullptr;
#endif
}

wxTextEntryDialog::~wxTextEntryDialog()
{
#if wxUSE_VALIDATORS
    delete m_validator;
#endif
}

bool wxTextEntryDialog::Create(wxWindow *parent,
                               const wxString& message,
                               const wxString& caption,
                               const wxString& value,
                               long style,
                               const wxPoint& pos,
                               const wxSize sz)
{
    m_winuiParent = GetParentForModalDialog(parent, 0);
    m_message = message;
    m_caption = caption;
    m_value = value;
    m_dialogStyle = style;
    m_pos = pos;
    m_size = sz;
    m_isPassword = (style & wxTE_PASSWORD) != 0;

    // Create a real (hidden) dialog window so that this object behaves like a
    // normal wxDialog for the application (valid GetHandle(), event routing,
    // parent relationship); the UI actually shown by ShowModal() is a WinUI
    // ContentDialog over the parent, this window is never made visible.
    return wxDialog::Create(parent, wxID_ANY, caption, pos, sz,
                            wxDEFAULT_DIALOG_STYLE);
}

int wxTextEntryDialog::ShowModal()
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
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        wxWinUIDialogIsland island;
        if ( !island.Create(parent) )
            return wxID_CANCEL;

        ContentDialog dialog = island.CreateDialog();
        dialog.Title(winrt::box_value(wxWinUIToHString(m_caption)));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.PrimaryButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(
            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON))));
        if ( m_dialogStyle & wxCANCEL )
        {
            dialog.CloseButtonText(wxWinUIToHString(wxWinUIRemoveMnemonics(
                wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON))));
        }

        StackPanel content;
        content.Spacing(8);

        TextBlock message;
        wxWinUISetDialogText(message, m_message);
        content.Children().Append(message);

        TextBox textBox{ nullptr };
        PasswordBox passwordBox{ nullptr };

        if ( m_isPassword )
        {
            passwordBox = PasswordBox();
            passwordBox.Password(wxWinUIToHString(m_value));
            passwordBox.MinWidth(320);
            if ( !m_hint.empty() )
                passwordBox.PlaceholderText(wxWinUIToHString(m_hint));

            const int maxLength = wxWinUIContentDialogMaxLength(m_maxLength);
            if ( maxLength > 0 )
                passwordBox.MaxLength(maxLength);

            content.Children().Append(passwordBox);
        }
        else
        {
            textBox = TextBox();
            textBox.Text(wxWinUIToHString(m_value));
            textBox.MinWidth(320);
            textBox.AcceptsReturn((m_dialogStyle & wxTE_MULTILINE) != 0);
            textBox.TextWrapping(TextWrapping::Wrap);
            if ( m_dialogStyle & wxTE_MULTILINE )
                textBox.Height(140);
            if ( !m_hint.empty() )
                textBox.PlaceholderText(wxWinUIToHString(m_hint));

            const int maxLength = wxWinUIContentDialogMaxLength(m_maxLength);
            if ( maxLength > 0 )
                textBox.MaxLength(maxLength);

            content.Children().Append(textBox);
        }

        TextBlock errorText;
        errorText.Visibility(Visibility::Collapsed);
        content.Children().Append(errorText);

        dialog.Content(content);

        bool accepted = false;
        wxString acceptedValue;

        dialog.PrimaryButtonClick(
            [&](ContentDialog const& WXUNUSED(sender),
                ContentDialogButtonClickEventArgs const& event)
            {
                wxString value = m_isPassword
                    ? wxWinUIFromHString(passwordBox.Password())
                    : wxWinUIFromHString(textBox.Text());

                if ( m_forceUpper )
                    value.MakeUpper();

#if wxUSE_VALIDATORS
                if ( m_validator )
                {
                    const wxString error = m_validator->IsValid(value);
                    if ( !error.empty() )
                    {
                        errorText.Text(wxWinUIToHString(error));
                        errorText.Visibility(Visibility::Visible);
                        event.Cancel(true);
                        return;
                    }
                }
#endif

                acceptedValue = value;
                accepted = true;
            });

        const ContentDialogResult dialogResult = island.ShowDialog(dialog);
        island.Close();

        if ( dialogResult == ContentDialogResult::Primary && accepted )
        {
            m_value = acceptedValue;
            return wxID_OK;
        }

        return wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("TextEntryDialog ContentDialog", e);
    }

    return wxID_CANCEL;
}

void wxTextEntryDialog::SetValue(const wxString& val)
{
    m_value = val;
}

void wxTextEntryDialog::SetHint(const wxString& hint)
{
    m_hint = hint;
}

void wxTextEntryDialog::SetMaxLength(unsigned long len)
{
    m_maxLength = len;
}

void wxTextEntryDialog::ForceUpper()
{
    m_forceUpper = true;
}

#if wxUSE_VALIDATORS

void wxTextEntryDialog::SetTextValidator(wxTextValidatorStyle style)
{
    SetTextValidator(wxTextValidator(style));
}

void wxTextEntryDialog::SetTextValidator(const wxTextValidator& validator)
{
    delete m_validator;
    m_validator = static_cast<wxTextValidator*>(validator.Clone());
}

#endif // wxUSE_VALIDATORS

bool wxTextEntryDialog::TransferDataToWindow()
{
    return true;
}

bool wxTextEntryDialog::TransferDataFromWindow()
{
    return true;
}

void wxTextEntryDialog::OnOK(wxCommandEvent& event)
{
    event.Skip();
}

bool wxPasswordEntryDialog::Create(wxWindow *parent,
                                   const wxString& message,
                                   const wxString& caption,
                                   const wxString& value,
                                   long style,
                                   const wxPoint& pos)
{
    return wxTextEntryDialog::Create(parent, message, caption, value,
                                     style | wxTE_PASSWORD, pos);
}

#endif // wxUSE_TEXTDLG && wxUSE_WINUI3
