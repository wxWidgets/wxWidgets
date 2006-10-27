/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/toplevel.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __VMS
#define XIconifyWindow XICONIFYWINDOW
#endif

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/icon.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#include "wx/gtk/private.h"
#include "wx/evtloop.h"

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "wx/gtk/win_gtk.h"

#include "wx/unix/utilsx11.h"

// XA_CARDINAL
#include <X11/Xatom.h>

// ----------------------------------------------------------------------------
// data
// ----------------------------------------------------------------------------

extern int              g_openDialogs;
extern wxWindowGTK     *g_delayedFocus;

// the frame that is currently active (i.e. its child has focus). It is
// used to generate wxActivateEvents
static wxTopLevelWindowGTK *g_activeFrame = (wxTopLevelWindowGTK*) NULL;
static wxTopLevelWindowGTK *g_lastActiveFrame = (wxTopLevelWindowGTK*) NULL;

// if we detect that the app has got/lost the focus, we set this variable to
// either TRUE or FALSE and an activate event will be sent during the next
// OnIdle() call and it is reset to -1: this value means that we shouldn't
// send any activate events at all
static int g_sendActivateEvent = -1;

//-----------------------------------------------------------------------------
// RequestUserAttention related functions
//-----------------------------------------------------------------------------

extern "C" {
static void wxgtk_window_set_urgency_hint (GtkWindow *win,
                                           gboolean setting)
{
    wxASSERT_MSG( GTK_WIDGET_REALIZED(win), wxT("wxgtk_window_set_urgency_hint: GdkWindow not realized") );
    GdkWindow *window = GTK_WIDGET(win)->window;
    XWMHints *wm_hints;

    wm_hints = XGetWMHints(GDK_WINDOW_XDISPLAY(window), GDK_WINDOW_XWINDOW(window));

    if (!wm_hints)
        wm_hints = XAllocWMHints();

    if (setting)
        wm_hints->flags |= XUrgencyHint;
    else
        wm_hints->flags &= ~XUrgencyHint;

    XSetWMHints(GDK_WINDOW_XDISPLAY(window), GDK_WINDOW_XWINDOW(window), wm_hints);
    XFree(wm_hints);
}

static gboolean gtk_frame_urgency_timer_callback( wxTopLevelWindowGTK *win )
{
#if GTK_CHECK_VERSION(2,7,0)
    if(!gtk_check_version(2,7,0))
        gtk_window_set_urgency_hint(GTK_WINDOW( win->m_widget ), FALSE);
    else
#endif
        wxgtk_window_set_urgency_hint(GTK_WINDOW( win->m_widget ), FALSE);

    win->m_urgency_hint = -2;
    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "focus_in_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_frame_focus_in_callback( GtkWidget *widget,
                                         GdkEvent *WXUNUSED(event),
                                         wxTopLevelWindowGTK *win )
{
    // don't need to install idle handler, its done from "event" signal

    switch ( g_sendActivateEvent )
    {
        case -1:
            // we've got focus from outside, synthetize wxActivateEvent
            g_sendActivateEvent = 1;
            break;

        case 0:
            // another our window just lost focus, it was already ours before
            // - don't send any wxActivateEvent
            g_sendActivateEvent = -1;
            break;
    }

    g_activeFrame = win;
    g_lastActiveFrame = g_activeFrame;

    // wxPrintf( wxT("active: %s\n"), win->GetTitle().c_str() );

    // MR: wxRequestUserAttention related block
    switch( win->m_urgency_hint )
    {
        default:
            g_source_remove( win->m_urgency_hint );
            // no break, fallthrough to remove hint too
        case -1:
#if GTK_CHECK_VERSION(2,7,0)
            if(!gtk_check_version(2,7,0))
                gtk_window_set_urgency_hint(GTK_WINDOW( widget ), FALSE);
            else
#endif
            {
                wxgtk_window_set_urgency_hint(GTK_WINDOW( widget ), FALSE);
            }

            win->m_urgency_hint = -2;
            break;

        case -2: break;
    }

    wxLogTrace(wxT("activate"), wxT("Activating frame %p (from focus_in)"), g_activeFrame);
    wxActivateEvent event(wxEVT_ACTIVATE, true, g_activeFrame->GetId());
    event.SetEventObject(g_activeFrame);
    g_activeFrame->GetEventHandler()->ProcessEvent(event);

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "focus_out_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_frame_focus_out_callback( GtkWidget *widget,
                                          GdkEventFocus *WXUNUSED(gdk_event),
                                          wxTopLevelWindowGTK *win )
{
    // don't need to install idle handler, its done from "event" signal

    // if the focus goes out of our app alltogether, OnIdle() will send
    // wxActivateEvent, otherwise gtk_window_focus_in_callback() will reset
    // g_sendActivateEvent to -1
    g_sendActivateEvent = 0;

    // wxASSERT_MSG( (g_activeFrame == win), wxT("TLW deactivatd although it wasn't active") );

    // wxPrintf( wxT("inactive: %s\n"), win->GetTitle().c_str() );

    if (g_activeFrame)
    {
        wxLogTrace(wxT("activate"), wxT("Activating frame %p (from focus_in)"), g_activeFrame);
        wxActivateEvent event(wxEVT_ACTIVATE, false, g_activeFrame->GetId());
        event.SetEventObject(g_activeFrame);
        g_activeFrame->GetEventHandler()->ProcessEvent(event);

        g_activeFrame = NULL;
    }

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "focus" from m_window
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_frame_focus_callback( GtkWidget *WXUNUSED(widget),
                                          GtkDirectionType WXUNUSED(d),
                                          wxWindow *WXUNUSED(win) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // This disables GTK's tab traversal
    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_frame_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxTopLevelWindowGTK *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return;

    if ((win->m_width != alloc->width) || (win->m_height != alloc->height))
    {
/*
        wxPrintf( wxT("gtk_frame_size_callback from ") );
        if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
           wxPrintf( win->GetClassInfo()->GetClassName() );
        wxPrintf( wxT(" %d %d %d %d\n"), (int)alloc->x,
                                (int)alloc->y,
                                (int)alloc->width,
                                (int)alloc->height );
*/

        // Tell the wxWindow class about the new size
        win->m_width = alloc->width;
        win->m_height = alloc->height;
        
        win->GtkUpdateSize();
    }
}
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget),
                           GdkEvent *WXUNUSED(event),
                           wxTopLevelWindowGTK *win )
{
    // don't need to install idle handler, its done from "event" signal

    if (win->IsEnabled() &&
        (g_openDialogs == 0 || (win->GetExtraStyle() & wxTOPLEVEL_EX_DIALOG) ||
         win->IsGrabbed()))
        win->Close();

    return TRUE;
}
}


