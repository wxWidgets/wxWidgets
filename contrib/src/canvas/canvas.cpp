/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.cpp
// Author:      Robert Roebling
// Created:     XX/XX/XX
// Copyright:   2000 (c) Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "canvas.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/canvas/canvas.h"

#ifdef __WXGTK__
    #include <gtk/gtk.h>
    #include <gdk/gdkrgb.h>
    #include "wx/gtk/win_gtk.h"
#endif

// WDR: class implementations

//----------------------------------------------------------------------------
// wxCanvasObject
//----------------------------------------------------------------------------

wxCanvasObject::wxCanvasObject( int x, int y, int width, int height )
{
    m_owner = NULL;
    m_area.x = x;
    m_area.y = y;
    m_area.width = width;
    m_area.height = height;
    m_isControl = FALSE;
    m_isVector = FALSE;
    m_isImage = FALSE;
}

void wxCanvasObject::Move( int x, int y )
{
    int old_x = m_area.x;
    int old_y = m_area.y;
    
    m_area.x = x;
    m_area.y = y;
    
    if (!m_isControl)
    {
        // TODO: sometimes faster to merge into 1 Update or
        // to break up into four
        m_owner->Update( old_x, old_y, m_area.width, m_area.height );
        m_owner->Update( x, y, m_area.width, m_area.height );
    }
}

void wxCanvasObject::WriteSVG( wxTextOutputStream &stream )
{
}

void wxCanvasObject::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
}

//----------------------------------------------------------------------------
// wxCanvasImage
//----------------------------------------------------------------------------

wxCanvasImage::wxCanvasImage( const wxImage &image, int x, int y )
   : wxCanvasObject( x, y, image.GetWidth(), image.GetHeight() )
{
    m_image = image;
    m_isImage = TRUE;
}

void wxCanvasImage::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
    m_owner->GetBuffer()->Paste( m_image, m_area.x, m_area.y );
}

void wxCanvasImage::WriteSVG( wxTextOutputStream &stream )
{
    // no idea
}

//----------------------------------------------------------------------------
// wxCanvas
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxCanvas,wxScrolledWindow)

BEGIN_EVENT_TABLE(wxCanvas,wxScrolledWindow)
    EVT_CHAR( wxCanvas::OnChar )
    EVT_PAINT( wxCanvas::OnPaint )
    EVT_SIZE( wxCanvas::OnSize )
    EVT_IDLE( wxCanvas::OnIdle )
    EVT_MOUSE_EVENTS( wxCanvas::OnMouse )
    EVT_SET_FOCUS( wxCanvas::OnSetFocus )
    EVT_KILL_FOCUS( wxCanvas::OnKillFocus )
END_EVENT_TABLE()

wxCanvas::wxCanvas( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxScrolledWindow( parent, id, position, size, style )
{
    m_needUpdate = FALSE;
    m_objects.DeleteContents( TRUE );
}

wxCanvas::~wxCanvas()
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }
}

void wxCanvas::SetArea( int width, int height )
{
    m_buffer = wxImage( width, height );
    SetScrollbars( 10, 10, width/10, height/10 );
}

void wxCanvas::Update( int x, int y, int width, int height )
{
    m_needUpdate = TRUE;
    
    m_updateRects.Append(
        (wxObject*) new wxRect( x,y,width,height ) );
    
    // speed up with direct access
    int xx,yy,ww,hh;
    for (yy = y; yy < y+height; yy++)
        for (xx = x; xx < x+width; xx++)
            m_buffer.SetRGB( xx, yy, 0, 0, 0 );

    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();
        xx = obj->GetX();
        yy = obj->GetY();
        ww = obj->GetWidth();
        hh = obj->GetHeight();
            
        // if intersect
        {
            obj->Render( x, y, width, height );
        }
            
        node = node->Next();
    }
}

