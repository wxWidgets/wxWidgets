/////////////////////////////////////////////////////////////////////////////
// Name:        image.cpp
// Purpose:     wxImage
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "image.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_IMAGE

#include "wx/image.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"
#include "wx/hash.h"
#include "wx/utils.h"

// For memcpy
#include <string.h>
#include <math.h>

#ifdef __SALFORDC__
    #undef FAR
#endif


//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------

class wxImageRefData: public wxObjectRefData
{
public:
    wxImageRefData();
    virtual ~wxImageRefData();

    int             m_width;
    int             m_height;
    unsigned char  *m_data;

    bool            m_hasMask;
    unsigned char   m_maskRed,m_maskGreen,m_maskBlue;

    // alpha channel data, may be NULL for the formats without alpha support
    unsigned char  *m_alpha;

    bool            m_ok;
    bool            m_static;

#if wxUSE_PALETTE
    wxPalette       m_palette;
#endif // wxUSE_PALETTE

    wxArrayString   m_optionNames;
    wxArrayString   m_optionValues;

    DECLARE_NO_COPY_CLASS(wxImageRefData)
};

wxImageRefData::wxImageRefData()
{
    m_width = 0;
    m_height = 0;
    m_data =
    m_alpha = (unsigned char *) NULL;

    m_maskRed = 0;
    m_maskGreen = 0;
    m_maskBlue = 0;
    m_hasMask = false;

    m_ok = false;
    m_static = false;
}

wxImageRefData::~wxImageRefData()
{
    if (!m_static)
    {
        free( m_data );
        free( m_alpha );
    }
}

wxList wxImage::sm_handlers;

wxImage wxNullImage;

//-----------------------------------------------------------------------------

#define M_IMGDATA ((wxImageRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxImage, wxObject)

wxImage::wxImage( int width, int height, bool clear )
{
    Create( width, height, clear );
}

wxImage::wxImage( int width, int height, unsigned char* data, bool static_data )
{
    Create( width, height, data, static_data );
}

wxImage::wxImage( int width, int height, unsigned char* data, unsigned char* alpha, bool static_data )
{
    Create( width, height, data, alpha, static_data );
}

wxImage::wxImage( const wxString& name, long type, int index )
{
    LoadFile( name, type, index );
}

wxImage::wxImage( const wxString& name, const wxString& mimetype, int index )
{
    LoadFile( name, mimetype, index );
}

#if wxUSE_STREAMS
wxImage::wxImage( wxInputStream& stream, long type, int index )
{
    LoadFile( stream, type, index );
}

wxImage::wxImage( wxInputStream& stream, const wxString& mimetype, int index )
{
    LoadFile( stream, mimetype, index );
}
#endif // wxUSE_STREAMS

wxImage::wxImage( const wxImage& image )
    : wxObject()
{
    Ref(image);
}

wxImage::wxImage( const wxImage* image )
{
    if (image) Ref(*image);
}

bool wxImage::Create( int width, int height, bool clear )
{
    UnRef();

    m_refData = new wxImageRefData();

    M_IMGDATA->m_data = (unsigned char *) malloc( width*height*3 );
    if (!M_IMGDATA->m_data)
    {
        UnRef();
        return false;
    }

    if (clear)
        memset(M_IMGDATA->m_data, 0, width*height*3);

    M_IMGDATA->m_width = width;
    M_IMGDATA->m_height = height;
    M_IMGDATA->m_ok = true;

    return true;
}

bool wxImage::Create( int width, int height, unsigned char* data, bool static_data )
{
    UnRef();

    wxCHECK_MSG( data, false, _T("NULL data in wxImage::Create") );

    m_refData = new wxImageRefData();

    M_IMGDATA->m_data = data;
    M_IMGDATA->m_width = width;
    M_IMGDATA->m_height = height;
    M_IMGDATA->m_ok = true;
    M_IMGDATA->m_static = static_data;

    return true;
}

bool wxImage::Create( int width, int height, unsigned char* data, unsigned char* alpha, bool static_data )
{
    UnRef();

    wxCHECK_MSG( data, false, _T("NULL data in wxImage::Create") );

    m_refData = new wxImageRefData();

    M_IMGDATA->m_data = data;
    M_IMGDATA->m_alpha = alpha;
    M_IMGDATA->m_width = width;
    M_IMGDATA->m_height = height;
    M_IMGDATA->m_ok = true;
    M_IMGDATA->m_static = static_data;

    return true;
}

void wxImage::Destroy()
{
    UnRef();
}

wxImage wxImage::Copy() const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height, false );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );
    image.SetMask( M_IMGDATA->m_hasMask );

    memcpy( data, GetData(), M_IMGDATA->m_width*M_IMGDATA->m_height*3 );

    // also copy the image options
    wxImageRefData *imgData = (wxImageRefData *)image.m_refData;
    imgData->m_optionNames = M_IMGDATA->m_optionNames;
    imgData->m_optionValues = M_IMGDATA->m_optionValues;

    return image;
}

