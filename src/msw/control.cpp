/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/dcclient.h"
#endif

#include "wx/msw/private.h"

#if defined(__WIN95__) && !defined(__GNUWIN32__)
#include <commctrl.h>
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#undef GetWindowProc
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
	EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()
#endif

// Item members
wxControl::wxControl(void)
{
  m_backgroundColour = *wxWHITE;
  m_foregroundColour = *wxBLACK;
  m_callback = 0;
}

wxControl::~wxControl(void)
{
  m_isBeingDeleted = TRUE;

  // If we delete an item, we should initialize the parent panel,
  // because it could now be invalid.
  wxWindow *parent = (wxWindow *)GetParent();
  if (parent)
  {
    if (parent->GetDefaultItem() == this)
        parent->SetDefaultItem(NULL);
  }
}

void wxControl::SetLabel(const wxString& label)
{
  if (GetHWND())
    SetWindowText((HWND) GetHWND(), (const char *)label);
}

wxString wxControl::GetLabel(void) const
{
  wxBuffer[0] = 0;
  if (GetHWND())
    GetWindowText((HWND)GetHWND(), wxBuffer, 1000);

  return wxString(wxBuffer);
}

// Call this repeatedly for several wnds to find the overall size
// of the widget.
// Call it initially with -1 for all values in rect.
// Keep calling for other widgets, and rect will be modified
// to calculate largest bounding rectangle.
void wxFindMaxSize(WXHWND wnd, RECT *rect)
{
  int left = rect->left;
  int right = rect->right;
  int top = rect->top;
  int bottom = rect->bottom;

  GetWindowRect((HWND) wnd, rect);

  if (left < 0)
    return;

  if (left < rect->left)
    rect->left = left;

  if (right > rect->right)
    rect->right = right;

  if (top < rect->top)
    rect->top = top;

  if (bottom > rect->bottom)
    rect->bottom = bottom;

}

/*
// Not currently used
void wxConvertDialogToPixels(wxWindow *control, int *x, int *y)
{
  if (control->m_windowParent && control->m_windowParent->is_dialog)
  {
    DWORD word = GetDialogBaseUnits();
    int xs = LOWORD(word);
    int ys = HIWORD(word);
    *x = (int)(*x * xs/4);
    *y = (int)(*y * ys/8);
  }
  else
  {
    *x = *x;
    *y = *y;
  }
}
*/

void wxControl::MSWOnMouseMove(const int x, const int y, const WXUINT flags)
{
/*
  // Trouble with this is that it sets the cursor for controls too :-(
  if (m_windowCursor.Ok() && !wxIsBusy())
    ::SetCursor(m_windowCursor.GetHCURSOR());
*/

  if (!m_mouseInWindow)
  {
    // Generate an ENTER event
    m_mouseInWindow = TRUE;
    MSWOnMouseEnter(x, y, flags);
  }

  wxMouseEvent event(wxEVT_MOTION);

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime);
  event.SetEventObject( this );

  // Window gets a click down message followed by a mouse move
  // message even if position isn't changed!  We want to discard
  // the trailing move event if x and y are the same.
  if ((m_lastEvent == wxEVT_RIGHT_DOWN || m_lastEvent == wxEVT_LEFT_DOWN ||
       m_lastEvent == wxEVT_MIDDLE_DOWN) &&
      (m_lastXPos == event.GetX() && m_lastYPos == event.GetY()))
  {
    m_lastXPos = event.GetX(); m_lastYPos = event.GetY();
    m_lastEvent = wxEVT_MOTION;
    return;
  }

  m_lastEvent = wxEVT_MOTION;
  m_lastXPos = event.GetX(); m_lastYPos = event.GetY();
  GetEventHandler()->OldOnMouseEvent(event);
}

long wxControl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

bool wxControl::MSWNotify(const WXWPARAM wParam, const WXLPARAM lParam)
{
#if defined(__WIN95__)
	wxCommandEvent event(0, m_windowId);
	int eventType = 0;
	NMHDR *hdr1 = (NMHDR*) lParam;
	switch ( hdr1->code )
	{
		case NM_CLICK:
		{
			eventType = wxEVT_COMMAND_LEFT_CLICK;
			break;
		}
		case NM_DBLCLK:
		{
			eventType = wxEVT_COMMAND_LEFT_DCLICK;
			break;
		}
		case NM_RCLICK:
		{
			eventType = wxEVT_COMMAND_RIGHT_CLICK;
			break;
		}
		case NM_RDBLCLK:
		{
			eventType = wxEVT_COMMAND_RIGHT_DCLICK;
			break;
		}
		case NM_SETFOCUS:
		{
			eventType = wxEVT_COMMAND_SET_FOCUS;
			break;
		}
		case NM_KILLFOCUS:
		{
			eventType = wxEVT_COMMAND_KILL_FOCUS;
			break;
		}
		case NM_RETURN:
		{
			eventType = wxEVT_COMMAND_ENTER;
			break;
		}
/* Not implemented
		case NM_OUTOFMEMORY:
		{
			eventType = wxEVT_COMMAND_OUT_OF_MEMORY;
			break;
		}
*/
		default :
			return FALSE;
			break;
	}
    event.SetEventType(eventType);
	event.SetEventObject(this);

	if ( !ProcessEvent(event) )
		return FALSE;
	return TRUE;
#else
  return FALSE;
#endif
}

/*
 * Allocates control IDs within the appropriate range
 */


int NewControlId(void)
{
  static int controlId = 0;
  controlId ++;
  return controlId;
}

void wxControl::ProcessCommand (wxCommandEvent & event)
{
  // Tries:
  // 1) A callback function (to become obsolete)
  // 2) OnCommand, starting at this window and working up parent hierarchy
  // 3) OnCommand then calls ProcessEvent to search the event tables.
  if (m_callback)
    {
      (void) (*(m_callback)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }
}

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
  // In general, you don't want to erase the background of a control,
  // or you'll get a flicker.
  // TODO: move this 'null' function into each control that
  // might flicker.

  RECT rect;
  ::GetClientRect((HWND) GetHWND(), &rect);

  HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

  ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
  ::DeleteObject(hBrush);
  ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
}

void wxControl::SetClientSize (const int width, const int height)
{
  SetSize (-1, -1, width, height);
}

void wxControl::Centre (const int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxWindow *parent = (wxWindow *) GetParent ();
  if (!parent)
    return;

  parent->GetClientSize (&panel_width, &panel_height);
  GetSize (&width, &height);
  GetPosition (&x, &y);

  new_x = x;
  new_y = y;

  if (direction & wxHORIZONTAL)
    new_x = (int) ((panel_width - width) / 2);

  if (direction & wxVERTICAL)
    new_y = (int) ((panel_height - height) / 2);

  SetSize (new_x, new_y, width, height);
  int temp_x, temp_y;
  GetPosition (&temp_x, &temp_y);
  GetPosition (&temp_x, &temp_y);
}


