/////////////////////////////////////////////////////////////////////////////
// Name:        window.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/layout.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/dcclient.h"
#include "wx/dnd.h"
#include "wx/menu.h"
#include "wx/statusbr.h"
#include "wx/intl.h"
#include "gdk/gdkprivate.h"
#include "gdk/gdkkeysyms.h"

#include <math.h>

//-----------------------------------------------------------------------------
// documentation on internals
//-----------------------------------------------------------------------------

/*
   I have been asked several times about writing some documentation about
   the GTK port of wxWindows, especially its internal structures. Obviously,
   you cannot understand wxGTK without knowing a little about the GTK, but
   some more information about what the wxWindow, which is the base class
   for all other window classes, does seems required as well.

   What does wxWindow do? It contains the common interface for the following
   jobs of its descendants:

   1) Define the rudimentary behaviour common to all window classes, such as
   resizing, intercepting user input (so as to make it possible to use these
   events for special purposes in a derived class), window names etc.

   2) Provide the possibility to contain and manage children, if the derived
   class is allowed to contain children, which holds true for those window
   classes which do not display a native GTK widget. To name them, these
   classes are wxPanel, wxScrolledWindow, wxDialog, wxFrame. The MDI frame-
   work classes are a special case and are handled a bit differently from
   the rest. The same holds true for the wxNotebook class.

   3) Provide the possibility to draw into a client area of a window. This,
   too, only holds true for classes that do not display a native GTK widget
   as above.

   4) Provide the entire mechanism for scrolling widgets. This actual inter-
   face for this is usually in wxScrolledWindow, but the GTK implementation
   is in this class.

   5) A multitude of helper or extra methods for special purposes, such as
   Drag'n'Drop, managing validators etc.

   Normally one might expect, that one wxWindows window would always correspond
   to one GTK widget. Under GTK, there is no such allround widget that has all
   the functionality. Moreover, the GTK defines a client area as a different
   widget from the actual widget you are handling. Last but not least some
   special classes (e.g. wxFrame) handle different categories of widgets and
   still have the possibility to draw something in the client area.
   It was therefore required to write a special purpose GTK widget, that would
   represent a client area in the sense of wxWindows capable to do the jobs
   2), 3) and 4). I have written this class and it resides in win_gtk.c of
   this directory.

   All windows must have a widget, with which they interact with other under-
   lying GTK widgets. It is this widget, e.g. that has to be resized etc and
   thw wxWindow class has a member variable called m_widget which holds a
   pointer to this widget. When the window class represents a GTK native widget,
   this is (in most cases) the only GTK widget the class manages. E.g. the
   wxStatitText class handles only a GtkLabel widget a pointer to which you
   can find in m_widget (defined in wxWindow)
   
   When the class has a client area for drawing into and for containing children
   it has to handle the client area widget (of the type GtkMyFixed, defined in 
   win_gtk.c), but there could be any number of widgets, handled by a class 
   The common rule for all windows is only, that the widget that interacts with 
   the rest of GTK must be referenced in m_widget and all other widgets must be 
   children of this widget on the GTK level. The top-most widget, which also 
   represents the client area, must be in the m_wxwindow field and must be of 
   the type GtkMyFixed.

   As I said, the window classes that display a GTK native widget only have
   one widget, so in the case of e.g. the wxButton class m_widget holds a
   pointer to a GtkButton widget. But windows with client areas (for drawing
   and children) have a m_widget field that is a pointer to a GtkScrolled-
   Window and a m_wxwindow field that is pointer to a GtkMyFixed and this
   one is (in the GTK sense) a child of the GtkScrolledWindow.

   If the m_wxwindow field is set, then all input to this widget is inter-
   cepted and sent to the wxWindows class. If not, all input to the widget
   that gets pointed to by m_widget gets intercepted and sent to the class.

*/

//-----------------------------------------------------------------------------
// cond comp
//-----------------------------------------------------------------------------

#if (GTK_MINOR_VERSION == 1)
#if (GTK_MICRO_VERSION >= 3)
#define NEW_GTK_DND_CODE
#endif
#endif

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;
extern wxList wxTopLevelWindows;
extern bool   g_blockEventsOnDrag;
static bool   g_capturing = FALSE;

// hack: we need something to pass to gtk_menu_popup, so we store the time of
// the last click here
static guint32 gs_timeLastClick = 0;

//-----------------------------------------------------------------------------
// "expose_event" (of m_wxwindow, not of m_widget)
//-----------------------------------------------------------------------------

static void gtk_window_expose_callback( GtkWidget *WXUNUSED(widget), GdkEventExpose *gdk_event, wxWindow *win )
{
    if (!win->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    win->m_updateRegion.Union( gdk_event->area.x,
                               gdk_event->area.y,
                               gdk_event->area.width,
                               gdk_event->area.height );

    if (gdk_event->count > 0) return;

/*
    printf( "OnExpose from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxPaintEvent event( win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    win->m_updateRegion.Clear();
}

//-----------------------------------------------------------------------------
// "draw" (of m_wxwindow, not of m_widget)
//-----------------------------------------------------------------------------

static void gtk_window_draw_callback( GtkWidget *WXUNUSED(widget), GdkRectangle *rect, wxWindow *win )
{
    if (!win->HasVMT()) return;
    if (g_blockEventsOnDrag) return;

    win->m_updateRegion.Union( rect->x, rect->y, rect->width, rect->height );

    wxPaintEvent event( win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    win->m_updateRegion.Clear();
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

static gint gtk_window_key_press_callback( GtkWidget *widget, GdkEventKey *gdk_event, wxWindow *win )
{
    if (!win->HasVMT()) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

/*
    printf( "OnKeyPress from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    long key_code = 0;
    switch (gdk_event->keyval)
    {
        case GDK_BackSpace:     key_code = WXK_BACK;        break;
        case GDK_Tab:           key_code = WXK_TAB;         break;
        case GDK_Linefeed:      key_code = WXK_RETURN;      break;
        case GDK_Clear:         key_code = WXK_CLEAR;       break;
        case GDK_Return:        key_code = WXK_RETURN;      break;
        case GDK_Pause:         key_code = WXK_PAUSE;       break;
        case GDK_Scroll_Lock:   key_code = WXK_SCROLL;      break;
        case GDK_Escape:        key_code = WXK_ESCAPE;      break;
        case GDK_Delete:        key_code = WXK_DELETE;      break;
        case GDK_Home:          key_code = WXK_HOME;        break;
        case GDK_Left:          key_code = WXK_LEFT;        break;
        case GDK_Up:            key_code = WXK_UP;          break;
        case GDK_Right:         key_code = WXK_RIGHT;       break;
        case GDK_Down:          key_code = WXK_DOWN;        break;
        case GDK_Prior:         key_code = WXK_PRIOR;       break;
//      case GDK_Page_Up:       key_code = WXK_PAGEUP;      break;
        case GDK_Next:          key_code = WXK_NEXT;        break;
//      case GDK_Page_Down:     key_code = WXK_PAGEDOWN;    break;
        case GDK_End:           key_code = WXK_END;         break;
        case GDK_Begin:         key_code = WXK_HOME;        break;
        case GDK_Select:        key_code = WXK_SELECT;      break;
        case GDK_Print:         key_code = WXK_PRINT;       break;
        case GDK_Execute:       key_code = WXK_EXECUTE;     break;
        case GDK_Insert:        key_code = WXK_INSERT;      break;
        case GDK_Num_Lock:      key_code = WXK_NUMLOCK;     break;
        case GDK_KP_Tab:        key_code = WXK_TAB;         break;
        case GDK_KP_Enter:      key_code = WXK_RETURN;      break;
        case GDK_KP_Home:       key_code = WXK_HOME;        break;
        case GDK_KP_Left:       key_code = WXK_LEFT;        break;
        case GDK_KP_Up:         key_code = WXK_UP;          break;
        case GDK_KP_Right:      key_code = WXK_RIGHT;       break;
        case GDK_KP_Down:       key_code = WXK_DOWN;        break;
        case GDK_KP_Prior:      key_code = WXK_PRIOR;       break;
//      case GDK_KP_Page_Up:    key_code = WXK_PAGEUP;      break;
        case GDK_KP_Next:       key_code = WXK_NEXT;        break;
//      case GDK_KP_Page_Down:  key_code = WXK_PAGEDOWN;    break;
        case GDK_KP_End:        key_code = WXK_END;         break;
        case GDK_KP_Begin:      key_code = WXK_HOME;        break;
        case GDK_KP_Insert:     key_code = WXK_INSERT;      break;
        case GDK_KP_Delete:     key_code = WXK_DELETE;      break;
        case GDK_KP_Multiply:   key_code = WXK_MULTIPLY;    break;
        case GDK_KP_Add:        key_code = WXK_ADD;         break;
        case GDK_KP_Separator:  key_code = WXK_SEPARATOR;   break;
        case GDK_KP_Subtract:   key_code = WXK_SUBTRACT;    break;
        case GDK_KP_Decimal:    key_code = WXK_DECIMAL;     break;
        case GDK_KP_Divide:     key_code = WXK_DIVIDE;      break;
        case GDK_KP_0:          key_code = WXK_NUMPAD0;     break;
        case GDK_KP_1:          key_code = WXK_NUMPAD1;     break;
        case GDK_KP_2:          key_code = WXK_NUMPAD2;     break;
        case GDK_KP_3:          key_code = WXK_NUMPAD3;     break;
        case GDK_KP_4:          key_code = WXK_NUMPAD4;     break;
        case GDK_KP_5:          key_code = WXK_NUMPAD5;     break;
        case GDK_KP_6:          key_code = WXK_NUMPAD6;     break;
        case GDK_KP_7:          key_code = WXK_NUMPAD7;     break;
        case GDK_KP_8:          key_code = WXK_NUMPAD7;     break;
        case GDK_KP_9:          key_code = WXK_NUMPAD9;     break;
        case GDK_F1:            key_code = WXK_F1;          break;
        case GDK_F2:            key_code = WXK_F2;          break;
        case GDK_F3:            key_code = WXK_F3;          break;
        case GDK_F4:            key_code = WXK_F4;          break;
        case GDK_F5:            key_code = WXK_F5;          break;
        case GDK_F6:            key_code = WXK_F6;          break;
        case GDK_F7:            key_code = WXK_F7;          break;
        case GDK_F8:            key_code = WXK_F8;          break;
        case GDK_F9:            key_code = WXK_F9;          break;
        case GDK_F10:           key_code = WXK_F10;         break;
        case GDK_F11:           key_code = WXK_F11;         break;
        case GDK_F12:           key_code = WXK_F12;         break;
        default:
        {
            if ((gdk_event->keyval >= 0x20) && (gdk_event->keyval <= 0xFF))
                key_code = gdk_event->keyval;
        }
    }

    if (!key_code) return FALSE;

    wxKeyEvent event( wxEVT_CHAR );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_keyCode = key_code;
    event.m_x = 0;
    event.m_y = 0;
    event.SetEventObject( win );

    bool ret = win->GetEventHandler()->ProcessEvent( event );

    if (!ret)
    {
        wxWindow *ancestor = win;
        while (ancestor)
        {
            int command = ancestor->GetAcceleratorTable()->GetCommand( event );
            if (command != -1)
            {
                wxCommandEvent command_event( wxEVT_COMMAND_MENU_SELECTED, command );
                ret = ancestor->GetEventHandler()->ProcessEvent( command_event );
                break;
            }
            ancestor = ancestor->GetParent();
        }
    }

    if (ret)
    {
        if ((gdk_event->keyval >= 0x20) && (gdk_event->keyval <= 0xFF))
            gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "key_press_event" );
    }

    return ret;
}

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

static gint gtk_window_button_press_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxWindow *win )
{
    if (!win->IsOwnGtkWindow( gdk_event->window )) return TRUE;

    if (g_blockEventsOnDrag) return TRUE;

    if (win->m_wxwindow)
    {
        if (GTK_WIDGET_CAN_FOCUS(win->m_wxwindow) && !GTK_WIDGET_HAS_FOCUS (win->m_wxwindow) )
        {
            gtk_widget_grab_focus (win->m_wxwindow);

/*
            printf( "GrabFocus from " );
            if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
                printf( win->GetClassInfo()->GetClassName() );
            printf( ".\n" );
*/

        }
    }

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnButtonPress from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxEventType event_type = wxEVT_LEFT_DOWN;

    if (gdk_event->button == 1)
    {
        switch (gdk_event->type)
        {
            case GDK_BUTTON_PRESS: event_type = wxEVT_LEFT_DOWN; break;
            case GDK_2BUTTON_PRESS: event_type = wxEVT_LEFT_DCLICK; break;
            default:  break;
        }
    }
    else if (gdk_event->button == 2)
    {
        switch (gdk_event->type)
        {
            case GDK_BUTTON_PRESS: event_type = wxEVT_MIDDLE_DOWN; break;
            case GDK_2BUTTON_PRESS: event_type = wxEVT_MIDDLE_DCLICK; break;
            default:  break;
        }
    }
    else if (gdk_event->button == 3)
    {
        switch (gdk_event->type)
        {
            case GDK_BUTTON_PRESS: event_type = wxEVT_RIGHT_DOWN; break;
            case GDK_2BUTTON_PRESS: event_type = wxEVT_RIGHT_DCLICK; break;
            default:  break;
        }
    }

    wxMouseEvent event( event_type );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_leftDown = (gdk_event->state & GDK_BUTTON1_MASK);
    event.m_middleDown = (gdk_event->state & GDK_BUTTON2_MASK);
    event.m_rightDown = (gdk_event->state & GDK_BUTTON3_MASK);

    event.m_x = (long)gdk_event->x;
    event.m_y = (long)gdk_event->y;

    // Some control don't have their own X window and thus cannot get
    // any events.

    if (!g_capturing)
    {
        wxNode *node = win->GetChildren()->First();
        while (node)
        {
            wxWindow *child = (wxWindow*)node->Data();
            if ((child->m_x <= event.m_x) &&
                (child->m_y <= event.m_y) &&
                (child->m_x+child->m_width  >= event.m_x) &&
                (child->m_y+child->m_height >= event.m_y))
            {
                win = child;
                event.m_x -= child->m_x;
                event.m_y -= child->m_y;
                break;
            }
            node = node->Next();
        }
    }

    event.SetEventObject( win );

    gs_timeLastClick = gdk_event->time;

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "button_press_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "button_release_event"
//-----------------------------------------------------------------------------

