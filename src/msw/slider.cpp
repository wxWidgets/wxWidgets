/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/slider.h"
#endif

#include "wx/msw/private.h"

// Can opt to not use track bar under Win95 if you prefer it - set to 0
#define USE_TRACK_BAR 1

#if defined(__WIN95__) && !defined(__GNUWIN32__)
#include <commctrl.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

#if WXWIN_COMPATIBILITY
BEGIN_EVENT_TABLE(wxSlider, wxControl)
  EVT_SCROLL(wxSlider::OnScroll)
END_EVENT_TABLE()
#endif

#endif

// Slider
wxSlider::wxSlider(void)
{
  m_staticValue = 0;
  m_staticMin = 0;
  m_staticMax = 0;
  m_pageSize = 1;
  m_lineSize = 1;
  m_rangeMax = 0;
  m_rangeMin = 0;
  m_tickFreq = 0;
}

bool wxSlider::Create(wxWindow *parent, const wxWindowID id,
           const int value, const int minValue, const int maxValue,
           const wxPoint& pos,
           const wxSize& size, const long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);

  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetDefaultBackgroundColour()) ;
  SetForegroundColour(parent->GetDefaultForegroundColour()) ;

  m_staticValue = 0;
  m_staticMin = 0;
  m_staticMax = 0;
  m_pageSize = 1;
  m_lineSize = 1;
  m_windowStyle = style;
  m_tickFreq = 0;

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

