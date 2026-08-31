/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/inputrouter.cpp
// Purpose:     generation-safe native hit resolution for the WinUI bridge
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/private/inputrouter.h"

#if wxUSE_WINUI3

#include "wx/msw/private.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <algorithm>
#include <atomic>
#include <utility>
#include <vector>

namespace
{

std::atomic<unsigned long long> gs_resolveAttempts{0};
std::atomic<unsigned long long> gs_resolveRestarts{0};
std::atomic<unsigned long long> gs_resolveUnstable{0};
std::atomic<unsigned long long> gs_transparentCandidates{0};
thread_local bool gs_nativeResolveActive = false;

// WM_NCHITTEST is application code and, with a WinUI island in the same
// top-level window, can synchronously route another root pointer event.  A
// nested resolver would otherwise send WM_NCHITTEST again and recurse until
// the UI thread exhausts its stack.  Keep one boundary shared by both public
// resolution contracts and fail nested work closed.
class wxWinUINativeResolveScope final
{
public:
    wxWinUINativeResolveScope()
        : m_isOutermost(!gs_nativeResolveActive)
    {
        if ( m_isOutermost )
            gs_nativeResolveActive = true;
    }

    ~wxWinUINativeResolveScope()
    {
        if ( m_isOutermost )
            gs_nativeResolveActive = false;
    }

    bool IsOutermost() const { return m_isOutermost; }

private:
    const bool m_isOutermost;

    wxDECLARE_NO_COPY_CLASS(wxWinUINativeResolveScope);
};

bool wxWinUIPointInRect(HWND hwnd, const POINT& point)
{
    RECT rect;
    if ( !::GetWindowRect(hwnd, &rect) || !::PtInRect(&rect, point) )
        return false;

    HRGN region = ::CreateRectRgn(0, 0, 0, 0);
    if ( !region )
        return false;

    const int regionType = ::GetWindowRgn(hwnd, region);
    const bool rtl =
        (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    // USER32 expresses a mirrored window region from the upper-right
    // corner. The screen rectangle is always physically left-to-right, so
    // mirror its X coordinate before asking GDI about the returned HRGN.
    const int regionX = rtl ? rect.right - 1 - point.x
                            : point.x - rect.left;
    const bool inside =
        regionType == ERROR ||
        ::PtInRegion(region, regionX, point.y - rect.top);
    ::DeleteObject(region);
    return inside;
}

bool wxWinUIPointInClient(HWND hwnd, const POINT& point)
{
    RECT client;
    if ( !::GetClientRect(hwnd, &client) )
        return false;

    POINT corners[2] =
    {
        { client.left, client.top },
        { client.right, client.bottom }
    };
    ::SetLastError(ERROR_SUCCESS);
    if ( !::MapWindowPoints(hwnd, HWND_DESKTOP, corners, 2) &&
         ::GetLastError() != ERROR_SUCCESS )
    {
        return false;
    }

    const RECT screenClient =
    {
        std::min(corners[0].x, corners[1].x),
        std::min(corners[0].y, corners[1].y),
        std::max(corners[0].x, corners[1].x),
        std::max(corners[0].y, corners[1].y)
    };
    return ::PtInRect(&screenClient, point);
}

} // namespace

class wxWinUINativeResolverImpl
{
public:
    using Candidates = std::vector<wxWinUINativeTarget>;

    struct NativeFingerprint
    {
        RECT windowRect{};
        RECT clientRect{};
        RECT screenClientRect{};
        LONG_PTR style = 0;
        LONG_PTR exStyle = 0;
        int regionType = ERROR;
        std::vector<unsigned char> regionData;
    };

    struct SnapshotResult
    {
        Candidates candidates;
        std::vector<NativeFingerprint> fingerprints;
        bool opaque = false;
    };

    struct ExclusionIdentity
    {
        WXHWND hwnd = nullptr;
        unsigned long long generation = 0;

        bool Capture(WXHWND value)
        {
            hwnd = value;
            generation = value
                ? wxWinUIMSWGetNativeHwndGeneration(value)
                : 0;
            return !value || generation != 0;
        }

        bool IsValid() const
        {
            return !hwnd ||
                   wxWinUIMSWGetNativeHwndGeneration(hwnd) == generation;
        }
    };

