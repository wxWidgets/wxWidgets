/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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
#include "wx/dcclient.h"
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const int wxMENU_HEIGHT    = 30;
const int wxSTATUS_HEIGHT  = 25;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern wxList wxTopLevelWindows;
extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_frame_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxFrame *win )
{
  if (!win->HasVMT()) return;

/*
  printf( "OnFrameResize from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( ".\n" );
*/

  win->GtkOnSize( alloc->x, alloc->y, alloc->width, alloc->height );
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static gint gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxFrame *win )
{
/*
  printf( "OnDelete from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( ".\n" );
*/

  win->Close();

  return TRUE;
}

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxFrame *win )
{
  if (!win->HasVMT()) return FALSE;
  
  win->m_x = event->x;
  win->m_y = event->y;
  
  return FALSE;
}

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_SIZE(wxFrame::OnSize)
  EVT_CLOSE(wxFrame::OnCloseWindow)
  EVT_IDLE(wxFrame::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

wxFrame::wxFrame()
{
  m_frameMenuBar = (wxMenuBar *) NULL;
  m_frameStatusBar = (wxStatusBar *) NULL;
  m_frameToolBar = (wxToolBar *) NULL;
  m_sizeSet = FALSE;
  m_addPrivateChild = FALSE;
  m_wxwindow = (GtkWidget *) NULL;
  m_mainWindow = (GtkWidget *) NULL;
  wxTopLevelWindows.Insert( this );
}

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_frameMenuBar = (wxMenuBar *) NULL;
  m_frameStatusBar = (wxStatusBar *) NULL;
  m_frameToolBar = (wxToolBar *) NULL;
  m_sizeSet = FALSE;
  m_addPrivateChild = FALSE;
  m_wxwindow = (GtkWidget *) NULL;
  m_mainWindow = (GtkWidget *) NULL;
  Create( parent, id, title, pos, size, style, name );
  wxTopLevelWindows.Insert( this );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_needParent = FALSE;

  PreCreation( parent, id, pos, size, style, name );

  m_title = title;

  GtkWindowType win_type = GTK_WINDOW_TOPLEVEL;
  if (style & wxSIMPLE_BORDER) win_type = GTK_WINDOW_POPUP;
  
  m_widget = gtk_window_new( win_type );
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
}

wxFrame::~wxFrame()
{
  if (m_frameMenuBar) delete m_frameMenuBar;
  if (m_frameStatusBar) delete m_frameStatusBar;
  if (m_frameToolBar) delete m_frameToolBar;

//  if (m_mainWindow) gtk_widget_destroy( m_mainWindow );

  wxTopLevelWindows.DeleteObject( this );
  if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
}

bool wxFrame::Show( bool show )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  if (show)
  {
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    m_sizeSet = FALSE;
    ProcessEvent( event );
  }
  return wxWindow::Show( show );
}

void wxFrame::Enable( bool enable )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  if (!m_mainWindow) return;
  
  wxWindow::Enable( enable );
  gtk_widget_set_sensitive( m_mainWindow, enable );
}

void wxFrame::OnCloseWindow( wxCloseEvent &event )
{
  if (GetEventHandler()->OnClose() || event.GetForce()) this->Destroy();
}

bool wxFrame::Destroy()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
}

void wxFrame::ImplementSetPosition(void)
{
  if ((m_x != -1) || (m_y != -1))
     gtk_widget_set_uposition( m_widget, m_x, m_y );
}

void wxFrame::Centre( int direction )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  if (direction & wxHORIZONTAL == wxHORIZONTAL) m_x = (gdk_screen_width () - m_width) / 2;
  if (direction & wxVERTICAL == wxVERTICAL) m_y = (gdk_screen_height () - m_height) / 2;
  ImplementSetPosition();
}

void wxFrame::GetClientSize( int *width, int *height ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  wxWindow::GetClientSize( width, height );
  if (height)
  {
    if (m_frameMenuBar) (*height) -= wxMENU_HEIGHT;
    if (m_frameStatusBar) (*height) -= wxSTATUS_HEIGHT;
    if (m_frameToolBar)
    {
      int y = 0;
      m_frameToolBar->GetSize( (int *) NULL, &y );
      (*height) -= y;
    }
  }
}

void wxFrame::SetClientSize( int const width, int const height )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  int h = height;
  if (m_frameMenuBar) h += wxMENU_HEIGHT;
  if (m_frameStatusBar) h += wxSTATUS_HEIGHT;
  if (m_frameToolBar)
  {
    int y = 0;
    m_frameToolBar->GetSize( (int *) NULL, &y );
    h += y;
  }
  wxWindow::SetClientSize( width, h );
}

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
  if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
  if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
  if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_minWidth;
  if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_minHeight;

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
  }

  int toolbar_height = 0;
  if (m_frameToolBar) m_frameToolBar->GetSize( (int *) NULL, &toolbar_height );
  
  main_y += toolbar_height;
  main_height -= toolbar_height;
  
  gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_wxwindow, main_x, main_y );
  gtk_widget_set_usize( m_wxwindow, main_width, main_height );

  if (m_frameMenuBar)
  {
    gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_frameMenuBar->m_widget, 1, 1 );
    gtk_widget_set_usize( m_frameMenuBar->m_widget, width-2, wxMENU_HEIGHT-2 );
  }

  if (m_frameToolBar)
  {
    gtk_myfixed_move( GTK_MYFIXED(m_mainWindow), m_frameToolBar->m_widget, 1, wxMENU_HEIGHT );
    gtk_widget_set_usize( m_frameToolBar->m_widget, width-2, toolbar_height );
  }
  
  if (m_frameStatusBar)
  {
    m_frameStatusBar->SetSize( 0, main_height-wxSTATUS_HEIGHT, width, wxSTATUS_HEIGHT );
  }

  m_sizeSet = TRUE;

  wxSizeEvent event( wxSize(m_width,m_height), GetId() );
  event.SetEventObject( this );
  ProcessEvent( event );
}

void wxFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  if ( GetAutoLayout() )
    Layout();
  else {
    // no child: go out !
    if (!GetChildren()->First())
      return;
      
    // do we have exactly one child?
    wxWindow *child = (wxWindow *) NULL;
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
      }
    }

    // yes: set it's size to fill all the frame
    int client_x, client_y;
    GetClientSize(&client_x, &client_y);
    child->SetSize( 1, 1, client_x-2, client_y);
  }
}

void wxFrame::AddChild( wxWindow *child )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  wxASSERT_MSG( (m_wxwindow != NULL), "invalid frame" );
  wxASSERT_MSG( (m_mainWindow != NULL), "invalid frame" );
  wxASSERT_MSG( (child != NULL),  "invalid child" );
  wxASSERT_MSG( (child->m_widget != NULL), "invalid child" );
  
  // wxFrame and wxDialog as children aren't placed into the parents
  
  if (IS_KIND_OF(child,wxMDIChildFrame)) wxFAIL_MSG( "wxFrame::AddChild error.\n" );
  
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

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  wxASSERT_MSG( (m_wxwindow != NULL), "invalid frame" );
  wxASSERT_MSG( (m_mainWindow != NULL), "invalid frame" );
  
  m_frameMenuBar = menuBar;
  
  if (m_frameMenuBar)
  {
    wxNode *node = m_frameMenuBar->m_menus.First();
    while (node)
    {
      wxMenu *menu = (wxMenu*)node->Data();
      SetInvokingWindow( menu, this );
      node = node->Next();
    }
    
    if (m_frameMenuBar->m_parent != this)
    {
      m_frameMenuBar->m_parent = this;
      gtk_myfixed_put( GTK_MYFIXED(m_mainWindow),
        m_frameMenuBar->m_widget, m_frameMenuBar->m_x, m_frameMenuBar->m_y );
    }
  }
}

wxMenuBar *wxFrame::GetMenuBar(void) const
{
  return m_frameMenuBar;
}

wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  wxCHECK_MSG( m_frameToolBar == NULL, FALSE, "recreating toolbar in wxFrame" );

  m_addPrivateChild = TRUE;
  m_frameToolBar = OnCreateToolBar( style, id, name );
  m_addPrivateChild = FALSE;
  
  return m_frameToolBar;
}

wxToolBar* wxFrame::OnCreateToolBar( long style, wxWindowID id, const wxString& name )
{
  return new wxToolBar( this, id, wxDefaultPosition, wxDefaultSize, style, name );
}

wxToolBar *wxFrame::GetToolBar(void) const 
{ 
  return m_frameToolBar; 
}

wxStatusBar* wxFrame::CreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  wxCHECK_MSG( m_frameStatusBar == NULL, FALSE, "recreating status bar in wxFrame" );

  m_frameStatusBar = OnCreateStatusBar( number, style, id, name );
  
  return m_frameStatusBar;
}

wxStatusBar *wxFrame::OnCreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
  wxStatusBar *statusBar = (wxStatusBar *) NULL;
    
  statusBar = new wxStatusBar(this, id, wxPoint(0, 0), wxSize(100, 20), style, name);
    
  // Set the height according to the font and the border size
  wxClientDC dc(statusBar);
  dc.SetFont( *statusBar->GetFont() );

  long x, y;
  dc.GetTextExtent( "X", &x, &y );

  int height = (int)( (y  * 1.1) + 2* statusBar->GetBorderY());

  statusBar->SetSize( -1, -1, 100, height );

  statusBar->SetFieldsCount( number );
  return statusBar;
}

void wxFrame::SetStatusText(const wxString& text, int number)
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set text for" );

  m_frameStatusBar->SetStatusText(text, number);
}

void wxFrame::SetStatusWidths(int n, const int widths_field[] )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set widths for" );

  m_frameStatusBar->SetStatusWidths(n, widths_field);
}

wxStatusBar *wxFrame::GetStatusBar(void) const
{
  return m_frameStatusBar;
}

void wxFrame::SetTitle( const wxString &title )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  m_title = title;
  if (m_title.IsNull()) m_title = "";
  gtk_window_set_title( GTK_WINDOW(m_widget), title );
}

void wxFrame::SetIcon( const wxIcon &icon )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
  m_icon = icon;
  if (!icon.Ok()) return;
  
  wxMask *mask = icon.GetMask();
  GdkBitmap *bm = (GdkBitmap *) NULL;
  if (mask) bm = mask->GetBitmap();
  
  gdk_window_set_icon( m_widget->window, (GdkWindow *) NULL, icon.GetPixmap(), bm );
}
