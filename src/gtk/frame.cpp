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
#if wxUSE_TOOLBAR
#include "wx/toolbar.h"
#endif
#if wxUSE_STATUSBAR
#include "wx/statusbr.h"
#endif
#include "wx/dcclient.h"

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"
#include "gdk/gdkkeysyms.h"
#include "gdk/gdkx.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const int wxMENU_HEIGHT    = 27;
const int wxSTATUS_HEIGHT  = 25;
const int wxPLACE_HOLDER   = 0;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// debug
//-----------------------------------------------------------------------------

#ifdef __WXDEBUG__

extern void debug_focus_in( GtkWidget* widget, const wxChar* name, const wxChar *window );

#endif

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
        win->UpdateSize();
    }
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static gint gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxFrame *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

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
    win->UpdateSize();
}

//-----------------------------------------------------------------------------
// "child_detached" of menu bar
//-----------------------------------------------------------------------------

static void gtk_menu_detached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_menuBarDetached = TRUE;
    win->UpdateSize();
}

#if wxUSE_TOOLBAR
//-----------------------------------------------------------------------------
// "child_attached" of tool bar
//-----------------------------------------------------------------------------

static void gtk_toolbar_attached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_toolBarDetached = FALSE;

    win->UpdateSize();
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
    win->UpdateSize();
}
#endif // wxUSE_TOOLBAR

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint 
#if (GTK_MINOR_VERSON > 0)
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *WXUNUSED(event), wxFrame *win )
#else
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxFrame *win )
#endif
{
    if (g_isIdle) 
        wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;

#if (GTK_MINOR_VERSON > 0)
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

    // FIXME I don't know when does it appear, but it's not in 1.2.2
#if GTK_CHECK_VERSION(1, 2, 3)
    /* I haven't been able to set the position of
       the dialog before it is shown, so I set the
       position in "realize" */
    wxLogDebug( "%d %d\n", win->m_x, win->m_y );
    gtk_window_reposition( GTK_WINDOW(widget), win->m_x, win->m_y );
#endif // GTK > 1.2.2

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
    gint flag =	GDK_HINT_POS;
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
    if (win->m_icon != wxNullIcon)
    {
        wxIcon icon( win->m_icon );
        win->m_icon = wxNullIcon;
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

#if wxUSE_TOOLBAR
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
    parent->UpdateSize();
}

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
    EVT_SIZE(wxFrame::OnSize)
    EVT_IDLE(wxFrame::OnIdle)
    EVT_CLOSE(wxFrame::OnCloseWindow)
    EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

void wxFrame::Init()
{
    m_frameMenuBar = (wxMenuBar *) NULL;
#if wxUSE_STATUSBAR
    m_frameStatusBar = (wxStatusBar *) NULL;
#endif // wxUSE_STATUSBAR
#if wxUSE_TOOLBAR
    m_frameToolBar = (wxToolBar *) NULL;
#endif // wxUSE_TOOLBAR
    m_sizeSet = FALSE;
    m_miniEdge = 0;
    m_miniTitle = 0;
    m_mainWidget = (GtkWidget*) NULL;
    m_menuBarDetached = FALSE;
    m_toolBarDetached = FALSE;
    m_insertInClientArea = TRUE;
    m_isFrame = TRUE;
}

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    Init();

    Create( parent, id, title, pos, size, style, name );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
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

    GtkWindowType win_type = GTK_WINDOW_DIALOG;  // this makes window placement work
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

    return TRUE;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = TRUE;

    if (m_frameMenuBar) delete m_frameMenuBar;
    m_frameMenuBar = (wxMenuBar *) NULL;

#if wxUSE_STATUSBAR
    if (m_frameStatusBar) delete m_frameStatusBar;
    m_frameStatusBar = (wxStatusBar *) NULL;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    if (m_frameToolBar) delete m_frameToolBar;
    m_frameToolBar = (wxToolBar *) NULL;
#endif // wxUSE_TOOLBAR

    wxTopLevelWindows.DeleteObject( this );

    if (wxTheApp->GetTopWindow() == this)
        wxTheApp->SetTopWindow( (wxWindow*) NULL );

    if (wxTopLevelWindows.Number() == 0)
        wxTheApp->ExitMainLoop();
}

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

bool wxFrame::Destroy()
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    if (!wxPendingDelete.Member(this)) wxPendingDelete.Append(this);

    return TRUE;
}

