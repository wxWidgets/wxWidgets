///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/utils.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gdk/gdkprivate.h"

#include <X11/Xlib.h>

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

#ifdef NEW_GTK_DND_CODE

#include "gtk/gtkdnd.h"
#include "gtk/gtkselection.h"

// ----------------------------------------------------------------------------
// "drag_leave"
// ----------------------------------------------------------------------------

static void target_drag_leave( GtkWidget *WXUNUSED(widget),
			       GdkDragContext *WXUNUSED(context),
			       guint WXUNUSED(time) )
{
    printf( "leave.\n" );
}

// ----------------------------------------------------------------------------
// "drag_motion"
// ----------------------------------------------------------------------------

static gboolean target_drag_motion( GtkWidget *WXUNUSED(widget),
			            GdkDragContext *context,
			            gint WXUNUSED(x),
			            gint WXUNUSED(y),
			            guint time )
{
    printf( "motion.\n" );
    gdk_drag_status( context, context->suggested_action, time );
    return TRUE;
}

// ----------------------------------------------------------------------------
// "drag_drop"
// ----------------------------------------------------------------------------

static gboolean target_drag_drop( GtkWidget *widget,
			          GdkDragContext *context,
			          gint x,
			          gint y,
			          guint time )
{
    printf( "drop at: %d,%d.\n", x, y );
  
    if (context->targets)
    {
        gtk_drag_get_data( widget, 
	                   context, 
		           GPOINTER_TO_INT (context->targets->data), 
		           time );
    }
    return FALSE;
}

// ----------------------------------------------------------------------------
// "drag_data_received"
// ----------------------------------------------------------------------------

static void target_drag_data_received( GtkWidget *WXUNUSED(widget),
			               GdkDragContext *context,
			               gint x,
			               gint y,
			               GtkSelectionData *data,
			               guint WXUNUSED(info),
			               guint time )
{
    printf( "data receive at: %d,%d.\n", x, y );
  
    if ((data->length >= 0) && (data->format == 8))
    {
      wxString str = (const char*)data->data;
      printf( "Received %s\n.", WXSTRINGCAST str );
      gtk_drag_finish( context, TRUE, FALSE, time );
      return;
    }
  
    gtk_drag_finish (context, FALSE, FALSE, time);
}

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
}

wxDropTarget::~wxDropTarget()
{
}

