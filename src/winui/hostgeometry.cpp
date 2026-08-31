/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostgeometry.cpp
// Purpose:     Coalesced geometry projection on the existing host and slot authorities
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"
#ifdef WXWINUI_TEST_SUPPORT
    #include "host-geometry-test-access.h"
#endif // WXWINUI_TEST_SUPPORT
#include "hostgeometry.h"
#include "hostnative.h"
#include "hostprofile.h"
#include "hostvisual.h"
#include "hostuia.h"

#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/xamlhost.h"
#include "wx/msw/private.h"
#include "wx/scopeguard.h"
#include "wx/log.h"
#include "wx/toplevel.h"
#include "wx/utils.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
    #include "hosttooltip.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <winrt/Microsoft.UI.Xaml.Automation.h>

namespace
{

// Existing geometry diagnostics and single-shot seams follow their sole
// producers. Input profiling and cursor authority remain host-owned.
unsigned gs_flushSchedules = 0;
unsigned gs_flushRuns = 0;
unsigned gs_flushCallbackAttempts = 0;
unsigned gs_impossibleZOrderOverlaps = 0;
#ifdef WXWINUI_TEST_SUPPORT
std::function<void (wxWindow *)> gs_testSlotSyncedHook;
std::function<void ()> gs_testZOrderSetHook;
#endif // WXWINUI_TEST_SUPPORT

class wxWinUIOwnedRegion
{
public:
    explicit wxWinUIOwnedRegion(HRGN region = nullptr) : m_region(region) {}
    ~wxWinUIOwnedRegion()
    {
        if ( m_region )
            ::DeleteObject(m_region);
    }

    explicit operator bool() const { return m_region != nullptr; }
    HRGN Get() const { return m_region; }
    HRGN Release()
    {
        const HRGN region = m_region;
        m_region = nullptr;
        return region;
    }

private:
    HRGN m_region;

    wxDECLARE_NO_COPY_CLASS(wxWinUIOwnedRegion);
};

} // anonymous namespace

void wxWinUISlot::NudgeDirty()
{
    // One coalesced re-sync pushes geometry and state to the content.
    if ( m_lifetime && m_lifetime->GetHost() && m_window )
        m_lifetime->GetHost()->MarkDirty(m_window);
}

