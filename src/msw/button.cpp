/////////////////////////////////////////////////////////////////////////////
// Name:        msw/button.cpp
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

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

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

    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();


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

    wxSize sz = GetDefaultSize();
    if (wBtn > sz.x) sz.x = wBtn;
    if (hBtn > sz.y) sz.y = hBtn;

    return sz;
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

    if ( btnOldDefault )
    {
        // remove the BS_DEFPUSHBUTTON style from the other button
        long style = GetWindowLong(GetHwndOf(btnOldDefault), GWL_STYLE);

        // don't do it with the owner drawn buttons because it will reset
        // BS_OWNERDRAW style bit too (BS_OWNERDRAW & BS_DEFPUSHBUTTON != 0)!
        if ( (style & BS_OWNERDRAW) != BS_OWNERDRAW )
        {
            style &= ~BS_DEFPUSHBUTTON;
            SendMessage(GetHwndOf(btnOldDefault), BM_SETSTYLE, style, 1L);
        }
        else
        {
            // redraw the button - it will notice itself that it's not the
            // default one any longer
            btnOldDefault->Refresh();
        }
    }

    // set this button as the default
    long style = GetWindowLong(GetHwnd(), GWL_STYLE);
    if ( (style & BS_OWNERDRAW) != BS_OWNERDRAW )
    {
        style |= BS_DEFPUSHBUTTON;
        SendMessage(GetHwnd(), BM_SETSTYLE, style, 1L);
    }
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

long wxButton::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // when we receive focus, we want to become the default button in our
    // parent panel
    if ( nMsg == WM_SETFOCUS )
    {
        SetDefault();

        // let the default processign take place too
    }

    // let the base class do all real processing
    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

// ----------------------------------------------------------------------------
// owner-drawn buttons support
// ----------------------------------------------------------------------------

#ifdef __WIN32__

// drawing helpers

static void DrawButtonText(HDC hdc,
                           RECT *pRect,
                           const wxString& text,
                           COLORREF col)
{
    COLORREF colOld = SetTextColor(hdc, col);
    int modeOld = SetBkMode(hdc, TRANSPARENT);

    DrawText(hdc, text, text.length(), pRect,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetBkMode(hdc, modeOld);
    SetTextColor(hdc, colOld);
}

static void DrawRect(HDC hdc, const RECT& r)
{
    MoveToEx(hdc, r.left, r.top, NULL);
    LineTo(hdc, r.right, r.top);
    LineTo(hdc, r.right, r.bottom);
    LineTo(hdc, r.left, r.bottom);
    LineTo(hdc, r.left, r.top);
}

void wxButton::MakeOwnerDrawn()
{
    long style = GetWindowLong(GetHwnd(), GWL_STYLE);
    if ( (style & BS_OWNERDRAW) != BS_OWNERDRAW )
    {
        // make it so
        style |= BS_OWNERDRAW;
        SetWindowLong(GetHwnd(), GWL_STYLE, style);
    }
}

bool wxButton::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return FALSE;
    }

    MakeOwnerDrawn();

    Refresh();

    return TRUE;
}

bool wxButton::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return FALSE;
    }

    MakeOwnerDrawn();

    Refresh();

    return TRUE;
}

/*
   The button frame looks like this normally:

   WWWWWWWWWWWWWWWWWWB
   WHHHHHHHHHHHHHHHHGB  W = white       (HILIGHT)
   WH               GB  H = light grey  (LIGHT)
   WH               GB  G = dark grey   (SHADOW)
   WH               GB  B = black       (DKSHADOW)
   WH               GB
   WGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBB

   When the button is selected, the button becomes like this (the total button
   size doesn't change):

   BBBBBBBBBBBBBBBBBBB
   BWWWWWWWWWWWWWWWWBB
   BWHHHHHHHHHHHHHHGBB
   BWH             GBB
   BWH             GBB
   BWGGGGGGGGGGGGGGGBB
   BBBBBBBBBBBBBBBBBBB
   BBBBBBBBBBBBBBBBBBB

   When the button is pushed (while selected) it is like:

   BBBBBBBBBBBBBBBBBBB
   BGGGGGGGGGGGGGGGGGB
   BG               GB
   BG               GB
   BG               GB
   BG               GB
   BGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBB
*/

