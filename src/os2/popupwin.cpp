///////////////////////////////////////////////////////////////////////////////
// Name:        os2/popupwin.cpp
// Purpose:     implements wxPopupWindow for OS2
// Author:      Dave Webster
// Modified by:
// Created:     13.05.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Dave Webster <dwebster@bhmi.com>
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


#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/popupwin.h"

wxWindowList wxPopupWindow::m_svShownPopups;

// ============================================================================
// implementation
// ============================================================================

bool wxPopupWindow::Create(
  wxWindow*                         pParent
, int                               nFlags
)
{
    return wxPopupWindowBase::Create(pParent) &&
                    wxWindow::Create( pParent
                                     ,-1
                                     ,wxDefaultPosition
                                     ,wxDefaultSize
                                     ,nFlags | wxPOPUP_WINDOW
                                    );
} // end of wxPopupWindow::Create

void wxPopupWindow::DoGetPosition(
  int*                              pnX
, int*                              pnY
) const
{
    //
    // The position of a "top level" window such as this should be in
    // screen coordinates, not in the client ones which MSW gives us
    // (because we are a child window)
    //
    wxPopupWindowBase::DoGetPosition(pnX, pnY);
    GetParent()->ClientToScreen(pnX, pnY);
} // end of wxPopupWindow::DoGetPosition

WXDWORD wxPopupWindow::OS2GetStyle(
  long                              lFlags
, WXDWORD*                          dwExstyle
) const
{
    WXDWORD                         dwStyle = wxWindow::OS2GetStyle( lFlags & wxBORDER_MASK
                                                                    ,dwExstyle
                                                                   );

    return dwStyle;
} // end of wxPopupWindow::OS2GetStyle

bool wxPopupWindow::Show(
  bool                              bShow
)
{
    SWP                             vSwp;
    //
    // Skip wxWindow::Show() which calls wxBringWindowToTop(): this results in
    // activating the popup window and stealing the atcivation from our parent
    // which means that the parent frame becomes deactivated when opening a
    // combobox, for example -- definitely not what we want
    //
    if (!wxWindowBase::Show(bShow))
        return FALSE;

    if (bShow)
    {
        m_svShownPopups.Append(this);
    }
    else // remove from the shown list
    {
        m_svShownPopups.DeleteObject(this);
    }
    ::WinQueryWindowPos(GetHwnd(), &vSwp);

    if (bShow)
    {
        ::WinSetWindowPos( GetHwnd()
                          ,HWND_TOP
                          ,vSwp.x
                          ,vSwp.y
                          ,vSwp.cx
                          ,vSwp.cy
                          ,SWP_DEACTIVATE | SWP_SHOW | SWP_ZORDER
                         );
    }
    else
    {
        ::WinSetWindowPos( GetHwnd()
                          ,HWND_BOTTOM
                          ,vSwp.x
                          ,vSwp.y
                          ,vSwp.cx
                          ,vSwp.cy
                          ,SWP_HIDE | SWP_ZORDER
                         );
    }
    return TRUE;
} // end of wxPopupWindow::Show

/* static */
wxPopupWindow* wxPopupWindow::FindPopupFor(
  wxWindow*                         pWinParent
)
{
    //
    // Find a popup with the given parent in the linked list of all shown
    // popups
    //
    for ( wxWindowList::Node *node = m_svShownPopups.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow*                   pWin = node->GetData();

        if (pWin->GetParent() == pWinParent )
            return (wxPopupWindow *)pWin;
    }
    return NULL;
} // end of wxPopupWindow::FindPopupFor

