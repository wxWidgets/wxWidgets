/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

#include "wx/dcscreen.h"
#include "wx/window.h"
#include "gdk/gdkx.h"

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC(void)
{
  m_ok = FALSE;
  m_window = (GdkWindow *) NULL;
  m_cmap = gdk_colormap_get_system();
  
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

bool wxScreenDC::StartDrawingOnTop( wxWindow *WXUNUSED(window) )
{
  return TRUE;
/*
  if (!window)
  {
    StartDrawingOnTop();
    return;
  }
  wxRectangle rect;
  rect.x = 0;
  rect.y = 0;
  window->GetPosition( &rect.x, &rect.y );
  rect.width = 0;
  rect.height = 0;
  window->GetSize( &rect.width, &rect.height );
  window->ClientToScreen( &rect.x, &rect.y );
  StartDrawingOnTop( &rect );
  return TRUE;
*/
}

bool wxScreenDC::StartDrawingOnTop( wxRectangle *WXUNUSED(rect) )
{
  return TRUE;
/*
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
  
  GTK cannot set transparent backgrounds. :-(

  GdkWindowAttr attr;
  attr.x = x;
  attr.y = y;
  attr.width = width;
  attr.height = height;
  attr.override_redirect = TRUE;
  attr.wclass = GDK_INPUT_OUTPUT;
  attr.event_mask = 0;
  attr.window_type = GDK_WINDOW_TEMP;
  m_window = gdk_window_new( NULL, &attr, GDK_WA_NOREDIR | GDK_WA_X | GDK_WA_Y );
  
  gdk_window_show( m_window );

  m_window = GDK_ROOT_PARENT();
  
  SetUpDC();
  
  gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_brushGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_textGC, GDK_INCLUDE_INFERIORS );
  gdk_gc_set_subwindow( m_bgGC, GDK_INCLUDE_INFERIORS );

  return TRUE;
*/
}

bool wxScreenDC::EndDrawingOnTop(void)
{
  return TRUE;
/*
  if (m_window) gdk_window_destroy( m_window );
  m_window = NULL;
  m_isOk = FALSE;
  return TRUE;
*/
}

