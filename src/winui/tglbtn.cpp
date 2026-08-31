/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/tglbtn.cpp
// Purpose:     wxWinUI wxToggleButton implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"
#include "wx/bitmap.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "button-test-access.h"
#endif

#include "private.h"
#include "wx/winui/private/appearance.h"

#include <winrt/Windows.UI.Text.h>

#include <cmath>
#include <exception>
#include <memory>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
MUXC::Image wxWinUIFindToggleImage(
    const winrt::Windows::Foundation::IInspectable& content)
{
    if ( !content )
        return nullptr;

    if ( const auto image = content.try_as<MUXC::Image>() )
        return image;

    if ( const auto panel = content.try_as<MUXC::Panel>() )
    {
        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( const auto image =
                     wxWinUIFindToggleImage(children.GetAt(i)) )
            {
                return image;
            }
        }
    }

    return nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

enum class wxWinUIToggleProjectionState
{
    Applied,
    Restart,
    Dead,
    Failed
};

constexpr unsigned wxWinUIMaxToggleProjectionPasses = 8;
constexpr double wxWinUIToggleMinProjectionScale = 1.0 / 64.0;
constexpr double wxWinUIToggleMaxProjectionScale = 64.0;

thread_local std::vector<const wxBitmapToggleButton *>
    gs_bitmapToggleGetterStack;

template <typename F>
void wxWinUIToggleRunVisualNoexcept(F&& operation) noexcept
{
    try
    {
        operation();
    }
    catch ( const std::exception& e )
    {
        try
        {
            wxLogWarning(
                "wxWinUI: ToggleButton visual synchronization failed: %s",
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
        }
    }
    catch ( ... )
    {
        try
        {
            wxLogWarning(
                "wxWinUI: ToggleButton visual synchronization failed with "
                "an unknown exception.");
        }
        catch ( ... )
        {
        }
    }
}

} // anonymous namespace

class wxWinUIToggleButtonCallbackState final
{
public:
    explicit wxWinUIToggleButtonCallbackState(wxToggleButton *owner)
        : m_owner(owner)
    {
    }

    wxToggleButton *GetOwner() const { return m_owner; }
    void Invalidate() { m_owner = nullptr; }

private:
    wxToggleButton *m_owner;
};

class wxWinUIToggleButtonImpl
{
public:
    ~wxWinUIToggleButtonImpl()
    {
        Close();
    }

    void Close()
    {
        // The XAML peer can outlive the wx control through an AutomationPeer.
        // Make every retained delegate harmless before attempting revocation:
        // token removal is best-effort and can itself run XAML code.
        if ( callbackState )
            callbackState->Invalidate();

        if ( button )
        {
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
                   [peer = button](winrt::event_token token)
                   {
                       peer.Checked(token);
                   },
                   "WinUI ToggleButton Checked removal");
            revoke(uncheckedToken,
                   [peer = button](winrt::event_token token)
                   {
                       peer.Unchecked(token);
                   },
                   "WinUI ToggleButton Unchecked removal");
            revoke(pointerEnteredToken,
                   [peer = button](winrt::event_token token)
                   {
                       peer.PointerEntered(token);
                   },
                   "WinUI ToggleButton PointerEntered removal");
            revoke(pointerExitedToken,
                   [peer = button](winrt::event_token token)
                   {
                       peer.PointerExited(token);
                   },
                   "WinUI ToggleButton PointerExited removal");
            revoke(gotFocusToken,
                   [peer = button](winrt::event_token token)
                   {
                       peer.GotFocus(token);
                   },
                   "WinUI ToggleButton GotFocus removal");
            revoke(lostFocusToken,
                   [peer = button](winrt::event_token token)
                   {
                       peer.LostFocus(token);
                   },
                   "WinUI ToggleButton LostFocus removal");
        }

        host.Close();
        button = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState;
    winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton button{ nullptr };
    winrt::event_token checkedToken{};
    winrt::event_token uncheckedToken{};
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token gotFocusToken{};
    winrt::event_token lostFocusToken{};
    bool updating = false;
    bool hovered = false;
    bool focused = false;
    wxAnyButton::State projectedBitmapState =
        wxAnyButton::State_Normal;
    // A bundle implementation and every XAML property setter are synchronous
    // application-code boundaries. Nested updates only publish a newer
    // revision: the outer driver consumes it without recursive stack growth.
    bool contentProjectionInProgress = false;
    bool contentProjectionRequested = false;
    bool contentProjectionReplayScheduled = false;
    bool contentProjectionReplayEntryPending = false;
    bool contentProjectionDeferredUsed = false;
    bool pendingForceRender = false;
    wxAnyButton::State pendingForcedState = wxAnyButton::State_Max;
    double pendingRequestedScale = 0.0;
    std::uint64_t contentReplayTicket = 0;
    std::uint64_t contentRequestRevision = 0;
    std::uint64_t contentGeneration = 0;
    bool appearanceProjectionInProgress = false;
    bool appearanceProjectionRequested = false;
    bool appearanceProjectionReplayScheduled = false;
    bool appearanceProjectionReplayEntryPending = false;
    bool appearanceProjectionDeferredUsed = false;
    bool pendingAppearanceForceRender = false;
    std::uint64_t appearanceReplayTicket = 0;
    std::uint64_t appearanceRequestRevision = 0;
    wxSize normalBitmapLogicalSize;
    bool normalBitmapLogicalSizeValid = false;
};

wxDEFINE_EVENT( wxEVT_TOGGLEBUTTON, wxCommandEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxToggleButton);

wxToggleButton::wxToggleButton()
{
}

