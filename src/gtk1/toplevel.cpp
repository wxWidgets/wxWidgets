/////////////////////////////////////////////////////////////////////////////
// Name:        toplevel.cpp
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
    #pragma implementation "toplevel.h"
#endif

#ifdef __VMS
#define XIconifyWindow XICONIFYWINDOW
#endif

#include "wx/defs.h"

#include "wx/dialog.h"
#include "wx/control.h"
#include "wx/app.h"
#include "wx/dcclient.h"

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include "wx/gtk/win_gtk.h"

// ----------------------------------------------------------------------------
// idle system
// ----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;
extern int g_openDialogs;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

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

static void gtk_frame_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxTopLevelWindowGTK *win )
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
        win->m_queuedFullRedraw = TRUE;
        win->GtkUpdateSize();
    }
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static gint gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxTopLevelWindowGTK *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (win->IsEnabled() &&
        (g_openDialogs == 0 || (win->GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)))
        win->Close();

    return TRUE;
}


//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *WXUNUSED(event), wxTopLevelWindowGTK *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT || !win->IsShown())
        return FALSE;

    int x = 0;
    int y = 0;
    gdk_window_get_root_origin( win->m_widget->window, &x, &y );
    win->m_x = x;
    win->m_y = y;

    wxMoveEvent mevent( wxPoint(win->m_x,win->m_y), win->GetId() );
    mevent.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( mevent );

    return FALSE;
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

// we cannot MWM hints and icons before the widget has been realized,
// so we do this directly after realization

static void
gtk_frame_realized_callback( GtkWidget * WXUNUSED(widget), wxTopLevelWindowGTK *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // All this is for Motif Window Manager "hints" and is supposed to be
    // recognized by other WM as well. Not tested.
    gdk_window_set_decorations(win->m_widget->window,
                               (GdkWMDecoration)win->m_gdkDecor);
    gdk_window_set_functions(win->m_widget->window,
                               (GdkWMFunction)win->m_gdkFunc);

    // GTK's shrinking/growing policy
    if ((win->m_gdkFunc & GDK_FUNC_RESIZE) == 0)
        gtk_window_set_policy(GTK_WINDOW(win->m_widget), 0, 0, 1);
    else
        gtk_window_set_policy(GTK_WINDOW(win->m_widget), 1, 1, 1);

    // reset the icon
    wxIcon iconOld = win->GetIcon();
    if ( iconOld != wxNullIcon )
    {
        wxIcon icon( iconOld );
        win->SetIcon( wxNullIcon );
        win->SetIcon( icon );
    }

    // we set the focus to the child that accepts the focus. this
    // doesn't really have to be done in "realize" but why not? 
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
}

//-----------------------------------------------------------------------------
// "map_event" from m_widget
//-----------------------------------------------------------------------------

static void
gtk_frame_map_callback( GtkWidget * WXUNUSED(widget),
                        GdkEvent * WXUNUSED(event),
                        wxTopLevelWindow *win )
{
    win->SetIconizeState(FALSE);
}

//-----------------------------------------------------------------------------
// "unmap_event" from m_widget
//-----------------------------------------------------------------------------

static void
gtk_frame_unmap_callback( GtkWidget * WXUNUSED(widget),
                          GdkEvent * WXUNUSED(event),
                          wxTopLevelWindow *win )
{
    win->SetIconizeState(TRUE);
}

//-----------------------------------------------------------------------------
// "expose_event" of m_client
//-----------------------------------------------------------------------------

static int gtk_window_expose_callback( GtkWidget *widget, GdkEventExpose *gdk_event, wxWindow *win )
{
    GtkPizza *pizza = GTK_PIZZA(widget);

    gtk_paint_flat_box (win->m_widget->style,
                        pizza->bin_window, GTK_STATE_NORMAL,
                        GTK_SHADOW_NONE,
                        &gdk_event->area,
                        win->m_widget,
                        (char *)"base",
                        0, 0, -1, -1);

    return TRUE;
}

//-----------------------------------------------------------------------------
// "draw" of m_client
//-----------------------------------------------------------------------------


