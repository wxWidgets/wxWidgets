/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "scrolbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/utils.h"
#endif

#include "wx/scrolbar.h"
#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

BEGIN_EVENT_TABLE(wxScrollBar, wxControl)
#if WXWIN_COMPATIBILITY
  EVT_SCROLL(wxScrollBar::OnScroll)
#endif
END_EVENT_TABLE()

#endif

// Scrollbar
bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    if (!parent)
        return FALSE;
    parent->AddChild(this);
    SetName(name);
    SetValidator(validator);
    
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

    if (width == -1)
    {
      if (style & wxHORIZONTAL)
        width = 140;
      else
        width = 14;
    }
    if (height == -1)
    {
      if (style & wxVERTICAL)
        height = 140;
      else
        height = 14;
    }

    // Now create scrollbar
    DWORD _direction = (style & wxHORIZONTAL) ?
                        SBS_HORZ: SBS_VERT;
    HWND scroll_bar = CreateWindowEx(MakeExtendedStyle(style), wxT("SCROLLBAR"), wxT("scrollbar"),
                         _direction | WS_CHILD | WS_VISIBLE,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                         wxGetInstance(), NULL);

    m_pageSize = 1;
    m_viewSize = 1;
    m_objectSize = 1;

    ::SetScrollRange(scroll_bar, SB_CTL, 0, 1, FALSE);
    ::SetScrollPos(scroll_bar, SB_CTL, 0, FALSE);
    ShowWindow(scroll_bar, SW_SHOW);

    SetFont(parent->GetFont());

    m_hWnd = (WXHWND)scroll_bar;

    // Subclass again for purposes of dialog editing mode
    SubclassWin((WXHWND) scroll_bar);

    SetSize(x, y, width, height);

    return TRUE;
}

wxScrollBar::~wxScrollBar(void)
{
}

bool wxScrollBar::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    int position = ::GetScrollPos((HWND) control, SB_CTL);
    int minPos, maxPos;
    ::GetScrollRange((HWND) control, SB_CTL, &minPos, &maxPos);

#if defined(__WIN95__)
    // A page size greater than one has the effect of reducing the effective
    // range, therefore the range has already been boosted artificially - so
    // reduce it again.
    if ( m_pageSize > 1 )
        maxPos -= (m_pageSize - 1);
#endif // __WIN95__

    wxEventType scrollEvent = wxEVT_NULL;

    int nScrollInc;
    switch ( wParam )
    {
        case SB_TOP:
            nScrollInc = maxPos - position;
            scrollEvent = wxEVT_SCROLL_TOP;
            break;

        case SB_BOTTOM:
            nScrollInc = - position;
            scrollEvent = wxEVT_SCROLL_BOTTOM;
            break;

        case SB_LINEUP:
            nScrollInc = -1;
            scrollEvent = wxEVT_SCROLL_LINEUP;
            break;

        case SB_LINEDOWN:
            nScrollInc = 1;
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
            nScrollInc = pos - position;
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;

        default:
            nScrollInc = 0;
    }

    if ( nScrollInc == 0 )
    {
        // no event to process, so don't process it
        return FALSE;
    }

    int new_pos = position + nScrollInc;

    if (new_pos < 0)
        new_pos = 0;
    if (new_pos > maxPos)
        new_pos = maxPos;

    SetThumbPosition(new_pos);
    wxScrollEvent event(scrollEvent, m_windowId);
    event.SetPosition(new_pos);
    event.SetEventObject( this );

    return GetEventHandler()->ProcessEvent(event);
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
#if defined(__WIN95__)
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = 0;
  info.nMin = 0;
  info.nPos = viewStart;
  info.fMask = SIF_POS ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#else
  ::SetScrollPos((HWND) GetHWND(), SB_CTL, viewStart, TRUE);
#endif
}

int wxScrollBar::GetThumbPosition(void) const
{
    return ::GetScrollPos((HWND)m_hWnd, SB_CTL);
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh)
{
  m_viewSize = pageSize;
  m_pageSize = thumbSize;
  m_objectSize = range;

  // The range (number of scroll steps) is the
  // object length minus the page size.
  int range1 = wxMax((m_objectSize - m_pageSize), 0) ;

#if defined(__WIN95__)
  // Try to adjust the range to cope with page size > 1
  // (see comment for SetPageLength)
  if ( m_pageSize > 1 )
  {
    range1 += (m_pageSize - 1);
  }

  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = m_pageSize;
  info.nMin = 0;
  info.nMax = range1;
  info.nPos = position;

  info.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, refresh);
#else
  ::SetScrollPos((HWND)m_hWnd, SB_CTL, position, TRUE);
  ::SetScrollRange((HWND)m_hWnd, SB_CTL, 0, range1, TRUE);
#endif
}


/* From the WIN32 documentation:
In version 4.0 or later, the maximum value that a scroll bar can report
(that is, the maximum scrolling position) depends on the page size.
If the scroll bar has a page size greater than one, the maximum scrolling position
is less than the maximum range value. You can use the following formula to calculate
the maximum scrolling position:
  
MaxScrollPos = MaxRangeValue - (PageSize - 1) 
*/

#if WXWIN_COMPATIBILITY
void wxScrollBar::SetPageSize(int pageLength)
{
  m_pageSize = pageLength;

#if defined(__WIN95__)
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = pageLength;
  info.fMask = SIF_PAGE ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#endif
}

void wxScrollBar::SetObjectLength(int objectLength)
{
  m_objectSize = objectLength;

  // The range (number of scroll steps) is the
  // object length minus the view size.
  int range = wxMax((objectLength - m_viewSize), 0) ;

#if defined(__WIN95__)
  // Try to adjust the range to cope with page size > 1
  // (see comment for SetPageLength)
  if ( m_pageSize > 1 )
  {
    range += (m_pageSize - 1);
  }

  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = 0;
  info.nMin = 0;
  info.nMax = range;
  info.nPos = 0;
  info.fMask = SIF_RANGE ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#else
  ::SetScrollRange((HWND)m_hWnd, SB_CTL, 0, range, TRUE);
#endif
}

void wxScrollBar::SetViewLength(int viewLength)
{
    m_viewSize = viewLength;
}

void wxScrollBar::GetValues(int *viewStart, int *viewLength, int *objectLength,
           int *pageLength) const
{
    *viewStart = ::GetScrollPos((HWND)m_hWnd, SB_CTL);
    *viewLength = m_viewSize;
    *objectLength = m_objectSize;
    *pageLength = m_pageSize;
}
#endif

WXHBRUSH wxScrollBar::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
  return 0;
}

void wxScrollBar::Command(wxCommandEvent& event)
{
    SetThumbPosition(event.m_commandInt);
    ProcessCommand(event);
}

#if WXWIN_COMPATIBILITY
// Backward compatibility
void wxScrollBar::OnScroll(wxScrollEvent& event)
{
    wxEventType oldEvent = event.GetEventType();
    event.SetEventType( wxEVT_COMMAND_SCROLLBAR_UPDATED );
    if ( !GetEventHandler()->ProcessEvent(event) )
    {
        event.SetEventType( oldEvent );
        if (!GetParent()->GetEventHandler()->ProcessEvent(event))
            event.Skip();
    }
}
#endif
