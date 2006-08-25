/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/window.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XWarpPointer XWARPPOINTER
#endif

#include "wx/window.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/dcclient.h"
    #include "wx/menu.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
    #include "wx/toolbar.h"
    #include "wx/combobox.h"
    #include "wx/layout.h"
    #include "wx/statusbr.h"
    #include "wx/math.h"
#endif

#include "wx/module.h"

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#include "wx/fontutil.h"

#ifdef __WXDEBUG__
    #include "wx/thread.h"
#endif

#include <ctype.h>

// FIXME: Due to a hack we use GtkCombo in here, which is deprecated since gtk2.3.0
#include <gtk/gtkversion.h>
#if defined(GTK_DISABLE_DEPRECATED) && GTK_CHECK_VERSION(2,3,0)
#undef GTK_DISABLE_DEPRECATED
#endif

#include "wx/gtk/private.h"
#include <gdk/gdkprivate.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>

#include "wx/gtk/win_gtk.h"

#include <pango/pangox.h>

#ifdef HAVE_XIM
    #undef HAVE_XIM
#endif

extern GtkContainerClass *pizza_parent_class;

//-----------------------------------------------------------------------------
// documentation on internals
//-----------------------------------------------------------------------------

/*
   I have been asked several times about writing some documentation about
   the GTK port of wxWidgets, especially its internal structures. Obviously,
   you cannot understand wxGTK without knowing a little about the GTK, but
   some more information about what the wxWindow, which is the base class
   for all other window classes, does seems required as well.

   I)

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

   6) Display a border (sunken, raised, simple or none).

   Normally one might expect, that one wxWidgets window would always correspond
   to one GTK widget. Under GTK, there is no such all-round widget that has all
   the functionality. Moreover, the GTK defines a client area as a different
   widget from the actual widget you are handling. Last but not least some
   special classes (e.g. wxFrame) handle different categories of widgets and
   still have the possibility to draw something in the client area.
   It was therefore required to write a special purpose GTK widget, that would
   represent a client area in the sense of wxWidgets capable to do the jobs
   2), 3) and 4). I have written this class and it resides in win_gtk.c of
   this directory.

   All windows must have a widget, with which they interact with other under-
   lying GTK widgets. It is this widget, e.g. that has to be resized etc and
   the wxWindow class has a member variable called m_widget which holds a
   pointer to this widget. When the window class represents a GTK native widget,
   this is (in most cases) the only GTK widget the class manages. E.g. the
   wxStaticText class handles only a GtkLabel widget a pointer to which you
   can find in m_widget (defined in wxWindow)

   When the class has a client area for drawing into and for containing children
   it has to handle the client area widget (of the type GtkPizza, defined in
   win_gtk.c), but there could be any number of widgets, handled by a class
   The common rule for all windows is only, that the widget that interacts with
   the rest of GTK must be referenced in m_widget and all other widgets must be
   children of this widget on the GTK level. The top-most widget, which also
   represents the client area, must be in the m_wxwindow field and must be of
   the type GtkPizza.

   As I said, the window classes that display a GTK native widget only have
   one widget, so in the case of e.g. the wxButton class m_widget holds a
   pointer to a GtkButton widget. But windows with client areas (for drawing
   and children) have a m_widget field that is a pointer to a GtkScrolled-
   Window and a m_wxwindow field that is pointer to a GtkPizza and this
   one is (in the GTK sense) a child of the GtkScrolledWindow.

   If the m_wxwindow field is set, then all input to this widget is inter-
   cepted and sent to the wxWidgets class. If not, all input to the widget
   that gets pointed to by m_widget gets intercepted and sent to the class.

   II)

   The design of scrolling in wxWidgets is markedly different from that offered
   by the GTK itself and therefore we cannot simply take it as it is. In GTK,
   clicking on a scrollbar belonging to scrolled window will inevitably move
   the window. In wxWidgets, the scrollbar will only emit an event, send this
   to (normally) a wxScrolledWindow and that class will call ScrollWindow()
   which actually moves the window and its sub-windows. Note that GtkPizza
   memorizes how much it has been scrolled but that wxWidgets forgets this
   so that the two coordinates systems have to be kept in synch. This is done
   in various places using the pizza->xoffset and pizza->yoffset values.

   III)

   Singularly the most broken code in GTK is the code that is supposed to
   inform subwindows (child windows) about new positions. Very often, duplicate
   events are sent without changes in size or position, equally often no
   events are sent at all (All this is due to a bug in the GtkContainer code
   which got fixed in GTK 1.2.6). For that reason, wxGTK completely ignores
   GTK's own system and it simply waits for size events for toplevel windows
   and then iterates down the respective size events to all window. This has
   the disadvantage that windows might get size events before the GTK widget
   actually has the reported size. This doesn't normally pose any problem, but
   the OpenGL drawing routines rely on correct behaviour. Therefore, I have
   added the m_nativeSizeEvents flag, which is true only for the OpenGL canvas,
   i.e. the wxGLCanvas will emit a size event, when (and not before) the X11
   window that is used for OpenGL output really has that size (as reported by
   GTK).

   IV)

   If someone at some point of time feels the immense desire to have a look at,
   change or attempt to optimise the Refresh() logic, this person will need an
   intimate understanding of what "draw" and "expose" events are and what
   they are used for, in particular when used in connection with GTK's
   own windowless widgets. Beware.

   V)

   Cursors, too, have been a constant source of pleasure. The main difficulty
   is that a GdkWindow inherits a cursor if the programmer sets a new cursor
   for the parent. To prevent this from doing too much harm, I use idle time
   to set the cursor over and over again, starting from the toplevel windows
   and ending with the youngest generation (speaking of parent and child windows).
   Also don't forget that cursors (like much else) are connected to GdkWindows,
   not GtkWidgets and that the "window" field of a GtkWidget might very well
   point to the GdkWindow of the parent widget (-> "window-less widget") and
   that the two obviously have very different meanings.

*/

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;
extern bool       g_blockEventsOnScroll;
extern wxCursor   g_globalCursor;

static GdkGC *g_eraseGC = NULL;

// mouse capture state: the window which has it and if the mouse is currently
// inside it
static wxWindowGTK  *g_captureWindow = (wxWindowGTK*) NULL;
static bool g_captureWindowHasMouse = false;

wxWindowGTK  *g_focusWindow = (wxWindowGTK*) NULL;

// the last window which had the focus - this is normally never NULL (except
// if we never had focus at all) as even when g_focusWindow is NULL it still
// keeps its previous value
wxWindowGTK *g_focusWindowLast = (wxWindowGTK*) NULL;

// If a window get the focus set but has not been realized
// yet, defer setting the focus to idle time.
wxWindowGTK *g_delayedFocus = (wxWindowGTK*) NULL;

extern bool g_mainThreadLocked;

//-----------------------------------------------------------------------------
// debug
//-----------------------------------------------------------------------------

#ifdef __WXDEBUG__

#if wxUSE_THREADS
#   define DEBUG_MAIN_THREAD if (wxThread::IsMain() && g_mainThreadLocked) printf("gui reentrance");
#else
#   define DEBUG_MAIN_THREAD
#endif
#else
#define DEBUG_MAIN_THREAD
#endif // Debug

// the trace mask used for the focus debugging messages
#define TRACE_FOCUS _T("focus")

//-----------------------------------------------------------------------------
// missing gdk functions
//-----------------------------------------------------------------------------

void
gdk_window_warp_pointer (GdkWindow      *window,
                         gint            x,
                         gint            y)
{
  if (!window)
    window = gdk_get_default_root_window();

  if (!GDK_WINDOW_DESTROYED(window))
  {
      XWarpPointer (GDK_WINDOW_XDISPLAY(window),
                    None,              /* not source window -> move from anywhere */
                    GDK_WINDOW_XID(window),  /* dest window */
                    0, 0, 0, 0,        /* not source window -> move from anywhere */
                    x, y );
  }
}

//-----------------------------------------------------------------------------
// local code (see below)
//-----------------------------------------------------------------------------

// returns the child of win which currently has focus or NULL if not found
//
// Note: can't be static, needed by textctrl.cpp.
wxWindow *wxFindFocusedChild(wxWindowGTK *win)
{
    wxWindow *winFocus = wxWindowGTK::FindFocus();
    if ( !winFocus )
        return (wxWindow *)NULL;

    if ( winFocus == win )
        return (wxWindow *)win;

    for ( wxWindowList::compatibility_iterator node = win->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *child = wxFindFocusedChild(node->GetData());
        if ( child )
            return child;
    }

    return (wxWindow *)NULL;
}

static void GetScrollbarWidth(GtkWidget* widget, int& w, int& h)
{
    GtkScrolledWindow* scroll_window = GTK_SCROLLED_WINDOW(widget);
    GtkScrolledWindowClass* scroll_class = GTK_SCROLLED_WINDOW_CLASS(GTK_OBJECT_GET_CLASS(scroll_window));
    GtkRequisition scroll_req;

    w = 0;
    if (scroll_window->vscrollbar_visible)
    {
        scroll_req.width = 2;
        scroll_req.height = 2;
        (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(scroll_window->vscrollbar) )->size_request )
            (scroll_window->vscrollbar, &scroll_req );
        w = scroll_req.width +
            scroll_class->scrollbar_spacing;
    }

    h = 0;
    if (scroll_window->hscrollbar_visible)
    {
        scroll_req.width = 2;
        scroll_req.height = 2;
        (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(scroll_window->hscrollbar) )->size_request )
            (scroll_window->hscrollbar, &scroll_req );
        h = scroll_req.height +
            scroll_class->scrollbar_spacing;
    }
}

static void draw_frame( GtkWidget *widget, wxWindowGTK *win )
{
    // wxUniversal widgets draw the borders and scrollbars themselves
#ifndef __WXUNIVERSAL__
    if (!win->m_hasVMT)
        return;

    int dw = 0;
    int dh = 0;

    if (win->m_hasScrolling)
    {
        GetScrollbarWidth(widget, dw, dh);
    }

    int dx = 0;
    int dy = 0;
    if (GTK_WIDGET_NO_WINDOW (widget))
    {
        dx += widget->allocation.x;
        dy += widget->allocation.y;
    }

    if (win->HasFlag(wxRAISED_BORDER))
    {
        gtk_paint_shadow (widget->style,
                          widget->window,
                          GTK_STATE_NORMAL,
                          GTK_SHADOW_OUT,
                          NULL, NULL, NULL, // FIXME: No clipping?
                          dx, dy,
                          widget->allocation.width-dw, widget->allocation.height-dh );
        return;
    }

    if (win->HasFlag(wxSUNKEN_BORDER))
    {
        gtk_paint_shadow (widget->style,
                          widget->window,
                          GTK_STATE_NORMAL,
                          GTK_SHADOW_IN,
                          NULL, NULL, NULL, // FIXME: No clipping?
                          dx, dy,
                          widget->allocation.width-dw, widget->allocation.height-dh );
        return;
    }

    if (win->HasFlag(wxSIMPLE_BORDER))
    {
        GdkGC *gc;
        gc = gdk_gc_new( widget->window );
        gdk_gc_set_foreground( gc, &widget->style->black );
        gdk_draw_rectangle( widget->window, gc, FALSE,
                         dx, dy,
                         widget->allocation.width-dw-1, widget->allocation.height-dh-1 );
        g_object_unref (gc);
        return;
    }
#endif // __WXUNIVERSAL__
}

//-----------------------------------------------------------------------------
// "expose_event" of m_widget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_own_expose_callback( GtkWidget *widget,
                                GdkEventExpose *gdk_event,
                                wxWindowGTK *win )
{
    if (gdk_event->count > 0) return FALSE;

    draw_frame( widget, win );

    (* GTK_WIDGET_CLASS (pizza_parent_class)->expose_event) (widget, gdk_event);

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "size_request" of m_widget
//-----------------------------------------------------------------------------

// make it extern because wxStaticText needs to disconnect this one
extern "C" {
void wxgtk_window_size_request_callback(GtkWidget *widget,
                                        GtkRequisition *requisition,
                                        wxWindow *win)
{
    int w, h;
    win->GetSize( &w, &h );
    if (w < 2)
        w = 2;
    if (h < 2)
        h = 2;

    requisition->height = h;
    requisition->width = w;
}
}

extern "C" {
static
void wxgtk_combo_size_request_callback(GtkWidget *widget,
                                       GtkRequisition *requisition,
                                       wxComboBox *win)
{
    // This callback is actually hooked into the text entry
    // of the combo box, not the GtkHBox.

    int w, h;
    win->GetSize( &w, &h );
    if (w < 2)
        w = 2;
    if (h < 2)
        h = 2;

    GtkCombo *gcombo = GTK_COMBO(win->m_widget);

    GtkRequisition entry_req;
    entry_req.width = 2;
    entry_req.height = 2;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(gcombo->button) )->size_request )
        (gcombo->button, &entry_req );

    requisition->width = w - entry_req.width;
    requisition->height = entry_req.height;
}
}

//-----------------------------------------------------------------------------
// "expose_event" of m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_expose_callback( GtkWidget *widget,
                            GdkEventExpose *gdk_event,
                            wxWindow *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    // This callback gets called in drawing-idle time under
    // GTK 2.0, so we don't need to defer anything to idle
    // time anymore.

    GtkPizza *pizza = GTK_PIZZA( widget );
    if (gdk_event->window != pizza->bin_window) return FALSE;

#if 0
    if (win->GetName())
    {
        wxPrintf( wxT("OnExpose from ") );
        if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
            wxPrintf( win->GetClassInfo()->GetClassName() );
        wxPrintf( wxT(" %d %d %d %d\n"), (int)gdk_event->area.x,
                                         (int)gdk_event->area.y,
                                         (int)gdk_event->area.width,
                                         (int)gdk_event->area.height );
    }

    gtk_paint_box
    (
        win->m_wxwindow->style,
        pizza->bin_window,
        GTK_STATE_NORMAL,
        GTK_SHADOW_OUT,
        (GdkRectangle*) NULL,
        win->m_wxwindow,
        (char *)"button", // const_cast
        20,20,24,24
    );
#endif

    win->GetUpdateRegion() = wxRegion( gdk_event->region );

    win->GtkSendPaintEvents();


    // Let parent window draw window-less widgets
    (* GTK_WIDGET_CLASS (pizza_parent_class)->expose_event) (widget, gdk_event);

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "key_press_event" from any window
//-----------------------------------------------------------------------------

// These are used when transforming Ctrl-alpha to ascii values 1-26
inline bool wxIsLowerChar(int code)
{
    return (code >= 'a' && code <= 'z' );
}

inline bool wxIsUpperChar(int code)
{
    return (code >= 'A' && code <= 'Z' );
}


// set WXTRACE to this to see the key event codes on the console
#define TRACE_KEYS  _T("keyevent")

