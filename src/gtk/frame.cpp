/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "frame.h"
#endif

#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/control.h"
#include "wx/app.h"
#include "wx/menu.h"
#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif
#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
#endif
#include "wx/dcclient.h"

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include "wx/gtk/win_gtk.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int wxMENU_HEIGHT    = 27;
const int wxSTATUS_HEIGHT  = 25;
const int wxPLACE_HOLDER   = 0;

// ----------------------------------------------------------------------------
// idle system
// ----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;
extern int g_openDialogs;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

// ----------------------------------------------------------------------------
// data
// ----------------------------------------------------------------------------

extern wxList wxPendingDelete;

// ----------------------------------------------------------------------------
// debug
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__

extern void debug_focus_in( GtkWidget* widget, const wxChar* name, const wxChar *window );

#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// "focus" from m_window
//-----------------------------------------------------------------------------

static gint gtk_frame_focus_callback( GtkWidget *widget, GtkDirectionType WXUNUSED(d), wxWindow *WXUNUSED(win) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // This disables GTK's tab traversal
    gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "focus" );
    return TRUE;
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_frame_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxFrame *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return;

    if ((win->m_width != alloc->width) || (win->m_height != alloc->height))
    {
/*
        wxPrintf( "OnSize from " );
        if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
           wxPrintf( win->GetClassInfo()->GetClassName() );
        wxPrintf( " %d %d %d %d\n", (int)alloc->x,
                                (int)alloc->y,
                                (int)alloc->width,
                                (int)alloc->height );
*/

        win->m_width = alloc->width;
        win->m_height = alloc->height;
        win->GtkUpdateSize();
    }
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static gint gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxFrame *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (g_openDialogs == 0)
        win->Close();

    return TRUE;
}

//-----------------------------------------------------------------------------
// "child_attached" of menu bar
//-----------------------------------------------------------------------------

static void gtk_menu_attached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_menuBarDetached = FALSE;
    win->GtkUpdateSize();
}

//-----------------------------------------------------------------------------
// "child_detached" of menu bar
//-----------------------------------------------------------------------------

static void gtk_menu_detached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_menuBarDetached = TRUE;
    win->GtkUpdateSize();
}

#if wxUSE_TOOLBAR
//-----------------------------------------------------------------------------
// "child_attached" of tool bar
//-----------------------------------------------------------------------------

static void gtk_toolbar_attached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_toolBarDetached = FALSE;

    win->GtkUpdateSize();
}

//-----------------------------------------------------------------------------
// "child_detached" of tool bar
//-----------------------------------------------------------------------------

static void gtk_toolbar_detached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;

    win->m_toolBarDetached = TRUE;
    win->GtkUpdateSize();
}
#endif // wxUSE_TOOLBAR

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint
#if (GTK_MINOR_VERSION > 0)
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *WXUNUSED(event), wxFrame *win )
#else
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxFrame *win )
#endif
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return FALSE;

#if (GTK_MINOR_VERSION > 0)
    int x = 0;
    int y = 0;
    gdk_window_get_root_origin( win->m_widget->window, &x, &y );
    win->m_x = x;
    win->m_y = y;
#else
    win->m_x = event->x;
    win->m_y = event->y;
#endif

    wxMoveEvent mevent( wxPoint(win->m_x,win->m_y), win->GetId() );
    mevent.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( mevent );

    return FALSE;
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

/* we cannot MWM hints and icons before the widget has been realized,
   so we do this directly after realization */

