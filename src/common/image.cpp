/////////////////////////////////////////////////////////////////////////////
// Name:        image.cpp
// Purpose:     wxImage
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "image.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/image.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

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
    ~wxImageRefData();

    int             m_width;
    int             m_height;
    unsigned char  *m_data;
    bool            m_hasMask;
    unsigned char   m_maskRed,m_maskGreen,m_maskBlue;
    bool            m_ok;
    bool            m_static;
    wxPalette       m_palette;
    wxArrayString   m_optionNames;
    wxArrayString   m_optionValues;
};

wxImageRefData::wxImageRefData()
{
    m_width = 0;
    m_height = 0;
    m_data = (unsigned char*) NULL;
    m_ok = FALSE;
    m_maskRed = 0;
    m_maskGreen = 0;
    m_maskBlue = 0;
    m_hasMask = FALSE;
    m_static = FALSE;
}

wxImageRefData::~wxImageRefData()
{
    if (m_data && !m_static)
        free( m_data );
}

wxList wxImage::sm_handlers;

wxImage wxNullImage;

//-----------------------------------------------------------------------------

#define M_IMGDATA ((wxImageRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxImage, wxObject)

wxImage::wxImage()
{
}

wxImage::wxImage( int width, int height )
{
    Create( width, height );
}

wxImage::wxImage( int width, int height, unsigned char* data, bool static_data )
{
    Create( width, height, data, static_data );
}

wxImage::wxImage( const wxString& name, long type )
{
    LoadFile( name, type );
}

wxImage::wxImage( const wxString& name, const wxString& mimetype )
{
    LoadFile( name, mimetype );
}

#if wxUSE_STREAMS
wxImage::wxImage( wxInputStream& stream, long type )
{
    LoadFile( stream, type );
}

wxImage::wxImage( wxInputStream& stream, const wxString& mimetype )
{
    LoadFile( stream, mimetype );
}
#endif // wxUSE_STREAMS

wxImage::wxImage( const wxImage& image )
{
    Ref(image);
}

wxImage::wxImage( const wxImage* image )
{
    if (image) Ref(*image);
}

void wxImage::Create( int width, int height )
{
    UnRef();

    m_refData = new wxImageRefData();

    M_IMGDATA->m_data = (unsigned char *) malloc( width*height*3 );
    if (M_IMGDATA->m_data)
    {
        for (int l = 0; l < width*height*3; l++) M_IMGDATA->m_data[l] = 0;

        M_IMGDATA->m_width = width;
        M_IMGDATA->m_height = height;
        M_IMGDATA->m_ok = TRUE;
    }
    else
    {
        UnRef();
    }
}

void wxImage::Create( int width, int height, unsigned char* data, bool static_data )
{
    UnRef();

    m_refData = new wxImageRefData();

    M_IMGDATA->m_data = data;
    if (M_IMGDATA->m_data)
    {
        M_IMGDATA->m_width = width;
        M_IMGDATA->m_height = height;
        M_IMGDATA->m_ok = TRUE;
        M_IMGDATA->m_static = static_data;
    }
    else
    {
        UnRef();
    }
}

void wxImage::Destroy()
{
    UnRef();
}

wxImage wxImage::Copy() const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    memcpy( data, GetData(), M_IMGDATA->m_width*M_IMGDATA->m_height*3 );

    return image;
}

wxImage wxImage::Scale( int width, int height ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    wxCHECK_MSG( (width > 0) && (height > 0), image, wxT("invalid image size") );

    image.Create( width, height );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    long old_height = M_IMGDATA->m_height;
    long old_width  = M_IMGDATA->m_width;

    char unsigned *source_data = M_IMGDATA->m_data;
    char unsigned *target_data = data;

    for (long j = 0; j < height; j++)
    {
        long y_offset = (j * old_height / height) * old_width;

        for (long i = 0; i < width; i++)
        {
            memcpy( target_data,
                source_data + 3*(y_offset + ((i * old_width )/ width)),
                3 );
            target_data += 3;
        }
    }

    return image;
}

wxImage wxImage::Rotate90( bool clockwise ) const
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_height, M_IMGDATA->m_width );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    long height = M_IMGDATA->m_height;
    long width  = M_IMGDATA->m_width;

    char unsigned *source_data = M_IMGDATA->m_data;
    char unsigned *target_data;

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

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, wxT("unable to create image") );

    if (M_IMGDATA->m_hasMask)
        image.SetMaskColour( M_IMGDATA->m_maskRed, M_IMGDATA->m_maskGreen, M_IMGDATA->m_maskBlue );

    long height = M_IMGDATA->m_height;
    long width  = M_IMGDATA->m_width;

    char unsigned *source_data = M_IMGDATA->m_data;
    char unsigned *target_data;

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

    image.Create( subwidth, subheight );

    char unsigned *subdata = image.GetData(), *data=GetData();

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

    char unsigned *data = GetData();

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