static gint gtk_window_button_release_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxWindow *win )
{
    if (!win->IsOwnGtkWindow( gdk_event->window )) return TRUE;

    if (g_blockEventsOnDrag) return TRUE;

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnButtonRelease from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxEventType event_type = wxEVT_NULL;

    switch (gdk_event->button)
    {
        case 1: event_type = wxEVT_LEFT_UP; break;
        case 2: event_type = wxEVT_MIDDLE_UP; break;
        case 3: event_type = wxEVT_RIGHT_UP; break;
    }

    wxMouseEvent event( event_type );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_leftDown = (gdk_event->state & GDK_BUTTON1_MASK);
    event.m_middleDown = (gdk_event->state & GDK_BUTTON2_MASK);
    event.m_rightDown = (gdk_event->state & GDK_BUTTON3_MASK);
    event.m_x = (long)gdk_event->x;
    event.m_y = (long)gdk_event->y;

    // Some control don't have their own X window and thus cannot get
    // any events.

    if (!g_capturing)
    {
        wxNode *node = win->GetChildren()->First();
        while (node)
        {
            wxWindow *child = (wxWindow*)node->Data();
            if ((child->m_x <= event.m_x) &&
                (child->m_y <= event.m_y) &&
                (child->m_x+child->m_width  >= event.m_x) &&
                (child->m_y+child->m_height >= event.m_y))
            {
                win = child;
                event.m_x -= child->m_x;
                event.m_y -= child->m_y;
                break;
            }
            node = node->Next();
        }
    }

    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "button_release_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "motion_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_window_motion_notify_callback( GtkWidget *widget, GdkEventMotion *gdk_event, wxWindow *win )
{
    if (!win->IsOwnGtkWindow( gdk_event->window )) return TRUE;

    if (g_blockEventsOnDrag) return TRUE;

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnMotion from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
      printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxMouseEvent event( wxEVT_MOTION );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_leftDown = (gdk_event->state & GDK_BUTTON1_MASK);
    event.m_middleDown = (gdk_event->state & GDK_BUTTON2_MASK);
    event.m_rightDown = (gdk_event->state & GDK_BUTTON3_MASK);

    event.m_x = (long)gdk_event->x;
    event.m_y = (long)gdk_event->y;

    // Some control don't have their own X window and thus cannot get
    // any events.

    if (!g_capturing)
    {
        wxNode *node = win->GetChildren()->First();
        while (node)
        {
            wxWindow *child = (wxWindow*)node->Data();
            if ((child->m_x <= event.m_x) &&
                (child->m_y <= event.m_y) &&
                (child->m_x+child->m_width  >= event.m_x) &&
                (child->m_y+child->m_height >= event.m_y))
            {
                win = child;
                event.m_x -= child->m_x;
                event.m_y -= child->m_y;
                break;
            }
            node = node->Next();
        }
    }

    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "motion_notify_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "focus_in_event"
//-----------------------------------------------------------------------------

static gint gtk_window_focus_in_callback( GtkWidget *widget, GdkEvent *WXUNUSED(event), wxWindow *win )
{
    if (g_blockEventsOnDrag) return TRUE;
    if (win->m_wxwindow)
    {
        if (GTK_WIDGET_CAN_FOCUS(win->m_wxwindow))
        {
            GTK_WIDGET_SET_FLAGS (win->m_wxwindow, GTK_HAS_FOCUS);
/*
            printf( "SetFocus flag from " );
            if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
                printf( win->GetClassInfo()->GetClassName() );
            printf( ".\n" );
*/
        }
    }

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnSetFocus from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( "   " );
    printf( WXSTRINGCAST win->GetLabel() );
    printf( ".\n" );
*/

    wxFocusEvent event( wxEVT_SET_FOCUS, win->GetId() );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "focus_in_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "focus_out_event"
//-----------------------------------------------------------------------------

static gint gtk_window_focus_out_callback( GtkWidget *widget, GdkEvent *WXUNUSED(event), wxWindow *win )
{
    if (g_blockEventsOnDrag) return TRUE;
    if (win->m_wxwindow)
    {
      if (GTK_WIDGET_CAN_FOCUS(win->m_wxwindow))
          GTK_WIDGET_UNSET_FLAGS (win->m_wxwindow, GTK_HAS_FOCUS);
    }

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnKillFocus from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxFocusEvent event( wxEVT_KILL_FOCUS, win->GetId() );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "focus_out_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "enter_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_window_enter_callback( GtkWidget *widget, GdkEventCrossing *gdk_event, wxWindow *win )
{
    if (widget->window != gdk_event->window) return TRUE;

    if (g_blockEventsOnDrag) return TRUE;

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnEnter from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if ((widget->window) && (win->m_cursor))
        gdk_window_set_cursor( widget->window, win->m_cursor->GetCursor() );

    wxMouseEvent event( wxEVT_ENTER_WINDOW );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
       gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "enter_notify_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "leave_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_window_leave_callback( GtkWidget *widget, GdkEventCrossing *gdk_event, wxWindow *win )
{
    if (widget->window != gdk_event->window) return TRUE;

    if (g_blockEventsOnDrag) return TRUE;

    if (!win->HasVMT()) return TRUE;

/*
    printf( "OnLeave from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if ((widget->window) && (win->m_cursor))
        gdk_window_set_cursor( widget->window, wxSTANDARD_CURSOR->GetCursor() );

    wxMouseEvent event( wxEVT_LEAVE_WINDOW );
    event.SetEventObject( win );

    if (win->GetEventHandler()->ProcessEvent( event ))
        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "leave_notify_event" );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "value_changed" from m_vAdjust
//-----------------------------------------------------------------------------

static void gtk_window_vscroll_callback( GtkWidget *WXUNUSED(widget), wxWindow *win )
{
    if (g_blockEventsOnDrag) return;

/*
    printf( "OnVScroll from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if (!win->HasVMT()) return;

    float diff = win->m_vAdjust->value - win->m_oldVerticalPos;
    if (fabs(diff) < 0.2) return;

    wxEventType command = wxEVT_NULL;

    float line_step = win->m_vAdjust->step_increment;
    float page_step = win->m_vAdjust->page_increment;

    if (fabs(diff-line_step) < 0.2) command = wxEVT_SCROLL_LINEDOWN;
    else if (fabs(diff+line_step) < 0.2) command = wxEVT_SCROLL_LINEUP;
    else if (fabs(diff-page_step) < 0.2) command = wxEVT_SCROLL_PAGEDOWN;
    else if (fabs(diff+page_step) < 0.2) command = wxEVT_SCROLL_PAGEUP;
    else command = wxEVT_SCROLL_THUMBTRACK;

    int value = (int)(win->m_vAdjust->value+0.5);

    wxScrollEvent event( command, win->GetId(), value, wxVERTICAL );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "value_changed" from m_hAdjust
//-----------------------------------------------------------------------------

static void gtk_window_hscroll_callback( GtkWidget *WXUNUSED(widget), wxWindow *win )
{
    if (g_blockEventsOnDrag) return;

/*
    printf( "OnHScroll from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if (!win->HasVMT()) return;

    float diff = win->m_hAdjust->value - win->m_oldHorizontalPos;
    if (fabs(diff) < 0.2) return;

    wxEventType command = wxEVT_NULL;

    float line_step = win->m_hAdjust->step_increment;
    float page_step = win->m_hAdjust->page_increment;

    if (fabs(diff-line_step) < 0.2) command = wxEVT_SCROLL_LINEDOWN;
    else if (fabs(diff+line_step) < 0.2) command = wxEVT_SCROLL_LINEUP;
    else if (fabs(diff-page_step) < 0.2) command = wxEVT_SCROLL_PAGEDOWN;
    else if (fabs(diff+page_step) < 0.2) command = wxEVT_SCROLL_PAGEUP;
    else command = wxEVT_SCROLL_THUMBTRACK;

    int value = (int)(win->m_hAdjust->value+0.5);

    wxScrollEvent event( command, win->GetId(), value, wxHORIZONTAL );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "changed" from m_vAdjust
//-----------------------------------------------------------------------------

static void gtk_window_vscroll_change_callback( GtkWidget *WXUNUSED(widget), wxWindow *win )
{
    if (g_blockEventsOnDrag) return;

/*
    printf( "OnVScroll change from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if (!win->HasVMT()) return;

    wxEventType command = wxEVT_SCROLL_THUMBTRACK;
    int value = (int)(win->m_vAdjust->value+0.5);

    wxScrollEvent event( command, win->GetId(), value, wxVERTICAL );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "changed" from m_hAdjust
//-----------------------------------------------------------------------------

static void gtk_window_hscroll_change_callback( GtkWidget *WXUNUSED(widget), wxWindow *win )
{
    if (g_blockEventsOnDrag) return;

/*
    printf( "OnHScroll change from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    if (!win->HasVMT()) return;

    wxEventType command = wxEVT_SCROLL_THUMBTRACK;
    int value = (int)(win->m_hAdjust->value+0.5);

    wxScrollEvent event( command, win->GetId(), value, wxHORIZONTAL );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "button_press_event" from scrollbar
//-----------------------------------------------------------------------------

static gint gtk_scrollbar_button_press_callback( GtkRange *widget, GdkEventButton *gdk_event, wxWindow *win )
{
    if (gdk_event->window != widget->slider) return FALSE;

    win->m_isScrolling = TRUE;

    return FALSE;
}

//-----------------------------------------------------------------------------
// "button_release_event" from scrollbar
//-----------------------------------------------------------------------------

static gint gtk_scrollbar_button_release_callback( GtkRange *widget, GdkEventButton *gdk_event, wxWindow *win )
{
    if (gdk_event->window != widget->slider) return FALSE;

    GtkScrolledWindow *s_window = GTK_SCROLLED_WINDOW(win->m_widget);

    if (widget == GTK_RANGE(s_window->vscrollbar))
        gtk_signal_emit_by_name( GTK_OBJECT(win->m_hAdjust), "value_changed" );
    else
        gtk_signal_emit_by_name( GTK_OBJECT(win->m_vAdjust), "value_changed" );

    win->m_isScrolling = FALSE;

    return FALSE;
}


#ifdef NEW_GTK_DND_CODE

#else

//-----------------------------------------------------------------------------
// "drop_data_available_event"
//-----------------------------------------------------------------------------

static void gtk_window_drop_callback( GtkWidget *widget, GdkEventDropDataAvailable *event, wxWindow *win )
{
    if (!win->HasVMT()) return;

    if (win->GetDropTarget())
    {
        int x = 0;
        int y = 0;
        gdk_window_get_pointer( widget->window, &x, &y, (GdkModifierType *) NULL );

        printf( "Drop data is of type %s.\n", event->data_type );
  
        win->GetDropTarget()->OnDrop( x, y, (const void*)event->data, (size_t)event->data_numbytes );
  }

/*
  g_free (event->dropdataavailable.data);
  g_free (event->dropdataavailable.data_type);
*/
}

#endif
       // NEW_GTK_DND_CODE

//-----------------------------------------------------------------------------
// InsertChild for wxWindow.
//-----------------------------------------------------------------------------

// Callback for wxWindow. This very strange beast has to be used because
// C++ has no virtual methods in a constructor. We have to emulate a 
// virtual function here as wxNotebook requires a different way to insert
// a child in it. I had opted for creating a wxNotebookPage window class
// which would have made this superflouus (such in the MDI window system),
// but no-one is listening to me...

static void wxInsertChildInWindow( wxWindow* parent, wxWindow* child )
{
    gtk_myfixed_put( GTK_MYFIXED(parent->m_wxwindow), 
                     GTK_WIDGET(child->m_widget), 
  		     child->m_x, 
		     child->m_y );

    gtk_widget_set_usize( GTK_WIDGET(child->m_widget), 
                          child->m_width, 
			  child->m_height );
}

//-----------------------------------------------------------------------------
// wxWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindow,wxEvtHandler)

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
    EVT_SIZE(wxWindow::OnSize)
    EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindow::OnInitDialog)
    EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

wxWindow::wxWindow()
{
    m_widget = (GtkWidget *) NULL;
    m_wxwindow = (GtkWidget *) NULL;
    m_parent = (wxWindow *) NULL;
    m_children.DeleteContents( FALSE );
  
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
    m_minWidth = -1;
    m_minHeight = -1;
    m_maxWidth = -1;
    m_maxHeight = -1;
  
    m_retCode = 0;
  
    m_eventHandler = this;
    m_windowValidator = (wxValidator *) NULL;
  
    m_windowId = -1;
  
    m_cursor = (wxCursor *) NULL;
    m_font = *wxSWISS_FONT;
    m_windowStyle = 0;
    m_windowName = "noname";
  
    m_constraints = (wxLayoutConstraints *) NULL;
    m_constraintsInvolvedIn = (wxList *) NULL;
    m_windowSizer = (wxSizer *) NULL;
    m_sizerParent = (wxWindow *) NULL;
    m_autoLayout = FALSE;
  
    m_sizeSet = FALSE;
    m_hasVMT = FALSE;
    m_needParent = TRUE;
  
    m_hasScrolling = FALSE;
    m_isScrolling = FALSE;
    m_hAdjust = (GtkAdjustment*) NULL;
    m_vAdjust = (GtkAdjustment*) NULL;
    m_oldHorizontalPos = 0.0;
    m_oldVerticalPos = 0.0;
  
    m_isShown = FALSE;
    m_isEnabled = TRUE;
  
    m_dropTarget = (wxDropTarget*) NULL;
    m_resizing = FALSE;
    m_scrollGC = (GdkGC*) NULL;
    m_widgetStyle = (GtkStyle*) NULL;
  
    m_insertCallback = wxInsertChildInWindow;
  
    m_clientObject = (wxClientData*) NULL;
    m_clientData = NULL;
}

wxWindow::wxWindow( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size,
                    long style, const wxString &name  )
{
    m_insertCallback = wxInsertChildInWindow;
    Create( parent, id, pos, size, style, name );
}
  
bool wxWindow::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       long style, const wxString &name  )
{
    m_isShown = FALSE;
    m_isEnabled = TRUE;
    m_needParent = TRUE;

    PreCreation( parent, id, pos, size, style, name );

    m_widget = gtk_scrolled_window_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );
    m_hasScrolling = TRUE;

    GtkScrolledWindow *s_window = GTK_SCROLLED_WINDOW(m_widget);

    gtk_signal_connect( GTK_OBJECT(s_window->vscrollbar), "button_press_event",
          (GtkSignalFunc)gtk_scrollbar_button_press_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(s_window->hscrollbar), "button_press_event",
          (GtkSignalFunc)gtk_scrollbar_button_press_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(s_window->vscrollbar), "button_release_event",
          (GtkSignalFunc)gtk_scrollbar_button_release_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(s_window->hscrollbar), "button_release_event",
          (GtkSignalFunc)gtk_scrollbar_button_release_callback, (gpointer) this );

    GtkScrolledWindowClass *scroll_class = GTK_SCROLLED_WINDOW_CLASS( GTK_OBJECT(m_widget)->klass );
    scroll_class->scrollbar_spacing = 0;

    gtk_scrolled_window_set_policy( s_window, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    m_oldHorizontalPos = 0.0;
    m_oldVerticalPos = 0.0;

    m_hAdjust = gtk_range_get_adjustment( GTK_RANGE(s_window->hscrollbar) );
    m_vAdjust = gtk_range_get_adjustment( GTK_RANGE(s_window->vscrollbar) );

    gtk_signal_connect( GTK_OBJECT(m_hAdjust), "value_changed",
          (GtkSignalFunc) gtk_window_hscroll_callback, (gpointer) this );
    gtk_signal_connect( GTK_OBJECT(m_vAdjust), "value_changed",
          (GtkSignalFunc) gtk_window_vscroll_callback, (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_hAdjust), "changed",
          (GtkSignalFunc) gtk_window_hscroll_change_callback, (gpointer) this );
    gtk_signal_connect(GTK_OBJECT(m_vAdjust), "changed",
          (GtkSignalFunc) gtk_window_vscroll_change_callback, (gpointer) this );

    GtkViewport *viewport = GTK_VIEWPORT(s_window->viewport);

    if (m_windowStyle & wxRAISED_BORDER)
    {
        gtk_viewport_set_shadow_type( viewport, GTK_SHADOW_OUT );
    }
    else if (m_windowStyle & wxSUNKEN_BORDER)
    {
        gtk_viewport_set_shadow_type( viewport, GTK_SHADOW_IN );
    }
    else
    {
        gtk_viewport_set_shadow_type( viewport, GTK_SHADOW_NONE );
    }

    m_wxwindow = gtk_myfixed_new();

    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    if (m_windowStyle & wxTAB_TRAVERSAL == wxTAB_TRAVERSAL)
        GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
    else
        GTK_WIDGET_SET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );

    // shut the viewport up
    gtk_viewport_set_hadjustment( viewport, (GtkAdjustment*) gtk_adjustment_new( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) );
    gtk_viewport_set_vadjustment( viewport, (GtkAdjustment*) gtk_adjustment_new( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) );

    // I _really_ don't want scrollbars in the beginning
    m_vAdjust->lower = 0.0;
    m_vAdjust->upper = 1.0;
    m_vAdjust->value = 0.0;
    m_vAdjust->step_increment = 1.0;
    m_vAdjust->page_increment = 1.0;
    m_vAdjust->page_size = 5.0;
    gtk_signal_emit_by_name( GTK_OBJECT(m_vAdjust), "changed" );
    m_hAdjust->lower = 0.0;
    m_hAdjust->upper = 1.0;
    m_hAdjust->value = 0.0;
    m_hAdjust->step_increment = 1.0;
    m_hAdjust->page_increment = 1.0;
    m_hAdjust->page_size = 5.0;
    gtk_signal_emit_by_name( GTK_OBJECT(m_hAdjust), "changed" );

    gtk_widget_show( m_wxwindow );
  
    if (m_parent) m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();
  
    Show( TRUE );

    return TRUE;
}

wxWindow::~wxWindow()
{
  m_hasVMT = FALSE;

  if (m_dropTarget) delete m_dropTarget;

  if (m_parent) m_parent->RemoveChild( this );
  if (m_widget) Show( FALSE );

  DestroyChildren();

  if (m_widgetStyle) gtk_style_unref( m_widgetStyle );
  
  if (m_scrollGC) gdk_gc_unref( m_scrollGC );
    
  if (m_wxwindow) gtk_widget_destroy( m_wxwindow );

  if (m_widget) gtk_widget_destroy( m_widget );

  wxDELETE(m_cursor);

  DeleteRelatedConstraints();
  if (m_constraints)
  {
    // This removes any dangling pointers to this window
    // in other windows' constraintsInvolvedIn lists.
    UnsetConstraints(m_constraints);
    delete m_constraints;
    m_constraints = (wxLayoutConstraints *) NULL;
  }
  if (m_windowSizer)
  {
    delete m_windowSizer;
    m_windowSizer = (wxSizer *) NULL;
  }
  // If this is a child of a sizer, remove self from parent
  if (m_sizerParent) m_sizerParent->RemoveChild((wxWindow *)this);

  // Just in case the window has been Closed, but
  // we're then deleting immediately: don't leave
  // dangling pointers.
  wxPendingDelete.DeleteObject(this);

  // Just in case we've loaded a top-level window via
  // wxWindow::LoadNativeDialog but we weren't a dialog
  // class
  wxTopLevelWindows.DeleteObject(this);

  if (m_windowValidator) delete m_windowValidator;
  
  if (m_clientObject) delete m_clientObject;
}

void wxWindow::PreCreation( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  if (m_needParent && (parent == NULL))
    wxFatalError( "Need complete parent.", name );
    
  m_widget = (GtkWidget*) NULL;
  m_wxwindow = (GtkWidget*) NULL;
  m_hasVMT = FALSE;
  m_parent = parent;
  m_children.DeleteContents( FALSE );
  
  m_width = size.x;
  if (m_width == -1) m_width = 20;
  m_height = size.y;
  if (m_height == -1) m_height = 20;
  
  m_x = (int)pos.x;
  m_y = (int)pos.y;
  
  if (!m_needParent)  // some reasonable defaults
  {
    if (m_x == -1)
    {
      m_x = (gdk_screen_width () - m_width) / 2;
      if (m_x < 10) m_x = 10;
    }
    if (m_y == -1)
    {
      m_y = (gdk_screen_height () - m_height) / 2;
      if (m_y < 10) m_y = 10;
    }
  }
  
  m_minWidth = -1;
  m_minHeight = -1;
  m_maxWidth = -1;
  m_maxHeight = -1;
  
  m_retCode = 0;
  
  m_eventHandler = this;
  
  m_windowId = id;
  
  m_sizeSet = FALSE;
  
  m_cursor = new wxCursor( wxCURSOR_ARROW );
  m_font = *wxSWISS_FONT;
//  m_backgroundColour = wxWHITE;
//  m_foregroundColour = wxBLACK;
  m_windowStyle = style;
  m_windowName = name;
  
  m_constraints = (wxLayoutConstraints *) NULL;
  m_constraintsInvolvedIn = (wxList *) NULL;
  m_windowSizer = (wxSizer *) NULL;
  m_sizerParent = (wxWindow *) NULL;
  m_autoLayout = FALSE;
  
  m_hasScrolling = FALSE;
  m_isScrolling = FALSE;
  m_hAdjust = (GtkAdjustment *) NULL;
  m_vAdjust = (GtkAdjustment *) NULL;
  m_oldHorizontalPos = 0.0;
  m_oldVerticalPos = 0.0;
  
  m_isShown = FALSE;
  m_isEnabled = TRUE;
  
  m_dropTarget = (wxDropTarget *) NULL;
  m_resizing = FALSE;
  m_windowValidator = (wxValidator *) NULL;
  m_scrollGC = (GdkGC*) NULL;
  m_widgetStyle = (GtkStyle*) NULL;
  
  m_clientObject = (wxClientData*)NULL;
  m_clientData = NULL;
}

void wxWindow::PostCreation()
{
  if (m_wxwindow)
  {
    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "expose_event",
      GTK_SIGNAL_FUNC(gtk_window_expose_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "draw",
      GTK_SIGNAL_FUNC(gtk_window_draw_callback), (gpointer)this );
  }

  ConnectWidget( GetConnectWidget() );

  if (m_widget && m_parent) gtk_widget_realize( m_widget );

  if (m_wxwindow) gtk_widget_realize( m_wxwindow );

  SetCursor( *wxSTANDARD_CURSOR );

  m_hasVMT = TRUE;
}

void wxWindow::ConnectWidget( GtkWidget *widget )
{
  gtk_signal_connect( GTK_OBJECT(widget), "key_press_event",
    GTK_SIGNAL_FUNC(gtk_window_key_press_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "button_press_event",
    GTK_SIGNAL_FUNC(gtk_window_button_press_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "button_release_event",
    GTK_SIGNAL_FUNC(gtk_window_button_release_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "motion_notify_event",
    GTK_SIGNAL_FUNC(gtk_window_motion_notify_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "focus_in_event",
    GTK_SIGNAL_FUNC(gtk_window_focus_in_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "focus_out_event",
    GTK_SIGNAL_FUNC(gtk_window_focus_out_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "enter_notify_event",
    GTK_SIGNAL_FUNC(gtk_window_enter_callback), (gpointer)this );

  gtk_signal_connect( GTK_OBJECT(widget), "leave_notify_event",
    GTK_SIGNAL_FUNC(gtk_window_leave_callback), (gpointer)this );
}

bool wxWindow::HasVMT()
{
  return m_hasVMT;
}

bool wxWindow::Close( bool force )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
  event.SetForce(force);

  return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::Destroy()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  m_hasVMT = FALSE;
  delete this;
  return TRUE;
}

bool wxWindow::DestroyChildren()
{
  if (GetChildren())
  {
    wxNode *node;
    while ((node = GetChildren()->First()) != (wxNode *)NULL)
    {
      wxWindow *child;
      if ((child = (wxWindow *)node->Data()) != (wxWindow *)NULL)
      {
        delete child;
        if (GetChildren()->Member(child)) delete node;
      }
    }
  }
  return TRUE;
}

void wxWindow::PrepareDC( wxDC &WXUNUSED(dc) )
{
  // are we to set fonts here ?
}

wxPoint wxWindow::GetClientAreaOrigin() const
{
  return wxPoint(0,0);
}

void wxWindow::AdjustForParentClientOrigin( int& x, int& y, int sizeFlags )
{
  if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
  {
      wxPoint pt(GetParent()->GetClientAreaOrigin());
      x += pt.x; 
      y += pt.y;
  }
}

void wxWindow::SetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid window" );
    wxASSERT_MSG( (m_parent != NULL), "wxWindow::SetSize requires parent.\n" );
  
    // Don't do anything for children of wxNotebook
    if (m_parent->m_wxwindow == NULL) return;

    if (m_resizing) return; // I don't like recursions
    m_resizing = TRUE;

    int old_width = m_width;
    int old_height = m_height;
  
    if ((sizeFlags & wxSIZE_USE_EXISTING) == wxSIZE_USE_EXISTING)
    {
        if (x != -1) m_x = x;
        if (y != -1) m_y = y;
        if (width != -1) m_width = width;
        if (height != -1) m_height = height;
    }
    else
    {
        m_x = x;
        m_y = y;
        m_width = width;
       m_height = height;
    }

    if ((sizeFlags & wxSIZE_AUTO_WIDTH) == wxSIZE_AUTO_WIDTH)
    {
        if (width == -1) m_width = 80;
    }

    if ((sizeFlags & wxSIZE_AUTO_HEIGHT) == wxSIZE_AUTO_HEIGHT)
    {
        if (height == -1) m_height = 26;
    }
  
    if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
    if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_minWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_minHeight;

    wxPoint pt( m_parent->GetClientAreaOrigin() );
    gtk_myfixed_move( GTK_MYFIXED(m_parent->m_wxwindow), m_widget, m_x+pt.x, m_y+pt.y );
  
    if ((old_width != m_width) || (old_height != m_height))
        gtk_widget_set_usize( m_widget, m_width, m_height );
  
    m_sizeSet = TRUE;

    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    ProcessEvent( event );

    m_resizing = FALSE;
}

void wxWindow::SetSize( int width, int height )
{
    SetSize( -1, -1, width, height, wxSIZE_USE_EXISTING );
}

void wxWindow::Move( int x, int y )
{
    SetSize( x, y, -1, -1, wxSIZE_USE_EXISTING );
}

void wxWindow::GetSize( int *width, int *height ) const
{
    wxASSERT_MSG( (m_widget != NULL), "invalid window" );

    if (width) (*width) = m_width;
    if (height) (*height) = m_height;
}

void wxWindow::SetClientSize( int width, int height )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (!m_wxwindow)
  {
    SetSize( width, height );
  }
  else
  {
    int dw = 0;
    int dh = 0;

    if (!m_hasScrolling)
    {
/*
      do we have sunken dialogs ?

      GtkStyleClass *window_class = m_wxwindow->style->klass;

      dw += 2 * window_class->xthickness;
      dh += 2 * window_class->ythickness;
*/
    }
    else
    {
      GtkScrolledWindow *scroll_window = GTK_SCROLLED_WINDOW(m_widget);
      GtkScrolledWindowClass *scroll_class = GTK_SCROLLED_WINDOW_CLASS( GTK_OBJECT(m_widget)->klass );

      GtkWidget *viewport = scroll_window->viewport;
      GtkStyleClass *viewport_class = viewport->style->klass;

      GtkWidget *hscrollbar = scroll_window->hscrollbar;
      GtkWidget *vscrollbar = scroll_window->vscrollbar;

      if ((m_windowStyle & wxRAISED_BORDER) ||
          (m_windowStyle & wxSUNKEN_BORDER))
      {
        dw += 2 * viewport_class->xthickness;
        dh += 2 * viewport_class->ythickness;
      }

      if (GTK_WIDGET_VISIBLE(vscrollbar))
      {
        dw += vscrollbar->allocation.width;
        dw += scroll_class->scrollbar_spacing;
      }

      if (GTK_WIDGET_VISIBLE(hscrollbar))
      {
        dh += hscrollbar->allocation.height;
        dw += scroll_class->scrollbar_spacing;
      }
    }

    SetSize( width+dw, height+dh );
  }
}

void wxWindow::GetClientSize( int *width, int *height ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (!m_wxwindow)
  {
    if (width) (*width) = m_width;
    if (height) (*height) = m_height;
  }
  else
  {
    int dw = 0;
    int dh = 0;

    if (!m_hasScrolling)
    {
/*
      do we have sunken dialogs ?

      GtkStyleClass *window_class = m_wxwindow->style->klass;

      dw += 2 * window_class->xthickness;
      dh += 2 * window_class->ythickness;
*/
    }
    else
    {
      GtkScrolledWindow *scroll_window = GTK_SCROLLED_WINDOW(m_widget);
      GtkScrolledWindowClass *scroll_class = GTK_SCROLLED_WINDOW_CLASS( GTK_OBJECT(m_widget)->klass );

      GtkWidget *viewport = scroll_window->viewport;
      GtkStyleClass *viewport_class = viewport->style->klass;

      GtkWidget *hscrollbar = scroll_window->hscrollbar;
      GtkWidget *vscrollbar = scroll_window->vscrollbar;

      if ((m_windowStyle & wxRAISED_BORDER) ||
          (m_windowStyle & wxSUNKEN_BORDER))
      {
        dw += 2 * viewport_class->xthickness;
        dh += 2 * viewport_class->ythickness;
      }

      if (GTK_WIDGET_VISIBLE(vscrollbar))
      {
//        dw += vscrollbar->allocation.width;
        dw += 15;                               // range.slider_width = 11 + 2*2pts edge
        dw += scroll_class->scrollbar_spacing;
      }

      if (GTK_WIDGET_VISIBLE(hscrollbar))
      {
//        dh += hscrollbar->allocation.height;
        dh += 15;
        dh += scroll_class->scrollbar_spacing;
      }
    }

    if (width) (*width) = m_width - dw;
    if (height) (*height) = m_height - dh;
  }
}

void wxWindow::GetPosition( int *x, int *y ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (x) (*x) = m_x;
  if (y) (*y) = m_y;
}

void wxWindow::ClientToScreen( int *x, int *y )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  GdkWindow *source = (GdkWindow *) NULL;
  if (m_wxwindow)
    source = m_wxwindow->window;
  else
    source = m_widget->window;

  int org_x = 0;
  int org_y = 0;
  gdk_window_get_origin( source, &org_x, &org_y );

  if (!m_wxwindow)
  {
    if (GTK_WIDGET_NO_WINDOW (m_widget))
    {
      org_x += m_widget->allocation.x;
      org_y += m_widget->allocation.y;
    }
  }

  wxPoint pt(GetClientAreaOrigin());
  org_x += pt.x;
  org_y += pt.y;
  
  if (x) *x += org_x;
  if (y) *y += org_y;
}

void wxWindow::ScreenToClient( int *x, int *y )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  GdkWindow *source = (GdkWindow *) NULL;
  if (m_wxwindow)
    source = m_wxwindow->window;
  else
    source = m_widget->window;

  int org_x = 0;
  int org_y = 0;
  gdk_window_get_origin( source, &org_x, &org_y );

  if (!m_wxwindow)
  {
    if (GTK_WIDGET_NO_WINDOW (m_widget))
    {
      org_x += m_widget->allocation.x;
      org_y += m_widget->allocation.y;
    }
  }

  wxPoint pt(GetClientAreaOrigin());
  org_x -= pt.x;
  org_y -= pt.y;
  
  if (x) *x -= org_x;
  if (y) *y -= org_y;
}

void wxWindow::Centre( int direction )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  int x = m_x;
  int y = m_y;
  
  if (m_parent)
  {
      int p_w = 0;
      int p_h = 0;
      m_parent->GetSize( &p_w, &p_h );
      if (direction & wxHORIZONTAL == wxHORIZONTAL) x = (p_w - m_width) / 2;
      if (direction & wxVERTICAL == wxVERTICAL) y = (p_h - m_height) / 2;
  }
  else
  {
      if (direction & wxHORIZONTAL == wxHORIZONTAL) x = (gdk_screen_width () - m_width) / 2;
      if (direction & wxVERTICAL == wxVERTICAL) y = (gdk_screen_height () - m_height) / 2;
  }
  
  Move( x, y );
}

void wxWindow::Fit()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  int maxX = 0;
  int maxY = 0;
  wxNode *node = GetChildren()->First();
  while ( node )
  {
    wxWindow *win = (wxWindow *)node->Data();
    int wx, wy, ww, wh;
    win->GetPosition(&wx, &wy);
    win->GetSize(&ww, &wh);
    if ( wx + ww > maxX )
      maxX = wx + ww;
    if ( wy + wh > maxY )
      maxY = wy + wh;

    node = node->Next();
  }
  SetClientSize(maxX + 5, maxY + 10);
}

void wxWindow::SetSizeHints( int minW, int minH, int maxW, int maxH, int WXUNUSED(incW), int WXUNUSED(incH) )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  m_minWidth = minW;
  m_minHeight = minH;
  m_maxWidth = maxW;
  m_maxHeight = maxH;
}

void wxWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  //if (GetAutoLayout()) Layout();
}

bool wxWindow::Show( bool show )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (show)
    gtk_widget_show( m_widget );
  else
    gtk_widget_hide( m_widget );
  m_isShown = show;
  return TRUE;
}

void wxWindow::Enable( bool enable )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  m_isEnabled = enable;
  gtk_widget_set_sensitive( m_widget, enable );
  if (m_wxwindow) gtk_widget_set_sensitive( m_wxwindow, enable );
}

int wxWindow::GetCharHeight() const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (!m_font.Ok())
  {
    wxFAIL_MSG( "invalid font" );
    return -1;
  }

  GdkFont *font = m_font.GetInternalFont( 1.0 );
  return font->ascent + font->descent;
}

int wxWindow::GetCharWidth() const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (!m_font.Ok())
  {
    wxFAIL_MSG( "invalid font" );
    return -1;
  }

  GdkFont *font = m_font.GetInternalFont( 1.0 );
  return gdk_string_width( font, "H" );
}

void wxWindow::GetTextExtent( const wxString& string, int *x, int *y,
  int *descent, int *externalLeading, const wxFont *theFont, bool WXUNUSED(use16) ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxFont fontToUse = m_font;
  if (theFont) fontToUse = *theFont;

  if (!fontToUse.Ok())
  {
    wxFAIL_MSG( "invalid font" );
    return;
  }
  wxASSERT_MSG( (m_font.Ok()), "invalid font" );

  GdkFont *font = fontToUse.GetInternalFont( 1.0 );
  if (x) (*x) = gdk_string_width( font, string );
  if (y) (*y) = font->ascent + font->descent;
  if (descent) (*descent) = font->descent;
  if (externalLeading) (*externalLeading) = 0;  // ??
}

