/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "radiobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobox.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS

#ifndef __GNUWIN32__
    #include <commctrl.h>
#endif

    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

// VZ: the new behaviour is to create the radio buttons as children of the
//     radiobox instead of creating them as children of the radiobox' parent.
//
//     This seems more logical, more consistent with what other frameworks do
//     and allows tooltips to work with radioboxes, so there should be no
//     reason to revert to the backward compatible behaviour - but I still
//     leave this possibility just in case.

#define RADIOBTN_PARENT_IS_RADIOBOX 0

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// wnd proc for radio buttons
#ifdef __WIN32__
LRESULT APIENTRY _EXPORT wxRadioBtnWndProc(HWND hWnd,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam);

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static WXFARPROC s_wndprocRadioBtn = (WXFARPROC)NULL;

#endif // __WIN32__

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxRadioBox
// ---------------------------------------------------------------------------

int wxRadioBox::GetNumVer() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return m_majorDim;
    }
    else
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
}

int wxRadioBox::GetNumHor() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
    else
    {
        return m_majorDim;
    }
}

bool wxRadioBox::MSWCommand(WXUINT cmd, WXWORD id)
{
    if ( cmd == BN_CLICKED )
    {
        int selectedButton = -1;

        for ( int i = 0; i < m_noItems; i++ )
        {
            if ( id == wxGetWindowId(m_radioButtons[i]) )
            {
                selectedButton = i;

                break;
            }
        }

        wxASSERT_MSG( selectedButton != -1, wxT("click from alien button?") );

        if ( selectedButton != m_selectedButton )
        {
            m_selectedButton = selectedButton;

            SendNotificationEvent();
        }
        //else: don't generate events when the selection doesn't change

        return TRUE;
    }
    else
        return FALSE;
}

#if WXWIN_COMPATIBILITY
wxRadioBox::wxRadioBox(wxWindow *parent, wxFunction func, const char *title,
        int x, int y, int width, int height,
        int n, char **choices,
        int majorDim, long style, const char *name)
{
    wxString *choices2 = new wxString[n];
    for ( int i = 0; i < n; i ++) choices2[i] = choices[i];
    Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), n, choices2, majorDim, style,
            wxDefaultValidator, name);
    Callback(func);
    delete choices2;
}

#endif

