/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "scrolbar.h"
#endif

#include "wx/scrolbar.h"

#if wxUSE_SCROLLBAR

#include "wx/utils.h"

#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;
extern bool   g_blockEventsOnScroll;

static const float sensitivity = 0.02;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

static void gtk_scrollbar_callback( GtkAdjustment *adjust, wxScrollBar *win )
{ 
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;
    
    float diff = adjust->value - win->m_oldPos;
    if (fabs(diff) < sensitivity) return;
    
    win->m_oldPos = adjust->value;

    GtkRange *range = GTK_RANGE( win->m_widget );

    wxEventType command = wxEVT_SCROLL_THUMBTRACK;
    if      (range->scroll_type == GTK_SCROLL_STEP_BACKWARD) command = wxEVT_SCROLL_LINEUP;
    else if (range->scroll_type == GTK_SCROLL_STEP_FORWARD)  command = wxEVT_SCROLL_LINEDOWN;
    else if (range->scroll_type == GTK_SCROLL_PAGE_BACKWARD) command = wxEVT_SCROLL_PAGEUP;
    else if (range->scroll_type == GTK_SCROLL_PAGE_FORWARD)  command = wxEVT_SCROLL_PAGEDOWN;
    
    int value = (int)ceil(adjust->value);
      
    int orient = win->HasFlag(wxSB_VERTICAL) ? wxVERTICAL : wxHORIZONTAL;
  
    wxScrollEvent event( command, win->GetId(), value, orient );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
  
/*
    wxCommandEvent cevent( wxEVT_COMMAND_SCROLLBAR_UPDATED, win->GetId() );
    cevent.SetEventObject( win );
    win->ProcessEvent( cevent );
*/
}

//-----------------------------------------------------------------------------
// "button_press_event" from slider
//-----------------------------------------------------------------------------

static gint gtk_scrollbar_button_press_callback( GtkRange *widget, 
                                                 GdkEventButton *gdk_event, 
                                                 wxScrollBar *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

//  g_blockEventsOnScroll = TRUE;  doesn't work in DialogEd

    win->m_isScrolling = (gdk_event->window == widget->slider);
  
    return FALSE;
}

//-----------------------------------------------------------------------------
// "button_release_event" from slider
//-----------------------------------------------------------------------------

static gint gtk_scrollbar_button_release_callback( GtkRange *WXUNUSED(widget), 
                                                   GdkEventButton *WXUNUSED(gdk_event), 
                                                   wxScrollBar *win )
{
    if (g_isIdle) wxapp_install_idle_handler();
    
//  g_blockEventsOnScroll = FALSE;
  
    if (win->m_isScrolling)
    {
        wxEventType command = wxEVT_SCROLL_THUMBTRACK;
        int value = (int)ceil(win->m_adjust->value);
        int dir = win->HasFlag(wxSB_VERTICAL) ? wxVERTICAL : wxHORIZONTAL;

        wxScrollEvent event( command, value, dir );
        event.SetScrolling( FALSE );
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );
    }
      
    win->m_isScrolling = FALSE;
    
    return FALSE;
}

//-----------------------------------------------------------------------------
// wxScrollBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar,wxControl)

wxScrollBar::~wxScrollBar(void)
{
}

bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxValidator& validator, const wxString& name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
  
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxScrollBar creation failed") );
	return FALSE;
    }

    m_oldPos = 0.0;

    if ((style & wxSB_VERTICAL) == wxSB_VERTICAL)
        m_widget = gtk_vscrollbar_new( (GtkAdjustment *) NULL );
    else
        m_widget = gtk_hscrollbar_new( (GtkAdjustment *) NULL );

    m_adjust = gtk_range_get_adjustment( GTK_RANGE(m_widget) );
  
    gtk_signal_connect( GTK_OBJECT(m_adjust), 
                        "value_changed",
                        (GtkSignalFunc) gtk_scrollbar_callback, 
                        (gpointer) this );
  
    gtk_signal_connect( GTK_OBJECT(m_widget), 
                        "button_press_event",
                        (GtkSignalFunc)gtk_scrollbar_button_press_callback, 
                        (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_widget), 
                        "button_release_event",
                        (GtkSignalFunc)gtk_scrollbar_button_release_callback, 
                        (gpointer) this );

    m_parent->DoAddChild( this );
  
    PostCreation();
  
    SetBackgroundColour( parent->GetBackgroundColour() );

    Show( TRUE );
    
    return TRUE;
}

