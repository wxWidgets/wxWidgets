///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/popupwin.cpp
// Purpose:     implements wxPopupWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.05.02
// Copyright:   (c) 2002 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_POPUPWIN

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/popupwin.h"

#include "wx/msw/private.h"     // for GetDesktopWindow()

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
    return (WXHWND)::GetDesktopWindow();
}

void wxPopupWindow::SetFocus()
{
    // Focusing on a popup window does not work on MSW unless WS_POPUP style is
    // set (which is never the case currently, see the note in MSWGetParent()).
    // We do not even want to try to set the focus, as it returns an error from
    // SetFocus() on recent Windows versions (since Vista) and the resulting
    // debug message is annoying.
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
            wxLogLastError(wxT("SetWindowPos"));
        }

        // and set it as the foreground window so the mouse can be captured
        ::SetForegroundWindow(GetHwnd());
    }

    return true;
}

#endif // #if wxUSE_POPUPWIN
