/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/scrollbarprojection.cpp
// Purpose:     Real WinUI scrollbar peers for wx-managed HWND scroll models
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "scrollbarprojection.h"
#include "scrollbarindicator.h"
#include "private.h"
#include "hostnative.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/msw/private.h"
#include "wx/weakref.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/window.h"
#include "wx/settings.h"
#include "wx/winui/winui.h"
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <set>
#include <utility>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;

namespace
{
constexpr wchar_t ManagedProperty[] = L"wxWinUI.ManagedWindowScrollbars";
std::vector<std::weak_ptr<wxWinUIScrollBarProjection>> gs_projections;

// The windows which were given scrollbars through the wx API, the only ones
// which can carry the property above: a synchronisation looks at these
// instead of asking every window of the frame for the property, hidden
// pages included, which was a tenth of what a resize costs.
std::set<wxWindow *> gs_managedWindows;

struct ScrollbarBootstrap
{
    wxWeakRef<wxWindow> owner;
    HWND hwnd = nullptr;
    unsigned long long generation = 0;

    bool IsCurrent() const
    {
        if ( !owner || GetHwndOf(owner.get()) != hwnd || !generation ||
             wxWinUIMSWGetHwndGeneration(owner.get(), hwnd) != generation ||
             !::GetPropW(hwnd, ManagedProperty) ) return false;
        for ( wxWindow* window = owner.get(); window; window = window->GetParent() )
        {
            if ( wxWinUITLWHostIsDestroyScheduled(window) ) return false;
            if ( window->IsTopLevel() ) return true;
        }
        return false;
    }
};

using BootstrapKey = std::pair<HWND, unsigned long long>;
std::map<BootstrapKey, std::weak_ptr<ScrollbarBootstrap>> gs_bootstraps;

void QueueScrollbarBootstrap(wxWindow* owner)
{
    if ( !owner || !wxTheApp || !owner->GetHandle() ||
         wxWinUITLWHostIsDestroyScheduled(owner) ||
         wxWinUIIsPhysicalDisconnectPublicationPoisoned() ||
         wxWinUITopLevelHost::ForWindow(owner, false) ) return;
    const HWND hwnd = GetHwndOf(owner);
    if ( !::GetPropW(hwnd, ManagedProperty) ) return;
    const auto generation = wxWinUIMSWGetHwndGeneration(owner, hwnd);
    if ( !generation ) return;
    const BootstrapKey key{hwnd, generation};
    const auto found = gs_bootstraps.find(key);
    if ( found != gs_bootstraps.end() && !found->second.expired() ) return;
    auto request = std::make_shared<ScrollbarBootstrap>();
    request->owner = owner; request->hwnd = hwnd; request->generation = generation;
    gs_bootstraps[key] = request;
    // SetScrollbar is used during native creation and layout. Bootstrap must
    // not enter WinRT, initialize a TLW, or call user code on that setter's
    // stack. The queued request owns no wx object and rechecks the exact HWND
    // incarnation, current hierarchy and final scrollbar model at execution.
    wxTheApp->CallAfter([request, key]()
    {
        const auto pending = gs_bootstraps.find(key);
        if ( pending != gs_bootstraps.end() && pending->second.lock() == request )
            gs_bootstraps.erase(pending);
        if ( !request->IsCurrent() || wxWinUIIsPhysicalDisconnectPublicationPoisoned() ||
             !(::GetWindowLongPtr(request->hwnd, GWL_STYLE) & (WS_HSCROLL | WS_VSCROLL)) ) return;
        if ( !wxWinUITopLevelHost::ForWindow(request->owner.get(), true) ||
             !request->IsCurrent() ) return;
        wxWinUITLWHostNotifyNativeLayout(request->owner.get(), false);
    });
}

bool SameRect(const RECT& a, const RECT& b)
{
    return ::EqualRect(&a, &b) != FALSE;
}

wxWinUITopLevelHost::WindowPartKind PartKind(LONG objectId)
{
    return objectId == OBJID_VSCROLL
        ? wxWinUITopLevelHost::WindowPartKind::VerticalScrollBar
        : wxWinUITopLevelHost::WindowPartKind::HorizontalScrollBar;
}

bool ClientScreen(HWND hwnd, RECT& rect)
{
    if ( !::GetClientRect(hwnd, &rect) ) return false;
    ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    if ( rect.left > rect.right ) std::swap(rect.left, rect.right);
    return !::IsRectEmpty(&rect);
}

// Where the WinUI scrollbar of a managed window is shown, in screen
// coordinates: a strip of its client area along the edge, over the content.
// USER32 reserves no non-client band for these windows (see WM_NCCALCSIZE in
// wxWindowMSW::MSWHandleMessage), so there is no classic scrollbar which
// could show underneath; only its model and its style bits are used.
bool OverlayBarRect(HWND hwnd, LONG objectId, RECT& rect)
{
    const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    const bool vertical = objectId == OBJID_VSCROLL;
    if ( !(style & (vertical ? WS_VSCROLL : WS_HSCROLL)) ) return false;
    RECT client{};
    if ( !ClientScreen(hwnd, client) ) return false;
    const UINT dpi = ::GetDpiForWindow(hwnd);
    const int cx = ::GetSystemMetricsForDpi(SM_CXVSCROLL, dpi);
    const int cy = ::GetSystemMetricsForDpi(SM_CYHSCROLL, dpi);
    const LONG_PTR exStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    // Physical side of the vertical bar: left under exactly one of the two
    // mirroring styles.
    const bool leftBar = ((exStyle & WS_EX_LEFTSCROLLBAR) != 0) !=
                         ((exStyle & WS_EX_LAYOUTRTL) != 0);
    const bool both = (style & WS_VSCROLL) && (style & WS_HSCROLL);
    rect = client;
    if ( vertical )
    {
        if ( leftBar ) rect.right = std::min(client.right, client.left + cx);
        else rect.left = std::max(client.left, client.right - cx);
        if ( both ) rect.bottom = std::max(client.top, client.bottom - cy);
    }
    else
    {
        rect.top = std::max(client.top, client.bottom - cy);
        if ( both )
        {
            if ( leftBar ) rect.left = std::min(client.right, client.left + cx);
            else rect.right = std::max(client.left, client.right - cx);
        }
    }
    return !::IsRectEmpty(&rect);
}

COLORREF ReadBackingPixel(HWND hwnd, POINT screen)
{
    RECT window{};
    if ( !hwnd || !::GetWindowRect(hwnd, &window) ) return CLR_INVALID;
    HDC dc = ::GetWindowDC(hwnd);
    if ( !dc ) return CLR_INVALID;
    const int saved = ::SaveDC(dc);
    COLORREF colour = CLR_INVALID;
    if ( saved && ::SetLayout(dc, 0) != GDI_ERROR )
        colour = ::GetPixel(dc, screen.x - window.left, screen.y - window.top);
    if ( saved ) ::RestoreDC(dc, saved);
    ::ReleaseDC(hwnd, dc);
    return colour;
}

X::FrameworkElement FindDiagnosticPart(const X::DependencyObject& node,
                                     const wchar_t* name, unsigned& budget)
{
    if ( !node || !budget ) return nullptr;
    --budget;
    if ( const auto element = node.try_as<X::FrameworkElement>() )
        if ( element.Name() == name ) return element;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count && budget; ++i )
        if ( const auto found = FindDiagnosticPart(
                X::Media::VisualTreeHelper::GetChild(node, i), name, budget) )
            return found;
    return nullptr;
}

