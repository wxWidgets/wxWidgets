/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"
#include "wx/dialog.h"
#include "wx/menu.h"
#include "wx/gtk/win_gtk.h"
#include <wx/intl.h>

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const int wxMENU_HEIGHT    = 30;

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{
  if ((win->m_x == alloc->x) &&
      (win->m_y == alloc->y) &&
      (win->m_width == alloc->width) &&
      (win->m_height == alloc->height))
  {
    return;
  }

  win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );
}

//-----------------------------------------------------------------------------
// page change callback
//-----------------------------------------------------------------------------

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
    }
    node = node->Next();
  }
}

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame,wxFrame)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
END_EVENT_TABLE()

wxMDIParentFrame::wxMDIParentFrame(void)
{
  m_clientWindow = (wxMDIClientWindow *) NULL;
  m_currentChild = (wxMDIChildFrame *) NULL;
  m_parentFrameActive = TRUE;
}

wxMDIParentFrame::wxMDIParentFrame( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  m_clientWindow = (wxMDIClientWindow *) NULL;
  m_currentChild = (wxMDIChildFrame *) NULL;
  m_parentFrameActive = TRUE;
  Create( parent, id, title, pos, size, style, name );
}

wxMDIParentFrame::~wxMDIParentFrame(void)
{
}

bool wxMDIParentFrame::Create( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  wxFrame::Create( parent, id, title, pos, size, style, name );

  OnCreateClient();

  return TRUE;
}

void wxMDIParentFrame::GtkOnSize( int x, int y, int width, int height )
{
  wxFrame::GtkOnSize( x, y, width, height );

  if (m_mdiMenuBar)
  {
    m_mdiMenuBar->m_x = 1;  
    m_mdiMenuBar->m_y = 1;
    m_mdiMenuBar->m_width = m_width-2;
    m_mdiMenuBar->m_height = wxMENU_HEIGHT-2;
    gtk_myfixed_move( GTK_MYFIXED(m_wxwindow), m_mdiMenuBar->m_widget, 1, 1 );
    gtk_widget_set_usize( m_mdiMenuBar->m_widget, m_width-2, wxMENU_HEIGHT-2 );
  }
}

void wxMDIParentFrame::SetMDIMenuBar( wxMenuBar *menu_bar )
{
  if (m_mdiMenuBar) m_mdiMenuBar->Show( FALSE );
  m_mdiMenuBar = menu_bar;
  if (m_mdiMenuBar)
  {
    m_mdiMenuBar->m_x = 1;  
    m_mdiMenuBar->m_y = 1;
    m_mdiMenuBar->m_width = m_width-2;
    m_mdiMenuBar->m_height = wxMENU_HEIGHT-2;
    gtk_myfixed_move( GTK_MYFIXED(m_wxwindow), m_mdiMenuBar->m_widget, 1, 1 );
    gtk_widget_set_usize( m_mdiMenuBar->m_widget, m_width-2, wxMENU_HEIGHT-2 );
    m_mdiMenuBar->Show( TRUE );
  }
}

void wxMDIParentFrame::GetClientSize(int *width, int *height ) const
{
  wxFrame::GetClientSize( width, height );
}

wxMDIChildFrame *wxMDIParentFrame::GetActiveChild(void) const
{
  return m_currentChild;
}

wxMDIClientWindow *wxMDIParentFrame::GetClientWindow(void) const
{
  return m_clientWindow;
}

wxMDIClientWindow *wxMDIParentFrame::OnCreateClient(void)
{
  m_clientWindow = new wxMDIClientWindow( this );
  return m_clientWindow;
}

void wxMDIParentFrame::ActivateNext(void)
{
  if (m_clientWindow)
    gtk_notebook_next_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
}

void wxMDIParentFrame::ActivatePrevious(void)
{
  if (m_clientWindow)
    gtk_notebook_prev_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
}

void wxMDIParentFrame::OnActivate( wxActivateEvent& WXUNUSED(event) )
{
}

