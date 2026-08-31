/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/infobar.cpp
// Purpose:     wxWinUI wxInfoBar implementation (WinUI InfoBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_INFOBAR

#include "wx/infobar.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/sizer.h"
    #include "wx/stockitem.h"
#endif

#include "private.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "infobar-test-access.h"
    #include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
    #include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#endif

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Text.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

wxIMPLEMENT_DYNAMIC_CLASS(wxInfoBar, wxInfoBarBase);

namespace
{

MUXM::SolidColorBrush wxWinUIInfoBarBrush(const wxColour& col)
{
    return wxWinUIBrush(col.Red(), col.Green(), col.Blue(), col.Alpha());
}

void wxWinUIInfoBarApplyFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !font.IsOk() )
    {
        // an invalid font resets the window font: bring the peer back to
        // its theme typography instead of keeping the previous values
        control.ClearValue(MUXC::Control::FontFamilyProperty());
        control.ClearValue(MUXC::Control::FontSizeProperty());
        control.ClearValue(MUXC::Control::FontWeightProperty());
        control.ClearValue(MUXC::Control::FontStyleProperty());
        return;
    }

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        control.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));
    else
        control.ClearValue(MUXC::Control::FontFamilyProperty());

    const double pt = font.GetFractionalPointSize();
    control.FontSize(pt > 0.0 ? pt * 96.0 / 72.0 : 14.0);
    control.FontWeight(font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
                           ? winrt::Microsoft::UI::Text::FontWeights::Bold()
                           : winrt::Microsoft::UI::Text::FontWeights::Normal());
    control.FontStyle(font.GetStyle() == wxFONTSTYLE_NORMAL
                          ? winrt::Windows::UI::Text::FontStyle::Normal
                          : winrt::Windows::UI::Text::FontStyle::Italic);
}

#ifdef WXWINUI_TEST_SUPPORT
// Depth-first search of the visual tree for a named template element.
MUX::FrameworkElement
wxWinUIFindElementByName(MUX::DependencyObject const& root,
                         winrt::hstring const& name)
{
    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        auto child = MUXM::VisualTreeHelper::GetChild(root, i);
        if ( auto element = child.try_as<MUX::FrameworkElement>() )
        {
            if ( element.Name() == name )
                return element;
        }
        if ( auto found = wxWinUIFindElementByName(child, name) )
            return found;
    }
    return nullptr;
}
#endif

class wxWinUIInfoBarCallbackState final
{
public:
    static constexpr unsigned MaxProjectionPassBudget = 16;

    enum class CloseIntent : unsigned char
    {
        Unspecified,
        Close,
        Open
    };

    explicit wxWinUIInfoBarCallbackState(wxInfoBar *owner)
        : m_owner(owner)
    {
    }

    wxInfoBar *GetOwner() const
    {
        return m_owner.load(std::memory_order_acquire);
    }

    std::uint64_t GetModelRevision() const
    {
        return m_modelRevision.load(std::memory_order_acquire);
    }