static void gtk_window_draw_callback( GtkWidget *widget, GdkRectangle *rect, wxWindow *win )
{
    GtkPizza *pizza = GTK_PIZZA(widget);

    gtk_paint_flat_box (win->m_widget->style,
                        pizza->bin_window, GTK_STATE_NORMAL,
                        GTK_SHADOW_NONE,
                        rect,
                        win->m_widget,
                        (char *)"base",
                        0, 0, -1, -1);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowGTK itself
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InsertChild for wxTopLevelWindowGTK
//-----------------------------------------------------------------------------

/* Callback for wxTopLevelWindowGTK. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxWindows requires different ways to insert
 * a child in container classes. */

static void wxInsertChildInTopLevelWindow( wxTopLevelWindowGTK* parent, wxWindow* child )
{
    wxASSERT( GTK_IS_WIDGET(child->m_widget) );

    if (!parent->m_insertInClientArea)
    {
        // these are outside the client area
        wxTopLevelWindowGTK* frame = (wxTopLevelWindowGTK*) parent;
        gtk_pizza_put( GTK_PIZZA(frame->m_mainWidget),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );
    }
    else
    {
        // these are inside the client area
        gtk_pizza_put( GTK_PIZZA(parent->m_wxwindow),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );
    }

    // resize on OnInternalIdle
    parent->GtkUpdateSize();
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowGTK creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Init()
{
    m_sizeSet = FALSE;
    m_miniEdge = 0;
    m_miniTitle = 0;
    m_mainWidget = (GtkWidget*) NULL;
    m_insertInClientArea = TRUE;
    m_isFrame = TRUE;
    m_isIconized = FALSE;
    m_fsIsShowing = FALSE;
    m_themeEnabled = TRUE;
    m_gdkDecor = m_gdkFunc = 0;
}

bool wxTopLevelWindowGTK::Create( wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& title,
                                  const wxPoint& pos,
                                  const wxSize& sizeOrig,
                                  long style,
                                  const wxString &name )
{
    // always create a frame of some reasonable, even if arbitrary, size (at
    // least for MSW compatibility)
    wxSize size = sizeOrig;
    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeDpy = wxGetDisplaySize();
        if ( size.x == -1 )
            size.x = sizeDpy.x / 3;
        if ( size.y == -1 )
            size.y = sizeDpy.y / 5;
    }

    wxTopLevelWindows.Append( this );

    m_needParent = FALSE;
    
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxTopLevelWindowGTK creation failed") );
        return FALSE;
    }

    m_title = title;

    m_insertCallback = (wxInsertChildFunction) wxInsertChildInTopLevelWindow;

    GtkWindowType win_type = GTK_WINDOW_TOPLEVEL;

    if (style & wxFRAME_TOOL_WINDOW)
        win_type = GTK_WINDOW_POPUP;

    if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
    {
        // there is no more GTK_WINDOW_DIALOG in 2.0
#ifdef __WXGTK20__
        win_type = GTK_WINDOW_TOPLEVEL;
#else
        win_type = GTK_WINDOW_DIALOG;
#endif
    }

    m_widget = gtk_window_new( win_type );

    if (m_parent && (GTK_IS_WINDOW(m_parent->m_widget)) &&
        (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(m_parent->m_widget) );
    }

    if (!name.IsEmpty())
        gtk_window_set_wmclass( GTK_WINDOW(m_widget), name.mb_str(), name.mb_str() );

#ifdef __WXDEBUG__
    debug_focus_in( m_widget, wxT("wxTopLevelWindowGTK::m_widget"), name );
#endif

    gtk_window_set_title( GTK_WINDOW(m_widget), title.mbc_str() );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_frame_delete_callback), (gpointer)this );

    // m_mainWidget holds the toolbar, the menubar and the client area
    m_mainWidget = gtk_pizza_new();
    gtk_widget_show( m_mainWidget );
    GTK_WIDGET_UNSET_FLAGS( m_mainWidget, GTK_CAN_FOCUS );
    gtk_container_add( GTK_CONTAINER(m_widget), m_mainWidget );

    // for m_mainWidget themes
    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "expose_event",
                GTK_SIGNAL_FUNC(gtk_window_expose_callback), (gpointer)this );
    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "draw",
                GTK_SIGNAL_FUNC(gtk_window_draw_callback), (gpointer)this );

