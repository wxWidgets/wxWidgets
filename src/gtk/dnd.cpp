///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget class
// Author:      Robert Roebling
// Copyright:   Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"
#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"

#include "gdk/gdkprivate.h"

#include <X11/Xlib.h>

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
};

wxDropTarget::~wxDropTarget()
{
};

void wxDropTarget::Drop( GdkEvent *event, int x, int y )
{
  printf( "Drop data is of type %s.\n", event->dropdataavailable.data_type );
  
  OnDrop( x, y, (char *)event->dropdataavailable.data);
};

void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
  gtk_widget_dnd_drop_set( widget, FALSE, NULL, 0, FALSE );
};

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------

bool wxTextDropTarget::OnDrop( long x, long y, const void *pData )
{
  OnDropText( x, y, (const char*)pData );
  return TRUE;
};

bool wxTextDropTarget::OnDropText( long x, long y, const char *psz )
{
  printf( "Got dropped text: %s.\n", psz );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
};

void wxTextDropTarget::RegisterWidget( GtkWidget *widget )
{
  char *accepted_drop_types[] = { "text/plain" };
  gtk_widget_dnd_drop_set( widget, TRUE, accepted_drop_types, 1, FALSE );
};

//-------------------------------------------------------------------------
// wxDragSource
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drag request

void gtk_drag_callback( GtkWidget *widget, GdkEvent *event, wxDragSource *drag )
{
  printf( "OnDragRequest.\n" );
  
  gtk_widget_dnd_data_set( widget, event, drag->m_data, drag->m_size );
};

wxDragSource::wxDragSource( wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;

  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
 
  m_data = NULL;
  m_size = 0;
  
  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
};

wxDragSource::~wxDragSource(void)
{
  g_blockEventsOnDrag = FALSE;
};
   
void wxDragSource::SetData( char *data, long size )
{
  m_size = size;
  m_data = data;
};

void wxDragSource::Start( int x, int y )
{
  if (gdk_dnd.dnd_grabbed) return;
  if (gdk_dnd.drag_really) return;
  if (m_size == 0) return;
  if (!m_data) return;
  
  GdkWindowPrivate *wp = (GdkWindowPrivate*) m_widget->window;
  
  RegisterWindow();
  ConnectWindow();
  
  gdk_dnd.drag_perhaps = TRUE;

  gdk_dnd.dnd_drag_start.x = 5;
  gdk_dnd.dnd_drag_start.y = 5;
  gdk_dnd.real_sw = wp;
	  
  if (gdk_dnd.drag_startwindows)
  {
    g_free( gdk_dnd.drag_startwindows );
    gdk_dnd.drag_startwindows = NULL;
  };
  gdk_dnd.drag_numwindows = gdk_dnd.drag_really = 0;
  
  XWindowAttributes dnd_winattr;
  XGetWindowAttributes( gdk_display, wp->xwindow, &dnd_winattr );
  wp->dnd_drag_savedeventmask = dnd_winattr.your_event_mask;
  
  gdk_dnd_drag_addwindow( m_widget->window );
  
  GdkEventDragBegin ev;
  ev.type = GDK_DRAG_BEGIN;
  ev.window = m_widget->window;
  ev.u.allflags = 0;
  ev.u.flags.protocol_version = DND_PROTOCOL_VERSION;
  
  gdk_event_put( (GdkEvent*)&ev );
  
  XGrabPointer( gdk_display, wp->xwindow, False, 
                ButtonMotionMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		GrabModeAsync, GrabModeAsync, gdk_root_window, None, CurrentTime );
		
  gdk_dnd_set_drag_cursors( m_defaultCursor.GetCursor(), m_goaheadCursor.GetCursor() );
  
  gdk_dnd.dnd_grabbed = TRUE;
  gdk_dnd.drag_really = 1;
  gdk_dnd_display_drag_cursor( x, y, FALSE, TRUE );
  
  while (gdk_dnd.drag_really || gdk_dnd.drag_perhaps) wxYield();
  
  UnconnectWindow();
  UnregisterWindow();
};

void wxDragSource::ConnectWindow(void)
{
  gtk_signal_connect( GTK_OBJECT(m_widget), "drag_request_event",
    GTK_SIGNAL_FUNC(gtk_drag_callback), (gpointer)this );
};

void wxDragSource::UnconnectWindow(void)
{
  if (!m_widget) return;
  
  gtk_signal_disconnect_by_data( GTK_OBJECT(m_widget), (gpointer)this );
};

void wxDragSource::UnregisterWindow(void)
{
  if (!m_widget) return;
  
  gtk_widget_dnd_drag_set( m_widget, FALSE, NULL, 0 );
};
  
//-------------------------------------------------------------------------
// wxTextDragSource
//-------------------------------------------------------------------------

void wxTextDragSource::SetTextData( const wxString &text )
{
  m_tmp = text;
  SetData( WXSTRINGCAST(m_tmp), m_tmp.Length()+1 );
};

void wxTextDragSource::RegisterWindow(void)
{
  if (!m_widget) return;
  
  char *accepted_drop_types[] = { "text/plain" };
  gtk_widget_dnd_drag_set( m_widget, TRUE, accepted_drop_types, 1 );
};