// translates an X key symbol to WXK_XXX value
//
// if isChar is true it means that the value returned will be used for EVT_CHAR
// event and then we choose the logical WXK_XXX, i.e. '/' for GDK_KP_Divide,
// for example, while if it is false it means that the value is going to be
// used for KEY_DOWN/UP events and then we translate GDK_KP_Divide to
// WXK_NUMPAD_DIVIDE
static long wxTranslateKeySymToWXKey(KeySym keysym, bool isChar)
{
    long key_code;

    switch ( keysym )
    {
        // Shift, Control and Alt don't generate the CHAR events at all
        case GDK_Shift_L:
        case GDK_Shift_R:
            key_code = isChar ? 0 : WXK_SHIFT;
            break;
        case GDK_Control_L:
        case GDK_Control_R:
            key_code = isChar ? 0 : WXK_CONTROL;
            break;
        case GDK_Meta_L:
        case GDK_Meta_R:
        case GDK_Alt_L:
        case GDK_Alt_R:
        case GDK_Super_L:
        case GDK_Super_R:
            key_code = isChar ? 0 : WXK_ALT;
            break;

        // neither do the toggle modifies
        case GDK_Scroll_Lock:
            key_code = isChar ? 0 : WXK_SCROLL;
            break;

        case GDK_Caps_Lock:
            key_code = isChar ? 0 : WXK_CAPITAL;
            break;

        case GDK_Num_Lock:
            key_code = isChar ? 0 : WXK_NUMLOCK;
            break;


        // various other special keys
        case GDK_Menu:
            key_code = WXK_MENU;
            break;

        case GDK_Help:
            key_code = WXK_HELP;
            break;

        case GDK_BackSpace:
            key_code = WXK_BACK;
            break;

        case GDK_ISO_Left_Tab:
        case GDK_Tab:
            key_code = WXK_TAB;
            break;

        case GDK_Linefeed:
        case GDK_Return:
            key_code = WXK_RETURN;
            break;

        case GDK_Clear:
            key_code = WXK_CLEAR;
            break;

        case GDK_Pause:
            key_code = WXK_PAUSE;
            break;

        case GDK_Select:
            key_code = WXK_SELECT;
            break;

        case GDK_Print:
            key_code = WXK_PRINT;
            break;

        case GDK_Execute:
            key_code = WXK_EXECUTE;
            break;

        case GDK_Escape:
            key_code = WXK_ESCAPE;
            break;

        // cursor and other extended keyboard keys
        case GDK_Delete:
            key_code = WXK_DELETE;
            break;

        case GDK_Home:
            key_code = WXK_HOME;
            break;

        case GDK_Left:
            key_code = WXK_LEFT;
            break;

        case GDK_Up:
            key_code = WXK_UP;
            break;

        case GDK_Right:
            key_code = WXK_RIGHT;
            break;

        case GDK_Down:
            key_code = WXK_DOWN;
            break;

        case GDK_Prior:     // == GDK_Page_Up
            key_code = WXK_PAGEUP;
            break;

        case GDK_Next:      // == GDK_Page_Down
            key_code = WXK_PAGEDOWN;
            break;

        case GDK_End:
            key_code = WXK_END;
            break;

        case GDK_Begin:
            key_code = WXK_HOME;
            break;

        case GDK_Insert:
            key_code = WXK_INSERT;
            break;


        // numpad keys
        case GDK_KP_0:
        case GDK_KP_1:
        case GDK_KP_2:
        case GDK_KP_3:
        case GDK_KP_4:
        case GDK_KP_5:
        case GDK_KP_6:
        case GDK_KP_7:
        case GDK_KP_8:
        case GDK_KP_9:
            key_code = (isChar ? '0' : WXK_NUMPAD0) + keysym - GDK_KP_0;
            break;

        case GDK_KP_Space:
            key_code = isChar ? ' ' : WXK_NUMPAD_SPACE;
            break;

        case GDK_KP_Tab:
            key_code = isChar ? WXK_TAB : WXK_NUMPAD_TAB;
            break;

        case GDK_KP_Enter:
            key_code = isChar ? WXK_RETURN : WXK_NUMPAD_ENTER;
            break;

        case GDK_KP_F1:
            key_code = isChar ? WXK_F1 : WXK_NUMPAD_F1;
            break;

        case GDK_KP_F2:
            key_code = isChar ? WXK_F2 : WXK_NUMPAD_F2;
            break;

        case GDK_KP_F3:
            key_code = isChar ? WXK_F3 : WXK_NUMPAD_F3;
            break;

        case GDK_KP_F4:
            key_code = isChar ? WXK_F4 : WXK_NUMPAD_F4;
            break;

        case GDK_KP_Home:
            key_code = isChar ? WXK_HOME : WXK_NUMPAD_HOME;
            break;

        case GDK_KP_Left:
            key_code = isChar ? WXK_LEFT : WXK_NUMPAD_LEFT;
            break;

        case GDK_KP_Up:
            key_code = isChar ? WXK_UP : WXK_NUMPAD_UP;
            break;

        case GDK_KP_Right:
            key_code = isChar ? WXK_RIGHT : WXK_NUMPAD_RIGHT;
            break;

        case GDK_KP_Down:
            key_code = isChar ? WXK_DOWN : WXK_NUMPAD_DOWN;
            break;

        case GDK_KP_Prior: // == GDK_KP_Page_Up
            key_code = isChar ? WXK_PAGEUP : WXK_NUMPAD_PAGEUP;
            break;

        case GDK_KP_Next: // == GDK_KP_Page_Down
            key_code = isChar ? WXK_PAGEDOWN : WXK_NUMPAD_PAGEDOWN;
            break;

        case GDK_KP_End:
            key_code = isChar ? WXK_END : WXK_NUMPAD_END;
            break;

        case GDK_KP_Begin:
            key_code = isChar ? WXK_HOME : WXK_NUMPAD_BEGIN;
            break;

        case GDK_KP_Insert:
            key_code = isChar ? WXK_INSERT : WXK_NUMPAD_INSERT;
            break;

        case GDK_KP_Delete:
            key_code = isChar ? WXK_DELETE : WXK_NUMPAD_DELETE;
            break;

        case GDK_KP_Equal:
            key_code = isChar ? '=' : WXK_NUMPAD_EQUAL;
            break;

        case GDK_KP_Multiply:
            key_code = isChar ? '*' : WXK_NUMPAD_MULTIPLY;
            break;

        case GDK_KP_Add:
            key_code = isChar ? '+' : WXK_NUMPAD_ADD;
            break;

        case GDK_KP_Separator:
            // FIXME: what is this?
            key_code = isChar ? '.' : WXK_NUMPAD_SEPARATOR;
            break;

        case GDK_KP_Subtract:
            key_code = isChar ? '-' : WXK_NUMPAD_SUBTRACT;
            break;

        case GDK_KP_Decimal:
            key_code = isChar ? '.' : WXK_NUMPAD_DECIMAL;
            break;

        case GDK_KP_Divide:
            key_code = isChar ? '/' : WXK_NUMPAD_DIVIDE;
            break;


        // function keys
        case GDK_F1:
        case GDK_F2:
        case GDK_F3:
        case GDK_F4:
        case GDK_F5:
        case GDK_F6:
        case GDK_F7:
        case GDK_F8:
        case GDK_F9:
        case GDK_F10:
        case GDK_F11:
        case GDK_F12:
            key_code = WXK_F1 + keysym - GDK_F1;
            break;

        default:
            key_code = 0;
    }

    return key_code;
}

static inline bool wxIsAsciiKeysym(KeySym ks)
{
    return ks < 256;
}

static void wxFillOtherKeyEventFields(wxKeyEvent& event,
                                      wxWindowGTK *win,
                                      GdkEventKey *gdk_event)
{
    int x = 0;
    int y = 0;
    GdkModifierType state;
    if (gdk_event->window)
        gdk_window_get_pointer(gdk_event->window, &x, &y, &state);

    event.SetTimestamp( gdk_event->time );
    event.SetId(win->GetId());
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK) != 0;
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK) != 0;
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK) != 0;
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK) != 0;
    event.m_scanCode = gdk_event->keyval;
    event.m_rawCode = (wxUint32) gdk_event->keyval;
    event.m_rawFlags = 0;
#if wxUSE_UNICODE
    event.m_uniChar = gdk_keyval_to_unicode(gdk_event->keyval);
#endif
    wxGetMousePosition( &x, &y );
    win->ScreenToClient( &x, &y );
    event.m_x = x;
    event.m_y = y;
    event.SetEventObject( win );
}


static bool
wxTranslateGTKKeyEventToWx(wxKeyEvent& event,
                           wxWindowGTK *win,
                           GdkEventKey *gdk_event)
{
    // VZ: it seems that GDK_KEY_RELEASE event doesn't set event->string
    //     but only event->keyval which is quite useless to us, so remember
    //     the last character from GDK_KEY_PRESS and reuse it as last resort
    //
    // NB: should be MT-safe as we're always called from the main thread only
    static struct
    {
        KeySym keysym;
        long   keycode;
    } s_lastKeyPress = { 0, 0 };

    KeySym keysym = gdk_event->keyval;

    wxLogTrace(TRACE_KEYS, _T("Key %s event: keysym = %ld"),
               event.GetEventType() == wxEVT_KEY_UP ? _T("release")
                                                    : _T("press"),
               keysym);

    long key_code = wxTranslateKeySymToWXKey(keysym, false /* !isChar */);

    if ( !key_code )
    {
        // do we have the translation or is it a plain ASCII character?
        if ( (gdk_event->length == 1) || wxIsAsciiKeysym(keysym) )
        {
            // we should use keysym if it is ASCII as X does some translations
            // like "I pressed while Control is down" => "Ctrl-I" == "TAB"
            // which we don't want here (but which we do use for OnChar())
            if ( !wxIsAsciiKeysym(keysym) )
            {
                keysym = (KeySym)gdk_event->string[0];
            }

            // we want to always get the same key code when the same key is
            // pressed regardless of the state of the modifiers, i.e. on a
            // standard US keyboard pressing '5' or '%' ('5' key with
            // Shift) should result in the same key code in OnKeyDown():
            // '5' (although OnChar() will get either '5' or '%').
            //
            // to do it we first translate keysym to keycode (== scan code)
            // and then back but always using the lower register
            Display *dpy = (Display *)wxGetDisplay();
            KeyCode keycode = XKeysymToKeycode(dpy, keysym);

            wxLogTrace(TRACE_KEYS, _T("\t-> keycode %d"), keycode);

            KeySym keysymNormalized = XKeycodeToKeysym(dpy, keycode, 0);

            // use the normalized, i.e. lower register, keysym if we've
            // got one
            key_code = keysymNormalized ? keysymNormalized : keysym;

            // as explained above, we want to have lower register key codes
            // normally but for the letter keys we want to have the upper ones
            //
            // NB: don't use XConvertCase() here, we want to do it for letters
            // only
            key_code = toupper(key_code);
        }
        else // non ASCII key, what to do?
        {
            // by default, ignore it
            key_code = 0;

            // but if we have cached information from the last KEY_PRESS
            if ( gdk_event->type == GDK_KEY_RELEASE )
            {
                // then reuse it
                if ( keysym == s_lastKeyPress.keysym )
                {
                    key_code = s_lastKeyPress.keycode;
                }
            }
        }

        if ( gdk_event->type == GDK_KEY_PRESS )
        {
            // remember it to be reused for KEY_UP event later
            s_lastKeyPress.keysym = keysym;
            s_lastKeyPress.keycode = key_code;
        }
    }

    wxLogTrace(TRACE_KEYS, _T("\t-> wxKeyCode %ld"), key_code);

    // sending unknown key events doesn't really make sense
    if ( !key_code )
        return false;

    // now fill all the other fields
    wxFillOtherKeyEventFields(event, win, gdk_event);

    event.m_keyCode = key_code;
#if wxUSE_UNICODE
    if ( gdk_event->type == GDK_KEY_PRESS ||  gdk_event->type == GDK_KEY_RELEASE )
    {
        event.m_uniChar = key_code;
    }
#endif

    return true;
}


struct wxGtkIMData
{
    GtkIMContext *context;
    GdkEventKey  *lastKeyEvent;

    wxGtkIMData()
    {
        context = gtk_im_multicontext_new();
        lastKeyEvent = NULL;
    }
    ~wxGtkIMData()
    {
        g_object_unref (context);
    }
};

extern "C" {
static gboolean
gtk_window_key_press_callback( GtkWidget *widget,
                               GdkEventKey *gdk_event,
                               wxWindow *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return FALSE;
    if (g_blockEventsOnDrag)
        return FALSE;


    wxKeyEvent event( wxEVT_KEY_DOWN );
    bool ret = false;
    bool return_after_IM = false;

    if( wxTranslateGTKKeyEventToWx(event, win, gdk_event) )
    {
        // Emit KEY_DOWN event
        ret = win->GetEventHandler()->ProcessEvent( event );
    }
    else
    {
        // Return after IM processing as we cannot do
        // anything with it anyhow.
        return_after_IM = true;
    }

    // 2005.01.26 modified by Hong Jen Yee (hzysoft@sina.com.tw):
    // When we get a key_press event here, it could be originate
    // from the current widget or its child widgets.  However, only the widget
    // with the INPUT FOCUS can generate the INITIAL key_press event.  That is,
    // if the CURRENT widget doesn't have the FOCUS at all, this event definitely
    // originated from its child widgets and shouldn't be passed to IM context.
    // In fact, what a GTK+ IM should do is filtering keyEvents and convert them
    // into text input ONLY WHEN THE WIDGET HAS INPUT FOCUS.  Besides, when current
    // widgets has both IM context and input focus, the event should be filtered
    // by gtk_im_context_filter_keypress().
    // Then, we should, according to GTK+ 2.0 API doc, return whatever it returns.
    if ((!ret) && (win->m_imData != NULL) && ( wxWindow::FindFocus() == win ))
    {
        // We should let GTK+ IM filter key event first. According to GTK+ 2.0 API
        // docs, if IM filter returns true, no further processing should be done.
        // we should send the key_down event anyway.
        bool intercepted_by_IM = gtk_im_context_filter_keypress(win->m_imData->context, gdk_event);
        win->m_imData->lastKeyEvent = NULL;
        if (intercepted_by_IM)
        {
            wxLogTrace(TRACE_KEYS, _T("Key event intercepted by IM"));
            return TRUE;
        }
    }

    if (return_after_IM)
        return FALSE;

#if wxUSE_ACCEL
    if (!ret)
    {
        wxWindowGTK *ancestor = win;
        while (ancestor)
        {
            int command = ancestor->GetAcceleratorTable()->GetCommand( event );
            if (command != -1)
            {
                wxCommandEvent command_event( wxEVT_COMMAND_MENU_SELECTED, command );
                ret = ancestor->GetEventHandler()->ProcessEvent( command_event );
                break;
            }
            if (ancestor->IsTopLevel())
                break;
            ancestor = ancestor->GetParent();
        }
    }
#endif // wxUSE_ACCEL

    // Only send wxEVT_CHAR event if not processed yet. Thus, ALT-x
    // will only be sent if it is not in an accelerator table.
    if (!ret)
    {
        long key_code;
        KeySym keysym = gdk_event->keyval;
        // Find key code for EVT_CHAR and EVT_CHAR_HOOK events
        key_code = wxTranslateKeySymToWXKey(keysym, true /* isChar */);
        if ( !key_code )
        {
            if ( wxIsAsciiKeysym(keysym) )
            {
                // ASCII key
                key_code = (unsigned char)keysym;
            }
            // gdk_event->string is actually deprecated
            else if ( gdk_event->length == 1 )
            {
                key_code = (unsigned char)gdk_event->string[0];
            }
        }

        if ( key_code )
        {
            wxLogTrace(TRACE_KEYS, _T("Char event: %ld"), key_code);

            event.m_keyCode = key_code;

            // To conform to the docs we need to translate Ctrl-alpha
            // characters to values in the range 1-26.
            if ( event.ControlDown() &&
                 ( wxIsLowerChar(key_code) || wxIsUpperChar(key_code) ))
            {
                if ( wxIsLowerChar(key_code) )
                    event.m_keyCode = key_code - 'a' + 1;
                if ( wxIsUpperChar(key_code) )
                    event.m_keyCode = key_code - 'A' + 1;
#if wxUSE_UNICODE
                event.m_uniChar = event.m_keyCode;
#endif
            }

            // Implement OnCharHook by checking ancestor top level windows
            wxWindow *parent = win;
            while (parent && !parent->IsTopLevel())
                parent = parent->GetParent();
            if (parent)
            {
                event.SetEventType( wxEVT_CHAR_HOOK );
                ret = parent->GetEventHandler()->ProcessEvent( event );
            }

            if (!ret)
            {
                event.SetEventType(wxEVT_CHAR);
                ret = win->GetEventHandler()->ProcessEvent( event );
            }
        }
    }





    // win is a control: tab can be propagated up
    if ( !ret &&
         ((gdk_event->keyval == GDK_Tab) || (gdk_event->keyval == GDK_ISO_Left_Tab)) &&
// VZ: testing for wxTE_PROCESS_TAB shouldn't be done here - the control may
//     have this style, yet choose not to process this particular TAB in which
//     case TAB must still work as a navigational character
// JS: enabling again to make consistent with other platforms
//     (with wxTE_PROCESS_TAB you have to call Navigate to get default
//     navigation behaviour)
#if wxUSE_TEXTCTRL
         (! (win->HasFlag(wxTE_PROCESS_TAB) && win->IsKindOf(CLASSINFO(wxTextCtrl)) )) &&
#endif
         win->GetParent() && (win->GetParent()->HasFlag( wxTAB_TRAVERSAL)) )
    {
        wxNavigationKeyEvent new_event;
        new_event.SetEventObject( win->GetParent() );
        // GDK reports GDK_ISO_Left_Tab for SHIFT-TAB
        new_event.SetDirection( (gdk_event->keyval == GDK_Tab) );
        // CTRL-TAB changes the (parent) window, i.e. switch notebook page
        new_event.SetWindowChange( (gdk_event->state & GDK_CONTROL_MASK) );
        new_event.SetCurrentFocus( win );
        ret = win->GetParent()->GetEventHandler()->ProcessEvent( new_event );
    }

    if (ret)
    {
        g_signal_stop_emission_by_name (widget, "key_press_event");
        return TRUE;
    }

    return FALSE;
}
}

