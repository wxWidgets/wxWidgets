/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/frame.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/frame.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int wxSTATUS_HEIGHT  = 25;
static const int wxPLACE_HOLDER   = 0;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE

//-----------------------------------------------------------------------------
// "child_attached" of menu bar
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_menu_attached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_menuBarDetached = false;
    win->GtkUpdateSize();
}
}

//-----------------------------------------------------------------------------
// "child_detached" of menu bar
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_menu_detached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    // Raise the client area area
    gdk_window_raise( win->m_wxwindow->window );

    win->m_menuBarDetached = true;
    win->GtkUpdateSize();
}
}

#endif // wxUSE_MENUS_NATIVE

#if wxUSE_TOOLBAR
//-----------------------------------------------------------------------------
// "child_attached" of tool bar
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_toolbar_attached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    win->m_toolBarDetached = false;
    win->GtkUpdateSize();
}
}

//-----------------------------------------------------------------------------
// "child_detached" of tool bar
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_toolbar_detached_callback( GtkWidget *WXUNUSED(widget), GtkWidget *WXUNUSED(child), wxFrame *win )
{
    if (!win->m_hasVMT) return;

    // Raise the client area area
    gdk_window_raise( win->m_wxwindow->window );

    win->m_toolBarDetached = true;
    win->GtkUpdateSize();
}
}
#endif // wxUSE_TOOLBAR


// ----------------------------------------------------------------------------
// wxFrame itself
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InsertChild for wxFrame
//-----------------------------------------------------------------------------

/* Callback for wxFrame. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxWidgets requires different ways to insert
 * a child in container classes. */

static void wxInsertChildInFrame( wxFrame* parent, wxWindow* child )
{
    wxASSERT( GTK_IS_WIDGET(child->m_widget) );

    if (!parent->m_insertInClientArea)
    {
        // These are outside the client area
        wxFrame* frame = (wxFrame*) parent;
        gtk_pizza_put( GTK_PIZZA(frame->m_mainWidget),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );

#if wxUSE_TOOLBAR_NATIVE
        // We connect to these events for recalculating the client area
        // space when the toolbar is floating
        if (wxIS_KIND_OF(child,wxToolBar))
        {
            wxToolBar *toolBar = (wxToolBar*) child;
            if (toolBar->GetWindowStyle() & wxTB_DOCKABLE)
            {
                g_signal_connect (toolBar->m_widget, "child_attached",
                                  G_CALLBACK (gtk_toolbar_attached_callback),
                                  parent);
                g_signal_connect (toolBar->m_widget, "child_detached",
                                  G_CALLBACK (gtk_toolbar_detached_callback),
                                  parent);
            }
        }
#endif // wxUSE_TOOLBAR
    }
    else
    {
        // These are inside the client area
        gtk_pizza_put( GTK_PIZZA(parent->m_wxwindow),
                         GTK_WIDGET(child->m_widget),
                         child->m_x,
                         child->m_y,
                         child->m_width,
                         child->m_height );
    }
}

// ----------------------------------------------------------------------------
// wxFrame creation
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_menuBarDetached = false;
    m_toolBarDetached = false;
    m_menuBarHeight = 2;
}

bool wxFrame::Create( wxWindow *parent,
                      wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& sizeOrig,
                      long style,
                      const wxString &name )
{
    bool rt = wxTopLevelWindow::Create(parent, id, title, pos, sizeOrig,
                                       style, name);
    m_insertCallback = (wxInsertChildFunction) wxInsertChildInFrame;

    return rt;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = true;
    DeleteAllBars();
}

// ----------------------------------------------------------------------------
// overridden wxWindow methods
// ----------------------------------------------------------------------------

void wxFrame::DoGetClientSize( int *width, int *height ) const
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    wxTopLevelWindow::DoGetClientSize( width, height );

    if (height)
    {
#if wxUSE_MENUS_NATIVE
        // menu bar
        if (m_frameMenuBar && !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOMENUBAR) != 0))
        {
            if (!m_menuBarDetached)
                (*height) -= m_menuBarHeight;
            else
                (*height) -= wxPLACE_HOLDER;
        }
#endif // wxUSE_MENUS_NATIVE

#if wxUSE_STATUSBAR
        // status bar
        if (m_frameStatusBar && m_frameStatusBar->IsShown() &&
            !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOSTATUSBAR) != 0))
            (*height) -= wxSTATUS_HEIGHT;
#endif // wxUSE_STATUSBAR
    }