// Radio box item
wxRadioBox::wxRadioBox()
{
    m_selectedButton = -1;
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_radioButtons = NULL;
    m_majorDim = 0;
    m_radioWidth = NULL;
    m_radioHeight = NULL;
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    // initialize members
    m_selectedButton = -1;
    m_noItems = 0;

    m_majorDim =  majorDim == 0 ? n : majorDim;
    m_noRowsOrCols = majorDim;

    // common initialization
    if ( !CreateControl(parent, id, pos, size, style, val, name) )
        return FALSE;

    // create the static box
    if ( !MSWCreateControl(wxT("BUTTON"), BS_GROUPBOX, pos, size, title, 0) )
        return FALSE;

    // and now create the buttons
    m_noItems = n;
#if RADIOBTN_PARENT_IS_RADIOBOX
    HWND hwndParent = GetHwnd();
#else
    HWND hwndParent = GetHwndOf(parent);
#endif

    // Some radio boxes test consecutive id.
    (void)NewControlId();
    m_radioButtons = new WXHWND[n];
    m_radioWidth = new int[n];
    m_radioHeight = new int[n];

    WXHFONT hfont = 0;
    wxFont& font = GetFont();
    if ( font.Ok() )
    {
        hfont = font.GetResourceHandle();
    }

    for ( int i = 0; i < n; i++ )
    {
        m_radioWidth[i] =
        m_radioHeight[i] = -1;
        long styleBtn = BS_AUTORADIOBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
        if ( i == 0 && style == 0 )
            styleBtn |= WS_GROUP;

        long newId = NewControlId();

        HWND hwndBtn = ::CreateWindow(_T("BUTTON"),
                                      choices[i],
                                      styleBtn,
                                      0, 0, 0, 0,   // will be set in SetSize()
                                      hwndParent,
                                      (HMENU)newId,
                                      wxGetInstance(),
                                      NULL);

        if ( !hwndBtn )
        {
            wxLogLastError("CreateWindow(radio btn)");

            return FALSE;
        }

        m_radioButtons[i] = (WXHWND)hwndBtn;

        SubclassRadioButton((WXHWND)hwndBtn);

        if ( hfont )
        {
            ::SendMessage(hwndBtn, WM_SETFONT, (WPARAM)hfont, 0L);
        }

        m_subControls.Add(newId);
    }

    // Create a dummy radio control to end the group.
    (void)::CreateWindow(_T("BUTTON"),
                         _T(""),
                         WS_GROUP | BS_AUTORADIOBUTTON | WS_CHILD,
                         0, 0, 0, 0, hwndParent,
                         (HMENU)NewControlId(), wxGetInstance(), NULL);

    SetSelection(0);

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

wxRadioBox::~wxRadioBox()
{
    m_isBeingDeleted = TRUE;

    if (m_radioButtons)
    {
        int i;
        for (i = 0; i < m_noItems; i++)
            ::DestroyWindow((HWND)m_radioButtons[i]);
        delete[] m_radioButtons;
    }

    if (m_radioWidth)
        delete[] m_radioWidth;
    if (m_radioHeight)
        delete[] m_radioHeight;

}

wxString wxRadioBox::GetLabel(int item) const
{
    wxCHECK_MSG( item >= 0 && item < m_noItems, wxT(""), wxT("invalid radiobox index") );

    return wxGetWindowText(m_radioButtons[item]);
}

void wxRadioBox::SetLabel(int item, const wxString& label)
{
    wxCHECK_RET( item >= 0 && item < m_noItems, wxT("invalid radiobox index") );

    m_radioWidth[item] = m_radioHeight[item] = -1;
    SetWindowText((HWND)m_radioButtons[item], label.c_str());
}

void wxRadioBox::SetLabel(int item, wxBitmap *bitmap)
{
    /*
       m_radioWidth[item] = bitmap->GetWidth() + FB_MARGIN;
       m_radioHeight[item] = bitmap->GetHeight() + FB_MARGIN;
     */
    wxFAIL_MSG(wxT("not implemented"));
}

int wxRadioBox::FindString(const wxString& s) const
{
    for (int i = 0; i < m_noItems; i++)
    {
        if ( s == wxGetWindowText(m_radioButtons[i]) )
            return i;
    }

    return wxNOT_FOUND;
}

void wxRadioBox::SetSelection(int N)
{
    wxCHECK_RET( (N >= 0) && (N < m_noItems), wxT("invalid radiobox index") );

    // Following necessary for Win32s, because Win32s translate BM_SETCHECK
    if (m_selectedButton >= 0 && m_selectedButton < m_noItems)
        ::SendMessage((HWND) m_radioButtons[m_selectedButton], BM_SETCHECK, 0, 0L);

    ::SendMessage((HWND)m_radioButtons[N], BM_SETCHECK, 1, 0L);
    ::SetFocus((HWND)m_radioButtons[N]);

    m_selectedButton = N;
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection() const
{
    return m_selectedButton;
}

// Find string for position
wxString wxRadioBox::GetString(int N) const
{
    return wxGetWindowText(m_radioButtons[N]);
}

// Restored old code.
void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    int widthOld, heightOld;
    GetSize(&widthOld, &heightOld);

    int xx = x;
    int yy = y;

    if (x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        xx = currentX;
    if (y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        yy = currentY;

#if RADIOBTN_PARENT_IS_RADIOBOX
    int y_offset = 0;
    int x_offset = 0;
#else
    int y_offset = yy;
    int x_offset = xx;
#endif

    int current_width, cyf;

    int cx1,cy1;
    wxGetCharSize(m_hWnd, &cx1, &cy1, & GetFont());

    // Attempt to have a look coherent with other platforms: We compute the
    // biggest toggle dim, then we align all items according this value.
    int maxWidth =  -1;
    int maxHeight = -1;

    int i;
    for (i = 0 ; i < m_noItems; i++)
    {
        int eachWidth;
        int eachHeight;
        if (m_radioWidth[i]<0)
        {
            // It's a labelled toggle
            GetTextExtent(wxGetWindowText(m_radioButtons[i]),
                          &current_width, &cyf);
            eachWidth = (int)(current_width + RADIO_SIZE);
            eachHeight = (int)((3*cyf)/2);
        }
        else
        {
            eachWidth = m_radioWidth[i];
            eachHeight = m_radioHeight[i];
        }

        if (maxWidth<eachWidth)
            maxWidth = eachWidth;
        if (maxHeight<eachHeight)
            maxHeight = eachHeight;
    }

    if (m_hWnd)
    {
        int totWidth;
        int totHeight;

        int nbHor = GetNumHor(),
            nbVer = GetNumVer();

        // this formula works, but I don't know why.
        // Please, be sure what you do if you modify it!!
        if (m_radioWidth[0]<0)
            totHeight = (nbVer * maxHeight) + cy1/2;
        else
            totHeight = nbVer * (maxHeight+cy1/2);
        totWidth  = nbHor * (maxWidth+cx1);

        int extraHeight = cy1;

#if defined(CTL3D) && !CTL3D
        // Requires a bigger group box in plain Windows
        extraHeight *= 3;
        extraHeight /= 2;
#endif

        // only change our width/height if asked for
        if ( width == -1 )
        {
            if ( sizeFlags & wxSIZE_AUTO_WIDTH )
                width = totWidth + cx1;
            else
                width = widthOld;
        }

        if ( height == -1 )
        {
            if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
                height = totHeight + extraHeight;
            else
                height = heightOld;
        }

        ::MoveWindow(GetHwnd(), xx, yy, width, height, TRUE);

        x_offset += cx1;
        y_offset += cy1;
    }

#if defined(CTL3D) && (!CTL3D)
    y_offset += (int)(cy1/2); // Fudge factor since buttons overlapped label
    // JACS 2/12/93. CTL3D draws group label quite high.
#endif
    int startX = x_offset;
    int startY = y_offset;

    for ( i = 0 ; i < m_noItems; i++)
    {
        // Bidimensional radio adjustment
        if (i&&((i%m_majorDim)==0)) // Why is this omitted for i = 0?
        {
            if (m_windowStyle & wxRA_VERTICAL)
            {
                y_offset = startY;
                x_offset += maxWidth + cx1;
            }
            else
            {
                x_offset = startX;
                y_offset += maxHeight;
                if (m_radioWidth[0]>0)
                    y_offset += cy1/2;
            }
        }
        int eachWidth;
        int eachHeight;
        if (m_radioWidth[i]<0)
        {
            // It's a labeled item
            GetTextExtent(wxGetWindowText(m_radioButtons[i]),
                          &current_width, &cyf);

            // How do we find out radio button bitmap size!!
            // By adjusting them carefully, manually :-)
            eachWidth = (int)(current_width + RADIO_SIZE);
            eachHeight = (int)((3*cyf)/2);
        }
        else
        {
            eachWidth = m_radioWidth[i];
            eachHeight = m_radioHeight[i];
        }

        // VZ: make all buttons of the same, maximal size - like this they
        //     cover the radiobox entirely and the radiobox tooltips are always
        //     shown (otherwise they are not when the mouse pointer is in the
        //     radiobox part not belonging to any radiobutton)
        ::MoveWindow((HWND)m_radioButtons[i],
                     x_offset, y_offset, maxWidth, maxHeight,
                     TRUE);

        if (m_windowStyle & wxRA_SPECIFY_ROWS)
        {
            y_offset += maxHeight;
            if (m_radioWidth[0]>0)
                y_offset += cy1/2;
        }
        else
            x_offset += maxWidth + cx1;
    }
}

void wxRadioBox::GetSize(int *width, int *height) const
{
    RECT rect;
    rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

    if (m_hWnd)
        wxFindMaxSize(m_hWnd, &rect);

    int i;
    for (i = 0; i < m_noItems; i++)
        wxFindMaxSize(m_radioButtons[i], &rect);

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

void wxRadioBox::GetPosition(int *x, int *y) const
{
    wxWindow *parent = GetParent();
    RECT rect = { -1, -1, -1, -1 };

    int i;
    for (i = 0; i < m_noItems; i++)
        wxFindMaxSize(m_radioButtons[i], &rect);

    if (m_hWnd)
        wxFindMaxSize(m_hWnd, &rect);

    // Since we now have the absolute screen coords, if there's a parent we
    // must subtract its top left corner
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    if (parent)
    {
        ::ScreenToClient((HWND) parent->GetHWND(), &point);
    }

    // We may be faking the client origin. So a window that's really at (0, 30)
    // may appear (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        point.x -= pt.x;
        point.y -= pt.y;
    }

    *x = point.x;
    *y = point.y;
}

void wxRadioBox::SetFocus()
{
    if (m_noItems > 0)
    {
        if (m_selectedButton == -1)
            ::SetFocus((HWND) m_radioButtons[0]);
        else
            ::SetFocus((HWND) m_radioButtons[m_selectedButton]);
    }

}

bool wxRadioBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return FALSE;

    int nCmdShow = show ? SW_SHOW : SW_HIDE;
    for ( int i = 0; i < m_noItems; i++ )
    {
        ::ShowWindow((HWND)m_radioButtons[i], nCmdShow);
    }

    return TRUE;
}

// Enable a specific button
void wxRadioBox::Enable(int item, bool enable)
{
    wxCHECK_RET( item >= 0 && item < m_noItems,
                 wxT("invalid item in wxRadioBox::Enable()") );

    ::EnableWindow((HWND) m_radioButtons[item], enable);
}

// Enable all controls
bool wxRadioBox::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

    for (int i = 0; i < m_noItems; i++)
        ::EnableWindow((HWND) m_radioButtons[i], enable);

    return TRUE;
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
    wxCHECK_RET( item >= 0 && item < m_noItems,
                 wxT("invalid item in wxRadioBox::Show()") );

    ::ShowWindow((HWND)m_radioButtons[item], show ? SW_SHOW : SW_HIDE);
}

// For single selection items only
wxString wxRadioBox::GetStringSelection() const
{
    wxString result;
    int sel = GetSelection();
    if (sel > -1)
        result = GetString(sel);

    return result;
}

bool wxRadioBox::SetStringSelection(const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
    {
        SetSelection (sel);
        return TRUE;
    }
    else
        return FALSE;
}

bool wxRadioBox::ContainsHWND(WXHWND hWnd) const
{
    int i;
    for (i = 0; i < Number(); i++)
    {
        if (GetRadioButtons()[i] == hWnd)
            return TRUE;
    }

    return FALSE;
}

void wxRadioBox::Command(wxCommandEvent & event)
{
    SetSelection (event.m_commandInt);
    ProcessCommand (event);
}

void wxRadioBox::SubclassRadioButton(WXHWND hWndBtn)
{
#ifdef __WIN32__
    HWND hwndBtn = (HWND)hWndBtn;

    if ( !s_wndprocRadioBtn )
        s_wndprocRadioBtn = (WXFARPROC)::GetWindowLong(hwndBtn, GWL_WNDPROC);

    // No GWL_USERDATA in Win16, so omit this subclassing.
    ::SetWindowLong(hwndBtn, GWL_WNDPROC, (long)wxRadioBtnWndProc);
    ::SetWindowLong(hwndBtn, GWL_USERDATA, (long)this);
#endif // __WIN32__
}

void wxRadioBox::SendNotificationEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_windowId);
    event.SetInt( m_selectedButton );
    event.SetString( GetString(m_selectedButton) );
    event.SetEventObject( this );
    ProcessCommand(event);
}