void wxWindow::MakeModal( bool modal )
{
  return;
  // Disable all other windows
  if (this->IsKindOf(CLASSINFO(wxDialog)) || this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxNode *node = wxTopLevelWindows.First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win != this)
        win->Enable(!modal);

      node = node->Next();
    }
  }
}

void wxWindow::SetFocus()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  GtkWidget *connect_widget = GetConnectWidget();
  if (connect_widget)
  {
    if (GTK_WIDGET_CAN_FOCUS(connect_widget) && !GTK_WIDGET_HAS_FOCUS (connect_widget) )
    {
      gtk_widget_grab_focus (connect_widget);
    }
  }
}

bool wxWindow::OnClose()
{
  return TRUE;
}

void wxWindow::AddChild( wxWindow *child )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );
  wxASSERT_MSG( (child != NULL), "invalid child" );

  m_children.Append( child );
}

wxList *wxWindow::GetChildren()
{
  return (&m_children);
}

void wxWindow::RemoveChild( wxWindow *child )
{
  if (GetChildren()) GetChildren()->DeleteObject( child );
  child->m_parent = (wxWindow *) NULL;
}

void wxWindow::SetReturnCode( int retCode )
{
  m_retCode = retCode;
}

int wxWindow::GetReturnCode()
{
  return m_retCode;
}