#if defined(__WIN95__) && USE_TRACK_BAR
  long msStyle ;

  if ( m_windowStyle & wxSL_LABELS )
  {
      msStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER;

      bool want3D;
      WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

      m_staticValue = (WXHWND) CreateWindowEx(exStyle, "STATIC", NULL,
                               msStyle,
                               0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                               wxGetInstance(), NULL);

      // Now create min static control
      sprintf(wxBuffer, "%d", minValue);
      m_staticMin = (WXHWND) CreateWindowEx(0, "STATIC", wxBuffer,
                             STATIC_FLAGS,
                             0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                             wxGetInstance(), NULL);
  }

  msStyle = 0;
  if (m_windowStyle & wxSL_VERTICAL)
    msStyle = TBS_VERT | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;
  else
    msStyle = TBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;

  if ( m_windowStyle & wxSL_AUTOTICKS )
	msStyle |= TBS_AUTOTICKS ;

  if ( m_windowStyle & wxSL_LEFT )
	msStyle |= TBS_LEFT;
  else if ( m_windowStyle & wxSL_RIGHT )
  	msStyle |= TBS_RIGHT;
  else if ( m_windowStyle & wxSL_TOP )
	msStyle |= TBS_TOP;
  else if ( m_windowStyle & wxSL_BOTTOM )
	msStyle |= TBS_BOTTOM;
  else if ( m_windowStyle & wxSL_BOTH )
	msStyle |= TBS_BOTH;
  else if ( ! (m_windowStyle & wxSL_AUTOTICKS) )
	msStyle |= TBS_NOTICKS;

  if ( m_windowStyle & wxSL_SELRANGE )
	msStyle |= TBS_ENABLESELRANGE ;

  HWND scroll_bar = CreateWindowEx(MakeExtendedStyle(m_windowStyle), TRACKBAR_CLASS, wxBuffer,
                         msStyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                         wxGetInstance(), NULL);

  m_rangeMax = maxValue;
  m_rangeMin = minValue;

  m_pageSize = (int)((maxValue-minValue)/10);

  ::SendMessage(scroll_bar, TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
  ::SendMessage(scroll_bar, TBM_SETPOS, TRUE, (LPARAM)value);
  ::SendMessage(scroll_bar, TBM_SETPAGESIZE, 0, (LPARAM)m_pageSize);

  m_hWnd = (WXHWND)scroll_bar;

  SubclassWin(GetHWND());

  SetFont(parent->GetFont());

  if ( m_windowStyle & wxSL_LABELS )
  {
      // Finally, create max value static item
      sprintf(wxBuffer, "%d", maxValue);
      m_staticMax = (WXHWND) CreateWindowEx(0, "STATIC", wxBuffer,
                             STATIC_FLAGS,
                             0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                             wxGetInstance(), NULL);

      if (GetFont())
      {
//        GetFont()->RealizeResource();
        if (GetFont()->GetResourceHandle())
        {
		    if ( m_staticMin )
      		    SendMessage((HWND)m_staticMin,WM_SETFONT,
                  	    (WPARAM)GetFont()->GetResourceHandle(),0L);
		    if ( m_staticMax )
      		    SendMessage((HWND)m_staticMax,WM_SETFONT,
                      (WPARAM)GetFont()->GetResourceHandle(),0L);
      	    if (m_staticValue)
        	    SendMessage((HWND)m_staticValue,WM_SETFONT,
                        (WPARAM)GetFont()->GetResourceHandle(),0L);
        }
      }
  }
#else
  // non-Win95 implementation
  
  long msStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  m_staticValue = (WXHWND) CreateWindowEx(exStyle, "STATIC", NULL,
                           msStyle,
                           0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                           wxGetInstance(), NULL);

  // Now create min static control
  sprintf(wxBuffer, "%d", minValue);
  m_staticMin = (WXHWND) CreateWindowEx(0, "STATIC", wxBuffer,
                         STATIC_FLAGS,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                         wxGetInstance(), NULL);

  // Now create slider
  m_windowId = (int)NewControlId();

  msStyle = 0;
  if (m_windowStyle & wxSL_VERTICAL)
    msStyle = SBS_VERT | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;
  else
    msStyle = SBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;

  HWND scroll_bar = CreateWindowEx(MakeExtendedStyle(m_windowStyle), "SCROLLBAR", wxBuffer,
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
  sprintf(wxBuffer, "%d", maxValue);
  m_staticMax = (WXHWND) CreateWindowEx(0, "STATIC", wxBuffer,
                         STATIC_FLAGS,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                         wxGetInstance(), NULL);

  SetFont(* parent->GetFont());

  if (GetFont())
  {
//    GetFont()->RealizeResource();
    if (GetFont()->GetResourceHandle())
    {
		if ( m_staticMin )
      		SendMessage((HWND)m_staticMin,WM_SETFONT,
                  	(WPARAM)GetFont()->GetResourceHandle(),0L);
		if ( m_staticMax )
      		SendMessage((HWND)m_staticMax,WM_SETFONT,
                  (WPARAM)GetFont()->GetResourceHandle(),0L);
      	if (m_staticValue)
        	SendMessage((HWND)m_staticValue,WM_SETFONT,
                    (WPARAM)GetFont()->GetResourceHandle(),0L);
    }
  }
#endif

  SetSize(x, y, width, height);
  SetValue(value);

  return TRUE;
}

void wxSlider::MSWOnVScroll(const WXWORD wParam, const WXWORD pos, const WXHWND control)
{
#if defined(__WIN95__) && USE_TRACK_BAR
    int position = 0; // Dummy - not used in this mode
#else
    int position = ::GetScrollPos((HWND)control, SB_CTL);
#endif

    int nScrollInc;
    int scrollEvent = 0;
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
                    return;
    }

#if !(WIN95 && USE_TRACK_BAR)
    if (nScrollInc != 0)
#endif
    {

#if defined(__WIN95__) && USE_TRACK_BAR
      int newPos = (int)::SendMessage((HWND) control, TBM_GETPOS, 0, 0);
#else
      int newPos = position + nScrollInc;
#endif
      if (!(newPos < GetMin() || newPos > GetMax()))
      {
        SetValue(newPos);

        wxScrollEvent event(scrollEvent, m_windowId);
        event.SetPosition(newPos);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
      }
    }
}

void wxSlider::MSWOnHScroll(const WXWORD wParam, const WXWORD pos, const WXHWND control)
{
	MSWOnVScroll(wParam, pos, control);
}

wxSlider::~wxSlider(void)
{
    if (m_staticMin)
      DestroyWindow((HWND) m_staticMin);
    if (m_staticMax)
      DestroyWindow((HWND) m_staticMax);
    if (m_staticValue)
      DestroyWindow((HWND) m_staticValue);
}

