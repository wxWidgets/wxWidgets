/////////////////////////////////////////////////////////////////////////////
// Name:        msw/radiobox.cpp
// Purpose:     wxRadioBox implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "radiobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBOX

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobox.h"
    #include "wx/settings.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS
    #if !defined(__GNUWIN32_OLD__) || defined(__CYGWIN10__)
        #include <commctrl.h>
    #endif
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// TODO: wxCONSTRUCTOR
#if 0 // wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxRadioBoxStyle )

wxBEGIN_FLAGS( wxRadioBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxRA_SPECIFY_COLS)
    wxFLAGS_MEMBER(wxRA_HORIZONTAL)
    wxFLAGS_MEMBER(wxRA_SPECIFY_ROWS)
    wxFLAGS_MEMBER(wxRA_VERTICAL)

wxEND_FLAGS( wxRadioBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxRadioBox, wxControl,"wx/radiobox.h")

wxBEGIN_PROPERTIES_TABLE(wxRadioBox)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_RADIOBOX_SELECTED , wxCommandEvent )
    wxPROPERTY_FLAGS( WindowStyle , wxRadioBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

#else
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

/*
    selection
    content
        label
        dimension
        item
*/

// there are two possible ways to create the radio buttons: either as children
// of the radiobox or as siblings of it - allow playing with both variants for
// now, eventually we will choose the best one for our purposes
//
// two main problems are the keyboard navigation inside the radiobox (arrows
// should switch between buttons, not pass focus to the next control) and the
// tooltips - a tooltip is associated with the radiobox itself, not the
// children...
//
// the problems with setting this to 1:
// a) Alt-<mnemonic of radiobox> isn't handled properly by IsDialogMessage()
//    because it sets focus to the next control accepting it which is not a
//    radio button but a radiobox sibling in this case - the only solution to
//    this would be to handle Alt-<mnemonic> ourselves
// b) the problems with setting radiobox colours under Win98/2K were reported
//    but I couldn't reproduce it so I have no idea about what causes it
//
// the problems with setting this to 0:
// a) the tooltips are not shown for the radiobox - possible solution: make
//    TTM_WINDOWFROMPOS handling code in msw/tooltip.cpp work (easier said than
//    done because I don't know why it doesn't work)
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

int wxRadioBox::GetCount() const
{
    return m_noItems;
}

int wxRadioBox::GetColumnCount() const
{
    return GetNumHor();
}

int wxRadioBox::GetRowCount() const
{
    return GetNumVer();
}

// returns the number of rows
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

// returns the number of columns
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
        if (id == GetId())
            return true;

        int selectedButton = wxID_ANY;

        for ( int i = 0; i < m_noItems; i++ )
        {
            if ( id == wxGetWindowId(m_radioButtons[i]) )
            {
                selectedButton = i;

                break;
            }
        }

        if ( selectedButton == wxID_ANY )
        {
            // just ignore it - due to a hack with WM_NCHITTEST handling in our
            // wnd proc, we can receive dummy click messages when we click near
            // the radiobox edge (this is ugly but Julian wouldn't let me get
            // rid of this...)
            return false;
        }

        if ( selectedButton != m_selectedButton )
        {
            m_selectedButton = selectedButton;

            SendNotificationEvent();
        }
        //else: don't generate events when the selection doesn't change

        return true;
    }
    else
        return false;
}

// Radio box item
wxRadioBox::wxRadioBox()
{
    m_selectedButton = wxID_ANY;
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
    m_selectedButton = wxID_ANY;
    m_noItems = 0;

    m_majorDim = majorDim == 0 ? n : majorDim;
    m_noRowsOrCols = majorDim;

    // common initialization
    if ( !CreateControl(parent, id, pos, size, style, val, name) )
        return false;

    // create the static box
    if ( !MSWCreateControl(wxT("BUTTON"), BS_GROUPBOX | WS_GROUP,
                           pos, size, title, 0) )
        return false;

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
    wxFont font = GetFont();
    if ( font.Ok() )
    {
        hfont = font.GetResourceHandle();
    }

    for ( int i = 0; i < n; i++ )
    {
        m_radioWidth[i] =
        m_radioHeight[i] = wxDefaultCoord;
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
            wxLogLastError(wxT("CreateWindow(radio btn)"));

            return false;
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
                         wxEmptyString,
                         WS_GROUP | BS_AUTORADIOBUTTON | WS_CHILD,
                         0, 0, 0, 0, hwndParent,
                         (HMENU)NewControlId(), wxGetInstance(), NULL);

#ifdef __WXWINCE__
    // Set the z-order correctly
    SetWindowPos(GetHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
#endif

    SetSelection(0);
    SetSize(pos.x, pos.y, size.x, size.y);

    // Now that we have items determine what is the best size and set it.
    SetBestSize(size);

    return true;
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, title, pos, size, chs.GetCount(),
                  chs.GetStrings(), majorDim, style, val, name);
}

