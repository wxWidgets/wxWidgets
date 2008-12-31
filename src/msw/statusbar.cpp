///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statusbar.cpp
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STATUSBAR && wxUSE_NATIVE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/frame.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"
#include <windowsx.h>

#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// windowsx.h and commctrl.h don't define those, so we do it here
#define StatusBar_SetParts(h, n, w) SendMessage(h, SB_SETPARTS, (WPARAM)n, (LPARAM)w)
#define StatusBar_SetText(h, n, t)  SendMessage(h, SB_SETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)t)
#define StatusBar_GetTextLen(h, n)  LOWORD(SendMessage(h, SB_GETTEXTLENGTH, (WPARAM)n, 0))
#define StatusBar_GetText(h, n, s)  LOWORD(SendMessage(h, SB_GETTEXT, (WPARAM)n, (LPARAM)(LPTSTR)s))

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBar class
// ----------------------------------------------------------------------------

wxStatusBar::wxStatusBar()
{
    SetParent(NULL);
    m_hWnd = 0;
    m_windowId = 0;
}

bool wxStatusBar::Create(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( parent, false, wxT("status bar must have a parent") );

    SetName(name);
    SetWindowStyleFlag(style);
    SetParent(parent);

    parent->AddChild(this);

    m_windowId = id == wxID_ANY ? NewControlId() : id;

    DWORD wstyle = WS_CHILD | WS_VISIBLE;

    if ( style & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;

    // setting SBARS_SIZEGRIP is perfectly useless: it's always on by default
    // (at least in the version of comctl32.dll I'm using), and the only way to
    // turn it off is to use CCS_TOP style - as we position the status bar
    // manually anyhow (see DoMoveWindow), use CCS_TOP style if wxST_SIZEGRIP
    // is not given
    if ( !(style & wxST_SIZEGRIP) )
    {
        wstyle |= CCS_TOP;
    }
    else
    {
#ifndef __WXWINCE__
        // may be some versions of comctl32.dll do need it - anyhow, it won't
        // do any harm
        wstyle |= SBARS_SIZEGRIP;
#endif
    }

    m_hWnd = CreateWindow
             (
                STATUSCLASSNAME,
                _T(""),
                wstyle,
                0, 0, 0, 0,
                GetHwndOf(parent),
                (HMENU)wxUIntToPtr(m_windowId.GetValue()),
                wxGetInstance(),
                NULL
             );
    if ( m_hWnd == 0 )
    {
        wxLogSysError(_("Failed to create a status bar."));

        return false;
    }

    SetFieldsCount(1);
    SubclassWin(m_hWnd);
    InheritAttributes();

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    // we must refresh the frame size when the statusbar is created, because
    // its client area might change
    //
    // notice that we must post the event, not send it, as the frame doesn't
    // know that we're its status bar yet so laying it out right now wouldn't
    // work correctly, we need to wait until we return to the main loop
    PostSizeEventToParent();

    return true;
}

wxStatusBar::~wxStatusBar()
{
    // we must refresh the frame size when the statusbar is deleted but the
    // frame is not - otherwise statusbar leaves a hole in the place it used to
    // occupy
    PostSizeEventToParent();
}

void wxStatusBar::SetFieldsCount(int nFields, const int *widths)
{
    // this is a Windows limitation
    wxASSERT_MSG( (nFields > 0) && (nFields < 255), _T("too many fields") );

    wxStatusBarBase::SetFieldsCount(nFields, widths);

    SetFieldsWidth();
}

void wxStatusBar::SetStatusWidths(int n, const int widths[])
{
    wxStatusBarBase::SetStatusWidths(n, widths);

    SetFieldsWidth();
}

void wxStatusBar::SetFieldsWidth()
{
    if ( m_panes.IsEmpty() )
        return;

    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    int extraWidth = aBorders[2]; // space between fields

    wxArrayInt widthsAbs =
        CalculateAbsWidths(GetClientSize().x - extraWidth*(m_panes.GetCount() - 1));

    int *pWidths = new int[m_panes.GetCount()];

    int nCurPos = 0;
    for ( int i = 0; i < m_panes.GetCount(); i++ ) {
        nCurPos += widthsAbs[i] + extraWidth;
        pWidths[i] = nCurPos;
    }

    if ( !StatusBar_SetParts(GetHwnd(), m_panes.GetCount(), pWidths) ) {
        wxLogLastError(wxT("StatusBar_SetParts"));
    }

    delete [] pWidths;
}

void wxStatusBar::SetStatusText(const wxString& strText, int nField)
{
    wxCHECK_RET( (nField >= 0) && (nField < m_nFields),
                 _T("invalid statusbar field index") );

    if ( strText == GetStatusText(nField) )
    {
       // don't call StatusBar_SetText() to avoid flicker
       return;
    }

    // Get field style, if any
    int style;
    switch(m_panes[nField].nStyle)
    {
    case wxSB_RAISED:
        style = SBT_POPOUT;
        break;
    case wxSB_FLAT:
        style = SBT_NOBORDERS;
        break;

    case wxSB_NORMAL:
    default:
        style = 0;
        break;
    }

    // Pass both field number and style. MSDN library doesn't mention
    // that nField and style have to be 'ORed'
    if ( !StatusBar_SetText(GetHwnd(), nField | style, strText.wx_str()) )
    {
        wxLogLastError(wxT("StatusBar_SetText"));
    }
}

wxString wxStatusBar::GetStatusText(int nField) const
{
    wxCHECK_MSG( (nField >= 0) && (nField < m_panes.GetCount()), wxEmptyString,
                 _T("invalid statusbar field index") );

    wxString str;
    int len = StatusBar_GetTextLen(GetHwnd(), nField);
    if ( len > 0 )
    {
        StatusBar_GetText(GetHwnd(), nField, wxStringBuffer(str, len));
    }

    return str;
}

int wxStatusBar::GetBorderX() const
{
    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    return aBorders[0];
}

int wxStatusBar::GetBorderY() const
{
    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    return aBorders[1];
}

void wxStatusBar::SetMinHeight(int height)
{
    SendMessage(GetHwnd(), SB_SETMINHEIGHT, height + 2*GetBorderY(), 0);

    // have to send a (dummy) WM_SIZE to redraw it now
    SendMessage(GetHwnd(), WM_SIZE, 0, 0);
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    wxCHECK_MSG( (i >= 0) && (i < m_panes.GetCount()), false,
                 _T("invalid statusbar field index") );

    RECT r;
    if ( !::SendMessage(GetHwnd(), SB_GETRECT, i, (LPARAM)&r) )
    {
        wxLogLastError(wxT("SendMessage(SB_GETRECT)"));
    }

#if wxUSE_UXTHEME
    wxUxThemeHandle theme((wxStatusBar *)this, L"Status"); // const_cast
    if ( theme )
    {
        // by default Windows has a 2 pixel border to the right of the left
        // divider (or it could be a bug) but it looks wrong so remove it
        if ( i != 0 )
        {
            r.left -= 2;
        }

        wxUxThemeEngine::Get()->GetThemeBackgroundContentRect(theme, NULL,
                                                              1 /* SP_PANE */, 0,
                                                              &r, &r);
    }
#endif

    wxCopyRECTToRect(r, rect);

    return true;
}

// no idea for a default width, just choose something
#define DEFAULT_FIELD_WIDTH 25

wxSize wxStatusBar::DoGetBestSize() const
{
    int borders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)borders);

    // calculate width
    int width = 0;
    for ( int i = 0; i < m_panes.GetCount(); ++i )
    {
        int widthField =
            m_bSameWidthForAllPanes ? DEFAULT_FIELD_WIDTH : m_panes[i].nWidth;
        if ( widthField >= 0 )
        {
            width += widthField;
        }
        else
        {
            // variable width field, its width is really a proportion
            // related to the other fields
            width += -widthField*DEFAULT_FIELD_WIDTH;
        }

        // add the space between fields
        width += borders[2];
    }

    if ( !width )
    {
        // still need something even for an empty status bar
        width = 2*DEFAULT_FIELD_WIDTH;
    }


    // calculate height
    int height;
    wxGetCharSize(GetHWND(), NULL, &height, GetFont());
    height = EDIT_HEIGHT_FROM_CHAR_HEIGHT(height);
    height += borders[1];

    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

