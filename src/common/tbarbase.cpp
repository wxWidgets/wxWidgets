/////////////////////////////////////////////////////////////////////////////
// Name:        tbarbase.cpp
// Purpose:     Toolbar base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbarbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/frame.h"

// For ::UpdateWindow
#ifdef __WXMSW__
#include <windows.h>
#endif

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"

IMPLEMENT_ABSTRACT_CLASS(wxToolBarBase, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxToolBarTool, wxObject)

BEGIN_EVENT_TABLE(wxToolBarBase, wxControl)
  EVT_SCROLL(wxToolBarBase::OnScroll)
  EVT_SIZE(wxToolBarBase::OnSize)
  EVT_IDLE(wxToolBarBase::OnIdle)
END_EVENT_TABLE()

// Keep a list of all toolbars created, so you can tell whether a toolbar
// is still valid: a tool may have quit the toolbar.
static wxList gs_ToolBars;

#ifdef __WXGTK__
wxToolBarTool::wxToolBarTool(wxToolBar *owner, int theIndex,
                    const wxBitmap& theBitmap1, const  wxBitmap& theBitmap2,
            bool toggle, wxObject *clientData,
                 const wxString& helpS1, const wxString& helpS2,
                    GtkWidget *pixmap  )
#else
wxToolBarTool::wxToolBarTool(int theIndex,
                    const wxBitmap& theBitmap1, const wxBitmap& theBitmap2, bool toggle,
                    long xPos, long yPos, const wxString& helpS1, const wxString& helpS2)
#endif
{
  m_toolStyle = wxTOOL_STYLE_BUTTON;
#ifdef __WXGTK__
  m_owner = owner;
  m_pixmap = pixmap;
  m_item = (GtkWidget*) NULL;
  m_clientData = clientData;
  m_x = 0;
  m_y = 0;
#else
  m_clientData = NULL;
  m_x = xPos;
  m_y = yPos;
#endif
  m_index = theIndex;
  m_isToggle = toggle;
  m_toggleState = FALSE;
  m_enabled = TRUE;
  m_bitmap1 = theBitmap1;
  m_bitmap2 = theBitmap2;
  m_width = m_height = 0;
  m_deleteSecondBitmap = FALSE;
  if (m_bitmap1.Ok())
  {
    m_width = m_bitmap1.GetWidth()+2;
    m_height = m_bitmap1.GetHeight()+2;
  }
  m_shortHelpString = helpS1;
  m_longHelpString = helpS2;
  m_control = (wxControl*) NULL;
}

wxToolBarTool::wxToolBarTool(wxControl *control)
{
    m_toolStyle = wxTOOL_STYLE_CONTROL;
    m_control = control;
    m_index = control->GetId();
}

wxToolBarTool::~wxToolBarTool()
{
/*
  if (m_deleteSecondBitmap && m_bitmap2)
    delete m_bitmap2;
*/
}


// class wxToolBar

wxToolBarBase::wxToolBarBase(void) : m_tools(wxKEY_INTEGER)
{
  gs_ToolBars.Append(this);

  m_maxRows = 1;
  m_maxCols = 32000;
  m_maxWidth = 0;
  m_maxHeight = 0;
  m_defaultWidth = 16;
  m_defaultHeight = 15;
  m_xMargin = 0;
  m_yMargin = 0;
  m_toolPacking = 1;
  m_toolSeparation = 5;
  m_currentTool = -1;

  m_xScrollPixelsPerLine = 0;
  m_yScrollPixelsPerLine = 0;
  m_xScrollingEnabled = TRUE;
  m_yScrollingEnabled = TRUE;
  m_xScrollPosition = 0;
  m_yScrollPosition = 0;
  m_calcScrolledOffset = TRUE;
  m_xScrollLines = 0;
  m_yScrollLines = 0;
  m_xScrollLinesPerPage = 0;
  m_yScrollLinesPerPage = 0;
}