void wxWindow::Raise()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (m_widget) gdk_window_raise( m_widget->window );
}

void wxWindow::Lower()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (m_widget) gdk_window_lower( m_widget->window );
}

wxEvtHandler *wxWindow::GetEventHandler()
{
  return m_eventHandler;
}

void wxWindow::SetEventHandler( wxEvtHandler *handler )
{
  m_eventHandler = handler;
}

void wxWindow::PushEventHandler(wxEvtHandler *handler)
{
  handler->SetNextHandler(GetEventHandler());
  SetEventHandler(handler);
}

wxEvtHandler *wxWindow::PopEventHandler(bool deleteHandler)
{
  if (GetEventHandler())
  {
    wxEvtHandler *handlerA = GetEventHandler();
    wxEvtHandler *handlerB = handlerA->GetNextHandler();
    handlerA->SetNextHandler((wxEvtHandler *) NULL);
    SetEventHandler(handlerB);
    if (deleteHandler)
    {
      delete handlerA;
      return (wxEvtHandler*) NULL;
    }
    else
      return handlerA;
  }
  else
   return (wxEvtHandler *) NULL;
}

wxValidator *wxWindow::GetValidator()
{
  return m_windowValidator;
}

void wxWindow::SetValidator( const wxValidator& validator )
{
  if (m_windowValidator) delete m_windowValidator;
  m_windowValidator = validator.Clone();
  if (m_windowValidator) m_windowValidator->SetWindow(this);
}

