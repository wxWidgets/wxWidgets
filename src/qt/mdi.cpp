/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"
#include "wx/dialog.h"
#include "wx/menu.h"

//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame,wxFrame)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
END_EVENT_TABLE()

wxMDIParentFrame::wxMDIParentFrame(void)
{
  m_clientWindow = NULL;
  m_currentChild = NULL;
  m_parentFrameActive = TRUE;
};

wxMDIParentFrame::wxMDIParentFrame( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  m_clientWindow = NULL;
  m_currentChild = NULL;
  m_parentFrameActive = TRUE;
  Create( parent, id, title, pos, size, style, name );
};

wxMDIParentFrame::~wxMDIParentFrame(void)
{
};

bool wxMDIParentFrame::Create( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  wxFrame::Create( parent, id, title, pos, size, style, name );
  
  OnCreateClient();
  
  return TRUE;
};

void wxMDIParentFrame::GetClientSize(int *width, int *height ) const
{
  wxFrame::GetClientSize( width, height );
};

wxMDIChildFrame *wxMDIParentFrame::GetActiveChild(void) const
{
  return m_currentChild;
};

wxMDIClientWindow *wxMDIParentFrame::GetClientWindow(void) const
{
  return m_clientWindow;
};

wxMDIClientWindow *wxMDIParentFrame::OnCreateClient(void)
{
  m_clientWindow = new wxMDIClientWindow( this );
  return m_clientWindow;
};

void wxMDIParentFrame::ActivateNext(void)
{
};

void wxMDIParentFrame::ActivatePrevious(void)
{
};

void wxMDIParentFrame::OnActivate( wxActivateEvent& WXUNUSED(event) )
{
};

void wxMDIParentFrame::OnSysColourChanged( wxSysColourChangedEvent& WXUNUSED(event) )
{
};

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxFrame)
  
BEGIN_EVENT_TABLE(wxMDIChildFrame, wxFrame)
  EVT_ACTIVATE(wxMDIChildFrame::OnActivate)
END_EVENT_TABLE()

wxMDIChildFrame::wxMDIChildFrame(void)
{
};

wxMDIChildFrame::wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  Create( parent, id, title, wxDefaultPosition, size, style, name );
};

wxMDIChildFrame::~wxMDIChildFrame(void)
{
};

bool wxMDIChildFrame::Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  m_title = title;
  return wxWindow::Create( parent->GetClientWindow(), id, wxDefaultPosition, size, style, name );
};

void wxMDIChildFrame::GetClientSize( int *width, int *height ) const
{
  wxWindow::GetClientSize( width, height );
}

void wxMDIChildFrame::AddChild( wxWindow *child )
{
  wxWindow::AddChild( child );
}
  
static void SetInvokingWindow( wxMenu *menu, wxWindow *win )
{
  menu->SetInvokingWindow( win );
  wxNode *node = menu->m_items.First();
  while (node)
  {
    wxMenuItem *menuitem = (wxMenuItem*)node->Data();
    if (menuitem->IsSubMenu())
      SetInvokingWindow( menuitem->GetSubMenu(), win );
    node = node->Next();
  };
};

void wxMDIChildFrame::SetMenuBar( wxMenuBar *menu_bar )
{
  m_menuBar = menu_bar;
  
  if (m_menuBar)
  {
    wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->m_parent;
    
    if (m_menuBar->m_parent != this)
    {
      wxNode *node = m_menuBar->m_menus.First();
      while (node)
      {
        wxMenu *menu = (wxMenu*)node->Data();
        SetInvokingWindow( menu, this );
        node = node->Next();
      };
      
      m_menuBar->m_parent = mdi_frame;
    }
    mdi_frame->SetMDIMenuBar( m_menuBar );

  }
};

wxMenuBar *wxMDIChildFrame::GetMenuBar()
{
  return m_menuBar;
};

void wxMDIChildFrame::Activate(void)
{
};

void wxMDIChildFrame::OnActivate( wxActivateEvent &WXUNUSED(event) )
{
};

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow,wxWindow)

wxMDIClientWindow::wxMDIClientWindow(void)
{
};

wxMDIClientWindow::wxMDIClientWindow( wxMDIParentFrame *parent, long style )
{
  CreateClient( parent, style );
};

wxMDIClientWindow::~wxMDIClientWindow(void)
{
};

bool wxMDIClientWindow::CreateClient( wxMDIParentFrame *WXUNUSED(parent), long WXUNUSED(style) )
{
  return TRUE;
};

void wxMDIClientWindow::AddChild( wxWindow *WXUNUSED(child) )
{
};


