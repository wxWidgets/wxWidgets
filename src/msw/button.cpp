/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
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
    #pragma implementation "button.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/brush.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
#endif

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    if ( !CreateBase(parent, id, pos, size, style, name) )
        return FALSE;

    SetValidator(validator);

    parent->AddChild((wxButton *)this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    m_hWnd = (WXHWND)CreateWindowEx
                     (
                      MakeExtendedStyle(m_windowStyle),
                      _T("BUTTON"),
                      label,
                      WS_VISIBLE | WS_TABSTOP | WS_CHILD,
                      0, 0, 0, 0, 
                      GetWinHwnd(parent),
                      (HMENU)m_windowId,
                      wxGetInstance(),
                      NULL
                     );

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(parent->GetFont());

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

wxButton::~wxButton()
{
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if ( panel )
    {
        if ( panel->GetDefaultItem() == this )
        {
            // don't leave the panel with invalid default item
            panel->SetDefaultItem(NULL);
        }
    }
}

// ----------------------------------------------------------------------------
// size management including autosizing
// ----------------------------------------------------------------------------

void wxButton::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    int x1 = x;
    int y1 = y;
    if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        x1 = currentX;
    if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        y1 = currentY;

    AdjustForParentClientOrigin(x1, y1, sizeFlags);

    int actualWidth = width;
    int actualHeight = height;
    int ww, hh;
    GetSize(&ww, &hh);

    int current_width;
    int cyf;
    wxString buf = wxGetWindowText(GetHWND());
    GetTextExtent(buf, &current_width, &cyf, NULL, NULL, &GetFont());

    // If we're prepared to use the existing width, then...
    if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    {
        actualWidth = ww;
    }
    else if (width == -1)
    {
        int cx;
        int cy;
        wxGetCharSize(GetHWND(), &cx, &cy, & this->GetFont());
        actualWidth = (int)(current_width + 3*cx) ;
    }

    // If we're prepared to use the existing height, then...
    if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    {
        actualHeight = hh;
    }
    else if (height == -1)
    {
        actualHeight = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cyf);
    }

    MoveWindow(GetHwnd(), x1, y1, actualWidth, actualHeight, TRUE);
}

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxPanel *panel = wxDynamicCast(parent, wxPanel);
    if ( panel )
        panel->SetDefaultItem(this);

    if ( parent )
    {
        SendMessage(GetWinHwnd(parent), DM_SETDEFID, m_windowId, 0L);
    }

    SendMessage(GetHwnd(), BM_SETSTYLE, BS_DEFPUSHBUTTON, 1L);
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);

    return ProcessCommand(event);
}

void wxButton::Command(wxCommandEvent & event)
{
    ProcessCommand(event);
}

// ----------------------------------------------------------------------------
// event/message handlers
// ----------------------------------------------------------------------------

bool wxButton::MSWCommand(WXUINT param, WXWORD id)
{
    bool processed = FALSE;
    switch ( param )
    {
        case 1:                                             // 1 for accelerator
        case BN_CLICKED:
            processed = SendClickEvent();
            break;
    }

    return processed;
}

WXHBRUSH wxButton::OnCtlColor(WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

