/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/setup.h"
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobox.h"
#endif

#include "wx/os2/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// wnd proc for radio buttons
MRESULT wxRadioBtnWndProc(HWND hWnd,
                          UINT message,
                          MPARAM wParam,
                          MPARAM lParam);

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static s_wndprocRadioBtn = NULL;

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

bool wxRadioBox::OS2Command(WXUINT cmd, WXWORD id)
{
// TODO:
/*
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
*/
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
#if wxUSE_VALIDATORS
        const wxValidator& val, const wxString& name)
#endif
{
    m_selectedButton = -1;
    m_noItems = n;

    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(val);
#endif
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

    long msStyle = 0; // TODO: GROUP_FLAGS;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(0, &want3D);

    HWND hwndParent = (HWND)parent->GetHWND();
/*
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
*/
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
// TODO:
/*
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

        m_subControls.Append((wxObject *)(WXDWORD)(WXWORD)newId);
*/
    }

    // Create a dummy radio control to end the group.
//    (void)CreateWindowEx(0, RADIO_CLASS, wxT(""), WS_GROUP | RADIO_FLAGS,
//                         0, 0, 0, 0, hwndParent,
//                         (HMENU)NewControlId(), wxGetInstance(), NULL);

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
// TODO:
/*
        for (i = 0; i < m_noItems; i++)
            ::DestroyWindow((HWND)m_radioButtons[i]);
        delete[] m_radioButtons;
*/
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
// TODO:    SetWindowText((HWND)m_radioButtons[item], label.c_str());
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
// TODO:
/*
    if (m_selectedButton >= 0 && m_selectedButton < m_noItems)
        ::SendMessage((HWND) m_radioButtons[m_selectedButton], BM_SETCHECK, 0, 0L);

    ::SendMessage((HWND)m_radioButtons[N], BM_SETCHECK, 1, 0L);
    ::SetFocus((HWND)m_radioButtons[N]);
*/
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

    if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        xx = currentX;
    if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        yy = currentY;

    wxString buf;

    int y_offset = yy;
    int x_offset = xx;
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
            buf = wxGetWindowText(m_radioButtons[i]);
            GetTextExtent(buf, &current_width, &cyf);
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

// TODO:  MoveWindow(GetHwnd(), x_offset, y_offset, width, height, TRUE);

        x_offset += cx1;
        y_offset += cy1;
    }

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
            buf = wxGetWindowText(m_radioButtons[i]);
            GetTextExtent(buf, &current_width, &cyf);

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

// TODO:
/*
        MoveWindow((HWND)m_radioButtons[i], x_offset, y_offset,
                   eachWidth, eachHeight,
                   TRUE);
*/
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
    rect.xLeft = -1; rect.xRight = -1; rect.yTop = -1; rect.yBottom = -1;

    if (m_hWnd)
        wxFindMaxSize(m_hWnd, &rect);

    int i;
    for (i = 0; i < m_noItems; i++)
        wxFindMaxSize(m_radioButtons[i], &rect);

    *width = rect.xRight - rect.xLeft;
    *height = rect.yBottom - rect.yTop;
}

void wxRadioBox::GetPosition(int *x, int *y) const
{
    wxWindow *parent = GetParent();
    RECT rect;
    rect.xLeft = -1; rect.xRight = -1; rect.yTop = -1; rect.yBottom = -1;

    int i;
    for (i = 0; i < m_noItems; i++)
        wxFindMaxSize(m_radioButtons[i], &rect);

    if (m_hWnd)
        wxFindMaxSize(m_hWnd, &rect);

    // Since we now have the absolute screen coords,
    // if there's a parent we must subtract its top left corner
    POINTL point;
    point.x = rect.xLeft;
    point.y = rect.yTop;
// TODO:
/*
    if (parent)
    {
        ::ScreenToClient((HWND) parent->GetHWND(), &point);
    }
*/
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

void wxRadioBox::SetFocus()
{
// TODO:
/*
    if (m_noItems > 0)
    {
        if (m_selectedButton == -1)
            ::SetFocus((HWND) m_radioButtons[0]);
        else
            ::SetFocus((HWND) m_radioButtons[m_selectedButton]);
    }
*/
}

bool wxRadioBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return FALSE;

    int nCmdShow = 0; // TODO: show ? SW_SHOW : SW_HIDE;
    for ( int i = 0; i < m_noItems; i++ )
    {
// TODO:        ::ShowWindow((HWND)m_radioButtons[i], nCmdShow);
    }

    return TRUE;
}

// Enable a specific button
void wxRadioBox::Enable(int item, bool enable)
{
    wxCHECK_RET( item >= 0 && item < m_noItems,
                 wxT("invalid item in wxRadioBox::Enable()") );

// TODO: ::EnableWindow((HWND) m_radioButtons[item], enable);
}

// Enable all controls
bool wxRadioBox::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

// TODO:
/*
    for (int i = 0; i < m_noItems; i++)
        ::EnableWindow((HWND) m_radioButtons[i], enable);
*/
    return TRUE;
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
    wxCHECK_RET( item >= 0 && item < m_noItems,
                 wxT("invalid item in wxRadioBox::Show()") );

// TODO:    ::ShowWindow((HWND)m_radioButtons[item], show ? SW_SHOW : SW_HIDE);
}

WXHBRUSH wxRadioBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
        WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
// TODO:
/*
    if (GetParent()->GetTransparentBackground())
        SetBkMode((HDC) pDC, TRANSPARENT);
    else
        SetBkMode((HDC) pDC, OPAQUE);

    ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));
*/
    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

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
    {
        if (GetRadioButtons()[i] == hWnd)
            return TRUE;
    }

    return FALSE;
}

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.m_commandInt);
    ProcessCommand (event);
}

void wxRadioBox::SubclassRadioButton(WXHWND hWndBtn)
{
    HWND hwndBtn = (HWND)hWndBtn;

// TODO:
/*
    if ( !s_wndprocRadioBtn )
        s_wndprocRadioBtn = (WXFARPROC)::GetWindowLong(hwndBtn, GWL_WNDPROC);

    // No GWL_USERDATA in Win16, so omit this subclassing.
    ::SetWindowLong(hwndBtn, GWL_WNDPROC, (long)wxRadioBtnWndProc);
    ::SetWindowLong(hwndBtn, GWL_USERDATA, (long)this);
*/
}

void wxRadioBox::SendNotificationEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_windowId);
    event.SetInt( m_selectedButton );
    event.SetString( GetString(m_selectedButton) );
    event.SetEventObject( this );
    ProcessCommand(event);
}

// ---------------------------------------------------------------------------
// window proc for radio buttons
// ---------------------------------------------------------------------------

MRESULT wxRadioBtnWndProc(HWND hwnd,
                          UINT msg,
                          MPARAM wParam,
                          MPARAM lParam)
{
    bool processed = TRUE;
//    if ( msg != WM_KEYDOWN )
//        processed = FALSE;

    if ( processed )
    {
        wxRadioBox *radiobox = NULL; // TODO: (wxRadioBox *)::GetWindowLong(hwnd, GWL_USERDATA);

        wxCHECK_MSG( radiobox, 0, wxT("radio button without radio box?") );

        int sel = radiobox->GetSelection();

// TODO:
/*
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
*/
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

// TODO:
/*
    if ( !processed )
        return ::CallWindowProc(CASTWNDPROC s_wndprocRadioBtn, hwnd, msg, wParam, lParam);
    else
        return 0;
*/
    return 0;
}

