/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/popupwin.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include <gtk/gtk.h>

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// "button_press"
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_popup_button_press (GtkWidget *widget, GdkEvent *gdk_event, wxPopupWindow* win )
{
    GtkWidget *child = gtk_get_event_widget (gdk_event);

    /* Ignore events sent out before we connected to the signal */
    if (win->m_time >= ((GdkEventButton*)gdk_event)->time)
        return FALSE;

    /*  We don't ask for button press events on the grab widget, so
     *  if an event is reported directly to the grab widget, it must
     *  be on a window outside the application (and thus we remove
     *  the popup window). Otherwise, we check if the widget is a child
     *  of the grab widget, and only remove the popup window if it
     *  is not. */
    if (child != widget)
    {
        while (child)
        {
            if (child == widget)
                return FALSE;
            child = child->parent;
        }
    }

    wxFocusEvent event( wxEVT_KILL_FOCUS, win->GetId() );
    event.SetEventObject( win );

    (void)win->GetEventHandler()->ProcessEvent( event );

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

extern "C" {
bool gtk_dialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxPopupWindow *win )
{
    if (win->IsEnabled())
        win->Close();

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

/* we cannot MWM hints and icons before the widget has been realized,
   so we do this directly after realization */

extern "C" {
static gint
gtk_dialog_realized_callback( GtkWidget * WXUNUSED(widget), wxPopupWindow *win )
{
    /* all this is for Motif Window Manager "hints" and is supposed to be
       recognized by other WM as well. not tested. */
    long decor = (long) GDK_DECOR_BORDER;
    long func = (long) GDK_FUNC_MOVE ;

    gdk_window_set_decorations( win->m_widget->window, (GdkWMDecoration)decor);
    gdk_window_set_functions( win->m_widget->window, (GdkWMFunction)func);

    gtk_window_set_resizable(GTK_WINDOW(win->m_widget), FALSE);

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// InsertChild for wxPopupWindow
//-----------------------------------------------------------------------------

/* Callback for wxFrame. This very strange beast has to be used because
 * C++ has no virtual methods in a constructor. We have to emulate a
 * virtual function here as wxWidgets requires different ways to insert
 * a child in container classes. */

static void wxInsertChildInPopupWin(wxWindowGTK* parent, wxWindowGTK* child)
{
    gtk_widget_set_size_request(
        child->m_widget, child->m_width, child->m_height);
    gtk_fixed_put(
        GTK_FIXED(parent->m_wxwindow), child->m_widget, child->m_x, child->m_y);

    if (parent->HasFlag(wxTAB_TRAVERSAL))
    {
        /* we now allow a window to get the focus as long as it
           doesn't have any children. */
        GTK_WIDGET_UNSET_FLAGS( parent->m_wxwindow, GTK_CAN_FOCUS );
    }
}

//-----------------------------------------------------------------------------
// wxPopupWindow
//-----------------------------------------------------------------------------

#ifdef __WXUNIVERSAL__
BEGIN_EVENT_TABLE(wxPopupWindow,wxPopupWindowBase)
    EVT_SIZE(wxPopupWindow::OnSize)
END_EVENT_TABLE()
#endif

wxPopupWindow::~wxPopupWindow()
{
}

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    if (!PreCreation( parent, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("popup") ))
    {
        wxFAIL_MSG( wxT("wxPopupWindow creation failed") );
        return false;
    }

    // Unlike windows, top level windows are created hidden by default.
    m_isShown = false;

    // All dialogs should really have this style
    m_windowStyle |= wxTAB_TRAVERSAL;

    m_insertCallback = wxInsertChildInPopupWin;

    m_widget = gtk_window_new( GTK_WINDOW_POPUP );

    if ((m_parent) && (GTK_IS_WINDOW(m_parent->m_widget)))
        gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(m_parent->m_widget) );

    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    g_signal_connect (m_widget, "delete_event",
                      G_CALLBACK (gtk_dialog_delete_callback), this);

    m_wxwindow = wxPizza::New(m_windowStyle);
    gtk_widget_show( m_wxwindow );
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );

    if (m_parent) m_parent->AddChild( this );

    PostCreation();

    /*  we cannot set MWM hints  before the widget has
        been realized, so we do this directly after realization */
    g_signal_connect (m_widget, "realize",
                      G_CALLBACK (gtk_dialog_realized_callback), this);

    m_time = gtk_get_current_event_time();

    g_signal_connect (m_widget, "button_press_event",
                      G_CALLBACK (gtk_popup_button_press), this);

    return true;
}

void wxPopupWindow::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxPopupWindow") );
}

void wxPopupWindow::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), wxT("invalid dialog") );
    wxASSERT_MSG( (m_wxwindow != NULL), wxT("invalid dialog") );

    int old_x = m_x;
    int old_y = m_y;

    int old_width = m_width;
    int old_height = m_height;

    if (x != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        m_x = x;
    if (y != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        m_y = y;
    if (width != -1)
        m_width = width;
    if (height != -1)
        m_height = height;

    ConstrainSize();

    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y))
        {
            gtk_window_move( GTK_WINDOW(m_widget), m_x, m_y );
        }
    }

    if ((m_width != old_width) || (m_height != old_height))
    {
        // gtk_window_resize does not work for GTK_WINDOW_POPUP
        gtk_widget_set_size_request( m_widget, m_width, m_height );
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
}

bool wxPopupWindow::Show( bool show )
{
    if (show && !IsShown())
    {
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    bool ret = wxWindow::Show( show );

    return ret;
}

#endif // wxUSE_POPUPWIN