wxImage wxImage::ShrinkBy( int xFactor , int yFactor ) const
{
    if( xFactor == 1 && yFactor == 1 )
        return Copy() ;

    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    // can't scale to/from 0 size
    wxCHECK_MSG( (xFactor > 0) && (yFactor > 0), image,
                 wxT("invalid new image size") );

    long old_height = M_IMGDATA->m_height,
         old_width  = M_IMGDATA->m_width;

    wxCHECK_MSG( (old_height > 0) && (old_width > 0), image,
                 wxT("invalid old image size") );

    long width = old_width / xFactor ;
    long height = old_height / yFactor ;

    image.Create( width, height, false );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    bool hasMask = false ;
    unsigned char maskRed = 0;
    unsigned char maskGreen = 0;
    unsigned char maskBlue =0 ;
    if (M_IMGDATA->m_hasMask)
    {
        hasMask = true ;
        maskRed = M_IMGDATA->m_maskRed;
        maskGreen = M_IMGDATA->m_maskGreen;
        maskBlue =M_IMGDATA->m_maskBlue ;

        image.SetMaskColour( M_IMGDATA->m_maskRed,
                             M_IMGDATA->m_maskGreen,
                             M_IMGDATA->m_maskBlue );
    }
    char unsigned *source_data = M_IMGDATA->m_data;
    char unsigned *target_data = data;

    for (long y = 0; y < height; y++)
    {
        for (long x = 0; x < width; x++)
        {
            unsigned long avgRed = 0 ;
            unsigned long avgGreen = 0;
            unsigned long avgBlue = 0;
            unsigned long counter = 0 ;
            // determine average
            for ( int y1 = 0 ; y1 < yFactor ; ++y1 )
            {
                long y_offset = (y * yFactor + y1) * old_width;
                for ( int x1 = 0 ; x1 < xFactor ; ++x1 )
                {
                    unsigned char *pixel = source_data + 3 * ( y_offset + x * xFactor + x1 ) ;
                    unsigned char red = pixel[0] ;
                    unsigned char green = pixel[1] ;
                    unsigned char blue = pixel[2] ;
                    if ( !hasMask || red != maskRed || green != maskGreen || blue != maskBlue )
                    {
                        avgRed += red ;
                        avgGreen += green ;
                        avgBlue += blue ;
                        counter++ ;
                    }
                }
            }
            if ( counter == 0 )
            {
                *(target_data++) = M_IMGDATA->m_maskRed ;
                *(target_data++) = M_IMGDATA->m_maskGreen ;
                *(target_data++) = M_IMGDATA->m_maskBlue ;
            }
            else
            {
                *(target_data++) = (unsigned char)(avgRed / counter);
                *(target_data++) = (unsigned char)(avgGreen / counter);
                *(target_data++) = (unsigned char)(avgBlue / counter);
            }
        }
    }

    // In case this is a cursor, make sure the hotspot is scalled accordingly:
    if ( HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X) )
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X,
                (GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X))/xFactor);
    if ( HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y) )
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y,
                (GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y))/yFactor);

    return image;
}

wxImage wxImage::Scale( int width, int height ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    // can't scale to/from 0 size
    wxCHECK_MSG( (width > 0) && (height > 0), image,
                 wxT("invalid new image size") );

    long old_height = M_IMGDATA->m_height,
         old_width  = M_IMGDATA->m_width;
    wxCHECK_MSG( (old_height > 0) && (old_width > 0), image,
                 wxT("invalid old image size") );

    if ( old_width % width == 0 && old_width >= width &&
        old_height % height == 0 && old_height >= height )
    {
        return ShrinkBy( old_width / width , old_height / height ) ;
    }
    image.Create( width, height, false );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
    {
        image.SetMaskColour( M_IMGDATA->m_maskRed,
                             M_IMGDATA->m_maskGreen,
                             M_IMGDATA->m_maskBlue );
    }

    unsigned char *source_data = M_IMGDATA->m_data;
    unsigned char *target_data = data;

    long x_delta = (old_width<<16) / width;
    long y_delta = (old_height<<16) / height;

    unsigned char* dest_pixel = target_data;

    long y = 0;
    for ( long j = 0; j < height; j++ )
        {
        unsigned char* src_line = &source_data[(y>>16)*old_width*3];

        long x = 0;
        for ( long i = 0; i < width; i++ )
        {
             unsigned char* src_pixel = &src_line[(x>>16)*3];
             dest_pixel[0] = src_pixel[0];
             dest_pixel[1] = src_pixel[1];
             dest_pixel[2] = src_pixel[2];
             dest_pixel += 3;
             x += x_delta;
        }

        y += y_delta;
    }

    // In case this is a cursor, make sure the hotspot is scalled accordingly:
    if ( HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X) )
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X,
                (GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X)*width)/old_width);
    if ( HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y) )
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y,
                (GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y)*height)/old_height);

    return image;
}

wxImage wxImage::Rotate90( bool clockwise ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_height, M_IMGDATA->m_width, false );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    long height = M_IMGDATA->m_height;
    long width  = M_IMGDATA->m_width;

    unsigned char *source_data = M_IMGDATA->m_data;
    unsigned char *target_data;

    for (long j = 0; j < height; j++)
    {
        for (long i = 0; i < width; i++)
        {
            if (clockwise)
                target_data = data + (((i+1)*height) - j - 1)*3;
            else
                target_data = data + ((height*(width-1)) + j - (i*height))*3;
            memcpy( target_data, source_data, 3 );
            source_data += 3;
        }
    }

    return image;
}

wxImage wxImage::Mirror( bool horizontally ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height, false );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    long height = M_IMGDATA->m_height;
    long width  = M_IMGDATA->m_width;

    unsigned char *source_data = M_IMGDATA->m_data;
    unsigned char *target_data;

    if (horizontally)
    {
        for (long j = 0; j < height; j++)
        {
            data += width*3;
            target_data = data-3;
            for (long i = 0; i < width; i++)
            {
                memcpy( target_data, source_data, 3 );
                source_data += 3;
                target_data -= 3;
            }
        }
    }
    else
    {
        for (long i = 0; i < height; i++)
        {
            target_data = data + 3*width*(height-1-i);
            memcpy( target_data, source_data, (size_t)3*width );
            source_data += 3*width;
        }
    }

    return image;
}

