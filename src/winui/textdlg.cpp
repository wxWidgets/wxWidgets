/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/textdlg.cpp
// Purpose:     wxTextEntryDialog using a WinUI dialog presenter
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "textdialog-test-access.h"
#endif

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
#include "wx/scopeguard.h"
#include "wx/stockitem.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogcontracts.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <cmath>

#include <algorithm>
#include <memory>
#include <winrt/Windows.Foundation.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

// What the dialog shows, in DIPs: its message, which wraps at the width of
// the field, and under it the field. No more: the buttons are under their
// line right after it.
wxSize wxWinUITextEntryBodySize(long style, const wxString& message,
                                const wxWindow *window)
{
    const int width = 360;
    int lines = 0;
    for ( const wxString& line : wxSplit(message, '\n', '\0') )
    {
        const int extent =
            window ? window->ToDIP(window->GetTextExtent(line).x) : 0;
        lines += wxMax(1, (extent + width - 1) / width);
    }
    lines = wxMax(1, lines);

    return wxSize(width,
                  lines * 20 + 8 + ((style & wxTE_MULTILINE) ? 140 : 32));
}

} // anonymous namespace

class wxWinUITextEntryPeerState final
    : public std::enable_shared_from_this<wxWinUITextEntryPeerState>
{
public:
    ~wxWinUITextEntryPeerState()
    {
        Disconnect();
    }

    void Attach(const MUXC::TextBox& textBox,
                const MUXC::PasswordBox& passwordBox,
                bool forceUpper)
    {
        m_textBox = textBox;
        m_passwordBox = passwordBox;

        const std::weak_ptr<wxWinUITextEntryPeerState> weakState =
            shared_from_this();
        if ( m_textBox )
        {
            m_textToken = m_textBox.TextChanged(
                [weakState](
                    const winrt::Windows::Foundation::IInspectable&,
                    const MUXC::TextChangedEventArgs&)
                {
                    if ( const auto state = weakState.lock() )
                        state->ApplyUppercase();
                });
        }
        else if ( m_passwordBox )
        {
            m_passwordToken = m_passwordBox.PasswordChanged(
                [weakState](
                    const winrt::Windows::Foundation::IInspectable&,
                    const MUX::RoutedEventArgs&)
                {
                    if ( const auto state = weakState.lock() )
                        state->ApplyUppercase();
                });
        }

        if ( forceUpper )
            EnableForceUpper();
    }

    void Disconnect()
    {
        if ( !m_active )
            return;
        m_active = false;

        try
        {
            if ( m_textBox && m_textToken.value )
                m_textBox.TextChanged(m_textToken);
            if ( m_passwordBox && m_passwordToken.value )
                m_passwordBox.PasswordChanged(m_passwordToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }

        m_textToken = {};
        m_passwordToken = {};
        m_textBox = nullptr;
        m_passwordBox = nullptr;
    }

    void EnableForceUpper()
    {
        m_forceUpper = true;
        ApplyUppercase();
    }

    bool SetValue(const wxString& value)
    {
        if ( !m_active )
            return false;

        try
        {
            if ( m_textBox )
                m_textBox.Text(wxWinUIToHString(value));
            else if ( m_passwordBox )
                m_passwordBox.Password(wxWinUIToHString(value));
            else
                return false;

            ApplyUppercase();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("TextEntryDialog test value", e);
            return false;
        }
    }

    wxString GetValue() const
    {
        if ( !m_active )
            return {};

        try
        {
            if ( m_textBox )
                return wxWinUIFromHString(m_textBox.Text());
            if ( m_passwordBox )
                return wxWinUIFromHString(m_passwordBox.Password());
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("TextEntryDialog peer value", e);
        }

        return {};
    }

private:
    void ApplyUppercase()
    {
        if ( !m_active || !m_forceUpper || m_updating )
            return;

        try
        {
            wxString value = GetValue();
            wxString upper = value;
            upper.MakeUpper();
            if ( upper == value )
                return;

            m_updating = true;
            wxScopeGuard clear = wxMakeGuard(
                [this]()
                {
                    m_updating = false;
                });
            wxUnusedVar(clear);

            if ( m_textBox )
            {
                const int oldStart = m_textBox.SelectionStart();
                const int oldLength = m_textBox.SelectionLength();
                m_textBox.Text(wxWinUIToHString(upper));
                const int length = static_cast<int>(upper.length());
                const int start = (std::min)(oldStart, length);
                m_textBox.Select(
                    start,
                    (std::min)(oldLength, length - start));
            }
            else if ( m_passwordBox )
            {
                m_passwordBox.Password(wxWinUIToHString(upper));
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("TextEntryDialog ForceUpper", e);
        }
    }

    MUXC::TextBox m_textBox{ nullptr };
    MUXC::PasswordBox m_passwordBox{ nullptr };
    winrt::event_token m_textToken{};
    winrt::event_token m_passwordToken{};
    bool m_active = true;
    bool m_forceUpper = false;
    bool m_updating = false;
};

const char wxGetTextFromUserPromptStr[] = "Input Text";
const char wxGetPasswordFromUserPromptStr[] = "Enter Password";

wxIMPLEMENT_CLASS(wxTextEntryDialog, wxDialog);
wxIMPLEMENT_CLASS(wxPasswordEntryDialog, wxTextEntryDialog);

// All the members carry default initializers in the class declaration, so
// both constructors start from the same clean state.
wxTextEntryDialog::wxTextEntryDialog() = default;

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
    // wxTE_MULTILINE and wxDIALOG_NO_PARENT have the same numeric value.
    // As in the generic dialog, don't treat text-control flags as dialog flags.
    m_winuiParent = GetParentForModalDialog(parent, 0);
    m_message = message;
    m_caption = caption;
    m_value = value;
    m_dialogStyle = style;
    m_pos = pos;
    m_size = sz;
    m_isPassword = (style & wxTE_PASSWORD) != 0;

    if ( !wxDialog::Create(m_winuiParent, wxID_ANY, caption, pos, sz,
                           wxDEFAULT_DIALOG_STYLE) )
    {
        return false;
    }

    // Initialize only unspecified dimensions. Window presentation borrows
    // this dialog and must not reset its later Move()/SetSize() changes.
    const wxWeakRef<wxWindow> weakThis(this);
    if ( sz.x == wxDefaultCoord || sz.y == wxDefaultCoord )
    {
        const wxSize naturalSize = FromDIP(
            wxWinUIDialogPresenter::GetWindowClientSize(
                wxWinUITextEntryBodySize(style, message, this),
                (style & wxCANCEL) ? 2 : 1));
        wxSize clientSize = GetClientSize();
        if ( sz.x == wxDefaultCoord )
            clientSize.x = naturalSize.x;
        if ( sz.y == wxDefaultCoord )
            clientSize.y = naturalSize.y;
        SetClientSize(clientSize);
        if ( !weakThis || IsBeingDeleted() )
            return false;
    }

    if ( style & wxCENTRE )
        Centre(wxBOTH);

    return weakThis && !IsBeingDeleted();
}

int wxTextEntryDialog::ShowModal()
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

    if ( !wxWinUI3Initialize() )
        return wxID_CANCEL;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        wxWinUIDialogPresenter presenter;
        if ( !presenter.CreateForDialog(this) )
            return wxID_CANCEL;

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

            const int maxLength = wxWinUITextEntryMaxLength(m_maxLength);
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

            const int maxLength = wxWinUITextEntryMaxLength(m_maxLength);
            if ( maxLength > 0 )
                textBox.MaxLength(maxLength);

            content.Children().Append(textBox);
        }

        auto peerState =
            std::make_shared<wxWinUITextEntryPeerState>();
        peerState->Attach(textBox, passwordBox, m_forceUpper);
        m_peerState = peerState;
        const wxWeakRef<wxTextEntryDialog> peerOwner(this);
        wxScopeGuard releasePeerState = wxMakeGuard(
            [&]()
            {
                peerState->Disconnect();
                if ( wxTextEntryDialog * const live = peerOwner.get();
                     live && live->m_peerState == peerState )
                {
                    live->m_peerState.reset();
                }
            });
        wxUnusedVar(releasePeerState);

        TextBlock errorText;
        errorText.Visibility(Visibility::Collapsed);
        content.Children().Append(errorText);

        presenter.SetContent(content);
        presenter.SetContentSize(
            wxWinUITextEntryBodySize(m_dialogStyle, m_message, this));

        bool accepted = false;
        wxString acceptedValue;
        const wxWeakRef<wxTextEntryDialog> weakSelf(this);

        // Validate on OK; returning false keeps the dialog open with the error
        // message shown under the entry field.
        presenter.SetAcceptHandler(
            [weakSelf, peerState, errorText,
             &accepted, &acceptedValue](int id) -> bool
            {
                wxTextEntryDialog *live = weakSelf.get();
                if ( !live )
                    return false;

                if ( id != wxID_OK )
                    return true;

                wxString value = peerState->GetValue();

                if ( live->m_forceUpper )
                    value.MakeUpper();

#if wxUSE_VALIDATORS
                if ( live->m_validator )
                {
                    const wxString error =
                        live->m_validator->IsValid(value);
                    live = weakSelf.get();
                    if ( !live )
                        return false;

                    if ( !error.empty() )
                    {
                        errorText.Text(wxWinUIToHString(error));
                        errorText.Visibility(Visibility::Visible);
                        return false;
                    }
                }
#endif

                if ( !weakSelf )
                    return false;

                acceptedValue = value;
                accepted = true;
                return true;
            });

        presenter.AddButton(wxID_OK,
                            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON), true);
        if ( m_dialogStyle & wxCANCEL )
        {
            presenter.AddButton(wxID_CANCEL,
                                wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON));
        }

        const int result = presenter.ShowModal();
        wxTextEntryDialog * const live = weakSelf.get();
        if ( !live || live->IsBeingDeleted() )
            return wxID_CANCEL;

        if ( result == wxID_OK && accepted )
            live->m_value = acceptedValue;

        // Direct EndModal() retains its explicit result, but doesn't perform
        // validation or accept the in-progress value on the caller's behalf.
        return result;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("TextEntryDialog presenter", e);
    }

    return wxID_CANCEL;
}