void LogScrollbarSurface(HWND owner, HWND bridge, LONG objectId,
                         const RECT& band, const P::ScrollBar& peer) noexcept
{
    // These are reads of the two actual GDI backing surfaces, not desktop
    // screenshots. A clean owner DC alone cannot prove that the island above
    // it is also clean. Keep this behind the existing deduplicated log entry.
    const bool vertical = objectId == OBJID_VSCROLL;
    const POINT track{band.left + 2, band.top + 2};
    RECT client{};
    const bool hasClient = ClientScreen(owner, client);
    POINT adjacent = track;
    if ( vertical ) adjacent.x = hasClient && band.right <= client.left
        ? band.right + 2 : band.left - 2;
    else adjacent.y = band.top - 2;
    wxWinUIInputLog(
        "SCROLLBAR surfaces owner=%p bridge=%p object=%ld at=%ld,%ld ownerTrack=0x%08lx bridgeTrack=0x%08lx ownerAdjacent=0x%08lx bridgeAdjacent=0x%08lx",
        static_cast<void*>(owner), static_cast<void*>(bridge), objectId,
        track.x, track.y, static_cast<unsigned long>(ReadBackingPixel(owner, track)),
        static_cast<unsigned long>(ReadBackingPixel(bridge, track)),
        static_cast<unsigned long>(ReadBackingPixel(owner, adjacent)),
        static_cast<unsigned long>(ReadBackingPixel(bridge, adjacent)));
    try
    {
        unsigned budget = 192;
        const auto templateRoot = FindDiagnosticPart(peer, L"Root", budget);
        budget = 192;
        const auto trackPart = FindDiagnosticPart(peer,
            vertical ? L"VerticalTrackRect" : L"HorizontalTrackRect", budget);
        double effectiveOpacity = trackPart ? trackPart.Opacity() : -1.0;
        bool visible = bool(trackPart);
        X::DependencyObject node = trackPart;
        for ( unsigned depth = 0; node && depth != 16; ++depth )
        {
            if ( const auto element = node.try_as<X::UIElement>() )
            {
                visible = visible && element.Visibility() == X::Visibility::Visible;
                if ( node != trackPart ) effectiveOpacity *= element.Opacity();
            }
            if ( node == peer ) break;
            node = X::Media::VisualTreeHelper::GetParent(node);
        }
        wxString states;
        if ( templateRoot )
            for ( const auto& group : X::VisualStateManager::GetVisualStateGroups(templateRoot) )
                if ( const auto state = group.CurrentState() )
                    states += wxWinUIFromHString(state.Name()) + ";";
        wxWinUIInputLog(
            "SCROLLBAR template owner=%p object=%ld track=%d visible=%d opacity=%g effectiveOpacity=%g states=%ls",
            static_cast<void*>(owner), objectId, bool(trackPart), visible,
            trackPart ? trackPart.Opacity() : -1.0, effectiveOpacity, states.wc_str());
    }
    catch ( const winrt::hresult_error& )
    {
        // Diagnostics must not revoke a usable projection or force layout.
    }
}

// The WinUI scrollbar is drawn over the client area and USER32 has no
// non-client band for these windows: there is no classic backing to repaint
// or to clean when a bar appears, changes or goes away.
bool InvalidateScrollbarBands(HWND, const std::vector<RECT>&)
{
    return true;
}

wxEventType ScrollEvent(P::ScrollEventType type)
{
    switch ( type )
    {
        case P::ScrollEventType::SmallDecrement: return wxEVT_SCROLLWIN_LINEUP;
        case P::ScrollEventType::SmallIncrement: return wxEVT_SCROLLWIN_LINEDOWN;
        case P::ScrollEventType::LargeDecrement: return wxEVT_SCROLLWIN_PAGEUP;
        case P::ScrollEventType::LargeIncrement: return wxEVT_SCROLLWIN_PAGEDOWN;
        case P::ScrollEventType::ThumbPosition: return wxEVT_SCROLLWIN_THUMBRELEASE;
        case P::ScrollEventType::ThumbTrack: return wxEVT_SCROLLWIN_THUMBTRACK;
        case P::ScrollEventType::First: return wxEVT_SCROLLWIN_TOP;
        case P::ScrollEventType::Last: return wxEVT_SCROLLWIN_BOTTOM;
        case P::ScrollEventType::EndScroll: return wxEVT_NULL;
    }
    return wxEVT_NULL;
}

}

