/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobox.h"
#endif

#include "wx/radiobox.h"

#if wxUSE_RADIOBOX

#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/log.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_radiobutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxRadioBox *rb )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!rb->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    if (rb->m_alreadySent)
    {
        rb->m_alreadySent = FALSE;
        return;
    }

    rb->m_alreadySent = TRUE;

    wxCommandEvent event( wxEVT_COMMAND_RADIOBOX_SELECTED, rb->GetId() );
    event.SetInt( rb->GetSelection() );
    event.SetString( rb->GetStringSelection() );
    event.SetEventObject( rb );
    rb->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

static gint gtk_radiobox_keypress_callback( GtkWidget *widget, GdkEventKey *gdk_event, wxRadioBox *rb )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!rb->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

    if ((gdk_event->keyval != GDK_Up) &&
        (gdk_event->keyval != GDK_Down) &&
        (gdk_event->keyval != GDK_Left) &&
        (gdk_event->keyval != GDK_Right))
    {
        return FALSE;
    }
    
    wxNode *node = rb->m_boxes.Find( (wxObject*) widget );
    if (!node)
    {
        return FALSE;
    }
    
    gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "key_press_event" );
    
    if ((gdk_event->keyval == GDK_Up) ||
        (gdk_event->keyval == GDK_Left))
    {
        if (node == rb->m_boxes.First())
	    node = rb->m_boxes.Last();
	else
	    node = node->Previous();
    }
    else
    {
        if (node == rb->m_boxes.Last())
	    node = rb->m_boxes.First();
	else
	    node = node->Next();
    }
    
    GtkWidget *button = (GtkWidget*) node->Data();
	
    gtk_widget_grab_focus( button );
    
    return TRUE;
}

static gint gtk_radiobutton_focus_in( GtkWidget *widget,
                                      GdkEvent *WXUNUSED(event),
                                      wxRadioBox *win )
{
    if ( win->m_lostFocus )
    {
        // no, we didn't really lose it
        win->m_lostFocus = FALSE;
    }
    else if ( !win->m_hasFocus )
    {
        win->m_hasFocus = TRUE;

        wxFocusEvent event( wxEVT_SET_FOCUS, win->GetId() );
        event.SetEventObject( win );

        // never stop the signal emission, it seems to break the kbd handling
        // inside the radiobox
        (void)win->GetEventHandler()->ProcessEvent( event );
    }

    return FALSE;
}

static gint gtk_radiobutton_focus_out( GtkWidget *widget,
                                       GdkEvent *WXUNUSED(event),
                                       wxRadioBox *win )
{
  //    wxASSERT_MSG( win->m_hasFocus, _T("got focus out without any focus in?") );
  // Replace with a warning, else we dump core a lot!
  //  if (!win->m_hasFocus)
  //      wxLogWarning(_T("Radiobox got focus out without any focus in.") );

    // we might have lost the focus, but may be not - it may have just gone to
    // another button in the same radiobox, so we'll check for it in the next
    // idle iteration (leave m_hasFocus == TRUE for now)
    win->m_lostFocus = TRUE;

    return FALSE;
}

//-----------------------------------------------------------------------------
// wxRadioBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox,wxControl)

void wxRadioBox::Init()
{
    m_alreadySent = FALSE;
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    m_hasFocus =
    m_lostFocus = FALSE;
}

