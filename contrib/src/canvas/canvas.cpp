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

#ifndef wxUSE_FREETYPE
    #define wxUSE_FREETYPE 1
#endif

#if wxUSE_FREETYPE
    #include <freetype/freetype.h>
#endif

//----------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------

#if wxUSE_FREETYPE
FT_Library g_freetypeLibrary;
#endif

//----------------------------------------------------------------------------
// wxCanvasObject
//----------------------------------------------------------------------------

wxCanvasObject::wxCanvasObject()
{
    m_owner = NULL;
    m_area.x = -1;
    m_area.y = -1;
    m_area.width = -1;
    m_area.height = -1;
    m_isControl = FALSE;
    m_isVector = FALSE;
    m_isImage = FALSE;
}

void wxCanvasObject::SetArea( int x, int y, int width, int height )
{
    m_area.x = x;
    m_area.y = y;
    m_area.width = width;
    m_area.height = height;
}

void wxCanvasObject::SetArea( wxRect rect )
{
    m_area.x = rect.x;
    m_area.y = rect.y;
    m_area.width = rect.width;
    m_area.height = rect.height;
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

bool wxCanvasObject::IsHit( int x, int y, int margin )
{
    return ((x >= m_area.x-margin) &&
            (x <= m_area.x+m_area.width+margin) &&
            (y >= m_area.y-margin) &&
            (y <= m_area.y+m_area.height+margin));
}

void wxCanvasObject::CaptureMouse()
{
    m_owner->SetCaptureMouse( this );
}

void wxCanvasObject::ReleaseMouse()
{
    m_owner->SetCaptureMouse( NULL );
}

bool wxCanvasObject::IsCapturedMouse()
{
    return m_owner->m_captureMouse==this;
}


void wxCanvasObject::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
}

void wxCanvasObject::Recreate()
{
}

void wxCanvasObject::WriteSVG( wxTextOutputStream &stream )
{
}

//----------------------------------------------------------------------------
// wxCanvasRect
//----------------------------------------------------------------------------

wxCanvasRect::wxCanvasRect( double x, double y, double w, double h,
                            unsigned char red, unsigned char green, unsigned char blue )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;

    m_red = red;
    m_green = green;
    m_blue = blue;
}

void wxCanvasRect::Recreate()
{
    SetArea( m_owner->GetDeviceX( m_x ),
             m_owner->GetDeviceY( m_y ),
             m_owner->GetDeviceWidth( m_width ),
             m_owner->GetDeviceHeight( m_height ) );
}

void wxCanvasRect::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
    wxImage *image = m_owner->GetBuffer();
    int buffer_x = m_owner->GetBufferX();
    int buffer_y = m_owner->GetBufferY();
    
    int start_y = clip_y - buffer_y;
    int end_y = clip_y+clip_height - buffer_y;
    
    int start_x = clip_x - buffer_x;
    int end_x = clip_x+clip_width - buffer_x;
    
    // speed up later
    for (int y = start_y; y < end_y; y++)
        for (int x = start_x; x < end_x; x++)
            image->SetRGB( x, y, m_red, m_green, m_blue );
}

void wxCanvasRect::WriteSVG( wxTextOutputStream &stream )
{
}

//----------------------------------------------------------------------------
// wxCanvasLine
//----------------------------------------------------------------------------

wxCanvasLine::wxCanvasLine( double x1, double y1, double x2, double y2,
                            unsigned char red, unsigned char green, unsigned char blue )
   : wxCanvasObject()
{
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;

    m_red = red;
    m_green = green;
    m_blue = blue;
}

void wxCanvasLine::Recreate()
{
    int x1 = m_owner->GetDeviceX( m_x1 );
    int y1 = m_owner->GetDeviceY( m_y1 );
    int x2 = m_owner->GetDeviceX( m_x2 );
    int y2 = m_owner->GetDeviceY( m_y2 );
    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    SetArea( x1, y1, x2-x1+1, y2-y1+1 );
}