wxImage wxImage::GetSubImage( const wxRect &rect ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    wxCHECK_MSG( (rect.GetLeft()>=0) && (rect.GetTop()>=0) && (rect.GetRight()<=GetWidth()) && (rect.GetBottom()<=GetHeight()),
                 image, wxT("invalid subimage size") );

    int subwidth=rect.GetWidth();
    const int subheight=rect.GetHeight();

    image.Create( subwidth, subheight, false );

    unsigned char *subdata = image.GetData(), *data=GetData();

    wxCHECK_MSG( subdata, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    const int subleft=3*rect.GetLeft();
    const int width=3*GetWidth();
    subwidth*=3;

    data+=rect.GetTop()*width+subleft;

    for (long j = 0; j < subheight; ++j)
    {
        memcpy( subdata, data, subwidth);
        subdata+=subwidth;
        data+=width;
    }

    return image;
}

void wxImage::Paste( const wxImage &image, int x, int y )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );
    wxCHECK_RET( image.Ok(), wxT("invalid image") );

    int xx = 0;
    int yy = 0;
    int width = image.GetWidth();
    int height = image.GetHeight();

    if (x < 0)
    {
        xx = -x;
        width += x;
    }
    if (y < 0)
    {
        yy = -y;
        height += y;
    }

    if ((x+xx)+width > M_IMGDATA->m_width)
        width = M_IMGDATA->m_width - (x+xx);
    if ((y+yy)+height > M_IMGDATA->m_height)
        height = M_IMGDATA->m_height - (y+yy);

    if (width < 1) return;
    if (height < 1) return;

    if ((!HasMask() && !image.HasMask()) ||
       ((HasMask() && image.HasMask() &&
         (GetMaskRed()==image.GetMaskRed()) &&
         (GetMaskGreen()==image.GetMaskGreen()) &&
         (GetMaskBlue()==image.GetMaskBlue()))))
    {
        width *= 3;
        unsigned char* source_data = image.GetData() + xx*3 + yy*3*image.GetWidth();
        int source_step = image.GetWidth()*3;

        unsigned char* target_data = GetData() + (x+xx)*3 + (y+yy)*3*M_IMGDATA->m_width;
        int target_step = M_IMGDATA->m_width*3;
        for (int j = 0; j < height; j++)
        {
            memcpy( target_data, source_data, width );
            source_data += source_step;
            target_data += target_step;
        }
        return;
    }

    if (!HasMask() && image.HasMask())
    {
        unsigned char r = image.GetMaskRed();
        unsigned char g = image.GetMaskGreen();
        unsigned char b = image.GetMaskBlue();

        width *= 3;
        unsigned char* source_data = image.GetData() + xx*3 + yy*3*image.GetWidth();
        int source_step = image.GetWidth()*3;

        unsigned char* target_data = GetData() + (x+xx)*3 + (y+yy)*3*M_IMGDATA->m_width;
        int target_step = M_IMGDATA->m_width*3;

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i+=3)
            {
                if ((source_data[i]   != r) &&
                    (source_data[i+1] != g) &&
                    (source_data[i+2] != b))
                {
                    memcpy( target_data+i, source_data+i, 3 );
                }
            }
            source_data += source_step;
            target_data += target_step;
        }
    }
}

void wxImage::Replace( unsigned char r1, unsigned char g1, unsigned char b1,
                       unsigned char r2, unsigned char g2, unsigned char b2 )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    unsigned char *data = GetData();

    const int w = GetWidth();
    const int h = GetHeight();

    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
        {
            if ((data[0] == r1) && (data[1] == g1) && (data[2] == b1))
            {
                data[0] = r2;
                data[1] = g2;
                data[2] = b2;
            }
            data += 3;
        }
}

wxImage wxImage::ConvertToMono( unsigned char r, unsigned char g, unsigned char b ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height, false );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
    {
        if (M_IMGDATA->m_maskRed == r && M_IMGDATA->m_maskGreen == g &&
                                         M_IMGDATA->m_maskBlue == b)
            image.SetMaskColour( 255, 255, 255 );
        else
            image.SetMaskColour( 0, 0, 0 );
    }

    long size = M_IMGDATA->m_height * M_IMGDATA->m_width;

    unsigned char *srcd = M_IMGDATA->m_data;
    unsigned char *tard = image.GetData();

    for ( long i = 0; i < size; i++, srcd += 3, tard += 3 )
    {
        if (srcd[0] == r && srcd[1] == g && srcd[2] == b)
            tard[0] = tard[1] = tard[2] = 255;
        else
            tard[0] = tard[1] = tard[2] = 0;
    }

    return image;
}

void wxImage::SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_RET( (x>=0) && (y>=0) && (x<w) && (y<h), wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    M_IMGDATA->m_data[ pos   ] = r;
    M_IMGDATA->m_data[ pos+1 ] = g;
    M_IMGDATA->m_data[ pos+2 ] = b;
}

unsigned char wxImage::GetRed( int x, int y ) const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos];
}

unsigned char wxImage::GetGreen( int x, int y ) const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+1];
}

unsigned char wxImage::GetBlue( int x, int y ) const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+2];
}

bool wxImage::Ok() const
{
    // image of 0 width or height can't be considered ok - at least because it
    // causes crashes in ConvertToBitmap() if we don't catch it in time
    wxImageRefData *data = M_IMGDATA;
    return data && data->m_ok && data->m_width && data->m_height;
}

unsigned char *wxImage::GetData() const
{
    wxCHECK_MSG( Ok(), (unsigned char *)NULL, wxT("invalid image") );

    return M_IMGDATA->m_data;
}

void wxImage::SetData( unsigned char *data )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    wxImageRefData *newRefData = new wxImageRefData();

    newRefData->m_width = M_IMGDATA->m_width;
    newRefData->m_height = M_IMGDATA->m_height;
    newRefData->m_data = data;
    newRefData->m_ok = true;
    newRefData->m_maskRed = M_IMGDATA->m_maskRed;
    newRefData->m_maskGreen = M_IMGDATA->m_maskGreen;
    newRefData->m_maskBlue = M_IMGDATA->m_maskBlue;
    newRefData->m_hasMask = M_IMGDATA->m_hasMask;

    UnRef();

    m_refData = newRefData;
}

void wxImage::SetData( unsigned char *data, int new_width, int new_height )
{
    wxImageRefData *newRefData = new wxImageRefData();

    if (m_refData)
    {
        newRefData->m_width = new_width;
        newRefData->m_height = new_height;
        newRefData->m_data = data;
        newRefData->m_ok = true;
        newRefData->m_maskRed = M_IMGDATA->m_maskRed;
        newRefData->m_maskGreen = M_IMGDATA->m_maskGreen;
        newRefData->m_maskBlue = M_IMGDATA->m_maskBlue;
        newRefData->m_hasMask = M_IMGDATA->m_hasMask;
    }
    else
    {
        newRefData->m_width = new_width;
        newRefData->m_height = new_height;
        newRefData->m_data = data;
        newRefData->m_ok = true;
    }

    UnRef();

    m_refData = newRefData;
}

// ----------------------------------------------------------------------------
// alpha channel support
// ----------------------------------------------------------------------------