extern "C" {
static void
gtk_wxwindow_commit_cb (GtkIMContext *context,
                        const gchar  *str,
                        wxWindow     *window)
{
    wxKeyEvent event( wxEVT_KEY_DOWN );

    // take modifiers, cursor position, timestamp etc. from the last
    // key_press_event that was fed into Input Method:
    if (window->m_imData->lastKeyEvent)
    {
        wxFillOtherKeyEventFields(event,
                                  window, window->m_imData->lastKeyEvent);
    }

    const wxWxCharBuffer data(wxGTK_CONV_BACK(str));
    if( !data )
        return;

    bool ret = false;

    // Implement OnCharHook by checking ancestor top level windows
    wxWindow *parent = window;
    while (parent && !parent->IsTopLevel())
        parent = parent->GetParent();

    for( const wxChar* pstr = data; *pstr; pstr++ )
    {
#if wxUSE_UNICODE
        event.m_uniChar = *pstr;
        // Backward compatible for ISO-8859-1
        event.m_keyCode = *pstr < 256 ? event.m_uniChar : 0;
        wxLogTrace(TRACE_KEYS, _T("IM sent character '%c'"), event.m_uniChar);
#else
        event.m_keyCode = *pstr;
#endif  // wxUSE_UNICODE

        // To conform to the docs we need to translate Ctrl-alpha
        // characters to values in the range 1-26.
        if ( event.ControlDown() &&
             ( wxIsLowerChar(*pstr) || wxIsUpperChar(*pstr) ))
        {
            if ( wxIsLowerChar(*pstr) )
                event.m_keyCode = *pstr - 'a' + 1;
            if ( wxIsUpperChar(*pstr) )
                event.m_keyCode = *pstr - 'A' + 1;

            event.m_keyCode = *pstr - 'a' + 1;
#if wxUSE_UNICODE
            event.m_uniChar = event.m_keyCode;
#endif
        }

        if (parent)
        {
            event.SetEventType( wxEVT_CHAR_HOOK );
            ret = parent->GetEventHandler()->ProcessEvent( event );
        }

        if (!ret)
        {
            event.SetEventType(wxEVT_CHAR);
            ret = window->GetEventHandler()->ProcessEvent( event );
        }
    }
}
}


//-----------------------------------------------------------------------------
// "key_release_event" from any window
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_key_release_callback( GtkWidget *widget,
                                 GdkEventKey *gdk_event,
                                 wxWindowGTK *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return FALSE;

    if (g_blockEventsOnDrag)
        return FALSE;

    wxKeyEvent event( wxEVT_KEY_UP );
    if ( !wxTranslateGTKKeyEventToWx(event, win, gdk_event) )
    {
        // unknown key pressed, ignore (the event would be useless anyhow)
        return FALSE;
    }

    if ( !win->GetEventHandler()->ProcessEvent( event ) )
        return FALSE;

    g_signal_stop_emission_by_name (widget, "key_release_event");
    return TRUE;
}
}

// ============================================================================
// the mouse events
// ============================================================================

// ----------------------------------------------------------------------------
// mouse event processing helpers
// ----------------------------------------------------------------------------

// init wxMouseEvent with the info from GdkEventXXX struct
template<typename T> void InitMouseEvent(wxWindowGTK *win,
                                         wxMouseEvent& event,
                                         T *gdk_event)
{
    event.SetTimestamp( gdk_event->time );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_leftDown = (gdk_event->state & GDK_BUTTON1_MASK);
    event.m_middleDown = (gdk_event->state & GDK_BUTTON2_MASK);
    event.m_rightDown = (gdk_event->state & GDK_BUTTON3_MASK);
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
       event.m_linesPerAction = 3;
       event.m_wheelDelta = 120;
       if (((GdkEventButton*)gdk_event)->button == 4)
           event.m_wheelRotation = 120;
       else if (((GdkEventButton*)gdk_event)->button == 5)
           event.m_wheelRotation = -120;
    }

    wxPoint pt = win->GetClientAreaOrigin();
    event.m_x = (wxCoord)gdk_event->x - pt.x;
    event.m_y = (wxCoord)gdk_event->y - pt.y;

    event.SetEventObject( win );
    event.SetId( win->GetId() );
    event.SetTimestamp( gdk_event->time );
}

static void AdjustEventButtonState(wxMouseEvent& event)
{
    // GDK reports the old state of the button for a button press event, but
    // for compatibility with MSW and common sense we want m_leftDown be TRUE
    // for a LEFT_DOWN event, not FALSE, so we will invert
    // left/right/middleDown for the corresponding click events

    if ((event.GetEventType() == wxEVT_LEFT_DOWN) ||
        (event.GetEventType() == wxEVT_LEFT_DCLICK) ||
        (event.GetEventType() == wxEVT_LEFT_UP))
    {
        event.m_leftDown = !event.m_leftDown;
        return;
    }

    if ((event.GetEventType() == wxEVT_MIDDLE_DOWN) ||
        (event.GetEventType() == wxEVT_MIDDLE_DCLICK) ||
        (event.GetEventType() == wxEVT_MIDDLE_UP))
    {
        event.m_middleDown = !event.m_middleDown;
        return;
    }

    if ((event.GetEventType() == wxEVT_RIGHT_DOWN) ||
        (event.GetEventType() == wxEVT_RIGHT_DCLICK) ||
        (event.GetEventType() == wxEVT_RIGHT_UP))
    {
        event.m_rightDown = !event.m_rightDown;
        return;
    }
}

// find the window to send the mouse event too
static
wxWindowGTK *FindWindowForMouseEvent(wxWindowGTK *win, wxCoord& x, wxCoord& y)
{
    wxCoord xx = x;
    wxCoord yy = y;

    if (win->m_wxwindow)
    {
        GtkPizza *pizza = GTK_PIZZA(win->m_wxwindow);
        xx += pizza->xoffset;
        yy += pizza->yoffset;
    }

    wxWindowList::compatibility_iterator node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindowGTK *child = node->GetData();

        node = node->GetNext();
        if (!child->IsShown())
            continue;

        if (child->IsTransparentForMouse())
        {
            // wxStaticBox is transparent in the box itself
            int xx1 = child->m_x;
            int yy1 = child->m_y;
            int xx2 = child->m_x + child->m_width;
            int yy2 = child->m_y + child->m_height;

            // left
            if (((xx >= xx1) && (xx <= xx1+10) && (yy >= yy1) && (yy <= yy2)) ||
            // right
                ((xx >= xx2-10) && (xx <= xx2) && (yy >= yy1) && (yy <= yy2)) ||
            // top
                ((xx >= xx1) && (xx <= xx2) && (yy >= yy1) && (yy <= yy1+10)) ||
            // bottom
                ((xx >= xx1) && (xx <= xx2) && (yy >= yy2-1) && (yy <= yy2)))
            {
                win = child;
                x -= child->m_x;
                y -= child->m_y;
                break;
            }

        }
        else
        {
            if ((child->m_wxwindow == (GtkWidget*) NULL) &&
                (child->m_x <= xx) &&
                (child->m_y <= yy) &&
                (child->m_x+child->m_width  >= xx) &&
                (child->m_y+child->m_height >= yy))
            {
                win = child;
                x -= child->m_x;
                y -= child->m_y;
                break;
            }
        }
    }

    return win;
}

// ----------------------------------------------------------------------------
// common event handlers helpers
// ----------------------------------------------------------------------------

int wxWindowGTK::GTKCallbackCommonPrologue(GdkEventAny *event) const
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!m_hasVMT)
        return FALSE;
    if (g_blockEventsOnDrag)
        return TRUE;
    if (g_blockEventsOnScroll)
        return TRUE;

    if (!GTKIsOwnWindow(event->window))
        return FALSE;

    return -1;
}

// overloads for all GDK event types we use here: we need to have this as
// GdkEventXXX can't be implicitly cast to GdkEventAny even if it, in fact,
// derives from it in the sense that the structs have the same layout
#define wxDEFINE_COMMON_PROLOGUE_OVERLOAD(T)                                  \
    static int wxGtkCallbackCommonPrologue(T *event, wxWindowGTK *win)        \
    {                                                                         \
        return win->GTKCallbackCommonPrologue((GdkEventAny *)event);          \
    }

wxDEFINE_COMMON_PROLOGUE_OVERLOAD(GdkEventButton)
wxDEFINE_COMMON_PROLOGUE_OVERLOAD(GdkEventMotion)
wxDEFINE_COMMON_PROLOGUE_OVERLOAD(GdkEventCrossing)

#undef wxDEFINE_COMMON_PROLOGUE_OVERLOAD

#define wxCOMMON_CALLBACK_PROLOGUE(event, win)                                \
    const int rc = wxGtkCallbackCommonPrologue(event, win);                   \
    if ( rc != -1 )                                                           \
        return rc

// send the wxChildFocusEvent and wxFocusEvent, common code of
// gtk_window_focus_in_callback() and SetFocus()
static bool DoSendFocusEvents(wxWindow *win)
{
    // Notify the parent keeping track of focus for the kbd navigation
    // purposes that we got it.
    wxChildFocusEvent eventChildFocus(win);
    (void)win->GetEventHandler()->ProcessEvent(eventChildFocus);

    wxFocusEvent eventFocus(wxEVT_SET_FOCUS, win->GetId());
    eventFocus.SetEventObject(win);

    return win->GetEventHandler()->ProcessEvent(eventFocus);
}

