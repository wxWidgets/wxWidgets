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
#include <utility>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;

namespace
{
constexpr wchar_t ManagedProperty[] = L"wxWinUI.ManagedWindowScrollbars";
std::vector<std::weak_ptr<wxWinUIScrollBarProjection>> gs_projections;

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

bool ClientScreen(HWND hwnd, RECT& rect)
{
    if ( !::GetClientRect(hwnd, &rect) ) return false;
    ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    if ( rect.left > rect.right ) std::swap(rect.left, rect.right);
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

// RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME) invalidates the
// entire client as well as the frame. A native scrollbar's backing is only
// non-client: model updates must never repaint the scrolling content merely
// to clean a track. Map each physical band to USER32 client coordinates (the
// two-point RECT mapping also accounts for WS_EX_LAYOUTRTL), then explicitly
// remove the client, including after a resize has moved an old band into it.
bool InvalidateScrollbarBands(HWND hwnd, const std::vector<RECT>& bands)
{
    HRGN update = ::CreateRectRgn(0, 0, 0, 0);
    if ( !update ) return false;
    bool valid = true;
    for ( RECT band : bands )
    {
        ::MapWindowPoints(nullptr, hwnd, reinterpret_cast<POINT*>(&band), 2);
        HRGN part = ::CreateRectRgnIndirect(&band);
        if ( !part || ::CombineRgn(update, update, part, RGN_OR) == ERROR )
            valid = false;
        if ( part ) ::DeleteObject(part);
        if ( !valid ) break;
    }
    RECT client{};
    HRGN clientRegion = ::GetClientRect(hwnd, &client)
        ? ::CreateRectRgnIndirect(&client) : nullptr;
    if ( !clientRegion || ::CombineRgn(update, update, clientRegion, RGN_DIFF) == ERROR )
        valid = false;
    if ( clientRegion ) ::DeleteObject(clientRegion);
    if ( valid )
        valid = ::RedrawWindow(hwnd, nullptr, update,
            RDW_INVALIDATE | RDW_FRAME | RDW_NOCHILDREN) != FALSE;
    ::DeleteObject(update);
    return valid;
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
             !::GetScrollBarInfo(entry->hwnd, entry->objectId, &bar) ||
             (bar.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) ||
             ::IsRectEmpty(&bar.rcScrollBar) ) return false;
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
        return entry;
    }