#ifdef __WXDEBUG__
    debug_focus_in( m_mainWidget, wxT("wxTopLevelWindowGTK::m_mainWidget"), name );
#endif

    // m_wxwindow only represents the client area without toolbar and menubar
    m_wxwindow = gtk_pizza_new();
    gtk_widget_show( m_wxwindow );
    gtk_container_add( GTK_CONTAINER(m_mainWidget), m_wxwindow );

#ifdef __WXDEBUG__
    debug_focus_in( m_wxwindow, wxT("wxTopLevelWindowGTK::m_wxwindow"), name );
#endif

    // we donm't allow the frame to get the focus as otherwise
    // the frame will grab it at arbitrary focus changes
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    if (m_parent) m_parent->AddChild( this );

    // the user resized the frame by dragging etc.
    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_frame_size_callback), (gpointer)this );

    PostCreation();

    if ((m_x != -1) || (m_y != -1))
        gtk_widget_set_uposition( m_widget, m_x, m_y );
        
    gtk_window_set_default_size( GTK_WINDOW(m_widget), m_width, m_height );

    //  we cannot set MWM hints and icons before the widget has
    //  been realized, so we do this directly after realization
    gtk_signal_connect( GTK_OBJECT(m_widget), "realize",
                        GTK_SIGNAL_FUNC(gtk_frame_realized_callback), (gpointer) this );

    // the only way to get the window size is to connect to this event
    gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
        GTK_SIGNAL_FUNC(gtk_frame_configure_callback), (gpointer)this );

    // map and unmap for iconized state
    gtk_signal_connect( GTK_OBJECT(m_widget), "map_event",
        GTK_SIGNAL_FUNC(gtk_frame_map_callback), (gpointer)this );
    gtk_signal_connect( GTK_OBJECT(m_widget), "unmap_event",
        GTK_SIGNAL_FUNC(gtk_frame_unmap_callback), (gpointer)this );

    // the only way to get the window size is to connect to this event
    gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
        GTK_SIGNAL_FUNC(gtk_frame_configure_callback), (gpointer)this );

    // disable native tab traversal
    gtk_signal_connect( GTK_OBJECT(m_widget), "focus",
        GTK_SIGNAL_FUNC(gtk_frame_focus_callback), (gpointer)this );

    // decorations
    if ((m_miniEdge > 0) || (style & wxSIMPLE_BORDER) || (style & wxNO_BORDER))
    {
        m_gdkDecor = 0;
        m_gdkFunc = 0;
    }
    else
    {
        m_gdkDecor = (long) GDK_DECOR_BORDER;
        m_gdkFunc = (long) GDK_FUNC_MOVE;

        // All this is for Motif Window Manager "hints" and is supposed to be
        // recognized by other WMs as well.
        if ((style & wxCAPTION) != 0)
            m_gdkDecor |= GDK_DECOR_TITLE;
        if ((style & wxSYSTEM_MENU) != 0)
        {
            m_gdkFunc |= GDK_FUNC_CLOSE;
            m_gdkDecor |= GDK_DECOR_MENU;
        }
        if ((style & wxMINIMIZE_BOX) != 0)
        {
            m_gdkFunc |= GDK_FUNC_MINIMIZE;
            m_gdkDecor |= GDK_DECOR_MINIMIZE;
        }
        if ((style & wxMAXIMIZE_BOX) != 0)
        {
            m_gdkFunc |= GDK_FUNC_MAXIMIZE;
            m_gdkDecor |= GDK_DECOR_MAXIMIZE;
        }
        if ((style & wxRESIZE_BORDER) != 0)
        {
           m_gdkFunc |= GDK_FUNC_RESIZE;
           m_gdkDecor |= GDK_DECOR_RESIZEH;
        }
    }

    return TRUE;
}

wxTopLevelWindowGTK::~wxTopLevelWindowGTK()
{
    m_isBeingDeleted = TRUE;
    
    gtk_window_set_focus( GTK_WINDOW(m_widget), NULL );

    wxTopLevelWindows.DeleteObject( this );

    if (wxTheApp->GetTopWindow() == this)
        wxTheApp->SetTopWindow( (wxWindow*) NULL );

    if ((wxTopLevelWindows.Number() == 0) &&
        (wxTheApp->GetExitOnFrameDelete()))
    {
        wxTheApp->ExitMainLoop();
    }
}