//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_configure_callback( GtkWidget *WXUNUSED(widget),
                              GdkEventConfigure *WXUNUSED(event),
                              wxTopLevelWindowGTK *win )
{
    // don't need to install idle handler, its done from "event" signal

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
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

// we cannot MWM hints and icons before the widget has been realized,
// so we do this directly after realization

extern "C" {
static void
gtk_frame_realized_callback( GtkWidget * WXUNUSED(widget),
                             wxTopLevelWindowGTK *win )
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
        gtk_window_set_resizable(GTK_WINDOW(win->m_widget), FALSE);
    else
        gtk_window_set_policy(GTK_WINDOW(win->m_widget), 1, 1, 1);

    // reset the icon
    wxIconBundle iconsOld = win->GetIcons();
    if ( iconsOld.GetIcon(-1).Ok() )
    {
        win->SetIcon( wxNullIcon );
        win->SetIcons( iconsOld );
    }
}
}

//-----------------------------------------------------------------------------
// "map_event" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_frame_map_callback( GtkWidget * WXUNUSED(widget),
                        GdkEvent * WXUNUSED(event),
                        wxTopLevelWindow *win )
{
    win->SetIconizeState(false);
}
}

//-----------------------------------------------------------------------------
// "unmap_event" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_frame_unmap_callback( GtkWidget * WXUNUSED(widget),
                          GdkEvent * WXUNUSED(event),
                          wxTopLevelWindow *win )
{
    win->SetIconizeState(true);
}
}

//-----------------------------------------------------------------------------
// "expose_event" of m_client
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_expose_callback( GtkWidget *widget,
                            GdkEventExpose *gdk_event,
                            wxWindow *win )
{
    GtkPizza *pizza = GTK_PIZZA(widget);

    gtk_paint_flat_box (win->m_widget->style,
                        pizza->bin_window, GTK_STATE_NORMAL,
                        GTK_SHADOW_NONE,
                        &gdk_event->area,
                        win->m_widget,
                        (char *)"base",
                        0, 0, -1, -1);

    return FALSE;
}
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowGTK itself
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InsertChild for wxTopLevelWindowGTK
//-----------------------------------------------------------------------------

