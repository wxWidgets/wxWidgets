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

#define USE_FREETYPE 1

#if USE_FREETYPE
#include <freetype/freetype.h>
#endif

//----------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------

#if USE_FREETYPE
FT_Library g_freetypeLibrary;
#endif

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
    int start_x = wxMax( 0, clip_x-m_area.x );
    int end_x = wxMin( m_area.width, clip_width+clip_x-m_area.x );
    int start_y = wxMax( 0, clip_y-m_area.y );
    int end_y = wxMin( m_area.height, clip_height+clip_y-m_area.y );
    
    if (end_x < start_x) return;
    if (end_y < start_y) return;
    
    if ((start_x == 0) && 
        (start_y == 0) && 
        (end_x == m_area.width) &&
        (end_y == m_area.height))
    {
        m_owner->GetBuffer()->Paste( m_image, m_area.x, m_area.y );
    }
    else
    {
        wxRect rect( start_x, start_y, end_x-start_x, end_y-start_y );
        wxImage sub_image( m_image.GetSubImage( rect ) );
        m_owner->GetBuffer()->Paste( sub_image, m_area.x+start_x, m_area.y+start_y );
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
   : wxCanvasObject( -1, -1, -1, -1 )
{
    m_control = control;
    UpdateSize();
}

wxCanvasControl::~wxCanvasControl()
{
    m_control->Destroy();
}

void wxCanvasControl::Move( int x, int y )
{
    m_control->Move( x, y );
}

void wxCanvasControl::UpdateSize()
{
    m_control->GetSize( &m_area.width, &m_area.height );
    m_control->GetPosition( &m_area.x, &m_area.y );
}

//----------------------------------------------------------------------------
// wxCanvasText
//----------------------------------------------------------------------------

class wxFaceData
{
public:
#if USE_FREETYPE
     FT_Face   m_face;
#else
     void     *m_dummy;
#endif    
};

wxCanvasText::wxCanvasText( const wxString &text, int x, int y, const wxString &fontFile, int size )
   : wxCanvasObject( x, y, -1, -1 )
{
    m_text = text;
    m_fontFileName = fontFile;
    m_size = size;
    
    m_red = 0;
    m_green = 0;
    m_blue = 0;
    
    // test
    m_area.width = 100;
    m_area.height = m_size;
    m_alpha = new unsigned char[100*m_size];
    memset( m_alpha, 0, m_area.width*m_area.height );
    
#if USE_FREETYPE    
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
    CreateBuffer();
#endif
}

wxCanvasText::~wxCanvasText()
{
#if USE_FREETYPE    
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

    int start_x = wxMax( 0, clip_x-m_area.x );
    int end_x = wxMin( m_area.width, clip_width+clip_x-m_area.x );
    int start_y = wxMax( 0, clip_y-m_area.y );
    int end_y = wxMin( m_area.height, clip_height+clip_y-m_area.y );
    
    for (int y = start_y; y < end_y; y++)
        for (int x = start_x; x < end_x; x++)
        {
            int alpha = m_alpha[y*m_area.width + x];
            if (alpha)
            {
                int image_x = m_area.x+x;
                int image_y = m_area.y+y;
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

void wxCanvasText::CreateBuffer()
{
#if USE_FREETYPE    
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
END_EVENT_TABLE()

wxCanvas::wxCanvas( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxScrolledWindow( parent, id, position, size, style )
{
    m_needUpdate = FALSE;
    m_objects.DeleteContents( TRUE );
    m_red = 0;
    m_green = 0;
    m_blue = 0;
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

void wxCanvas::SetColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_red = red;
    m_green = green;
    m_blue = blue;
    
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

void wxCanvas::Update( int x, int y, int width, int height )
{
    m_needUpdate = TRUE;
    
    m_updateRects.Append(
        (wxObject*) new wxRect( x,y,width,height ) );
    
    // speed up with direct access, maybe add wxImage::Clear(x,y,w,h)
    int xx,yy,ww,hh;
    for (yy = y; yy < y+height; yy++)
        for (xx = x; xx < x+width; xx++)
            m_buffer.SetRGB( xx, yy, m_red, m_green, m_blue );

    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();
        xx = obj->GetX();
        yy = obj->GetY();
        ww = obj->GetWidth();
        hh = obj->GetHeight();
            
        if (!obj->IsControl())  // calc intersection !
        {
            obj->Render( x, y, width, height );
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
    
    m_needUpdate = FALSE;
}

void wxCanvas::UpdateNow()
{
    if (!m_needUpdate) return;
    
    wxClientDC dc( this );
    PrepareDC( dc );
    
    BlitBuffer( dc );
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
    wxPaintDC dc(this);
    PrepareDC( dc );
    
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
    
    BlitBuffer( dc );
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
#if USE_FREETYPE
    int error = FT_Init_FreeType( &g_freetypeLibrary );
    if (error) return FALSE;
#endif

    return TRUE;
}

void wxCanvasModule::OnExit()
{
#if USE_FREETYPE
    FT_Done_FreeType( g_freetypeLibrary );
#endif
}