wxToggleButton::wxToggleButton(wxWindow *parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxToggleButton::~wxToggleButton()
{
    Unbind(wxEVT_DPI_CHANGED, &wxToggleButton::OnDPIChanged, this);
    // Tear down XAML callbacks before any other derived member disappears.
    m_winui.reset();
}

bool wxToggleButton::Create(wxWindow *parent,
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

    m_winui.reset(new wxWinUIToggleButtonImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIToggleButtonCallbackState>(this);
    wxWinUIToggleButtonImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    try
    {
        m_winui->button =
            winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton();
        const auto handler =
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                wxToggleButton *owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->updating )
                {
                    return;
                }

                try
                {
                    if ( auto checked = owner->m_winui->button.IsChecked() )
                        owner->m_state = checked.Value();
                    else
                        owner->m_state = false;
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ToggleButton state read", e);
                    return;
                }

                // A rendering failure must not suppress the semantic toggle
                // notification: IsChecked and m_state have already changed.
                // UpdateWinUIContent() still performs all lifetime checks, and
                // callbackState is the authority before emitting the event.
                // A user wxBitmapBundle implementation is an ordinary C++
                // boundary. Never let its rendering failure escape the WinRT
                // delegate or suppress the semantic toggle event.
                wxWinUIToggleRunVisualNoexcept(
                    [owner]() { owner->UpdateWinUIContent(); });

                owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }

                // The command event can destroy owner. Nothing may follow it.
                owner->SendToggleEvent();
            };
        // Observe the semantic state transition instead of Click. This covers
        // mouse/keyboard activation and the native UIA Toggle provider alike,
        // while SetValue() remains silent under the updating guard.
        m_winui->checkedToken = m_winui->button.Checked(handler);
        m_winui->uncheckedToken = m_winui->button.Unchecked(handler);
        m_winui->pointerEnteredToken = m_winui->button.PointerEntered(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                wxToggleButton * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                owner->m_winui->hovered = true;
                wxWinUIToggleRunVisualNoexcept(
                    [owner]() { owner->UpdateWinUIContent(); });
            });
        m_winui->pointerExitedToken = m_winui->button.PointerExited(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                wxToggleButton * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                owner->m_winui->hovered = false;
                wxWinUIToggleRunVisualNoexcept(
                    [owner]() { owner->UpdateWinUIContent(); });
            });
        m_winui->gotFocusToken = m_winui->button.GotFocus(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxToggleButton * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                owner->m_winui->focused = true;
                wxWinUIToggleRunVisualNoexcept(
                    [owner]() { owner->UpdateWinUIContent(); });
            });
        m_winui->lostFocusToken = m_winui->button.LostFocus(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxToggleButton * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                owner->m_winui->focused = false;
                wxWinUIToggleRunVisualNoexcept(
                    [owner]() { owner->UpdateWinUIContent(); });
            });

        if ( !UpdateWinUIContent(false) ||
             !UpdateWinUIAppearance(false) )
        {
            wxToggleButton * const owner = callbackState->GetOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == createImpl )
            {
                owner->m_winui.reset();
            }
            return false;
        }

        wxToggleButton *owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }

        const auto peer = createImpl->button;
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

        owner->SetInitialSize(size);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ToggleButton creation", e);
        wxToggleButton * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxToggleButton * const owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }
    owner->Bind(
        wxEVT_DPI_CHANGED, &wxToggleButton::OnDPIChanged, owner);

    return true;
}

void wxToggleButton::SetValue(bool value)
{
    m_state = value;
    if ( !m_winui || !m_winui->button )
        return;

    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        impl->callbackState;
    const auto peer = impl->button;
    impl->updating = true;
    try
    {
        peer.IsChecked(value);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ToggleButton state update", e);
        wxToggleButton * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == impl &&
             owner->m_winui->callbackState == callbackState )
        {
            owner->m_winui->updating = false;
        }
        return;
    }

    wxToggleButton * const owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    owner->m_winui->updating = false;
    owner->UpdateWinUIContent();
}

bool wxToggleButton::GetValue() const
{
    return m_state;
}

void wxToggleButton::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    if ( !UpdateWinUIContent(false) )
        return;
    UpdateWinUIAppearance();
}

void wxToggleButton::Command(wxCommandEvent& event)
{
    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;

    // Command delivery is semantic and must not be lost because a custom
    // bitmap bundle failed while synchronizing the visual state.
    wxWinUIToggleRunVisualNoexcept(
        [this, &event]() { SetValue(event.GetInt() != 0); });

    if ( callbackState )
    {
        wxToggleButton * const owner = callbackState->GetOwner();
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

bool wxToggleButton::SetFont(const wxFont& font)
{
    const bool changed = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return changed;
}

bool wxToggleButton::SetForegroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return changed;
}

bool wxToggleButton::SetBackgroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return changed;
}

bool wxToggleButton::MSWOnEffectiveLayoutDirectionChanged()
{
    return UpdateWinUIAppearance();
}

void wxToggleButton::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);
    if ( !UpdateWinUIContent(false) )
        return;
    UpdateWinUIAppearance();
}

void wxToggleButton::OnDPIChanged(wxDPIChangedEvent& event)
{
    // This is a no-op for text toggles, but virtual dispatch rebuilds a
    // wxBitmapToggleButton from the bundle at the new control scale.
    event.Skip();
    InvalidateBestSize();
    wxWinUIToggleRunVisualNoexcept(
        [this]() { UpdateWinUIContent(); });
}

wxSize wxToggleButton::DoGetBestSize() const
{
    const wxSize defaultSize =
        wxButtonBase::GetDefaultSize(const_cast<wxToggleButton *>(this));

    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( text.empty() )
        return defaultSize;

    // Measure using the same explicit/default font as the live peer.
    wxSize best = wxWinUIMeasureText(
        this, text, m_hasFont ? GetFont() : wxNullFont);

    best.x += FromDIP(28);
    best.y += FromDIP(14);

    best.IncTo(defaultSize);
    return best;
}

void wxToggleButton::SendToggleEvent()
{
    wxCommandEvent event(wxEVT_TOGGLEBUTTON, GetId());
    event.SetInt(m_state ? 1 : 0);
    event.SetEventObject(this);
    ProcessCommand(event);
}