void wxImage::SetAlpha(int x, int y, unsigned char alpha)
{
    wxCHECK_RET( Ok() && HasAlpha(), wxT("invalid image or no alpha channel") );

    int w = M_IMGDATA->m_width,
        h = M_IMGDATA->m_height;

    wxCHECK_RET( x >=0 && y >= 0 && x < w && y < h, wxT("invalid image index") );

    M_IMGDATA->m_alpha[y*w + x] = alpha;
}

unsigned char wxImage::GetAlpha(int x, int y) const
{
    wxCHECK_MSG( Ok() && HasAlpha(), 0, wxT("invalid image or no alpha channel") );

    int w = M_IMGDATA->m_width,
        h = M_IMGDATA->m_height;

    wxCHECK_MSG( x >=0 && y >= 0 && x < w && y < h, 0, wxT("invalid image index") );

    return M_IMGDATA->m_alpha[y*w + x];
}

bool wxImage::ConvertColourToAlpha( unsigned char r, unsigned char g, unsigned char b )
{
    SetAlpha( NULL );
    
    int w = M_IMGDATA->m_width,
        h = M_IMGDATA->m_height;
    
    unsigned char *alpha = GetAlpha();
    unsigned char *data = GetData();
    
    int x,y;
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            {
                *alpha = *data;
                alpha++;
                *data = r;
                data++;
                *data = g;
                data++;
                *data = b;
                data++;
            }

    return true;
}

void wxImage::SetAlpha( unsigned char *alpha )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    if ( !alpha )
    {
        alpha = (unsigned char *)malloc(M_IMGDATA->m_width*M_IMGDATA->m_height);
    }

    free(M_IMGDATA->m_alpha);
    M_IMGDATA->m_alpha = alpha;
}

unsigned char *wxImage::GetAlpha() const
{
    wxCHECK_MSG( Ok(), (unsigned char *)NULL, wxT("invalid image") );

    return M_IMGDATA->m_alpha;
}

// ----------------------------------------------------------------------------
// mask support
// ----------------------------------------------------------------------------

void wxImage::SetMaskColour( unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    M_IMGDATA->m_maskRed = r;
    M_IMGDATA->m_maskGreen = g;
    M_IMGDATA->m_maskBlue = b;
    M_IMGDATA->m_hasMask = true;
}

unsigned char wxImage::GetMaskRed() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return M_IMGDATA->m_maskRed;
}

unsigned char wxImage::GetMaskGreen() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return M_IMGDATA->m_maskGreen;
}

unsigned char wxImage::GetMaskBlue() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return M_IMGDATA->m_maskBlue;
}

void wxImage::SetMask( bool mask )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    M_IMGDATA->m_hasMask = mask;
}

bool wxImage::HasMask() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid image") );

    return M_IMGDATA->m_hasMask;
}

int wxImage::GetWidth() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return M_IMGDATA->m_width;
}

int wxImage::GetHeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return M_IMGDATA->m_height;
}

bool wxImage::SetMaskFromImage(const wxImage& mask,
                               unsigned char mr, unsigned char mg, unsigned char mb)
{
    // check that the images are the same size
    if ( (M_IMGDATA->m_height != mask.GetHeight() ) || (M_IMGDATA->m_width != mask.GetWidth () ) )
    {
        wxLogError( _("Image and mask have different sizes.") );
        return false;
    }

    // find unused colour
    unsigned char r,g,b ;
    if (!FindFirstUnusedColour(&r, &g, &b))
    {
        wxLogError( _("No unused colour in image being masked.") );
        return false ;
    }

    unsigned char *imgdata = GetData();
    unsigned char *maskdata = mask.GetData();

    const int w = GetWidth();
    const int h = GetHeight();

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            if ((maskdata[0] == mr) && (maskdata[1]  == mg) && (maskdata[2] == mb))
            {
                imgdata[0] = r;
                imgdata[1] = g;
                imgdata[2] = b;
            }
            imgdata  += 3;
            maskdata += 3;
        }
    }

    SetMaskColour(r, g, b);
    SetMask(true);

    return true;
}

bool wxImage::ConvertAlphaToMask(unsigned char threshold)
{
    if (!HasAlpha())
        return true;

    unsigned char mr, mg, mb;
    if (!FindFirstUnusedColour(&mr, &mg, &mb))
    {
        wxLogError( _("No unused colour in image being masked.") );
        return false;
    }

    SetMask(true);
    SetMaskColour(mr, mg, mb);

    unsigned char *imgdata = GetData();
    unsigned char *alphadata = GetAlpha();

    int w = GetWidth();
    int h = GetHeight();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++, imgdata += 3, alphadata++)
        {
            if (*alphadata < threshold)
            {
                imgdata[0] = mr;
                imgdata[1] = mg;
                imgdata[2] = mb;
            }
        }
    }

    free(M_IMGDATA->m_alpha);
    M_IMGDATA->m_alpha = NULL;

    return true;
}

#if wxUSE_PALETTE

// Palette functions

bool wxImage::HasPalette() const
{
    if (!Ok())
        return false;

    return M_IMGDATA->m_palette.Ok();
}

const wxPalette& wxImage::GetPalette() const
{
    wxCHECK_MSG( Ok(), wxNullPalette, wxT("invalid image") );

    return M_IMGDATA->m_palette;
}

void wxImage::SetPalette(const wxPalette& palette)
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    M_IMGDATA->m_palette = palette;
}

#endif // wxUSE_PALETTE

// Option functions (arbitrary name/value mapping)
void wxImage::SetOption(const wxString& name, const wxString& value)
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    int idx = M_IMGDATA->m_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
    {
        M_IMGDATA->m_optionNames.Add(name);
        M_IMGDATA->m_optionValues.Add(value);
    }
    else
    {
        M_IMGDATA->m_optionNames[idx] = name;
        M_IMGDATA->m_optionValues[idx] = value;
    }
}

void wxImage::SetOption(const wxString& name, int value)
{
    wxString valStr;
    valStr.Printf(wxT("%d"), value);
    SetOption(name, valStr);
}

wxString wxImage::GetOption(const wxString& name) const
{
    wxCHECK_MSG( Ok(), wxEmptyString, wxT("invalid image") );

    int idx = M_IMGDATA->m_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
        return wxEmptyString;
    else
        return M_IMGDATA->m_optionValues[idx];
}