    void Remove(const std::shared_ptr<Entry>& entry)
    {
        if ( entry->indicator ) entry->indicator->Retire();
        entry->live = false;
        const bool repaint = std::exchange(entry->painted, false);
        entry->prepared = false;
        if ( root )
        {
            uint32_t index;
            if ( root.Children().IndexOf(entry->peer, index) ) root.Children().RemoveAt(index);
        }
        if ( repaint && entry->owner && GetHwndOf(entry->owner.get()) == entry->hwnd &&
             !InvalidateScrollbarBands(entry->hwnd, {entry->clip}) )
        {
            // The entry is already unpublished: ordinary MSW painting is
            // restored even if allocating/requesting the NC redraw failed.
            wxLogWarning("wxWinUI: failed to invalidate a retired scrollbar band");
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
    if ( wxWinUIInputLogEnabled() )
    {
        for ( auto it = self->diagnosticRejected.begin(); it != self->diagnosticRejected.end(); )
            if ( !::IsWindow(it->first.first) ) it = self->diagnosticRejected.erase(it);
            else ++it;
    }
    std::vector<wxWeakRef<wxWindow>> windows;
    std::function<void(wxWindow*)> collect = [&](wxWindow* parent)
    {
        for ( wxWindow* child : parent->GetChildren() )
        {
            if ( ::GetPropW(GetHwndOf(child), ManagedProperty) ) windows.emplace_back(child);
            collect(child);
        }
    };
    collect(self->topLevel.get());
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
            SCROLLBARINFO presence{sizeof(SCROLLBARINFO)};
            const bool hasPresence = ::GetScrollBarInfo(key.first, objectId, &presence) != FALSE;
            if ( !hasPresence ||
                 (presence.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) ||
                 ::IsRectEmpty(&presence.rcScrollBar) ) continue;
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
                C::Canvas::SetLeft(peer, (bar.rcScrollBar.left - clientScreen.left) / scale);
                C::Canvas::SetTop(peer, (bar.rcScrollBar.top - clientScreen.top) / scale);
                // Canvas.ZIndex is bounded by WinUI to one million, not
                // INT_MAX. Keep native non-client bands above ordinary slots
                // without rejecting the entire publication with E_INVALIDARG.
                C::Canvas::SetZIndex(peer, 1000000);
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
                uint32_t nativeIndex;
                if ( !self->root.Children().IndexOf(peer, nativeIndex) )
                {
                    self->root.Children().Append(peer);
                }
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

void wxWinUIWindowScrollbarModelChanged(wxWindow* owner, int orient, bool nativePaint)
{
    if ( !owner || !owner->GetHandle() ) return;
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

WXHANDLE wxWinUIWindowScrollbarPaintRegion(wxWindow* owner, WXHANDLE updateRegion)
{
    if ( !owner ) return nullptr;
    const HWND hwnd = GetHwndOf(owner);
    std::vector<RECT> rects;
    for ( const auto& weak : gs_projections )
        if ( const auto projection = weak.lock() )
            if ( projection->GetPaintRects(hwnd, rects) ) break;
    if ( rects.empty() ) return nullptr;
    RECT window{};
    if ( !::GetWindowRect(hwnd, &window) ) return nullptr;
    HRGN result = ::CreateRectRgnIndirect(&window);
    if ( !result ) return nullptr;
    const HRGN requested = static_cast<HRGN>(updateRegion);
    if ( requested && requested != reinterpret_cast<HRGN>(1) &&
         ::CombineRgn(result, result, requested, RGN_AND) == ERROR )
    { ::DeleteObject(result); return nullptr; }
    HRGN covered = ::CreateRectRgn(0,0,0,0);
    if ( !covered ) { ::DeleteObject(result); return nullptr; }
    for ( const RECT& rect : rects )
    {
        HRGN part = ::CreateRectRgnIndirect(&rect);
        if ( !part || ::CombineRgn(covered, covered, part, RGN_OR) == ERROR )
        { if ( part ) ::DeleteObject(part); ::DeleteObject(covered); ::DeleteObject(result); return nullptr; }
        ::DeleteObject(part);
    }
    if ( ::CombineRgn(result, result, covered, RGN_DIFF) == ERROR )
    { ::DeleteObject(covered); ::DeleteObject(result); return nullptr; }
    // WM_NCPAINT can request only one track or a fragment of it. Preserve
    // that contract for backing cleanup too, rather than clearing every
    // published track each time a neighbouring border needs repainting.
    if ( requested && requested != reinterpret_cast<HRGN>(1) &&
         ::CombineRgn(covered, covered, requested, RGN_AND) == ERROR )
    { ::DeleteObject(covered); ::DeleteObject(result); return nullptr; }
    if ( ::OffsetRgn(covered, -window.left, -window.top) == ERROR )
    { ::DeleteObject(covered); ::DeleteObject(result); return nullptr; }
    HDC dc = ::GetWindowDC(hwnd);
    if ( !dc ) { ::DeleteObject(covered); ::DeleteObject(result); return nullptr; }
    const int saved = ::SaveDC(dc);
    if ( !saved )
    { ::DeleteObject(covered); ::ReleaseDC(hwnd,dc); ::DeleteObject(result); return nullptr; }
    ::SetLayout(dc, 0); // Win32: clearing LAYOUT_RTL selects left-to-right.
    const bool clipped = ::ExtSelectClipRgn(dc, covered, RGN_AND) != ERROR;
    ::DeleteObject(covered);
    const bool material = wxWinUIWindowUsesBackdrop(owner);
    // A black GDI fill has alpha zero on the redirected surface. It exposes
    // the existing DWM material; this is backing cleanup, never scrollbar art.
    wxColour colour = owner->GetBackgroundColour();
    if ( !colour.IsOk() ) colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    HBRUSH brush = ::CreateSolidBrush(material ? RGB(0,0,0) :
        RGB(colour.Red(),colour.Green(),colour.Blue()));
    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("SCROLLBAR NC backing hwnd=%p material=%d background=%u,%u,%u style=%d bands=%zu clipped=%d",
            static_cast<void*>(hwnd), material,
            static_cast<unsigned>(colour.Red()), static_cast<unsigned>(colour.Green()),
            static_cast<unsigned>(colour.Blue()),
            static_cast<int>(owner->GetBackgroundStyle()), rects.size(), clipped);
        RECT dcClip{};
        const int clipKind = ::GetClipBox(dc, &dcClip);
        for ( RECT band : rects )
        {
            ::OffsetRect(&band, -window.left, -window.top);
            const COLORREF before = ::GetPixel(dc, band.left + 2, band.top + 2);
            wxWinUIInputLog(
                "SCROLLBAR NC DC hwnd=%p clipKind=%d clip=%ld,%ld,%ld,%ld band=%ld,%ld,%ld,%ld visible=%d sample=0x%08lx",
                static_cast<void*>(hwnd), clipKind, dcClip.left, dcClip.top, dcClip.right, dcClip.bottom,
                band.left, band.top, band.right, band.bottom,
                ::RectVisible(dc, &band) != FALSE, static_cast<unsigned long>(before));
        }
    }
    bool ok = clipped && brush != nullptr;
    if ( brush && clipped )
    {
        for ( RECT rect : rects )
        {
            ::OffsetRect(&rect, -window.left, -window.top);
            ok = ::FillRect(dc, &rect, brush) != 0 && ok;
        }
    }
    if ( brush ) ::DeleteObject(brush);
    ::RestoreDC(dc,saved); ::ReleaseDC(hwnd,dc);
    if ( !ok ) { ::DeleteObject(result); return nullptr; }
    return static_cast<WXHANDLE>(result);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