bool wxToggleButton::UpdateWinUIContent(bool forceRender)
{
    if ( !m_winui || !m_winui->button )
        return true;

    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        impl->callbackState;
    const auto peer = impl->button;

    const bool replayEntry =
        impl->contentProjectionReplayEntryPending &&
        !impl->contentProjectionInProgress;
    if ( replayEntry )
    {
        impl->contentProjectionReplayEntryPending = false;
        impl->contentProjectionRequested = true;
    }
    else
    {
        const bool externalRequest =
            !impl->contentProjectionInProgress;
        if ( externalRequest )
        {
            // A real mutation supersedes a queued replay and owns a fresh
            // synchronous-plus-one-deferred budget.
            impl->contentProjectionDeferredUsed = false;
            impl->contentProjectionReplayScheduled = false;
            if ( ++impl->contentReplayTicket == 0 )
                ++impl->contentReplayTicket;
            impl->pendingForceRender = forceRender;
        }
        else
        {
            impl->pendingForceRender =
                impl->pendingForceRender || forceRender;
        }

        if ( ++impl->contentRequestRevision == 0 )
            ++impl->contentRequestRevision;
        impl->contentProjectionRequested = true;
    }

    if ( impl->contentProjectionInProgress )
        return true;

    impl->contentProjectionInProgress = true;
    wxScopeGuard projectionGuard =
        wxMakeGuard(
            [callbackState, impl]()
            {
                // This guard also runs for arbitrary exceptions thrown by
                // application overrides. Never dereference impl unless its
                // original owner still publishes it.
                wxToggleButton * const owner =
                    callbackState ? callbackState->GetOwner() : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->contentProjectionInProgress = false;
                }
            });
    wxUnusedVar(projectionGuard);

    const auto getLiveOwner =
        [callbackState, impl]() -> wxToggleButton *
        {
            wxToggleButton * const owner =
                callbackState ? callbackState->GetOwner() : nullptr;
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                       ? owner
                       : nullptr;
        };

    for ( unsigned pass = 0;
          pass < wxWinUIMaxToggleProjectionPasses;
          ++pass )
    {
        wxToggleButton * const live = getLiveOwner();
        if ( !live )
            return false;

        impl->contentProjectionRequested = false;
        const std::uint64_t requestRevision =
            impl->contentRequestRevision;
        const bool passForceRender = impl->pendingForceRender;
        const auto getRequestState =
            [getLiveOwner, requestRevision]()
                -> wxWinUIToggleProjectionState
            {
                wxToggleButton * const owner = getLiveOwner();
                if ( !owner )
                    return wxWinUIToggleProjectionState::Dead;
                return owner->m_winui->contentRequestRevision ==
                               requestRevision
                           ? wxWinUIToggleProjectionState::Applied
                           : wxWinUIToggleProjectionState::Restart;
            };

        // GetLabel() is virtual application code. Publish the revision first
        // and validate it before doing any subsequent wx-model read.
        const wxString label = live->GetLabel();
        wxWinUIToggleProjectionState projection =
            getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        const wxString visibleLabel =
            wxControl::GetLabelText(label);
        const winrt::hstring accessKey = wxWinUIToHString(
            wxWinUIParseLabel(label).accessKey);

        projection = wxWinUIToggleProjectionState::Failed;
        try
        {
            MUXC::TextBlock textBlock;
            textBlock.Text(wxWinUIToHString(visibleLabel));
            peer.Content(textBlock);
            projection = getRequestState();

            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                // wxWinUIApplyAccessKey() deliberately writes two dependency
                // properties. Revalidate between them so a nested SetLabel()
                // cannot leave UIElement::AccessKey stale.
                MUXA::AutomationProperties::SetAccessKey(
                    peer, accessKey);
                projection = getRequestState();
            }
            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                peer.AccessKey(accessKey);
                projection = getRequestState();
            }

            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                wxToggleButton *owner = getLiveOwner();
                if ( !owner )
                {
                    projection = wxWinUIToggleProjectionState::Dead;
                }
                else
                {
                    ++owner->m_winui->contentGeneration;
                    if ( owner->m_winui->contentGeneration == 0 )
                        ++owner->m_winui->contentGeneration;

                    if ( passForceRender )
                    {
                        owner->m_winui->host.ForceRender();
                        projection = getRequestState();
                    }
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ToggleButton content", e);
            const wxWinUIToggleProjectionState current =
                getRequestState();
            projection =
                current == wxWinUIToggleProjectionState::Applied
                    ? wxWinUIToggleProjectionState::Failed
                    : current;
        }

        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        wxToggleButton * const owner = getLiveOwner();
        if ( !owner )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Failed )
        {
            owner->m_winui->contentProjectionInProgress = false;
            owner->m_winui->contentProjectionRequested = false;
            owner->m_winui->pendingForceRender = false;
            return false;
        }
        if ( owner->m_winui->contentProjectionRequested ||
             owner->m_winui->contentRequestRevision != requestRevision )
        {
            continue;
        }

        owner->m_winui->contentProjectionInProgress = false;
        owner->m_winui->pendingForceRender = false;
        return true;
    }

    wxToggleButton * const owner = getLiveOwner();
    if ( !owner )
        return false;

    owner->m_winui->contentProjectionInProgress = false;
    owner->m_winui->contentProjectionRequested = true;
    if ( !owner->m_winui->contentProjectionDeferredUsed && wxTheApp )
    {
        owner->m_winui->contentProjectionDeferredUsed = true;
        owner->m_winui->contentProjectionReplayScheduled = true;
        if ( ++owner->m_winui->contentReplayTicket == 0 )
            ++owner->m_winui->contentReplayTicket;
        const std::uint64_t ticket =
            owner->m_winui->contentReplayTicket;
        const std::weak_ptr<wxWinUIToggleButtonCallbackState> weakState(
            callbackState);

        wxTheApp->CallAfter(
            [weakState, impl, ticket]()
            {
                const auto state = weakState.lock();
                if ( !state )
                    return;

                wxToggleButton * const owner = state->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     !owner->m_winui->contentProjectionReplayScheduled ||
                     owner->m_winui->contentReplayTicket != ticket )
                {
                    return;
                }

                owner->m_winui->contentProjectionReplayScheduled = false;
                owner->m_winui->contentProjectionReplayEntryPending = true;
                const bool replayForce =
                    owner->m_winui->pendingForceRender;
                owner->UpdateWinUIContent(replayForce);

                wxToggleButton * const current = state->GetOwner();
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == state )
                {
                    current->m_winui
                        ->contentProjectionReplayEntryPending = false;
                }
            });
        return true;
    }

    // A pathological callback also exhausted the sole deferred replay.
    // Quarantine it until the next explicit mutation rearms the budget.
    owner->m_winui->contentProjectionRequested = false;
    owner->m_winui->pendingForceRender = false;
    return false;
}