int wxImage::GetOptionInt(const wxString& name) const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    return wxAtoi(GetOption(name));
}

bool wxImage::HasOption(const wxString& name) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid image") );

    return (M_IMGDATA->m_optionNames.Index(name, false) != wxNOT_FOUND);
}

bool wxImage::LoadFile( const wxString& filename, long type, int index )
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        wxBufferedInputStream bstream( stream );
        return LoadFile(bstream, type, index);
    }
    else
    {
        wxLogError( _("Can't load image from file '%s': file does not exist."), filename.c_str() );

        return false;
    }
#else // !wxUSE_STREAMS
    return false;
#endif // wxUSE_STREAMS
}

bool wxImage::LoadFile( const wxString& filename, const wxString& mimetype, int index )
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        wxBufferedInputStream bstream( stream );
        return LoadFile(bstream, mimetype, index);
    }
    else
    {
        wxLogError( _("Can't load image from file '%s': file does not exist."), filename.c_str() );

        return false;
    }
#else // !wxUSE_STREAMS
    return false;
#endif // wxUSE_STREAMS
}



bool wxImage::SaveFile( const wxString& filename ) const
{
    wxString ext = filename.AfterLast('.').Lower();

    wxImageHandler * pHandler = FindHandler(ext, -1);
    if (pHandler)
    {
        SaveFile(filename, pHandler->GetType());
        return true;
    }

    wxLogError(_("Can't save image to file '%s': unknown extension."), filename.c_str());

    return false;
}

bool wxImage::SaveFile( const wxString& filename, int type ) const
{
#if wxUSE_STREAMS
    ((wxImage*)this)->SetOption(wxIMAGE_OPTION_FILENAME, filename);

    wxFileOutputStream stream(filename);

    if ( stream.IsOk() )
    {
        wxBufferedOutputStream bstream( stream );
        return SaveFile(bstream, type);
    }
#endif // wxUSE_STREAMS

    return false;
}

bool wxImage::SaveFile( const wxString& filename, const wxString& mimetype ) const
{
#if wxUSE_STREAMS
    ((wxImage*)this)->SetOption(wxIMAGE_OPTION_FILENAME, filename);

    wxFileOutputStream stream(filename);

    if ( stream.IsOk() )
    {
        wxBufferedOutputStream bstream( stream );
        return SaveFile(bstream, mimetype);
    }
#endif // wxUSE_STREAMS

    return false;
}

bool wxImage::CanRead( const wxString &name )
{
#if wxUSE_STREAMS
  wxFileInputStream stream(name);
  return CanRead(stream);
#else
  return false;
#endif
}

int wxImage::GetImageCount( const wxString &name, long type )
{
#if wxUSE_STREAMS
  wxFileInputStream stream(name);
  if (stream.Ok())
      return GetImageCount(stream, type);
#endif

  return 0;
}

#if wxUSE_STREAMS

bool wxImage::CanRead( wxInputStream &stream )
{
    const wxList& list = GetHandlers();

    for ( wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext() )
    {
        wxImageHandler *handler=(wxImageHandler*)node->GetData();
        if (handler->CanRead( stream ))
            return true;
    }

    return false;
}

int wxImage::GetImageCount( wxInputStream &stream, long type )
{
    wxImageHandler *handler;

    if ( type == wxBITMAP_TYPE_ANY )
    {
        wxList &list=GetHandlers();

        for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
        {
             handler=(wxImageHandler*)node->GetData();
             if ( handler->CanRead(stream) )
                 return handler->GetImageCount(stream);

        }

        wxLogWarning(_("No handler found for image type."));
        return 0;
    }

    handler = FindHandler(type);

    if ( !handler )
    {
        wxLogWarning(_("No image handler for type %d defined."), type);
        return false;
    }

    if ( handler->CanRead(stream) )
    {
        return handler->GetImageCount(stream);
    }
    else
    {
        wxLogError(_("Image file is not of type %d."), type);
        return 0;
    }
}

bool wxImage::LoadFile( wxInputStream& stream, long type, int index )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler;

    if ( type == wxBITMAP_TYPE_ANY )
    {
        wxList &list=GetHandlers();

        for ( wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext() )
        {
             handler=(wxImageHandler*)node->GetData();
             if ( handler->CanRead(stream) )
                 return handler->LoadFile(this, stream, true/*verbose*/, index);

        }

        wxLogWarning( _("No handler found for image type.") );
        return false;
    }

    handler = FindHandler(type);

    if (handler == 0)
    {
        wxLogWarning( _("No image handler for type %d defined."), type );

        return false;
    }

    return handler->LoadFile(this, stream, true/*verbose*/, index);
}

bool wxImage::LoadFile( wxInputStream& stream, const wxString& mimetype, int index )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler = FindHandlerMime(mimetype);

    if (handler == 0)
    {
        wxLogWarning( _("No image handler for type %s defined."), mimetype.GetData() );

        return false;
    }

    return handler->LoadFile( this, stream, true/*verbose*/, index );
}

bool wxImage::SaveFile( wxOutputStream& stream, int type ) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid image") );

    wxImageHandler *handler = FindHandler(type);

    if (handler == 0)
    {
        wxLogWarning( _("No image handler for type %d defined."), type );

        return false;
    }

    return handler->SaveFile( (wxImage*)this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, const wxString& mimetype ) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid image") );

    wxImageHandler *handler = FindHandlerMime(mimetype);

    if (handler == 0)
    {
        wxLogWarning( _("No image handler for type %s defined."), mimetype.GetData() );

        return false;
    }

    return handler->SaveFile( (wxImage*)this, stream );
}
#endif // wxUSE_STREAMS

void wxImage::AddHandler( wxImageHandler *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == 0)
    {
        sm_handlers.Append( handler );
    }
    else
    {
        // This is not documented behaviour, merely the simplest 'fix'
        // for preventing duplicate additions.  If someone ever has
        // a good reason to add and remove duplicate handlers (and they
        // may) we should probably refcount the duplicates.
        //   also an issue in InsertHandler below.

        wxLogDebug( _T("Adding duplicate image handler for '%s'"),
                    handler->GetName().c_str() );
        delete handler;
    }
}