void wxCanvasLine::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
    wxImage *image = m_owner->GetBuffer();
    int buffer_x = m_owner->GetBufferX();
    int buffer_y = m_owner->GetBufferY();
    
    if ((m_area.width == 0) && (m_area.height == 0))
    {
        image->SetRGB( m_area.x-buffer_x, m_area.y-buffer_y, m_red, m_green, m_blue );
    }
    else
    {
        int x1 = m_owner->GetDeviceX( m_x1 );
        int y1 = m_owner->GetDeviceY( m_y1 );
        int x2 = m_owner->GetDeviceX( m_x2 );
        int y2 = m_owner->GetDeviceY( m_y2 );

        wxInt32 d, ii, jj, di, ai, si, dj, aj, sj;
        di = x1 - x2;
        ai = abs(di) << 1;
        si = (di < 0)? -1 : 1;
        dj = y1 - y2;
        aj = abs(dj) << 1;
        sj = (dj < 0)? -1 : 1;

        ii = x2;
        jj = y2;

        if (ai > aj)
        {
            // iterate over i
            d = aj - (ai >> 1);

            while (ii != x1)
            {
                if ((ii >= clip_x) && (ii < clip_x+clip_width) &&
                    (jj >= clip_y) && (jj < clip_y+clip_height))
                {
                    image->SetRGB( ii-buffer_x, jj-buffer_y, m_red, m_blue, m_green );
                }
                if (d >= 0)
                {
                    jj += sj;
                    d  -= ai;
                }
                ii += si;
                d  += aj;
            }
        }
        else
        {
            // iterate over j
            d = ai - (aj >> 1);

            while (jj != y1)
            {
                if ((ii >= clip_x) && (ii < clip_x+clip_width) &&
                    (jj >= clip_y) && (jj < clip_y+clip_height))
                {
                    image->SetRGB( ii-buffer_x, jj-buffer_y, m_red, m_blue, m_green );
                }
                if (d >= 0)
                {
                    ii += si;
                    d  -= aj;
                }
                jj += sj;
                d  += ai;
            }
        }
    }
}

void wxCanvasLine::WriteSVG( wxTextOutputStream &stream )
{
    // no idea
}

//----------------------------------------------------------------------------
// wxCanvasImage
//----------------------------------------------------------------------------

wxCanvasImage::wxCanvasImage( const wxImage &image, double x, double y, double w, double h )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;

    m_image = image;
    m_isImage = TRUE;
}

void wxCanvasImage::Recreate()
{
    SetArea( m_owner->GetDeviceX( m_x ),
             m_owner->GetDeviceY( m_y ),
             m_owner->GetDeviceWidth( m_width ),
             m_owner->GetDeviceHeight( m_height ) );

    if ((m_area.width == m_image.GetWidth()) &&
        (m_area.width == m_image.GetWidth()))
        m_tmp = m_image;
    else
        m_tmp = m_image.Scale( m_area.width, m_area.height );
}

void wxCanvasImage::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
    int buffer_x = m_owner->GetBufferX();
    int buffer_y = m_owner->GetBufferY();
    
    if ((clip_x == m_area.x) &&
        (clip_y == m_area.y) &&
        (clip_width == m_area.width) &&
        (clip_height == m_area.height))
    {
        m_owner->GetBuffer()->Paste( m_tmp, clip_x-buffer_x, clip_y-buffer_y );
    }
    else
    {
        // local coordinates
        int start_x = clip_x - m_area.x;
        int start_y = clip_y - m_area.y;

        wxRect rect( start_x, start_y, clip_width, clip_height );
        wxImage sub_image( m_tmp.GetSubImage( rect ) );
        m_owner->GetBuffer()->Paste( sub_image, clip_x-buffer_x, clip_y-buffer_y );
    }
}

void wxCanvasImage::WriteSVG( wxTextOutputStream &stream )
{
    // no idea
}

//----------------------------------------------------------------------------
// wxCanvasCtrl
//----------------------------------------------------------------------------