struct wxWinUIScrollBarProjection::Impl : std::enable_shared_from_this<Impl>
{
    struct Entry
    {
        wxWeakRef<wxWindow> owner;
        HWND hwnd = nullptr;
        unsigned long long generation = 0;
        LONG objectId = 0;
        P::ScrollBar peer{nullptr};
        std::shared_ptr<wxWinUIScrollBarIndicator> indicator;
        RECT band{}, clip{};
        bool live = true, prepared = false, painted = false, backingDirty = true;
        unsigned mutation = 0;
        std::uint64_t inputSerial = 0;
        wxString diagnosticState;
    };
    wxWeakRef<wxWindow> topLevel;
    C::Canvas root{nullptr};
    std::weak_ptr<wxWinUIHostLifetime> lifetime;
    std::map<std::pair<HWND,LONG>, std::shared_ptr<Entry>> entries;
    std::map<std::pair<HWND,LONG>, unsigned long long> diagnosticRejected;
    winrt::Windows::Foundation::IInspectable rootPointer{nullptr};
    bool active = true, syncing = false, syncDeferred = false;
    unsigned dispatchDepth = 0;

    bool Current() const
    {
        const auto state = lifetime.lock();
        return active && topLevel && !wxWinUITLWHostIsDestroyScheduled(topLevel.get()) &&
            state && state->GetHost() && root;
    }

    bool Current(const std::shared_ptr<Entry>& entry) const
    {
        if ( !Current() || !entry->live || !entry->owner ||
             wxWinUITLWHostIsDestroyScheduled(entry->owner.get()) ||
             GetHwndOf(entry->owner.get()) != entry->hwnd ||
             wxWinUIMSWGetHwndGeneration(entry->owner.get(), entry->hwnd) != entry->generation )
            return false;
        wxWindow* ancestor = entry->owner.get();
        while ( ancestor && ancestor != topLevel.get() ) ancestor = ancestor->GetParent();
        return ancestor == topLevel.get();
    }

    bool Read(const std::shared_ptr<Entry>& entry, SCROLLINFO& info,
              SCROLLBARINFO& bar, RECT& clipped) const
    {
        if ( !Current(entry) || !::IsWindowVisible(entry->hwnd) ||
             !::GetPropW(entry->hwnd, ManagedProperty) ) return false;
        const bool vertical = entry->objectId == OBJID_VSCROLL;
        if ( !(::GetWindowLongPtr(entry->hwnd, GWL_STYLE) & (vertical ? WS_VSCROLL : WS_HSCROLL)) )
            return false;
        info = {sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS};
        bar = {sizeof(SCROLLBARINFO)};
        if ( !::GetScrollInfo(entry->hwnd, vertical ? SB_VERT : SB_HORZ, &info) ||
             !OverlayBarRect(entry->hwnd, entry->objectId, bar.rcScrollBar) )
            return false;
        // The style bit stays set with nothing to scroll only for a bar
        // which is always shown: present, but disabled.
        if ( info.nMax - info.nMin + 1 <= static_cast<int>(info.nPage) )
            bar.rgstate[0] |= STATE_SYSTEM_UNAVAILABLE;
        clipped = bar.rcScrollBar;
        // Do not project through explicit shaped HWND regions. The existing
        // native band cut remains the conservative fallback for these owners.
        HRGN shape = ::CreateRectRgn(0, 0, 0, 0);
        if ( !shape ) return false;
        bool valid = true;
        for ( HWND hwnd = entry->hwnd; hwnd; hwnd = ::GetParent(hwnd) )
        {
            if ( ::GetWindowRgn(hwnd, shape) != ERROR ) { valid = false; break; }
            if ( hwnd != entry->hwnd )
            {
                RECT client{};
                if ( !ClientScreen(hwnd, client) || !::IntersectRect(&clipped, &clipped, &client) )
                { valid = false; break; }
            }
            if ( hwnd == GetHwndOf(topLevel.get()) ) break;
        }
        ::DeleteObject(shape);
        return valid;
    }

    bool ReadPaintedBand(const std::shared_ptr<Entry>& entry, RECT& clip) const
    {
        // Region publication owns the HWND's native scrollbar band until it
        // is withdrawn. Moving/resizing that same HWND does not return paint
        // ownership to USER32 while the queued XAML geometry catches up.
        // Covers() deliberately remains stricter for bridge region layout.
        // Never use the old screen-space clip here: it may now overlap client
        // content, and it says nothing about the band at its new position.
        SCROLLINFO info{};
        SCROLLBARINFO bar{};
        return entry->painted && Read(entry, info, bar, clip);
    }

    void Notify(const std::shared_ptr<Entry>& entry)
    {
        if ( Current(entry) ) wxWinUITLWHostNotifyNativeLayout(entry->owner.get(), false);
    }