wxImage wxImage::ConvertToMono( unsigned char r, unsigned char g, unsigned char b )
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, wxT("invalid image") );

    image.Create( M_IMGDATA->m_width, M_IMGDATA->m_height );

    char unsigned *data = image.GetData();

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

    char unsigned *srcd = M_IMGDATA->m_data;
    char unsigned *tard = image.GetData();

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

char unsigned *wxImage::GetData() const
{
    wxCHECK_MSG( Ok(), (char unsigned *)NULL, wxT("invalid image") );

    return M_IMGDATA->m_data;
}

void wxImage::SetData( char unsigned *data )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    wxImageRefData *newRefData = new wxImageRefData();

    newRefData->m_width = M_IMGDATA->m_width;
    newRefData->m_height = M_IMGDATA->m_height;
    newRefData->m_data = data;
    newRefData->m_ok = TRUE;
    newRefData->m_maskRed = M_IMGDATA->m_maskRed;
    newRefData->m_maskGreen = M_IMGDATA->m_maskGreen;
    newRefData->m_maskBlue = M_IMGDATA->m_maskBlue;
    newRefData->m_hasMask = M_IMGDATA->m_hasMask;

    UnRef();

    m_refData = newRefData;
}

void wxImage::SetData( char unsigned *data, int new_width, int new_height )
{
    wxImageRefData *newRefData = new wxImageRefData();

    if (m_refData)
    {
        newRefData->m_width = new_width;
        newRefData->m_height = new_height;
        newRefData->m_data = data;
        newRefData->m_ok = TRUE;
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
        newRefData->m_ok = TRUE;
    }

    UnRef();

    m_refData = newRefData;
}

void wxImage::SetMaskColour( unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    M_IMGDATA->m_maskRed = r;
    M_IMGDATA->m_maskGreen = g;
    M_IMGDATA->m_maskBlue = b;
    M_IMGDATA->m_hasMask = TRUE;
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
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid image") );

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

// Palette functions

bool wxImage::HasPalette() const
{
    if (!Ok())
        return FALSE;

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

// Option functions (arbitrary name/value mapping)
void wxImage::SetOption(const wxString& name, const wxString& value)
{
    wxCHECK_RET( Ok(), wxT("invalid image") );

    int idx = M_IMGDATA->m_optionNames.Index(name, FALSE);
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

    int idx = M_IMGDATA->m_optionNames.Index(name, FALSE);
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
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid image") );

    return (M_IMGDATA->m_optionNames.Index(name, FALSE) != wxNOT_FOUND);
}

bool wxImage::LoadFile( const wxString& filename, long type )
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        wxBufferedInputStream bstream( stream );
        return LoadFile(bstream, type);
    }
    else
    {
        wxLogError( _("Can't load image from file '%s': file does not exist."), filename.c_str() );

        return FALSE;
    }
#else // !wxUSE_STREAMS
    return FALSE;
#endif // wxUSE_STREAMS
}

bool wxImage::LoadFile( const wxString& filename, const wxString& mimetype )
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        wxBufferedInputStream bstream( stream );
        return LoadFile(bstream, mimetype);
    }
    else
    {
        wxLogError( _("Can't load image from file '%s': file does not exist."), filename.c_str() );

        return FALSE;
    }
#else // !wxUSE_STREAMS
    return FALSE;
#endif // wxUSE_STREAMS
}

bool wxImage::SaveFile( const wxString& filename, int type )
{
#if wxUSE_STREAMS
    wxFileOutputStream stream(filename);

    if ( stream.LastError() == wxStream_NOERROR )
    {
        wxBufferedOutputStream bstream( stream );
        return SaveFile(bstream, type);
    }
#endif // wxUSE_STREAMS

    return FALSE;
}

bool wxImage::SaveFile( const wxString& filename, const wxString& mimetype )
{
#if wxUSE_STREAMS
    wxFileOutputStream stream(filename);

    if ( stream.LastError() == wxStream_NOERROR )
    {
        wxBufferedOutputStream bstream( stream );
        return SaveFile(bstream, mimetype);
    }
#endif // wxUSE_STREAMS

    return FALSE;
}

bool wxImage::CanRead( const wxString &name )
{
#if wxUSE_STREAMS
  wxFileInputStream stream(name);
  return CanRead(stream);
#else
  return FALSE;
#endif
}