static gint
gtk_frame_realized_callback( GtkWidget *widget, wxFrame *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    /* I haven't been able to set the position of
       the dialog before it is shown, so I set the
       position in "realize" */
    gtk_widget_set_uposition( widget, win->m_x, win->m_y );

    /* all this is for Motif Window Manager "hints" and is supposed to be
       recognized by other WM as well. not tested. */
    long decor = (long) GDK_DECOR_BORDER;
    long func = (long) GDK_FUNC_MOVE;

    if ((win->GetWindowStyle() & wxCAPTION) != 0)
        decor |= GDK_DECOR_TITLE;
    if ((win->GetWindowStyle() & wxSYSTEM_MENU) != 0)
    {
       decor |= GDK_DECOR_MENU;
       func |= GDK_FUNC_CLOSE;
    }
    if ((win->GetWindowStyle() & wxMINIMIZE_BOX) != 0)
    {
        func |= GDK_FUNC_MINIMIZE;
        decor |= GDK_DECOR_MINIMIZE;
    }
    if ((win->GetWindowStyle() & wxMAXIMIZE_BOX) != 0)
    {
        func |= GDK_FUNC_MAXIMIZE;
        decor |= GDK_DECOR_MAXIMIZE;
    }
    if ((win->GetWindowStyle() & wxRESIZE_BORDER) != 0)
    {
       func |= GDK_FUNC_RESIZE;
       decor |= GDK_DECOR_RESIZEH;
    }

    gdk_window_set_decorations( win->m_widget->window, (GdkWMDecoration)decor);
    gdk_window_set_functions( win->m_widget->window, (GdkWMFunction)func);

    /* GTK's shrinking/growing policy */
    if ((win->GetWindowStyle() & wxRESIZE_BORDER) == 0)
        gtk_window_set_policy(GTK_WINDOW(win->m_widget), 0, 0, 1);
    else
        gtk_window_set_policy(GTK_WINDOW(win->m_widget), 1, 1, 1);

    /* set size hints */
    gint flag = 0; // GDK_HINT_POS;
    if ((win->GetMinWidth() != -1) || (win->GetMinHeight() != -1)) flag |= GDK_HINT_MIN_SIZE;
    if ((win->GetMaxWidth() != -1) || (win->GetMaxHeight() != -1)) flag |= GDK_HINT_MAX_SIZE;
    if (flag)
    {
        gdk_window_set_hints( win->m_widget->window,
                              win->m_x, win->m_y,
                              win->GetMinWidth(), win->GetMinHeight(),
                              win->GetMaxWidth(), win->GetMaxHeight(),
                              flag );
    }

    /* reset the icon */
    wxIcon iconOld = win->GetIcon();
    if ( iconOld != wxNullIcon )
    {
        wxIcon icon( iconOld );
        win->SetIcon( wxNullIcon );
        win->SetIcon( icon );
    }

    /* we set the focus to the child that accepts the focus. this
       doesn't really have to be done in "realize" but why not? */
    wxWindowList::Node *node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindow *child = node->GetData();
        if (child->AcceptsFocus())
        {
            child->SetFocus();
            break;
        }

        node = node->GetNext();
    }

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxFrame itself
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InsertChild for wxFrame
//-----------------------------------------------------------------------------

/* Callback for wxFrame. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxWindows requires different ways to insert
 * a child in container classes. */

static void wxInsertChildInFrame( wxFrame* parent, wxWindow* child )
{
    wxASSERT( GTK_IS_WIDGET(child->m_widget) );

    if (!parent->m_insertInClientArea)
    {
        /* these are outside the client area */
        wxFrame* frame = (wxFrame*) parent;
        gtk_pizza_put( GTK_PIZZA(frame->m_mainWidget),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );

#if wxUSE_TOOLBAR_NATIVE
        /* we connect to these events for recalculating the client area
           space when the toolbar is floating */
        if (wxIS_KIND_OF(child,wxToolBar))
        {
            wxToolBar *toolBar = (wxToolBar*) child;
            if (toolBar->GetWindowStyle() & wxTB_DOCKABLE)
            {
                gtk_signal_connect( GTK_OBJECT(toolBar->m_widget), "child_attached",
                    GTK_SIGNAL_FUNC(gtk_toolbar_attached_callback), (gpointer)parent );

                gtk_signal_connect( GTK_OBJECT(toolBar->m_widget), "child_detached",
                    GTK_SIGNAL_FUNC(gtk_toolbar_detached_callback), (gpointer)parent );
            }
        }
#endif // wxUSE_TOOLBAR
    }
    else
    {
        /* these are inside the client area */
        gtk_pizza_put( GTK_PIZZA(parent->m_wxwindow),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );
    }

    /* resize on OnInternalIdle */
    parent->GtkUpdateSize();
}

// ----------------------------------------------------------------------------
// wxFrame creation
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_sizeSet = FALSE;
    m_miniEdge = 0;
    m_miniTitle = 0;
    m_mainWidget = (GtkWidget*) NULL;
    m_menuBarDetached = FALSE;
    m_toolBarDetached = FALSE;
    m_insertInClientArea = TRUE;
    m_isFrame = TRUE;
}