wxToolBarBase::~wxToolBarBase ()
{
  gs_ToolBars.DeleteObject(this);

  for ( wxNode *node = m_tools.First(); node; node = node->Next() )
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    delete tool;
  }
}

// Only allow toggle if returns TRUE
bool wxToolBarBase::OnLeftClick(int toolIndex, bool toggleDown)
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED, toolIndex);
    event.SetEventObject(this);
    event.SetExtraLong((long) toggleDown);

    // Send events to this toolbar instead (and thence up the window hierarchy)
    GetEventHandler()->ProcessEvent(event);

    return TRUE;
}

// Call when right button down.
void wxToolBarBase::OnRightClick(int toolIndex,
                                 long WXUNUSED(x),
                                 long WXUNUSED(y))
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_RCLICKED, toolIndex);
    event.SetEventObject(this);
    event.SetInt(toolIndex);

    GetEventHandler()->ProcessEvent(event);
}

// Called when the mouse cursor enters a tool bitmap (no button pressed).
// Argument is -1 if mouse is exiting the toolbar.
// Note that for this event, the id of the window is used,
// and the integer parameter of wxCommandEvent is used to retrieve
// the tool id.
void wxToolBarBase::OnMouseEnter ( int toolIndex )
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_ENTER, GetId());
    event.SetEventObject(this);
    event.SetInt(toolIndex);

    GetEventHandler()->ProcessEvent(event);
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.
wxToolBarTool *wxToolBarBase::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, wxCoord xPos, wxCoord yPos, wxObject *clientData,
             const wxString& helpString1, const wxString& helpString2)
{
#ifdef __WXGTK__
  wxToolBarTool *tool = new wxToolBarTool( (wxToolBar*)this, index, bitmap, pushedBitmap, toggle,
                                           (wxObject*) NULL, helpString1, helpString2);
#else
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, pushedBitmap, toggle, xPos, yPos, helpString1, helpString2);
#endif
  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  // Calculate reasonable max size in case Layout() not called
  if ((tool->m_x + bitmap.GetWidth() + m_xMargin) > m_maxWidth)
    m_maxWidth = (tool->m_x + bitmap.GetWidth() + m_xMargin);

  if ((tool->m_y + bitmap.GetHeight() + m_yMargin) > m_maxHeight)
    m_maxHeight = (tool->m_y + bitmap.GetHeight() + m_yMargin);

  m_tools.Append((long)index, tool);
  return tool;
}

void wxToolBarBase::AddSeparator ()
{
  wxToolBarTool *tool = new wxToolBarTool;
  tool->m_index = -1;
  tool->m_toolStyle = wxTOOL_STYLE_SEPARATOR;
  m_tools.Append(-1, tool);
}

void wxToolBarBase::ClearTools()
{
  m_pressedTool = m_currentTool = -1;
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    wxNode *nextNode = node->Next();
    delete tool;
    delete node;
    node = nextNode;
  }
}

void wxToolBarBase::EnableTool(int index, bool enable)
{
  wxNode *node = m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
      tool->m_enabled = enable;
  }
}

void wxToolBarBase::ToggleTool(int WXUNUSED(index),
                               bool WXUNUSED(toggle))
{
}

void wxToolBarBase::SetToggle(int index, bool value)
{
  wxNode *node=m_tools.Find((long)index);
  if (node){
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    tool->m_isToggle = value;
  }
}

bool wxToolBarBase::GetToolState(int index) const
{
  wxNode *node = m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->m_toggleState;
    }
    else return FALSE;
  }
  else return FALSE;
}

bool wxToolBarBase::GetToolEnabled(int index) const
{
  wxNode *node = m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->m_enabled;
    }
    else return FALSE;
  }
  else return FALSE;
}

wxObject *wxToolBarBase::GetToolClientData(int index) const
{
  wxNode *node = m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->m_clientData;
    }
    else return NULL;
  }
  else return NULL;
}

void wxToolBarBase::SetToolShortHelp(int index, const wxString& helpString)
{
  wxNode *node=m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    tool->m_shortHelpString = helpString;
  }
}