static void DrawButtonFrame(HDC hdc, const RECT& rectBtn,
                            bool selected, bool pushed)
{
    RECT r;
    CopyRect(&r, &rectBtn);

    HPEN hpenBlack   = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW)),
         hpenGrey    = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW)),
         hpenLightGr = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DLIGHT)),
         hpenWhite   = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));

    HPEN hpenOld = (HPEN)SelectObject(hdc, hpenBlack);

    r.right--;
    r.bottom--;

    if ( pushed )
    {
        DrawRect(hdc, r);

        (void)SelectObject(hdc, hpenGrey);
        InflateRect(&r, -1, -1);

        DrawRect(hdc, r);
    }
    else // !pushed
    {
        if ( selected )
        {
            DrawRect(hdc, r);

            InflateRect(&r, -1, -1);
        }

        MoveToEx(hdc, r.left, r.bottom, NULL);
        LineTo(hdc, r.right, r.bottom);
        LineTo(hdc, r.right, r.top - 1);

        (void)SelectObject(hdc, hpenWhite);
        MoveToEx(hdc, r.left, r.bottom - 1, NULL);
        LineTo(hdc, r.left, r.top);
        LineTo(hdc, r.right, r.top);

        (void)SelectObject(hdc, hpenLightGr);
        MoveToEx(hdc, r.left + 1, r.bottom - 2, NULL);
        LineTo(hdc, r.left + 1, r.top + 1);
        LineTo(hdc, r.right - 1, r.top + 1);

        (void)SelectObject(hdc, hpenGrey);
        MoveToEx(hdc, r.left + 1, r.bottom - 1, NULL);
        LineTo(hdc, r.right - 1, r.bottom - 1);
        LineTo(hdc, r.right - 1, r.top);
    }

    (void)SelectObject(hdc, hpenOld);
    DeleteObject(hpenWhite);
    DeleteObject(hpenLightGr);
    DeleteObject(hpenGrey);
    DeleteObject(hpenBlack);
}

bool wxButton::MSWOnDraw(WXDRAWITEMSTRUCT *wxdis)
{
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)wxdis;

    RECT rectBtn;
    CopyRect(&rectBtn, &lpDIS->rcItem);

    COLORREF colBg = wxColourToRGB(GetBackgroundColour()),
             colFg = wxColourToRGB(GetForegroundColour());

    HDC hdc = lpDIS->hDC;
    UINT state = lpDIS->itemState;

    // first, draw the background
    HBRUSH hbrushBackground = ::CreateSolidBrush(colBg);

    FillRect(hdc, &rectBtn, hbrushBackground);

    // draw the border for the current state
    bool selected = (state & ODS_SELECTED) != 0;
    if ( !selected )
    {
        wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
        if ( panel )
        {
            selected = panel->GetDefaultItem() == this;
        }
    }
    bool pushed = (SendMessage(GetHwnd(), BM_GETSTATE, 0, 0) & BST_PUSHED) != 0;

    DrawButtonFrame(hdc, rectBtn, selected, pushed);

    // draw the focus rect if needed
    if ( state & ODS_FOCUS )
    {
        RECT rectFocus;
        CopyRect(&rectFocus, &rectBtn);

        // I don't know where does this constant come from, but this is how
        // Windows draws them
        InflateRect(&rectFocus, -4, -4);

        DrawFocusRect(hdc, &rectFocus);
    }

    if ( pushed )
    {
        // the label is shifted by 1 pixel to create "pushed" effect
        OffsetRect(&rectBtn, 1, 1);
    }

    DrawButtonText(hdc, &rectBtn, GetLabel(),
                   state & ODS_DISABLED ? GetSysColor(COLOR_GRAYTEXT)
                                        : colFg);

    ::DeleteObject(hbrushBackground);

    return TRUE;
}

#endif // __WIN32__