bool wxFrame::Create( wxWindow *parent,
                      wxWindowID id,
                      const wxString &title,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxString &name )
{
    wxTopLevelWindows.Append( this );

    m_needParent = FALSE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxFrame creation failed") );
        return FALSE;
    }

    m_title = title;

    m_insertCallback = (wxInsertChildFunction) wxInsertChildInFrame;

    GtkWindowType win_type = GTK_WINDOW_TOPLEVEL;
    if (style & wxSIMPLE_BORDER) win_type = GTK_WINDOW_POPUP;

    m_widget = gtk_window_new( win_type );

    if (!name.IsEmpty())
        gtk_window_set_wmclass( GTK_WINDOW(m_widget), name.mb_str(), name.mb_str() );

#ifdef __WXDEBUG__
    debug_focus_in( m_widget, wxT("wxFrame::m_widget"), name );
#endif

    gtk_window_set_title( GTK_WINDOW(m_widget), title.mbc_str() );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_frame_delete_callback), (gpointer)this );

    /* m_mainWidget holds the toolbar, the menubar and the client area */
    m_mainWidget = gtk_pizza_new();
    gtk_widget_show( m_mainWidget );
    GTK_WIDGET_UNSET_FLAGS( m_mainWidget, GTK_CAN_FOCUS );
    gtk_container_add( GTK_CONTAINER(m_widget), m_mainWidget );

#ifdef __WXDEBUG__
    debug_focus_in( m_mainWidget, wxT("wxFrame::m_mainWidget"), name );
#endif

    /* m_wxwindow only represents the client area without toolbar and menubar */
    m_wxwindow = gtk_pizza_new();
    gtk_widget_show( m_wxwindow );
    gtk_container_add( GTK_CONTAINER(m_mainWidget), m_wxwindow );

#ifdef __WXDEBUG__
    debug_focus_in( m_wxwindow, wxT("wxFrame::m_wxwindow"), name );
#endif

    /* we donm't allow the frame to get the focus as otherwise
       the frame will grabit at arbitrary fcous changes. */
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    if (m_parent) m_parent->AddChild( this );

    /* the user resized the frame by dragging etc. */
    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_frame_size_callback), (gpointer)this );

    PostCreation();

    /*  we cannot set MWM hints and icons before the widget has
        been realized, so we do this directly after realization */
    gtk_signal_connect( GTK_OBJECT(m_widget), "realize",
                        GTK_SIGNAL_FUNC(gtk_frame_realized_callback), (gpointer) this );

    /* the only way to get the window size is to connect to this event */
    gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
        GTK_SIGNAL_FUNC(gtk_frame_configure_callback), (gpointer)this );

    /* disable native tab traversal */
    gtk_signal_connect( GTK_OBJECT(m_widget), "focus",
        GTK_SIGNAL_FUNC(gtk_frame_focus_callback), (gpointer)this );

    return TRUE;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = TRUE;

    DeleteAllBars();

    wxTopLevelWindows.DeleteObject( this );

    if (wxTheApp->GetTopWindow() == this)
        wxTheApp->SetTopWindow( (wxWindow*) NULL );

    if ((wxTopLevelWindows.Number() == 0) &&
        (wxTheApp->GetExitOnFrameDelete()))
    {
        wxTheApp->ExitMainLoop();
    }
}

// ----------------------------------------------------------------------------
// overridden wxWindow methods
// ----------------------------------------------------------------------------

bool wxFrame::Show( bool show )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    if (show && !m_sizeSet)
    {
        /* by calling GtkOnSize here, we don't have to call
           either after showing the frame, which would entail
           much ugly flicker or from within the size_allocate
           handler, because GTK 1.1.X forbids that. */

        GtkOnSize( m_x, m_y, m_width, m_height );
    }

    return wxWindow::Show( show );
}

void wxFrame::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxFrame") );
}
    
void wxFrame::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    /* this shouldn't happen: wxFrame, wxMDIParentFrame and wxMDIChildFrame have m_wxwindow */
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    /* avoid recursions */
    if (m_resizing)
        return;
    m_resizing = TRUE;

    int old_x = m_x;
    int old_y = m_y;

    int old_width = m_width;
    int old_height = m_height;

    if ((sizeFlags & wxSIZE_ALLOW_MINUS_ONE) == 0)
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