wxCanvasControl::wxCanvasControl( wxWindow *control )
   : wxCanvasObject()
{
    m_isControl = TRUE;
    m_control = control;
}

wxCanvasControl::~wxCanvasControl()
{
    m_control->Destroy();
}

void wxCanvasControl::Recreate()
{
    m_control->GetSize( &m_area.width, &m_area.height );
    m_control->GetPosition( &m_area.x, &m_area.y );
}

void wxCanvasControl::Move( int x, int y )
{
    m_control->Move( x, y );
}

//----------------------------------------------------------------------------
// wxCanvasText
//----------------------------------------------------------------------------

class wxFaceData
{
public:
#if wxUSE_FREETYPE
     FT_Face   m_face;
#else
     void     *m_dummy;
#endif
};

wxCanvasText::wxCanvasText( const wxString &text, double x, double y, const wxString &fontFile, int size )
   : wxCanvasObject()
{
    m_text = text;
    m_fontFileName = fontFile;
    m_size = size;

    m_red = 0;
    m_green = 0;
    m_blue = 0;

    m_alpha = NULL;

    m_x = x;
    m_y = y;

#if wxUSE_FREETYPE
    wxFaceData *data = new wxFaceData;
    m_faceData = data;

    int error = FT_New_Face( g_freetypeLibrary,
                             m_fontFileName,
                             0,
                             &(data->m_face) );
                             
    error = FT_Set_Char_Size( data->m_face,
                              0,
                              m_size*64,
                              96,    // screen dpi
                              96 );
#endif
}

wxCanvasText::~wxCanvasText()
{
#if wxUSE_FREETYPE    
    wxFaceData *data = (wxFaceData*) m_faceData;
    delete data;
#endif

    if (m_alpha) delete [] m_alpha;
}

void wxCanvasText::SetRGB( unsigned char red, unsigned char green, unsigned char blue )
{
    m_red = red;
    m_green = green;
    m_blue = blue;
}

void wxCanvasText::SetFlag( int flag )
{
    m_flag = flag;
}

void wxCanvasText::Render( int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_alpha) return;
    
    wxImage *image = m_owner->GetBuffer();
    int buffer_x = m_owner->GetBufferX();
    int buffer_y = m_owner->GetBufferY();

    // local coordinates
    int start_x = clip_x - m_area.x;
    int end_x = clip_width + start_x;
    int start_y = clip_y - m_area.y;
    int end_y = clip_height + start_y;
    
    for (int y = start_y; y < end_y; y++)
        for (int x = start_x; x < end_x; x++)
        {
            int alpha = m_alpha[y*m_area.width + x];
            if (alpha)
            {
                int image_x = m_area.x+x - buffer_x;
                int image_y = m_area.y+y - buffer_y;
                if (alpha == 255)
                {
                    image->SetRGB( image_x, image_y, m_red, m_green, m_blue );
                    continue;
                }
                int red1 = (m_red * alpha) / 255;
                int green1 = (m_green * alpha) / 255;
                int blue1 = (m_blue * alpha) / 255;

                alpha = 255-alpha;
                int red2 = image->GetRed( image_x, image_y );
                int green2 = image->GetGreen( image_x, image_y );
                int blue2 = image->GetBlue( image_x, image_y );
                red2 = (red2 * alpha) / 255;
                green2 = (green2 * alpha) / 255;
                blue2 = (blue2 * alpha) / 255;
                
                image->SetRGB( image_x, image_y, red1+red2, green1+green2, blue1+blue2 );
            }
        }
}

void wxCanvasText::WriteSVG( wxTextOutputStream &stream )
{
}