bool wxToggleButton::UpdateWinUIAppearance(bool forceRender)
{
    if ( !m_winui || !m_winui->button )
        return true;

    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        impl->callbackState;
    const auto peer = impl->button;

    const bool replayEntry =
        impl->appearanceProjectionReplayEntryPending &&
        !impl->appearanceProjectionInProgress;
    if ( replayEntry )
    {
        impl->appearanceProjectionReplayEntryPending = false;
        impl->appearanceProjectionRequested = true;
    }
    else
    {
        const bool externalRequest =
            !impl->appearanceProjectionInProgress;
        if ( externalRequest )
        {
            impl->appearanceProjectionDeferredUsed = false;
            impl->appearanceProjectionReplayScheduled = false;
            if ( ++impl->appearanceReplayTicket == 0 )
                ++impl->appearanceReplayTicket;
            impl->pendingAppearanceForceRender = forceRender;
        }
        else
        {
            impl->pendingAppearanceForceRender =
                impl->pendingAppearanceForceRender || forceRender;
        }

        if ( ++impl->appearanceRequestRevision == 0 )
            ++impl->appearanceRequestRevision;
        impl->appearanceProjectionRequested = true;
    }

    if ( impl->appearanceProjectionInProgress )
        return true;

    impl->appearanceProjectionInProgress = true;
    wxScopeGuard projectionGuard =
        wxMakeGuard(
            [callbackState, impl]()
            {
                wxToggleButton * const owner =
                    callbackState ? callbackState->GetOwner() : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->appearanceProjectionInProgress =
                        false;
                }
            });
    wxUnusedVar(projectionGuard);

    const auto getLiveOwner =
        [callbackState, impl]() -> wxToggleButton *
        {
            wxToggleButton * const owner =
                callbackState ? callbackState->GetOwner() : nullptr;
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                       ? owner
                       : nullptr;
        };

    for ( unsigned pass = 0;
          pass < wxWinUIMaxToggleProjectionPasses;
          ++pass )
    {
        wxToggleButton * const live = getLiveOwner();
        if ( !live )
            return false;

        impl->appearanceProjectionRequested = false;
        const std::uint64_t requestRevision =
            impl->appearanceRequestRevision;
        const bool passForceRender =
            impl->pendingAppearanceForceRender;
        const auto getRequestState =
            [getLiveOwner, requestRevision]()
                -> wxWinUIToggleProjectionState
            {
                wxToggleButton * const owner = getLiveOwner();
                if ( !owner )
                    return wxWinUIToggleProjectionState::Dead;
                return owner->m_winui->appearanceRequestRevision ==
                               requestRevision
                           ? wxWinUIToggleProjectionState::Applied
                           : wxWinUIToggleProjectionState::Restart;
            };

        const wxFont font =
            live->m_hasFont ? live->GetFont() : wxNullFont;
        const wxColour foreground =
            live->UseForegroundColour()
                ? live->GetForegroundColour()
                : wxNullColour;
        const wxColour background =
            live->UseBackgroundColour()
                ? live->GetBackgroundColour()
                : wxNullColour;
        const bool enabled = live->IsEnabled();

        // GetLayoutDirection() is virtual application code. It is the final
        // model read and must be followed by a revision/lifetime check before
        // the first peer write.
        const MUX::FlowDirection flow =
            live->GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight;
        wxWinUIToggleProjectionState projection =
            getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        projection = wxWinUIToggleProjectionState::Failed;
        try
        {
            // wxWinUIApplyFont() writes four dependency properties. Spell
            // them out here so every synchronous callback boundary can be
            // revalidated independently.
            if ( font.IsOk() )
            {
                const wxString face = font.GetFaceName();
                if ( face.empty() )
                {
                    peer.ClearValue(
                        MUXC::Control::FontFamilyProperty());
                }
                else
                {
                    peer.FontFamily(
                        MUXM::FontFamily(wxWinUIToHString(face)));
                }
                projection = getRequestState();

                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    const double pointSize =
                        font.GetFractionalPointSize();
                    if ( pointSize > 0.0 )
                    {
                        peer.FontSize(pointSize * 96.0 / 72.0);
                    }
                    else
                    {
                        peer.ClearValue(
                            MUXC::Control::FontSizeProperty());
                    }
                    projection = getRequestState();
                }

                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    winrt::Windows::UI::Text::FontWeight weight{};
                    weight.Weight = static_cast<std::uint16_t>(
                        font.GetNumericWeight());
                    peer.FontWeight(weight);
                    projection = getRequestState();
                }

                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    winrt::Windows::UI::Text::FontStyle style =
                        winrt::Windows::UI::Text::FontStyle::Normal;
                    switch ( font.GetStyle() )
                    {
                        case wxFONTSTYLE_ITALIC:
                            style =
                                winrt::Windows::UI::Text::FontStyle::
                                    Italic;
                            break;

                        case wxFONTSTYLE_SLANT:
                            style =
                                winrt::Windows::UI::Text::FontStyle::
                                    Oblique;
                            break;

                        case wxFONTSTYLE_NORMAL:
                        default:
                            break;
                    }
                    peer.FontStyle(style);
                    projection = getRequestState();
                }
            }
            else
            {
                peer.ClearValue(MUXC::Control::FontFamilyProperty());
                projection = getRequestState();
                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    peer.ClearValue(
                        MUXC::Control::FontSizeProperty());
                    projection = getRequestState();
                }
                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    peer.ClearValue(
                        MUXC::Control::FontWeightProperty());
                    projection = getRequestState();
                }
                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    peer.ClearValue(
                        MUXC::Control::FontStyleProperty());
                    projection = getRequestState();
                }
            }

            if ( projection ==
                 wxWinUIToggleProjectionState::Applied )
            {
                wxWinUIApplyForeground(peer, foreground);
                projection = getRequestState();
            }
            if ( projection ==
                 wxWinUIToggleProjectionState::Applied )
            {
                wxWinUIApplyBackground(peer, background);
                projection = getRequestState();
            }
            if ( projection ==
                 wxWinUIToggleProjectionState::Applied )
            {
                peer.FlowDirection(flow);
                projection = getRequestState();
            }
            if ( projection ==
                 wxWinUIToggleProjectionState::Applied )
            {
                peer.IsEnabled(enabled);
                projection = getRequestState();
            }

            if ( projection ==
                     wxWinUIToggleProjectionState::Applied &&
                 passForceRender )
            {
                wxToggleButton * const owner = getLiveOwner();
                if ( !owner )
                {
                    projection = wxWinUIToggleProjectionState::Dead;
                }
                else
                {
                    owner->m_winui->host.ForceRender();
                    projection = getRequestState();
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ToggleButton appearance", e);
            const wxWinUIToggleProjectionState current =
                getRequestState();
            projection =
                current == wxWinUIToggleProjectionState::Applied
                    ? wxWinUIToggleProjectionState::Failed
                    : current;
        }

        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        wxToggleButton * const owner = getLiveOwner();
        if ( !owner )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Failed )
        {
            owner->m_winui->appearanceProjectionInProgress = false;
            owner->m_winui->appearanceProjectionRequested = false;
            owner->m_winui->pendingAppearanceForceRender = false;
            return false;
        }
        if ( owner->m_winui->appearanceProjectionRequested ||
             owner->m_winui->appearanceRequestRevision !=
                 requestRevision )
        {
            continue;
        }

        owner->m_winui->appearanceProjectionInProgress = false;
        owner->m_winui->pendingAppearanceForceRender = false;
        return true;
    }

    wxToggleButton * const owner = getLiveOwner();
    if ( !owner )
        return false;

    owner->m_winui->appearanceProjectionInProgress = false;
    owner->m_winui->appearanceProjectionRequested = true;
    if ( !owner->m_winui->appearanceProjectionDeferredUsed &&
         wxTheApp )
    {
        owner->m_winui->appearanceProjectionDeferredUsed = true;
        owner->m_winui->appearanceProjectionReplayScheduled = true;
        if ( ++owner->m_winui->appearanceReplayTicket == 0 )
            ++owner->m_winui->appearanceReplayTicket;
        const std::uint64_t ticket =
            owner->m_winui->appearanceReplayTicket;
        const std::weak_ptr<wxWinUIToggleButtonCallbackState> weakState(
            callbackState);

        wxTheApp->CallAfter(
            [weakState, impl, ticket]()
            {
                const auto state = weakState.lock();
                if ( !state )
                    return;

                wxToggleButton * const owner = state->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     !owner->m_winui
                          ->appearanceProjectionReplayScheduled ||
                     owner->m_winui->appearanceReplayTicket != ticket )
                {
                    return;
                }

                owner->m_winui
                    ->appearanceProjectionReplayScheduled = false;
                owner->m_winui
                    ->appearanceProjectionReplayEntryPending = true;
                const bool replayForce =
                    owner->m_winui->pendingAppearanceForceRender;
                owner->UpdateWinUIAppearance(replayForce);

                wxToggleButton * const current = state->GetOwner();
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == state )
                {
                    current->m_winui
                        ->appearanceProjectionReplayEntryPending = false;
                }
            });
        return true;
    }

    owner->m_winui->appearanceProjectionRequested = false;
    owner->m_winui->pendingAppearanceForceRender = false;
    return false;
}

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------

wxBitmapToggleButton::~wxBitmapToggleButton()
{
    m_bitmapDestroying = true;
    // Bundle implementations are application-owned and their destructors may
    // reenter through a retained XAML peer. Revoke/invalidate all delegates
    // before m_bitmaps begins its implicit member destruction.
    m_winui.reset();
}