// all event handlers must have C linkage as they're called from GTK+ C code
extern "C"
{

//-----------------------------------------------------------------------------
// "button_press_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_button_press_callback( GtkWidget *widget,
                                  GdkEventButton *gdk_event,
                                  wxWindowGTK *win )
{
    wxCOMMON_CALLBACK_PROLOGUE(gdk_event, win);

    if (win->m_wxwindow && (g_focusWindow != win) && win->AcceptsFocus())
    {
        gtk_widget_grab_focus( win->m_wxwindow );
    }

    // GDK sends surplus button down events
    // before a double click event. We
    // need to filter these out.
    if (gdk_event->type == GDK_BUTTON_PRESS)
    {
        GdkEvent *peek_event = gdk_event_peek();
        if (peek_event)
        {
            if ((peek_event->type == GDK_2BUTTON_PRESS) ||
                (peek_event->type == GDK_3BUTTON_PRESS))
            {
                gdk_event_free( peek_event );
                return TRUE;
            }
            else
            {
                gdk_event_free( peek_event );
            }
        }
    }

    wxEventType event_type = wxEVT_NULL;

    // GdkDisplay is a GTK+ 2.2.0 thing
#if defined(__WXGTK20__) && GTK_CHECK_VERSION(2, 2, 0)
    if ( gdk_event->type == GDK_2BUTTON_PRESS &&
            !gtk_check_version(2,2,0) &&
            gdk_event->button >= 1 && gdk_event->button <= 3 )
    {
        // Reset GDK internal timestamp variables in order to disable GDK
        // triple click events. GDK will then next time believe no button has
        // been clicked just before, and send a normal button click event.
        GdkDisplay* display = gtk_widget_get_display (widget);
        display->button_click_time[1] = 0;
        display->button_click_time[0] = 0;
    }
#endif // GTK 2+

    if (gdk_event->button == 1)
    {
        // note that GDK generates triple click events which are not supported
        // by wxWidgets but still have to be passed to the app as otherwise
        // clicks would simply go missing
        switch (gdk_event->type)
        {
            // we shouldn't get triple clicks at all for GTK2 because we
            // suppress them artificially using the code above but we still
            // should map them to something for GTK1 and not just ignore them
            // as this would lose clicks
            case GDK_3BUTTON_PRESS:     // we could also map this to DCLICK...
            case GDK_BUTTON_PRESS:
                event_type = wxEVT_LEFT_DOWN;
                break;

            case GDK_2BUTTON_PRESS:
                event_type = wxEVT_LEFT_DCLICK;
                break;

            default:
                // just to silence gcc warnings
                ;
        }
    }
    else if (gdk_event->button == 2)
    {
        switch (gdk_event->type)
        {
            case GDK_3BUTTON_PRESS:
            case GDK_BUTTON_PRESS:
                event_type = wxEVT_MIDDLE_DOWN;
                break;

            case GDK_2BUTTON_PRESS:
                event_type = wxEVT_MIDDLE_DCLICK;
                break;

            default:
                ;
        }
    }
    else if (gdk_event->button == 3)
    {
        switch (gdk_event->type)
        {
            case GDK_3BUTTON_PRESS:
            case GDK_BUTTON_PRESS:
                event_type = wxEVT_RIGHT_DOWN;
                break;

            case GDK_2BUTTON_PRESS:
                event_type = wxEVT_RIGHT_DCLICK;
                break;

            default:
                ;
        }
    }
    else if (gdk_event->button == 4 || gdk_event->button == 5)
    {
        if (gdk_event->type == GDK_BUTTON_PRESS )
        {
            event_type = wxEVT_MOUSEWHEEL;
        }
    }

    if ( event_type == wxEVT_NULL )
    {
        // unknown mouse button or click type
        return FALSE;
    }

    wxMouseEvent event( event_type );
    InitMouseEvent( win, event, gdk_event );

    AdjustEventButtonState(event);

    // wxListBox actually gets mouse events from the item, so we need to give it
    // a chance to correct this
    win->FixUpMouseEvent(widget, event.m_x, event.m_y);

    // find the correct window to send the event to: it may be a different one
    // from the one which got it at GTK+ level because some controls don't have
    // their own X window and thus cannot get any events.
    if ( !g_captureWindow )
        win = FindWindowForMouseEvent(win, event.m_x, event.m_y);

    // reset the event object and id in case win changed.
    event.SetEventObject( win );
    event.SetId( win->GetId() );

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        g_signal_stop_emission_by_name (widget, "button_press_event");
        return TRUE;
    }

    if (event_type == wxEVT_RIGHT_DOWN)
    {
        // generate a "context menu" event: this is similar to right mouse
        // click under many GUIs except that it is generated differently
        // (right up under MSW, ctrl-click under Mac, right down here) and
        //
        // (a) it's a command event and so is propagated to the parent
        // (b) under some ports it can be generated from kbd too
        // (c) it uses screen coords (because of (a))
        wxContextMenuEvent evtCtx(
            wxEVT_CONTEXT_MENU,
            win->GetId(),
            win->ClientToScreen(event.GetPosition()));
        evtCtx.SetEventObject(win);
        return win->GetEventHandler()->ProcessEvent(evtCtx);
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "button_release_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_button_release_callback( GtkWidget *widget,
                                    GdkEventButton *gdk_event,
                                    wxWindowGTK *win )
{
    wxCOMMON_CALLBACK_PROLOGUE(gdk_event, win);

    wxEventType event_type = wxEVT_NULL;

    switch (gdk_event->button)
    {
        case 1:
            event_type = wxEVT_LEFT_UP;
            break;

        case 2:
            event_type = wxEVT_MIDDLE_UP;
            break;

        case 3:
            event_type = wxEVT_RIGHT_UP;
            break;

        default:
            // unknown button, don't process
            return FALSE;
    }

    wxMouseEvent event( event_type );
    InitMouseEvent( win, event, gdk_event );

    AdjustEventButtonState(event);

    // same wxListBox hack as above
    win->FixUpMouseEvent(widget, event.m_x, event.m_y);

    if ( !g_captureWindow )
        win = FindWindowForMouseEvent(win, event.m_x, event.m_y);

    // reset the event object and id in case win changed.
    event.SetEventObject( win );
    event.SetId( win->GetId() );

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        g_signal_stop_emission_by_name (widget, "button_release_event");
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "motion_notify_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_motion_notify_callback( GtkWidget *widget,
                                   GdkEventMotion *gdk_event,
                                   wxWindowGTK *win )
{
    wxCOMMON_CALLBACK_PROLOGUE(gdk_event, win);

    if (gdk_event->is_hint)
    {
        int x = 0;
        int y = 0;
        GdkModifierType state;
        gdk_window_get_pointer(gdk_event->window, &x, &y, &state);
        gdk_event->x = x;
        gdk_event->y = y;
    }

    wxMouseEvent event( wxEVT_MOTION );
    InitMouseEvent(win, event, gdk_event);

    if ( g_captureWindow )
    {
        // synthesise a mouse enter or leave event if needed
        GdkWindow *winUnderMouse = gdk_window_at_pointer(NULL, NULL);
        // This seems to be necessary and actually been added to
        // GDK itself in version 2.0.X
        gdk_flush();

        bool hasMouse = winUnderMouse == gdk_event->window;
        if ( hasMouse != g_captureWindowHasMouse )
        {
            // the mouse changed window
            g_captureWindowHasMouse = hasMouse;

            wxMouseEvent eventM(g_captureWindowHasMouse ? wxEVT_ENTER_WINDOW
                                                        : wxEVT_LEAVE_WINDOW);
            InitMouseEvent(win, eventM, gdk_event);
            eventM.SetEventObject(win);
            win->GetEventHandler()->ProcessEvent(eventM);
        }
    }
    else // no capture
    {
        win = FindWindowForMouseEvent(win, event.m_x, event.m_y);

        // reset the event object and id in case win changed.
        event.SetEventObject( win );
        event.SetId( win->GetId() );
    }

    if ( !g_captureWindow )
    {
        wxSetCursorEvent cevent( event.m_x, event.m_y );
        if (win->GetEventHandler()->ProcessEvent( cevent ))
        {
            // Rewrite cursor handling here (away from idle).
        }
    }

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        g_signal_stop_emission_by_name (widget, "motion_notify_event");
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "scroll_event", (mouse wheel event)
//-----------------------------------------------------------------------------

static gboolean
window_scroll_event(GtkWidget*, GdkEventScroll* gdk_event, wxWindow* win)
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (gdk_event->direction != GDK_SCROLL_UP &&
        gdk_event->direction != GDK_SCROLL_DOWN)
    {
        return false;
    }

    wxMouseEvent event(wxEVT_MOUSEWHEEL);
    // Can't use InitMouse macro because scroll events don't have button
    event.SetTimestamp( gdk_event->time );
    event.m_shiftDown = (gdk_event->state & GDK_SHIFT_MASK);
    event.m_controlDown = (gdk_event->state & GDK_CONTROL_MASK);
    event.m_altDown = (gdk_event->state & GDK_MOD1_MASK);
    event.m_metaDown = (gdk_event->state & GDK_MOD2_MASK);
    event.m_leftDown = (gdk_event->state & GDK_BUTTON1_MASK);
    event.m_middleDown = (gdk_event->state & GDK_BUTTON2_MASK);
    event.m_rightDown = (gdk_event->state & GDK_BUTTON3_MASK);
    event.m_linesPerAction = 3;
    event.m_wheelDelta = 120;
    if (gdk_event->direction == GDK_SCROLL_UP)
        event.m_wheelRotation = 120;
    else
        event.m_wheelRotation = -120;

    wxPoint pt = win->GetClientAreaOrigin();
    event.m_x = (wxCoord)gdk_event->x - pt.x;
    event.m_y = (wxCoord)gdk_event->y - pt.y;

    event.SetEventObject( win );
    event.SetId( win->GetId() );
    event.SetTimestamp( gdk_event->time );

    return win->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// "popup-menu"
//-----------------------------------------------------------------------------

static gboolean wxgtk_window_popup_menu_callback(GtkWidget*, wxWindowGTK* win)
{
    wxContextMenuEvent event(wxEVT_CONTEXT_MENU, win->GetId(), wxPoint(-1, -1));
    event.SetEventObject(win);
    return win->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// "focus_in_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_focus_in_callback( GtkWidget *widget,
                              GdkEventFocus *WXUNUSED(event),
                              wxWindow *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (win->m_imData)
        gtk_im_context_focus_in(win->m_imData->context);

    g_focusWindowLast =
    g_focusWindow = win;

    wxLogTrace(TRACE_FOCUS,
               _T("%s: focus in"), win->GetName().c_str());

#ifdef HAVE_XIM
    if (win->m_ic)
        gdk_im_begin(win->m_ic, win->m_wxwindow->window);
#endif

#if wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = win->GetCaret();
    if ( caret )
    {
        caret->OnSetFocus();
    }
#endif // wxUSE_CARET

    gboolean ret = FALSE;

    // does the window itself think that it has the focus?
    if ( !win->m_hasFocus )
    {
        // not yet, notify it
        win->m_hasFocus = true;

        (void)DoSendFocusEvents(win);

        ret = TRUE;
    }

    // Disable default focus handling for custom windows
    // since the default GTK+ handler issues a repaint
    if (win->m_wxwindow)
        return ret;

    return FALSE;
}

//-----------------------------------------------------------------------------
// "focus_out_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_focus_out_callback( GtkWidget *widget,
                               GdkEventFocus *gdk_event,
                               wxWindowGTK *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (win->m_imData)
        gtk_im_context_focus_out(win->m_imData->context);

    wxLogTrace( TRACE_FOCUS,
                _T("%s: focus out"), win->GetName().c_str() );


    wxWindowGTK *winFocus = wxFindFocusedChild(win);
    if ( winFocus )
        win = winFocus;

    g_focusWindow = (wxWindowGTK *)NULL;

#ifdef HAVE_XIM
    if (win->m_ic)
        gdk_im_end();
#endif

#if wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = win->GetCaret();
    if ( caret )
    {
        caret->OnKillFocus();
    }
#endif // wxUSE_CARET

    gboolean ret = FALSE;

    // don't send the window a kill focus event if it thinks that it doesn't
    // have focus already
    if ( win->m_hasFocus )
    {
        win->m_hasFocus = false;

        wxFocusEvent event( wxEVT_KILL_FOCUS, win->GetId() );
        event.SetEventObject( win );

        (void)win->GetEventHandler()->ProcessEvent( event );

        ret = TRUE;
    }

    // Disable default focus handling for custom windows
    // since the default GTK+ handler issues a repaint
    if (win->m_wxwindow)
        return ret;

    return FALSE;
}

//-----------------------------------------------------------------------------
// "enter_notify_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_enter_callback( GtkWidget *widget,
                           GdkEventCrossing *gdk_event,
                           wxWindowGTK *win )
{
    wxCOMMON_CALLBACK_PROLOGUE(gdk_event, win);

    // Event was emitted after a grab
    if (gdk_event->mode != GDK_CROSSING_NORMAL) return FALSE;

    int x = 0;
    int y = 0;
    GdkModifierType state = (GdkModifierType)0;

    gdk_window_get_pointer( widget->window, &x, &y, &state );

    wxMouseEvent event( wxEVT_ENTER_WINDOW );
    InitMouseEvent(win, event, gdk_event);
    wxPoint pt = win->GetClientAreaOrigin();
    event.m_x = x + pt.x;
    event.m_y = y + pt.y;

    if ( !g_captureWindow )
    {
        wxSetCursorEvent cevent( event.m_x, event.m_y );
        if (win->GetEventHandler()->ProcessEvent( cevent ))
        {
            // Rewrite cursor handling here (away from idle).
        }
    }

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
       g_signal_stop_emission_by_name (widget, "enter_notify_event");
       return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "leave_notify_event"
//-----------------------------------------------------------------------------

static gboolean
gtk_window_leave_callback( GtkWidget *widget,
                           GdkEventCrossing *gdk_event,
                           wxWindowGTK *win )
{
    wxCOMMON_CALLBACK_PROLOGUE(gdk_event, win);

    // Event was emitted after an ungrab
    if (gdk_event->mode != GDK_CROSSING_NORMAL) return FALSE;

    wxMouseEvent event( wxEVT_LEAVE_WINDOW );
    event.SetTimestamp( gdk_event->time );
    event.SetEventObject( win );

    int x = 0;
    int y = 0;
    GdkModifierType state = (GdkModifierType)0;

    gdk_window_get_pointer( widget->window, &x, &y, &state );

    event.m_shiftDown = (state & GDK_SHIFT_MASK) != 0;
    event.m_controlDown = (state & GDK_CONTROL_MASK) != 0;
    event.m_altDown = (state & GDK_MOD1_MASK) != 0;
    event.m_metaDown = (state & GDK_MOD2_MASK) != 0;
    event.m_leftDown = (state & GDK_BUTTON1_MASK) != 0;
    event.m_middleDown = (state & GDK_BUTTON2_MASK) != 0;
    event.m_rightDown = (state & GDK_BUTTON3_MASK) != 0;

    wxPoint pt = win->GetClientAreaOrigin();
    event.m_x = x + pt.x;
    event.m_y = y + pt.y;

    if (win->GetEventHandler()->ProcessEvent( event ))
    {
        g_signal_stop_emission_by_name (widget, "leave_notify_event");
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "value_changed" from scrollbar
//-----------------------------------------------------------------------------

static void
gtk_scrollbar_value_changed(GtkRange* range, wxWindow* win)
{
    wxEventType eventType = win->GetScrollEventType(range);
    if (eventType != wxEVT_NULL)
    {
        // Convert scroll event type to scrollwin event type
        eventType += wxEVT_SCROLLWIN_TOP - wxEVT_SCROLL_TOP;

        // find the scrollbar which generated the event
        wxWindowGTK::ScrollDir dir = win->ScrollDirFromRange(range);

        // generate the corresponding wx event
        const int orient = win->OrientFromScrollDir(dir);
        wxScrollWinEvent event(eventType, win->GetScrollPos(orient), orient);
        event.SetEventObject(win);

        win->m_blockValueChanged[dir] = true;
        win->GetEventHandler()->ProcessEvent(event);
        win->m_blockValueChanged[dir] = false;
    }
}

//-----------------------------------------------------------------------------
// "button_press_event" from scrollbar
//-----------------------------------------------------------------------------

static gboolean
gtk_scrollbar_button_press_event(GtkRange*, GdkEventButton*, wxWindow* win)
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    g_blockEventsOnScroll = true;
    win->m_mouseButtonDown = true;

    return false;
}

//-----------------------------------------------------------------------------
// "event_after" from scrollbar
//-----------------------------------------------------------------------------

static void
gtk_scrollbar_event_after(GtkRange* range, GdkEvent* event, wxWindow* win)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        g_signal_handlers_block_by_func(range, (void*)gtk_scrollbar_event_after, win);

        const int orient = win->OrientFromScrollDir(
                                        win->ScrollDirFromRange(range));
        wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBRELEASE, win->GetScrollPos(orient), orient);
        event.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event);
    }
}

//-----------------------------------------------------------------------------
// "button_release_event" from scrollbar
//-----------------------------------------------------------------------------

static gboolean
gtk_scrollbar_button_release_event(GtkRange* range, GdkEventButton*, wxWindow* win)
{
    DEBUG_MAIN_THREAD

    g_blockEventsOnScroll = false;
    win->m_mouseButtonDown = false;
    // If thumb tracking
    if (win->m_isScrolling)
    {
        win->m_isScrolling = false;
        // Hook up handler to send thumb release event after this emission is finished.
        // To allow setting scroll position from event handler, sending event must
        // be deferred until after the GtkRange handler for this signal has run
        g_signal_handlers_unblock_by_func(range, (void*)gtk_scrollbar_event_after, win);
    }

    return false;
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

/* We cannot set colours and fonts before the widget has
   been realized, so we do this directly after realization. */

static void
gtk_window_realized_callback( GtkWidget *m_widget, wxWindow *win )
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    if (win->m_imData)
    {
        GtkPizza *pizza = GTK_PIZZA( m_widget );
        gtk_im_context_set_client_window( win->m_imData->context,
                                          pizza->bin_window );
    }

    wxWindowCreateEvent event( win );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static
void gtk_window_size_callback( GtkWidget *WXUNUSED(widget),
                               GtkAllocation *WXUNUSED(alloc),
                               wxWindow *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    int client_width = 0;
    int client_height = 0;
    win->GetClientSize( &client_width, &client_height );
    if ((client_width == win->m_oldClientWidth) && (client_height == win->m_oldClientHeight))
        return;

    win->m_oldClientWidth = client_width;
    win->m_oldClientHeight = client_height;

    if (!win->m_nativeSizeEvent)
    {
        wxSizeEvent event( win->GetSize(), win->GetId() );
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );
    }
}


#ifdef HAVE_XIM
    #define WXUNUSED_UNLESS_XIM(param)  param
#else
    #define WXUNUSED_UNLESS_XIM(param)  WXUNUSED(param)
#endif

/* Resize XIM window */
static
void gtk_wxwindow_size_callback( GtkWidget* WXUNUSED_UNLESS_XIM(widget),
                                 GtkAllocation* WXUNUSED_UNLESS_XIM(alloc),
                                 wxWindowGTK* WXUNUSED_UNLESS_XIM(win) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

#ifdef HAVE_XIM
    if (!win->m_ic)
        return;

    if  (gdk_ic_get_style (win->m_ic) & GDK_IM_PREEDIT_POSITION)
    {
        gint width, height;

        gdk_drawable_get_size (widget->window, &width, &height);
        win->m_icattr->preedit_area.width = width;
        win->m_icattr->preedit_area.height = height;
        gdk_ic_set_attr (win->m_ic, win->m_icattr, GDK_IC_PREEDIT_AREA);
    }
#endif // HAVE_XIM
}

