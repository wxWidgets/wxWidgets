/////////////////////////////////////////////////////////////////////////////
// Name:        gauge95.cpp
// Purpose:     wxGauge95 class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gauge95.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if wxUSE_GAUGE && defined(__WIN95__)

#include "wx/msw/gauge95.h"
#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(wxGauge95, wxControl)

bool wxGauge95::Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);

  if (parent) parent->AddChild(this);
  m_rangeMax = range;
  m_gaugePos = 0;

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  m_windowStyle = style;

  if ( id == -1 )
      m_windowId = (int)NewControlId();
  else
      m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msFlags = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

#ifndef PBS_VERTICAL
#define PBS_VERTICAL 0x04
#endif

  if (m_windowStyle & wxGA_VERTICAL)
    msFlags |= PBS_VERTICAL;

#ifndef PBS_SMOOTH
#define PBS_SMOOTH 0x01
#endif

  if (m_windowStyle & wxGA_SMOOTH)
    msFlags |= PBS_SMOOTH;

  HWND wx_button =
      CreateWindowEx(MakeExtendedStyle(m_windowStyle), PROGRESS_CLASS, NULL, msFlags,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND) wx_button);

  SendMessage((HWND) GetHWND(), PBM_SETRANGE, 0, MAKELPARAM(0, range));

  SetFont(parent->GetFont());

  if (width == -1)
    width = 50;
  if (height == -1)
    height = 28;
  SetSize(x, y, width, height);

  ShowWindow((HWND) GetHWND(), SW_SHOW);

  return TRUE;
}

void wxGauge95::SetShadowWidth(int w)
{
}

void wxGauge95::SetBezelFace(int w)
{
}

void wxGauge95::SetRange(int r)
{
  m_rangeMax = r;

  SendMessage((HWND) GetHWND(), PBM_SETRANGE, 0, MAKELPARAM(0, r));
}

void wxGauge95::SetValue(int pos)
{
  m_gaugePos = pos;

  SendMessage((HWND) GetHWND(), PBM_SETPOS, pos, 0);
}

int wxGauge95::GetShadowWidth(void) const
{
  return 0;
}

int wxGauge95::GetBezelFace(void) const
{
  return 0;
}

int wxGauge95::GetRange(void) const
{
  return m_rangeMax;
}

int wxGauge95::GetValue(void) const
{
  return m_gaugePos;
}

bool wxGauge95::SetForegroundColour(const wxColour& col)
{
    if ( !wxControl::SetForegroundColour(col) )
        return FALSE;

    m_foregroundColour = col ;

    return TRUE;
}

bool wxGauge95::SetBackgroundColour(const wxColour& col)
{
    if ( !wxControl::SetBackgroundColour(col) )
        return FALSE;

    m_backgroundColour = col ;

    return TRUE;
}

#endif // wxUSE_GAUGE
