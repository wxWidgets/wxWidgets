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
    #include <stdio.h>
    #include "wx/setup.h"
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobox.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// get the id of the window
#ifdef __WIN32__
    #define GET_WIN_ID(hwnd) ::GetWindowLong((HWND)hwnd, GWL_ID)
#else // Win16
    #define GET_WIN_ID(hwnd) ::GetWindowWord((HWND)hwnd, GWW_ID)
#endif // Win32/16

// wnd proc for radio buttons
#ifdef __WIN32__
LRESULT APIENTRY _EXPORT wxRadioBtnWndProc(HWND hWnd,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam);
#endif

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
// static WNDPROC s_wndprocRadioBtn = (WNDPROC)NULL;
static WXFARPROC s_wndprocRadioBtn = (WXFARPROC)NULL;

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

bool wxRadioBox::MSWCommand(WXUINT param, WXWORD id)
{
    if ( param == BN_CLICKED )
    {
        m_selectedButton = -1;

        for ( int i = 0; i < m_noItems; i++ )
        {
            if ( id == GET_WIN_ID(m_radioButtons[i]) )
            {
                m_selectedButton = i;

                break;
            }
        }

        wxASSERT_MSG( m_selectedButton != -1, "click from alien button?" );

        wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_windowId);
        event.SetInt( m_selectedButton );
        event.SetEventObject( this );
        ProcessCommand(event);

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

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size,
        int n, const wxString choices[],
        int majorDim, long style,
        const wxValidator& val, const wxString& name)
{
    m_selectedButton = -1;
    m_noItems = n;

    SetName(name);
    SetValidator(val);

    parent->AddChild(this);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();

    m_windowStyle = (long&)style;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    if ( majorDim == 0 )
        m_majorDim = n;
    else
        m_majorDim = majorDim;
    m_noRowsOrCols = majorDim;

    long msStyle = GROUP_FLAGS;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(0, &want3D);
    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    /*
       if ( want3D || wxStyleHasBorder(m_windowStyle) )
       msStyle |= WS_BORDER;
     */

    HWND hwndParent = (HWND)parent->GetHWND();

    m_hWnd = (WXHWND)::CreateWindowEx
                        (
                         (DWORD)exStyle,
                         GROUP_CLASS,
                         title,
                         msStyle,
                         0, 0, 0, 0,
                         hwndParent,
                         (HMENU)m_windowId,
                         wxGetInstance(),
                         NULL
                        );

#if wxUSE_CTL3D
    if (want3D)
    {
        Ctl3dSubclassCtl((HWND)m_hWnd);
        m_useCtl3D = TRUE;
    }
#endif // wxUSE_CTL3D

    SetFont(parent->GetFont());

    SubclassWin(m_hWnd);

    // Some radio boxes test consecutive id.
    (void)NewControlId();
    m_radioButtons = new WXHWND[n];
    m_radioWidth = new int[n];
    m_radioHeight = new int[n];
    int i;
    for (i = 0; i < n; i++)
    {
        m_radioWidth[i] = m_radioHeight[i] = -1;
        long groupStyle = 0;
        if ( i == 0 && style == 0 )
            groupStyle = WS_GROUP;
        long newId = NewControlId();
        long msStyle = groupStyle | RADIO_FLAGS;

        HWND hwndBtn = CreateWindowEx(exStyle, RADIO_CLASS,
                                      choices[i], msStyle,
                                      0,0,0,0,
                                      hwndParent,
                                      (HMENU)newId, wxGetInstance(),
                                      NULL);

        m_radioButtons[i] = (WXHWND)hwndBtn;
        SubclassRadioButton((WXHWND)hwndBtn);

        wxFont& font = GetFont();
        if ( font.Ok() )
        {
            SendMessage(hwndBtn, WM_SETFONT,
                        (WPARAM)font.GetResourceHandle(), 0L);
        }

        m_subControls.Append((wxObject *)newId);
    }

    // Create a dummy radio control to end the group.
    (void)CreateWindowEx(0, RADIO_CLASS, "", WS_GROUP | RADIO_FLAGS,
                         0, 0, 0, 0, hwndParent,
                         (HMENU)NewControlId(), wxGetInstance(), NULL);

    SetSelection(0);

    SetSize(x, y, width, height);

    return TRUE;
}

