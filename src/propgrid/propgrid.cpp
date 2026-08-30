/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgrid.cpp
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Created:     2004-09-25
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcmemory.h"
    #include "wx/button.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/scrolwin.h"
    #include "wx/dirdlg.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/statusbr.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/textctrl.h"
    #include "wx/settings.h"
#endif

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/manager.h"
#include "wx/propgrid/private.h"
#include "wx/private/windowlifetime.h"

#if wxPG_USE_RENDERER_NATIVE
    #include "wx/renderer.h"
#endif

#include "wx/odcombo.h"

#include "wx/timer.h"
#include "wx/dcbuffer.h"
#include "wx/scopeguard.h"
#include "wx/display.h"
#include "wx/weakref.h"
#include "wx/app.h"

#ifdef __WXMSW__
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/private.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/private/wrapgtk.h"
#elif defined(__WXOSX_COCOA__)
    #include "wx/osx/private.h"
#elif defined(__WXQT__)
    #include <QtCore/QPointer>
    #include <QtWidgets/QWidget>
#endif

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Two pics for the expand / collapse buttons.
// Files are not supplied with this project (since it is
// recommended to use either custom or native rendering).
// If you want them, get wxTreeMultiCtrl by Jorgen Bodde,
// and copy xpm files from archive to wxPropertyGrid src directory
// (and also comment/undef wxPG_ICON_WIDTH in private.h
// and set wxPG_USE_RENDERER_NATIVE to 0).
#ifndef wxPG_ICON_WIDTH
  #if defined(__WXMAC__)
    #include "mac_collapse.xpm"
    #include "mac_expand.xpm"
  #elif defined(__WXGTK__)
    #include "linux_collapse.xpm"
    #include "linux_expand.xpm"
  #else
    #include "default_collapse.xpm"
    #include "default_expand.xpm"
  #endif
#endif


#define wxPG_YSPACING_MIN               1
#define wxPG_DEFAULT_VSPACING           2 // This matches .NET propertygrid's value,
                                          // but causes normal combobox to spill out under MSW
#define wxPG_DEFAULT_CURSOR             wxNullCursor
#define wxPG_PIXELS_PER_UNIT            m_lineHeight

//#define wxPG_TOOLTIP_DELAY              1000

// This is the number of pixels the expander button inside
// property cells (i.e. not in the grey margin area are
// adjusted.
#define IN_CELL_EXPANDER_BUTTON_X_ADJUST    2

namespace
{

#if wxPG_USE_EDITOR_CALLBACK_EPOCH
thread_local unsigned int gs_deferredEditorCallbackDepth = 0;

class EditorDeferredDeleteBatch;
// Intrusive queue head: TLS must remain trivial for MinGW process teardown.
// The batch nodes themselves are already owned pending-delete envelopes, so no
// auxiliary allocation or intentionally leaked TLS container is needed.
thread_local EditorDeferredDeleteBatch*
    gs_waitingDeferredEditorBatches = nullptr;
#endif

#ifdef __WXMSW__
thread_local bool gs_failNextDirectEditorParkingForTesting = false;
thread_local bool gs_failNextFallbackEditorParkingForTesting = false;
thread_local unsigned int gs_editorParkingHostCount = 0;
thread_local unsigned int gs_deferredEditorBatchCount = 0;

class EditorCreationTransaction;
thread_local EditorCreationTransaction* gs_activeEditorCreation = nullptr;

const wchar_t* const EDITOR_PARKING_COOKIE =
    L"wxPropertyGrid.EditorParkingIdentity";
const wchar_t* const EDITOR_PARKING_HOST_COOKIE =
    L"wxPropertyGrid.EditorParkingHostIdentity";
int gs_quarantinedEditorParkingHostCookie;

struct EditorParkingDpiApi
{
    typedef WXDPI_AWARENESS_CONTEXT
        (WINAPI *GetWindowContextFn)(HWND);
    typedef WXDPI_AWARENESS_CONTEXT
        (WINAPI *SetThreadContextFn)(WXDPI_AWARENESS_CONTEXT);
    typedef BOOL
        (WINAPI *ContextsEqualFn)(WXDPI_AWARENESS_CONTEXT,
                                  WXDPI_AWARENESS_CONTEXT);

    EditorParkingDpiApi()
        : getWindowContext(nullptr),
          setThreadContext(nullptr),
          contextsEqual(nullptr)
    {
        const HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
        if ( !user32 )
            return;

        getWindowContext = reinterpret_cast<GetWindowContextFn>(
            ::GetProcAddress(user32, "GetWindowDpiAwarenessContext"));
        setThreadContext = reinterpret_cast<SetThreadContextFn>(
            ::GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
        contextsEqual = reinterpret_cast<ContextsEqualFn>(
            ::GetProcAddress(user32, "AreDpiAwarenessContextsEqual"));
    }

    bool Equal(WXDPI_AWARENESS_CONTEXT lhs,
               WXDPI_AWARENESS_CONTEXT rhs) const
    {
        if ( !lhs || !rhs )
            return lhs == rhs;

        return contextsEqual ? contextsEqual(lhs, rhs) != FALSE : lhs == rhs;
    }

    GetWindowContextFn getWindowContext;
    SetThreadContextFn setThreadContext;
    ContextsEqualFn contextsEqual;
};

const EditorParkingDpiApi& GetEditorParkingDpiApi()
{
    static const EditorParkingDpiApi api;
    return api;
}

class EditorParkingDpiContextScope final
{
public:
    explicit EditorParkingDpiContextScope(
        WXDPI_AWARENESS_CONTEXT context)
        : m_setContext(GetEditorParkingDpiApi().setThreadContext),
          m_previousContext(nullptr)
    {
        if ( m_setContext && context )
            m_previousContext = m_setContext(context);
    }

    ~EditorParkingDpiContextScope()
    {
        if ( m_setContext && m_previousContext )
            m_setContext(m_previousContext);
    }

private:
    EditorParkingDpiApi::SetThreadContextFn m_setContext;
    WXDPI_AWARENESS_CONTEXT m_previousContext;
};

bool IsEditorParkingWindowOnCurrentThread(HWND hwnd)
{
    DWORD processId = 0;
    const DWORD threadId = ::GetWindowThreadProcessId(hwnd, &processId);
    return threadId != 0 &&
           threadId == ::GetCurrentThreadId() &&
           processId == ::GetCurrentProcessId();
}

bool IsDirectMessageOnlyWindow(HWND hwnd)
{
    HWND child = nullptr;
    while ( (child = ::FindWindowExW(HWND_MESSAGE,
                                     child,
                                     nullptr,
                                     nullptr)) )
    {
        if ( child == hwnd )
            return true;
    }

    return false;
}

struct EditorWrappedWindowSnapshot
{
    EditorWrappedWindowSnapshot(wxWindow* window_,
                                HWND hwnd_,
                                unsigned int depth_,
                                bool nativeDescendant_,
                                bool root_)
        : window(window_),
          hwnd(hwnd_),
          depth(depth_),
          nativeDescendant(nativeDescendant_),
          root(root_)
    {
    }

    wxWeakRef<wxWindow> window;
    HWND hwnd;
    unsigned int depth;
    bool nativeDescendant;
    bool root;
};

struct EditorNativeChildSnapshot
{
    EditorNativeChildSnapshot(HWND hwnd_, HWND parent_)
        : hwnd(hwnd_),
          parent(parent_)
    {
    }

    HWND hwnd;
    HWND parent;
};

class EditorNativeSnapshot final
{
public:
    EditorNativeSnapshot(wxWindow* editor, HWND expectedParent)
        : m_editor(editor),
          m_rootHwnd(reinterpret_cast<HWND>(editor->GetHandle())),
          m_expectedParent(expectedParent),
          m_rootContext(nullptr),
          m_topologyCaptured(false),
          m_valid(false),
          m_parked(false)
    {
        // Capture wx ownership before the first operation that can fail. Even
        // an invalid HWND snapshot must retain enough topology to prevent the
        // grid base destructor from deleting a live wrapper synchronously.
        CaptureWrappedWindow(editor, 0);
        m_topologyCaptured = !m_wrapped.empty();

        if ( !m_rootHwnd || !::IsWindow(m_rootHwnd) ||
             !IsEditorParkingWindowOnCurrentThread(m_rootHwnd) )
        {
            return;
        }

        const LONG_PTR style = ::GetWindowLongPtr(m_rootHwnd, GWL_STYLE);
        if ( !(style & WS_CHILD) || (style & WS_POPUP) ||
             ::GetParent(m_rootHwnd) != m_expectedParent )
        {
            return;
        }

        if ( !::SetPropW(m_rootHwnd,
                         EDITOR_PARKING_COOKIE,
                         reinterpret_cast<HANDLE>(this)) )
        {
            wxLogApiError(
                wxS("SetProp(property-grid editor parking identity)"),
                ::GetLastError());
            return;
        }

        // Mark every currently live logical wrapper, including an external
        // combo popup. EnumChildWindows() below additionally marks native
        // children without wx wrappers. USER32 removes properties with a
        // destroyed HWND, making these cookies an ABA guard as well.
        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( wrapped.root || !wrapped.hwnd )
                continue;

            if ( !::IsWindow(wrapped.hwnd) ||
                 !IsEditorParkingWindowOnCurrentThread(wrapped.hwnd) ||
                 !::SetPropW(wrapped.hwnd,
                             EDITOR_PARKING_COOKIE,
                             reinterpret_cast<HANDLE>(this)) )
            {
                if ( ::IsWindow(wrapped.hwnd) )
                {
                    wxLogApiError(
                        wxS("SetProp(property-grid wrapped editor identity)"),
                        ::GetLastError());
                }
                return;
            }
        }

        const EditorParkingDpiApi& dpi = GetEditorParkingDpiApi();
        if ( dpi.getWindowContext )
            m_rootContext = dpi.getWindowContext(m_rootHwnd);

        m_valid = true;
        const BOOL enumerated =
            ::EnumChildWindows(
                m_rootHwnd,
                &EditorNativeSnapshot::CaptureNativeChild,
                reinterpret_cast<LPARAM>(this));
        if ( !enumerated && ::GetWindow(m_rootHwnd, GW_CHILD) )
        {
            // EnumChildWindows() can return zero for a leaf window even
            // though there was no callback failure to report. Accept that
            // case only after independently proving that no native subtree
            // could have escaped the snapshot. If a direct child exists,
            // enumeration was incomplete and parking must fail closed.
            m_valid = false;
        }
    }

    ~EditorNativeSnapshot()
    {
        RemoveIdentityCookies();
    }

    bool IsValid() const { return m_valid; }
    bool HasCapturedTopology() const { return m_topologyCaptured; }
    bool IsParked() const { return m_parked; }
    void SetParked() { m_parked = true; }
    HWND GetRootHwnd() const { return m_rootHwnd; }
    HWND GetExpectedParent() const { return m_expectedParent; }
    WXDPI_AWARENESS_CONTEXT GetRootContext() const
    {
        return m_rootContext;
    }

    wxWindow* GetLiveEditor() const
    {
        wxWindow* const editor = m_editor.get();
        if ( !editor ||
             reinterpret_cast<HWND>(editor->GetHandle()) != m_rootHwnd )
        {
            return nullptr;
        }

        return editor;
    }

    bool MatchesRootIdentity() const
    {
        return GetLiveEditor() &&
               ::IsWindow(m_rootHwnd) &&
               ::GetPropW(m_rootHwnd, EDITOR_PARKING_COOKIE) ==
                   reinterpret_cast<HANDLE>(
                       const_cast<EditorNativeSnapshot*>(this)) &&
               IsEditorParkingWindowOnCurrentThread(m_rootHwnd);
    }

    bool RecoverOrphanedIdentityAfterAncestorDestruction(
        HWND destroyedAncestor)
    {
        if ( ::IsWindow(destroyedAncestor) ||
             !m_topologyCaptured ||
             !m_rootHwnd ||
             !::IsWindow(m_rootHwnd) ||
             ::GetParent(m_rootHwnd) != nullptr ||
             !IsEditorParkingWindowOnCurrentThread(m_rootHwnd) )
        {
            return false;
        }

        wxWindow* const editor = GetLiveEditor();
        const LONG_PTR style =
            ::GetWindowLongPtr(m_rootHwnd, GWL_STYLE);
        if ( !editor ||
             wxFindWinFromHandle(m_rootHwnd) != editor ||
             !(style & WS_CHILD) ||
             (style & WS_POPUP) )
        {
            return false;
        }

        // USER32 may orphan a still-associated child while tearing down the
        // WinUI bridge ancestor and drop its window properties. Prove every wx
        // and native identity independently before restoring our cookies.
        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( !wrapped.root && !wrapped.nativeDescendant )
                continue;

            wxWindow* const window = wrapped.window.get();
            if ( !window ||
                 !wrapped.hwnd ||
                 !::IsWindow(wrapped.hwnd) ||
                 reinterpret_cast<HWND>(window->GetHandle()) != wrapped.hwnd ||
                 wxFindWinFromHandle(wrapped.hwnd) != window ||
                 !IsEditorParkingWindowOnCurrentThread(wrapped.hwnd) )
            {
                return false;
            }
        }

        for ( const EditorNativeChildSnapshot& child : m_nativeChildren )
        {
            if ( !::IsWindow(child.hwnd) ||
                 ::GetParent(child.hwnd) != child.parent ||
                 !::IsChild(m_rootHwnd, child.hwnd) ||
                 !IsEditorParkingWindowOnCurrentThread(child.hwnd) )
            {
                return false;
            }
        }

        if ( !::SetPropW(m_rootHwnd,
                         EDITOR_PARKING_COOKIE,
                         reinterpret_cast<HANDLE>(this)) )
        {
            return false;
        }

        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( wrapped.root || !wrapped.nativeDescendant ||
                 !wrapped.hwnd )
            {
                continue;
            }
            if ( !::SetPropW(wrapped.hwnd,
                             EDITOR_PARKING_COOKIE,
                             reinterpret_cast<HANDLE>(this)) )
            {
                return false;
            }
        }
        for ( const EditorNativeChildSnapshot& child : m_nativeChildren )
        {
            if ( !::SetPropW(child.hwnd,
                             EDITOR_PARKING_COOKIE,
                             reinterpret_cast<HANDLE>(this)) )
            {
                return false;
            }
        }

        return MatchesRootIdentity() &&
               VerifyNativeDescendantIdentity();
    }

    bool HideExternalPopupWindows()
    {
        if ( wxComboCtrl* const combo =
                 wxDynamicCast(m_editor.get(), wxComboCtrl) )
        {
            if ( combo->IsPopupShown() )
            {
                combo->Dismiss();
                if ( !MatchesRootIdentity() )
                    return false;
            }
            else if ( !combo->IsPopupWindowState(wxComboCtrl::Hidden) )
            {
                // A transient deactivation can leave the combo in Closing
                // with DismissOnDeactivate already queued. Complete its
                // callback now, while SetFocus() still has a valid editor
                // handle; the queued notification will then observe Hidden
                // and become a no-op after a last-resort dissociation.
                combo->OnPopupDismiss(false);
                if ( !MatchesRootIdentity() )
                    return false;
            }
        }

        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( wrapped.root || wrapped.nativeDescendant )
                continue;

            wxWindow* const window = wrapped.window.get();
            if ( !window )
                continue;

            if ( !WrappedIdentityMatches(wrapped) )
                return false;

            const LONG_PTR style =
                ::GetWindowLongPtr(wrapped.hwnd, GWL_STYLE);
            if ( style & WS_POPUP )
            {
                ::ShowWindow(wrapped.hwnd, SW_HIDE);

                if ( !MatchesRootIdentity() ||
                     (wrapped.window.get() &&
                      !WrappedIdentityMatches(wrapped)) )
                    return false;
            }
        }

        return true;
    }

    bool ReleaseFocusFromNativeSubtree()
    {
        const HWND focused = ::GetFocus();
        if ( focused &&
             (focused == m_rootHwnd ||
              ::IsChild(m_rootHwnd, focused)) )
        {
            // Do this while every wx/HWND association is still intact. Hiding
            // or dissociating a focused native child first can make the ensuing
            // focus-restoration callback call SetFocus() on a null handle.
            ::SetFocus(nullptr);
            if ( !MatchesRootIdentity() )
                return false;

            const HWND focusAfter = ::GetFocus();
            return !focusAfter ||
                   (focusAfter != m_rootHwnd &&
                    !::IsChild(m_rootHwnd, focusAfter));
        }

        return true;
    }

    bool VerifyParkedUnder(HWND parent) const
    {
        if ( !MatchesRootIdentity() ||
             ::IsWindowVisible(m_rootHwnd) )
        {
            return false;
        }

        if ( parent == HWND_MESSAGE )
        {
            if ( !IsDirectMessageOnlyWindow(m_rootHwnd) )
                return false;
        }
        else if ( ::GetParent(m_rootHwnd) != parent )
        {
            return false;
        }

        const LONG_PTR style = ::GetWindowLongPtr(m_rootHwnd, GWL_STYLE);
        if ( !(style & WS_CHILD) || (style & WS_POPUP) )
            return false;

        return VerifyNativeDescendantIdentity();
    }

    bool IsRootStillInside(HWND possibleAncestor) const
    {
        return MatchesRootIdentity() &&
               (::GetParent(m_rootHwnd) == possibleAncestor ||
                ::IsChild(possibleAncestor, m_rootHwnd));
    }

    // DestroyWindow(parent) can be entered from the editor's WM_KILLFOCUS.
    // USER32 then detaches the child and removes its properties while the
    // child's native callback is still on the stack; the wx association may
    // outlive the native HWND until that callback unwinds. This is not a
    // parkable editor, but neither is it safe to delete or dissociate its wx
    // wrapper from inside the callback.
    // Keep the batch alive until the enclosing callback epoch has ended, when
    // normal WM_NCDESTROY processing has completed the association teardown.
    bool IsAwaitingNativeDestructionAfterAncestor(
        HWND destroyedAncestor) const
    {
        if ( ::IsWindow(destroyedAncestor) ||
             !m_topologyCaptured ||
             !m_rootHwnd )
        {
            return false;
        }

        // A destroyed HWND can remain in the wx handle association until its
        // native callback returns. The numeric handle itself is already gone,
        // so retaining the wrapper in the deferred batch is unconditionally
        // safe; deleting it from the current native stack is not.
        if ( !::IsWindow(m_rootHwnd) )
            return GetLiveEditor() != nullptr;

        if ( ::GetParent(m_rootHwnd) != nullptr ||
             HasFocusInNativeSubtree() )
        {
            return false;
        }

        wxWindow* const editor = GetLiveEditor();
        wxWindow* const associated = wxFindWinFromHandle(m_rootHwnd);
        const LONG_PTR style =
            ::GetWindowLongPtr(m_rootHwnd, GWL_STYLE);

        // During the tail of WM_NCDESTROY, IsWindow() can still report true
        // even though USER32 has already cleared the style, thread identity
        // and our property. wxWidgets deliberately keeps the association until
        // the native callback returns. Keeping the batch alive is safe while
        // that association still names the captured wrapper: Drain() later
        // proves that the wrapper and native subtree are both gone, and fails
        // closed if an application merely removed the cookie itself.
        if ( editor &&
             associated == editor &&
             ::GetPropW(m_rootHwnd, EDITOR_PARKING_COOKIE) !=
                 reinterpret_cast<HANDLE>(
                     const_cast<EditorNativeSnapshot*>(this)) )
        {
            return true;
        }

        return editor &&
               (!associated || associated == editor) &&
               IsEditorParkingWindowOnCurrentThread(m_rootHwnd) &&
               (style & WS_CHILD) &&
               !(style & WS_POPUP);
    }

    // Clear wx wrappers only for handles that USER32 is about to destroy with
    // the grid. Logical popup children are intentionally excluded because
    // their HWNDs are not native descendants of the editor root.
    bool DissociateNativeSubtreeBeforeParentDestruction(HWND dyingParent)
    {
        if ( !m_topologyCaptured || m_wrapped.empty() ||
             !MatchesRootIdentity() ||
             HasFocusInNativeSubtree() ||
             !VerifyNativeDescendantIdentity() )
        {
            return false;
        }

        if ( MatchesRootIdentity() && !IsRootStillInside(dyingParent) )
        {
            // A SetParent() may report an error even though it completed the
            // move. Only accept this case after validating the entire captured
            // native subtree: VerifyParkedUnder() may have failed because a
            // descendant changed identity, not because of SetParent's return.
            const HWND actualParent = ::GetParent(m_rootHwnd);
            return !::IsWindowVisible(m_rootHwnd) &&
                   actualParent &&
                   ((actualParent == HWND_MESSAGE &&
                     IsDirectMessageOnlyWindow(m_rootHwnd)) ||
                    actualParent != HWND_MESSAGE);
        }

        bool safeToDeleteWrappers = true;
        for ( auto it = m_wrapped.rbegin(); it != m_wrapped.rend(); ++it )
        {
            if ( !it->root && !it->nativeDescendant )
                continue;

            wxWindow* const window = it->window.get();
            if ( !window )
                continue;

            if ( reinterpret_cast<HWND>(window->GetHandle()) != it->hwnd )
            {
                safeToDeleteWrappers = false;
                continue;
            }

            if ( !it->hwnd || !::IsWindow(it->hwnd) )
            {
                window->DissociateHandle();
                continue;
            }

            const bool stillInNativeSubtree =
                it->root || (::IsWindow(m_rootHwnd) &&
                             ::IsChild(m_rootHwnd, it->hwnd));
            if ( !stillInNativeSubtree ||
                 ::GetPropW(it->hwnd, EDITOR_PARKING_COOKIE) !=
                     reinterpret_cast<HANDLE>(this) )
            {
                safeToDeleteWrappers = false;
                continue;
            }

            window->DissociateHandle();
            ::RemovePropW(it->hwnd, EDITOR_PARKING_COOKIE);
        }

        return safeToDeleteWrappers;
    }

    bool AllWrappersGone() const
    {
        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            // External popup wrappers own an independent top-level HWND and
            // follow their normal asynchronous Destroy() path. They were
            // hidden and never reparented, but need not die in this batch.
            if ( (wrapped.root || wrapped.nativeDescendant) &&
                 wrapped.window.get() )
                return false;
        }

        return true;
    }

    bool RootHwndGone() const
    {
        // A live HWND with a missing/changed cookie is ambiguous, not gone.
        // Treating it as destroyed could free this snapshot while the HWND
        // still refers to it (or silently abandon a directly parked root).
        return !::IsWindow(m_rootHwnd);
    }

    bool AllCapturedNativeHwndsGone() const
    {
        if ( ::IsWindow(m_rootHwnd) )
            return false;

        // A native child can be reparented out by a destruction callback and
        // survive the root. Never free this snapshot (and its ABA cookies) in
        // that case, even when no wx wrapper remains.
        for ( const EditorNativeChildSnapshot& child : m_nativeChildren )
        {
            if ( ::IsWindow(child.hwnd) )
                return false;
        }

        return true;
    }

private:
    bool HasFocusInNativeSubtree() const
    {
        const HWND focused = ::GetFocus();
        return focused &&
               (focused == m_rootHwnd ||
                (::IsWindow(m_rootHwnd) &&
                 ::IsChild(m_rootHwnd, focused)));
    }

    bool WrappedIdentityMatches(
        const EditorWrappedWindowSnapshot& wrapped) const
    {
        wxWindow* const window = wrapped.window.get();
        return window &&
               wrapped.hwnd &&
               reinterpret_cast<HWND>(window->GetHandle()) == wrapped.hwnd &&
               ::IsWindow(wrapped.hwnd) &&
               ::GetPropW(wrapped.hwnd, EDITOR_PARKING_COOKIE) ==
                   reinterpret_cast<HANDLE>(
                       const_cast<EditorNativeSnapshot*>(this)) &&
               IsEditorParkingWindowOnCurrentThread(wrapped.hwnd);
    }

    bool VerifyNativeDescendantIdentity() const
    {
        for ( const EditorNativeChildSnapshot& child : m_nativeChildren )
        {
            if ( !::IsWindow(child.hwnd) ||
                 ::GetPropW(child.hwnd, EDITOR_PARKING_COOKIE) !=
                     reinterpret_cast<HANDLE>(
                         const_cast<EditorNativeSnapshot*>(this)) ||
                 ::GetParent(child.hwnd) != child.parent ||
                 !::IsChild(m_rootHwnd, child.hwnd) ||
                 !IsEditorParkingWindowOnCurrentThread(child.hwnd) )
            {
                return false;
            }
        }

        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( !wrapped.root && !wrapped.nativeDescendant )
                continue;

            if ( wrapped.window.get() && !WrappedIdentityMatches(wrapped) )
                return false;
        }

        return true;
    }

    static BOOL CALLBACK CaptureNativeChild(HWND hwnd, LPARAM data)
    {
        EditorNativeSnapshot* const self =
            reinterpret_cast<EditorNativeSnapshot*>(data);
        self->m_nativeChildren.emplace_back(hwnd, ::GetParent(hwnd));
        if ( !::SetPropW(hwnd,
                         EDITOR_PARKING_COOKIE,
                         reinterpret_cast<HANDLE>(self)) )
        {
            self->m_valid = false;
            return TRUE;
        }

        return TRUE;
    }

    void CaptureWrappedWindow(wxWindow* window, unsigned int depth)
    {
        const HWND hwnd = reinterpret_cast<HWND>(window->GetHandle());
        const bool root = depth == 0;
        const bool nativeDescendant =
            root ||
            (hwnd && ::IsWindow(hwnd) && ::IsChild(m_rootHwnd, hwnd));

        m_wrapped.emplace_back(
            window, hwnd, depth, nativeDescendant, root);

        for ( wxWindow* const child : window->GetChildren() )
            CaptureWrappedWindow(child, depth + 1);
    }

    void RemoveIdentityCookies()
    {
        if ( m_rootHwnd && ::IsWindow(m_rootHwnd) &&
             ::GetPropW(m_rootHwnd, EDITOR_PARKING_COOKIE) ==
                 reinterpret_cast<HANDLE>(this) )
        {
            ::RemovePropW(m_rootHwnd, EDITOR_PARKING_COOKIE);
        }

        for ( const EditorNativeChildSnapshot& child : m_nativeChildren )
        {
            if ( ::IsWindow(child.hwnd) &&
                 ::GetPropW(child.hwnd, EDITOR_PARKING_COOKIE) ==
                     reinterpret_cast<HANDLE>(this) )
            {
                ::RemovePropW(child.hwnd, EDITOR_PARKING_COOKIE);
            }
        }

        for ( const EditorWrappedWindowSnapshot& wrapped : m_wrapped )
        {
            if ( wrapped.hwnd && ::IsWindow(wrapped.hwnd) &&
                 ::GetPropW(wrapped.hwnd, EDITOR_PARKING_COOKIE) ==
                     reinterpret_cast<HANDLE>(this) )
            {
                ::RemovePropW(wrapped.hwnd, EDITOR_PARKING_COOKIE);
            }
        }
    }

    wxWeakRef<wxWindow> m_editor;
    HWND m_rootHwnd;
    HWND m_expectedParent;
    WXDPI_AWARENESS_CONTEXT m_rootContext;
    std::vector<EditorWrappedWindowSnapshot> m_wrapped;
    std::vector<EditorNativeChildSnapshot> m_nativeChildren;
    bool m_topologyCaptured;
    bool m_valid;
    bool m_parked;
};

enum class EditorParkingOutcome
{
    Parked,
    Destroyed,
    Failed
};
#endif // __WXMSW__

#if wxPG_USE_EDITOR_CALLBACK_EPOCH
struct EditorDeferredObjectSlot
{
    EditorDeferredObjectSlot(wxObject* identity_, wxEvtHandler* object_)
        : identity(identity_),
          object(object_)
    {
    }

    wxObject* identity;
    wxEvtHandlerRef object;
};

#ifdef __WXMSW__
struct EditorParkingHost
{
    EditorParkingHost(HWND hwnd_,
                      WXDPI_AWARENESS_CONTEXT context_,
                      std::unique_ptr<int> identity_)
        : hwnd(hwnd_),
          context(context_),
          identity(std::move(identity_)),
          quarantined(false)
    {
    }

    HWND hwnd;
    WXDPI_AWARENESS_CONTEXT context;
    std::unique_ptr<int> identity;
    bool quarantined;
};
#endif

class EditorDeferredDeleteBatch final : public wxObject
{
public:
    EditorDeferredDeleteBatch()
        : m_waitingNext(nullptr),
          m_drained(false),
          m_draining(false),
          m_invariantFailure(false)
    {
#ifdef __WXMSW__
        ++gs_deferredEditorBatchCount;
#endif
    }

    ~EditorDeferredDeleteBatch() override
    {
        Drain();
#ifdef __WXMSW__
        wxASSERT(gs_deferredEditorBatchCount != 0);
        --gs_deferredEditorBatchCount;
#endif
    }

#ifdef __WXMSW__
    EditorNativeSnapshot* AddEditorSnapshot(wxWindow* editor,
                                            HWND expectedParent)
    {
        std::unique_ptr<EditorNativeSnapshot> snapshot(
            new EditorNativeSnapshot(editor, expectedParent));
        EditorNativeSnapshot* const result = snapshot.get();
        m_editorSnapshots.push_back(std::move(snapshot));
        return result;
    }
#endif

    static constexpr size_t InvalidObjectIndex =
        static_cast<size_t>(-1);

    // Adopt every candidate before any operation capable of dispatching a
    // window event. This transfers any pre-existing wxPendingDelete entry and
    // prevents nested idle processing from deleting a later member while an
    // earlier editor is being parked.
    size_t AdoptObject(wxObject* object)
    {
        if ( !object )
            return InvalidObjectIndex;

        if ( !m_adoptedObjects.insert(object).second )
            return InvalidObjectIndex;

        while ( wxPendingDelete.Member(object) )
            wxPendingDelete.DeleteObject(object);

        // PropertyGrid only queues wxWindow controls and their wxEvtHandler
        // forwarders. Both are wxTrackable, which is essential here: deleting
        // one member may synchronously delete a later member.
        wxEvtHandler* const trackable =
            wxDynamicCast(object, wxEvtHandler);
        if ( !trackable )
        {
            wxFAIL_MSG(
                wxS("Untrackable object in deferred editor teardown"));
            m_invariantFailure = true;
            return InvalidObjectIndex; // fail closed: deliberately leaked
        }

        m_objects.emplace_back(object, trackable);
        return m_objects.size() - 1;
    }

    wxObject* GetLiveObject(size_t index) const
    {
        if ( index >= m_objects.size() )
            return nullptr;

        return m_objects[index].object.get();
    }

    void ReleaseObject(size_t index)
    {
        if ( index >= m_objects.size() )
            return;

        EditorDeferredObjectSlot& slot = m_objects[index];
        m_adoptedObjects.erase(slot.identity);
        slot.identity = nullptr;
        slot.object.Release();
    }

    void MarkInvariantFailure()
    {
        m_invariantFailure = true;
    }

    EditorDeferredDeleteBatch* GetWaitingNext() const
    {
        return m_waitingNext;
    }

    void SetWaitingNext(EditorDeferredDeleteBatch* next)
    {
        m_waitingNext = next;
    }

    bool HasWork() const
    {
        return !m_objects.empty() ||
#ifdef __WXMSW__
               !m_editorSnapshots.empty() ||
               !m_hosts.empty() ||
#endif
               m_invariantFailure;
    }

#ifdef __WXMSW__
    EditorParkingOutcome ParkEditor(EditorNativeSnapshot& snapshot)
    {
        if ( !snapshot.IsValid() || !snapshot.MatchesRootIdentity() )
            return snapshot.GetLiveEditor()
                ? EditorParkingOutcome::Failed
                : EditorParkingOutcome::Destroyed;

        if ( !snapshot.HideExternalPopupWindows() )
            return snapshot.GetLiveEditor()
                ? EditorParkingOutcome::Failed
                : EditorParkingOutcome::Destroyed;

        if ( !snapshot.ReleaseFocusFromNativeSubtree() )
        {
            if ( !snapshot.MatchesRootIdentity() )
            {
                return snapshot.GetLiveEditor()
                    ? EditorParkingOutcome::Failed
                    : EditorParkingOutcome::Destroyed;
            }

            // WM_KILLFOCUS is allowed to re-enter and transiently restore
            // focus to the same editor. Retry exactly once after that callback
            // has fully returned; never loop on a hostile focus policy.
            if ( !snapshot.ReleaseFocusFromNativeSubtree() )
            {
                return snapshot.GetLiveEditor()
                    ? EditorParkingOutcome::Failed
                    : EditorParkingOutcome::Destroyed;
            }
        }

        HWND const editorHwnd = snapshot.GetRootHwnd();
        ::ShowWindow(editorHwnd, SW_HIDE);
        if ( !snapshot.MatchesRootIdentity() )
            return EditorParkingOutcome::Destroyed;
        if ( ::IsWindowVisible(editorHwnd) )
            return EditorParkingOutcome::Failed;

        const bool failDirect =
            gs_failNextDirectEditorParkingForTesting;
        gs_failNextDirectEditorParkingForTesting = false;

        DWORD directError = ERROR_SUCCESS;
        if ( TrySetParent(snapshot, HWND_MESSAGE, failDirect, &directError) )
        {
            snapshot.SetParked();
            return EditorParkingOutcome::Parked;
        }

        if ( !snapshot.GetLiveEditor() )
            return EditorParkingOutcome::Destroyed;

        const HWND hostHwnd = EnsureHost(snapshot);
        if ( !hostHwnd )
            return EditorParkingOutcome::Failed;
        if ( !snapshot.MatchesRootIdentity() )
            return snapshot.GetLiveEditor()
                ? EditorParkingOutcome::Failed
                : EditorParkingOutcome::Destroyed;

        const bool failFallback =
            gs_failNextFallbackEditorParkingForTesting;
        gs_failNextFallbackEditorParkingForTesting = false;

        DWORD fallbackError = ERROR_SUCCESS;
        if ( TrySetParent(snapshot,
                          hostHwnd,
                          failFallback,
                          &fallbackError) )
        {
            snapshot.SetParked();
            return EditorParkingOutcome::Parked;
        }

        if ( snapshot.GetLiveEditor() )
        {
            wxLogApiError(
                wxS("SetParent(property-grid editor parking host)"),
                fallbackError != ERROR_SUCCESS
                    ? fallbackError
                    : directError);
            return EditorParkingOutcome::Failed;
        }

        return EditorParkingOutcome::Destroyed;
    }
#endif

    void Drain()
    {
        if ( m_drained || m_draining )
            return;

        m_draining = true;
#ifdef __WXMSW__
        wxASSERT_MSG(
            ::GetCurrentThreadId() == m_threadId,
            wxS("Property-grid deferred editor batch changed thread"));
#endif

        // Clear each slot before invoking user destructors. If one of them
        // enters a nested idle loop, this batch is no longer globally queued
        // and no object can be replayed.
        for ( EditorDeferredObjectSlot& slot : m_objects )
        {
            wxObject* const current = slot.object.get();
            m_adoptedObjects.erase(slot.identity);
            slot.identity = nullptr;
            // Drop our tracker before invoking the destructor. If it destroys
            // another batch member, that member's still-live tracker becomes
            // null and the later slot is skipped rather than double-deleted.
            slot.object.Release();
            delete current;
        }

#ifdef __WXMSW__
        bool wrappersGone = true;
        bool rootsGone = true;
        bool nativeHwndsGone = true;
        for ( const std::unique_ptr<EditorNativeSnapshot>& snapshot :
                  m_editorSnapshots )
        {
            wrappersGone = snapshot->AllWrappersGone() && wrappersGone;
            rootsGone = snapshot->RootHwndGone() && rootsGone;
            nativeHwndsGone =
                snapshot->AllCapturedNativeHwndsGone() &&
                nativeHwndsGone;
        }

        if ( m_invariantFailure || !wrappersGone || !rootsGone ||
             !nativeHwndsGone )
        {
            wxFAIL_MSG(
                wxS("Deferred property-grid editor survived its batch"));
            QuarantineHosts();

            // Cookies on surviving HWNDs must never point at freed storage.
            // This is an exceptional fail-closed leak, intentionally visible
            // through the non-zero host counter where a host exists.
            for ( std::unique_ptr<EditorNativeSnapshot>& snapshot :
                      m_editorSnapshots )
            {
                snapshot.release();
            }
        }
        else
        {
            ReleaseHosts();
            m_editorSnapshots.clear();
        }
#endif

        m_draining = false;
        m_drained = true;
    }

private:
#ifdef __WXMSW__
    static bool TrySetParent(EditorNativeSnapshot& snapshot,
                             HWND parent,
                             bool failForTesting,
                             DWORD* error)
    {
        if ( failForTesting )
        {
            *error = ERROR_ACCESS_DENIED;
            return false;
        }

        ::SetLastError(ERROR_SUCCESS);
        const HWND oldParent =
            ::SetParent(snapshot.GetRootHwnd(), parent);
        *error = ::GetLastError();

        // SetParent() is synchronous and can run arbitrary window procedures.
        // Its return value alone is not an identity or postcondition check.
        if ( snapshot.VerifyParkedUnder(parent) )
            return true;

        if ( !oldParent && *error == ERROR_SUCCESS )
            *error = ERROR_INVALID_WINDOW_HANDLE;
        return false;
    }

    HWND EnsureHost(const EditorNativeSnapshot& snapshot)
    {
        const EditorParkingDpiApi& dpi = GetEditorParkingDpiApi();
        for ( EditorParkingHost& host : m_hosts )
        {
            if ( host.quarantined ||
                 !dpi.Equal(host.context, snapshot.GetRootContext()) )
            {
                continue;
            }

            if ( !::IsWindow(host.hwnd) ||
                 !IsDirectMessageOnlyWindow(host.hwnd) ||
                 ::GetPropW(host.hwnd, EDITOR_PARKING_HOST_COOKIE) !=
                     reinterpret_cast<HANDLE>(host.identity.get()) )
            {
                m_invariantFailure = true;
                QuarantineHost(host);
                continue;
            }

            return host.hwnd;
        }

        EditorParkingDpiContextScope contextScope(snapshot.GetRootContext());
        const HWND hostHwnd =
            ::CreateWindowExW(
                0,
                L"STATIC",
                L"",
                WS_CHILD,
                0, 0, 0, 0,
                HWND_MESSAGE,
                nullptr,
                nullptr,
                nullptr);
        if ( !hostHwnd )
        {
            wxLogApiError(
                wxS("CreateWindowEx(property-grid editor parking host)"),
                ::GetLastError());
            return nullptr;
        }

        WXDPI_AWARENESS_CONTEXT hostContext = nullptr;
        if ( dpi.getWindowContext )
            hostContext = dpi.getWindowContext(hostHwnd);

        std::unique_ptr<int> hostIdentity(new int);
        const bool hostIsValid =
            IsEditorParkingWindowOnCurrentThread(hostHwnd) &&
            IsDirectMessageOnlyWindow(hostHwnd) &&
            dpi.Equal(hostContext, snapshot.GetRootContext()) &&
            ::SetPropW(hostHwnd,
                       EDITOR_PARKING_HOST_COOKIE,
                       reinterpret_cast<HANDLE>(hostIdentity.get()));
        if ( !hostIsValid )
        {
            const DWORD error = ::GetLastError();
            if ( !::DestroyWindow(hostHwnd) &&
                 ::IsWindow(hostHwnd) &&
                 ::GetPropW(hostHwnd, EDITOR_PARKING_HOST_COOKIE) ==
                     reinterpret_cast<HANDLE>(hostIdentity.get()) )
            {
                ::RemovePropW(hostHwnd, EDITOR_PARKING_HOST_COOKIE);
                if ( ::GetPropW(hostHwnd, EDITOR_PARKING_HOST_COOKIE) ==
                     reinterpret_cast<HANDLE>(hostIdentity.get()) )
                {
                    hostIdentity.release();
                }
            }
            if ( error != ERROR_SUCCESS )
            {
                wxLogApiError(
                    wxS("Create property-grid editor parking host"),
                    error);
            }
            return nullptr;
        }

        m_hosts.emplace_back(
            hostHwnd, hostContext, std::move(hostIdentity));
        ++gs_editorParkingHostCount;
        return hostHwnd;
    }

    void ReleaseHosts()
    {
        for ( EditorParkingHost& host : m_hosts )
        {
            if ( host.quarantined )
                continue;

            const bool identityMatches =
                ::IsWindow(host.hwnd) &&
                ::GetPropW(host.hwnd, EDITOR_PARKING_HOST_COOKIE) ==
                    reinterpret_cast<HANDLE>(host.identity.get());
            const bool hasChildren =
                identityMatches &&
                ::GetWindow(host.hwnd, GW_CHILD) != nullptr;

            if ( !identityMatches || hasChildren )
            {
                wxFAIL_MSG(
                    wxS("Invalid property-grid editor parking host"));
                QuarantineHost(host);
                continue;
            }

            // Keep the identity cookie installed until DestroyWindow succeeds.
            // If it fails, QuarantineHost() can still replace our pointer with
            // the permanent quarantine marker.
            if ( !::DestroyWindow(host.hwnd) )
            {
                wxLogApiError(
                    wxS("DestroyWindow(property-grid editor parking host)"),
                    ::GetLastError());
                QuarantineHost(host);
                continue;
            }

            wxASSERT(gs_editorParkingHostCount != 0);
            --gs_editorParkingHostCount;
            host.hwnd = nullptr;
        }

        m_hosts.erase(
            std::remove_if(
                m_hosts.begin(),
                m_hosts.end(),
                [](const EditorParkingHost& host)
                {
                    return !host.hwnd;
                }),
            m_hosts.end());
    }

    void QuarantineHost(EditorParkingHost& host)
    {
        if ( !host.hwnd || host.quarantined )
            return;

        if ( ::IsWindow(host.hwnd) )
        {
            HANDLE const cookie =
                ::GetPropW(host.hwnd, EDITOR_PARKING_HOST_COOKIE);
            if ( cookie == reinterpret_cast<HANDLE>(host.identity.get()) ||
                 cookie == nullptr )
            {
                if ( !::SetPropW(
                         host.hwnd,
                         EDITOR_PARKING_HOST_COOKIE,
                         reinterpret_cast<HANDLE>(
                             &gs_quarantinedEditorParkingHostCookie)) )
                {
                    ::RemovePropW(
                        host.hwnd,
                        EDITOR_PARKING_HOST_COOKIE);
                    if ( ::GetPropW(
                             host.hwnd,
                             EDITOR_PARKING_HOST_COOKIE) ==
                         reinterpret_cast<HANDLE>(host.identity.get()) )
                    {
                        // The HWND still references this token: make just the
                        // stable token permanent, never the batch object.
                        host.identity.release();
                    }
                }
            }
        }
        host.quarantined = true;
    }

    void QuarantineHosts()
    {
        for ( EditorParkingHost& host : m_hosts )
            QuarantineHost(host);
    }
#endif

    std::vector<EditorDeferredObjectSlot> m_objects;
    std::unordered_set<wxObject*> m_adoptedObjects;
#ifdef __WXMSW__
    std::vector<std::unique_ptr<EditorNativeSnapshot>> m_editorSnapshots;
    std::vector<EditorParkingHost> m_hosts;
    const DWORD m_threadId = ::GetCurrentThreadId();
#endif
    EditorDeferredDeleteBatch* m_waitingNext;
    bool m_drained;
    bool m_draining;
    bool m_invariantFailure;
};

#ifdef __WXMSW__
// A custom editor can create one or more child controls and destroy the grid
// before CreateControls() returns, i.e. before m_wndEditor/m_wndEditor2 can be
// assigned. Keep a stack-local baseline discoverable through a trivial TLS
// pointer so the grid destructor can adopt these otherwise invisible children.
class EditorCreationTransaction final
{
public:
    explicit EditorCreationTransaction(wxPropertyGrid* grid)
        : m_grid(grid),
          m_previous(gs_activeEditorCreation)
    {
        for ( wxWindow* const child : grid->GetChildren() )
            m_childrenBefore.insert(child);
        gs_activeEditorCreation = this;
    }

    ~EditorCreationTransaction()
    {
        wxASSERT(gs_activeEditorCreation == this);
        gs_activeEditorCreation = m_previous;
    }

    static void CollectNewChildren(wxPropertyGrid* grid,
                                   std::vector<wxWindow*>& children)
    {
        for ( EditorCreationTransaction* transaction =
                  gs_activeEditorCreation;
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_grid != grid )
                continue;

            for ( wxWindow* const child : grid->GetChildren() )
            {
                if ( !transaction->m_childrenBefore.count(child) &&
                     std::find(children.begin(), children.end(), child) ==
                         children.end() )
                {
                    children.push_back(child);
                }
            }
        }
    }

private:
    wxPropertyGrid* const m_grid;
    EditorCreationTransaction* const m_previous;
    std::unordered_set<wxWindow*> m_childrenBefore;
};
#endif

void PublishDeferredEditorBatch(EditorDeferredDeleteBatch* batch)
{
    if ( !batch )
        return;

    // Queue one envelope, never its constituent editor/handler objects. The
    // global pending-delete implementation removes the envelope before
    // deleting it, so a nested idle from an editor destructor cannot release
    // the host or replay another member of this batch.
    if ( !wxPendingDelete.Member(batch) )
        wxPendingDelete.Append(batch);
    wxWakeUpIdle();
}

void QueueDeferredEditorBatch(EditorDeferredDeleteBatch* batch)
{
    if ( gs_deferredEditorCallbackDepth )
    {
        batch->SetWaitingNext(gs_waitingDeferredEditorBatches);
        gs_waitingDeferredEditorBatches = batch;
    }
    else
        PublishDeferredEditorBatch(batch);
}

void PublishWaitingDeferredEditorBatches()
{
    if ( !gs_waitingDeferredEditorBatches )
        return;

    EditorDeferredDeleteBatch* batch =
        gs_waitingDeferredEditorBatches;
    gs_waitingDeferredEditorBatches = nullptr;

    // Restore FIFO publication for deterministic destructor order.
    std::vector<EditorDeferredDeleteBatch*> batches;
    while ( batch )
    {
        EditorDeferredDeleteBatch* const next = batch->GetWaitingNext();
        batch->SetWaitingNext(nullptr);
        batches.push_back(batch);
        batch = next;
    }

    for ( auto it = batches.rbegin(); it != batches.rend(); ++it )
        PublishDeferredEditorBatch(*it);
}

// Atomically take ownership of a group of editor windows/forwarders, unlink
// every native editor wrapper from the grid before the first callback-capable
// Win32 call, then park and defer the group as one envelope.
#ifdef __WXMSW__
void DeferPropertyGridEditorObjects(
    wxPropertyGrid* grid,
    const std::vector<wxObject*>& objects)
{
    wxPGDeferredEditorCallbackEpoch callbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(grid);
    EditorDeferredDeleteBatch* const batch =
        new EditorDeferredDeleteBatch;
    const HWND gridHwnd = reinterpret_cast<HWND>(grid->GetHandle());

    std::vector<size_t> adoptedOrder;
    for ( wxObject* const object : objects )
    {
        const size_t index = batch->AdoptObject(object);
        if ( index != EditorDeferredDeleteBatch::InvalidObjectIndex )
            adoptedOrder.push_back(index);
    }

    struct EditorWork
    {
        size_t objectIndex;
        EditorNativeSnapshot* snapshot;
    };
    std::vector<EditorWork> editors;

    // No native call and no user callback is permitted before every object is
    // adopted and every editor is absent from the grid's logical child list.
    for ( const size_t objectIndex : adoptedOrder )
    {
        wxObject* const object = batch->GetLiveObject(objectIndex);
        wxWindow* const editor = wxDynamicCast(object, wxWindow);
        if ( !editor || editor->GetParent() != grid )
            continue;

        EditorNativeSnapshot* const snapshot =
            batch->AddEditorSnapshot(editor, gridHwnd);
        grid->GetChildren().DeleteObject(editor);
        if ( editor->GetParent() == grid )
            editor->SetParent(nullptr);
        if ( snapshot->GetRootHwnd() )
            editors.push_back({objectIndex, snapshot});
    }

    for ( const EditorWork& work : editors )
    {
        if ( !batch->GetLiveObject(work.objectIndex) )
            continue;

        EditorParkingOutcome parking =
            batch->ParkEditor(*work.snapshot);
        if ( parking == EditorParkingOutcome::Failed &&
             !::IsWindow(gridHwnd) &&
             (work.snapshot->MatchesRootIdentity() ||
              work.snapshot->
                  RecoverOrphanedIdentityAfterAncestorDestruction(
                      gridHwnd)) )
        {
            // A focus callback can destroy the grid halfway through the first
            // attempt, leaving a valid hidden editor temporarily parentless.
            // Once that callback has returned, one bounded retry can park this
            // now-stable root without weakening the normal failure seams.
            parking = batch->ParkEditor(*work.snapshot);
        }
        const bool parentIsBeingDestroyed =
            !wxWeakWindowIsAvailableForCallbacks(weakGrid, grid) ||
            !::IsWindow(gridHwnd) ||
            wxWindowIsUnavailableForCallbacks(grid);
        if ( parking == EditorParkingOutcome::Failed &&
             parentIsBeingDestroyed &&
             !work.snapshot->
                  DissociateNativeSubtreeBeforeParentDestruction(gridHwnd) &&
             !work.snapshot->
                  IsAwaitingNativeDestructionAfterAncestor(gridHwnd) )
        {
            const HWND root = work.snapshot->GetRootHwnd();
            wxUnusedVar(root);
            wxFAIL_MSG(wxString::Format(
                wxS("Unsafe property-grid editor HWND identity ")
                wxS("(root=%p live=%d valid=%d match=%d parent=%p ")
                wxS("focus=%p gridAlive=%d assocSelf=%d assocNone=%d ")
                wxS("cookie=%d thread=%d child=%d popup=%d visible=%d)"),
                root,
                work.snapshot->GetLiveEditor() ? 1 : 0,
                work.snapshot->IsValid() ? 1 : 0,
                work.snapshot->MatchesRootIdentity() ? 1 : 0,
                root ? ::GetParent(root) : nullptr,
                ::GetFocus(),
                ::IsWindow(gridHwnd) ? 1 : 0,
                root && wxFindWinFromHandle(root) ==
                            work.snapshot->GetLiveEditor() ? 1 : 0,
                root && !wxFindWinFromHandle(root) ? 1 : 0,
                root && ::GetPropW(root, EDITOR_PARKING_COOKIE) ==
                            reinterpret_cast<HANDLE>(work.snapshot) ? 1 : 0,
                root && IsEditorParkingWindowOnCurrentThread(root) ? 1 : 0,
                root && (::GetWindowLongPtr(root, GWL_STYLE) & WS_CHILD)
                    ? 1 : 0,
                root && (::GetWindowLongPtr(root, GWL_STYLE) & WS_POPUP)
                    ? 1 : 0,
                root && ::IsWindowVisible(root) ? 1 : 0));
            batch->ReleaseObject(work.objectIndex);
            batch->MarkInvariantFailure();
        }
        // When the grid is still alive, keep the failed-to-park HWND
        // associated with its deferred wrapper. The wrapper destructor can
        // then destroy the hidden native subtree safely after the callback
        // epoch. Dissociation is only safe while the native parent itself is
        // being torn down.
    }

    if ( batch->HasWork() )
        QueueDeferredEditorBatch(batch);
    else
        delete batch;
}

#endif // __WXMSW__
#endif // wxPG_USE_EDITOR_CALLBACK_EPOCH

#if WXWIN_COMPATIBILITY_3_0
// Hash containing for every active wxPG the list of editors and their event handlers
// to be deleted in the idle event handler.
// It emulates member variable 'm_deletedEditorObjects' in 3.0 compatibility mode.
WX_DECLARE_HASH_MAP(wxPropertyGrid*, wxArrayPGObject*,
                    wxPointerHash, wxPointerEqual,
                    DeletedObjects);

DeletedObjects gs_deletedEditorObjects;
#endif

// Invalidate the owner before its derived state or any editor is destroyed.
// Unlike detachment, this must also cover forwarders on nested/custom children
// which may outlive the wxPropertyGrid destructor on every platform.
void DisconnectPropertyGridEditorForwarders(wxPropertyGrid* grid);

// Defined after wxPropertyGridEditorEventForwarder itself. Detach and take
// ownership only of forwarders installed by SetupChildEventHandling(). Other
// handlers remain attached until the control or application removes them.
void
DetachPropertyGridEditorForwarders(
    wxWindow* window,
    wxPropertyGrid* grid,
    std::vector<wxEvtHandler*>& forwarders);

// Bit values for wxPropertyGrid::m_coloursCustomized.
enum CustomColour
{
    CustomColour_None           = 0x0000,
    CustomColour_Margin         = 0x0001,
    CustomColour_CaptionBg      = 0x0002,
    CustomColour_CaptionText    = 0x0004,
    CustomColour_CellBg         = 0x0008,
    CustomColour_CellText       = 0x0010,
    CustomColour_SelectionBg    = 0x0020,
    CustomColour_SelectionText  = 0x0040,
    CustomColour_Line           = 0x0080,
    CustomColour_DisabledText   = 0x0100
};

} // anonymous namespace

// -----------------------------------------------------------------------

wxPGPropertyGridTransientState::wxPGPropertyGridTransientState(
    wxPropertyGrid* grid)
    : identity(grid)
{
}

namespace
{

using wxPGPropertyGridTransientStates =
    std::unordered_map<wxPropertyGrid*, wxPGPropertyGridTransientState>;

wxPGPropertyGridTransientStates& wxPGGetPropertyGridTransientStates()
{
    // Deliberately leak this implementation registry: window constructors and
    // destructors may run during static teardown, where destruction order of
    // an ordinary function-local map would otherwise be undefined.
    static wxPGPropertyGridTransientStates* const states =
        new wxPGPropertyGridTransientStates;
    return *states;
}

} // anonymous namespace

wxPGPropertyGridTransientState&
wxPGGetPropertyGridTransientState(wxPropertyGrid* grid)
{
    wxASSERT_MSG(grid, "null PropertyGrid sidecar owner");

    wxPGPropertyGridTransientStates& states =
        wxPGGetPropertyGridTransientStates();
    auto it = states.find(grid);
    if ( it != states.end() && it->second.identity.get() == grid )
        return it->second;

    if ( it != states.end() )
        states.erase(it);
    return states.emplace(grid, wxPGPropertyGridTransientState(grid))
        .first->second;
}

const wxPGPropertyGridTransientState&
wxPGGetPropertyGridTransientState(const wxPropertyGrid* grid)
{
    return wxPGGetPropertyGridTransientState(
        const_cast<wxPropertyGrid*>(grid));
}

void wxPGResetPropertyGridTransientState(wxPropertyGrid* grid)
{
    wxPGPropertyGridTransientStates& states =
        wxPGGetPropertyGridTransientStates();
    states.erase(grid);
    states.emplace(grid, wxPGPropertyGridTransientState(grid));
}

void wxPGErasePropertyGridTransientState(wxPropertyGrid* grid)
{
    wxPGPropertyGridTransientStates& states =
        wxPGGetPropertyGridTransientStates();
    const auto it = states.find(grid);
    if ( it != states.end() && it->second.identity.get() == grid )
        states.erase(it);
}

// -----------------------------------------------------------------------

#if wxPG_USE_EDITOR_CALLBACK_EPOCH
wxPGDeferredEditorCallbackEpoch::wxPGDeferredEditorCallbackEpoch()
{
    ++gs_deferredEditorCallbackDepth;
}

wxPGDeferredEditorCallbackEpoch::~wxPGDeferredEditorCallbackEpoch()
{
    wxASSERT(gs_deferredEditorCallbackDepth != 0);
    if ( --gs_deferredEditorCallbackDepth == 0 )
        PublishWaitingDeferredEditorBatches();
}
#endif

#ifdef __WXMSW__
void wxPGMSWFailNextDirectEditorParkingForTesting()
{
    gs_failNextDirectEditorParkingForTesting = true;
}

void wxPGMSWFailNextFallbackEditorParkingForTesting()
{
    gs_failNextFallbackEditorParkingForTesting = true;
}

void wxPGMSWResetEditorParkingFailuresForTesting()
{
    gs_failNextDirectEditorParkingForTesting = false;
    gs_failNextFallbackEditorParkingForTesting = false;
}

unsigned int wxPGMSWGetEditorParkingHostCountForTesting()
{
    return gs_editorParkingHostCount;
}

unsigned int wxPGMSWGetDeferredEditorBatchCountForTesting()
{
    return gs_deferredEditorBatchCount;
}

bool wxPGMSWIsEditorParkingHostForTesting(WXWidget hwnd)
{
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    return nativeHwnd &&
           ::IsWindow(nativeHwnd) &&
           ::GetPropW(nativeHwnd, EDITOR_PARKING_HOST_COOKIE) != nullptr &&
           IsDirectMessageOnlyWindow(nativeHwnd);
}
#endif

// -----------------------------------------------------------------------

#if wxUSE_INTL
void wxPropertyGrid::AutoGetTranslation ( bool enable )
{
    wxPGGlobalVars->m_autoGetTranslation = enable;
}
#else
void wxPropertyGrid::AutoGetTranslation ( bool ) { }
#endif

// -----------------------------------------------------------------------

const char wxPropertyGridNameStr[] = "wxPropertyGrid";

// -----------------------------------------------------------------------
// Statics in one class for easy destruction.
// -----------------------------------------------------------------------

// This is the real global wxPGGlobalVarsClass object allocated on demand.
static wxPGGlobalVarsClass* g_PGGlobalVars = nullptr;

#include "wx/module.h"

class wxPGGlobalVarsClassManager : public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxPGGlobalVarsClassManager);
public:
    wxPGGlobalVarsClassManager() = default;
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override { wxDELETE(g_PGGlobalVars); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxPGGlobalVarsClassManager, wxModule);


// When wxPG is loaded dynamically after the application is already running
// then the built-in module system won't pick this one up.  Add it manually.
void wxPGInitResourceModule()
{
    wxModule* module = new wxPGGlobalVarsClassManager;
    wxModule::RegisterModule(module);
    wxModule::InitializeModules();
}

// This object is a proxy that allows us to only create wxPGGlobalVarsClass
// object when it's really needed. Note that we need to use an object for this
// instead of a function for compatibility reasons: a lot of existing code uses
// wxPGGlobalVars directly.
wxPGGlobalVarsPtr wxPGGlobalVars;

wxPGGlobalVarsClass* wxPGGlobalVarsPtr::operator->() const
{
    if ( !g_PGGlobalVars )
        g_PGGlobalVars = new wxPGGlobalVarsClass();

    return g_PGGlobalVars;
}

bool wxPGGlobalVarsPtr::operator!() const
{
    return !g_PGGlobalVars;
}

wxPGGlobalVarsPtr::operator bool() const
{
    return !!g_PGGlobalVars;
}

wxPGGlobalVarsClass::wxPGGlobalVarsClass()
    : m_fontFamilyChoices(nullptr)
    , m_defaultRenderer(new wxPGDefaultRenderer())
#if WXWIN_COMPATIBILITY_3_2
    // Prepare some shared variants
    , m_vEmptyString(wxString())
    , m_vZero(0L)
    , m_vMinusOne(-1L)
    , m_vTrue(true)
    , m_vFalse(false)
    // Prepare cached string constants
    , m_strstring(wxS("string"))
    , m_strlong(wxS("long"))
    , m_strbool(wxS("bool"))
    , m_strlist(wxS("list"))
    , m_strDefaultValue(wxS("DefaultValue"))
    , m_strMin(wxS("Min"))
    , m_strMax(wxS("Max"))
    , m_strUnits(wxS("Units"))
    , m_strHint(wxS("Hint"))
#endif // WXWIN_COMPATIBILITY_3_2
    , m_autoGetTranslation(false)
    , m_offline(0)
    , m_extraStyle(0)
    , m_warnings(0)
{
#if WXWIN_COMPATIBILITY_3_2
    wxPGProperty::sm_wxPG_LABEL = new wxString("@!");
#endif // WXWIN_COMPATIBILITY_3_2

    /* TRANSLATORS: Name of Boolean false value */
    m_boolChoices.Add(_("False"));
    /* TRANSLATORS: Name of Boolean true value */
    m_boolChoices.Add(_("True"));
}


wxPGGlobalVarsClass::~wxPGGlobalVarsClass()
{
    delete m_defaultRenderer;

    // This will always have one ref
    delete m_fontFamilyChoices;

#if wxUSE_VALIDATORS
    for ( wxValidator* val : m_arrValidators )
    {
        delete val;
    }
#endif

    // Destroy editor class instances.
    // iterate over all the elements in the class
    for( const auto& vt_it : m_mapEditorClasses )
    {
        delete vt_it.second;
    }

    // Make sure the global pointers have been reset
    wxASSERT(wxPG_EDITOR(TextCtrl) == nullptr);
    wxASSERT(wxPG_EDITOR(ChoiceAndButton) == nullptr);

#if WXWIN_COMPATIBILITY_3_2
    delete wxPGProperty::sm_wxPG_LABEL;
#endif // WXWIN_COMPATIBILITY_3_2
}

void wxPropertyGridInitGlobalsIfNeeded()
{
}

// -----------------------------------------------------------------------
// wxPropertyGrid
// -----------------------------------------------------------------------

#ifndef wxPG_ICON_WIDTH
namespace
{
    // create two bitmap nodes for drawing
    static wxBitmap s_expandbmp(expand_xpm);
    static wxBitmap s_collbmp(collapse_xpm);
}
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertyGrid, wxControl);

wxBEGIN_EVENT_TABLE(wxPropertyGrid, wxControl)
  EVT_IDLE(wxPropertyGrid::OnIdle)
  EVT_PAINT(wxPropertyGrid::OnPaint)
  EVT_SIZE(wxPropertyGrid::OnResize)
  EVT_ENTER_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_LEAVE_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_MOUSE_CAPTURE_CHANGED(wxPropertyGrid::OnCaptureChange)
  EVT_SCROLLWIN(wxPropertyGrid::OnScrollEvent)
  EVT_CHILD_FOCUS(wxPropertyGrid::OnChildFocusEvent)
  EVT_SET_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_KILL_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_SYS_COLOUR_CHANGED(wxPropertyGrid::OnSysColourChanged)
  EVT_DPI_CHANGED(wxPropertyGrid::OnDPIChanged)
  EVT_MOTION(wxPropertyGrid::OnMouseMove)
  EVT_LEFT_DOWN(wxPropertyGrid::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGrid::OnMouseUp)
  EVT_RIGHT_UP(wxPropertyGrid::OnMouseRightClick)
  EVT_LEFT_DCLICK(wxPropertyGrid::OnMouseDoubleClick)
  EVT_KEY_DOWN(wxPropertyGrid::OnKey)
wxEND_EVENT_TABLE()

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid()
    : wxSystemThemedControl<wxScrolled<wxControl>>()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxString& name )
    : wxSystemThemedControl<wxScrolled<wxControl>>()
{
    Init1();
    Create(parent,id,pos,size,style,name);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Create( wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name )
{

    if (!(style&wxBORDER_MASK))
    {
        style |= wxBORDER_THEME;
    }

    style |= wxVSCROLL;

    // Filter out wxTAB_TRAVERSAL - we will handle TABs manually
    style &= ~(wxTAB_TRAVERSAL);
    style |= wxWANTS_CHARS;

    wxControl::Create(parent, id, pos, size,
                      (style & wxWINDOW_STYLE_MASK) | wxScrolledWindowStyle,
                      wxDefaultValidator,
                      name);

    EnableSystemThemeByDefault();

    m_windowStyle |= (style & wxPG_WINDOW_STYLE_MASK);

    Init2();

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::EditorsValueWasModified()
{
    ++wxPGGetPropertyGridTransientState(this).editorModificationRevision;
    m_iFlags |= wxPG_FL_VALUE_MODIFIED;
}

void wxPropertyGrid::EditorsValueWasNotModified()
{
    ++wxPGGetPropertyGridTransientState(this).editorModificationRevision;
    m_iFlags &= ~wxPG_FL_VALUE_MODIFIED;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetVerticalSpacing( int vspacing )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    m_vspacing = static_cast<unsigned char>(vspacing);
    CalculateFontAndBitmapStuff(vspacing);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state || !state )
    {
        return;
    }

    if ( !state->m_itemsAdded )
        Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetLayoutDirection( wxLayoutDirection dir )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveLayoutDirection = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveLayoutDirection);

    if ( !m_pState )
    {
        wxControl::SetLayoutDirection(dir);
        return;
    }
    wxPropertyGridPageState* const layoutState = m_pState;

    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != layoutState )
            return;
    }

    if ( m_labelEditor )
    {
        DoEndLabelEdit(true, wxPGSelectPropertyFlags::NoValidate);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != layoutState )
            return;
    }

    if ( wxOwnerDrawnComboBox* const combo =
            wxDynamicCast(GetEditorControl(), wxOwnerDrawnComboBox) )
    {
        combo->HidePopup();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != layoutState )
            return;
    }

    wxControl::SetLayoutDirection(dir);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != layoutState )
        return;

    m_propHover = nullptr;
    m_colHover = -1;
    CustomSetCursor(wxCURSOR_ARROW, true);

    if ( GetSelection() )
    {
        CorrectEditorWidgetSizeX();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != layoutState )
        {
            return;
        }
    }

    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != layoutState )
    {
        return;
    }
    Refresh();
}

// -----------------------------------------------------------------------

//
// Initialize values to defaults
//
void wxPropertyGrid::Init1()
{
    wxPGResetPropertyGridTransientState(this);

    // Register editor classes, if necessary.
    if ( wxPGGlobalVars->m_mapEditorClasses.empty() )
        wxPropertyGrid::RegisterDefaultEditors();

    m_validatingEditor = 0;
    m_iFlags = 0;
    m_pState = nullptr;
    m_wndEditor = m_wndEditor2 = nullptr;
    m_selColumn = 1;
    m_colHover = 1;
    m_propHover = nullptr;
    m_labelEditor = nullptr;
    m_labelEditorProperty = nullptr;
    m_eventObject = this;
    m_curFocused = nullptr;
    m_processedEvent = nullptr;
    m_tlp = nullptr;
    m_sortFunction = nullptr;
    m_inDoPropertyChanged = false;
    m_inCommitChangesFromEditor = false;
    m_inDoSelectProperty = false;
    m_inOnValidationFailure = false;
    m_permanentValidationFailureBehavior = wxPGVFBFlags::Default;
    m_dragStatus = 0;
    m_dragOffset = 0;
    wxPGGetPropertyGridTransientState(this).dragOffset = 0;
    m_draggedSplitter = -1;
    m_startingSplitterX = 0;
    m_editorFocused = false;

    // Set up default unspecified value 'colour'
    m_unspecifiedAppearance.SetFgCol(*wxLIGHT_GREY);

    // Set default keys
    AddActionTrigger(wxPGKeyboardAction::NextProperty, WXK_RIGHT);
    AddActionTrigger(wxPGKeyboardAction::NextProperty, WXK_DOWN);
    AddActionTrigger(wxPGKeyboardAction::PrevProperty, WXK_LEFT);
    AddActionTrigger(wxPGKeyboardAction::PrevProperty, WXK_UP);
    AddActionTrigger(wxPGKeyboardAction::ExpandProperty, WXK_RIGHT);
    AddActionTrigger(wxPGKeyboardAction::CollapseProperty, WXK_LEFT);
    AddActionTrigger(wxPGKeyboardAction::CancelEdit, WXK_ESCAPE);
    AddActionTrigger(wxPGKeyboardAction::PressButton, WXK_DOWN, wxMOD_ALT);
    AddActionTrigger(wxPGKeyboardAction::PressButton, WXK_F4);

    m_coloursCustomized = CustomColour_None;

    m_doubleBuffer = nullptr;

    m_lineHeight = 0;

    m_width = m_height = 0;

    /* TRANSLATORS: Text  displayed for unspecified value */
    m_commonValues.push_back(new wxPGCommonValue(_("Unspecified"), wxPGGlobalVars->m_defaultRenderer) );
    m_cvUnspecified = 0;

    m_chgInfo_changedProperty = nullptr;
#if WXWIN_COMPATIBILITY_3_0
    // Object array for this wxPG shouldn't exist in the hash map.
    wxASSERT( gs_deletedEditorObjects.find(this) == gs_deletedEditorObjects.end() );
    gs_deletedEditorObjects[this] = new wxArrayPGObject;
#endif
}

// -----------------------------------------------------------------------

//
// Initialize after parent etc. set
//
void wxPropertyGrid::Init2()
{
    wxASSERT( !(m_iFlags & wxPG_FL_INITIALIZED ) );

#ifdef __WXMAC__
   // Smaller controls on Mac
   SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    // Now create state, if one didn't exist already
    // (wxPropertyGridManager might have created it for us).
    if ( !m_pState )
    {
        m_pState = CreateState();
        m_pState->m_pPropGrid = this;
        m_iFlags |= wxPG_FL_CREATEDSTATE;
    }

    if ( !(m_windowStyle & wxPG_SPLITTER_AUTO_CENTER) )
        m_pState->m_dontCenterSplitter = true;

    if ( m_windowStyle & wxPG_HIDE_CATEGORIES )
    {
        m_pState->InitNonCatMode();

        m_pState->m_properties = m_pState->m_abcArray;
    }

    GetClientSize(&m_width,&m_height);

#ifndef wxPG_ICON_WIDTH
    // calculate average font height for bitmap centering

    m_iconWidth = s_expandbmp.GetWidth();
    m_iconHeight = s_expandbmp.GetHeight();
#else
    m_iconWidth = FromDIP(wxPG_ICON_WIDTH);
    m_iconHeight = FromDIP(wxPG_ICON_WIDTH);
#endif

    m_gutterWidth = wxMax(0, (FromDIP(16) - m_iconWidth) / 2);
    m_subgroup_extramargin = m_iconWidth + m_gutterWidth;

    m_curcursor = wxCURSOR_ARROW;
    m_cursorSizeWE = wxCursor(wxCURSOR_SIZEWE);

    // adjust bitmap icon y position so they are centered
    m_vspacing = wxPG_DEFAULT_VSPACING;

    CalculateFontAndBitmapStuff( m_vspacing );

    // Allocate cell data
    m_propertyDefaultCell.SetEmptyData();
    m_categoryDefaultCell.SetEmptyData();

    RegainColours();

    // This helps with flicker
    SetBackgroundStyle( wxBG_STYLE_PAINT );

    // Rely on native double-buffering by default.
#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    SetExtraStyle(GetExtraStyle() | wxPG_EX_NATIVE_DOUBLE_BUFFERING);
#endif // wxALWAYS_NATIVE_DOUBLE_BUFFER

    // Hook the top-level parent
    m_tlpClosed = nullptr;
    m_tlpClosedTime = 0;

    // set virtual size to this window size
    wxSize clientSize = GetClientSize();
    SetVirtualSize(clientSize);

    m_iFlags |= wxPG_FL_INITIALIZED;

    wxSize wndsize = GetSize();
    m_ncWidth = wndsize.GetWidth();

    // Need to call OnResize handler or size given in constructor/Create
    // will never work.
    wxSizeEvent sizeEvent(wndsize,0);
    OnResize(sizeEvent);
}

// -----------------------------------------------------------------------

wxPropertyGrid::~wxPropertyGrid()
{
    DisconnectPropertyGridEditorForwarders(this);
    SendDestroyEvent();

#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif

    // Keep this information separately: the live event must be detached below
    // before any editor cleanup, but its presence still determines whether
    // deleting the editor objects synchronously is safe.
    const bool wasProcessingEvent = m_processedEvent != nullptr;
    const bool wasHandlingCustomEditorEvent =
        HasInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);
    const bool wasHandlingPropertyCallback =
        wxPGGetPropertyGridTransientState(this).propertyCallbackDepth != 0;
    const bool wasSelectingProperty = m_inDoSelectProperty;
    const bool wasEndingLabelEdit = wxPGGetPropertyGridTransientState(this).endingLabelEdit;
    const bool deferEditorDeletion =
        wasProcessingEvent ||
        wasHandlingCustomEditorEvent ||
        wasHandlingPropertyCallback ||
        wasSelectingProperty ||
        wasEndingLabelEdit;

    //
    // Remove grid and property pointers from live wxPropertyGridEvents.
    while ( !m_liveEvents.empty() )
    {
        wxPropertyGridEvent* const evt = m_liveEvents.back();
        evt->SetPropertyGrid(nullptr);
        evt->SetProperty(nullptr);
    }

    if ( wasProcessingEvent )
    {
        // A handler may synchronously destroy the grid. Detach the live event
        // before the base classes are torn down; SendEvent() and its callers
        // use weak identities and won't touch the grid after this point.
        m_processedEvent->Skip(false);
        m_processedEvent->StopPropagation();
        m_processedEvent = nullptr;
    }

#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    auto& deferredEditorObjects = m_deletedEditorObjects;

    // Do not call DoSelectProperty() during destruction: SendDestroyEvent()
    // has already made the grid unavailable for callbacks, so selection
    // guards would return before detaching the editors' pushed handlers.
    // Collect every editor callback-free before any later destructor
    // operation can dispatch a native message or enter a nested idle loop.
    const auto collectEditor =
        [this, &deferredEditorObjects](wxWindow*& member)
        {
            wxWindow* const editor = member;
            if ( !editor )
                return;

            member = nullptr;
            std::vector<wxEvtHandler*> forwarders;
            DetachPropertyGridEditorForwarders(
                editor, this, forwarders);
            for ( wxEvtHandler* const forwarder : forwarders )
                deferredEditorObjects.push_back(forwarder);
            deferredEditorObjects.push_back(editor);
        };

    wxWindow* const secondaryBefore = m_wndEditor2;
    collectEditor(m_wndEditor2);
    if ( m_wndEditor == secondaryBefore )
        m_wndEditor = nullptr;
    collectEditor(m_wndEditor);

    if ( m_labelEditor )
    {
        wxWindow* labelEditor = m_labelEditor;
        m_labelEditor = nullptr;
        m_labelEditorProperty = nullptr;
        m_selColumn = 1;
        deferredEditorObjects.push_back(labelEditor);
    }

#ifdef __WXMSW__
    // A custom CreateControls() callback may destroy the grid before its
    // wxPGWindowList is returned and before either editor member is set.
    std::vector<wxWindow*> inFlightCreatedChildren;
    EditorCreationTransaction::CollectNewChildren(
        this, inFlightCreatedChildren);
    for ( wxWindow* const child : inFlightCreatedChildren )
    {
        std::vector<wxEvtHandler*> forwarders;
        DetachPropertyGridEditorForwarders(
            child, this, forwarders);
        for ( wxEvtHandler* const forwarder : forwarders )
            deferredEditorObjects.push_back(forwarder);
        deferredEditorObjects.push_back(child);
    }
#endif

#if wxPG_USE_EDITOR_CALLBACK_EPOCH
    EditorDeferredDeleteBatch* deferredBatch =
        new EditorDeferredDeleteBatch;
#ifdef __WXMSW__
    const HWND gridHwnd = reinterpret_cast<HWND>(GetHandle());
#endif
    std::vector<size_t> adoptedOrder;
    while ( !m_deletedEditorObjects.empty() )
    {
        wxObject* obj = m_deletedEditorObjects.back();
        m_deletedEditorObjects.pop_back();
        const size_t index = deferredBatch->AdoptObject(obj);
        if ( index != EditorDeferredDeleteBatch::InvalidObjectIndex )
            adoptedOrder.push_back(index);
    }
#endif

    if ( deferEditorDeletion )
    {
        // We are inside application/editor code and cannot delete anything
        // that can still be present on its stack. The envelope is published
        // to the global idle queue only after the outermost callback epoch.
        // On MSW it also owns the native parking hosts.
#if wxPG_USE_EDITOR_CALLBACK_EPOCH
#ifdef __WXMSW__
        struct DestructorEditorWork
        {
            size_t objectIndex;
            EditorNativeSnapshot* snapshot;
        };
        std::vector<DestructorEditorWork> editorsToPark;
#else
        std::vector<size_t> editorsToDetach;
#endif

        // Phase A: remove every editor wrapper from the grid's logical child
        // ownership before parking any one of them can dispatch a callback.
        for ( const size_t objectIndex : adoptedOrder )
        {
            wxObject* const obj =
                deferredBatch->GetLiveObject(objectIndex);
            wxWindow* const editor = wxDynamicCast(obj, wxWindow);
            if ( !editor || editor->GetParent() != this )
                continue;

#ifdef __WXMSW__
            EditorNativeSnapshot* const nativeSnapshot =
                deferredBatch->AddEditorSnapshot(editor, gridHwnd);
#endif
            GetChildren().DeleteObject(editor);
            if ( editor->GetParent() == this )
                editor->SetParent(nullptr);
#ifdef __WXMSW__
            if ( nativeSnapshot->GetRootHwnd() )
            {
                editorsToPark.push_back(
                    {objectIndex, nativeSnapshot});
            }
#else
            editorsToDetach.push_back(objectIndex);
#endif
        }

        // Phase B: all later editor roots are now protected from a reentrant
        // wxWindowBase destructor while each native parking transaction runs.
#ifdef __WXMSW__
        for ( const DestructorEditorWork& work : editorsToPark )
        {
            if ( !deferredBatch->GetLiveObject(work.objectIndex) )
                continue;

            const EditorParkingOutcome parking =
                deferredBatch->ParkEditor(*work.snapshot);
            if ( parking == EditorParkingOutcome::Failed &&
                 !work.snapshot->
                     DissociateNativeSubtreeBeforeParentDestruction(
                         gridHwnd) )
            {
                const HWND root = work.snapshot->GetRootHwnd();
                wxUnusedVar(root);
                wxFAIL_MSG(wxString::Format(
                    wxS("Unsafe property-grid editor HWND identity ")
                    wxS("(root=%p live=%d valid=%d match=%d parent=%p ")
                    wxS("focus=%p gridAlive=%d assocSelf=%d assocNone=%d ")
                    wxS("cookie=%d thread=%d child=%d popup=%d visible=%d)"),
                    root,
                    work.snapshot->GetLiveEditor() ? 1 : 0,
                    work.snapshot->IsValid() ? 1 : 0,
                    work.snapshot->MatchesRootIdentity() ? 1 : 0,
                    root ? ::GetParent(root) : nullptr,
                    ::GetFocus(),
                    ::IsWindow(gridHwnd) ? 1 : 0,
                    root && wxFindWinFromHandle(root) ==
                                work.snapshot->GetLiveEditor() ? 1 : 0,
                    root && !wxFindWinFromHandle(root) ? 1 : 0,
                    root && ::GetPropW(root, EDITOR_PARKING_COOKIE) ==
                                reinterpret_cast<HANDLE>(work.snapshot) ? 1 : 0,
                    root && IsEditorParkingWindowOnCurrentThread(root) ? 1 : 0,
                    root && (::GetWindowLongPtr(root, GWL_STYLE) & WS_CHILD)
                        ? 1 : 0,
                    root && (::GetWindowLongPtr(root, GWL_STYLE) & WS_POPUP)
                        ? 1 : 0,
                    root && ::IsWindowVisible(root) ? 1 : 0));
                deferredBatch->ReleaseObject(work.objectIndex);
                deferredBatch->MarkInvariantFailure();
            }
        }
#else
        for ( const size_t objectIndex : editorsToDetach )
        {
            wxWindow* const editor = wxDynamicCast(
                deferredBatch->GetLiveObject(objectIndex), wxWindow);
            if ( !editor || editor->GetParent() )
                continue;

            // Detach the native ownership too: removing only the wx child
            // would let destruction of the native parent retire its widget
            // while the editor's size/focus callback is still on the stack.
            // The Cocoa peer owns its NSView and wxGTK owns a widget ref, so
            // both controls survive removal from their native container.
#ifdef __WXOSX_COCOA__
            if ( editor->GetPeer() )
                editor->GetPeer()->RemoveFromParent();
#elif defined(__WXGTK__)
            if ( GtkWidget* const widget = editor->GetHandle() )
            {
                if ( GtkWidget* const parent = gtk_widget_get_parent(widget) )
                {
                    // A removal callback can destroy the wx wrapper. Keep
                    // the widget alive until the native call itself returns;
                    // the batch's weak slot then prevents a second deletion.
                    g_object_ref(widget);
                    gtk_container_remove(GTK_CONTAINER(parent), widget);
                    g_object_unref(widget);
                }
            }
#elif defined(__WXQT__)
            // GetHandle() is the owned root QWidget, including for scroll
            // areas: it is not their borrowed viewport. Preserve its flags
            // using the port's reparenting helper. Qt hides it and releases
            // the parent's ownership; the deferred wx wrapper still owns it.
            const QPointer<QWidget> widget(editor->GetHandle());
            if ( widget && widget->parentWidget() )
                wxWindow::QtReparent(widget.data(), nullptr);
#endif
        }
#endif

        if ( deferredBatch->HasWork() )
            QueueDeferredEditorBatch(deferredBatch);
        else
            delete deferredBatch;
        deferredBatch = nullptr;
#else
        while ( !m_deletedEditorObjects.empty() )
        {
            wxObject* obj = m_deletedEditorObjects.back();
            m_deletedEditorObjects.pop_back();
            wxPendingDelete.Append(obj);
        }
#endif
    }
    else
    {
#if !wxPG_USE_EDITOR_CALLBACK_EPOCH
        // Delete pending editor controls
        DeletePendingObjects();
#endif
    }

    // This should prevent things from going too badly wrong. Editor ownership
    // has already been resolved above, before these callback-capable native
    // cleanup operations.
    m_iFlags &= ~(wxPG_FL_INITIALIZED);

    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
    {
        m_dragStatus = 0;
        wxPGGetPropertyGridTransientState(this).draggedState = nullptr;
        m_iFlags &= ~wxPG_FL_MOUSE_CAPTURED;
        if ( HasCapture() )
            ReleaseMouse();
    }

    // Call with nullptr to disconnect event handling.
    if ( HasExtraStyle(wxPG_EX_ENABLE_TLP_TRACKING) )
    {
        OnTLPChanging(nullptr);

        wxASSERT_MSG( !IsEditorsValueModified(),
                      wxS("Most recent change in property editor was ")
                      wxS("lost!!! (if you don't want this to happen, ")
                      wxS("close your frames and dialogs using ")
                      wxS("Close(false).)") );
    }

#if wxPG_USE_EDITOR_CALLBACK_EPOCH
    // On the ordinary path, editor objects were atomically adopted before any
    // callback-capable teardown, but are destroyed only after the grid has
    // been made inert and disconnected from its top-level tracking.
    if ( deferredBatch )
        delete deferredBatch;
#endif

    if ( m_doubleBuffer )
        delete m_doubleBuffer;

    if ( m_iFlags & wxPG_FL_CREATEDSTATE )
        delete m_pState;

    // Delete common value records
    for ( wxPGCommonValue* v : m_commonValues )
    {
        delete v;
    }
#if WXWIN_COMPATIBILITY_3_0
    wxASSERT( gs_deletedEditorObjects[this]->empty() );

    delete gs_deletedEditorObjects[this];
    gs_deletedEditorObjects.erase(this);
#endif
    wxPGErasePropertyGridTransientState(this);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Destroy()
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return true;
    }

    return wxControl::Destroy();
}

// -----------------------------------------------------------------------

wxPropertyGridPageState* wxPropertyGrid::CreateState() const
{
    return new wxPropertyGridPageState();
}

// -----------------------------------------------------------------------
// wxPropertyGrid overridden wxWindow methods
// -----------------------------------------------------------------------

void wxPropertyGrid::SetWindowStyleFlag( long style )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    long old_style = m_windowStyle;

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxASSERT( m_pState );

        if ( !(style & wxPG_HIDE_CATEGORIES) && (old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Enable categories
            EnableCategories( true );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
        }
        else if ( (style & wxPG_HIDE_CATEGORIES) && !(old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Disable categories
            EnableCategories( false );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
        }
        if ( !(old_style & wxPG_AUTO_SORT) && (style & wxPG_AUTO_SORT) )
        {
            //
            // Auto sort enabled
            //
            if ( !IsFrozen() )
            {
                PrepareAfterItemsAdded();
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                     m_pState != state )
                {
                    return;
                }
            }
            else
                m_pState->m_itemsAdded = true;
        }
#if wxUSE_TOOLTIPS
        if ( !(old_style & wxPG_TOOLTIPS) && (style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips enabled
            //
            //wxToolTip::SetDelay(wxPG_TOOLTIP_DELAY);
        }
        else if ( (old_style & wxPG_TOOLTIPS) && !(style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips disabled
            //
            UnsetToolTip();
        }
#endif // wxUSE_TOOLTIPS
    }

    wxControl::SetWindowStyleFlag ( style );
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        if ( (old_style & wxPG_HIDE_MARGIN) != (style & wxPG_HIDE_MARGIN) )
        {
            CalculateFontAndBitmapStuff( m_vspacing );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
            Refresh();
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoThaw()
{
    if ( !IsFrozen() )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const state = m_pState;
        wxControl::DoThaw();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state || !state )
        {
            return;
        }
        RecalculateVirtualSize();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
        Refresh();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }

        // Force property re-selection
        // NB: We must copy the selection.
        wxArrayPGProperty selection = state->m_selection;
        DoSetSelection(selection, wxPGSelectPropertyFlags::Force | wxPGSelectPropertyFlags::Nonvisible);
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoAddToSelection( wxPGProperty* prop, wxPGSelectPropertyFlags selFlags )
{
    wxCHECK( prop, false );
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state || wxWindowIsUnavailableForCallbacks(this) || prop->GetParentState() != state ||
         IsPropertyPendingRemoval(prop) )
    {
        return false;
    }

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectionCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSelectionCallback);

    const auto transactionIsValid =
        [weakThis, this, state, prop]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   prop->GetParentState() == state &&
                   !IsPropertyPendingRemoval(prop);
        };

    if ( !HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) )
        return DoSelectProperty(prop, selFlags);

    wxArrayPGProperty& selection = state->m_selection;

    if ( selection.empty() )
    {
        return DoSelectProperty(prop, selFlags);
    }
    else
    {
        // For categories, only one can be selected at a time
        if ( prop->IsCategory() || selection[0]->IsCategory() )
            return true;

        selection.push_back(prop);

        if ( !(selFlags & wxPGSelectPropertyFlags::DontSendEvent) )
        {
            SendEvent( wxEVT_PG_SELECTED, prop, nullptr );
            if ( !transactionIsValid() ||
                 !state->DoIsPropertySelected(prop) )
            {
                return false;
            }
        }

        DrawItem(prop);
        if ( !transactionIsValid() ||
             !state->DoIsPropertySelected(prop) )
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoRemoveFromSelection( wxPGProperty* prop, wxPGSelectPropertyFlags selFlags )
{
    wxCHECK( prop, false );
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    // Deferred deletion must still be able to detach the property from the
    // selection when it is retried at idle.
    if ( !state || wxWindowIsUnavailableForCallbacks(this) || prop->GetParentState() != state ||
         (IsPropertyPendingRemoval(prop) &&
          !(selFlags & wxPGSelectPropertyFlags::Deleting)) )
    {
        return false;
    }

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectionCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSelectionCallback);

    bool res;

    wxArrayPGProperty& selection = state->m_selection;
    if ( selection.size() <= 1 )
    {
        res = DoSelectProperty(nullptr, selFlags);
    }
    else
    {
        state->DoRemoveFromSelection(prop);
        DrawItem(prop);
        res = wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
              m_pState == state &&
              prop->GetParentState() == state &&
              !IsPropertyPendingRemoval(prop) &&
              !state->DoIsPropertySelected(prop);
    }

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoSelectAndEdit( wxPGProperty* prop,
                                      unsigned int colIndex,
                                      wxPGSelectPropertyFlags selFlags )
{
    wxCHECK( prop, false );
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state || wxWindowIsUnavailableForCallbacks(this) || prop->GetParentState() != state ||
         IsPropertyPendingRemoval(prop) )
    {
        return false;
    }

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectionCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSelectionCallback);

    const auto transactionIsValid =
        [weakThis, this, state, prop]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   prop->GetParentState() == state &&
                   !IsPropertyPendingRemoval(prop);
        };

    //
    // NB: Enable following if label editor background colour is
    //     ever changed to any other than m_colSelBack.
    //
    // We use this workaround to prevent visible flicker when editing
    // a cell. At least on wxMSW, there is a difficult to find
    // (and perhaps prevent) redraw somewhere between making property
    // selected and enabling label editing.
    //
    //wxColour prevColSelBack = m_colSelBack;
    //m_colSelBack = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );

    bool res;

    if ( colIndex == 1 )
    {
        res = DoSelectProperty(prop, selFlags);
        if ( !res || !transactionIsValid() || GetSelection() != prop )
            return false;
    }
    else
    {
        // send event
        if ( !DoClearSelection(false,
                               wxPGSelectPropertyFlags::NoRefresh) ||
             !transactionIsValid() )
        {
            return false;
        }

        if ( state->m_editableColumns.find(colIndex) ==
             state->m_editableColumns.end() )
        {
            res = DoAddToSelection(prop, selFlags);
        }
        else
        {
            res = DoAddToSelection(prop, selFlags| wxPGSelectPropertyFlags::NoRefresh);

            if ( res && transactionIsValid() && GetSelection() == prop )
                DoBeginLabelEdit(colIndex, selFlags);
        }

        if ( !res || !transactionIsValid() || GetSelection() != prop )
            return false;
    }

    //m_colSelBack = prevColSelBack;
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::AddToSelectionFromInputEvent( wxPGProperty* prop,
                                                   unsigned int colIndex,
                                                   wxMouseEvent* mouseEvent,
                                                   wxPGSelectPropertyFlags selFlags )
{
    wxCHECK( prop, false );
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state || wxWindowIsUnavailableForCallbacks(this) || prop->GetParentState() != state ||
         IsPropertyPendingRemoval(prop) )
    {
        return false;
    }

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectionCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSelectionCallback);

    const auto transactionIsValid =
        [weakThis, this, state, prop]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   prop->GetParentState() == state &&
                   !IsPropertyPendingRemoval(prop);
        };

    // Event callbacks below can mutate the live selection array.
    const wxArrayPGProperty selection = GetSelectedProperties();
    bool alreadySelected = state->DoIsPropertySelected(prop);
    bool res = true;

    // Set to 2 if also add all items in between
    int addToExistingSelection = 0;

    if ( HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) )
    {
        if ( mouseEvent )
        {
            if ( mouseEvent->GetEventType() == wxEVT_RIGHT_DOWN ||
                 mouseEvent->GetEventType() == wxEVT_RIGHT_UP )
            {
                // Allow right-click for context menu without
                // disturbing the selection.
                if ( selection.size() <= 1 ||
                     !alreadySelected )
                    return DoSelectAndEdit(prop, colIndex, selFlags);
                return true;
            }
            else
            {
                if ( mouseEvent->ControlDown() )
                {
                    addToExistingSelection = 1;
                }
                else if ( mouseEvent->ShiftDown() )
                {
                    if ( !selection.empty() && !prop->IsCategory() )
                        addToExistingSelection = 2;
                    else
                        addToExistingSelection = 1;
                }
            }
        }
    }

    if ( addToExistingSelection == 1 )
    {
        // Add/remove one
        if ( !alreadySelected )
        {
            res = DoAddToSelection(prop, selFlags);
        }
        else if ( selection.size() > 1 )
        {
            res = DoRemoveFromSelection(prop, selFlags);
        }
    }
    else if ( addToExistingSelection == 2 )
    {
        // Add this, and all in between

        // Find top selected property
        wxPGProperty* topSelProp = selection[0];
        int topSelPropY = topSelProp->GetY();
        for ( unsigned int i=1; i<selection.size(); i++ )
        {
            wxPGProperty* p = selection[i];
            int y = p->GetY();
            if ( y < topSelPropY )
            {
                topSelProp = p;
                topSelPropY = y;
            }
        }

        wxPGProperty* startFrom;
        wxPGProperty* stopAt;

        if ( prop->GetY() <= topSelPropY )
        {
            // Property is above selection (or same)
            startFrom = prop;
            stopAt = topSelProp;
        }
        else
        {
            // Property is below selection
            startFrom = topSelProp;
            stopAt = prop;
        }

        // Snapshot the range before sending any selection event. A handler can
        // rebuild the page tree and invalidate a live iterator.
        wxArrayPGProperty propertiesToAdd;
        for ( wxPropertyGridIterator it = GetIterator(wxPG_ITERATE_VISIBLE, startFrom);
              !it.AtEnd();
              ++it )
        {
            wxPGProperty* p = *it;

            if ( !p->IsCategory() &&
                 !state->DoIsPropertySelected(p) )
            {
                propertiesToAdd.push_back(p);
            }

            if ( p == stopAt )
                break;
        }

        for ( wxPGProperty* const p : propertiesToAdd )
        {
            if ( !transactionIsValid() ||
                 p->GetParentState() != state ||
                 IsPropertyPendingRemoval(p) ||
                 !DoAddToSelection(p, selFlags) )
            {
                return false;
            }
        }
    }
    else
    {
        res = DoSelectAndEdit(prop, colIndex, selFlags);
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoSetSelection( const wxArrayPGProperty& newSelection,
                                     wxPGSelectPropertyFlags selFlags )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state || wxWindowIsUnavailableForCallbacks(this) )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectionCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSelectionCallback);

    // This argument is allowed to alias the current selection.
    const wxArrayPGProperty selection = newSelection;
    const auto transactionIsValid =
        [weakThis, this, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state;
        };

    if ( !selection.empty() )
    {
        wxPGProperty* const first = selection[0];
        if ( first->GetParentState() != state ||
             IsPropertyPendingRemoval(first) ||
             !DoSelectProperty(first, selFlags) ||
             !transactionIsValid() )
        {
            return;
        }
    }
    else
    {
        if ( !DoClearSelection(false, selFlags) || !transactionIsValid() )
            return;
    }

    for ( unsigned int i = 1; i < selection.size(); i++ )
    {
        wxPGProperty* const property = selection[i];
        if ( property->GetParentState() != state ||
             IsPropertyPendingRemoval(property) ||
             !DoAddToSelection(property, selFlags) ||
             !transactionIsValid() )
        {
            return;
        }
    }

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::MakeColumnEditable( unsigned int column,
                                         bool editable )
{
    // The second column is always editable. To make it read-only is a property
    // by property decision by setting its wxPGFlags::ReadOnly flag.
    wxASSERT_MSG
    (
         column != 1,
         wxS("Set wxPGFlags::ReadOnly property flag instead")
    );

    if ( editable )
    {
        m_pState->m_editableColumns.insert(column);
    }
    else
    {
        m_pState->m_editableColumns.erase(column);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoBeginLabelEdit( unsigned int colIndex,
                                       wxPGSelectPropertyFlags selFlags )
{
    if ( wxPGGetPropertyGridTransientState(this).beginLabelEditGuard || wxPGGetPropertyGridTransientState(this).endingLabelEdit )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPGProperty* selected = GetSelection();
    wxCHECK_RET(selected, wxS("No property selected"));
    wxCHECK_RET(colIndex != 1, wxS("Do not use this for column 1"));
    wxPropertyGridPageState* const initialState = m_pState;
    if ( !initialState || selected->GetParentState() != initialState ||
         IsPropertyPendingRemoval(selected) ||
         selected->HasFlag(wxPGFlags::BeingDeleted) )
    {
        return;
    }

    const wxWeakRef<wxWindow> weakThis(this);
    const auto propertyTransactionIsValid =
        [weakThis, this, initialState, selected]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == initialState &&
                   GetSelection() == selected &&
                   selected->GetParentState() == initialState &&
                   !IsPropertyPendingRemoval(selected) &&
                   !selected->HasFlag(wxPGFlags::BeingDeleted);
        };
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveCallback);

    wxPGGetPropertyGridTransientState(this).beginLabelEditGuard = 1;
    wxScopeGuard leaveBeginLabelEdit = wxMakeGuard([weakThis, this]()
    {
        // A BEGIN handler is allowed to destroy the grid synchronously.
        if ( weakThis.get() == this )
            wxPGGetPropertyGridTransientState(this).beginLabelEditGuard = 0;
    });
    wxUnusedVar(leaveBeginLabelEdit);

    if ( !(selFlags & wxPGSelectPropertyFlags::DontSendEvent) )
    {
        const bool vetoed = SendEvent( wxEVT_PG_LABEL_EDIT_BEGIN,
                                       selected, nullptr,
                                       wxPGSelectPropertyFlags::Null,
                                       colIndex );
        if ( vetoed || !propertyTransactionIsValid() )
            return;
    }

    wxString text;
    const wxPGCell* cell = nullptr;
    if ( selected->HasCell(colIndex) )
    {
        cell = &selected->GetCell(colIndex);
        if ( !cell->HasText() && colIndex == 0 )
            text = selected->GetLabel();
    }

    if ( !cell  )
    {
        if ( colIndex == 0 )
            text = selected->GetLabel();
        else
            cell = &selected->GetOrCreateCell(colIndex);
    }

    if ( cell && cell->HasText() )
        text = cell->GetText();

    DoEndLabelEdit(true, wxPGSelectPropertyFlags::NoValidate);  // send event
    if ( !propertyTransactionIsValid() || m_labelEditor )
        return;

    m_selColumn = colIndex;

    wxRect r;
    if ( !TryGetEditorWidgetRect(selected, m_selColumn, &r) )
        return;

#ifdef __WXMSW__
    EditorCreationTransaction labelCreationTransaction(this);
#endif
    wxWindow* tc = GenerateEditorTextCtrl(r.GetPosition(),
                                          r.GetSize(),
                                          text,
                                          nullptr,
                                          wxTE_PROCESS_ENTER,
                                          0,
                                          colIndex);
    if ( !propertyTransactionIsValid() )
        return;
    wxCHECK_RET( tc, wxS("Failed to create the property label editor") );
    const wxWeakRef<wxWindow> weakEditor(tc);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakEditor, tc) ||
         tc->GetParent() != this )
        return;

    tc->Bind(wxEVT_TEXT_ENTER, &wxPropertyGrid::OnLabelEditorEnterPress, this);
    tc->Bind(wxEVT_KEY_DOWN, &wxPropertyGrid::OnLabelEditorKeyPress, this);

    m_labelEditor = wxStaticCast(tc, wxTextCtrl);
    // Get actual position within required rectangle
    m_labelEditorPosRel = m_labelEditor->GetPosition() - r.GetPosition();
    m_labelEditorProperty = selected;

    tc->SetFocus();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;
    if ( !wxWeakWindowIsAvailableForCallbacks(weakEditor, tc) )
    {
        if ( m_labelEditor == tc )
        {
            m_labelEditor = nullptr;
            m_labelEditorProperty = nullptr;
            m_selColumn = 1;
        }
        return;
    }
    if ( m_labelEditor != tc || m_labelEditorProperty != selected )
        return;
    if ( !propertyTransactionIsValid() )
    {
        m_labelEditor = nullptr;
        m_labelEditorProperty = nullptr;
        m_selColumn = 1;
        DestroyEditorWnd(tc);
    }
}

// -----------------------------------------------------------------------

void
wxPropertyGrid::OnLabelEditorEnterPress( wxCommandEvent& WXUNUSED(event) )
{
    DoEndLabelEdit(true);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnLabelEditorKeyPress( wxKeyEvent& event )
{
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_ESCAPE )
    {
        DoEndLabelEdit(false);
    }
    else
    {
        HandleKeyEvent(event, true);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoEndLabelEdit( bool commit, wxPGSelectPropertyFlags selFlags )
{
    if ( !m_labelEditor )
        return;

    if ( commit && wxPGGetPropertyGridTransientState(this).endingLabelEdit )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxTextCtrl* const editor = m_labelEditor;
    wxPGProperty* prop = m_labelEditorProperty;
    wxASSERT(prop);
    const unsigned int labelColIdx = m_selColumn;
    wxPropertyGridPageState* const initialState = m_pState;
    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakEditor(editor);
    const auto propertyTransactionIsValid =
        [weakThis, this, initialState, prop]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == initialState &&
                   GetSelection() == prop &&
                   prop->GetParentState() == initialState &&
                   !IsPropertyPendingRemoval(prop) &&
                   !prop->HasFlag(wxPGFlags::BeingDeleted);
        };
    bool propertyStillCurrent = propertyTransactionIsValid();
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveCallback);

    const bool wasEndingLabelEdit = wxPGGetPropertyGridTransientState(this).endingLabelEdit;
    wxPGGetPropertyGridTransientState(this).endingLabelEdit = true;
    wxScopeGuard leaveEndLabelEdit = wxMakeGuard(
        [weakThis, this, wasEndingLabelEdit]()
        {
            if ( weakThis.get() == this )
                wxPGGetPropertyGridTransientState(this).endingLabelEdit = wasEndingLabelEdit;
        });
    wxUnusedVar(leaveEndLabelEdit);

    if ( commit && !propertyStillCurrent )
        commit = false;

    if ( commit )
    {
        if ( !(selFlags & wxPGSelectPropertyFlags::DontSendEvent) )
        {
            // Don't send wxEVT_PG_LABEL_EDIT_ENDING event recursively
            // for the same property and the same label.
            if ( m_processedEvent &&
                 m_processedEvent->GetEventType() == wxEVT_PG_LABEL_EDIT_ENDING &&
                 m_processedEvent->GetProperty() == prop )
            {
                return;
            }

            // wxPGSelectPropertyFlags::NoValidate is passed correctly in selFlags
            const bool vetoed = SendEvent( wxEVT_PG_LABEL_EDIT_ENDING,
                                           prop, nullptr, selFlags,
                                           labelColIdx );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;

            const bool editorStillCurrent =
                wxWeakWindowIsAvailableForCallbacks(weakEditor, editor) &&
                m_labelEditor == editor &&
                m_labelEditorProperty == prop &&
                m_selColumn == labelColIdx;

            if ( !editorStillCurrent )
            {
                // Destroy() is asynchronous on some ports. Do not retain a
                // control which is already scheduled for destruction merely
                // because the event was vetoed and its property stayed valid.
                if ( m_labelEditor == editor &&
                     !wxWeakWindowIsAvailableForCallbacks(weakEditor,
                                                          editor) )
                {
                    m_labelEditor = nullptr;
                    m_labelEditorProperty = nullptr;
                    m_selColumn = 1;
                }
                return;
            }

            // A veto keeps the current label editor open only while the
            // complete transaction it refers to is still current. Event
            // handlers are allowed to remove the property, switch pages or
            // destroy the label control itself.
            if ( vetoed && propertyTransactionIsValid() )
                return;

            // Selection changes and property removal are legal in the public
            // callback. The old editor must still be closed, but its stale
            // property must not be dereferenced or committed.
            if ( !propertyTransactionIsValid() )
            {
                commit = false;
                propertyStillCurrent = false;
            }
        }

        if ( commit )
        {
            wxString text = editor->GetValue();
            if ( !propertyTransactionIsValid() ||
                 !wxWeakWindowIsAvailableForCallbacks(weakEditor, editor) ||
                 m_labelEditor != editor ||
                 m_labelEditorProperty != prop )
            {
                commit = false;
                propertyStillCurrent = false;
            }

            // Cell handling for label in column 0
            // is done internally in SetLabel() function
            if ( commit && labelColIdx == 0 )
            {
                prop->SetLabel(text);
            }
            else if ( commit )
            {
                wxPGCell* cell = nullptr;
                if ( prop->HasCell(labelColIdx) )
                {
                    cell = &prop->GetCell(labelColIdx);
                }
                else
                {
                    cell = &prop->GetOrCreateCell(labelColIdx);
                }

                if ( cell )
                    cell->SetText(text);
            }

            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
            if ( !propertyTransactionIsValid() )
                propertyStillCurrent = false;
        }
    }

    m_selColumn = 1;
    wxWindow* const focus = wxWindow::FindFocus();
    const bool editorHadFocus =
        focus == editor || (focus && editor->IsDescendant(focus));
    const bool wasFocused =
        editorHadFocus || (m_iFlags & wxPG_FL_FOCUSED) != 0;

    m_labelEditor = nullptr;
    m_labelEditorProperty = nullptr;
    DestroyEditorWnd(editor);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    // Fix focus (needed at least on wxGTK)
    if ( wasFocused )
    {
        if ( editorHadFocus )
            SetFocus();
        else
            SetFocusOnCanvas();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    if ( propertyStillCurrent && propertyTransactionIsValid() )
        DrawItem(prop);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetExtraStyle( long exStyle )
{
    if ( exStyle & wxPG_EX_ENABLE_TLP_TRACKING )
        OnTLPChanging(::wxGetTopLevelParent(this));
    else
        OnTLPChanging(nullptr);

    if ( exStyle & wxPG_EX_NATIVE_DOUBLE_BUFFERING )
    {
        // Only apply wxPG_EX_NATIVE_DOUBLE_BUFFERING if the window
        // truly was double-buffered.
        if ( !IsDoubleBuffered() )
        {
            exStyle &= ~(wxPG_EX_NATIVE_DOUBLE_BUFFERING);
        }
        else
        {
            wxDELETE(m_doubleBuffer);
        }
    }

    wxControl::SetExtraStyle( exStyle );

    if ( exStyle & wxPG_EX_INIT_NOCAT )
        m_pState->InitNonCatMode();

    if ( exStyle & wxPG_EX_HELP_AS_TOOLTIPS )
        m_windowStyle |= wxPG_TOOLTIPS;

    // Set global style
    wxPGGlobalVars->m_extraStyle = exStyle;
}

// -----------------------------------------------------------------------

// returns the best acceptable minimal size
wxSize wxPropertyGrid::DoGetBestSize() const
{
    const wxWeakRef<wxWindow> weakThis(
        const_cast<wxPropertyGrid*>(this));
    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return wxDefaultSize;

    int lineHeight = wxMax(FromDIP(15), m_lineHeight);

    // don't make the grid too tall (limit height to 10 items) but don't
    // make it too small either
    int numLines = wxMin
                   (
                    wxMax(state->DoGetRoot()->GetChildCount(), 3),
                    10
                   );

    int width = m_marginWidth;
    for ( unsigned int i = 0; i < state->GetColumnCount(); i++ )
    {
        const int columnWidth =
            state->GetColumnFitWidth(state->DoGetRoot(), i, true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return wxDefaultSize;
        }
        width += columnWidth;
    }

    return wxSize(width, lineHeight*numLines + 40);
}

void wxPropertyGrid::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);
    Refresh(false);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnTLPChanging( wxWindow* newTLP )
{
    if ( newTLP == m_tlp )
        return;

    wxMilliClock_t currentTime = ::wxGetLocalTimeMillis();

    //
    // Parent changed so let's re-determine and re-hook the
    // correct top-level window.
    if ( m_tlp )
    {
        m_tlp->Unbind(wxEVT_CLOSE_WINDOW, &wxPropertyGrid::OnTLPClose, this);
        m_tlpClosed = m_tlp;
        m_tlpClosedTime = currentTime;
    }

    if ( newTLP )
    {
        // Only accept new tlp if same one was not just dismissed.
        if ( newTLP != m_tlpClosed ||
             m_tlpClosedTime+250 < currentTime )
        {
            newTLP->Bind(wxEVT_CLOSE_WINDOW, &wxPropertyGrid::OnTLPClose, this);
            m_tlpClosed = nullptr;
        }
        else
        {
            newTLP = nullptr;
        }
    }

    m_tlp = newTLP;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnTLPClose( wxCloseEvent& event )
{
    // ClearSelection forces value validation/commit.
    if ( event.CanVeto() && !DoClearSelection() )
    {
        event.Veto();
        return;
    }

    // Ok, it can close, set tlp pointer to nullptr. Some other event
    // handler can of course veto the close, but our OnIdle() should
    // then be able to regain the tlp pointer.
    OnTLPChanging(nullptr);

    event.Skip();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Reparent( wxWindowBase *newParent )
{
    OnTLPChanging((wxWindow*)newParent);

    bool res = wxControl::Reparent(newParent);

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ScrollWindow(int dx, int dy, const wxRect* rect)
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxControl::ScrollWindow(dx, dy, rect);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    if ( dx != 0 )
    {
        // Notify wxPropertyGridManager about the grid being scrolled horizontally
        // to scroll the column header, if present.
        SendEvent(wxEVT_PG_HSCROLL, dx);
    }
}
// -----------------------------------------------------------------------

void wxPropertyGrid::SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                                   int noUnitsX, int noUnitsY,
                                   int xPos, int yPos, bool noRefresh)
{
    const wxWeakRef<wxWindow> weakThis(this);
    int oldX;
    CalcUnscrolledPosition(0, 0, &oldX, nullptr);
    wxSystemThemedControl<wxScrolled<wxControl>>::SetScrollbars(pixelsPerUnitX, pixelsPerUnitY,
                                  noUnitsX, noUnitsY, xPos, yPos, noRefresh);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    int newX;
    CalcUnscrolledPosition(0, 0, &newX, nullptr);
    if ( newX != oldX )
    {
        // Notify wxPropertyGridManager about the grid being scrolled horizontally
        // to scroll the column header, if present.
        SendEvent(wxEVT_PG_HSCROLL, oldX - newX);
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid Font and Colour Methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateFontAndBitmapStuff( int vspacing )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    int x = 0, y = 0;

    m_captionFont = wxControl::GetFont();

    GetTextExtent(wxS("jG"), &x, &y, nullptr, nullptr, &m_captionFont);
    m_fontHeight = y;

#ifdef wxPG_ICON_WIDTH
#if wxPG_USE_RENDERER_NATIVE
    wxSize iconSize = wxRendererNative::Get().GetExpanderSize(this);
#else
    wxSize iconSize = wxRendererNative::GetGeneric().GetExpanderSize(this);
#endif
    m_iconWidth = iconSize.GetWidth();
    m_iconHeight = iconSize.GetHeight();
#endif

    m_gutterWidth = wxMax(0, (FromDIP(16) - m_iconWidth) / 2);

    m_subgroup_extramargin = m_iconWidth + m_gutterWidth;

    int vdiv = 6;
    if ( vspacing <= 1 ) vdiv = 12;
    else if ( vspacing >= 3 ) vdiv = 3;

    m_spacingy = m_fontHeight / vdiv;
    if ( m_spacingy < wxPG_YSPACING_MIN )
        m_spacingy = wxPG_YSPACING_MIN;

    m_marginWidth = 0;
    if ( !(m_windowStyle & wxPG_HIDE_MARGIN) )
        m_marginWidth = m_gutterWidth*2 + m_iconWidth;

    m_captionFont.SetWeight(wxFONTWEIGHT_BOLD);
    GetTextExtent(wxS("jG"), &x, &y, nullptr, nullptr, &m_captionFont);

    m_lineHeight = m_fontHeight+(2*m_spacingy)+1;

    // button spacing
    m_buttonSpacingY = (m_lineHeight - m_iconHeight) / 2;
    if ( m_buttonSpacingY < 0 ) m_buttonSpacingY = 0;

    if ( state )
    {
        state->CalculateFontAndBitmapStuff(vspacing);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }

    SetScrollRate(wxPG_PIXELS_PER_UNIT, wxPG_PIXELS_PER_UNIT);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        RecalculateVirtualSize();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }

    InvalidateBestSize();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    if ((m_iFlags & wxPG_FL_INITIALIZED)!=0) {
        RegainColours();
        Refresh();
    }
    event.Skip();
}

void wxPropertyGrid::OnDPIChanged(wxDPIChangedEvent &event)
{
    event.Skip();

    if ( !m_pState )
        return;

    const wxWeakRef<wxWindow> weakThis(this);

    // The drag snapshot is expressed in physical pixels. It must never cross
    // a DPI boundary or a later capture-lost rollback would restore a value
    // from the old scale.
    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    wxPropertyGridManager* const manager =
        wxDynamicCast(m_eventObject, wxPropertyGridManager);
    const wxWeakRef<wxWindow> weakManager(manager);
    std::vector<wxPropertyGridPageState*> states;
    if ( manager )
    {
        states.reserve(manager->GetPageCount() + 1);
        for ( size_t i = 0; i < manager->GetPageCount(); ++i )
        {
            wxPropertyGridPage* const page =
                manager->GetPage(static_cast<unsigned int>(i));
            if ( page )
                states.push_back(page->GetStatePtr());
        }
        if ( manager->m_emptyPage )
            states.push_back(manager->m_emptyPage->GetStatePtr());
    }

    if ( std::find(states.begin(), states.end(), m_pState) == states.end() )
        states.push_back(m_pState);

    const auto stateIsStillOwned =
        [this, manager, &weakThis, &weakManager](
            const wxPropertyGridPageState* candidate)
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || !candidate )
                return false;
            if ( m_pState == candidate )
                return true;
            if ( !manager || !wxWeakWindowIsAvailableForCallbacks(weakManager, manager) ||
                 m_eventObject != manager )
            {
                return false;
            }
            if ( manager->m_emptyPage &&
                 manager->m_emptyPage->GetStatePtr() == candidate )
            {
                return true;
            }
            for ( size_t i = 0; i < manager->GetPageCount(); ++i )
            {
                const wxPropertyGridPage* const page =
                    manager->GetPage(static_cast<unsigned int>(i));
                if ( page && page->GetStatePtr() == candidate )
                    return true;
            }
            return false;
        };

    for ( wxPropertyGridPageState* const state : states )
    {
        if ( !stateIsStillOwned(state) )
            continue;

        state->ScaleForDPI(event);
    }

    wxPropertyGridPageState* const dpiState = m_pState;
    const bool restoreDontCenter = dpiState->m_dontCenterSplitter;
    dpiState->m_dontCenterSplitter = true;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveDPIChange = wxMakeGuard(
        [weakThis, weakManager, this, manager,
         dpiState, restoreDontCenter]()
        {
            if ( weakThis.get() == this )
            {
                bool stateStillOwned = m_pState == dpiState;
                if ( !stateStillOwned &&
                     manager && weakManager.get() == manager &&
                     m_eventObject == manager )
                {
                    if ( manager->m_emptyPage &&
                         manager->m_emptyPage->GetStatePtr() == dpiState )
                    {
                        stateStillOwned = true;
                    }
                    for ( size_t i = 0;
                          !stateStillOwned && i < manager->GetPageCount();
                          ++i )
                    {
                        const wxPropertyGridPage* const page =
                            manager->GetPage(static_cast<unsigned int>(i));
                        if ( page && page->GetStatePtr() == dpiState )
                        {
                            stateStillOwned = true;
                            break;
                        }
                    }
                }
                if ( stateStillOwned )
                    dpiState->m_dontCenterSplitter = restoreDontCenter;
                --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
            }
        });
    wxUnusedVar(leaveDPIChange);

    CalculateFontAndBitmapStuff(m_vspacing);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != dpiState )
        return;

    for ( wxPropertyGridPageState* const state : states )
    {
        if ( state != m_pState && stateIsStillOwned(state) )
        {
            state->CalculateFontAndBitmapStuff(m_vspacing);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != dpiState )
            {
                return;
            }
        }
    }

    if ( GetSelection() )
    {
        CorrectEditorWidgetSizeX();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != dpiState )
        {
            return;
        }
    }

    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != dpiState )
    {
        return;
    }

    if ( !HasExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        // Recreate the back buffer with correct DPI.
        delete m_doubleBuffer;
        m_doubleBuffer = nullptr;
        ReallocDoubleBufferIfNeeded();
    }

    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != dpiState )
    {
        return;
    }

    Refresh();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != dpiState )
    {
        return;
    }

    if ( wxPGProperty* const selected = GetSelection() )
        RefreshProperty(selected);

}

// -----------------------------------------------------------------------

static wxColour wxPGAdjustColour(const wxColour& src, int ra,
                                 int ga = 1000, int ba = 1000,
                                 bool forceDifferent = false)
{
    if ( ga >= 1000 )
        ga = ra;
    if ( ba >= 1000 )
        ba = ra;

    // Recursion guard (allow 2 max)
    static int isinside = 0;
    isinside++;
    wxCHECK_MSG( isinside < 3,
                 *wxBLACK,
                 wxS("wxPGAdjustColour should not be recursively called more than once") );

    wxColour dst;

    int r = src.Red();
    int g = src.Green();
    int b = src.Blue();
    int r2 = r + ra;
    if ( r2>255 ) r2 = 255;
    else if ( r2<0) r2 = 0;
    int g2 = g + ga;
    if ( g2>255 ) g2 = 255;
    else if ( g2<0) g2 = 0;
    int b2 = b + ba;
    if ( b2>255 ) b2 = 255;
    else if ( b2<0) b2 = 0;

    // Make sure they are somewhat different
    if ( forceDifferent && (abs((r+g+b)-(r2+g2+b2)) < abs(ra/2)) )
        dst = wxPGAdjustColour(src,-(ra*2));
    else
        dst = wxColour(r2,g2,b2);

    // Recursion guard (allow 2 max)
    isinside--;

    return dst;
}


static int wxPGGetColAvg( const wxColour& col )
{
    return (col.Red() + col.Green() + col.Blue()) / 3;
}


void wxPropertyGrid::RegainColours()
{
    if ( !(m_coloursCustomized & CustomColour_CaptionBg) )
    {
        wxColour col = wxSystemSettings::GetColour( wxSYS_COLOUR_GRIDLINES );
    #ifdef __WXOSX__
        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            // Make sure colour is light enough
            int colDec = wxPGGetColAvg(col);
            if ( colDec < 30 )
                col = wxPGAdjustColour(col, 20);
        }
        else
    #endif
        {
            // Make sure colour is dark enough
        #ifdef __WXGTK__
            int colDec = wxPGGetColAvg(col) - 230;
        #else
            int colDec = wxPGGetColAvg(col) - 200;
        #endif
            if ( colDec > 0 )
                col = wxPGAdjustColour(col,-colDec);
        }
        m_colCapBack = col;
        m_categoryDefaultCell.GetData()->SetBgCol(m_colCapBack);
    }

    if ( !(m_coloursCustomized & CustomColour_Margin) )
        m_colMargin = m_colCapBack;

    if ( !(m_coloursCustomized & CustomColour_CaptionText) )
    {
    #ifdef __WXGTK__
        int colDec = -90;
    #else
        int colDec = -72;
    #endif
        wxColour capForeCol = wxPGAdjustColour(m_colCapBack,colDec,5000,5000,true);
        if (wxPGGetColAvg(m_colCapBack) < 100)
            capForeCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );

        m_colCapFore = capForeCol;
        m_categoryDefaultCell.GetData()->SetFgCol(capForeCol);
    }

    if ( !(m_coloursCustomized & CustomColour_CellBg) )
    {
        wxColour bgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgCol;
        m_propertyDefaultCell.GetData()->SetBgCol(bgCol);
        if ( !m_unspecifiedAppearance.GetBgCol().IsOk() )
            m_unspecifiedAppearance.SetBgCol(bgCol);
    }

    if ( !(m_coloursCustomized & CustomColour_CellText) )
    {
        wxColour fgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
        m_colPropFore = fgCol;
        m_propertyDefaultCell.GetData()->SetFgCol(fgCol);
        if ( !m_unspecifiedAppearance.GetFgCol().IsOk() )
            m_unspecifiedAppearance.SetFgCol(fgCol);
    }

    if ( !(m_coloursCustomized & CustomColour_SelectionBg) )
        m_colSelBack = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );

    if ( !(m_coloursCustomized & CustomColour_SelectionText) )
        m_colSelFore = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT );

    if ( !(m_coloursCustomized & CustomColour_Line) )
        m_colLine = m_colCapBack;

    if ( !(m_coloursCustomized & CustomColour_DisabledText) )
        m_colDisPropFore = wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT );

    m_colEmptySpace = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColours()
{
    m_coloursCustomized = CustomColour_None;

    RegainColours();

    Refresh();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SetFont( const wxFont& font )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;

    // Must disable active editor.
    DoClearSelection();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return false;
    }

    bool res = wxControl::SetFont( font );
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return false;
    }
    if ( res && GetParent()) // may not have been Create()ed yet if SetFont called from SetWindowVariant
    {
        CalculateFontAndBitmapStuff( m_vspacing );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return false;
        }
        Refresh();
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetLineColour( const wxColour& col )
{
    m_colLine = col;
    m_coloursCustomized |= CustomColour_Line;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetMarginColour( const wxColour& col )
{
    m_colMargin = col;
    m_coloursCustomized |= CustomColour_Margin;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellBackgroundColour( const wxColour& col )
{
    m_colPropBack = col;
    m_coloursCustomized |= CustomColour_CellBg;

    m_propertyDefaultCell.GetData()->SetBgCol(col);
    m_unspecifiedAppearance.SetBgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellTextColour( const wxColour& col )
{
    m_colPropFore = col;
    m_coloursCustomized |= CustomColour_CellText;

    m_propertyDefaultCell.GetData()->SetFgCol(col);
    m_unspecifiedAppearance.SetFgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetEmptySpaceColour( const wxColour& col )
{
    m_colEmptySpace = col;

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellDisabledTextColour( const wxColour& col )
{
    m_colDisPropFore = col;
    m_coloursCustomized |= CustomColour_DisabledText;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionBackgroundColour( const wxColour& col )
{
    m_colSelBack = col;
    m_coloursCustomized |= CustomColour_SelectionBg;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionTextColour( const wxColour& col )
{
    m_colSelFore = col;
    m_coloursCustomized |= CustomColour_SelectionText;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionBackgroundColour( const wxColour& col )
{
    m_colCapBack = col;
    m_coloursCustomized |= CustomColour_CaptionBg;

    m_categoryDefaultCell.GetData()->SetBgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionTextColour( const wxColour& col )
{
    m_colCapFore = col;
    m_coloursCustomized |= CustomColour_CaptionText;

    m_categoryDefaultCell.GetData()->SetFgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property adding and removal
// -----------------------------------------------------------------------

void wxPropertyGrid::PrepareAfterItemsAdded()
{
    if ( !m_pState || !m_pState->m_itemsAdded ) return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    state->m_itemsAdded = false;

    if ( m_windowStyle & wxPG_AUTO_SORT )
    {
        Sort(wxPGPropertyValuesFlags::SortTopLevelOnly);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }

    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    // Fix editor position
    CorrectEditorWidgetPosY();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property operations
// -----------------------------------------------------------------------

bool wxPropertyGrid::EnsureVisible( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* const initialSelection = GetSelection();
    wxWindow* const primary = m_wndEditor;
    wxWindow* const secondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakPrimary(primary);
    const wxWeakRef<wxWindow> weakSecondary(secondary);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveEnsureVisible = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveEnsureVisible);

    const auto transactionIsValid =
        [weakThis, weakPrimary, weakSecondary, this, state, p,
         initialSelection, primary, secondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            bool editorsAreValid = true;
            if ( primary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPrimary, primary)) )
            {
                if ( m_wndEditor == primary )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( secondary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondary, secondary)) )
            {
                if ( m_wndEditor2 == secondary )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }

            return editorsAreValid &&
                   !wxWindowIsUnavailableForCallbacks(this) &&
                   m_pState == state &&
                   GetSelection() == initialSelection &&
                   p->GetParentState() == state &&
                   !IsPropertyPendingRemoval(p) &&
                   !p->HasFlag(wxPGFlags::BeingDeleted) &&
                   m_wndEditor == primary &&
                   m_wndEditor2 == secondary;
        };

    if ( !transactionIsValid() )
        return false;

    Update();
    if ( !transactionIsValid() )
        return false;

    bool changed = false;

    // Is it inside collapsed section?
    if ( !p->IsVisible() )
    {
        // expand parents
        wxPGProperty* parent = p->GetParent();
        wxPGProperty* grandparent = parent->GetParent();

        if ( grandparent && grandparent != m_pState->DoGetRoot() )
        {
            Expand( grandparent );
            if ( !transactionIsValid() )
                return false;
        }

        Expand( parent );
        if ( !transactionIsValid() )
            return false;
        changed = true;
    }

    // Need to scroll?
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    int y = p->GetY();

    if ( y < vy )
    {
        Scroll(vx, y/wxPG_PIXELS_PER_UNIT );
        if ( !transactionIsValid() )
            return false;
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }
    else if ( (y+m_lineHeight) > (vy+m_height) )
    {
        Scroll(vx, (y-m_height+(m_lineHeight*2))/wxPG_PIXELS_PER_UNIT );
        if ( !transactionIsValid() )
            return false;
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }

    if ( changed )
    {
        DrawItems( p, p );
        if ( !transactionIsValid() )
            return false;
    }

    return changed;
}

// -----------------------------------------------------------------------
// wxPropertyGrid helper methods called by properties
// -----------------------------------------------------------------------

// Control font changer helper.
void wxPropertyGrid::SetCurControlBoldFont()
{
    wxWindow* editor = GetEditorControl();
    editor->SetFont( m_captionFont );
}

// -----------------------------------------------------------------------

wxPoint wxPropertyGrid::GetGoodEditorDialogPosition( wxPGProperty* p,
                                                     const wxSize& sz )
{
    if ( IsSmallScreen() )
    {
        // On small-screen devices, always show dialogs with default position and size.
        return wxDefaultPosition;
    }

    int splitterX = GetSplitterPosition();
    int x = splitterX;
    int y = p->GetY();

    wxCHECK_MSG( y >= 0, wxDefaultPosition, wxS("invalid y?") );

    ImprovedClientToScreen( &x, &y );

    wxRect displayRect = wxDisplay(this).GetGeometry();

    x -= displayRect.GetX();
    y -= displayRect.GetY();

    int new_x;
    int new_y;

    if ( x > (displayRect.GetWidth()/2) )
        // left
        new_x = x + (m_width-splitterX) - sz.x;
    else
        // right
        new_x = x;

    if ( y > (displayRect.GetHeight()/2) )
        // above
        new_y = y - sz.y;
    else
        // below
        new_y = y + m_lineHeight;

    return wxPoint(new_x + displayRect.GetX(), new_y + displayRect.GetY());
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::ExpandEscapeSequences( wxString& dst_str, const wxString& src_str )
{
    dst_str.clear();

    if ( src_str.empty() )
    {
        return dst_str;
    }

    bool prev_is_slash = false;

    for ( wxUniChar a : src_str )
    {
        if ( a != wxS('\\') )
        {
            if ( !prev_is_slash )
            {
                dst_str << a;
            }
            else
            {
                if ( a == wxS('n') )
                    dst_str << wxS('\n');
                else if ( a == wxS('r') )
                    dst_str << wxS('\r');
                else if ( a == wxS('t') )
                    dst_str << wxS('\t');
                else
                    dst_str << a;
            }
            prev_is_slash = false;
        }
        else
        {
            if ( prev_is_slash )
            {
                dst_str << wxS('\\');
                prev_is_slash = false;
            }
            else
            {
                prev_is_slash = true;
            }
        }
    }
    return dst_str;
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::CreateEscapeSequences( wxString& dst_str, const wxString& src_str )
{
    dst_str.clear();

    if ( src_str.empty() )
    {
        return dst_str;
    }

    for( wxUniChar a : src_str )
    {
        if ( a == wxS('\r') )
            // Carriage Return.
            dst_str << wxS("\\r");
        else if ( a == wxS('\n') )
            // Line Feed.
            dst_str << wxS("\\n");
        else if ( a == wxS('\t') )
            // Tab.
            dst_str << wxS("\\t");
        else if ( a == wxS('\\') )
            // Escape character (backslash).
            dst_str << wxS("\\\\");
        else
            dst_str << a;
    }
    return dst_str;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::IsSmallScreen()
{
    return wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;
}

// -----------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_2
// static
wxBitmap wxPropertyGrid::RescaleBitmap(const wxBitmap& srcBmp,
                                       double scaleX, double scaleY)
{
    wxSize size = srcBmp.GetSize();
    size.x = wxRound(size.x * scaleX);
    size.y = wxRound(size.y * scaleY);

    wxBitmap dstBmp(srcBmp);
    wxBitmap::Rescale(dstBmp, size);
    return dstBmp;
}
#endif // WXWIN_COMPATIBILITY_3_2

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGrid::DoGetItemAtY( int y ) const
{
    // Outside?
    if ( y < 0 )
        return nullptr;

    unsigned int a = 0;
    return m_pState->DoGetRoot()->GetItemAtY(y, m_lineHeight, &a);
}

// -----------------------------------------------------------------------
// wxPropertyGrid graphics related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    const wxWeakRef<wxWindow> weakThis(this);
    // wxBufferedPaintDC cannot be used here: its destructor queries the
    // associated wxWindow while blitting, but a renderer is application code
    // and may synchronously resize this grid and replace m_doubleBuffer (or
    // begin grid teardown). Keep the blit explicit and conditional instead.
    wxPaintDC paintDC(this);
    wxBitmap paintBuffer;
    std::unique_ptr<wxMemoryDC> bufferDC;
    wxDC* dcPtr = &paintDC;
    if ( !HasExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        if ( m_doubleBuffer && m_doubleBuffer->IsOk() )
        {
            // Retain the backing store independently of m_doubleBuffer for the
            // complete callback transaction.
            paintBuffer = *m_doubleBuffer;
            bufferDC.reset(new wxMemoryDC(paintBuffer));
            bufferDC->CopyAttributes(paintDC);
            dcPtr = bufferDC.get();
        }
    }
    wxASSERT( dcPtr );
    PrepareDC(*dcPtr);
    // Unused area will be cleared when drawing the items
/*
    dcPtr->SetBackground(m_colEmptySpace);
    dcPtr->Clear();
*/

    // Don't paint after destruction has begun
    if ( !HasInternalFlag(wxPG_FL_INITIALIZED) )
        return;

    // Find out where the window is scrolled to
    int vx, vy;
    GetViewStart(&vx, &vy);
    vx *= wxPG_PIXELS_PER_UNIT;
    vy *= wxPG_PIXELS_PER_UNIT;

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();
    r.Offset(vx, vy);

    // Repaint this rectangle
    DrawItems(*dcPtr, r.y, r.y + r.height-1, &r);

    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    if ( bufferDC )
    {
        // Mirror wxBufferedDC::UnMask(), but only while the destination window
        // is provably alive. This is the crucial difference from relying on a
        // destructor-driven blit after an arbitrary renderer callback.
        bufferDC->SetUserScale(1.0, 1.0);

        wxCoord deviceOriginX = 0;
        wxCoord deviceOriginY = 0;
        bufferDC->GetDeviceOrigin(&deviceOriginX, &deviceOriginY);

        int destinationWidth;
        int destinationHeight;
        paintDC.GetSize(&destinationWidth, &destinationHeight);
        const int width = wxMin(paintBuffer.GetWidth(), destinationWidth);
        const int height = wxMin(paintBuffer.GetHeight(), destinationHeight);
        const wxPoint logicalOrigin = bufferDC->GetLogicalOrigin();
        paintDC.Blit(-logicalOrigin.x,
                     -logicalOrigin.y,
                     width,
                     height,
                     bufferDC.get(),
                     -deviceOriginX,
                     -deviceOriginY);
    }

    // We assume that the size set when grid is shown
    // is what is desired.
    SetInternalFlag(wxPG_FL_GOOD_SIZE_SET);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawExpanderButton( wxDC& dc, const wxRect& rect,
                                         wxPGProperty* property ) const
{
    // Prepare rectangle to be used
    wxRect r(rect);
    r.Offset(m_gutterWidth, m_buttonSpacingY);
    r.width = m_iconWidth; r.height = m_iconHeight;

    // wxRenderer functions are non-mutating in nature, so it
    // should be safe to cast "const wxPropertyGrid*" to "wxWindow*".
    // Hopefully this does not cause problems.
#ifndef wxPG_ICON_WIDTH
    wxBitmap bmp = property->IsExpanded() ? s_collbmp : s_expandbmp;
    dc.DrawBitmap(bmp, r.x, r.y, true);
#else
#if wxPG_USE_RENDERER_NATIVE
    wxRendererNative::Get().
#else
    wxRendererNative::GetGeneric().
#endif
        DrawTreeItemButton(
            const_cast<wxPropertyGrid*>(this),
            dc,
            r,
            property->IsExpanded() ? wxCONTROL_EXPANDED : wxCONTROL_NONE
        );
#endif
}

// -----------------------------------------------------------------------

//
// This is the one called by OnPaint event handler and others.
// topy and bottomy are already unscrolled (i.e. physical)
//
void wxPropertyGrid::DrawItems( wxDC& dc,
                                unsigned int topItemY,
                                unsigned int bottomItemY,
                                const wxRect* itemsRect )
{
    if ( IsFrozen() ||
         m_height < 1 ||
         bottomItemY < topItemY ||
         !m_pState )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).drawingDepth;
    wxScopeGuard leaveDrawing = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).drawingDepth;
    });
    wxUnusedVar(leaveDrawing);

    state->EnsureVirtualHeight();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    wxRect tempItemsRect;
    if ( !itemsRect )
    {
        tempItemsRect = wxRect(0, topItemY,
                               state->GetVirtualWidth(),
                               bottomItemY - topItemY + 1);
        itemsRect = &tempItemsRect;
    }

    // items added check
    if ( state->m_itemsAdded )
    {
        PrepareAfterItemsAdded();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }

    if ( state->DoGetRoot()->GetChildCount() > 0 )
    {
        const unsigned int columnCount = state->GetColumnCount();
        const size_t columnWidthsCount = state->m_colWidths.size();
        int paintFinishY = DoDrawItems(dc, itemsRect) + 1;
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state ||
             state->GetColumnCount() != columnCount ||
             state->m_colWidths.size() != columnWidthsCount )
        {
            return;
        }
        int drawBottomY = itemsRect->GetBottom();

        // Clear area beyond last painted property
        if ( paintFinishY < drawBottomY )
        {
            dc.SetPen(m_colEmptySpace);
            dc.SetBrush(m_colEmptySpace);
            dc.DrawRectangle(0, paintFinishY,
                                    m_width,
                                    drawBottomY-paintFinishY+1);
        }
    }
    else
    {
        // Just clear the area
        dc.SetPen(m_colEmptySpace);
        dc.SetBrush(m_colEmptySpace);
        dc.DrawRectangle(*itemsRect);
    }
}

// -----------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_0
int wxPropertyGrid::DoDrawItemsBase( wxDC& dc,
                                 const wxRect* itemsRect,
                                 bool WXUNUSED(isBuffered) ) const
#else
int wxPropertyGrid::DoDrawItems( wxDC& dc,
                                 const wxRect* itemsRect ) const
#endif
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const self = const_cast<wxPropertyGrid*>(this);
    const wxWeakRef<wxWindow> weakThis(self);
    wxPropertyGridPageState* const initialState = m_pState;
    if ( !initialState )
        return -1;
    const unsigned int initialColumnCount = initialState->GetColumnCount();
    const size_t initialColumnWidthsCount = initialState->m_colWidths.size();

    ++wxPGGetPropertyGridTransientState(self).propertyCallbackDepth;
    wxScopeGuard leaveDrawing = wxMakeGuard([weakThis, self]()
    {
        if ( weakThis.get() == self )
            --wxPGGetPropertyGridTransientState(self).propertyCallbackDepth;
    });
    wxUnusedVar(leaveDrawing);

    const auto transactionIsValid =
        [weakThis, self, initialState, initialColumnCount,
         initialColumnWidthsCount](wxPGProperty* property = nullptr)
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, self) ||
                 self->m_pState != initialState ||
                 initialState->GetColumnCount() != initialColumnCount ||
                 initialState->m_colWidths.size() != initialColumnWidthsCount )
            {
                return false;
            }

            return !property ||
                   !self->IsPropertyPendingRemoval(property);
        };

    const wxPGProperty* firstItem = DoGetItemAtY(itemsRect->y);
    if ( !firstItem ) // Signal a need to clear entire paint area if grid is empty
        return -1;

    if ( IsFrozen() || m_height < 1 )
        return itemsRect->GetBottom();

    wxCHECK_MSG( !m_pState->m_itemsAdded, itemsRect->GetBottom(),
                 wxS("no items added") );
    wxASSERT( m_pState->DoGetRoot()->GetChildCount() );

    int lh = m_lineHeight;
    int firstItemTopY = itemsRect->y;
    int lastItemBottomY = itemsRect->GetBottom();

    // Align y coordinates to item boundaries
    firstItemTopY -= firstItemTopY % lh;
    lastItemBottomY += lh - (lastItemBottomY % lh);
    lastItemBottomY -= 1;

    // Entire range outside scrolled, visible area?
    if ( firstItemTopY >= (int)m_pState->GetVirtualHeight() ||
         lastItemBottomY <= 0 )
        return itemsRect->GetBottom();

    wxCHECK_MSG( firstItemTopY < lastItemBottomY,
                 itemsRect->GetBottom(),
                 wxS("invalid y values") );

    /*
    wxLogDebug(wxS(" -> DoDrawItems(\"%s\" -> \"%s\"")
               wxS(" %i -> %i height=%i (ch=%i), itemsRect = 0x%lX %i,%i %ix%i)"),
        firstItem->GetLabel(),
        lastItem->GetLabel(),
        firstItemTopY, lastItemBottomY,
        (int)(lastItemBottomY - firstItemTopY),
        (int)m_height,
        (unsigned long)&itemsRect,
        itemsRect->x, itemsRect->y, itemsRect->width, itemsRect->height );
    */

    long windowStyle = m_windowStyle;

    int x = m_marginWidth;

    wxFont normalFont = GetFont();

    bool reallyFocused = (m_iFlags & wxPG_FL_FOCUSED) != 0;

    bool isPgEnabled = IsEnabled();

    //
    // Prepare some pens and brushes that are often changed to.
    //

    wxBrush marginBrush(m_colMargin);
    wxPen marginPen(m_colMargin);
    wxBrush capbgbrush(m_colCapBack,wxBRUSHSTYLE_SOLID);
    wxPen linepen(m_colLine,1,wxPENSTYLE_SOLID);
    wxPen splitterFocusPen(
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
        wxMax(1, FromDIP(2)),
        wxPENSTYLE_SOLID);

    // pen that has same colour as text
    wxPen outlinepen(m_colPropFore,1,wxPENSTYLE_SOLID);

    //
    // Clear margin with background colour
    //
    dc.SetBrush( marginBrush );
    if ( !(windowStyle & wxPG_HIDE_MARGIN) )
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(-1,firstItemTopY-1,x+2,lastItemBottomY-firstItemTopY+2);
    }

    const wxPGProperty* firstSelected = GetSelection();
    const wxPropertyGridPageState* state = initialState;
    const std::vector<int>& colWidths = state->m_colWidths;
    const unsigned int colCount = initialColumnCount;

    dc.SetFont(normalFont);

    wxPropertyGridConstIterator it( state, wxPG_ITERATE_VISIBLE, firstItem );
    int endScanBottomY = lastItemBottomY + lh;
    int y = firstItemTopY;

    //
    // Pre-generate list of visible properties.
    std::vector<wxPGProperty*> visPropArray;
    visPropArray.reserve((m_height/m_lineHeight)+6);

    for ( ; !it.AtEnd(); it.Next() )
    {
        const wxPGProperty* p = *it;

        if ( !p->HasFlag(wxPGFlags::Hidden) )
        {
            visPropArray.push_back(const_cast<wxPGProperty*>(p));

            if ( y > endScanBottomY )
                break;

            y += lh;
        }
    }

    visPropArray.push_back(nullptr);

    wxPGProperty* nextP = visPropArray[0];

    // Calculate splitters positions
    std::vector<int> splitterPos;
    splitterPos.reserve(colCount);
    int sx = x;
    for ( int cw : colWidths )
    {
        sx += cw;
        splitterPos.push_back(sx);
    }

    int viewLeftEdge = itemsRect->x;
    int viewRightEdge = viewLeftEdge + itemsRect->width - 1;
    // Determine columns range to be drawn
    unsigned int firstCol = 0;
    while ( firstCol < colCount-1 && splitterPos[firstCol] < viewLeftEdge )
        firstCol++;
    unsigned int lastCol = firstCol;
    while ( lastCol < colCount-1 && splitterPos[lastCol] < viewRightEdge )
        lastCol++;
    // Calculate position of the right edge of the last cell
    int cellX = splitterPos[lastCol]+ 1;

    y = firstItemTopY;
    for ( unsigned int arrInd=1;
          nextP && y <= lastItemBottomY;
          arrInd++ )
    {
        wxPGProperty* p = nextP;
        nextP = visPropArray[arrInd];

        int textMarginHere = x;
        int renderFlags = 0;

        int greyDepth = m_marginWidth;
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) )
            greyDepth += (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin;

        // Use basic depth if in non-categoric mode and parent is base array.
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) || p->GetParent() != m_pState->DoGetRoot() )
        {
            textMarginHere += ((p->GetDepth()-1)*m_subgroup_extramargin);
        }

        // Paint margin area
        dc.SetBrush(marginBrush);
        dc.SetPen(marginPen);
        dc.DrawRectangle( 0, y, greyDepth, lh );

        dc.SetPen( linepen );

        int y2 = y + lh;

#ifdef __WXMSW__
        // Margin Edge
        // Modified by JACS to not draw a margin if wxPG_HIDE_MARGIN is specified, since it
        // looks better, at least under Windows when we have a themed border (the themed-window-specific
        // whitespace between the real border and the propgrid margin exacerbates the double-border look).

        // Is this or its parent themed?
        bool suppressMarginEdge = (GetWindowStyle() & wxPG_HIDE_MARGIN) &&
            (((GetWindowStyle() & wxBORDER_MASK) == wxBORDER_THEME) ||
            (((GetWindowStyle() & wxBORDER_MASK) == wxBORDER_NONE) && ((GetParent()->GetWindowStyle() & wxBORDER_MASK) == wxBORDER_THEME)));
        if (suppressMarginEdge)
        {
            // Blank out the margin edge
            dc.SetPen(wxPen(GetBackgroundColour()));
            dc.DrawLine( greyDepth, y, greyDepth, y2 );
            dc.SetPen( linepen );
        }
        else
#endif // __WXMSW__
        {
            dc.DrawLine(greyDepth, y, greyDepth, y2);
        }

        // Splitters
        for (unsigned int i = firstCol; i <= lastCol; i++)
        {
            if ( reallyFocused &&
                 static_cast<int>(i) == wxPGGetPropertyGridTransientState(this).keyboardSplitter )
            {
                dc.SetPen(splitterFocusPen);
            }
            dc.DrawLine(splitterPos[i], y, splitterPos[i], y2);
            dc.SetPen(linepen);
        }

        // Horizontal Line, below
        //   (not if both this and next is category caption)
        if ( p->IsCategory() &&
             nextP && nextP->IsCategory() )
            dc.SetPen(m_colCapBack);

        dc.DrawLine(greyDepth, y2 - 1, cellX, y2 - 1);

        //
        // Need to override row colours?
        wxColour rowFgCol;
        wxColour rowBgCol;

        bool isSelected = state->DoIsPropertySelected(p);

        if ( !isSelected )
        {
            // Disabled may get different colour.
            if ( !p->IsEnabled() )
            {
                renderFlags |= wxPGCellRenderer::Disabled |
                               wxPGCellRenderer::DontUseCellFgCol;
                rowFgCol = m_colDisPropFore;
            }
        }
        else
        {
            renderFlags |= wxPGCellRenderer::Selected;

            if ( !p->IsCategory() )
            {
                renderFlags |= wxPGCellRenderer::DontUseCellFgCol |
                               wxPGCellRenderer::DontUseCellBgCol;

                if ( reallyFocused )
                {
                    rowFgCol = (p == firstSelected) ? m_colSelFore : m_colPropFore;
                    rowBgCol = m_colSelBack;
                }
                else if ( isPgEnabled )
                {
                    rowFgCol = m_colPropFore;
                    rowBgCol = m_colMargin;
                }
                else
                {
                    rowFgCol = m_colDisPropFore;
                    rowBgCol = m_colMargin;
                }
            }
        }

        wxBrush rowBgBrush;

        if ( rowBgCol.IsOk() )
            rowBgBrush = wxBrush(rowBgCol);

        if ( HasInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL) )
            renderFlags = renderFlags & ~wxPGCellRenderer::DontUseCellColours;

        //
        // Fill additional margin area with background colour of first cell
        if ( greyDepth < textMarginHere )
        {
            if ( !(renderFlags & wxPGCellRenderer::DontUseCellBgCol) )
            {
                wxPGCell& cell = p->GetCell(0);
                rowBgCol = cell.GetBgCol();
                rowBgBrush = wxBrush(rowBgCol);
            }
            dc.SetBrush(rowBgBrush);
            dc.SetPen(rowBgCol);
            dc.DrawRectangle(greyDepth+1, y,
                             textMarginHere-greyDepth, lh-1);
        }

        bool fontChanged = false;

        // Expander button rectangle
        wxRect butRect( ((p->GetDepth() - 1) * m_subgroup_extramargin),
                        y,
                        m_marginWidth,
                        lh );

        // Default cell rect fill the entire row
        wxRect cellRect(greyDepth, y, cellX - greyDepth, lh-1);

        bool isCategory = p->IsCategory();

        if ( isCategory )
        {
            dc.SetFont(m_captionFont);
            fontChanged = true;

            if ( renderFlags & wxPGCellRenderer::DontUseCellBgCol )
            {
                dc.SetBrush(rowBgBrush);
                dc.SetPen(rowBgCol);
            }

            if ( renderFlags & wxPGCellRenderer::DontUseCellFgCol )
            {
                dc.SetTextForeground(rowFgCol);
            }
        }
        else
        {
            // Fine tune button rectangle to actually fit the cell
            if ( butRect.x > 0 )
                butRect.x += IN_CELL_EXPANDER_BUTTON_X_ADJUST;

            if ( p->HasFlag(wxPGFlags::Modified) &&
                 (windowStyle & wxPG_BOLD_MODIFIED) )
            {
                dc.SetFont(m_captionFont);
                fontChanged = true;
            }

            // Magic fine-tuning for non-category rows
            cellRect.x += 1;
        }

        int firstCellWidth = colWidths[0] - (greyDepth - m_marginWidth);
        int firstCellX = cellRect.x;

        // Calculate cellRect.x for the last cell
        cellRect.x = cellX;

        // Draw cells from back to front so that we can easily tell if the
        // cell on the right was empty from text
        bool prevFilled = true;
        unsigned int ci = lastCol + 1;
        do
        {
            ci--;

            int textXAdd = 0;

            if ( ci == 0 )
            {
                textXAdd = textMarginHere - greyDepth;
                cellRect.width = firstCellWidth;
                cellRect.x = firstCellX;
            }
            else
            {
                int colWidth = colWidths[ci];
                cellRect.width = colWidth;
                cellRect.x -= colWidth;
            }

            // Merge with column to the right?
            if ( !prevFilled && isCategory )
            {
                cellRect.width += colWidths[ci+1];
            }

            if ( !isCategory )
                cellRect.width -= 1;

            wxWindow* cellEditor = nullptr;
            int cellRenderFlags = renderFlags;

            // Tree Item Button (must be drawn before clipping is set up)
            if ( ci == 0 && !HasFlag(wxPG_HIDE_MARGIN) && p->HasVisibleChildren() )
            {
                DrawExpanderButton( dc, butRect, p );
                if ( !transactionIsValid(p) )
                    return y - 1;
            }

            // Background
            if ( isSelected && (ci == 1 || ci == m_selColumn) )
            {
                if ( p == firstSelected )
                {
                    if ( ci == 1 && m_wndEditor )
                        cellEditor = m_wndEditor;
                    else if ( ci == m_selColumn && m_labelEditor )
                        cellEditor = m_labelEditor;
                }

                if ( cellEditor )
                {
                    wxColour editorBgCol =
                        cellEditor->GetBackgroundColour();
                    dc.SetBrush(editorBgCol);
                    dc.SetPen(editorBgCol);
                    dc.SetTextForeground(m_colPropFore);
                    dc.DrawRectangle(cellRect);

                    if ( m_dragStatus != 0 ||
                         (m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE) )
                        cellEditor = nullptr;
                }
                else
                {
                    dc.SetBrush(m_colPropBack);
                    dc.SetPen(m_colPropBack);
                    dc.SetTextForeground(p->IsEnabled() ? m_colPropFore : m_colDisPropFore);
                }
            }
            else
            {
                if ( renderFlags & wxPGCellRenderer::DontUseCellBgCol )
                {
                    dc.SetBrush(rowBgBrush);
                    dc.SetPen(rowBgCol);
                }

                if ( renderFlags & wxPGCellRenderer::DontUseCellFgCol )
                {
                    dc.SetTextForeground(rowFgCol);
                }
            }

            dc.SetClippingRegion(cellRect);

            cellRect.x += textXAdd;
            cellRect.width -= textXAdd;

            // Foreground
            if ( !cellEditor )
            {
                wxPGCellRenderer* renderer;
                int cmnVal = p->GetCommonValue();
                if ( !transactionIsValid(p) )
                {
                    dc.DestroyClippingRegion();
                    return y - 1;
                }
                if ( cmnVal == -1 || ci != 1 )
                {
                    renderer = p->GetCellRenderer(ci);
                    if ( !transactionIsValid(p) || !renderer )
                    {
                        dc.DestroyClippingRegion();
                        return y - 1;
                    }
                    prevFilled = renderer->Render(dc, cellRect, this,
                                                  p, ci, -1,
                                                  cellRenderFlags );
                }
                else
                {
                    renderer = GetCommonValue(cmnVal)->GetRenderer();
                    if ( !transactionIsValid(p) || !renderer )
                    {
                        dc.DestroyClippingRegion();
                        return y - 1;
                    }
                    prevFilled = renderer->Render(dc, cellRect, this,
                                                  p, ci, -1,
                                                  cellRenderFlags );
                }
                if ( !transactionIsValid(p) )
                {
                    dc.DestroyClippingRegion();
                    return y - 1;
                }
            }
            else
            {
                prevFilled = true;
            }

            dc.DestroyClippingRegion();
        }
        while ( ci > firstCol );

        if ( fontChanged )
            dc.SetFont(normalFont);

        y += lh;
    }

    // Clear empty space beyond the right edge of the grid
    dc.SetPen(wxPen(m_colEmptySpace));
    dc.SetBrush(wxBrush(m_colEmptySpace));
    dc.DrawRectangle(cellX, firstItemTopY, viewRightEdge - cellX + 1, lastItemBottomY - firstItemTopY);

    return y - 1;
}

// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetPropertyRect( const wxPGProperty* p1, const wxPGProperty* p2 ) const
{
    if ( m_width < 10 || m_height < 10 ||
         !m_pState->DoGetRoot()->GetChildCount() ||
         p1 == nullptr )
        return wxRect(0,0,0,0);

    int vy = 0;

    //
    // Return rect which encloses the given property range
    // (in logical grid coordinates)
    //

    int visTop = p1->GetY();
    int visBottom;
    if ( p2 )
        visBottom = p2->GetY() + m_lineHeight;
    else
        visBottom = m_height + visTop;

    // If selected property is inside the range, we'll extend the range to include
    // control's size.
    wxPGProperty* selected = GetSelection();
    if ( selected )
    {
        int selectedY = selected->GetY();
        if ( selectedY >= visTop && selectedY < visBottom )
        {
            wxWindow* editor = GetEditorControl();
            if ( editor )
            {
                int visBottom2 = selectedY + editor->GetSize().y;
                if ( visBottom2 > visBottom )
                    visBottom = visBottom2;
            }
        }
    }

    return wxRect(0,visTop-vy,m_pState->GetVirtualWidth(),visBottom-visTop);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItems( const wxPGProperty* p1, const wxPGProperty* p2 )
{
    if ( IsFrozen() || !m_pState )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveDrawing = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveDrawing);

    const auto transactionIsValid =
        [weakThis, this, state](const wxPGProperty* property = nullptr)
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   (!property ||
                    (!IsPropertyPendingRemoval(
                         const_cast<wxPGProperty*>(property)) &&
                     property->GetParentState() == state));
        };

    if ( state->m_itemsAdded )
    {
        PrepareAfterItemsAdded();
        if ( !transactionIsValid(p1) || (p2 && !transactionIsValid(p2)) )
            return;
    }

    wxRect r = GetPropertyRect(p1, p2);
    if ( !transactionIsValid(p1) || (p2 && !transactionIsValid(p2)) )
        return;
    if ( !r.IsEmpty() )
    {
        // Convert rectangle from logical grid coordinates to physical ones
        int vx, vy;
        GetViewStart(&vx, &vy);
        vx *= wxPG_PIXELS_PER_UNIT;
        vy *= wxPG_PIXELS_PER_UNIT;
        r.Offset(-vx, -vy);
        RefreshRect(r);
        if ( !transactionIsValid(p1) || (p2 && !transactionIsValid(p2)) )
            return;
        // A renderer can change the selection while it is being painted.
        // Keep its invalidation queued instead of recursively painting with
        // the same native drawing context (notably Qt's per-window QPainter).
        if ( !wxPGGetPropertyGridTransientState(this).drawingDepth )
            Update();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshProperty( wxPGProperty* p )
{
    wxCHECK_RET( p, wxS("invalid property id") );
    if ( !m_pState )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveRefresh = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveRefresh);

    const auto transactionIsValid = [weakThis, this, state, p]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               m_pState == state && !IsPropertyPendingRemoval(p) &&
               p->GetParentState() == state;
    };

    if ( state->DoIsPropertySelected(p) || p->IsChildSelected(true) )
    {
        // NB: We must copy the selection.
        wxArrayPGProperty selection = state->m_selection;
        wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Force;
        // We want to keep property's editor focused.
        if ( IsEditorFocused() )
            selFlags |= wxPGSelectPropertyFlags::Focus;
        DoSetSelection(selection, selFlags);
        if ( !transactionIsValid() )
            return;
    }

    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndValueRelated( wxPGProperty* p )
{
    wxCHECK_RET( p, wxS("invalid property id") );

    if ( IsFrozen() || !m_pState )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveDrawing = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveDrawing);

    const auto transactionIsValid =
        [weakThis, this, state](wxPGProperty* property)
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state && property &&
                   !IsPropertyPendingRemoval(property) &&
                   property->GetParentState() == state;
        };

    if ( !transactionIsValid(p) )
        return;

    // Draw item, children, and parent too, if it is not category
    wxPGProperty* parent = p->GetParent();

    while ( transactionIsValid(parent) &&
            !parent->IsCategory() && !parent->IsRoot() )
    {
         DrawItem(parent);
         if ( !transactionIsValid(p) || !transactionIsValid(parent) )
             return;
         parent = parent->GetParent();
    }

    if ( transactionIsValid(p) )
        DrawItemAndChildren(p);
}

void wxPropertyGrid::DrawItemAndChildren( wxPGProperty* p )
{
    wxCHECK_RET( p, wxS("invalid property id") );

    if ( !m_pState )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveDrawing = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveDrawing);

    const auto transactionIsValid = [weakThis, this, state, p]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               m_pState == state && !IsPropertyPendingRemoval(p) &&
               p->GetParentState() == state;
    };

    // Do not draw if in non-visible page
    if ( !transactionIsValid() )
        return;

    // do not draw a single item if multiple pending
    if ( m_pState->m_itemsAdded || IsFrozen() )
        return;

    // Update child control.
    wxPGProperty* selected = GetSelection();
    if ( selected && selected->GetParent() == p )
    {
        RefreshEditor();
        if ( !transactionIsValid() )
            return;
    }

    const wxPGProperty* lastDrawn = p->GetLastVisibleSubItem();
    if ( !transactionIsValid() ||
         (lastDrawn && IsPropertyPendingRemoval(
                           const_cast<wxPGProperty*>(lastDrawn))) )
    {
        return;
    }

    DrawItems(p, lastDrawn);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Refresh( bool WXUNUSED(eraseBackground),
                              const wxRect *rect )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return;
    PrepareAfterItemsAdded();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state )
        return;

    wxWindow::Refresh(false, rect);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state )
        return;

#if wxPG_REFRESH_CONTROLS
    // I think this really helps only GTK+1.2
    wxWindow* const editor = m_wndEditor;
    const wxWeakRef<wxWindow> weakEditor(editor);
    if ( editor )
        editor->Refresh();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state ||
         (editor &&
          (!wxWeakWindowIsAvailableForCallbacks(weakEditor, editor) ||
           m_wndEditor != editor)) )
    {
        return;
    }

    wxWindow* const secondaryEditor = m_wndEditor2;
    if ( secondaryEditor )
        secondaryEditor->Refresh();
#endif
}

// -----------------------------------------------------------------------
// wxPropertyGrid global operations
// -----------------------------------------------------------------------

void wxPropertyGrid::Clear()
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return;

    state->DoClear();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    m_propHover = nullptr;

    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    // Need to clear some area at the end
    if ( !IsFrozen() )
        RefreshRect(wxRect(0, 0, m_width, m_height));
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableCategories( bool enable )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state || !DoClearSelection() ||
         !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return false;
    }

    if ( enable )
    {
        //
        // Enable categories
        //

        m_windowStyle &= ~(wxPG_HIDE_CATEGORIES);
    }
    else
    {
        //
        // Disable categories
        //
        m_windowStyle |= wxPG_HIDE_CATEGORIES;
    }

    const bool categoriesChanged = state->EnableCategories(enable);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return false;
    }
    if ( !categoriesChanged )
        return false;

    if ( !IsFrozen() )
    {
        if ( m_windowStyle & wxPG_AUTO_SORT )
        {
            m_pState->m_itemsAdded = true; // force
            PrepareAfterItemsAdded();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return false;
            }
        }
    }
    else
        state->m_itemsAdded = true;

    // No need for RecalculateVirtualSize() here - it is already called in
    // wxPropertyGridPageState method above.

    Refresh();

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SwitchState( wxPropertyGridPageState* pNewState )
{
    wxASSERT( pNewState );
    wxASSERT( pNewState->GetGrid() );

    if ( pNewState == m_pState )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const oldState = m_pState;

    // A splitter transaction owns its page-state snapshot. End it before a
    // page switch can make that snapshot dangling (notably RemovePage()).
    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != oldState )
            return;
    }

    wxArrayPGProperty oldSelection = oldState->m_selection;

    // Call ClearSelection() instead of DoClearSelection()
    // so that selection clear events are not sent.
    if ( !ClearSelection() )
        return;
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != oldState )
        return;

    oldState->m_selection = oldSelection;

    bool orig_mode = oldState->IsInNonCatMode();
    bool new_state_mode = pNewState->IsInNonCatMode();

    m_pState = pNewState;
    wxPGGetPropertyGridTransientState(this).keyboardSplitter = -1;

    // Validate width
    int pgWidth = GetClientSize().x;
    if ( HasVirtualWidth() )
    {
        if ( pNewState->GetVirtualWidth() < pgWidth )
        {
            pNewState->m_width = pgWidth;
            pNewState->CheckColumnWidths();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != pNewState )
            {
                return;
            }
        }
    }
    else
    {
        //
        // Just in case, fully re-center splitter
        //if ( HasFlag( wxPG_SPLITTER_AUTO_CENTER ) )
        //    pNewState->m_fSplitterX = -1.0;

        pNewState->OnClientWidthChange(pgWidth,
                                       pgWidth - pNewState->GetVirtualWidth());
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != pNewState )
            return;
    }

    m_propHover = nullptr;

    // If necessary, convert state to correct mode.
    if ( orig_mode != new_state_mode )
    {
        // This should refresh as well.
        EnableCategories( orig_mode?false:true );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != pNewState )
        {
            return;
        }
    }
    else if ( !IsFrozen() )
    {
        // Refresh, if not frozen.
        m_pState->PrepareAfterItemsAdded();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != pNewState )
        {
            return;
        }

        // Reselect (Use SetSelection() instead of Do-variant so that
        // events won't be sent).
        SetSelection(m_pState->m_selection);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != pNewState )
        {
            return;
        }

        RecalculateVirtualSize(0);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != pNewState )
        {
            return;
        }
        Refresh();
    }
    else
        m_pState->m_itemsAdded = true;
}

// -----------------------------------------------------------------------

// Public column geometry mutations first retire any input transaction owning
// the old page/column snapshot.
void wxPropertyGrid::SetColumnCount( int colCount )
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    m_pState->SetColumnCount(colCount);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    wxPGGetPropertyGridTransientState(this).keyboardSplitter = -1;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSplitterPosition( int newXPos, int col )
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    DoSetSplitter(newXPos, col, wxPGSplitterPositionFlags::Refresh);
}

// -----------------------------------------------------------------------

// Call to SetSplitterPosition will always disable splitter auto-centering
// if parent window is shown.
void wxPropertyGrid::DoSetSplitter( int newxpos,
                                    int splitterIndex,
                                    wxPGSplitterPositionFlags flags )
{
    if ( newxpos < FromDIP(wxPG_DRAG_MARGIN) )
        return;

    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return;

    if ( !!(flags & wxPGSplitterPositionFlags::FromEvent) )
        state->m_dontCenterSplitter = true;

    const wxWeakRef<wxWindow> weakThis(this);
    state->DoSetSplitter(newxpos, splitterIndex, flags);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
        return;

    if ( !!(flags & wxPGSplitterPositionFlags::Refresh) )
    {
        if ( GetSelection() )
        {
            CorrectEditorWidgetSizeX();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
        }

        Refresh();
    }

    return;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColumnSizes( bool enableAutoResizing )
{
    if ( m_pState )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const state = m_pState;
        if ( m_dragStatus )
        {
            FinishSplitterDrag(true);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
        }

        state->ResetColumnSizes(wxPGSplitterPositionFlags::Null);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }

        if ( GetSelection() )
        {
            CorrectEditorWidgetSizeX();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
            {
                return;
            }
        }
        Refresh();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }

        if ( enableAutoResizing && HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
            state->m_dontCenterSplitter = false;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CenterSplitter( bool enableAutoResizing )
{
    const wxWeakRef<wxWindow> weakThis(this);
    SetSplitterPosition( m_width/2 );
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;
    if ( enableAutoResizing && HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
        m_pState->m_dontCenterSplitter = false;
}

// -----------------------------------------------------------------------
// wxPropertyGrid item iteration (GetNextProperty etc.) methods
// -----------------------------------------------------------------------

// Returns nearest paint visible property (such that will be painted unless
// window is scrolled or resized). If given property is paint visible, then
// it itself will be returned
wxPGProperty* wxPropertyGrid::GetNearestPaintVisible( wxPGProperty* p ) const
{
    int vy1;// Top left corner of client
    GetViewStart(nullptr,&vy1);
    vy1 *= wxPG_PIXELS_PER_UNIT;

    int vy2 = vy1 + m_height;
    int propY = p->GetY2(m_lineHeight);

    if ( (propY + m_lineHeight) < vy1 )
    {
    // Too high
        return DoGetItemAtY( vy1 );
    }
    else if ( propY > vy2 )
    {
    // Too low
        return DoGetItemAtY( vy2 );
    }

    // Itself paint visible
    return p;

}

// -----------------------------------------------------------------------
// Methods related to change in value, value modification and sending events
// -----------------------------------------------------------------------

bool wxPropertyGrid::IsPropertyPendingRemoval( wxPGProperty* property ) const
{
    if ( !property )
        return true;

    const auto containsProperty =
        [property](const std::set<wxPGProperty*>& properties)
        {
            for ( wxPGProperty* root : properties )
            {
                if ( property == root || property->IsSomeParent(root) )
                    return true;
            }
            return false;
        };

    return containsProperty(m_deletedProperties) ||
           containsProperty(m_removedProperties);
}

// -----------------------------------------------------------------------

// commits any changes in editor of selected property
// return true if validation did not fail
// flags are same as with DoSelectProperty
bool wxPropertyGrid::CommitChangesFromEditor(wxPGSelectPropertyFlags flags)
{
    // Committing already?
    if ( m_inCommitChangesFromEditor )
        return true;

    // PerformValidation()/DoPropertyChanged() own the single m_chgInfo
    // transaction. A commit started from one of their callbacks cannot apply a
    // second value safely, so reject it before touching the dirty state.
    if ( wxPGGetPropertyGridTransientState(this).inPerformValidation || m_inDoPropertyChanged )
        return false;

    // Don't do this if already processing editor event. It might
    // induce recursive dialogs and crap like that.
    if ( m_iFlags & wxPG_FL_IN_HANDLECUSTOMEDITOREVENT )
    {
        if ( m_inDoPropertyChanged )
            return true;

        return false;
    }

    wxPGProperty* selected = GetSelection();

    if ( m_wndEditor &&
         IsEditorsValueModified() &&
         (m_iFlags & wxPG_FL_INITIALIZED) &&
         selected )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const initialState = m_pState;
        wxWindow* const editorControl = m_wndEditor;
        wxWindow* const secondaryControl = m_wndEditor2;
        const wxWeakRef<wxWindow> weakEditorControl(editorControl);
        const wxWeakRef<wxWindow> weakSecondaryControl(secondaryControl);
        const auto propertyTransactionIsValid =
            [weakThis, this, initialState, selected]()
            {
                return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                       m_pState == initialState &&
                       GetSelection() == selected &&
                       selected->GetParentState() == initialState &&
                       !IsPropertyPendingRemoval(selected) &&
                       !selected->HasFlag(wxPGFlags::BeingDeleted);
            };
        const auto transactionIsValid =
            [weakThis, weakEditorControl, weakSecondaryControl,
             this, editorControl,
              secondaryControl, propertyTransactionIsValid]()
            {
                if ( !propertyTransactionIsValid() )
                    return false;

                bool editorsAreValid = true;
                if ( editorControl &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakEditorControl, editorControl)) )
                {
                    // A public editor/property callback may synchronously
                    // destroy the control. Retire the non-owning member before
                    // any later path (including grid destruction) can observe
                    // the dangling address.
                    if ( m_wndEditor == editorControl )
                        m_wndEditor = nullptr;
                    editorsAreValid = false;
                }
                if ( secondaryControl &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryControl, secondaryControl)) )
                {
                    if ( m_wndEditor2 == secondaryControl )
                        m_wndEditor2 = nullptr;
                    editorsAreValid = false;
                }

                return editorsAreValid &&
                       editorControl &&
                       wxWeakWindowIsAvailableForCallbacks(weakEditorControl,
                                                           editorControl) &&
                       m_wndEditor == editorControl &&
                       m_wndEditor2 == secondaryControl;
            };
        if ( !transactionIsValid() )
            return false;

        wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
        ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        m_inCommitChangesFromEditor = true;
        wxScopeGuard leaveCommit = wxMakeGuard([weakThis, this]()
        {
            if ( weakThis.get() == this )
            {
                m_inCommitChangesFromEditor = false;
                --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
            }
        });
        wxUnusedVar(leaveCommit);

        wxVariant variant(selected->GetValueRef());
        bool valueIsPending = false;

        // JACS - necessary to avoid new focus being found spuriously within OnIdle
        // due to another window getting focus
        const wxWeakRef<wxWindow> oldFocus(m_curFocused);

        bool validationFailure = false;
        bool forceSuccess = !!(flags & (wxPGSelectPropertyFlags::NoValidate | wxPGSelectPropertyFlags::Force));

        m_chgInfo_changedProperty = nullptr;

        // If truly modified, schedule value as pending.
        const std::uint64_t revisionBeforeReadingEditor =
            wxPGGetPropertyGridTransientState(this).editorModificationRevision;
        const wxPGEditor* const selectedEditor =
            selected->GetEditorClass();
        if ( !transactionIsValid() || !selectedEditor )
        {
            return false;
        }
        const bool gotValue =
            selectedEditor->GetValueFromControl(variant,
                                                selected,
                                                editorControl);
        if ( !transactionIsValid() )
            return false;

        if ( gotValue )
        {
            const bool editorIsValid = DoEditorValidate();
            if ( !transactionIsValid() )
                return false;

            bool valueIsValid = false;
            if ( editorIsValid )
            {
                valueIsValid = PerformValidation(selected, variant);
                if ( !transactionIsValid() )
                    return false;
            }

            if ( editorIsValid && valueIsValid )
            {
                valueIsPending = true;
            }
            else
            {
                validationFailure = true;
            }
        }
        else if ( wxPGGetPropertyGridTransientState(this).editorModificationRevision ==
                      revisionBeforeReadingEditor )
        {
            EditorsValueWasNotModified();
        }

        bool res = true;

        if ( validationFailure && !forceSuccess )
        {
            if ( oldFocus )
            {
                oldFocus->SetFocus();
                if ( !transactionIsValid() )
                    return false;
                m_curFocused = oldFocus;
            }

            res = OnValidationFailure(selected, variant);
            if ( !transactionIsValid() )
                return false;

            // Now prevent further validation failure messages
            if ( res &&
                 wxPGGetPropertyGridTransientState(this).editorModificationRevision == revisionBeforeReadingEditor )
            {
                EditorsValueWasNotModified();
                OnValidationFailureReset(selected);
                // Resetting a marked cell deliberately recreates its editor.
                // The reset routine sanitizes the old slots itself, and there
                // is no subsequent use of either captured control here.
                if ( !propertyTransactionIsValid() )
                    return false;
            }
        }
        else if ( valueIsPending )
        {
            const bool propertyChanged =
                DoPropertyChanged(selected, flags);
            // DoPropertyChanged() owns and validates the editor transaction it
            // uses. It can also legitimately replace the controls, so only
            // the enclosing property transaction must still be identical on
            // return.
            if ( !propertyChanged || !propertyTransactionIsValid() )
                return false;
            if ( wxPGGetPropertyGridTransientState(this).editorModificationRevision ==
                    revisionBeforeReadingEditor )
            {
                EditorsValueWasNotModified();
            }
        }

        return res;
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::PerformValidation( wxPGProperty* p, wxVariant& pendingValue,
                                        int flags )
{
    //
    // Runs all validation functionality.
    // Returns true if value passes all tests.
    //

    if ( wxPGGetPropertyGridTransientState(this).inPerformValidation )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPGGetPropertyGridTransientState(this).inPerformValidation = true;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leavePerformValidation = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
        {
            wxPGGetPropertyGridTransientState(this).inPerformValidation = false;
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leavePerformValidation);

    wxPropertyGridPageState* const initialState = m_pState;
    wxPropertyGridPageState* const initialPropertyState =
        p->GetParentState();
    const bool propertyWasSelected = GetSelection() == p;

    m_validationInfo.SetFailureBehavior(m_permanentValidationFailureBehavior);
    m_validationInfo.SetFailing(true);

    //
    // Variant list a special value that cannot be validated
    // by normal means.
    if ( !pendingValue.IsType(wxPG_VARIANT_TYPE_LIST) )
    {
        const bool valid = p->ValidateValue(pendingValue, m_validationInfo);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != initialState ||
             p->GetParentState() != initialPropertyState ||
             IsPropertyPendingRemoval(p) ||
             (propertyWasSelected && GetSelection() != p) )
        {
            return false;
        }
        if ( !valid )
            return false;
    }

    //
    // Adapt list to child values, if necessary
    wxVariant* pPendingValue = &pendingValue;
    wxVariant* pList = nullptr;

    // If parent has wxPGFlags::Aggregate flag, or uses composite
    // string value, then we need treat as it was changed instead
    // (or, in addition, as is the case with composite string parent).
    // This includes creating list variant for child values.

    wxPGProperty* pwc = p->GetParent();
    wxPGProperty* changedProperty = p;
    wxPGProperty* baseChangedProperty = changedProperty;
    wxVariant bcpPendingList;

    wxVariant listValue = pendingValue;
    listValue.SetName(p->GetBaseName());

    while ( pwc &&
            (pwc->HasFlag(wxPGFlags::Aggregate) || pwc->HasFlag(wxPGFlags::ComposedValue)) )
    {
        wxVariantList tempList;
        wxVariant lv(tempList, pwc->GetBaseName());
        lv.Append(listValue);
        listValue = lv;
        pPendingValue = &listValue;

        if ( pwc->HasFlag(wxPGFlags::Aggregate) )
        {
            baseChangedProperty = pwc;
            bcpPendingList = lv;
        }

        changedProperty = pwc;
        pwc = pwc->GetParent();
    }

    wxVariant value;
    wxPGProperty* evtChangingProperty = changedProperty;

    if ( !pPendingValue->IsType(wxPG_VARIANT_TYPE_LIST) )
    {
        value = *pPendingValue;
    }
    else
    {
        // Convert list to child values
        pList = pPendingValue;
        changedProperty->AdaptListToValue( *pPendingValue, &value );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != initialState ||
             p->GetParentState() != initialPropertyState ||
             IsPropertyPendingRemoval(p) ||
             (propertyWasSelected && GetSelection() != p) )
        {
            return false;
        }
    }

    wxVariant evtChangingValue = value;

    if ( flags & SendEvtChanging )
    {
        // FIXME: After proper ValueToString()s added, remove
        // this. It is just a temporary fix, as evt_changing
        // will simply not work for wxPGFlags::ComposedValue
        // (unless it is selected, and textctrl editor is open).
        if ( changedProperty->HasFlag(wxPGFlags::ComposedValue) )
        {
            evtChangingProperty = baseChangedProperty;
            if ( evtChangingProperty != p )
            {
                evtChangingProperty->AdaptListToValue( bcpPendingList, &evtChangingValue );
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                     m_pState != initialState ||
                     p->GetParentState() != initialPropertyState ||
                     IsPropertyPendingRemoval(p) ||
                     (propertyWasSelected && GetSelection() != p) )
                {
                    return false;
                }
            }
            else
            {
                evtChangingValue = pendingValue;
            }
        }

        if ( evtChangingProperty->HasFlag(wxPGFlags::ComposedValue) )
        {
            if ( changedProperty == GetSelection() )
            {
                wxWindow* editor = GetEditorControl();
                wxASSERT( wxDynamicCast(editor, wxTextCtrl) );
                evtChangingValue = wxStaticCast(editor, wxTextCtrl)->GetValue();
            }
            else
            {
                wxLogDebug(wxS("WARNING: wxEVT_PG_CHANGING is about to happen with old value."));
            }
        }
    }

    wxASSERT( m_chgInfo_changedProperty == nullptr );
    m_chgInfo_changedProperty = changedProperty;
    m_chgInfo_baseChangedProperty = baseChangedProperty;
    m_chgInfo_pendingValue = value;

    if ( pList )
        m_chgInfo_valueList = *pList;
    else
        m_chgInfo_valueList.MakeNull();

    // If changedProperty is not property which value was edited,
    // then call wxPGProperty::ValidateValue() for that as well.
    if ( p != changedProperty && !value.IsType(wxPG_VARIANT_TYPE_LIST) )
    {
        const bool valid =
            changedProperty->ValidateValue(value, m_validationInfo);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != initialState ||
             p->GetParentState() != initialPropertyState ||
             IsPropertyPendingRemoval(p) ||
             (propertyWasSelected && GetSelection() != p) )
        {
            return false;
        }
        if ( !valid )
            return false;
    }

    if ( flags & SendEvtChanging )
    {
        // SendEvent returns true if event was vetoed
        const bool vetoed =
            SendEvent(wxEVT_PG_CHANGING, evtChangingProperty,
                      &evtChangingValue);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != initialState ||
             p->GetParentState() != initialPropertyState ||
             IsPropertyPendingRemoval(p) ||
             (propertyWasSelected && GetSelection() != p) )
        {
            return false;
        }

        if ( IsPropertyPendingRemoval(p) ||
             IsPropertyPendingRemoval(changedProperty) ||
             IsPropertyPendingRemoval(evtChangingProperty) )
        {
            return false;
        }

        if ( vetoed )
            return false;
    }

    if ( flags & IsStandaloneValidation )
    {
        // If called in 'generic' context, we need to reset
        // m_chgInfo_changedProperty and write back translated value.
        m_chgInfo_changedProperty = nullptr;
        pendingValue = value;
    }

    m_validationInfo.SetFailing(false);

    return true;
}

// -----------------------------------------------------------------------

#if wxUSE_STATUSBAR
wxStatusBar* wxPropertyGrid::GetStatusBar()
{
    wxWindow* topWnd = ::wxGetTopLevelParent(this);
    wxFrame* frame = wxDynamicCast(topWnd, wxFrame);
    if ( frame )
    {
        return frame->GetStatusBar();
    }
    return nullptr;
}
#endif

// -----------------------------------------------------------------------

void wxPropertyGrid::DoShowPropertyError( wxPGProperty* WXUNUSED(property), const wxString& msg )
{
    if ( msg.empty() )
        return;

#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxStatusBar* pStatusBar = GetStatusBar();
        if ( pStatusBar )
        {
            pStatusBar->SetStatusText(msg);
            return;
        }
    }
#endif

    /* TRANSLATORS: Caption of message box displaying any property error */
    ::wxMessageBox(msg, _("Property Error"));
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoHidePropertyError( wxPGProperty* WXUNUSED(property) )
{
#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxStatusBar* pStatusBar = GetStatusBar();
        if ( pStatusBar )
        {
            pStatusBar->SetStatusText(wxString());
            return;
        }
    }
#endif
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnValidationFailure( wxPGProperty* property,
                                          wxVariant& invalidValue )
{
    if ( m_inOnValidationFailure )
        return true;

    const wxWeakRef<wxWindow> weakThis(this);
    const bool propertyWasSelected = GetSelection() == property;
    wxPropertyGridPageState* const propertyState =
        property ? property->GetParentState() : nullptr;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    m_inOnValidationFailure = true;
    wxScopeGuard leaveValidationFailure = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
        {
            m_inOnValidationFailure = false;
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leaveValidationFailure);

    wxWindow* const editor = m_wndEditor;
    wxWindow* const secondaryEditor = m_wndEditor2;
    const wxWeakRef<wxWindow> weakEditor(editor);
    const wxWeakRef<wxWindow> weakSecondaryEditor(secondaryEditor);
    const auto validationTransactionIsValid =
        [weakThis, weakEditor, weakSecondaryEditor, this, property,
         propertyState, propertyWasSelected, editor, secondaryEditor]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            bool editorsAreValid = true;
            if ( editor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakEditor, editor)) )
            {
                if ( m_wndEditor == editor )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( secondaryEditor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryEditor, secondaryEditor)) )
            {
                if ( m_wndEditor2 == secondaryEditor )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }

            return editorsAreValid &&
                   !wxWindowIsUnavailableForCallbacks(this) &&
                   property &&
                   property->GetParentState() == propertyState &&
                   !IsPropertyPendingRemoval(property) &&
                   (!propertyWasSelected || GetSelection() == property) &&
                   m_wndEditor == editor &&
                   m_wndEditor2 == secondaryEditor;
        };
    wxPGVFBFlags vfb = m_validationInfo.GetFailureBehavior();

    if ( m_inDoSelectProperty )
    {
        // When property selection is being changed, do not display any
        // messages, if some were already shown for this property.
        if ( property->HasFlag(wxPGFlags::InvalidValue) )
        {
            m_validationInfo.SetFailureBehavior(
                vfb & (~(wxPGVFBFlags::ShowMessage |
                        wxPGVFBFlags::ShowMessageBox |
                        wxPGVFBFlags::ShowMessageOnStatusBar)));
        }
    }

    // First call property's handler
    property->OnValidationFailure(invalidValue);
    if ( !validationTransactionIsValid() )
    {
        return true;
    }

    bool res = DoOnValidationFailure(property, invalidValue);
    if ( !validationTransactionIsValid() )
    {
        return res;
    }

    //
    // For non-wxTextCtrl editors, we do need to revert the value
    if ( !wxDynamicCast(editor, wxTextCtrl) &&
         property == GetSelection() )
    {
        const wxPGEditor* const propertyEditor =
            property->GetEditorClass();
        if ( !validationTransactionIsValid() || !propertyEditor || !editor )
        {
            return res;
        }
        propertyEditor->UpdateControl(property, editor);
        if ( !validationTransactionIsValid() )
        {
            return res;
        }
    }

    property->SetFlag(wxPGFlags::InvalidValue);

    return res;
}

void wxPropertyGrid::OnValidationFailureReset( wxPGProperty* property )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxWindow* const primary = m_wndEditor;
    wxWindow* const secondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakPrimary(primary);
    const wxWeakRef<wxWindow> weakSecondary(secondary);
    const auto sanitizeEditorsAndGridIsValid =
        [weakThis, weakPrimary, weakSecondary, this, primary, secondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            if ( primary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPrimary, primary)) )
            {
                if ( m_wndEditor == primary )
                    m_wndEditor = nullptr;
            }
            if ( secondary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondary, secondary)) )
            {
                if ( m_wndEditor2 == secondary )
                    m_wndEditor2 = nullptr;
            }

            // RefreshProperty() below intentionally recreates the controls.
            // A changed slot is therefore valid here; the captured weak refs
            // are only needed to retire an old dangling non-owning member.
            return !wxWindowIsUnavailableForCallbacks(this);
        };
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveValidationReset = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveValidationReset);

    if ( property && property->HasFlag(wxPGFlags::InvalidValue) )
    {
        // Clear before invoking the overridable hook: it may synchronously
        // destroy the grid or remove this property.
        property->ClearFlag(wxPGFlags::InvalidValue);
        DoOnValidationFailureReset(property);
        if ( !sanitizeEditorsAndGridIsValid() )
            return;
    }

    m_validationInfo.ClearFailureMessage();
}

bool wxPropertyGrid::DoOnValidationFailure( wxPGProperty* property, wxVariant& WXUNUSED(invalidValue) )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const initialState = m_pState;
    const bool propertyWasSelected = GetSelection() == property;
    wxWindow* const editor = m_wndEditor;
    wxWindow* const secondaryEditor = m_wndEditor2;
    const wxWeakRef<wxWindow> weakEditor(editor);
    const wxWeakRef<wxWindow> weakSecondaryEditor(secondaryEditor);
    const auto transactionIsValid =
        [weakThis, weakEditor, weakSecondaryEditor, this, initialState,
         property, propertyWasSelected, editor, secondaryEditor]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            bool editorsAreValid = true;
            if ( editor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakEditor, editor)) )
            {
                if ( m_wndEditor == editor )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( secondaryEditor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryEditor, secondaryEditor)) )
            {
                if ( m_wndEditor2 == secondaryEditor )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }

            return editorsAreValid &&
                   !wxWindowIsUnavailableForCallbacks(this) &&
                   m_pState == initialState &&
                   property &&
                   property->GetParentState() == initialState &&
                   !IsPropertyPendingRemoval(property) &&
                   (!propertyWasSelected || GetSelection() == property) &&
                   m_wndEditor == editor &&
                   m_wndEditor2 == secondaryEditor;
        };
    wxPGVFBFlags vfb = m_validationInfo.GetFailureBehavior();

    if ( !transactionIsValid() )
        return false;

    if ( !!(vfb & wxPGVFBFlags::Beep) )
    {
        ::wxBell();
        if ( !transactionIsValid() )
            return false;
    }

    if ( !!(vfb & wxPGVFBFlags::MarkCell) &&
         !property->HasFlag(wxPGFlags::InvalidValue) )
    {
        unsigned int colCount = m_pState->GetColumnCount();

        // We need backup marked property's cells
        m_propCellsBackup = property->m_cells;

        wxColour vfbFg = *wxWHITE;
        wxColour vfbBg = *wxRED;

        property->EnsureCells(colCount);

        for ( unsigned int i=0; i<colCount; i++ )
        {
            wxPGCell& cell = property->m_cells[i];
            cell.SetFgCol(vfbFg);
            cell.SetBgCol(vfbBg);
        }

        if ( property == GetSelection() )
        {
            SetInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

            if ( editor )
            {
                editor->SetForegroundColour(vfbFg);
                if ( !transactionIsValid() )
                    return false;
                editor->SetBackgroundColour(vfbBg);
                if ( !transactionIsValid() )
                    return false;
            }
        }

        DrawItemAndChildren(property);
        if ( !transactionIsValid() )
            return false;
    }

    if ( !!(vfb & (wxPGVFBFlags::ShowMessage |
                wxPGVFBFlags::ShowMessageBox |
                wxPGVFBFlags::ShowMessageOnStatusBar)) )
    {
        wxString msg = m_validationInfo.GetFailureMessage();

        if ( msg.empty() )
            msg = _("You have entered invalid value. Press ESC to cancel editing.");

    #if wxUSE_STATUSBAR
        if ( !!(vfb & wxPGVFBFlags::ShowMessageOnStatusBar) )
        {
            if ( !wxPGGlobalVars->m_offline )
            {
                wxStatusBar* pStatusBar = GetStatusBar();
                if ( pStatusBar )
                {
                    pStatusBar->SetStatusText(msg);
                    if ( !transactionIsValid() )
                        return false;
                }
            }
        }
    #endif

        // Displaying error dialog box can cause (native) focus changes
        // so let's preserve the current focus in order to restore it afterwards.
        const wxWeakRef<wxWindow> focusedWnd(wxWindow::FindFocus());

        if ( !!(vfb & wxPGVFBFlags::ShowMessage) )
        {
            DoShowPropertyError(property, msg);
            if ( !transactionIsValid() )
                return false;
        }

        if ( !!(vfb & wxPGVFBFlags::ShowMessageBox) )
        {
            /* TRANSLATORS: Caption of message box displaying any property error */
            ::wxMessageBox(msg, _("Property Error"));
            if ( !transactionIsValid() )
                return false;
        }

        // Restore the focus
        if ( focusedWnd )
        {
            if ( !transactionIsValid() )
                return false;
            focusedWnd->SetFocus();
            if ( !transactionIsValid() )
                return false;
        }
    }

    return !(vfb & wxPGVFBFlags::StayInProperty);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoOnValidationFailureReset( wxPGProperty* property )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPGVFBFlags vfb = m_validationInfo.GetFailureBehavior();

    if ( !!(vfb & wxPGVFBFlags::MarkCell) )
    {
        // Revert cells
        property->m_cells = m_propCellsBackup;

        ClearInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

        if ( property == GetSelection() && GetEditorControl() )
        {
            // Calling this will recreate the control, thus resetting its colour
            RefreshProperty(property);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
        }
        else
        {
            DrawItemAndChildren(property);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
        }
    }

#if wxUSE_STATUSBAR
    if ( !!(vfb & wxPGVFBFlags::ShowMessageOnStatusBar) )
    {
        if ( !wxPGGlobalVars->m_offline )
        {
            wxStatusBar* pStatusBar = GetStatusBar();
            if ( pStatusBar )
                pStatusBar->SetStatusText(wxString());
        }
    }
#endif

    if ( !!(vfb & wxPGVFBFlags::ShowMessage) )
    {
        DoHidePropertyError(property);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    m_validationInfo.SetFailing(false);
}

// -----------------------------------------------------------------------

// flags are same as with DoSelectProperty
bool wxPropertyGrid::DoPropertyChanged( wxPGProperty* p, wxPGSelectPropertyFlags selFlags )
{
    if ( m_inDoPropertyChanged )
        return true;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const initialState = m_pState;
    // ChangePropertyValue() also accepts properties from a manager page that
    // isn't displayed. Track their owner independently from the visible page.
    wxPropertyGridPageState* const initialPropertyState = p->GetParentState();
    const bool propertyWasSelected = GetSelection() == p;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    m_inDoPropertyChanged = true;
    wxScopeGuard leavePropertyChanged = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
        {
            m_inDoPropertyChanged = false;
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leavePropertyChanged);

    wxPGProperty* selected = GetSelection();

    initialPropertyState->m_anyModified = true;

    // Maybe need to update control
    wxASSERT( m_chgInfo_changedProperty != nullptr );

    // These values were calculated in PerformValidation()
    wxPGProperty* changedProperty = m_chgInfo_changedProperty;
    wxVariant value = m_chgInfo_pendingValue;
    const auto propertyTransactionIsValid =
        [this, p, changedProperty, initialState, initialPropertyState,
         propertyWasSelected, &weakThis]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == initialState &&
                   p &&
                   changedProperty &&
                   p->GetParentState() == initialPropertyState &&
                   changedProperty->GetParentState() == initialPropertyState &&
                   !IsPropertyPendingRemoval(p) &&
                   !IsPropertyPendingRemoval(changedProperty) &&
                   (!propertyWasSelected || GetSelection() == p);
        };

    // If property's value is being changed, assume it is valid
    OnValidationFailureReset(selected);
    if ( !propertyTransactionIsValid() )
        return false;

    wxWindow* const editor = m_wndEditor;
    wxWindow* const secondaryEditor = m_wndEditor2;
    const wxWeakRef<wxWindow> weakEditor(editor);
    const wxWeakRef<wxWindow> weakSecondaryEditor(secondaryEditor);
    const auto transactionIsValid =
        [propertyTransactionIsValid, weakEditor, weakSecondaryEditor,
         this, editor, secondaryEditor]()
        {
            if ( !propertyTransactionIsValid() )
                return false;

            bool editorsAreValid = true;
            if ( editor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakEditor, editor)) )
            {
                if ( m_wndEditor == editor )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( secondaryEditor &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryEditor, secondaryEditor)) )
            {
                if ( m_wndEditor2 == secondaryEditor )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }
            return editorsAreValid &&
                   m_wndEditor == editor &&
                   m_wndEditor2 == secondaryEditor;
        };

    changedProperty->SetValue(value, &m_chgInfo_valueList, wxPGSetValueFlags::ByUser);
    if ( !transactionIsValid() )
        return false;

    // Set as Modified (not if dragging just began)
    if ( !p->HasFlag(wxPGFlags::Modified) )
    {
        p->SetFlag(wxPGFlags::Modified);
        if ( p == selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
        {
            if ( editor )
            {
                SetCurControlBoldFont();
                if ( !transactionIsValid() )
                    return false;
            }
        }
    }

    // Propagate updates to parent(s)
    wxPGProperty* topPaintedProperty = changedProperty->GetMainParent();
    wxPGProperty* pwc = p;
    wxPGProperty* prevPwc = nullptr;

    while ( prevPwc != topPaintedProperty )
    {
        pwc->SetFlag(wxPGFlags::Modified);

        if ( pwc == selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
        {
            if ( editor )
            {
                SetCurControlBoldFont();
                if ( !transactionIsValid() )
                    return false;
            }
        }

        prevPwc = pwc;
        pwc = pwc->GetParent();
    }

    // Draw the actual property
    DrawItemAndChildren( topPaintedProperty );
    if ( !transactionIsValid() )
        return false;

    //
    // If value was set by wxPGProperty::OnEvent, then update the editor
    // control.
    if ( !!(selFlags & wxPGSelectPropertyFlags::DialogVal) )
    {
        RefreshEditor();
        if ( !transactionIsValid() )
            return false;
    }
    else
    {
#if wxPG_REFRESH_CONTROLS
        if ( editor )
        {
            editor->Refresh();
            if ( !transactionIsValid() )
                return false;
        }
        if ( secondaryEditor )
        {
            secondaryEditor->Refresh();
            if ( !transactionIsValid() )
                return false;
        }
#endif
    }
    if ( !transactionIsValid() )
        return false;

    // Sanity check
    wxPGProperty* const changedParent = changedProperty->GetParent();
    if ( !changedParent || IsPropertyPendingRemoval(changedParent) )
        return false;
    wxASSERT( !changedParent->HasFlag(wxPGFlags::Aggregate) );

    // If top parent has composite string value, then send to child parents,
    // starting from baseChangedProperty.
    if ( changedProperty->HasFlag(wxPGFlags::ComposedValue) )
    {
        pwc = m_chgInfo_baseChangedProperty;

        while ( pwc != changedProperty )
        {
            wxPGProperty* const parentBeforeEvent = pwc->GetParent();
            SendEvent( wxEVT_PG_CHANGED, pwc, nullptr );
            if ( !transactionIsValid() ||
                 IsPropertyPendingRemoval(pwc) ||
                 !parentBeforeEvent ||
                 IsPropertyPendingRemoval(parentBeforeEvent) ||
                 pwc->GetParent() != parentBeforeEvent )
            {
                return false;
            }
            pwc = parentBeforeEvent;
        }
    }

    SendEvent( wxEVT_PG_CHANGED, changedProperty, nullptr );
    if ( !transactionIsValid() )
    {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ChangePropertyValue( wxPGPropArg id, wxVariant newValue )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    if ( m_inDoPropertyChanged || wxPGGetPropertyGridTransientState(this).inPerformValidation )
        return false;

    m_chgInfo_changedProperty = nullptr;

    if ( PerformValidation(p, newValue) )
    {
        DoPropertyChanged(p);
        return true;
    }
    else
    {
        OnValidationFailure(p, newValue);
    }

    return false;
}

// -----------------------------------------------------------------------

wxVariant wxPropertyGrid::GetUncommittedPropertyValue()
{
    wxPGProperty* prop = GetSelectedProperty();

    if ( !prop )
        return wxVariant();

    wxTextCtrl* tc = GetEditorTextCtrl();
    wxVariant value = prop->GetValue();

    if ( !tc || !IsEditorsValueModified() )
        return value;

#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    if ( !prop->StringToValueWithCheck(value, tc->GetValue()) )
#else
    if ( !prop->StringToValue(value, tc->GetValue()) )
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
        return value;

    if ( !PerformValidation(prop, value, IsStandaloneValidation) )
        return prop->GetValue();

    return value;
}

// -----------------------------------------------------------------------

// Runs wxValidator for the selected property
bool wxPropertyGrid::DoEditorValidate()
{
#if wxUSE_VALIDATORS
    if ( m_validatingEditor )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    m_validatingEditor = 1;
    wxScopeGuard leaveValidation = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
        {
            m_validatingEditor = 0;
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leaveValidation);

    m_validationInfo.SetFailureBehavior(m_permanentValidationFailureBehavior);
    m_validationInfo.SetFailing(true);

    wxPGProperty* selected = GetSelection();
    if ( selected )
    {
        wxPropertyGridPageState* const initialState = m_pState;
        wxWindow* const wnd = m_wndEditor;
        wxWindow* const secondaryWnd = m_wndEditor2;
        const wxWeakRef<wxWindow> weakEditor(wnd);
        const wxWeakRef<wxWindow> weakSecondaryEditor(secondaryWnd);
        const auto transactionIsValid =
            [weakThis, weakEditor, weakSecondaryEditor,
             this, initialState, selected, wnd, secondaryWnd]()
            {
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return false;

                bool editorsAreValid = true;
                if ( wnd &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakEditor, wnd)) )
                {
                    if ( m_wndEditor == wnd )
                        m_wndEditor = nullptr;
                    editorsAreValid = false;
                }
                if ( secondaryWnd &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryEditor, secondaryWnd)) )
                {
                    if ( m_wndEditor2 == secondaryWnd )
                        m_wndEditor2 = nullptr;
                    editorsAreValid = false;
                }

                return editorsAreValid &&
                       !wxWindowIsUnavailableForCallbacks(this) &&
                       m_pState == initialState &&
                       GetSelection() == selected &&
                       selected->GetParentState() == initialState &&
                       !IsPropertyPendingRemoval(selected) &&
                       !selected->HasFlag(wxPGFlags::BeingDeleted) &&
                       m_wndEditor == wnd &&
                       m_wndEditor2 == secondaryWnd;
            };

        if ( !transactionIsValid() )
            return false;

        wxValidator* validator = selected->GetValidator();
        if ( !transactionIsValid() )
            return false;
        if ( validator && wnd )
        {
            validator->SetWindow(wnd);
            if ( !transactionIsValid() )
                return false;
            const bool validationSucceeded = validator->Validate(this);
            if ( !transactionIsValid() )
                return false;
            if ( !validationSucceeded )
                return false;
        }
    }

    m_validationInfo.SetFailing(false);
#endif
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleCustomEditorEvent( wxEvent &event )
{
    //
    // NB: We should return true if the event was recognized as
    //     a dedicated wxPropertyGrid event, and as such was
    //     either properly handled or ignored.
    //

    // It is possible that this handler receives event even before
    // the control has been properly initialized. Let's skip the
    // event handling in that case.
    if ( !m_pState )
        return false;

    // Don't care about the event if it originated from the
    // 'label editor'. In this function we only care about the
    // property value editor.
    if ( m_labelEditor && event.GetId() == m_labelEditor->GetId() )
    {
        event.Skip();
        return true;
    }

    wxPropertyGridPageState* const editorState = m_pState;
    wxPGProperty* selected = GetSelection();

    // Somehow, event is handled after property has been deselected.
    // Possibly, but very rare.
    if ( !selected ||
          selected->HasFlag(wxPGFlags::BeingDeleted) ||
          m_inOnValidationFailure ||
          // Also don't handle editor event if wxEVT_PG_CHANGED or
          // similar is currently doing something (showing a
          // message box, for instance).
          m_processedEvent )
        return true;

    if ( m_iFlags & wxPG_FL_IN_HANDLECUSTOMEDITOREVENT )
        return true;

    wxVariant pendingValue(selected->GetValueRef());
    wxWindow* wnd = GetEditorControl();
    wxWindow* editorWnd = wxDynamicCast(event.GetEventObject(), wxWindow);
    wxWindow* const secondaryWnd = GetEditorControlSecondary();
    wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null;
    bool wasUnspecified = selected->IsValueUnspecified();
    int usesAutoUnspecified = selected->UsesAutoUnspecified();
    bool valueIsPending = false;
    const std::uint64_t revisionBeforeHandlingEvent =
        wxPGGetPropertyGridTransientState(this).editorModificationRevision;
    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakWnd(wnd);
    const wxWeakRef<wxWindow> weakEditorWnd(editorWnd);
    const wxWeakRef<wxWindow> weakSecondaryWnd(secondaryWnd);
    const auto editorTransactionIsValid =
        [this, editorState, selected, wnd, editorWnd, secondaryWnd,
         &weakThis, &weakWnd, &weakEditorWnd, &weakSecondaryWnd]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != editorState )
                return false;

            if ( wnd &&
                 !wxWeakWindowIsAvailableForCallbacks(weakWnd, wnd) )
            {
                if ( m_wndEditor == wnd )
                    m_wndEditor = nullptr;
                return false;
            }
            if ( secondaryWnd &&
                 !wxWeakWindowIsAvailableForCallbacks(weakSecondaryWnd,
                                                      secondaryWnd) )
            {
                if ( m_wndEditor2 == secondaryWnd )
                    m_wndEditor2 = nullptr;
                return false;
            }

            return GetSelection() == selected &&
                   !IsPropertyPendingRemoval(selected) &&
                   (!editorWnd ||
                    wxWeakWindowIsAvailableForCallbacks(weakEditorWnd,
                                                        editorWnd)) &&
                   GetEditorControl() == wnd &&
                   GetEditorControlSecondary() == secondaryWnd;
        };

    m_chgInfo_changedProperty = nullptr;

    m_iFlags &= ~wxPG_FL_VALUE_CHANGE_IN_EVENT;

    //
    // Ignore focus changes within the composite editor control
    if ( event.GetEventType() == wxEVT_SET_FOCUS || event.GetEventType() == wxEVT_KILL_FOCUS )
    {
        wxFocusEvent* fevt = wxDynamicCast(&event, wxFocusEvent);
        wxWindow* win = fevt->GetWindow();
        while ( win )
        {
            if ( win == wnd )
            {
                event.Skip();
                return true;
            }

            win = win->GetParent();
        }
    }
    // Filter out excess wxTextCtrl modified events
    else if ( event.GetEventType() == wxEVT_TEXT && wnd )
    {
        if ( wxDynamicCast(wnd, wxTextCtrl) )
        {
            wxTextCtrl* tc = (wxTextCtrl*) wnd;

            wxString newTcValue = tc->GetValue();
            if ( m_prevTcValue == newTcValue )
                return true;
            m_prevTcValue = newTcValue;
        }
        else if ( wxDynamicCast(wnd, wxComboCtrl) )
        {
            // In some cases we might stumble unintentionally on
            // wxComboCtrl's embedded wxTextCtrl's events. Let's
            // avoid them.
            if ( wxDynamicCast(editorWnd, wxTextCtrl) )
                return false;

            wxComboCtrl* cc = (wxComboCtrl*) wnd;

            wxString newTcValue = cc->GetTextCtrl()->GetValue();
            if ( m_prevTcValue == newTcValue )
                return true;
            m_prevTcValue = newTcValue;
        }
    }

    SetInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveCustomEditorEvent = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
        {
            ClearInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leaveCustomEditorEvent);

    bool validationFailure = false;
    bool buttonWasHandled = false;
    bool result = false;

    //
    // Try common button handling
    if ( IsMainButtonEvent(event) )
    {
        wxPGEditorDialogAdapter* adapter = selected->GetEditorDialog();
        if ( !editorTransactionIsValid() )
        {
            return result;
        }

        if ( adapter )
        {
            buttonWasHandled = true;
            // Store as res2, as previously (and still currently alternatively)
            // dialogs can be shown by handling wxEVT_BUTTON
            // in wxPGProperty::OnEvent().
            adapter->ShowDialog( this, selected );
            delete adapter;
            if ( !editorTransactionIsValid() )
                return result;
        }
    }

    if ( !buttonWasHandled )
    {
        if ( wnd || m_wndEditor2 )
        {
            // First call editor class' event handler.
            const wxPGEditor* editor = selected->GetEditorClass();
            if ( !editor || !editorTransactionIsValid() )
            {
                return result;
            }

            const bool editorHandled =
                editor->OnEvent(this, selected, editorWnd, event);
            if ( !editorTransactionIsValid() )
                return result;

            if ( editorHandled )
            {
                result = true;

                // If changes, validate them
                const bool editorIsValid = DoEditorValidate();
                if ( !editorTransactionIsValid() )
                    return result;

                if ( editorIsValid )
                {
                    const bool gotValue =
                        editor->GetValueFromControl(pendingValue,
                                                    selected,
                                                    wnd);
                    if ( !editorTransactionIsValid() )
                    {
                        return result;
                    }
                    if ( gotValue )
                        valueIsPending = true;

                    // Mark value always as pending if validation is currently
                    // failing and value was not unspecified
                    if ( !valueIsPending &&
                         !pendingValue.IsNull() &&
                         m_validationInfo.IsFailing() )
                         valueIsPending = true;
                }
                else
                {
                    validationFailure = true;
                }
            }
        }

        // Then the property's custom handler (must be always called, unless
        // validation failed).
        if ( !validationFailure )
        {
            if ( !editorTransactionIsValid() )
                return result;

            buttonWasHandled = selected->OnEvent( this, editorWnd, event );
            if ( !editorTransactionIsValid() )
                return result;
        }
    }

    // SetValueInEvent(), as called in one of the functions referred above
    // overrides editor's value.
    if ( WasValueChangedInEvent() )
    {
        valueIsPending = true;
        pendingValue = m_changeInEventValue;
        selFlags |= wxPGSelectPropertyFlags::DialogVal;
    }

    if ( !validationFailure && valueIsPending )
    {
        if ( !PerformValidation(selected, pendingValue) )
            validationFailure = true;
        if ( !editorTransactionIsValid() )
            return result;
    }

    if ( validationFailure)
    {
        OnValidationFailure(selected, pendingValue);
        if ( !editorTransactionIsValid() )
            return result;
    }
    else if ( valueIsPending )
    {
        selFlags |= ( !wasUnspecified && selected->IsValueUnspecified() && usesAutoUnspecified )
                                                 ? wxPGSelectPropertyFlags::SetUnspec : wxPGSelectPropertyFlags::Null;

        DoPropertyChanged(selected, selFlags);
        if ( !editorTransactionIsValid() )
            return result;
        if ( wxPGGetPropertyGridTransientState(this).editorModificationRevision == revisionBeforeHandlingEvent )
            EditorsValueWasNotModified();

        // Regardless of editor type, unfocus editor on
        // text-editing related enter press.
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }
    }
    else
    {
        // No value after all

        // Regardless of editor type, unfocus editor on
        // text-editing related enter press.
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }

        // Let unhandled button click events go to the parent
        if ( !buttonWasHandled && event.GetEventType() == wxEVT_BUTTON )
        {
            result = true;
            wxCommandEvent evt(wxEVT_BUTTON,GetId());
            GetEventHandler()->AddPendingEvent(evt);
        }
    }

    return result;
}

// -----------------------------------------------------------------------
// wxPropertyGrid editor control helper methods
// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetEditorWidgetRect( wxPGProperty* p,
                                            int column ) const
{
    wxRect rect;
    wxPropertyGrid* const self = const_cast<wxPropertyGrid*>(this);
    self->TryGetEditorWidgetRect(p, column, &rect);
    return rect;
}

bool wxPropertyGrid::TryGetEditorWidgetRect( wxPGProperty* p,
                                             int column,
                                             wxRect* rect )
{
    wxCHECK_MSG( p && rect && m_pState, false,
                 wxS("invalid property editor geometry request") );

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* const selection = GetSelection();

    int itemy = p->GetY2(m_lineHeight);
    int splitterX = state->DoGetSplitterPosition(column-1);
    int colEnd = splitterX + state->GetColumnWidth(column);
    int imageOffset = 0;

    int vx, vy;  // Top left corner of client
    GetViewStart(&vx, &vy);
    vx *= wxPG_PIXELS_PER_UNIT;
    vy *= wxPG_PIXELS_PER_UNIT;

    if ( column == 1 )
    {
        // TODO: If custom image detection changes from current, change this.
        if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
        {
            // OnMeasureImage() is virtual application code. Keep a property
            // removed by it alive until this transaction has unwound, and do
            // not touch either the property or the page after invalidation.
            wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
            ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
            wxScopeGuard leaveMeasureImage =
                wxMakeGuard([weakThis, this]()
                {
                    if ( weakThis.get() == this )
                        --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
                });
            wxUnusedVar(leaveMeasureImage);

            int iw = p->OnMeasureImage().x;
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state || GetSelection() != selection ||
                 selection != p || IsPropertyPendingRemoval(p) )
            {
                return false;
            }

            wxASSERT( iw == wxDefaultCoord || iw >= 0 );
            if ( iw == wxDefaultCoord || iw == 0 )
                iw = wxPG_CUSTOM_IMAGE_WIDTH;
            imageOffset = p->GetImageOffset(iw);
        }
    }
    else if ( column == 0 )
    {
        splitterX += (p->GetDepth() - 1) * m_subgroup_extramargin;
    }

    *rect = wxRect(
        splitterX+imageOffset+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1-vx,
        itemy-vy,
        colEnd-splitterX-wxPG_XBEFOREWIDGET-wxPG_CONTROL_MARGIN-imageOffset-1,
        m_lineHeight-1
    );
    return true;
}

// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetImageRect( wxPGProperty* p, int item ) const
{
    wxSize sz = GetImageSize(p, item);
    return wxRect(wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                  wxPG_CUSTOM_IMAGE_SPACINGY,
                  sz.x,
                  sz.y);
}

// return size of custom paint image
wxSize wxPropertyGrid::GetImageSize( wxPGProperty* p, int item ) const
{
    wxSize size;
    TryGetImageSize(p, item, &size);
    return size;
}

bool wxPropertyGrid::TryGetImageSize( wxPGProperty* p,
                                      int item,
                                      wxSize* size ) const
{
    wxCHECK_MSG( size, false, wxS("null image size output") );

    // If called with nullptr property, then return default image
    // size for properties that use image.
    if ( !p )
    {
        *size = wxSize(wxPG_CUSTOM_IMAGE_WIDTH,
                       wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight));
        return true;
    }

    wxPropertyGrid* const self = const_cast<wxPropertyGrid*>(this);
    const wxWeakRef<wxWindow> weakThis(self);
    wxPropertyGridPageState* const state = m_pState;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(self).propertyCallbackDepth;
    wxScopeGuard leaveMeasureImage =
        wxMakeGuard([weakThis, self]()
        {
            if ( weakThis.get() == self )
                --wxPGGetPropertyGridTransientState(self).propertyCallbackDepth;
        });
    wxUnusedVar(leaveMeasureImage);
    const auto transactionIsValid =
        [weakThis, self, state, p]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, self) &&
                   self->m_pState == state &&
                   !self->IsPropertyPendingRemoval(p);
        };

    wxSize cis = p->OnMeasureImage(item);
    if ( !transactionIsValid() )
    {
        *size = wxSize(0, 0);
        return false;
    }

    int choiceCount = p->GetChoices().GetCount();
    int comVals = p->GetDisplayedCommonValueCount();
    if ( item >= choiceCount && comVals > 0 )
    {
        unsigned int cvi = item-choiceCount;
        cis = GetCommonValue(cvi)->GetRenderer()->GetImageSize(nullptr, 1, cvi);
        if ( !transactionIsValid() )
        {
            *size = wxSize(0, 0);
            return false;
        }
    }
    else if ( item >= 0 && choiceCount == 0 )
    {
        *size = wxSize(0, 0);
        return true;
    }

    wxASSERT( cis.x == wxDefaultCoord || cis.x >= 0 );
    if ( cis.x == wxDefaultCoord )
    {
        cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
    }
    wxASSERT( cis.y == wxDefaultCoord || cis.y >= 0 );
    if ( cis.y == wxDefaultCoord || cis.y == 0 )
    {
        cis.y = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
    }
    *size = cis;
    return true;
}

// -----------------------------------------------------------------------

// takes scrolling into account
void wxPropertyGrid::ImprovedClientToScreen( int* px, int* py ) const
{
    wxASSERT(px && py);
    CalcScrolledPosition(*px, *py, px, py);
    ClientToScreen( px, py );

}

// -----------------------------------------------------------------------

wxPropertyGridHitTestResult wxPropertyGrid::HitTest( const wxPoint& pt ) const
{
    return m_pState->HitTest(CalcUnscrolledPosition(pt));
}

// -----------------------------------------------------------------------

// custom set cursor
void wxPropertyGrid::CustomSetCursor( int type, bool override )
{
    if ( type == m_curcursor && !override ) return;

    wxCursor cursor = (type == wxCURSOR_SIZEWE) ? m_cursorSizeWE : wxPG_DEFAULT_CURSOR;
    SetCursor(cursor);

    m_curcursor = type;
}

// -----------------------------------------------------------------------

wxString
wxPropertyGrid::GetUnspecifiedValueText(wxPGPropValFormatFlags flags) const
{
    const wxPGCell& ua = GetUnspecifiedValueAppearance();

    if ( ua.HasText() &&
         !(flags & wxPGPropValFormatFlags::FullValue) &&
         !(flags & wxPGPropValFormatFlags::EditableValue) )
        return ua.GetText();

    return wxString();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property selection, editor creation
// -----------------------------------------------------------------------

//
// This class forwards events from property editor controls to wxPropertyGrid.
class wxPropertyGridEditorEventForwarder : public wxEvtHandler
{
public:
    wxPropertyGridEditorEventForwarder( wxPropertyGrid* propGrid )
        : wxEvtHandler(),
          m_propGrid(propGrid->IsBeingDeleted() ? nullptr : propGrid),
          m_gridIdentity(propGrid),
          m_registryNext(ms_firstForwarder)
    {
        ms_firstForwarder = this;
    }

    virtual ~wxPropertyGridEditorEventForwarder()
    {
        wxPropertyGridEditorEventForwarder** link = &ms_firstForwarder;
        while ( *link && *link != this )
            link = &(*link)->m_registryNext;
        if ( *link == this )
            *link = m_registryNext;
    }

    static bool IsForGrid(wxEvtHandler* handler, wxPropertyGrid* grid)
    {
        for ( wxPropertyGridEditorEventForwarder* forwarder =
                  ms_firstForwarder;
              forwarder;
              forwarder = forwarder->m_registryNext )
        {
            if ( forwarder == handler )
                return forwarder->m_gridIdentity == grid;
        }

        return false;
    }

    static void DisconnectFromGrid(wxPropertyGrid* grid)
    {
        for ( wxPropertyGridEditorEventForwarder* forwarder =
                  ms_firstForwarder;
              forwarder;
              forwarder = forwarder->m_registryNext )
        {
            if ( forwarder->m_propGrid == grid )
                forwarder->m_propGrid = nullptr;
        }
    }

private:
    bool ProcessEvent( wxEvent& event ) override
    {
        // Always skip
        event.Skip();

        wxPropertyGrid* const propGrid = m_propGrid;
        if ( !propGrid )
            return wxEvtHandler::ProcessEvent(event);

        const wxWeakRef<wxWindow> weakGrid(propGrid);

        // Keep editor objects alive for the entire forwarding transaction, not
        // just while HandleCustomEditorEvent() is running. A handler further
        // down the pushed event-handler chain may destroy the grid, in which
        // case its destructor must defer deleting this active forwarder and
        // the editor currently dispatching the event.
        wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
        ++wxPGGetPropertyGridTransientState(propGrid).propertyCallbackDepth;
        const auto leaveForwarding = wxMakeGuard(
            [weakGrid, propGrid]
            {
                if ( weakGrid.get() == propGrid )
                    --wxPGGetPropertyGridTransientState(propGrid).propertyCallbackDepth;
            });

        const bool isMainButtonEvent = propGrid->IsMainButtonEvent(event);

        propGrid->HandleCustomEditorEvent(event);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) )
            return true;

        //
        // NB: We should return true if the event was recognized as
        //     a dedicated wxPropertyGrid event, and as such was
        //     either properly handled or ignored.
        //
        if ( isMainButtonEvent )
            return true;

        //
        // NB: On wxMSW, a wxTextCtrl with wxTE_PROCESS_ENTER
        //     may beep annoyingly if that event is skipped
        //     and passed to parent event handler.
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
            return true;

        return wxEvtHandler::ProcessEvent(event);
    }

    wxPropertyGrid* m_propGrid;
    // Opaque identity retained only for the existing deferred-detachment
    // bookkeeping. It must never be dereferenced after DisconnectFromGrid().
    const wxPropertyGrid* const m_gridIdentity;
    wxPropertyGridEditorEventForwarder* m_registryNext;
    static wxPropertyGridEditorEventForwarder* ms_firstForwarder;
};

wxPropertyGridEditorEventForwarder*
    wxPropertyGridEditorEventForwarder::ms_firstForwarder = nullptr;

namespace
{

void DisconnectPropertyGridEditorForwarders(wxPropertyGrid* grid)
{
    wxPropertyGridEditorEventForwarder::DisconnectFromGrid(grid);
}

void
DetachPropertyGridEditorForwarders(
    wxWindow* window,
    wxPropertyGrid* grid,
    std::vector<wxEvtHandler*>& forwarders)
{
    if ( !window )
        return;

    for ( wxEvtHandler* handler = window->GetEventHandler();
          handler && handler != window; )
    {
        wxEvtHandler* const next = handler->GetNextHandler();
        if ( wxPropertyGridEditorEventForwarder::IsForGrid(
                 handler, grid) )
        {
            if ( window->RemoveEventHandler(handler) )
                forwarders.push_back(handler);
        }
        // In particular, the generic text hint handler is owned by the text
        // entry and must still be attached when its destructor pops it.
        handler = next;
    }
}

} // anonymous namespace

// Setups event handling for child control
void wxPropertyGrid::SetupChildEventHandling( wxWindow* argWnd )
{
    wxWindowID id = argWnd->GetId();

    if ( argWnd == m_wndEditor )
    {
        argWnd->Bind(wxEVT_MOTION, &wxPropertyGrid::OnMouseMoveChild, this, id);
        argWnd->Bind(wxEVT_LEFT_UP, &wxPropertyGrid::OnMouseUpChild, this, id);
        argWnd->Bind(wxEVT_LEFT_DOWN, &wxPropertyGrid::OnMouseClickChild, this, id);
        argWnd->Bind(wxEVT_RIGHT_UP, &wxPropertyGrid::OnMouseRightClickChild, this, id);
        argWnd->Bind(wxEVT_ENTER_WINDOW, &wxPropertyGrid::OnMouseEntry, this, id);
        argWnd->Bind(wxEVT_LEAVE_WINDOW, &wxPropertyGrid::OnMouseEntry, this, id);
    }

    wxPropertyGridEditorEventForwarder* forwarder;
    forwarder = new wxPropertyGridEditorEventForwarder(this);
    argWnd->PushEventHandler(forwarder);

    argWnd->Bind(wxEVT_KEY_DOWN, &wxPropertyGrid::OnChildKeyDown, this, id);
}

void wxPropertyGrid::DeletePendingObjects()
{
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
#ifdef __WXMSW__
    std::vector<wxObject*> objects;
    while ( !m_deletedEditorObjects.empty() )
    {
        objects.push_back(m_deletedEditorObjects.back());
        m_deletedEditorObjects.pop_back();
    }
    if ( !objects.empty() )
        DeferPropertyGridEditorObjects(this, objects);
#else
    struct TrackedPendingObject
    {
        TrackedPendingObject(wxObject* identity_, wxEvtHandler* object_)
            : identity(identity_),
              object(object_)
        {
        }

        wxObject* identity;
        wxEvtHandlerRef object;
    };

    std::unordered_set<wxObject*> adopted;
    std::vector<TrackedPendingObject> objects;
    while ( !m_deletedEditorObjects.empty() )
    {
        wxObject* const object = m_deletedEditorObjects.back();
        m_deletedEditorObjects.pop_back();
        if ( !object || !adopted.insert(object).second )
            continue;

        while ( wxPendingDelete.Member(object) )
            wxPendingDelete.DeleteObject(object);

        wxEvtHandler* const trackable =
            wxDynamicCast(object, wxEvtHandler);
        if ( !trackable )
        {
            wxFAIL_MSG(
                wxS("Untrackable object in property-grid editor cleanup"));
            continue;
        }
        objects.emplace_back(object, trackable);
    }

    for ( TrackedPendingObject& slot : objects )
    {
        wxObject* const object = slot.object.get();
        slot.object.Release();
        delete object;
    }
#endif
}

void wxPropertyGrid::DestroyEditorWnd( wxWindow* wnd )
{
    if ( !wnd )
        return;

#ifdef __WXMSW__
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    std::vector<wxObject*> objects;
    objects.push_back(wnd);
    while ( !m_deletedEditorObjects.empty() )
    {
        objects.push_back(m_deletedEditorObjects.back());
        m_deletedEditorObjects.pop_back();
    }
    DeferPropertyGridEditorObjects(this, objects);
#else
    wnd->Hide();

    // Do not free editors immediately (for sake of processing events)
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    m_deletedEditorObjects.push_back(wnd);
#endif
}

void wxPropertyGrid::FreeEditors()
{
#ifdef __WXMSW__
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    wxWindow* const primary = m_wndEditor;
    wxWindow* const secondary =
        m_wndEditor2 == primary ? nullptr : m_wndEditor2;
    wxWindow* const focusBefore = wxWindow::FindFocus();
    const bool editorHadFocus =
        focusBefore &&
        ((primary &&
          (focusBefore == primary ||
           primary->IsDescendant(focusBefore))) ||
         (secondary &&
          (focusBefore == secondary ||
           secondary->IsDescendant(focusBefore))));
    m_wndEditor = nullptr;
    m_wndEditor2 = nullptr;
    m_editorFocused = false;

    std::vector<wxObject*> objects;
    const auto collect =
        [this, &objects](wxWindow* editor)
        {
            if ( !editor )
                return;

            // Preserve the historical destruction order: wrapper first, then
            // every detached PropertyGrid forwarder.
            objects.push_back(editor);
            std::vector<wxEvtHandler*> forwarders;
            DetachPropertyGridEditorForwarders(
                editor, this, forwarders);
            for ( wxEvtHandler* const forwarder : forwarders )
                objects.push_back(forwarder);
        };

    collect(primary);
    collect(secondary);
    while ( !m_deletedEditorObjects.empty() )
    {
        objects.push_back(m_deletedEditorObjects.back());
        m_deletedEditorObjects.pop_back();
    }

    const wxWeakRef<wxWindow> weakThis(this);
    if ( !objects.empty() )
        DeferPropertyGridEditorObjects(this, objects);
    if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
    {
        if ( editorHadFocus )
            SetFocus();
        else
            SetFocusOnCanvas();
    }
#else
    const wxWeakRef<wxWindow> weakThis(this);

    //
    // Return focus back to canvas from children (this is required at least for
    // GTK+, which, unlike Windows, clears focus when control is destroyed
    // instead of moving it to closest parent).
    SetFocusOnCanvas();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    // Do not free editors immediately if processing events
    if ( m_wndEditor2 )
    {
        wxEvtHandler* handler = m_wndEditor2->PopEventHandler(false);
        m_wndEditor2->Hide();
        m_deletedEditorObjects.push_back(handler);
        DestroyEditorWnd(m_wndEditor2);
        m_wndEditor2 = nullptr;
    }

    if ( m_wndEditor )
    {
        wxEvtHandler* handler = m_wndEditor->PopEventHandler(false);
        m_wndEditor->Hide();
        m_deletedEditorObjects.push_back(handler);
        DestroyEditorWnd(m_wndEditor);
        m_wndEditor = nullptr;
    }
#endif
}

// Call with nullptr to de-select property
bool wxPropertyGrid::DoSelectProperty( wxPGProperty* p, wxPGSelectPropertyFlags flags )
{
    /*
    if (p)
    {
        wxLogDebug(wxS("SelectProperty( %s (%s[%i]) )"),p->GetLabel(),
            p->m_parent->GetLabel(),p->GetIndexInParent());
    }
    else
    {
        wxLogDebug(wxS("SelectProperty( nullptr, -1 )"));
    }
    */

    if ( m_inDoSelectProperty )
        return true;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const initialState = m_pState;
    m_inDoSelectProperty = true;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSelectProperty = wxMakeGuard([weakThis, this]()
    {
        if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) && m_pState )
        {
            wxPGProperty* const selected = GetSelection();
            if ( selected && IsPropertyPendingRemoval(selected) )
            {
                // DeleteProperty() cannot recursively deselect while this
                // transaction is active. Retire the abandoned selection and
                // editors after its callback returns, but before releasing
                // the callback epoch that keeps the property alive. Clear
                // selection first: editor cleanup can itself send events.
                m_pState->DoSetSelection(nullptr);
                m_editorFocused = false;
                ClearInternalFlag(wxPG_FL_ABNORMAL_EDITOR);
                EditorsValueWasNotModified();
                FreeEditors();
            }
        }

        if ( weakThis.get() == this )
        {
            ClearInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);
            m_inDoSelectProperty = false;
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        }
    });
    wxUnusedVar(leaveSelectProperty);

    if ( !initialState )
        return false;

    wxArrayPGProperty prevSelection = initialState->m_selection;
    wxPGProperty* prevFirstSel;

    prevFirstSel = prevSelection.empty()? nullptr: prevSelection[0];

    if ( prevFirstSel && prevFirstSel->HasFlag(wxPGFlags::BeingDeleted) )
        prevFirstSel = nullptr;

    // Always send event, as this is indirect call
    DoEndLabelEdit(true, wxPGSelectPropertyFlags::NoValidate);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != initialState ||
         (p &&
          (p->GetParentState() != initialState ||
           IsPropertyPendingRemoval(p) ||
           p->HasFlag(wxPGFlags::BeingDeleted))) )
        return false;

/*
    if ( prevFirstSel )
        wxPrintf( "Selected %s\n", prevFirstSel->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "None selected\n" );

    if (p)
        wxPrintf( "P =  %s\n", p->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "P = nullptr\n" );
*/

    wxWindow* primaryCtrl = nullptr;

    // If we are frozen, then just set the values.
    if ( IsFrozen() )
    {
        m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
        m_editorFocused = false;
        m_pState->DoSetSelection(p);

        // If frozen, always free controls. But don't worry, as Thaw will
        // recall SelectProperty to recreate them.
        FreeEditors();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != initialState ||
             (p && IsPropertyPendingRemoval(p)) )
            return false;

        // Prevent any further selection measures in this call
        p = nullptr;
    }
    else
    {
        // Is it the same?
        if ( prevFirstSel == p &&
             prevSelection.size() <= 1 &&
             !(flags & wxPGSelectPropertyFlags::Force) )
        {
            // Only set focus if not deselecting
            if ( p )
            {
                wxWindow* const currentPrimary = m_wndEditor;
                wxWindow* const currentSecondary = m_wndEditor2;
                const wxWeakRef<wxWindow> weakPrimary(currentPrimary);
                const wxWeakRef<wxWindow> weakSecondary(currentSecondary);
                const auto sameSelectionTransactionIsValid =
                    [weakThis, weakPrimary, weakSecondary, this,
                     initialState, p, currentPrimary, currentSecondary]()
                    {
                        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                            return false;

                        bool editorsAreValid = true;
                        if ( currentPrimary &&
                             (!wxWeakWindowIsAvailableForCallbacks(weakPrimary, currentPrimary)) )
                        {
                            if ( m_wndEditor == currentPrimary )
                                m_wndEditor = nullptr;
                            editorsAreValid = false;
                        }
                        if ( currentSecondary &&
                             (!wxWeakWindowIsAvailableForCallbacks(weakSecondary, currentSecondary)) )
                        {
                            if ( m_wndEditor2 == currentSecondary )
                                m_wndEditor2 = nullptr;
                            editorsAreValid = false;
                        }

                        return editorsAreValid &&
                               !wxWindowIsUnavailableForCallbacks(this) &&
                               m_pState == initialState &&
                               GetSelection() == p &&
                               p->GetParentState() == initialState &&
                               !IsPropertyPendingRemoval(p) &&
                               !p->HasFlag(wxPGFlags::BeingDeleted) &&
                               m_wndEditor == currentPrimary &&
                               m_wndEditor2 == currentSecondary;
                    };

                if ( !sameSelectionTransactionIsValid() )
                    return false;

                if ( !!(flags & wxPGSelectPropertyFlags::Focus) )
                {
                    if ( m_wndEditor )
                    {
                        wxWindow* const editorControl = currentPrimary;
                        if ( !editorControl )
                            return false;
                        editorControl->SetFocus();
                        if ( !sameSelectionTransactionIsValid() )
                            return false;
                        m_editorFocused = true;
                    }
                }
                else
                {
                    SetFocusOnCanvas();
                    if ( !sameSelectionTransactionIsValid() )
                        return false;
                }
            }

            return true;
        }

        //
        // First, deactivate previous
        if ( prevFirstSel )
        {
            // Must double-check if this is an selected in case of force switch
            if ( p != prevFirstSel )
            {
                if ( !CommitChangesFromEditor(flags) )
                {
                    // Validation has failed, so we can't exit the previous editor
                    //::wxMessageBox(_("Please correct the value or press ESC to cancel the edit."),
                    //               _("Invalid Value"),wxOK|wxICON_ERROR);
                    return false;
                }
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != initialState ||
                     (p && IsPropertyPendingRemoval(p)) )
                {
                    return false;
                }
            }

            // This should be called after CommitChangesFromEditor(), so that
            // OnValidationFailure() still has information on property's
            // validation state.
            OnValidationFailureReset(prevFirstSel);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != initialState ||
                 (p && IsPropertyPendingRemoval(p)) )
            {
                return false;
            }

            FreeEditors();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != initialState ||
                 (p && IsPropertyPendingRemoval(p)) )
                return false;

            m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
            EditorsValueWasNotModified();
        }

        SetInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);

        m_pState->DoSetSelection(p);

        // Redraw unselected
        for( wxPGProperty* selProp : prevSelection )
        {
            if ( !selProp ||
                 selProp->GetParentState() != initialState ||
                 IsPropertyPendingRemoval(selProp) ||
                 selProp->HasFlag(wxPGFlags::BeingDeleted) )
            {
                continue;
            }
            DrawItem(selProp);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != initialState ||
                 (p && (p->GetParentState() != initialState ||
                        IsPropertyPendingRemoval(p) ||
                        p->HasFlag(wxPGFlags::BeingDeleted))) )
                return false;
        }

        //
        // Then, activate the one given.
        if ( p )
        {
            int propY = p->GetY2(m_lineHeight);

            int splitterX;
            CalcScrolledPosition(GetSplitterPosition(), 0, &splitterX, nullptr);

            m_editorFocused = false;
            m_iFlags |= wxPG_FL_PRIMARY_FILLS_ENTIRE;

            wxASSERT( m_wndEditor == nullptr );

            //
            // Only create editor for non-disabled non-caption
            if ( !p->IsCategory() && !p->HasFlag(wxPGFlags::Disabled) )
            {
            // do this for non-caption items

                m_selColumn = 1;

                const wxPGEditor* editor = p->GetEditorClass();
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                     m_pState != initialState || GetSelection() != p ||
                     p->GetParentState() != initialState ||
                     IsPropertyPendingRemoval(p) ||
                     p->HasFlag(wxPGFlags::BeingDeleted) )
                {
                    return false;
                }
                wxCHECK_MSG(editor, false,
                    wxS("null editor class not allowed"));

                // Do we need to paint the custom image, if any?
                m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);
                if ( p->HasFlag(wxPGFlags::CustomImage) )
                {
                    const bool canContainCustomImage =
                        editor->CanContainCustomImage();
                    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                         m_pState != initialState || GetSelection() != p ||
                         p->GetParentState() != initialState ||
                         IsPropertyPendingRemoval(p) ||
                         p->HasFlag(wxPGFlags::BeingDeleted) )
                    {
                        return false;
                    }
                    if ( !canContainCustomImage )
                        m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;
                }

                wxRect grect;
                if ( !TryGetEditorWidgetRect(p, m_selColumn, &grect) )
                    return false;
                wxPoint goodPos = grect.GetPosition();

                // Editor appearance can now be considered clear
                m_editorAppearance.SetEmptyData();

                m_iFlags &= ~wxPG_FL_FIXED_WIDTH_EDITOR;

#ifdef __WXMSW__
                EditorCreationTransaction editorCreationTransaction(this);
#endif
                wxPGWindowList wndList =
                    editor->CreateControls(this,
                                           p,
                                           goodPos,
                                           grect.GetSize());
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != initialState ||
                     GetSelection() != p || IsPropertyPendingRemoval(p) )
                {
                    if ( weakThis.get() == this )
                    {
#ifdef __WXMSW__
                        std::vector<wxObject*> abandonedEditors;
                        abandonedEditors.push_back(wndList.GetPrimary());
                        if ( wndList.GetSecondary() != wndList.GetPrimary() )
                        {
                            abandonedEditors.push_back(
                                wndList.GetSecondary());
                        }
                        DeferPropertyGridEditorObjects(
                            this, abandonedEditors);
#else
                        if ( wndList.GetSecondary() != wndList.GetPrimary() )
                            DestroyEditorWnd(wndList.GetSecondary());
                        DestroyEditorWnd(wndList.GetPrimary());
#endif
                    }
                    return false;
                }

                m_wndEditor = wndList.GetPrimary();
                m_wndEditor2 = wndList.GetSecondary();
                if ( m_wndEditor2 == m_wndEditor )
                    m_wndEditor2 = nullptr;

                wxWindow* const editorWindow = m_wndEditor;
                wxWindow* const secondaryCtrl = m_wndEditor2;
                const wxWeakRef<wxWindow> weakEditorWindow(editorWindow);
                const wxWeakRef<wxWindow> weakSecondary(secondaryCtrl);
                primaryCtrl = GetEditorControl();
                const wxWeakRef<wxWindow> weakPrimary(primaryCtrl);
                const auto editorTransactionIsValid =
                    [weakThis, weakEditorWindow, weakSecondary, weakPrimary,
                     this, initialState, p, editorWindow, secondaryCtrl,
                     primaryCtrl]()
                    {
                        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                            return false;

                        bool editorsAreValid = true;
                        if ( editorWindow &&
                             (!wxWeakWindowIsAvailableForCallbacks(weakEditorWindow, editorWindow)) )
                        {
                            if ( m_wndEditor == editorWindow )
                                m_wndEditor = nullptr;
                            editorsAreValid = false;
                        }
                        if ( secondaryCtrl &&
                             (!wxWeakWindowIsAvailableForCallbacks(weakSecondary, secondaryCtrl)) )
                        {
                            if ( m_wndEditor2 == secondaryCtrl )
                                m_wndEditor2 = nullptr;
                            editorsAreValid = false;
                        }

                        return editorsAreValid &&
                               !wxWindowIsUnavailableForCallbacks(this) &&
                               m_pState == initialState &&
                               GetSelection() == p &&
                               p->GetParentState() == initialState &&
                               !IsPropertyPendingRemoval(p) &&
                               !p->HasFlag(wxPGFlags::BeingDeleted) &&
                               m_wndEditor == editorWindow &&
                               m_wndEditor2 == secondaryCtrl &&
                               (!primaryCtrl ||
                                 (wxWeakWindowIsAvailableForCallbacks(weakPrimary, primaryCtrl) &&
                                  GetEditorControl() == primaryCtrl));
                    };

                if ( (editorWindow && !primaryCtrl) ||
                     !editorTransactionIsValid() )
                    return false;

                // Remember actual positions within required cell.
                // These values can be used when there will be required
                // to reposition the cell.
                if ( editorWindow )
                {
                    m_wndEditorPosRel = editorWindow->GetPosition() - goodPos;
                }
                if ( secondaryCtrl )
                {
                    m_wndEditor2PosRel = secondaryCtrl->GetPosition() - goodPos;
                }

                //
                // Essentially, primaryCtrl == m_wndEditor
                //

                // NOTE: It is allowed for m_wndEditor to be null - in this
                //       case value is drawn as normal, and m_wndEditor2 is
                //       assumed to be a right-aligned button that triggers
                //       a separate editorCtrl window.

                if ( editorWindow )
                {
                    wxASSERT_MSG( editorWindow->GetParent() == GetPanel(),
                                  wxS("CreateControls must use result of ")
                                  wxS("wxPropertyGrid::GetPanel() as parent ")
                                  wxS("of controls.") );

                    // Set validator, if any
                #if wxUSE_VALIDATORS
                    wxValidator* validator = p->GetValidator();
                    if ( !editorTransactionIsValid() )
                        return false;
                    if ( validator )
                    {
                        primaryCtrl->SetValidator(*validator);
                        if ( !editorTransactionIsValid() )
                            return false;
                    }
                #endif

                    if ( editorWindow->GetSize().y > (m_lineHeight+6) )
                        m_iFlags |= wxPG_FL_ABNORMAL_EDITOR;

                    // If it has modified status, use bold font
                    // (must be done before capturing m_ctrlXAdjust)
                    if ( p->HasFlag(wxPGFlags::Modified) &&
                         (m_windowStyle & wxPG_BOLD_MODIFIED) )
                    {
                        SetCurControlBoldFont();
                        if ( !editorTransactionIsValid() )
                            return false;
                    }
                    // Store x relative to splitter (we'll need it).
                    m_ctrlXAdjust = editorWindow->GetPosition().x - splitterX;

                    // Check if background clear is not necessary
                    wxPoint pos = editorWindow->GetPosition();
                    if ( pos.x > (splitterX+1) || pos.y > propY )
                    {
                        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
                    }

                    editorWindow->SetSizeHints(3, 3);
                    if ( !editorTransactionIsValid() )
                        return false;

                    SetupChildEventHandling(primaryCtrl);
                    if ( !editorTransactionIsValid() )
                        return false;

                    // Focus and select all (wxTextCtrl, wxComboBox etc.)
                    if ( !!(flags & wxPGSelectPropertyFlags::Focus) )
                    {
                        primaryCtrl->SetFocus();
                        if ( !editorTransactionIsValid() )
                            return false;

                        editor->OnFocus(p, primaryCtrl);
                        if ( !editorTransactionIsValid() )
                            return false;
                    }
                    else
                    {
                        if ( p->IsValueUnspecified() )
                        {
                            SetEditorAppearance(m_unspecifiedAppearance,
                                                true);
                            if ( !editorTransactionIsValid() )
                                return false;
                        }
                    }
                }

                if ( secondaryCtrl )
                {
                    wxASSERT_MSG( secondaryCtrl->GetParent() == GetPanel(),
                                  wxS("CreateControls must use result of ")
                                  wxS("wxPropertyGrid::GetPanel() as parent ")
                                  wxS("of controls.") );

                    // Get proper id for wndSecondary
                    m_wndSecId = secondaryCtrl->GetId();
                    wxWindowList children = secondaryCtrl->GetChildren();
                    wxWindowList::iterator node = children.begin();
                    if ( node != children.end() )
                        m_wndSecId = ((wxWindow*)*node)->GetId();

                    secondaryCtrl->SetSizeHints(3,3);
                    if ( !editorTransactionIsValid() )
                        return false;

                    secondaryCtrl->Show();
                    if ( !editorTransactionIsValid() )
                        return false;

                    SetupChildEventHandling(secondaryCtrl);
                    if ( !editorTransactionIsValid() )
                        return false;

                    // If no primary editor, focus to button to allow
                    // it to interpret ENTER etc.
                    // NOTE: Due to problems focusing away from it, this
                    //       has been disabled.
                    /*
                    if ( (flags & wxPGSelectPropertyFlags::Focus) && !m_wndEditor )
                        m_wndEditor2->SetFocus();
                    */
                }

                if ( !!(flags & wxPGSelectPropertyFlags::Focus) )
                    m_editorFocused = true;

            }
            else
            {
                // Make sure focus is in grid canvas (important for wxGTK,
                // at least)
                SetFocusOnCanvas();
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                     m_pState != initialState || GetSelection() != p ||
                     p->GetParentState() != initialState ||
                     IsPropertyPendingRemoval(p) ||
                     p->HasFlag(wxPGFlags::BeingDeleted) )
                    return false;
            }

            EditorsValueWasNotModified();

            wxWindow* const activePrimary = m_wndEditor;
            wxWindow* const activeSecondary = m_wndEditor2;
            const wxWeakRef<wxWindow> weakActivePrimary(activePrimary);
            const wxWeakRef<wxWindow> weakActiveSecondary(activeSecondary);
            const auto activeSelectionTransactionIsValid =
                [weakThis, weakActivePrimary, weakActiveSecondary,
                 this, initialState, p, activePrimary, activeSecondary]()
                {
                    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                        return false;

                    bool editorsAreValid = true;
                    if ( activePrimary &&
                         (!wxWeakWindowIsAvailableForCallbacks(weakActivePrimary, activePrimary)) )
                    {
                        if ( m_wndEditor == activePrimary )
                            m_wndEditor = nullptr;
                        editorsAreValid = false;
                    }
                    if ( activeSecondary &&
                         (!wxWeakWindowIsAvailableForCallbacks(weakActiveSecondary, activeSecondary)) )
                    {
                        if ( m_wndEditor2 == activeSecondary )
                            m_wndEditor2 = nullptr;
                        editorsAreValid = false;
                    }

                    return editorsAreValid &&
                           !wxWindowIsUnavailableForCallbacks(this) &&
                           m_pState == initialState &&
                           GetSelection() == p &&
                           p->GetParentState() == initialState &&
                           !IsPropertyPendingRemoval(p) &&
                           !p->HasFlag(wxPGFlags::BeingDeleted) &&
                           m_wndEditor == activePrimary &&
                           m_wndEditor2 == activeSecondary;
                };

            if ( !activeSelectionTransactionIsValid() )
                return false;

            // If it's inside collapsed section, expand parent, scroll, etc.
            // Also, if it was partially visible, scroll it into view.
            if ( !(flags & wxPGSelectPropertyFlags::Nonvisible) )
            {
                EnsureVisible( p );
                if ( !activeSelectionTransactionIsValid() )
                    return false;
            }

            if ( activePrimary )
            {
                activePrimary->Show(true);
                if ( !activeSelectionTransactionIsValid() )
                    return false;
            }

            if ( !(flags & wxPGSelectPropertyFlags::NoRefresh) )
            {
                if ( !activeSelectionTransactionIsValid() )
                    return false;
                DrawItem(p);
                if ( !activeSelectionTransactionIsValid() )
                    return false;
            }
        }
        else
        {
            // Make sure focus is in grid canvas
            SetFocusOnCanvas();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != initialState )
                return false;
        }

        ClearInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);
    }

    wxPGProperty* const selectedAfterTransition = GetSelection();
    wxWindow* const postSelectionPrimary = m_wndEditor;
    wxWindow* const postSelectionSecondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakPostSelectionPrimary(postSelectionPrimary);
    const wxWeakRef<wxWindow> weakPostSelectionSecondary(postSelectionSecondary);
    const auto postSelectionTransactionIsValid =
        [weakThis, weakPostSelectionPrimary, weakPostSelectionSecondary,
         this, initialState, selectedAfterTransition, postSelectionPrimary,
         postSelectionSecondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            bool editorsAreValid = true;
            if ( postSelectionPrimary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPostSelectionPrimary, postSelectionPrimary)) )
            {
                if ( m_wndEditor == postSelectionPrimary )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( postSelectionSecondary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPostSelectionSecondary, postSelectionSecondary)) )
            {
                if ( m_wndEditor2 == postSelectionSecondary )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }

            return editorsAreValid &&
                   !wxWindowIsUnavailableForCallbacks(this) &&
                   m_pState == initialState &&
                   GetSelection() == selectedAfterTransition &&
                   (!selectedAfterTransition ||
                    (selectedAfterTransition->GetParentState() == initialState &&
                     !IsPropertyPendingRemoval(selectedAfterTransition) &&
                     !selectedAfterTransition->HasFlag(
                         wxPGFlags::BeingDeleted))) &&
                   m_wndEditor == postSelectionPrimary &&
                   m_wndEditor2 == postSelectionSecondary;
        };

    if ( !postSelectionTransactionIsValid() )
        return false;

    // Do not retain references into a property across status-bar/tooltip
    // updates: both can synchronously dispatch application code.
    const wxString helpString = p ? p->GetHelpString() : wxString();
    if ( !postSelectionTransactionIsValid() ||
         (p && (GetSelection() != p || IsPropertyPendingRemoval(p))) )
    {
        return false;
    }

    if ( !HasExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS) )
    {
#if wxUSE_STATUSBAR

        //
        // Show help text in status bar.
        //   (if found and grid not embedded in manager with help box and
        //    style wxPG_EX_HELP_AS_TOOLTIPS is not used).
        //
        wxStatusBar* statusbar = GetStatusBar();
        if ( statusbar )
        {
            if ( !helpString.empty() )
            {
                // Set help box text.
                statusbar->SetStatusText(helpString);
                if ( !postSelectionTransactionIsValid() ||
                     (p && (GetSelection() != p ||
                            IsPropertyPendingRemoval(p))) )
                {
                    return false;
                }
                m_iFlags |= wxPG_FL_STRING_IN_STATUSBAR;
            }
            else if ( m_iFlags & wxPG_FL_STRING_IN_STATUSBAR )
            {
                // Clear help box - but only if it was written
                // by us at previous time.
                statusbar->SetStatusText(wxString());
                if ( !postSelectionTransactionIsValid() )
                {
                    return false;
                }
                m_iFlags &= ~(wxPG_FL_STRING_IN_STATUSBAR);
            }
        }
#endif
    }
    else
    {
#if wxUSE_TOOLTIPS
        //
        // Show help as a tool tip on the editor control.
        //
        wxWindow* const helpEditor = GetEditorControl();
        const wxWeakRef<wxWindow> weakHelpEditor(helpEditor);
        if ( !helpString.empty() && helpEditor )
        {
            helpEditor->SetToolTip(helpString);
            if ( !postSelectionTransactionIsValid() ||
                 (p && (GetSelection() != p ||
                        IsPropertyPendingRemoval(p))) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakHelpEditor, helpEditor) ||
                 GetEditorControl() != helpEditor )
            {
                return false;
            }
        }
#endif
    }

    // call wx event handler (here so that it also occurs on deselection)
    // In case of deselection previously selected property
    // is passed to the event object.
    if (!p)
    {
        if ( prevFirstSel &&
             prevFirstSel->GetParentState() == initialState &&
             !IsPropertyPendingRemoval(prevFirstSel) )
        {
            p = prevFirstSel;
        }
    }
    if ( !(flags & wxPGSelectPropertyFlags::DontSendEvent) && p)
    {
        SendEvent( wxEVT_PG_SELECTED, p, nullptr );
        if ( !postSelectionTransactionIsValid() ||
             IsPropertyPendingRemoval(p) )
        {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::UnfocusEditor()
{
    wxPGProperty* const selected = GetSelection();

    if ( !selected || !m_wndEditor || IsFrozen() )
        return true;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;

    if ( !CommitChangesFromEditor() )
        return false;
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
         GetSelection() != selected || IsPropertyPendingRemoval(selected) )
    {
        return false;
    }

    SetFocusOnCanvas();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
         GetSelection() != selected || IsPropertyPendingRemoval(selected) )
    {
        return false;
    }

    DrawItem(selected);

    return wxWeakWindowIsAvailableForCallbacks(weakThis, this);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshEditor()
{
    wxPGProperty* p = GetSelection();
    if ( !p )
        return;

    wxWindow* wnd = GetEditorControl();
    if ( !wnd )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakEditor(wnd);
    wxPropertyGridPageState* const state = m_pState;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveRefreshEditor = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveRefreshEditor);

    // Set editor font boldness - must do this before
    // calling UpdateControl().
    if ( HasFlag(wxPG_BOLD_MODIFIED) )
    {
        if ( p->HasFlag(wxPGFlags::Modified) )
            wnd->SetFont(GetCaptionFont());
        else
            wnd->SetFont(GetFont());

        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
             GetSelection() != p || IsPropertyPendingRemoval(p) ||
             !wxWeakWindowIsAvailableForCallbacks(weakEditor, wnd) ||
             GetEditorControl() != wnd )
        {
            return;
        }
    }

    const wxPGEditor* editorClass = p->GetEditorClass();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
         GetSelection() != p || IsPropertyPendingRemoval(p) ||
         !wxWeakWindowIsAvailableForCallbacks(weakEditor, wnd) ||
         GetEditorControl() != wnd || !editorClass )
    {
        return;
    }

    editorClass->UpdateControl(p, wnd);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
         GetSelection() != p || IsPropertyPendingRemoval(p) ||
         !wxWeakWindowIsAvailableForCallbacks(weakEditor, wnd) ||
         GetEditorControl() != wnd )
    {
        return;
    }

    if ( p->IsValueUnspecified() )
        SetEditorAppearance(m_unspecifiedAppearance, true);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SelectProperty( wxPGPropArg id, bool focus )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    wxPGSelectPropertyFlags flags = wxPGSelectPropertyFlags::DontSendEvent;
    if ( focus )
        flags |= wxPGSelectPropertyFlags::Focus;

    return DoSelectProperty(p, flags);
}

// -----------------------------------------------------------------------
// wxPropertyGrid expand/collapse state
// -----------------------------------------------------------------------

bool wxPropertyGrid::DoCollapse( wxPGProperty* p, bool sendEvents )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* selected = GetSelection();

    // If active editor was inside collapsed section, then disable it
    if ( selected && selected->IsSomeParent(p) )
    {
        DoClearSelection();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
            return false;
    }

    // Store dont-center-splitter flag because we need to temporarily set it.
    // Restore it before the public event: the handler is allowed to switch
    // pages, but this operation must not leak policy into the old page.
    const bool prevDontCenterSplitter = state->m_dontCenterSplitter;
    state->m_dontCenterSplitter = true;
    bool res = state->DoCollapse(p);
    state->m_dontCenterSplitter = prevDontCenterSplitter;

    if ( res )
    {
        if ( sendEvents )
        {
            SendEvent( wxEVT_PG_ITEM_COLLAPSED, p );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state ||
                 IsPropertyPendingRemoval(p) )
            {
                return false;
            }
        }

        RecalculateVirtualSize();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
            return false;
        Refresh();
    }

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoExpand( wxPGProperty* p, bool sendEvents )
{
    wxCHECK_MSG( p, false, wxS("invalid property id") );
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;

    // See DoCollapse(): don't let a page-switching event retain this
    // implementation detail in the page that was expanded.
    const bool prevDontCenterSplitter = state->m_dontCenterSplitter;
    state->m_dontCenterSplitter = true;
    bool res = state->DoExpand(p);
    state->m_dontCenterSplitter = prevDontCenterSplitter;

    if ( res )
    {
        if ( sendEvents )
        {
            SendEvent( wxEVT_PG_ITEM_EXPANDED, p );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state ||
                 IsPropertyPendingRemoval(p) )
            {
                return false;
            }
        }

        RecalculateVirtualSize();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
            return false;
        Refresh();
    }

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoHideProperty( wxPGProperty* p, bool hide, wxPGPropertyValuesFlags flags )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return false;

    if ( IsFrozen() )
        return state->DoHideProperty(p, hide, flags);

    wxArrayPGProperty selection = state->m_selection;  // Must use a copy
    for( wxPGProperty* selected: selection )
    {
        if ( selected == p || selected->IsSomeParent(p) )
        {
            if ( !DoRemoveFromSelection(p) )
                return false;
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state || IsPropertyPendingRemoval(p) )
            {
                return false;
            }
        }
    }

    state->DoHideProperty(p, hide, flags);

    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state || IsPropertyPendingRemoval(p) )
    {
        return false;
    }
    Refresh();

    return true;
}


// -----------------------------------------------------------------------
// wxPropertyGrid size related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::RecalculateVirtualSize( int forceXPos )
{
    // Don't check for !HasInternalFlag(wxPG_FL_INITIALIZED) here. Otherwise
    // virtual size calculation may go wrong.
    if ( HasInternalFlag(wxPG_FL_RECALCULATING_VIRTUAL_SIZE) ||
         IsFrozen() ||
         !m_pState )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;

    int h0 = state->m_virtualHeight;
    state->EnsureVirtualHeight();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    wxASSERT_LEVEL_2_MSG(
        state->GetVirtualHeight() == state->GetActualVirtualHeight(),
        wxS("VirtualHeight and ActualVirtualHeight should match")
    );

    m_iFlags |= wxPG_FL_RECALCULATING_VIRTUAL_SIZE;
    wxScopeGuard leaveRecalculate = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            m_iFlags &= ~wxPG_FL_RECALCULATING_VIRTUAL_SIZE;
    });
    wxUnusedVar(leaveRecalculate);

    int w = state->GetVirtualWidth();
    int h = state->m_virtualHeight;
    // Now adjust virtual size.
    SetVirtualSize(w, h);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    // If virtual height was changed, then recalculate editor control position(s)
    if ( h != h0 )
    {
        CorrectEditorWidgetPosY();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }
    if ( forceXPos != -1 )
    {
        Scroll(forceXPos, wxDefaultCoord);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }

    // Must re-get size now
    GetClientSize(&m_width, &m_height);

    if ( !HasVirtualWidth() )
    {
        state->SetVirtualWidth(m_width);
    }
    state->CheckColumnWidths();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }

    if ( GetSelection() )
    {
        CorrectEditorWidgetSizeX();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ReallocDoubleBufferIfNeeded()
{
    if ( !HasExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        double scaleFactor = GetDPIScaleFactor();
        int dblh = (m_lineHeight*2);
        if ( !m_doubleBuffer )
        {
            // Create double buffer bitmap to draw on, if none
            int w = wxMax(m_width, FromDIP(250));
            int h = wxMax(m_height + dblh, FromDIP(400));
            m_doubleBuffer = new wxBitmap;
            m_doubleBuffer->CreateWithLogicalSize( w, h, scaleFactor );
        }
        else
        {
            int w = m_doubleBuffer->GetLogicalWidth();
            int h = m_doubleBuffer->GetLogicalHeight();

            // Double buffer must be large enough
            if ( w < m_width || h < (m_height+dblh) )
            {
                if ( w < m_width ) w = m_width;
                if ( h < (m_height+dblh) ) h = m_height + dblh;
                delete m_doubleBuffer;
                m_doubleBuffer = new wxBitmap;
                m_doubleBuffer->CreateWithLogicalSize( w, h, scaleFactor );
            }
        }
    }
}

void wxPropertyGrid::OnResize( wxSizeEvent& event )
{
    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

    int width, height;
    GetClientSize(&width, &height);

    m_width = width;
    m_height = height;

    ReallocDoubleBufferIfNeeded();

    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    const int oldNcWidth = m_ncWidth;

    // Publish the new non-client width before COLS_RESIZED. Its handler may
    // synchronously query us, switch page, or destroy the grid.
    m_ncWidth = event.GetSize().x;
    state->OnClientWidthChange( width, m_ncWidth - oldNcWidth, true );
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
        return;

    if ( !IsFrozen() )
    {
        if ( m_pState->m_itemsAdded )
            PrepareAfterItemsAdded();
        else
            // Without this, virtual size (at least under wxGTK) will be skewed
            RecalculateVirtualSize();

        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state )
        {
            return;
        }
        Refresh();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetVirtualWidth( int width )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return;

    if ( width == -1 )
    {
        // Disable virtual width
        width = GetClientSize().x;
        ClearInternalFlag(wxPG_FL_HAS_VIRTUAL_WIDTH);
    }
    else
    {
        // Enable virtual width
        SetInternalFlag(wxPG_FL_HAS_VIRTUAL_WIDTH);
    }
    state->SetVirtualWidth( width );
    RecalculateVirtualSize();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         m_pState != state )
    {
        return;
    }
    Refresh();
}

void wxPropertyGrid::SetFocusOnCanvas()
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveFocusChange = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveFocusChange);

    // By default, to prevent wxPropertyGrid from stealing focus from
    // other controls, only move focus to the grid if it was already
    // in one of its child controls.
    // If wxPG_EX_ALWAYS_ALLOW_FOCUS flag is set then wxPropertyGrid
    // can take focus on the entire grid area (canvas) even if focus
    // is moved from another control.
    if ( HasExtraStyle(wxPG_EX_ALWAYS_ALLOW_FOCUS) )
    {
        SetFocus();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }
    else
    {
        wxWindow* focus = wxWindow::FindFocus();
        if ( focus )
        {
            wxWindow* parent = focus->GetParent();
            while ( parent )
            {
                if ( parent == this )
                {
                    SetFocus();
                    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                        return;
                    break;
                }
                parent = parent->GetParent();
            }
        }
    }

    m_editorFocused = false;
}

// -----------------------------------------------------------------------
// wxPropertyGrid mouse event handling
// -----------------------------------------------------------------------

// selFlags uses same values DoSelectProperty's flags
// Returns true if event was vetoed.
bool wxPropertyGrid::SendEvent( wxEventType eventType, wxPGProperty* p,
                                wxVariant* pValue,
                                 wxPGSelectPropertyFlags selFlags,
                                 unsigned int column )
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;

    // selFlags should have wxPGSelectPropertyFlags::NoValidate if event is not
    // vetoable.

    // Send property grid event of specific type and with specific property
    wxWindow* const eventObject = m_eventObject;
    wxCHECK_MSG( eventObject, false, "property grid event object is null" );

    wxPropertyGridEvent evt( eventType, eventObject->GetId() );
    evt.SetPropertyGrid(this);
    evt.SetEventObject(eventObject);
    evt.SetProperty(p);
    evt.SetColumn(column);
    if ( eventType == wxEVT_PG_CHANGING )
    {
        wxASSERT( pValue );
        evt.SetCanVeto(true);
        m_validationInfo.SetValue(pValue ? *pValue : wxVariant());
        evt.SetupValidationInfo();
    }
    else
    {
        if ( p )
            evt.SetPropertyValue(p->GetValue());

        if ( !(selFlags & wxPGSelectPropertyFlags::NoValidate) )
            evt.SetCanVeto(true);
    }

    wxPropertyGridEvent* prevProcessedEvent = m_processedEvent;
    m_processedEvent = &evt;
    const wxWeakRef<wxWindow> weakThis(this);
    eventObject->HandleWindowEvent(evt);
    if ( weakThis.get() == this )
        m_processedEvent = prevProcessedEvent;

    return evt.WasVetoed();
}

void wxPropertyGrid::SendEvent(wxEventType eventType, int intVal)
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;

    wxWindow* const eventObject = m_eventObject;
    wxCHECK_RET( eventObject, "property grid event object is null" );

    wxPropertyGridEvent evt(eventType, eventObject->GetId());
    evt.SetPropertyGrid(this);
    evt.SetEventObject(eventObject);
    evt.SetProperty(nullptr);
    evt.SetColumn(0);
    evt.SetInt(intVal);

    wxPropertyGridEvent* prevProcessedEvent = m_processedEvent;
    m_processedEvent = &evt;
    const wxWeakRef<wxWindow> weakThis(this);
    eventObject->HandleWindowEvent(evt);
    if ( weakThis.get() == this )
        m_processedEvent = prevProcessedEvent;
}

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseClick( int x, unsigned int y, wxMouseEvent &event )
{
    bool res = true;
    const wxWeakRef<wxWindow> weakThis(this);

    // Need to set focus?
    if ( !(m_iFlags & wxPG_FL_FOCUSED) )
    {
        SetFocusOnCanvas();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return res;
    }

    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return res;

    int splitterHit = -1;
    int splitterHitOffset = 0;
    int columnHit = state->HitTestH( x, &splitterHit, &splitterHitOffset );
    x = state->GetColumnLogicalX(x);

    wxPGProperty* p = DoGetItemAtY(y);

    if ( p )
    {
        int depth = (int)p->GetDepth() - 1;

        int marginEnds = m_marginWidth + ( depth * m_subgroup_extramargin );

        if ( x >= marginEnds )
        {
            // Outside margin.

            if ( p->IsCategory() )
            {
                // This is category.
                wxPropertyCategory* pwc = (wxPropertyCategory*)p;

                int textX = m_marginWidth + ((pwc->GetDepth()-1)*m_subgroup_extramargin);

                // Expand, collapse, activate etc. if click on text or left of splitter.
                if ( x >= textX
                     &&
                     ( x < (textX+pwc->GetTextExtent(this, m_captionFont)+(wxPG_CAPRECTXMARGIN*2)) ||
                       columnHit == 0
                     )
                    )
                {
                    if ( !AddToSelectionFromInputEvent( p,
                                                        columnHit,
                                                        &event ) )
                        return res;

                    // On double-click, expand/collapse.
                    if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                    {
                        if ( pwc->IsExpanded() ) DoCollapse( p, true );
                        else DoExpand( p, true );
                    }
                }
            }
            else if ( splitterHit == -1 )
            {
                // Click on value.
                wxPGSelectPropertyFlags selFlag = wxPGSelectPropertyFlags::Null;
                const bool activationByClick = columnHit == 1;
                if ( activationByClick )
                {
                    m_iFlags |= wxPG_FL_ACTIVATION_BY_CLICK;
                    selFlag = wxPGSelectPropertyFlags::Focus;
                }
                bool selectionSucceeded;
                {
                    wxScopeGuard clearActivationByClick =
                        wxMakeGuard([weakThis, this, activationByClick]()
                        {
                            if ( activationByClick && weakThis.get() == this )
                                m_iFlags &= ~wxPG_FL_ACTIVATION_BY_CLICK;
                        });
                    wxUnusedVar(clearActivationByClick);
                    selectionSucceeded =
                        AddToSelectionFromInputEvent(p,
                                                     columnHit,
                                                     &event,
                                                     selFlag);
                }
                if ( !selectionSucceeded )
                    return res;

                if ( p->GetChildCount() && !p->IsCategory() )
                    // On double-click, expand/collapse.
                    if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                    {
                        if ( p->IsExpanded() ) DoCollapse( p, true );
                        else DoExpand( p, true );
                    }

                // Do not Skip() the event after selection has been made.
                // Otherwise default event handling behaviour kicks in
                // and may revert focus back to the main canvas.
                res = true;
            }
            else
            {
            // click on splitter
                if ( !(m_windowStyle & wxPG_STATIC_SPLITTER) )
                {
                    if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
                    {
                        // Double-clicking the splitter causes auto-centering
                        if ( m_pState->GetColumnCount() <= 2 )
                        {
                            const unsigned int columnCount =
                                state->GetColumnCount();
                            wxWindow* const resizePrimary = m_wndEditor;
                            wxWindow* const resizeSecondary = m_wndEditor2;
                            const wxWeakRef<wxWindow>
                                weakResizePrimary(resizePrimary);
                            const wxWeakRef<wxWindow>
                                weakResizeSecondary(resizeSecondary);
                            const auto resizeEditorsAreValid =
                                [weakThis, weakResizePrimary,
                                 weakResizeSecondary, this, resizePrimary,
                                 resizeSecondary]()
                                {
                                    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                                        return false;

                                    bool editorsAreValid = true;
                                    if ( resizePrimary &&
                                         !wxWeakWindowIsAvailableForCallbacks(
                                             weakResizePrimary,
                                             resizePrimary) )
                                    {
                                        if ( m_wndEditor == resizePrimary )
                                            m_wndEditor = nullptr;
                                        editorsAreValid = false;
                                    }
                                    if ( resizeSecondary &&
                                         !wxWeakWindowIsAvailableForCallbacks(
                                             weakResizeSecondary,
                                             resizeSecondary) )
                                    {
                                        if ( m_wndEditor2 == resizeSecondary )
                                            m_wndEditor2 = nullptr;
                                        editorsAreValid = false;
                                    }
                                    return editorsAreValid &&
                                           m_wndEditor == resizePrimary &&
                                           m_wndEditor2 == resizeSecondary;
                                };
                            ResetColumnSizes( true );
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                                 !resizeEditorsAreValid() ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount )
                            {
                                return res;
                            }

                            SendEvent(wxEVT_PG_COLS_RESIZED, wxNullProperty);
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                                 !resizeEditorsAreValid() ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount ||
                                 columnCount < 2 )
                            {
                                return res;
                            }
                            SendEvent(wxEVT_PG_COL_DRAGGING,
                                      m_propHover,
                                      nullptr,
                                      wxPGSelectPropertyFlags::NoValidate,
                                      0); // dragged splitter is always 0 here
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                                 !resizeEditorsAreValid() )
                            {
                                return res;
                            }
                        }
                    }
                    else if ( m_dragStatus == 0 )
                    {
                        //
                        // Begin dragging the splitter
                        //

                        // send event
                        DoEndLabelEdit(true, wxPGSelectPropertyFlags::NoValidate);
                        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state )
                            return res;

                        const unsigned int columnCount =
                            state->GetColumnCount();

                        if ( splitterHit < 0 ||
                             static_cast<unsigned int>(splitterHit + 1) >=
                                 columnCount )
                        {
                            return res;
                        }

                        if ( m_wndEditor )
                        {
                            wxWindow* const editor = m_wndEditor;
                            const wxWeakRef<wxWindow> weakEditor(editor);

                            // Changes must be committed here or the value
                            // won't be drawn correctly.
                            if ( !CommitChangesFromEditor() )
                                return res;
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount ||
                                 IsPropertyPendingRemoval(p) )
                            {
                                return res;
                            }

                            if ( !wxWeakWindowIsAvailableForCallbacks(
                                     weakEditor, editor) ||
                                 m_wndEditor != editor )
                                return res;
                        }

                        wxWindow* const beginPrimary = m_wndEditor;
                        wxWindow* const beginSecondary = m_wndEditor2;
                        const wxWeakRef<wxWindow> weakBeginPrimary(beginPrimary);
                        const wxWeakRef<wxWindow>
                            weakBeginSecondary(beginSecondary);
                        const auto beginEditorsAreValid =
                            [weakThis, weakBeginPrimary, weakBeginSecondary,
                             this, beginPrimary, beginSecondary]()
                            {
                                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                                    return false;

                                bool editorsAreValid = true;
                                if ( beginPrimary &&
                                     !wxWeakWindowIsAvailableForCallbacks(
                                         weakBeginPrimary, beginPrimary) )
                                {
                                    if ( m_wndEditor == beginPrimary )
                                        m_wndEditor = nullptr;
                                    editorsAreValid = false;
                                }
                                if ( beginSecondary &&
                                     !wxWeakWindowIsAvailableForCallbacks(
                                         weakBeginSecondary,
                                         beginSecondary) )
                                {
                                    if ( m_wndEditor2 == beginSecondary )
                                        m_wndEditor2 = nullptr;
                                    editorsAreValid = false;
                                }
                                return editorsAreValid &&
                                       m_wndEditor == beginPrimary &&
                                       m_wndEditor2 == beginSecondary;
                            };

                        wxPGGetPropertyGridTransientState(this).draggedState = state;
                        wxPGGetPropertyGridTransientState(this).draggedColumnCount = columnCount;
                        m_draggedSplitter = splitterHit;
                        m_startingSplitterX =
                            state->DoGetSplitterPosition(splitterHit);
                        const int dragOffset = x - m_startingSplitterX;
                        wxPGGetPropertyGridTransientState(this).dragOffset =
                            dragOffset;
                        // Preserve the legacy protected member for derived
                        // classes; internal geometry uses the full-width
                        // sidecar value above.
                        m_dragOffset = static_cast<signed char>(dragOffset);
                        wxPGGetPropertyGridTransientState(this).splitterDontCenterBeforeDrag =
                            state->m_dontCenterSplitter;
                        wxPGGetPropertyGridTransientState(this).splitterWasPreSetBeforeDrag =
                            state->m_isSplitterPreSet;
                        m_dragStatus = 1;
                        wxPGGetPropertyGridTransientState(this).splitterBeginDispatching = true;
                        wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;

                        // Commit is deliberately completed before BEGIN. Arm
                        // the transaction before dispatching BEGIN so that an
                        // accepted callback changing page/topology can still
                        // be paired with END by FinishSplitterDrag().
                        const bool vetoed =
                            SendEvent(wxEVT_PG_COL_BEGIN_DRAG,
                                      p, nullptr,
                                      wxPGSelectPropertyFlags::Null,
                                      (unsigned int)splitterHit);
                        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                            return res;
                        wxPGGetPropertyGridTransientState(this).splitterBeginDispatching = false;
                        const bool editorsSurvivedBegin =
                            beginEditorsAreValid();

                        if ( vetoed )
                        {
                            // A vetoed BEGIN never starts a public drag and
                            // therefore has no matching END.
                            if ( m_dragStatus &&
                                 wxPGGetPropertyGridTransientState(this).draggedState == state &&
                                 m_draggedSplitter == splitterHit )
                            {
                                m_dragStatus = 0;
                                m_draggedSplitter = -1;
                                wxPGGetPropertyGridTransientState(this).draggedState = nullptr;
                                wxPGGetPropertyGridTransientState(this).draggedColumnCount = 0;
                                m_dragOffset = 0;
                                wxPGGetPropertyGridTransientState(this).dragOffset = 0;
                                wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = false;
                            }
                            wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;
                            return res;
                        }

                        if ( !editorsSurvivedBegin )
                        {
                            FinishSplitterDrag(true);
                            return res;
                        }

                        // A public handler can switch pages or change the
                        // number of columns. Never apply the old splitter
                        // index to that new topology.
                        if ( wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated )
                        {
                            FinishSplitterDrag(true);
                            return res;
                        }
                        if ( m_dragStatus == 0 ||
                             m_pState != state ||
                             state->GetColumnCount() != columnCount ||
                             IsPropertyPendingRemoval(p) ||
                             wxWindowIsUnavailableForCallbacks(this) )
                        {
                            if ( m_dragStatus )
                                FinishSplitterDrag(true);
                            return res;
                        }

                        if ( beginPrimary )
                        {
                            beginPrimary->Show(false);
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                                return res;
                            if ( !beginEditorsAreValid() ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount ||
                                 m_draggedSplitter != splitterHit )
                            {
                                FinishSplitterDrag(true);
                                return res;
                            }
                            wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = true;
                        }

                        if ( beginSecondary )
                        {
                            beginSecondary->Hide();
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                                 !beginEditorsAreValid() ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount ||
                                 m_draggedSplitter != splitterHit )
                            {
                                FinishSplitterDrag(true);
                                return res;
                            }
                            wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = true;
                        }

                        if ( !(m_iFlags & wxPG_FL_MOUSE_CAPTURED) )
                        {
                            CaptureMouse();
                            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                                return res;
                            if ( !HasCapture() ||
                                 !beginEditorsAreValid() ||
                                 m_dragStatus == 0 ||
                                 wxPGGetPropertyGridTransientState(this).draggedState != state ||
                                 m_pState != state ||
                                 state->GetColumnCount() != columnCount ||
                                 m_draggedSplitter != splitterHit )
                            {
                                FinishSplitterDrag(true);
                                return res;
                            }
                            m_iFlags |= wxPG_FL_MOUSE_CAPTURED;
                        }
                    }
                }
            }
        }
        else
        {
        // Click on margin.
            if ( p->GetChildCount() )
            {
                int nx = x + m_marginWidth - marginEnds; // Normalize x.

                // Fine tune cell button x
                if ( !p->IsCategory() )
                    nx -= IN_CELL_EXPANDER_BUTTON_X_ADJUST;

                if ( nx >= m_gutterWidth && nx < (m_gutterWidth+m_iconWidth) )
                {
                    int y2 = y % m_lineHeight;
                    if ( y2 >= m_buttonSpacingY && y2 < (m_buttonSpacingY+m_iconHeight) )
                    {
                        // On click on expander button, expand/collapse
                        if ( p->IsExpanded() )
                            DoCollapse( p, true );
                        else
                            DoExpand( p, true );
                    }
                }
            }
        }
    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseRightClick( int WXUNUSED(x),
                                            unsigned int WXUNUSED(y),
                                            wxMouseEvent& event )
{
    if ( m_propHover )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const state = m_pState;
        if ( !AddToSelectionFromInputEvent(p, m_colHover, &event) ||
             !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
             IsPropertyPendingRemoval(p) )
        {
            return false;
        }

        // Send right click event.
        SendEvent( wxEVT_PG_RIGHT_CLICK, p );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseDoubleClick( int WXUNUSED(x),
                                             unsigned int WXUNUSED(y),
                                             wxMouseEvent& event )
{
    if ( m_propHover )
    {
        // Select property here as well
        wxPGProperty* const p = m_propHover;
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const state = m_pState;
        if ( !AddToSelectionFromInputEvent(p, m_colHover, &event) ||
             !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
             IsPropertyPendingRemoval(p) )
        {
            return false;
        }

        // Send double-click event.
        SendEvent( wxEVT_PG_DOUBLE_CLICK, p );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseMove( int x, unsigned int y,
                                      wxMouseEvent &event )
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveMouseMoveCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveMouseMoveCallback);

    // Safety check (needed because mouse capturing may
    // otherwise freeze the control)
    if ( m_dragStatus > 0 && !event.Dragging() )
    {
        HandleMouseUp(x, y, event);
        return false;
    }

    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return false;
    int splitterHit = -1;
    int splitterHitOffset = 0;
    int columnHit = state->HitTestH( x, &splitterHit, &splitterHitOffset );
    x = state->GetColumnLogicalX(x);

    #if wxUSE_TOOLTIPS
        wxPGProperty* prevHover = m_propHover;
        int prevCol = m_colHover;
    #endif
    m_colHover = columnHit;

    if ( m_dragStatus > 0 )
    {
        if ( state != wxPGGetPropertyGridTransientState(this).draggedState ||
             state->GetColumnCount() != wxPGGetPropertyGridTransientState(this).draggedColumnCount ||
             m_draggedSplitter < 0 ||
             static_cast<unsigned int>(m_draggedSplitter + 1) >=
                 wxPGGetPropertyGridTransientState(this).draggedColumnCount )
        {
            FinishSplitterDrag(true);
            return false;
        }

        wxWindow* const dragPrimary = m_wndEditor;
        wxWindow* const dragSecondary = m_wndEditor2;
        const wxWeakRef<wxWindow> weakDragPrimary(dragPrimary);
        const wxWeakRef<wxWindow> weakDragSecondary(dragSecondary);
        const auto dragEditorsAreValid =
            [weakThis, weakDragPrimary, weakDragSecondary,
             this, dragPrimary, dragSecondary]()
            {
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return false;

                bool editorsAreValid = true;
                if ( dragPrimary &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakDragPrimary, dragPrimary)) )
                {
                    if ( m_wndEditor == dragPrimary )
                        m_wndEditor = nullptr;
                    editorsAreValid = false;
                }
                if ( dragSecondary &&
                     (!wxWeakWindowIsAvailableForCallbacks(weakDragSecondary, dragSecondary)) )
                {
                    if ( m_wndEditor2 == dragSecondary )
                        m_wndEditor2 = nullptr;
                    editorsAreValid = false;
                }

                return editorsAreValid &&
                       m_wndEditor == dragPrimary &&
                       m_wndEditor2 == dragSecondary;
            };

        const int dragMargin = FromDIP(wxPG_DRAG_MARGIN);

        if ( x > (m_marginWidth + dragMargin) &&
             x < (m_pState->GetVirtualWidth() - dragMargin) )
        {

            const int splitterX =
                state->DoGetSplitterPosition(m_draggedSplitter);
            const int newSplitterX =
                x - wxPGGetPropertyGridTransientState(this).dragOffset;

            // Splitter redraw required?
            if ( newSplitterX != splitterX )
            {
                wxPGProperty* const hover = m_propHover;
                const int draggedSplitter = m_draggedSplitter;

                // Move everything
                DoSetSplitter(newSplitterX,
                              draggedSplitter,
                              wxPGSplitterPositionFlags::Refresh |
                              wxPGSplitterPositionFlags::FromEvent);
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return false;
                if ( !dragEditorsAreValid() ||
                     m_pState != state ||
                     state->GetColumnCount() != wxPGGetPropertyGridTransientState(this).draggedColumnCount ||
                     m_draggedSplitter != draggedSplitter ||
                     (hover &&
                      (hover->GetParentState() != state ||
                       IsPropertyPendingRemoval(hover) ||
                       hover->HasFlag(wxPGFlags::BeingDeleted))) )
                {
                    FinishSplitterDrag(true);
                    return false;
                }

                m_dragStatus = 2;
                SendEvent(wxEVT_PG_COLS_RESIZED, wxNullProperty);
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return false;
                if ( !dragEditorsAreValid() ||
                     m_pState != state ||
                     state->GetColumnCount() != wxPGGetPropertyGridTransientState(this).draggedColumnCount ||
                     m_draggedSplitter != draggedSplitter ||
                     (hover &&
                      (hover->GetParentState() != state ||
                       IsPropertyPendingRemoval(hover) ||
                       hover->HasFlag(wxPGFlags::BeingDeleted))) )
                {
                    FinishSplitterDrag(true);
                    return false;
                }

                SendEvent(wxEVT_PG_COL_DRAGGING,
                          hover,
                          nullptr,
                    wxPGSelectPropertyFlags::NoValidate,
                          (unsigned int)draggedSplitter);
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return false;
                if ( !dragEditorsAreValid() ||
                     m_pState != state ||
                     state->GetColumnCount() != wxPGGetPropertyGridTransientState(this).draggedColumnCount ||
                     m_draggedSplitter != draggedSplitter ||
                     (hover &&
                      (hover->GetParentState() != state ||
                       IsPropertyPendingRemoval(hover) ||
                       hover->HasFlag(wxPGFlags::BeingDeleted))) )
                {
                    FinishSplitterDrag(true);
                    return false;
                }
            }
        }

        return false;
    }
    else
    {

        int ih = m_lineHeight;
        int sy = y;

        int curPropHoverY = y - (y % ih);

        // On which item it hovers
        if ( !m_propHover
             ||
             ( sy < m_propHoverY || sy >= (m_propHoverY+ih) )
           )
        {
            // Mouse moves on another property

            m_propHover = DoGetItemAtY(y);
            m_propHoverY = curPropHoverY;

            // Send hover event
            SendEvent( wxEVT_PG_HIGHLIGHTED, m_propHover );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state ||
                 (m_propHover && IsPropertyPendingRemoval(m_propHover)) )
                return false;
        }

#if wxUSE_TOOLTIPS
        //
        // If tooltips are enabled, show label or value as a tip
        // in case it doesn't otherwise show in full length.
        //
        if ( m_windowStyle & wxPG_TOOLTIPS )
        {
            if ( m_propHover != prevHover || prevCol != m_colHover )
            {
                wxPGProperty* const hover = m_propHover;
                const int hoverColumn = m_colHover;
                const auto gridTransactionIsValid =
                    [weakThis, this, state]()
                    {
                        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                               m_pState == state;
                    };
                const auto hoverTransactionIsValid =
                    [gridTransactionIsValid, this, state, hover,
                     hoverColumn]()
                    {
                        return gridTransactionIsValid() &&
                               m_propHover == hover &&
                               m_colHover == hoverColumn &&
                               hover->GetParentState() == state &&
                               !IsPropertyPendingRemoval(hover);
                    };

                if ( hover && !hover->IsCategory() )
                {
                    if ( !hoverTransactionIsValid() )
                        return false;

                    if ( HasExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS) )
                    {
                        // Show help string as a tooltip
                        const wxString tipString = hover->GetHelpString();
                        if ( !hoverTransactionIsValid() )
                            return false;

                        SetToolTip(tipString);
                        if ( !hoverTransactionIsValid() )
                            return false;
                    }
                    else if ( hoverColumn >= 0 &&
                              hoverColumn <
                                  static_cast<int>(state->GetColumnCount()) )
                    {
                        // Show cropped value string as a tooltip
                        wxString tipString;
                        wxPGCell cell;
                        const int item = hoverColumn == 1
                                             ? hover->GetChoiceSelection()
                                             : -1;
                        if ( !hoverTransactionIsValid() )
                            return false;
                        hover->GetDisplayInfo(hoverColumn,
                                              item,
                                              0,
                                              &tipString,
                                              &cell);
                        if ( !hoverTransactionIsValid() )
                            return false;
                        int space = state->GetColumnWidth(hoverColumn);

                        int imageWidth = 0;
                        const wxBitmapBundle cellBitmap = cell.GetBitmap();
                        const wxBitmap bmp = cellBitmap.GetBitmapFor(this);
                        if ( !hoverTransactionIsValid() )
                            return false;
                        if ( bmp.IsOk() )
                        {
                            imageWidth = bmp.GetWidth();
                            int hMax = m_lineHeight - wxPG_CUSTOM_IMAGE_SPACINGY - 1;
                            if ( bmp.GetHeight() > hMax )
                                imageWidth = int(double(imageWidth) * hMax / bmp.GetHeight());
                        }

                        if ( hoverColumn == 0 )
                        {
                            if ( !(m_windowStyle & wxPG_HIDE_CATEGORIES) ||
                                 hover->GetParent() != state->DoGetRoot() )
                            {
                                space -= (hover->GetDepth() - 1) *
                                         m_subgroup_extramargin;
                            }
                        }
                        else if ( hoverColumn == 1 &&
                                  !hover->IsValueUnspecified() )
                        {
                            wxSize imageSize;
                            if ( !TryGetImageSize(hover,
                                                  -1,
                                                  &imageSize) ||
                                 !hoverTransactionIsValid() )
                            {
                                return false;
                            }
                            if ( imageSize.x > 0 )
                                imageWidth = imageSize.x;
#if WXWIN_COMPATIBILITY_3_2
                            // Special implementation with check if user-overriden obsolete function is still in use
                            tipString = hover->GetValueAsStringWithCheck();
#else
                            tipString = hover->GetValueAsString();
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
                            if ( !hoverTransactionIsValid() )
                                return false;
                            if ( GetColumnCount() <= 2 )
                            {
                                const wxString unitsString =
                                    hover->GetAttribute(wxPG_ATTR_UNITS,
                                                        wxString());
                                if ( !hoverTransactionIsValid() )
                                    return false;
                                if ( !unitsString.empty() )
                                    tipString = wxString::Format(wxS("%s %s"), tipString, unitsString );
                            }
                        }

                        space -= hover->GetImageOffset(imageWidth);
                        if ( !hoverTransactionIsValid() )
                            return false;
                        space -= (wxPG_XBEFORETEXT + 1);
                        int tw, th;
                        const wxFont* font = nullptr;
                        if ( (m_windowStyle & wxPG_BOLD_MODIFIED) &&
                             hover->HasFlag(wxPGFlags::Modified) )
                            font = &m_captionFont;
                        if ( cell.GetFont().IsOk() )
                            font = &cell.GetFont();
                        GetTextExtent( tipString, &tw, &th, nullptr, nullptr, font );
                        if ( tw > space )
                        {
                            SetToolTip( tipString );
                            if ( !hoverTransactionIsValid() )
                                return false;
                        }
                    }
                    else
                    {
                        SetToolTip(wxString());
                        if ( !hoverTransactionIsValid() )
                            return false;
                    }
                }
                else
                {
                    SetToolTip(wxString());
                    if ( !gridTransactionIsValid() )
                        return false;
                }
            }
        }
#endif // wxUSE_TOOLTIPS

        if ( splitterHit == -1 ||
             !m_propHover ||
             HasFlag(wxPG_STATIC_SPLITTER) )
        {
            // hovering on something else
            if ( m_curcursor != wxCURSOR_ARROW )
                CustomSetCursor( wxCURSOR_ARROW );
        }
        else
        {
            // Do not allow splitter cursor on caption items.
            // (also not if we were dragging and it's started
            // outside the splitter region)

            if ( !m_propHover->IsCategory() &&
                 !event.Dragging() )
            {

                // hovering on splitter

                // NB: Condition disabled since MouseLeave event (from the
                //     editor control) cannot be reliably detected.
                //if ( m_curcursor != wxCURSOR_SIZEWE )
                CustomSetCursor( wxCURSOR_SIZEWE, true );

                return false;
            }
            else
            {
                // hovering on something else
                if ( m_curcursor != wxCURSOR_ARROW )
                    CustomSetCursor( wxCURSOR_ARROW );
            }
        }

        //
        // Multi select by dragging
        //
        if ( HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) &&
             event.LeftIsDown() &&
             m_propHover &&
             GetSelection() &&
             columnHit != 1 &&
             !state->DoIsPropertySelected(m_propHover) )
        {
            // Additional requirement is that the hovered property
            // is adjacent to edges of selection.
            const wxArrayPGProperty selection = GetSelectedProperties();
            wxPGProperty* const hover = m_propHover;

            // Since categories cannot be selected along with 'other'
            // properties, exclude them from iterator flags.
            int iterFlags = wxPG_ITERATE_VISIBLE & (~wxPGFlags::Category);

            for ( int i=(selection.size()-1); i>=0; i-- )
            {
                // TODO: This could be optimized by keeping track of
                //       which properties are at the edges of selection.
                wxPGProperty* selProp = selection[i];
                if ( state->ArePropertiesAdjacent(hover, selProp,
                                                  iterFlags) )
                {
                    if ( !DoAddToSelection(hover) )
                        return false;
                    break;
                }
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::MoveSplitterFromKeyboard( int splitter, int delta )
{
    if ( delta == 0 || wxWindowIsUnavailableForCallbacks(this) )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveKeyboardSplitter = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveKeyboardSplitter);

    if ( m_dragStatus )
    {
        FinishSplitterDrag(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return false;
        if ( m_dragStatus )
            return true;
    }

    wxPropertyGridPageState* const state = m_pState;
    if ( !state )
        return false;

    const unsigned int columnCount = state->GetColumnCount();
    if ( splitter < 0 ||
         static_cast<unsigned int>(splitter + 1) >= columnCount )
    {
        return false;
    }

    wxPGProperty* const selected = GetSelection();
    if ( m_wndEditor )
    {
        if ( !CommitChangesFromEditor() )
            return true;
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             m_pState != state ||
             state->GetColumnCount() != columnCount ||
             (selected && (GetSelection() != selected ||
                           IsPropertyPendingRemoval(selected))) )
        {
            return true;
        }
    }

    wxWindow* const keyboardPrimary = m_wndEditor;
    wxWindow* const keyboardSecondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakKeyboardPrimary(keyboardPrimary);
    const wxWeakRef<wxWindow> weakKeyboardSecondary(keyboardSecondary);
    const auto keyboardEditorsAreValid =
        [weakThis, weakKeyboardPrimary, weakKeyboardSecondary,
         this, keyboardPrimary, keyboardSecondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            bool editorsAreValid = true;
            if ( keyboardPrimary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakKeyboardPrimary, keyboardPrimary)) )
            {
                if ( m_wndEditor == keyboardPrimary )
                    m_wndEditor = nullptr;
                editorsAreValid = false;
            }
            if ( keyboardSecondary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakKeyboardSecondary, keyboardSecondary)) )
            {
                if ( m_wndEditor2 == keyboardSecondary )
                    m_wndEditor2 = nullptr;
                editorsAreValid = false;
            }

            return editorsAreValid &&
                   m_wndEditor == keyboardPrimary &&
                   m_wndEditor2 == keyboardSecondary;
        };

    wxPGGetPropertyGridTransientState(this).draggedState = state;
    wxPGGetPropertyGridTransientState(this).draggedColumnCount = columnCount;
    m_draggedSplitter = splitter;
    m_startingSplitterX = state->DoGetSplitterPosition(splitter);
    m_dragOffset = 0;
    wxPGGetPropertyGridTransientState(this).dragOffset = 0;
    wxPGGetPropertyGridTransientState(this).splitterDontCenterBeforeDrag = state->m_dontCenterSplitter;
    wxPGGetPropertyGridTransientState(this).splitterWasPreSetBeforeDrag = state->m_isSplitterPreSet;
    wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = false;
    m_dragStatus = 1;
    wxPGGetPropertyGridTransientState(this).splitterBeginDispatching = true;
    wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;

    const bool vetoed =
        SendEvent(wxEVT_PG_COL_BEGIN_DRAG,
                  selected,
                  nullptr,
                  wxPGSelectPropertyFlags::Null,
                  static_cast<unsigned int>(splitter));
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return true;
    wxPGGetPropertyGridTransientState(this).splitterBeginDispatching = false;
    const bool editorsSurvivedBegin = keyboardEditorsAreValid();

    if ( vetoed )
    {
        if ( m_dragStatus &&
             wxPGGetPropertyGridTransientState(this).draggedState == state &&
             m_draggedSplitter == splitter )
        {
            m_dragStatus = 0;
            m_draggedSplitter = -1;
            wxPGGetPropertyGridTransientState(this).draggedState = nullptr;
            wxPGGetPropertyGridTransientState(this).draggedColumnCount = 0;
            m_dragOffset = 0;
            wxPGGetPropertyGridTransientState(this).dragOffset = 0;
            wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = false;
        }
        wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;
        return true;
    }

    if ( !editorsSurvivedBegin )
    {
        FinishSplitterDrag(true);
        return true;
    }

    if ( wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated )
    {
        FinishSplitterDrag(true);
        return true;
    }

    if ( m_dragStatus == 0 || wxWindowIsUnavailableForCallbacks(this) ||
         m_pState != state ||
         state->GetColumnCount() != columnCount ||
         (selected && (GetSelection() != selected ||
                       IsPropertyPendingRemoval(selected))) )
    {
        if ( m_dragStatus )
            FinishSplitterDrag(true);
        return true;
    }

    const int oldPosition = state->DoGetSplitterPosition(splitter);
    DoSetSplitter(oldPosition + delta,
                  splitter,
                  wxPGSplitterPositionFlags::Refresh |
                  wxPGSplitterPositionFlags::FromEvent);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return true;

    if ( !keyboardEditorsAreValid() ||
         wxWindowIsUnavailableForCallbacks(this) || m_pState != state ||
         state->GetColumnCount() != columnCount ||
         m_draggedSplitter != splitter ||
         (selected && (GetSelection() != selected ||
                       IsPropertyPendingRemoval(selected))) )
    {
        if ( m_dragStatus )
            FinishSplitterDrag(true);
        return true;
    }

    if ( state->DoGetSplitterPosition(splitter) != oldPosition )
    {
        m_dragStatus = 2;
        SendEvent(wxEVT_PG_COLS_RESIZED, wxNullProperty);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return true;
        if ( !keyboardEditorsAreValid() ||
             wxWindowIsUnavailableForCallbacks(this) || m_pState != state ||
             state->GetColumnCount() != columnCount ||
             m_draggedSplitter != splitter ||
             (selected && (GetSelection() != selected ||
                           IsPropertyPendingRemoval(selected))) )
        {
            if ( m_dragStatus )
                FinishSplitterDrag(true);
            return true;
        }

        SendEvent(wxEVT_PG_COL_DRAGGING,
                  selected,
                  nullptr,
                  wxPGSelectPropertyFlags::NoValidate,
                  static_cast<unsigned int>(splitter));
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return true;
        if ( !keyboardEditorsAreValid() ||
             wxWindowIsUnavailableForCallbacks(this) || m_pState != state ||
             state->GetColumnCount() != columnCount ||
             m_draggedSplitter != splitter ||
             (selected && (GetSelection() != selected ||
                           IsPropertyPendingRemoval(selected))) )
        {
            if ( m_dragStatus )
                FinishSplitterDrag(true);
            return true;
        }
    }

    FinishSplitterDrag(false);
    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::FinishSplitterDrag( bool cancel )
{
    // BEGIN is still deciding whether the public transaction exists. A
    // topology mutator may need to retire the provisional drag during that
    // callback, but END must not be emitted until we know BEGIN wasn't vetoed.
    if ( wxPGGetPropertyGridTransientState(this).splitterBeginDispatching )
    {
        wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = true;
        return;
    }

    if ( m_dragStatus == 0 )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        wxWindow* const primary = m_wndEditor;
        wxWindow* const secondary = m_wndEditor2;
        const wxWeakRef<wxWindow> weakPrimary(primary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;
        const bool releaseCapture =
            m_iFlags & wxPG_FL_MOUSE_CAPTURED;
        m_iFlags &= ~wxPG_FL_MOUSE_CAPTURED;
        if ( releaseCapture && HasCapture() )
        {
            ReleaseMouse();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
            if ( primary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPrimary, primary)) &&
                 m_wndEditor == primary )
            {
                m_wndEditor = nullptr;
            }
            if ( secondary &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondary, secondary)) &&
                 m_wndEditor2 == secondary )
            {
                m_wndEditor2 = nullptr;
            }
        }
        return;
    }

    wxPropertyGridPageState* const state = wxPGGetPropertyGridTransientState(this).draggedState;
    const unsigned int columnCount = wxPGGetPropertyGridTransientState(this).draggedColumnCount;
    const int splitter = m_draggedSplitter;
    const int startingPosition = m_startingSplitterX;
    const bool restoreDontCenter = wxPGGetPropertyGridTransientState(this).splitterDontCenterBeforeDrag;
    const bool restoreWasPreSet = wxPGGetPropertyGridTransientState(this).splitterWasPreSetBeforeDrag;
    const bool editorsWereHidden = wxPGGetPropertyGridTransientState(this).splitterEditorsHidden;
    const bool topologyWasInvalidated = wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated;
    const auto topologyStillExists =
        [state, columnCount, splitter, topologyWasInvalidated]()
    {
        return !topologyWasInvalidated &&
               state &&
               state->GetColumnCount() == columnCount &&
               splitter >= 0 &&
               static_cast<unsigned int>(splitter + 1) < columnCount;
    };
    const auto topologyIsCurrent =
        [this, state, topologyStillExists]()
    {
        return m_pState == state && topologyStillExists();
    };

    // Publish the idle state before releasing capture or sending an event:
    // both operations can synchronously re-enter the grid.
    m_dragStatus = 0;
    m_draggedSplitter = -1;
    wxPGGetPropertyGridTransientState(this).draggedState = nullptr;
    wxPGGetPropertyGridTransientState(this).draggedColumnCount = 0;
    m_dragOffset = 0;
    wxPGGetPropertyGridTransientState(this).dragOffset = 0;
    wxPGGetPropertyGridTransientState(this).splitterEditorsHidden = false;
    wxPGGetPropertyGridTransientState(this).splitterBeginDispatching = false;
    wxPGGetPropertyGridTransientState(this).splitterBeginInvalidated = false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxWindow* const primaryAtFinish = m_wndEditor;
    wxWindow* const secondaryAtFinish = m_wndEditor2;
    const wxWeakRef<wxWindow> weakPrimaryAtFinish(primaryAtFinish);
    const wxWeakRef<wxWindow> weakSecondaryAtFinish(secondaryAtFinish);
    const auto sanitizeEditorMembers =
        [weakThis, weakPrimaryAtFinish, weakSecondaryAtFinish,
         this, primaryAtFinish, secondaryAtFinish]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            if ( primaryAtFinish &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakPrimaryAtFinish, primaryAtFinish)) &&
                 m_wndEditor == primaryAtFinish )
            {
                m_wndEditor = nullptr;
            }
            if ( secondaryAtFinish &&
                 (!wxWeakWindowIsAvailableForCallbacks(weakSecondaryAtFinish, secondaryAtFinish)) &&
                 m_wndEditor2 == secondaryAtFinish )
            {
                m_wndEditor2 = nullptr;
            }

            return true;
        };
    const auto primaryCanBeRestored =
        [weakPrimaryAtFinish, this, primaryAtFinish]()
        {
            return primaryAtFinish &&
                   wxWeakWindowIsAvailableForCallbacks(weakPrimaryAtFinish, primaryAtFinish) &&
                   m_wndEditor == primaryAtFinish;
        };
    const auto secondaryCanBeRestored =
        [weakSecondaryAtFinish, this, secondaryAtFinish]()
        {
            return secondaryAtFinish &&
                   wxWeakWindowIsAvailableForCallbacks(weakSecondaryAtFinish, secondaryAtFinish) &&
                   m_wndEditor2 == secondaryAtFinish;
        };
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveSplitterCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveSplitterCallback);

    if ( topologyStillExists() )
    {
        if ( cancel )
        {
            state->DoSetSplitter(
                startingPosition,
                splitter,
                wxPGSplitterPositionFlags::FromAutoCenter);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
            state->m_dontCenterSplitter = restoreDontCenter;
            state->m_isSplitterPreSet = restoreWasPreSet;
            if ( topologyIsCurrent() )
            {
                CorrectEditorWidgetSizeX();
                if ( !sanitizeEditorMembers() )
                    return;
                Refresh();
                if ( !sanitizeEditorMembers() )
                    return;
            }
        }
        else if ( abs(startingPosition -
                      state->DoGetSplitterPosition(splitter)) > 1 )
        {
            state->m_dontCenterSplitter = true;
        }
    }

    // Clear the flag first so the capture-changed event generated by a normal
    // ReleaseMouse() cannot be mistaken for an external cancellation.
    const bool releaseCapture = m_iFlags & wxPG_FL_MOUSE_CAPTURED;
    m_iFlags &= ~wxPG_FL_MOUSE_CAPTURED;
    if ( releaseCapture && HasCapture() )
    {
        ReleaseMouse();
        if ( !sanitizeEditorMembers() )
            return;
    }

    CustomSetCursor(wxCURSOR_ARROW);
    if ( !sanitizeEditorMembers() )
        return;

    wxPGProperty* const selected = GetSelection();
    if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) && selected &&
         m_pState == state &&
         selected->GetParentState() == state &&
         !IsPropertyPendingRemoval(selected) &&
         !selected->HasFlag(wxPGFlags::BeingDeleted) )
    {
        DrawItem(selected);
        if ( !sanitizeEditorMembers() )
            return;
    }

    if ( editorsWereHidden && primaryCanBeRestored() )
    {
        primaryAtFinish->Show(true);
        if ( !sanitizeEditorMembers() )
            return;
    }
    if ( editorsWereHidden && secondaryCanBeRestored() )
    {
        secondaryAtFinish->Show(true);
        if ( !sanitizeEditorMembers() )
            return;
    }

    m_editorFocused = false;

    // BEGIN was accepted, so END is unconditional even if its handler changed
    // page or column topology. The column number is the original transaction
    // identity and no stale property pointer is exposed.
    SendEvent(wxEVT_PG_COL_END_DRAG,
              wxNullProperty,
              nullptr,
              wxPGSelectPropertyFlags::NoValidate,
              static_cast<unsigned int>(splitter));
    sanitizeEditorMembers();
}

// -----------------------------------------------------------------------

// Also handles Leaving event
bool wxPropertyGrid::HandleMouseUp( int WXUNUSED(x), unsigned int WXUNUSED(y),
                                    wxMouseEvent &WXUNUSED(event) )
{
    // No event type check: calling this method always ends the transaction.
    FinishSplitterDrag(false);
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnMouseCommon( wxMouseEvent& event, int* px, int* py )
{
    const wxWeakRef<wxWindow> weakThis(this);
    int splitterX = GetSplitterPosition();

    int ux, uy;
    CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );
    // ux and child rectangles are in the backend's physical client
    // coordinates. Horizontal handlers receive the same physical x and
    // convert it exactly once after HitTestH().
    splitterX = m_pState->GetColumnLogicalX(splitterX);

    wxWindow* wnd = GetEditorControl();

    // Hide popup on clicks
    if ( event.GetEventType() != wxEVT_MOTION )
        if ( wxDynamicCast(wnd, wxOwnerDrawnComboBox) )
        {
            ((wxOwnerDrawnComboBox*)wnd)->HidePopup();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;
            wnd = GetEditorControl();
        }

    wxRect r;
    if ( wnd )
        r = wnd->GetRect();
    if ( wnd == nullptr || m_dragStatus ||
         (
           ux <= (splitterX + FromDIP(wxPG_SPLITTERX_DETECTMARGIN2)) ||
           ux >= (r.x+r.width) ||
           event.m_y < r.y ||
           event.m_y >= (r.y+r.height)
         )
       )
    {
        *px = ux;
        *py = uy;
        return true;
    }
    else
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseClick( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        if ( !HandleMouseClick(x, y, event) )
            event.Skip();
    }
    else
    {
        event.Skip();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseRightClick( wxMouseEvent &event )
{
    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseRightClick(x,y,event);
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseDoubleClick( wxMouseEvent &event )
{
    const wxWeakRef<wxWindow> weakThis(this);

    // Always run standard mouse-down handler as well
    OnMouseClick(event);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseDoubleClick(x,y,event);

    // Do not Skip() event here - OnMouseClick() call above
    // should have already taken care of it.
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseMove( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        HandleMouseMove(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseUp( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        if ( !HandleMouseUp(x, y, event) )
            event.Skip();
    }
    else
    {
        event.Skip();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseEntry( wxMouseEvent &event )
{
    // This may get called from child control as well, so event's
    // mouse position cannot be relied on.

    if ( event.Entering() )
    {
        if ( !(m_iFlags & wxPG_FL_MOUSE_INSIDE) )
        {
            // TODO: Fix this (detect parent and only do
            //   cursor trick if it is a manager).
            wxASSERT( GetParent() );
            GetParent()->SetCursor(wxNullCursor);

            m_iFlags |= wxPG_FL_MOUSE_INSIDE;
        }
        else
            GetParent()->SetCursor(wxNullCursor);
    }
    else if ( event.Leaving() )
    {
        // Without this, wxSpinCtrl editor will sometimes have wrong cursor
        SetCursor( wxNullCursor );

        // Get real cursor position
        wxPoint pt = ScreenToClient(::wxGetMousePosition());

        if ( pt.x <= 0 || pt.y <= 0 || pt.x >= m_width || pt.y >= m_height )
        {
            {
                if ( m_iFlags & wxPG_FL_MOUSE_INSIDE )
                {
                    m_iFlags &= ~(wxPG_FL_MOUSE_INSIDE);
                }

                if ( m_dragStatus )
                    wxPropertyGrid::HandleMouseUp ( -1, 10000, event );
            }
        }
    }

    event.Skip();
}

// -----------------------------------------------------------------------

// Common code used by various OnMouseXXXChild methods.
bool wxPropertyGrid::OnMouseChildCommon( wxMouseEvent &event, int* px, int *py )
{
    wxWindow* topCtrlWnd = (wxWindow*)event.GetEventObject();
    wxASSERT( topCtrlWnd );
    int x, y;
    event.GetPosition(&x,&y);

    int splitterX = GetSplitterPosition();

    wxRect r = topCtrlWnd->GetRect();
    int ux, uy;
    CalcUnscrolledPosition(r.x + x, r.y + y, &ux, &uy);
    ux = m_pState->GetColumnLogicalX(ux);

    if ( !m_dragStatus &&
         ux > (splitterX + FromDIP(wxPG_SPLITTERX_DETECTMARGIN2)) &&
         y >= 0 && y < r.height
       )
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
        event.Skip();
    }
    else
    {
        if ( px )
        {
            *px = ux;
        }
        if ( py )
        {
            *py = uy;
        }
        return true;
    }
    return false;
}

void wxPropertyGrid::OnMouseClickChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseClick(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseRightClickChild( wxMouseEvent &event )
{
    int x,y;
    wxASSERT( m_wndEditor );
    // These coords may not be exact (about +-2),
    // but that should not matter (right click is about item, not position).
    wxPoint pt = m_wndEditor->GetPosition();
    CalcUnscrolledPosition( event.m_x + pt.x, event.m_y + pt.y, &x, &y );

    // FIXME: Used to set m_propHover to selection here. Was it really
    //        necessary?

    bool res = HandleMouseRightClick(x,y,event);
    if ( !res ) event.Skip();
}

void wxPropertyGrid::OnMouseMoveChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseMove(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseUpChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseUp(x,y,event);
        if ( !res ) event.Skip();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid keyboard event handling
// -----------------------------------------------------------------------

std::pair<wxPGKeyboardAction, wxPGKeyboardAction> wxPropertyGrid::KeyEventToActions(const wxKeyEvent& event) const
{
    // Translates wxKeyEvent to wxPGKeyboardAction::XXX

    int keycode = event.GetKeyCode();
    int modifiers = event.GetModifiers();

    wxASSERT(!(modifiers & ~(0xFFFF)));

    int hashMapKey = (keycode & 0xFFFF) | ((modifiers & 0xFFFF) << 16);

    auto it = m_actionTriggers.find(hashMapKey);

    if ( it == m_actionTriggers.end() )
        return std::make_pair(wxPGKeyboardAction::Invalid, wxPGKeyboardAction::Invalid);

    return it->second;
}

#if WXWIN_COMPATIBILITY_3_2
wxPGKeyboardAction wxPropertyGrid::KeyEventToActions(wxKeyEvent &event, wxPGKeyboardAction* pSecond) const
{
    // Translates wxKeyEvent to wxPGKeyboardAction::XXX
    std::pair<wxPGKeyboardAction, wxPGKeyboardAction> actions = KeyEventToActions(event);

    if ( pSecond )
    {
        *pSecond = actions.second;
    }

    return actions.first;
}
#endif // WXWIN_COMPATIBILITY_3_2

wxPGKeyboardAction wxPropertyGrid::KeyEventToAction(wxKeyEvent& event) const
{
    return KeyEventToActions(event).first;
}

void wxPropertyGrid::AddActionTrigger(wxPGKeyboardAction action, int keycode, int modifiers)
{
    wxASSERT( !(modifiers&~(0xFFFF)) );

    int hashMapKey = (keycode & 0xFFFF) | ((modifiers & 0xFFFF) << 16);

    std::pair<wxPGKeyboardAction, wxPGKeyboardAction> curActions;

    auto it = m_actionTriggers.find(hashMapKey);
    if ( it != m_actionTriggers.end() )
    {
        // This key combination is already used
        curActions = it->second;

        // Can add secondary?
        wxASSERT_MSG( curActions.second == wxPGKeyboardAction::Invalid,
                      "You can only add up to two separate actions per key combination." );

        curActions.second = action;
    }
    else
    {
        curActions = std::make_pair(action, wxPGKeyboardAction::Invalid);
    }

    m_actionTriggers[hashMapKey] = curActions;
}

void wxPropertyGrid::ClearActionTriggers(wxPGKeyboardAction action)
{
//    wxCHECK_RET(!(action & ~(0xFFFF)), wxS("You can only clear triggers for one action at a time.")

    auto it = m_actionTriggers.begin();
    while ( it != m_actionTriggers.end() )
    {
        if ( it->second.second == action )
        {
            it->second.second = wxPGKeyboardAction::Invalid;
        }

        if ( it->second.first == action )
        {
            if ( it->second.second == wxPGKeyboardAction::Invalid )
            {
                it = m_actionTriggers.erase(it);
                continue;
            }

            it->second.first = it->second.second;
        }

        ++it;
    }
}

void wxPropertyGrid::HandleKeyEvent( wxKeyEvent &event, bool fromChild )
{
    //
    // Handles key event when editor control is not focused.
    //

    wxCHECK_RET(!IsFrozen(), "wxPropertyGrid shouldn't be frozen");

    // Traversal between items, collapsing/expanding, etc.
    wxPGProperty* selected = GetSelection();
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const initialState = m_pState;
    const auto selectionIsCurrent =
        [weakThis, this, initialState](wxPGProperty* property)
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               m_pState == initialState &&
               GetSelection() == property &&
               (!property || !IsPropertyPendingRemoval(property));
    };
    int keycode = event.GetKeyCode();
    bool editorFocused = IsEditorFocused();

    // Keyboard-accessible splitter control. It is intentionally restricted to
    // the grid canvas so text/combo editors retain their normal Ctrl+Arrow
    // behaviour. Ctrl+Up/Down selects a boundary; Ctrl+Left/Right moves it by
    // 1 DIP, or 8 DIPs while Shift is held.
    if ( !fromChild && event.ControlDown() && !event.AltDown() &&
         !event.MetaDown() &&
         (keycode == WXK_UP || keycode == WXK_DOWN ||
          keycode == WXK_LEFT || keycode == WXK_RIGHT) )
    {
        const unsigned int columnCount = m_pState->GetColumnCount();
        if ( columnCount < 2 )
        {
            event.Skip();
            return;
        }

        const int lastSplitter = static_cast<int>(columnCount) - 2;
        if ( wxPGGetPropertyGridTransientState(this).keyboardSplitter < 0 || wxPGGetPropertyGridTransientState(this).keyboardSplitter > lastSplitter )
        {
            wxPGGetPropertyGridTransientState(this).keyboardSplitter =
                wxMin(wxMax(m_selColumn - 1, 0), lastSplitter);
        }

        if ( keycode == WXK_UP || keycode == WXK_DOWN )
        {
            const int direction = keycode == WXK_UP ? -1 : 1;
            wxPGGetPropertyGridTransientState(this).keyboardSplitter =
                wxMin(wxMax(wxPGGetPropertyGridTransientState(this).keyboardSplitter + direction, 0),
                      lastSplitter);
            Refresh();
            return;
        }

        int direction = keycode == WXK_LEFT ? -1 : 1;
        if ( GetLayoutDirection() == wxLayout_RightToLeft )
            direction = -direction;

        const int step = FromDIP(event.ShiftDown() ? 8 : 1);
        MoveSplitterFromKeyboard(wxPGGetPropertyGridTransientState(this).keyboardSplitter, direction * step);
        return;
    }

    if ( keycode == WXK_TAB )
    {
        if ( !event.ShiftDown() )
        {
            if ( !editorFocused && m_wndEditor )
            {
                DoSelectProperty( selected, wxPGSelectPropertyFlags::Focus );
            }
            else
            {
                // Tab traversal workaround for platforms on which
                // wxWindow::Navigate() may navigate into first child
                // instead of next sibling.
            #if defined(__WXGTK__)
                SetFocus();
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return;
            #endif

                Navigate(wxNavigationKeyEvent::IsForward);
            }
        }
        else
        {
            if ( editorFocused )
            {
                UnfocusEditor();
            }
            else
            {
            #if defined(__WXGTK__)
                SetFocus();
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                    return;
            #endif

                Navigate(wxNavigationKeyEvent::IsBackward);
            }
        }

        return;
    }

    // Ignore Alt and Control when they are down alone
    if ( keycode == WXK_ALT ||
         keycode == WXK_CONTROL )
    {
        event.Skip();
        return;
    }

    wxPGKeyboardAction action;
    wxPGKeyboardAction secondAction;
    std::tie(action, secondAction) = KeyEventToActions(event);

    if ( editorFocused && action == wxPGKeyboardAction::CancelEdit )
    {
        //
        // Esc cancels any changes
        if ( IsEditorsValueModified() )
        {
            EditorsValueWasNotModified();

            // Update the control as well
            const wxPGEditor* const editor = selected->GetEditorClass();
            if ( !selectionIsCurrent(selected) || !editor )
                return;

            wxWindow* const control = GetEditorControl();
            if ( !control )
                return;
            const wxWeakRef<wxWindow> weakControl(control);

            const wxString displayed = selected->GetDisplayedString();
            if ( !selectionIsCurrent(selected) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakControl, control) ||
                 GetEditorControl() != control )
            {
                return;
            }

            editor->SetControlStringValue(selected, control, displayed);
            if ( !selectionIsCurrent(selected) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakControl, control) ||
                 GetEditorControl() != control )
            {
                return;
            }
        }

        OnValidationFailureReset(selected);
        if ( !selectionIsCurrent(selected) )
            return;

        UnfocusEditor();
        return;
    }

    // Except for TAB, ESC, and any keys specifically dedicated to
    // wxPropertyGrid itself, handle child control events in child control.
    if ( fromChild && m_dedicatedKeys.count(keycode) == 0 )
    {
        // Only propagate event if it had modifiers
        if ( !event.HasModifiers() )
        {
            event.StopPropagation();
        }
        event.Skip();
        return;
    }

    bool wasHandled = false;

    if ( selected )
    {
        // Show dialog?
        if ( ButtonTriggerKeyTest(action, event) )
            return;

        wxPGProperty* p = selected;

        if ( action == wxPGKeyboardAction::Edit && !editorFocused )
        {
            // Mark as handled only for editable property
            if ( !p->IsCategory() && p->IsEnabled() && !p->HasFlag(wxPGFlags::ReadOnly) )
            {
                DoSelectProperty( p, wxPGSelectPropertyFlags::Focus );
                if ( !selectionIsCurrent(p) )
                    return;
                wasHandled = true;
            }
        }

        if ( !selectionIsCurrent(p) )
            return;

        // Travel and expand/collapse
        int selectDir = -2;

        if ( p->GetChildCount() )
        {
            if ( action == wxPGKeyboardAction::CollapseProperty || secondAction == wxPGKeyboardAction::CollapseProperty )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || DoCollapse(p, true) )
                    wasHandled = true;
                if ( !selectionIsCurrent(p) )
                    return;
            }
            else if ( action == wxPGKeyboardAction::ExpandProperty || secondAction == wxPGKeyboardAction::ExpandProperty )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || DoExpand(p, true) )
                    wasHandled = true;
                if ( !selectionIsCurrent(p) )
                    return;
            }
        }

        if ( !wasHandled )
        {
            if ( action == wxPGKeyboardAction::PrevProperty || secondAction == wxPGKeyboardAction::PrevProperty )
            {
                selectDir = -1;
            }
            else if ( action == wxPGKeyboardAction::NextProperty || secondAction == wxPGKeyboardAction::NextProperty )
            {
                selectDir = 1;
            }
        }

        if ( selectDir >= -1 )
        {
            p = wxPropertyGridIterator::OneStep( m_pState, wxPG_ITERATE_VISIBLE, p, selectDir );
            if ( p )
            {
                wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null;
                int reopenLabelEditorCol = -1;

                if ( action == wxPGKeyboardAction::Edit )
                {
                    // Make the next editor focused as well
                    // if we are actually going to edit the property.
                    selFlags |= wxPGSelectPropertyFlags::Focus;
                }
                else
                {
                    // Also maintain the same label editor focus state
                    if ( m_labelEditor )
                        reopenLabelEditorCol = m_selColumn;
                }

                if ( !DoSelectProperty(p, selFlags) ||
                     !selectionIsCurrent(p) )
                {
                    return;
                }

                if ( reopenLabelEditorCol >= 0 )
                {
                    DoBeginLabelEdit(reopenLabelEditorCol);
                    if ( !selectionIsCurrent(p) )
                        return;
                }
            }
            else if ( action == wxPGKeyboardAction::Edit )
            {
                // For first and last item just validate the value
                CommitChangesFromEditor();
            }
            wasHandled = true;
        }
    }
    else
    {
        // If nothing was selected, select the first item now
        // (or navigate out of tab).
        if ( action != wxPGKeyboardAction::CancelEdit && secondAction != wxPGKeyboardAction::CancelEdit )
        {
            wxPGProperty* p = wxPropertyGridInterface::GetFirst();
            if ( p ) DoSelectProperty(p);
            wasHandled = true;
        }
    }

    if ( !wasHandled )
        event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKey( wxKeyEvent &event )
{
    // If there was editor open and focused, then this event should not
    // really be processed here.
    if ( IsEditorFocused() )
    {
        // However, if event had modifiers, it is probably still best
        // to skip it.
        if ( event.HasModifiers() )
            event.Skip();
        else
            event.StopPropagation();
        return;
    }

    HandleKeyEvent(event, false);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ButtonTriggerKeyTest(wxPGKeyboardAction action, wxKeyEvent& event)
{
    if ( action == wxPGKeyboardAction::Invalid )
    {
        action = KeyEventToActions(event).first;
    }

    // Does the keycode trigger button?
    if ( action == wxPGKeyboardAction::PressButton &&
         m_wndEditor2 )
    {
        wxCommandEvent evt(wxEVT_BUTTON, m_wndEditor2->GetId());
        GetEventHandler()->AddPendingEvent(evt);
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnChildKeyDown( wxKeyEvent &event )
{
    HandleKeyEvent(event, true);
}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    // Skip fake idle events generated e.g. by calling
    // wxYield from within event handler.
    if ( m_processedEvent || wxPGGetPropertyGridTransientState(this).propertyCallbackDepth )
        return;

    const wxWeakRef<wxWindow> weakThis(this);

    //
    // Check if the focus is in this control or one of its children
    wxWindow* newFocused = wxWindow::FindFocus();

    if ( newFocused != m_curFocused )
    {
        HandleFocusChange( newFocused );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;
    }

    //
    // Check if top-level parent has changed
    if ( HasExtraStyle(wxPG_EX_ENABLE_TLP_TRACKING) )
    {
        wxWindow* tlp = ::wxGetTopLevelParent(this);
        if ( tlp != m_tlp )
        {
            OnTLPChanging(tlp);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return;
        }
    }

    // Delete pending property editors and their event handlers.
    DeletePendingObjects();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return;

    //
    // Resolve pending property removals
    // In order to determine whether deletion/removal
    // was done we need to track the size of the list
    // before and after the operation.
    // (Note that lists are changed at every operation.)
    size_t cntAfter = m_deletedProperties.size();
    while ( cntAfter > 0 )
    {
        size_t cntBefore = cntAfter;

        DeleteProperty(*(m_deletedProperties.begin()));
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;

        cntAfter = m_deletedProperties.size();
        wxASSERT_MSG( cntAfter <= cntBefore,
            wxS("Increased number of pending items after deletion") );
        // Break if deletion was not done
        if ( cntAfter >= cntBefore )
            break;
    }
    cntAfter = m_removedProperties.size();
    while ( cntAfter > 0 )
    {
        size_t cntBefore = cntAfter;

        RemoveProperty(*(m_removedProperties.begin()));
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return;

        cntAfter = m_removedProperties.size();
        wxASSERT_MSG( cntAfter <= cntBefore,
            wxS("Increased number of pending items after removal") );
        // Break if removal was not done
        if ( cntAfter >= cntBefore )
            break;
    }
}

bool wxPropertyGrid::IsEditorFocused() const
{
    wxWindow* focus = wxWindow::FindFocus();
    if ( !focus )
        return false;

    if ( focus == m_wndEditor || focus == m_wndEditor2 ||
         focus == GetEditorControl() ||
         // In case a combobox text control is focused
         (focus && focus->GetParent() && (focus->GetParent() == m_wndEditor)) )
         return true;

    return false;
}

// Called by focus event handlers. newFocused is the window that becomes focused.
void wxPropertyGrid::HandleFocusChange( wxWindow* newFocused )
{
    //
    // Never allow focus to be changed when handling editor event.
    // Especially because they may be displaying a dialog which
    // could cause all kinds of weird (native) focus changes.
    if ( HasInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT) )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakNewFocused(newFocused);
    wxPropertyGridPageState* const state = m_pState;
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    wxScopeGuard leaveFocusCallback = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveFocusCallback);

    unsigned int oldFlags = m_iFlags;
    bool wasEditorFocused = false;
    wxWindow* wndEditor = m_wndEditor;

    m_iFlags &= ~(wxPG_FL_FOCUSED);

    wxWindow* parent = newFocused;

    // This must be one of nextFocus' parents.
    while ( parent )
    {
        if ( parent == wndEditor )
        {
            // If editor is active consider focus set on its components
            // as a focus set on the editor itself (to prevent doing actions
            // when focus is switched between subcontrols of a compound
            // editor like e.g. wxComboCtrl).
            newFocused = wndEditor;
            wasEditorFocused = true;
        }
        // Use m_eventObject, which is either wxPropertyGrid or
        // wxPropertyGridManager, as appropriate.
        else if ( parent == m_eventObject )
        {
            m_iFlags |= wxPG_FL_FOCUSED;
            break;
        }
        parent = parent->GetParent();
    }

    // Notify editor control when it receives a focus
    if ( wasEditorFocused && m_curFocused != newFocused )
    {
        wxPGProperty* p = GetSelection();
        if ( p )
        {
            const wxPGEditor* editor = p->GetEditorClass();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
                 GetSelection() != p || IsPropertyPendingRemoval(p) ||
                 !editor )
            {
                return;
            }

            wxWindow* const editorControl = GetEditorControl();
            const wxWeakRef<wxWindow> weakEditor(editorControl);
            ResetEditorAppearance();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
                 GetSelection() != p || IsPropertyPendingRemoval(p) ||
                 (editorControl &&
                  (!wxWeakWindowIsAvailableForCallbacks(weakEditor,
                                                        editorControl) ||
                   GetEditorControl() != editorControl)) )
            {
                return;
            }

            editor->OnFocus(p, editorControl);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state ||
                 GetSelection() != p || IsPropertyPendingRemoval(p) ||
                 (editorControl &&
                  (!wxWeakWindowIsAvailableForCallbacks(weakEditor,
                                                        editorControl) ||
                   GetEditorControl() != editorControl)) )
            {
                return;
            }
        }
    }

    m_curFocused = newFocused && weakNewFocused ? weakNewFocused.get() : nullptr;

    if ( (m_iFlags & wxPG_FL_FOCUSED) !=
         (oldFlags & wxPG_FL_FOCUSED) )
    {
        if ( !(m_iFlags & wxPG_FL_FOCUSED) )
        {
            wxPGGetPropertyGridTransientState(this).keyboardSplitter = -1;

            // Need to store changed value
            if ( !CommitChangesFromEditor() )
                return;
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state )
                return;
        }
        else
        {
            /*
            //
            // Preliminary code for tab-order respecting
            // tab-traversal (but should be moved to
            // OnNav handler)
            //
            wxWindow* prevFocus = event.GetWindow();
            wxWindow* useThis = this;
            if ( m_iFlags & wxPG_FL_IN_MANAGER )
                useThis = GetParent();

            if ( prevFocus &&
                 prevFocus->GetParent() == useThis->GetParent() )
            {
                wxList& children = useThis->GetParent()->GetChildren();

                wxNode* node = children.Find(prevFocus);

                if ( node->GetNext() &&
                     useThis == node->GetNext()->GetData() )
                    DoSelectProperty(GetFirst());
                else if ( node->GetPrevious () &&
                          useThis == node->GetPrevious()->GetData() )
                    DoSelectProperty(GetLastProperty());

            }
            */
        }

        // Redraw selected
        if ( m_iFlags & wxPG_FL_INITIALIZED )
        {
            const wxArrayPGProperty sel = GetSelectedProperties();
            for( wxPGProperty* selProp : sel )
            {
                if ( IsPropertyPendingRemoval(selProp) )
                    continue;
                DrawItem(selProp);
                if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) || m_pState != state )
                    return;
            }
        }
    }
}

void wxPropertyGrid::OnFocusEvent( wxFocusEvent& event )
{
    if ( event.GetEventType() == wxEVT_SET_FOCUS )
        HandleFocusChange((wxWindow*)event.GetEventObject());
    // Line changed to "else" when applying wxPropertyGrid patch #1675902
    //else if ( event.GetWindow() )
    else
        HandleFocusChange(event.GetWindow());

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnChildFocusEvent( wxChildFocusEvent& event )
{
    HandleFocusChange((wxWindow*)event.GetEventObject());
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnScrollEvent( wxScrollWinEvent &event )
{
    m_iFlags |= wxPG_FL_SCROLLED;

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnCaptureChange( wxMouseCaptureChangedEvent& WXUNUSED(event) )
{
    if ( m_dragStatus )
        FinishSplitterDrag(true);
    else
        m_iFlags &= ~wxPG_FL_MOUSE_CAPTURED;
}

// -----------------------------------------------------------------------
// Property editor related functions
// -----------------------------------------------------------------------

// noDefCheck = true prevents infinite recursion.
wxPGEditor* wxPropertyGrid::DoRegisterEditorClass( wxPGEditor* editorClass,
                                                   const wxString& editorName,
                                                   bool noDefCheck )
{
    wxCHECK( editorClass, nullptr );

    if ( !noDefCheck && wxPGGlobalVars->m_mapEditorClasses.empty() )
        RegisterDefaultEditors();

    wxString name = editorName.empty() ? editorClass->GetName() : editorName;

    // Existing editor under this name?
    auto vt_it = wxPGGlobalVars->m_mapEditorClasses.find(name);

    if ( vt_it != wxPGGlobalVars->m_mapEditorClasses.end() )
    {
        // If this name was already used, try class name.
        name = editorClass->GetClassInfo()->GetClassName();
        vt_it = wxPGGlobalVars->m_mapEditorClasses.find(name);
    }

    wxCHECK_MSG( vt_it == wxPGGlobalVars->m_mapEditorClasses.end(),
                 vt_it->second,
                 wxS("Editor with given name was already registered") );

    wxPGGlobalVars->m_mapEditorClasses[name] = editorClass;

    return editorClass;
}

// Use this in RegisterDefaultEditors.
#define wxPGRegisterDefaultEditorClass(EDITOR) \
    if ( wxPGEditor_##EDITOR == nullptr ) \
    { \
        wxPGEditor_##EDITOR = wxPropertyGrid::RegisterEditorClass( \
            new wxPG##EDITOR##Editor, true ); \
    }

// Registers all default editor classes
void wxPropertyGrid::RegisterDefaultEditors()
{
    wxPGRegisterDefaultEditorClass( TextCtrl );
    wxPGRegisterDefaultEditorClass( Choice );
    wxPGRegisterDefaultEditorClass( ComboBox );
    wxPGRegisterDefaultEditorClass( TextCtrlAndButton );
#if wxPG_INCLUDE_CHECKBOX
    wxPGRegisterDefaultEditorClass( CheckBox );
#endif
    wxPGRegisterDefaultEditorClass( ChoiceAndButton );

    // Register SpinCtrl etc. editors before use
    RegisterAdditionalEditors();
}

// -----------------------------------------------------------------------
// wxPGStringTokenizer
//   Needed to handle C-style string lists (e.g. "str1" "str2")
// -----------------------------------------------------------------------

wxPGStringTokenizer::wxPGStringTokenizer( const wxString& str, wxChar delimiter )
    : m_str(str), m_curPos(str.begin()), m_delimiter(delimiter)
{
}

bool wxPGStringTokenizer::HasMoreTokens()
{
    const wxString& str = m_str;

    wxString::const_iterator i = m_curPos;

    wxUniChar delim = m_delimiter;
    wxUniChar prev_a = wxS('\0');

    bool inToken = false;

    while ( i != str.end() )
    {
        wxUniChar a = *i;

        if ( !inToken )
        {
            if ( a == delim )
            {
                inToken = true;
                m_readyToken.clear();
            }
        }
        else
        {
            if ( prev_a != wxS('\\') )
            {
                if ( a != delim )
                {
                    if ( a != wxS('\\') )
                        m_readyToken << a;
                }
                else
                {
                    ++i;
                    m_curPos = i;
                    return true;
                }
                prev_a = a;
            }
            else
            {
                m_readyToken << a;
                prev_a = wxS('\0');
            }
        }
        ++i;
    }

    m_curPos = str.end();

    if ( inToken )
        return true;

    return false;
}

wxString wxPGStringTokenizer::GetNextToken()
{
    return m_readyToken;
}

// -----------------------------------------------------------------------
// wxPropertyGridEvent
// -----------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertyGridEvent, wxCommandEvent);


wxDEFINE_EVENT( wxEVT_PG_SELECTED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_CHANGING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_CHANGED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_HIGHLIGHTED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_RIGHT_CLICK, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_PAGE_CHANGED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_ITEM_EXPANDED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_ITEM_COLLAPSED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_LABEL_EDIT_BEGIN, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_LABEL_EDIT_ENDING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_BEGIN_DRAG, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_DRAGGING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_END_DRAG, wxPropertyGridEvent );
// Events used only internally
wxDEFINE_EVENT( wxEVT_PG_HSCROLL, wxPropertyGridEvent);
wxDEFINE_EVENT( wxEVT_PG_COLS_RESIZED, wxPropertyGridEvent);

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(wxEventType commandType, int id)
    : wxCommandEvent(commandType,id)
    , m_property(nullptr)
    , m_pg(nullptr)
    , m_validationInfo(nullptr)
    , m_column(1)
    , m_canVeto(false)
    , m_wasVetoed(false)
{
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(const wxPropertyGridEvent& event)
    : wxCommandEvent(event)
    , m_property(nullptr)
    , m_pg(nullptr)
    , m_validationInfo(nullptr)
    , m_column(1)
    , m_canVeto(false)
    , m_wasVetoed(false)
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif

    // A clone owns a value snapshot. In particular it must never retain the
    // source grid's transient validation-info pointer when queued to another
    // thread or when the source grid is destroyed.
    m_property = event.m_property;
    m_propertyName = event.m_propertyName;
    m_value = event.m_validationInfo
                  ? event.m_validationInfo->GetValue()
                  : event.m_value;
    m_column = event.m_column;
    m_canVeto = event.m_canVeto;
    m_wasVetoed = event.m_wasVetoed;
    m_pg = event.m_pg;
    if ( m_pg )
        m_pg->m_liveEvents.push_back(this);
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridEvent::GetProperty() const
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif
    return m_property;
}

// -----------------------------------------------------------------------

wxVariant wxPropertyGridEvent::GetPropertyValue() const
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif
    return m_validationInfo ? m_validationInfo->GetValue() : m_value;
}

// -----------------------------------------------------------------------

void wxPropertyGridEvent::SetProperty(wxPGProperty* property)
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif
    m_property = property;
    if ( property )
        m_propertyName = property->GetName();
}

// -----------------------------------------------------------------------

void wxPropertyGridEvent::SetupValidationInfo()
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif
    wxASSERT(m_pg);
    wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
    m_validationInfo = &m_pg->GetValidationInfo();
    m_value = m_validationInfo->GetValue();
}

// -----------------------------------------------------------------------

void wxPropertyGridEvent::SetPropertyGrid(wxPropertyGrid* pg)
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif

    if ( m_pg == pg )
        return;

    // m_validationInfo belongs to the old grid. Preserve the documented
    // post-destruction value contract before unlinking from that grid. This
    // snapshot and all registry changes form one critical transaction.
    if ( m_pg && m_validationInfo )
    {
        m_value = m_validationInfo->GetValue();
        m_validationInfo = nullptr;
    }

    if ( m_pg )
    {
        std::vector<wxPropertyGridEvent*>& oldEvents = m_pg->m_liveEvents;
        oldEvents.erase(
            std::remove(oldEvents.begin(), oldEvents.end(), this),
            oldEvents.end());
    }

    m_pg = pg;
    if ( m_pg )
    {
        std::vector<wxPropertyGridEvent*>& newEvents = m_pg->m_liveEvents;
        if ( std::find(newEvents.begin(), newEvents.end(), this) ==
             newEvents.end() )
        {
            newEvents.push_back(this);
        }
    }
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::~wxPropertyGridEvent()
{
    SetPropertyGrid(nullptr);
}

// -----------------------------------------------------------------------

wxEvent* wxPropertyGridEvent::Clone() const
{
    return new wxPropertyGridEvent( *this );
}

// -----------------------------------------------------------------------
// wxPropertyGridPopulator
// -----------------------------------------------------------------------

wxPropertyGridPopulator::wxPropertyGridPopulator()
    : m_pg(nullptr)
    , m_state(nullptr)
{
    wxPGGlobalVars->m_offline++;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::SetState( wxPropertyGridPageState* state )
{
    m_state = state;
    m_propHierarchy.clear();
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::SetGrid( wxPropertyGrid* pg )
{
    m_pg = pg;
    pg->Freeze();
}

// -----------------------------------------------------------------------

wxPropertyGridPopulator::~wxPropertyGridPopulator()
{
    // Free unused sets of choices
    for( const auto& it : m_dictIdChoices )
    {
        it.second->DecRef();
    }

    if ( m_pg )
    {
        m_pg->Thaw();
        m_pg->GetPanel()->Refresh();
    }
    wxPGGlobalVars->m_offline--;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPopulator::Add( const wxString& propClass,
                                            const wxString& propLabel,
                                            const wxString& propName,
                                            const wxString* propValue,
                                            wxPGChoices* pChoices )
{
    wxClassInfo* classInfo = wxClassInfo::FindClass(propClass);
    wxPGProperty* parent = GetCurParent();

    if ( parent->HasFlag(wxPGFlags::Aggregate) )
    {
        ProcessError(wxString::Format(wxS("new children cannot be added to '%s'"),parent->GetName()));
        return nullptr;
    }

    if ( !classInfo || !classInfo->IsKindOf(wxCLASSINFO(wxPGProperty)) )
    {
        ProcessError(wxString::Format(wxS("'%s' is not valid property class"),propClass));
        return nullptr;
    }

    wxPGProperty* property = (wxPGProperty*) classInfo->CreateObject();

    property->SetLabel(propLabel);
    property->DoSetName(propName);

    if ( pChoices && pChoices->IsOk() )
        property->SetChoices(*pChoices);

    m_state->DoInsert(parent, -1, property);

    if ( propValue )
        property->SetValueFromString( *propValue, wxPGPropValFormatFlags::FullValue|
                                                  wxPGPropValFormatFlags::ProgrammaticValue );

    return property;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::AddChildren( wxPGProperty* property )
{
    // Preserve inherited attributes to be able to restore them later:
    // attributes recursively set for the children of this property shouldn't
    // be inherited by its siblings.
    const auto inheritedAttributesOrig = m_inheritedAttributes;

    // Apply inherited attributes to the property.
    for ( const auto& it : m_inheritedAttributes )
    {
        property->SetAttribute(it.first, it.second);
    }

    m_propHierarchy.push_back(property);
    DoScanForChildren();
    m_propHierarchy.pop_back();

    m_inheritedAttributes = std::move(inheritedAttributesOrig);
}

// -----------------------------------------------------------------------

wxPGChoices wxPropertyGridPopulator::ParseChoices( const wxString& choicesString,
                                                   const wxString& idString )
{
    // Using id?
    if ( choicesString[0] == wxT('@') )
    {
        wxString ids = choicesString.substr(1);
        auto it = m_dictIdChoices.find(ids);
        if ( it == m_dictIdChoices.end() )
        {
            ProcessError(wxString::Format(wxS("No choices defined for id '%s'"), ids));
            return wxPGChoices();
        }

        return wxPGChoices(it->second);
    }

    if ( !idString.empty() )
    {
        auto it = m_dictIdChoices.find(idString);
        if ( it != m_dictIdChoices.end() )
        {
            return wxPGChoices(it->second);
        }
    }

    // Parse choices string
    wxPGChoices choices;
    wxString label;
    wxString value;
    int state = 0;
    bool labelValid = false;

    for ( wxUniChar c : choicesString )
    {
        if ( state != 1 )
        {
            if ( c == wxS('"') )
            {
                if ( labelValid )
                {
                    long l;
                    if ( !value.ToLong(&l, 0) ) l = wxPG_INVALID_VALUE;
                    choices.Add(label, l);
                }
                labelValid = false;
                //wxLogDebug(wxS("%s, %s"),label,value);
                value.clear();
                label.clear();
                state = 1;
            }
            else if ( c == wxS('=') )
            {
                if ( labelValid )
                {
                    state = 2;
                }
            }
            else if ( state == 2 && (wxIsalnum(c) || c == wxS('x')) )
            {
                value << c;
            }
        }
        else
        {
            if ( c == wxS('"') )
            {
                state = 0;
                labelValid = true;
            }
            else
                label << c;
        }
    }

    if ( labelValid )
    {
        long l;
        if ( !value.ToLong(&l, 0) ) l = wxPG_INVALID_VALUE;
        choices.Add(label, l);
    }

    if ( !choices.IsOk() )
    {
        choices.EnsureData();
    }

    // Assign to id
    if ( !idString.empty() )
        m_dictIdChoices[idString] = choices.GetData();

    return choices;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::ToLongPCT( const wxString& s, long* pval, long max )
{
    if ( s.Last() == wxT('%') )
    {
        wxString s2 = s.substr(0,s.length()-1);
        long val;
        if ( s2.ToLong(&val, 10) )
        {
            *pval = (val*max)/100;
            return true;
        }
        return false;
    }

    return s.ToLong(pval, 10);
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::AddAttribute( const wxString& name,
                                            const wxString& type,
                                            const wxString& value,
                                            wxPGPropertyValuesFlags flags )
{
    if ( m_propHierarchy.empty() )
        return false;

    wxPGProperty* p = m_propHierarchy.back();
    wxString valuel = value.Lower();
    wxVariant variant;

    if ( type.empty() )
    {
        long v;

        // Auto-detect type
        if ( valuel == wxS("true") || valuel == wxS("yes") || valuel == wxS("1") )
            variant = true;
        else if ( valuel == wxS("false") || valuel == wxS("no") || valuel == wxS("0") )
            variant = false;
        else if ( value.ToLong(&v, 0) )
            variant = v;
        else
            variant = value;
    }
    else
    {
        if ( type == wxS("string") )
        {
            variant = value;
        }
        else if ( type == wxS("int") )
        {
            long v = 0;
            value.ToLong(&v, 0);
            variant = v;
        }
        else if ( type == wxS("bool") )
        {
            if ( valuel == wxS("true") || valuel == wxS("yes") || valuel == wxS("1") )
                variant = true;
            else
                variant = false;
        }
        else
        {
            ProcessError(wxString::Format(wxS("Invalid attribute type '%s'"),type));
            return false;
        }
    }

    if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
    {
        m_inheritedAttributes[name] = variant;
    }

    p->SetAttribute( name, variant );

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::ProcessError( const wxString& msg )
{
    wxLogError(_("Error in resource: %s"),msg);
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