void wxCanvasText::Recreate()
{
    if (m_alpha) delete [] m_alpha;

    m_area.x = m_owner->GetDeviceX( m_x );
    m_area.y = m_owner->GetDeviceY( m_y );

    m_area.width = 100;  // TODO, calculate length
    m_area.height = m_size;
    m_alpha = new unsigned char[100*m_size];
    memset( m_alpha, 0, m_area.width*m_area.height );
    
#if wxUSE_FREETYPE    
    FT_Face face = ((wxFaceData*)m_faceData)->m_face;
    FT_GlyphSlot slot = face->glyph;
    int pen_x = 0;
    int pen_y = m_size;
    
    for (int n = 0; n < (int)m_text.Len(); n++)
    {
        FT_UInt index = FT_Get_Char_Index( face, m_text[n] );

        int error = FT_Load_Glyph( face, index, FT_LOAD_DEFAULT );
        if (error) continue;
        
        error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
        if (error) continue;
        
        FT_Bitmap *bitmap = &slot->bitmap;
        unsigned char* buffer = bitmap->buffer;
        for (int y = 0; y < bitmap->rows; y++)
            for (int x = 0; x < bitmap->width; x++)
            {
                unsigned char alpha = buffer[ y*bitmap->pitch + x ];
                if (alpha == 0) continue;

                int xx = pen_x + slot->bitmap_left + x;
                int yy = pen_y - slot->bitmap_top + y;
                m_alpha[ yy * m_area.width + xx ] = alpha;
            }
        
        pen_x += slot->advance.x >> 6;
        pen_y += slot->advance.y >> 6;
    }    
#endif
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
    EVT_ERASE_BACKGROUND( wxCanvas::OnEraseBackground )
END_EVENT_TABLE()

wxCanvas::wxCanvas( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxScrolledWindow( parent, id, position, size, style )
{
    m_bufferX = 0;
    m_bufferY = 0;
    m_needUpdate = FALSE;
    m_objects.DeleteContents( TRUE );
    m_red = 0;
    m_green = 0;
    m_blue = 0;
    m_lastMouse = (wxCanvasObject*)NULL;
    m_captureMouse = (wxCanvasObject*)NULL;
    m_frozen = TRUE;
    m_requestNewBuffer = TRUE;
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
    SetScrollbars( 10, 10, width/10, height/10 );
}

void wxCanvas::SetColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_red = red;
    m_green = green;
    m_blue = blue;

    SetBackgroundColour( wxColour( red, green, blue ) );

    if (m_frozen) return;

    unsigned char *data = m_buffer.GetData();

    for (int y = 0; y < m_buffer.GetHeight(); y++)
        for (int x = 0; x < m_buffer.GetWidth(); x++)
        {
            data[0] = red;
            data++;
            data[0] = green;
            data++;
            data[0] = blue;
            data++;
        }
}

void wxCanvas::SetCaptureMouse( wxCanvasObject *obj )
{
    if (obj)
    {
        wxWindow::CaptureMouse();
        m_captureMouse = obj;
    }
    else
    {
        wxWindow::ReleaseMouse();
        m_captureMouse = NULL;
    }
}

void wxCanvas::Freeze()
{
    m_frozen = TRUE;
}

void wxCanvas::Thaw()
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_frozen = FALSE;

    if (m_buffer.Ok())
        Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), m_buffer.GetHeight() );
}