void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
  wxCHECK_RET( widget != NULL, "unregister widget is NULL" );
  
  gtk_drag_dest_set( widget,
		     (GtkDestDefaults) 0,
		     (GtkTargetEntry*) NULL, 
		     0,
		     (GdkDragAction) 0 );
		     
  gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

  gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

  gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

  gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
  wxCHECK_RET( widget != NULL, "register widget is NULL" );
  
  GtkTargetEntry format;
  format.info = 0;
  format.flags = 0;
  
  int valid = 0;
  for ( size_t i = 0; i < GetFormatCount(); i++ )
  {
    wxDataFormat df = GetFormat( i );
    switch (df) 
    {
      case wxDF_TEXT:
        format.target = "text/plain";
	valid++;
	break;
      case wxDF_FILENAME:
        format.target = "file:ALL";
	valid++;
	break;
      default:
        break;
    }
  }
  
  wxASSERT_MSG( valid != 0, "No valid DnD format supported." );
  
  gtk_drag_dest_set( widget,
		     GTK_DEST_DEFAULT_ALL,
		     &format, 
		     1,
		     (GdkDragAction)(GDK_ACTION_COPY | GDK_ACTION_MOVE) ); 
		     
  gtk_signal_connect( GTK_OBJECT(widget), "drag_leave",
		      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

  gtk_signal_connect( GTK_OBJECT(widget), "drag_motion",
		      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

  gtk_signal_connect( GTK_OBJECT(widget), "drag_drop",
		      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

  gtk_signal_connect( GTK_OBJECT(widget), "drag_data_received",
		      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------

bool wxTextDropTarget::OnDrop( long x, long y, const void *data, size_t WXUNUSED(size) )
{
  OnDropText( x, y, (const char*)data );
  return TRUE;
}

bool wxTextDropTarget::OnDropText( long x, long y, const char *psz )
{
  printf( "Got dropped text: %s.\n", psz );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
}

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

bool wxFileDropTarget::OnDropFiles( long x, long y, size_t nFiles, const char * const aszFiles[] )
{
  printf( "Got %d dropped files.\n", (int)nFiles );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  for (size_t i = 0; i < nFiles; i++)
  {
    printf( aszFiles[i] );
    printf( "\n" );
  }
  return TRUE;
}

bool wxFileDropTarget::OnDrop(long x, long y, const void *data, size_t size )
{
  size_t number = 0;
  char *text = (char*) data;
  for (size_t i = 0; i < size; i++)
    if (text[i] == 0) number++;

  if (number == 0) return TRUE;    
    
  char **files = new char*[number];
  
  text = (char*) data;
  for (size_t i = 0; i < number; i++)
  {
    files[i] = text;
    int len = strlen( text );
    text += len+1;
  }

  bool ret = OnDropFiles( x, y, 1, files ); 
  
  free( files );
  
  return ret;
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

wxDropSource::wxDropSource( wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
  m_data = (wxDataObject *) NULL;
  m_retValue = wxDragCancel;

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

wxDropSource::wxDropSource( wxDataObject &data, wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  m_retValue = wxDragCancel;
  
  m_data = &data;

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

void wxDropSource::SetData( wxDataObject &data )
{
  m_data = &data;  
}

wxDropSource::~wxDropSource(void)
{
//  if (m_data) delete m_data;

  g_blockEventsOnDrag = FALSE;
}
   
wxDragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
  wxASSERT_MSG( m_data, "wxDragSource: no data" );
  
  if (!m_data) return (wxDragResult) wxDragNone;
  if (m_data->GetDataSize() == 0) return (wxDragResult) wxDragNone;
  
  RegisterWindow();
  
  // TODO
  
  UnregisterWindow();
  
  g_blockEventsOnDrag = FALSE;
  
  return m_retValue;
}

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
        formats += "file:ALL";
	break;
      default:
        break;
    }
  
  char *str = WXSTRINGCAST formats;
  
  // TODO
}

void wxDropSource::UnregisterWindow(void)
{
  if (!m_widget) return;
  
  // TODO
}


#else  // NEW_CODE

GtkWidget *shape_create_icon (char     **data,
			      gint      x,
			      gint      y,
			      gint      px,
			      gint      py,
			      gint      window_type);

/* XPM */
static char * gv_xpm[] = {
"40 34 3 1",
" 	s None c None",
".	c black",
"X	c white",
"                                        ",
"                                        ",
"                  ......                ",
"                ..XXXXXX..              ",
"               .XXXXXXXXXX.             ",
"              .XXXXXXXXXXXX.            ",
"              .XXXXXXXXXXXX.            ",
"             .XXXXXXXXXXXXXX.           ",
"             .XXX..XXXX..XXX.           ",
"          ....XX....XX....XX.           ",
"         .XXX.XXX..XXXX..XXX....        ",
"        .XXXXXXXXXXXXXXXXXXX.XXX.       ",
"        .XXXXXXXXXXXXXXXXXXXXXXXX.      ",
"        .XXXXXXXXXXXXXXXXXXXXXXXX.      ",
"         ..XXXXXXXXXXXXXXXXXXXXXX.      ",
"           .XXXXXXXXXXXXXXXXXX...       ",
"           ..XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXXX.         ",
"            .XXXXXXXXXXXXXXXXX.         ",
"            .XXXXXXXXXXXXXXXXXX.        ",
"            .XXXXXXXXXXXXXXXXXXX.       ",
"           .XXXXXXXXXXXXXXXXXXXXX.      ",
"           .XXXXXXXXXXXXXX.XXXXXXX.     ",
"          .XXXXXXX.XXXXXXX.XXXXXXX.     ",
"         .XXXXXXXX.XXXXXXX.XXXXXXX.     ",
"         .XXXXXXX...XXXXX...XXXXX.      ",
"         .XXXXXXX.  .....   .....       ",
"         ..XXXX..                       ",
"           ....                         ",
"                                        ",
"                                        ",
"                                        "};
			      
/* XPM */
static char * page_xpm[] = {
/* width height ncolors chars_per_pixel */
"32 32 5 1",
/* colors */
" 	s None	c None",
".	c black",
"X	c wheat",
"o	c tan",
"O	c #6699FF",
/* pixels */
"    ...................         ",
"    .XXXXXXXXXXXXXXXXX..        ",
"    .XXXXXXXXXXXXXXXXX.o.       ",
"    .XXXXXXXXXXXXXXXXX.oo.      ",
"    .XXXXXXXXXXXXXXXXX.ooo.     ",
"    .XXXXXXXXXXXXXXXXX.oooo.    ",
"    .XXXXXXXXXXXXXXXXX.......   ",
"    .XXXXXOOOOOOOOOOXXXooooo.   ",
"    .XXXXXXXXXXXXXXXXXXooooo.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .........................   "};
			      
			      
//-----------------------------------------------------------------------------
// "drop_data_available_event"
//-----------------------------------------------------------------------------

static void gtk_target_callback( GtkWidget *widget, 
                                 GdkEventDropDataAvailable *event, 
			         wxDropTarget *target )
{
    if (target)
    {
        int x = 0;
        int y = 0;
        gdk_window_get_pointer( widget->window, &x, &y, (GdkModifierType *) NULL );

//        printf( "Drop data is of type %s.\n", event->data_type );
  
        target->OnDrop( x, y, (const void*)event->data, (size_t)event->data_numbytes );
    }

/*
    g_free (event->data);
    g_free (event->data_type);
*/
}

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
}

wxDropTarget::~wxDropTarget()
{
}

void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
    if (!widget) return;
  
    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
      GTK_SIGNAL_FUNC(gtk_target_callback), (gpointer) this );

    gtk_widget_dnd_drop_set( widget, FALSE, (gchar **) NULL, 0, FALSE );
}

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
    wxString formats;
    int valid = 0;
  
    for ( size_t i = 0; i < GetFormatCount(); i++ )
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
              formats += "file:ALL";
	      valid++;
	      break;
           default:
              break;
        }
    }
  
    char *str = WXSTRINGCAST formats;
  
    gtk_widget_dnd_drop_set( widget, TRUE, &str, valid, FALSE );
  
    gtk_signal_connect( GTK_OBJECT(widget), "drop_data_available_event",
      GTK_SIGNAL_FUNC(gtk_target_callback), (gpointer) this );
}

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------