int wxScrollBar::GetThumbPosition(void) const
{
    return (int)(m_adjust->value+0.5);
}

int wxScrollBar::GetThumbSize() const
{
    return (int)(m_adjust->page_size+0.5);
}

int wxScrollBar::GetPageSize() const
{
    return (int)(m_adjust->page_increment+0.5);
}

int wxScrollBar::GetRange() const
{
    return (int)(m_adjust->upper+0.5);
}

void wxScrollBar::SetThumbPosition( int viewStart )
{
    if (m_isScrolling) return;
  
    float fpos = (float)viewStart;
    m_oldPos = fpos;
    if (fabs(fpos-m_adjust->value) < 0.2) return;
    m_adjust->value = fpos;
  
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_adjust), 
                        (GtkSignalFunc) gtk_scrollbar_callback, 
                        (gpointer) this );
                        
    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );
    
    gtk_signal_connect( GTK_OBJECT(m_adjust), 
                        "value_changed",
                        (GtkSignalFunc) gtk_scrollbar_callback, 
                        (gpointer) this );
}

void wxScrollBar::SetScrollbar( int position, int thumbSize, int range, int pageSize,
      bool WXUNUSED(refresh) )
{
    float fpos = (float)position;
    float frange = (float)range;
    float fthumb = (float)thumbSize;
    float fpage = (float)pageSize;
      
    if ((fabs(frange-m_adjust->upper) < 0.2) &&
        (fabs(fthumb-m_adjust->page_size) < 0.2) &&
        (fabs(fpage-m_adjust->page_increment) < 0.2))
    {
        SetThumbPosition( position );
        return;
    }
  
    m_oldPos = fpos;
  
    m_adjust->lower = 0.0;
    m_adjust->upper = frange;
    m_adjust->value = fpos;
    m_adjust->step_increment = 1.0;
    m_adjust->page_increment = (float)(wxMax(fpage,0));
    m_adjust->page_size = fthumb;

    gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );
}

/* Backward compatibility */
int wxScrollBar::GetValue(void) const
{
    return GetThumbPosition();
}

void wxScrollBar::SetValue( int viewStart )
{
    SetThumbPosition( viewStart );
}

void wxScrollBar::GetValues( int *viewStart, int *viewLength, int *objectLength, int *pageLength ) const
{
    int pos = (int)(m_adjust->value+0.5);
    int thumb = (int)(m_adjust->page_size+0.5);
    int page = (int)(m_adjust->page_increment+0.5);
    int range = (int)(m_adjust->upper+0.5);
  
    *viewStart = pos;
    *viewLength = range;
    *objectLength = thumb;
    *pageLength = page;
}

int wxScrollBar::GetViewLength() const
{
    return (int)(m_adjust->upper+0.5);
}

int wxScrollBar::GetObjectLength() const
{
    return (int)(m_adjust->page_size+0.5);
}

void wxScrollBar::SetPageSize( int pageLength )
{
    int pos = (int)(m_adjust->value+0.5);
    int thumb = (int)(m_adjust->page_size+0.5);
    int range = (int)(m_adjust->upper+0.5);
    SetScrollbar( pos, thumb, range, pageLength );
}

void wxScrollBar::SetObjectLength( int objectLength )
{
    int pos = (int)(m_adjust->value+0.5);
    int page = (int)(m_adjust->page_increment+0.5);
    int range = (int)(m_adjust->upper+0.5);
    SetScrollbar( pos, objectLength, range, page );
}

void wxScrollBar::SetViewLength( int viewLength )
{
    int pos = (int)(m_adjust->value+0.5);
    int thumb = (int)(m_adjust->page_size+0.5);
    int page = (int)(m_adjust->page_increment+0.5);
    SetScrollbar( pos, thumb, viewLength, page );
}

bool wxScrollBar::IsOwnGtkWindow( GdkWindow *window )
{
    GtkRange *range = GTK_RANGE(m_widget);
    return ( (window == GTK_WIDGET(range)->window) ||
             (window == range->trough) ||
             (window == range->slider) ||
             (window == range->step_forw) ||
             (window == range->step_back) );
}

void wxScrollBar::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

#endif