void wxCanvas::Update( int x, int y, int width, int height, bool blit )
{
    if (m_frozen) return;
    
    // clip to buffer
    if (x < m_bufferX)
    {
        width -= m_bufferX-x;
        x = m_bufferX;
    }
    if (width < 0) return;

    if (y < m_bufferY)
    {
        height -= m_bufferY-y;
        y = m_bufferY;
    }
    if (height < 0) return;

    if (x+width > m_bufferX+m_buffer.GetWidth())
    {
        width = m_bufferX+m_buffer.GetWidth() - x;
    }
    if (width < 0) return;

    if (y+height > m_bufferY+m_buffer.GetHeight())
    {
        height = m_bufferY+m_buffer.GetHeight() - y;
    }
    if (height < 0) return;

    // update is within the buffer
    m_needUpdate = TRUE;

    // has to be blitted to screen later
    if (blit)
    {
        m_updateRects.Append(
            (wxObject*) new wxRect( x,y,width,height ) );
    }

    // speed up with direct access, maybe add wxImage::Clear(x,y,w,h,r,g,b)
    int start_y = y - m_bufferY;
    int end_y = y+height - m_bufferY;
    int start_x = x - m_bufferX;
    int end_x = x+width - m_bufferX;
    for (int yy = start_y; yy < end_y; yy++)
        for (int xx = start_x; xx < end_x; xx++)
            m_buffer.SetRGB( xx, yy, m_red, m_green, m_blue );

    // cycle through all objects
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        if (!obj->IsControl())
        {
            // If we have 10.000 objects, we will go through
            // this 10.000 times for each update, so we have
            // to optimise carefully.
            int clip_x = obj->GetX();
            int clip_width = obj->GetWidth();
            if (clip_x < x)
            {
                clip_width -= x-clip_x;
                clip_x = x;
            }
            if (clip_width > 0)
            {
                if (clip_x + clip_width > x + width)
                    clip_width = x+width-clip_x;

                if (clip_width > 0)
                {
                    int clip_y = obj->GetY();
                    int clip_height = obj->GetHeight();
                    if (clip_y < y)
                    {
                        clip_height -= y-clip_y;
                        clip_y = y;
                    }
                    if (clip_height > 0)
                    {
                        if (clip_y + clip_height > y + height)
                            clip_height = y+height-clip_y;

                        if (clip_height > 0)
                            obj->Render( clip_x, clip_y, clip_width, clip_height );
                    }
                }
            }
        }

        node = node->Next();
    }
}

void wxCanvas::BlitBuffer( wxDC &dc )
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();

        wxRect sub_rect( *rect );
        sub_rect.x -= m_bufferX;
        sub_rect.y -= m_bufferY;
        
        wxImage sub_image( m_buffer.GetSubImage( sub_rect ) );

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

            gdk_draw_rgb_image( GTK_PIZZA(m_wxwindow)->bin_window,
                            m_wxwindow->style->black_gc,
                            sub_rect.x, sub_rect.y,
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

    m_needUpdate = FALSE;
}

void wxCanvas::UpdateNow()
{
    if (m_frozen) return;

    if (!m_needUpdate) return;

    wxClientDC dc( this );
    PrepareDC( dc );

    BlitBuffer( dc );
}

int wxCanvas::GetDeviceX( double x )
{
    return (int) x;
}

int wxCanvas::GetDeviceY( double y )
{
    return (int) y;
}

int wxCanvas::GetDeviceWidth( double width )
{
    return (int) width;
}

int wxCanvas::GetDeviceHeight( double height )
{
    return (int) height;
}

void wxCanvas::Recreate()
{
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->Recreate();

        node = node->Next();
    }
}

void wxCanvas::Prepend( wxCanvasObject* obj )
{
    m_objects.Insert( obj );

    obj->SetOwner( this );
    obj->Recreate();

    if (!obj->IsControl())
        Update( obj->GetX(), obj->GetY(), obj->GetWidth(), obj->GetHeight() );
}

void wxCanvas::Append( wxCanvasObject* obj )
{
    m_objects.Append( obj );

    obj->SetOwner( this );
    obj->Recreate();

    if (!obj->IsControl())
        Update( obj->GetX(), obj->GetY(), obj->GetWidth(), obj->GetHeight() );
}

void wxCanvas::Insert( size_t before, wxCanvasObject* obj )
{
    m_objects.Insert( before, obj );

    obj->SetOwner( this );
    obj->Recreate();

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
    wxPaintDC dc(this);
    PrepareDC( dc );

    if (!m_buffer.Ok()) return;
    
    if (m_frozen) return;

    m_needUpdate = TRUE;

    wxRegionIterator it( GetUpdateRegion() );
    while (it)
    {
        int x = it.GetX();
        int y = it.GetY();

        int w = it.GetWidth();
        int h = it.GetHeight();

        if (x+w > m_buffer.GetWidth())
            w = m_buffer.GetWidth() - x;
        if (y+h > m_buffer.GetHeight())
            h = m_buffer.GetHeight() - y;

        if ((w > 0) && (h > 0))
        {
            CalcUnscrolledPosition( x, y, &x, &y );
            m_updateRects.Append( (wxObject*) new wxRect( x, y, w, h ) );
        }

        it++;
    }

    BlitBuffer( dc );
}

