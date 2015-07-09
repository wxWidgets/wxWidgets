///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/nativewin.cpp
// Purpose:     wxNativeWindow implementation
// Author:      Vadim Zeitlin
// Created:     2008-03-05
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/nativewin.h"
#include "wx/msw/private.h"

// ============================================================================
// implementation
// ============================================================================

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

