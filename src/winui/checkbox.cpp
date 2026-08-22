/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/checkbox.cpp
// Purpose:     wxWinUI wxCheckBox implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
#endif

#include "private.h"
#include "wx/winui/private/appearance.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <memory>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

class wxWinUICheckBoxCallbackState final
{
public:
    explicit wxWinUICheckBoxCallbackState(wxCheckBox *owner)
        : m_owner(owner)
    {
    }

    wxCheckBox *GetOwner() const { return m_owner; }
    void Invalidate() { m_owner = nullptr; }

private:
    wxCheckBox *m_owner;
};

class wxWinUICheckBoxImpl
{
public:
    ~wxWinUICheckBoxImpl()
    {
        Close();
    }

    void Close()
    {
        // A retained AutomationPeer/provider can keep the XAML CheckBox alive
        // after its wx owner is gone. Invalidate the shared callback state
        // before revoking anything, because revocation and host detachment may
        // synchronously run application/XAML code.
        if ( callbackState )
            callbackState->Invalidate();

        if ( checkBox )
        {
            const MUXC::CheckBox peer = checkBox;
            const auto revoke =
                [](winrt::event_token& token,
                   auto&& remove,
                   const char *context)
                {
                    if ( !token.value )
                        return;

                    try
                    {
                        remove(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(context, e);
                    }
                    token = {};
                };

            revoke(checkedToken,
                   [peer](winrt::event_token token)
                   {
                       peer.Checked(token);
                   },
                   "WinUI CheckBox Checked removal");
            revoke(uncheckedToken,
                   [peer](winrt::event_token token)
                   {
                       peer.Unchecked(token);
                   },
                   "WinUI CheckBox Unchecked removal");
            revoke(indeterminateToken,
                   [peer](winrt::event_token token)
                   {
                       peer.Indeterminate(token);
                   },
                   "WinUI CheckBox Indeterminate removal");
        }

        host.Close();
        checkBox = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUICheckBoxCallbackState> callbackState;
    MUXC::CheckBox checkBox{ nullptr };
    winrt::event_token checkedToken{};
    winrt::event_token uncheckedToken{};
    winrt::event_token indeterminateToken{};
    bool updating = false;
};

wxCheckBox::wxCheckBox()
    : m_state(wxCHK_UNCHECKED)
{
}

wxCheckBox::wxCheckBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& label,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : m_state(wxCHK_UNCHECKED)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxCheckBox::~wxCheckBox()
{
    // Invalidate retained XAML callbacks at destructor entry, before the
    // remaining derived members are torn down.
    m_winui.reset();
}

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    WXValidateStyle(&style);

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUICheckBoxImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUICheckBoxCallbackState>(this);
    wxWinUICheckBoxImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUICheckBoxCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    try
    {
        m_winui->checkBox = MUXC::CheckBox();

        // The WinUI check box cycles through the indeterminate state on click
        // only when IsThreeState is set; this must therefore only be enabled
        // for wxCHK_3STATE check boxes that also allow the user to set the 3rd
        // state.  A program-only 3rd state is still shown when set explicitly.
        m_winui->checkBox.IsThreeState(Is3State() && Is3rdStateAllowedForUser());

        auto handler =
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxCheckBox * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->updating )
                {
                    return;
                }

                try
                {
                    auto isChecked = owner->m_winui->checkBox.IsChecked();
                    if ( !isChecked )
                        owner->m_state = wxCHK_UNDETERMINED;
                    else
                    {
                        owner->m_state = isChecked.Value()
                            ? wxCHK_CHECKED
                            : wxCHK_UNCHECKED;
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException("WinUI CheckBox state read", e);
                    return;
                }

                // The event can destroy owner. Nothing may follow it.
                owner->SendCheckBoxEvent();
            };

        m_winui->checkedToken = m_winui->checkBox.Checked(handler);
        m_winui->uncheckedToken = m_winui->checkBox.Unchecked(handler);
        m_winui->indeterminateToken = m_winui->checkBox.Indeterminate(handler);

        if ( !UpdateWinUIContent(false) ||
             !UpdateWinUIAppearance(false) )
        {
            wxCheckBox * const owner = callbackState->GetOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == createImpl )
            {
                owner->m_winui.reset();
            }
            return false;
        }

        wxCheckBox *owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }

        const MUXC::CheckBox peer = createImpl->checkBox;
        const bool contentSet =
            createImpl->host.SetContent(peer);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox creation", e);
        wxCheckBox * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxCheckBox * const owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);

    wxCheckBox * const ownerAfterSize = callbackState->GetOwner();
    return ownerAfterSize && ownerAfterSize->m_winui &&
           ownerAfterSize->m_winui.get() == createImpl;
}

void wxCheckBox::SetValue(bool value)
{
    DoSet3StateValue(value ? wxCHK_CHECKED : wxCHK_UNCHECKED);
}

bool wxCheckBox::GetValue() const
{
    // Match the established wx contract: the third state is truthy, only
    // wxCHK_UNCHECKED maps to false.
    return m_state != wxCHK_UNCHECKED;
}

void wxCheckBox::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    if ( !UpdateWinUIContent() )
        return;

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
        SetSize(GetBestSize());
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    const int state = event.GetInt();
    wxCHECK_RET( state == wxCHK_UNCHECKED ||
                 state == wxCHK_CHECKED ||
                 state == wxCHK_UNDETERMINED,
                 wxT("event.GetInt() returned an invalid checkbox state") );

    wxWinUICheckBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUICheckBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;

    DoSet3StateValue(static_cast<wxCheckBoxState>(state));

    if ( callbackState )
    {
        wxCheckBox * const owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState )
        {
            return;
        }
        owner->ProcessCommand(event);
        return;
    }

    ProcessCommand(event);
}