void wxWindow::SetClientObject( wxClientData *data )
{
  if (m_clientObject) delete m_clientObject;
  m_clientObject = data;
}

wxClientData *wxWindow::GetClientObject()
{
  return m_clientObject;
}

void wxWindow::SetClientData( void *data )
{
  m_clientData = data;
}

void *wxWindow::GetClientData()
{
  return m_clientData;
}

bool wxWindow::IsBeingDeleted()
{
  return FALSE;
}

void wxWindow::SetId( wxWindowID id )
{
  m_windowId = id;
}

wxWindowID wxWindow::GetId()
{
  return m_windowId;
}

void wxWindow::SetCursor( const wxCursor &cursor )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (m_cursor == NULL)
  {
    wxFAIL_MSG( "wxWindow::SetCursor m_cursor == NULL" );
    m_cursor = new wxCursor( wxCURSOR_ARROW );
  }

  if (cursor.Ok())
  {
    if (*((wxCursor*)&cursor) == m_cursor) return;
    *m_cursor = cursor;
  }
  else
  {
    *m_cursor = *wxSTANDARD_CURSOR;
  }

  if ((m_widget) && (m_widget->window))
    gdk_window_set_cursor( m_widget->window, m_cursor->GetCursor() );

  if ((m_wxwindow) && (m_wxwindow->window))
    gdk_window_set_cursor( m_wxwindow->window, m_cursor->GetCursor() );
}

void wxWindow::Refresh( bool eraseBackground, const wxRect *rect )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  if (eraseBackground && m_wxwindow && m_wxwindow->window)
  {
    if (rect)
      gdk_window_clear_area( m_wxwindow->window,
        rect->x,
        rect->y,
        rect->width,
        rect->height );
    else
      Clear();
  }
  if (!rect)
  {
    if (m_wxwindow)
    {
      int w = 0;
      int h = 0;
      GetClientSize( &w, &h );

      GdkRectangle gdk_rect;
      gdk_rect.x = 0;
      gdk_rect.y = 0;
      gdk_rect.width = w;
      gdk_rect.height = h;
      gtk_widget_draw( m_wxwindow, &gdk_rect );
    }
  }
  else
  {
    GdkRectangle gdk_rect;
    gdk_rect.x = rect->x;
    gdk_rect.y = rect->y;
    gdk_rect.width = rect->width;
    gdk_rect.height = rect->height;

    if (m_wxwindow)
      gtk_widget_draw( m_wxwindow, &gdk_rect );
    else
      gtk_widget_draw( m_widget, &gdk_rect );
  }
}

wxRegion wxWindow::GetUpdateRegion() const
{
  return m_updateRegion;
}

bool wxWindow::IsExposed( int x, int y) const
{
  return (m_updateRegion.Contains( x, y ) != wxOutRegion );
}

bool wxWindow::IsExposed( int x, int y, int w, int h ) const
{
  return (m_updateRegion.Contains( x, y, w, h ) != wxOutRegion );
}

bool wxWindow::IsExposed( const wxPoint& pt ) const
{
  return (m_updateRegion.Contains( pt.x, pt.y ) != wxOutRegion );
}

bool wxWindow::IsExposed( const wxRect& rect ) const
{
  return (m_updateRegion.Contains( rect.x, rect.y, rect.width, rect.height ) != wxOutRegion );
}

void wxWindow::Clear()
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  if (m_wxwindow && m_wxwindow->window) gdk_window_clear( m_wxwindow->window );
}

wxColour wxWindow::GetBackgroundColour() const
{
  return m_backgroundColour;
}

void wxWindow::SetBackgroundColour( const wxColour &colour )
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  m_backgroundColour = colour;
  if (!m_backgroundColour.Ok()) return;
  
  if (m_wxwindow)
  {
    GdkWindow *window = m_wxwindow->window;
    m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
    gdk_window_set_background( window, m_backgroundColour.GetColor() );
    gdk_window_clear( window );
  }
  
  ApplyWidgetStyle();
}

wxColour wxWindow::GetForegroundColour() const
{
  return m_foregroundColour;
}

void wxWindow::SetForegroundColour( const wxColour &colour )
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  m_foregroundColour = colour;
  if (!m_foregroundColour.Ok()) return;
  
  ApplyWidgetStyle();
}

GtkStyle *wxWindow::GetWidgetStyle()
{
  if (m_widgetStyle) gtk_style_unref( m_widgetStyle );
  
  m_widgetStyle = 
    gtk_style_copy( 
      gtk_widget_get_style( m_widget ) );
      
  return m_widgetStyle;
}

void wxWindow::SetWidgetStyle()
{
  GtkStyle *style = GetWidgetStyle();
  
  gdk_font_unref( style->font );
  style->font = gdk_font_ref( m_font.GetInternalFont( 1.0 ) );
  
  if (m_foregroundColour.Ok())
  {
    m_foregroundColour.CalcPixel( gdk_window_get_colormap( m_widget->window ) );
    style->fg[GTK_STATE_NORMAL] = *m_foregroundColour.GetColor();
    style->fg[GTK_STATE_PRELIGHT] = *m_foregroundColour.GetColor();
    style->fg[GTK_STATE_ACTIVE] = *m_foregroundColour.GetColor();
  }
  
  if (m_backgroundColour.Ok())
  {
    m_backgroundColour.CalcPixel( gdk_window_get_colormap( m_widget->window ) );
    style->bg[GTK_STATE_NORMAL] = *m_backgroundColour.GetColor();
    style->base[GTK_STATE_NORMAL] = *m_backgroundColour.GetColor();
    style->bg[GTK_STATE_PRELIGHT] = *m_backgroundColour.GetColor();
    style->base[GTK_STATE_PRELIGHT] = *m_backgroundColour.GetColor();
    style->bg[GTK_STATE_ACTIVE] = *m_backgroundColour.GetColor();
    style->base[GTK_STATE_ACTIVE] = *m_backgroundColour.GetColor();
    style->bg[GTK_STATE_INSENSITIVE] = *m_backgroundColour.GetColor();
    style->base[GTK_STATE_INSENSITIVE] = *m_backgroundColour.GetColor();
  }
}

void wxWindow::ApplyWidgetStyle()
{
}

bool wxWindow::Validate()
{
  wxCHECK_MSG( m_widget != NULL, FALSE, "invalid window" );

  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if (child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->Validate(this))
      { return FALSE; }
    node = node->Next();
  }
  return TRUE;
}

bool wxWindow::TransferDataToWindow()
{
  wxCHECK_MSG( m_widget != NULL, FALSE, "invalid window" );

  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if (child->GetValidator() && /* child->GetValidator()->Ok() && */
  !child->GetValidator()->TransferToWindow() )
    {
      wxMessageBox( _("Application Error"), _("Could not transfer data to window"), wxOK|wxICON_EXCLAMATION );
      return FALSE;
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxWindow::TransferDataFromWindow()
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->TransferFromWindow() )
      { return FALSE; }
   node = node->Next();
  }
  return TRUE;
}

void wxWindow::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
  m_acceleratorTable = accel;
}

void wxWindow::OnInitDialog( wxInitDialogEvent &WXUNUSED(event) )
{
  TransferDataToWindow();
}

void wxWindow::InitDialog()
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  wxInitDialogEvent event(GetId());
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
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

bool wxWindow::PopupMenu( wxMenu *menu, int WXUNUSED(x), int WXUNUSED(y) )
{
  wxCHECK_MSG( m_widget != NULL, FALSE, "invalid window" );

  wxCHECK_MSG( menu != NULL, FALSE, "invalid popup-menu" );
  
  SetInvokingWindow( menu, this );
  gtk_menu_popup(
                  GTK_MENU(menu->m_menu),
                  (GtkWidget *)NULL,          // parent menu shell
                  (GtkWidget *)NULL,          // parent menu item
                  (GtkMenuPositionFunc)NULL,
                  NULL,                       // client data
                  0,                          // button used to activate it
                  0//gs_timeLastClick            // the time of activation
                );
  return TRUE;
}

void wxWindow::SetDropTarget( wxDropTarget *dropTarget )
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  GtkWidget *dnd_widget = GetConnectWidget();

  DisconnectDnDWidget( dnd_widget );

  if (m_dropTarget) delete m_dropTarget;
  m_dropTarget = dropTarget;

  ConnectDnDWidget( dnd_widget );
}

wxDropTarget *wxWindow::GetDropTarget() const
{
  return m_dropTarget;
}