/* Callback for wxTopLevelWindowGTK. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxWidgets requires different ways to insert
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
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowGTK creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Init()
{
    m_sizeSet = false;
    m_miniEdge = 0;
    m_miniTitle = 0;
    m_mainWidget = (GtkWidget*) NULL;
    m_insertInClientArea = true;
    m_isIconized = false;
    m_fsIsShowing = false;
    m_fsSaveFlag = 0;
    m_themeEnabled = true;
    m_gdkDecor = m_gdkFunc = 0;
    m_grabbed = false;

    m_urgency_hint = -2;
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
    size.x = WidthDefault(size.x);
    size.y = HeightDefault(size.y);

    wxTopLevelWindows.Append( this );

    m_needParent = false;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxTopLevelWindowGTK creation failed") );
        return false;
    }

    m_title = title;

    m_insertCallback = (wxInsertChildFunction) wxInsertChildInTopLevelWindow;

    // NB: m_widget may be !=NULL if it was created by derived class' Create,
    //     e.g. in wxTaskBarIconAreaGTK
    if (m_widget == NULL)
    {
        if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
        {
            m_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            // Tell WM that this is a dialog window and make it center
            // on parent by default (this is what GtkDialog ctor does):
            gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                                     GDK_WINDOW_TYPE_HINT_DIALOG);
            gtk_window_set_position(GTK_WINDOW(m_widget),
                                    GTK_WIN_POS_CENTER_ON_PARENT);
        }
        else
        {
            m_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#if GTK_CHECK_VERSION(2,1,0)
            if (!gtk_check_version(2,1,0))
            {
                if (style & wxFRAME_TOOL_WINDOW)
                {
                    gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                                             GDK_WINDOW_TYPE_HINT_UTILITY);

                    // On some WMs, like KDE, a TOOL_WINDOW will still show
                    // on the taskbar, but on Gnome a TOOL_WINDOW will not.
                    // For consistency between WMs and with Windows, we
                    // should set the NO_TASKBAR flag which will apply
                    // the set_skip_taskbar_hint if it is available,
                    // ensuring no taskbar entry will appear.
                    style |= wxFRAME_NO_TASKBAR;
                }
            }
#endif
        }
    }

    wxWindow *topParent = wxGetTopLevelParent(m_parent);
    if (topParent && (((GTK_IS_WINDOW(topParent->m_widget)) &&
                       (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)) ||
                       (style & wxFRAME_FLOAT_ON_PARENT)))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget),
                                      GTK_WINDOW(topParent->m_widget) );
    }

#if GTK_CHECK_VERSION(2,2,0)
    if (!gtk_check_version(2,2,0))
    {
        if (style & wxFRAME_NO_TASKBAR)
        {
            gtk_window_set_skip_taskbar_hint(GTK_WINDOW(m_widget), TRUE);
        }
    }
#endif

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        if (style & wxSTAY_ON_TOP)
        {
            gtk_window_set_keep_above(GTK_WINDOW(m_widget), TRUE);
        }
    }
#endif

#if 0
    if (!name.empty())
        gtk_window_set_role( GTK_WINDOW(m_widget), wxGTK_CONV( name ) );
#endif

    gtk_window_set_title( GTK_WINDOW(m_widget), wxGTK_CONV( title ) );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    g_signal_connect (m_widget, "delete_event",
                      G_CALLBACK (gtk_frame_delete_callback), this);

    // m_mainWidget holds the toolbar, the menubar and the client area
    m_mainWidget = gtk_pizza_new();
    gtk_widget_show( m_mainWidget );
    GTK_WIDGET_UNSET_FLAGS( m_mainWidget, GTK_CAN_FOCUS );
    gtk_container_add( GTK_CONTAINER(m_widget), m_mainWidget );

    if (m_miniEdge == 0) // wxMiniFrame has its own version.
    {
       // For m_mainWidget themes
       g_signal_connect (m_mainWidget, "expose_event",
                         G_CALLBACK (gtk_window_expose_callback), this);
    }

    // m_wxwindow only represents the client area without toolbar and menubar
    m_wxwindow = gtk_pizza_new();
    gtk_widget_show( m_wxwindow );
    gtk_container_add( GTK_CONTAINER(m_mainWidget), m_wxwindow );

    // we donm't allow the frame to get the focus as otherwise
    // the frame will grab it at arbitrary focus changes
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    if (m_parent) m_parent->AddChild( this );

    // the user resized the frame by dragging etc.
    g_signal_connect (m_widget, "size_allocate",
                      G_CALLBACK (gtk_frame_size_callback), this);

    PostCreation();

    if ((m_x != -1) || (m_y != -1))
        gtk_widget_set_uposition( m_widget, m_x, m_y );

    gtk_window_set_default_size( GTK_WINDOW(m_widget), m_width, m_height );

    //  we cannot set MWM hints and icons before the widget has
    //  been realized, so we do this directly after realization
    g_signal_connect (m_widget, "realize",
                      G_CALLBACK (gtk_frame_realized_callback), this);

    // map and unmap for iconized state
    g_signal_connect (m_widget, "map_event",
                      G_CALLBACK (gtk_frame_map_callback), this);
    g_signal_connect (m_widget, "unmap_event",
                      G_CALLBACK (gtk_frame_unmap_callback), this);

    // the only way to get the window size is to connect to this event
    g_signal_connect (m_widget, "configure_event",
                      G_CALLBACK (gtk_frame_configure_callback), this);

    // disable native tab traversal
    g_signal_connect (m_widget, "focus",
                      G_CALLBACK (gtk_frame_focus_callback), this);

    // activation
    g_signal_connect_after (m_widget, "focus_in_event",
                      G_CALLBACK (gtk_frame_focus_in_callback), this);
    g_signal_connect_after (m_widget, "focus_out_event",
                      G_CALLBACK (gtk_frame_focus_out_callback), this);

    // decorations
    if ((style & wxSIMPLE_BORDER) || (style & wxNO_BORDER))
    {
        m_gdkDecor = 0;
        m_gdkFunc = 0;
    }
    else
    if (m_miniEdge > 0)
    {
        m_gdkDecor = 0;
        m_gdkFunc = 0;
        
        if ((style & wxRESIZE_BORDER) != 0)
           m_gdkFunc |= GDK_FUNC_RESIZE;
    }
    else
    {
        m_gdkDecor = (long) GDK_DECOR_BORDER;
        m_gdkFunc = (long) GDK_FUNC_MOVE;

        // All this is for Motif Window Manager "hints" and is supposed to be
        // recognized by other WMs as well.
        if ((style & wxCAPTION) != 0)
        {
            m_gdkDecor |= GDK_DECOR_TITLE;
        }
        if ((style & wxCLOSE_BOX) != 0)
        {
            m_gdkFunc |= GDK_FUNC_CLOSE;
        }
        if ((style & wxSYSTEM_MENU) != 0)
        {
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

    return true;
}

wxTopLevelWindowGTK::~wxTopLevelWindowGTK()
{
    if (m_grabbed)
    {
        wxFAIL_MSG(_T("Window still grabbed"));
        RemoveGrab();
    }

    m_isBeingDeleted = true;

    // it may also be GtkScrolledWindow in the case of an MDI child
    if (GTK_IS_WINDOW(m_widget))
    {
        gtk_window_set_focus( GTK_WINDOW(m_widget), NULL );
    }

    if (g_activeFrame == this)
        g_activeFrame = NULL;
    if (g_lastActiveFrame == this)
        g_lastActiveFrame = NULL;
}

bool wxTopLevelWindowGTK::EnableCloseButton( bool enable )
{
    if (enable)
        m_gdkFunc |= GDK_FUNC_CLOSE;
    else
        m_gdkFunc &= ~GDK_FUNC_CLOSE;
    
    if (GTK_WIDGET_REALIZED(m_widget) && (m_widget->window))
        gdk_window_set_functions( m_widget->window, (GdkWMFunction)m_gdkFunc );
        
    return true;
}

bool wxTopLevelWindowGTK::ShowFullScreen(bool show, long style )
{
    if (show == m_fsIsShowing)
        return false; // return what?

    m_fsIsShowing = show;

    wxX11FullScreenMethod method =
        wxGetFullScreenMethodX11((WXDisplay*)GDK_DISPLAY(),
                                 (WXWindow)GDK_ROOT_WINDOW());

#if GTK_CHECK_VERSION(2,2,0)
    // NB: gtk_window_fullscreen() uses freedesktop.org's WMspec extensions
    //     to switch to fullscreen, which is not always available. We must
    //     check if WM supports the spec and use legacy methods if it
    //     doesn't.
    if ( (method == wxX11_FS_WMSPEC) && !gtk_check_version(2,2,0) )
    {
        if (show)
        {
            m_fsSaveFlag = style;
            gtk_window_fullscreen( GTK_WINDOW( m_widget ) );
        }
        else
        {
            m_fsSaveFlag = 0;
            gtk_window_unfullscreen( GTK_WINDOW( m_widget ) );
        }
    }
    else
#endif // GTK+ >= 2.2.0
    {
        GdkWindow *window = m_widget->window;

        if (show)
        {
            m_fsSaveFlag = style;
            GetPosition( &m_fsSaveFrame.x, &m_fsSaveFrame.y );
            GetSize( &m_fsSaveFrame.width, &m_fsSaveFrame.height );

            int screen_width,screen_height;
            wxDisplaySize( &screen_width, &screen_height );

            gint client_x, client_y, root_x, root_y;
            gint width, height;

            if (method != wxX11_FS_WMSPEC)
            {
                // don't do it always, Metacity hates it
                m_fsSaveGdkFunc = m_gdkFunc;
                m_fsSaveGdkDecor = m_gdkDecor;
                m_gdkFunc = m_gdkDecor = 0;
                gdk_window_set_decorations(window, (GdkWMDecoration)0);
                gdk_window_set_functions(window, (GdkWMFunction)0);
            }

            gdk_window_get_origin (m_widget->window, &root_x, &root_y);
            gdk_window_get_geometry (m_widget->window, &client_x, &client_y,
                         &width, &height, NULL);

            gdk_window_move_resize (m_widget->window, -client_x, -client_y,
                        screen_width + 1, screen_height + 1);

            wxSetFullScreenStateX11((WXDisplay*)GDK_DISPLAY(),
                                    (WXWindow)GDK_ROOT_WINDOW(),
                                    (WXWindow)GDK_WINDOW_XWINDOW(window),
                                    show, &m_fsSaveFrame, method);
        }
        else // hide
        {
            m_fsSaveFlag = 0;
            if (method != wxX11_FS_WMSPEC)
            {
                // don't do it always, Metacity hates it
                m_gdkFunc = m_fsSaveGdkFunc;
                m_gdkDecor = m_fsSaveGdkDecor;
                gdk_window_set_decorations(window, (GdkWMDecoration)m_gdkDecor);
                gdk_window_set_functions(window, (GdkWMFunction)m_gdkFunc);
            }

            wxSetFullScreenStateX11((WXDisplay*)GDK_DISPLAY(),
                                    (WXWindow)GDK_ROOT_WINDOW(),
                                    (WXWindow)GDK_WINDOW_XWINDOW(window),
                                    show, &m_fsSaveFrame, method);

            SetSize(m_fsSaveFrame.x, m_fsSaveFrame.y,
                    m_fsSaveFrame.width, m_fsSaveFrame.height);
        }
    }

    // documented behaviour is to show the window if it's still hidden when
    // showing it full screen
    if ( show && !IsShown() )
        Show();

    return true;
}

// ----------------------------------------------------------------------------
// overridden wxWindow methods
// ----------------------------------------------------------------------------

bool wxTopLevelWindowGTK::Show( bool show )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    if (show == IsShown())
        return true;

    if (show && !m_sizeSet)
    {
        /* by calling GtkOnSize here, we don't have to call
           either after showing the frame, which would entail
           much ugly flicker or from within the size_allocate
           handler, because GTK 1.1.X forbids that. */

        GtkOnSize();
    }

    // This seems no longer to be needed and the call
    // itself is deprecated.
    //
    //if (show)
    //    gtk_widget_set_uposition( m_widget, m_x, m_y );

    return wxWindow::Show( show );
}