unsigned wxWinUITopLevelHost::GetFlushScheduleCount()
{
    return gs_flushSchedules;
}

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUIHostGeometryTestAccess::GetFlushRunCount()
{
    return gs_flushRuns;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUIHostGeometryTestAccess::GetFlushCallbackAttemptCount()
{
    return gs_flushCallbackAttempts;
}
#endif // WXWINUI_TEST_SUPPORT

unsigned wxWinUITopLevelHost::GetImpossibleZOrderCount()
{
    return gs_impossibleZOrderOverlaps;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostGeometryTestAccess::ResetImpossibleZOrderCount()
{
    gs_impossibleZOrderOverlaps = 0;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostGeometryTestAccess::SetNextSlotSyncedHook(
    std::function<void (wxWindow *)> callback)
{
    gs_testSlotSyncedHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostGeometryTestAccess::SetNextZOrderSetHook(
    std::function<void ()> callback)
{
    gs_testZOrderSetHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

void wxWinUITopLevelHost::SetSlotClipRect(
    wxWindow *window,
    const wxRect& rectDIP)
{
    SetSlotClipRect(window,
                    rectDIP.x,
                    rectDIP.y,
                    rectDIP.width,
                    rectDIP.height);
}

void wxWinUITopLevelHost::SetSlotClipRect(
    wxWindow *window,
    double xDIP,
    double yDIP,
    double widthDIP,
    double heightDIP)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // SetSlotClipRect() is the exact API: normalize negative extents to an
    // intentionally empty clip rather than overloading them as "unbounded".
    // Only the legacy height wrapper below authors its private -1 width
    // sentinel.
    const wxWinUIClipRectDIP clip{
        std::isfinite(xDIP) ? xDIP : 0.0,
        std::isfinite(yDIP) ? yDIP : 0.0,
        std::isfinite(widthDIP) ? wxMax(0.0, widthDIP) : 0.0,
        std::isfinite(heightDIP) ? wxMax(0.0, heightDIP) : 0.0};
    if ( slot->m_hasClipRect && slot->m_clipRectDIP == clip )
        return;

    slot->m_clipRectDIP = clip;
    slot->m_hasClipRect = true;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

void wxWinUITopLevelHost::ClearSlotClip(wxWindow *window)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // Clear is an authored state even when the freshly-created destination
    // slot is already unbounded. During cross-TLW attachment this write must
    // supersede the source slot's older clip instead of being mistaken for
    // untouched revision zero.
    slot->m_clipRectDIP = {};
    slot->m_hasClipRect = false;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

void wxWinUITopLevelHost::SetSlotClipHeight(
    wxWindow *window,
    double heightDIP)
{
    if ( !std::isfinite(heightDIP) || heightDIP <= 0.0 )
    {
        ClearSlotClip(window);
        return;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const wxWinUIClipRectDIP clip{0.0, 0.0, -1.0, heightDIP};
    if ( slot->m_hasClipRect && slot->m_clipRectDIP == clip )
        return;

    slot->m_clipRectDIP = clip;
    slot->m_hasClipRect = true;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

wxWinUIClipRectDIP wxWinUITopLevelHost::PhysicalClipRectToDIP(
    const wxRect& rectPx,
    double scale)
{
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;

    return wxWinUIClipRectDIP{
        rectPx.x / scale,
        rectPx.y / scale,
        wxMax(0, rectPx.width) / scale,
        wxMax(0, rectPx.height) / scale};
}

wxRect wxWinUITopLevelHost::ClipRectDIPToPhysical(
    const wxWinUIClipRectDIP& rectDIP,
    double scale,
    const wxSize& slotSizePx)
{
    if ( !std::isfinite(scale) || scale <= 0.0 ||
         !std::isfinite(rectDIP.x) ||
         !std::isfinite(rectDIP.y) ||
         !std::isfinite(rectDIP.width) ||
         !std::isfinite(rectDIP.height) )
    {
        return wxRect();
    }

    const auto roundToInt = [](double value)
    {
        const double minimum =
            static_cast<double>((std::numeric_limits<int>::min)());
        const double maximum =
            static_cast<double>((std::numeric_limits<int>::max)());
        return static_cast<int>(std::lround(
            wxMax(minimum, wxMin(maximum, value))));
    };
    const int x = roundToInt(rectDIP.x * scale);
    const int y = roundToInt(rectDIP.y * scale);
    const int width = rectDIP.width < 0.0
        ? wxMax(0, slotSizePx.x - x)
        : wxMax(0, roundToInt(rectDIP.width * scale));
    const int height = wxMax(
        0, roundToInt(rectDIP.height * scale));
    return wxRect(x, y, width, height);
}

bool wxWinUITopLevelHost::MarkSlotsUnderAncestor(wxWindow *ancestor)
{
    if ( !ancestor )
        return false;

    bool marked = false;
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( !slot )
            continue;
        if ( entry.first != ancestor &&
             std::find(slot->m_ancestors.begin(),
                       slot->m_ancestors.end(),
                       ancestor) == slot->m_ancestors.end() )
        {
            continue;
        }

        if ( ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
        MarkDirty(entry.first);
        marked = true;
    }
    return marked;
}

void wxWinUITopLevelHost::OnAncestorGeometry(wxEvent& event)
{
    event.Skip();

    wxWindow * const ancestor =
        wxDynamicCast(event.GetEventObject(), wxWindow);
    InvalidateStructure();
    if ( !ancestor )
    {
        // Without the window that moved there is nothing to narrow
        // down: resync everything.
        MarkAllDirty();
        return;
    }

    // Anything under it may have moved on screen or changed visibility, and
    // an ancestor is rarely the only thing which moved: the flush is
    // coalesced, so resync everything rather than guess.
    MarkSlotsUnderAncestor(ancestor);
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnSlotWindowGeometry(wxEvent& event)
{
    event.Skip();

    wxWindow * const window = wxDynamicCast(event.GetEventObject(), wxWindow);
    if ( window && m_slots.count(window) )
    {
        wxWinUISlot * const slot = FindSlot(window);
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
        InvalidateStructure();
        MarkDirty(window);
    }
}

void wxWinUITopLevelHost::OnTLWSize(wxSizeEvent& event)
{
    event.Skip();
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
    }
    InvalidateStructure();
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWDpiChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
    }

    // The island's RasterizationScale follows the monitor by itself; the
    // slot geometry (positions, sizes, clips, scrollbar cutouts) has to be
    // recomputed with the new scale.
    InvalidateStructure();
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWVisibility(wxEvent& event)
{
    event.Skip();

    // Show/iconize events can propagate in port-specific code. Only the TLW
    // controls whether all of this host's slots must be collapsed together.
    if ( event.GetEventObject() == m_tlw )
    {
        InvalidateStructure();
        MarkAllDirty();
    }
}

// Called from wxTopLevelWindowMSW::DoThaw(): run the flush the freeze held
// back.  The notification must come from the TLW override, not from
// wxWindowMSW::DoThaw(): the freeze is detected at the TLW level, and a TLW
// may thaw without any child running the base DoThaw().
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyThaw(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));
    if ( host )
        host->NotifyThaw();
}

void wxWinUITopLevelHost::NotifyThaw()
{
    if ( !m_frozenDirty )
        return;

    m_frozenDirty = false;
    MarkAllDirty();
}

// Called from wxWindowMSW::MSWWindowProc() on WM_ENABLE: EnableWindow() only
// messages the window it was called on, but the effective enabled state of
// every hosted descendant changed with it.  One coalesced re-sync covers
// them all (the per-slot caches make the no-change case cheap).
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyEnable(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost::MarkAllHostsDirty();
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyNativeLayout(wxWindow *window,
                                 bool zOrderMayHaveChanged)
{
    if ( !window )
        return;

    // Both lookups are read-only. In particular, never call ForWindow():
    // moving an ordinary/native window or changing its scrollbar must not
    // instantiate WinUI infrastructure as a side effect.
    wxWinUITopLevelHost * const slotHost =
        wxWinUITopLevelHost::FindSlotOwner(window);
    wxWinUITopLevelHost * const tlwHost =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));

    if ( slotHost )
        slotHost->NotifyNativeLayoutMutation(zOrderMayHaveChanged, window);
    if ( tlwHost && tlwHost != slotHost )
        tlwHost->NotifyNativeLayoutMutation(zOrderMayHaveChanged, window);
}

// Called on the slot-state mutations that produce no geometry event
// (DoSetToolTip, WM_SETTEXT feeding the UIA name): one coalesced re-sync of
// the one window.  No-op for windows without a slot.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifySlotState(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( host )
        host->MarkDirty(window);
}

void wxWinUITopLevelHost::MarkDirty(wxWindow *window)
{
    if ( m_shuttingDown )
        return;

    ++gs_profileMarkDirty;
    m_dirty.insert(window);
    ScheduleFlush();
}

void wxWinUITopLevelHost::InvalidateStructure()
{
    // Zero is reserved for the never-applied initial state. More
    // importantly, this value is never cleared: a nested flush can advance
    // it but cannot hide that fact from an outer z-order snapshot.
    if ( ++m_structureGeneration == 0 )
        ++m_structureGeneration;
}

void wxWinUITopLevelHost::MarkAllDirty()
{
    if ( m_shuttingDown )
        return;

    ++gs_profileMarkAllDirty;

    m_allDirty = true;
    ScheduleFlush();
}

void wxWinUITopLevelHost::NotifyNativeLayoutMutation(
    bool zOrderMayHaveChanged,
    wxWindow *mutated)
{
    if ( m_shuttingDown )
        return;

    ++gs_profileLayoutNotify;

    // The same pass also diagnoses overlaps that the one composition band
    // cannot represent. Geometry/visibility changes can create or remove
    // such an overlap even when USER32's sibling order itself is unchanged.
    InvalidateStructure();
    wxUnusedVar(zOrderMayHaveChanged);
    wxUnusedVar(mutated);

    // Native scrollbars alter the bridge region and a native ancestor move
    // changes every descendant slot. The common coalesced pass is both
    // cheaper and safer than trying to infer the affected descendants here:
    // narrowing it to the slots under the window which moved left the others
    // holding stale geometry, i.e. tool bars which stopped drawing and
    // stopped taking clicks until the next full pass.
    MarkAllDirty();
}

void wxWinUITopLevelHost::ScheduleFlush()
{
    // While the TLW is frozen, schedule nothing at all: each queued flush
    // would just see the freeze and bail out, so a Move/Size storm under
    // Freeze() would keep posting one CallAfter per event.  Note that a
    // catch-up flush is owed instead; wxWinUITLWHostNotifyThaw() -- called
    // from wxTopLevelWindowMSW::DoThaw() -- schedules that single flush at
    // thaw time.
    if ( m_tlw && m_tlw->IsFrozen() )
    {
        m_frozenDirty = true;
        return;
    }

    if ( m_flushScheduled )
        return;

    m_flushScheduled = true;
    ++gs_flushSchedules;
    CallAfter(&wxWinUITopLevelHost::FlushSync);
}

void wxWinUITopLevelHost::FlushSync()
{
    // Every XAML setter and every test/application hook below may destroy the
    // TLW and pump the deferred-delete callback. Keep this allocation alive
    // until the complete outermost flush stack has unwound.
    OperationGuard hostOperation(this);

    ++gs_flushCallbackAttempts;
    m_flushScheduled = false;

    if ( m_shuttingDown || !m_source )
        return;

    if ( wxWinUIInputLogEnabled() )
    {
        // Sampled: a flush STORM (self-rescheduling loop) must be visible in
        // the black box without drowning it.
        static unsigned s_flushCount = 0;
        if ( (++s_flushCount % 32) == 1 )
            wxWinUIInputLog("flush #%u", s_flushCount);
    }

    if ( m_tlw->IsFrozen() )
    {
        // This flush was scheduled before the freeze started (new ones are
        // not posted while frozen, see ScheduleFlush): remember that a
        // catch-up is owed and let wxWinUITLWHostNotifyThaw() schedule it.
        m_frozenDirty = true;
        return;
    }

    ++gs_flushRuns;
    ++gs_profileFlushRuns;

    // A flush re-synchronises every slot of this window. One per input
    // event is a storm, and the profile has to show it: the pointer
    // samples it happens between are far rarer than the flushes.
    if ( wxWinUIInputProfileEnabled() )
    {
        static unsigned s_runs = 0;
        static long s_since = 0;
        const long now = static_cast<long>(::GetTickCount());
        if ( !s_since )
            s_since = now;
        if ( ++s_runs == 50 )
        {
            if ( FILE * const f = wxWinUIInputProfileFile() )
            {
                fprintf(f, "flush: 50 full slot syncs in %ld ms, "
                           "%u slots, layoutnotify=%u (suppressed %u) "
                           "markall=%u markone=%u policy=%u relayout=%u\n",
                        now - s_since,
                        static_cast<unsigned>(m_slots.size()),
                        gs_profileLayoutNotify,
                        gs_profileLayoutSuppressed,
                        gs_profileMarkAllDirty,
                        gs_profileMarkDirty,
                        gs_profileCursorPolicy,
                        wxWinUIGetContentRelayoutCount());
                gs_profileLayoutNotify = 0;
                gs_profileLayoutSuppressed = 0;
                gs_profileMarkAllDirty = 0;
                gs_profileMarkDirty = 0;
                gs_profileCursorPolicy = 0;
                fflush(f);
            }
            s_runs = 0;
            s_since = now;
        }
    }
    ++m_flushRunsForTest;

    EnsureInnerSubclass();
    EnsureBridgeOnTop();
    RebuildBridgeRegion();
    if ( m_shuttingDown )
        return;

    // Snapshot invalidatable callback states, not map iterators or raw window
    // pointers.  XAML property setters below may synchronously emit LostFocus;
    // application code handling the resulting wx event is allowed to destroy
    // any slot and mutate m_slots/m_dirty.
    std::vector<std::shared_ptr<wxWinUISlotLifetime>> pending;
    if ( m_allDirty )
    {
        pending.reserve(m_slots.size());
        for ( const auto& kv : m_slots )
            pending.push_back(kv.second->m_lifetime);
    }
    else
    {
        pending.reserve(m_dirty.size());
        for ( wxWindow *window : m_dirty )
        {
            if ( wxWinUISlot * const slot = FindSlot(window) )
                pending.push_back(slot->m_lifetime);
        }
    }

    // Clear the generation before applying XAML state.  A reentrant dirty
    // notification then belongs to the next generation and must not be
    // erased when this one finishes.
    m_allDirty = false;
    m_dirty.clear();

    // Snapshot the structural epoch before crossing any slot setter. A
    // setter can synchronously mutate a slot already processed (or outside
    // this pending set), advancing the epoch while that slot's cached
    // visible region is still stale. Such a frame must never publish the
    // newer epoch; the dirty notification it raised owns the next pass.
    const unsigned long long syncStructureGeneration =
        m_structureGeneration;

    // Windows reparented into another TLW migrate after the sweep.
    std::vector<std::shared_ptr<wxWinUISlotLifetime>> migrating;
    for ( const auto& state : pending )
    {
        if ( !state || state->GetHost() != this )
            continue;

        wxWindow * const window = state->GetWindow();
        if ( !window || !window->GetHandle() )
            continue;

        wxWinUISlot * const slot = FindSlot(window);
        if ( !slot || slot->m_lifetime != state )
            continue;

        if ( wxGetTopLevelParent(window) != m_tlw )
        {
            migrating.push_back(state);
            continue;
        }

        SyncSlot(*slot);
        if ( m_shuttingDown )
            return;
    }

    if ( m_structureGeneration == syncStructureGeneration &&
         m_structureAppliedGeneration != syncStructureGeneration )
    {
        // Publish only a pass which completed against this exact epoch. A
        // nested FlushSync() is free to publish a NEWER generation; the outer
        // pass must then fail without overwriting it.
        if ( RecomputeZOrder(syncStructureGeneration) )
            m_structureAppliedGeneration = syncStructureGeneration;
        if ( m_shuttingDown )
            return;
    }

    for ( const auto& state : migrating )
    {
        if ( !state || state->GetHost() != this )
            continue;

        wxWindow * const window = state->GetWindow();
        if ( window )
            MigrateSlotToCurrentTLW(window);
        if ( m_shuttingDown )
            return;
    }

    RetryDeferredFocusAfterFlush();
    if ( m_shuttingDown )
        return;

    // Re-hit-test the last real point after layout/re-template/animation:
    // the former slot may now expose another slot or a native child. This is
    // skipped until the root has observed an actual point.
    if ( m_lastPointerScreen.x > -50000 &&
         m_lastPointerScreen.y > -50000 )
    {
        RefreshPointerCursorAtLastPoint();
    }
    else if ( m_islandPointerCursorApplied ||
              m_activeCursorSurface != ActiveCursorSurface::GenericXaml )
    {
        // A test seam can establish the active surface before the first real
        // routed point, and an InputPointerSource override may be reset by a
        // XAML re-template even while the pointer is stationary. Reassert the
        // already-selected surface after the sync instead of silently leaving
        // the runtime's default Arrow in force.
        RefreshActivePointerCursor();
    }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    // Native OLE ownership is acquired only after the first visible XAML
    // presentation and this pass's bridge geometry/z-order publication.
    if ( !m_shuttingDown )
        TryInitializeDropBrokerAfterPresentation();
#endif
}

// The single revalidation used by geometry, tooltip policy and UIA.
// Its comparisons retain the original ordering and callback boundaries.
wxWinUISlot *wxWinUITopLevelHost::ResolveSlotSync(
    const wxWinUISlotSyncTransaction& transaction)
{
    auto& state = transaction.state;
    auto& w = transaction.w;
    auto& content = transaction.content;
    auto& semanticTarget = transaction.semanticTarget;
    auto& contentGeneration = transaction.contentGeneration;
    auto& syncEpoch = transaction.syncEpoch;
    auto& automationNameStateRevision =
        transaction.automationNameStateRevision;
    auto& automationNameStyleRevision =
        transaction.automationNameStyleRevision;
    auto& automationNameImplicitStyleRevision =
        transaction.automationNameImplicitStyleRevision;
    auto& clipRevision = transaction.clipRevision;
    auto& automationNameOverrideRevision =
        transaction.automationNameOverrideRevision;
#if wxUSE_TOOLTIPS
    auto& toolTipPolicyEnabled = transaction.toolTipPolicyEnabled;
    auto& tip = transaction.tip;
    auto& toolTipText = transaction.toolTipText;
    auto& toolTipMaximumWidth = transaction.toolTipMaximumWidth;
    auto& toolTipDPI = transaction.toolTipDPI;
#endif

    if ( !state || state->GetHost() != this || state->GetWindow() != w )
        return nullptr;

    wxWinUISlot * const current = FindSlot(w);
    if ( !current ||
         current->m_lifetime != state ||
         current->m_content != content ||
         current->m_semanticTarget != semanticTarget ||
          current->m_contentGeneration != contentGeneration ||
          current->m_syncEpoch != syncEpoch ||
          current->m_uiaNameStateRevision !=
              automationNameStateRevision ||
          current->m_automationNameStyleRevision !=
              automationNameStyleRevision ||
          current->m_automationNameImplicitStyleRevision !=
              automationNameImplicitStyleRevision ||
          current->m_clipRevision != clipRevision ||
          current->m_contentTransactionInProgress ||
         current->m_automationNameOverrideRevision !=
             automationNameOverrideRevision )
    {
        return nullptr;
    }

#if wxUSE_TOOLTIPS
    if ( wxWinUIAreToolTipsEnabled() != toolTipPolicyEnabled )
        return nullptr;

    wxToolTip * const currentTip = w->GetToolTip();
    if ( currentTip != tip )
        return nullptr;
    if ( currentTip )
    {
        const int currentMaximumWidth =
            currentTip->GetWinUIMaxWidthAtCreation();
        const int currentDPI =
            currentMaximumWidth > 0 ? w->GetDPI().GetWidth() : 0;
        if ( currentTip->GetTip() != toolTipText ||
             currentMaximumWidth != toolTipMaximumWidth ||
             currentDPI != toolTipDPI )
        {
            return nullptr;
        }
    }
#endif
    return current;
}

void wxWinUITopLevelHost::SyncSlot(wxWinUISlot& slot)
{
    wxWinUISlot::OperationGuard slotOperation(&slot);

    // This epoch is per-entry, not per-content. A nested FlushSync() over the
    // same slot always supersedes its outer caller, even when both observe the
    // same element generation.
    if ( ++slot.m_syncEpoch == 0 )
        ++slot.m_syncEpoch;
    const unsigned long long syncEpoch = slot.m_syncEpoch;

    // SetContent() has already crossed (or is about to cross) a carrier
    // setter, but has not yet published the matching model. Never sync that
    // deliberately unobservable state. Requeue one coalesced pass for after
    // the transaction publishes commit or rollback.
    if ( slot.m_contentTransactionInProgress )
    {
        slot.NudgeDirty();
        return;
    }

    if ( slot.m_contentLoadedObserverPending )
    {
        const bool observerInstalled =
            slot.InstallContentLoadedObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_contentLoadedObserverPending )
        {
            slot.NudgeDirty();
            return;
        }
    }

    if ( slot.m_toolTipPropertyObserverPending )
    {
        const bool observerInstalled =
            slot.InstallToolTipPropertyObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_toolTipPropertyObserverPending )
        {
            // A nested replacement made this registration attempt obsolete.
            // Let the new generation's own observer and dirty pass win.
            slot.NudgeDirty();
            return;
        }
    }

    if ( slot.m_automationNameStylePropertyObserverPending )
    {
        const bool observerInstalled =
            slot.InstallAutomationNameStylePropertyObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_automationNameStylePropertyObserverPending )
        {
            // A nested replacement made this registration attempt obsolete.
            slot.NudgeDirty();
            return;
        }
    }

    // RegisterPropertyChangedCallback observes effective values, not their
    // source. In particular, local x:Null -> UnsetValue can be silent when
    // FrameworkElement::Style() remains null while an implicit Style starts
    // contributing setters. Re-snapshot the source whenever the host already
    // has another reason to synchronize this slot. This is bounded by normal
    // wx dirty work and avoids a permanent Rendering/timer subscription.
    if ( const auto observerState =
             slot.m_automationNameStylePropertyState )
    {
        const auto framework =
            slot.m_automationNameStyleObservedContent;
        if ( observerState->active && framework )
        {
            try
            {
                if ( wxWinUIUpdateLocalStyleSourceSnapshot(
                         observerState, framework) )
                {
                    if ( ++slot.m_automationNameStyleRevision == 0 )
                        ++slot.m_automationNameStyleRevision;
                    if ( ++slot.m_automationNameImplicitStyleRevision == 0 )
                        ++slot.m_automationNameImplicitStyleRevision;
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                // Leave the old snapshot intact so a later ordinary sync can
                // retry instead of falsely declaring the source current.
                wxWinUILogException(
                    "snapshot automation Style source during sync", e);
            }
        }
    }

    // Snapshot every wx/slot value before crossing into a XAML setter.  Any
    // setter may synchronously raise LostFocus; application code handling the
    // synthesized wx event may unregister this slot.  From the first setter
    // until final revalidation, use only strong local XAML references and the
    // invalidatable lifetime state -- never the slot reference again.
    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;
    wxWindow * const w = state ? state->GetWindow() : nullptr;
    const HWND hwnd = GetHwndOf(w);
    if ( !hwnd || !slot.m_container )
        return;

    const WXHWND wxHwnd = w->GetHWND();
    const auto container = slot.m_container;
    const auto carrier = slot.m_carrier;
    const auto content = slot.m_content;
    const auto semanticTarget = slot.m_semanticTarget;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    const wxWinUIClipRectDIP clipRectDIP = slot.m_clipRectDIP;
    const bool hasClipRect = slot.m_hasClipRect;
    const unsigned long long clipRevision = slot.m_clipRevision;
    const int previousEnabled = slot.m_lastEnabled;
#if wxUSE_TOOLTIPS
    const bool previousToolTipSynced = slot.m_toolTipSynced;
    const bool previousToolTipOwned = slot.m_toolTipOwned;
    const bool previousToolTipSuppressed = slot.m_toolTipSuppressed;
    const bool previousToolTipPresent = slot.m_lastToolTipPresent;
    const wxString previousToolTip = slot.m_lastToolTip;
    const int previousToolTipMaximumWidth =
        slot.m_lastToolTipMaximumWidth;
    const int previousToolTipDPI = slot.m_lastToolTipDPI;
    const auto previousAppliedToolTip = slot.m_lastAppliedToolTip;
    const auto previousOriginalToolTip = slot.m_originalToolTip;
    const auto previousOriginalToolTipLocalValue =
        slot.m_originalToolTipLocalValue;
#endif
    const bool previousHelpTextSynced = slot.m_helpTextSynced;
    const bool previousHelpTextOwned = slot.m_helpTextOwned;
    const bool previousHelpTextPresent =
        slot.m_lastHelpTextPresent;
    const wxString previousHelpText = slot.m_lastHelpText;
    const wxString previousAppliedHelpText =
        slot.m_lastAppliedHelpText;
    const auto previousOriginalHelpTextLocalValue =
        slot.m_originalHelpTextLocalValue;
    const bool previousUIANameSynced = slot.m_uiaNameSynced;
    const bool previousUIANameOwned = slot.m_uiaNameOwned;
    const bool previousUIANameAppOwned = slot.m_uiaNameAppOwned;
    const bool previousUIANameRelinquished =
        slot.m_uiaNameRelinquished;
    const bool previousUIANameSuppressedByLabeledBy =
        slot.m_uiaNameSuppressedByLabeledBy;
    const wxString previousUIAName = slot.m_lastUIAName;
    const wxString previousAppliedUIAName = slot.m_lastAppliedUIAName;
    const auto previousOriginalUIANameLocalValue =
        slot.m_originalUIANameLocalValue;
    unsigned long long automationNameStateRevision =
        slot.m_uiaNameStateRevision;
    const unsigned long long automationNameStyleRevision =
        slot.m_automationNameStyleRevision;
    const unsigned long long automationNameImplicitStyleRevision =
        slot.m_automationNameImplicitStyleRevision;
    const unsigned long long
        previousAutomationNameImplicitStyleInspectedRevision =
            slot.m_automationNameImplicitStyleInspectedRevision;
    const bool hasAutomationNameOverride =
        slot.m_hasAutomationNameOverride;
    const wxString automationNameOverride =
        slot.m_automationNameOverride;
    const unsigned long long automationNameOverrideRevision =
        slot.m_automationNameOverrideRevision;
    const bool hasApplicationAuthoredXaml =
        wxDynamicCast(w, wxWinUIXamlHost) != nullptr;
#if wxUSE_ACCESSIBILITY
    const bool hasWxAccessible = w->GetAccessible() != nullptr;
#else
    const bool hasWxAccessible = false;
#endif
    const bool previousCursorSynced = slot.m_cursorSynced;
    const wxCursor previousCursor = slot.m_lastCursor;
    const unsigned long long previousCursorPolicyGeneration =
        slot.m_lastCursorPolicyGeneration;
    const winrt::com_ptr<wxWinUISlotGrid> slotGrid =
        wxWinUIGetSlotGrid(&slot);

    const HWND hwndTLW = GetHwndOf(m_tlw);

    // Control rect in TLW client coordinates (physical px) -- the island
    // covers exactly the TLW client area.  The TLW itself can be slotted too
    // (the Window-presentation dialog shell hosts its whole content this
    // way): its slot simply covers the client area.
    wxRect rect;
    if ( w == m_tlw )
    {
        RECT rc;
        ::GetClientRect(hwndTLW, &rc);
        rect = wxRect(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    }
    else
    {
        // All island geometry is physical-LTR: window rects are physical
        // already, so anchor them on the physical client origin.  (See
        // GetClientScreenRect for why ScreenToClient must not be used.)
        const RECT rcCS = GetClientScreenRect();
        RECT wr;
        ::GetWindowRect(hwnd, &wr);
        rect = wxRect(wr.left - rcCS.left, wr.top - rcCS.top,
                      wr.right - wr.left, wr.bottom - wr.top);
    }

    // Clip against every ancestor's client area, like HWND clipping does.
    wxRect vis = rect;

    const double scale = GetScale();
    if ( hasClipRect )
    {
        const wxRect localLimit = ClipRectDIPToPhysical(
            clipRectDIP, scale, rect.GetSize());
        const wxRect limit(rect.x + localLimit.x,
                           rect.y + localLimit.y,
                           localLimit.width,
                           localLimit.height);
        vis.Intersect(limit);
    }
    if ( w != m_tlw )
    {
        const RECT rcCS = GetClientScreenRect();
        for ( wxWindow *p = w->GetParent();
              p && p != m_tlw && !vis.IsEmpty();
              p = p->GetParent() )
        {
            const HWND hp = GetHwndOf(p);
            if ( !hp )
                break;

            RECT rc;
            ::GetClientRect(hp, &rc);
            ::MapWindowPoints(hp, nullptr, reinterpret_cast<POINT *>(&rc), 2);
            if ( rc.left > rc.right )
            {
                const LONG swap = rc.left;
                rc.left = rc.right;
                rc.right = swap;
            }

            vis.Intersect(wxRect(rc.left - rcCS.left, rc.top - rcCS.top,
                                 rc.right - rc.left, rc.bottom - rc.top));
        }
    }

    const auto direction =
        w->GetLayoutDirection() == wxLayout_RightToLeft
            ? winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft
            : winrt::Microsoft::UI::Xaml::FlowDirection::LeftToRight;
    // IsShownOnScreen() does not make minimization an explicit part of its
    // contract. The XAML tree must not remain live/hit-testable while its TLW
    // is hidden or iconic, even if every wx child still has its shown bit.
    const bool tlwVisible =
        hwndTLW && ::IsWindowVisible(hwndTLW) && !::IsIconic(hwndTLW);
    const bool effectiveVisible =
        tlwVisible && w->IsShownOnScreen() && !vis.IsEmpty();
    const bool enabled = w->IsEnabled();

#if wxUSE_TOOLTIPS
    wxToolTip * const tip = w->GetToolTip();
    const bool toolTipPresent = tip != nullptr;
    const wxString toolTipText = tip ? tip->GetTip() : wxString();
    const int toolTipMaximumWidth =
        tip ? tip->GetWinUIMaxWidthAtCreation() : 0;
    const int toolTipDPI =
        toolTipMaximumWidth > 0 ? w->GetDPI().GetWidth() : 0;
    const bool wxToolTipChanged =
        previousToolTipPresent != toolTipPresent ||
        previousToolTip != toolTipText ||
        previousToolTipMaximumWidth != toolTipMaximumWidth ||
        previousToolTipDPI != toolTipDPI;
    const bool toolTipPolicyEnabled = wxWinUIAreToolTipsEnabled();
    bool nextToolTipOwned = previousToolTipOwned;
    auto nextAppliedToolTip = previousAppliedToolTip;
    auto nextOriginalToolTip = previousOriginalToolTip;
    auto nextOriginalToolTipLocalValue =
        previousOriginalToolTipLocalValue;
#endif

    wxString helpText;
#if wxUSE_HELP
    helpText = w->GetHelpText();
#endif
    bool helpTextPresent = !helpText.empty();
#if wxUSE_TOOLTIPS
    if ( !helpTextPresent && toolTipPresent )
    {
        helpText = toolTipText;
        helpTextPresent = true;
    }
#endif
    const bool wxHelpTextChanged =
        previousHelpTextPresent != helpTextPresent ||
        previousHelpText != helpText;
    bool nextHelpTextOwned = previousHelpTextOwned;
    wxString nextAppliedHelpText = previousAppliedHelpText;
    auto nextOriginalHelpTextLocalValue =
        previousOriginalHelpTextLocalValue;

    const wxString uiaLabel =
        hasAutomationNameOverride
            ? wxStripMenuCodes(automationNameOverride, wxStrip_Mnemonics)
            : wxWinUIGetAutomationName(w);
    bool nextUIANameOwned = previousUIANameOwned;
    bool nextUIANameAppOwned = previousUIANameAppOwned;
    bool nextUIANameRelinquished = previousUIANameRelinquished;
    bool nextUIANameSuppressedByLabeledBy =
        previousUIANameSuppressedByLabeledBy;
    wxString nextAppliedUIAName = previousAppliedUIAName;
    auto nextOriginalUIANameLocalValue =
        previousOriginalUIANameLocalValue;

    // Native WM_SETCURSOR walks up the parent chain until a window supplies
    // a cursor. A XAML slot is a separate visual tree and never gets that
    // walk, so resolve the same inherited value here. Parent cursor changes
    // mark the whole host dirty (see wxWinUISetWindowCursor()).
    wxCursor effectiveCursor;
    winrt::Microsoft::UI::Input::InputCursor effectiveInputCursor{ nullptr };
    const unsigned long long resolvedCursorPolicyGeneration =
        wxWinUIGetCursorPolicyGeneration();
    const bool effectiveCursorResolved =
        ResolveEffectivePointerCursor(
            w, &effectiveInputCursor, &effectiveCursor);
    const bool cursorChanged =
        !previousCursorSynced ||
        previousCursorPolicyGeneration !=
            resolvedCursorPolicyGeneration ||
        !previousCursor.IsSameAs(effectiveCursor);

    // Safe after any reentrant XAML callback: the shared state is invalidated
    // before a slot is removed.  Only return a slot whose generation is still
    // exactly the one snapshotted above.
    // Bind existing stack snapshots only: no getter, allocation or new
    // owning reference is evaluated at this module boundary.
    const wxWinUISlotSyncTransaction transaction
    {
        state,
        w,
        content,
        semanticTarget,
        contentGeneration,
        syncEpoch,
        automationNameStateRevision,
        automationNameStyleRevision,
        automationNameImplicitStyleRevision,
        clipRevision,
        automationNameOverrideRevision,
#if wxUSE_TOOLTIPS
        toolTipPolicyEnabled,
        tip,
        toolTipText,
        toolTipMaximumWidth,
        toolTipDPI,
#endif
        slot,
        previousHelpTextSynced,
        previousAppliedHelpText,
        helpTextPresent,
        helpText,
        wxHelpTextChanged,
        nextHelpTextOwned,
        nextAppliedHelpText,
        nextOriginalHelpTextLocalValue,
        previousUIANameSynced,
        previousAutomationNameImplicitStyleInspectedRevision,
        hasApplicationAuthoredXaml,
        previousUIANameRelinquished,
        previousUIANameOwned,
        previousUIANameAppOwned,
        previousUIANameSuppressedByLabeledBy,
        previousUIAName,
        previousAppliedUIAName,
        uiaLabel,
        nextUIANameOwned,
        nextUIANameAppOwned,
        nextUIANameRelinquished,
        nextUIANameSuppressedByLabeledBy,
        nextAppliedUIAName,
        nextOriginalUIANameLocalValue,
    };

    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        return ResolveSlotSync(transaction);
    };

    SyncSlotAccessibilityAuthority(slot, hasWxAccessible);
    if ( !getCurrentSlot() )
        return;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using winrt::Microsoft::UI::Xaml::Controls::Canvas;

        Canvas::SetLeft(container, rect.x / scale);
        Canvas::SetTop(container, rect.y / scale);
        container.Width(rect.width / scale);
        container.Height(rect.height / scale);
        if ( !getCurrentSlot() )
            return;

        // Follow dynamic direction changes (SetLayoutDirection after
        // creation): the hosted content mirrors, the positions above stay
        // physical.
        if ( container.FlowDirection() != direction )
            container.FlowDirection(direction);

        if ( vis == rect || vis.IsEmpty() )
        {
            container.Clip(nullptr);
        }
        else
        {
            winrt::Microsoft::UI::Xaml::Media::RectangleGeometry geometry;
            geometry.Rect(winrt::Windows::Foundation::Rect
            {
                float((vis.x - rect.x) / scale),
                float((vis.y - rect.y) / scale),
                float(vis.width / scale),
                float(vis.height / scale)
            });
            container.Clip(geometry);
        }
        if ( !getCurrentSlot() )
            return;

        container.Visibility(effectiveVisible ? Visibility::Visible
                                              : Visibility::Collapsed);
        if ( !getCurrentSlot() )
            return;

        // ----- state adapter -----
        // The flush is the one common seam every slot goes through: re-assert
        // the wx-side state here instead of overriding 30 components.  The
        // caches keep the no-change case to a few comparisons.

        // Effective enabled: IsEnabled() folds the ancestors in, which the
        // per-control implementations cannot see (a disabled panel sends no
        // message to its children).  The state goes on the host-owned
        // CARRIER, never on the content itself: Control::IsEnabled(false)
        // is inherited by every descendant control (keyboard focus and UIA
        // state included, composite contents too), while the content's own
        // IsEnabled -- possibly intrinsically false -- is left alone.
        const bool enabledStateNeedsSync =
            previousEnabled != static_cast<int>(enabled) ||
            !carrier ||
            carrier.IsEnabled() != enabled ||
            container.IsHitTestVisible() != enabled;
        if ( enabledStateNeedsSync )
        {
            if ( !carrier )
                return;

            carrier.IsEnabled(enabled);
            if ( !getCurrentSlot() )
                return;

            // IsEnabled callbacks may change the wx effective state or enter
            // a nested flush. Do not apply/cache the old hit-test verdict.
            if ( w->IsEnabled() != enabled ||
                 carrier.IsEnabled() != enabled )
            {
                MarkDirty(w);
                return;
            }

            container.IsHitTestVisible(enabled);
            if ( !getCurrentSlot() )
                return;
            if ( w->IsEnabled() != enabled ||
                 carrier.IsEnabled() != enabled ||
                 container.IsHitTestVisible() != enabled )
            {
                MarkDirty(w);
                return;
            }
        }

#if wxUSE_TOOLTIPS
        // Tooltip ownership is explicit. A tooltip authored in the XAML
        // survives the initial sync untouched. Once wx SetToolTip() supplies
        // an override we remember the exact previous IInspectable (which may
        // be a ToolTip object) and restore it on UnsetToolTip(). A direct
        // application mutation while wx owns the property relinquishes that
        // ownership instead of being overwritten by the next dirty flush.
        if ( content )
        {
            ++slot.m_toolTipPolicySyncDepth;
            const auto toolTipPolicySyncGuard = wxMakeGuard([&slot]()
            {
                wxASSERT_MSG(slot.m_toolTipPolicySyncDepth != 0,
                             "unbalanced WinUI tooltip slot transaction");
                --slot.m_toolTipPolicySyncDepth;
            });
            wxUnusedVar(toolTipPolicySyncGuard);

            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

            const auto actual = ToolTipService::GetToolTip(content);
            const auto actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return;

            if ( !previousToolTipSynced )
            {
                nextOriginalToolTip = actual;
                nextOriginalToolTipLocalValue = actualLocal;
                nextToolTipOwned = false;
                nextAppliedToolTip = nullptr;
                if ( toolTipPresent )
                {
                    if ( !wxWinUISetToolTip(content,
                                            toolTipText,
                                            w,
                                            &nextAppliedToolTip,
                                            toolTipMaximumWidth) )
                        return;
                    nextToolTipOwned =
                        toolTipText.empty() ||
                        static_cast<bool>(nextAppliedToolTip);
                }
            }
            else
            {
                const bool applicationClearedSuppressedNull =
                    previousToolTipSuppressed &&
                    actualLocal ==
                        DependencyProperty::UnsetValue();
                if ( applicationClearedSuppressedNull )
                {
                    if ( previousToolTipOwned &&
                         previousAppliedToolTip &&
                         !wxWinUIRestoreToolTip(
                             content,
                             DependencyProperty::UnsetValue(),
                             previousAppliedToolTip) )
                    {
                        return;
                    }
                    if ( !getCurrentSlot() )
                        return;
                    nextToolTipOwned = false;
                    nextAppliedToolTip = nullptr;
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                }

                if ( previousToolTipSuppressed &&
                     !applicationClearedSuppressedNull &&
                     actual )
                {
                    const bool restoredManagedValue =
                        previousToolTipOwned &&
                        actual == previousAppliedToolTip &&
                        wxWinUIIsManagedToolTipCurrent(
                            content, previousAppliedToolTip);
                    if ( !restoredManagedValue )
                    {
                        // A non-null value appearing while our global policy
                        // owns the null is a direct application mutation.
                        // Preserve it as the new baseline. When policy is
                        // still disabled it is detached again below; during
                        // re-enable it stays visible.
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                        nextOriginalToolTip = actual;
                        nextOriginalToolTipLocalValue = actualLocal;
                        if ( previousToolTipOwned &&
                             previousAppliedToolTip &&
                             !wxWinUIRestoreToolTip(
                                 content,
                                 winrt::Microsoft::UI::Xaml::
                                     DependencyProperty::UnsetValue(),
                                 previousAppliedToolTip) )
                        {
                            return;
                        }
                        if ( !getCurrentSlot() )
                            return;
                    }
                }

                if ( !previousToolTipSuppressed &&
                     nextToolTipOwned &&
                     actual != previousAppliedToolTip &&
                     !wxWinUIIsManagedToolTipSuppressed(
                         content, previousAppliedToolTip) )
                {
                    // The application replaced our exact object directly.
                    // Text equality is irrelevant to attached-property
                    // ownership.
                    nextToolTipOwned = false;
                    nextAppliedToolTip = nullptr;
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                    if ( previousAppliedToolTip &&
                         !wxWinUIRestoreToolTip(
                             content,
                             winrt::Microsoft::UI::Xaml::
                                 DependencyProperty::UnsetValue(),
                             previousAppliedToolTip) )
                        return;
                    if ( !getCurrentSlot() )
                        return;
                }
                else if ( !nextToolTipOwned &&
                          !previousToolTipSuppressed &&
                          !wxToolTipChanged )
                {
                    // Track direct authored-XAML changes even when no wx-side
                    // value changed. This baseline is what a later global
                    // disable/enable transaction must restore.
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                }

                if ( previousToolTipSuppressed &&
                     toolTipPolicyEnabled )
                {
                    // Re-enable is a restore transaction, not an application
                    // mutation: null is the value installed by the previous
                    // global-disable pass.
                    if ( nextToolTipOwned && toolTipPresent )
                    {
                        if ( actual != nextAppliedToolTip ||
                             wxToolTipChanged )
                        {
                            if ( !wxWinUISetToolTip(content,
                                                   toolTipText,
                                                   w,
                                                   &nextAppliedToolTip,
                                                   toolTipMaximumWidth) )
                                return;
                            nextToolTipOwned =
                                toolTipText.empty() ||
                                static_cast<bool>(nextAppliedToolTip);
                        }
                    }
                    else
                    {
                        if ( !actualLocal )
                        {
                            wxWinUIRestoreToolTipLocalValue(
                                content,
                                nextOriginalToolTipLocalValue);
                        }
                        else
                        {
                            nextOriginalToolTip = actual;
                            nextOriginalToolTipLocalValue =
                                actualLocal;
                        }
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                    }
                }
                else if ( nextToolTipOwned )
                {
                    if ( !toolTipPresent )
                    {
                        bool restored = true;
                        if ( nextAppliedToolTip )
                        {
                            restored = wxWinUIRestoreToolTip(
                                content,
                                nextOriginalToolTipLocalValue,
                                nextAppliedToolTip);
                        }
                        else
                        {
                            // An explicit empty wx tooltip owns null without a
                            // registry object. Restore only while null is still
                            // authoritative; a direct XAML object wins.
                            const auto actualNow =
                                ToolTipService::GetToolTip(content);
                            if ( !actualNow &&
                                 !wxWinUIReadToolTipLocalValue(content) )
                            {
                                wxWinUIRestoreToolTipLocalValue(
                                    content,
                                    nextOriginalToolTipLocalValue);
                            }
                        }
                        if ( !restored )
                        {
                            return;
                        }
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                    }
                    else if ( wxToolTipChanged )
                    {
                        if ( !wxWinUISetToolTip(content,
                                                toolTipText,
                                                w,
                                                &nextAppliedToolTip,
                                                toolTipMaximumWidth) )
                            return;
                        nextToolTipOwned =
                            toolTipText.empty() ||
                            static_cast<bool>(nextAppliedToolTip);
                    }
                }
                else if ( toolTipPresent && wxToolTipChanged )
                {
                    // A new explicit wx-side value reclaims ownership,
                    // preserving whatever XAML value currently exists.
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                    if ( !wxWinUISetToolTip(content,
                                            toolTipText,
                                            w,
                                            &nextAppliedToolTip,
                                            toolTipMaximumWidth) )
                        return;
                    nextToolTipOwned =
                        toolTipText.empty() ||
                        static_cast<bool>(nextAppliedToolTip);
                }
            }

            if ( !getCurrentSlot() )
                return;

            // ToolTipService normally reacts to target state itself, but an
            // explicitly-open ToolTip can otherwise outlive a wx Hide() or
            // Disable() transition. Close the current object without
            // detaching it, so showing/enabling the slot preserves ownership
            // and lets WinUI resume its normal pointer/focus activation.
            if ( toolTipPolicyEnabled &&
                 (!enabled || !effectiveVisible) )
            {
                const auto visible =
                    ToolTipService::GetToolTip(content);
                if ( const auto object =
                         visible.try_as<
                             winrt::Microsoft::UI::Xaml::Controls::ToolTip>();
                     object && object.IsOpen() )
                {
                    object.IsOpen(false);
                    if ( !getCurrentSlot() )
                        return;

                    // IsOpen is a re-entrant dependency property. If a Closed
                    // handler replaced the attached value, abandon this stale
                    // snapshot and let the property observer resynchronize it.
                    if ( ToolTipService::GetToolTip(content) != visible )
                    {
                        MarkDirty(w);
                        return;
                    }
                }
            }

            bool nextToolTipSuppressed = false;
            if ( !toolTipPolicyEnabled )
            {
                // WinUI exposes no process-wide ToolTipService switch.
                // Close and detach every synchronous application replacement
                // in this same call. Null is ours only after an exact
                // post-setter re-read.
                const wxWinUIToolTipSuppressionResult result =
                    wxWinUISuppressSlotToolTip(
                        content, getCurrentSlot,
                        nextToolTipOwned,
                        nextAppliedToolTip,
                        nextOriginalToolTip,
                        nextOriginalToolTipLocalValue);
                if ( result !=
                        wxWinUIToolTipSuppressionResult::Suppressed )
                {
                    if ( result ==
                            wxWinUIToolTipSuppressionResult::BudgetExhausted )
                    {
                        wxLogWarning(
                            "wxWinUI: synchronous tooltip suppression "
                            "replacement budget exhausted");
                        if ( wxWinUISlot * const current = getCurrentSlot() )
                            current->NudgeDirty();
                    }
                    else if ( result ==
                              wxWinUIToolTipSuppressionResult::Failed )
                    {
                        if ( wxWinUISlot * const current = getCurrentSlot() )
                            current->NudgeDirty();
                    }
                    return;
                }
                nextToolTipSuppressed = true;
            }

            if ( wxWinUISlot * const current = getCurrentSlot() )
                current->m_toolTipSuppressed = nextToolTipSuppressed;
            else
                return;
        }

        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            current->m_toolTipSynced = true;
            current->m_toolTipOwned = nextToolTipOwned;
            current->m_lastToolTipPresent = toolTipPresent;
            current->m_lastToolTip = toolTipText;
            current->m_lastToolTipMaximumWidth =
                toolTipMaximumWidth;
            current->m_lastToolTipDPI = toolTipDPI;
            current->m_lastAppliedToolTip = nextAppliedToolTip;
            current->m_originalToolTip = nextOriginalToolTip;
            current->m_originalToolTipLocalValue =
                nextOriginalToolTipLocalValue;
        }
        else
            return;
#endif // wxUSE_TOOLTIPS

        if ( !SyncSlotAutomationProperties(transaction) )
            return;

        if ( cursorChanged )
        {
            // A missing implementation object is a transient registration
            // failure: leave the cache uncommitted so the next flush retries.
            if ( !effectiveCursorResolved ||
                 wxWinUIGetCursorPolicyGeneration() !=
                    resolvedCursorPolicyGeneration ||
                 !slotGrid ||
                 !slotGrid->SetWindowCursor(effectiveInputCursor) )
            {
                return;
            }
            wxWinUINoteSlotCursorSet();
            if ( wxWinUIGetCursorPolicyGeneration() !=
                    resolvedCursorPolicyGeneration ||
                 !getCurrentSlot() )
                return;

            wxWinUISlot * const current = getCurrentSlot();
            if ( !current )
                return;
            current->m_cursorSynced = true;
            current->m_lastCursor = effectiveCursor;
            current->m_lastCursorPolicyGeneration =
                resolvedCursorPolicyGeneration;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot geometry sync", e);
        return;
    }

    // Commit caches only after every setter succeeded and the same slot
    // generation is still registered.  An exception or reentrant removal
    // leaves the old cache values, forcing a retry on the next dirty flush.
    wxWinUISlot * const current = getCurrentSlot();
    if ( !current )
        return;
    try
    {
        if ( !carrier ||
             w->IsEnabled() != enabled ||
             carrier.IsEnabled() != enabled ||
             container.IsHitTestVisible() != enabled )
        {
            MarkDirty(w);
            return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("verify slot enabled state", e);
        MarkDirty(w);
        return;
    }

    current->m_hwnd = wxHwnd;
    current->m_rectPx = rect;
    current->m_visibleRectPx = vis;
    current->m_visible = effectiveVisible;
    current->m_lastEnabled = static_cast<int>(enabled);

#ifdef WXWINUI_TEST_SUPPORT
    // Consume first: the callback may synchronously run another FlushSync().
    // The outer frame has already snapshotted its structural generation and
    // will refuse to publish if this hook changes it.
    std::function<void (wxWindow *)> slotSyncedHook =
        std::move(gs_testSlotSyncedHook);
    if ( slotSyncedHook )
        slotSyncedHook(w);
#endif // WXWINUI_TEST_SUPPORT
}

bool wxWinUITopLevelHost::RecomputeZOrder(
    unsigned long long expectedGeneration)
{
    // Recompute can itself be entered by a nested FlushSync() from a XAML
    // callback. Both frames must keep the host alive independently until
    // their local snapshots have been abandoned.
    OperationGuard hostOperation(this);
    if ( m_shuttingDown ||
         m_structureGeneration != expectedGeneration )
    {
        return false;
    }

    // USER32, not wxWindowList insertion order, is the authority after
    // Raise(), Lower() or a direct SetWindowPos(). Snapshot the physical HWND
    // tree bottom-to-top before touching XAML: SetZIndex can synchronously
    // run application code, so the application phase below uses only
    // invalidatable slot lifetimes and revalidates every native identity.
    struct SlotOrderSnapshot
    {
        std::shared_ptr<wxWinUISlotLifetime> state;
        winrt::Microsoft::UI::Xaml::Controls::Grid container{ nullptr };
        HWND hwnd = nullptr;
        unsigned long long hwndGeneration = 0;
        RECT visibleRectScreen = {};
        bool visible = false;
    };

    const HWND hwndTLW = GetHwndOf(m_tlw);
    const RECT clientScreen = GetClientScreenRect();
    std::vector<SlotOrderSnapshot> slots;
    std::map<HWND, std::size_t> slotByHwnd;
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        const std::shared_ptr<wxWinUISlotLifetime> state =
            slot ? slot->m_lifetime : nullptr;
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        const WXHWND wxHwnd = window ? window->GetHWND() : nullptr;
        const HWND hwnd = static_cast<HWND>(wxHwnd);
        const unsigned long long generation =
            hwnd ? wxWinUIMSWGetHwndGeneration(window, wxHwnd) : 0;
        if ( !slot || !slot->m_container || !state ||
             state->GetHost() != this || !hwnd || !generation )
        {
            continue;
        }

        SlotOrderSnapshot snapshot;
        snapshot.state = state;
        snapshot.container = slot->m_container;
        snapshot.hwnd = hwnd;
        snapshot.hwndGeneration = generation;
        snapshot.visible =
            slot->m_visible && !slot->m_visibleRectPx.IsEmpty();
        if ( snapshot.visible )
        {
            const wxRect& visible = slot->m_visibleRectPx;
            snapshot.visibleRectScreen =
            {
                clientScreen.left + visible.x,
                clientScreen.top + visible.y,
                clientScreen.left + visible.x + visible.width,
                clientScreen.top + visible.y + visible.height
            };
        }

        const std::size_t index = slots.size();
        slots.push_back(std::move(snapshot));
        if ( !slotByHwnd.emplace(hwnd, index).second )
        {
            wxWinUIDebugLog(
                "wxWinUI: duplicate slot HWND in z-order snapshot: %p",
                static_cast<void *>(hwnd));
        }
    }

    // Identify exact wx-owned HWNDs separately from implementation HWNDs.
    // Native implementation children of a control don't constitute an
    // application-requested native/XAML interleave.
    std::set<HWND> wxHwnds;
    std::function<void (wxWindow *)> collectWxHwnds =
        [&](wxWindow *parent)
        {
            if ( !parent )
                return;
            for ( wxWindow *child : parent->GetChildren() )
            {
                const HWND hwnd = GetHwndOf(child);
                if ( hwnd )
                    wxHwnds.insert(hwnd);
                collectWxHwnds(child);
            }
        };
    collectWxHwnds(m_tlw);

    struct NativeSurface
    {
        RECT bounds = {};
        // Exact visible region in screen coordinates. A shared handle keeps
        // SurfaceSummary cheap to concatenate while guaranteeing DeleteObject
        // on every early return and reentrant-abort path.
        std::shared_ptr<void> region;
    };

    struct SurfaceSummary
    {
        std::vector<RECT> xaml;
        std::vector<NativeSurface> native;
    };

    const auto overlaps =
        [](const std::vector<RECT>& below,
           const std::vector<NativeSurface>& above) -> bool
        {
            for ( const RECT& lhs : below )
            {
                for ( const NativeSurface& rhs : above )
                {
                    RECT intersection = {};
                    if ( ::IntersectRect(
                             &intersection, &lhs, &rhs.bounds) &&
                         intersection.right > intersection.left &&
                         intersection.bottom > intersection.top &&
                         rhs.region &&
                         ::RectInRegion(
                             static_cast<HRGN>(rhs.region.get()),
                             &intersection) )
                    {
                        return true;
                    }
                }
            }
            return false;
        };

    const auto makeRegion =
        [](HRGN region) -> std::shared_ptr<void>
        {
            return std::shared_ptr<void>(
                region,
                [](void *value)
                {
                    if ( value )
                        ::DeleteObject(static_cast<HRGN>(value));
                });
        };

    const auto makeNativeSurface =
        [&](HWND hwnd, NativeSurface *surface) -> bool
        {
            if ( !surface || !hwnd || !::IsWindowVisible(hwnd) )
                return false;

            RECT bounds = {};
            if ( !::GetWindowRect(hwnd, &bounds) ||
                 bounds.right <= bounds.left ||
                 bounds.bottom <= bounds.top )
            {
                return false;
            }

            // Child HWNDs are clipped by every ancestor client area. Apply
            // this first so a native surface outside a scrolled/paged
            // viewport cannot create a false impossible-overlap diagnostic.
            for ( HWND ancestor = ::GetParent(hwnd);
                  ancestor;
                  ancestor = ::GetParent(ancestor) )
            {
                RECT client = {};
                if ( !::GetClientRect(ancestor, &client) )
                    return false;
                ::MapWindowPoints(
                    ancestor, nullptr,
                    reinterpret_cast<POINT *>(&client), 2);
                if ( client.left > client.right )
                    std::swap(client.left, client.right);
                if ( client.top > client.bottom )
                    std::swap(client.top, client.bottom);

                RECT clipped = {};
                if ( !::IntersectRect(&clipped, &bounds, &client) )
                    return false;
                bounds = clipped;

                if ( ancestor == hwndTLW )
                    break;
            }

            HRGN visible = ::CreateRectRgn(
                bounds.left, bounds.top, bounds.right, bounds.bottom);
            if ( !visible )
                return false;
            std::shared_ptr<void> visibleOwner = makeRegion(visible);

            // Respect explicit SetWindowRgn shapes on the window and every
            // ancestor. Holes and rounded/non-rectangular windows otherwise
            // turn bounding-box contact into a false positive.
            for ( HWND shaped = hwnd;
                  shaped;
                  shaped = ::GetParent(shaped) )
            {
                HRGN local = ::CreateRectRgn(0, 0, 0, 0);
                if ( !local )
                    return false;
                std::shared_ptr<void> localOwner = makeRegion(local);

                const int type = ::GetWindowRgn(shaped, local);
                if ( type != ERROR )
                {
                    RECT windowRect = {};
                    if ( !::GetWindowRect(shaped, &windowRect) ||
                         ::OffsetRgn(local,
                                     windowRect.left,
                                     windowRect.top) == ERROR ||
                         ::CombineRgn(visible, visible, local,
                                      RGN_AND) == ERROR )
                    {
                        return false;
                    }
                }

                if ( shaped == hwndTLW )
                    break;
            }

            RECT exactBounds = {};
            if ( ::GetRgnBox(visible, &exactBounds) == ERROR ||
                 exactBounds.right <= exactBounds.left ||
                 exactBounds.bottom <= exactBounds.top )
            {
                return false;
            }

            surface->bounds = exactBounds;
            surface->region = std::move(visibleOwner);
            return true;
        };

    const auto appendSummary =
        [](SurfaceSummary& destination, const SurfaceSummary& source)
        {
            destination.xaml.insert(destination.xaml.end(),
                                    source.xaml.begin(), source.xaml.end());
            destination.native.insert(destination.native.end(),
                                      source.native.begin(),
                                      source.native.end());
        };

    const auto reportImpossible =
        [](HWND parent, const char *kind)
        {
            ++gs_impossibleZOrderOverlaps;
            wxWinUIDebugLog(
                "wxWinUI: impossible native/XAML z-order overlap "
                "(%s, parent=%p, count=%u); retaining one shared bridge",
                kind, static_cast<void *>(parent),
                gs_impossibleZOrderOverlaps);
        };

    std::vector<std::size_t> order;
    order.reserve(slots.size());
    std::set<std::size_t> orderedSlots;
    std::set<HWND> visitedHwnds;

    const auto appendSlot =
        [&](HWND hwnd)
        {
            const auto found = slotByHwnd.find(hwnd);
            if ( found != slotByHwnd.end() &&
                 orderedSlots.insert(found->second).second )
            {
                order.push_back(found->second);
            }
        };

    const bool tlwVisible =
        hwndTLW && ::IsWindowVisible(hwndTLW) && !::IsIconic(hwndTLW);

    std::function<SurfaceSummary (HWND)> walkHwndTree =
        [&](HWND parent) -> SurfaceSummary
        {
            SurfaceSummary combined;
            std::vector<HWND> topToBottom;
            for ( HWND child = ::GetWindow(parent, GW_CHILD);
                  child;
                  child = ::GetWindow(child, GW_HWNDNEXT) )
            {
                topToBottom.push_back(child);
            }

            // GetWindow(GW_CHILD/GW_HWNDNEXT) enumerates top-to-bottom.
            // Canvas.ZIndex grows in the opposite direction.
            std::vector<RECT> lowerXaml;
            bool siblingLimitReported = false;
            for ( auto it = topToBottom.rbegin();
                  it != topToBottom.rend();
                  ++it )
            {
                const HWND child = *it;
                if ( child == m_bridge ||
                     !visitedHwnds.insert(child).second )
                {
                    continue;
                }

                appendSlot(child);

                SurfaceSummary current;
                const auto ownSlot = slotByHwnd.find(child);
                const bool ownXaml = ownSlot != slotByHwnd.end();
                bool ownXamlVisible = false;
                if ( ownXaml )
                {
                    const SlotOrderSnapshot& snapshot =
                        slots[ownSlot->second];
                    if ( snapshot.visible )
                    {
                        current.xaml.push_back(
                            snapshot.visibleRectScreen);
                        ownXamlVisible = true;
                    }
                }
                else if ( wxHwnds.count(child) != 0 && tlwVisible )
                {
                    NativeSurface native;
                    if ( makeNativeSurface(child, &native) )
                        current.native.push_back(std::move(native));
                }

                SurfaceSummary descendants = walkHwndTree(child);

                // A native logical child is painted above its parent. If the
                // parent itself is XAML-backed and they overlap, the single
                // topmost bridge cannot honour that parent/child request.
                if ( ownXamlVisible &&
                     overlaps(current.xaml, descendants.native) )
                {
                    reportImpossible(child, "parent-child");
                }
                appendSummary(current, descendants);

                // `current` is above everything accumulated in lowerXaml.
                // A native surface in it therefore asks to sit above an
                // overlapping XAML surface, which a topmost shared bridge
                // cannot represent. Report once for this sibling group.
                if ( !siblingLimitReported &&
                     overlaps(lowerXaml, current.native) )
                {
                    reportImpossible(parent, "siblings");
                    siblingLimitReported = true;
                }

                lowerXaml.insert(lowerXaml.end(),
                                 current.xaml.begin(), current.xaml.end());
                appendSummary(combined, current);
            }
            return combined;
        };

    // A frame-class dialog may itself own the first/bottom slot.
    if ( hwndTLW )
        appendSlot(hwndTLW);

    SurfaceSummary tlwDescendants;
    if ( hwndTLW )
        tlwDescendants = walkHwndTree(hwndTLW);

    // Unlike ordinary slots, the TLW is the root passed to walkHwndTree(),
    // not one of the visited children. Its own XAML surface is therefore
    // ordered correctly above but must be joined explicitly to the overlap
    // analysis: a native child is painted above its parent HWND yet cannot
    // rise above the single topmost island bridge.
    const auto tlwSlot = slotByHwnd.find(hwndTLW);
    if ( tlwSlot != slotByHwnd.end() )
    {
        const SlotOrderSnapshot& snapshot = slots[tlwSlot->second];
        if ( snapshot.visible )
        {
            const std::vector<RECT> tlwXaml
            {
                snapshot.visibleRectScreen
            };
            if ( overlaps(tlwXaml, tlwDescendants.native) )
                reportImpossible(hwndTLW, "parent-child");
        }
    }

    // A slot can transiently remain in its old host during a failed/cascaded
    // cross-TLW migration. Do not drop it from the XAML ordering pass merely
    // because its native HWND no longer appears under this TLW; migration
    // immediately after this pass will reconcile it.
    for ( std::size_t i = 0; i < slots.size(); ++i )
    {
        if ( orderedSlots.insert(i).second )
        {
            order.push_back(i);
            wxWinUIDebugLog(
                "wxWinUI: slot HWND %p absent from TLW z-order snapshot",
                static_cast<void *>(slots[i].hwnd));
        }
    }

    int z = 0;
    for ( const std::size_t index : order )
    {
        if ( m_shuttingDown ||
             m_structureGeneration != expectedGeneration )
        {
            return false;
        }

        const SlotOrderSnapshot& snapshot = slots[index];
        const int desiredZ = ++z;
        wxWindow * const window =
            snapshot.state ? snapshot.state->GetWindow() : nullptr;
        wxWinUISlot * const current =
            window ? FindSlot(window) : nullptr;
        if ( !window || !current ||
             snapshot.state->GetHost() != this ||
             current->m_lifetime != snapshot.state ||
             current->m_container != snapshot.container ||
             static_cast<HWND>(window->GetHWND()) != snapshot.hwnd ||
             wxWinUIMSWGetHwndGeneration(
                 window, window->GetHWND()) != snapshot.hwndGeneration )
        {
            continue;
        }

        try
        {
            winrt::Microsoft::UI::Xaml::Controls::Canvas::SetZIndex(
                snapshot.container, desiredZ);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("slot z-order sync", e);
        }

#ifdef WXWINUI_TEST_SUPPORT
        // Deterministic seam at the same post-setter boundary. Consume it
        // first so its nested flush cannot recursively invoke itself.
        std::function<void ()> zOrderHook =
            std::move(gs_testZOrderSetHook);
        if ( zOrderHook )
            zOrderHook();
#endif // WXWINUI_TEST_SUPPORT

        // A bool is insufficient here: a nested FlushSync() can consume and
        // clear it before returning. The monotone epoch permanently records
        // that this outer snapshot became stale.
        if ( m_shuttingDown ||
             m_structureGeneration != expectedGeneration )
        {
            return false;
        }
    }

    return !m_shuttingDown &&
           m_structureGeneration == expectedGeneration;
}

