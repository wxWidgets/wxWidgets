/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.cpp
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
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

wxToolBarTool::~wxToolBarTool()
{
};

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget), wxToolBarTool *tool )
{
  if (!tool->m_enabled) return;

  if (tool->m_isToggle) tool->m_toggleState = !tool->m_toggleState;

  tool->m_owner->OnLeftClick( tool->m_index, tool->m_toggleState );
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar,wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxControl)
END_EVENT_TABLE()

wxToolBar::wxToolBar()
{
};

wxToolBar::wxToolBar( wxWindow *parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
  Create( parent, id, pos, size, style, name );
};

wxToolBar::~wxToolBar()
{
};

bool wxToolBar::Create( wxWindow *parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
  m_needParent = TRUE;

  PreCreation( parent, id, pos, size, style, name );

  m_tools.DeleteContents( TRUE );

  m_widget = gtk_handle_box_new();

  m_toolbar = GTK_TOOLBAR( gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS ) );

  gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );

  gtk_widget_show( GTK_WIDGET(m_toolbar) );

  PostCreation();

  Show( TRUE );

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
  if (!bitmap.Ok()) return NULL;

  wxToolBarTool *tool = new wxToolBarTool( this, toolIndex, bitmap, pushedBitmap, toggle,
  clientData, helpString1, helpString2 );

  GdkPixmap *pixmap = bitmap.GetPixmap();

  GdkBitmap *mask = NULL;
  if (bitmap.GetMask()) mask = bitmap.GetMask()->GetBitmap();

  GtkWidget *tool_pixmap = gtk_pixmap_new( pixmap, mask );
  gtk_misc_set_alignment( GTK_MISC(tool_pixmap), 0.5, 0.5 );

  GtkToolbarChildType ctype = GTK_TOOLBAR_CHILD_BUTTON;
  if (toggle) ctype = GTK_TOOLBAR_CHILD_TOGGLEBUTTON;

  gtk_toolbar_append_element( m_toolbar,
    ctype, NULL, NULL, helpString1, "", tool_pixmap, (GtkSignalFunc)gtk_toolbar_callback, (gpointer)tool );

  m_tools.Append( tool );

  return tool;
};

void wxToolBar::AddSeparator(void)
{
  gtk_toolbar_append_space( m_toolbar );
};

void wxToolBar::ClearTools(void)
{
  wxFAIL_MSG("wxToolBar::ClearTools not implemented");
};

void wxToolBar::Realize(void)
{
  m_x = 0;
  m_y = 0;
  m_width = 100;
  m_height = 0;
  
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
    if (tool->m_bitmap1.Ok())
    {
      int tool_height = tool->m_bitmap1.GetHeight();
      if (tool_height > m_height) m_height = tool_height;
    };
    
    node = node->Next();
  };
  
  m_height += 10;
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
  wxFAIL_MSG("wxToolBar::ToggleTool not implemented");
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
  gtk_toolbar_set_space_size( m_toolbar, separation );
};

