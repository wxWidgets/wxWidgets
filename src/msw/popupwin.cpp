///////////////////////////////////////////////////////////////////////////////
// Name:        msw/popupwin.cpp
// Purpose:     implements wxPopupWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.05.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "popup.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/popupwin.h"

#include "wx/msw/private.h"     // for WS_CHILD and WS_POPUP

wxWindowList wxPopupWindow::ms_shownPopups;

// ============================================================================
// implementation
// ============================================================================

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, -1,
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
    // we only hnour the border flags
    WXDWORD style = wxWindow::MSWGetStyle(flags & wxBORDER_MASK, exstyle);

    // and we mustn't have WS_CHILD style or we would be limited to the parents
    // client area
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
    // skip wxWindow::Show() which calls wxBringWindowToTop(): this results in
    // activating the popup window and stealing the atcivation from our parent
    // which means that the parent frame becomes deactivated when opening a
    // combobox, for example -- definitely not what we want
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    if ( show )
    {
        ms_shownPopups.Append(this);
    }
    else // remove from the shown list
    {
        ms_shownPopups.DeleteObject(this);
    }

    ::ShowWindow(GetHwnd(), show ? SW_SHOWNOACTIVATE : SW_HIDE);

    return TRUE;
}

/* static */
wxPopupWindow *wxPopupWindow::FindPopupFor(wxWindow *winParent)
{
    // find a popup with the given parent in the linked list of all shown
    // popups
    for ( wxWindowList::Node *node = ms_shownPopups.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();
        if ( win->GetParent() == winParent )
            return (wxPopupWindow *)win;
    }

    return NULL;
}

