/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "frame.h"
#endif

#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/control.h"
#include "wx/app.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/mdi.h"
#include "wx/gtk/win_gtk.h"

const wxMENU_HEIGHT    = 28;
const wxSTATUS_HEIGHT  = 25;

extern wxList wxTopLevelWindows;
extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// set size

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
// configure

gint gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxFrame *win )
{
  if (!win->HasVMT()) return FALSE;
  
  win->m_x = event->x;
  win->m_y = event->y;
  
  return FALSE;
};

//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_SIZE(wxFrame::OnSize)
  EVT_CLOSE(wxFrame::OnCloseWindow)
  EVT_IDLE(wxFrame::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

wxFrame::wxFrame()
{
  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;
  m_frameToolBar = NULL;
  m_sizeSet = FALSE;
  m_addPrivateChild = FALSE;
  m_wxwindow = NULL;
  m_mainWindow = NULL;
  wxTopLevelWindows.Insert( this );
};

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;
  m_frameToolBar = NULL;
  m_sizeSet = FALSE;
  m_addPrivateChild = FALSE;
  m_wxwindow = NULL;
  m_mainWindow = NULL;
  Create( parent, id, title, pos, size, style, name );
  wxTopLevelWindows.Insert( this );
};

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_needParent = FALSE;

  PreCreation( parent, id, pos, size, style, name );

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

  gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
    GTK_SIGNAL_FUNC(gtk_frame_size_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
    GTK_SIGNAL_FUNC(gtk_frame_configure_callback), (gpointer)this );
    
  PostCreation();

  gtk_widget_realize( m_mainWindow );

  return TRUE;
};

wxFrame::~wxFrame()
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

void wxFrame::OnCloseWindow( wxCloseEvent &event )
{
    if ( GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
};

bool wxFrame::Destroy()
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
    if (m_frameToolBar)
    {
      int y = 0;
      m_frameToolBar->GetSize( NULL, &y );
      (*height) -= y;
    }
  };
};

void wxFrame::SetClientSize( int const width, int const height )
{
  int h = height;
  if (m_frameMenuBar) h += wxMENU_HEIGHT;
  if (m_frameStatusBar) h += wxSTATUS_HEIGHT;
  if (m_frameToolBar)
  {
    int y = 0;
    m_frameToolBar->GetSize( NULL, &y );
    h += y;
  }
  wxWindow::SetClientSize( width, h );
};

void wxFrame::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y), int width, int height )
{
  // due to a bug in gtk, x,y are always 0
  // m_x = x;
  // m_y = y;

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
//  gtk_widget_set_usize( m_mainWindow, width, height );

  if (m_frameMenuBar)
  {
    main_y = wxMENU_HEIGHT;
    main_height -= wxMENU_HEIGHT;
  };

  int toolbar_height = 0;
  if (m_frameToolBar) m_frameToolBar->GetSize( NULL, &toolbar_height );
  
  main_y += toolbar_height;
  main_height -= toolbar_height;
  
  gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_wxwindow, main_x, main_y );
  gtk_widget_set_usize( m_wxwindow, main_width, main_height );

  if (m_frameMenuBar)
  {
    gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_frameMenuBar->m_widget, 1, 1 );
    gtk_widget_set_usize( m_frameMenuBar->m_widget, width-2, wxMENU_HEIGHT-2 );
  };

  if (m_frameToolBar)
  {
    gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_frameToolBar->m_widget, 1, wxMENU_HEIGHT );
    gtk_widget_set_usize( m_frameToolBar->m_widget, width-2, toolbar_height );
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
    // no child: go out !
    if (!GetChildren()->First())
      return;
      
    // do we have exactly one child?
    wxWindow *child = NULL;
    for(wxNode *node = GetChildren()->First(); node; node = node->Next())
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!IS_KIND_OF(win,wxFrame) && !IS_KIND_OF(win,wxDialog)
#if 0  // not in m_children anyway
          && (win != m_frameMenuBar) &&
             (win != m_frameToolBar) &&
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

void wxFrame::AddChild( wxWindow *child )
{
  // wxFrame and wxDialog as children aren't placed into the parents
  
  if (IS_KIND_OF(child,wxMDIChildFrame)) printf( "wxFrame::AddChild error.\n" );
  
  if ( IS_KIND_OF(child,wxFrame) || IS_KIND_OF(child,wxDialog))
  {
    m_children.Append( child );
    
    if ((child->m_x != -1) && (child->m_y != -1))
      gtk_widget_set_uposition( child->m_widget, child->m_x, child->m_y );
      
    return;
  }
  
  if (m_addPrivateChild)
  {
    gtk_myfixed_put( GTK_MYFIXED(m_mainWindow), child->m_widget, child->m_x, child->m_y );
      
    gtk_widget_set_usize( child->m_widget, child->m_width, child->m_height );
  }
  else
  {
    m_children.Append( child );
    
    if (m_wxwindow)
      gtk_myfixed_put( GTK_MYFIXED(m_wxwindow), child->m_widget, child->m_x, child->m_y );
      
    gtk_widget_set_usize( child->m_widget, child->m_width, child->m_height );
  }    
};

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

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
  m_frameMenuBar = menuBar;
  
  if (m_frameMenuBar)
  {
    wxNode *node = m_frameMenuBar->m_menus.First();
    while (node)
    {
      wxMenu *menu = (wxMenu*)node->Data();
      SetInvokingWindow( menu, this );
      node = node->Next();
    };
    
    if (m_frameMenuBar->m_parent != this)
    {
      m_frameMenuBar->m_parent = this;
      gtk_myfixed_put( GTK_MYFIXED(m_mainWindow),
        m_frameMenuBar->m_widget, m_frameMenuBar->m_x, m_frameMenuBar->m_y );
    }
  }
};

wxMenuBar *wxFrame::GetMenuBar(void)
{
  return m_frameMenuBar;
};

wxToolBar *wxFrame::CreateToolBar( long style , wxWindowID id, const wxString& name )
{
  m_addPrivateChild = TRUE;
  
  m_frameToolBar = new wxToolBar( this, id, wxDefaultPosition, wxDefaultSize, style, name );
  
  m_addPrivateChild = FALSE;
  
  return m_frameToolBar;
};

wxToolBar *wxFrame::GetToolBar(void)
{
  return m_frameToolBar;
};

wxStatusBar* wxFrame::CreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
  if (m_frameStatusBar)
  delete m_frameStatusBar;

  m_frameStatusBar = new wxStatusBar( this, id, wxPoint(0,0), wxSize(100,20), style, name );

  m_frameStatusBar->SetFieldsCount( number );
  
  return m_frameStatusBar;
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

void wxFrame::SetTitle( const wxString &title )
{
  m_title = title;
  gtk_window_set_title( GTK_WINDOW(m_widget), title );
};

void wxFrame::SetSizeHints(int minW, int minH, int maxW, int maxH, int WXUNUSED(incW) )
{
  if (m_wxwindow)
    gdk_window_set_hints( m_wxwindow->window, -1, -1, 
	                  minW, minH, maxW, maxH, GDK_HINT_MIN_SIZE | GDK_HINT_MIN_SIZE );
}

void wxFrame::SetIcon( const wxIcon &icon )
{
  m_icon = icon;
  if (!icon.Ok()) return;
  
  wxMask *mask = icon.GetMask();
  GdkBitmap *bm = NULL;
  if (mask) bm = mask->GetBitmap();
  
  gdk_window_set_icon( m_widget->window, NULL, icon.GetPixmap(), bm );
}