#if wxUSE_STREAMS

bool wxImage::CanRead( wxInputStream &stream )
{
  wxList &list=GetHandlers();

  for ( wxList::Node *node = list.GetFirst(); node; node = node->GetNext() )
    {
      wxImageHandler *handler=(wxImageHandler*)node->GetData();
      if (handler->CanRead( stream ))
        return TRUE;
    }

  return FALSE;
}

bool wxImage::LoadFile( wxInputStream& stream, long type )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler;

    if (type==wxBITMAP_TYPE_ANY)
    {
        wxList &list=GetHandlers();

        for ( wxList::Node *node = list.GetFirst(); node; node = node->GetNext() )
        {
             handler=(wxImageHandler*)node->GetData();
             if (handler->CanRead( stream ))
                 return handler->LoadFile( this, stream );

        }

        wxLogWarning( _("No handler found for image type.") );
        return FALSE;
    }

    handler = FindHandler(type);

    if (handler == NULL)
    {
        wxLogWarning( _("No image handler for type %d defined."), type );

        return FALSE;
    }

    return handler->LoadFile( this, stream );
}

bool wxImage::LoadFile( wxInputStream& stream, const wxString& mimetype )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler = FindHandlerMime(mimetype);

    if (handler == NULL)
    {
        wxLogWarning( _("No image handler for type %s defined."), mimetype.GetData() );

        return FALSE;
    }

    return handler->LoadFile( this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, int type )
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid image") );

    wxImageHandler *handler = FindHandler(type);

    if (handler == NULL)
    {
        wxLogWarning( _("No image handler for type %d defined."), type );

        return FALSE;
    }

    return handler->SaveFile( this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, const wxString& mimetype )
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid image") );

    wxImageHandler *handler = FindHandlerMime(mimetype);

    if (handler == NULL)
    {
        wxLogWarning( _("No image handler for type %s defined."), mimetype.GetData() );

        return FALSE;
    }

    return handler->SaveFile( this, stream );
}
#endif // wxUSE_STREAMS

void wxImage::AddHandler( wxImageHandler *handler )
{
    // make sure that the memory will be freed at the program end
    sm_handlers.DeleteContents(TRUE);

    sm_handlers.Append( handler );
}

void wxImage::InsertHandler( wxImageHandler *handler )
{
    // make sure that the memory will be freed at the program end
    sm_handlers.DeleteContents(TRUE);

    sm_handlers.Insert( handler );
}

bool wxImage::RemoveHandler( const wxString& name )
{
    wxImageHandler *handler = FindHandler(name);
    if (handler)
    {
        sm_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxImageHandler *wxImage::FindHandler( const wxString& name )
{
    wxNode *node = sm_handlers.First();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler*)node->Data();
        if (handler->GetName().Cmp(name) == 0) return handler;

        node = node->Next();
    }
    return (wxImageHandler *)NULL;
}

wxImageHandler *wxImage::FindHandler( const wxString& extension, long bitmapType )
{
    wxNode *node = sm_handlers.First();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler*)node->Data();
        if ( (handler->GetExtension().Cmp(extension) == 0) &&
            (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->Next();
    }
    return (wxImageHandler*)NULL;
}

wxImageHandler *wxImage::FindHandler( long bitmapType )
{
    wxNode *node = sm_handlers.First();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->Data();
        if (handler->GetType() == bitmapType) return handler;
        node = node->Next();
    }
    return NULL;
}

wxImageHandler *wxImage::FindHandlerMime( const wxString& mimetype )
{
    wxNode *node = sm_handlers.First();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->Data();
        if (handler->GetMimeType().IsSameAs(mimetype, FALSE)) return handler;
        node = node->Next();
    }
    return NULL;
}

void wxImage::InitStandardHandlers()
{
  AddHandler( new wxBMPHandler );
}