bool wxTopLevelWindowGTK::ShowFullScreen(bool show, long style )
{
    if (show == m_fsIsShowing) return FALSE; // return what?

    m_fsIsShowing = show;

    if (show)
    {
        m_fsSaveGdkFunc = m_gdkFunc;
        m_fsSaveGdkDecor = m_gdkDecor;
        m_fsSaveFlag = style;
        GetPosition( &m_fsSaveFrame.x, &m_fsSaveFrame.y );
        GetSize( &m_fsSaveFrame.width, &m_fsSaveFrame.height );

        gtk_widget_hide( m_widget );
        gtk_widget_unrealize( m_widget );

        m_gdkDecor = (long) GDK_DECOR_BORDER;
        m_gdkFunc = (long) GDK_FUNC_MOVE;

        int x;
        int y;
        wxDisplaySize( &x, &y );
        SetSize( 0, 0, x, y );

        gtk_widget_realize( m_widget );
        gtk_widget_show( m_widget );
    }
    else
    {
        gtk_widget_hide( m_widget );
        gtk_widget_unrealize( m_widget );

        m_gdkFunc = m_fsSaveGdkFunc;
        m_gdkDecor = m_fsSaveGdkDecor;

        SetSize( m_fsSaveFrame.x, m_fsSaveFrame.y, m_fsSaveFrame.width, m_fsSaveFrame.height );

        gtk_widget_realize( m_widget );
        gtk_widget_show( m_widget );
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// overridden wxWindow methods
// ----------------------------------------------------------------------------

bool wxTopLevelWindowGTK::Show( bool show )
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

void wxTopLevelWindowGTK::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxTopLevelWindowGTK") );
}

void wxTopLevelWindowGTK::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    // this shouldn't happen: wxFrame, wxMDIParentFrame and wxMDIChildFrame have m_wxwindow
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    // avoid recursions
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

    int minWidth = GetMinWidth(),
        minHeight = GetMinHeight(),
        maxWidth = GetMaxWidth(),
        maxHeight = GetMaxHeight();

    if ((minWidth != -1) && (m_width < minWidth)) m_width = minWidth;
    if ((minHeight != -1) && (m_height < minHeight)) m_height = minHeight;
    if ((maxWidth != -1) && (m_width > maxWidth)) m_width = maxWidth;
    if ((maxHeight != -1) && (m_height > maxHeight)) m_height = maxHeight;

    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y))
        {
            gtk_widget_set_uposition( m_widget, m_x, m_y );
        }
    }

    if ((m_width != old_width) || (m_height != old_height))
    {
        if (m_widget->window)
            gdk_window_resize( m_widget->window, m_width, m_height );
        else
            gtk_window_set_default_size( GTK_WINDOW(m_widget), m_width, m_height );

        /* we set the size in GtkOnSize, i.e. mostly the actual resizing is
           done either directly before the frame is shown or in idle time
           so that different calls to SetSize() don't lead to flicker. */
        m_sizeSet = FALSE;
    }

    m_resizing = FALSE;
}

void wxTopLevelWindowGTK::DoGetClientSize( int *width, int *height ) const
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxWindow::DoGetClientSize( width, height );
    if (height)
    {
        // mini edge
        *height -= m_miniEdge*2 + m_miniTitle;
    }
    if (width)
    {
        *width -= m_miniEdge*2;
    }
}

void wxTopLevelWindowGTK::DoSetClientSize( int width, int height )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    DoSetSize(-1, -1,
              width + m_miniEdge*2, height  + m_miniEdge*2 + m_miniTitle, 0);
}