void wxWindow::ConnectDnDWidget( GtkWidget *widget )
{
  if (!m_dropTarget) return;

  m_dropTarget->RegisterWidget( widget );

#ifdef NEW_GTK_DND_CODE

#else

  gtk_signal_connect( GTK_OBJECT(widget), "drop_data_available_event",
    GTK_SIGNAL_FUNC(gtk_window_drop_callback), (gpointer)this );
    
#endif
    
}

void wxWindow::DisconnectDnDWidget( GtkWidget *widget )
{
  if (!m_dropTarget) return;

#ifdef NEW_GTK_DND_CODE

#else

  gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
    GTK_SIGNAL_FUNC(gtk_window_drop_callback), (gpointer)this );

  m_dropTarget->UnregisterWidget( widget );
  
#endif

}

GtkWidget* wxWindow::GetConnectWidget()
{
  GtkWidget *connect_widget = m_widget;
  if (m_wxwindow) connect_widget = m_wxwindow;

  return connect_widget;
}

bool wxWindow::IsOwnGtkWindow( GdkWindow *window )
{
  if (m_wxwindow) return (window == m_wxwindow->window);
  return (window == m_widget->window);
}

void wxWindow::SetFont( const wxFont &font )
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  if (((wxFont*)&font)->Ok())
    m_font = font;
  else
    m_font = *wxSWISS_FONT;
    
  ApplyWidgetStyle();
}

wxFont *wxWindow::GetFont()
{
  return &m_font;
}

void wxWindow::SetWindowStyleFlag( long flag )
{
  m_windowStyle = flag;
}

long wxWindow::GetWindowStyleFlag() const
{
  return m_windowStyle;
}

void wxWindow::CaptureMouse()
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  wxCHECK_RET( g_capturing == FALSE, "CaptureMouse called twice" );

  GtkWidget *connect_widget = GetConnectWidget();
  gtk_grab_add( connect_widget );
  gdk_pointer_grab ( connect_widget->window, FALSE,
                    (GdkEventMask)
        (GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_POINTER_MOTION_MASK),
        (GdkWindow *) NULL, (GdkCursor *) NULL, GDK_CURRENT_TIME );
  g_capturing = TRUE;
}

void wxWindow::ReleaseMouse()
{
  wxCHECK_RET( m_widget != NULL, "invalid window" );

  wxCHECK_RET( g_capturing == TRUE, "ReleaseMouse called twice" );

  GtkWidget *connect_widget = GetConnectWidget();
  gtk_grab_remove( connect_widget );
  gdk_pointer_ungrab ( GDK_CURRENT_TIME );
  g_capturing = FALSE;
}

void wxWindow::SetTitle( const wxString &WXUNUSED(title) )
{
}

wxString wxWindow::GetTitle() const
{
  return (wxString&)m_windowName;
}

wxString wxWindow::GetLabel() const
{
  return GetTitle();
}

void wxWindow::SetName( const wxString &name )
{
  m_windowName = name;
}

wxString wxWindow::GetName() const
{
  return (wxString&)m_windowName;
}

bool wxWindow::IsShown() const
{
  return m_isShown;
}

bool wxWindow::IsRetained()
{
  return FALSE;
}

wxWindow *wxWindow::FindWindow( long id )
{
  if (id == m_windowId) return this;
  wxNode *node = m_children.First();
  while (node)
  {
    wxWindow *child = (wxWindow*)node->Data();
    wxWindow *res = child->FindWindow( id );
    if (res) return res;
    node = node->Next();
  }
  return (wxWindow *) NULL;
}

wxWindow *wxWindow::FindWindow( const wxString& name )
{
  if (name == m_windowName) return this;
  wxNode *node = m_children.First();
  while (node)
  {
    wxWindow *child = (wxWindow*)node->Data();
    wxWindow *res = child->FindWindow( name );
    if (res) return res;
    node = node->Next();
  }
  return (wxWindow *) NULL;
}

void wxWindow::SetScrollbar( int orient, int pos, int thumbVisible,
      int range, bool refresh )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return;

  if (orient == wxHORIZONTAL)
  {
    float fpos = (float)pos;
    float frange = (float)range;
    float fthumb = (float)thumbVisible;

    if ((fabs(frange-m_hAdjust->upper) < 0.2) &&
        (fabs(fthumb-m_hAdjust->page_size) < 0.2))
    {
      SetScrollPos( orient, pos, refresh );
      return;
    }

    m_oldHorizontalPos = fpos;

    m_hAdjust->lower = 0.0;
    m_hAdjust->upper = frange;
    m_hAdjust->value = fpos;
    m_hAdjust->step_increment = 1.0;
    m_hAdjust->page_increment = (float)(wxMax(fthumb,0));
    m_hAdjust->page_size = fthumb;
  }
  else
  {
    float fpos = (float)pos;
    float frange = (float)range;
    float fthumb = (float)thumbVisible;

    if ((fabs(frange-m_vAdjust->upper) < 0.2) &&
        (fabs(fthumb-m_vAdjust->page_size) < 0.2))
    {
      SetScrollPos( orient, pos, refresh );
      return;
    }

    m_oldVerticalPos = fpos;

    m_vAdjust->lower = 0.0;
    m_vAdjust->upper = frange;
    m_vAdjust->value = fpos;
    m_vAdjust->step_increment = 1.0;
    m_vAdjust->page_increment = (float)(wxMax(fthumb,0));
    m_vAdjust->page_size = fthumb;
  }

  if (m_wxwindow->window)
  {
    if (orient == wxHORIZONTAL)
      gtk_signal_emit_by_name( GTK_OBJECT(m_hAdjust), "changed" );
    else
      gtk_signal_emit_by_name( GTK_OBJECT(m_vAdjust), "changed" );

    gtk_widget_set_usize( m_widget, m_width, m_height );
  }
}

void wxWindow::SetScrollPos( int orient, int pos, bool WXUNUSED(refresh) )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return;

  if (orient == wxHORIZONTAL)
  {
    float fpos = (float)pos;
    m_oldHorizontalPos = fpos;

    if (fabs(fpos-m_hAdjust->value) < 0.2) return;
    m_hAdjust->value = fpos;
  }
  else
  {
    float fpos = (float)pos;
    m_oldVerticalPos = fpos;
    if (fabs(fpos-m_vAdjust->value) < 0.2) return;
    m_vAdjust->value = fpos;
  }

  if (!m_isScrolling)
  {
    if (m_wxwindow->window)
    {
      if (orient == wxHORIZONTAL)
        gtk_signal_emit_by_name( GTK_OBJECT(m_hAdjust), "value_changed" );
      else
        gtk_signal_emit_by_name( GTK_OBJECT(m_vAdjust), "value_changed" );
    }
  }
}

int wxWindow::GetScrollThumb( int orient ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return 0;

  if (orient == wxHORIZONTAL)
    return (int)(m_hAdjust->page_size+0.5);
  else
    return (int)(m_vAdjust->page_size+0.5);
}

int wxWindow::GetScrollPos( int orient ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return 0;

  if (orient == wxHORIZONTAL)
    return (int)(m_hAdjust->value+0.5);
  else
    return (int)(m_vAdjust->value+0.5);
}

int wxWindow::GetScrollRange( int orient ) const
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return 0;

  if (orient == wxHORIZONTAL)
    return (int)(m_hAdjust->upper+0.5);
  else
    return (int)(m_vAdjust->upper+0.5);
}

void wxWindow::ScrollWindow( int dx, int dy, const wxRect* WXUNUSED(rect) )
{
  wxASSERT_MSG( (m_widget != NULL), "invalid window" );

  wxASSERT_MSG( (m_wxwindow != NULL), "window needs client area" );

  if (!m_wxwindow) return;

    int cw = 0;
    int ch = 0;
    GetClientSize( &cw, &ch );

    int w = cw - abs(dx);
    int h = ch - abs(dy);
    if ((h < 0) || (w < 0))
    {
      Refresh();
      return;
    }
    int s_x = 0;
    int s_y = 0;
    if (dx < 0) s_x = -dx;
    if (dy < 0) s_y = -dy;
    int d_x = 0;
    int d_y = 0;
    if (dx > 0) d_x = dx;
    if (dy > 0) d_y = dy;
    
    if (!m_scrollGC)
    {
      m_scrollGC = gdk_gc_new( m_wxwindow->window );
      gdk_gc_set_exposures( m_scrollGC, TRUE );
    }
    
    gdk_window_copy_area( m_wxwindow->window, m_scrollGC, d_x, d_y,
      m_wxwindow->window, s_x, s_y, w, h );

    wxRect rect;
    if (dx < 0) rect.x = cw+dx; else rect.x = 0;
    if (dy < 0) rect.y = ch+dy; else rect.y = 0;
    if (dy != 0) rect.width = cw; else rect.width = abs(dx);
    if (dx != 0) rect.height = ch; else rect.height = abs(dy);

    Refresh( TRUE, &rect );
}

//-------------------------------------------------------------------------------------
//          Layout
//-------------------------------------------------------------------------------------

wxLayoutConstraints *wxWindow::GetConstraints() const
{
  return m_constraints;
}

void wxWindow::SetConstraints( wxLayoutConstraints *constraints )
{
  if (m_constraints)
  {
    UnsetConstraints(m_constraints);
    delete m_constraints;
  }
  m_constraints = constraints;
  if (m_constraints)
  {
    // Make sure other windows know they're part of a 'meaningful relationship'
    if (m_constraints->left.GetOtherWindow() && (m_constraints->left.GetOtherWindow() != this))
      m_constraints->left.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->top.GetOtherWindow() && (m_constraints->top.GetOtherWindow() != this))
      m_constraints->top.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->right.GetOtherWindow() && (m_constraints->right.GetOtherWindow() != this))
      m_constraints->right.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->bottom.GetOtherWindow() && (m_constraints->bottom.GetOtherWindow() != this))
      m_constraints->bottom.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->width.GetOtherWindow() && (m_constraints->width.GetOtherWindow() != this))
      m_constraints->width.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->height.GetOtherWindow() && (m_constraints->height.GetOtherWindow() != this))
      m_constraints->height.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreX.GetOtherWindow() && (m_constraints->centreX.GetOtherWindow() != this))
      m_constraints->centreX.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreY.GetOtherWindow() && (m_constraints->centreY.GetOtherWindow() != this))
      m_constraints->centreY.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
  }
