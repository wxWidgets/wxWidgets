/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "frame.h"
#endif

#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/control.h"
#include "wx/app.h"
#include "wx/gtk/win_gtk.h"

const wxMENU_HEIGHT    = 28;
const wxSTATUS_HEIGHT  = 25;

extern wxList wxTopLevelWindows;
extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// size

void gtk_frame_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxFrame *win )
{
  if (!win->HasVMT()) return;

/*
  printf( "OnFrameResize from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( ".\n" );
*/

  win->GtkOnSize( alloc->x, alloc->y, alloc->width, alloc->height );
};

//-----------------------------------------------------------------------------
// delete

bool gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxFrame *win )
{
/*
  printf( "OnDelete from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( ".\n" );
*/

  win->Close();

  return TRUE;
};

//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_CLOSE(wxFrame::OnCloseWindow)
  EVT_SIZE(wxFrame::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

wxFrame::wxFrame(void)
{
  m_doingOnSize = FALSE;
  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;
  m_sizeSet = FALSE;
  wxTopLevelWindows.Insert( this );
};

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_sizeSet = FALSE;
  Create( parent, id, title, pos, size, style, name );
  wxTopLevelWindows.Insert( this );
};

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_needParent = FALSE;
  m_mainWindow = NULL;
  m_wxwindow = NULL;

  PreCreation( parent, id, pos, size, style, name );

  m_doingOnSize = FALSE;

  m_title = title;

  m_widget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  if ((size.x != -1) && (size.y != -1))
    gtk_widget_set_usize( m_widget, m_width, m_height );
  if ((pos.x != -1) && (pos.y != -1))
    gtk_widget_set_uposition( m_widget, m_x, m_y );

  gtk_window_set_title( GTK_WINDOW(m_widget), title );
  GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

  gtk_widget_set( m_widget, "GtkWindow::allow_shrink", TRUE, NULL);

  gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
    GTK_SIGNAL_FUNC(gtk_frame_delete_callback), (gpointer)this );

  m_mainWindow = gtk_myfixed_new();
  gtk_widget_show( m_mainWindow );
  GTK_WIDGET_UNSET_FLAGS( m_mainWindow, GTK_CAN_FOCUS );

  gtk_container_add( GTK_CONTAINER(m_widget), m_mainWindow );
  gtk_widget_set_uposition( m_mainWindow, 0, 0 );

  m_wxwindow = gtk_myfixed_new();
  gtk_widget_show( m_wxwindow );
  GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

  gtk_container_add( GTK_CONTAINER(m_mainWindow), m_wxwindow );

  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;

  gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
    GTK_SIGNAL_FUNC(gtk_frame_size_callback), (gpointer)this );

  PostCreation();

  gtk_widget_realize( m_mainWindow );

  return TRUE;
};

wxFrame::~wxFrame(void)
{
  if (m_frameMenuBar) delete m_frameMenuBar;
  if (m_frameStatusBar) delete m_frameStatusBar;

//  if (m_mainWindow) gtk_widget_destroy( m_mainWindow );

  wxTopLevelWindows.DeleteObject( this );
  if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
};

bool wxFrame::Show( bool show )
{
  if (show)
  {
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    m_sizeSet = FALSE;
    ProcessEvent( event );
  };
  return wxWindow::Show( show );
};

void wxFrame::Enable( bool enable )
{
  wxWindow::Enable( enable );
  gtk_widget_set_sensitive( m_mainWindow, enable );
};

void wxFrame::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
  this->Destroy();
};

bool wxFrame::Destroy(void)
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
}

void wxFrame::GetClientSize( int *width, int *height ) const
{
  wxWindow::GetClientSize( width, height );
  if (height)
  {
    if (m_frameMenuBar) (*height) -= wxMENU_HEIGHT;
    if (m_frameStatusBar) (*height) -= wxSTATUS_HEIGHT;
  };
};