bool wxBitmapToggleButton::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxBitmapBundle& label,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    if ( m_bitmapDestroying )
        return false;

    // Store the bitmap first so that the base Create's UpdateWinUIContent
    // call (virtual) already renders it.
    wxBitmapBundle retainedBitmaps[State_Max];
    for ( unsigned state = 0; state < State_Max; ++state )
        retainedBitmaps[state] = m_bitmaps[state];
    const wxBitmapBundle labelCopy(label);

    if ( ++m_bitmapRevision == 0 )
        ++m_bitmapRevision;
    for ( wxBitmapBundle& bitmap : m_bitmaps )
        bitmap = wxBitmapBundle();
    m_bitmaps[State_Normal] = labelCopy;
    m_bitmapMargins = wxSize(0, 0);
    m_bitmapPosition = wxLEFT;

    return wxToggleButton::Create(parent, id, wxString(), pos, size, style,
                                  validator, name);
}

void wxBitmapToggleButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET( which >= State_Normal && which < State_Max,
                 wxT("invalid toggle button bitmap state") );
    if ( m_bitmapDestroying )
        return;

    // Keep every replaced implementation alive until the last owner access in
    // this method. An application-owned bundle destructor may delete/reenter
    // this control, but it will then run only while returning from the method.
    wxBitmapBundle retainedBitmaps[State_Max];
    for ( unsigned state = 0; state < State_Max; ++state )
        retainedBitmaps[state] = m_bitmaps[state];
    const wxBitmapBundle replacement(bitmap);

    if ( ++m_bitmapRevision == 0 )
        ++m_bitmapRevision;

    if ( which == State_Normal && m_winui )
        m_winui->normalBitmapLogicalSizeValid = false;

    if ( which == State_Normal && !replacement.IsOk() )
    {
        for ( wxBitmapBundle& stateBitmap : m_bitmaps )
            stateBitmap = wxBitmapBundle();
    }
    else
    {
        m_bitmaps[which] = replacement;
        if ( which == State_Focused && replacement.IsOk() &&
             !m_bitmaps[State_Current].IsOk() )
        {
            m_bitmaps[State_Current] = replacement;
        }
    }

    InvalidateBestSize();
    UpdateWinUIContent();
}

wxBitmap wxBitmapToggleButton::DoGetBitmap(State which) const
{
    return GetBitmapForState(which);
}

wxSize wxBitmapToggleButton::DoGetBitmapMargins() const
{
    if ( m_bitmapDestroying )
        return wxSize(0, 0);

    return m_bitmapMargins;
}

void wxBitmapToggleButton::DoSetBitmapMargins(wxCoord x, wxCoord y)
{
    if ( m_bitmapDestroying )
        return;

    m_bitmapMargins = wxSize(x, y);
    InvalidateBestSize();
    UpdateWinUIContent();
}

void wxBitmapToggleButton::DoSetBitmapPosition(wxDirection dir)
{
    if ( m_bitmapDestroying )
        return;

    m_bitmapPosition = dir;
    InvalidateBestSize();
    UpdateWinUIContent();
}

wxSize wxBitmapToggleButton::DoGetBestSize() const
{
    if ( m_bitmapDestroying )
    {
        return wxButtonBase::GetDefaultSize(
            const_cast<wxBitmapToggleButton *>(this));
    }

    // wxWindowBase keeps using this object after DoGetBestSize() returns while
    // caching the result. Never enter an application-owned bundle callback
    // here: the content transaction publishes this validated logical size.
    if ( !m_winui || !m_winui->normalBitmapLogicalSizeValid )
        return wxToggleButton::DoGetBestSize();

    const wxString text =
        wxControl::GetLabelText(GetLabel());
    const wxSize textSize =
        text.empty()
            ? wxSize()
            : wxWinUIMeasureText(
                  this, text, m_hasFont ? GetFont() : wxNullFont);

    wxSize image = FromDIP(m_winui->normalBitmapLogicalSize);
    image.x += 2 * m_bitmapMargins.x;
    image.y += 2 * m_bitmapMargins.y;
    const int gap = text.empty() ? 0 : FromDIP(6);

    wxSize best;
    if ( text.empty() )
    {
        best = image;
    }
    else if ( m_bitmapPosition == wxTOP ||
              m_bitmapPosition == wxBOTTOM )
    {
        best = wxSize(wxMax(image.x, textSize.x),
                      image.y + textSize.y + gap);
    }
    else
    {
        best = wxSize(image.x + textSize.x + gap,
                      wxMax(image.y, textSize.y));
    }

    best.x += FromDIP(24);
    best.y += FromDIP(14);
    return best;
}