;
}
;

void wxWindow::SetAutoLayout( bool autoLayout )
{
  m_autoLayout = autoLayout;
}

bool wxWindow::GetAutoLayout() const
{
  return m_autoLayout;
}

wxSizer *wxWindow::GetSizer() const
{
  return m_windowSizer;
}

void wxWindow::SetSizerParent( wxWindow *win )
{
  m_sizerParent = win;
}

wxWindow *wxWindow::GetSizerParent() const
{
  return m_sizerParent;
}

// This removes any dangling pointers to this window
// in other windows' constraintsInvolvedIn lists.
void wxWindow::UnsetConstraints(wxLayoutConstraints *c)
{
  if (c)
  {
    if (c->left.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->left.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->top.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->top.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->right.GetOtherWindow() && (c->right.GetOtherWindow() != this))
      c->right.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->bottom.GetOtherWindow() && (c->bottom.GetOtherWindow() != this))
      c->bottom.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->width.GetOtherWindow() && (c->width.GetOtherWindow() != this))
      c->width.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->height.GetOtherWindow() && (c->height.GetOtherWindow() != this))
      c->height.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreX.GetOtherWindow() && (c->centreX.GetOtherWindow() != this))
      c->centreX.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreY.GetOtherWindow() && (c->centreY.GetOtherWindow() != this))
      c->centreY.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
  }
}

// Back-pointer to other windows we're involved with, so if we delete
// this window, we must delete any constraints we're involved with.
void wxWindow::AddConstraintReference(wxWindow *otherWin)
{
  if (!m_constraintsInvolvedIn)
    m_constraintsInvolvedIn = new wxList;
  if (!m_constraintsInvolvedIn->Member(otherWin))
    m_constraintsInvolvedIn->Append(otherWin);
}

// REMOVE back-pointer to other windows we're involved with.
void wxWindow::RemoveConstraintReference(wxWindow *otherWin)
{
  if (m_constraintsInvolvedIn)
    m_constraintsInvolvedIn->DeleteObject(otherWin);
}

// Reset any constraints that mention this window
void wxWindow::DeleteRelatedConstraints()
{
  if (m_constraintsInvolvedIn)
  {
    wxNode *node = m_constraintsInvolvedIn->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      wxNode *next = node->Next();
      wxLayoutConstraints *constr = win->GetConstraints();

      // Reset any constraints involving this window
      if (constr)
      {
        constr->left.ResetIfWin((wxWindow *)this);
        constr->top.ResetIfWin((wxWindow *)this);
        constr->right.ResetIfWin((wxWindow *)this);
        constr->bottom.ResetIfWin((wxWindow *)this);
        constr->width.ResetIfWin((wxWindow *)this);
        constr->height.ResetIfWin((wxWindow *)this);
        constr->centreX.ResetIfWin((wxWindow *)this);
        constr->centreY.ResetIfWin((wxWindow *)this);
      }
      delete node;
      node = next;
    }
    delete m_constraintsInvolvedIn;
    m_constraintsInvolvedIn = (wxList *) NULL;
  }
}

void wxWindow::SetSizer(wxSizer *sizer)
{
  m_windowSizer = sizer;
  if (sizer)
    sizer->SetSizerParent((wxWindow *)this);
}

/*
 * New version
 */

bool wxWindow::Layout()
{
  if (GetConstraints())
  {
    int w, h;
    GetClientSize(&w, &h);
    GetConstraints()->width.SetValue(w);
    GetConstraints()->height.SetValue(h);
  }

  // If top level (one sizer), evaluate the sizer's constraints.
  if (GetSizer())
  {
    int noChanges;
    GetSizer()->ResetConstraints();   // Mark all constraints as unevaluated
    GetSizer()->LayoutPhase1(&noChanges);
    GetSizer()->LayoutPhase2(&noChanges);
    GetSizer()->SetConstraintSizes(); // Recursively set the real window sizes
    return TRUE;
  }
  else
  {
    // Otherwise, evaluate child constraints
    ResetConstraints();   // Mark all constraints as unevaluated
    DoPhase(1);           // Just one phase need if no sizers involved
    DoPhase(2);
    SetConstraintSizes(); // Recursively set the real window sizes
  }
  return TRUE;
}


// Do a phase of evaluating constraints:
// the default behaviour. wxSizers may do a similar
// thing, but also impose their own 'constraints'
// and order the evaluation differently.
bool wxWindow::LayoutPhase1(int *noChanges)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    return constr->SatisfyConstraints((wxWindow *)this, noChanges);
  }
  else
    return TRUE;
}

bool wxWindow::LayoutPhase2(int *noChanges)
{
  *noChanges = 0;

  // Layout children
  DoPhase(1);
  DoPhase(2);
  return TRUE;
}

// Do a phase of evaluating child constraints
bool wxWindow::DoPhase(int phase)
{
  int noIterations = 0;
  int maxIterations = 500;
  int noChanges = 1;
  int noFailures = 0;
  wxList succeeded;
  while ((noChanges > 0) && (noIterations < maxIterations))
  {
    noChanges = 0;
    noFailures = 0;
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      if (!child->IsKindOf(CLASSINFO(wxFrame)) && !child->IsKindOf(CLASSINFO(wxDialog)))
      {
        wxLayoutConstraints *constr = child->GetConstraints();
        if (constr)
        {
          if (succeeded.Member(child))
          {
          }
          else
          {
            int tempNoChanges = 0;
            bool success = ( (phase == 1) ? child->LayoutPhase1(&tempNoChanges) : child->LayoutPhase2(&tempNoChanges) ) ;
            noChanges += tempNoChanges;
            if (success)
            {
              succeeded.Append(child);
            }
          }
        }
      }
      node = node->Next();
    }
    noIterations ++;
  }
  return TRUE;
}

void wxWindow::ResetConstraints()
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    constr->left.SetDone(FALSE);
    constr->top.SetDone(FALSE);
    constr->right.SetDone(FALSE);
    constr->bottom.SetDone(FALSE);
    constr->width.SetDone(FALSE);
    constr->height.SetDone(FALSE);
    constr->centreX.SetDone(FALSE);
    constr->centreY.SetDone(FALSE);
  }
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
      win->ResetConstraints();
    node = node->Next();
  }
}

// Need to distinguish between setting the 'fake' size for
// windows and sizers, and setting the real values.
void wxWindow::SetConstraintSizes(bool recurse)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr && constr->left.GetDone() && constr->right.GetDone() &&
                constr->width.GetDone() && constr->height.GetDone())
  {
    int x = constr->left.GetValue();
    int y = constr->top.GetValue();
    int w = constr->width.GetValue();
    int h = constr->height.GetValue();

    // If we don't want to resize this window, just move it...
    if ((constr->width.GetRelationship() != wxAsIs) ||
        (constr->height.GetRelationship() != wxAsIs))
    {
      // Calls Layout() recursively. AAAGH. How can we stop that.
      // Simply take Layout() out of non-top level OnSizes.
      SizerSetSize(x, y, w, h);
    }
    else
    {
      SizerMove(x, y);
    }
  }
  else if (constr)
  {
    char *windowClass = this->GetClassInfo()->GetClassName();

    wxString winName;
  if (GetName() == "")
    winName = _("unnamed");
  else
    winName = GetName();
    wxDebugMsg(_("Constraint(s) not satisfied for window of type %s, name %s:\n"), (const char *)windowClass, (const char *)winName);
    if (!constr->left.GetDone())
      wxDebugMsg(_("  unsatisfied 'left' constraint.\n"));
    if (!constr->right.GetDone())
      wxDebugMsg(_("  unsatisfied 'right' constraint.\n"));
    if (!constr->width.GetDone())
      wxDebugMsg(_("  unsatisfied 'width' constraint.\n"));
    if (!constr->height.GetDone())
      wxDebugMsg(_("  unsatisfied 'height' constraint.\n"));
    wxDebugMsg(_("Please check constraints: try adding AsIs() constraints.\n"));
  }

  if (recurse)
  {
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
        win->SetConstraintSizes();
      node = node->Next();
    }
  }
}

// This assumes that all sizers are 'on' the same
// window, i.e. the parent of this window.
void wxWindow::TransformSizerToActual(int *x, int *y) const
{
  if (!m_sizerParent || m_sizerParent->IsKindOf(CLASSINFO(wxDialog)) ||
         m_sizerParent->IsKindOf(CLASSINFO(wxFrame)) )
    return;

  int xp, yp;
  m_sizerParent->GetPosition(&xp, &yp);
  m_sizerParent->TransformSizerToActual(&xp, &yp);
  *x += xp;
  *y += yp;
}

void wxWindow::SizerSetSize(int x, int y, int w, int h)
{
  int xx = x;
  int yy = y;
  TransformSizerToActual(&xx, &yy);
  SetSize(xx, yy, w, h);
}

void wxWindow::SizerMove(int x, int y)
{
  int xx = x;
  int yy = y;
  TransformSizerToActual(&xx, &yy);
  Move(xx, yy);
}

// Only set the size/position of the constraint (if any)
void wxWindow::SetSizeConstraint(int x, int y, int w, int h)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
    if (w != -1)
    {
      constr->width.SetValue(w);
      constr->width.SetDone(TRUE);
    }
    if (h != -1)
    {
      constr->height.SetValue(h);
      constr->height.SetDone(TRUE);
    }
  }
}

void wxWindow::MoveConstraint(int x, int y)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
  }
}

void wxWindow::GetSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetSize(w, h);
}

void wxWindow::GetClientSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetClientSize(w, h);
}

void wxWindow::GetPositionConstraint(int *x, int *y) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *x = constr->left.GetValue();
    *y = constr->top.GetValue();
  }
  else
    GetPosition(x, y);
}

bool wxWindow::AcceptsFocus() const
{
  return IsEnabled() && IsShown();
}

void wxWindow::OnIdle(wxIdleEvent& WXUNUSED(event) )
{
  UpdateWindowUI();
}
