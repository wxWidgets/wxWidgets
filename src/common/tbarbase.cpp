/////////////////////////////////////////////////////////////////////////////
// Name:        common/tbarbase.cpp
// Purpose:     wxToolBarBase implementation
// Author:      Julian Smart
// Modified by: VZ at 11.12.99 (wxScrollableToolBar splitted off)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_ABSTRACT_CLASS(wxToolBarBase, wxControl)
    IMPLEMENT_DYNAMIC_CLASS(wxToolBarTool, wxObject)

    BEGIN_EVENT_TABLE(wxToolBarBase, wxControl)
        EVT_IDLE(wxToolBarBase::OnIdle)
    END_EVENT_TABLE()
#endif

WX_DEFINE_LIST(wxToolBarToolsList);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

bool wxToolBarToolBase::Enable(bool enable)
{
    if ( m_enabled == enable )
        return FALSE;

    m_enabled = enable;

    return TRUE;
}

bool wxToolBarToolBase::Toggle(bool toggle)
{
    if ( m_toggled == toggle )
        return FALSE;

    m_toggled = toggle;

    return TRUE;
}

bool wxToolBarToolBase::SetToggle(bool toggle)
{
    if ( m_isToggle == toggle )
        return FALSE;

    m_isToggle = toggle;

    return TRUE;
}

bool wxToolBarToolBase::SetShortHelp(const wxString& help)
{
    if ( m_shortHelpString == help )
        return FALSE;

    m_shortHelpString = help;

    return TRUE;
}

bool wxToolBarToolBase::SetLongHelp(const wxString& help)
{
    if ( m_longHelpString == help )
        return FALSE;

    m_longHelpString = help;

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxToolBarBase adding/deleting items
// ----------------------------------------------------------------------------

wxToolBarBase::wxToolBarBase()
{
    // the list owns the pointers
    m_tools.DeleteContents(TRUE);

    m_xMargin = m_yMargin = NULL;

    m_maxRows = m_maxCols = 0;
}

wxToolBarTool *wxToolBarBase::AddTool(int id,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& pushedBitmap,
                                      bool toggle,
                                      wxCoord WXUNUSED(xPos),
                                      wxCoord WXUNUSED(yPos),
                                      wxObject *clientData,
                                      const wxString& helpString1,
                                      const wxString& helpString2)
{
    wxToolBarTool *tool = wxToolBarToolBase::New(this, id,
                                                 bitmap, pushedBitmap,
                                                 toggle, clientData,
                                                 helpString1, helpString2);

    if ( !tool || !DoInsertTool(GetToolsCount(), tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Append(tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::InsertTool(size_t pos,
                                         int id,
                                         const wxBitmap& bitmap,
                                         const wxBitmap& pushedBitmap,
                                         bool toggle,
                                         wxCoord WXUNUSED(xPos),
                                         wxCoord WXUNUSED(yPos),
                                         wxObject *clientData,
                                         const wxString& helpString1,
                                         const wxString& helpString2)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarTool *)NULL,
                 _T("invalid position in wxToolBar::InsertTool()") );

    wxToolBarTool *tool = wxToolBarToolBase::New(this, id,
                                                 bitmap, pushedBitmap,
                                                 toggle, clientData,
                                                 helpString1, helpString2);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::AddControl(wxControl *control)
{
    wxCHECK_MSG( control, NULL, _T("toolbar: can't insert NULL control") );

    wxCHECK_MSG( control->GetParent() == this, NULL,
                 _T("control must have toolbar as parent") );

    wxToolBarTool *tool = wxToolBarToolBase::New(this, control);

    if ( !tool || !DoInsertTool(GetToolsCount(), tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Append(tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::InsertControl(size_t pos, wxControl *control)
{
    wxCHECK_MSG( control, NULL, _T("toolbar: can't insert NULL control") );

    wxCHECK_MSG( control->GetParent() == this, NULL,
                 _T("control must have toolbar as parent") );

    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarTool *)NULL,
                 _T("invalid position in wxToolBar::InsertControl()") );

    wxToolBarTool *tool = wxToolBarToolBase::New(this, control);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::AddSeparator()
{
    wxToolBarTool *tool = wxToolBarToolBase::New(this, wxID_SEPARATOR);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Append(tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::InsertSeparator(size_t pos)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarTool *)NULL,
                 _T("invalid position in wxToolBar::InsertSeparator()") );

    wxToolBarTool *tool = wxToolBarToolBase::New(this, wxID_SEPARATOR);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarTool *wxToolBarBase::RemoveTool(int id)
{
    size_t pos = 0;
    wxToolBarToolsList::Node *node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        if ( node->GetData()->GetId() == id )
            break;

        pos++;
    }

    if ( !node )
    {
        // don't give any error messages - sometimes we might call RemoveTool()
        // without knowing whether the tool is or not in the toolbar
        return (wxToolBarTool *)NULL;
    }

    wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
    if ( !DoDeleteTool(pos, tool) )
    {
        return (wxToolBarTool *)NULL;
    }

    // the node would delete the data, so set it to NULL to avoid this
    node->SetData(NULL);

    m_tools.DeleteNode(node);

    return tool;
}

bool wxToolBarBase::DeleteToolByPos(size_t pos)
{
    wxCHECK_MSG( pos < GetToolsCount(), FALSE,
                 _T("invalid position in wxToolBar::DeleteToolByPos()") );

    wxToolBarToolsList::Node *node = m_tools.Item(pos);

    if ( !DoDeleteTool(pos, node->GetData()) )
    {
        return FALSE;
    }

    m_tools.DeleteNode(node);

    return TRUE;
}

bool wxToolBarBase::DeleteTool(int id)
{
    size_t pos = 0;
    wxToolBarToolsList::Node *node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        if ( node->GetData()->GetId() == id )
            break;

        pos++;
    }

    if ( !DoDeleteTool(pos, node->GetData()) )
    {
        return FALSE;
    }

    m_tools.DeleteNode(node);

    return TRUE;
}

wxToolBarToolBase *wxToolBarBase::FindById(int id) const
{
    wxToolBarToolBase *tool = (wxToolBarToolBase *)NULL;

    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        tool = node->GetData();
        if ( tool->GetId() == id )
        {
            // found
            break;
        }
    }

    return tool;
}

void wxToolBarBase::ClearTools()
{
    m_tools.Clear();
}

bool wxToolBarBase::Realize()
{
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxToolBarBase tools state
// ----------------------------------------------------------------------------

void wxToolBarBase::EnableTool(int id, bool enable)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        if ( tool->Enable(enable) )
        {
            DoEnableTool(tool, enable);
        }
    }
}

void wxToolBarBase::ToggleTool(int id, bool toggle);
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        if ( tool->Toggle(enable) )
        {
            DoToggleTool(tool, toggle);
        }
    }
}