wxBitmap wxBitmapToggleButton::GetBitmapForState(
    State which,
    double requestedScale) const
{
    wxCHECK_MSG( which >= State_Normal && which < State_Max, wxBitmap(),
                 wxT("invalid toggle button bitmap state") );

    if ( m_bitmapDestroying || !m_bitmaps[which].IsOk() )
        return wxBitmap();

    for ( const wxBitmapToggleButton * const active :
          gs_bitmapToggleGetterStack )
    {
        if ( active == this )
            return wxBitmap();
    }
    gs_bitmapToggleGetterStack.push_back(this);
    wxScopeGuard getterGuard =
        wxMakeGuard(
            []()
            {
                wxASSERT(!gs_bitmapToggleGetterStack.empty());
                gs_bitmapToggleGetterStack.pop_back();
            });
    wxUnusedVar(getterGuard);

    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        m_winui ? m_winui->callbackState : nullptr;
    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const wxWeakRef<wxWindow> lifetime(
        const_cast<wxBitmapToggleButton *>(this));

    for ( unsigned pass = 0;
          pass < wxWinUIMaxToggleProjectionPasses;
          ++pass )
    {
        wxWindow * const liveWindow = lifetime.get();
        wxBitmapToggleButton *owner =
            liveWindow
                ? static_cast<wxBitmapToggleButton *>(liveWindow)
                : nullptr;
        if ( !owner || owner->m_bitmapDestroying )
        {
            return wxBitmap();
        }
        if ( callbackState )
        {
            wxBitmapToggleButton * const callbackOwner =
                wxDynamicCast(
                    callbackState->GetOwner(), wxBitmapToggleButton);
            if ( callbackOwner != owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState )
            {
                return wxBitmap();
            }
        }

        const wxBitmapBundle bundle = owner->m_bitmaps[which];
        if ( !bundle.IsOk() )
            return wxBitmap();
        const std::uint64_t bitmapRevision =
            owner->m_bitmapRevision;
        const std::uint64_t contentRevision =
            impl ? impl->contentRequestRevision : 0;
        const auto getState =
            [lifetime, callbackState, impl, bitmapRevision,
             contentRevision]()
                -> wxWinUIToggleProjectionState
            {
                wxWindow * const liveWindow = lifetime.get();
                wxBitmapToggleButton * const current =
                    liveWindow
                        ? static_cast<wxBitmapToggleButton *>(liveWindow)
                        : nullptr;
                if ( !current || current->m_bitmapDestroying )
                {
                    return wxWinUIToggleProjectionState::Dead;
                }
                if ( current->m_bitmapRevision != bitmapRevision )
                    return wxWinUIToggleProjectionState::Restart;
                if ( !callbackState )
                    return wxWinUIToggleProjectionState::Applied;

                wxBitmapToggleButton * const callbackOwner =
                    wxDynamicCast(
                        callbackState->GetOwner(),
                        wxBitmapToggleButton);
                if ( callbackOwner != current ||
                     !callbackOwner->m_winui ||
                     callbackOwner->m_winui.get() != impl ||
                     callbackOwner->m_winui->callbackState !=
                         callbackState )
                {
                    return wxWinUIToggleProjectionState::Dead;
                }
                return callbackOwner->m_winui
                               ->contentRequestRevision ==
                           contentRevision
                           ? wxWinUIToggleProjectionState::Applied
                           : wxWinUIToggleProjectionState::Restart;
            };

        double scale = requestedScale;
        if ( !std::isfinite(scale) || scale <= 0.0 )
        {
            scale = owner->GetDPIScaleFactor();
            const wxWinUIToggleProjectionState state = getState();
            if ( state == wxWinUIToggleProjectionState::Dead )
                return wxBitmap();
            if ( state == wxWinUIToggleProjectionState::Restart )
                continue;
        }
        if ( !std::isfinite(scale) ||
             scale < wxWinUIToggleMinProjectionScale ||
             scale > wxWinUIToggleMaxProjectionScale )
        {
            scale = 1.0;
        }

        const wxSize defaultSize = bundle.GetDefaultSize();
        wxWinUIToggleProjectionState state = getState();
        if ( state == wxWinUIToggleProjectionState::Dead )
            return wxBitmap();
        if ( state == wxWinUIToggleProjectionState::Restart )
            continue;
        if ( defaultSize.x <= 0 || defaultSize.y <= 0 )
            return wxBitmap();

        const wxSize pixelSize =
            bundle.GetPreferredBitmapSizeAtScale(scale);
        state = getState();
        if ( state == wxWinUIToggleProjectionState::Dead )
            return wxBitmap();
        if ( state == wxWinUIToggleProjectionState::Restart )
            continue;
        if ( pixelSize.x <= 0 || pixelSize.y <= 0 )
            return wxBitmap();

        wxBitmap bitmap =
            bundle.GetImpl()->GetBitmap(pixelSize);
        state = getState();
        if ( state == wxWinUIToggleProjectionState::Dead )
            return wxBitmap();
        if ( state == wxWinUIToggleProjectionState::Restart )
            continue;
        if ( bitmap.IsOk() )
        {
            bitmap.SetScaleFactor(
                static_cast<double>(pixelSize.y) /
                defaultSize.y);
        }
        return bitmap;
    }

    return wxBitmap();
}

wxAnyButton::State wxBitmapToggleButton::GetCurrentBitmapState() const
{
    if ( !IsEnabled() )
        return State_Disabled;

    if ( GetValue() && m_bitmaps[State_Pressed].IsOk() )
        return State_Pressed;
    if ( m_winui && m_winui->hovered &&
         m_bitmaps[State_Current].IsOk() )
    {
        return State_Current;
    }
    if ( m_winui && m_winui->focused &&
         m_bitmaps[State_Focused].IsOk() )
    {
        return State_Focused;
    }

    return State_Normal;
}

bool wxBitmapToggleButton::UpdateWinUIContent(bool forceRender)
{
    return UpdateWinUIBitmapContent(
        forceRender, State_Max, 0.0);
}

