/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minifram.h"
#endif

#include "wx/minifram.h"
#include "wx/dcscreen.h"

#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"


//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxMiniFrame *mf )
{
    mf->Close();
}

//-----------------------------------------------------------------------------
// wxMiniFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxMiniFrame,wxFrame)
    EVT_PAINT(wxMiniFrame::OnPaint)
    EVT_MOUSE_EVENTS(wxMiniFrame::OnMouse)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame,wxFrame)

bool wxMiniFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    style = style | wxSIMPLE_BORDER;
    
    m_miniEdge = 3;
    m_miniTitle = 13;
    m_isDragging = FALSE;
    m_oldX = -1;
    m_oldY = -1;
    m_diffX = 0;
    m_diffY = 0;
    
    wxFrame::Create( parent, id, title, pos, size, style, name );

    GtkWidget *close_button = gtk_button_new_with_label( "x" );
    
    gtk_myfixed_put( GTK_MYFIXED(m_wxwindow), close_button, 4, 4 );
    gtk_widget_set_usize( close_button, 12, 11 );
    
    gtk_widget_show( close_button );
    
    gtk_signal_connect( GTK_OBJECT(close_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );
    
    return TRUE;
}

void wxMiniFrame::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    if (m_miniTitle == 0) return;
    if (m_title.IsEmpty()) return;
  
    wxPaintDC dc(this);
  
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle( 0, 0, m_width, m_height );
  
    dc.SetPen( *wxWHITE_PEN );
    dc.DrawLine( 1, 1, m_width-2, 1 );
    dc.DrawLine( 1, 1, 1, m_height-2 );
  
    dc.SetPen( *wxMEDIUM_GREY_PEN );
    dc.DrawLine( 1, m_height-1, m_width-2, m_height-1 );
    dc.DrawLine( m_width-1, 1, m_width-1, m_height-2 );
  
    dc.SetBrush( *wxBLUE_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle( m_miniEdge, m_miniEdge, m_width - 2*m_miniEdge, m_miniTitle );
  
    dc.SetTextForeground( *wxWHITE );
    dc.SetFont( *wxSMALL_FONT );
    dc.DrawText( m_title, 14 + m_miniEdge, 1 + m_miniEdge );
}

void wxMiniFrame::DrawFrame( int x, int y )
{
    int org_x = 0;    
    int org_y = 0;
    gdk_window_get_origin( m_wxwindow->window, &org_x, &org_y );
    x += org_x;
    y += org_y;

    wxScreenDC dc;
    dc.SetLogicalFunction( wxXOR );
  
    dc.DrawRectangle( x, y, m_width, m_height );
}

void wxMiniFrame::OnMouse( wxMouseEvent &event )
{
    int x = event.GetX();
    int y = event.GetY();

    if (event.LeftDown())
    {
        CaptureMouse();
        m_diffX = x;
        m_diffY = y;
        DrawFrame( 0, 0 );
        m_oldX = 0;
        m_oldY = 0;
        m_isDragging = TRUE;
        return;
    }

    if (event.Dragging() && m_isDragging)
    {
        DrawFrame( m_oldX, m_oldY );
        m_oldX = x - m_diffX;
        m_oldY = y - m_diffY;
        DrawFrame( m_oldX, m_oldY );
        return;
    } 
  
    if (event.LeftUp() && m_isDragging)
    {
       m_isDragging = FALSE;
       DrawFrame( m_oldX, m_oldY );
       ReleaseMouse();
    
       int org_x = 0;    
       int org_y = 0;
       gdk_window_get_origin( m_wxwindow->window, &org_x, &org_y );
       x += org_x - m_diffX;
       y += org_y - m_diffY;
       m_x = x;
       m_y = y;
       gtk_widget_set_uposition( m_widget, x, y );

       return;
    }
  
    event.Skip();
}