void wxWinUITopLevelHost::EnsureBridgeOnTop()
{
    if ( !m_bridge || !m_tlw )
        return;

    // wx children created after the bridge stack above it; re-pin (cheap,
    // and only when actually needed).
    const HWND top = ::GetWindow(GetHwndOf(m_tlw), GW_CHILD);
    if ( top == m_bridge )
        return;

    // A transient island (popup menu, overlay dialog, rich tooltip) pins its
    // own bridge on top for its lifetime: never leapfrog those.
    if ( top )
    {
        wchar_t cls[96] = L"";
        ::GetClassNameW(top, cls, WXSIZEOF(cls));
        if ( wxString(cls).Contains(wxS("DesktopChildSiteBridge")) )
            return;
    }

    if ( wxWinUIInputLogEnabled() )
    {
        wchar_t cls[64] = L"";
        if ( top )
            ::GetClassNameW(top, cls, WXSIZEOF(cls));
        wxWinUIInputLog("bridge REPIN over %p '%S'",
                        static_cast<void *>(top), cls);
    }

    if ( !::SetWindowPos(m_bridge, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) )
    {
        const DWORD error = ::GetLastError();
        if ( !m_bridgePinFailureLogged )
        {
            wxLogWarning("wxWinUI: bridge re-pinning failed (error %lu)",
                         error);
            m_bridgePinFailureLogged = true;
        }
    }
    else
        m_bridgePinFailureLogged = false;
}