void wxStatusBar::DoMoveWindow(int x, int y, int width, int height)
{
    if ( GetParent()->IsSizeDeferred() )
    {
        wxWindowMSW::DoMoveWindow(x, y, width, height);
    }
    else
    {
        // parent pos/size isn't deferred so do it now but don't send
        // WM_WINDOWPOSCHANGING since we don't want to change pos/size later
        // we must use SWP_NOCOPYBITS here otherwise it paints incorrectly
        // if other windows are size deferred
        ::SetWindowPos(GetHwnd(), NULL, x, y, width, height,
                       SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
#ifndef __WXWINCE__
                       | SWP_NOCOPYBITS | SWP_NOSENDCHANGING
#endif
                       );
    }

    // adjust fields widths to the new size
    SetFieldsWidth();

    // we have to trigger wxSizeEvent if there are children window in status
    // bar because GetFieldRect returned incorrect (not updated) values up to
    // here, which almost certainly resulted in incorrectly redrawn statusbar
    if ( m_children.GetCount() > 0 )
    {
        wxSizeEvent event(GetClientSize(), m_windowId);
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

void wxStatusBar::SetStatusStyles(int n, const int styles[])
{
    wxStatusBarBase::SetStatusStyles(n, styles);

    if (n != m_panes.GetCount())
        return;

    for (int i = 0; i < n; i++)
    {
        int style;
        switch(styles[i])
        {
        case wxSB_RAISED:
            style = SBT_POPOUT;
            break;
        case wxSB_FLAT:
            style = SBT_NOBORDERS;
            break;
        case wxSB_NORMAL:
        default:
            style = 0;
            break;
        }
        // The SB_SETTEXT message is both used to set the field's text as well as
        // the fields' styles. MSDN library doesn't mention
        // that nField and style have to be 'ORed'
        wxString text = GetStatusText(i);
        if (!StatusBar_SetText(GetHwnd(), style | i, text.wx_str()))
        {
            wxLogLastError(wxT("StatusBar_SetText"));
        }
    }
}

WXLRESULT
wxStatusBar::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
#ifndef __WXWINCE__
    if ( nMsg == WM_WINDOWPOSCHANGING )
    {
        WINDOWPOS *lpPos = (WINDOWPOS *)lParam;
        int x, y, w, h;
        GetPosition(&x, &y);
        GetSize(&w, &h);

        // we need real window coords and not wx client coords
        AdjustForParentClientOrigin(x, y);

        lpPos->x  = x;
        lpPos->y  = y;
        lpPos->cx = w;
        lpPos->cy = h;

        return 0;
    }

    if ( nMsg == WM_NCLBUTTONDOWN )
    {
        // if hit-test is on gripper then send message to TLW to begin
        // resizing. It is possible to send this message to any window.
        if ( wParam == HTBOTTOMRIGHT )
        {
            wxWindow *win;

            for ( win = GetParent(); win; win = win->GetParent() )
            {
                if ( win->IsTopLevel() )
                {
                    SendMessage(GetHwndOf(win), WM_NCLBUTTONDOWN,
                                wParam, lParam);

                    return 0;
                }
            }
        }
    }
#endif

    return wxStatusBarBase::MSWWindowProc(nMsg, wParam, lParam);
}

#endif // wxUSE_STATUSBAR && wxUSE_NATIVE_STATUSBAR