bool wxRadioBox::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    // also set the font of our radio buttons
    WXHFONT hfont = wxFont(font).GetResourceHandle();
    for ( int n = 0; n < m_noItems; n++ )
    {
        ::SendMessage((HWND)m_radioButtons[n], WM_SETFONT, (WPARAM)hfont, 0L);
    }

    return TRUE;
}

long wxRadioBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch ( nMsg )
    {
        case WM_CTLCOLORSTATIC:
            // set the colour of the radio buttons to be the same as ours
            {
                HDC hdc = (HDC)wParam;

                const wxColour& colBack = GetBackgroundColour();
                ::SetBkColor(hdc, wxColourToRGB(colBack));
                ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

                wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

                return (WXHBRUSH)brush->GetResourceHandle();
            }

        // This is required for the radiobox to be sensitive to mouse input,
        // e.g. for Dialog Editor.
        case WM_NCHITTEST:
            {
                int xPos = LOWORD(lParam);  // horizontal position of cursor
                int yPos = HIWORD(lParam);  // vertical position of cursor

                ScreenToClient(&xPos, &yPos);

                // Make sure you can drag by the top of the groupbox, but let
                // other (enclosed) controls get mouse events also
                if (yPos < 10)
                    return (long)HTCLIENT;
            }
            // fall through

        default:
            return wxControl::MSWWindowProc(nMsg, wParam, lParam);
    }
}

