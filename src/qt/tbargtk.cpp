/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.cpp
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbargtk.h"
#endif

#include "wx/toolbar.h"

//-----------------------------------------------------------------------------
// wxToolBarTool
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBarTool,wxObject)
  
wxToolBarTool::wxToolBarTool( wxToolBar *owner, int theIndex, 
      const wxBitmap& bitmap1, const  wxBitmap& bitmap2,
      bool toggle, wxObject *clientData,
      const wxString& shortHelpString, const wxString& longHelpString )
{
  m_owner = owner;
  m_index = theIndex;
  m_bitmap1 = bitmap1;
  m_bitmap2 = bitmap2;
  m_isToggle = toggle;
  m_enabled = TRUE;
  m_toggleState = FALSE;
  m_shortHelpString = shortHelpString;
  m_longHelpString = longHelpString;
  m_isMenuCommand = TRUE;
  m_clientData = clientData;
  m_deleteSecondBitmap = FALSE;
};

wxToolBarTool::~wxToolBarTool(void)
{
};

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar,wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxControl)
END_EVENT_TABLE()

wxToolBar::wxToolBar(void)
{
};

wxToolBar::wxToolBar( wxWindow *parent, wxWindowID id, 
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
  Create( parent, id, pos, size, style, name );
};

wxToolBar::~wxToolBar(void)
{
};

bool wxToolBar::Create( wxWindow *parent, wxWindowID id, 
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
  return TRUE;
};

bool wxToolBar::OnLeftClick( int toolIndex, bool toggleDown )
{
  wxCommandEvent event( wxEVT_COMMAND_TOOL_CLICKED, toolIndex );
  event.SetEventObject(this);
  event.SetInt( toolIndex );
  event.SetExtraLong((long) toggleDown);

  GetEventHandler()->ProcessEvent(event);

  return TRUE;
};

void wxToolBar::OnRightClick( int toolIndex, float WXUNUSED(x), float WXUNUSED(y) )
{
  wxCommandEvent event( wxEVT_COMMAND_TOOL_RCLICKED, toolIndex );
  event.SetEventObject( this );
  event.SetInt( toolIndex );

  GetEventHandler()->ProcessEvent(event);
};

void wxToolBar::OnMouseEnter( int toolIndex )
{
  wxCommandEvent event( wxEVT_COMMAND_TOOL_ENTER, toolIndex );
  event.SetEventObject(this);
  event.SetInt( toolIndex );

  GetEventHandler()->ProcessEvent(event);
};

wxToolBarTool *wxToolBar::AddTool( int toolIndex, const wxBitmap& bitmap, 
  const wxBitmap& pushedBitmap, bool toggle,
  float WXUNUSED(xPos), float WXUNUSED(yPos), wxObject *clientData,
  const wxString& helpString1, const wxString& helpString2 )
{
};

void wxToolBar::AddSeparator(void)
{
};

void wxToolBar::ClearTools(void)
{
};

void wxToolBar::Realize(void)
{
};

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
    if (tool->m_index == toolIndex)
    { 
      tool->m_enabled = enable;
      return;
    }
    node = node->Next();
  };
};

void wxToolBar::ToggleTool(int WXUNUSED(toolIndex), bool WXUNUSED(toggle) ) 
{
};

wxObject *wxToolBar::GetToolClientData(int index) const
{
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
    if (tool->m_index == index) return tool->m_clientData;;
    node = node->Next();
  };
  return (wxObject*)NULL;
};

bool wxToolBar::GetToolState(int toolIndex) const
{
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
    if (tool->m_index == toolIndex) return tool->m_toggleState;
    node = node->Next();
  };
  return FALSE;
};

bool wxToolBar::GetToolEnabled(int toolIndex) const
{
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
    if (tool->m_index == toolIndex) return tool->m_enabled;
    node = node->Next();
  };
  return FALSE;
};

void wxToolBar::SetMargins( int WXUNUSED(x), int WXUNUSED(y) )
{
};

void wxToolBar::SetToolPacking( int WXUNUSED(packing) )
{
};

void wxToolBar::SetToolSeparation( int separation )
{
};

