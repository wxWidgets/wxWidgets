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
    #include "wx/panel.h"
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

wxSize wxButton::DoGetBestSize()
{
    wxString label = wxGetWindowText(GetHWND());
    int wBtn;
    GetTextExtent(label, &wBtn, NULL);

    int wChar, hChar;
    wxGetCharSize(GetHWND(), &wChar, &hChar, &GetFont());

    // add a margin - the button is wider than just its label
    wBtn += 3*wChar;

    // the button height is proportional to the height of the font used
    int hBtn = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(hChar);

    return wxSize(wBtn, hBtn);
}

/* static */
wxSize wxButton::GetDefaultSize()
{
    // the base unit is the height of the system GUI font
    int wChar, hChar;
    wxGetCharSize(0, &wChar, &hChar, NULL);

    // the button height is proportional to the height of the font used
    int hBtn = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(hChar);

    // and the width/height ration is 75/23
    return wxSize((75 * hBtn) / 23, hBtn);
}

// ----------------------------------------------------------------------------
// set this button as the default one in its panel
// ----------------------------------------------------------------------------

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxButton *btnOldDefault = NULL;
    wxPanel *panel = wxDynamicCast(parent, wxPanel);
    if ( panel )
    {
        btnOldDefault = panel->GetDefaultItem();
        panel->SetDefaultItem(this);
    }

    if ( parent )
    {
        SendMessage(GetWinHwnd(parent), DM_SETDEFID, m_windowId, 0L);
    }

    if ( btnOldDefault )
    {
        // remove the BS_DEFPUSHBUTTON style from the other button
        long style = GetWindowLong(GetHwndOf(btnOldDefault), GWL_STYLE);
        style &= ~BS_DEFPUSHBUTTON;
        SendMessage(GetHwndOf(btnOldDefault), BM_SETSTYLE, style, 1L);
    }

    // set this button as the default
    long style = GetWindowLong(GetHwnd(), GWL_STYLE);
    style |= BS_DEFPUSHBUTTON;
    SendMessage(GetHwnd(), BM_SETSTYLE, style, 1L);
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