wxString wxToolBarBase::GetToolShortHelp(int index) const
{
  wxNode *node=m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    return tool->m_shortHelpString;
  }
  else
    return wxString("");
}

void wxToolBarBase::SetToolLongHelp(int index, const wxString& helpString)
{
  wxNode *node=m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    tool->m_longHelpString = helpString;
  }
}

wxString wxToolBarBase::GetToolLongHelp(int index) const
{
  wxNode *node=m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    return tool->m_longHelpString;
  }
  else
    return wxString("");
}

wxToolBarTool *wxToolBarBase::FindToolForPosition(long x, long y) const
{
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if ((x >= tool->m_x) && (y >= tool->m_y) &&
        (x <= (tool->m_x + tool->GetWidth())) &&
        (y <= (tool->m_y + tool->GetHeight())))
      return tool;

    node = node->Next();
  }
  return NULL;
}

wxSize wxToolBarBase::GetMaxSize ( void ) const
{
  return wxSize(m_maxWidth, m_maxHeight);
}

// Okay, so we've left the tool we're in ... we must check if
// the tool we're leaving was a 'sprung push button' and if so,
// spring it back to the up state.
//
void wxToolBarBase::SetMargins(int x, int y)
{
  m_xMargin = x;
  m_yMargin = y;
}

void wxToolBarBase::SetToolPacking(int packing)
{
  m_toolPacking = packing;
}

void wxToolBarBase::SetToolSeparation(int separation)
{
  m_toolSeparation = separation;
}

void wxToolBarBase::Command(wxCommandEvent& WXUNUSED(event))
{
}

void wxToolBarBase::LayoutTools()
{
}


// SCROLLING IMPLEMENTATION

/*
 * pixelsPerUnitX/pixelsPerUnitY: number of pixels per unit (e.g. pixels per text line)
 * noUnitsX/noUnitsY:        : no. units per scrollbar
 */
void wxToolBarBase::SetScrollbars (int pixelsPerUnitX, int pixelsPerUnitY,
           int noUnitsX, int noUnitsY,
           int xPos, int yPos)
{
      m_xScrollPixelsPerLine = pixelsPerUnitX;
      m_yScrollPixelsPerLine = pixelsPerUnitY;
      m_xScrollLines = noUnitsX;
      m_yScrollLines = noUnitsY;

      int w, h;
      GetSize(&w, &h);

      // Recalculate scroll bar range and position
    if (m_xScrollLines > 0)
    {
      m_xScrollPosition = xPos;
      SetScrollPos (wxHORIZONTAL, m_xScrollPosition, TRUE);
    }
    else
    {
      SetScrollbar(wxHORIZONTAL, 0, 0, 0, FALSE);
      m_xScrollPosition = 0;
    }

    if (m_yScrollLines > 0)
    {
      m_yScrollPosition = yPos;
      SetScrollPos (wxVERTICAL, m_yScrollPosition, TRUE);
    }
    else
    {
      SetScrollbar(wxVERTICAL, 0, 0, 0, FALSE);
      m_yScrollPosition = 0;
    }
    AdjustScrollbars();
    Refresh();
#ifdef __WXMSW__
    ::UpdateWindow ((HWND) GetHWND());
#endif
}


void wxToolBarBase::OnScroll(wxScrollEvent& event)
{
  int orient = event.GetOrientation();

  int nScrollInc = CalcScrollInc(event);
  if (nScrollInc == 0)
    return;

  if (orient == wxHORIZONTAL)
  {
    int newPos = m_xScrollPosition + nScrollInc;
    SetScrollPos(wxHORIZONTAL, newPos, TRUE );
  }
  else
  {
    int newPos = m_yScrollPosition + nScrollInc;
    SetScrollPos(wxVERTICAL, newPos, TRUE );
  }

  if (orient == wxHORIZONTAL)
  {
    if (m_xScrollingEnabled)
      ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, NULL);
    else
      Refresh();
  }
  else
  {
    if (m_yScrollingEnabled)
      ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, NULL);
    else
      Refresh();
  }

  if (orient == wxHORIZONTAL)
  {
    m_xScrollPosition += nScrollInc;
  }
  else
  {
    m_yScrollPosition += nScrollInc;
  }

}

