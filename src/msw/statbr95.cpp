///////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbr95.cpp
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbr95.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/setup.h"
  #include "wx/frame.h"
  #include "wx/settings.h"
  #include "wx/dcclient.h"
#endif

#if defined(__WIN95__) && wxUSE_NATIVE_STATUSBAR

#include "wx/intl.h"
#include "wx/log.h"
#include "wx/statusbr.h"

#include "wx/msw/private.h"
#include <windowsx.h>

#if !defined(__GNUWIN32__) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStatusBar95, wxWindow);
IMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxStatusBar95)

BEGIN_EVENT_TABLE(wxStatusBar95, wxWindow)
    EVT_SIZE(wxStatusBar95::OnSize)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// windowsx.h and commctrl.h don't define those, so we do it here
#define StatusBar_SetParts(h, n, w) SendMessage(h, SB_SETPARTS, (WPARAM)n, (LPARAM)w)
#define StatusBar_SetText(h, n, t)  SendMessage(h, SB_SETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)t)
#define StatusBar_GetTextLen(h, n)  LOWORD(SendMessage(h, SB_GETTEXTLENGTH, (WPARAM)n, 0))
#define StatusBar_GetText(h, n, s)  LOWORD(SendMessage(h, SB_GETTEXT, (WPARAM)n, (LPARAM)(LPTSTR)s))

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static WNDPROC gs_wndprocStatBar = NULL;