namespace
{

// What the dialog of numbers shows, in DIPs: as the dialog of text, with
// the header of the field when there is a prompt.
wxSize wxWinUINumberEntryBodySize(const wxString& message,
                                  const wxString& prompt,
                                  const wxWindow *window)
{
    wxSize body = wxWinUITextEntryBodySize(0, message, window);
    if ( !prompt.empty() )
        body.y += 28;
    return body;
}

} // anonymous namespace

wxSize wxWinUIGetNumberEntryClientSize(const wxWindow *dialog,
                                       const wxString& message,
                                       const wxString& prompt)
{
    return wxWinUIDialogPresenter::GetWindowClientSize(
        wxWinUINumberEntryBodySize(message, prompt, dialog), 2);
}

int wxWinUIShowNumberEntry(wxDialog *dialog,
                           const wxString& message,
                           const wxString& prompt,
                           long *value,
                           long min, long max)
{
    wxCHECK_MSG( dialog && value, wxID_CANCEL, "no dialog or no value" );

    if ( !wxWinUI3Initialize() )
        return wxID_CANCEL;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        wxWinUIDialogPresenter presenter;
        if ( !presenter.CreateForDialog(dialog) )
            return wxID_CANCEL;

        StackPanel content;
        content.Spacing(8);

        TextBlock text;
        wxWinUISetDialogText(text, message);
        content.Children().Append(text);

        NumberBox number;
        if ( !prompt.empty() )
            number.Header(winrt::box_value(wxWinUIToHString(prompt)));
        number.Minimum(static_cast<double>(min));
        number.Maximum(static_cast<double>(max));
        number.Value(static_cast<double>(*value));
        number.SmallChange(1);
        number.LargeChange(10);
        number.AcceptsExpression(false);
        number.SpinButtonPlacementMode(
            NumberBoxSpinButtonPlacementMode::Inline);
        number.MinWidth(320);
        content.Children().Append(number);

        presenter.SetContent(content);
        presenter.SetContentSize(
            wxWinUINumberEntryBodySize(message, prompt, dialog));

        long accepted = *value;
        presenter.SetAcceptHandler(
            [number, &accepted, min, max](int id) -> bool
            {
                if ( id != wxID_OK )
                    return true;

                // An empty field is not a number: the dialog stays.
                const double entered = number.Value();
                if ( !std::isfinite(entered) )
                    return false;

                const long rounded = std::lround(entered);
                if ( rounded < min || rounded > max )
                    return false;

                accepted = rounded;
                return true;
            });

        presenter.AddButton(wxID_OK,
                            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON), true);
        presenter.AddButton(wxID_CANCEL,
                            wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON));

        // The program may destroy the dialog while it is shown: its value
        // is written only if it is still there.
        const wxWeakRef<wxDialog> weakDialog(dialog);
        const int result = presenter.ShowModal();
        if ( !weakDialog || weakDialog->IsBeingDeleted() )
            return wxID_CANCEL;

        if ( result == wxID_OK )
            *value = accepted;
        return result == wxID_OK ? wxID_OK : wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("NumberEntryDialog presenter", e);
    }

    return wxID_CANCEL;
}

void wxTextEntryDialog::SetValue(const wxString& val)
{
    m_value = val;
    if ( m_forceUpper )
        m_value.MakeUpper();
    if ( m_peerState )
        m_peerState->SetValue(m_value);
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
    m_value.MakeUpper();
    if ( m_peerState )
        m_peerState->EnableForceUpper();
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextEntryDialogTestAccess::SetPeerValue(
    wxTextEntryDialog& dialog,
    const wxString& value)
{
    return dialog.m_peerState && dialog.m_peerState->SetValue(value);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
wxString wxWinUITextEntryDialogTestAccess::GetPeerValue(
    const wxTextEntryDialog& dialog)
{
    return dialog.m_peerState ? dialog.m_peerState->GetValue() : wxString();
}
#endif // WXWINUI_TEST_SUPPORT

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
    const wxWeakRef<wxWindow> weakThis(this);
    const auto peerState = m_peerState;
    if ( peerState && !peerState->SetValue(m_value) )
        return false;

    return weakThis && !IsBeingDeleted() && wxDialog::TransferDataToWindow();
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