void wxWinUITopLevelHost::RebuildBridgeRegion()
{
    OperationGuard operation(this);

    if ( !m_bridge || !m_tlw )
        return;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const HWND bridge = m_bridge;
    const HWND hwndTLW = GetHwndOf(m_tlw);
    if ( !hwndTLW || !::IsWindow(bridge) )
        return;

    RECT rcClient;
    if ( !::GetClientRect(bridge, &rcClient) )
        return;
    const bool rtl =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;

    // Collect the scrollbar bands first, so an unchanged set skips the
    // (redraw-forcing) SetWindowRgn call entirely.
    std::vector<RECT> cuts;
    cuts.push_back(rcClient);   // client size participates in the signature

    // The exact physical rect comes from GetScrollBarInfo: correct wherever
    // the bar actually is (left side under RTL layouts), correct under any
    // per-monitor DPI, and absent when the bar is currently hidden -- the
    // style bit alone says none of that. Cuts are collected in physical-LTR
    // coordinates. SetWindowRgn interprets an RTL target from its upper-right
    // corner, so they are mirrored only when building the native region
    // below; the physical signature remains stable and intelligible.
    const RECT rcCS = GetClientScreenRect();
    const auto addBarCut = [&](HWND hwnd, LONG objId)
    {
        SCROLLBARINFO sbi = { sizeof(SCROLLBARINFO) };
        if ( !::GetScrollBarInfo(hwnd, objId, &sbi) )
            return;
        if ( sbi.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN) )
            return;

        const RECT cut = { sbi.rcScrollBar.left - rcCS.left,
                           sbi.rcScrollBar.top - rcCS.top,
                           sbi.rcScrollBar.right - rcCS.left,
                           sbi.rcScrollBar.bottom - rcCS.top };
        cuts.push_back(cut);
    };

    std::function<void (wxWindow *)> walk = [&](wxWindow *win)
    {
        for ( wxWindow *child : win->GetChildren() )
        {
            const HWND hwnd = GetHwndOf(child);
            if ( hwnd && ::IsWindowVisible(hwnd) )
            {
                const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
                if ( style & WS_VSCROLL )
                    addBarCut(hwnd, OBJID_VSCROLL);
                if ( style & WS_HSCROLL )
                    addBarCut(hwnd, OBJID_HSCROLL);
            }

            walk(child);
        }
    };
    walk(m_tlw);

    if ( m_regionValid && rtl == m_regionRTL &&
         cuts.size() == m_regionCuts.size() &&
             memcmp(cuts.data(), m_regionCuts.data(),
                    cuts.size() * sizeof(RECT)) == 0 )
        return;

    wxWinUIOwnedRegion region(
        ::CreateRectRgn(0, 0, rcClient.right, rcClient.bottom));
    if ( !region )
    {
        if ( !m_regionFailureLogged )
        {
            wxLogWarning("wxWinUI: failed to allocate the bridge window "
                         "region");
            m_regionFailureLogged = true;
        }
        return;
    }

    for ( size_t i = 1; i < cuts.size(); ++i )
    {
        RECT nativeCut = cuts[i];
        if ( rtl )
        {
            const LONG oldLeft = nativeCut.left;
            nativeCut.left = rcClient.right - nativeCut.right;
            nativeCut.right = rcClient.right - oldLeft;
        }

        wxWinUIOwnedRegion cut(
            ::CreateRectRgn(nativeCut.left, nativeCut.top,
                            nativeCut.right, nativeCut.bottom));
        if ( !cut )
        {
            if ( !m_regionFailureLogged )
            {
                wxLogWarning("wxWinUI: failed to allocate a bridge cut-out "
                             "region");
                m_regionFailureLogged = true;
            }
            return;
        }

        if ( ::CombineRgn(region.Get(), region.Get(), cut.Get(), RGN_DIFF)
                == ERROR )
        {
            if ( !m_regionFailureLogged )
            {
                wxLogWarning("wxWinUI: failed to combine a bridge cut-out "
                             "region");
                m_regionFailureLogged = true;
            }
            return;
        }
    }

    // USER32 owns the region after a successful call only. Commit the
    // signature at that same point: until then m_regionCuts continues to
    // describe the region that is actually installed, and the next flush
    // retries this transaction.
    if ( !::SetWindowRgn(bridge, region.Get(), TRUE) )
    {
        if ( !m_regionFailureLogged )
        {
            wxLogWarning("wxWinUI: SetWindowRgn failed");
            m_regionFailureLogged = true;
        }
        return;
    }

    // USER32 took ownership at the successful return, even if one of the
    // synchronous window-position callbacks destroyed or replaced the host.
    region.Release();

    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_bridge != bridge || !::IsWindow(bridge) )
    {
        return;
    }

    const bool currentRTL =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    if ( currentRTL != rtl )
    {
        // The installed region used the old coordinate convention. Keep the
        // cache invalid and schedule one bounded coalesced correction.
        m_regionValid = false;
        ScheduleFlush();
        return;
    }

    m_regionCuts = std::move(cuts);
    m_regionRTL = rtl;
    m_regionValid = true;
    m_regionFailureLogged = false;
}

