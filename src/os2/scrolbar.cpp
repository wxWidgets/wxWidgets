/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/utils.h"
#endif

#include "wx/scrolbar.h"
#include "wx/os2/private.h"

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

    // TODO create scrollbar

    m_pageSize = 1;
    m_viewSize = 1;
    m_objectSize = 1;

    SetFont(parent->GetFont());

    m_hWnd = 0; // TODO: (WXHWND)scroll_bar;

    // Subclass again for purposes of dialog editing mode
    SubclassWin((WXHWND) scroll_bar);

    SetSize(x, y, width, height);

    return TRUE;
}

wxScrollBar::~wxScrollBar()
{
}

bool wxScrollBar::OS2OnScroll(int WXUNUSED(orientation), WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    // TODO:
/*
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
*/
    return FALSE;
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    // TODO
}

int wxScrollBar::GetThumbPosition() const
{
    // TODO
    return 0;
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh)
{
    m_viewSize = pageSize;
    m_pageSize = thumbSize;
    m_objectSize = range;

    // TODO
}

#if WXWIN_COMPATIBILITY
void wxScrollBar::SetPageSize(int pageLength)
{
    m_pageSize = pageLength;

    // TODO:
}

void wxScrollBar::SetObjectLength(int objectLength)
{
    m_objectSize = objectLength;

    // The range (number of scroll steps) is the
    // object length minus the view size.
    int range = wxMax((objectLength - m_viewSize), 0) ;

    // TODO:
}

void wxScrollBar::SetViewLength(int viewLength)
{
    m_viewSize = viewLength;
}

void wxScrollBar::GetValues(int *viewStart, int *viewLength, int *objectLength,
           int *pageLength) const
{
// TODO:
/*
    *viewStart = ::GetScrollPos((HWND)m_hWnd, SB_CTL);
    *viewLength = m_viewSize;
    *objectLength = m_objectSize;
    *pageLength = m_pageSize;
*/
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
    // TODO:
/*
    wxEventType oldEvent = event.GetEventType();
    event.SetEventType( wxEVT_COMMAND_SCROLLBAR_UPDATED );
    if ( !GetEventHandler()->ProcessEvent(event) )
    {
        event.SetEventType( oldEvent );
        if (!GetParent()->GetEventHandler()->ProcessEvent(event))
            event.Skip();
    }
*/
}
#endif