//-----------------------------------------------------------------------------
// "realize" from m_wxwindow
//-----------------------------------------------------------------------------

/* Initialize XIM support */

static void
gtk_wxwindow_realized_callback( GtkWidget * WXUNUSED_UNLESS_XIM(widget),
                                wxWindowGTK * WXUNUSED_UNLESS_XIM(win) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

#ifdef HAVE_XIM
    if (win->m_ic) return;
    if (!widget) return;
    if (!gdk_im_ready()) return;

    win->m_icattr = gdk_ic_attr_new();
    if (!win->m_icattr) return;

    gint width, height;
    GdkEventMask mask;
    GdkColormap *colormap;
    GdkICAttr *attr = win->m_icattr;
    unsigned attrmask = GDK_IC_ALL_REQ;
    GdkIMStyle style;
    GdkIMStyle supported_style = (GdkIMStyle)
                                  (GDK_IM_PREEDIT_NONE |
                                   GDK_IM_PREEDIT_NOTHING |
                                   GDK_IM_PREEDIT_POSITION |
                                   GDK_IM_STATUS_NONE |
                                   GDK_IM_STATUS_NOTHING);

    if (widget->style && widget->style->font->type != GDK_FONT_FONTSET)
        supported_style = (GdkIMStyle)(supported_style & ~GDK_IM_PREEDIT_POSITION);

    attr->style = style = gdk_im_decide_style (supported_style);
    attr->client_window = widget->window;

    if ((colormap = gtk_widget_get_colormap (widget)) !=
            gtk_widget_get_default_colormap ())
    {
        attrmask |= GDK_IC_PREEDIT_COLORMAP;
        attr->preedit_colormap = colormap;
    }

    attrmask |= GDK_IC_PREEDIT_FOREGROUND;
    attrmask |= GDK_IC_PREEDIT_BACKGROUND;
    attr->preedit_foreground = widget->style->fg[GTK_STATE_NORMAL];
    attr->preedit_background = widget->style->base[GTK_STATE_NORMAL];

    switch (style & GDK_IM_PREEDIT_MASK)
    {
        case GDK_IM_PREEDIT_POSITION:
            if (widget->style && widget->style->font->type != GDK_FONT_FONTSET)
            {
                g_warning ("over-the-spot style requires fontset");
                break;
            }

            gdk_drawable_get_size (widget->window, &width, &height);

            attrmask |= GDK_IC_PREEDIT_POSITION_REQ;
            attr->spot_location.x = 0;
            attr->spot_location.y = height;
            attr->preedit_area.x = 0;
            attr->preedit_area.y = 0;
            attr->preedit_area.width = width;
            attr->preedit_area.height = height;
            attr->preedit_fontset = widget->style->font;

            break;
    }

      win->m_ic = gdk_ic_new (attr, (GdkICAttributesType)attrmask);

      if (win->m_ic == NULL)
          g_warning ("Can't create input context.");
      else
      {
          mask = gdk_window_get_events (widget->window);
          mask = (GdkEventMask)(mask | gdk_ic_get_events (win->m_ic));
          gdk_window_set_events (widget->window, mask);

          if (GTK_WIDGET_HAS_FOCUS(widget))
              gdk_im_begin (win->m_ic, widget->window);
      }
#endif // HAVE_XIM
}

} // extern "C"

// ----------------------------------------------------------------------------
// this wxWindowBase function is implemented here (in platform-specific file)
// because it is static and so couldn't be made virtual
// ----------------------------------------------------------------------------

wxWindow *wxWindowBase::DoFindFocus()
{
    // the cast is necessary when we compile in wxUniversal mode
    return (wxWindow *)g_focusWindow;
}

//-----------------------------------------------------------------------------
// InsertChild for wxWindowGTK.
//-----------------------------------------------------------------------------

/* Callback for wxWindowGTK. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxNotebook requires a different way to insert
 * a child in it. I had opted for creating a wxNotebookPage window class
 * which would have made this superfluous (such in the MDI window system),
 * but no-one was listening to me... */

static void wxInsertChildInWindow( wxWindowGTK* parent, wxWindowGTK* child )
{
    /* the window might have been scrolled already, do we
       have to adapt the position */
    GtkPizza *pizza = GTK_PIZZA(parent->m_wxwindow);
    child->m_x += pizza->xoffset;
    child->m_y += pizza->yoffset;

    gtk_pizza_put( GTK_PIZZA(parent->m_wxwindow),
                     GTK_WIDGET(child->m_widget),
                     child->m_x,
                     child->m_y,
                     child->m_width,
                     child->m_height );
}

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

wxWindow *wxGetActiveWindow()
{
    return wxWindow::FindFocus();
}


wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    gint x;
    gint y;
    GdkModifierType mask;

    gdk_window_get_pointer(NULL, &x, &y, &mask);

    ms.SetX(x);
    ms.SetY(y);
    ms.SetLeftDown(mask & GDK_BUTTON1_MASK);
    ms.SetMiddleDown(mask & GDK_BUTTON2_MASK);
    ms.SetRightDown(mask & GDK_BUTTON3_MASK);

    ms.SetControlDown(mask & GDK_CONTROL_MASK);
    ms.SetShiftDown(mask & GDK_SHIFT_MASK);
    ms.SetAltDown(mask & GDK_MOD1_MASK);
    ms.SetMetaDown(mask & GDK_MOD2_MASK);

    return ms;
}

//-----------------------------------------------------------------------------
// wxWindowGTK
//-----------------------------------------------------------------------------

// in wxUniv/MSW this class is abstract because it doesn't have DoPopupMenu()
// method
#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowGTK, wxWindowBase)
#else // __WXGTK__
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif // __WXUNIVERSAL__/__WXGTK__

void wxWindowGTK::Init()
{
    // GTK specific
    m_widget = (GtkWidget *) NULL;
    m_wxwindow = (GtkWidget *) NULL;
    m_focusWidget = (GtkWidget *) NULL;

    // position/size
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;

    m_sizeSet = false;
    m_hasVMT = false;
    m_needParent = true;
    m_isBeingDeleted = false;
    
    m_showOnIdle= false;

    m_noExpose = false;
    m_nativeSizeEvent = false;

    m_hasScrolling = false;
    m_isScrolling = false;
    m_mouseButtonDown = false;
    m_blockScrollEvent = false;

    // initialize scrolling stuff
    for ( int dir = 0; dir < ScrollDir_Max; dir++ )
    {
        m_scrollBar[dir] = NULL;
        m_scrollPos[dir] = 0;
        m_blockValueChanged[dir] = false;
    }

    m_oldClientWidth =
    m_oldClientHeight = 0;

    m_resizing = false;

    m_insertCallback = (wxInsertChildFunction) NULL;

    m_acceptsFocus = false;
    m_hasFocus = false;

    m_clipPaintRegion = false;

    m_needsStyleChange = false;

    m_cursor = *wxSTANDARD_CURSOR;

    m_imData = NULL;
    m_dirtyTabOrder = false;
}

wxWindowGTK::wxWindowGTK()
{
    Init();
}

wxWindowGTK::wxWindowGTK( wxWindow *parent,
                          wxWindowID id,
                          const wxPoint &pos,
                          const wxSize &size,
                          long style,
                          const wxString &name  )
{
    Init();

    Create( parent, id, pos, size, style, name );
}

bool wxWindowGTK::Create( wxWindow *parent,
                          wxWindowID id,
                          const wxPoint &pos,
                          const wxSize &size,
                          long style,
                          const wxString &name  )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxWindowGTK creation failed") );
        return false;
    }

    m_insertCallback = wxInsertChildInWindow;

    m_widget = gtk_scrolled_window_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(m_widget);

    GtkScrolledWindowClass *scroll_class = GTK_SCROLLED_WINDOW_CLASS( GTK_OBJECT_GET_CLASS(m_widget) );
    scroll_class->scrollbar_spacing = 0;

    gtk_scrolled_window_set_policy( scrolledWindow, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    m_scrollBar[ScrollDir_Horz] = GTK_RANGE(scrolledWindow->hscrollbar);
    m_scrollBar[ScrollDir_Vert] = GTK_RANGE(scrolledWindow->vscrollbar);

    m_wxwindow = gtk_pizza_new();

#ifndef __WXUNIVERSAL__
    GtkPizza *pizza = GTK_PIZZA(m_wxwindow);

    if (HasFlag(wxRAISED_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_OUT );
    }
    else if (HasFlag(wxSUNKEN_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_IN );
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_THIN );
    }
    else
    {
        gtk_pizza_set_shadow_type( pizza, GTK_MYSHADOW_NONE );
    }
#endif // __WXUNIVERSAL__

    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );

    GTK_WIDGET_SET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
    m_acceptsFocus = true;

    // connect various scroll-related events
    for ( int dir = 0; dir < ScrollDir_Max; dir++ )
    {
        // these handlers block mouse events to any window during scrolling
        // such as motion events and prevent GTK and wxWidgets from fighting
        // over where the slider should be
        g_signal_connect(m_scrollBar[dir], "button_press_event",
                         G_CALLBACK(gtk_scrollbar_button_press_event), this);
        g_signal_connect(m_scrollBar[dir], "button_release_event",
                         G_CALLBACK(gtk_scrollbar_button_release_event), this);

        gulong handler_id = g_signal_connect(m_scrollBar[dir], "event_after",
                                G_CALLBACK(gtk_scrollbar_event_after), this);
        g_signal_handler_block(m_scrollBar[dir], handler_id);

        // these handlers get notified when scrollbar slider moves
        g_signal_connect(m_scrollBar[dir], "value_changed",
                         G_CALLBACK(gtk_scrollbar_value_changed), this);
    }

    gtk_widget_show( m_wxwindow );

    if (m_parent)
        m_parent->DoAddChild( this );

    m_focusWidget = m_wxwindow;

    PostCreation();

    return true;
}

wxWindowGTK::~wxWindowGTK()
{
    SendDestroyEvent();

    if (g_focusWindow == this)
        g_focusWindow = NULL;

    if ( g_delayedFocus == this )
        g_delayedFocus = NULL;

    m_isBeingDeleted = true;
    m_hasVMT = false;

    // destroy children before destroying this window itself
    DestroyChildren();

    // unhook focus handlers to prevent stray events being
    // propagated to this (soon to be) dead object
    if (m_focusWidget != NULL)
    {
        g_signal_handlers_disconnect_by_func (m_focusWidget,
                                              (gpointer) gtk_window_focus_in_callback,
                                              this);
        g_signal_handlers_disconnect_by_func (m_focusWidget,
                                              (gpointer) gtk_window_focus_out_callback,
                                              this);
    }

    if (m_widget)
        Show( false );

#ifdef HAVE_XIM
    if (m_ic)
        gdk_ic_destroy (m_ic);
    if (m_icattr)
        gdk_ic_attr_destroy (m_icattr);
#endif

    // delete before the widgets to avoid a crash on solaris
    delete m_imData;

    if (m_wxwindow)
    {
        gtk_widget_destroy( m_wxwindow );
        m_wxwindow = (GtkWidget*) NULL;
    }

    if (m_widget)
    {
        gtk_widget_destroy( m_widget );
        m_widget = (GtkWidget*) NULL;
    }
}

bool wxWindowGTK::PreCreation( wxWindowGTK *parent, const wxPoint &pos,  const wxSize &size )
{
    wxCHECK_MSG( !m_needParent || parent, false, wxT("Need complete parent.") );

    // Use either the given size, or the default if -1 is given.
    // See wxWindowBase for these functions.
    m_width = WidthDefault(size.x) ;
    m_height = HeightDefault(size.y);

    m_x = (int)pos.x;
    m_y = (int)pos.y;

    return true;
}

void wxWindowGTK::PostCreation()
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid window") );

    if (m_wxwindow)
    {
        if (!m_noExpose)
        {
            // these get reported to wxWidgets -> wxPaintEvent

            gtk_pizza_set_external( GTK_PIZZA(m_wxwindow), TRUE );

            g_signal_connect (m_wxwindow, "expose_event",
                              G_CALLBACK (gtk_window_expose_callback), this);

            gtk_widget_set_redraw_on_allocate( GTK_WIDGET(m_wxwindow), HasFlag( wxFULL_REPAINT_ON_RESIZE ) );
        }

        // Create input method handler
        m_imData = new wxGtkIMData;

        // Cannot handle drawing preedited text yet
        gtk_im_context_set_use_preedit( m_imData->context, FALSE );

        g_signal_connect (m_imData->context, "commit",
                          G_CALLBACK (gtk_wxwindow_commit_cb), this);

        // these are called when the "sunken" or "raised" borders are drawn
        g_signal_connect (m_widget, "expose_event",
                          G_CALLBACK (gtk_window_own_expose_callback), this);
    }

    // focus handling

    if (!GTK_IS_WINDOW(m_widget))
    {
        if (m_focusWidget == NULL)
            m_focusWidget = m_widget;

        if (m_wxwindow)
        {
            g_signal_connect (m_focusWidget, "focus_in_event",
                          G_CALLBACK (gtk_window_focus_in_callback), this);
            g_signal_connect (m_focusWidget, "focus_out_event",
                                G_CALLBACK (gtk_window_focus_out_callback), this);
        }
        else
        {
            g_signal_connect_after (m_focusWidget, "focus_in_event",
                          G_CALLBACK (gtk_window_focus_in_callback), this);
            g_signal_connect_after (m_focusWidget, "focus_out_event",
                                G_CALLBACK (gtk_window_focus_out_callback), this);
        }
    }

    // connect to the various key and mouse handlers

    GtkWidget *connect_widget = GetConnectWidget();

    ConnectWidget( connect_widget );

    /* We cannot set colours, fonts and cursors before the widget has
       been realized, so we do this directly after realization */
    g_signal_connect (connect_widget, "realize",
                      G_CALLBACK (gtk_window_realized_callback), this);

    if (m_wxwindow)
    {
        // Catch native resize events
        g_signal_connect (m_wxwindow, "size_allocate",
                          G_CALLBACK (gtk_window_size_callback), this);

        // Initialize XIM support
        g_signal_connect (m_wxwindow, "realize",
                          G_CALLBACK (gtk_wxwindow_realized_callback), this);

        // And resize XIM window
        g_signal_connect (m_wxwindow, "size_allocate",
                          G_CALLBACK (gtk_wxwindow_size_callback), this);
    }

    if (GTK_IS_COMBO(m_widget))
    {
        GtkCombo *gcombo = GTK_COMBO(m_widget);

        g_signal_connect (gcombo->entry, "size_request",
                          G_CALLBACK (wxgtk_combo_size_request_callback),
                          this);
    }
#ifdef GTK_IS_FILE_CHOOSER_BUTTON
    else if (GTK_IS_FILE_CHOOSER_BUTTON(m_widget))
    {
        // If we connect to the "size_request" signal of a GtkFileChooserButton
        // then that control won't be sized properly when placed inside sizers
        // (this can be tested removing this elseif and running XRC or WIDGETS samples)
        // FIXME: what should be done here ?
    }
