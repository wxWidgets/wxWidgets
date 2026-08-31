///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/nativewin.cpp
// Purpose:     wxNativeWindow implementation
// Author:      Vadim Zeitlin
// Created:     2008-03-05
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/nativewin.h"
#include "wx/msw/private.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/weakref.h"
    #include "wx/winui/private/tlwhostmsw.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeWindow
// ----------------------------------------------------------------------------

bool
wxNativeWindow::Create(wxWindow* parent,
                       wxWindowID winid,
                       wxNativeWindowHandle hwnd)
{
    wxCHECK_MSG( hwnd, false, wxS("Invalid null HWND") );
    wxCHECK_MSG( parent, false, wxS("Must have a valid parent") );
    wxASSERT_MSG( ::GetParent(hwnd) == GetHwndOf(parent),
                  wxS("The native window has incorrect parent") );

    const wxRect r = wxRectFromRECT(wxGetWindowRect(hwnd));

    // Skip wxWindow::Create() which would try to create a new HWND, we don't
    // want this as we already have one.
    if ( !CreateBase(parent, winid,
                     r.GetPosition(), r.GetSize(),
                     0, wxDefaultValidator, wxS("nativewindow")) )
        return false;

    parent->AddChild(this);

    SubclassWin(hwnd);

    if ( winid == wxID_ANY )
    {
        // We allocated a new ID to the control, use it at Windows level as
        // well because we assume that our and MSW IDs are the same in many
        // places and it seems prudent to avoid breaking this assumption.
        SetId(GetId());
    }
    else // We used a fixed ID.
    {
        // For the same reason as above, check that it's the same as the one
        // used by the native HWND.
        wxASSERT_MSG( ::GetWindowLong(hwnd, GWL_ID) == winid,
                      wxS("Mismatch between wx and native IDs") );
    }

    InheritAttributes();

    return true;
}

void wxNativeWindow::DoDisown()
{
    // We don't do anything here, clearing m_ownedByUser flag is enough.
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

WXLRESULT wxNativeWindow::MSWWindowProc(WXUINT nMsg,
                                        WXWPARAM wParam,
                                        WXLPARAM lParam)
{
    if ( nMsg != WM_DESTROY )
        return wxWindow::MSWWindowProc(nMsg, wParam, lParam);

    // Call the complete wxWindow/original-WNDPROC chain exactly once while
    // the external HWND is unquestionably still this native lifetime. Either
    // callback is allowed to delete this wrapper, so retain no unchecked
    // access to it across the call.
    const WXHWND hwnd = GetHWND();
    const unsigned long long associationGeneration =
        wxWinUIMSWGetHwndGeneration(this, hwnd);
    const wxWeakRef<wxWindow> lifetime(this);
    const WXLRESULT result =
        wxWindow::MSWWindowProc(nMsg, wParam, lParam);

    wxWindow * const live = lifetime.get();
    if ( live != this || !hwnd || live->GetHWND() != hwnd ||
         wxFindWinFromHandle(static_cast<HWND>(hwnd)) != live )
    {
        return result;
    }

    // A nested callback may have changed the wx/HWND association without
    // deleting either object. Never detach a replacement generation.
    if ( !associationGeneration ||
         wxWinUIMSWGetHwndGeneration(live, hwnd) != associationGeneration )
    {
        return result;
    }

    // WM_DESTROY precedes WM_NCDESTROY, hence this is still the exact HWND
    // lifetime above. Restore its external WNDPROC and retire the wx map now;
    // the original proc receives WM_NCDESTROY directly afterwards.
    static_cast<wxNativeWindow *>(live)->wxWindow::DissociateHandle();
    return result;
}

#endif // __WXWINUI__ && wxUSE_WINUI3

wxNativeWindow::~wxNativeWindow()
{
    // Restore the original window proc and reset HWND to 0 to prevent it from
    // being destroyed in the base class dtor if it's owned by user code. The
    // owner is also allowed to destroy its HWND before this wrapper: use the
    // invalid-handle-aware detach path instead of assuming that IsWindow()
    // still succeeds here.
    if ( m_ownedByUser )
        DissociateHandle();
}

// ----------------------------------------------------------------------------
// wxNativeContainerWindow
// ----------------------------------------------------------------------------

bool wxNativeContainerWindow::Create(wxNativeContainerWindowHandle hwnd)
{
    if ( !::IsWindow(hwnd) )
    {
        // strictly speaking, the fact that IsWindow() returns true doesn't
        // mean that the window handle is valid -- it could be being deleted
        // right now, for example
        //
        // but if it returns false, the handle is definitely invalid
        return false;
    }

    // make this HWND really a wxWindow
    SubclassWin(hwnd);

    // inherit the other attributes we can from the native HWND
    AdoptAttributesFromHWND();

    wxTopLevelWindows.Append(this);

    return true;
}

bool wxNativeContainerWindow::IsShown() const
{
    return (IsWindowVisible(static_cast<HWND>(m_hWnd)) != 0);
}

void wxNativeContainerWindow::OnNativeDestroyed()
{
    // don't use Close() or even Destroy() here, we really don't want to keep
    // an object using a no more existing HWND around for longer than necessary
    delete this;
}

WXLRESULT wxNativeContainerWindow::MSWWindowProc(WXUINT nMsg,
                                                 WXWPARAM wParam,
                                                 WXLPARAM lParam)
{
    switch ( nMsg )
    {
        case WM_CLOSE:
            // wxWindow itself, unlike wxFrame, doesn't react to WM_CLOSE and
            // just ignores it without even passing it to DefWindowProc(),
            // which means that the original WM_CLOSE handler wouldn't be
            // called if we didn't explicitly do it here.
            return MSWDefWindowProc(nMsg, wParam, lParam);

        case WM_DESTROY:
            // Send it to the original handler which may have some cleanup to
            // do as well. Notice that we must do it before calling
            // OnNativeDestroyed() as we can't use this object after doing it.
            MSWDefWindowProc(nMsg, wParam, lParam);

            OnNativeDestroyed();

            return 0;
    }

    return wxTopLevelWindow::MSWWindowProc(nMsg, wParam, lParam);
}

wxNativeContainerWindow::~wxNativeContainerWindow()
{
    // prevent the base class dtor from destroying the window, it doesn't
    // belong to us so we should leave it alive
    DissociateHandle();
}

