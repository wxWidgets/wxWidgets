/////////////////////////////////////////////////////////////////////////////
// Name:        slidermsw.cpp
// Purpose:     wxSliderMSW
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "slidrmsw.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#endif

#include "wx/utils.h"
#include "wx/brush.h"
#include "wx/msw/slidrmsw.h"
#include "wx/msw/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxSliderMSW, wxControl)

// Slider
wxSliderMSW::wxSliderMSW()
{
  m_staticValue = 0;
  m_staticMin = 0;
  m_staticMax = 0;
  m_pageSize = 1;
  m_lineSize = 1;
  m_rangeMax = 0;
  m_rangeMin = 0;
}

bool wxSliderMSW::Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    if ( (style & wxBORDER_MASK) == wxBORDER_DEFAULT )
        style |= wxBORDER_NONE;

  SetName(name);
#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif // wxUSE_VALIDATORS

  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  m_staticValue = 0;
  m_staticMin = 0;
  m_staticMax = 0;
  m_pageSize = 1;
  m_lineSize = 1;
  m_windowStyle = style;

  if ( id == wxID_ANY )
      m_windowId = (int)NewControlId();
  else
    m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  // non-Win95 implementation

  long msStyle = SS_CENTER;

  WXDWORD exStyle = 0;
  msStyle |= MSWGetStyle(GetWindowStyle(), & exStyle) ;

  m_staticValue = (WXHWND) CreateWindowEx(exStyle, wxT("STATIC"), NULL,
                           msStyle,
                           0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                           wxGetInstance(), NULL);

  // Now create min static control
  wxString buf;
  buf.Printf(wxT("%d"), minValue);
  DWORD wstyle = STATIC_FLAGS;
  if ( m_windowStyle & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;
  m_staticMin = (WXHWND) CreateWindowEx(0, wxT("STATIC"), buf,
                         wstyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                         wxGetInstance(), NULL);

  msStyle = 0;
  if (m_windowStyle & wxSL_VERTICAL)
    msStyle = SBS_VERT | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;
  else
    msStyle = SBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;

  HWND scroll_bar = CreateWindowEx(exStyle, wxT("SCROLLBAR"), wxEmptyString,
                         msStyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                         wxGetInstance(), NULL);

  m_pageSize = (int)((maxValue-minValue)/10);
  m_rangeMax = maxValue;
  m_rangeMin = minValue;

  ::SetScrollRange(scroll_bar, SB_CTL, minValue, maxValue, FALSE);
  ::SetScrollPos(scroll_bar, SB_CTL, value, FALSE);
  ShowWindow(scroll_bar, SW_SHOW);

  m_hWnd = (WXHWND)scroll_bar;

  // Subclass again for purposes of dialog editing mode
  SubclassWin(GetHWND());

  // Finally, create max value static item
  buf.Printf(wxT("%d"), maxValue);
  wstyle = STATIC_FLAGS;
  if ( m_windowStyle & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;
  m_staticMax = (WXHWND) CreateWindowEx(0, wxT("STATIC"), buf,
                         wstyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                         wxGetInstance(), NULL);

  SetFont(parent->GetFont());

  if (GetFont().Ok())
  {
//    GetFont()->RealizeResource();
    if (GetFont().GetResourceHandle())
    {
        if ( m_staticMin )
              SendMessage((HWND)m_staticMin,WM_SETFONT,
                      (WPARAM)GetFont().GetResourceHandle(),0L);
        if ( m_staticMax )
              SendMessage((HWND)m_staticMax,WM_SETFONT,
                  (WPARAM)GetFont().GetResourceHandle(),0L);
          if (m_staticValue)
            SendMessage((HWND)m_staticValue,WM_SETFONT,
                    (WPARAM)GetFont().GetResourceHandle(),0L);
    }
  }

  SetSize(x, y, width, height);
  SetValue(value);

  return true;
}

bool wxSliderMSW::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    int position = ::GetScrollPos((HWND)control, SB_CTL);

    int nScrollInc;
    wxEventType scrollEvent = wxEVT_NULL;
    switch ( wParam )
    {
        case SB_TOP:
            nScrollInc = m_rangeMax - position;
            scrollEvent = wxEVT_SCROLL_TOP;
            break;

        case SB_BOTTOM:
            nScrollInc = - position;
            scrollEvent = wxEVT_SCROLL_BOTTOM;
            break;

        case SB_LINEUP:
            nScrollInc = - GetLineSize();
            scrollEvent = wxEVT_SCROLL_LINEUP;
            break;

        case SB_LINEDOWN:
            nScrollInc = GetLineSize();
            scrollEvent = wxEVT_SCROLL_LINEDOWN;
            break;

        case SB_PAGEUP:
            nScrollInc = -GetPageSize();
            scrollEvent = wxEVT_SCROLL_PAGEUP;
            break;

        case SB_PAGEDOWN:
            nScrollInc = GetPageSize();
            scrollEvent = wxEVT_SCROLL_PAGEDOWN;
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
#ifdef __WIN32__
            nScrollInc = (signed short)pos - position;
#else
            nScrollInc = pos - position;
#endif
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;

        default:
            nScrollInc = 0;
    }

    if (nScrollInc == 0)
    {
        // no event...
        return false;
    }

    int newPos = position + nScrollInc;

    if ( (newPos < GetMin()) || (newPos > GetMax()) )
    {
        // out of range - but we did process it
        return true;
    }

    SetValue(newPos);

    wxScrollEvent event(scrollEvent, m_windowId);
    event.SetPosition(newPos);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);

    wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, GetId() );
    cevent.SetInt( newPos );
    cevent.SetEventObject( this );

    return GetEventHandler()->ProcessEvent( cevent );
}

