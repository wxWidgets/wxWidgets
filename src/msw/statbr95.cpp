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

#ifdef __WIN95__

#include "wx/log.h"
#include "wx/generic/statusbr.h"
#include "wx/msw/statbr95.h"

#include "wx/msw/private.h"
#include <windowsx.h>

#if !defined(__GNUWIN32__) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

#if wxUSE_NATIVE_STATUSBAR

#if     !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxStatusBar95, wxStatusBar);

  BEGIN_EVENT_TABLE(wxStatusBar95, wxStatusBar)
    EVT_SIZE(wxStatusBar95::OnSize)
  END_EVENT_TABLE()
#endif  //USE_SHARED_LIBRARY


// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// windowsx.h and commctrl.h don't define those, so we do it here
#define StatusBar_SetParts(h, n, w) SendMessage(h, SB_SETPARTS, (WPARAM)n, (LPARAM)w)
#define StatusBar_SetText(h, n, t)  SendMessage(h, SB_SETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)t)
#define StatusBar_GetTextLen(h, n)  LOWORD(SendMessage(h, SB_GETTEXTLENGTH, (WPARAM)n, 0))
#define StatusBar_GetText(h, n, s)  LOWORD(SendMessage(h, SB_GETTEXT, (WPARAM)n, (LPARAM)(LPTSTR)s))

#define hwnd      ((HWND)m_hWnd)

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

wxStatusBar95::wxStatusBar95(wxWindow *parent, wxWindowID id, long style)
{
  Create(parent, id, style);
}

bool wxStatusBar95::Create(wxWindow *parent, wxWindowID id, long style)
{
  SetParent(parent);

  if (id == -1)
    m_windowId = NewControlId();
  else
    m_windowId = id;

  DWORD wstyle = WS_CHILD | WS_VISIBLE;
  if ( style & wxST_SIZEGRIP )
    wstyle |= SBARS_SIZEGRIP;

  m_hWnd = (WXHWND)CreateStatusWindow(wstyle,
                                      wxT(""),
                                      (HWND)parent->GetHWND(),
                                      m_windowId);
  if ( m_hWnd == 0 ) {
    wxLogSysError(wxT("can't create status bar window"));
    return FALSE;
  }

  // this doesn't work: display problems (white 1-2 pixel borders...)
  //  SubclassWin(m_hWnd);

  return TRUE;
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
  wxASSERT( (nFields > 0) && (nFields < 255) );

  m_nFields = nFields;

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBar95::SetStatusWidths(int n, const int widths[])
{
  // @@ I don't understand what this function is for...
  wxASSERT( n == m_nFields );

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBar95::SetFieldsWidth()
{
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
        nTotalWidth += m_statusWidths[i];
    }

    if ( nVarCount == 0 ) {
      // wrong! at least one field must be of variable width
      wxFAIL;

      nVarCount++;
    }

    int nVarWidth = (nWindowWidth - nTotalWidth) / nVarCount;

    // do fill the array
    int nCurPos = 0;
    for ( i = 0; i < m_nFields; i++ ) {
      if ( m_statusWidths[i] == -1 )
        nCurPos += nVarWidth;
      else
        nCurPos += m_statusWidths[i];
      pWidths[i] = nCurPos;
    }
  }

  if ( !StatusBar_SetParts(hwnd, m_nFields, pWidths) ) {
    wxLogLastError(wxT("StatusBar_SetParts"));
  }

  delete [] pWidths;
}

void wxStatusBar95::SetStatusText(const wxString& strText, int nField)
{
  if ( !StatusBar_SetText(hwnd, nField, strText) ) {
    wxLogLastError(wxT("StatusBar_SetText"));
  }
}

wxString wxStatusBar95::GetStatusText(int nField) const
{
  wxASSERT( (nField > -1) && (nField < m_nFields) );

  wxString str(wxT(""));
  int len = StatusBar_GetTextLen(hwnd, nField);
  if (len > 0)
  {
        StatusBar_GetText(hwnd, nField, str.GetWriteBuf(len));
        str.UngetWriteBuf();
  }
  return str;
}

void wxStatusBar95::OnSize(wxSizeEvent& event)
{
  FORWARD_WM_SIZE(hwnd, SIZE_RESTORED, event.GetSize().x, event.GetSize().y,
                  SendMessage);

  // adjust fields widths to the new size
  SetFieldsWidth();
}

#endif  // wxUSE_NATIVE_STATUSBAR

#else
    #error "wxStatusBar95 is only available under Windows 95 and later."
#endif // __WIN95__
  