void wxCanvas::ScrollWindow( int dx, int dy, const wxRect* rect )
{
    // If any updates are pending, do them now since they will
    // expect the previous m_bufferX and m_bufferY values.
    UpdateNow();

    // The buffer always starts at the top left corner of the
    // client area. Indeed, it is the client area.
    CalcUnscrolledPosition( 0, 0, &m_bufferX, &m_bufferY );

    unsigned char* data = m_buffer.GetData();
    
    if (dy != 0)
    {
        if (dy > 0)
        {
            unsigned char *source = data;
            unsigned char *dest = data + (dy * m_buffer.GetWidth() * 3);
            size_t count = (size_t) (m_buffer.GetWidth() * 3 * (m_buffer.GetHeight()-dy));
            memmove( dest, source, count );
            
            // We update the new buffer area, but there is no need to
            // blit (last param FALSE) since the ensuing paint event will
            // do that anyway.
            Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), dy, FALSE );
        }
        else
        {
            unsigned char *dest = data;
            unsigned char *source = data + (-dy * m_buffer.GetWidth() * 3);
            size_t count = (size_t) (m_buffer.GetWidth() * 3 * (m_buffer.GetHeight()+dy));
            memmove( dest, source, count );
            
            // We update the new buffer area, but there is no need to
            // blit (last param FALSE) since the ensuing paint event will
            // do that anyway.
            Update( m_bufferX, m_bufferY+m_buffer.GetHeight()+dy, m_buffer.GetWidth(), -dy, FALSE );
        }
    }
    
    if (dx != 0)
    {
        if (dx > 0)
        {
            unsigned char *source = data;
            for (int y = 0; y < m_buffer.GetHeight(); y++)
            {
                unsigned char *dest = source + dx*3;
                memmove( dest, source, (m_buffer.GetWidth()-dx) * 3 );
                source += m_buffer.GetWidth()*3;
            }
            
            // We update the new buffer area, but there is no need to
            // blit (last param FALSE) since the ensuing paint event will
            // do that anyway.
            Update( m_bufferX, m_bufferY, dx, m_buffer.GetHeight(), FALSE );
        }
        else
        {
            unsigned char *dest = data;
            for (int y = 0; y < m_buffer.GetHeight(); y++)
            {
                unsigned char *source = dest - dx*3;
                memmove( dest, source, (m_buffer.GetWidth()+dx) * 3 );
                dest += m_buffer.GetWidth()*3;
            }
            
            // We update the new buffer area, but there is no need to
            // blit (last param FALSE) since the ensuing paint event will
            // do that anyway.
            Update( m_bufferX+m_buffer.GetWidth()+dx, m_bufferY, -dx, m_buffer.GetHeight(), FALSE );
        }
    }

    wxWindow::ScrollWindow( dx, dy, rect );
}