void wxFrame::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    /* this shouldn't happen: wxFrame, wxMDIParentFrame and wxMDIChildFrame have m_wxwindow */
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    /* avoid recursions */
    if (m_resizing) return;
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

    // FIXME I don't know when does it appear, but it's not in 1.2.2
#if GTK_CHECK_VERSION(1, 2, 3)
    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y))
        {
            gtk_window_reposition( GTK_WINDOW(m_widget), m_x, m_y );
        }
    }
#endif // GTK > 1.2.2

    if ((m_width != old_width) || (m_height != old_height))
    {
        /* we set the size in GtkOnSize, i.e. mostly the actual resizing is
           done either directly before the frame is shown or in idle time
           so that different calls to SetSize() don't lead to flicker. */
        m_sizeSet = FALSE;
    }

    m_resizing = FALSE;
}

void wxFrame::Centre( int direction )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    int x = 0;
    int y = 0;

    if ((direction & wxHORIZONTAL) == wxHORIZONTAL) x = (gdk_screen_width () - m_width) / 2;
    if ((direction & wxVERTICAL) == wxVERTICAL) y = (gdk_screen_height () - m_height) / 2;

    Move( x, y );
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
#endif

#if wxUSE_TOOLBAR
        /* tool bar */
        if (m_frameToolBar)
        {
            if (!m_toolBarDetached)
            {
                int y = 0;
                m_frameToolBar->GetSize( (int *) NULL, &y );
                (*height) -= y;
            }
            else
                (*height) -= wxPLACE_HOLDER;
        }
#endif

        /* mini edge */
        (*height) -= m_miniEdge*2 + m_miniTitle;
    }
    if (width)
    {
        (*width) -= m_miniEdge*2;
    }
}

void wxFrame::DoSetClientSize( int width, int height )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    printf( "set size %d %d\n", width, height );

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
            if (!m_toolBarDetached)
            {
                int y = 0;
                m_frameToolBar->GetSize( (int *) NULL, &y );
                height += y;
            }
            else
                height += wxPLACE_HOLDER;
        }
#endif

    DoSetSize( -1, -1, width + m_miniEdge*2, height  + m_miniEdge*2 + m_miniTitle, 0 );
}

void wxFrame::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y), int width, int height )
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
    int client_area_y_offset = 0;

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
            int ww = m_width - 2*m_miniEdge;
            int hh = m_frameToolBar->m_height;
	    if (m_toolBarDetached) hh = wxPLACE_HOLDER;
            m_frameToolBar->m_x = xx;
            m_frameToolBar->m_y = yy;
            /* m_frameToolBar->m_height = hh;   don't change the toolbar's reported size
               m_frameToolBar->m_width = ww; */
            gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                  m_frameToolBar->m_widget,
                                  xx, yy, ww, hh );
            client_area_y_offset += hh;
        }
#endif

        int client_x = m_miniEdge;
        int client_y = client_area_y_offset + m_miniEdge + m_miniTitle;
        int client_w = m_width - 2*m_miniEdge;
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

void wxFrame::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
    Destroy();
}

void wxFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
    {
        Layout();
    }
    else
#endif // wxUSE_CONSTRAINTS
    {
        /* do we have exactly one child? */
        wxWindow *child = (wxWindow *)NULL;
        for ( wxNode *node = GetChildren().First(); node; node = node->Next() )
        {
            wxWindow *win = (wxWindow *)node->Data();
            if ( !wxIS_KIND_OF(win,wxFrame) && !wxIS_KIND_OF(win,wxDialog) )
            {
                if (child)
                {
                    /* it's the second one: do nothing */
                    return;
                }

                child = win;
            }
        }

        /* no children at all? */
        if (child)
        {
            /* yes: set it's size to fill all the frame */
            int client_x, client_y;
            DoGetClientSize( &client_x, &client_y );
            child->SetSize( 1, 1, client_x-2, client_y-2 );
        }
    }
}

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

wxMenuBar *wxFrame::GetMenuBar() const
{
    return m_frameMenuBar;
}

void wxFrame::OnMenuHighlight(wxMenuEvent& event)
{
#if wxUSE_STATUSBAR
    if (GetStatusBar())
    {
        // if no help string found, we will clear the status bar text
        wxString helpString;

        int menuId = event.GetMenuId();
        if ( menuId != wxID_SEPARATOR && menuId != -2 /* wxID_TITLE */ )
        {
            wxMenuBar *menuBar = GetMenuBar();
            if ( menuBar )
            {
                // it's ok if we don't find the item because it might belong to
                // the popup menu
                wxMenuItem *item = menuBar->FindItem(menuId);
                if ( item )
                    helpString = item->GetHelp();
            }
        }

        SetStatusText(helpString);
    }
#endif // wxUSE_STATUSBAR
}