    std::uint64_t BumpModelRevision()
    {
        // Reentrant model writes are part of the current transaction and must
        // not re-arm its budget. A later public mutation, entered after the
        // transaction unwinds, is a fresh request and releases quarantine.
        if ( !m_projectionActive.load(std::memory_order_acquire) )
        {
            m_projectionPassesRemaining.store(
                MaxProjectionPassBudget, std::memory_order_release);
            m_projectionQuarantined.store(false, std::memory_order_release);
            m_projectionBudgetWarning.store(false, std::memory_order_release);
        }

        std::uint64_t revision =
            m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        if ( !revision )
        {
            revision =
                m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        return revision;
    }

    bool TryBeginProjection()
    {
        bool expected = false;
        if ( !m_projectionActive.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            RequestProjection();
            return false;
        }
        return true;
    }

    void EndProjection()
    {
        m_projectionActive.store(false, std::memory_order_release);
    }

    void RequestProjection()
    {
        m_projectionRequested.store(true, std::memory_order_release);
    }

    bool ConsumeProjectionRequest()
    {
        return m_projectionRequested.exchange(
            false, std::memory_order_acq_rel);
    }

    bool TryConsumeProjectionPass()
    {
        unsigned remaining =
            m_projectionPassesRemaining.load(std::memory_order_acquire);
        while ( remaining )
        {
            if ( m_projectionPassesRemaining.compare_exchange_weak(
                     remaining, remaining - 1,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire) )
            {
                return true;
            }
        }
        return false;
    }

    bool HasProjectionPassesRemaining() const
    {
        return m_projectionPassesRemaining.load(
                   std::memory_order_acquire) != 0;
    }

    bool TryScheduleDeferredProjection()
    {
        bool expected = false;
        return m_deferredProjectionScheduled.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    bool ConsumeDeferredProjection()
    {
        return m_deferredProjectionScheduled.exchange(
            false, std::memory_order_acq_rel);
    }

    void CancelDeferredProjection()
    {
        m_deferredProjectionScheduled.store(
            false, std::memory_order_release);
    }

    bool HasDeferredProjection() const
    {
        return m_deferredProjectionScheduled.load(
            std::memory_order_acquire);
    }

    bool MarkProjectionBudgetWarning()
    {
        return !m_projectionBudgetWarning.exchange(
            true, std::memory_order_acq_rel);
    }

    void ClearProjectionBudgetWarning()
    {
        m_projectionBudgetWarning.store(false, std::memory_order_release);
    }

    void QuarantineProjection()
    {
        m_projectionPassesRemaining.store(0, std::memory_order_release);
        m_projectionRequested.store(false, std::memory_order_release);
        m_projectionQuarantined.store(true, std::memory_order_release);
        CancelDeferredProjection();
    }

    bool IsProjectionQuarantined() const
    {
        return m_projectionQuarantined.load(std::memory_order_acquire);
    }

    std::uint64_t PrepareContentGeneration()
    {
        return m_nextContentGeneration.fetch_add(
                   1, std::memory_order_acq_rel) + 1;
    }

    void CommitContentGeneration(std::uint64_t generation)
    {
        m_contentGeneration.store(generation, std::memory_order_release);
    }

    wxInfoBar *GetContentOwner(std::uint64_t generation) const
    {
        if ( generation == 0 ||
             generation !=
                 m_contentGeneration.load(std::memory_order_acquire) )
        {
            return nullptr;
        }

        return GetOwner();
    }

    bool BeginCloseRequest()
    {
        bool expected = false;
        if ( !m_closeRequestPending.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            return false;
        }

        m_closeIntent.store(CloseIntent::Unspecified,
                            std::memory_order_release);
        return true;
    }

    bool IsCloseRequestPending() const
    {
        return m_closeRequestPending.load(std::memory_order_acquire);
    }

    void MarkCloseIntent(CloseIntent intent)
    {
        m_closeIntent.store(intent, std::memory_order_release);
    }

    CloseIntent EndCloseRequest()
    {
        const CloseIntent intent =
            m_closeIntent.load(std::memory_order_acquire);
        m_closeRequestPending.store(false, std::memory_order_release);
        m_closeIntent.store(CloseIntent::Unspecified,
                            std::memory_order_release);
        return intent;
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_contentGeneration.store(0, std::memory_order_release);
        m_nextContentGeneration.fetch_add(1, std::memory_order_acq_rel);
        m_modelRevision.fetch_add(1, std::memory_order_acq_rel);
        QuarantineProjection();
        m_closeRequestPending.store(false, std::memory_order_release);
        m_closeIntent.store(CloseIntent::Unspecified,
                            std::memory_order_release);
    }

private:
    std::atomic<wxInfoBar *> m_owner;
    std::atomic<std::uint64_t> m_contentGeneration{0};
    std::atomic<std::uint64_t> m_nextContentGeneration{0};
    std::atomic<std::uint64_t> m_modelRevision{1};
    std::atomic<bool> m_projectionActive{false};
    std::atomic<bool> m_projectionRequested{false};
    std::atomic<unsigned> m_projectionPassesRemaining{
        MaxProjectionPassBudget
    };
    std::atomic<bool> m_deferredProjectionScheduled{false};
    std::atomic<bool> m_projectionQuarantined{false};
    std::atomic<bool> m_projectionBudgetWarning{false};
    std::atomic<bool> m_closeRequestPending{false};
    std::atomic<CloseIntent> m_closeIntent{CloseIntent::Unspecified};
};

class wxWinUIInfoBarProjectionGuard final
{
public:
    explicit wxWinUIInfoBarProjectionGuard(
        std::shared_ptr<wxWinUIInfoBarCallbackState> state)
        : m_state(std::move(state)),
          m_acquired(m_state->TryBeginProjection())
    {
    }

    ~wxWinUIInfoBarProjectionGuard()
    {
        if ( m_acquired )
            m_state->EndProjection();
    }

    explicit operator bool() const { return m_acquired; }

private:
    std::shared_ptr<wxWinUIInfoBarCallbackState> m_state;
    bool m_acquired;
};

struct wxWinUIInfoBarButtonHandler
{
    wxWindowID id = wxID_NONE;
    MUXC::Button button{ nullptr };
    winrt::event_token clickToken{};
};

void wxWinUIRevokeInfoBarContentHandlers(
    std::vector<wxWinUIInfoBarButtonHandler>& buttonHandlers,
    MUXC::CheckBox& checkBox,
    winrt::event_token& checkedToken,
    winrt::event_token& uncheckedToken)
{
    for ( wxWinUIInfoBarButtonHandler& handler : buttonHandlers )
    {
        if ( handler.button && handler.clickToken.value )
        {
            try
            {
                handler.button.Click(handler.clickToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI InfoBar button handler removal", e);
            }
        }
        handler.clickToken = {};
    }
    buttonHandlers.clear();

    if ( checkBox && checkedToken.value )
    {
        try
        {
            checkBox.Checked(checkedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI InfoBar checked handler removal", e);
        }
    }
    checkedToken = {};

    if ( checkBox && uncheckedToken.value )
    {
        try
        {
            checkBox.Unchecked(uncheckedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI InfoBar unchecked handler removal", e);
        }
    }
    uncheckedToken = {};
    checkBox = nullptr;
}

} // anonymous namespace

class wxWinUIInfoBarImpl
{
public:
    ~wxWinUIInfoBarImpl()
    {
        Close();
    }

    void RevokeContentHandlers()
    {
        wxWinUIRevokeInfoBarContentHandlers(
            buttonHandlers, checkBox, checkedToken, uncheckedToken);
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

#ifdef WXWINUI_TEST_SUPPORT
        nextContentWriteHookForTesting = nullptr;
        nextContentWriteContextForTesting = nullptr;
#endif
        RevokeContentHandlers();

        if ( bar && closingToken.value )
        {
            try
            {
                bar.Closing(closingToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI InfoBar Closing handler removal", e);
            }
        }
        closingToken = {};

        host.Close();
        bar = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState;
    MUXC::InfoBar bar{ nullptr };
    MUXC::CheckBox checkBox{ nullptr };
    winrt::event_token closingToken{};
    winrt::event_token checkedToken{};
    winrt::event_token uncheckedToken{};
    std::vector<wxWinUIInfoBarButtonHandler> buttonHandlers;
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIInfoBarTestAccess::ContentWriteHook
        nextContentWriteHookForTesting = nullptr;
    void *nextContentWriteContextForTesting = nullptr;
#endif
};

wxInfoBar::wxInfoBar()
{
}

wxInfoBar::wxInfoBar(wxWindow *parent, wxWindowID winid, long style)
{
    Create(parent, winid, style);
}

wxInfoBar::~wxInfoBar()
{
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }
}

bool wxInfoBar::Create(wxWindow *parent, wxWindowID winid, long style)
{
    // calling Hide() before Create() ensures that we're created initially
    // hidden
    Hide();
    if ( !wxControl::Create(parent, winid, wxDefaultPosition, wxDefaultSize,
                            style, wxDefaultValidator,
                            wxASCII_STR("infobar")) )
        return false;

    m_winui = std::make_unique<wxWinUIInfoBarImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUIInfoBarCallbackState>(this);
    wxWinUIInfoBarImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    try
    {
        m_winui->bar = MUXC::InfoBar();
        m_winui->bar.IsOpen(false);
        m_winui->bar.IsClosable(true);

        // The native close (X) button must behave like the generic one: emit
        // wxEVT_BUTTON with wxID_CLOSE through the wx handler chain (the
        // official sample binds it to read the checkbox state) and dismiss
        // the bar unless the application handled the event and kept it open.
        //
        // When Closing fires, WinUI has already set IsOpen to false, and it
        // re-opens the bar itself after the callback if Cancel is true (see
        // OnCloseButtonClick()/RaiseClosingEvent() in microsoft-ui-xaml).
        // So the decision has to be made *inside* the callback: route the wx
        // event, let Dismiss() record its verdict through the state flags
        // (writing IsOpen here would not toggle anything), and cancel the
        // XAML closing only when the bar is meant to stay open.  A real
        // Dismiss() outside a click closes with Reason::Programmatic and
        // takes none of these branches.
        m_winui->closingToken = m_winui->bar.Closing(
            [callbackState](
                MUXC::InfoBar const&,
                MUXC::InfoBarClosingEventArgs const& args)
            {
                if ( args.Reason() != MUXC::InfoBarCloseReason::CloseButton )
                    return;

                wxInfoBar *owner = callbackState->GetOwner();
                if ( !owner || !callbackState->BeginCloseRequest() )
                    return;

                owner->OnButtonClick(wxID_CLOSE);

                owner = callbackState->GetOwner();
                const wxWinUIInfoBarCallbackState::CloseIntent intent =
                    callbackState->EndCloseRequest();
                if ( owner &&
                     intent !=
                         wxWinUIInfoBarCallbackState::CloseIntent::Close )
                {
                    args.Cancel(true);
                }
            });

        // Apply the appearance that was possibly set before creation.
        if ( m_hasFont )
            wxWinUIInfoBarApplyFont(m_winui->bar, GetFont());
        if ( m_hasFgCol )
            m_winui->bar.Foreground(wxWinUIInfoBarBrush(GetForegroundColour()));

        const bool contentSet =
            createImpl->host.SetContent(createImpl->bar);
        wxInfoBar * const owner = callbackState->GetOwner();
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
        wxWinUILogException("WinUI InfoBar creation", e);
        wxInfoBar * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxInfoBar * const owner = callbackState->GetOwner();
    return owner && owner->m_winui &&
           owner->m_winui.get() == createImpl;
}

void wxInfoBar::ShowMessage(const wxString& msg, int flags)
{
    if ( !m_winui || !m_winui->bar || !m_winui->callbackState )
        return;

    wxWinUIInfoBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        impl->callbackState;
    MUXC::InfoBarSeverity severity = MUXC::InfoBarSeverity::Informational;
    if ( flags & wxICON_ERROR )
        severity = MUXC::InfoBarSeverity::Error;
    else if ( flags & wxICON_WARNING )
        severity = MUXC::InfoBarSeverity::Warning;
    else if ( flags & wxICON_INFORMATION )
        severity = MUXC::InfoBarSeverity::Informational;

    const int icon = flags & wxICON_MASK;

    const bool closeRequestWasPending =
        callbackState->IsCloseRequestPending();
    bool openRequested = false;
    try
    {
        const MUXC::InfoBar bar = impl->bar;
        bar.Severity(severity);
        bar.IsIconVisible(icon != 0 && icon != wxICON_NONE);
        bar.Message(wxWinUIToHString(msg));

        // A close-button Closing callback is already the transaction which
        // will materialize the final IsOpen value. Writing true here would
        // re-open the dependency property before a later reentrant Dismiss()
        // can publish its Close intent. Leave the peer false and let
        // args.Cancel(true) reopen it only if Open is still the last writer
        // when the callback returns.
        if ( !closeRequestWasPending &&
             !callbackState->IsCloseRequestPending() )
        {
            bar.IsOpen(true);
        }
        openRequested = true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar ShowMessage", e);
    }

    wxInfoBar *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    // ShowMessage() is a new decision if it is called reentrantly from the
    // close-button handler. Publish it before layout, so any later Dismiss()
    // dispatched by layout remains the authoritative last writer.
    if ( openRequested && callbackState->IsCloseRequestPending() )
    {
        callbackState->MarkCloseIntent(
            wxWinUIInfoBarCallbackState::CloseIntent::Open);
    }

    // The message changes the natural size of the bar.
    owner->InvalidateBestSize();
    if ( !owner->IsShown() )
    {
        owner->DoShow();
    }
    else // we're already shown
    {
        // just update the layout to correspond to the new message
        owner->UpdateParent();
    }

    owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    // Synchronous host flushes can dispatch arbitrary application callbacks.
    // Keep this as the final operation of ShowMessage().
    impl->host.ForceRender();
}

void wxInfoBar::Dismiss()
{
    if ( !m_winui || !m_winui->bar || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        m_winui->callbackState;
    if ( callbackState->IsCloseRequestPending() )
    {
        // We are inside the Closing callback of a close-button click:
        // IsOpen is already false there, so writing it would not close
        // anything. Record the decision in the state retained by the
        // callback, which remains valid even if an event destroys this bar.
        callbackState->MarkCloseIntent(
            wxWinUIInfoBarCallbackState::CloseIntent::Close);
    }
    else
    {
        const MUXC::InfoBar bar = m_winui->bar;
        try
        {
            bar.IsOpen(false);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    if ( callbackState->GetOwner() == this )
        DoHide();
}

void wxInfoBar::DoShow()
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIInfoBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        impl->callbackState;
    const auto currentOwner =
        [callbackState, impl]() -> wxInfoBar *
        {
            wxInfoBar * const owner = callbackState->GetOwner();
            return owner && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == callbackState
                ? owner
                : nullptr;
        };

    // re-layout the parent first so that the window expands into an already
    // unoccupied by the other controls area: for this we need to change our
    // internal visibility flag to force Layout() to take us into account (see
    // the identical dance in the generic implementation)

    // just change the internal flag indicating that the window is visible,
    // without really showing it
    wxWindowBase::Show();
    wxInfoBar *owner = currentOwner();
    if ( !owner )
        return;

    // adjust the parent layout to account for us
    owner->UpdateParent();
    owner = currentOwner();
    if ( !owner )
        return;

    // reset the flag back before really showing the window or it wouldn't be
    // shown at all because it would believe itself already visible
    owner->wxWindowBase::Show(false);
    owner = currentOwner();
    if ( !owner )
        return;

    // finally do really show the window: the effects are implemented with
    // AnimateWindow() on our HWND (see MSWShowWithEffect()); if that fails,
    // fall back to showing the bar without any effect rather than not
    // showing it at all
    const wxShowEffect effect = owner->GetShowEffect();
    const unsigned timeout = owner->GetEffectDuration();
    if ( effect == wxSHOW_EFFECT_NONE ||
            !owner->ShowWithEffect(effect, timeout) )
    {
        owner = currentOwner();
        if ( !owner )
            return;

        // a failed ShowWithEffect() may have already toggled the wx
        // visibility flag before AnimateWindow() failed; reset it so that
        // the plain Show() below really shows the window instead of being
        // a no-op on an already-"shown" flag
        owner->wxWindowBase::Show(false);
        owner = currentOwner();
        if ( owner )
            owner->Show();
    }
}

void wxInfoBar::DoHide()
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIInfoBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        impl->callbackState;
    const auto currentOwner =
        [callbackState, impl]() -> wxInfoBar *
        {
            wxInfoBar * const owner = callbackState->GetOwner();
            return owner && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == callbackState
                ? owner
                : nullptr;
        };

    // same fallback policy (and visibility-flag reset) as in DoShow()
    const wxShowEffect effect = GetHideEffect();
    const unsigned timeout = GetEffectDuration();
    if ( effect == wxSHOW_EFFECT_NONE ||
            !HideWithEffect(effect, timeout) )
    {
        wxInfoBar *owner = currentOwner();
        if ( !owner )
            return;
        owner->wxWindowBase::Show(true);
        owner = currentOwner();
        if ( !owner )
            return;
        owner->Hide();
    }

    if ( wxInfoBar * const owner = currentOwner() )
        owner->UpdateParent();
}

wxInfoBar::BarPlacement wxInfoBar::GetBarPlacement() const
{
    wxSizer * const sizer = GetContainingSizer();
    if ( !sizer )
        return BarPlacement_Unknown;

    const wxSizerItemList& siblings = sizer->GetChildren();
    if ( siblings.GetFirst()->GetData()->GetWindow() == this )
        return BarPlacement_Top;
    else if ( siblings.GetLast()->GetData()->GetWindow() == this )
        return BarPlacement_Bottom;
    else
        return BarPlacement_Unknown;
}

wxShowEffect wxInfoBar::GetShowEffect() const
{
    if ( m_showEffect != wxSHOW_EFFECT_MAX )
        return m_showEffect;

    switch ( GetBarPlacement() )
    {
        case BarPlacement_Top:
            return wxSHOW_EFFECT_SLIDE_TO_BOTTOM;

        case BarPlacement_Bottom:
            return wxSHOW_EFFECT_SLIDE_TO_TOP;

        default:
            wxFAIL_MSG( "unknown info bar placement" );
            wxFALLTHROUGH;

        case BarPlacement_Unknown:
            return wxSHOW_EFFECT_NONE;
    }
}

wxShowEffect wxInfoBar::GetHideEffect() const
{
    if ( m_hideEffect != wxSHOW_EFFECT_MAX )
        return m_hideEffect;

    switch ( GetBarPlacement() )
    {
        case BarPlacement_Top:
            return wxSHOW_EFFECT_SLIDE_TO_TOP;

        case BarPlacement_Bottom:
            return wxSHOW_EFFECT_SLIDE_TO_BOTTOM;

        default:
            wxFAIL_MSG( "unknown info bar placement" );
            wxFALLTHROUGH;

        case BarPlacement_Unknown:
            return wxSHOW_EFFECT_NONE;
    }
}

void wxInfoBar::ShowCheckBox(const wxString& checkBoxText, bool checked)
{
    wxASSERT_MSG( HasFlag(wxINFOBAR_CHECKBOX),
                  "wxINFOBAR_CHECKBOX style should be set if calling ShowCheckBox()!");

    m_checkBoxLabel = checkBoxText;
    m_checked = checked;

    RebuildContent();
}

bool wxInfoBar::SetFont(const wxFont& font)
{
    if ( !wxInfoBarBase::SetFont(font) )
        return false;

    // check that we're not called before Create()
    if ( m_winui && m_winui->bar && m_winui->callbackState )
    {
        wxWinUIInfoBarImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
            impl->callbackState;
        try
        {
            wxWinUIInfoBarApplyFont(impl->bar, font);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI InfoBar SetFont", e);
        }

        wxInfoBar * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == impl &&
             owner->m_winui->callbackState == callbackState )
        {
            owner->InvalidateBestSize();
            // Final boundary: never touch owner/impl after this call.
            impl->host.ForceRender();
        }
    }

    return true;
}

bool wxInfoBar::SetForegroundColour(const wxColor& colour)
{
    if ( !wxInfoBarBase::SetForegroundColour(colour) )
        return false;

    if ( m_winui && m_winui->bar && m_winui->callbackState )
    {
        wxWinUIInfoBarImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
            impl->callbackState;
        try
        {
            if ( colour.IsOk() )
                impl->bar.Foreground(wxWinUIInfoBarBrush(colour));
            else
                impl->bar.ClearValue(MUXC::Control::ForegroundProperty());

            wxInfoBar * const owner = callbackState->GetOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == callbackState )
            {
                // Final boundary: never touch owner/impl after this call.
                impl->host.ForceRender();
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI InfoBar SetForegroundColour", e);
        }
    }

    return true;
}

void wxInfoBar::AddButton(wxWindowID btnid, const wxString& label)
{
    ButtonInfo info;
    info.id = btnid;
    info.label = label.empty() ? wxGetStockLabel(btnid, wxSTOCK_NOFLAGS) : label;
    m_buttons.push_back(info);
    RebuildContent();
}

void wxInfoBar::RemoveButton(wxWindowID btnid)
{
    // remove the last-added button with this id, like the generic version
    // (ids should be unique, but if they are repeated removing the most
    // recently added one makes more sense)
    for ( size_t i = m_buttons.size(); i > 0; --i )
    {
        if ( m_buttons[i - 1].id == btnid )
        {
            m_buttons.erase(m_buttons.begin() + (i - 1));
            RebuildContent();
            return;
        }
    }

    wxFAIL_MSG( wxString::Format("button with id %d not found", btnid) );
}

size_t wxInfoBar::GetButtonCount() const
{
    return m_buttons.size();
}

wxWindowID wxInfoBar::GetButtonId(size_t idx) const
{
    wxCHECK_MSG( idx < m_buttons.size(), wxID_NONE, wxT("invalid infobar button index") );
    return m_buttons[idx].id;
}

bool wxInfoBar::HasButtonId(wxWindowID btnid) const
{
    for ( size_t i = 0; i < m_buttons.size(); ++i )
    {
        if ( m_buttons[i].id == btnid )
            return true;
    }
    return false;
}

wxSize wxInfoBar::DoGetBestSize() const
{
    // Measure the real XAML content once it has been realized; before that,
    // fall back to a plausible default size.
    if ( m_winui && m_winui->host.IsContentLoaded() )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size.x > 0 && size.y > 0 )
            return size;
    }

    return wxWindow::FromDIP(wxSize(200, 48), const_cast<wxInfoBar*>(this));
}

void wxInfoBar::RebuildContent(bool externalMutation)
{
    if ( !m_winui || !m_winui->bar || !m_winui->callbackState )
        return;

    wxWinUIInfoBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        impl->callbackState;
    if ( externalMutation )
        callbackState->BumpModelRevision();

    const wxWinUIInfoBarProjectionGuard projectionGuard(callbackState);
    if ( !projectionGuard )
        return;

    enum class ProjectionState
    {
        Current,
        Restart,
        Dead
    };

    const auto stateForRevision =
        [callbackState, impl](std::uint64_t revision)
        {
            wxInfoBar * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState )
            {
                return ProjectionState::Dead;
            }

            return callbackState->GetModelRevision() == revision
                ? ProjectionState::Current
                : ProjectionState::Restart;
        };

    const auto quarantine =
        [&]()
        {
            callbackState->QuarantineProjection();
            if ( callbackState->MarkProjectionBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI InfoBar content kept changing during projection; "
                    "quarantining this request until the next external "
                    "mutation.");
            }
        };

    const auto deferLatest =
        [&]()
        {
            if ( !callbackState->HasProjectionPassesRemaining() )
            {
                quarantine();
                return;
            }

            callbackState->RequestProjection();
            if ( callbackState->HasDeferredProjection() )
                return;

            if ( !wxTheApp )
            {
                quarantine();
                return;
            }

            // Logging is an application boundary: a custom log target may
            // yield the event loop. It must happen before the deferred ticket
            // is armed, otherwise that ticket could be consumed while the
            // current projection guard is still active.
            if ( callbackState->MarkProjectionBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI InfoBar content kept changing during projection; "
                    "using its one bounded asynchronous replay.");
            }

            if ( !callbackState->TryScheduleDeferredProjection() )
            {
                if ( !callbackState->HasDeferredProjection() )
                    quarantine();
                return;
            }

            const std::weak_ptr<wxWinUIInfoBarCallbackState>
                weakState(callbackState);

            // Posting is deliberately the final operation: no application
            // boundary may pump this callback before projectionGuard ends.
            wxTheApp->CallAfter(
                [weakState, impl]()
                {
                    const auto state = weakState.lock();
                    if ( !state || !state->ConsumeDeferredProjection() )
                        return;

                    wxInfoBar * const owner = state->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state )
                    {
                        return;
                    }

                    owner->RebuildContent(false);
                });
        };