    static wxWinUIHitResolution Resolve(wxWindow *tlw,
                                        const POINT& screenPoint,
                                        WXHWND bridge,
                                        WXHWND inner,
                                        wxWinUINativeHit *hit)
    {
        if ( hit )
            *hit = wxWinUINativeHit();

        const wxWinUINativeResolveScope resolveScope;
        if ( !resolveScope.IsOutermost() )
            return Unstable();

        if ( !tlw )
            return wxWinUIHitResolution::Miss;

        const HWND initialTLW = GetHwndOf(tlw);
        if ( initialTLW )
        {
            const DWORD threadId =
                ::GetWindowThreadProcessId(initialTLW, nullptr);
            if ( !threadId || threadId != ::GetCurrentThreadId() )
                return Unstable();
        }

        ExclusionIdentity bridgeIdentity;
        ExclusionIdentity innerIdentity;
        if ( !bridgeIdentity.Capture(bridge) ||
             !innerIdentity.Capture(inner) )
        {
            return Unstable();
        }

        wxWeakRef<wxWindow> weakTLW(tlw);
        for ( unsigned attempt = 0; attempt != 3; ++attempt )
        {
            ++gs_resolveAttempts;

            if ( !bridgeIdentity.IsValid() || !innerIdentity.IsValid() )
                return Unstable();

            wxWindow *currentTLW = weakTLW.get();
            if ( !currentTLW )
                return Unstable();

            const SnapshotResult before =
                Snapshot(currentTLW, screenPoint, bridge, inner, false);
            if ( before.opaque )
                return Unstable();
            if ( before.candidates.empty() )
                return wxWinUIHitResolution::Miss;

            bool restart = false;
            for ( const wxWinUINativeTarget& candidate :
                  before.candidates )
            {
                if ( !candidate.IsValid() )
                {
                    restart = true;
                    break;
                }

                const HWND hwnd =
                    reinterpret_cast<HWND>(candidate.GetLeafHwnd());
                const LRESULT ht = ::SendMessage(
                    hwnd,
                    WM_NCHITTEST,
                    0,
                    MAKELPARAM(static_cast<short>(screenPoint.x),
                               static_cast<short>(screenPoint.y)));

                currentTLW = weakTLW.get();
                if ( !currentTLW ||
                     !bridgeIdentity.IsValid() ||
                     !innerIdentity.IsValid() )
                {
                    return Unstable();
                }

                const SnapshotResult after =
                    Snapshot(currentTLW, screenPoint, bridge, inner, false);
                if ( after.opaque )
                    return Unstable();
                if ( !SameSnapshot(before, after) )
                {
                    restart = true;
                    break;
                }

                // HTERROR is an explicit failure from WM_NCHITTEST, not a
                // non-client zone.  Never turn it into a routable native
                // target or click through to a lower sibling.
                if ( ht == HTERROR )
                    return Unstable();

                if ( ht == HTTRANSPARENT )
                {
                    ++gs_transparentCandidates;
                    continue;
                }
                if ( ht == HTNOWHERE )
                    continue;

                if ( !candidate.IsValid() )
                {
                    restart = true;
                    break;
                }

                if ( hit )
                {
                    hit->m_target = candidate;
                    hit->m_hitTest = ht;
                    hit->m_area = ht == HTCLIENT
                        ? wxWinUINativeArea::Client
                        : wxWinUINativeArea::NonClient;
                    hit->m_screen = screenPoint;
                    hit->m_client = screenPoint;
                    hit->m_bridgeHwnd = bridgeIdentity.hwnd;
                    hit->m_innerHwnd = innerIdentity.hwnd;
                    hit->m_bridgeGeneration = bridgeIdentity.generation;
                    hit->m_innerGeneration = innerIdentity.generation;
                    ::SetLastError(ERROR_SUCCESS);
                    if ( !::MapWindowPoints(HWND_DESKTOP, hwnd,
                                            &hit->m_client, 1) &&
                         ::GetLastError() != ERROR_SUCCESS )
                    {
                        *hit = wxWinUINativeHit();
                        restart = true;
                        break;
                    }
                }

                return wxWinUIHitResolution::Hit;
            }

            if ( !restart )
                return wxWinUIHitResolution::Miss;

            ++gs_resolveRestarts;
        }

        return Unstable();
    }

