/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

#include "wx/dcscreen.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// global data initialization
//-----------------------------------------------------------------------------

GdkWindow *wxScreenDC::sm_overlayWindow  = (GdkWindow*) NULL;
       int wxScreenDC::sm_overlayWindowX = 0;
       int wxScreenDC::sm_overlayWindowY = 0;

			     
//-----------------------------------------------------------------------------
// create X window
//-----------------------------------------------------------------------------

extern "C" {

#include "gdk/gdk.h"
#include "gdk/gdkprivate.h"
#include "gdk/gdkx.h"
#include <netinet/in.h>

int my_nevent_masks = 17;
int my_event_masks_table[19] =
{
  ExposureMask,
  PointerMotionMask,
  PointerMotionHintMask,
  ButtonMotionMask,
  Button1MotionMask,
  Button2MotionMask,
  Button3MotionMask,
  ButtonPressMask | OwnerGrabButtonMask,
  ButtonReleaseMask | OwnerGrabButtonMask,
  KeyPressMask,
  KeyReleaseMask,
  EnterWindowMask,
  LeaveWindowMask,
  FocusChangeMask,
  StructureNotifyMask,
  PropertyChangeMask,
  VisibilityChangeMask,
  0,				/* PROXIMITY_IN */
  0				/* PROXIMTY_OUT */
};

GdkWindow*
gdk_window_transparent_new ( GdkWindow     *parent,
		             GdkWindowAttr *attributes,
		             gint           attributes_mask)
{
  GdkWindow *window;
  GdkWindowPrivate *gprivate;
  GdkWindowPrivate *parent_private;
  GdkVisual *visual;
  Display *parent_display;
  Window xparent;
  Visual *xvisual;
  XSetWindowAttributes xattributes;
  long xattributes_mask;
  XSizeHints size_hints;
  XWMHints wm_hints;
  XClassHint *class_hint;
  int x, y, depth;
  unsigned int gclass;
  char *title;
  int i;

  g_return_val_if_fail (attributes != NULL, NULL);

  if (!parent)
    parent = (GdkWindow*) &gdk_root_parent;

  parent_private = (GdkWindowPrivate*) parent;
  if (parent_private->destroyed)
    return NULL;

  xparent = parent_private->xwindow;
  parent_display = parent_private->xdisplay;

  gprivate = g_new (GdkWindowPrivate, 1);
  window = (GdkWindow*) gprivate;

  gprivate->parent = parent;

  if (parent_private != &gdk_root_parent)
    parent_private->children = g_list_prepend (parent_private->children, window);

  gprivate->xdisplay = parent_display;
  gprivate->destroyed = FALSE;
  gprivate->resize_count = 0;
  gprivate->ref_count = 1;
  xattributes_mask = 0;

  if (attributes_mask & GDK_WA_X)
    x = attributes->x;
  else
    x = 0;

  if (attributes_mask & GDK_WA_Y)
    y = attributes->y;
  else
    y = 0;

  gprivate->x = x;
  gprivate->y = y;
  gprivate->width = (attributes->width > 1) ? (attributes->width) : (1);
  gprivate->height = (attributes->height > 1) ? (attributes->height) : (1);
  gprivate->window_type = attributes->window_type;
  gprivate->extension_events = FALSE;
  
#ifndef NEW_GTK_DND_CODE
  gprivate->dnd_drag_data_type = None;
  gprivate->dnd_drag_data_typesavail =
    gprivate->dnd_drop_data_typesavail = NULL;
  gprivate->dnd_drop_enabled = gprivate->dnd_drag_enabled =
    gprivate->dnd_drag_accepted = gprivate->dnd_drag_datashow =
    gprivate->dnd_drop_data_numtypesavail =
    gprivate->dnd_drag_data_numtypesavail = 0;
  gprivate->dnd_drag_eventmask = gprivate->dnd_drag_savedeventmask = 0;
#endif

  gprivate->filters = NULL;
  gprivate->children = NULL;

  window->user_data = NULL;

  if (attributes_mask & GDK_WA_VISUAL)
    visual = attributes->visual;
  else
    visual = gdk_visual_get_system ();
  xvisual = ((GdkVisualPrivate*) visual)->xvisual;

  xattributes.event_mask = StructureNotifyMask;
  for (i = 0; i < my_nevent_masks; i++)
    {
      if (attributes->event_mask & (1 << (i + 1)))
	xattributes.event_mask |= my_event_masks_table[i];
    }

  if (xattributes.event_mask)
    xattributes_mask |= CWEventMask;

  if(attributes_mask & GDK_WA_NOREDIR) {
	xattributes.override_redirect =
		(attributes->override_redirect == FALSE)?False:True;
	xattributes_mask |= CWOverrideRedirect;
  } else
    xattributes.override_redirect = False;

      gclass = InputOutput;
      depth = visual->depth;

      if (attributes_mask & GDK_WA_COLORMAP)
	gprivate->colormap = attributes->colormap;
      else
	gprivate->colormap = gdk_colormap_get_system ();

	  xattributes.colormap = ((GdkColormapPrivate*) gprivate->colormap)->xcolormap;
	  xattributes_mask |= CWColormap;

	  xparent = gdk_root_window;

	  xattributes.save_under = True;
	  xattributes.override_redirect = True;
	  xattributes.cursor = None;
	  xattributes_mask |= CWSaveUnder | CWOverrideRedirect;

  gprivate->xwindow = XCreateWindow (gprivate->xdisplay, xparent,
				    x, y, gprivate->width, gprivate->height,
				    0, depth, gclass, xvisual,
				    xattributes_mask, &xattributes);
  gdk_window_ref (window);
  gdk_xid_table_insert (&gprivate->xwindow, window);

  if (gprivate->colormap)
    gdk_colormap_ref (gprivate->colormap);

      XSetWMProtocols (gprivate->xdisplay, gprivate->xwindow, gdk_wm_window_protocols, 2);

  size_hints.flags = PSize;
  size_hints.width = gprivate->width;
  size_hints.height = gprivate->height;

  wm_hints.flags = InputHint | StateHint | WindowGroupHint;
  wm_hints.window_group = gdk_leader_window;
  wm_hints.input = True;
  wm_hints.initial_state = NormalState;

  /* FIXME: Is there any point in doing this? Do any WM's pay
   * attention to PSize, and even if they do, is this the
   * correct value???
   */
  XSetWMNormalHints (gprivate->xdisplay, gprivate->xwindow, &size_hints);

  XSetWMHints (gprivate->xdisplay, gprivate->xwindow, &wm_hints);

  if (attributes_mask & GDK_WA_TITLE)
    title = attributes->title;
  else
#if (GTK_MINOR_VERSION == 1)
    title = "Unknown"; // GLH: Well I don't know for the moment what to write here.
#else
    title = gdk_progname;
#endif

  XmbSetWMProperties (gprivate->xdisplay, gprivate->xwindow,
                      title, title,
                      NULL, 0,
                      NULL, NULL, NULL);

  if (attributes_mask & GDK_WA_WMCLASS)
    {
      class_hint = XAllocClassHint ();
      class_hint->res_name = attributes->wmclass_name;
      class_hint->res_class = attributes->wmclass_class;
      XSetClassHint (gprivate->xdisplay, gprivate->xwindow, class_hint);
      XFree (class_hint);
    }

  return window;
}

} // extern "C"

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC(void)
{
  m_ok = FALSE;
  m_window = (GdkWindow *) NULL;
  m_cmap = gdk_colormap_get_system();
  
  if (sm_overlayWindow)
  {
    m_window = sm_overlayWindow;
    m_deviceOriginX = - sm_overlayWindowX;
    m_deviceOriginY = - sm_overlayWindowY;
  }
  else
    m_window = GDK_ROOT_PARENT();
  
  SetUpDC();
  
  gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_brushGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_textGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_bgGC, GDK_INCLUDE_INFERIORS );
}

