///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/popupwin.cpp
// Purpose:     implements wxPopupWindow for Palm OS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
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

#include "wx/palmos/private.h"     // for GetDesktopWindow()

IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)

// ============================================================================
// implementation
// ============================================================================

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    return false;
}

void wxPopupWindow::DoGetPosition(int *x, int *y) const
{
}

WXDWORD wxPopupWindow::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    return 0;
}

WXHWND wxPopupWindow::MSWGetParent() const
{
    return (WXHWND) 0;
}

bool wxPopupWindow::Show(bool show)
{
    return false;
}

#endif // #if wxUSE_POPUPWIN