void wxTopLevelWindowGTK::Raise()
{
    gtk_window_present( GTK_WINDOW( m_widget ) );
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
    m_resizing = true;

    int old_x = m_x;
    int old_y = m_y;

    int old_width = m_width;
    int old_height = m_height;

    if ((sizeFlags & wxSIZE_ALLOW_MINUS_ONE) == 0)
    {
        if (x != -1) m_x = x;
        if (y != -1) m_y = y;
    }
    else
    {
        m_x = x;
        m_y = y;
    }
    if (width != -1) m_width = width;
    if (height != -1) m_height = height;
    
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

#ifdef __WXGPE__
    // GPE's window manager doesn't like size hints
    // at all, esp. when the user has to use the
    // virtual keyboard.
    minWidth = -1;
    minHeight = -1;
    maxWidth = -1;
    maxHeight = -1;
#endif

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
        m_sizeSet = false;
    }

    m_resizing = false;
}

void wxTopLevelWindowGTK::DoGetClientSize( int *width, int *height ) const
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    if (height)
    {
        *height = m_height - 2 * m_miniEdge - m_miniTitle;
        if (*height < 0)
            *height = 0;
    }
    if (width)
    {
        *width = m_width - 2 * m_miniEdge;
        if (*width < 0)
            *width = 0;
    }
}