    static wxWinUIHitResolution ResolveWindowAtPoint(
        wxWindow *tlw,
        const POINT& screenPoint,
        WXHWND bridge,
        WXHWND inner,
        wxWinUINativeTarget *target)
    {
        if ( target )
            *target = wxWinUINativeTarget();

        const wxWinUINativeResolveScope resolveScope;
        if ( !resolveScope.IsOutermost() )
            return Unstable();

        if ( !tlw )
            return wxWinUIHitResolution::Miss;

        const HWND initialTLW = GetHwndOf(tlw);
        if ( initialTLW )
        {
            const DWORD threadId =
                ::GetWindowThreadProcessId(initialTLW, nullptr);
            if ( !threadId || threadId != ::GetCurrentThreadId() )
                return Unstable();
        }

        ExclusionIdentity bridgeIdentity;
        ExclusionIdentity innerIdentity;
        if ( !bridgeIdentity.Capture(bridge) ||
             !innerIdentity.Capture(inner) )
        {
            return Unstable();
        }

        wxWeakRef<wxWindow> weakTLW(tlw);
        for ( unsigned attempt = 0; attempt != 3; ++attempt )
        {
            ++gs_resolveAttempts;
            if ( !bridgeIdentity.IsValid() || !innerIdentity.IsValid() )
                return Unstable();

            wxWindow * const currentTLW = weakTLW.get();
            if ( !currentTLW )
                return Unstable();

            const SnapshotResult snapshot =
                Snapshot(currentTLW, screenPoint, bridge, inner, true);
            if ( snapshot.opaque )
                return Unstable();
            if ( snapshot.candidates.empty() )
                return wxWinUIHitResolution::Miss;

            const wxWinUINativeTarget& candidate =
                snapshot.candidates.front();
            if ( !candidate.IsValid() )
            {
                ++gs_resolveRestarts;
                continue;
            }

            if ( target )
                *target = candidate;
            return wxWinUIHitResolution::Hit;
        }

        return Unstable();
    }

    static bool BuildTarget(HWND leaf,
                            wxWindow *tlw,
                            wxWinUINativeTarget& target)
    {
        if ( !leaf || !tlw || !::IsWindow(leaf) )
            return false;

        wxWindow * const window =
            wxGetWindowFromHWND(reinterpret_cast<WXHWND>(leaf));
        if ( !window || wxGetTopLevelParent(window) != tlw )
            return false;

        const HWND shell = GetHwndOf(window);
        const HWND hwndTLW = GetHwndOf(tlw);
        if ( !shell || !hwndTLW )
            return false;

        const unsigned long long shellGeneration =
            wxWinUIMSWGetHwndGeneration(
                window, reinterpret_cast<WXHWND>(shell));
        const unsigned long long leafGeneration =
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(leaf));
        const unsigned long long tlwGeneration =
            wxWinUIMSWGetHwndGeneration(
                tlw, reinterpret_cast<WXHWND>(hwndTLW));
        if ( !leafGeneration || !shellGeneration || !tlwGeneration )
            return false;

        target.m_window = window;
        target.m_tlw = tlw;
        target.m_leafHwnd = reinterpret_cast<WXHWND>(leaf);
        target.m_shellHwnd = reinterpret_cast<WXHWND>(shell);
        target.m_tlwHwnd = reinterpret_cast<WXHWND>(hwndTLW);
        target.m_leafGeneration = leafGeneration;
        target.m_shellGeneration = shellGeneration;
        target.m_tlwGeneration = tlwGeneration;
        return true;
    }

private:
    static wxWinUIHitResolution Unstable()
    {
        ++gs_resolveUnstable;
        return wxWinUIHitResolution::Unstable;
    }

    static bool CollectChildren(HWND parent,
                                wxWindow *tlw,
                                DWORD threadId,
                                const POINT& screenPoint,
                                HWND bridge,
                                HWND inner,
                                bool includeDisabled,
                                Candidates& candidates)
    {
        for ( HWND child = ::GetWindow(parent, GW_CHILD);
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            if ( child == bridge || child == inner )
                continue;
            if ( !::IsWindowVisible(child) ||
                 !wxWinUIPointInRect(child, screenPoint) )
            {
                continue;
            }

            // WindowFromPoint() ignores disabled windows and keeps walking
            // the Z order, so skip the entire disabled subtree too.
            if ( !includeDisabled && !::IsWindowEnabled(child) )
                continue;

            // Never synchronously cross an input-thread boundary. Unknown
            // ownership is fail-closed at this branch rather than becoming a
            // click-through to an unrelated lower sibling.
            if ( ::GetWindowThreadProcessId(child, nullptr) != threadId )
            {
                return false;
            }

            if ( wxWinUIPointInClient(child, screenPoint) )
            {
                if ( !CollectChildren(child, tlw, threadId, screenPoint,
                                      bridge, inner, includeDisabled,
                                      candidates) )
                {
                    return false;
                }
            }

            wxWinUINativeTarget target;
            if ( !BuildTarget(child, tlw, target) )
            {
                // Unknown native content is opaque for the same reason: do
                // not make it click through to an unrelated lower sibling.
                return false;
            }
            candidates.push_back(target);
        }
        return true;
    }