    void ObservePointer(HWND target, POINT screen, bool touch)
    {
        if ( !Current() ) return;
        const HWND tlw = GetHwndOf(topLevel.get());
        if ( target && target != tlw && !::IsChild(tlw, target) ) return;
        std::vector<std::shared_ptr<Entry>> snapshot;
        snapshot.reserve(entries.size());
        for ( const auto& pair : entries ) snapshot.push_back(pair.second);
        for ( const auto& entry : snapshot )
        {
            if ( !Current(entry) || !entry->prepared || !entry->indicator ||
                 !::IsWindowVisible(entry->hwnd) ) continue;
            RECT area{};
            if ( !::GetWindowRect(entry->hwnd, &area) ) continue;
            for ( HWND parent = ::GetParent(entry->hwnd); parent; parent = ::GetParent(parent) )
            {
                RECT client{};
                if ( !ClientScreen(parent, client) || !::IntersectRect(&area, &area, &client) )
                { ::SetRectEmpty(&area); break; }
                if ( parent == tlw ) break;
            }
            if ( ::PtInRect(&area, screen) ) entry->indicator->Activity(touch);
        }
    }

    void Emit(const std::shared_ptr<Entry>& entry, wxEventType type, double value)
    {
        if ( !Current(entry) || !entry->prepared || entry->mutation ||
             type == wxEVT_NULL || !std::isfinite(value) ) return;
        SCROLLINFO info{}; SCROLLBARINFO bar{}; RECT clip{};
        if ( !Read(entry, info, bar, clip) ) return;
        const int maximum = std::max(info.nMin,
            info.nMax - static_cast<int>(info.nPage ? info.nPage - 1 : 0));
        const int position = static_cast<int>(std::clamp(value,
            static_cast<double>(info.nMin), static_cast<double>(maximum)));
        wxWindow* const owner = entry->owner.get();
        wxWinUITLWHostWindowEventGuard guard(owner);
        ++dispatchDepth;
        struct Dispatch
        {
            Impl& self;
            ~Dispatch()
            {
                if ( --self.dispatchDepth == 0 && self.syncDeferred )
                {
                    self.syncDeferred = false;
                    if ( self.Current() )
                        wxWinUITLWHostNotifyNativeLayout(self.topLevel.get(), false);
                }
            }
        } dispatch{*this};
        wxScrollWinEvent event(type, position,
            entry->objectId == OBJID_VSCROLL ? wxVERTICAL : wxHORIZONTAL);
        event.SetEventObject(owner);
        owner->HandleWindowEvent(event);
        // The application, not the XAML peer, owns SCROLLINFO. Full 32-bit
        // positions are delivered directly (WM_SCROLL has only 16 bits and
        // its SIF_TRACKPOS belongs to USER32's unrelated native gesture).
        Notify(entry);
    }

    std::shared_ptr<Entry> Make(wxWindow* owner, LONG objectId)
    {
        auto entry = std::make_shared<Entry>();
        entry->owner = owner;
        entry->hwnd = GetHwndOf(owner);
        entry->generation = wxWinUIMSWGetHwndGeneration(owner, entry->hwnd);
        entry->objectId = objectId;
        entry->peer = P::ScrollBar();
        entry->peer.Orientation(objectId == OBJID_VSCROLL ? C::Orientation::Vertical : C::Orientation::Horizontal);
        entry->peer.IsTabStop(false);
        entry->indicator = wxWinUIScrollBarIndicator::Create(entry->peer);
        X::Automation::AutomationProperties::SetAutomationId(entry->peer,
            wxWinUIToHString(wxString::Format("wxWindow.Scrollbar.%llu.%s",
                static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(entry->hwnd)),
                objectId == OBJID_VSCROLL ? "Vertical" : "Horizontal")));
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Entry> weakEntry = entry;
        entry->peer.Scroll([weak, weakEntry](const auto&, const P::ScrollEventArgs& args)
        {
            const auto self = weak.lock(); const auto item = weakEntry.lock();
            if ( !self || !item || !self->Current(item) ) return;
            item->indicator->Activity(item->peer.IndicatorMode() == P::ScrollingIndicatorMode::TouchIndicator);
            ++item->inputSerial; // cancels the ValueChanged-only/UIA fallback
            self->Emit(item, ScrollEvent(args.ScrollEventType()), args.NewValue());
        });
        entry->peer.ValueChanged([weak, weakEntry](const auto&, const P::RangeBaseValueChangedEventArgs& args)
        {
            const auto self = weak.lock(); const auto item = weakEntry.lock();
            if ( !self || !item || !self->Current(item) ) return;
            // Also observe actual model-driven movement while mutation suppresses
            // feedback to wx. Unloaded initialization never reveals indicators.
            item->indicator->Activity(item->peer.IndicatorMode() == P::ScrollingIndicatorMode::TouchIndicator);
            if ( !self->Current(item) || item->mutation ) return;
            const auto serial = ++item->inputSerial;
            const double value = args.NewValue();
            item->owner->CallAfter([weak, weakEntry, serial, value]()
            {
                const auto live = weak.lock(); const auto entry = weakEntry.lock();
                if ( live && entry && entry->inputSerial == serial )
                    live->Emit(entry, wxEVT_SCROLLWIN_THUMBTRACK, value);
            });
        });
        entry->peer.Loaded([weak, weakEntry](const auto&, const auto&)
        {
            const auto self = weak.lock(); const auto item = weakEntry.lock();
            if ( self && item ) self->Notify(item);
        });
        // A XAML ScrollBar ignores the wheel and the root router leaves alone
        // what a XAML element owns, so the wheel would be dead over the bar.
        // Give it to the scrolled window as the ordinary queued message; over
        // the horizontal bar a vertical wheel scrolls horizontally.
        entry->peer.PointerWheelChanged([weak, weakEntry](const auto&, const X::Input::PointerRoutedEventArgs& args)
        {
            const auto self = weak.lock(); const auto item = weakEntry.lock();
            if ( !self || !item || !self->Current(item) ) return;
            try
            {
                const auto props = args.GetCurrentPoint(nullptr).Properties();
                int delta = props.MouseWheelDelta();
                bool horizontal = props.IsHorizontalMouseWheel();
                if ( !delta ) return;
                if ( !horizontal && item->objectId == OBJID_HSCROLL ) { horizontal = true; delta = -delta; }
                WORD keys = 0;
                if ( ::GetKeyState(VK_SHIFT) & 0x8000 ) keys |= MK_SHIFT;
                if ( ::GetKeyState(VK_CONTROL) & 0x8000 ) keys |= MK_CONTROL;
                POINT screen{};
                ::GetCursorPos(&screen);
                args.Handled(true);
                ::PostMessageW(item->hwnd, horizontal ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
                    MAKEWPARAM(keys, static_cast<WORD>(static_cast<short>(delta))),
                    MAKELPARAM(screen.x, screen.y));
            }
            catch ( const winrt::hresult_error& ) { }
        });
        return entry;
    }

    void Remove(const std::shared_ptr<Entry>& entry)
    {
        if ( entry->indicator ) entry->indicator->Retire();
        entry->live = false;
        const bool repaint = std::exchange(entry->painted, false);
        entry->prepared = false;
        // The parts of a window which is gone went with it.
        const auto state = lifetime.lock();
        auto* const host = state ? state->GetHost() : nullptr;
        if ( host && entry->owner )
            host->SetWindowPart(entry->owner.get(), PartKind(entry->objectId), nullptr);
        if ( repaint && entry->owner && GetHwndOf(entry->owner.get()) == entry->hwnd &&
             !InvalidateScrollbarBands(entry->hwnd, {entry->clip}) )
        {
            // The entry is already unpublished: ordinary MSW painting is
            // restored even if allocating/requesting the NC redraw failed.
            wxLogDebug("wxWinUI: failed to invalidate a retired scrollbar band");
        }
    }
};