/*
    if ((sizeFlags & wxSIZE_AUTO_WIDTH) == wxSIZE_AUTO_WIDTH)
    {
        if (width == -1) m_width = 80;
    }

    if ((sizeFlags & wxSIZE_AUTO_HEIGHT) == wxSIZE_AUTO_HEIGHT)
    {
       if (height == -1) m_height = 26;
    }
*/

    if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
    if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_maxWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_maxHeight;

    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y))
        {
            gtk_widget_set_uposition( m_widget, m_x, m_y );
        }
    }

    if ((m_width != old_width) || (m_height != old_height))
    {
        /* we set the size in GtkOnSize, i.e. mostly the actual resizing is
           done either directly before the frame is shown or in idle time
           so that different calls to SetSize() don't lead to flicker. */
        m_sizeSet = FALSE;
    }

    m_resizing = FALSE;
}

void wxFrame::DoGetClientSize( int *width, int *height ) const
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxWindow::DoGetClientSize( width, height );
    if (height)
    {
        /* menu bar */
        if (m_frameMenuBar)
        {
            if (!m_menuBarDetached)
                (*height) -= wxMENU_HEIGHT;
            else
                (*height) -= wxPLACE_HOLDER;
        }

#if wxUSE_STATUSBAR
        /* status bar */
        if (m_frameStatusBar) (*height) -= wxSTATUS_HEIGHT;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
        /* tool bar */
        if (m_frameToolBar)
        {
            if (m_toolBarDetached)
            {
                *height -= wxPLACE_HOLDER;
            }
            else
            {
                int x, y;
                m_frameToolBar->GetSize( &x, &y );
                if ( m_frameToolBar->GetWindowStyle() & wxTB_VERTICAL )
                {
                    *width -= x;
                }
                else
                {
                    *height -= y;
                }
            }
        }
#endif // wxUSE_TOOLBAR

        /* mini edge */
        *height -= m_miniEdge*2 + m_miniTitle;
    }
    if (width)
    {
        *width -= m_miniEdge*2;
    }
}

void wxFrame::DoSetClientSize( int width, int height )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

        /* menu bar */
        if (m_frameMenuBar)
        {
            if (!m_menuBarDetached)
                height += wxMENU_HEIGHT;
            else
                height += wxPLACE_HOLDER;
        }

#if wxUSE_STATUSBAR
        /* status bar */
        if (m_frameStatusBar) height += wxSTATUS_HEIGHT;
#endif

#if wxUSE_TOOLBAR
        /* tool bar */
        if (m_frameToolBar)
        {
            if (m_toolBarDetached)
            {
                height += wxPLACE_HOLDER;
            }
            else
            {
                int x, y;
                m_frameToolBar->GetSize( &x, &y );
                if ( m_frameToolBar->GetWindowStyle() & wxTB_VERTICAL )
                {
                    width += x;
                }
                else
                {
                    height += y;
                }
            }
        }
#endif

    DoSetSize( -1, -1, width + m_miniEdge*2, height  + m_miniEdge*2 + m_miniTitle, 0 );
}