    static SnapshotResult Snapshot(wxWindow *tlw,
                                   const POINT& screenPoint,
                                   WXHWND bridgeHandle,
                                   WXHWND innerHandle,
                                   bool includeDisabled)
    {
        SnapshotResult result;
        if ( !tlw || tlw->IsBeingDeleted() )
            return result;

        const HWND hwndTLW = GetHwndOf(tlw);
        if ( !hwndTLW ||
             !::IsWindow(hwndTLW) ||
             !::IsWindowVisible(hwndTLW) ||
             (!includeDisabled && !::IsWindowEnabled(hwndTLW)) ||
             !wxWinUIPointInRect(hwndTLW, screenPoint) )
        {
            return result;
        }

        const DWORD threadId =
            ::GetWindowThreadProcessId(hwndTLW, nullptr);
        if ( !threadId || threadId != ::GetCurrentThreadId() )
        {
            // A caller on another GUI thread cannot safely query this tree or
            // synchronously send WM_NCHITTEST to it.  This is an indeterminate
            // snapshot, not an ordinary miss.
            result.opaque = true;
            return result;
        }

        const HWND bridge = reinterpret_cast<HWND>(bridgeHandle);
        const HWND inner = reinterpret_cast<HWND>(innerHandle);
        if ( !CollectChildren(hwndTLW, tlw, threadId, screenPoint,
                              bridge, inner, includeDisabled,
                              result.candidates) )
        {
            result.candidates.clear();
            result.opaque = true;
            return result;
        }

        wxWinUINativeTarget root;
        if ( BuildTarget(hwndTLW, tlw, root) )
            result.candidates.push_back(root);

        result.fingerprints.reserve(result.candidates.size());
        for ( const wxWinUINativeTarget& candidate : result.candidates )
        {
            NativeFingerprint fingerprint;
            if ( !candidate.IsValid() ||
                 !CaptureFingerprint(
                     reinterpret_cast<HWND>(candidate.GetLeafHwnd()),
                     fingerprint) )
            {
                result.candidates.clear();
                result.fingerprints.clear();
                result.opaque = true;
                return result;
            }
            result.fingerprints.push_back(std::move(fingerprint));
        }
        return result;
    }

    static bool CaptureWindowLong(HWND hwnd, int index, LONG_PTR& value)
    {
        ::SetLastError(ERROR_SUCCESS);
        value = ::GetWindowLongPtr(hwnd, index);
        return value != 0 || ::GetLastError() == ERROR_SUCCESS;
    }

    static bool EqualRectangles(const RECT& first, const RECT& second)
    {
        return first.left == second.left &&
               first.top == second.top &&
               first.right == second.right &&
               first.bottom == second.bottom;
    }

    static bool CaptureFingerprint(HWND hwnd,
                                   NativeFingerprint& fingerprint)
    {
        if ( !hwnd ||
             !::GetWindowRect(hwnd, &fingerprint.windowRect) ||
             !::GetClientRect(hwnd, &fingerprint.clientRect) ||
             !CaptureWindowLong(hwnd, GWL_STYLE, fingerprint.style) ||
             !CaptureWindowLong(hwnd, GWL_EXSTYLE, fingerprint.exStyle) )
        {
            return false;
        }

        POINT corners[2] =
        {
            { fingerprint.clientRect.left, fingerprint.clientRect.top },
            { fingerprint.clientRect.right, fingerprint.clientRect.bottom }
        };
        ::SetLastError(ERROR_SUCCESS);
        if ( !::MapWindowPoints(hwnd, HWND_DESKTOP, corners, 2) &&
             ::GetLastError() != ERROR_SUCCESS )
        {
            return false;
        }
        fingerprint.screenClientRect =
        {
            std::min(corners[0].x, corners[1].x),
            std::min(corners[0].y, corners[1].y),
            std::max(corners[0].x, corners[1].x),
            std::max(corners[0].y, corners[1].y)
        };

        HRGN region = ::CreateRectRgn(0, 0, 0, 0);
        if ( !region )
            return false;

        fingerprint.regionType = ::GetWindowRgn(hwnd, region);
        bool ok = true;
        if ( fingerprint.regionType != ERROR )
        {
            const DWORD size = ::GetRegionData(region, 0, nullptr);
            if ( !size )
            {
                ok = false;
            }
            else
            {
                fingerprint.regionData.resize(size);
                ok = ::GetRegionData(
                         region, size,
                         reinterpret_cast<RGNDATA *>(
                             fingerprint.regionData.data())) == size;
            }
        }
        ::DeleteObject(region);
        return ok;
    }