wxWinUIScrollBarProjection::wxWinUIScrollBarProjection(std::shared_ptr<Impl> impl)
    : m_impl(std::move(impl)) {}
wxWinUIScrollBarProjection::~wxWinUIScrollBarProjection() { Shutdown(); }

std::shared_ptr<wxWinUIScrollBarProjection> wxWinUIScrollBarProjection::Create(
    wxWindow* topLevel, const C::Canvas& root,
    const std::shared_ptr<wxWinUIHostLifetime>& lifetime)
{
    auto impl = std::make_shared<Impl>();
    impl->topLevel = topLevel; impl->root = root; impl->lifetime = lifetime;
    const std::weak_ptr<Impl> weak = impl;
    impl->rootPointer = winrt::box_value(X::Input::PointerEventHandler(
        [weak](const auto&, const X::Input::PointerRoutedEventArgs& args)
        {
            const auto self = weak.lock();
            if ( !self || !self->Current() ) return;
            try
            {
                const auto state = self->lifetime.lock();
                auto* const host = state ? state->GetHost() : nullptr;
                if ( !host ) return;
                const auto point = args.GetCurrentPoint(self->root);
                const auto position = point.Position();
                const auto client = host->GetClientScreenRect();
                const double scale = host->GetScale();
                self->ObservePointer(nullptr,
                    {client.left + static_cast<LONG>(std::lround(position.X * scale)),
                     client.top + static_cast<LONG>(std::lround(position.Y * scale))},
                    point.PointerDeviceType() == winrt::Microsoft::UI::Input::PointerDeviceType::Touch);
            }
            catch ( const winrt::hresult_error& ) { }
        }));
    for ( const auto& routed : {X::UIElement::PointerMovedEvent(),
            X::UIElement::PointerEnteredEvent(), X::UIElement::PointerPressedEvent(),
            X::UIElement::PointerWheelChangedEvent()} )
        root.AddHandler(routed, impl->rootPointer, true);
    auto projection = std::shared_ptr<wxWinUIScrollBarProjection>(new wxWinUIScrollBarProjection(impl));
    gs_projections.erase(std::remove_if(gs_projections.begin(), gs_projections.end(),
        [](const auto& weak) { return weak.expired(); }), gs_projections.end());
    gs_projections.push_back(projection);
    return projection;
}