void wxImage::InsertHandler( wxImageHandler *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == 0)
    {
        sm_handlers.Insert( handler );
    }
    else
    {
        // see AddHandler for additional comments.
        wxLogDebug( _T("Inserting duplicate image handler for '%s'"),
                    handler->GetName().c_str() );
        delete handler;
    }
}

bool wxImage::RemoveHandler( const wxString& name )
{
    wxImageHandler *handler = FindHandler(name);
    if (handler)
    {
        sm_handlers.DeleteObject(handler);
        delete handler;
        return true;
    }
    else
        return false;
}

wxImageHandler *wxImage::FindHandler( const wxString& name )
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler*)node->GetData();
        if (handler->GetName().Cmp(name) == 0) return handler;

        node = node->GetNext();
    }
    return 0;
}

wxImageHandler *wxImage::FindHandler( const wxString& extension, long bitmapType )
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler*)node->GetData();
        if ( (handler->GetExtension().Cmp(extension) == 0) &&
            (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->GetNext();
    }
    return 0;
}

wxImageHandler *wxImage::FindHandler( long bitmapType )
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->GetData();
        if (handler->GetType() == bitmapType) return handler;
        node = node->GetNext();
    }
    return 0;
}

wxImageHandler *wxImage::FindHandlerMime( const wxString& mimetype )
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->GetData();
        if (handler->GetMimeType().IsSameAs(mimetype, false)) return handler;
        node = node->GetNext();
    }
    return 0;
}

void wxImage::InitStandardHandlers()
{
#if wxUSE_STREAMS
    AddHandler(new wxBMPHandler);
#endif // wxUSE_STREAMS
}

void wxImage::CleanUpHandlers()
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete handler;
        node = next;
    }

    sm_handlers.Clear();
}

wxString wxImage::GetImageExtWildcard()
{
    wxString fmts;

    wxList& Handlers = wxImage::GetHandlers();
    wxList::compatibility_iterator Node = Handlers.GetFirst();
    while ( Node )
    {
        wxImageHandler* Handler = (wxImageHandler*)Node->GetData();
        fmts += wxT("*.") + Handler->GetExtension();
        Node = Node->GetNext();
        if ( Node ) fmts += wxT(";");
    }

    return wxT("(") + fmts + wxT(")|") + fmts;
}

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxImageHandler,wxObject)

#if wxUSE_STREAMS
bool wxImageHandler::LoadFile( wxImage *WXUNUSED(image), wxInputStream& WXUNUSED(stream), bool WXUNUSED(verbose), int WXUNUSED(index) )
{
    return false;
}

bool wxImageHandler::SaveFile( wxImage *WXUNUSED(image), wxOutputStream& WXUNUSED(stream), bool WXUNUSED(verbose) )
{
    return false;
}

int wxImageHandler::GetImageCount( wxInputStream& WXUNUSED(stream) )
{
    return 1;
}

bool wxImageHandler::CanRead( const wxString& name )
{
    if (wxFileExists(name))
    {
        wxFileInputStream stream(name);
        return CanRead(stream);
    }

    wxLogError( _("Can't check image format of file '%s': file does not exist."), name.c_str() );

    return false;
}

bool wxImageHandler::CallDoCanRead(wxInputStream& stream)
{
    wxFileOffset posOld = stream.TellI();
    if ( posOld == wxInvalidOffset )
    {
        // can't test unseekable stream
        return false;
    }

    bool ok = DoCanRead(stream);

    // restore the old position to be able to test other formats and so on
    if ( stream.SeekI(posOld) == wxInvalidOffset )
    {
        wxLogDebug(_T("Failed to rewind the stream in wxImageHandler!"));

        // reading would fail anyhow as we're not at the right position
        return false;
    }

    return ok;
}

#endif // wxUSE_STREAMS

// ----------------------------------------------------------------------------
// image histogram stuff
// ----------------------------------------------------------------------------

bool
wxImageHistogram::FindFirstUnusedColour(unsigned char *r,
                                        unsigned char *g,
                                        unsigned char *b,
                                        unsigned char r2,
                                        unsigned char b2,
                                        unsigned char g2) const
{
    unsigned long key = MakeKey(r2, g2, b2);

    while ( find(key) != end() )
    {
        // color already used
        r2++;
        if ( r2 >= 255 )
        {
            r2 = 0;
            g2++;
            if ( g2 >= 255 )
            {
                g2 = 0;
                b2++;
                if ( b2 >= 255 )
                {
                    wxLogError(_("No unused colour in image.") );
                    return false;
                }
            }
        }

        key = MakeKey(r2, g2, b2);
    }

    if ( r )
        *r = r2;
    if ( g )
        *g = g2;
    if ( b )
        *b = b2;

    return true;
}

bool
wxImage::FindFirstUnusedColour(unsigned char *r,
                               unsigned char *g,
                               unsigned char *b,
                               unsigned char r2,
                               unsigned char b2,
                               unsigned char g2) const
{
    wxImageHistogram histogram;

    ComputeHistogram(histogram);

    return histogram.FindFirstUnusedColour(r, g, b, r2, g2, b2);
}



// GRG, Dic/99
// Counts and returns the number of different colours. Optionally stops
// when it exceeds 'stopafter' different colours. This is useful, for
// example, to see if the image can be saved as 8-bit (256 colour or
// less, in this case it would be invoked as CountColours(256)). Default
// value for stopafter is -1 (don't care).
//
unsigned long wxImage::CountColours( unsigned long stopafter ) const
{
    wxHashTable h;
    wxObject dummy;
    unsigned char r, g, b;
    unsigned char *p;
    unsigned long size, nentries, key;

    p = GetData();
    size = GetWidth() * GetHeight();
    nentries = 0;

    for (unsigned long j = 0; (j < size) && (nentries <= stopafter) ; j++)
    {
        r = *(p++);
        g = *(p++);
        b = *(p++);
        key = wxImageHistogram::MakeKey(r, g, b);

        if (h.Get(key) == NULL)
        {
            h.Put(key, &dummy);
            nentries++;
        }
    }

    return nentries;
}