void wxTopLevelWindowGTK::DoSetClientSize( int width, int height )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    DoSetSize(-1, -1,
              width + m_miniEdge*2, height  + m_miniEdge*2 + m_miniTitle, 0);
}

void wxTopLevelWindowGTK::GtkOnSize()
{
    // avoid recursions
    if (m_resizing) return;
    m_resizing = true;

    if ( m_wxwindow == NULL ) return;

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

#ifdef __WXGPE__
    // GPE's window manager doesn't like size hints
    // at all, esp. when the user has to use the
    // virtual keyboard.
    minWidth = -1;
    minHeight = -1;
    maxWidth = -1;
    maxHeight = -1;
#endif

    if ((minWidth != -1) && (m_width < minWidth)) m_width = minWidth;
    if ((minHeight != -1) && (m_height < minHeight)) m_height = minHeight;
    if ((maxWidth != -1) && (m_width > maxWidth)) m_width = maxWidth;
    if ((maxHeight != -1) && (m_height > maxHeight)) m_height = maxHeight;

    if (m_mainWidget)
    {
        // set size hints
        gint            flag = 0; // GDK_HINT_POS;
        GdkGeometry     geom;

        if ((minWidth != -1) || (minHeight != -1)) flag |= GDK_HINT_MIN_SIZE;
        if ((maxWidth != -1) || (maxHeight != -1)) flag |= GDK_HINT_MAX_SIZE;

        geom.min_width = minWidth;
        geom.min_height = minHeight;

            // Because of the way we set GDK_HINT_MAX_SIZE above, if either of
            // maxHeight or maxWidth is set, we must set them both, else the
            // remaining -1 will be taken literally.

            // I'm certain this also happens elsewhere, and is the probable
            // cause of other such things as:
            // Gtk-WARNING **: gtk_widget_size_allocate():
            //       attempt to allocate widget with width 65535 and height 600
            // but I don't have time to track them all now..
            //
            // Really we need to encapulate all this height/width business and
            // stop any old method from ripping at the members directly and
            // scattering -1's without regard for who might resolve them later.

        geom.max_width = ( maxHeight == -1 ) ? maxWidth
                         : ( maxWidth == -1 ) ? wxGetDisplaySize().GetWidth()
                           : maxWidth ;

        geom.max_height = ( maxWidth == -1 ) ? maxHeight    // ( == -1 here )
                          : ( maxHeight == -1 ) ? wxGetDisplaySize().GetHeight()
                            : maxHeight ;

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
        if (client_w < 0)
            client_w = 0;
        if (client_h < 0)
            client_h = 0;

        // Let the parent perform the resize
        gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                              m_wxwindow,
                              client_x, client_y, client_w, client_h );
    }
    else
    {
        // If there is no m_mainWidget between m_widget and m_wxwindow there
        // is no need to set the size or position of m_wxwindow.
    }

    m_sizeSet = true;

    // send size event to frame
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    m_resizing = false;
}