void wxCanvas::OnMouse(wxMouseEvent &event)
{
    int x = event.GetX();
    int y = event.GetY();
    CalcUnscrolledPosition( x, y, &x, &y );

    if (event.GetEventType() == wxEVT_MOTION)
    {
        if (m_captureMouse) //no matter what go to this one
        {
            wxMouseEvent child_event( wxEVT_MOTION );
            child_event.SetEventObject(m_captureMouse);
            child_event.m_x = x - m_captureMouse->GetX();
            child_event.m_y = y - m_captureMouse->GetY();
            child_event.m_leftDown = event.m_leftDown;
            child_event.m_rightDown = event.m_rightDown;
            child_event.m_middleDown = event.m_middleDown;
            child_event.m_controlDown = event.m_controlDown;
            child_event.m_shiftDown = event.m_shiftDown;
            child_event.m_altDown = event.m_altDown;
            child_event.m_metaDown = event.m_metaDown;
            m_captureMouse->ProcessEvent( child_event );
        }
        else
        {
            wxNode *node = m_objects.Last();
            while (node)
            {
                wxCanvasObject *obj = (wxCanvasObject*) node->Data();

                if (!obj->IsControl())
                {
                    if (obj->IsHit(x,y))
                    {
                        wxMouseEvent child_event( wxEVT_MOTION );
                        child_event.SetEventObject( obj );
                        child_event.m_x = x - obj->GetX();
                        child_event.m_y = y - obj->GetY();
                        child_event.m_leftDown = event.m_leftDown;
                        child_event.m_rightDown = event.m_rightDown;
                        child_event.m_middleDown = event.m_middleDown;
                        child_event.m_controlDown = event.m_controlDown;
                        child_event.m_shiftDown = event.m_shiftDown;
                        child_event.m_altDown = event.m_altDown;
                        child_event.m_metaDown = event.m_metaDown;

                        if ((obj != m_lastMouse) && (m_lastMouse != NULL))
                        {
                            child_event.SetEventType( wxEVT_LEAVE_WINDOW );
                            child_event.SetEventObject( m_lastMouse );
                            child_event.m_x = x - m_lastMouse->GetX();
                            child_event.m_y = y - m_lastMouse->GetY();
                            m_lastMouse->ProcessEvent( child_event );

                            m_lastMouse = obj;
                            child_event.SetEventType( wxEVT_ENTER_WINDOW );
                            child_event.SetEventObject( m_lastMouse );
                            child_event.m_x = x - m_lastMouse->GetX();
                            child_event.m_y = y - m_lastMouse->GetY();
                            m_lastMouse->ProcessEvent( child_event );

                            child_event.SetEventType( wxEVT_MOTION );
                            child_event.SetEventObject( obj );
                        }
                        obj->ProcessEvent( child_event );
                        return;
                    }
                }
                node = node->Previous();
            }
        }
        if (m_lastMouse)
        {
            wxMouseEvent child_event( wxEVT_LEAVE_WINDOW );
            child_event.SetEventObject( m_lastMouse );
            child_event.m_x = x - m_lastMouse->GetX();
            child_event.m_y = y - m_lastMouse->GetY();
            child_event.m_leftDown = event.m_leftDown;
            child_event.m_rightDown = event.m_rightDown;
            child_event.m_middleDown = event.m_middleDown;
            child_event.m_controlDown = event.m_controlDown;
            child_event.m_shiftDown = event.m_shiftDown;
            child_event.m_altDown = event.m_altDown;
            child_event.m_metaDown = event.m_metaDown;
            m_lastMouse->ProcessEvent( child_event );

            m_lastMouse = (wxCanvasObject*) NULL;
            return;
        }
    }
    event.Skip();
}

void wxCanvas::OnSize(wxSizeEvent &event)
{
    int w,h;
    GetClientSize( &w, &h );
    m_buffer = wxImage( w, h );
        
    CalcUnscrolledPosition( 0, 0, &m_bufferX, &m_bufferY );
        
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_frozen = FALSE;

    Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), m_buffer.GetHeight(), FALSE );

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

void wxCanvas::OnEraseBackground(wxEraseEvent &event)
{
}

//--------------------------------------------------------------------
// wxCanvasModule
//--------------------------------------------------------------------

class wxCanvasModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxCanvasModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxCanvasModule, wxModule)

bool wxCanvasModule::OnInit()
{
#if wxUSE_FREETYPE
    int error = FT_Init_FreeType( &g_freetypeLibrary );
    if (error) return FALSE;
#endif

    return TRUE;
}

void wxCanvasModule::OnExit()
{
#if wxUSE_FREETYPE
    FT_Done_FreeType( g_freetypeLibrary );
#endif
}