wxRadioBox::~wxRadioBox()
{
    m_isBeingDeleted = TRUE;

    if (m_radioButtons)
    {
        int i;
        for (i = 0; i < m_noItems; i++)
            DestroyWindow((HWND) m_radioButtons[i]);
        delete[] m_radioButtons;
    }
    if (m_radioWidth)
        delete[] m_radioWidth;
    if (m_radioHeight)
        delete[] m_radioHeight;
    if (m_hWnd)
        ::DestroyWindow((HWND) m_hWnd);
    m_hWnd = 0;

}

wxString wxRadioBox::GetLabel(int item) const
{
    GetWindowText((HWND)m_radioButtons[item], wxBuffer, 300);
    return wxString(wxBuffer);
}

void wxRadioBox::SetLabel(int item, const wxString& label)
{
    m_radioWidth[item] = m_radioHeight[item] = -1;
    SetWindowText((HWND)m_radioButtons[item], (const char *)label);
}

void wxRadioBox::SetLabel(int item, wxBitmap *bitmap)
{
    /*
       m_radioWidth[item] = bitmap->GetWidth() + FB_MARGIN;
       m_radioHeight[item] = bitmap->GetHeight() + FB_MARGIN;
     */
}

int wxRadioBox::FindString(const wxString& s) const
{
    int i;
    for (i = 0; i < m_noItems; i++)
    {
        GetWindowText((HWND) m_radioButtons[i], wxBuffer, 1000);
        if (s == wxBuffer)
            return i;
    }
    return -1;
}

void wxRadioBox::SetSelection(int N)
{
    wxCHECK_RET( (N >= 0) && (N < m_noItems), "invalid radiobox index" );

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
  int xx = x;
  int yy = y;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    xx = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    yy = currentY;

  char buf[400];

  int y_offset = yy;
  int x_offset = xx;
  int current_width, cyf;

  int cx1,cy1;
  wxGetCharSize(m_hWnd, &cx1, &cy1, & GetFont());
  // Attempt to have a look coherent with other platforms:
  // We compute the biggest toggle dim, then we align all
  // items according this value.
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
      GetWindowText((HWND) m_radioButtons[i], buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL, & GetFont());
      eachWidth = (int)(current_width + RADIO_SIZE);
      eachHeight = (int)((3*cyf)/2);
    }
    else
    {
      eachWidth = m_radioWidth[i];
      eachHeight = m_radioHeight[i];
    }
    if (maxWidth<eachWidth) maxWidth = eachWidth;
    if (maxHeight<eachHeight) maxHeight = eachHeight;
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

#if (!CTL3D)
    // Requires a bigger group box in plain Windows
    MoveWindow((HWND) m_hWnd,x_offset,y_offset,totWidth+cx1,totHeight+(3*cy1)/2,TRUE);
#else
    MoveWindow((HWND) m_hWnd,x_offset,y_offset,totWidth+cx1,totHeight+cy1,TRUE);
#endif
    x_offset += cx1;
    y_offset += cy1;
  }

#if (!CTL3D)
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
      GetWindowText((HWND) m_radioButtons[i], buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL, & GetFont());

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

    MoveWindow((HWND) m_radioButtons[i],x_offset,y_offset,eachWidth,eachHeight,TRUE);
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
    RECT rect;
    rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

    int i;
    for (i = 0; i < m_noItems; i++)
        wxFindMaxSize(m_radioButtons[i], &rect);

    if (m_hWnd)
        wxFindMaxSize(m_hWnd, &rect);

    // Since we now have the absolute screen coords,
    // if there's a parent we must subtract its top left corner
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    if (parent)
    {
        ::ScreenToClient((HWND) parent->GetHWND(), &point);
    }
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        point.x -= pt.x;
        point.y -= pt.y;
    }

    *x = point.x;
    *y = point.y;
}