wxScreenDC::~wxScreenDC(void)
{
  EndDrawingOnTop();
}

bool wxScreenDC::StartDrawingOnTop( wxWindow *window )
{
  if (!window) return StartDrawingOnTop();
  
  int x = 0;
  int y = 0;
  window->GetPosition( &x, &y );
  int w = 0;
  int h = 0;
  window->GetSize( &w, &h );
  window->ClientToScreen( &x, &y );
  
  wxRect rect;
  rect.x = x;
  rect.y = y;
  rect.width = 0;
  rect.height = 0;
  
  return StartDrawingOnTop( &rect );
}

bool wxScreenDC::StartDrawingOnTop( wxRect *rect )
{
  int x = 0;
  int y = 0;
  int width = gdk_screen_width();
  int height = gdk_screen_height();
  if (rect)
  {
    x = rect->x;
    y = rect->y;
    width = rect->width;
    height = rect->height;
  }
  
  sm_overlayWindowX = x;
  sm_overlayWindowY = y;

  GdkWindowAttr attr;
  attr.x = x;
  attr.y = y;
  attr.width = width;
  attr.height = height;
  attr.override_redirect = TRUE;
  attr.wclass = GDK_INPUT_OUTPUT;
  attr.event_mask = 0;
  attr.window_type = GDK_WINDOW_TEMP;
  
  // GTK cannot set transparent backgrounds. :-(
  sm_overlayWindow = gdk_window_transparent_new( NULL, &attr, GDK_WA_NOREDIR | GDK_WA_X | GDK_WA_Y );
  
  if (sm_overlayWindow) gdk_window_show( sm_overlayWindow );

  return (sm_overlayWindow);
}

bool wxScreenDC::EndDrawingOnTop(void)
{
  if (sm_overlayWindow) gdk_window_destroy( sm_overlayWindow );
  
  sm_overlayWindow = NULL;
  sm_overlayWindowX = 0;
  sm_overlayWindowY = 0;
  
  return TRUE;
}