int wxSlider::GetValue(void) const
{
#if defined(__WIN95__) && USE_TRACK_BAR
  return ::SendMessage((HWND) GetHWND(), TBM_GETPOS, 0, 0);
#else
  return ::GetScrollPos((HWND) GetHWND(), SB_CTL);
#endif
}

void wxSlider::SetValue(const int value)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage((HWND) GetHWND(), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);
#else
  ::SetScrollPos((HWND) GetHWND(), SB_CTL, value, TRUE);
#endif
  if (m_staticValue)
  {
    sprintf(wxBuffer, "%d", value);
    SetWindowText((HWND) m_staticValue, wxBuffer);
  }
}

void wxSlider::GetSize(int *width, int *height) const
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

void wxSlider::GetPosition(int *x, int *y) const
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

  *x = point.x;
  *y = point.y;
}

void wxSlider::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
{
  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  char buf[300];

  int x_offset = x;
  int y_offset = y;

  int cx;     // slider,min,max sizes
  int cy;
  float cyf;

  wxGetCharSize(GetHWND(), &cx, &cy,GetFont());

  if ((m_windowStyle & wxSL_VERTICAL) != wxSL_VERTICAL)
  {
	if ( m_windowStyle & wxSL_LABELS )
	{
    float min_len = 0.0;

    GetWindowText((HWND) m_staticMin, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL, GetFont());

    float max_len = 0.0;

    GetWindowText((HWND) m_staticMax, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, GetFont());
    if (m_staticValue)
    {
      int new_width = (int)(wxMax(min_len, max_len));
	  int valueHeight = (int)cyf;
#ifdef __WIN32__
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
#if defined(__WIN95__)
      // The height needs to be a bit bigger under Win95 if using native
	  // 3D effects.
	  valueHeight = (int) (valueHeight * 1.5) ;
#endif
      MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      x_offset += new_width + cx;
    }

    MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    x_offset += (int)(min_len + cx);

    int slider_length = (int)(w1 - x_offset - max_len - cx);

#if defined(__WIN95__) && USE_TRACK_BAR
    int slider_height = h1;
	if (slider_height < 0 )
		slider_height = 20;
#else
    int slider_height = cy;
#endif

    // Slider must have a minimum/default length/height
    if (slider_length < 100)
      slider_length = 100;

    MoveWindow((HWND) GetHWND(), x_offset, y_offset, slider_length, slider_height, TRUE);
    x_offset += slider_length + cx;

    MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
	else
	{
		// No labels
		if ( w1 < 0 )
			w1 = 200;
		if ( h1 < 0 )
			h1 = 20;
    	MoveWindow((HWND) GetHWND(), x1, y1, w1, h1, TRUE);
	}
  }
  else
  {
	if ( m_windowStyle & wxSL_LABELS )
	{
    float min_len;
    GetWindowText((HWND) m_staticMin, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL,GetFont());

    float max_len;
    GetWindowText((HWND) m_staticMax, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, GetFont());

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

#if defined(__WIN95__)
      // The height needs to be a bit bigger under Win95 if using native
	  // 3D effects.
	  valueHeight = (int) (valueHeight * 1.5) ;
#endif

      MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      y_offset += valueHeight;
    }

    MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    y_offset += cy;

    int slider_length = (int)(h1 - y_offset - cy - cy);
#if defined(__WIN95__) && USE_TRACK_BAR
    int slider_width = w1;
	if (slider_width < 0 )
		slider_width = 20;
#else
    // Use character height as an estimate of slider width (yes, width)
    int slider_width = cy;
#endif

    // Slider must have a minimum/default length
    if (slider_length < 100)
      slider_length = 100;

    MoveWindow((HWND) GetHWND(), x_offset, y_offset, slider_width, slider_length, TRUE);
    y_offset += slider_length;

    MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
	else
	{
		// No labels
		if ( w1 < 0 )
			w1 = 20;
		if ( h1 < 0 )
			h1 = 200;
    	MoveWindow((HWND) GetHWND(), x1, y1, w1, h1, TRUE);
	}
  }

/*
#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(width, height);
#else
  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
*/

}

void wxSlider::SetRange(const int minValue, const int maxValue)
{
  m_rangeMin = minValue;
  m_rangeMax = maxValue;

#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage((HWND) GetHWND(), TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
#else
  ::SetScrollRange((HWND) GetHWND(), SB_CTL, m_rangeMin, m_rangeMax, TRUE);
#endif
  char buf[40];
  if ( m_staticMin )
  {
  	sprintf(buf, "%d", m_rangeMin);
  	SetWindowText((HWND) m_staticMin, buf);
  }

  if ( m_staticMax )
  {
    sprintf(buf, "%d", m_rangeMax);
    SetWindowText((HWND) m_staticMax, buf);
  }
}

WXHBRUSH wxSlider::OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
  if ( nCtlColor == CTLCOLOR_SCROLLBAR )
	return 0;

  // Otherwise, it's a static
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

// For trackbars only
void wxSlider::SetTickFreq(const int n, const int pos)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  m_tickFreq = n;
  ::SendMessage( (HWND) GetHWND(), TBM_SETTICFREQ, (WPARAM) n, (LPARAM) pos );
#endif
}

