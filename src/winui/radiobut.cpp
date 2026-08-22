/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/radiobut.cpp
// Purpose:     wxWinUI wxRadioButton implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBTN

#include "wx/radiobut.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/font.h"
#endif

#include "private.h"
#include "wx/winui/private/appearance.h"
#include "wx/weakref.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <memory>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

wxRadioButton *wxWinUIFindRadioGroupLeader(wxWindow *parent,
                                           wxRadioButton *needle)
{
    if ( !parent || !needle )
        return nullptr;

    wxRadioButton *leader = nullptr;
    for ( wxWindow * const child : parent->GetChildren() )
    {
        wxRadioButton * const button =
            wxDynamicCast(child, wxRadioButton);
        if ( !button )
            continue;

        if ( button->HasFlag(wxRB_SINGLE) )
        {
            if ( button == needle )
                return button;
            leader = nullptr;
            continue;
        }

        if ( !leader || button->HasFlag(wxRB_GROUP) )
            leader = button;
        if ( button == needle )
            return leader;
    }

    return nullptr;
}

} // anonymous namespace

class wxWinUIRadioButtonCallbackState final
{
public:
    explicit wxWinUIRadioButtonCallbackState(wxRadioButton *owner)
        : m_owner(owner)
    {
    }

    wxRadioButton *GetOwner() const { return m_owner; }
    void Invalidate() { m_owner = nullptr; }

private:
    wxRadioButton *m_owner;
};

class wxWinUIRadioButtonImpl
{
public:
    ~wxWinUIRadioButtonImpl()
    {
        Close();
    }

    void Close()
    {
        // An AutomationPeer may retain the XAML peer beyond the wx control.
        // Invalidate first so a failed revocation can never leave a live raw
        // owner pointer behind.
        if ( callbackState )
            callbackState->Invalidate();

        if ( radioButton && checkedToken.value )
        {
            try
            {
                radioButton.Checked(checkedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI RadioButton Checked removal", e);
            }
            checkedToken = {};
        }

        host.Close();
        radioButton = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState;
    MUXC::RadioButton radioButton{ nullptr };
    winrt::event_token checkedToken{};
    bool updating = false;
};

wxRadioButton::wxRadioButton()
    : m_isChecked(false)
{
}

wxRadioButton::wxRadioButton(wxWindow *parent,
                             wxWindowID id,
                             const wxString& label,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
    : wxRadioButton()
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxRadioButton::~wxRadioButton()
{
    // Invalidate retained XAML callbacks at destructor entry.
    m_winui.reset();
    UpdateGroupNames(GetParent(), this);
}

bool wxRadioButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIRadioButtonImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIRadioButtonCallbackState>(this);
    wxWinUIRadioButtonImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    try
    {
        m_winui->radioButton = MUXC::RadioButton();
        m_winui->checkedToken = m_winui->radioButton.Checked(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxRadioButton *owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->updating )
                {
                    return;
                }

                owner->m_isChecked = true;
                owner->ClearRadioGroup();

                // Clearing the group can call overridden sibling code.
                // Re-check the lifetime before dispatching the command event,
                // which itself may destroy owner (so nothing follows it).
                owner = callbackState->GetOwner();
                if ( owner && owner->m_winui &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->SendRadioEvent();
                }
            });

        if ( !UpdateWinUIContent(false) ||
             !UpdateWinUIAppearance(false) )
        {
            wxRadioButton * const owner = callbackState->GetOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == createImpl )
            {
                owner->m_winui.reset();
            }
            return false;
        }

        wxRadioButton *owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }

        const MUXC::RadioButton peer = createImpl->radioButton;
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

        owner->UpdateGroupNames(parent);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton creation", e);
        wxRadioButton * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxRadioButton *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    if ( owner->HasFlag(wxRB_GROUP) )
    {
        owner->SetValue(true);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }
    }

    return true;
}

void wxRadioButton::SetValue(bool value)
{
    if ( m_isChecked == value )
    {
        UpdateWinUIContent();
        return;
    }

    m_isChecked = value;
    if ( !UpdateWinUIContent() )
        return;

    if ( value && !HasFlag(wxRB_SINGLE) )
        ClearRadioGroup();
}

bool wxRadioButton::GetValue() const
{
    return m_isChecked;
}

void wxRadioButton::SetLabel(const wxString& label)
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