void wxTopLevelWindowGTK::OnInternalIdle()
{
    if (!m_sizeSet && GTK_WIDGET_REALIZED(m_wxwindow))
    {
        GtkOnSize();

        // we'll come back later
        if (g_isIdle)
            wxapp_install_idle_handler();
        return;
    }

    // set the focus if not done yet and if we can already do it
    if ( GTK_WIDGET_REALIZED(m_wxwindow) )
    {
        if ( g_delayedFocus &&
             wxGetTopLevelParent((wxWindow*)g_delayedFocus) == this )
        {
            wxLogTrace(_T("focus"),
                       _T("Setting focus from wxTLW::OnIdle() to %s(%s)"),
                       g_delayedFocus->GetClassInfo()->GetClassName(),
                       g_delayedFocus->GetLabel().c_str());

            g_delayedFocus->SetFocus();
            g_delayedFocus = NULL;
        }
    }

    wxWindow::OnInternalIdle();

    // Synthetize activate events.
    if ( g_sendActivateEvent != -1 )
    {
        bool activate = g_sendActivateEvent != 0;

        // if (!activate) wxPrintf( wxT("de") );
        // wxPrintf( wxT("activate\n") );

        // do it only once
        g_sendActivateEvent = -1;

        wxTheApp->SetActive(activate, (wxWindow *)g_lastActiveFrame);
    }
}