bool wxRadioBox::Create( wxWindow *parent, wxWindowID id, const wxString& title,
                         const wxPoint &pos, const wxSize &size,
                         int n, const wxString choices[], int majorDim,
                         long style, const wxValidator& validator,
                         const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxRadioBox creation failed") );
	return FALSE;
    }

    m_widget = gtk_frame_new( title.mbc_str() );

    // majorDim may be 0 if all trailing parameters were omitted, so don't
    // assert here but just use the correct value for it
    m_majorDim = majorDim == 0 ? n : majorDim;

    GtkRadioButton *m_radio = (GtkRadioButton*) NULL;

    wxString label;
    GSList *radio_button_group = (GSList *) NULL;
    for (int i = 0; i < n; i++)
    {
        if ( i != 0 )
            radio_button_group = gtk_radio_button_group( GTK_RADIO_BUTTON(m_radio) );

        label.Empty();
        for ( const wxChar *pc = choices[i]; *pc; pc++ )
        {
            if ( *pc != wxT('&') )
                label += *pc;
        }

        m_radio = GTK_RADIO_BUTTON( gtk_radio_button_new_with_label( radio_button_group, label.mbc_str() ) );

        gtk_signal_connect( GTK_OBJECT(m_radio), "key_press_event",
           GTK_SIGNAL_FUNC(gtk_radiobox_keypress_callback), (gpointer)this );
	   
        m_boxes.Append( (wxObject*) m_radio );

        ConnectWidget( GTK_WIDGET(m_radio) );

        if (!i) gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_radio), TRUE );

        gtk_signal_connect( GTK_OBJECT(m_radio), "clicked",
            GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );

        gtk_signal_connect( GTK_OBJECT(m_radio), "focus_in_event",
            GTK_SIGNAL_FUNC(gtk_radiobutton_focus_in), (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(m_radio), "focus_out_event",
            GTK_SIGNAL_FUNC(gtk_radiobutton_focus_out), (gpointer)this );

        gtk_pizza_put( GTK_PIZZA(m_parent->m_wxwindow),
                         GTK_WIDGET(m_radio),
                         m_x+10, m_y+10+(i*24), 10, 10 );
    }

    m_parent->DoAddChild( this );

    PostCreation();
    
    ApplyWidgetStyle();

    SetLabel( title );

    SetFont( parent->GetFont() );

    wxSize ls = LayoutItems();

    GtkRequisition req;
    req.width = 2;
    req.height = 2;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request ) (m_widget, &req );
    if (req.width > ls.x) ls.x = req.width;
    
    wxSize newSize = size;
    if (newSize.x == -1) newSize.x = ls.x;
    if (newSize.y == -1) newSize.y = ls.y;
    SetSize( newSize.x, newSize.y );

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );

    return TRUE;
}

wxRadioBox::~wxRadioBox()
{
    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkWidget *button = GTK_WIDGET( node->Data() );
        gtk_widget_destroy( button );
        node = node->Next();
    }
}

void wxRadioBox::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxWindow::DoSetSize( x, y, width, height, sizeFlags );
    
    LayoutItems();
}

wxSize wxRadioBox::LayoutItems()
{
    int x = 7;
    int y = 15;

    if ( m_majorDim == 0 )
    {
        // avoid dividing by 0 below
        wxFAIL_MSG( wxT("dimension of radiobox should not be 0!") );

        m_majorDim = 1;
    }

    int num_per_major = (m_boxes.GetCount() - 1) / m_majorDim +1;

    wxSize res( 0, 0 );

    int num_of_cols = 0;
    int num_of_rows = 0;
    if (HasFlag(wxRA_SPECIFY_COLS))
    {
        num_of_cols = m_majorDim;
        num_of_rows = num_per_major;
    }
    else
    {
        num_of_cols = num_per_major;
        num_of_rows = m_majorDim;
    }
	
    if ( HasFlag(wxRA_SPECIFY_COLS) ||
         (HasFlag(wxRA_SPECIFY_ROWS) && (num_of_cols > 1)) )
    {
        for (int j = 0; j < num_of_cols; j++)
        {
            y = 15;

            int max_len = 0;
            wxNode *node = m_boxes.Nth( j*num_of_rows );
            for (int i1 = 0; i1< num_of_rows; i1++)
            {
                GtkWidget *button = GTK_WIDGET( node->Data() );
		
                GtkRequisition req;
                req.width = 2;
                req.height = 2;
                (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(button) )->size_request )
                      (button, &req );
		
                if (req.width > max_len) max_len = req.width;

                gtk_pizza_move( GTK_PIZZA(m_parent->m_wxwindow), button, m_x+x, m_y+y );
                y += req.height;

                node = node->Next();
                if (!node) break;
            }

            // we don't know the max_len before

            node = m_boxes.Nth( j*num_of_rows );
            for (int i2 = 0; i2< num_of_rows; i2++)
            {
                GtkWidget *button = GTK_WIDGET( node->Data() );

                gtk_pizza_resize( GTK_PIZZA(m_parent->m_wxwindow), button, max_len, 20 );

                node = node->Next();
                if (!node) break;
            }

            if (y > res.y) res.y = y;

            x += max_len + 2;
        }

        res.x = x+4;
        res.y += 4;
    }
    else
    {
        int max = 0;

        wxNode *node = m_boxes.First();
        while (node)
        {
            GtkWidget *button = GTK_WIDGET( node->Data() );

            GtkRequisition req;
            req.width = 2;
            req.height = 2;
            (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(button) )->size_request )
                  (button, &req );

            if (req.width > max) max = req.width;

            node = node->Next();
        }

        node = m_boxes.First();
        while (node)
        {
            GtkWidget *button = GTK_WIDGET( node->Data() );

            gtk_pizza_set_size( GTK_PIZZA(m_parent->m_wxwindow), button, m_x+x, m_y+y, max, 20 );
            x += max;

            node = node->Next();
        }
        res.x = x+4;
        res.y = 40;
    }

    return res;
}