void wxRadioButton::Command(wxCommandEvent& event)
{
    wxWinUIRadioButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;

    SetValue(event.IsChecked());

    if ( callbackState )
    {
        wxRadioButton * const owner = callbackState->GetOwner();
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

bool wxRadioButton::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return rc;
}

bool wxRadioButton::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxRadioButton::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxRadioButton::MSWOnEffectiveLayoutDirectionChanged()
{
    return UpdateWinUIContent();
}

bool wxRadioButton::Reparent(wxWindowBase *newParent)
{
    const wxWeakRef<wxRadioButton> self(this);
    const wxWeakRef<wxWindow> oldParent(GetParent());
    const wxWeakRef<wxWindow> requestedParent(
        static_cast<wxWindow *>(newParent));
    if ( !wxControl::Reparent(newParent) )
        return false;

    // Group identity is defined by sibling order and wxRB_GROUP/SINGLE, not
    // by the XAML visual tree. A cross-parent move must therefore republish
    // the native GroupName for every affected sibling list. The common
    // reparent transaction ends with inherited RTL projection and can
    // synchronously delete or redirect this radio, so retain only weak
    // parents and never dereference this after the base call.
    const wxWeakRef<wxWindow> finalParent(
        self ? self->GetParent() : nullptr);
    if ( wxWindow * const parent = oldParent.get() )
        UpdateGroupNames(parent);
    if ( wxWindow * const parent = requestedParent.get();
         parent && parent != oldParent.get() )
    {
        UpdateGroupNames(parent);
    }
    if ( wxWindow * const parent = finalParent.get();
         parent && parent != oldParent.get() &&
         parent != requestedParent.get() )
    {
        UpdateGroupNames(parent);
    }
    return true;
}

#if wxUSE_TOOLTIPS
void wxRadioButton::DoSetToolTipText(const wxString& tip)
{
    wxControl::DoSetToolTipText(tip);
}

void wxRadioButton::DoSetToolTip(wxToolTip *tip)
{
    wxControl::DoSetToolTip(tip);
}
#endif // wxUSE_TOOLTIPS

void wxRadioButton::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);
    UpdateWinUIAppearance();
}

wxSize wxRadioButton::DoGetBestSize() const
{
    // Measure the actual WinUI radio button (glyph + label + padding).
    if ( m_winui )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size != wxDefaultSize )
            return size;
    }

    // Not realised yet: fall back to a guess big enough for the label, which
    // will be corrected once the content is loaded.
    wxSize best = wxWindow::FromDIP(wxSize(32, 32), const_cast<wxRadioButton *>(this));
    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( !text.empty() )
        best.x += GetTextExtent(text).x;

    return best;
}