// ---------------------------------------------------------------------------
// window proc for radio buttons
// ---------------------------------------------------------------------------

#ifdef __WIN32__

LRESULT APIENTRY _EXPORT wxRadioBtnWndProc(HWND hwnd,
                                           UINT msg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
    bool processed = FALSE;
    if ( msg == WM_KEYDOWN
#if wxUSE_TOOLTIPS
         || msg == WM_NOTIFY
#endif // wxUSE_TOOLTIPS
       )
    {
        wxRadioBox *radiobox = (wxRadioBox *)::GetWindowLong(hwnd, GWL_USERDATA);

        wxCHECK_MSG( radiobox, 0, wxT("radio button without radio box?") );

#if wxUSE_TOOLTIPS && !defined(__GNUWIN32__)
        if ( msg == WM_NOTIFY )
        {
            NMHDR* hdr = (NMHDR *)lParam;
            if ( (int)hdr->code == TTN_NEEDTEXT )
            {
                wxToolTip *tt = radiobox->GetToolTip();
                if ( tt )
                {
                    TOOLTIPTEXT *ttt = (TOOLTIPTEXT *)lParam;
                    ttt->lpszText = (wxChar *)tt->GetTip().c_str();

                    processed = TRUE;
                }
            }
        }
        else // msg == WM_KEYDOWN
#endif // wxUSE_TOOLTIPS
        {
            processed = TRUE;

            int sel = radiobox->GetSelection();

            switch ( wParam )
            {
                case VK_UP:
                    sel--;
                    break;

                case VK_LEFT:
                    sel -= radiobox->GetNumVer();
                    break;

                case VK_DOWN:
                    sel++;
                    break;

                case VK_RIGHT:
                    sel += radiobox->GetNumVer();
                    break;

                case VK_TAB:
                    {
                        wxNavigationKeyEvent event;
                        event.SetDirection(!(::GetKeyState(VK_SHIFT) & 0x100));
                        event.SetWindowChange(FALSE);
                        event.SetEventObject(radiobox);

                        if ( radiobox->GetEventHandler()->ProcessEvent(event) )
                            return 0;
                    }
                    // fall through

                default:
                    processed = FALSE;
            }

            if ( processed )
            {
                if ( sel >= 0 && sel < radiobox->Number() )
                {
                    radiobox->SetSelection(sel);

                    // emulate the button click
                    radiobox->SendNotificationEvent();
                }
            }
        }
    }

    if ( processed )
        return 0;

    return ::CallWindowProc(CASTWNDPROC s_wndprocRadioBtn, hwnd, msg, wParam, lParam);
}

#endif // __WIN32__