wxRadioBox::~wxRadioBox()
{
    m_isBeingDeleted = true;

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

void wxRadioBox::SetString(int item, const wxString& label)
{
    wxCHECK_RET( item >= 0 && item < m_noItems, wxT("invalid radiobox index") );

    m_radioWidth[item] = m_radioHeight[item] = wxDefaultCoord;
    SetWindowText((HWND)m_radioButtons[item], label.c_str());
}

void wxRadioBox::SetSelection(int N)
{
    wxCHECK_RET( (N >= 0) && (N < m_noItems), wxT("invalid radiobox index") );

    // Following necessary for Win32s, because Win32s translate BM_SETCHECK
    if (m_selectedButton >= 0 && m_selectedButton < m_noItems)
        ::SendMessage((HWND) m_radioButtons[m_selectedButton], BM_SETCHECK, 0, 0L);

    ::SendMessage((HWND)m_radioButtons[N], BM_SETCHECK, 1, 0L);

    m_selectedButton = N;
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection() const
{
    return m_selectedButton;
}

// Find string for position
wxString wxRadioBox::GetString(int item) const
{
    wxCHECK_MSG( item >= 0 && item < m_noItems, wxEmptyString,
                 wxT("invalid radiobox index") );

    return wxGetWindowText(m_radioButtons[item]);
}

// ----------------------------------------------------------------------------
// size calculations
// ----------------------------------------------------------------------------

wxSize wxRadioBox::GetMaxButtonSize() const
{
    // calculate the max button size
    int widthMax = 0,
        heightMax = 0;
    for ( int i = 0 ; i < m_noItems; i++ )
    {
        int width, height;
        if ( m_radioWidth[i] < 0 )
        {
            GetTextExtent(wxGetWindowText(m_radioButtons[i]), &width, &height);

            // adjust the size to take into account the radio box itself
            // FIXME this is totally bogus!
            width += RADIO_SIZE;
            height *= 3;
            height /= 2;
        }
        else
        {
            width = m_radioWidth[i];
            height = m_radioHeight[i];
        }

        if ( widthMax < width )
            widthMax = width;
        if ( heightMax < height )
            heightMax = height;
    }

    return wxSize(widthMax, heightMax);
}

wxSize wxRadioBox::GetTotalButtonSize(const wxSize& sizeBtn) const
{
    // the radiobox should be big enough for its buttons
    int cx1, cy1;
    wxGetCharSize(m_hWnd, &cx1, &cy1, GetFont());

    int extraHeight = cy1;

    /* We'll assume the adjustments below are OK for Win 3.1 too
#if defined(CTL3D) && !CTL3D
    // Requires a bigger group box in plain Windows
    extraHeight *= 3;
    extraHeight /= 2;
#endif
    */

    int height = GetNumVer() * sizeBtn.y + cy1/2 + extraHeight;
    int width  = GetNumHor() * (sizeBtn.x + cx1) + cx1;

    // Add extra space under the label, if it exists.
    if (!wxControl::GetLabel().IsEmpty())
        height += cy1/2;

    // and also wide enough for its label
    int widthLabel;
    GetTextExtent(GetTitle(), &widthLabel, NULL);
    widthLabel += RADIO_SIZE; // FIXME this is bogus too
    if ( widthLabel > width )
        width = widthLabel;

    return wxSize(width, height);
}

wxSize wxRadioBox::DoGetBestSize() const
{
    return GetTotalButtonSize(GetMaxButtonSize());
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

    if (x == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        xx = currentX;
    if (y == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        yy = currentY;

#if RADIOBTN_PARENT_IS_RADIOBOX
    int y_offset = 0;
    int x_offset = 0;
#else
    int y_offset = yy;
    int x_offset = xx;
#endif

    int cx1, cy1;
    wxGetCharSize(m_hWnd, &cx1, &cy1, GetFont());

    // Attempt to have a look coherent with other platforms: We compute the
    // biggest toggle dim, then we align all items according this value.
    wxSize maxSize = GetMaxButtonSize();
    int maxWidth = maxSize.x,
        maxHeight = maxSize.y;

    wxSize totSize = GetTotalButtonSize(maxSize);
    int totWidth = totSize.x,
        totHeight = totSize.y;

    // only change our width/height if asked for
    if ( width == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
            width = totWidth;
        else
            width = widthOld;
    }

    if ( height == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
            height = totHeight;
        else
            height = heightOld;
    }

    ::MoveWindow(GetHwnd(), xx, yy, width, height, TRUE);

    // Now position all the buttons: the current button will be put at
    // wxPoint(x_offset, y_offset) and the new row/column will start at
    // startX/startY. The size of all buttons will be the same wxSize(maxWidth,
    // maxHeight) except for the buttons in the last column which should extend
    // to the right border of radiobox and thus can be wider than this.

    // Also, remember that wxRA_SPECIFY_COLS means that we arrange buttons in
    // left to right order and m_majorDim is the number of columns while
    // wxRA_SPECIFY_ROWS means that the buttons are arranged top to bottom and
    // m_majorDim is the number of rows.

    x_offset += cx1;
    y_offset += cy1;

    // Add extra space under the label, if it exists.
    if (!wxControl::GetLabel().IsEmpty())
        y_offset += cy1/2;

    int startX = x_offset;
    int startY = y_offset;

    for ( int i = 0; i < m_noItems; i++ )
    {
        // the last button in the row may be wider than the other ones as the
        // radiobox may be wider than the sum of the button widths (as it
        // happens, for example, when the radiobox label is very long)
        bool isLastInTheRow;
        if ( m_windowStyle & wxRA_SPECIFY_COLS )
        {
            // item is the last in its row if it is a multiple of the number of
            // columns or if it is just the last item
            int n = i + 1;
            isLastInTheRow = ((n % m_majorDim) == 0) || (n == m_noItems);
        }
        else // wxRA_SPECIFY_ROWS
        {
            // item is the last in the row if it is in the last columns
            isLastInTheRow = i >= (m_noItems/m_majorDim)*m_majorDim;
        }

        // is this the start of new row/column?
        if ( i && (i % m_majorDim == 0) )
        {
            if ( m_windowStyle & wxRA_SPECIFY_ROWS )
            {
                // start of new column
                y_offset = startY;
                x_offset += maxWidth + cx1;
            }
            else // start of new row
            {
                x_offset = startX;
                y_offset += maxHeight;
                if (m_radioWidth[0]>0)
                    y_offset += cy1/2;
            }
        }

        int widthBtn;
        if ( isLastInTheRow )
        {
            // make the button go to the end of radio box
            widthBtn = startX + width - x_offset - 2*cx1;
            if ( widthBtn < maxWidth )
                widthBtn = maxWidth;
        }
        else
        {
            // normal button, always of the same size
            widthBtn = maxWidth;
        }

        // VZ: make all buttons of the same, maximal size - like this they
        //     cover the radiobox entirely and the radiobox tooltips are always
        //     shown (otherwise they are not when the mouse pointer is in the
        //     radiobox part not belonging to any radiobutton)
        ::MoveWindow((HWND)m_radioButtons[i],
                     x_offset, y_offset, widthBtn, maxHeight,
                     TRUE);

        // where do we put the next button?
        if ( m_windowStyle & wxRA_SPECIFY_ROWS )
        {
            // below this one
            y_offset += maxHeight;
            if (m_radioWidth[0]>0)
                y_offset += cy1/2;
        }
        else
        {
            // to the right of this one
            x_offset += widthBtn + cx1;
        }
    }
}

void wxRadioBox::GetSize(int *width, int *height) const
{
    RECT rect = { -1, -1, -1, -1 };

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
        ::SetFocus((HWND)m_radioButtons[m_selectedButton == wxID_ANY
                                            ? 0
                                            : m_selectedButton]);
    }

}

bool wxRadioBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    int nCmdShow = show ? SW_SHOW : SW_HIDE;
    for ( int i = 0; i < m_noItems; i++ )
    {
        ::ShowWindow((HWND)m_radioButtons[i], nCmdShow);
    }

    return true;
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
        return false;

    for (int i = 0; i < m_noItems; i++)
        ::EnableWindow((HWND) m_radioButtons[i], enable);

    return true;
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
    wxCHECK_RET( item >= 0 && item < m_noItems,
                 wxT("invalid item in wxRadioBox::Show()") );

    ::ShowWindow((HWND)m_radioButtons[item], show ? SW_SHOW : SW_HIDE);
}

bool wxRadioBox::ContainsHWND(WXHWND hWnd) const
{
    size_t count = GetCount();
    for ( size_t i = 0; i < count; i++ )
    {
        if ( GetRadioButtons()[i] == hWnd )
            return true;
    }

    return false;
}

void wxRadioBox::Command(wxCommandEvent & event)
{
    SetSelection (event.m_commandInt);
    SetFocus();
    ProcessCommand (event);
}

// NB: if this code is changed, wxGetWindowForHWND() which relies on having the
//     radiobox pointer in GWL_USERDATA for radio buttons must be updated too!
void wxRadioBox::SubclassRadioButton(WXHWND hWndBtn)
{
    HWND hwndBtn = (HWND)hWndBtn;

    if ( !s_wndprocRadioBtn )
        s_wndprocRadioBtn = (WXFARPROC)wxGetWindowProc(hwndBtn);

    wxSetWindowProc(hwndBtn, wxRadioBtnWndProc);
    wxSetWindowUserData(hwndBtn, this);
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
        return false;
    }

    // also set the font of our radio buttons
    WXHFONT hfont = wxFont(font).GetResourceHandle();
    for ( int n = 0; n < m_noItems; n++ )
    {
        HWND hwndBtn = (HWND)m_radioButtons[n];
        ::SendMessage(hwndBtn, WM_SETFONT, (WPARAM)hfont, 0L);

        // otherwise the buttons are not redrawn correctly
        ::InvalidateRect(hwndBtn, NULL, FALSE /* don't erase bg */);
    }

    return true;
}

// ----------------------------------------------------------------------------
// our window proc
// ----------------------------------------------------------------------------

WXLRESULT wxRadioBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
#if RADIOBTN_PARENT_IS_RADIOBOX
    switch ( nMsg )
    {
        // handle this message to set correct colours for our buttons here
        case WM_CTLCOLORSTATIC:
            {
                WXHDC hdc;
                WXHWND hwnd;
                UnpackCtlColor(wParam, lParam, &hdc, &hwnd);

                WXHBRUSH hbr = MSWControlColor((WXHDC)hdc);
                if ( hbr )
                    return (WXLRESULT)hbr;
                //else: fall through to default window proc
            }
    }
#endif // RADIOBTN_PARENT_IS_RADIOBOX

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// window proc for radio buttons
// ---------------------------------------------------------------------------

#ifdef __WIN32__

LRESULT APIENTRY _EXPORT wxRadioBtnWndProc(HWND hwnd,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
    switch ( message )
    {
        case WM_GETDLGCODE:
            // we must tell IsDialogMessage()/our kbd processing code that we
            // want to process arrows ourselves because neither of them is
            // smart enough to handle arrows properly for us
            {
                long lDlgCode = ::CallWindowProc(CASTWNDPROC s_wndprocRadioBtn, hwnd,
                                                 message, wParam, lParam);

                return lDlgCode | DLGC_WANTARROWS;
            }

#if wxUSE_TOOLTIPS
        case WM_NOTIFY:
            {
                NMHDR* hdr = (NMHDR *)lParam;
                if ( hdr->code == TTN_NEEDTEXT )
                {
                    wxRadioBox *
                        radiobox = (wxRadioBox *)wxGetWindowUserData(hwnd);

                    wxCHECK_MSG( radiobox, 0,
                                 wxT("radio button without radio box?") );

                    wxToolTip *tooltip = radiobox->GetToolTip();
                    if ( tooltip )
                    {
                        TOOLTIPTEXT *ttt = (TOOLTIPTEXT *)lParam;
                        ttt->lpszText = (wxChar *)tooltip->GetTip().c_str();
                    }

                    // processed
                    return 0;
                }
            }
            break;
#endif // wxUSE_TOOLTIPS

        case WM_KEYDOWN:
            {
                wxRadioBox *radiobox = (wxRadioBox *)wxGetWindowUserData(hwnd);

                wxCHECK_MSG( radiobox, 0, wxT("radio button without radio box?") );

                bool processed = true;

                wxDirection dir;
                switch ( wParam )
                {
                    case VK_UP:
                        dir = wxUP;
                        break;

                    case VK_LEFT:
                        dir = wxLEFT;
                        break;

                    case VK_DOWN:
                        dir = wxDOWN;
                        break;

                    case VK_RIGHT:
                        dir = wxRIGHT;
                        break;

                    default:
                        processed = false;

                        // just to suppress the compiler warning
                        dir = wxALL;
                }

                if ( processed )
                {
                    int selOld = radiobox->GetSelection();
                    int selNew = radiobox->GetNextItem
                                 (
                                  selOld,
                                  dir,
                                  radiobox->GetWindowStyle()
                                 );

                    if ( selNew != selOld )
                    {
                        radiobox->SetSelection(selNew);
                        radiobox->SetFocus();

                        // emulate the button click
                        radiobox->SendNotificationEvent();

                        return 0;
                    }
                }
            }
            break;

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            {
                wxRadioBox *radiobox = (wxRadioBox *)wxGetWindowUserData(hwnd);

                wxCHECK_MSG( radiobox, 0, wxT("radio button without radio box?") );

                // if we don't do this, no focus events are generated for the
                // radiobox and, besides, we need to notify the parent about
                // the focus change, otherwise the focus handling logic in
                // wxControlContainer doesn't work
                if ( message == WM_SETFOCUS )
                    radiobox->HandleSetFocus((WXHWND)wParam);
                else
                    radiobox->HandleKillFocus((WXHWND)wParam);
            }
            break;

#ifdef __WIN32__
        case WM_HELP:
            {
                wxRadioBox *radiobox = (wxRadioBox *)wxGetWindowUserData(hwnd);

                wxCHECK_MSG( radiobox, 0, wxT("radio button without radio box?") );

                bool processed wxDUMMY_INITIALIZE(true);

                // HELPINFO doesn't seem to be supported on WinCE.
#ifndef __WXWINCE__
                HELPINFO* info = (HELPINFO*) lParam;
                // Don't yet process menu help events, just windows
                if (info->iContextType == HELPINFO_WINDOW)
#endif
                {
                    wxWindow* subjectOfHelp = radiobox;
                    bool eventProcessed = false;
                    while (subjectOfHelp && !eventProcessed)
                    {
                        wxHelpEvent helpEvent(wxEVT_HELP, subjectOfHelp->GetId(),
#ifdef __WXWINCE__
                                              wxPoint(0, 0)
#else
                                              wxPoint(info->MousePos.x, info->MousePos.y)
#endif
                            ) ; // info->iCtrlId);
                        helpEvent.SetEventObject(radiobox);
                        eventProcessed = radiobox->GetEventHandler()->ProcessEvent(helpEvent);

                        // Go up the window hierarchy until the event is handled (or not)
                        subjectOfHelp = subjectOfHelp->GetParent();
                    }
                    processed = eventProcessed;
                }
#ifndef __WXWINCE__
                else if (info->iContextType == HELPINFO_MENUITEM)
                {
                    wxHelpEvent helpEvent(wxEVT_HELP, info->iCtrlId) ;
                    helpEvent.SetEventObject(radiobox);
                    processed = radiobox->GetEventHandler()->ProcessEvent(helpEvent);
                }
                else
                    processed = false;
#endif
                if (processed)
                    return 0;

                break;
            }
#endif // __WIN32__
    }

    return ::CallWindowProc(CASTWNDPROC s_wndprocRadioBtn, hwnd, message, wParam, lParam);
}

#endif // __WIN32__

#endif // wxUSE_RADIOBOX