// ----------------------------------------------------------------------------
// frame title/icon
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::SetTitle( const wxString &title )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    if ( title == m_title )
        return;

    m_title = title;

    gtk_window_set_title( GTK_WINDOW(m_widget), wxGTK_CONV( title ) );
}

void wxTopLevelWindowGTK::SetIcon( const wxIcon &icon )
{
    SetIcons( wxIconBundle( icon ) );
}

void wxTopLevelWindowGTK::SetIcons( const wxIconBundle &icons )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxTopLevelWindowBase::SetIcons( icons );

    GList *list = NULL;
    size_t max = icons.m_icons.GetCount();

    for (size_t i = 0; i < max; i++)
    {
        if (icons.m_icons[i].Ok())
        {
            list = g_list_prepend(list, icons.m_icons[i].GetPixbuf());
        }
    }
    gtk_window_set_icon_list(GTK_WINDOW(m_widget), list);
    g_list_free(list);
}

// ----------------------------------------------------------------------------
// frame state: maximized/iconized/normal
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Maximize(bool maximize)
{
    if (maximize)
        gtk_window_maximize( GTK_WINDOW( m_widget ) );
    else
        gtk_window_unmaximize( GTK_WINDOW( m_widget ) );
}

bool wxTopLevelWindowGTK::IsMaximized() const
{
    if(!m_widget->window)
        return false;

    return gdk_window_get_state(m_widget->window) & GDK_WINDOW_STATE_MAXIMIZED;
}

void wxTopLevelWindowGTK::Restore()
{
    // "Present" seems similar enough to "restore"
    gtk_window_present( GTK_WINDOW( m_widget ) );
}