void wxSlider::SetPageSize(const int pageSize)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) pageSize );
#endif
  m_pageSize = pageSize;
}

int wxSlider::GetPageSize(void) const
{
  return m_pageSize;
}

void wxSlider::ClearSel(void)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_CLEARSEL, (WPARAM) TRUE, (LPARAM) 0 );
#endif
}

void wxSlider::ClearTicks(void)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0 );
#endif
}

void wxSlider::SetLineSize(const int lineSize)
{
  m_lineSize = lineSize;
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_SETLINESIZE, (WPARAM) 0, (LPARAM) lineSize );
#endif
}

int wxSlider::GetLineSize(void) const
{
#if defined(__WIN95__) && USE_TRACK_BAR
  return (int) ::SendMessage( (HWND) GetHWND(), TBM_GETLINESIZE, (WPARAM) 0, (LPARAM) 0 );
#else
  return m_lineSize;
#endif
}

int wxSlider::GetSelEnd(void) const
{
#if defined(__WIN95__) && USE_TRACK_BAR
  return (int) ::SendMessage( (HWND) GetHWND(), TBM_SETSELEND, (WPARAM) 0, (LPARAM) 0 );
#else
  return 0;
#endif
}

int wxSlider::GetSelStart(void) const
{
#if defined(__WIN95__) && USE_TRACK_BAR
  return (int) ::SendMessage( (HWND) GetHWND(), TBM_GETSELSTART, (WPARAM) 0, (LPARAM) 0 );
#else
  return 0;
#endif
}

void wxSlider::SetSelection(const int minPos, const int maxPos)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG( minPos, maxPos) );
#endif
}

void wxSlider::SetThumbLength(const int len)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_SETTHUMBLENGTH, (WPARAM) len, (LPARAM) 0 );
#endif
}

int wxSlider::GetThumbLength(void) const
{
#if defined(__WIN95__) && USE_TRACK_BAR
  return (int) ::SendMessage( (HWND) GetHWND(), TBM_GETTHUMBLENGTH, (WPARAM) 0, (LPARAM) 0 );
#else
  return 0;
#endif
}

void wxSlider::SetTick(const int tickPos)
{
#if defined(__WIN95__) && USE_TRACK_BAR
  ::SendMessage( (HWND) GetHWND(), TBM_SETTIC, (WPARAM) 0, (LPARAM) tickPos );
#endif
}

bool wxSlider::ContainsHWND(WXHWND hWnd) const
{
	return ( hWnd == GetStaticMin() || hWnd == GetStaticMax() || hWnd == GetEditValue() );
}

#if WXWIN_COMPATIBILITY
// Backward compatibility
void wxSlider::OnScroll(wxScrollEvent& event)
{
    int oldEvent = event.GetEventType();
    event.SetEventType( wxEVT_COMMAND_SLIDER_UPDATED );
    if ( !GetEventHandler()->ProcessEvent(event) )
    {
        event.SetEventType( oldEvent );
        if (!GetParent()->GetEventHandler()->ProcessEvent(event))
            event.Skip();
    }
}
#endif

void wxSlider::Command (wxCommandEvent & event)
{
  SetValue (event.GetInt());
  ProcessCommand (event);
}

bool wxSlider::Show(const bool show)
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
    return TRUE;
}