void wxFrame::GtkOnSize( int x, int y, int width, int height )
{
  m_x = x;
  m_y = y;

  if ((m_height == height) && (m_width == width) &&
      (m_sizeSet)) return;
  if (!m_mainWindow) return;
  if (!m_wxwindow) return;

  m_width = width;
  m_height = height;

  gtk_widget_set_usize( m_widget, width, height );

  int main_x = 0;
  int main_y = 0;
  int main_height = height;
  int main_width = width;

  // This emulates Windows behaviour:
  // The menu bar is part of the main window, but the status bar
  // is on the implementation side in the client area. The
  // function GetClientSize returns the size of the client area
  // minus the status bar height. Under wxGTK, the main window
  // is represented by m_mainWindow. The menubar is inserted
  // into m_mainWindow whereas the statusbar is insertes into
  // m_wxwindow just like any other window.

//  not really needed
  gtk_widget_set_usize( m_mainWindow, width, height );

  if (m_frameMenuBar)
  {
    main_y = wxMENU_HEIGHT;
    main_height -= wxMENU_HEIGHT;
  };

  gtk_widget_set_uposition( GTK_WIDGET(m_wxwindow), main_x, main_y );
  gtk_widget_set_usize( GTK_WIDGET(m_wxwindow), main_width, main_height );

  if (m_frameMenuBar)
  {
    gtk_widget_set_uposition( m_frameMenuBar->m_widget, 1, 1 );
    gtk_widget_set_usize( m_frameMenuBar->m_widget, width-2, wxMENU_HEIGHT-2 );
  };

  if (m_frameStatusBar)
  {
    m_frameStatusBar->SetSize( 0, main_height-wxSTATUS_HEIGHT, width, wxSTATUS_HEIGHT );
  };

  m_sizeSet = TRUE;

  wxSizeEvent event( wxSize(m_width,m_height), GetId() );
  event.SetEventObject( this );
  ProcessEvent( event );
};

void wxFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  if ( GetAutoLayout() )
    Layout();
  else {
    // do we have exactly one child?
    wxWindow *child = NULL;
    for(wxNode *node = GetChildren()->First(); node; node = node->Next())
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!win->IsKindOf(CLASSINFO(wxFrame)) &&
          !win->IsKindOf(CLASSINFO(wxDialog))
#if 0 // not in m_children anyway
          && (win != m_frameMenuBar) &&
             (win != m_frameStatusBar)
#endif
         )
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

void SetInvokingWindow( wxMenu *menu, wxWindow *win )
{
  menu->SetInvokingWindow( win );
  wxNode *node = menu->m_items.First();
  while (node)
  {
    wxMenuItem *menuitem = (wxMenuItem*)node->Data();
    if (menuitem->m_isSubMenu) SetInvokingWindow( menuitem->m_subMenu, win );
    node = node->Next();
  };
};

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
  m_frameMenuBar = menuBar;

  wxNode *node = m_frameMenuBar->m_menus.First();
  while (node)
  {
    wxMenu *menu = (wxMenu*)node->Data();
    SetInvokingWindow( menu, this );
    node = node->Next();
  };

  m_frameMenuBar->m_parent = this;
  gtk_myfixed_put( GTK_MYFIXED(m_mainWindow),
    m_frameMenuBar->m_widget, m_frameMenuBar->m_x, m_frameMenuBar->m_y );
};

bool wxFrame::CreateStatusBar( int number )
{
  if (m_frameStatusBar)
  delete m_frameStatusBar;

  m_frameStatusBar = new wxStatusBar( this, -1, wxPoint(0,0), wxSize(100,20) );

  m_frameStatusBar->SetFieldsCount( number );
  return TRUE;
};

void wxFrame::SetStatusText( const wxString &text, int number )
{
  if (m_frameStatusBar) m_frameStatusBar->SetStatusText( text, number );
};

void wxFrame::SetStatusWidths( int n, int *width )
{
  if (m_frameStatusBar) m_frameStatusBar->SetStatusWidths( n, width );
};

wxStatusBar *wxFrame::GetStatusBar(void)
{
  return m_frameStatusBar;
};

wxMenuBar *wxFrame::GetMenuBar(void)
{
  return m_frameMenuBar;
};

void wxFrame::SetTitle( const wxString &title )
{
  m_title = title;
  gtk_window_set_title( GTK_WINDOW(m_widget), title );
};

wxString wxFrame::GetTitle(void) const
{
  return (wxString&)m_title;
};

