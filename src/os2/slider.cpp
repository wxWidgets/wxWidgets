/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include <wx/utils.h>
#include <wx/brush.h>
#endif

#include "wx/slider.h"
#include "wx/os2/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

// Slider
wxSlider::wxSlider()
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

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos,
           const wxSize& size, long style,
#if wxUSE_VALIDATORS
           const wxValidator& validator,
#endif
           const wxString& name)
{
    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    if (parent) parent->AddChild(this);

    m_lineSize = 1;
    m_windowStyle = style;
    m_tickFreq = 0;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_rangeMax = maxValue;
    m_rangeMin = minValue;

    m_pageSize = (int)((maxValue-minValue)/10);

    // TODO create slider

    return FALSE;
}

bool wxSlider::OS2OnScroll(int WXUNUSED(orientation), WXWORD wParam,
                           WXWORD pos, WXHWND control)
{
    int position = 0; // Dummy - not used in this mode

    int nScrollInc;
    wxEventType scrollEvent = wxEVT_NULL;
// TODO:
/*
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
#else // Win16
            nScrollInc = pos - position;
#endif // Win32/16
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;

        default:
            nScrollInc = 0;
    }

    if ( nScrollInc == 0 )
    {
        // no event...
        return FALSE;
    }

    int newPos = (int)::SendMessage((HWND) control, TBM_GETPOS, 0, 0);
    if ( (newPos < GetMin()) || (newPos > GetMax()) )
    {
        // out of range - but we did process it
        return TRUE;
    }
*/
    int       newPos = 0; //temporary
    SetValue(newPos);

    wxScrollEvent event(scrollEvent, m_windowId);
    event.SetPosition(newPos);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);

    wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, GetId() );
    cevent.SetEventObject( this );

    return GetEventHandler()->ProcessEvent( cevent );
}

wxSlider::~wxSlider()
{
    // TODO:
}

int wxSlider::GetValue() const
{
    // TODO
    return 0;
}

void wxSlider::SetValue(int value)
{
    // TODO
}

void wxSlider::GetSize(int *width, int *height) const
{
    // TODO
}

void wxSlider::GetPosition(int *x, int *y) const
{
    // TODO
}

// TODO one day, make sense of all this horros and replace it with a readable
//      DoGetBestSize()
void wxSlider::DoSetSize(int x, int y, int width, int height, int sizeFlags)
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

// TODO:
/*

  AdjustForParentClientOrigin(x1, y1, sizeFlags);

  wxChar buf[300];

  int x_offset = x;
  int y_offset = y;

  int cx;     // slider,min,max sizes
  int cy;
  int cyf;

  wxGetCharSize(GetHWND(), &cx, &cy, & this->GetFont());

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
      // The height needs to be a bit bigger under Win95 if using native
      // 3D effects.
      valueHeight = (int) (valueHeight * 1.5) ;
      MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      x_offset += new_width + cx;
    }

    MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    x_offset += (int)(min_len + cx);

    int slider_length = (int)(w1 - x_offset - max_len - cx);

    int slider_height = h1;
    if (slider_height < 0 )
        slider_height = 20;

    // Slider must have a minimum/default length/height
    if (slider_length < 100)
      slider_length = 100;

    MoveWindow(GetHwnd(), x_offset, y_offset, slider_length, slider_height, TRUE);
    x_offset += slider_length + cx;

    MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
    else
    {
        // No labels
        // If we're prepared to use the existing size, then...
        if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
        {
            GetSize(&w1, &h1);
        }
        if ( w1 < 0 )
            w1 = 200;
        if ( h1 < 0 )
            h1 = 20;
        MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
    }
  }
  else
  {
    if ( m_windowStyle & wxSL_LABELS )
    {
    int min_len;
    GetWindowText((HWND) m_staticMin, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL, & this->GetFont());

    int max_len;
    GetWindowText((HWND) m_staticMax, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, & this->GetFont());

    if (m_staticValue)
    {
      int new_width = (int)(wxMax(min_len, max_len));
      int valueHeight = (int)cyf;
//// Suggested change by George Tasker - remove this block...
#ifdef __WIN32__
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
 ... and replace with following line:
      new_width += cx;

      // The height needs to be a bit bigger under Win95 if using native
      // 3D effects.
      valueHeight = (int) (valueHeight * 1.5) ;

      MoveWindow((HWND) m_staticValue, x_offset, y_offset, new_width, valueHeight, TRUE);
      y_offset += valueHeight;
    }

    MoveWindow((HWND) m_staticMin, x_offset, y_offset, (int)min_len, cy, TRUE);
    y_offset += cy;

    int slider_length = (int)(h1 - y_offset - cy - cy);

    int slider_width = w1;
    if (slider_width < 0 )
        slider_width = 20;

    // Slider must have a minimum/default length
    if (slider_length < 100)
      slider_length = 100;

    MoveWindow(GetHwnd(), x_offset, y_offset, slider_width, slider_length, TRUE);
    y_offset += slider_length;

    MoveWindow((HWND) m_staticMax, x_offset, y_offset, (int)max_len, cy, TRUE);
    }
    else
    {
        // No labels
        // If we're prepared to use the existing size, then...
        if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
        {
            GetSize(&w1, &h1);
        }
        if ( w1 < 0 )
            w1 = 20;
        if ( h1 < 0 )
            h1 = 200;
        MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
    }
  }
*/
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    // TODO
}

WXHBRUSH wxSlider::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    // TODO:
/*
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
  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
*/
  return (WXHBRUSH)0;
}

// For trackbars only
void wxSlider::SetTickFreq(int n, int pos)
{
    // TODO
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // TODO
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // TODO
}

void wxSlider::ClearTicks()
{
    // TODO
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    // TODO
}

int wxSlider::GetLineSize() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelEnd() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelStart() const
{
    // TODO
    return 0;
}

void wxSlider::SetSelection(int minPos, int maxPos)
{
    // TODO
}

void wxSlider::SetThumbLength(int len)
{
    // TODO
}

int wxSlider::GetThumbLength() const
{
    // TODO
    return 0;
}

void wxSlider::SetTick(int tickPos)
{
    // TODO
}

bool wxSlider::ContainsHWND(WXHWND hWnd) const
{
    return ( hWnd == GetStaticMin() || hWnd == GetStaticMax() || hWnd == GetEditValue() );
}

void wxSlider::Command (wxCommandEvent & event)
{
  SetValue (event.GetInt());
  ProcessCommand (event);
}

bool wxSlider::Show(bool show)
{
    // TODO
    return TRUE;
}