bool wxCheckBox::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return rc;
}

bool wxCheckBox::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxCheckBox::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

#if wxUSE_TOOLTIPS
void wxCheckBox::DoSetToolTipText(const wxString& tip)
{
    wxControl::DoSetToolTipText(tip);
}

void wxCheckBox::DoSetToolTip(wxToolTip *tip)
{
    wxControl::DoSetToolTip(tip);
}
#endif // wxUSE_TOOLTIPS

void wxCheckBox::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);
    UpdateWinUIAppearance();
}

bool wxCheckBox::MSWOnEffectiveLayoutDirectionChanged()
{
    // UpdateWinUIContent() owns a local FlowDirection in order to implement
    // wxALIGN_RIGHT, so it must be refreshed explicitly instead of relying on
    // the shared slot container's inherited direction.
    return UpdateWinUIContent();
}

wxSize wxCheckBox::DoGetBestClientSize() const
{
    // Measure the actual WinUI check box (glyph + label + padding) so the size
    // matches the real rendering rather than a hard-coded guess.
    if ( m_winui )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size != wxDefaultSize )
            return size;
    }

    // Not realised yet: fall back to a guess big enough for the label, which
    // will be corrected once the content is loaded.
    wxSize best = wxWindow::FromDIP(wxSize(32, 32), const_cast<wxCheckBox *>(this));
    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( !text.empty() )
        best.x += GetTextExtent(text).x;

    return best;
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState value)
{
    if ( value == wxCHK_UNDETERMINED && !Is3State() )
        value = wxCHK_UNCHECKED;

    m_state = value;
    UpdateWinUIContent();
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return m_state;
}

void wxCheckBox::SendCheckBoxEvent()
{
    wxCommandEvent event(wxEVT_CHECKBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(static_cast<int>(m_state));
    ProcessCommand(event);
}

bool wxCheckBox::UpdateWinUIContent(bool forceRender)
{
    if ( !m_winui || !m_winui->checkBox )
        return true;

    wxWinUICheckBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUICheckBoxCallbackState> callbackState =
        impl->callbackState;
    const MUXC::CheckBox peer = impl->checkBox;
    const auto getCurrentOwner = [callbackState, impl]() -> wxCheckBox *
    {
        wxCheckBox * const owner = callbackState
                                      ? callbackState->GetOwner()
                                      : nullptr;
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    const wxString label = GetLabel();
    const wxString visibleLabel = wxControl::GetLabelText(label);
    const bool isRTL = GetLayoutDirection() == wxLayout_RightToLeft;
    const bool alignRight = HasFlag(wxALIGN_RIGHT);
    const wxCheckBoxState state = m_state;

    impl->updating = true;

    try
    {
        MUXC::TextBlock textBlock;
        textBlock.Text(wxWinUIToHString(visibleLabel));

        const MUX::FlowDirection textDirection =
            isRTL ? MUX::FlowDirection::RightToLeft
                  : MUX::FlowDirection::LeftToRight;

        // wxALIGN_RIGHT is the absolute "label to the left of the glyph"
        // contract. In LTR this requires reversing the CheckBox layout while
        // restoring the label's own text direction. In RTL the natural
        // CheckBox layout already places the label on the left.
        peer.FlowDirection(
            isRTL || alignRight
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
        textBlock.FlowDirection(textDirection);

        peer.Content(textBlock);
        wxWinUIApplyAccessKey(peer, label);

        if ( state == wxCHK_UNDETERMINED )
            peer.IsChecked(nullptr);
        else
            peer.IsChecked(state == wxCHK_CHECKED);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox content", e);
        if ( wxCheckBox * const owner = getCurrentOwner() )
            owner->m_winui->updating = false;
        return false;
    }

    wxCheckBox * const owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->m_winui->updating = false;
    if ( !forceRender )
        return true;

    owner->m_winui->host.ForceRender();
    return getCurrentOwner() != nullptr;
}

bool wxCheckBox::UpdateWinUIAppearance(bool forceRender)
{
    if ( !m_winui || !m_winui->checkBox )
        return true;

    wxWinUICheckBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUICheckBoxCallbackState> callbackState =
        impl->callbackState;
    const MUXC::CheckBox peer = impl->checkBox;
    const auto getCurrentOwner = [callbackState, impl]() -> wxCheckBox *
    {
        wxCheckBox * const owner = callbackState
                                      ? callbackState->GetOwner()
                                      : nullptr;
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    const wxFont font = m_hasFont ? GetFont() : wxNullFont;
    const wxColour foreground =
        UseForegroundColour() ? GetForegroundColour() : wxNullColour;
    const wxColour background =
        UseBackgroundColour() ? GetBackgroundColour() : wxNullColour;
    const bool enabled = IsEnabled();

    try
    {
        wxWinUIApplyFont(peer, font);
        wxWinUIApplyForeground(peer, foreground);
        wxWinUIApplyBackground(peer, background);
        peer.IsEnabled(enabled);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox appearance", e);
        return false;
    }

    wxCheckBox * const owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( !forceRender )
        return true;

    owner->m_winui->host.ForceRender();
    return getCurrentOwner() != nullptr;
}

#endif // wxUSE_CHECKBOX