int wxToolBarBase::CalcScrollInc(wxScrollEvent& event)
{
  int pos = event.GetPosition();
  int orient = event.GetOrientation();

  int nScrollInc = 0;
  switch (event.GetEventType())
  {
    case wxEVT_SCROLL_TOP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = - m_xScrollPosition;
      else
        nScrollInc = - m_yScrollPosition;
      break;
    }
    case wxEVT_SCROLL_BOTTOM:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = m_xScrollLines - m_xScrollPosition;
      else
        nScrollInc = m_yScrollLines - m_yScrollPosition;
      break;
    }
    case wxEVT_SCROLL_LINEUP:
    {
      nScrollInc = -1;
      break;
    }
    case wxEVT_SCROLL_LINEDOWN:
    {
      nScrollInc = 1;
      break;
    }
    case wxEVT_SCROLL_PAGEUP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
      else
        nScrollInc = -GetScrollPageSize(wxVERTICAL);
      break;
    }
    case wxEVT_SCROLL_PAGEDOWN:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = GetScrollPageSize(wxHORIZONTAL);
      else
        nScrollInc = GetScrollPageSize(wxVERTICAL);
      break;
    }
    case wxEVT_SCROLL_THUMBTRACK:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = pos - m_xScrollPosition;
      else
        nScrollInc = pos - m_yScrollPosition;
      break;
    }
    default:
    {
      break;
    }
  }
  if (orient == wxHORIZONTAL)
  {
        int w, h;
        GetClientSize(&w, &h);

        int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
        int noPositions = (int) ( ((nMaxWidth - w)/(float)m_xScrollPixelsPerLine) + 0.5 );
        if (noPositions < 0)
              noPositions = 0;

        if ( (m_xScrollPosition + nScrollInc) < 0 )
            nScrollInc = -m_xScrollPosition; // As -ve as we can go
        else if ( (m_xScrollPosition + nScrollInc) > noPositions )
            nScrollInc = noPositions - m_xScrollPosition; // As +ve as we can go

        return nScrollInc;
  }
  else
  {
        int w, h;
        GetClientSize(&w, &h);

        int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
        int noPositions = (int) ( ((nMaxHeight - h)/(float)m_yScrollPixelsPerLine) + 0.5 );
        if (noPositions < 0)
              noPositions = 0;

        if ( (m_yScrollPosition + nScrollInc) < 0 )
            nScrollInc = -m_yScrollPosition; // As -ve as we can go
        else if ( (m_yScrollPosition + nScrollInc) > noPositions )
            nScrollInc = noPositions - m_yScrollPosition; // As +ve as we can go

        return nScrollInc;
  }
}

// Adjust the scrollbars - new version.
void wxToolBarBase::AdjustScrollbars()
{
  int w, h;
  GetClientSize(&w, &h);

  // Recalculate scroll bar range and position
  if (m_xScrollLines > 0)
  {
    int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
    int newRange = (int) ( ((nMaxWidth)/(float)m_xScrollPixelsPerLine) + 0.5 );
    if (newRange < 0)
      newRange = 0;

    m_xScrollPosition = wxMin(newRange, m_xScrollPosition);

    // Calculate page size i.e. number of scroll units you get on the
    // current client window
    int noPagePositions = (int) ( (w/(float)m_xScrollPixelsPerLine) + 0.5 );
    if (noPagePositions < 1)
      noPagePositions = 1;

    SetScrollbar(wxHORIZONTAL, m_xScrollPosition, noPagePositions, newRange);
    SetScrollPageSize(wxHORIZONTAL, noPagePositions);
  }
  if (m_yScrollLines > 0)
  {
    int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
    int newRange = (int) ( ((nMaxHeight)/(float)m_yScrollPixelsPerLine) + 0.5 );
    if (newRange < 0)
      newRange = 0;

    m_yScrollPosition = wxMin(newRange, m_yScrollPosition);

    // Calculate page size i.e. number of scroll units you get on the
    // current client window
    int noPagePositions = (int) ( (h/(float)m_yScrollPixelsPerLine) + 0.5 );
    if (noPagePositions < 1)
      noPagePositions = 1;

    SetScrollbar(wxVERTICAL, m_yScrollPosition, noPagePositions, newRange);
    SetScrollPageSize(wxVERTICAL, noPagePositions);
  }
}