unsigned long wxImage::ComputeHistogram( wxImageHistogram &h ) const
{
    unsigned char *p = GetData();
    unsigned long nentries = 0;

    h.clear();

    const unsigned long size = GetWidth() * GetHeight();

    unsigned char r, g, b;
    for ( unsigned long n = 0; n < size; n++ )
    {
        r = *p++;
        g = *p++;
        b = *p++;

        wxImageHistogramEntry& entry = h[wxImageHistogram::MakeKey(r, g, b)];

        if ( entry.value++ == 0 )
            entry.index = nentries++;
    }

    return nentries;
}

/*
 * Rotation code by Carlos Moreno
 */

// GRG: I've removed wxRotationPoint - we already have wxRealPoint which
//      does exactly the same thing. And I also got rid of wxRotationPixel
//      bacause of potential problems in architectures where alignment
//      is an issue, so I had to rewrite parts of the code.

static const double gs_Epsilon = 1e-10;

static inline int wxCint (double x)
{
    return (x > 0) ? (int) (x + 0.5) : (int) (x - 0.5);
}


// Auxiliary function to rotate a point (x,y) with respect to point p0
// make it inline and use a straight return to facilitate optimization
// also, the function receives the sine and cosine of the angle to avoid
// repeating the time-consuming calls to these functions -- sin/cos can
// be computed and stored in the calling function.

inline wxRealPoint rotated_point (const wxRealPoint & p, double cos_angle, double sin_angle, const wxRealPoint & p0)
{
    return wxRealPoint (p0.x + (p.x - p0.x) * cos_angle - (p.y - p0.y) * sin_angle,
                        p0.y + (p.y - p0.y) * cos_angle + (p.x - p0.x) * sin_angle);
}

inline wxRealPoint rotated_point (double x, double y, double cos_angle, double sin_angle, const wxRealPoint & p0)
{
    return rotated_point (wxRealPoint(x,y), cos_angle, sin_angle, p0);
}