void wxFrame::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y),
                         int width, int height )
{
    // due to a bug in gtk, x,y are always 0
    // m_x = x;
    // m_y = y;

    /* avoid recursions */
    if (m_resizing) return;
    m_resizing = TRUE;

    /* this shouldn't happen: wxFrame, wxMDIParentFrame and wxMDIChildFrame have m_wxwindow */
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    m_width = width;
    m_height = height;

    /* space occupied by m_frameToolBar and m_frameMenuBar */
    int client_area_x_offset = 0,
        client_area_y_offset = 0;

    /* wxMDIChildFrame derives from wxFrame but it _is_ a wxWindow as it uses
       wxWindow::Create to create it's GTK equivalent. m_mainWidget is only
       set in wxFrame::Create so it is used to check what kind of frame we
       have here. if m_mainWidget is NULL it is a wxMDIChildFrame and so we
       skip the part which handles m_frameMenuBar, m_frameToolBar and (most
       importantly) m_mainWidget */

    if (m_mainWidget)
    {
        /* check if size is in legal range */
        if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
        if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
        if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_maxWidth;
        if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_maxHeight;

        /* I revert back to wxGTK's original behaviour. m_mainWidget holds the
         * menubar, the toolbar and the client area, which is represented by
         * m_wxwindow.
         * this hurts in the eye, but I don't want to call SetSize()
         * because I don't want to call any non-native functions here. */

        if (m_frameMenuBar)
        {
            int xx = m_miniEdge;
            int yy = m_miniEdge + m_miniTitle;
            int ww = m_width  - 2*m_miniEdge;
            int hh = wxMENU_HEIGHT;
            if (m_menuBarDetached) hh = wxPLACE_HOLDER;
            m_frameMenuBar->m_x = xx;
            m_frameMenuBar->m_y = yy;
            m_frameMenuBar->m_width = ww;
            m_frameMenuBar->m_height = hh;
            gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                  m_frameMenuBar->m_widget,
                                  xx, yy, ww, hh );
            client_area_y_offset += hh;
        }

#if wxUSE_TOOLBAR
        if ((m_frameToolBar) &&
            (m_frameToolBar->m_widget->parent == m_mainWidget))
        {
            int xx = m_miniEdge;
            int yy = m_miniEdge + m_miniTitle;
            if (m_frameMenuBar)
            {
                if (!m_menuBarDetached)
                    yy += wxMENU_HEIGHT;
                else
                    yy += wxPLACE_HOLDER;
            }

            m_frameToolBar->m_x = xx;
            m_frameToolBar->m_y = yy;

            /* don't change the toolbar's reported height/width */
            int ww, hh;
            if ( m_frameToolBar->GetWindowStyle() & wxTB_VERTICAL )
            {
                ww = m_toolBarDetached ? wxPLACE_HOLDER
                                       : m_frameToolBar->m_width;
                hh = m_height - 2*m_miniEdge;

                client_area_x_offset += ww;
            }
            else
            {
                ww = m_width - 2*m_miniEdge;
                hh = m_toolBarDetached ? wxPLACE_HOLDER
                                       : m_frameToolBar->m_height;

                client_area_y_offset += hh;
            }

            gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                  m_frameToolBar->m_widget,
                                  xx, yy, ww, hh );
        }
#endif // wxUSE_TOOLBAR

        int client_x = client_area_x_offset + m_miniEdge;
        int client_y = client_area_y_offset + m_miniEdge + m_miniTitle;
        int client_w = m_width - client_area_x_offset - 2*m_miniEdge;
        int client_h = m_height - client_area_y_offset- 2*m_miniEdge - m_miniTitle;
        gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                              m_wxwindow,
                              client_x, client_y, client_w, client_h );
    }
    else
    {
        /* if there is no m_mainWidget between m_widget and m_wxwindow there
           is no need to set the size or position of m_wxwindow. */
    }

#if wxUSE_STATUSBAR
    if (m_frameStatusBar)
    {
        int xx = 0 + m_miniEdge;
        int yy = m_height - wxSTATUS_HEIGHT - m_miniEdge - client_area_y_offset;
        int ww = m_width - 2*m_miniEdge;
        int hh = wxSTATUS_HEIGHT;
        m_frameStatusBar->m_x = xx;
        m_frameStatusBar->m_y = yy;
        m_frameStatusBar->m_width = ww;
        m_frameStatusBar->m_height = hh;
        gtk_pizza_set_size( GTK_PIZZA(m_wxwindow),
                            m_frameStatusBar->m_widget,
                            xx, yy, ww, hh );
    }
#endif

    /* we actually set the size of a frame here and no-where else */
    gtk_widget_set_usize( m_widget, m_width, m_height );

    m_sizeSet = TRUE;

    // send size event to frame
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    // send size event to status bar
    if (m_frameStatusBar)
    {
        wxSizeEvent event2( wxSize(m_frameStatusBar->m_width,m_frameStatusBar->m_height), m_frameStatusBar->GetId() );
        event2.SetEventObject( m_frameStatusBar );
        m_frameStatusBar->GetEventHandler()->ProcessEvent( event2 );
    }

    m_resizing = FALSE;
}

void wxFrame::MakeModal( bool modal )
{
    if (modal)
        gtk_grab_add( m_widget );
    else
        gtk_grab_remove( m_widget );
}

void wxFrame::OnInternalIdle()
{
    if (!m_sizeSet && GTK_WIDGET_REALIZED(m_wxwindow))
    {
        GtkOnSize( m_x, m_y, m_width, m_height );

        // we'll come back later
        if (g_isIdle)
            wxapp_install_idle_handler();
        return;
    }

    if (m_frameMenuBar) m_frameMenuBar->OnInternalIdle();
#if wxUSE_TOOLBAR
    if (m_frameToolBar) m_frameToolBar->OnInternalIdle();
#endif
#if wxUSE_STATUSBAR
    if (m_frameStatusBar) m_frameStatusBar->OnInternalIdle();
#endif

    wxWindow::OnInternalIdle();
}

