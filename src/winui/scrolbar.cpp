/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/scrolbar.cpp
// Purpose:     wxWinUI wxScrollBar implementation (WinUI ScrollBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SCROLLBAR

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "range-test-access.h"
#endif

#include <atomic>
#include <cmath>
#include <cstdint>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

namespace
{

class wxWinUIScrollBarCallbackState
{
public:
    explicit wxWinUIScrollBarCallbackState(wxScrollBar *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxScrollBar *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;

        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndPeerMutation()
    {
        m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

private:
    std::atomic<wxScrollBar *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUIScrollBarPeerMutationGuard
{
public:
    explicit wxWinUIScrollBarPeerMutationGuard(
        const std::shared_ptr<wxWinUIScrollBarCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUIScrollBarPeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

private:
    std::shared_ptr<wxWinUIScrollBarCallbackState> m_state;
};

void wxWinUIEmitScrollBarEvent(
    const std::shared_ptr<wxWinUIScrollBarCallbackState>& state,
    std::uint64_t generation,
    wxEventType type,
    int position)
{
    wxScrollBar * const owner = state->GetOwner(generation);
    if ( !owner )
        return;

    wxScrollEvent event(
        type,
        owner->GetId(),
        position,
        owner->IsVertical() ? wxVERTICAL : wxHORIZONTAL);
    event.SetEventObject(owner);
    owner->HandleWindowEvent(event);
}

} // anonymous namespace

// wxScrollBar RTTI and the wxEVT_SCROLL_* events are provided by the common
// code (src/common/scrolbarcmn.cpp and src/common/event.cpp).

class wxWinUIScrollBarImpl
{
public:
    static wxScrollBar::WinUIPeerAction GetPeerAction(
        MUXCP::ScrollEventType action);
    static wxEventType GetEventType(wxScrollBar::WinUIPeerAction action);

    ~wxWinUIScrollBarImpl()
    {
        Close();
    }

    void Close()
    {
        // Invalidate before revoking the delegate or detaching the peer:
        // either operation can drain a queued Scroll notification.
        if ( callbackState )
            callbackState->Invalidate();

        if ( bar && scrollToken.value )
        {
            try
            {
                bar.Scroll(scrollToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ScrollBar Scroll removal", e);
            }
        }
        scrollToken = {};

        host.Close();
        bar = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIScrollBarCallbackState> callbackState;
    MUXCP::ScrollBar bar{ nullptr };
    winrt::event_token scrollToken{};
};

wxScrollBar::WinUIPeerAction wxWinUIScrollBarImpl::GetPeerAction(
    MUXCP::ScrollEventType action)
{
    switch ( action )
    {
        case MUXCP::ScrollEventType::SmallDecrement:
            return wxScrollBar::WinUIPeerAction::SmallDecrement;

        case MUXCP::ScrollEventType::SmallIncrement:
            return wxScrollBar::WinUIPeerAction::SmallIncrement;

        case MUXCP::ScrollEventType::LargeDecrement:
            return wxScrollBar::WinUIPeerAction::LargeDecrement;

        case MUXCP::ScrollEventType::LargeIncrement:
            return wxScrollBar::WinUIPeerAction::LargeIncrement;

        case MUXCP::ScrollEventType::ThumbPosition:
            return wxScrollBar::WinUIPeerAction::ThumbPosition;

        case MUXCP::ScrollEventType::ThumbTrack:
            return wxScrollBar::WinUIPeerAction::ThumbTrack;

        case MUXCP::ScrollEventType::First:
            return wxScrollBar::WinUIPeerAction::First;

        case MUXCP::ScrollEventType::Last:
            return wxScrollBar::WinUIPeerAction::Last;

        case MUXCP::ScrollEventType::EndScroll:
            return wxScrollBar::WinUIPeerAction::EndScroll;
    }

    wxFAIL_MSG("unhandled WinUI ScrollEventType");
    return wxScrollBar::WinUIPeerAction::EndScroll;
}

wxEventType wxWinUIScrollBarImpl::GetEventType(
    wxScrollBar::WinUIPeerAction action)
{
    switch ( action )
    {
        case wxScrollBar::WinUIPeerAction::SmallDecrement:
            return wxEVT_SCROLL_LINEUP;

        case wxScrollBar::WinUIPeerAction::SmallIncrement:
            return wxEVT_SCROLL_LINEDOWN;

        case wxScrollBar::WinUIPeerAction::LargeDecrement:
            return wxEVT_SCROLL_PAGEUP;

        case wxScrollBar::WinUIPeerAction::LargeIncrement:
            return wxEVT_SCROLL_PAGEDOWN;

        case wxScrollBar::WinUIPeerAction::ThumbPosition:
            return wxEVT_SCROLL_THUMBRELEASE;

        case wxScrollBar::WinUIPeerAction::ThumbTrack:
            return wxEVT_SCROLL_THUMBTRACK;

        case wxScrollBar::WinUIPeerAction::First:
            return wxEVT_SCROLL_TOP;

        case wxScrollBar::WinUIPeerAction::Last:
            return wxEVT_SCROLL_BOTTOM;

        case wxScrollBar::WinUIPeerAction::EndScroll:
            return wxEVT_SCROLL_CHANGED;
    }

    wxFAIL_MSG("unhandled WinUI scrollbar action");
    return wxEVT_SCROLL_CHANGED;
}

wxScrollBar::wxScrollBar() = default;

wxScrollBar::wxScrollBar(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                         const wxSize& size, long style,
                         const wxValidator& validator, const wxString& name)
    : wxScrollBar()
{
    Create(parent, id, pos, size, style, validator, name);
}

wxScrollBar::~wxScrollBar()
{
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }
}

bool wxScrollBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                         const wxSize& size, long style,
                         const wxValidator& validator, const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( !(style & (wxSB_HORIZONTAL | wxSB_VERTICAL)) )
        style |= wxSB_HORIZONTAL;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui = std::make_unique<wxWinUIScrollBarImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUIScrollBarCallbackState>(this);
    wxWinUIScrollBarImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIScrollBarCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    MUXCP::ScrollBar createPeer{ nullptr };
    try
    {
        m_winui->bar = MUXCP::ScrollBar();
        createPeer = m_winui->bar;
        m_winui->bar.Orientation(
            IsVertical() ? MUXC::Orientation::Vertical
                         : MUXC::Orientation::Horizontal);

        const std::shared_ptr<wxWinUIScrollBarCallbackState> callbackState =
            m_winui->callbackState;
        const std::uint64_t generation = callbackState->Generation();
        m_winui->scrollToken = m_winui->bar.Scroll(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXCP::ScrollEventArgs const& event)
            {
                wxScrollBar * const owner =
                    callbackState->GetOwner(generation);
                if ( !owner ||
                     callbackState->IsPeerMutationInProgress() )
                {
                    return;
                }

                owner->OnPeerScroll(
                    static_cast<int>(std::lround(event.NewValue())),
                    wxWinUIScrollBarImpl::GetPeerAction(
                        event.ScrollEventType()));
            });

        ApplyToPeer();
        const bool contentSet =
            createImpl->host.SetContent(createPeer);
        wxScrollBar * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->bar != createPeer )
        {
            return false;
        }

        if ( !contentSet )
        {
            liveOwner->m_winui->Close();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ScrollBar creation", e);
        wxScrollBar * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxScrollBar *liveOwner =
        createState->GetOwner(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState ||
         liveOwner->m_winui->bar != createPeer )
    {
        return false;
    }

    liveOwner->SetInitialSize(size);
    liveOwner = createState->GetOwner(createGeneration);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == createImpl &&
           liveOwner->m_winui->callbackState == createState &&
           liveOwner->m_winui->bar == createPeer;
}

int wxScrollBar::GetMaxPosition() const
{
    return wxMax(0, m_range - m_thumbSize);
}

int wxScrollBar::ClampPosition(int position) const
{
    return wxClip(position, 0, GetMaxPosition());
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    m_position = ClampPosition(viewStart);
    ApplyToPeer();
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range,
                               int pageSize, bool WXUNUSED(refresh))
{
    m_thumbSize = wxMax(0, thumbSize);
    m_range = wxMax(0, range);
    m_pageSize = wxMax(1, pageSize);
    m_position = ClampPosition(position);
    ApplyToPeer();
}

void wxScrollBar::ApplyToPeer()
{
    if ( !m_winui || !m_winui->bar || !m_winui->callbackState )
        return;

    try
    {
        wxWinUIScrollBarPeerMutationGuard guard(m_winui->callbackState);
        m_winui->bar.Minimum(0);
        m_winui->bar.Maximum(GetMaxPosition());
        m_winui->bar.ViewportSize(m_thumbSize);
        m_winui->bar.SmallChange(1);
        m_winui->bar.LargeChange(m_pageSize);
        m_winui->bar.Value(m_position);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ScrollBar peer update", e);
        return;
    }

    m_winui->host.ForceRender();
}

void wxScrollBar::OnPeerScroll(int newValue, WinUIPeerAction action)
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUIScrollBarCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();

    if ( !IsEnabled() )
    {
        ApplyToPeer();
        return;
    }

    const int canonicalPosition = ClampPosition(newValue);
    const bool moved = canonicalPosition != m_position;

    // Like the native MSW control, no-op movement notifications are
    // suppressed, while thumb release and EndScroll remain meaningful even
    // when their final position equals the previous one.
    if ( !moved &&
         action != WinUIPeerAction::ThumbPosition &&
         action != WinUIPeerAction::EndScroll )
    {
        if ( canonicalPosition != newValue )
            ApplyToPeer();
        return;
    }

    m_position = canonicalPosition;
    if ( canonicalPosition != newValue )
        ApplyToPeer();

    // Exactly one wx event corresponds to one XAML Scroll notification.
    // In particular CHANGED is reserved for EndScroll and is never appended
    // to line/page/track notifications.
    wxWinUIEmitScrollBarEvent(
        callbackState,
        generation,
        wxWinUIScrollBarImpl::GetEventType(action),
        canonicalPosition);
}

wxSize wxScrollBar::DoGetBestSize() const
{
    return wxWindow::FromDIP(
        IsVertical() ? wxSize(16, 140) : wxSize(140, 16),
        const_cast<wxScrollBar *>(this));
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIRangeTestAccess::ApplyScrollBarAction(wxScrollBar& bar,
                                                  ScrollBarAction action,
                                                  int value)
{
    if ( !bar.m_winui || !bar.m_winui->bar || !bar.m_winui->callbackState )
        return false;

    // Put the peer in the state a real Scroll notification would already
    // have established, without manufacturing a second Scroll callback.
    try
    {
        wxWinUIScrollBarPeerMutationGuard guard(bar.m_winui->callbackState);
        bar.m_winui->bar.Value(bar.ClampPosition(value));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ScrollBar test input", e);
        return false;
    }

    // This is the adapter called by the real XAML Scroll delegate. Keeping
    // the seam here exercises its clamp, disabled and event mapping rules
    // without synthesizing machine-wide pointer or keyboard input.
    bar.OnPeerScroll(value, action);
    return true;
}

bool wxWinUIRangeTestAccess::GetPeerState(const wxScrollBar& bar,
                                          double *minimum,
                                          double *maximum,
                                          double *value,
                                          double *viewport,
                                          double *smallChange,
                                          double *largeChange,
                                          bool *vertical)
{
    if ( !bar.m_winui || !bar.m_winui->bar )
        return false;

    try
    {
        if ( minimum )
            *minimum = bar.m_winui->bar.Minimum();
        if ( maximum )
            *maximum = bar.m_winui->bar.Maximum();
        if ( value )
            *value = bar.m_winui->bar.Value();
        if ( viewport )
            *viewport = bar.m_winui->bar.ViewportSize();
        if ( smallChange )
            *smallChange = bar.m_winui->bar.SmallChange();
        if ( largeChange )
            *largeChange = bar.m_winui->bar.LargeChange();
        if ( vertical )
        {
            *vertical =
                bar.m_winui->bar.Orientation() ==
                MUXC::Orientation::Vertical;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ScrollBar test peer query", e);
        return false;
    }

    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_SCROLLBAR