bool wxBitmapToggleButton::UpdateWinUIBitmapContent(
    bool forceRender,
    State forcedState,
    double requestedScale)
{
    if ( !m_winui || !m_winui->button )
        return true;

    if ( !std::isfinite(requestedScale) || requestedScale < 0.0 ||
         (requestedScale != 0.0 &&
          (requestedScale < wxWinUIToggleMinProjectionScale ||
           requestedScale > wxWinUIToggleMaxProjectionScale)) )
        return false;

    wxWinUIToggleButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIToggleButtonCallbackState> callbackState =
        impl->callbackState;
    const auto peer = impl->button;

    const bool replayEntry =
        impl->contentProjectionReplayEntryPending &&
        !impl->contentProjectionInProgress;
    if ( replayEntry )
    {
        impl->contentProjectionReplayEntryPending = false;
        impl->contentProjectionRequested = true;
    }
    else
    {
        const bool externalRequest =
            !impl->contentProjectionInProgress;
        if ( externalRequest )
        {
            impl->contentProjectionDeferredUsed = false;
            impl->contentProjectionReplayScheduled = false;
            if ( ++impl->contentReplayTicket == 0 )
                ++impl->contentReplayTicket;
            impl->pendingForceRender = forceRender;
        }
        else
        {
            impl->pendingForceRender =
                impl->pendingForceRender || forceRender;
        }

        impl->pendingForcedState = forcedState;
        impl->pendingRequestedScale = requestedScale;
        if ( ++impl->contentRequestRevision == 0 )
            ++impl->contentRequestRevision;
        impl->contentProjectionRequested = true;
    }

    if ( impl->contentProjectionInProgress )
        return true;

    impl->contentProjectionInProgress = true;
    wxScopeGuard projectionGuard =
        wxMakeGuard(
            [callbackState, impl]()
            {
                wxBitmapToggleButton * const owner =
                    callbackState
                        ? wxDynamicCast(
                              callbackState->GetOwner(),
                              wxBitmapToggleButton)
                        : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->contentProjectionInProgress = false;
                }
            });
    wxUnusedVar(projectionGuard);

    const auto getLiveOwner =
        [callbackState, impl]() -> wxBitmapToggleButton *
        {
            wxToggleButton * const baseOwner =
                callbackState ? callbackState->GetOwner() : nullptr;
            wxBitmapToggleButton * const owner =
                wxDynamicCast(baseOwner, wxBitmapToggleButton);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                       ? owner
                       : nullptr;
        };

    for ( unsigned pass = 0;
          pass < wxWinUIMaxToggleProjectionPasses;
          ++pass )
    {
        wxBitmapToggleButton * const live = getLiveOwner();
        if ( !live )
            return false;

        impl->contentProjectionRequested = false;
        const std::uint64_t requestRevision =
            impl->contentRequestRevision;
        const bool passForceRender = impl->pendingForceRender;
        const State passForcedState = impl->pendingForcedState;
        const double passRequestedScale =
            impl->pendingRequestedScale;
        const auto getRequestState =
            [getLiveOwner, requestRevision]()
                -> wxWinUIToggleProjectionState
            {
                wxBitmapToggleButton * const owner = getLiveOwner();
                if ( !owner )
                    return wxWinUIToggleProjectionState::Dead;
                return owner->m_winui->contentRequestRevision ==
                               requestRevision
                           ? wxWinUIToggleProjectionState::Applied
                           : wxWinUIToggleProjectionState::Restart;
            };

        // Snapshot one virtual/scalar boundary at a time. Each virtual getter
        // can run application code; never continue into another wx-model read
        // until the same owner and revision have been revalidated.
        const bool enabled = live->IsEnabled();
        wxWinUIToggleProjectionState projection =
            getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        const bool checked = live->GetValue();
        projection = getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        State bitmapState = passForcedState;
        if ( bitmapState < State_Normal || bitmapState >= State_Max )
        {
            if ( !enabled )
                bitmapState = State_Disabled;
            else if ( checked && live->m_bitmaps[State_Pressed].IsOk() )
                bitmapState = State_Pressed;
            else if ( impl->hovered &&
                      live->m_bitmaps[State_Current].IsOk() )
                bitmapState = State_Current;
            else if ( impl->focused &&
                      live->m_bitmaps[State_Focused].IsOk() )
                bitmapState = State_Focused;
            else
                bitmapState = State_Normal;
        }

        const wxBitmapBundle requestedBundle =
            live->m_bitmaps[bitmapState];
        const wxBitmapBundle normalBundle =
            live->m_bitmaps[State_Normal];
        const wxSize bitmapMargins = live->m_bitmapMargins;
        const wxDirection bitmapPosition =
            live->m_bitmapPosition;

        const wxString rawLabel = live->GetLabel();
        projection = getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        const wxSize dipMargins = live->ToDIP(bitmapMargins);
        projection = getRequestState();
        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        double effectiveScale = passRequestedScale;
        if ( effectiveScale == 0.0 )
        {
            effectiveScale = live->GetDPIScaleFactor();
            projection = getRequestState();
            if ( projection == wxWinUIToggleProjectionState::Dead )
                return false;
            if ( projection == wxWinUIToggleProjectionState::Restart )
                continue;
        }
        if ( !std::isfinite(effectiveScale) ||
             effectiveScale < wxWinUIToggleMinProjectionScale ||
             effectiveScale > wxWinUIToggleMaxProjectionScale )
        {
            effectiveScale = 1.0;
        }

        const wxString text =
            wxControl::GetLabelText(rawLabel);
        const winrt::hstring accessKey = wxWinUIToHString(
            wxWinUIParseLabel(rawLabel).accessKey);
        const wxBitmapBundle sourceBundle =
            requestedBundle.IsOk() ? requestedBundle : normalBundle;

        projection = wxWinUIToggleProjectionState::Failed;
        try
        {
            wxSize pixelSize;
            wxSize dip;
            wxBitmap bmp;
            wxSize normalBitmapLogicalSize;
            bool normalBitmapLogicalSizeValid = false;

            // Publish the normal bundle's size on every transaction, including
            // while a pressed/disabled state is being projected. Otherwise a
            // normal-bitmap replacement in that state leaves a stale best-size
            // cache. Revalidate immediately after the application boundary.
            if ( normalBundle.IsOk() )
            {
                normalBitmapLogicalSize =
                    normalBundle.GetDefaultSize();
                projection = getRequestState();
                if ( projection ==
                         wxWinUIToggleProjectionState::Applied )
                {
                    normalBitmapLogicalSizeValid =
                        normalBitmapLogicalSize.x > 0 &&
                        normalBitmapLogicalSize.y > 0;
                    if ( !normalBitmapLogicalSizeValid )
                    {
                        projection =
                            wxWinUIToggleProjectionState::Failed;
                    }
                }
            }
            else
            {
                projection = wxWinUIToggleProjectionState::Applied;
            }

            if ( projection ==
                     wxWinUIToggleProjectionState::Applied &&
                 sourceBundle.IsOk() )
            {
                // wxBitmapBundle keeps GetDefaultSize() as its logical size.
                // GetPreferredBitmapSizeAtScale() only chooses the best source
                // raster; it must not make the control shrink or grow in DIPs.
                // Both calls are application-owned virtual boundaries.
                const bool sourceIsNormal =
                    normalBundle.IsOk() &&
                    sourceBundle.IsSameAs(normalBundle);
                wxSize defaultSize = normalBitmapLogicalSize;
                if ( !sourceIsNormal )
                {
                    defaultSize = sourceBundle.GetDefaultSize();
                    projection = getRequestState();
                    if ( projection ==
                             wxWinUIToggleProjectionState::Applied &&
                         (defaultSize.x <= 0 || defaultSize.y <= 0) )
                    {
                        projection =
                            wxWinUIToggleProjectionState::Failed;
                    }
                }

                if ( projection ==
                     wxWinUIToggleProjectionState::Applied )
                {
                    pixelSize =
                        sourceBundle.GetPreferredBitmapSizeAtScale(
                            effectiveScale);
                    projection = getRequestState();
                }
                if ( projection ==
                         wxWinUIToggleProjectionState::Applied )
                {
                    if ( pixelSize.x <= 0 || pixelSize.y <= 0 )
                    {
                        projection =
                            wxWinUIToggleProjectionState::Failed;
                    }
                    else
                    {
                        // Calling the implementation directly avoids the
                        // wrapper's additional virtual GetDefaultSize()
                        // boundary, across which the owner couldn't be
                        // revalidated before entering GetBitmap().
                        bmp = sourceBundle.GetImpl()->GetBitmap(
                            pixelSize);
                        projection = getRequestState();
                    }
                }

                if ( projection ==
                         wxWinUIToggleProjectionState::Applied &&
                     bmp.IsOk() )
                {
                    if ( !requestedBundle.IsOk() &&
                         bitmapState == State_Disabled )
                    {
                        bmp = bmp.ConvertToDisabled();
                    }

                    if ( bmp.IsOk() )
                    {
                        dip = defaultSize;
                        bmp.SetScaleFactor(
                            static_cast<double>(pixelSize.y) /
                            defaultSize.y);
                    }
                }
            }
            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                winrt::Windows::Foundation::IInspectable
                    content{ nullptr };
                if ( bmp.IsOk() )
                {
                    if ( auto source =
                             wxWinUIWriteableBitmapFromBitmap(bmp) )
                    {
                        MUXC::Image image;
                        image.Source(source);
                        // XAML dimensions are DIPs. The MSW bitmap logical
                        // accessors otherwise risk scaling the bundle twice.
                        image.Width(dip.x);
                        image.Height(dip.y);
                        image.VerticalAlignment(
                            MUX::VerticalAlignment::Center);
                        image.HorizontalAlignment(
                            MUX::HorizontalAlignment::Center);

                        MUX::Thickness margin{};
                        margin.Left = margin.Right = dipMargins.x;
                        margin.Top = margin.Bottom = dipMargins.y;

                        if ( text.empty() )
                        {
                            image.Margin(margin);
                            content = image;
                        }
                        else
                        {
                            const double gap = 6.0;
                            switch ( bitmapPosition )
                            {
                                case wxRIGHT:
                                    margin.Left += gap;
                                    break;
                                case wxTOP:
                                    margin.Bottom += gap;
                                    break;
                                case wxBOTTOM:
                                    margin.Top += gap;
                                    break;
                                case wxLEFT:
                                default:
                                    margin.Right += gap;
                                    break;
                            }
                            image.Margin(margin);

                            MUXC::TextBlock label;
                            label.Text(wxWinUIToHString(text));
                            MUXC::StackPanel panel;
                            panel.Orientation(
                                bitmapPosition == wxTOP ||
                                bitmapPosition == wxBOTTOM
                                    ? MUXC::Orientation::Vertical
                                    : MUXC::Orientation::Horizontal);
                            panel.VerticalAlignment(
                                MUX::VerticalAlignment::Center);
                            panel.HorizontalAlignment(
                                MUX::HorizontalAlignment::Center);
                            if ( bitmapPosition == wxRIGHT ||
                                 bitmapPosition == wxBOTTOM )
                            {
                                panel.Children().Append(label);
                                panel.Children().Append(image);
                            }
                            else
                            {
                                panel.Children().Append(image);
                                panel.Children().Append(label);
                            }
                            content = panel;
                        }
                    }
                }

                // Fall back to a text peer if conversion of an otherwise
                // valid wxBitmap unexpectedly failed.
                if ( !content && !text.empty() )
                {
                    MUXC::TextBlock label;
                    label.Text(wxWinUIToHString(text));
                    content = label;
                }

                peer.Content(content);
                projection = getRequestState();
            }
            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                MUXA::AutomationProperties::SetAccessKey(
                    peer, accessKey);
                projection = getRequestState();
            }
            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                peer.AccessKey(accessKey);
                projection = getRequestState();
            }

            if ( projection == wxWinUIToggleProjectionState::Applied )
            {
                wxBitmapToggleButton *owner = getLiveOwner();
                if ( !owner )
                {
                    projection = wxWinUIToggleProjectionState::Dead;
                }
                else
                {
                    const bool bestSizeChanged =
                        owner->m_winui
                                ->normalBitmapLogicalSizeValid !=
                            normalBitmapLogicalSizeValid ||
                        (normalBitmapLogicalSizeValid &&
                         owner->m_winui
                                 ->normalBitmapLogicalSize !=
                             normalBitmapLogicalSize);
                    owner->m_winui->normalBitmapLogicalSize =
                        normalBitmapLogicalSize;
                    owner->m_winui->normalBitmapLogicalSizeValid =
                        normalBitmapLogicalSizeValid;
                    owner->m_winui->projectedBitmapState =
                        bitmapState;
                    ++owner->m_winui->contentGeneration;
                    if ( owner->m_winui->contentGeneration == 0 )
                        ++owner->m_winui->contentGeneration;

                    if ( bestSizeChanged )
                    {
                        owner->InvalidateBestSize();
                        projection = getRequestState();
                    }

                    if ( projection ==
                             wxWinUIToggleProjectionState::Applied &&
                         passForceRender )
                    {
                        owner->m_winui->host.ForceRender();
                        projection = getRequestState();
                    }
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI BitmapToggleButton content", e);
            const wxWinUIToggleProjectionState current =
                getRequestState();
            projection =
                current == wxWinUIToggleProjectionState::Applied
                    ? wxWinUIToggleProjectionState::Failed
                    : current;
        }

        if ( projection == wxWinUIToggleProjectionState::Dead )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Restart )
            continue;

        wxBitmapToggleButton * const owner = getLiveOwner();
        if ( !owner )
            return false;
        if ( projection == wxWinUIToggleProjectionState::Failed )
        {
            owner->m_winui->contentProjectionInProgress = false;
            owner->m_winui->contentProjectionRequested = false;
            owner->m_winui->pendingForceRender = false;
            return false;
        }
        if ( owner->m_winui->contentProjectionRequested ||
             owner->m_winui->contentRequestRevision != requestRevision )
        {
            continue;
        }

        owner->m_winui->contentProjectionInProgress = false;
        owner->m_winui->pendingForceRender = false;
        return true;
    }

    wxBitmapToggleButton * const owner = getLiveOwner();
    if ( !owner )
        return false;

    owner->m_winui->contentProjectionInProgress = false;
    owner->m_winui->contentProjectionRequested = true;
    if ( !owner->m_winui->contentProjectionDeferredUsed && wxTheApp )
    {
        owner->m_winui->contentProjectionDeferredUsed = true;
        owner->m_winui->contentProjectionReplayScheduled = true;
        if ( ++owner->m_winui->contentReplayTicket == 0 )
            ++owner->m_winui->contentReplayTicket;
        const std::uint64_t ticket =
            owner->m_winui->contentReplayTicket;
        const std::weak_ptr<wxWinUIToggleButtonCallbackState> weakState(
            callbackState);

        wxTheApp->CallAfter(
            [weakState, impl, ticket]()
            {
                const auto state = weakState.lock();
                if ( !state )
                    return;

                wxBitmapToggleButton * const owner =
                    wxDynamicCast(
                        state->GetOwner(), wxBitmapToggleButton);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     !owner->m_winui->contentProjectionReplayScheduled ||
                     owner->m_winui->contentReplayTicket != ticket )
                {
                    return;
                }

                owner->m_winui->contentProjectionReplayScheduled = false;
                owner->m_winui->contentProjectionReplayEntryPending = true;
                const bool replayForce =
                    owner->m_winui->pendingForceRender;
                owner->UpdateWinUIContent(replayForce);

                wxBitmapToggleButton * const current =
                    wxDynamicCast(
                        state->GetOwner(), wxBitmapToggleButton);
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == state )
                {
                    current->m_winui
                        ->contentProjectionReplayEntryPending = false;
                }
            });
        return true;
    }

    owner->m_winui->contentProjectionRequested = false;
    owner->m_winui->pendingForceRender = false;
    return false;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIButtonTestAccess::ProjectBitmap(
    wxBitmapToggleButton& button,
    State state,
    double scale)
{
    if ( state < wxAnyButton::State_Normal || state >= wxAnyButton::State_Max ||
          !std::isfinite(scale) || scale <= 0.0 ||
          scale < wxWinUIToggleMinProjectionScale ||
          scale > wxWinUIToggleMaxProjectionScale ||
          !button.m_winui || !button.m_winui->button )
    {
        return false;
    }

    return button.UpdateWinUIBitmapContent(true, state, scale);
}

bool wxWinUIButtonTestAccess::GetBitmapProjection(
    const wxBitmapToggleButton& button,
    wxSize *pixelSize,
    State *state,
    std::uint64_t *generation)
{
    if ( !button.m_winui || !button.m_winui->button )
        return false;

    try
    {
        if ( pixelSize )
        {
            *pixelSize = wxSize();
            const auto image =
                wxWinUIFindToggleImage(button.m_winui->button.Content());
            if ( image )
            {
                const auto source =
                    image.Source().try_as<MUXMI::WriteableBitmap>();
                if ( source )
                {
                    *pixelSize =
                        wxSize(source.PixelWidth(), source.PixelHeight());
                }
            }
        }
        if ( state )
            *state = button.m_winui->projectedBitmapState;
        if ( generation )
            *generation = button.m_winui->contentGeneration;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_TOGGLEBTN