#if wxUSE_TOOLBAR
    // tool bar
    if (m_frameToolBar && m_frameToolBar->IsShown())
    {
        if (m_toolBarDetached)
        {
            if (height != NULL)
                *height -= wxPLACE_HOLDER;
        }
        else
        {
            int x, y;
            m_frameToolBar->GetSize( &x, &y );
            if ( m_frameToolBar->IsVertical() )
            {
                if (width != NULL)
                    *width -= x;
            }
            else
            {
                if (height != NULL)
                    *height -= y;
            }
        }
    }
#endif // wxUSE_TOOLBAR

    if (width != NULL && *width < 0)
        *width = 0;
    if (height != NULL && *height < 0)
        *height = 0;
}

void wxFrame::DoSetClientSize( int width, int height )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

#if wxUSE_MENUS_NATIVE
        // menu bar
        if (m_frameMenuBar && !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOMENUBAR) != 0))
        {
            if (!m_menuBarDetached)
                height += m_menuBarHeight;
            else
                height += wxPLACE_HOLDER;
        }
#endif // wxUSE_MENUS_NATIVE

#if wxUSE_STATUSBAR
        // status bar
        if (m_frameStatusBar && m_frameStatusBar->IsShown() &&
            !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOSTATUSBAR) != 0))
            height += wxSTATUS_HEIGHT;
#endif

#if wxUSE_TOOLBAR
        // tool bar
        if (m_frameToolBar && m_frameToolBar->IsShown())
        {
            if (m_toolBarDetached)
            {
                height += wxPLACE_HOLDER;
            }
            else
            {
                int x, y;
                m_frameToolBar->GetSize( &x, &y );
                if ( m_frameToolBar->IsVertical() )
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

    wxTopLevelWindow::DoSetClientSize( width, height );
}

void wxFrame::GtkOnSize()
{
    // avoid recursions
    if (m_resizing) return;
    m_resizing = true;

    // this shouldn't happen: wxFrame, wxMDIParentFrame and wxMDIChildFrame have m_wxwindow
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    // space occupied by m_frameToolBar and m_frameMenuBar
    int client_area_x_offset = 0,
        client_area_y_offset = 0;

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
        // TODO
        // Rewrite this terrible code to using GtkVBox

        // m_mainWidget holds the menubar, the toolbar and the client
        // area, which is represented by m_wxwindow.

#if wxUSE_MENUS_NATIVE
        if (m_frameMenuBar && !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOMENUBAR) != 0))
        {
            if (!GTK_WIDGET_VISIBLE(m_frameMenuBar->m_widget))
                gtk_widget_show( m_frameMenuBar->m_widget );
            int xx = m_miniEdge;
            int yy = m_miniEdge + m_miniTitle;
            int ww = m_width  - 2*m_miniEdge;
            if (ww < 0)
                ww = 0;
            int hh = m_menuBarHeight;
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
        else
        {
            if (m_frameMenuBar)
            {
                if (GTK_WIDGET_VISIBLE(m_frameMenuBar->m_widget))
                    gtk_widget_hide( m_frameMenuBar->m_widget );
            }
        }
#endif // wxUSE_MENUS_NATIVE

#if wxUSE_TOOLBAR
        if ((m_frameToolBar) && m_frameToolBar->IsShown() &&
            (m_frameToolBar->m_widget->parent == m_mainWidget))
        {
            int xx = m_miniEdge;
            int yy = m_miniEdge + m_miniTitle;
#if wxUSE_MENUS_NATIVE
            if (m_frameMenuBar)
            {
                if (!m_menuBarDetached)
                    yy += m_menuBarHeight;
                else
                    yy += wxPLACE_HOLDER;
            }
#endif // wxUSE_MENUS_NATIVE

            m_frameToolBar->m_x = xx;
            m_frameToolBar->m_y = yy;

            // don't change the toolbar's reported height/width
            int ww, hh;
            if ( m_frameToolBar->GetWindowStyle() & wxTB_VERTICAL )
            {
                ww = m_toolBarDetached ? wxPLACE_HOLDER
                                       : m_frameToolBar->m_width;
                hh = m_height - 2*m_miniEdge;

                client_area_x_offset += ww;
            }
            else if( m_frameToolBar->HasFlag(wxTB_RIGHT) )
            {
               yy += 2;
               ww = m_toolBarDetached ? wxPLACE_HOLDER
                                      : m_frameToolBar->m_width;
               xx = GetClientSize().x - 1;
               hh = m_height - 2*m_miniEdge;
               if( hh < 0 )
                  hh = 0;

            }
            else if( m_frameToolBar->GetWindowStyle() & wxTB_BOTTOM )
            {
                xx = m_miniEdge;
                yy = GetClientSize().y;
#if wxUSE_MENUS_NATIVE
                yy += m_menuBarHeight;
#endif // wxUSE_MENU_NATIVE
                m_frameToolBar->m_x = xx;
                m_frameToolBar->m_y = yy;
                ww = m_width - 2*m_miniEdge;
                hh = m_toolBarDetached ? wxPLACE_HOLDER
                                       : m_frameToolBar->m_height;
            }
            else
            {
                ww = m_width - 2*m_miniEdge;
                hh = m_toolBarDetached ? wxPLACE_HOLDER
                                       : m_frameToolBar->m_height;

                client_area_y_offset += hh;
            }

            if (ww < 0)
                ww = 0;
            if (hh < 0)
                hh = 0;
            gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                  m_frameToolBar->m_widget,
                                  xx, yy, ww, hh );
        }