void wxTopLevelWindowGTK::Iconize( bool iconize )
{
    if (iconize)
        gtk_window_iconify( GTK_WINDOW( m_widget ) );
    else
        gtk_window_deiconify( GTK_WINDOW( m_widget ) );
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

void wxTopLevelWindowGTK::AddGrab()
{
    if (!m_grabbed)
    {
        m_grabbed = true;
        gtk_grab_add( m_widget );
        wxEventLoop().Run();
        gtk_grab_remove( m_widget );
    }
}

void wxTopLevelWindowGTK::RemoveGrab()
{
    if (m_grabbed)
    {
        gtk_main_quit();
        m_grabbed = false;
    }
}


// helper
static bool do_shape_combine_region(GdkWindow* window, const wxRegion& region)
{
    if (window)
    {
        if (region.IsEmpty())
        {
            gdk_window_shape_combine_mask(window, NULL, 0, 0);
        }
        else
        {
            gdk_window_shape_combine_region(window, region.GetRegion(), 0, 0);
            return true;
        }
    }
    return false;
}


bool wxTopLevelWindowGTK::SetShape(const wxRegion& region)
{
    wxCHECK_MSG( HasFlag(wxFRAME_SHAPED), false,
                 _T("Shaped windows must be created with the wxFRAME_SHAPED style."));

    GdkWindow *window = NULL;
    if (m_wxwindow)
    {
        window = GTK_PIZZA(m_wxwindow)->bin_window;
        do_shape_combine_region(window, region);
    }
    window = m_widget->window;
    return do_shape_combine_region(window, region);
}

bool wxTopLevelWindowGTK::IsActive()
{
    return (this == (wxTopLevelWindowGTK*)g_activeFrame);
}

void wxTopLevelWindowGTK::RequestUserAttention(int flags)
{
    bool new_hint_value = false;

    // FIXME: This is a workaround to focus handling problem
    // If RequestUserAttention is called for example right after a wxSleep, OnInternalIdle hasn't
    // yet been processed, and the internal focus system is not up to date yet.
    // wxYieldIfNeeded ensures the processing of it, but can have unwanted side effects - MR
    ::wxYieldIfNeeded();

    if(m_urgency_hint >= 0)
        g_source_remove(m_urgency_hint);

    m_urgency_hint = -2;

    if( GTK_WIDGET_REALIZED(m_widget) && !IsActive() )
    {
        new_hint_value = true;

        if (flags & wxUSER_ATTENTION_INFO)
        {
            m_urgency_hint = g_timeout_add(5000, (GSourceFunc)gtk_frame_urgency_timer_callback, this);
        } else {
            m_urgency_hint = -1;
        }
    }

#if GTK_CHECK_VERSION(2,7,0)
    if(!gtk_check_version(2,7,0))
        gtk_window_set_urgency_hint(GTK_WINDOW( m_widget ), new_hint_value);
    else
#endif
        wxgtk_window_set_urgency_hint(GTK_WINDOW( m_widget ), new_hint_value);
}

void wxTopLevelWindowGTK::SetWindowStyleFlag( long style )
{
#if defined(__WXGTK24__) || GTK_CHECK_VERSION(2,2,0)
    // Store which styles were changed
    long styleChanges = style ^ m_windowStyle;
#endif

    // Process wxWindow styles. This also updates the internal variable
    // Therefore m_windowStyle bits carry now the _new_ style values
    wxWindow::SetWindowStyleFlag(style);

    // just return for now if widget does not exist yet
    if (!m_widget)
        return;

#ifdef __WXGTK24__
    if ( (styleChanges & wxSTAY_ON_TOP) && !gtk_check_version(2,4,0) )
        gtk_window_set_keep_above(GTK_WINDOW(m_widget), m_windowStyle & wxSTAY_ON_TOP);
#endif // GTK+ 2.4
#if GTK_CHECK_VERSION(2,2,0)
    if ( (styleChanges & wxFRAME_NO_TASKBAR) && !gtk_check_version(2,2,0) )
    {
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(m_widget), m_windowStyle & wxFRAME_NO_TASKBAR);
    }
#endif // GTK+ 2.2
}

#include <X11/Xlib.h>

/* Get the X Window between child and the root window.
   This should usually be the WM managed XID */
static Window wxGetTopmostWindowX11(Display *dpy, Window child)
{
    Window root, parent;
    Window* children;
    unsigned int nchildren;

    XQueryTree(dpy, child, &root, &parent, &children, &nchildren);
    XFree(children);

    while (parent != root) {
        child = parent;
        XQueryTree(dpy, child, &root, &parent, &children, &nchildren);
        XFree(children);
    }

    return child;
}

bool wxTopLevelWindowGTK::SetTransparent(wxByte alpha)
{
    if (!m_widget || !m_widget->window)
        return false;

    Display* dpy = GDK_WINDOW_XDISPLAY (m_widget->window);
    // We need to get the X Window that has the root window as the immediate parent
    // and m_widget->window as a child. This should be the X Window that the WM manages and
    // from which the opacity property is checked from.
    Window win = wxGetTopmostWindowX11(dpy, GDK_WINDOW_XID (m_widget->window));

    unsigned int opacity = alpha * 0x1010101;

    // Using pure Xlib to not have a GTK version check mess due to gtk2.0 not having GdkDisplay
    if (alpha == 0xff)
        XDeleteProperty(dpy, win, XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False));
    else
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False),
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) &opacity, 1L);
    XSync(dpy, False);
    return true;
}

bool wxTopLevelWindowGTK::CanSetTransparent()
{
#if GTK_CHECK_VERSION(2,10,0)
    if (!gtk_check_version(2,10,0))
    {
        if (gtk_widget_is_composited (m_widget))
            return true;
    }
    else
#endif // In case of lower versions than gtk+-2.10.0 we could look for _NET_WM_CM_Sn ourselves
    {
        return false;
    }

#if 0 // Don't be optimistic here for the sake of wxAUI
    int opcode, event, error;
    // Check for the existence of a RGBA visual instead?
    return XQueryExtension(gdk_x11_get_default_xdisplay (),
                           "Composite", &opcode, &event, &error);
#endif
}