void wxImage::CleanUpHandlers()
{
    wxNode *node = sm_handlers.First();
    while (node)
    {
        wxImageHandler *handler = (wxImageHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxImageHandler,wxObject)

#if wxUSE_STREAMS
bool wxImageHandler::LoadFile( wxImage *WXUNUSED(image), wxInputStream& WXUNUSED(stream), bool WXUNUSED(verbose), int WXUNUSED(index) )
{
    return FALSE;
}

bool wxImageHandler::SaveFile( wxImage *WXUNUSED(image), wxOutputStream& WXUNUSED(stream), bool WXUNUSED(verbose) )
{
    return FALSE;
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

    else {
        wxLogError( _("Can't check image format of file '%s': file does not exist."), name.c_str() );

        return FALSE;
    }
//    return FALSE;
}

#endif // wxUSE_STREAMS



//-----------------------------------------------------------------------------
// wxBitmap convertion routines
//-----------------------------------------------------------------------------

#if wxUSE_GUI

#ifdef __WXGTK__
wxBitmap wxImage::ConvertToMonoBitmap( unsigned char red, unsigned char green, unsigned char blue )
{
    wxImage mono = this->ConvertToMono( red, green, blue );
    wxBitmap bitmap( mono, 1 );
    return bitmap;
}
#endif

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap( *this );
    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    *this = bitmap.ConvertToImage();
}

#endif



// A module to allow wxImage initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxImageModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxImageModule)
public:
    wxImageModule() {}
    bool OnInit() { wxImage::InitStandardHandlers(); return TRUE; };
    void OnExit() { wxImage::CleanUpHandlers(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxImageModule, wxModule)


//-----------------------------------------------------------------------------

// GRG, Dic/99
// Counts and returns the number of different colours. Optionally stops
// when it exceeds 'stopafter' different colours. This is useful, for
// example, to see if the image can be saved as 8-bit (256 colour or
// less, in this case it would be invoked as CountColours(256)). Default
// value for stopafter is -1 (don't care).
//
unsigned long wxImage::CountColours( unsigned long stopafter )
{
    wxHashTable h;
    wxObject dummy;
    unsigned char r, g, b, *p;
    unsigned long size, nentries, key;

    p = GetData();
    size = GetWidth() * GetHeight();
    nentries = 0;

    for (unsigned long j = 0; (j < size) && (nentries <= stopafter) ; j++)
    {
        r = *(p++);
        g = *(p++);
        b = *(p++);
        key = (r << 16) | (g << 8) | b;

        if (h.Get(key) == NULL)
        {
            h.Put(key, &dummy);
            nentries++;
        }
    }

    return nentries;
}


// GRG, Dic/99
// Computes the histogram of the image and fills a hash table, indexed
// with integer keys built as 0xRRGGBB, containing wxHNode objects. Each
// wxHNode contains an 'index' (useful to build a palette with the image
// colours) and a 'value', which is the number of pixels in the image with
// that colour.
//
unsigned long wxImage::ComputeHistogram( wxHashTable &h )
{
    unsigned char r, g, b, *p;
    unsigned long size, nentries, key;
    wxHNode *hnode;

    p = GetData();
    size = GetWidth() * GetHeight();
    nentries = 0;

    for (unsigned long j = 0; j < size; j++)
    {
        r = *(p++);
        g = *(p++);
        b = *(p++);
        key = (r << 16) | (g << 8) | b;

        hnode = (wxHNode *) h.Get(key);

        if (hnode)
            hnode->value++;
        else
        {
            hnode = new wxHNode();
            hnode->index = nentries++;
            hnode->value = 1;

            h.Put(key, (wxObject *)hnode);
        }
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

    // Create pointer-based array to accelerate access to wxImage's data
    unsigned char ** data = new unsigned char * [GetHeight()];

    data[0] = GetData();

    for (i = 1; i < GetHeight(); i++)
        data[i] = data[i - 1] + (3 * GetWidth());

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

    wxImage rotated (x2 - x1 + 1, y2 - y1 + 1);

    if (offset_after_rotation != NULL)
    {
        *offset_after_rotation = wxPoint (x1, y1);
    }

    // GRG: The rotated (destination) image is always accessed
    //      sequentially, so there is no need for a pointer-based
    //      array here (and in fact it would be slower).
    //
    unsigned char * dst = rotated.GetData();

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
                        *(dst++) = *(p++);
                    }
                    else if (d2 < gs_Epsilon)
                    {
                        unsigned char *p = data[y1] + (3 * x2);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                    }
                    else if (d3 < gs_Epsilon)
                    {
                        unsigned char *p = data[y2] + (3 * x2);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                    }
                    else if (d4 < gs_Epsilon)
                    {
                        unsigned char *p = data[y2] + (3 * x1);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
                        *(dst++) = *(p++);
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
                            ( (w1 * *(v1++) + w2 * *(v2++) +
                               w3 * *(v3++) + w4 * *(v4++)) /
                              (w1 + w2 + w3 + w4) );
                    }
                }
                else
                {
                    *(dst++) = blank_r;
                    *(dst++) = blank_g;
                    *(dst++) = blank_b;
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
                    *(dst++) = *(p++);
                }
                else
                {
                    *(dst++) = blank_r;
                    *(dst++) = blank_g;
                    *(dst++) = blank_b;
                }
            }
        }
    }

    delete [] data;

    return rotated;
}

