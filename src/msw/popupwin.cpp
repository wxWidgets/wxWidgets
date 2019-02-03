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

    // wxMSW uses 2 rather different implementations of wxPopupWindow
    // internally, each one with its own limitations, so we allow specifying
    // wxPU_CONTAINS_CONTROLS flag to select which one is used. The default is
    // to use a child window of the desktop for the popup, which is compatible
    // with the previous wxWidgets versions and works well for simple popups,
    // but many standard controls can't work as children of such a window
    // because it doesn't accept focus. So an alternative implementation for
    // the popups that will contain such controls is available, but this one
    // has problems due to the fact that it does take focus and not only can
    // (and does) this break existing code, but it also prevents the parent
    // window from keeping focus while showing the popup, as must be done when
    // using auto-completion tooltips, for example. So neither implementation
    // can be used in all cases and you have to explicitly choose your poison.
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        style &= ~WS_CHILD;
        style |= WS_POPUP;
    }

    if ( exstyle )
    {
        // a popup window floats on top of everything
        *exstyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    }

    return style;
}

WXHWND wxPopupWindow::MSWGetParent() const
{
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        return wxPopupWindowBase::MSWGetParent();
    }
    else
    {
        // we must be a child of the desktop to be able to extend beyond the
        // parent window client area (like the comboboxes drop downs do)
        return (WXHWND)::GetDesktopWindow();
    }
}

void wxPopupWindow::SetFocus()
{
    // Focusing on a popup window does not work on MSW unless WS_POPUP style
    // is set. Since this is only the case if the style wxPU_CONTAINS_CONTROLS
    // is used, we'll handle the focus in that case and otherwise do nothing.
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        wxPopupWindowBase::SetFocus();
    }
}

bool wxPopupWindow::Show(bool show)
{
    // It's important to update wxCurrentPopupWindow before showing the window,
    // to ensure that it already set by the time the owner gets WM_NCACTIVATE
    // from inside Show() so that it knows to remain [appearing] active.
    wxCurrentPopupWindow = show ? this : NULL;

    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        return wxPopupWindowBase::Show(show);
    }
    else
    {
        if ( !wxWindowMSW::Show(show) )
            return false;

        if ( show )
        {
            // raise to top of z order
            if ( !::SetWindowPos(GetHwnd(), HWND_TOP, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE) )
            {
                wxLogLastError(wxT("SetWindowPos"));
            }

            // and set it as the foreground window so the mouse can be captured
            ::SetForegroundWindow(GetHwnd());
        }

        return true;
    }
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
