/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/inputrouter.h
// Purpose:     generation-safe native hit resolution for the WinUI bridge
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_INPUTROUTER_H_
#define _WX_WINUI_PRIVATE_INPUTROUTER_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/window.h"

class wxWinUINativeResolverImpl;

// A native input target is an identity, not merely a HWND. It couples the
// transient leaf HWND with weak logical wx/TLW objects and generations for
// the leaf and both wx shell handles. It can safely cross a re-entrant
// callback, but users must still resolve the point again before delivering
// delayed input.
class WXDLLIMPEXP_CORE wxWinUINativeTarget
{
public:
    wxWinUINativeTarget() = default;

    bool IsOk() const { return m_leafHwnd != nullptr; }
    bool IsValid() const;
    bool Matches(const wxWinUINativeTarget& other) const;

    wxWindow *GetWindow() const { return m_window.get(); }
    wxWindow *GetTLW() const { return m_tlw.get(); }
    WXHWND GetLeafHwnd() const { return m_leafHwnd; }
    WXHWND GetShellHwnd() const { return m_shellHwnd; }
    unsigned long long GetShellGeneration() const
        { return m_shellGeneration; }
    unsigned long long GetLeafGeneration() const
        { return m_leafGeneration; }

private:
    friend class wxWinUINativeResolverImpl;

    wxWeakRef<wxWindow> m_window;
    wxWeakRef<wxWindow> m_tlw;
    WXHWND m_leafHwnd = nullptr;
    WXHWND m_shellHwnd = nullptr;
    WXHWND m_tlwHwnd = nullptr;
    unsigned long long m_leafGeneration = 0;
    unsigned long long m_shellGeneration = 0;
    unsigned long long m_tlwGeneration = 0;
};

enum class wxWinUINativeArea
{
    Client,
    NonClient
};

enum class wxWinUIHitResolution
{
    Miss,
    Hit,
    Unstable
};

class WXDLLIMPEXP_CORE wxWinUINativeHit
{
public:
    wxWinUINativeHit() = default;

    bool IsOk() const { return m_target.IsOk(); }
    // This validates the generation-safe target identity only. Continuation
    // after a callback must use wxWinUIRefreshNativeHit(), which also proves
    // that the bridge/inner exclusions and the geometric hit are unchanged.
    bool IsValid() const { return m_target.IsValid(); }

    const wxWinUINativeTarget& GetTarget() const { return m_target; }
    LRESULT GetHitTest() const { return m_hitTest; }
    wxWinUINativeArea GetArea() const { return m_area; }
    const POINT& GetScreenPoint() const { return m_screen; }
    const POINT& GetClientPoint() const { return m_client; }
    WXHWND GetBridgeExclusionHwnd() const { return m_bridgeHwnd; }
    WXHWND GetInnerExclusionHwnd() const { return m_innerHwnd; }
    unsigned long long GetBridgeExclusionGeneration() const
        { return m_bridgeGeneration; }
    unsigned long long GetInnerExclusionGeneration() const
        { return m_innerGeneration; }

private:
    friend class wxWinUINativeResolverImpl;
    // Fills a hit reused from a previous resolution; see the function.
    friend WXDLLIMPEXP_CORE wxWinUIHitResolution
    wxWinUIResolveNativeHitReusing(wxWindow *tlw,
                                   const POINT& screenPoint,
                                   WXHWND bridge,
                                   WXHWND inner,
                                   const wxWinUINativeHit& previous,
                                   bool layoutUnchanged,
                                   wxWinUINativeHit *hit);

    wxWinUINativeTarget m_target;
    LRESULT m_hitTest = HTNOWHERE;
    wxWinUINativeArea m_area = wxWinUINativeArea::Client;
    POINT m_screen = { 0, 0 };
    POINT m_client = { 0, 0 };
    WXHWND m_bridgeHwnd = nullptr;
    WXHWND m_innerHwnd = nullptr;
    unsigned long long m_bridgeGeneration = 0;
    unsigned long long m_innerGeneration = 0;
};