    constexpr unsigned MaxSynchronousProjectionPasses = 8;
    for ( unsigned pass = 0;
          pass < MaxSynchronousProjectionPasses;
          ++pass )
    {
        if ( !callbackState->TryConsumeProjectionPass() )
        {
            quarantine();
            return;
        }

        callbackState->ConsumeProjectionRequest();
        wxInfoBar *owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState )
        {
            return;
        }

        const std::uint64_t revision =
            callbackState->GetModelRevision();
        const wxVector<ButtonInfo> buttons = owner->m_buttons;
        const wxString checkBoxLabel = owner->m_checkBoxLabel;
        const bool checked = owner->m_checked;
        const std::uint64_t generation =
            callbackState->PrepareContentGeneration();
        std::vector<wxWinUIInfoBarButtonHandler>
            candidateButtonHandlers;
        MUXC::CheckBox candidateCheckBox{ nullptr };
        winrt::event_token candidateCheckedToken{};
        winrt::event_token candidateUncheckedToken{};

        const auto revokeCandidate =
            [&]()
            {
                wxWinUIRevokeInfoBarContentHandlers(
                    candidateButtonHandlers,
                    candidateCheckBox,
                    candidateCheckedToken,
                    candidateUncheckedToken);
            };

        try
        {
            const bool hasButtons = !buttons.empty();
            const bool hasCheckBox = !checkBoxLabel.empty();
            const MUXC::InfoBar bar = impl->bar;

            // Custom buttons replace the built-in close (X) button.
            bar.IsClosable(!hasButtons);
            ProjectionState state = stateForRevision(revision);
            if ( state == ProjectionState::Dead )
            {
                revokeCandidate();
                return;
            }
            if ( state == ProjectionState::Restart )
            {
                revokeCandidate();
                continue;
            }

            MUX::UIElement contentElement{ nullptr };
            if ( hasButtons || hasCheckBox )
            {
                // InfoBar.Content renders under the message: stack the row of
                // custom buttons and the checkbox, like the generic version.
                MUXC::StackPanel content;
                content.Orientation(MUXC::Orientation::Vertical);
                content.Spacing(8);
                content.Margin(
                    MUX::ThicknessHelper::FromLengths(0, 0, 0, 8));

                if ( hasButtons )
                {
                    MUXC::StackPanel buttonsPanel;
                    buttonsPanel.Orientation(
                        MUXC::Orientation::Horizontal);
                    buttonsPanel.Spacing(8);

                    for ( const ButtonInfo& info : buttons )
                    {
                        const wxWindowID btnid = info.id;
                        MUXC::Button button;
                        MUXC::TextBlock text;
                        text.Text(wxWinUIToHString(
                            wxControl::GetLabelText(info.label)));
                        button.Content(text);
                        const winrt::event_token clickToken = button.Click(
                            [callbackState, generation, btnid](
                                const winrt::Windows::Foundation::
                                    IInspectable&,
                                const MUX::RoutedEventArgs&)
                            {
                                wxInfoBar * const current =
                                    callbackState->GetContentOwner(
                                        generation);
                                if ( current )
                                    current->OnButtonClick(btnid);
                            });
                        buttonsPanel.Children().Append(button);
                        candidateButtonHandlers.push_back(
                            { btnid, button, clickToken });
                    }

                    content.Children().Append(buttonsPanel);
                }

                if ( hasCheckBox )
                {
                    MUXC::CheckBox checkBox;
                    checkBox.Content(winrt::box_value(
                        wxWinUIToHString(
                            wxControl::GetLabelText(checkBoxLabel))));
                    checkBox.IsChecked(checked);
                    candidateCheckedToken = checkBox.Checked(
                        [callbackState, generation](
                            const winrt::Windows::Foundation::IInspectable&,
                            const MUX::RoutedEventArgs&)
                        {
                            wxInfoBar * const current =
                                callbackState->GetContentOwner(generation);
                            if ( current )
                                current->m_checked = true;
                        });
                    candidateUncheckedToken = checkBox.Unchecked(
                        [callbackState, generation](
                            const winrt::Windows::Foundation::IInspectable&,
                            const MUX::RoutedEventArgs&)
                        {
                            wxInfoBar * const current =
                                callbackState->GetContentOwner(generation);
                            if ( current )
                                current->m_checked = false;
                        });
                    content.Children().Append(checkBox);
                    candidateCheckBox = checkBox;
                }

                contentElement = content;
            }

#ifdef WXWINUI_TEST_SUPPORT
            const wxWinUIInfoBarTestAccess::ContentWriteHook hook =
                impl->nextContentWriteHookForTesting;
            void * const hookContext =
                impl->nextContentWriteContextForTesting;
            impl->nextContentWriteHookForTesting = nullptr;
            impl->nextContentWriteContextForTesting = nullptr;
#endif

            bar.Content(contentElement);
#ifdef WXWINUI_TEST_SUPPORT
            if ( hook )
                hook(hookContext);
#endif

            state = stateForRevision(revision);
            if ( state == ProjectionState::Dead )
            {
                revokeCandidate();
                return;
            }

            // Publish the exact handlers for the content which is now in the
            // peer even if a nested mutation superseded its model snapshot.
            // This leaves a functional last accepted generation if a hostile
            // writer eventually exhausts the replay budget.
            owner = callbackState->GetOwner();
            owner->m_winui->RevokeContentHandlers();
            owner->m_winui->buttonHandlers.swap(
                candidateButtonHandlers);
            owner->m_winui->checkBox = candidateCheckBox;
            owner->m_winui->checkedToken = candidateCheckedToken;
            owner->m_winui->uncheckedToken =
                candidateUncheckedToken;
            candidateCheckBox = nullptr;
            candidateCheckedToken = {};
            candidateUncheckedToken = {};
            callbackState->CommitContentGeneration(generation);

            if ( state == ProjectionState::Restart )
                continue;

            // Buttons and checkbox change the natural size. Layout can run
            // arbitrary wx handlers, so revalidate before the final flush.
            owner->InvalidateBestSize();
            owner->UpdateParent();
            state = stateForRevision(revision);
            if ( state == ProjectionState::Dead )
                return;
            if ( state == ProjectionState::Restart )
                continue;

            impl->host.ForceRender();
            state = stateForRevision(revision);
            if ( state == ProjectionState::Dead )
                return;
            if ( state == ProjectionState::Restart ||
                 callbackState->ConsumeProjectionRequest() )
            {
                continue;
            }

            callbackState->CancelDeferredProjection();
            callbackState->ClearProjectionBudgetWarning();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            revokeCandidate();
            wxWinUILogException("WinUI InfoBar RebuildContent", e);
            return;
        }
    }

    deferLatest();
}