bool wxRadioBox::Show( bool show )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid radiobox") );

    if (!wxControl::Show(show))
    {
        // nothing to do
        return FALSE;
    }

    if ((m_windowStyle & wxNO_BORDER) != 0)
        gtk_widget_hide( m_widget );

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkWidget *button = GTK_WIDGET( node->Data() );

        if (show) gtk_widget_show( button ); else gtk_widget_hide( button );

        node = node->Next();
    }

    return TRUE;
}

int wxRadioBox::FindString( const wxString &s ) const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid radiobox") );

    int count = 0;

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkButton *button = GTK_BUTTON( node->Data() );

        GtkLabel *label = GTK_LABEL( button->child );
        if (s == label->label) return count;
        count++;

        node = node->Next();
    }

    return -1;
}

void wxRadioBox::SetFocus()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    if (m_boxes.GetCount() == 0) return;

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkToggleButton *button = GTK_TOGGLE_BUTTON( node->Data() );
        if (button->active)
        {
            gtk_widget_grab_focus( GTK_WIDGET(button) );
            return;
        }
        node = node->Next();
    }

}

void wxRadioBox::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    wxNode *node = m_boxes.Nth( n );

    wxCHECK_RET( node, wxT("radiobox wrong index") );

    GtkToggleButton *button = GTK_TOGGLE_BUTTON( node->Data() );

    GtkDisableEvents();
    
    gtk_toggle_button_set_state( button, 1 );
    
    GtkEnableEvents();
}

int wxRadioBox::GetSelection(void) const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid radiobox") );

    int count = 0;

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkToggleButton *button = GTK_TOGGLE_BUTTON( node->Data() );
        if (button->active) return count;
        count++;
        node = node->Next();
    }

    wxFAIL_MSG( wxT("wxRadioBox none selected") );

    return -1;
}

wxString wxRadioBox::GetString( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid radiobox") );

    wxNode *node = m_boxes.Nth( n );

    wxCHECK_MSG( node, wxT(""), wxT("radiobox wrong index") );

    GtkButton *button = GTK_BUTTON( node->Data() );
    GtkLabel *label = GTK_LABEL( button->child );

    return wxString( label->label );
}

wxString wxRadioBox::GetLabel( int item ) const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid radiobox") );

    return GetString( item );
}

void wxRadioBox::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    wxControl::SetLabel( label );

    gtk_frame_set_label( GTK_FRAME(m_widget), wxControl::GetLabel().mbc_str() );
}

void wxRadioBox::SetLabel( int item, const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    wxNode *node = m_boxes.Nth( item );

    wxCHECK_RET( node, wxT("radiobox wrong index") );

    GtkButton *button = GTK_BUTTON( node->Data() );
    GtkLabel *g_label = GTK_LABEL( button->child );

    gtk_label_set( g_label, label.mbc_str() );
}

void wxRadioBox::SetLabel( int WXUNUSED(item), wxBitmap *WXUNUSED(bitmap) )
{
    wxFAIL_MSG(wxT("wxRadioBox::SetLabel not implemented."));
}

