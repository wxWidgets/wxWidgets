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
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
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
    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

    parent->AddChild((wxButton *)this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    m_hWnd = (WXHWND)CreateWindowEx
                     (
                      MakeExtendedStyle(m_windowStyle),
                      wxT("BUTTON"),
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

    // bad hack added by Robert to make buttons at least
    // 80 pixels wide. There are probably better ways...
    // TODO. FIXME.
    wxSize nsize( GetSize() );
    if ((nsize.x < 80) || (nsize.y < 23))
    {
        if ((size.x == -1) && (nsize.x < 80))
            nsize.x = 80;
        if ((size.y == -1) && (nsize.y < 23))
            nsize.y = 23;
        SetSize( nsize );
    }

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

wxSize wxButton::DoGetBestSize() const
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
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

        // the size of a standard button in the dialog units is 50x14,
        // translate this to pixels
        // NB1: the multipliers come from the Windows convention
        // NB2: the extra +1/+2 were needed to get the size be the same as the
        //      size of the buttons in the standard dialog - I don't know how
        //      this happens, but on my system this size is 75x23 in pixels and
        //      23*8 isn't even divisible by 14... Would be nice to understand
        //      why these constants are needed though!
        s_sizeBtn.x = (50 * (dc.GetCharWidth() + 1))/4;
        s_sizeBtn.y = ((14 * dc.GetCharHeight()) + 2)/8;
    }

    return s_sizeBtn;
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

    // this doesn't work with bitmap buttons because it also removes the
    // "ownerdrawn" style...
    if ( btnOldDefault && !wxDynamicCast(btnOldDefault, wxBitmapButton) )
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
        case 1: // means that the message came from an accelerator
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
    const HDC& hdc = (HDC)pDC;

    const wxColour& colBack = GetBackgroundColour();
    ::SetBkColor(hdc, RGB(colBack.Red(), colBack.Green(), colBack.Blue()));

    const wxColour& colFor = GetForegroundColour();
    ::SetTextColor(hdc, RGB(colFor.Red(), colFor.Green(), colFor.Blue()));

    ::SetBkMode(hdc, OPAQUE);

    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(colBack,
                                                                 wxSOLID);
    backgroundBrush->RealizeResource();
    return (WXHBRUSH)backgroundBrush->GetResourceHandle();
}

long wxButton::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // make sure that we won't have BS_DEFPUSHBUTTON style any more if the
    // focus is being transfered to another button with the same parent -
    // otherwise, we could finish with 2 default buttons inside one panel
    if ( (nMsg == WM_KILLFOCUS) &&
         (GetWindowLong(GetHwnd(), GWL_STYLE) & BS_DEFPUSHBUTTON) )
    {
        wxWindow *parent = GetParent();
        wxWindow *win = wxFindWinFromHandle((WXHWND)wParam);
        if ( win && win->GetParent() == parent )
        {
            wxPanel *panel = wxDynamicCast(parent, wxPanel);
            if ( panel )
            {
                panel->SetDefaultItem(this);
            }
            // else: I don't know what to do - we'll still have the problem
            //       with multiple default buttons in a dialog...
        }
    }

    // let the base class do all real processing
    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}
