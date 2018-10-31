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

// Set to the popup window currently being shown, if any.
//
// Note that this global variable is used in src/msw/window.cpp and so must be
// extern.
wxPopupWindow* wxCurrentPopupWindow = NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    // popup windows are created hidden by default
    Hide();

    m_owner = wxGetTopLevelParent(parent);

    return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize,
                                flags);
}

WXDWORD wxPopupWindow::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    // we only honour the border flags, the others don't make sense for us
    WXDWORD style = wxWindow::MSWGetStyle(flags & wxBORDER_MASK, exstyle);

    // We need to be a popup (i.e. not a child) window in order to not be
    // confined to the parent window area, as is required for a drop down, for
    // example. Old implementation used WS_CHILD and made this window a child
    // of the desktop window, but this resulted in problems with handling input
    // in the popup children, and so was changed to the current version.
    style &= ~WS_CHILD;
    style |= WS_POPUP;

    if ( exstyle )
    {
        // a popup window floats on top of everything
        *exstyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    }

    return style;
}

bool wxPopupWindow::Show(bool show)
{
    // It's important to update wxCurrentPopupWindow before showing the window,
    // to ensure that it already set by the time the owner gets WM_NCACTIVATE
    // from inside Show() so that it knows to remain [appearing] active.
    wxCurrentPopupWindow = show ? this : NULL;

    return wxPopupWindowBase::Show(show);
}

// ----------------------------------------------------------------------------
// wxPopupTransientWindow
// ----------------------------------------------------------------------------

void wxPopupTransientWindow::Popup(wxWindow* focus)
{
    Show();

    // We can only set focus to one of our children as setting it to another
    // window would result in an immediate loss of activation and popup
    // disappearance.
    if ( focus && IsDescendant(focus) )
        focus->SetFocus();
}

void wxPopupTransientWindow::Dismiss()
{
    Hide();
}

bool
wxPopupTransientWindow::MSWHandleMessage(WXLRESULT *result,
                                         WXUINT message,
                                         WXWPARAM wParam,
                                         WXLPARAM lParam)
{
    switch ( message )
    {
        case WM_NCACTIVATE:
            if ( !wParam )
            {
                // Hide the window automatically when it loses activation.
                Dismiss();

                // Activation might have gone to a different window or maybe
                // even a different application, don't let our owner continue
                // to appear active in this case.
                wxWindow* const owner = MSWGetOwner();
                if ( owner )
                {
                    const HWND hwndActive = ::GetActiveWindow();
                    if ( hwndActive != GetHwndOf(owner) )
                    {
                        ::SendMessage(GetHwndOf(owner), WM_NCACTIVATE, FALSE, 0);
                    }
                }
            }
            break;
    }

    return wxPopupTransientWindowBase::MSWHandleMessage(result, message,
                                                        wParam, lParam);
}

#endif // #if wxUSE_POPUPWIN
