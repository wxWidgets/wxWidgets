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
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{ 
  if ((win->m_x == alloc->x) &&
      (win->m_y == alloc->y) &&
      (win->m_width == alloc->width) &&
      (win->m_height == alloc->height))
  {
    return;
  };
  
  win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );
};

// page change callback
static void gtk_page_change_callback( GtkNotebook *WXUNUSED(widget),
                                      GtkNotebookPage *page,
                                      gint WXUNUSED(nPage),
                                      wxMDIClientWindow *client_win )
{
  wxNode *node = client_win->m_children.First();
  while (node)
  {
    wxMDIChildFrame *child_frame = (wxMDIChildFrame *)node->Data();
    if (child_frame->m_page == page)
    {
      wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)client_win->m_parent;
      mdi_frame->m_currentChild = child_frame;
      mdi_frame->SetMDIMenuBar( child_frame->m_menuBar );
      return;
    };
    node = node->Next();
  }
}

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

void wxMDIParentFrame::GtkOnSize( int x, int y, int width, int height )
{
  wxFrame::GtkOnSize( x, y, width, height );
  
  if (m_mdiMenuBar)
  {
    int x = 0;
    int y = 0;
    GetClientSize( &x, &y );
    m_mdiMenuBar->SetSize( 1, 1, x-2, 26 ); 
  }
};

void wxMDIParentFrame::SetMDIMenuBar( wxMenuBar *menu_bar )
{
  if (m_mdiMenuBar) m_mdiMenuBar->Show( FALSE );
  m_mdiMenuBar = menu_bar;
  if (m_mdiMenuBar)
  {
    int x = 0;
    int y = 0;
    GetClientSize( &x, &y );
    m_mdiMenuBar->SetSize( 1, 1, x-2, 26 ); 
    m_mdiMenuBar->Show( TRUE );
  }
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
  if (m_clientWindow)
    gtk_notebook_next_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
};

void wxMDIParentFrame::ActivatePrevious(void)
{
  if (m_clientWindow)
    gtk_notebook_prev_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
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

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxPanel)
  
BEGIN_EVENT_TABLE(wxMDIChildFrame, wxPanel)
  EVT_CLOSE(wxMDIChildFrame::OnCloseWindow)
  EVT_SIZE(wxMDIChildFrame::OnSize)
  EVT_ACTIVATE(wxMDIChildFrame::OnActivate)
END_EVENT_TABLE()

wxMDIChildFrame::wxMDIChildFrame(void)
{
  m_menuBar = NULL;
  m_page = NULL;
};

wxMDIChildFrame::wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  m_menuBar = NULL;
  m_page = NULL;
  Create( parent, id, title, wxDefaultPosition, size, style, name );
};

wxMDIChildFrame::~wxMDIChildFrame(void)
{
  if (m_menuBar)
  {
    wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->m_parent;
    if (mdi_frame->m_currentChild == this)
    {
      mdi_frame->SetMDIMenuBar( NULL );
      mdi_frame->m_currentChild = NULL;
    };
    delete m_menuBar;
  }
};

bool wxMDIChildFrame::Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  m_title = title;
  return wxPanel::Create( parent->GetClientWindow(), id, wxDefaultPosition, size, style, name );
};

void wxMDIChildFrame::OnCloseWindow( wxCloseEvent &event )
{
    if ( GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
};

void wxMDIChildFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  if ( GetAutoLayout() )
    Layout();
  else {
    // no child: go out !
    if (!GetChildren()->First())
      return;
      
    // do we have exactly one child?
    wxWindow *child = NULL;
    for(wxNode *node = GetChildren()->First(); node; node = node->Next())
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!IS_KIND_OF(win,wxFrame) && !IS_KIND_OF(win,wxDialog))
      {
        if ( child )  // it's the second one: do nothing
          return;

        child = win;
      };
    };

    // yes: set it's size to fill all the frame
    int client_x, client_y;
    GetClientSize(&client_x, &client_y);
    child->SetSize( 1, 1, client_x-2, client_y);
  }
};

bool wxMDIChildFrame::Destroy(void)
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
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

    gtk_myfixed_put( GTK_MYFIXED(mdi_frame->m_mainWindow),
      m_menuBar->m_widget, m_menuBar->m_x, m_menuBar->m_y );
  }
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

bool wxMDIClientWindow::CreateClient( wxMDIParentFrame *parent, long style )
{
  m_needParent = TRUE;
  
  PreCreation( parent, -1, wxPoint(10,10), wxSize(100,100), style, "wxMDIClientWindow" );

  m_widget = gtk_notebook_new();
  
  gtk_signal_connect( GTK_OBJECT(m_widget), "switch_page",
                      GTK_SIGNAL_FUNC(gtk_page_change_callback), (gpointer)this );
		  
  gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );
  
  PostCreation();
  
  Show( TRUE );
  
  return TRUE;
};

void wxMDIClientWindow::AddChild( wxWindow *child )
{
  if (!child->IsKindOf(CLASSINFO(wxMDIChildFrame)))
  {
     wxFAIL_MSG("wxNotebook::AddChild: Child has to be wxMDIChildFrame");
     return;
  };
  
  m_children.Append( child );
  
  wxString s;
  wxMDIChildFrame* mdi_child = (wxMDIChildFrame*) child;
  s = mdi_child->m_title;
  if (s.IsNull()) s = "MDI child";
  
  GtkWidget *label_widget;
  label_widget = gtk_label_new( s );
  gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );
  
  gtk_signal_connect( GTK_OBJECT(child->m_widget), "size_allocate",
    GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)child );
    
  gtk_notebook_append_page( GTK_NOTEBOOK(m_widget), child->m_widget, label_widget );
  
  mdi_child->m_page = (GtkNotebookPage*) (g_list_last(GTK_NOTEBOOK(m_widget)->children)->data);
    
  gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), m_children.Number()-1 );
  
  gtk_page_change_callback( NULL, mdi_child->m_page, 0, this );
};


