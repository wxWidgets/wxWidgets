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


#if wxUSE_POPUPWIN

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/popupwin.h"

#include "wx/msw/private.h"     // for GetDesktopWindow()

// Set to the popup window currently being shown, if any.
//
// Note that this global variable is used in src/msw/window.cpp and so must be
// extern.
wxPopupWindow* wxCurrentPopupWindow = nullptr;

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

wxPopupWindow::~wxPopupWindow()
{
    // If the popup is destroyed without being hidden first, ensure that we are
    // not left with a dangling pointer.
    if ( wxCurrentPopupWindow == this )
        wxCurrentPopupWindow = nullptr;
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
    // Note that we're called from the ctor before the window is actually
    // created to hide the popup initially. This call doesn't really hide the
    // window, so don't do anything in this case, in particular don't change
    // wxCurrentPopupWindow value.
    if ( !GetHwnd() )
        return wxPopupWindowBase::Show(show);

    // It's important to update wxCurrentPopupWindow before showing the window,
    // to ensure that it's already set by the time the owner gets WM_NCACTIVATE
    // from inside Show() so that it knows to remain [appearing] active, see
    // the WM_NCACTIVATE handler in wxWindow::MSWHandleMessage().
    if ( show )
    {
        // There could have been a previous popup window which hasn't been
        // hidden yet. This will happen now, when we show this one, as it will
        // result in activation loss for the other one, so it's ok to overwrite
        // the old pointer, even if it's non-null.
        wxCurrentPopupWindow = this;
    }
    else
    {
        // Only reset the pointer if it points to this window, otherwise we
        // would lose the correct value in the situation described above.
        if ( wxCurrentPopupWindow == this )
            wxCurrentPopupWindow = nullptr;
    }

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

    // We can only set focus when using wxPU_CONTAINS_CONTROLS and then only to
    // one of our children as setting it to another window would result in an
    // immediate loss of activation and popup disappearance.
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) && focus && IsDescendant(focus) )
        focus->SetFocus();
}

void wxPopupTransientWindow::Dismiss()
{
    Hide();
}

void wxPopupTransientWindow::DismissOnDeactivate()
{
    // Hide the window automatically when it loses activation.
    DismissAndNotify();

    // Activation might have gone to a different window or maybe
    // even a different application, don't let our owner continue
    // to appear active in this case.
    wxWindow* const owner = MSWGetOwner();
    if ( owner )
    {
        if ( ::GetActiveWindow() != GetHwndOf(owner) )
        {
            ::SendMessage(GetHwndOf(owner), WM_NCACTIVATE, FALSE, 0);
        }
    }
}

void wxPopupTransientWindow::MSWDismissUnfocusedPopup()
{
    // When we use wxPU_CONTAINS_CONTROLS, we can react to the popup
    // deactivation in MSWHandleMessage(), but if we don't have focus, we don't
    // get any events ourselves, so we rely on wxWindow to forward them to us.
    if ( !HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        // It doesn't seem necessary to use CallAfter() here, as dismissing
        // this window shouldn't affect the focus, as it never has it anyhow.
        DismissAndNotify();
    }
}

bool
wxPopupTransientWindow::MSWHandleMessage(WXLRESULT *result,
                                         WXUINT message,
                                         WXWPARAM wParam,
                                         WXLPARAM lParam)
{
    switch ( message )
    {
        case WM_ACTIVATE:
            if ( wParam == WA_INACTIVE )
            {
                // We need to dismiss this window, however doing it directly
                // from here seems to confuse ::ShowWindow(), which ends up
                // calling this handler, and may result in losing activation
                // entirely, so postpone it slightly.
                //
                // Also note that the active window hasn't changed yet, so we
                // postpone calling it until DismissOnDeactivate() is executed.
                CallAfter(&wxPopupTransientWindow::DismissOnDeactivate);
            }
            break;
    }

    return wxPopupTransientWindowBase::MSWHandleMessage(result, message,
                                                        wParam, lParam);
}

#endif // #if wxUSE_POPUPWIN