bool wxTextDropTarget::OnDrop( long x, long y, const void *data, size_t WXUNUSED(size) )
{
    OnDropText( x, y, (const char*)data );
    return TRUE;
}

bool wxTextDropTarget::OnDropText( long x, long y, const char *psz )
{
    printf( "Got dropped text: %s.\n", psz );
    printf( "At x: %d, y: %d.\n", (int)x, (int)y );
    return TRUE;
}

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

bool wxFileDropTarget::OnDropFiles( long x, long y, size_t nFiles, const char * const aszFiles[] )
{
    printf( "Got %d dropped files.\n", (int)nFiles );
    printf( "At x: %d, y: %d.\n", (int)x, (int)y );
    for (size_t i = 0; i < nFiles; i++)
    {
        printf( aszFiles[i] );
        printf( "\n" );
    }
    return TRUE;
}

bool wxFileDropTarget::OnDrop(long x, long y, const void *data, size_t size )
{
    size_t number = 0;
    char *text = (char*) data;
    for (size_t i = 0; i < size; i++)
        if (text[i] == 0) number++;

    if (number == 0) return TRUE;    
    
    char **files = new char*[number];
  
    text = (char*) data;
    for (size_t i = 0; i < number; i++)
    {
        files[i] = text;
        int len = strlen( text );
        text += len+1;
    }

    bool ret = OnDropFiles( x, y, 1, files ); 
  
    free( files );
  
    return ret;
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

static void
shape_motion (GtkWidget      *widget, 
	      GdkEventMotion */*event*/);
	      
//-----------------------------------------------------------------------------
// drag request

void gtk_drag_callback( GtkWidget *widget, GdkEvent *event, wxDropSource *source )
{
    wxDataObject *data = source->m_data;

    switch (data->GetFormat())
    {
        case wxDF_TEXT:
	{
	    wxTextDataObject *text_object = (wxTextDataObject*) data;
	    
	    wxString text = text_object->GetText();
	    
	    char *s = WXSTRINGCAST text;
	    
            gtk_widget_dnd_data_set( widget, 
	                             event, 
				     (unsigned char*) s, 
				     (int) text.Length()+1 );
	
	    break;
	}
	
        case wxDF_FILENAME:
	{
	    wxFileDataObject *file_object = (wxFileDataObject*) data;
	    
	    wxString text = file_object->GetFiles();
	    
	    char *s = WXSTRINGCAST text;
	    
            gtk_widget_dnd_data_set( widget, 
	                             event, 
				     (unsigned char*) s, 
				     (int) text.Length()+1 );
	
	    break;
	}
	
	default:
	{
	    return;
	}
   }
  
  source->m_retValue = wxDragCopy;
}

wxDropSource::wxDropSource( wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
  m_data = (wxDataObject *) NULL;
  m_retValue = wxDragCancel;

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

wxDropSource::wxDropSource( wxDataObject &data, wxWindow *win )
{
  g_blockEventsOnDrag = TRUE;
  
  m_window = win;
  m_widget = win->m_widget;
  if (win->m_wxwindow) m_widget = win->m_wxwindow;
  m_retValue = wxDragCancel;
  
  m_data = &data;

  m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
  m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

void wxDropSource::SetData( wxDataObject &data )
{
  m_data = &data;  
}

wxDropSource::~wxDropSource(void)
{
//  if (m_data) delete m_data;

  g_blockEventsOnDrag = FALSE;
}
   
wxDragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
  if (gdk_dnd.dnd_grabbed) return (wxDragResult) wxDragNone;
  if (gdk_dnd.drag_really) return (wxDragResult) wxDragNone;
  
  wxASSERT_MSG( m_data, "wxDragSource: no data" );
  
  if (!m_data) return (wxDragResult) wxDragNone;
  
  static GtkWidget *drag_icon = (GtkWidget*) NULL;
  static GtkWidget *drop_icon = (GtkWidget*) NULL;

  GdkPoint hotspot_1 = {0,-5 };
      
     if (!drag_icon)
	{
	  drag_icon = shape_create_icon ( gv_xpm,
					 440, 140, 0,0, GTK_WINDOW_POPUP);
	  
	  gtk_signal_connect (GTK_OBJECT (drag_icon), "destroy",
			      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			      &drag_icon);

	  gtk_widget_hide (drag_icon);
	}
      
  GdkPoint hotspot_2 = {-5,-5};
	
      if (!drop_icon)
	{
	  drop_icon = shape_create_icon ( page_xpm,
					 440, 140, 0,0, GTK_WINDOW_POPUP);
	  
	  gtk_signal_connect (GTK_OBJECT (drop_icon), "destroy",
			      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			      &drop_icon);

	  gtk_widget_hide (drop_icon);
	}
	

    gdk_dnd_set_drag_shape( drag_icon->window,
			     &hotspot_1,
			     drop_icon->window,
			     &hotspot_2);
  
  
  GdkWindowPrivate *wp = (GdkWindowPrivate*) m_widget->window;
  
  RegisterWindow();
  
  gdk_dnd.drag_perhaps = TRUE;

  gdk_dnd.dnd_drag_start.x = 5;
  gdk_dnd.dnd_drag_start.y = 5;
  gdk_dnd.real_sw = wp;
	  
  if (gdk_dnd.drag_startwindows)
  {
    g_free( gdk_dnd.drag_startwindows );
    gdk_dnd.drag_startwindows = (GdkWindow **) NULL;
  }
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
  wxGetMousePosition( &x, &y );  
  
  gdk_dnd_display_drag_cursor( x, y, FALSE, TRUE );
  
/*
    shape_motion( drag_icon, (GdkEventMotion *)NULL );
    shape_motion( drop_icon, (GdkEventMotion *)NULL );
*/

  while (gdk_dnd.drag_really || gdk_dnd.drag_perhaps) wxYield();
  
  UnregisterWindow();
  
  g_blockEventsOnDrag = FALSE;
  
  return m_retValue;
}

void wxDropSource::RegisterWindow(void)
{
  if (!m_data) return;

  wxString formats;
    
  wxDataFormat df = m_data->GetFormat();
  
    switch (df) 
    {
      case wxDF_TEXT: 
        formats += "text/plain";
	break;
      case wxDF_FILENAME:
        formats += "file:ALL";
	break;
      default:
        break;
    }
  
  char *str = WXSTRINGCAST formats;
  
  gtk_widget_dnd_drag_set( m_widget, TRUE, &str, 1 );

  gtk_signal_connect( GTK_OBJECT(m_widget), "drag_request_event",
    GTK_SIGNAL_FUNC(gtk_drag_callback), (gpointer)this );
}

void wxDropSource::UnregisterWindow(void)
{
  if (!m_widget) return;
  
  gtk_widget_dnd_drag_set( m_widget, FALSE, (gchar **) NULL, 0 );
  
  gtk_signal_disconnect_by_data( GTK_OBJECT(m_widget), (gpointer)this );
}


/*
 * Shaped Windows
 */
static GdkWindow *root_win = (GdkWindow*) NULL;

typedef struct _cursoroffset {gint x,y;} CursorOffset;

static void
shape_pressed (GtkWidget *widget, GdkEventButton *event)
{
  CursorOffset *p;

  /* ignore double and triple click */
  if (event->type != GDK_BUTTON_PRESS)
    return;

  p = (CursorOffset *)gtk_object_get_user_data (GTK_OBJECT(widget));
  p->x = (int) event->x;
  p->y = (int) event->y;

  gtk_grab_add (widget);
  gdk_pointer_grab (widget->window, TRUE,
		    (GdkEventMask)
		      (GDK_BUTTON_RELEASE_MASK |
		       GDK_BUTTON_MOTION_MASK |
		       GDK_POINTER_MOTION_HINT_MASK),
		    (GdkWindow*)NULL, 
		    (GdkCursor*) NULL, 0);
}


static void
shape_released (GtkWidget *widget)
{
  gtk_grab_remove (widget);
  gdk_pointer_ungrab (0);
}

static void
shape_motion (GtkWidget      *widget, 
	      GdkEventMotion */*event*/)
{
  gint xp, yp;
  CursorOffset * p;
  GdkModifierType mask;

  p = (CursorOffset *)gtk_object_get_user_data (GTK_OBJECT (widget));

  /*
   * Can't use event->x / event->y here 
   * because I need absolute coordinates.
   */
  gdk_window_get_pointer (root_win, &xp, &yp, &mask);
  gtk_widget_set_uposition (widget, xp  - p->x, yp  - p->y);
}

GtkWidget *
shape_create_icon (char     **data,
		   gint      x,
		   gint      y,
		   gint      px,
		   gint      py,
		   gint      window_type)
{
  GtkWidget *window;
  GtkWidget *pixmap;
  GtkWidget *fixed;
  CursorOffset* icon_pos;
  GdkGC* gc;
  GdkBitmap *gdk_pixmap_mask;
  GdkPixmap *gdk_pixmap;
  GtkStyle *style;

  style = gtk_widget_get_default_style ();
  gc = style->black_gc;	

  /*
   * GDK_WINDOW_TOPLEVEL works also, giving you a title border
   */
  window = gtk_window_new ((GtkWindowType)window_type);
  
  fixed = gtk_fixed_new ();
  gtk_widget_set_usize (fixed, 100,100);
  gtk_container_add (GTK_CONTAINER (window), fixed);
  gtk_widget_show (fixed);
  
  gtk_widget_set_events (window, 
			 gtk_widget_get_events (window) |
			 GDK_BUTTON_MOTION_MASK |
			 GDK_POINTER_MOTION_HINT_MASK |
			 GDK_BUTTON_PRESS_MASK);

  gtk_widget_realize (window);
  
  gdk_pixmap = gdk_pixmap_create_from_xpm_d (window->window, &gdk_pixmap_mask, 
					   &style->bg[GTK_STATE_NORMAL],
					   (gchar**) data );

  pixmap = gtk_pixmap_new (gdk_pixmap, gdk_pixmap_mask);
  gtk_fixed_put (GTK_FIXED (fixed), pixmap, px,py);
  gtk_widget_show (pixmap);
  
  gtk_widget_shape_combine_mask (window, gdk_pixmap_mask, px,py);


  gtk_signal_connect (GTK_OBJECT (window), "button_press_event",
		      GTK_SIGNAL_FUNC (shape_pressed),NULL);
  gtk_signal_connect (GTK_OBJECT (window), "button_release_event",
		      GTK_SIGNAL_FUNC (shape_released),NULL);
  gtk_signal_connect (GTK_OBJECT (window), "motion_notify_event",
		      GTK_SIGNAL_FUNC (shape_motion),NULL);

  icon_pos = g_new (CursorOffset, 1);
  gtk_object_set_user_data(GTK_OBJECT(window), icon_pos);

  gtk_widget_set_uposition (window, x, y);
  gtk_widget_show (window);
  
  return window;
}

#endif 
       // NEW_GTK_DND_CODE
       
#endif

      // wxUSE_DRAG_AND_DROP