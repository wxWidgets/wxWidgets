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
  if (!widget) return;
  
  gtk_widget_dnd_drop_set( widget, FALSE, NULL, 0, FALSE );
};

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
  wxString formats;
  int valid = 0;
  
  for ( uint i = 0; i < GetFormatCount(); i++ )
  {
    wxDataFormat df = GetFormat( i );
    switch (df) 
    {
      case wxDF_TEXT:
	if (i > 0) formats += ";";
        formats += "text/plain";
	valid++;
	break;
      case wxDF_FILENAME:
	if (i > 0) formats += ";";
        formats += "url:any";
	valid++;
	break;
      default:
        break;
    };
  }
  
  char *str = WXSTRINGCAST formats;
  
  gtk_widget_dnd_drop_set( widget, TRUE, &str, valid, FALSE );
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

size_t wxTextDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxTextDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_TEXT;
}

// ----------------------------------------------------------------------------
// wxFileDropTarget
// ----------------------------------------------------------------------------

bool wxFileDropTarget::OnDropFiles( long x, long y, size_t nFiles, const char * const WXUNUSED(aszFiles)[] )
{
  printf( "Got %d dropped files.\n", (int)nFiles );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
}

bool wxFileDropTarget::OnDrop(long x, long y, const void *WXUNUSED(pData) )
{
  char *str = "/this/is/a/path.txt";

  return OnDropFiles(x, y, 1, &str ); 
}

size_t wxFileDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxFileDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_FILENAME;
}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drag request

void gtk_drag_callback( GtkWidget *widget, GdkEvent *event, wxDataObject *data )
{
  printf( "Data requested for dropping.\n" );
  
  uint size = data->GetDataSize();
  char *ptr = new char[size];
  data->GetDataHere( ptr );
  
  gtk_widget_dnd_data_set( widget, event, ptr, size );
  
  delete ptr;
};

wxDropSource::wxDropSource( wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
  m_data = NULL;  

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
};

wxDropSource::wxDropSource( wxDataObject &data, wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
  m_data = &data;

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
};

void wxDropSource::SetData( wxDataObject &data )
{
  m_data = &data;  
};

wxDropSource::~wxDropSource(void)
{
//  if (m_data) delete m_data;

  g_blockEventsOnDrag = FALSE;
};
   
wxDropSource::DragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
  if (gdk_dnd.dnd_grabbed) return None;
  if (gdk_dnd.drag_really) return None;
  
  if (!m_data) return None;
  if (m_data->GetDataSize() == 0) return None;
  
  GdkWindowPrivate *wp = (GdkWindowPrivate*) m_widget->window;
  
  RegisterWindow();
  
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
  
  int x = 0;
  int y = 0;
  gdk_window_get_pointer( m_widget->window, &x, &y, NULL );
  
  gdk_dnd_display_drag_cursor( x, y, FALSE, TRUE );
  
  while (gdk_dnd.drag_really || gdk_dnd.drag_perhaps) wxYield();
  
  UnregisterWindow();
  
  return Copy;
};

void wxDropSource::RegisterWindow(void)
{
  if (!m_data) return;

  wxString formats;
    
  wxDataFormat df = m_data->GetPreferredFormat();
  
    switch (df) 
    {
      case wxDF_TEXT: 
        formats += "text/plain";
	break;
      case wxDF_FILENAME:
        formats += "url:any";
	break;
      default:
        break;
    }
  
  char *str = WXSTRINGCAST formats;
  
  gtk_widget_dnd_drag_set( m_widget, TRUE, &str, 1 );

  gtk_signal_connect( GTK_OBJECT(m_widget), "drag_request_event",
    GTK_SIGNAL_FUNC(gtk_drag_callback), (gpointer)m_data );
};

void wxDropSource::UnregisterWindow(void)
{
  if (!m_widget) return;
  
  gtk_widget_dnd_drag_set( m_widget, FALSE, NULL, 0 );
  
  gtk_signal_disconnect_by_data( GTK_OBJECT(m_widget), (gpointer)m_data );
};