#endif
    else
    {
        // This is needed if we want to add our windows into native
        // GTK controls, such as the toolbar. With this callback, the
        // toolbar gets to know the correct size (the one set by the
        // programmer). Sadly, it misbehaves for wxComboBox.
        g_signal_connect (m_widget, "size_request",
                          G_CALLBACK (wxgtk_window_size_request_callback),
                          this);
    }

    InheritAttributes();

    m_hasVMT = true;

    // unless the window was created initially hidden (i.e. Hide() had been
    // called before Create()), we should show it at GTK+ level as well
    if ( IsShown() )
        gtk_widget_show( m_widget );
}

void wxWindowGTK::ConnectWidget( GtkWidget *widget )
{
    g_signal_connect (widget, "key_press_event",
                      G_CALLBACK (gtk_window_key_press_callback), this);
    g_signal_connect (widget, "key_release_event",
                      G_CALLBACK (gtk_window_key_release_callback), this);
    g_signal_connect (widget, "button_press_event",
                      G_CALLBACK (gtk_window_button_press_callback), this);
    g_signal_connect (widget, "button_release_event",
                      G_CALLBACK (gtk_window_button_release_callback), this);
    g_signal_connect (widget, "motion_notify_event",
                      G_CALLBACK (gtk_window_motion_notify_callback), this);
    g_signal_connect (widget, "scroll_event",
                      G_CALLBACK (window_scroll_event), this);
    g_signal_connect (widget, "popup_menu",
                     G_CALLBACK (wxgtk_window_popup_menu_callback), this);
    g_signal_connect (widget, "enter_notify_event",
                      G_CALLBACK (gtk_window_enter_callback), this);
    g_signal_connect (widget, "leave_notify_event",
                      G_CALLBACK (gtk_window_leave_callback), this);
}

bool wxWindowGTK::Destroy()
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid window") );

    m_hasVMT = false;

    return wxWindowBase::Destroy();
}

void wxWindowGTK::DoMoveWindow(int x, int y, int width, int height)
{
    gtk_pizza_set_size( GTK_PIZZA(m_parent->m_wxwindow), m_widget, x, y, width, height );
}

void wxWindowGTK::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid window") );
    wxASSERT_MSG( (m_parent != NULL), wxT("wxWindowGTK::SetSize requires parent.\n") );

/*
    printf( "DoSetSize: name %s, x,y,w,h: %d,%d,%d,%d \n", GetName().c_str(), x,y,width,height );
*/

    if (m_resizing) return; /* I don't like recursions */
    m_resizing = true;

    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    if (x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        x = currentX;
    if (y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        y = currentY;
    AdjustForParentClientOrigin(x, y, sizeFlags);

    // calculate the best size if we should auto size the window
    if ( ((sizeFlags & wxSIZE_AUTO_WIDTH) && width == -1) ||
         ((sizeFlags & wxSIZE_AUTO_HEIGHT) && height == -1) )
    {
        const wxSize sizeBest = GetBestSize();
        if ( (sizeFlags & wxSIZE_AUTO_WIDTH) && width == -1 )
            width = sizeBest.x;
        if ( (sizeFlags & wxSIZE_AUTO_HEIGHT) && height == -1 )
            height = sizeBest.y;
    }

    if (width != -1)
        m_width = width;
    if (height != -1)
        m_height = height;

    int minWidth  = GetMinWidth(),
        minHeight = GetMinHeight(),
        maxWidth  = GetMaxWidth(),
        maxHeight = GetMaxHeight();

    if ((minWidth  != -1) && (m_width  < minWidth )) m_width  = minWidth;
    if ((minHeight != -1) && (m_height < minHeight)) m_height = minHeight;
    if ((maxWidth  != -1) && (m_width  > maxWidth )) m_width  = maxWidth;
    if ((maxHeight != -1) && (m_height > maxHeight)) m_height = maxHeight;

#if wxUSE_TOOLBAR_NATIVE
    if (wxDynamicCast(GetParent(), wxToolBar))
    {
       // don't take the x,y values, they're wrong because toolbar sets them
       GtkWidget  *widget = GTK_WIDGET(m_widget);
       gtk_widget_set_size_request (widget, m_width, m_height);
       if (GTK_WIDGET_VISIBLE (widget))
            gtk_widget_queue_resize (widget);
    }
    else
#endif
    if (m_parent->m_wxwindow == NULL) // i.e. wxNotebook
    {
        // don't set the size for children of wxNotebook, just take the values.
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }
    else
    {
        GtkPizza *pizza = GTK_PIZZA(m_parent->m_wxwindow);
        if ((sizeFlags & wxSIZE_ALLOW_MINUS_ONE) == 0)
        {
            if (x != -1) m_x = x + pizza->xoffset;
            if (y != -1) m_y = y + pizza->yoffset;
        }
        else
        {
            m_x = x + pizza->xoffset;
            m_y = y + pizza->yoffset;
        }

        int left_border = 0;
        int right_border = 0;
        int top_border = 0;
        int bottom_border = 0;

        /* the default button has a border around it */
        if (GTK_WIDGET_CAN_DEFAULT(m_widget))
        {
            GtkBorder *default_border = NULL;
            gtk_widget_style_get( m_widget, "default_border", &default_border, NULL );
            if (default_border)
            {
                left_border += default_border->left;
                right_border += default_border->right;
                top_border += default_border->top;
                bottom_border += default_border->bottom;
                g_free( default_border );
            }
        }

        DoMoveWindow( m_x-top_border,
                      m_y-left_border,
                      m_width+left_border+right_border,
                      m_height+top_border+bottom_border );
    }

    if (m_hasScrolling)
    {
        /* Sometimes the client area changes size without the
           whole windows's size changing, but if the whole
           windows's size doesn't change, no wxSizeEvent will
           normally be sent. Here we add an extra test if
           the client test has been changed and this will
           be used then. */
        GetClientSize( &m_oldClientWidth, &m_oldClientHeight );
    }

/*
    wxPrintf( "OnSize sent from " );
    if (GetClassInfo() && GetClassInfo()->GetClassName())
        wxPrintf( GetClassInfo()->GetClassName() );
    wxPrintf( " %d %d %d %d\n", (int)m_x, (int)m_y, (int)m_width, (int)m_height );
*/

    if (!m_nativeSizeEvent)
    {
        wxSizeEvent event( wxSize(m_width,m_height), GetId() );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );
    }

    m_resizing = false;
}

bool wxWindowGTK::GtkShowFromOnIdle()
{
    if (IsShown() && m_showOnIdle && !GTK_WIDGET_VISIBLE (m_widget))
    {
        GtkAllocation alloc;
        alloc.x = m_x;
        alloc.y = m_y;
        alloc.width = m_width;
        alloc.height = m_height;
        gtk_widget_size_allocate( m_widget, &alloc );
        gtk_widget_show( m_widget );
        wxShowEvent eventShow(GetId(), true);
        eventShow.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventShow);
        m_showOnIdle = false;
        return true;
    }
    
    return false;
}

void wxWindowGTK::OnInternalIdle()
{
    // Check if we have to show window now
    if (GtkShowFromOnIdle()) return;

    if ( m_dirtyTabOrder )
    {
        m_dirtyTabOrder = false;
        RealizeTabOrder();
    }

    // Update style if the window was not yet realized
    // and SetBackgroundStyle(wxBG_STYLE_CUSTOM) was called
    if (m_needsStyleChange)
    {
        SetBackgroundStyle(GetBackgroundStyle());
        m_needsStyleChange = false;
    }
    
    // Update invalidated regions.
    GtkUpdate();

    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (cursor.Ok())
    {
        /* I now set the cursor anew in every OnInternalIdle call
           as setting the cursor in a parent window also effects the
           windows above so that checking for the current cursor is
           not possible. */

        if (m_wxwindow)
        {
            GdkWindow *window = GTK_PIZZA(m_wxwindow)->bin_window;
            if (window)
                gdk_window_set_cursor( window, cursor.GetCursor() );

            if (!g_globalCursor.Ok())
                cursor = *wxSTANDARD_CURSOR;

            window = m_widget->window;
            if ((window) && !(GTK_WIDGET_NO_WINDOW(m_widget)))
                gdk_window_set_cursor( window, cursor.GetCursor() );

        }
        else if ( m_widget )
        {
            GdkWindow *window = m_widget->window;
            if ( window && !GTK_WIDGET_NO_WINDOW(m_widget) )
               gdk_window_set_cursor( window, cursor.GetCursor() );
        }
    }

    if (wxUpdateUIEvent::CanUpdate(this))
        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
}

void wxWindowGTK::DoGetSize( int *width, int *height ) const
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (width) (*width) = m_width;
    if (height) (*height) = m_height;
}

void wxWindowGTK::DoSetClientSize( int width, int height )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (m_wxwindow)
    {
        int dw = 0;
        int dh = 0;

        if (m_hasScrolling)
        {
            GetScrollbarWidth(m_widget, dw, dh);
        }

#ifndef __WXUNIVERSAL__
        if (HasFlag(wxRAISED_BORDER) || HasFlag(wxSUNKEN_BORDER))
        {
            // shadow border size is 2
            dw += 2 * 2;
            dh += 2 * 2;
        }
        if (HasFlag(wxSIMPLE_BORDER))
        {
            // simple border size is 1
            dw += 1 * 2;
            dh += 1 * 2;
        }
#endif // __WXUNIVERSAL__

        width += dw;
        height += dh;
    }

    SetSize(width, height);
}

void wxWindowGTK::DoGetClientSize( int *width, int *height ) const
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    int w = m_width;
    int h = m_height;

    if (m_wxwindow)
    {
        int dw = 0;
        int dh = 0;

        if (m_hasScrolling)
        {
            GetScrollbarWidth(m_widget, dw, dh);
        }

#ifndef __WXUNIVERSAL__
        if (HasFlag(wxRAISED_BORDER) || HasFlag(wxSUNKEN_BORDER))
        {
            // shadow border size is 2
            dw += 2 * 2;
            dh += 2 * 2;
        }
        if (HasFlag(wxSIMPLE_BORDER))
        {
            // simple border size is 1
            dw += 1 * 2;
            dh += 1 * 2;
        }
#endif // __WXUNIVERSAL__

        w -= dw;
        h -= dh;
    }

    if (width) *width = w;
    if (height) *height = h;
}

void wxWindowGTK::DoGetPosition( int *x, int *y ) const
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    int dx = 0;
    int dy = 0;
    if (m_parent && m_parent->m_wxwindow)
    {
        GtkPizza *pizza = GTK_PIZZA(m_parent->m_wxwindow);
        dx = pizza->xoffset;
        dy = pizza->yoffset;
    }

    if (x) (*x) = m_x - dx;
    if (y) (*y) = m_y - dy;
}

void wxWindowGTK::DoClientToScreen( int *x, int *y ) const
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (!m_widget->window) return;

    GdkWindow *source = (GdkWindow *) NULL;
    if (m_wxwindow)
        source = GTK_PIZZA(m_wxwindow)->bin_window;
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

    if (x) *x += org_x;
    if (y) *y += org_y;
}

void wxWindowGTK::DoScreenToClient( int *x, int *y ) const
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (!m_widget->window) return;

    GdkWindow *source = (GdkWindow *) NULL;
    if (m_wxwindow)
        source = GTK_PIZZA(m_wxwindow)->bin_window;
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

    if (x) *x -= org_x;
    if (y) *y -= org_y;
}

bool wxWindowGTK::Show( bool show )
{
    wxCHECK_MSG( (m_widget != NULL), false, wxT("invalid window") );

    if (!wxWindowBase::Show(show))
    {
        // nothing to do
        return false;
    }

    if (show)
    {
        if (!m_showOnIdle)
        {
            gtk_widget_show( m_widget );
            wxShowEvent eventShow(GetId(), show);
            eventShow.SetEventObject(this);
            GetEventHandler()->ProcessEvent(eventShow);
        }
    }
    else
    {
        gtk_widget_hide( m_widget );
        wxShowEvent eventShow(GetId(), show);
        eventShow.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventShow);
    }

    return true;
}

static void wxWindowNotifyEnable(wxWindowGTK* win, bool enable)
{
    win->OnParentEnable(enable);

    // Recurse, so that children have the opportunity to Do The Right Thing
    // and reset colours that have been messed up by a parent's (really ancestor's)
    // Enable call
    for ( wxWindowList::compatibility_iterator node = win->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        if (!child->IsKindOf(CLASSINFO(wxDialog)) && !child->IsKindOf(CLASSINFO(wxFrame)))
            wxWindowNotifyEnable(child, enable);
    }
}

bool wxWindowGTK::Enable( bool enable )
{
    wxCHECK_MSG( (m_widget != NULL), false, wxT("invalid window") );

    if (!wxWindowBase::Enable(enable))
    {
        // nothing to do
        return false;
    }

    gtk_widget_set_sensitive( m_widget, enable );
    if ( m_wxwindow )
        gtk_widget_set_sensitive( m_wxwindow, enable );

    wxWindowNotifyEnable(this, enable);

    return true;
}

int wxWindowGTK::GetCharHeight() const
{
    wxCHECK_MSG( (m_widget != NULL), 12, wxT("invalid window") );

    wxFont font = GetFont();
    wxCHECK_MSG( font.Ok(), 12, wxT("invalid font") );

    PangoContext *context = NULL;
    if (m_widget)
        context = gtk_widget_get_pango_context( m_widget );

    if (!context)
        return 0;

    PangoFontDescription *desc = font.GetNativeFontInfo()->description;
    PangoLayout *layout = pango_layout_new(context);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_text(layout, "H", 1);
    PangoLayoutLine *line = (PangoLayoutLine *)pango_layout_get_lines(layout)->data;

    PangoRectangle rect;
    pango_layout_line_get_extents(line, NULL, &rect);

    g_object_unref (layout);

    return (int) PANGO_PIXELS(rect.height);
}

int wxWindowGTK::GetCharWidth() const
{
    wxCHECK_MSG( (m_widget != NULL), 8, wxT("invalid window") );

    wxFont font = GetFont();
    wxCHECK_MSG( font.Ok(), 8, wxT("invalid font") );

    PangoContext *context = NULL;
    if (m_widget)
        context = gtk_widget_get_pango_context( m_widget );

    if (!context)
        return 0;

    PangoFontDescription *desc = font.GetNativeFontInfo()->description;
    PangoLayout *layout = pango_layout_new(context);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_text(layout, "g", 1);
    PangoLayoutLine *line = (PangoLayoutLine *)pango_layout_get_lines(layout)->data;

    PangoRectangle rect;
    pango_layout_line_get_extents(line, NULL, &rect);

    g_object_unref (layout);

    return (int) PANGO_PIXELS(rect.width);
}

void wxWindowGTK::GetTextExtent( const wxString& string,
                                 int *x,
                                 int *y,
                                 int *descent,
                                 int *externalLeading,
                                 const wxFont *theFont ) const
{
    wxFont fontToUse = theFont ? *theFont : GetFont();

    wxCHECK_RET( fontToUse.Ok(), wxT("invalid font") );

    if (string.empty())
    {
        if (x) (*x) = 0;
        if (y) (*y) = 0;
        return;
    }

    PangoContext *context = NULL;
    if (m_widget)
        context = gtk_widget_get_pango_context( m_widget );

    if (!context)
    {
        if (x) (*x) = 0;
        if (y) (*y) = 0;
        return;
    }

    PangoFontDescription *desc = fontToUse.GetNativeFontInfo()->description;
    PangoLayout *layout = pango_layout_new(context);
    pango_layout_set_font_description(layout, desc);
    {
        const wxCharBuffer data = wxGTK_CONV( string );
        if ( data )
            pango_layout_set_text(layout, data, strlen(data));
    }

    PangoRectangle rect;
    pango_layout_get_extents(layout, NULL, &rect);

    if (x) (*x) = (wxCoord) PANGO_PIXELS(rect.width);
    if (y) (*y) = (wxCoord) PANGO_PIXELS(rect.height);
    if (descent)
    {
        PangoLayoutIter *iter = pango_layout_get_iter(layout);
        int baseline = pango_layout_iter_get_baseline(iter);
        pango_layout_iter_free(iter);
        *descent = *y - PANGO_PIXELS(baseline);
    }
    if (externalLeading) (*externalLeading) = 0;  // ??

    g_object_unref (layout);
}