bool wxRadioBox::Enable( bool enable )
{
    if ( !wxControl::Enable( enable ) )
        return FALSE;

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkButton *button = GTK_BUTTON( node->Data() );
        GtkWidget *label = button->child;
        gtk_widget_set_sensitive( GTK_WIDGET(button), enable );
        gtk_widget_set_sensitive( label, enable );
        node = node->Next();
    }

    return TRUE;
}

void wxRadioBox::Enable( int item, bool enable )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    wxNode *node = m_boxes.Nth( item );

    wxCHECK_RET( node, wxT("radiobox wrong index") );

    GtkButton *button = GTK_BUTTON( node->Data() );
    GtkWidget *label = button->child;
    gtk_widget_set_sensitive( GTK_WIDGET(button), enable );
    gtk_widget_set_sensitive( label, enable );
}

void wxRadioBox::Show( int item, bool show )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid radiobox") );

    wxNode *node = m_boxes.Nth( item );

    wxCHECK_RET( node, wxT("radiobox wrong index") );

    GtkWidget *button = GTK_WIDGET( node->Data() );

    if (show)
        gtk_widget_show( button );
    else
        gtk_widget_hide( button );
}

wxString wxRadioBox::GetStringSelection() const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid radiobox") );

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkToggleButton *button = GTK_TOGGLE_BUTTON( node->Data() );
        if (button->active)
        {
            GtkLabel *label = GTK_LABEL( GTK_BUTTON(button)->child );
            return label->label;
        }
        node = node->Next();
    }

    wxFAIL_MSG( wxT("wxRadioBox none selected") );
    return wxT("");
}

bool wxRadioBox::SetStringSelection( const wxString &s )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid radiobox") );

    int res = FindString( s );
    if (res == -1) return FALSE;
    SetSelection( res );

    return TRUE;
}

int wxRadioBox::Number() const
{
    return m_boxes.Number();
}

int wxRadioBox::GetNumberOfRowsOrCols() const
{
    return 1;
}

void wxRadioBox::SetNumberOfRowsOrCols( int WXUNUSED(n) )
{
    wxFAIL_MSG(wxT("wxRadioBox::SetNumberOfRowsOrCols not implemented."));
}

void wxRadioBox::GtkDisableEvents()
{
    wxNode *node = m_boxes.First();
    while (node)
    {
        gtk_signal_disconnect_by_func( GTK_OBJECT(node->Data()),
           GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );

	node = node->Next();
    }
}

void wxRadioBox::GtkEnableEvents()
{
    wxNode *node = m_boxes.First();
    while (node)
    {
        gtk_signal_connect( GTK_OBJECT(node->Data()), "clicked",
           GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );

	node = node->Next();
    }
}

void wxRadioBox::ApplyWidgetStyle()
{
    SetWidgetStyle();

    gtk_widget_set_style( m_widget, m_widgetStyle );

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkWidget *widget = GTK_WIDGET( node->Data() );
        gtk_widget_set_style( widget, m_widgetStyle );

        GtkButton *button = GTK_BUTTON( node->Data() );
        gtk_widget_set_style( button->child, m_widgetStyle );

        node = node->Next();
    }
}

#if wxUSE_TOOLTIPS
void wxRadioBox::ApplyToolTip( GtkTooltips *tips, const wxChar *tip )
{
    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkWidget *widget = GTK_WIDGET( node->Data() );
        gtk_tooltips_set_tip( tips, widget, wxConvCurrent->cWX2MB(tip), (gchar*) NULL );
        node = node->Next();
    }
}
#endif // wxUSE_TOOLTIPS

bool wxRadioBox::IsOwnGtkWindow( GdkWindow *window )
{
    if (window == m_widget->window) return TRUE;

    wxNode *node = m_boxes.First();
    while (node)
    {
        GtkWidget *button = GTK_WIDGET( node->Data() );

        if (window == button->window) return TRUE;

        node = node->Next();
    }

    return FALSE;
}

void wxRadioBox::OnInternalIdle()
{
    if ( m_lostFocus )
    {
        m_hasFocus = FALSE;
        m_lostFocus = FALSE;

        wxFocusEvent event( wxEVT_KILL_FOCUS, GetId() );
        event.SetEventObject( this );

        (void)GetEventHandler()->ProcessEvent( event );
    }
}

#endif // wxUSE_RADIOBOX