void wxMDIParentFrame::OnSysColourChanged( wxSysColourChangedEvent& WXUNUSED(event) )
{
}

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxFrame)
  
BEGIN_EVENT_TABLE(wxMDIChildFrame, wxFrame)
  EVT_ACTIVATE(wxMDIChildFrame::OnActivate)
END_EVENT_TABLE()

wxMDIChildFrame::wxMDIChildFrame(void)
{
  m_menuBar = (wxMenuBar *) NULL;
  m_page = (GtkNotebookPage *) NULL;
}

wxMDIChildFrame::wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  m_menuBar = (wxMenuBar *) NULL;
  m_page = (GtkNotebookPage *) NULL;
  Create( parent, id, title, wxDefaultPosition, size, style, name );
}

wxMDIChildFrame::~wxMDIChildFrame(void)
{
  if (m_menuBar)
  {
    wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->m_parent;
    if (mdi_frame->m_currentChild == this)
    {
      mdi_frame->SetMDIMenuBar( (wxMenuBar *) NULL );
      mdi_frame->m_currentChild = (wxMDIChildFrame *) NULL;
    }
    delete m_menuBar;
  }
}

bool wxMDIChildFrame::Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
  m_title = title;
  
  return wxWindow::Create( parent->GetClientWindow(), id, wxDefaultPosition, size, style, name );
}

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
  }
}

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
      }

      m_menuBar->m_parent = mdi_frame;
    }
    mdi_frame->SetMDIMenuBar( m_menuBar );

    gtk_myfixed_put( GTK_MYFIXED(mdi_frame->m_wxwindow),
      m_menuBar->m_widget, m_menuBar->m_x, m_menuBar->m_y );
  }
}

wxMenuBar *wxMDIChildFrame::GetMenuBar()
{
  return m_menuBar;
}

void wxMDIChildFrame::Activate(void)
{
}

void wxMDIChildFrame::OnActivate( wxActivateEvent &WXUNUSED(event) )
{
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxMDIClientWindow
//-----------------------------------------------------------------------------

static void wxInsertChildInMDI( wxMDIClientWindow* parent, wxMDIChildFrame* child )
{
  wxString s = child->m_title;
  if (s.IsNull()) s = _("MDI child");

  GtkWidget *label_widget = gtk_label_new( s );
  gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );

  gtk_signal_connect( GTK_OBJECT(child->m_widget), "size_allocate",
    GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)child );
    
  GtkNotebook *notebook = GTK_NOTEBOOK(parent->m_widget);
    
  gtk_notebook_append_page( notebook, child->m_widget, label_widget );

  child->m_page = (GtkNotebookPage*) (g_list_last(notebook->children)->data);

  gtk_notebook_set_page( notebook, parent->m_children.Number()-1 );
  
  gtk_page_change_callback( (GtkNotebook *) NULL, child->m_page, 0, parent );
}

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow,wxWindow)

wxMDIClientWindow::wxMDIClientWindow(void)
{
}

wxMDIClientWindow::wxMDIClientWindow( wxMDIParentFrame *parent, long style )
{
  CreateClient( parent, style );
}

wxMDIClientWindow::~wxMDIClientWindow(void)
{
}

bool wxMDIClientWindow::CreateClient( wxMDIParentFrame *parent, long style )
{
  m_needParent = TRUE;
  
  m_insertCallback = (wxInsertChildFunction)wxInsertChildInMDI;

  PreCreation( parent, -1, wxPoint(10,10), wxSize(100,100), style, "wxMDIClientWindow" );

  m_widget = gtk_notebook_new();

  gtk_signal_connect( GTK_OBJECT(m_widget), "switch_page",
                      GTK_SIGNAL_FUNC(gtk_page_change_callback), (gpointer)this );

  gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

  gtk_myfixed_put( GTK_MYFIXED(m_parent->m_wxwindow), m_widget, m_x, m_y );
  
  PostCreation();

  Show( TRUE );

  return TRUE;
}