    static bool SameFingerprint(const NativeFingerprint& first,
                                const NativeFingerprint& second)
    {
        return EqualRectangles(first.windowRect, second.windowRect) &&
               EqualRectangles(first.clientRect, second.clientRect) &&
               EqualRectangles(first.screenClientRect,
                               second.screenClientRect) &&
               first.style == second.style &&
               first.exStyle == second.exStyle &&
               first.regionType == second.regionType &&
               first.regionData == second.regionData;
    }

    static bool SameSnapshot(const SnapshotResult& first,
                             const SnapshotResult& second)
    {
        if ( first.candidates.size() != second.candidates.size() ||
             first.fingerprints.size() != second.fingerprints.size() ||
             first.candidates.size() != first.fingerprints.size() )
        {
            return false;
        }

        for ( size_t i = 0; i != first.candidates.size(); ++i )
        {
            if ( !first.candidates[i].Matches(second.candidates[i]) ||
                 !SameFingerprint(first.fingerprints[i],
                                  second.fingerprints[i]) )
            {
                return false;
            }
        }
        return true;
    }
};

bool wxWinUINativeTarget::IsValid() const
{
    wxWindow * const window = m_window.get();
    wxWindow * const tlw = m_tlw.get();
    const HWND leaf = reinterpret_cast<HWND>(m_leafHwnd);
    const HWND shell = reinterpret_cast<HWND>(m_shellHwnd);
    const HWND hwndTLW = reinterpret_cast<HWND>(m_tlwHwnd);

    if ( !window || !tlw ||
         window->IsBeingDeleted() || tlw->IsBeingDeleted() ||
         !leaf || !shell || !hwndTLW ||
         !::IsWindow(leaf) || !::IsWindow(shell) || !::IsWindow(hwndTLW) )
    {
        return false;
    }

    if ( GetHwndOf(window) != shell ||
         GetHwndOf(tlw) != hwndTLW ||
         wxGetTopLevelParent(window) != tlw ||
         wxGetWindowFromHWND(m_leafHwnd) != window )
    {
        return false;
    }

    return
        wxWinUIMSWGetHwndGeneration(window, m_shellHwnd) ==
            m_shellGeneration &&
        wxWinUIMSWGetHwndGeneration(tlw, m_tlwHwnd) ==
            m_tlwGeneration &&
        wxWinUIMSWGetNativeHwndGeneration(m_leafHwnd) ==
            m_leafGeneration &&
        ::GetWindowThreadProcessId(leaf, nullptr) ==
            ::GetWindowThreadProcessId(hwndTLW, nullptr);
}

bool wxWinUINativeTarget::Matches(
    const wxWinUINativeTarget& other) const
{
    return m_window.get() == other.m_window.get() &&
           m_tlw.get() == other.m_tlw.get() &&
           m_leafHwnd == other.m_leafHwnd &&
           m_shellHwnd == other.m_shellHwnd &&
           m_tlwHwnd == other.m_tlwHwnd &&
           m_leafGeneration == other.m_leafGeneration &&
           m_shellGeneration == other.m_shellGeneration &&
           m_tlwGeneration == other.m_tlwGeneration;
}

