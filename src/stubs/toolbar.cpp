/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "toolbar.h"
#endif

#include "wx/wx.h"
#include "wx/toolbar.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
END_EVENT_TABLE()
#endif

wxToolBar::wxToolBar()
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_defaultWidth = 24;
  m_defaultHeight = 22;
  // TODO
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_maxWidth = -1;
    m_maxHeight = -1;
  
    m_defaultWidth = 24;
    m_defaultHeight = 22;
    SetName(name);

    m_windowStyle = style;

    SetParent(parent);

    if (parent) parent->AddChild(this);

    // TODO create toolbar
  
    return FALSE;
}

wxToolBar::~wxToolBar()
{
    // TODO
}

bool wxToolBar::CreateTools()
{
    if (m_tools.Number() == 0)
        return FALSE;

    // TODO
    return FALSE;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x; m_defaultHeight = size.y;
    // TODO
}

wxSize wxToolBar::GetMaxSize() const
{
    // TODO
    return wxSize(0, 0);
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    // TODO
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        tool->m_enabled = enable;
        // TODO enable button
    }
}

void wxToolBar::ToggleTool(int toolIndex, bool toggle)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if (tool->m_isToggle)
        {
            tool->m_toggleState = toggle;
            // TODO: set toggle state
        }
    }
}

void wxToolBar::ClearTools()
{
    // TODO
    wxToolBarBase::ClearTools();
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.

wxToolBarTool *wxToolBar::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, long xPos, long yPos, wxObject *clientData, const wxString& helpString1, const wxString& helpString2)
{
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, (wxBitmap *)NULL, toggle, xPos, yPos, helpString1, helpString2);
  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  tool->SetSize(GetDefaultButtonWidth(), GetDefaultButtonHeight());

  m_tools.Append((long)index, tool);
  return tool;
}