void wxWinUIScrollBarProjection::Sync(const RECT& clientScreen, double scale)
{
    const auto self = m_impl;
    if ( !self->Current() || self->syncing || scale <= 0 ) return;
    // A wx handler may yield, hide/reparent/delete its scrolling owner, and
    // thereby run the host flush inside the native Scroll/Thumb callback.
    // Keep that peer attached until the dispatch stack has actually returned;
    // the outermost dispatch queues one causal repair, not a polling loop.
    if ( self->dispatchDepth ) { self->syncDeferred = true; return; }
    self->syncing = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{self->syncing};
    const auto hostState = self->lifetime.lock();
    auto* const host = hostState ? hostState->GetHost() : nullptr;
    if ( !host ) return;
    wxUnusedVar(clientScreen);
    if ( wxWinUIInputLogEnabled() )
    {
        for ( auto it = self->diagnosticRejected.begin(); it != self->diagnosticRejected.end(); )
            if ( !::IsWindow(it->first.first) ) it = self->diagnosticRejected.erase(it);
            else ++it;
    }
    std::vector<wxWeakRef<wxWindow>> windows;
    wxWindow * const topLevel = self->topLevel.get();
    for ( wxWindow *window : gs_managedWindows )
    {
        const HWND hwnd = GetHwndOf(window);
        if ( window != topLevel && hwnd &&
             wxGetTopLevelParent(window) == topLevel &&
             ::GetPropW(hwnd, ManagedProperty) )
        {
            windows.emplace_back(window);
        }
    }
    std::map<std::pair<HWND,LONG>, bool> wanted;
    for ( const auto& weak : windows )
    {
        if ( !self->Current() ) return;
        if ( !weak ) continue;
        for ( LONG objectId : {OBJID_HSCROLL, OBJID_VSCROLL} )
        {
            if ( !self->Current() || !weak ) return;
            const auto key = std::make_pair(GetHwndOf(weak.get()), objectId);
            const LONG_PTR style = ::GetWindowLongPtr(key.first, GWL_STYLE);
            if ( !::IsWindowVisible(key.first) ||
                 !(style & (objectId == OBJID_VSCROLL ? WS_VSCROLL : WS_HSCROLL)) ) continue;
            RECT presence{};
            if ( !OverlayBarRect(key.first, objectId, presence) ) continue;
            auto found = self->entries.find(key);
            if ( found != self->entries.end() && !self->Current(found->second) )
            {
                auto obsolete = found->second;
                self->entries.erase(found);
                self->Remove(obsolete);
                if ( !self->Current() || !weak ) return;
                found = self->entries.end();
            }
            std::shared_ptr<Impl::Entry> entry;
            try { entry = found == self->entries.end() ? self->Make(weak.get(), objectId) : found->second; }
            catch ( const winrt::hresult_error& error )
            { wxWinUILogException("WinUI window scrollbar creation", error); continue; }
            SCROLLINFO info{}; SCROLLBARINFO bar{}; RECT clip{};
            if ( !self->Read(entry, info, bar, clip) )
            {
                if ( wxWinUIInputLogEnabled() &&
                     self->diagnosticRejected[key] != entry->generation )
                {
                    self->diagnosticRejected[key] = entry->generation;
                    wxWinUIInputLog("SCROLLBAR projection rejected hwnd=%p object=%ld owner=%ls",
                        static_cast<void*>(key.first), objectId, weak->GetClassInfo()->GetClassName());
                }
                continue;
            }
            self->diagnosticRejected.erase(key);
            wanted[key] = true;
            const bool rtl = weak->GetLayoutDirection() == wxLayout_RightToLeft;
            const bool enabled = weak->IsEnabled();
            try
            {
                ++entry->mutation;
                struct Mutation { unsigned& depth; ~Mutation() { --depth; } } mutation{entry->mutation};
                if ( !SameRect(entry->band, bar.rcScrollBar) || !SameRect(entry->clip, clip) )
                    entry->prepared = false;
                auto peer = entry->peer;
                peer.FlowDirection(rtl ?
                    X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight);
                peer.Minimum(info.nMin);
                peer.Maximum(std::max(info.nMin, info.nMax - static_cast<int>(info.nPage ? info.nPage - 1 : 0)));
                peer.ViewportSize(info.nPage);
                peer.SmallChange(1); peer.LargeChange(std::max(1u, info.nPage));
                peer.Value(info.nPos);
                peer.IsEnabled(enabled && !(bar.rgstate[0] & STATE_SYSTEM_UNAVAILABLE));
                peer.Width((bar.rcScrollBar.right - bar.rcScrollBar.left) / scale);
                peer.Height((bar.rcScrollBar.bottom - bar.rcScrollBar.top) / scale);
                // In the canvas standing for its window, from the client
                // origin: the bar moves, is clipped and is hidden with it.
                RECT ownerClient{};
                if ( !ClientScreen(key.first, ownerClient) ) continue;
                C::Canvas::SetLeft(peer, (bar.rcScrollBar.left - ownerClient.left) / scale);
                C::Canvas::SetTop(peer, (bar.rcScrollBar.top - ownerClient.top) / scale);
                if ( !SameRect(entry->band, bar.rcScrollBar) || !SameRect(entry->clip, clip) )
                {
                    X::Media::RectangleGeometry geometry;
                    geometry.Rect({static_cast<float>((clip.left - bar.rcScrollBar.left) / scale),
                        static_cast<float>((clip.top - bar.rcScrollBar.top) / scale),
                        static_cast<float>((clip.right - clip.left) / scale),
                        static_cast<float>((clip.bottom - clip.top) / scale)});
                    peer.Clip(geometry);
                    entry->backingDirty = true;
                }
                if ( !self->Current(entry) ) return;
                if ( found == self->entries.end() )
                {
                    self->entries.emplace(key, entry); // publish callback authority before Loaded
                }
                if ( !weak ) return;
                host->SetWindowPart(weak.get(), PartKind(objectId), peer);
                if ( !self->Current(entry) ) return;
                SCROLLINFO current{}; SCROLLBARINFO currentBar{}; RECT currentClip{};
                if ( !self->Read(entry, current, currentBar, currentClip) ||
                     !SameRect(bar.rcScrollBar, currentBar.rcScrollBar) || !SameRect(clip,currentClip) ||
                     current.nMin != info.nMin || current.nMax != info.nMax ||
                     current.nPage != info.nPage || current.nPos != info.nPos ) continue;
                entry->band = bar.rcScrollBar; entry->clip = clip;
                entry->prepared = peer.IsLoaded() && peer.ActualWidth() > 0 && peer.ActualHeight() > 0;
                if ( wxWinUIInputLogEnabled() )
                {
                    const auto brush = peer.Background().try_as<X::Media::SolidColorBrush>();
                    const auto colour = brush ? brush.Color() : winrt::Windows::UI::Color{};
                    const auto state = wxString::Format(
                        "prepared=%d pos=%d range=%d,%d page=%u peer=%g mode=%d backgroundARGB=%u,%u,%u,%u rect=%ld,%ld,%ld,%ld",
                        entry->prepared, info.nPos, info.nMin, info.nMax, info.nPage, peer.Value(),
                        static_cast<int>(peer.IndicatorMode()), colour.A, colour.R, colour.G, colour.B,
                        clip.left, clip.top, clip.right, clip.bottom);
                    if ( state != entry->diagnosticState )
                    {
                        entry->diagnosticState = state;
                        wxWinUIInputLog("SCROLLBAR projection hwnd=%p object=%ld %ls",
                            static_cast<void*>(key.first), objectId, state.wc_str());
                        const auto hostState = self->lifetime.lock();
                        auto* const host = hostState ? hostState->GetHost() : nullptr;
                        LogScrollbarSurface(key.first, host ? host->GetBridgeHwnd() : nullptr,
                                            objectId, clip, peer);
                    }
                }
            }
            catch ( const winrt::hresult_error& error )
            {
                entry->prepared = false;
                wxWinUILogException("WinUI window scrollbar projection", error);
            }
        }
    }
    std::vector<std::shared_ptr<Impl::Entry>> removed;
    for ( auto it = self->entries.begin(); it != self->entries.end(); )
    {
        if ( !wanted.count(it->first) ) { removed.push_back(it->second); it = self->entries.erase(it); }
        else ++it;
    }
    for ( const auto& entry : removed )
    {
        if ( !self->Current() ) return;
        self->Remove(entry);
    }
}