void wxTopLevelWindowGTK::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y),
                                     int width, int height )
{
    // due to a bug in gtk, x,y are always 0
    // m_x = x;
    // m_y = y;

    // avoid recursions
    if (m_resizing) return;
    m_resizing = TRUE;

    if ( m_wxwindow == NULL ) return;

    m_width = width;
    m_height = height;

    /* wxMDIChildFrame derives from wxFrame but it _is_ a wxWindow as it uses
       wxWindow::Create to create it's GTK equivalent. m_mainWidget is only
       set in wxFrame::Create so it is used to check what kind of frame we
       have here. if m_mainWidget is NULL it is a wxMDIChildFrame and so we
       skip the part which handles m_frameMenuBar, m_frameToolBar and (most
       importantly) m_mainWidget */

    int minWidth = GetMinWidth(),
        minHeight = GetMinHeight(),
        maxWidth = GetMaxWidth(),
        maxHeight = GetMaxHeight();

    if ((minWidth != -1) && (m_width < minWidth)) m_width = minWidth;
    if ((minHeight != -1) && (m_height < minHeight)) m_height = minHeight;
    if ((maxWidth != -1) && (m_width > maxWidth)) m_width = maxWidth;
    if ((maxHeight != -1) && (m_height > maxHeight)) m_height = maxHeight;

    if (m_mainWidget)
    {
        // set size hints
        gint flag = 0; // GDK_HINT_POS;
        if ((minWidth != -1) || (minHeight != -1)) flag |= GDK_HINT_MIN_SIZE;
        if ((maxWidth != -1) || (maxHeight != -1)) flag |= GDK_HINT_MAX_SIZE;
        GdkGeometry geom;
        geom.min_width = minWidth;
        geom.min_height = minHeight;
        geom.max_width = maxWidth;
        geom.max_height = maxHeight;
        gtk_window_set_geometry_hints( GTK_WINDOW(m_widget),
                                       (GtkWidget*) NULL,
                                       &geom,
                                       (GdkWindowHints) flag );

        /* I revert back to wxGTK's original behaviour. m_mainWidget holds the
         * menubar, the toolbar and the client area, which is represented by
         * m_wxwindow.
         * this hurts in the eye, but I don't want to call SetSize()
         * because I don't want to call any non-native functions here. */

        int client_x = m_miniEdge;
        int client_y = m_miniEdge + m_miniTitle;
        int client_w = m_width - 2*m_miniEdge;
        int client_h = m_height - 2*m_miniEdge - m_miniTitle;
        gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                              m_wxwindow,
                              client_x, client_y, client_w, client_h );
    }
    else
    {
        // If there is no m_mainWidget between m_widget and m_wxwindow there
        // is no need to set the size or position of m_wxwindow.
    }

    m_sizeSet = TRUE;

    // send size event to frame
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    m_resizing = FALSE;
}

void wxTopLevelWindowGTK::OnInternalIdle()
{
    if (!m_sizeSet && GTK_WIDGET_REALIZED(m_wxwindow))
    {
        GtkOnSize( m_x, m_y, m_width, m_height );

        // we'll come back later
        if (g_isIdle)
            wxapp_install_idle_handler();
        return;
    }

    wxWindow::OnInternalIdle();
}


// ----------------------------------------------------------------------------
// frame title/icon
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::SetTitle( const wxString &title )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_title = title;
    gtk_window_set_title( GTK_WINDOW(m_widget), title.mbc_str() );
}

void wxTopLevelWindowGTK::SetIcon( const wxIcon &icon )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxTopLevelWindowBase::SetIcon(icon);

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
// frame state: maximized/iconized/normal
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Maximize(bool WXUNUSED(maximize))
{
    wxFAIL_MSG( _T("not implemented") );
}

bool wxTopLevelWindowGTK::IsMaximized() const
{
  //    wxFAIL_MSG( _T("not implemented") );

    // This is an approximation
    return FALSE;
}

void wxTopLevelWindowGTK::Restore()
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxTopLevelWindowGTK::Iconize( bool iconize )
{
   if (iconize)
   {
       GdkWindow *window = m_widget->window;

       // you should do it later, for example from OnCreate() handler
       wxCHECK_RET( window, _T("frame not created yet - can't iconize") );

       XIconifyWindow( GDK_WINDOW_XDISPLAY( window ),
                       GDK_WINDOW_XWINDOW( window ),
                       DefaultScreen( GDK_DISPLAY() ) );
   }
}

bool wxTopLevelWindowGTK::IsIconized() const
{
    return m_isIconized;
}

void wxTopLevelWindowGTK::SetIconizeState(bool iconize)
{
    if ( iconize != m_isIconized )
    {
        m_isIconized = iconize;
        (void)SendIconizeEvent(iconize);
    }
}

