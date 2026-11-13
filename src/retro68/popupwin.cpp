/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/popupwin.cpp
// Purpose:     wxPopupWindow implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

#include "wx/retro68/private/qd.h"

short wxPopupWindow::GetWindowProcID() const
{
    // Popup windows don't have any decorations, wxUniversal draws the
    // borders for them itself.
    return plainDBox;
}

#endif // wxUSE_POPUPWIN
