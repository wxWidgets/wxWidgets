/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif

#include "wx/statbox.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
    EVT_ERASE_BACKGROUND(wxStaticBox::OnEraseBackground)
END_EVENT_TABLE()

#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    if ( !MSWCreateControl(wxT("BUTTON"), BS_GROUPBOX) )
        return FALSE;

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

wxSize wxStaticBox::DoGetBestSize()
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wBox;
    GetTextExtent(wxGetWindowText(m_hWnd), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wBox, hBox);
}

WXHBRUSH wxStaticBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                 WXUINT message,
                                 WXWPARAM wParam,
                                 WXLPARAM lParam)
{
#if wxUSE_CTL3D
    if ( m_useCtl3D )
    {
        HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
        return (WXHBRUSH) hbrush;
    }
#endif // wxUSE_CTL3D

    HDC hdc = (HDC)pDC;
    if (GetParent()->GetTransparentBackground())
        SetBkMode(hdc, TRANSPARENT);
    else
        SetBkMode(hdc, OPAQUE);

    const wxColour& colBack = GetBackgroundColour();
    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush= wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

// VZ: this is probably the most commented function in wxWindows, but I still
//     don't understand what it does and why. Wouldn't it be better to _never_
//     erase the background here? What would we lose if we didn't do it?
//     (FIXME)

// Shouldn't erase the whole window, since the static box must only paint its
// outline.
void wxStaticBox::OnEraseBackground(wxEraseEvent& event)
{
    // If we don't have this (call Default()), we don't paint the background properly.
    // If we do have this, we seem to overwrite enclosed controls.
    // Is it the WS_CLIPCHILDREN style that's causing the problems?
    // Probably - without this style, the background of the window will show through,
    // so the control doesn't have to paint it. The window background will always be
    // painted before all other controls, therefore there are no problems with
    // controls being hidden by the static box.
    // So, if we could specify wxCLIP_CHILDREN in window, or not, we could optimise painting better.
    // We would assume wxCLIP_CHILDREN in a frame and a scrolled window, but not in a panel.
    // Is this too platform-specific?? What else can we do? Not a lot, since we have to pass
    // this information from arbitrary wxWindow derivatives, and it depends on what you wish to
    // do with the windows.
    // Alternatively, just make sure that wxStaticBox is always at the back! There are probably
    // few other circumstances where it matters about child clipping. But what about painting onto
    // to panel, inside a groupbox? Doesn't appear, because the box wipes it out.
    wxWindow *parent = GetParent();
    if ( parent && parent->GetHWND() &&
        (::GetWindowLong(GetHwndOf(parent), GWL_STYLE) & WS_CLIPCHILDREN) )
    {
        // TODO: May in fact need to generate a paint event for inside this
        // control's rectangle, otherwise all controls are going to be clipped -
        // ugh.

        // let wxControl::OnEraseBackground() do the job
        event.Skip();
    }
    //else: do *not* call event.Skip() or wxControl will erase the background
}

long wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( nMsg == WM_NCHITTEST)
    {
        int xPos = LOWORD(lParam);  // horizontal position of cursor
        int yPos = HIWORD(lParam);  // vertical position of cursor

        ScreenToClient(&xPos, &yPos);

        // Make sure you can drag by the top of the groupbox, but let
        // other (enclosed) controls get mouse events also
        if (yPos < 10)
            return (long)HTCLIENT;
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