// Physical screen rect of the TLW client area, normalized left<right even
// for mirrored (RTL) windows.  Island coordinates are physically LTR while
// ClientToScreen/ScreenToClient on a WS_EX_LAYOUTRTL window flip the X
// axis, so every island<->screen conversion must go through this rect and
// never through those two calls (with the TLW handle) directly.
RECT wxWinUITopLevelHost::GetClientScreenRect() const
{
    RECT rc = { 0, 0, 0, 0 };
    const HWND hwndTLW = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    if ( !hwndTLW )
        return rc;

    ::GetClientRect(hwndTLW, &rc);
    ::MapWindowPoints(hwndTLW, nullptr, reinterpret_cast<POINT *>(&rc), 2);

    if ( rc.left > rc.right )
    {
        const LONG swap = rc.left;
        rc.left = rc.right;
        rc.right = swap;
    }

    return rc;
}

wxPoint wxWinUITopLevelHost::ScreenToRootDIP(const wxPoint& screenPt) const
{
    if ( !m_tlw )
        return screenPt;

    const RECT rcCS = GetClientScreenRect();
    const POINT pt = { screenPt.x - rcCS.left, screenPt.y - rcCS.top };

    const double scale = GetScale();
    return wxPoint(static_cast<int>(pt.x / scale),
                   static_cast<int>(pt.y / scale));
}

double wxWinUITopLevelHost::GetScale() const
{
    try
    {
        const auto xamlRoot = GetXamlRoot();
        if ( xamlRoot )
        {
            const double scale = xamlRoot.RasterizationScale();
            if ( scale > 0 )
                return scale;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return m_tlw ? m_tlw->GetDPIScaleFactor() : 1.0;
}

#endif // wxUSE_WINUI3