bool wxWindowGTK::GTKSetDelayedFocusIfNeeded()
{
    if ( g_delayedFocus == this )
    {
        if ( GTK_WIDGET_REALIZED(m_widget) )
        {
            gtk_widget_grab_focus(m_widget);
            g_delayedFocus = NULL;

            return true;
        }
    }

    return false;
}

void wxWindowGTK::SetFocus()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );
    if ( m_hasFocus )
    {
        // don't do anything if we already have focus
        return;
    }

    if (m_wxwindow)
    {
        if (!GTK_WIDGET_HAS_FOCUS (m_wxwindow))
        {
            gtk_widget_grab_focus (m_wxwindow);
        }
    }
    else if (m_widget)
    {
        if (GTK_IS_CONTAINER(m_widget))
        {
            gtk_widget_child_focus( m_widget, GTK_DIR_TAB_FORWARD );
        }
        else
        if (GTK_WIDGET_CAN_FOCUS(m_widget) && !GTK_WIDGET_HAS_FOCUS (m_widget) )
        {

            if (!GTK_WIDGET_REALIZED(m_widget))
            {
                // we can't set the focus to the widget now so we remember that
                // it should be focused and will do it later, during the idle
                // time, as soon as we can
                wxLogTrace(TRACE_FOCUS,
                           _T("Delaying setting focus to %s(%s)"),
                           GetClassInfo()->GetClassName(), GetLabel().c_str());

                g_delayedFocus = this;
            }
            else
            {
                wxLogTrace(TRACE_FOCUS,
                           _T("Setting focus to %s(%s)"),
                           GetClassInfo()->GetClassName(), GetLabel().c_str());

                gtk_widget_grab_focus (m_widget);
            }
        }
        else
        {
           wxLogTrace(TRACE_FOCUS,
                      _T("Can't set focus to %s(%s)"),
                      GetClassInfo()->GetClassName(), GetLabel().c_str());
        }
    }
}

bool wxWindowGTK::AcceptsFocus() const
{
    return m_acceptsFocus && wxWindowBase::AcceptsFocus();
}

bool wxWindowGTK::Reparent( wxWindowBase *newParentBase )
{
    wxCHECK_MSG( (m_widget != NULL), false, wxT("invalid window") );

    wxWindowGTK *oldParent = m_parent,
             *newParent = (wxWindowGTK *)newParentBase;

    wxASSERT( GTK_IS_WIDGET(m_widget) );

    if ( !wxWindowBase::Reparent(newParent) )
        return false;

    wxASSERT( GTK_IS_WIDGET(m_widget) );

    /* prevent GTK from deleting the widget arbitrarily */
    gtk_widget_ref( m_widget );

    if (oldParent)
    {
        gtk_container_remove( GTK_CONTAINER(m_widget->parent), m_widget );
    }

    wxASSERT( GTK_IS_WIDGET(m_widget) );

    if (newParent)
    {
        if (GTK_WIDGET_VISIBLE (newParent->m_widget))
        {
            m_showOnIdle = true;
            gtk_widget_hide( m_widget );
        }
    
        /* insert GTK representation */
        (*(newParent->m_insertCallback))(newParent, this);
    }

    /* reverse: prevent GTK from deleting the widget arbitrarily */
    gtk_widget_unref( m_widget );

    return true;
}

void wxWindowGTK::DoAddChild(wxWindowGTK *child)
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid window") );

    wxASSERT_MSG( (child != NULL), wxT("invalid child window") );

    wxASSERT_MSG( (m_insertCallback != NULL), wxT("invalid child insertion function") );

    /* add to list */
    AddChild( child );

    /* insert GTK representation */
    (*m_insertCallback)(this, child);
}

void wxWindowGTK::AddChild(wxWindowBase *child)
{
    wxWindowBase::AddChild(child);
    m_dirtyTabOrder = true;
    if (g_isIdle)
        wxapp_install_idle_handler();
}

void wxWindowGTK::RemoveChild(wxWindowBase *child)
{
    wxWindowBase::RemoveChild(child);
    m_dirtyTabOrder = true;
    if (g_isIdle)
        wxapp_install_idle_handler();
}

void wxWindowGTK::DoMoveInTabOrder(wxWindow *win, MoveKind move)
{
    wxWindowBase::DoMoveInTabOrder(win, move);
    m_dirtyTabOrder = true;
    if (g_isIdle)
        wxapp_install_idle_handler();
}

bool wxWindowGTK::GTKWidgetNeedsMnemonic() const
{
    // none needed by default
    return false;
}

void wxWindowGTK::GTKWidgetDoSetMnemonic(GtkWidget* WXUNUSED(w))
{
    // nothing to do by default since none is needed
}

void wxWindowGTK::RealizeTabOrder()
{
    if (m_wxwindow)
    {
        if ( !m_children.empty() )
        {
            // we don't only construct the correct focus chain but also use
            // this opportunity to update the mnemonic widgets for the widgets
            // that need them

            GList *chain = NULL;
            wxWindowGTK* mnemonicWindow = NULL;

            for ( wxWindowList::const_iterator i = m_children.begin();
                  i != m_children.end();
                  ++i )
            {
                wxWindowGTK *win = *i;

                if ( mnemonicWindow )
                {
                    if ( win->AcceptsFocusFromKeyboard() )
                    {
                        // wxComboBox et al. needs to focus on on a different
                        // widget than m_widget, so if the main widget isn't
                        // focusable try the connect widget
                        GtkWidget* w = win->m_widget;
                        if ( !GTK_WIDGET_CAN_FOCUS(w) )
                        {
                            w = win->GetConnectWidget();
                            if ( !GTK_WIDGET_CAN_FOCUS(w) )
                                w = NULL;
                        }

                        if ( w )
                        {
                            mnemonicWindow->GTKWidgetDoSetMnemonic(w);
                            mnemonicWindow = NULL;
                        }
                    }
                }
                else if ( win->GTKWidgetNeedsMnemonic() )
                {
                    mnemonicWindow = win;
                }

                chain = g_list_prepend(chain, win->m_widget);
            }

            chain = g_list_reverse(chain);

            gtk_container_set_focus_chain(GTK_CONTAINER(m_wxwindow), chain);
            g_list_free(chain);
        }
        else // no children
        {
            gtk_container_unset_focus_chain(GTK_CONTAINER(m_wxwindow));
        }
    }
}

void wxWindowGTK::Raise()
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (m_wxwindow && m_wxwindow->window)
    {
        gdk_window_raise( m_wxwindow->window );
    }
    else if (m_widget->window)
    {
        gdk_window_raise( m_widget->window );
    }
}

void wxWindowGTK::Lower()
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if (m_wxwindow && m_wxwindow->window)
    {
        gdk_window_lower( m_wxwindow->window );
    }
    else if (m_widget->window)
    {
        gdk_window_lower( m_widget->window );
    }
}

bool wxWindowGTK::SetCursor( const wxCursor &cursor )
{
    wxCHECK_MSG( (m_widget != NULL), false, wxT("invalid window") );

    if (cursor == m_cursor)
       return false;

    if (g_isIdle)
        wxapp_install_idle_handler();

    return wxWindowBase::SetCursor( cursor.Ok() ? cursor : *wxSTANDARD_CURSOR );
}

void wxWindowGTK::GTKUpdateCursor()
{
    wxCursor cursor(g_globalCursor.Ok() ? g_globalCursor : GetCursor());
    if ( cursor.Ok() )
    {
        wxArrayGdkWindows windowsThis;
        GdkWindow * const winThis = GTKGetWindow(windowsThis);
        if ( winThis )
        {
            gdk_window_set_cursor(winThis, cursor.GetCursor());
        }
        else
        {
            const size_t count = windowsThis.size();
            for ( size_t n = 0; n < count; n++ )
            {
                gdk_window_set_cursor(windowsThis[n], cursor.GetCursor());
            }
        }
    }
}

void wxWindowGTK::WarpPointer( int x, int y )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    // We provide this function ourselves as it is
    // missing in GDK (top of this file).

    GdkWindow *window = (GdkWindow*) NULL;
    if (m_wxwindow)
        window = GTK_PIZZA(m_wxwindow)->bin_window;
    else
        window = GetConnectWidget()->window;

    if (window)
        gdk_window_warp_pointer( window, x, y );
}

wxWindowGTK::ScrollDir wxWindowGTK::ScrollDirFromRange(GtkRange *range) const
{
    // find the scrollbar which generated the event
    for ( int dir = 0; dir < ScrollDir_Max; dir++ )
    {
        if ( range == m_scrollBar[dir] )
            return (ScrollDir)dir;
    }

    wxFAIL_MSG( _T("event from unknown scrollbar received") );

    return ScrollDir_Max;
}

bool wxWindowGTK::DoScrollByUnits(ScrollDir dir, ScrollUnit unit, int units)
{
    bool changed = false;
    GtkRange* range = m_scrollBar[dir];
    if ( range && units )
    {
        GtkAdjustment* adj = range->adjustment;
        gdouble inc = unit == ScrollUnit_Line ? adj->step_increment
                                              : adj->page_increment;

        const int posOld = int(adj->value + 0.5);
        gtk_range_set_value(range, posOld + units*inc);

        changed = int(adj->value + 0.5) != posOld;
    }

    return changed;
}

bool wxWindowGTK::ScrollLines(int lines)
{
    return DoScrollByUnits(ScrollDir_Vert, ScrollUnit_Line, lines);
}

bool wxWindowGTK::ScrollPages(int pages)
{
    return DoScrollByUnits(ScrollDir_Vert, ScrollUnit_Page, pages);
}

void wxWindowGTK::Refresh( bool eraseBackground, const wxRect *rect )
{
    if (!m_widget)
        return;
    if (!m_widget->window)
        return;

    if (m_wxwindow)
    {
        if (!GTK_PIZZA(m_wxwindow)->bin_window) return;
    
        GdkRectangle gdk_rect,
                    *p;
        if (rect)
        {
            gdk_rect.x = rect->x;
            gdk_rect.y = rect->y;
            gdk_rect.width = rect->width;
            gdk_rect.height = rect->height;
            p = &gdk_rect;
        }
        else // invalidate everything
        {
            p = NULL;
        }

        gdk_window_invalidate_rect( GTK_PIZZA(m_wxwindow)->bin_window, p, TRUE );
    }
}

void wxWindowGTK::Update()
{
    GtkUpdate();

    // when we call Update() we really want to update the window immediately on
    // screen, even if it means flushing the entire queue and hence slowing down
    // everything -- but it should still be done, it's just that Update() should
    // be called very rarely
    gdk_flush();
}

void wxWindowGTK::GtkUpdate()
{
    if (m_wxwindow && GTK_PIZZA(m_wxwindow)->bin_window)
        gdk_window_process_updates( GTK_PIZZA(m_wxwindow)->bin_window, FALSE );
    if (m_widget && m_widget->window)
        gdk_window_process_updates( m_widget->window, FALSE );

    // for consistency with other platforms (and also because it's convenient
    // to be able to update an entire TLW by calling Update() only once), we
    // should also update all our children here
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        node->GetData()->GtkUpdate();
    }
}

void wxWindowGTK::GtkSendPaintEvents()
{
    if (!m_wxwindow)
    {
        m_updateRegion.Clear();
        return;
    }

    // Clip to paint region in wxClientDC
    m_clipPaintRegion = true;

    // widget to draw on
    GtkPizza *pizza = GTK_PIZZA (m_wxwindow);

    if (GetThemeEnabled() && (GetBackgroundStyle() == wxBG_STYLE_SYSTEM))
    {
        // find ancestor from which to steal background
        wxWindow *parent = wxGetTopLevelParent((wxWindow *)this);
        if (!parent)
            parent = (wxWindow*)this;

        if (GTK_WIDGET_MAPPED(parent->m_widget))
        {
            wxRegionIterator upd( m_updateRegion );
            while (upd)
            {
                GdkRectangle rect;
                rect.x = upd.GetX();
                rect.y = upd.GetY();
                rect.width = upd.GetWidth();
                rect.height = upd.GetHeight();

                gtk_paint_flat_box( parent->m_widget->style,
                            pizza->bin_window,
                            (GtkStateType)GTK_WIDGET_STATE(m_wxwindow),
                            GTK_SHADOW_NONE,
                            &rect,
                            parent->m_widget,
                            (char *)"base",
                            0, 0, -1, -1 );

                ++upd;
            }
        }
    }
    else

    {
        wxWindowDC dc( (wxWindow*)this );
        dc.SetClippingRegion( m_updateRegion );

        wxEraseEvent erase_event( GetId(), &dc );
        erase_event.SetEventObject( this );

        GetEventHandler()->ProcessEvent(erase_event);
    }

    wxNcPaintEvent nc_paint_event( GetId() );
    nc_paint_event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( nc_paint_event );

    wxPaintEvent paint_event( GetId() );
    paint_event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( paint_event );

    m_clipPaintRegion = false;

    m_updateRegion.Clear();
}

void wxWindowGTK::SetDoubleBuffered( bool on )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid window") );

    if ( m_wxwindow )
        gtk_widget_set_double_buffered( m_wxwindow, on );
}

void wxWindowGTK::ClearBackground()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );
}

#if wxUSE_TOOLTIPS
void wxWindowGTK::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip(tip);

    if (m_tooltip)
        m_tooltip->Apply( (wxWindow *)this );
}

void wxWindowGTK::ApplyToolTip( GtkTooltips *tips, const wxChar *tip )
{
    wxString tmp( tip );
    gtk_tooltips_set_tip( tips, GetConnectWidget(), wxGTK_CONV(tmp), (gchar*) NULL );
}
#endif // wxUSE_TOOLTIPS

bool wxWindowGTK::SetBackgroundColour( const wxColour &colour )
{
    wxCHECK_MSG( m_widget != NULL, false, wxT("invalid window") );

    if (!wxWindowBase::SetBackgroundColour(colour))
        return false;

    if (colour.Ok())
    {
        // We need the pixel value e.g. for background clearing.
        m_backgroundColour.CalcPixel(gtk_widget_get_colormap(m_widget));
    }

    // apply style change (forceStyle=true so that new style is applied
    // even if the bg colour changed from valid to wxNullColour)
    if (GetBackgroundStyle() != wxBG_STYLE_CUSTOM)
        ApplyWidgetStyle(true);

    return true;
}

bool wxWindowGTK::SetForegroundColour( const wxColour &colour )
{
    wxCHECK_MSG( m_widget != NULL, false, wxT("invalid window") );

    if (!wxWindowBase::SetForegroundColour(colour))
    {
        return false;
    }

    if (colour.Ok())
    {
        // We need the pixel value e.g. for background clearing.
        m_foregroundColour.CalcPixel(gtk_widget_get_colormap(m_widget));
    }

    // apply style change (forceStyle=true so that new style is applied
    // even if the bg colour changed from valid to wxNullColour):
    ApplyWidgetStyle(true);

    return true;
}

PangoContext *wxWindowGTK::GtkGetPangoDefaultContext()
{
    return gtk_widget_get_pango_context( m_widget );
}