// ----------------------------------------------------------------------------
// menu/tool/status bar stuff
// ----------------------------------------------------------------------------

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    m_frameMenuBar = menuBar;

    if (m_frameMenuBar)
    {
        m_frameMenuBar->SetInvokingWindow( this );

        if (m_frameMenuBar->GetParent() != this)
        {
            m_frameMenuBar->SetParent(this);
            gtk_pizza_put( GTK_PIZZA(m_mainWidget),
                m_frameMenuBar->m_widget,
                m_frameMenuBar->m_x,
                m_frameMenuBar->m_y,
                m_frameMenuBar->m_width,
                m_frameMenuBar->m_height );

            if (menuBar->GetWindowStyle() & wxMB_DOCKABLE)
            {
                gtk_signal_connect( GTK_OBJECT(menuBar->m_widget), "child_attached",
                    GTK_SIGNAL_FUNC(gtk_menu_attached_callback), (gpointer)this );

                gtk_signal_connect( GTK_OBJECT(menuBar->m_widget), "child_detached",
                    GTK_SIGNAL_FUNC(gtk_menu_detached_callback), (gpointer)this );
            }

            m_frameMenuBar->Show( TRUE );
        }
    }

    /* resize window in OnInternalIdle */
    m_sizeSet = FALSE;
}

#if wxUSE_TOOLBAR
wxToolBar* wxFrame::CreateToolBar( long style, wxWindowID id, const wxString& name )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_insertInClientArea = FALSE;

    m_frameToolBar = wxFrameBase::CreateToolBar( style, id, name );

    if (m_frameToolBar)
        GetChildren().DeleteObject( m_frameToolBar );

    m_insertInClientArea = TRUE;

    m_sizeSet = FALSE;

    return m_frameToolBar;
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    wxFrameBase::SetToolBar(toolbar);

    if (m_frameToolBar)
    {
        /* insert into toolbar area if not already there */
        if ((m_frameToolBar->m_widget->parent) &&
            (m_frameToolBar->m_widget->parent != m_mainWidget))
        {
            GetChildren().DeleteObject( m_frameToolBar );

            gtk_widget_reparent( m_frameToolBar->m_widget, m_mainWidget );
            GtkUpdateSize();
        }
    }
}

#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR

wxStatusBar* wxFrame::CreateStatusBar(int number,
                                      long style,
                                      wxWindowID id,
                                      const wxString& name)
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    // because it will change when toolbar is added
    m_sizeSet = FALSE;

    return wxFrameBase::CreateStatusBar( number, style, id, name );
}

void wxFrame::PositionStatusBar()
{
    if ( !m_frameStatusBar )
        return;

    m_sizeSet = FALSE;
}
#endif // wxUSE_STATUSBAR

// ----------------------------------------------------------------------------
// frame title/icon
// ----------------------------------------------------------------------------

void wxFrame::SetTitle( const wxString &title )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_title = title;
    gtk_window_set_title( GTK_WINDOW(m_widget), title.mbc_str() );
}

void wxFrame::SetIcon( const wxIcon &icon )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxFrameBase::SetIcon(icon);

    if ( !m_icon.Ok() )
        return;

    if (!m_widget->window)
        return;

    wxMask *mask = icon.GetMask();
    GdkBitmap *bm = (GdkBitmap *) NULL;
    if (mask) bm = mask->GetBitmap();

    gdk_window_set_icon( m_widget->window, (GdkWindow *) NULL, icon.GetPixmap(), bm );
}

// ----------------------------------------------------------------------------
// frame state: maximized/iconized/normal (TODO)
// ----------------------------------------------------------------------------

void wxFrame::Maximize(bool WXUNUSED(maximize))
{
}

bool wxFrame::IsMaximized() const
{
    return FALSE;
}

void wxFrame::Restore()
{
}

void wxFrame::Iconize( bool iconize )
{
   if (iconize)
   {
        XIconifyWindow( GDK_WINDOW_XDISPLAY( m_widget->window ),
                        GDK_WINDOW_XWINDOW( m_widget->window ),
                        DefaultScreen( GDK_DISPLAY() ) );
   }
}

bool wxFrame::IsIconized() const
{
    return FALSE;
}