bool wxRadioButton::UpdateWinUIContent(bool forceRender)
{
    if ( !m_winui || !m_winui->radioButton )
        return true;

    wxWinUIRadioButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::RadioButton peer = impl->radioButton;
    const auto getCurrentOwner = [callbackState, impl]() -> wxRadioButton *
    {
        wxRadioButton * const owner = callbackState
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
    const bool checked = m_isChecked;

    impl->updating = true;

    try
    {
        MUXC::TextBlock textBlock;
        textBlock.Text(wxWinUIToHString(visibleLabel));

        peer.FlowDirection(
            isRTL || alignRight
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
        textBlock.FlowDirection(
            isRTL ? MUX::FlowDirection::RightToLeft
                  : MUX::FlowDirection::LeftToRight);

        peer.Content(textBlock);
        wxWinUIApplyAccessKey(peer, label);
        peer.IsChecked(checked);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton content", e);
        if ( wxRadioButton * const owner = getCurrentOwner() )
            owner->m_winui->updating = false;
        return false;
    }

    wxRadioButton * const owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->m_winui->updating = false;
    if ( !forceRender )
        return true;

    owner->m_winui->host.ForceRender();
    return getCurrentOwner() != nullptr;
}

bool wxRadioButton::UpdateWinUIAppearance(bool forceRender)
{
    if ( !m_winui || !m_winui->radioButton )
        return true;

    wxWinUIRadioButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::RadioButton peer = impl->radioButton;
    const auto getCurrentOwner = [callbackState, impl]() -> wxRadioButton *
    {
        wxRadioButton * const owner = callbackState
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
        wxWinUILogException("WinUI RadioButton appearance", e);
        return false;
    }

    wxRadioButton * const owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( !forceRender )
        return true;

    owner->m_winui->host.ForceRender();
    return getCurrentOwner() != nullptr;
}

bool wxRadioButton::UpdateWinUIGroupName(wxRadioButton *groupLeader)
{
    if ( !m_winui || !m_winui->radioButton || !groupLeader )
        return true;

    wxWinUIRadioButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::RadioButton peer = impl->radioButton;
    const wxString groupName = wxString::Format(
        "wxRadioGroup_%p", static_cast<void *>(groupLeader));

    try
    {
        peer.GroupName(wxWinUIToHString(groupName));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton group update", e);
        return false;
    }

    wxRadioButton * const owner = callbackState
                                     ? callbackState->GetOwner()
                                     : nullptr;
    return owner && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == callbackState;
}

void wxRadioButton::UpdateGroupNames(wxWindowBase *parent,
                                     const wxRadioButton *ignored)
{
    if ( !parent )
        return;

    struct GroupEntry
    {
        wxWeakRef<wxRadioButton> button;
        wxWeakRef<wxRadioButton> leader;
    };
    std::vector<GroupEntry> entries;
    entries.reserve(parent->GetChildren().size());

    wxRadioButton *groupLeader = nullptr;
    for ( wxWindowList::compatibility_iterator node =
              parent->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxRadioButton * const button =
            wxDynamicCast(node->GetData(), wxRadioButton);
        if ( !button || button == ignored )
            continue;

        if ( button->HasFlag(wxRB_SINGLE) )
        {
            entries.push_back({ button, button });
            // The radio after a SINGLE starts a new implicit group.
            groupLeader = nullptr;
            continue;
        }

        if ( !groupLeader || button->HasFlag(wxRB_GROUP) )
            groupLeader = button;

        entries.push_back({ button, groupLeader });
    }

    // GroupName is a XAML setter and may re-enter application code. Never
    // retain a wxWindowList iterator or raw sibling across that boundary.
    for ( const GroupEntry& entry : entries )
    {
        wxRadioButton * const button = entry.button.get();
        wxRadioButton * const leader = entry.leader.get();
        if ( button && leader )
            button->UpdateWinUIGroupName(leader);
    }
}

void wxRadioButton::ClearRadioGroup()
{
    wxWindow * const parent = GetParent();
    if ( !parent || HasFlag(wxRB_SINGLE) || !m_winui )
        return;

    wxWinUIRadioButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioButtonCallbackState> callbackState =
        impl->callbackState;
    const wxWeakRef<wxWindow> weakParent(parent);
    const wxWeakRef<wxRadioButton> weakSelf(this);

    // Snapshot weak candidates before the first SetValue(). ForceRender()
    // inside it can destroy/reparent/reorder any sibling and invalidates every
    // live wxWindowList iterator.
    std::vector<wxWeakRef<wxRadioButton>> candidates;
    candidates.reserve(parent->GetChildren().size());
    for ( wxWindow * const child : parent->GetChildren() )
    {
        wxRadioButton * const button =
            wxDynamicCast(child, wxRadioButton);
        if ( button && button != this )
            candidates.emplace_back(button);
    }

    const auto getCurrentOwner =
        [callbackState, impl, weakSelf]() -> wxRadioButton *
        {
            wxRadioButton * const owner = callbackState
                                             ? callbackState->GetOwner()
                                             : nullptr;
            return owner && owner == weakSelf.get() &&
                           owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                       ? owner
                       : nullptr;
        };

    for ( const wxWeakRef<wxRadioButton>& weakCandidate : candidates )
    {
        wxRadioButton * const owner = getCurrentOwner();
        wxWindow * const liveParent = weakParent.get();
        wxRadioButton * const candidate = weakCandidate.get();
        if ( !owner || !liveParent )
            return;
        if ( !candidate || candidate->GetParent() != liveParent ||
             owner->GetParent() != liveParent )
        {
            continue;
        }

        // Re-evaluate group identity after each preceding callback: a sibling
        // moved across a wxRB_GROUP boundary must not be cleared by a stale
        // pre-callback snapshot.
        wxRadioButton * const ownerLeader =
            wxWinUIFindRadioGroupLeader(liveParent, owner);
        wxRadioButton * const candidateLeader =
            wxWinUIFindRadioGroupLeader(liveParent, candidate);
        if ( !ownerLeader || ownerLeader != candidateLeader )
            continue;

        candidate->SetValue(false);

        // SetValue() ends in ForceRender(). Do not touch this, the parent or
        // the next candidate until the exact owner+impl has been revalidated.
        if ( !getCurrentOwner() )
            return;
    }
}

void wxRadioButton::SendRadioEvent()
{
    wxCommandEvent event(wxEVT_RADIOBUTTON, GetId());
    event.SetEventObject(this);
    event.SetInt(m_isChecked);
    ProcessCommand(event);
}

#endif // wxUSE_RADIOBTN