namespace
{

// Is any window above `below` inside `parent` covering the point? Siblings are
// enumerated from the top of the z-order down to `below`, so only the ones
// that would win over it are examined.
bool wxWinUIAnySiblingAboveCovers(HWND parent,
                                  HWND below,
                                  const POINT& screenPoint,
                                  HWND bridge,
                                  HWND inner)
{
    for ( HWND sibling = ::GetWindow(parent, GW_CHILD);
          sibling && sibling != below;
          sibling = ::GetWindow(sibling, GW_HWNDNEXT) )
    {
        if ( sibling == bridge || sibling == inner )
            continue;
        if ( !::IsWindowVisible(sibling) || !::IsWindowEnabled(sibling) )
            continue;
        if ( wxWinUIPointInRect(sibling, screenPoint) )
            return true;
    }
    return false;
}

// Does any visible child of `parent` cover the point? Such a child would be
// the real target, not its parent.
bool wxWinUIAnyChildCovers(HWND parent,
                           const POINT& screenPoint,
                           HWND bridge,
                           HWND inner)
{
    for ( HWND child = ::GetWindow(parent, GW_CHILD);
          child;
          child = ::GetWindow(child, GW_HWNDNEXT) )
    {
        if ( child == bridge || child == inner )
            continue;
        if ( !::IsWindowVisible(child) )
            continue;
        if ( wxWinUIPointInRect(child, screenPoint) )
            return true;
    }
    return false;
}

} // anonymous namespace

wxWinUIHitResolution
wxWinUIResolveNativeHitReusing(wxWindow *tlw,
                               const POINT& screenPoint,
                               WXHWND bridge,
                               WXHWND inner,
                               const wxWinUINativeHit& previous,
                               bool layoutUnchanged,
                               wxWinUINativeHit *hit)
{
    const HWND bridgeHwnd = reinterpret_cast<HWND>(bridge);
    const HWND innerHwnd = reinterpret_cast<HWND>(inner);

    // Everything below is an attempt; anything unexpected falls back to the
    // full resolution rather than guessing.
    for ( ;; )
    {
        if ( !layoutUnchanged || !tlw || !hit || !previous.IsValid() )
            break;
        if ( previous.GetBridgeExclusionHwnd() != bridge ||
             previous.GetInnerExclusionHwnd() != inner )
        {
            break;
        }
        if ( (bridge &&
              wxWinUIMSWGetNativeHwndGeneration(bridge) !=
                previous.GetBridgeExclusionGeneration()) ||
             (inner &&
              wxWinUIMSWGetNativeHwndGeneration(inner) !=
                previous.GetInnerExclusionGeneration()) )
        {
            break;
        }

        const HWND hwndTLW = GetHwndOf(tlw);
        if ( !hwndTLW || !::IsWindow(hwndTLW) || !::IsWindowVisible(hwndTLW) ||
             !::IsWindowEnabled(hwndTLW) ||
             !wxWinUIPointInRect(hwndTLW, screenPoint) )
        {
            break;
        }
        if ( ::GetWindowThreadProcessId(hwndTLW, nullptr) !=
                ::GetCurrentThreadId() )
        {
            break;
        }

        const HWND leaf =
            reinterpret_cast<HWND>(previous.GetTarget().GetLeafHwnd());
        if ( !leaf || !::IsWindow(leaf) || !::IsWindowVisible(leaf) ||
             !::IsWindowEnabled(leaf) ||
             !wxWinUIPointInRect(leaf, screenPoint) )
        {
            break;
        }

        // The point may have moved onto something of the target's own, or
        // under a sibling that was already there: both change the answer even
        // though nothing moved.
        if ( leaf != hwndTLW &&
             wxWinUIAnyChildCovers(leaf, screenPoint, bridgeHwnd, innerHwnd) )
        {
            break;
        }
        bool covered = false;
        for ( HWND current = leaf; current && current != hwndTLW; )
        {
            const HWND parent = ::GetParent(current);
            if ( !parent )
            {
                covered = true;   // unexpected shape: fall back
                break;
            }
            if ( wxWinUIAnySiblingAboveCovers(parent, current, screenPoint,
                                              bridgeHwnd, innerHwnd) )
            {
                covered = true;
                break;
            }
            current = parent;
        }
        if ( covered )
            break;

        // The zone is position-dependent, so it is always asked again.
        const LRESULT ht = ::SendMessage(
            leaf, WM_NCHITTEST, 0,
            MAKELPARAM(static_cast<short>(screenPoint.x),
                       static_cast<short>(screenPoint.y)));
        if ( ht == HTTRANSPARENT || ht == HTNOWHERE )
            break;

        // The message ran application code: prove the identity again.
        wxWinUINativeTarget target;
        if ( !wxWinUIGetNativeTarget(tlw, reinterpret_cast<WXHWND>(leaf),
                                     &target) ||
             !target.Matches(previous.GetTarget()) )
        {
            break;
        }

        wxWinUINativeHit resolved = previous;
        resolved.m_target = target;
        resolved.m_hitTest = ht;
        resolved.m_area = ht == HTCLIENT
            ? wxWinUINativeArea::Client
            : wxWinUINativeArea::NonClient;
        resolved.m_screen = screenPoint;
        resolved.m_client = screenPoint;
        ::SetLastError(ERROR_SUCCESS);
        if ( !::MapWindowPoints(HWND_DESKTOP, leaf, &resolved.m_client, 1) &&
             ::GetLastError() != ERROR_SUCCESS )
        {
            break;
        }

        *hit = resolved;
        return wxWinUIHitResolution::Hit;
    }

    return wxWinUIResolveNativeHit(tlw, screenPoint, bridge, inner, hit);
}