wxImage wxImage::Rotate(double angle, const wxPoint & centre_of_rotation, bool interpolating, wxPoint * offset_after_rotation) const
{
    int i;
    angle = -angle;     // screen coordinates are a mirror image of "real" coordinates
    
    bool has_alpha = HasAlpha();

    // Create pointer-based array to accelerate access to wxImage's data
    unsigned char ** data = new unsigned char * [GetHeight()];
    data[0] = GetData();
    for (i = 1; i < GetHeight(); i++)
        data[i] = data[i - 1] + (3 * GetWidth());

    // Same for alpha channel    
    unsigned char ** alpha = NULL;
    if (has_alpha)
    {
        alpha = new unsigned char * [GetHeight()];
        alpha[0] = GetAlpha();
        for (i = 1; i < GetHeight(); i++)
            alpha[i] = alpha[i - 1] + GetWidth();
    }

    // precompute coefficients for rotation formula
    // (sine and cosine of the angle)
    const double cos_angle = cos(angle);
    const double sin_angle = sin(angle);

    // Create new Image to store the result
    // First, find rectangle that covers the rotated image;  to do that,
    // rotate the four corners

    const wxRealPoint p0(centre_of_rotation.x, centre_of_rotation.y);

    wxRealPoint p1 = rotated_point (0, 0, cos_angle, sin_angle, p0);
    wxRealPoint p2 = rotated_point (0, GetHeight(), cos_angle, sin_angle, p0);
    wxRealPoint p3 = rotated_point (GetWidth(), 0, cos_angle, sin_angle, p0);
    wxRealPoint p4 = rotated_point (GetWidth(), GetHeight(), cos_angle, sin_angle, p0);

    int x1 = (int) floor (wxMin (wxMin(p1.x, p2.x), wxMin(p3.x, p4.x)));
    int y1 = (int) floor (wxMin (wxMin(p1.y, p2.y), wxMin(p3.y, p4.y)));
    int x2 = (int) ceil (wxMax (wxMax(p1.x, p2.x), wxMax(p3.x, p4.x)));
    int y2 = (int) ceil (wxMax (wxMax(p1.y, p2.y), wxMax(p3.y, p4.y)));

    // Create rotated image
    wxImage rotated (x2 - x1 + 1, y2 - y1 + 1, false);
    // With alpha channel
    if (has_alpha)
        rotated.SetAlpha();

    if (offset_after_rotation != NULL)
    {
        *offset_after_rotation = wxPoint (x1, y1);
    }

    // GRG: The rotated (destination) image is always accessed
    //      sequentially, so there is no need for a pointer-based
    //      array here (and in fact it would be slower).
    //
    unsigned char * dst = rotated.GetData();
    
    unsigned char * alpha_dst = NULL;
    if (has_alpha)
        alpha_dst = rotated.GetAlpha();

    // GRG: if the original image has a mask, use its RGB values
    //      as the blank pixel, else, fall back to default (black).
    //
    unsigned char blank_r = 0;
    unsigned char blank_g = 0;
    unsigned char blank_b = 0;

    if (HasMask())
    {
        blank_r = GetMaskRed();
        blank_g = GetMaskGreen();
        blank_b = GetMaskBlue();
        rotated.SetMaskColour( blank_r, blank_g, blank_b );
    }

    // Now, for each point of the rotated image, find where it came from, by
    // performing an inverse rotation (a rotation of -angle) and getting the
    // pixel at those coordinates

    // GRG: I've taken the (interpolating) test out of the loops, so that
    //      it is done only once, instead of repeating it for each pixel.

    int x;
    if (interpolating)
    {
        for (int y = 0; y < rotated.GetHeight(); y++)
        {
            for (x = 0; x < rotated.GetWidth(); x++)
            {
                wxRealPoint src = rotated_point (x + x1, y + y1, cos_angle, -sin_angle, p0);

                if (-0.25 < src.x && src.x < GetWidth() - 0.75 &&
                    -0.25 < src.y && src.y < GetHeight() - 0.75)
                {
                    // interpolate using the 4 enclosing grid-points.  Those
                    // points can be obtained using floor and ceiling of the
                    // exact coordinates of the point
                        // C.M. 2000-02-17:  when the point is near the border, special care is required.

                    int x1, y1, x2, y2;

                    if (0 < src.x && src.x < GetWidth() - 1)
                    {
                        x1 = wxCint(floor(src.x));
                        x2 = wxCint(ceil(src.x));
                    }
                    else    // else means that x is near one of the borders (0 or width-1)
                    {
                        x1 = x2 = wxCint (src.x);
                    }

                    if (0 < src.y && src.y < GetHeight() - 1)
                    {
                        y1 = wxCint(floor(src.y));
                        y2 = wxCint(ceil(src.y));
                    }
                    else
                    {
                        y1 = y2 = wxCint (src.y);
                    }

                    // get four points and the distances (square of the distance,
                    // for efficiency reasons) for the interpolation formula

                    // GRG: Do not calculate the points until they are
                    //      really needed -- this way we can calculate
                    //      just one, instead of four, if d1, d2, d3
                    //      or d4 are < gs_Epsilon

                    const double d1 = (src.x - x1) * (src.x - x1) + (src.y - y1) * (src.y - y1);
                    const double d2 = (src.x - x2) * (src.x - x2) + (src.y - y1) * (src.y - y1);
                    const double d3 = (src.x - x2) * (src.x - x2) + (src.y - y2) * (src.y - y2);
                    const double d4 = (src.x - x1) * (src.x - x1) + (src.y - y2) * (src.y - y2);

                    // Now interpolate as a weighted average of the four surrounding
                    // points, where the weights are the distances to each of those points

                    // If the point is exactly at one point of the grid of the source
                    // image, then don't interpolate -- just assign the pixel

                    if (d1 < gs_Epsilon)        // d1,d2,d3,d4 are positive -- no need for abs()
                    {
                        unsigned char *p = data[y1] + (3 * x1);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *p;
                        
                        if (has_alpha)
                        {
                            unsigned char *p = alpha[y1] + x1;
                            *(alpha_dst++) = *p;
                        }
                    }
                    else if (d2 < gs_Epsilon)
                    {
                        unsigned char *p = data[y1] + (3 * x2);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *p;
                        
                        if (has_alpha)
                        {
                            unsigned char *p = alpha[y1] + x2;
                            *(alpha_dst++) = *p;
                        }
                    }
                    else if (d3 < gs_Epsilon)
                    {
                        unsigned char *p = data[y2] + (3 * x2);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *p;
                        
                        if (has_alpha)
                        {
                            unsigned char *p = alpha[y2] + x2;
                            *(alpha_dst++) = *p;
                        }
                    }
                    else if (d4 < gs_Epsilon)
                    {
                        unsigned char *p = data[y2] + (3 * x1);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *p;
                        
                        if (has_alpha)
                        {
                            unsigned char *p = alpha[y2] + x1;
                            *(alpha_dst++) = *p;
                        }
                    }
                    else
                    {
                        // weights for the weighted average are proportional to the inverse of the distance
                        unsigned char *v1 = data[y1] + (3 * x1);
                        unsigned char *v2 = data[y1] + (3 * x2);
                        unsigned char *v3 = data[y2] + (3 * x2);
                        unsigned char *v4 = data[y2] + (3 * x1);

                        const double w1 = 1/d1, w2 = 1/d2, w3 = 1/d3, w4 = 1/d4;

                        // GRG: Unrolled.

                        *(dst++) = (unsigned char)
                            ( (w1 * *(v1++) + w2 * *(v2++) +
                               w3 * *(v3++) + w4 * *(v4++)) /
                              (w1 + w2 + w3 + w4) );
                        *(dst++) = (unsigned char)
                            ( (w1 * *(v1++) + w2 * *(v2++) +
                               w3 * *(v3++) + w4 * *(v4++)) /
                              (w1 + w2 + w3 + w4) );
                        *(dst++) = (unsigned char)
                            ( (w1 * *v1 + w2 * *v2 +
                               w3 * *v3 + w4 * *v4) /
                              (w1 + w2 + w3 + w4) );
                              
                        if (has_alpha)
                        {
                            unsigned char *v1 = alpha[y1] + (x1);
                            unsigned char *v2 = alpha[y1] + (x2);
                            unsigned char *v3 = alpha[y2] + (x2);
                            unsigned char *v4 = alpha[y2] + (x1);

                            *(alpha_dst++) = (unsigned char)
                                ( (w1 * *v1 + w2 * *v2 +
                                   w3 * *v3 + w4 * *v4) /
                                  (w1 + w2 + w3 + w4) );
                        }
                    }
                }
                else
                {
                    *(dst++) = blank_r;
                    *(dst++) = blank_g;
                    *(dst++) = blank_b;
                   
                    if (has_alpha)
                        *(alpha_dst++) = 0;
                }
            }
        }
    }
    else    // not interpolating
    {
        for (int y = 0; y < rotated.GetHeight(); y++)
        {
            for (x = 0; x < rotated.GetWidth(); x++)
            {
                wxRealPoint src = rotated_point (x + x1, y + y1, cos_angle, -sin_angle, p0);

                const int xs = wxCint (src.x);      // wxCint rounds to the
                const int ys = wxCint (src.y);      // closest integer

                if (0 <= xs && xs < GetWidth() &&
                    0 <= ys && ys < GetHeight())
                {
                    unsigned char *p = data[ys] + (3 * xs);
                    *(dst++) = *(p++);
                    *(dst++) = *(p++);
                    *(dst++) = *p;
                    
                    if (has_alpha)
                    {
                        unsigned char *p = alpha[ys] + (xs);
                        *(alpha_dst++) = *p;
                    }
                }
                else
                {
                    *(dst++) = blank_r;
                    *(dst++) = blank_g;
                    *(dst++) = blank_b;
                    
                    if (has_alpha)
                        *(alpha_dst++) = 255;
                }
            }
        }
    }

    delete [] data;
    
    if (has_alpha)
        delete [] alpha;

    return rotated;
}





// A module to allow wxImage initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxImageModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxImageModule)
public:
    wxImageModule() {}
    bool OnInit() { wxImage::InitStandardHandlers(); return true; };
    void OnExit() { wxImage::CleanUpHandlers(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxImageModule, wxModule)


#endif // wxUSE_IMAGE