wxSliderMSW::~wxSliderMSW()
{
    if (m_staticMin)
      DestroyWindow((HWND) m_staticMin);
    if (m_staticMax)
      DestroyWindow((HWND) m_staticMax);
    if (m_staticValue)
      DestroyWindow((HWND) m_staticValue);
}

int wxSliderMSW::GetValue() const
{
  return ::GetScrollPos(GetHwnd(), SB_CTL);
}

void wxSliderMSW::SetValue(int value)
{
  ::SetScrollPos(GetHwnd(), SB_CTL, value, TRUE);
  if (m_staticValue)
  {
    wxString buf;
    buf.Printf(wxT("%d"), value);
    SetWindowText((HWND) m_staticValue, buf);
  }
}

void wxSliderMSW::GetSize(int *width, int *height) const
{
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize(GetHWND(), &rect);

  if (m_staticMin)
    wxFindMaxSize(m_staticMin, &rect);
  if (m_staticMax)
    wxFindMaxSize(m_staticMax, &rect);
  if (m_staticValue)
    wxFindMaxSize(m_staticValue, &rect);

  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxSliderMSW::GetPosition(int *x, int *y) const
{
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize(GetHWND(), &rect);

  if (m_staticMin)
    wxFindMaxSize(m_staticMin, &rect);
  if (m_staticMax)
    wxFindMaxSize(m_staticMax, &rect);
  if (m_staticValue)
    wxFindMaxSize(m_staticValue, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
    ::ScreenToClient((HWND) parent->GetHWND(), &point);

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

// TODO one day, make sense of all this horros and replace it with a readable
//      DoGetBestSize()
void wxSliderMSW::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  AdjustForParentClientOrigin(x1, y1, sizeFlags);

  wxChar buf[300];

  int x_offset = x;
  int y_offset = y;

  int cx;     // slider,min,max sizes
  int cy;
  int cyf;

  wxGetCharSize(GetHWND(), &cx, &cy, this->GetFont());

  if ((m_windowStyle & wxSL_VERTICAL) != wxSL_VERTICAL)
  {
    if ( m_windowStyle & wxSL_LABELS )
    {
    int min_len = 0;

    GetWindowText((HWND) m_staticMin, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL, & this->GetFont());

    int max_len = 0;

    GetWindowText((HWND) m_staticMax, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, & this->GetFont());
    if (m_staticValue)
    {
      int new_width = (int)(wxMax(min_len, max_len));
      int valueHeight = (int)cyf;
#ifdef __WIN32__
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
      ::MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      x_offset += new_width + cx;
    }

    ::MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    x_offset += (int)(min_len + cx);

    int slider_length = (int)(w1 - x_offset - max_len - cx);

    int slider_height = cy;

    // Slider must have a minimum/default length/height
    if (slider_length < 100)
      slider_length = 100;

    ::MoveWindow(GetHwnd(), x_offset, y_offset, slider_length, slider_height, TRUE);
    x_offset += slider_length + cx;

    ::MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
    else
    {
        // No labels
        if ( w1 < 0 )
            w1 = 200;
        if ( h1 < 0 )
            h1 = 20;
        ::MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
    }
  }
  else
  {
    if ( m_windowStyle & wxSL_LABELS )
    {
    int min_len;
    GetWindowText((HWND) m_staticMin, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL,& this->GetFont());

    int max_len;
    GetWindowText((HWND) m_staticMax, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, & this->GetFont());

    if (m_staticValue)
    {
      int new_width = (int)(wxMax(min_len, max_len));
      int valueHeight = (int)cyf;
/*** Suggested change by George Tasker - remove this block...
#ifdef __WIN32__
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
 ... and replace with following line: */
      new_width += cx;

      ::MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      y_offset += valueHeight;
    }

    ::MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    y_offset += cy;

    int slider_length = (int)(h1 - y_offset - cy - cy);

    // Use character height as an estimate of slider width (yes, width)
    int slider_width = cy;

    // Slider must have a minimum/default length
    if (slider_length < 100)
      slider_length = 100;

    ::MoveWindow(GetHwnd(), x_offset, y_offset, slider_width, slider_length, TRUE);
    y_offset += slider_length;

    ::MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
    else
    {
        // No labels
        if ( w1 < 0 )
            w1 = 20;
        if ( h1 < 0 )
            h1 = 200;
        ::MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
    }
  }
}

void wxSliderMSW::SetRange(int minValue, int maxValue)
{
  m_rangeMin = minValue;
  m_rangeMax = maxValue;

  ::SetScrollRange(GetHwnd(), SB_CTL, m_rangeMin, m_rangeMax, TRUE);
  wxChar buf[40];
  if ( m_staticMin )
  {
      wxSprintf(buf, wxT("%d"), m_rangeMin);
      SetWindowText((HWND) m_staticMin, buf);
  }

  if ( m_staticMax )
  {
    wxSprintf(buf, wxT("%d"), m_rangeMax);
    SetWindowText((HWND) m_staticMax, buf);
  }
}

WXHBRUSH wxSliderMSW::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
  if ( nCtlColor == CTLCOLOR_SCROLLBAR )
    return 0;

  // Otherwise, it's a static
  return wxControl::OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
}

void wxSliderMSW::SetPageSize(int pageSize)
{
  m_pageSize = pageSize;
}

int wxSliderMSW::GetPageSize() const
{
  return m_pageSize;
}

void wxSliderMSW::SetLineSize(int lineSize)
{
  m_lineSize = lineSize;
}

int wxSliderMSW::GetLineSize() const
{
  return m_lineSize;
}

// Not yet implemented
void wxSliderMSW::SetThumbLength(int WXUNUSED(lenPixels))
{
}

// Not yet implemented
int wxSliderMSW::GetThumbLength() const
{
    return 0;
}

bool wxSliderMSW::ContainsHWND(WXHWND hWnd) const
{
    return ( hWnd == GetStaticMin() || hWnd == GetStaticMax() || hWnd == GetEditValue() );
}

void wxSliderMSW::Command (wxCommandEvent & event)
{
  SetValue (event.GetInt());
  ProcessCommand (event);
}

bool wxSliderMSW::Show(bool show)
{
    wxWindow::Show(show);

    int cshow;
    if (show)
        cshow = SW_SHOW;
    else
        cshow = SW_HIDE;

    if(m_staticValue)
        ShowWindow((HWND) m_staticValue, (BOOL)cshow);
    if(m_staticMin)
        ShowWindow((HWND) m_staticMin, (BOOL)cshow);
    if(m_staticMax)
        ShowWindow((HWND) m_staticMax, (BOOL)cshow);
    return true;
}