wxWinUIHitResolution
wxWinUIResolveNativeHit(wxWindow *tlw,
                        const POINT& screenPoint,
                        WXHWND bridge,
                        WXHWND inner,
                        wxWinUINativeHit *hit)
{
    return wxWinUINativeResolverImpl::Resolve(
        tlw, screenPoint, bridge, inner, hit);
}

wxWinUIHitResolution
wxWinUIResolveNativeWindowAtPoint(wxWindow *tlw,
                                  const POINT& screenPoint,
                                  WXHWND bridge,
                                  WXHWND inner,
                                  wxWinUINativeTarget *target)
{
    return wxWinUINativeResolverImpl::ResolveWindowAtPoint(
        tlw, screenPoint, bridge, inner, target);
}

bool wxWinUIGetNativeTarget(wxWindow *tlw,
                            WXHWND hwnd,
                            wxWinUINativeTarget *target)
{
    if ( !target )
        return false;

    *target = wxWinUINativeTarget();
    return wxWinUINativeResolverImpl::BuildTarget(
        reinterpret_cast<HWND>(hwnd), tlw, *target);
}

bool wxWinUIRefreshNativeHit(wxWindow *tlw,
                             const wxWinUINativeHit& expected,
                             WXHWND bridge,
                             WXHWND inner,
                             wxWinUINativeHit *refreshed,
                             bool layoutUnchanged)
{
    if ( !refreshed )
        return false;
    *refreshed = wxWinUINativeHit();
    if ( !expected.IsValid() ||
         expected.GetBridgeExclusionHwnd() != bridge ||
         expected.GetInnerExclusionHwnd() != inner ||
         (bridge &&
          wxWinUIMSWGetNativeHwndGeneration(bridge) !=
            expected.GetBridgeExclusionGeneration()) ||
         (inner &&
          wxWinUIMSWGetNativeHwndGeneration(inner) !=
            expected.GetInnerExclusionGeneration()) )
    {
        return false;
    }

    wxWinUINativeHit current;
    const wxWinUIHitResolution resolution = layoutUnchanged
        ? wxWinUIResolveNativeHitReusing(
              tlw, expected.GetScreenPoint(), bridge, inner,
              expected, true, &current)
        : wxWinUIResolveNativeHit(
              tlw, expected.GetScreenPoint(), bridge, inner, &current);
    if ( resolution !=
             wxWinUIHitResolution::Hit ||
         !current.GetTarget().Matches(expected.GetTarget()) ||
         current.GetArea() != expected.GetArea() ||
         current.GetHitTest() != expected.GetHitTest() ||
         current.GetBridgeExclusionGeneration() !=
            expected.GetBridgeExclusionGeneration() ||
         current.GetInnerExclusionGeneration() !=
            expected.GetInnerExclusionGeneration() )
    {
        return false;
    }

    *refreshed = current;
    return true;
}

wxWinUIInputRouterCounters wxWinUIGetInputRouterCounters()
{
    wxWinUIInputRouterCounters counters;
    counters.resolveAttempts = gs_resolveAttempts.load();
    counters.resolveRestarts = gs_resolveRestarts.load();
    counters.resolveUnstable = gs_resolveUnstable.load();
    counters.transparentCandidates = gs_transparentCandidates.load();
    return counters;
}

void wxWinUIResetInputRouterCounters()
{
    gs_resolveAttempts = 0;
    gs_resolveRestarts = 0;
    gs_resolveUnstable = 0;
    gs_transparentCandidates = 0;
}

#endif // wxUSE_WINUI3