void wxToolBarBase::SetToggle(int id, bool toggle);
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        if ( tool->SetToggle(enable) )
        {
            DoSetToggle(tool, toggle);
        }
    }
}

void wxToolBarBase::SetToolShortHelp(int id, const wxString& help);
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        (void)tool->SetShortHelp(help);
    }
}

void wxToolBarBase::SetToolLongHelp(int id, const wxString& help);
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        (void)tool->SetLongHelp(help);
    }
}

wxObject *wxToolBarBase::GetToolClientData(int id) const
{
    wxToolBarToolBase *tool = FindById(id);

    return tool ? tool->GetClientData() : (wxObject *)NULL;
}

bool wxToolBarBase::GetToolState(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->IsToggled();
}

bool wxToolBarBase::GetToolEnabled(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->IsEnabled();
}

wxString wxToolBarBase::GetToolShortHelp(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->GetShortHelp();
}

wxString wxToolBarBase::GetToolLongHelp(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->GetLongHelp();
}

// ----------------------------------------------------------------------------
// wxToolBarBase geometry
// ----------------------------------------------------------------------------

void wxToolBarBase::SetMargins(int x, int y)
{
    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBarBase::SetRows(int WXUNUSED(nRows))
{
    // nothing
}

// ----------------------------------------------------------------------------
// event processing
// ----------------------------------------------------------------------------

// Only allow toggle if returns TRUE
bool wxToolBarBase::OnLeftClick(int id, bool toggleDown)
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED, id);
    event.SetEventObject(this);
    event.SetExtraLong((long) toggleDown);

    // Send events to this toolbar instead (and thence up the window hierarchy)
    GetEventHandler()->ProcessEvent(event);

    return TRUE;
}

// Call when right button down.
void wxToolBarBase::OnRightClick(int id,
                                 long WXUNUSED(x),
                                 long WXUNUSED(y))
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_RCLICKED, id);
    event.SetEventObject(this);
    event.SetInt(toolIndex);

    GetEventHandler()->ProcessEvent(event);
}

// Called when the mouse cursor enters a tool bitmap (no button pressed).
// Argument is -1 if mouse is exiting the toolbar.
// Note that for this event, the id of the window is used,
// and the integer parameter of wxCommandEvent is used to retrieve
// the tool id.
void wxToolBarBase::OnMouseEnter (int id)
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_ENTER, GetId());
    event.SetEventObject(this);
    event.SetInt(id);

    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// UI updates
// ----------------------------------------------------------------------------

void wxToolBarBase::OnIdle(wxIdleEvent& event)
{
    DoToolbarUpdates();

    event.Skip();
}

// Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
void wxToolBarBase::DoToolbarUpdates()
{
    wxEvtHandler* evtHandler = GetEventHandler();

    for ( wxToolBarToolsList::Node* node = GetTools().First();
          node;
          node = node->GetNext() )
    {
        int id = node->GetData()->GetId();

        wxUpdateUIEvent event(m_id);
        event.SetEventObject(this);

        if ( evtHandler->ProcessEvent(event) )
        {
            if ( event.GetSetEnabled() )
                EnableTool(id, event.GetEnabled());
            if ( event.GetSetChecked() )
                ToggleTool(id, event.GetChecked());
#if 0
            if ( event.GetSetText() )
                // Set tooltip?
#endif // 0
        }
    }
}

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
  m_id = theIndex;
  m_isToggle = toggle;
  m_toggleState = FALSE;
  m_enabled = TRUE;
  m_bitmap1 = theBitmap1;
  m_bitmap2 = theBitmap2;
  m_width = m_height = 0;
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
    m_id = control->GetId();
}

wxToolBarTool::~wxToolBarTool()
{
}


// class wxToolBar

wxToolBarBase::wxToolBarBase(void) : m_tools(wxKEY_INTEGER)
{
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
  for ( wxNode *node = m_tools.First(); node; node = node->Next() )
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    delete tool;
  }
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
  tool->m_id = -1;
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
void wxToolBarBase::LayoutTools()
{
}


// SCROLLING IMPLEMENTATION

BEGIN_EVENT_TABLE()
        EVT_SCROLL(wxToolBarBase::OnScroll)
        EVT_SIZE(wxToolBarBase::OnSize)
END_EVENT_TABLE()

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

#endif // wxUSE_TOOLBAR