void wxCanvas::UpdateNow()
{
    if (!m_needUpdate) return;
    
    wxClientDC dc( this );
    PrepareDC( dc );
    
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        wxImage sub_image( m_buffer.GetSubImage( *rect ) );

        // DirectDraw here, please

#ifdef __WXGTK__
        int bpp = wxDisplayDepth(); 
        if (bpp > 8)
        {
            // the init code is doubled in wxImage
            static bool s_hasInitialized = FALSE;

            if (!s_hasInitialized)
            {
                gdk_rgb_init();
                s_hasInitialized = TRUE;
            }
            
            int x = rect->x;
            int y = rect->y;
            CalcScrolledPosition( x, y, &x, &y );

            gdk_draw_rgb_image( GTK_PIZZA(m_wxwindow)->bin_window,
                            m_wxwindow->style->black_gc,
                            x, y,
                            sub_image.GetWidth(), sub_image.GetHeight(),
                            GDK_RGB_DITHER_NONE,
                            sub_image.GetData(),
                            sub_image.GetWidth()*3 );
        }
        else
        {
            wxBitmap bitmap( sub_image.ConvertToBitmap() );
            dc.DrawBitmap( bitmap, rect->x, rect->y );
        }
#endif

#ifndef __WXGTK__
        wxBitmap bitmap( sub_image.ConvertToBitmap() );
        dc.DrawBitmap( bitmap, rect->x, rect->y );
#endif
        
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }
}

void wxCanvas::Prepend( wxCanvasObject* obj )
{
    m_objects.Insert( obj );
    
    obj->SetOwner( this );
    
    if (!obj->IsControl())
        Update( obj->GetX(), obj->GetY(), obj->GetWidth(), obj->GetHeight() );
}

void wxCanvas::Append( wxCanvasObject* obj )
{
    m_objects.Append( obj );
    
    obj->SetOwner( this );
    
    if (!obj->IsControl())
        Update( obj->GetX(), obj->GetY(), obj->GetWidth(), obj->GetHeight() );
}

void wxCanvas::Insert( size_t before, wxCanvasObject* obj )
{
    m_objects.Insert( before, obj );
    
    obj->SetOwner( this );
    
    if (!obj->IsControl())
        Update( obj->GetX(), obj->GetY(), obj->GetWidth(), obj->GetHeight() );
}

void wxCanvas::Remove( wxCanvasObject* obj )
{
    int x = obj->GetX();
    int y = obj->GetY();
    int w = obj->GetWidth();
    int h = obj->GetHeight();
    bool ic = obj->IsControl();
    
    m_objects.DeleteObject( obj );
    
    if (!ic)
        Update( x, y, w, h );
}

void wxCanvas::OnPaint(wxPaintEvent &event)
{
#ifdef __WXMSW__
    wxPaintDC dc(this);
#endif
    
    m_needUpdate = TRUE;

    wxRegionIterator it( GetUpdateRegion() );
    while (it)
    {
        int x = it.GetX();
        int y = it.GetY();
        CalcUnscrolledPosition( x, y, &x, &y );
        
        int w = it.GetWidth();
        int h = it.GetHeight();
        if (x + w > m_buffer.GetWidth())
            w = m_buffer.GetWidth()-x;
        if (y + h > m_buffer.GetHeight())
            h = m_buffer.GetHeight()-y;
        
        m_updateRects.Append( (wxObject*) new wxRect( x, y, w, h ) );
        
        it++;
    }
}

void wxCanvas::OnMouse(wxMouseEvent &event)
{
    // Propagate to objects here
}

void wxCanvas::OnSize(wxSizeEvent &event)
{
    event.Skip();
}

void wxCanvas::OnIdle(wxIdleEvent &event)
{
    UpdateNow();
    event.Skip();
}

void wxCanvas::OnSetFocus(wxFocusEvent &event)
{
}

void wxCanvas::OnKillFocus(wxFocusEvent &event)
{
}

void wxCanvas::OnChar(wxKeyEvent &event)
{
    event.Skip();
}