struct WXDLLIMPEXP_CORE wxWinUIInputRouterCounters
{
    unsigned long long resolveAttempts = 0;
    unsigned long long resolveRestarts = 0;
    unsigned long long resolveUnstable = 0;
    unsigned long long transparentCandidates = 0;
};

// Resolve the deepest native target under a screen point while excluding the
// island bridge/input windows. HTTRANSPARENT advances to the next underlying
// sibling at the same Z level. If WM_NCHITTEST mutates the topology, the
// resolver restarts from a fresh snapshot and ultimately fails closed. A
// synchronous re-entry while resolving also fails closed as Unstable.
WXDLLIMPEXP_CORE wxWinUIHitResolution
wxWinUIResolveNativeHit(wxWindow *tlw,
                        const POINT& screenPoint,
                        WXHWND bridge,
                        WXHWND inner,
                        wxWinUINativeHit *hit);

// Resolve the deepest native wx window under a screen point while excluding
// the island bridge/input HWNDs, but without sending WM_NCHITTEST.  This is
// the WindowFromPoint()/ChildWindowFromPointEx() contract used by
// wxFindWindowAtPoint(): disabled children are included and HTTRANSPARENT is
// deliberately irrelevant.  The returned target remains generation-bound.
// It shares the same fail-closed re-entry boundary as native hit resolution.
WXDLLIMPEXP_CORE wxWinUIHitResolution
wxWinUIResolveNativeWindowAtPoint(wxWindow *tlw,
                                  const POINT& screenPoint,
                                  WXHWND bridge,
                                  WXHWND inner,
                                  wxWinUINativeTarget *target);

// Build a generation-safe identity for an exact native HWND already selected
// by USER32 (notably GetCapture()). Unknown/foreign HWNDs fail closed.
WXDLLIMPEXP_CORE bool
wxWinUIGetNativeTarget(wxWindow *tlw,
                       WXHWND hwnd,
                       wxWinUINativeTarget *target);

// Re-resolve the same screen point after a synchronous callback and require
// the exact generation-safe target, client/non-client area and HT* zone to be
// unchanged. This is the only safe way to continue dispatch after application
// code such as WM_SETCURSOR/WM_MOUSELEAVE, and is reusable by the DropBroker.
// Resolves the window under the pointer, reusing a previous resolution when
// the caller can guarantee that no native geometry mutation was reported since
// it was made.  Reuse still proves everything that could differ at the new
// position -- the target still owns the point, nothing of its own is above it,
// and its WM_NCHITTEST zone -- but it does so with a handful of queries on the
// windows concerned instead of walking and fingerprinting the whole tree,
// which is what a full resolution costs on every single mouse movement.
WXDLLIMPEXP_CORE wxWinUIHitResolution
wxWinUIResolveNativeHitReusing(wxWindow *tlw,
                               const POINT& screenPoint,
                               WXHWND bridge,
                               WXHWND inner,
                               const wxWinUINativeHit& previous,
                               bool layoutUnchanged,
                               wxWinUINativeHit *hit);

// layoutUnchanged tells this function that no native geometry mutation was
// reported between the original resolution and this call.  A pointer moves
// hundreds of times per second and each move needs this proof, so with that
// guarantee the expensive tree walk is replaced by re-checking the exact
// target: its generations, its visibility and that the point is still
// inside it.  Callers that cannot make the guarantee pass false and get
// the full re-resolution.
WXDLLIMPEXP_CORE bool
wxWinUIRefreshNativeHit(wxWindow *tlw,
                        const wxWinUINativeHit& expected,
                        WXHWND bridge,
                        WXHWND inner,
                        wxWinUINativeHit *refreshed,
                        bool layoutUnchanged = false);

WXDLLIMPEXP_CORE wxWinUIInputRouterCounters
wxWinUIGetInputRouterCounters();

WXDLLIMPEXP_CORE void wxWinUIResetInputRouterCounters();

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_INPUTROUTER_H_