// Default OnSize resets scrollbars, if any
void wxToolBarBase::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_CONSTRAINTS
  if (GetAutoLayout())
    Layout();
#endif

  AdjustScrollbars();
}

// Prepare the DC by translating it according to the current scroll position
void wxToolBarBase::PrepareDC(wxDC& dc)
{
    dc.SetDeviceOrigin(- m_xScrollPosition * m_xScrollPixelsPerLine, - m_yScrollPosition * m_yScrollPixelsPerLine);
}

void wxToolBarBase::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
      *x_unit = m_xScrollPixelsPerLine;
      *y_unit = m_yScrollPixelsPerLine;
}

int wxToolBarBase::GetScrollPageSize(int orient) const
{
    if ( orient == wxHORIZONTAL )
        return m_xScrollLinesPerPage;
    else
        return m_yScrollLinesPerPage;
}

void wxToolBarBase::SetScrollPageSize(int orient, int pageSize)
{
    if ( orient == wxHORIZONTAL )
        m_xScrollLinesPerPage = pageSize;
    else
        m_yScrollLinesPerPage = pageSize;
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */
void wxToolBarBase::Scroll (int x_pos, int y_pos)
{
  int old_x, old_y;
  ViewStart (&old_x, &old_y);
  if (((x_pos == -1) || (x_pos == old_x)) && ((y_pos == -1) || (y_pos == old_y)))
    return;

  if (x_pos > -1)
    {
      m_xScrollPosition = x_pos;
      SetScrollPos (wxHORIZONTAL, x_pos, TRUE);
    }
  if (y_pos > -1)
    {
      m_yScrollPosition = y_pos;
      SetScrollPos (wxVERTICAL, y_pos, TRUE);
    }
  Refresh();
#ifdef __WXMSW__
  UpdateWindow ((HWND) GetHWND());
#endif
}

void wxToolBarBase::EnableScrolling (bool x_scroll, bool y_scroll)
{
  m_xScrollingEnabled = x_scroll;
  m_yScrollingEnabled = y_scroll;
}

void wxToolBarBase::GetVirtualSize (int *x, int *y) const
{
      *x = m_xScrollPixelsPerLine * m_xScrollLines;
      *y = m_yScrollPixelsPerLine * m_yScrollLines;
}

// Where the current view starts from
void wxToolBarBase::ViewStart (int *x, int *y) const
{
  *x = m_xScrollPosition;
  *y = m_yScrollPosition;
}

void wxToolBarBase::OnIdle(wxIdleEvent&
#ifdef __WXGTK__
                            WXUNUSED(event)
#else
                            event
#endif
                          )
{
#ifndef __WXGTK__
    wxWindow::OnIdle(event);
#endif

    DoToolbarUpdates();
}

// Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
void wxToolBarBase::DoToolbarUpdates()
{
    wxEvtHandler* evtHandler = GetEventHandler() ;

    wxNode* node = GetTools().First();
    while (node)
    {
        wxToolBarTool* tool = (wxToolBarTool* ) node->Data();

        wxUpdateUIEvent event(tool->m_index);
        event.SetEventObject(this);

        if (evtHandler->ProcessEvent(event))
        {
            if (event.GetSetEnabled())
                EnableTool(tool->m_index, event.GetEnabled());
            if (event.GetSetChecked())
                ToggleTool(tool->m_index, event.GetChecked());
/*
            if (event.GetSetText())
                // Set tooltip?
*/
        }

        node = node->Next();
    }
}

#endif