bool wxWinUIScrollBarProjection::Covers(HWND hwnd, LONG objectId) const
{
    const auto it = m_impl->entries.find({hwnd,objectId});
    if ( it == m_impl->entries.end() || !it->second->prepared ) return false;
    SCROLLINFO info{}; SCROLLBARINFO bar{}; RECT clip{};
    return m_impl->Read(it->second, info, bar, clip) &&
        SameRect(it->second->band, bar.rcScrollBar) && SameRect(it->second->clip, clip);
}

void wxWinUIScrollBarProjection::CommitRegion()
{
    const auto self = m_impl;
    std::vector<std::shared_ptr<Impl::Entry>> entries;
    for ( const auto& pair : self->entries ) entries.push_back(pair.second);
    std::map<HWND, std::vector<std::shared_ptr<Impl::Entry>>> repaint;
    for ( const auto& entry : entries )
    {
        if ( !self->Current() ) return;
        const bool painted = Covers(entry->hwnd, entry->objectId);
        const bool changed = std::exchange(entry->painted, painted) != painted;
        const bool backingDirty = std::exchange(entry->backingDirty, false);
        if ( (changed || (painted && backingDirty)) && self->Current(entry) )
            repaint[entry->hwnd].push_back(entry);
    }
    for ( const auto& item : repaint )
    {
        if ( !self->Current() ) return;
        std::vector<RECT> bands;
        for ( const auto& entry : item.second )
            if ( self->Current(entry) ) bands.push_back(entry->clip);
        // Clean the shared corner when either of its adjacent tracks changes,
        // but do not add the untouched opposite track to the update region.
        const auto horizontal = self->entries.find({item.first, OBJID_HSCROLL});
        const auto vertical = self->entries.find({item.first, OBJID_VSCROLL});
        if ( horizontal != self->entries.end() && vertical != self->entries.end() &&
             self->Current(horizontal->second) && self->Current(vertical->second) &&
             horizontal->second->painted && vertical->second->painted )
        {
            const RECT& h = horizontal->second->clip;
            const RECT& v = vertical->second->clip;
            bands.push_back({v.left, h.top, v.right, h.bottom});
        }
        if ( !bands.empty() && !InvalidateScrollbarBands(item.first, bands) )
        {
            // An allocation/native invalidation failure must not silently
            // consume the required cleanup. Retry on the next causal flush.
            for ( const auto& entry : item.second )
                if ( self->Current(entry) ) entry->backingDirty = true;
        }
    }
}

void wxWinUIScrollBarProjection::Shutdown()
{
    if ( !m_impl || !m_impl->active ) return;
    m_impl->active = false;
    for ( const auto& pair : m_impl->entries )
    {
        if ( pair.second->indicator ) pair.second->indicator->Retire();
        pair.second->live = false; pair.second->prepared = false; pair.second->painted = false;
    }
    if ( m_impl->root && m_impl->rootPointer )
    {
        for ( const auto& routed : {X::UIElement::PointerMovedEvent(),
                X::UIElement::PointerEnteredEvent(), X::UIElement::PointerPressedEvent(),
                X::UIElement::PointerWheelChangedEvent()} )
            m_impl->root.RemoveHandler(routed, m_impl->rootPointer);
        m_impl->rootPointer = nullptr;
    }
    // The host retains the entire attached graph through Source.Close (and its
    // popup retirement boundary). No native child is removed here.
}

void wxWinUIScrollBarProjection::ObservePointer(HWND target, POINT screen, bool touch)
{
    m_impl->ObservePointer(target, screen, touch);
}

void wxWinUIWindowScrollbarsObserveMessage(const MSG& message) noexcept
{
    if ( message.message != WM_MOUSEMOVE && message.message != WM_MOUSEWHEEL &&
         message.message != WM_MOUSEHWHEEL && message.message != WM_POINTERUPDATE &&
         message.message != WM_POINTERDOWN && message.message != WM_POINTERWHEEL &&
         message.message != WM_POINTERHWHEEL ) return;
    bool touch = false;
    if ( message.message == WM_POINTERUPDATE || message.message == WM_POINTERDOWN ||
         message.message == WM_POINTERWHEEL || message.message == WM_POINTERHWHEEL )
    {
        POINTER_INPUT_TYPE type = PT_POINTER;
        touch = ::GetPointerType(GET_POINTERID_WPARAM(message.wParam), &type) && type == PT_TOUCH;
    }
    try
    {
        const auto projections = gs_projections;
        for ( const auto& weak : projections )
            if ( const auto projection = weak.lock() )
                projection->ObservePointer(message.hwnd, message.pt, touch);
    }
    catch ( const winrt::hresult_error& ) { }
}