#endif // wxUSE_TOOLBAR

        int client_x = client_area_x_offset + m_miniEdge;
        int client_y = client_area_y_offset + m_miniEdge + m_miniTitle;
        int client_w = m_width - client_area_x_offset - 2*m_miniEdge;
        int client_h = m_height - client_area_y_offset- 2*m_miniEdge - m_miniTitle;
        if (client_w < 0)
            client_w = 0;
        if (client_h < 0)
            client_h = 0;
        gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                              m_wxwindow,
                              client_x, client_y, client_w, client_h );
    }
    else
    {
        // If there is no m_mainWidget between m_widget and m_wxwindow there
        // is no need to set the size or position of m_wxwindow.
    }

#if wxUSE_STATUSBAR
    if (m_frameStatusBar && m_frameStatusBar->IsShown() &&
        !(m_fsIsShowing && (m_fsSaveFlag & wxFULLSCREEN_NOSTATUSBAR) != 0))
    {
        if (!GTK_WIDGET_VISIBLE(m_frameStatusBar->m_widget))
            gtk_widget_show( m_frameStatusBar->m_widget );

        int xx = 0 + m_miniEdge;
        int yy = m_height - wxSTATUS_HEIGHT - m_miniEdge - client_area_y_offset;
        int ww = m_width - 2*m_miniEdge;
        if (ww < 0)
            ww = 0;
        int hh = wxSTATUS_HEIGHT;
        m_frameStatusBar->m_x = xx;
        m_frameStatusBar->m_y = yy;
        m_frameStatusBar->m_width = ww;
        m_frameStatusBar->m_height = hh;
        gtk_pizza_set_size( GTK_PIZZA(m_wxwindow),
                            m_frameStatusBar->m_widget,
                            xx, yy, ww, hh );
    }
    else
    {
        if (m_frameStatusBar)
        {
            if (GTK_WIDGET_VISIBLE(m_frameStatusBar->m_widget))
                gtk_widget_hide( m_frameStatusBar->m_widget );
        }
    }
#endif // wxUSE_STATUSBAR

    m_sizeSet = true;

    // send size event to frame
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

#if wxUSE_STATUSBAR
    // send size event to status bar
    if (m_frameStatusBar)
    {
        wxSizeEvent event2( wxSize(m_frameStatusBar->m_width,m_frameStatusBar->m_height), m_frameStatusBar->GetId() );
        event2.SetEventObject( m_frameStatusBar );
        m_frameStatusBar->GetEventHandler()->ProcessEvent( event2 );
    }
#endif // wxUSE_STATUSBAR

    m_resizing = false;
}