void wxInfoBar::OnButtonClick(wxWindowID btnid)
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        m_winui->callbackState;
    wxCommandEvent event(wxEVT_BUTTON, btnid);
    event.SetEventObject(this);

    // If the application does not handle the button, dismiss the bar (this
    // matches the generic info bar behavior).
    const bool handled = HandleWindowEvent(event);
    wxInfoBar * const owner = callbackState->GetOwner();
    if ( !handled && owner )
        owner->Dismiss();
}

void wxInfoBar::UpdateParent()
{
    if ( wxWindow *parent = GetParent() )
        parent->Layout();
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIInfoBarTestAccess::IsPeerOpen(const wxInfoBar& info)
{
    const auto& impl = info.m_winui;
    if ( !impl || !impl->bar )
        return false;

    try
    {
        return impl->bar.IsOpen();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIInfoBarTestAccess::ClickCloseButton(wxInfoBar& info)
{
    const auto& impl = info.m_winui;
    if ( !impl || !impl->bar )
        return false;

    try
    {
        // "CloseButton" is the x:Name of the close button in the official
        // InfoBar control template.
        auto closeButton =
            wxWinUIFindElementByName(impl->bar, L"CloseButton")
                .try_as<MUXC::Button>();
        if ( !closeButton )
            return false;

        winrt::Microsoft::UI::Xaml::Automation::Peers::ButtonAutomationPeer
            peer(closeButton);
        peer.Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar close-button invoke", e);
        return false;
    }
}

bool wxWinUIInfoBarTestAccess::ClickButton(wxInfoBar& info, wxWindowID btnid)
{
    const auto& impl = info.m_winui;
    if ( !impl || !impl->callbackState )
        return false;

    const std::shared_ptr<wxWinUIInfoBarCallbackState> callbackState =
        impl->callbackState;
    for ( const wxWinUIInfoBarButtonHandler& handler :
              impl->buttonHandlers )
    {
        if ( handler.id != btnid || !handler.button )
            continue;

        try
        {
            winrt::Microsoft::UI::Xaml::Automation::Peers::
                ButtonAutomationPeer peer(handler.button);
            peer.Invoke();
            // Do not touch info after Invoke(): the wx handler is allowed to
            // have destroyed the InfoBar while the native callback unwound.
            wxUnusedVar(callbackState);
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI InfoBar custom-button invoke", e);
            return false;
        }
    }

    return false;
}

void wxWinUIInfoBarTestAccess::SetNextContentWriteHook(
    wxInfoBar& info,
    ContentWriteHook hook,
    void *context)
{
    const auto& impl = info.m_winui;
    if ( !impl )
        return;

    impl->nextContentWriteHookForTesting = hook;
    impl->nextContentWriteContextForTesting =
        hook ? context : nullptr;
}

bool wxWinUIInfoBarTestAccess::HasDeferredContentProjection(const wxInfoBar& info)
{
    const auto& impl = info.m_winui;
    return impl && impl->callbackState &&
           impl->callbackState->HasDeferredProjection();
}

bool wxWinUIInfoBarTestAccess::IsContentProjectionQuarantined(const wxInfoBar& info)
{
    const auto& impl = info.m_winui;
    return impl && impl->callbackState &&
           impl->callbackState->IsProjectionQuarantined();
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_INFOBAR