wxString wxRadioBox::GetLabel() const
{
    if (m_hWnd)
    {
        GetWindowText((HWND) m_hWnd, wxBuffer, 300);
        return wxString(wxBuffer);
    }
    else return wxString("");
}

void wxRadioBox::SetLabel(const wxString& label)
{
    if (m_hWnd)
        SetWindowText((HWND) m_hWnd, label);
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
    m_isShown = show;
    int cshow;
    if (show)
        cshow = SW_SHOW;
    else
        cshow = SW_HIDE;
    if (m_hWnd)
        ShowWindow((HWND) m_hWnd, cshow);
    int i;
    for (i = 0; i < m_noItems; i++)
        ShowWindow((HWND) m_radioButtons[i], cshow);
    return TRUE;
}

// Enable a specific button
void wxRadioBox::Enable(int item, bool enable)
{
    if (item<0)
        wxWindow::Enable(enable);
    else if (item < m_noItems)
        ::EnableWindow((HWND) m_radioButtons[item], enable);
}

// Enable all controls
void wxRadioBox::Enable(bool enable)
{
    wxControl::Enable(enable);

    int i;
    for (i = 0; i < m_noItems; i++)
        ::EnableWindow((HWND) m_radioButtons[i], enable);
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
    if (item<0)
        wxRadioBox::Show(show);
    else if (item < m_noItems)
    {
        int cshow;
        if (show)
            cshow = SW_SHOW;
        else
            cshow = SW_HIDE;
        ShowWindow((HWND) m_radioButtons[item], cshow);
    }
}

WXHBRUSH wxRadioBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
        WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if wxUSE_CTL3D
    if ( m_useCtl3D )
    {
        HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
        return (WXHBRUSH) hbrush;
    }
#endif

    if (GetParent()->GetTransparentBackground())
        SetBkMode((HDC) pDC, TRANSPARENT);
    else
        SetBkMode((HDC) pDC, OPAQUE);

    ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

    // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
    // has a zero usage count.
    //  backgroundBrush->RealizeResource();
    return (WXHBRUSH) backgroundBrush->GetResourceHandle();
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
        if (GetRadioButtons()[i] == hWnd)
            return TRUE;
    return FALSE;
}

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.m_commandInt);
    ProcessCommand (event);
}

long wxRadioBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if (nMsg == WM_NCHITTEST)
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

void wxRadioBox::SubclassRadioButton(WXHWND hWndBtn)
{
    HWND hwndBtn = (HWND)hWndBtn;

    if ( !s_wndprocRadioBtn )
        s_wndprocRadioBtn = (WXFARPROC)::GetWindowLong(hwndBtn, GWL_WNDPROC);
//        s_wndprocRadioBtn = (WNDPROC)::GetWindowLong(hwndBtn, GWL_WNDPROC);

    // No GWL_USERDATA in Win16, so omit this subclassing.
#ifdef __WIN32__
    ::SetWindowLong(hwndBtn, GWL_WNDPROC, (long)wxRadioBtnWndProc);
    ::SetWindowLong(hwndBtn, GWL_USERDATA, (long)this);
#endif
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
    bool processed = TRUE;
    if ( msg != WM_KEYDOWN )
        processed = FALSE;        

    if ( processed )
    {
        wxRadioBox *radiobox = (wxRadioBox *)::GetWindowLong(hwnd, GWL_USERDATA);

        wxCHECK_MSG( radiobox, 0, "radio button without radio box?" );

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
                radiobox->SetSelection(sel);
        }
    }

    if ( !processed )
        return ::CallWindowProc(CASTWNDPROC s_wndprocRadioBtn, hwnd, msg, wParam, lParam);
    else
        return 0;
}
#endif