void wxFrame::OnInternalIdle()
{
    wxFrameBase::OnInternalIdle();

#if wxUSE_MENUS_NATIVE
    if (m_frameMenuBar) m_frameMenuBar->OnInternalIdle();
#endif // wxUSE_MENUS_NATIVE
#if wxUSE_TOOLBAR
    if (m_frameToolBar) m_frameToolBar->OnInternalIdle();
#endif
#if wxUSE_STATUSBAR
    if (m_frameStatusBar)
    {
        m_frameStatusBar->OnInternalIdle();

        // There may be controls in the status bar that
        // need to be updated
        for ( wxWindowList::compatibility_iterator node = m_frameStatusBar->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
        {
            wxWindow *child = node->GetData();
            child->OnInternalIdle();
        }
    }
#endif
}

// ----------------------------------------------------------------------------
// menu/tool/status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE

void wxFrame::DetachMenuBar()
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid frame") );

    if ( m_frameMenuBar )
    {
        m_frameMenuBar->UnsetInvokingWindow( this );

        if (m_frameMenuBar->GetWindowStyle() & wxMB_DOCKABLE)
        {
            g_signal_handlers_disconnect_by_func (m_frameMenuBar->m_widget,
                    (gpointer) gtk_menu_attached_callback,
                    this);

            g_signal_handlers_disconnect_by_func (m_frameMenuBar->m_widget,
                    (gpointer) gtk_menu_detached_callback,
                    this);
        }

        gtk_widget_ref( m_frameMenuBar->m_widget );

        gtk_container_remove( GTK_CONTAINER(m_mainWidget), m_frameMenuBar->m_widget );
    }

    wxFrameBase::DetachMenuBar();
}

void wxFrame::AttachMenuBar( wxMenuBar *menuBar )
{
    wxFrameBase::AttachMenuBar(menuBar);

    if (m_frameMenuBar)
    {
        m_frameMenuBar->SetInvokingWindow( this );

        m_frameMenuBar->SetParent(this);
        gtk_pizza_put( GTK_PIZZA(m_mainWidget),
                m_frameMenuBar->m_widget,
                m_frameMenuBar->m_x,
                m_frameMenuBar->m_y,
                m_frameMenuBar->m_width,
                m_frameMenuBar->m_height );

        if (menuBar->GetWindowStyle() & wxMB_DOCKABLE)
        {
            g_signal_connect (menuBar->m_widget, "child_attached",
                              G_CALLBACK (gtk_menu_attached_callback),
                              this);
            g_signal_connect (menuBar->m_widget, "child_detached",
                              G_CALLBACK (gtk_menu_detached_callback),
                              this);
        }

        gtk_widget_show( m_frameMenuBar->m_widget );

        UpdateMenuBarSize();
    }
    else
    {
        m_menuBarHeight = 2;
        GtkUpdateSize();        // resize window in OnInternalIdle
    }
}

void wxFrame::UpdateMenuBarSize()
{
    m_menuBarHeight = 2;

    // this is called after Remove with a NULL m_frameMenuBar
    if ( m_frameMenuBar )
    {
        GtkRequisition req;
        gtk_widget_ensure_style(m_frameMenuBar->m_widget);
        // have to call class method directly because
        // "size_request" signal is overridden by wx
        GTK_WIDGET_GET_CLASS(m_frameMenuBar->m_widget)->size_request(
            m_frameMenuBar->m_widget, &req);

        m_menuBarHeight = req.height;
    }

    // resize window in OnInternalIdle
    GtkUpdateSize();
}

#endif // wxUSE_MENUS_NATIVE

#if wxUSE_TOOLBAR

wxToolBar* wxFrame::CreateToolBar( long style, wxWindowID id, const wxString& name )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid frame") );

    m_insertInClientArea = false;

    m_frameToolBar = wxFrameBase::CreateToolBar( style, id, name );

    m_insertInClientArea = true;

    GtkUpdateSize();

    return m_frameToolBar;
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    bool hadTbar = m_frameToolBar != NULL;

    wxFrameBase::SetToolBar(toolbar);

    if ( m_frameToolBar )
    {
        // insert into toolbar area if not already there
        if ((m_frameToolBar->m_widget->parent) &&
            (m_frameToolBar->m_widget->parent != m_mainWidget))
        {
            GetChildren().DeleteObject( m_frameToolBar );

            gtk_widget_reparent( m_frameToolBar->m_widget, m_mainWidget );
            GtkUpdateSize();
        }
    }
    else // toolbar unset
    {
        // still need to update size if it had been there before
        if ( hadTbar )
        {
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
    GtkUpdateSize();

    return wxFrameBase::CreateStatusBar( number, style, id, name );
}

void wxFrame::SetStatusBar(wxStatusBar *statbar)
{
    bool hadStatBar = m_frameStatusBar != NULL;

    wxFrameBase::SetStatusBar(statbar);

    if (hadStatBar && !m_frameStatusBar)
        GtkUpdateSize();
}

void wxFrame::PositionStatusBar()
{
    if ( !m_frameStatusBar )
        return;

    GtkUpdateSize();
}
#endif // wxUSE_STATUSBAR