bool wxWinUIScrollBarProjection::GetPaintRects(HWND hwnd, std::vector<RECT>& rects) const
{
    RECT horizontal{}, vertical{};
    for ( const LONG objectId : {OBJID_HSCROLL, OBJID_VSCROLL} )
    {
        const auto it = m_impl->entries.find({hwnd,objectId});
        RECT clip{};
        if ( it != m_impl->entries.end() && m_impl->ReadPaintedBand(it->second, clip) )
        {
            rects.push_back(clip);
            (objectId == OBJID_HSCROLL ? horizontal : vertical) = clip;
        }
    }
    if ( !::IsRectEmpty(&horizontal) && !::IsRectEmpty(&vertical) )
    {
        // The reserved corner is not a second scrollbar, but must not retain
        // an opaque USER32 square beneath two transparent native tracks.
        const RECT corner{vertical.left,horizontal.top,vertical.right,horizontal.bottom};
        rects.push_back(corner);
    }
    return !rects.empty();
}

bool wxWinUIScrollBarProjection::Paints(HWND hwnd, LONG objectId) const
{
    const auto it = m_impl->entries.find({hwnd,objectId});
    RECT clip{};
    return it != m_impl->entries.end() && m_impl->ReadPaintedBand(it->second, clip);
}

bool wxWinUIWindowScrollbarsProjected(WXHWND nativeHwnd, int orient)
{
    const HWND hwnd = static_cast<HWND>(nativeHwnd);
    for ( const auto& weak : gs_projections )
        if ( const auto projection = weak.lock() )
        {
            const bool horizontal = projection->Paints(hwnd,OBJID_HSCROLL);
            const bool vertical = projection->Paints(hwnd,OBJID_VSCROLL);
            if ( orient == wxHORIZONTAL ? horizontal : orient == wxVERTICAL ? vertical :
                 orient == wxBOTH ? horizontal && vertical : horizontal || vertical ) return true;
        }
    return false;
}

void wxWinUIScrollBarProjection::ModelChanged(HWND hwnd, int orient, bool nativePaint)
{
    for ( const auto& pair : m_impl->entries )
        if ( pair.first.first == hwnd && pair.first.second ==
            (orient == wxVERTICAL ? OBJID_VSCROLL : OBJID_HSCROLL) )
        {
            ++pair.second->inputSerial;
            // EnableScrollBar and SIF_DISABLENOSCROLL can paint directly even
            // with SetScrollInfo redraw disabled. ModelCommitted cleans an
            // already projected band before the setter returns; the causal
            // host flush remains the fallback for changed/unready geometry.
            // Themed SIF_POS updates can also draw despite fRedraw=FALSE;
            // the position setter uses the same synchronous commit boundary.
            pair.second->backingDirty = pair.second->backingDirty || nativePaint;
        }
}

void wxWinUIScrollBarProjection::ModelCommitted(HWND hwnd, int orient)
{
    const auto self = m_impl;
    const LONG objectId = orient == wxVERTICAL ? OBJID_VSCROLL : OBJID_HSCROLL;
    const auto found = self->entries.find({hwnd, objectId});
    if ( found == self->entries.end() ) return;
    const auto entry = found->second;
    RECT clip{};
    if ( !entry->backingDirty || !self->ReadPaintedBand(entry, clip) ) return;

    // USER32 may have drawn its disabled/enabled scrollbar without sending
    // WM_NCPAINT. A queued RedrawWindow is too late: a transparent XAML track
    // exposes these classic pixels until the next message-pump iteration.
    // Reuse exactly the WM_NCPAINT backing cleanup, restricted to this already
    // committed owner's current band, including page/range updates made from
    // WM_SIZE before the queued peer geometry is ready. This does not draw
    // scrollbar art, send a paint message, run layout, invalidate the client
    // or create a new projection.
    const auto serial = entry->inputSerial;
    HRGN requested = ::CreateRectRgnIndirect(&clip);
    if ( !requested ) return;
    const HRGN filtered = static_cast<HRGN>(wxWinUIWindowScrollbarPaintRegion(
        entry->owner.get(), static_cast<WXHANDLE>(requested)));
    ::DeleteObject(requested);
    if ( filtered )
    {
        ::DeleteObject(filtered);
        if ( self->Current(entry) && entry->inputSerial == serial )
            entry->backingDirty = false;
    }
    // Failed allocation/DC acquisition retains backingDirty, so the existing
    // committed region path still retries instead of silently losing cleanup.
}

void wxWinUIWindowScrollbarsForget(wxWindow* owner)
{
    gs_managedWindows.erase(owner);
}

void wxWinUIWindowScrollbarModelChanged(wxWindow* owner, int orient, bool nativePaint)
{
    if ( !owner || !owner->GetHandle() ) return;
    if ( !owner->IsBeingDeleted() ) gs_managedWindows.insert(owner);
    const HWND hwnd = GetHwndOf(owner);
    for ( const auto& weak : gs_projections )
        if ( const auto projection = weak.lock() )
            projection->ModelChanged(hwnd, orient, nativePaint);
    QueueScrollbarBootstrap(owner);
}

void wxWinUIWindowScrollbarModelCommitted(wxWindow* owner, int orient)
{
    if ( !owner || !owner->GetHandle() ) return;
    const HWND hwnd = GetHwndOf(owner);
    const auto projections = gs_projections;
    for ( const auto& weak : projections )
        if ( const auto projection = weak.lock() )
            projection->ModelCommitted(hwnd, orient);
}

WXHANDLE wxWinUIWindowScrollbarPaintRegion(wxWindow*, WXHANDLE)
{
    // Nothing to filter out of a non-client paint: managed windows reserve no
    // scrollbar band, so USER32 has no classic bar to draw there. Returning
    // no region makes the caller use the requested one unchanged.
    return nullptr;
}

#endif // __WXWINUI__ && wxUSE_WINUI3