#if wxUSE_TOOLBAR
wxToolBar* wxFrame::CreateToolBar( long style, wxWindowID id, const wxString& name )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxCHECK_MSG( m_frameToolBar == NULL, FALSE, wxT("recreating toolbar in wxFrame") );

    m_insertInClientArea = FALSE;

    m_frameToolBar = OnCreateToolBar( style, id, name );

    if (m_frameToolBar) GetChildren().DeleteObject( m_frameToolBar );

    m_insertInClientArea = TRUE;

    m_sizeSet = FALSE;

    return m_frameToolBar;
}

wxToolBar* wxFrame::OnCreateToolBar( long style, wxWindowID id, const wxString& name )
{
    return new wxToolBar( this, id, wxDefaultPosition, wxDefaultSize, style, name );
}

void wxFrame::SetToolBar(wxToolBar *toolbar) 
{ 
    m_frameToolBar = toolbar; 
    if (m_frameToolBar)
    {
        /* insert into toolbar area if not already there */
        if ((m_frameToolBar->m_widget->parent) &&
            (m_frameToolBar->m_widget->parent != m_mainWidget))
        {
            GetChildren().DeleteObject( m_frameToolBar );
	
	    gtk_widget_reparent( m_frameToolBar->m_widget, m_mainWidget );
	    UpdateSize();
	}
    }
}

wxToolBar *wxFrame::GetToolBar() const
{
    return m_frameToolBar;
}
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
wxStatusBar* wxFrame::CreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxCHECK_MSG( m_frameStatusBar == NULL, FALSE, wxT("recreating status bar in wxFrame") );

    m_frameStatusBar = OnCreateStatusBar( number, style, id, name );

    m_sizeSet = FALSE;

    return m_frameStatusBar;
}

wxStatusBar *wxFrame::OnCreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
    wxStatusBar *statusBar = (wxStatusBar *) NULL;

    statusBar = new wxStatusBar(this, id, wxPoint(0, 0), wxSize(100, 20), style, name);

    // Set the height according to the font and the border size
    wxClientDC dc(statusBar);
    dc.SetFont( statusBar->GetFont() );

    long x, y;
    dc.GetTextExtent( "X", &x, &y );

    int height = (int)( (y  * 1.1) + 2* statusBar->GetBorderY());

    statusBar->SetSize( -1, -1, 100, height );

    statusBar->SetFieldsCount( number );
    return statusBar;
}

wxStatusBar *wxFrame::GetStatusBar() const
{
    return m_frameStatusBar;
}

void wxFrame::SetStatusText(const wxString& text, int number)
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set text for") );

    m_frameStatusBar->SetStatusText(text, number);
}

void wxFrame::SetStatusWidths(int n, const int widths_field[] )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set widths for") );

    m_frameStatusBar->SetStatusWidths(n, widths_field);
}
#endif // wxUSE_STATUSBAR

void wxFrame::Command( int id )
{
    wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
    commandEvent.SetInt( id );
    commandEvent.SetEventObject( this );

    wxMenuBar *bar = GetMenuBar();
    if (!bar) return;

    wxMenuItem *item = bar->FindItem(id) ;
    if (item && item->IsCheckable())
    {
        bar->Check(id, !bar->IsChecked(id)) ;
    }

    wxEvtHandler* evtHandler = GetEventHandler();

    evtHandler->ProcessEvent(commandEvent);
}

void wxFrame::SetTitle( const wxString &title )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_title = title;
    if (m_title.IsNull()) m_title = wxT("");
    gtk_window_set_title( GTK_WINDOW(m_widget), title.mbc_str() );
}

void wxFrame::SetIcon( const wxIcon &icon )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_icon = icon;
    if (!icon.Ok()) return;

    if (!m_widget->window) return;

    wxMask *mask = icon.GetMask();
    GdkBitmap *bm = (GdkBitmap *) NULL;
    if (mask) bm = mask->GetBitmap();

    gdk_window_set_icon( m_widget->window, (GdkWindow *) NULL, icon.GetPixmap(), bm );
}

void wxFrame::Maximize(bool WXUNUSED(maximize)) 
{
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