GtkRcStyle *wxWindowGTK::CreateWidgetStyle(bool forceStyle)
{
    // do we need to apply any changes at all?
    if ( !forceStyle &&
         !m_font.Ok() &&
         !m_foregroundColour.Ok() && !m_backgroundColour.Ok() )
    {
        return NULL;
    }

    GtkRcStyle *style = gtk_rc_style_new();

    if ( m_font.Ok() )
    {
        style->font_desc =
            pango_font_description_copy( m_font.GetNativeFontInfo()->description );
    }

    if ( m_foregroundColour.Ok() )
    {
        const GdkColor *fg = m_foregroundColour.GetColor();

        style->fg[GTK_STATE_NORMAL] = *fg;
        style->color_flags[GTK_STATE_NORMAL] = GTK_RC_FG;

        style->fg[GTK_STATE_PRELIGHT] = *fg;
        style->color_flags[GTK_STATE_PRELIGHT] = GTK_RC_FG;

        style->fg[GTK_STATE_ACTIVE] = *fg;
        style->color_flags[GTK_STATE_ACTIVE] = GTK_RC_FG;
    }

    if ( m_backgroundColour.Ok() )
    {
        const GdkColor *bg = m_backgroundColour.GetColor();

        style->bg[GTK_STATE_NORMAL] = *bg;
        style->base[GTK_STATE_NORMAL] = *bg;
        style->color_flags[GTK_STATE_NORMAL] = (GtkRcFlags)
            (style->color_flags[GTK_STATE_NORMAL] | GTK_RC_BG | GTK_RC_BASE);

        style->bg[GTK_STATE_PRELIGHT] = *bg;
        style->base[GTK_STATE_PRELIGHT] = *bg;
        style->color_flags[GTK_STATE_PRELIGHT] = (GtkRcFlags)
            (style->color_flags[GTK_STATE_PRELIGHT] | GTK_RC_BG | GTK_RC_BASE);

        style->bg[GTK_STATE_ACTIVE] = *bg;
        style->base[GTK_STATE_ACTIVE] = *bg;
        style->color_flags[GTK_STATE_ACTIVE] = (GtkRcFlags)
            (style->color_flags[GTK_STATE_ACTIVE] | GTK_RC_BG | GTK_RC_BASE);

        style->bg[GTK_STATE_INSENSITIVE] = *bg;
        style->base[GTK_STATE_INSENSITIVE] = *bg;
        style->color_flags[GTK_STATE_INSENSITIVE] = (GtkRcFlags)
            (style->color_flags[GTK_STATE_INSENSITIVE] | GTK_RC_BG | GTK_RC_BASE);
    }

    return style;
}

void wxWindowGTK::ApplyWidgetStyle(bool forceStyle)
{
    GtkRcStyle *style = CreateWidgetStyle(forceStyle);
    if ( style )
    {
        DoApplyWidgetStyle(style);
        gtk_rc_style_unref(style);
    }

    // Style change may affect GTK+'s size calculation:
    InvalidateBestSize();
}

void wxWindowGTK::DoApplyWidgetStyle(GtkRcStyle *style)
{
    if (m_wxwindow)
        gtk_widget_modify_style(m_wxwindow, style);
    else
        gtk_widget_modify_style(m_widget, style);
}

bool wxWindowGTK::SetBackgroundStyle(wxBackgroundStyle style)
{
    wxWindowBase::SetBackgroundStyle(style);

    if (style == wxBG_STYLE_CUSTOM)
    {
        GdkWindow *window = (GdkWindow*) NULL;
        if (m_wxwindow)
            window = GTK_PIZZA(m_wxwindow)->bin_window;
        else
            window = GetConnectWidget()->window;

        if (window)
        {
            // Make sure GDK/X11 doesn't refresh the window
            // automatically.
            gdk_window_set_back_pixmap( window, None, False );
#ifdef __X__
            Display* display = GDK_WINDOW_DISPLAY(window);
            XFlush(display);
#endif
            m_needsStyleChange = false;
        }
        else
            // Do in OnIdle, because the window is not yet available
            m_needsStyleChange = true;

        // Don't apply widget style, or we get a grey background
    }
    else
    {
        // apply style change (forceStyle=true so that new style is applied
        // even if the bg colour changed from valid to wxNullColour):
        ApplyWidgetStyle(true);
    }
    return true;
}

#if wxUSE_DRAG_AND_DROP

void wxWindowGTK::SetDropTarget( wxDropTarget *dropTarget )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );

    GtkWidget *dnd_widget = GetConnectWidget();

    if (m_dropTarget) m_dropTarget->UnregisterWidget( dnd_widget );

    if (m_dropTarget) delete m_dropTarget;
    m_dropTarget = dropTarget;

    if (m_dropTarget) m_dropTarget->RegisterWidget( dnd_widget );
}

#endif // wxUSE_DRAG_AND_DROP

GtkWidget* wxWindowGTK::GetConnectWidget()
{
    GtkWidget *connect_widget = m_widget;
    if (m_wxwindow) connect_widget = m_wxwindow;

    return connect_widget;
}

bool wxWindowGTK::GTKIsOwnWindow(GdkWindow *window) const
{
    wxArrayGdkWindows windowsThis;
    GdkWindow * const winThis = GTKGetWindow(windowsThis);

    return winThis ? window == winThis
                   : windowsThis.Index(window) != wxNOT_FOUND;
}

GdkWindow *wxWindowGTK::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return m_wxwindow ? GTK_PIZZA(m_wxwindow)->bin_window : m_widget->window;
}

bool wxWindowGTK::SetFont( const wxFont &font )
{
    wxCHECK_MSG( m_widget != NULL, false, wxT("invalid window") );

    if (!wxWindowBase::SetFont(font))
        return false;

    // apply style change (forceStyle=true so that new style is applied
    // even if the font changed from valid to wxNullFont):
    ApplyWidgetStyle(true);

    return true;
}

void wxWindowGTK::DoCaptureMouse()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );

    GdkWindow *window = (GdkWindow*) NULL;
    if (m_wxwindow)
        window = GTK_PIZZA(m_wxwindow)->bin_window;
    else
        window = GetConnectWidget()->window;

    wxCHECK_RET( window, _T("CaptureMouse() failed") );

    const wxCursor* cursor = &m_cursor;
    if (!cursor->Ok())
        cursor = wxSTANDARD_CURSOR;

    gdk_pointer_grab( window, FALSE,
                      (GdkEventMask)
                         (GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_POINTER_MOTION_HINT_MASK |
                          GDK_POINTER_MOTION_MASK),
                      (GdkWindow *) NULL,
                      cursor->GetCursor(),
                      (guint32)GDK_CURRENT_TIME );
    g_captureWindow = this;
    g_captureWindowHasMouse = true;
}

void wxWindowGTK::DoReleaseMouse()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );

    wxCHECK_RET( g_captureWindow, wxT("can't release mouse - not captured") );

    g_captureWindow = (wxWindowGTK*) NULL;

    GdkWindow *window = (GdkWindow*) NULL;
    if (m_wxwindow)
        window = GTK_PIZZA(m_wxwindow)->bin_window;
    else
        window = GetConnectWidget()->window;

    if (!window)
        return;

    gdk_pointer_ungrab ( (guint32)GDK_CURRENT_TIME );
}

/* static */
wxWindow *wxWindowBase::GetCapture()
{
    return (wxWindow *)g_captureWindow;
}

bool wxWindowGTK::IsRetained() const
{
    return false;
}

void wxWindowGTK::BlockScrollEvent()
{
    wxASSERT(!m_blockScrollEvent);
    m_blockScrollEvent = true;
}

void wxWindowGTK::UnblockScrollEvent()
{
    wxASSERT(m_blockScrollEvent);
    m_blockScrollEvent = false;
}

void wxWindowGTK::SetScrollbar(int orient,
                               int pos,
                               int thumbVisible,
                               int range,
                               bool WXUNUSED(update))
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );
    wxCHECK_RET( m_wxwindow != NULL, wxT("window needs client area for scrolling") );

    if (range > 0)
    {
        m_hasScrolling = true;
    }
    else
    {
        // GtkRange requires upper > lower
        range =
        thumbVisible = 1;
    }

    if (pos > range - thumbVisible)
        pos = range - thumbVisible;
    if (pos < 0)
        pos = 0;
    GtkAdjustment* adj = m_scrollBar[ScrollDirFromOrient(orient)]->adjustment;
    adj->step_increment = 1;
    adj->page_increment =
    adj->page_size = thumbVisible;
    adj->upper = range;
    SetScrollPos(orient, pos);
    gtk_adjustment_changed(adj);
}

void wxWindowGTK::SetScrollPos(int orient, int pos, bool WXUNUSED(refresh))
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );
    wxCHECK_RET( m_wxwindow != NULL, wxT("window needs client area for scrolling") );

    // This check is more than an optimization. Without it, the slider
    //   will not move smoothly while tracking when using wxScrollHelper.
    if (GetScrollPos(orient) != pos)
    {
        const int dir = ScrollDirFromOrient(orient);
        GtkAdjustment* adj = m_scrollBar[dir]->adjustment;
        const int max = int(adj->upper - adj->page_size);
        if (pos > max)
            pos = max;
        if (pos < 0)
            pos = 0;
        m_scrollPos[dir] =
        adj->value = pos;
        // If a "value_changed" signal emission is not already in progress
        if (!m_blockValueChanged[dir])
        {
            gtk_adjustment_value_changed(adj);
        }
    }
}

int wxWindowGTK::GetScrollThumb(int orient) const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid window") );
    wxCHECK_MSG( m_wxwindow != NULL, 0, wxT("window needs client area for scrolling") );

    return int(m_scrollBar[ScrollDirFromOrient(orient)]->adjustment->page_size);
}

int wxWindowGTK::GetScrollPos( int orient ) const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid window") );
    wxCHECK_MSG( m_wxwindow != NULL, 0, wxT("window needs client area for scrolling") );

    return int(m_scrollBar[ScrollDirFromOrient(orient)]->adjustment->value + 0.5);
}

int wxWindowGTK::GetScrollRange( int orient ) const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid window") );
    wxCHECK_MSG( m_wxwindow != NULL, 0, wxT("window needs client area for scrolling") );

    return int(m_scrollBar[ScrollDirFromOrient(orient)]->adjustment->upper);
}

// Determine if increment is the same as +/-x, allowing for some small
//   difference due to possible inexactness in floating point arithmetic
static inline bool IsScrollIncrement(double increment, double x)
{
    wxASSERT(increment > 0);
    const double tolerance = 1.0 / 1024;
    return fabs(increment - fabs(x)) < tolerance;
}

wxEventType wxWindowGTK::GetScrollEventType(GtkRange* range)
{
    DEBUG_MAIN_THREAD

    if (g_isIdle)
        wxapp_install_idle_handler();

    wxASSERT(range == m_scrollBar[0] || range == m_scrollBar[1]);

    const int barIndex = range == m_scrollBar[1];
    GtkAdjustment* adj = range->adjustment;
    const int value = int(adj->value + 0.5);
    // save previous position
    const double oldPos = m_scrollPos[barIndex];
    // update current position
    m_scrollPos[barIndex] = adj->value;
    // If event should be ignored, or integral position has not changed
    if (!m_hasVMT || g_blockEventsOnDrag || value == int(oldPos + 0.5))
    {
        return wxEVT_NULL;
    }

    wxEventType eventType = wxEVT_SCROLL_THUMBTRACK;
    if (!m_isScrolling)
    {
        // Difference from last change event
        const double diff = adj->value - oldPos;
        const bool isDown = diff > 0;

        if (IsScrollIncrement(adj->step_increment, diff))
        {
            eventType = isDown ? wxEVT_SCROLL_LINEDOWN : wxEVT_SCROLL_LINEUP;
        }
        else if (IsScrollIncrement(adj->page_increment, diff))
        {
            eventType = isDown ? wxEVT_SCROLL_PAGEDOWN : wxEVT_SCROLL_PAGEUP;
        }
        else if (m_mouseButtonDown)
        {
            // Assume track event
            m_isScrolling = true;
        }
    }
    return eventType;
}

void wxWindowGTK::ScrollWindow( int dx, int dy, const wxRect* WXUNUSED(rect) )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid window") );

    wxCHECK_RET( m_wxwindow != NULL, wxT("window needs client area for scrolling") );

    // No scrolling requested.
    if ((dx == 0) && (dy == 0)) return;

    m_clipPaintRegion = true;

    gtk_pizza_scroll( GTK_PIZZA(m_wxwindow), -dx, -dy );

    m_clipPaintRegion = false;
}

void wxWindowGTK::GtkScrolledWindowSetBorder(GtkWidget* w, int wxstyle)
{
    //RN: Note that static controls usually have no border on gtk, so maybe
    //it makes sense to treat that as simply no border at the wx level
    //as well...
    if (!(wxstyle & wxNO_BORDER) && !(wxstyle & wxBORDER_STATIC))
    {
        GtkShadowType gtkstyle;

        if(wxstyle & wxBORDER_RAISED)
            gtkstyle = GTK_SHADOW_OUT;
        else if (wxstyle & wxBORDER_SUNKEN)
            gtkstyle = GTK_SHADOW_IN;
        else if (wxstyle & wxBORDER_DOUBLE)
            gtkstyle = GTK_SHADOW_ETCHED_IN;
        else //default
            gtkstyle = GTK_SHADOW_IN;

        gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW(w),
                                             gtkstyle );
    }
}

void wxWindowGTK::SetWindowStyleFlag( long style )
{
    // Updates the internal variable. NB: Now m_windowStyle bits carry the _new_ style values already
    wxWindowBase::SetWindowStyleFlag(style);
}

// Find the wxWindow at the current mouse position, also returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    wxWindow* found = wxFindWindowAtPoint(pt);
    return found;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
  /* This crashes when used within wxHelpContext,
     so we have to use the X-specific implementation below.
    gint x, y;
    GdkModifierType *mask;
    (void) gdk_window_get_pointer(NULL, &x, &y, mask);

    return wxPoint(x, y);
  */

    int x, y;
    GdkWindow* windowAtPtr = gdk_window_at_pointer(& x, & y);

    Display *display = windowAtPtr ? GDK_WINDOW_XDISPLAY(windowAtPtr) : GDK_DISPLAY();
    Window rootWindow = RootWindowOfScreen (DefaultScreenOfDisplay(display));
    Window rootReturn, childReturn;
    int rootX, rootY, winX, winY;
    unsigned int maskReturn;

    XQueryPointer (display,
           rootWindow,
           &rootReturn,
                   &childReturn,
                   &rootX, &rootY, &winX, &winY, &maskReturn);
    return wxPoint(rootX, rootY);

}

// Needed for implementing e.g. combobox on wxGTK within a modal dialog.
void wxAddGrab(wxWindow* window)
{
    gtk_grab_add( (GtkWidget*) window->GetHandle() );
}

void wxRemoveGrab(wxWindow* window)
{
    gtk_grab_remove( (GtkWidget*) window->GetHandle() );
}

// ----------------------------------------------------------------------------
// wxWinModule
// ----------------------------------------------------------------------------

class wxWinModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxWinModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxWinModule, wxModule)

bool wxWinModule::OnInit()
{
    // g_eraseGC = gdk_gc_new( gdk_get_default_root_window() );
    // gdk_gc_set_fill( g_eraseGC, GDK_SOLID );

    return true;
}

void wxWinModule::OnExit()
{
    if (g_eraseGC)
        g_object_unref (g_eraseGC);
}
