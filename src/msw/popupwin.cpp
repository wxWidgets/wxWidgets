///////////////////////////////////////////////////////////////////////////////
// Name:        msw/popupwin.cpp
// Purpose:     implements wxPopupWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.05.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "popup.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif //WX_PRECOMP

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

#include "wx/msw/private.h"     // for GetDesktopWindow()

IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)

// ============================================================================
// implementation
// ============================================================================

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    // popup windows are created hidden by default
    Hide();

    return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize,
                                flags | wxPOPUP_WINDOW);
}

void wxPopupWindow::DoGetPosition(int *x, int *y) const
{
    // the position of a "top level" window such as this should be in
    // screen coordinates, not in the client ones which MSW gives us
    // (because we are a child window)
    wxPopupWindowBase::DoGetPosition(x, y);

    GetParent()->ClientToScreen(x, y);
}

WXDWORD wxPopupWindow::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    // we only honour the border flags, the others don't make sense for us
    WXDWORD style = wxWindow::MSWGetStyle(flags & wxBORDER_MASK, exstyle);

    if ( exstyle )
    {
        // a popup window floats on top of everything
        *exstyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    }

    return style;
}

WXHWND wxPopupWindow::MSWGetParent() const
{
    // we must be a child of the desktop to be able to extend beyond the parent
    // window client area (like the comboboxes drop downs do)
    //
    // NB: alternative implementation would be to use WS_POPUP instead of
    //     WS_CHILD but then showing a popup would deactivate the parent which
    //     is ugly and working around this, although possible, is even more
    //     ugly
    // GetDesktopWindow() is not always supported on WinCE, and if
    // it is, it often returns NULL.
#ifdef __WXWINCE__
    return 0;
#else
    return (WXHWND)::GetDesktopWindow();
#endif
}

bool wxPopupWindow::Show(bool show)
{
    if ( !wxWindowMSW::Show(show) )
        return false;

    if ( show )
    {
        // raise to top of z order
        if (!::SetWindowPos(GetHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
        {
            wxLogLastError(_T("SetWindowPos"));
        }
    }

    return true;
}

#endif // #if wxUSE_POPUPWIN