LRESULT APIENTRY wxStatusBarProc(HWND hwnd,
                                 UINT message,
                                 WPARAM wParam,
                                 LPARAM lParam)
{
    if ( message == WM_COMMAND )
    {
        wxStatusBar95 *sb = (wxStatusBar95 *)GetWindowLong(hwnd, GWL_USERDATA);
        sb->MSWWindowProc(message, wParam, lParam);
    }

    return ::CallWindowProc(gs_wndprocStatBar, hwnd, message, wParam, lParam);
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBar95 class
// ----------------------------------------------------------------------------

wxStatusBar95::wxStatusBar95()
{
  SetParent(NULL);
  m_hWnd = 0;
  m_windowId = 0;
}

bool wxStatusBar95::Create(wxWindow *parent,
                           wxWindowID id,
                           long style,
                           const wxString& name)
{
    SetName(name);
    SetParent(parent);

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    DWORD wstyle = WS_CHILD | WS_VISIBLE;
    if ( style & wxST_SIZEGRIP )
        wstyle |= SBARS_SIZEGRIP;

    m_hWnd = (WXHWND)CreateStatusWindow(wstyle,
                                        wxEmptyString,
                                        GetHwndOf(parent),
                                        m_windowId);
    if ( m_hWnd == 0 )
    {
        wxLogSysError(_("Failed to create a status bar."));

        return FALSE;
    }

    // for some reason, subclassing in the usual way doesn't work at all - many
    // strange things start happening (status bar is not positioned correctly,
    // all methods fail...)
    //  SubclassWin(m_hWnd);

    // but we want to process the messages from it still, so must subclass it
    gs_wndprocStatBar = (WNDPROC)GetWindowLong(GetHwnd(), GWL_WNDPROC);
    SetWindowLong(GetHwnd(), GWL_WNDPROC, (LONG)wxStatusBarProc);
    SetWindowLong(GetHwnd(), GWL_USERDATA, (LONG)this);

    return TRUE;
}

wxStatusBar95::~wxStatusBar95()
{
    delete [] m_statusWidths;
}

void wxStatusBar95::CopyFieldsWidth(const int widths[])
{
  if (widths && !m_statusWidths)
    m_statusWidths = new int[m_nFields];

  if ( widths != NULL ) {
    for ( int i = 0; i < m_nFields; i++ )
      m_statusWidths[i] = widths[i];
  }
  else {
    delete [] m_statusWidths;
    m_statusWidths = NULL;
  }
}

void wxStatusBar95::SetFieldsCount(int nFields, const int widths[])
{
  // this is Windows limitation
  wxASSERT_MSG( (nFields > 0) && (nFields < 255), _T("too many fields") );

  m_nFields = nFields;

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBar95::SetStatusWidths(int n, const int widths[])
{
  wxASSERT_MSG( n == m_nFields, _T("field number mismatch") );

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBar95::SetFieldsWidth()
{
    if ( !m_nFields )
        return;

    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    int extraWidth = aBorders[2]; // space between fields

    int *pWidths = new int[m_nFields];

    int nWindowWidth, y;
    GetClientSize(&nWindowWidth, &y);

    if ( m_statusWidths == NULL ) {
        // default: all fields have the same width
        int nWidth = nWindowWidth / m_nFields;
        for ( int i = 0; i < m_nFields; i++ )
            pWidths[i] = (i + 1) * nWidth;
    }
    else {
        // -1 doesn't mean the same thing for wxWindows and Win32, recalc
        int nTotalWidth = 0,
        nVarCount = 0,
        i;
        for ( i = 0; i < m_nFields; i++ ) {
            if ( m_statusWidths[i] == -1 )
                nVarCount++;
            else
                nTotalWidth += m_statusWidths[i] + extraWidth;
        }

        if ( nVarCount == 0 ) {
            wxFAIL_MSG( _T("at least one field must be of variable width") );

            nVarCount++;
        }

        int nVarWidth = (nWindowWidth - nTotalWidth) / nVarCount;

        // do fill the array
        int nCurPos = 0;
        for ( i = 0; i < m_nFields; i++ ) {
            if ( m_statusWidths[i] == -1 )
                nCurPos += nVarWidth;
            else
                nCurPos += m_statusWidths[i] + extraWidth;
            pWidths[i] = nCurPos;
        }
    }

    if ( !StatusBar_SetParts(GetHwnd(), m_nFields, pWidths) ) {
        wxLogLastError(wxT("StatusBar_SetParts"));
    }

    delete [] pWidths;
}

void wxStatusBar95::SetStatusText(const wxString& strText, int nField)
{
    wxCHECK_RET( (nField >= 0) && (nField < m_nFields),
                 _T("invalid statusbar field index") );

  if ( !StatusBar_SetText(GetHwnd(), nField, strText) ) {
    wxLogLastError(wxT("StatusBar_SetText"));
  }
}

wxString wxStatusBar95::GetStatusText(int nField) const
{
    wxCHECK_MSG( (nField >= 0) && (nField < m_nFields), wxEmptyString,
                 _T("invalid statusbar field index") );

  wxString str;
  int len = StatusBar_GetTextLen(GetHwnd(), nField);
  if (len > 0)
  {
    StatusBar_GetText(GetHwnd(), nField, str.GetWriteBuf(len));
    str.UngetWriteBuf();
  }
  return str;
}

int wxStatusBar95::GetBorderX() const
{
    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    return aBorders[0];
}

int wxStatusBar95::GetBorderY() const
{
    int aBorders[3];
    SendMessage(GetHwnd(), SB_GETBORDERS, 0, (LPARAM)aBorders);

    return aBorders[1];
}

void wxStatusBar95::SetMinHeight(int height)
{
    SendMessage(GetHwnd(), SB_SETMINHEIGHT, height + 2*GetBorderY(), 0);

    // have to send a (dummy) WM_SIZE to redraw it now
    SendMessage(GetHwnd(), WM_SIZE, 0, 0);
}

bool wxStatusBar95::GetFieldRect(int i, wxRect& rect) const
{
    wxCHECK_MSG( (i >= 0) && (i < m_nFields), FALSE,
                 _T("invalid statusbar field index") );

    RECT r;
    if ( !::SendMessage(GetHwnd(), SB_GETRECT, i, (LPARAM)&r) )
    {
        wxLogLastError("SendMessage(SB_GETRECT)");
    }

    wxCopyRECTToRect(r, rect);

    return TRUE;
}

void wxStatusBar95::OnSize(wxSizeEvent& event)
{
  FORWARD_WM_SIZE(GetHwnd(), SIZE_RESTORED,
                  event.GetSize().x, event.GetSize().y,
                  SendMessage);

  // adjust fields widths to the new size
  SetFieldsWidth();
}

#endif // __WIN95__ && wxUSE_NATIVE_STATUSBAR

