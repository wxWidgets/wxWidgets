/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/gauge95.cpp
// Purpose:     wxGauge95 class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// old commctrl.h (< 4.71) don't have those
#ifndef PBS_SMOOTH
    #define PBS_SMOOTH 0x01
#endif

#ifndef PBS_VERTICAL
    #define PBS_VERTICAL 0x04
#endif

#ifndef PBM_SETBARCOLOR
    #define PBM_SETBARCOLOR         (WM_USER+9)
#endif

#ifndef PBM_SETBKCOLOR
    #define PBM_SETBKCOLOR          0x2001
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxGaugeStyle )

WX_BEGIN_FLAGS( wxGaugeStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    WX_FLAGS_MEMBER(wxBORDER_SIMPLE)
    WX_FLAGS_MEMBER(wxBORDER_SUNKEN)
    WX_FLAGS_MEMBER(wxBORDER_DOUBLE)
    WX_FLAGS_MEMBER(wxBORDER_RAISED)
    WX_FLAGS_MEMBER(wxBORDER_STATIC)
    WX_FLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    WX_FLAGS_MEMBER(wxSIMPLE_BORDER)
    WX_FLAGS_MEMBER(wxSUNKEN_BORDER)
    WX_FLAGS_MEMBER(wxDOUBLE_BORDER)
    WX_FLAGS_MEMBER(wxRAISED_BORDER)
    WX_FLAGS_MEMBER(wxSTATIC_BORDER)
    WX_FLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    WX_FLAGS_MEMBER(wxTAB_TRAVERSAL)
    WX_FLAGS_MEMBER(wxCLIP_CHILDREN)
    WX_FLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    WX_FLAGS_MEMBER(wxWANTS_CHARS)
    WX_FLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    WX_FLAGS_MEMBER(wxALWAYS_SHOW_SB )
    WX_FLAGS_MEMBER(wxVSCROLL)
    WX_FLAGS_MEMBER(wxHSCROLL)

    WX_FLAGS_MEMBER(wxGA_HORIZONTAL)
    WX_FLAGS_MEMBER(wxGA_VERTICAL)
    WX_FLAGS_MEMBER(wxGA_PROGRESSBAR)
    WX_FLAGS_MEMBER(wxGA_SMOOTH)

WX_END_FLAGS( wxGaugeStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxGauge, wxControl,"wx/gauge.h")

WX_BEGIN_PROPERTIES_TABLE(wxGauge95)
	WX_PROPERTY( Value , int , SetValue, GetValue, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
	WX_PROPERTY( Range , int , SetRange, GetRange, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
	WX_PROPERTY( ShadowWidth , int , SetShadowWidth, GetShadowWidth, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
	WX_PROPERTY( BezelFace , int , SetBezelFace, GetBezelFace, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_PROPERTY_FLAGS( WindowStyle , wxGaugeStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxGauge95)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_6( wxGauge95 , wxWindow* , Parent , wxWindowID , Id , int , Range , wxPoint , Position , wxSize , Size , long , WindowStyle ) 
#else
IMPLEMENT_DYNAMIC_CLASS(wxGauge95, wxControl)
#endif

// ============================================================================
// implementation
// ============================================================================

bool wxGauge95::Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif // wxUSE_VALIDATORS

  if (parent) parent->AddChild(this);
  m_rangeMax = range;
  m_gaugePos = 0;

  m_windowStyle = style;

  if ( id == -1 )
      m_windowId = (int)NewControlId();
  else
      m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  WXDWORD exStyle = 0;
  long msFlags = MSWGetStyle(style, & exStyle) ;

  if (m_windowStyle & wxGA_VERTICAL)
    msFlags |= PBS_VERTICAL;

  if (m_windowStyle & wxGA_SMOOTH)
    msFlags |= PBS_SMOOTH;

  HWND wx_button =
      CreateWindowEx(exStyle, PROGRESS_CLASS, NULL, msFlags,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

  m_hWnd = (WXHWND)wx_button;

  SetBackgroundColour(parent->GetBackgroundColour());
  SetForegroundColour(parent->GetForegroundColour());

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

void wxGauge95::SetShadowWidth(int WXUNUSED(w))
{
}

void wxGauge95::SetBezelFace(int WXUNUSED(w))
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

int wxGauge95::GetShadowWidth() const
{
  return 0;
}

int wxGauge95::GetBezelFace() const
{
  return 0;
}

int wxGauge95::GetRange() const
{
  return m_rangeMax;
}

int wxGauge95::GetValue() const
{
  return m_gaugePos;
}

bool wxGauge95::SetForegroundColour(const wxColour& col)
{
    if ( !wxControl::SetForegroundColour(col) )
        return FALSE;

    SendMessage(GetHwnd(), PBM_SETBARCOLOR, 0, (LPARAM)wxColourToRGB(col));

    return TRUE;
}

bool wxGauge95::SetBackgroundColour(const wxColour& col)
{
    if ( !wxControl::SetBackgroundColour(col) )
        return FALSE;

    SendMessage(GetHwnd(), PBM_SETBKCOLOR, 0, (LPARAM)wxColourToRGB(col));

    return TRUE;
}

#endif // wxUSE_GAUGE
