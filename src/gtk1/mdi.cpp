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

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame,wxFrame)

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

void wxMDIParentFrame::OnSize( wxSizeEvent& event )
{
  wxFrame::OnSize( event );
};

void wxMDIParentFrame::OnActivate( wxActivateEvent& WXUNUSED(event) )
{
};

void wxMDIParentFrame::SetMenuBar( wxMenuBar *menu_bar )
{
  wxFrame::SetMenuBar( menu_bar );
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

void wxMDIParentFrame::OnSysColourChanged( wxSysColourChangedEvent& WXUNUSED(event) )
{
};

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxPanel)
  
wxMDIChildFrame::wxMDIChildFrame(void)
{
};

wxMDIChildFrame::wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  Create( parent, id, title, pos, size, style, name );
};

wxMDIChildFrame::~wxMDIChildFrame(void)
{
};

bool wxMDIChildFrame::Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  m_title = title;
  return wxPanel::Create( parent->GetClientWindow(), id, pos, size, style, name );
};

void wxMDIChildFrame::SetMenuBar( wxMenuBar *WXUNUSED(menu_bar) )
{
};

void wxMDIChildFrame::Activate(void)
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

bool wxMDIClientWindow::CreateClient( wxMDIParentFrame *parent, long style )
{
  m_needParent = TRUE;
  
  PreCreation( parent, -1, wxPoint(10,10), wxSize(100,100), style, "wxMDIClientWindow" );

  m_widget = gtk_notebook_new();
  
  PostCreation();
  
  Show( TRUE );
  
  return TRUE;
};

void wxMDIClientWindow::AddChild( wxWindow *child )
{
  m_children.Append( child );
  
  wxString s;

  if (child->IsKindOf(CLASSINFO(wxMDIChildFrame)))
  {
    wxMDIChildFrame* mdi_child = (wxMDIChildFrame*) child;
    s = mdi_child->m_title;
  };
  
  if (s.IsNull()) s = "MDI child";
  
  GtkWidget *label_widget;
  label_widget = gtk_label_new( s );
  gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );

  gtk_notebook_append_page( GTK_NOTEBOOK(m_widget), child->m_widget, label_widget );
};


