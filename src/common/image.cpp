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

#ifdef __SALFORDC__
    #undef FAR
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
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
}

wxImageRefData::~wxImageRefData()
{
    if (m_data)
        free( m_data );
}

wxList wxImage::sm_handlers;

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

void wxImage::Destroy()
{
    UnRef();
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

unsigned char wxImage::GetRed( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos];
}

unsigned char wxImage::GetGreen( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, wxT("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+1];
}

unsigned char wxImage::GetBlue( int x, int y )
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
    return (M_IMGDATA && M_IMGDATA->m_ok);
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
    else
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
    else
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
// MSW conversion routines
//-----------------------------------------------------------------------------

#ifdef __WXMSW__

wxBitmap wxImage::ConvertToBitmap() const
{
    if ( !Ok() )
        return wxNullBitmap;

    // sizeLimit is the MS upper limit for the DIB size
#ifdef  WIN32
    int sizeLimit = 1024*768*3;
#else
    int sizeLimit = 0x7fff ;
#endif

    // width and height of the device-dependent bitmap
    int width = GetWidth();
    int bmpHeight = GetHeight();

    // calc the number of bytes per scanline and padding
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary = bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
    // calc the number of DIBs and heights of DIBs
    int numDIB = 1;
    int hRemain = 0;
    int height = sizeLimit/bytePerLine;
    if( height >= bmpHeight )
        height = bmpHeight;
    else
    {
        numDIB =  bmpHeight / height;
        hRemain = bmpHeight % height;
        if( hRemain >0 )  numDIB++;
    }

    // set bitmap parameters
    wxBitmap bitmap;
    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );
    bitmap.SetWidth( width );
    bitmap.SetHeight( bmpHeight );
    bitmap.SetDepth( wxDisplayDepth() );

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    BITMAPINFO *lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, bitmap, wxT("could not allocate memory for DIB header") );
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = (DWORD)width;
    lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
    lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
    //   the general formula for biSizeImage:
    //      ( ( ( ((DWORD)width*24) +31 ) & ~31 ) >> 3 ) * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *)malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate memory for DIB") );
        free( lpDIBh );
        return bitmap;
    }

    // create and set the device-dependent bitmap
    HDC hdc = ::GetDC(NULL);
    HDC memdc = ::CreateCompatibleDC( hdc );
    HBITMAP hbitmap;
    hbitmap = ::CreateCompatibleBitmap( hdc, width, bmpHeight );
    ::SelectObject( memdc, hbitmap);

    // copy image data into DIB data and then into DDB (in a loop)
    unsigned char *data = GetData();
    int i, j, n;
    int origin = 0;
    unsigned char *ptdata = data;
    unsigned char *ptbits;

    for( n=0; n<numDIB; n++ )
    {
        if( numDIB > 1 && n == numDIB-1 && hRemain > 0 )
        {
            // redefine height and size of the (possibly) last smaller DIB
            // memory is not reallocated
            height = hRemain;
            lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
            lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        }
        ptbits = lpBits;

        for( j=0; j<height; j++ )
        {
            for( i=0; i<width; i++ )
            {
                *(ptbits++) = *(ptdata+2);
                *(ptbits++) = *(ptdata+1);
                *(ptbits++) = *(ptdata  );
                ptdata += 3;
            }
            for( i=0; i< padding; i++ )   *(ptbits++) = 0;
        }
        ::StretchDIBits( memdc, 0, origin, width, height,\
            0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
        origin += height;
        // if numDIB = 1,  lines below can also be used
        //    hbitmap = CreateDIBitmap( hdc, &(lpDIBh->bmiHeader), CBM_INIT, lpBits, lpDIBh, DIB_RGB_COLORS );
        // The above line is equivalent to the following two lines.
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    ::SetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS);
        // or the following lines
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    HDC memdc = ::CreateCompatibleDC( hdc );
        //    ::SelectObject( memdc, hbitmap);
        //    ::SetDIBitsToDevice( memdc, 0, 0, width, height,
        //            0, 0, 0, height, (void *)lpBits, lpDIBh, DIB_RGB_COLORS);
        //    ::SelectObject( memdc, 0 );
        //    ::DeleteDC( memdc );
    }
    bitmap.SetHBITMAP( (WXHBITMAP) hbitmap );

    // similarly, created an mono-bitmap for the possible mask
    if( HasMask() )
    {
        hbitmap = ::CreateBitmap( (WORD)width, (WORD)bmpHeight, 1, 1, NULL );
        ::SelectObject( memdc, hbitmap);
        if( numDIB == 1 )   height = bmpHeight;
        else                height = sizeLimit/bytePerLine;
        lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
        lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        origin = 0;
        unsigned char r = GetMaskRed();
        unsigned char g = GetMaskGreen();
        unsigned char b = GetMaskBlue();
        unsigned char zero = 0, one = 255;
        ptdata = data;
        for( n=0; n<numDIB; n++ )
        {
            if( numDIB > 1 && n == numDIB - 1 && hRemain > 0 )
            {
                // redefine height and size of the (possibly) last smaller DIB
                // memory is not reallocated
                height = hRemain;
                lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
                lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
            }
            ptbits = lpBits;
            for( int j=0; j<height; j++ )
            {
                for(i=0; i<width; i++ )
                {
                    // was causing a code gen bug in cw : if( ( cr !=r) || (cg!=g) || (cb!=b) )
                        unsigned char cr = (*(ptdata++)) ;
                        unsigned char cg = (*(ptdata++)) ;
                        unsigned char cb = (*(ptdata++)) ;
                        
                    if( ( cr !=r) || (cg!=g) || (cb!=b) )
                    {
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                    }
                    else
                    {
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                    }
                }
                for( i=0; i< padding; i++ )   *(ptbits++) = zero;
            }
            ::StretchDIBits( memdc, 0, origin, width, height,\
                0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
            origin += height;
        }
        // create a wxMask object
        wxMask *mask = new wxMask();
        mask->SetMaskBitmap( (WXHBITMAP) hbitmap );
        bitmap.SetMask( mask );
        // It will be deleted when the wxBitmap object is deleted (as of 01/1999)
        /* The following can also be used but is slow to run
        wxColour colour( GetMaskRed(), GetMaskGreen(), GetMaskBlue());
        wxMask *mask = new wxMask( bitmap, colour );
        bitmap.SetMask( mask );
        */
    }

    // free allocated resources
    ::SelectObject( memdc, 0 );
    ::DeleteDC( memdc );
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);

#if WXWIN_COMPATIBILITY_2
    // check the wxBitmap object
    bitmap.GetBitmapData()->SetOk();
#endif // WXWIN_COMPATIBILITY_2

    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    // check the bitmap
    if( !bitmap.Ok() )
    {
        wxFAIL_MSG( wxT("invalid bitmap") );
        return;
    }

    // create an wxImage object
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    Create( width, height );
    unsigned char *data = GetData();
    if( !data )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return;
    }

    // calc the number of bytes per scanline and padding in the DIB
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary =  bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    BITMAPINFO *lpDIBh = (BITMAPINFO *) malloc( headersize );
    if( !lpDIBh )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB header") );
        free( data );
        return;
    }
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = width;
    lpDIBh->bmiHeader.biHeight = -height;
    lpDIBh->bmiHeader.biSizeImage = bytePerLine * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *) malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB") );
        free( data );
        free( lpDIBh );
        return;
    }

    // copy data from the device-dependent bitmap to the DIB
    HDC hdc = ::GetDC(NULL);
    HBITMAP hbitmap;
    hbitmap = (HBITMAP) bitmap.GetHBITMAP();
    ::GetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );

    // copy DIB data into the wxImage object
    int i, j;
    unsigned char *ptdata = data;
    unsigned char *ptbits = lpBits;
    for( i=0; i<height; i++ )
    {
        for( j=0; j<width; j++ )
        {
            *(ptdata++) = *(ptbits+2);
            *(ptdata++) = *(ptbits+1);
            *(ptdata++) = *(ptbits  );
            ptbits += 3;
        }
        ptbits += padding;
    }

    // similarly, set data according to the possible mask bitmap
    if( bitmap.GetMask() && bitmap.GetMask()->GetMaskBitmap() )
    {
        hbitmap = (HBITMAP) bitmap.GetMask()->GetMaskBitmap();
        // memory DC created, color set, data copied, and memory DC deleted
        HDC memdc = ::CreateCompatibleDC( hdc );
        ::SetTextColor( memdc, RGB( 0, 0, 0 ) );
        ::SetBkColor( memdc, RGB( 255, 255, 255 ) );
        ::GetDIBits( memdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );
        ::DeleteDC( memdc );
        // background color set to RGB(16,16,16) in consistent with wxGTK
        unsigned char r=16, g=16, b=16;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                if( *ptbits != 0 )
                    ptdata += 3;
                else
                {
                    *(ptdata++)  = r;
                    *(ptdata++)  = g;
                    *(ptdata++)  = b;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }
        SetMaskColour( r, g, b );
        SetMask( TRUE );
    }
    else
    {
        SetMask( FALSE );
    }
    // free allocated resources
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);
}

#endif

#ifdef __WXMAC__

#include <PictUtils.h>

extern CTabHandle wxMacCreateColorTable( int numColors ) ;
extern void wxMacDestroyColorTable( CTabHandle colors ) ;
extern void wxMacSetColorTableEntry( CTabHandle newColors , int index , int red , int green ,  int blue ) ;
extern GWorldPtr wxMacCreateGWorld( int height , int width , int depth ) ;
extern void wxMacDestroyGWorld( GWorldPtr gw ) ;

wxBitmap wxImage::ConvertToBitmap() const
{
    // width and height of the device-dependent bitmap
    int width = GetWidth();
    int height = GetHeight();

    // Create picture

    wxBitmap bitmap( width , height , wxDisplayDepth() ) ;

    // Create mask

    if (HasMask())
    {
            /*
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

        mask_image =  gdk_image_new_bitmap( gdk_visual_get_system(), mask_data, width, height );

        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, 1 );

        bitmap.SetMask( mask );
       */
    }

    // Render

    int r_mask = GetMaskRed();
    int g_mask = GetMaskGreen();
    int b_mask = GetMaskBlue();

                CGrafPtr         origPort ;
                GDHandle        origDevice ;
                
                GetGWorld( &origPort , &origDevice ) ;
                SetGWorld( bitmap.GetHBITMAP() , NULL ) ;

    register unsigned char* data = GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
                    unsigned char r = data[index++];
                    unsigned char g = data[index++];
                    unsigned char b = data[index++];
                       RGBColor color ;
                       color.red = ( r  << 8 ) + r ;
                       color.green = ( g << 8 ) + g ;
                       color.blue = ( b << 8 ) + b ;
                       SetCPixel( x , y , &color ) ;
               }
    }  // for height

           SetGWorld( origPort , origDevice ) ;

    if ( HasMask() )
    {
        wxColour colour( GetMaskRed(), GetMaskGreen(), GetMaskBlue());
        wxMask *mask = new wxMask( bitmap, colour );
        bitmap.SetMask( mask );
    }
    return bitmap;

}

wxImage::wxImage( const wxBitmap &bitmap )
{
    // check the bitmap
    if( !bitmap.Ok() )
    {
        wxFAIL_MSG( "invalid bitmap" );
        return;
    }

    // create an wxImage object
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    Create( width, height );
    /*
    unsigned char *data = GetData();
    if( !data )
    {
        wxFAIL_MSG( "could not allocate data for image" );
        return;
    }

    // calc the number of bytes per scanline and padding in the DIB
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    div_t lineBoundary = div( bytePerLine, sizeDWORD );
    int padding = 0;
    if( lineBoundary.rem > 0 )
    {
        padding = sizeDWORD - lineBoundary.rem;
        bytePerLine += padding;
    }

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    if( !lpDIBh )
    {
        wxFAIL_MSG( "could not allocate data for DIB header" );
        free( data );
        return;
    }
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = width;
    lpDIBh->bmiHeader.biHeight = -height;
    lpDIBh->bmiHeader.biSizeImage = bytePerLine * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *) malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( "could not allocate data for DIB" );
        free( data );
        free( lpDIBh );
        return;
    }

    // copy data from the device-dependent bitmap to the DIB
    HDC hdc = ::GetDC(NULL);
    HBITMAP hbitmap;
    hbitmap = (HBITMAP) bitmap.GetHBITMAP();
    ::GetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );

    // copy DIB data into the wxImage object
    int i, j;
    unsigned char *ptdata = data;
    unsigned char *ptbits = lpBits;
    for( i=0; i<height; i++ )
    {
        for( j=0; j<width; j++ )
        {
            *(ptdata++) = *(ptbits+2);
            *(ptdata++) = *(ptbits+1);
            *(ptdata++) = *(ptbits  );
            ptbits += 3;
        }
        ptbits += padding;
    }        

    // similarly, set data according to the possible mask bitmap
    if( bitmap.GetMask() && bitmap.GetMask()->GetMaskBitmap() )
    {
        hbitmap = (HBITMAP) bitmap.GetMask()->GetMaskBitmap();
        // memory DC created, color set, data copied, and memory DC deleted
        HDC memdc = ::CreateCompatibleDC( hdc );
        ::SetTextColor( memdc, RGB( 0, 0, 0 ) );
        ::SetBkColor( memdc, RGB( 255, 255, 255 ) );
        ::GetDIBits( memdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );
        ::DeleteDC( memdc );
        // background color set to RGB(16,16,16) in consistent with wxGTK
        unsigned char r=16, g=16, b=16;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                if( *ptbits != 0 )
                    ptdata += 3;
                else
                {
                    *(ptdata++)  = r;
                    *(ptdata++)  = g;
                    *(ptdata++)  = b;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }
        SetMaskColour( r, g, b );
        SetMask( TRUE );
    }
    else
    {
        SetMask( FALSE );
    }
    // free allocated resources
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);
    */
}

#endif

//-----------------------------------------------------------------------------
// GTK conversion routines
//-----------------------------------------------------------------------------

#ifdef __WXGTK__

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#if (GTK_MINOR_VERSION > 0)
#include <gdk/gdkrgb.h>
#endif

extern GtkWidget *wxRootWindow;

wxBitmap wxImage::ConvertToMonoBitmap( unsigned char red, unsigned char green, unsigned char blue )
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    bitmap.SetBitmap( gdk_pixmap_new( wxRootWindow->window, width, height, 1 ) );
    
    bitmap.SetDepth( 1 );

    // Create picture image

    unsigned char *data_data = (unsigned char*)malloc( ((width >> 3)+8) * height );
    
    GdkImage *data_image =
        gdk_image_new_bitmap( gdk_visual_get_system(), data_data, width, height );

    // Create mask image

    GdkImage *mask_image = (GdkImage*) NULL;

    if (HasMask())
    {
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

        mask_image =  gdk_image_new_bitmap( gdk_visual_get_system(), mask_data, width, height );

        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( wxRootWindow->window, width, height, 1 );

        bitmap.SetMask( mask );
    }

    int r_mask = GetMaskRed();
    int g_mask = GetMaskGreen();
    int b_mask = GetMaskBlue();

    unsigned char* data = GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            if (HasMask())
            {
                if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                    gdk_image_put_pixel( mask_image, x, y, 1 );
                else
                    gdk_image_put_pixel( mask_image, x, y, 0 );
            }
            
            if ((r == red) && (b == blue) && (g == green))
                gdk_image_put_pixel( data_image, x, y, 1 );
            else   
                gdk_image_put_pixel( data_image, x, y, 0 );

        } // for
    }  // for

    // Blit picture

    GdkGC *data_gc = gdk_gc_new( bitmap.GetBitmap() );

    gdk_draw_image( bitmap.GetBitmap(), data_gc, data_image, 0, 0, 0, 0, width, height );

    gdk_image_destroy( data_image );
    gdk_gc_unref( data_gc );

    // Blit mask

    if (HasMask())
    {
        GdkGC *mask_gc = gdk_gc_new( bitmap.GetMask()->GetBitmap() );

        gdk_draw_image( bitmap.GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

        gdk_image_destroy( mask_image );
        gdk_gc_unref( mask_gc );
    }

    return bitmap;
}


wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    bitmap.SetPixmap( gdk_pixmap_new( wxRootWindow->window, width, height, -1 ) );

     // Retrieve depth

    GdkVisual *visual = gdk_window_get_visual( bitmap.GetPixmap() );
    if (visual == NULL) visual = gdk_visual_get_system();
    int bpp = visual->depth;

    bitmap.SetDepth( bpp );

    if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;
    if (bpp < 8) bpp = 8;

#if (GTK_MINOR_VERSION > 0)

    if (!HasMask() && (bpp > 8))
    {
        static bool s_hasInitialized = FALSE;

        if (!s_hasInitialized)
        {
            gdk_rgb_init();
            s_hasInitialized = TRUE;
        }

        GdkGC *gc = gdk_gc_new( bitmap.GetPixmap() );

        gdk_draw_rgb_image( bitmap.GetPixmap(),
                            gc,
                            0, 0,
                            width, height,
                            GDK_RGB_DITHER_NONE,
                            GetData(),
                            width*3 );

        gdk_gc_unref( gc );

        return bitmap;
    }

#endif

    // Create picture image

    GdkImage *data_image =
        gdk_image_new( GDK_IMAGE_FASTEST, gdk_visual_get_system(), width, height );

    // Create mask image

    GdkImage *mask_image = (GdkImage*) NULL;

    if (HasMask())
    {
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

        mask_image =  gdk_image_new_bitmap( gdk_visual_get_system(), mask_data, width, height );

        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( wxRootWindow->window, width, height, 1 );

        bitmap.SetMask( mask );
    }

    // Render

    enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
    byte_order b_o = RGB;

    if (bpp >= 24)
    {
        GdkVisual *visual = gdk_visual_get_system();
        if ((visual->red_mask > visual->green_mask) && (visual->green_mask > visual->blue_mask))      b_o = RGB;
        else if ((visual->red_mask > visual->blue_mask) && (visual->blue_mask > visual->green_mask))  b_o = RGB;
        else if ((visual->blue_mask > visual->red_mask) && (visual->red_mask > visual->green_mask))   b_o = BRG;
        else if ((visual->blue_mask > visual->green_mask) && (visual->green_mask > visual->red_mask)) b_o = BGR;
        else if ((visual->green_mask > visual->red_mask) && (visual->red_mask > visual->blue_mask))   b_o = GRB;
        else if ((visual->green_mask > visual->blue_mask) && (visual->blue_mask > visual->red_mask))  b_o = GBR;
    }

    int r_mask = GetMaskRed();
    int g_mask = GetMaskGreen();
    int b_mask = GetMaskBlue();

    unsigned char* data = GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            if (HasMask())
            {
                if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                    gdk_image_put_pixel( mask_image, x, y, 1 );
                else
                    gdk_image_put_pixel( mask_image, x, y, 0 );
            }

            switch (bpp)
            {
            case 8:
                {
                    int pixel = -1;
                    if (wxTheApp->m_colorCube)
                    {
                        pixel = wxTheApp->m_colorCube[ ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + ((b & 0xf8) >> 3) ];
                    }
                    else
                    {
                        GdkColormap *cmap = gtk_widget_get_default_colormap();
                        GdkColor *colors = cmap->colors;
                        int max = 3 * (65536);

                        for (int i = 0; i < cmap->size; i++)
                        {
                            int rdiff = (r << 8) - colors[i].red;
                            int gdiff = (g << 8) - colors[i].green;
                            int bdiff = (b << 8) - colors[i].blue;
                            int sum = ABS (rdiff) + ABS (gdiff) + ABS (bdiff);
                            if (sum < max) { pixel = i; max = sum; }
                        }
                    }

                    gdk_image_put_pixel( data_image, x, y, pixel );

                    break;
                }
            case 15:
                {
                    guint32 pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
                    gdk_image_put_pixel( data_image, x, y, pixel );
                    break;
                }
            case 16:
                {
                    guint32 pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
                    gdk_image_put_pixel( data_image, x, y, pixel );
                    break;
                }
            case 32:
            case 24:
                {
                    guint32 pixel = 0;
                    switch (b_o)
                    {
                    case RGB: pixel = (r << 16) | (g << 8) | b; break;
                    case RBG: pixel = (r << 16) | (b << 8) | g; break;
                    case BRG: pixel = (b << 16) | (r << 8) | g; break;
                    case BGR: pixel = (b << 16) | (g << 8) | r; break;
                    case GRB: pixel = (g << 16) | (r << 8) | b; break;
                    case GBR: pixel = (g << 16) | (b << 8) | r; break;
                    }
                    gdk_image_put_pixel( data_image, x, y, pixel );
                }
            default: break;
            }
        } // for
    }  // for

    // Blit picture

    GdkGC *data_gc = gdk_gc_new( bitmap.GetPixmap() );

    gdk_draw_image( bitmap.GetPixmap(), data_gc, data_image, 0, 0, 0, 0, width, height );

    gdk_image_destroy( data_image );
    gdk_gc_unref( data_gc );

    // Blit mask

    if (HasMask())
    {
        GdkGC *mask_gc = gdk_gc_new( bitmap.GetMask()->GetBitmap() );

        gdk_draw_image( bitmap.GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

        gdk_image_destroy( mask_image );
        gdk_gc_unref( mask_gc );
    }

    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );

    GdkImage *gdk_image = (GdkImage*) NULL;
    if (bitmap.GetPixmap())
    {
        gdk_image = gdk_image_get( bitmap.GetPixmap(),
            0, 0,
            bitmap.GetWidth(), bitmap.GetHeight() );
    } else
    if (bitmap.GetBitmap())
    {
        gdk_image = gdk_image_get( bitmap.GetBitmap(),
            0, 0,
            bitmap.GetWidth(), bitmap.GetHeight() );
    } else
    {
        wxFAIL_MSG( wxT("Ill-formed bitmap") );
    }

    wxCHECK_RET( gdk_image, wxT("couldn't create image") );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        gdk_image_destroy( gdk_image );
        wxFAIL_MSG( wxT("couldn't create image") );
        return;
    }

    GdkImage *gdk_image_mask = (GdkImage*) NULL;
    if (bitmap.GetMask())
    {
        gdk_image_mask = gdk_image_get( bitmap.GetMask()->GetBitmap(),
            0, 0,
            bitmap.GetWidth(), bitmap.GetHeight() );

        SetMaskColour( 16, 16, 16 );  // anything unlikely and dividable
    }

    int bpp = -1;
    int red_shift_right = 0;
    int green_shift_right = 0;
    int blue_shift_right = 0;
    int red_shift_left = 0;
    int green_shift_left = 0;
    int blue_shift_left = 0;
    bool use_shift = FALSE;
    
    if (bitmap.GetPixmap())
    {
        GdkVisual *visual = gdk_window_get_visual( bitmap.GetPixmap() );

        if (visual == NULL) visual = gdk_window_get_visual( wxRootWindow->window );
        bpp = visual->depth;
        if (bpp == 16) bpp = visual->red_prec + visual->green_prec + visual->blue_prec;
        red_shift_right = visual->red_shift;
        red_shift_left = 8-visual->red_prec;
        green_shift_right = visual->green_shift;
        green_shift_left = 8-visual->green_prec;
        blue_shift_right = visual->blue_shift;
        blue_shift_left = 8-visual->blue_prec;
        
        use_shift = (visual->type == GDK_VISUAL_TRUE_COLOR) || (visual->type == GDK_VISUAL_DIRECT_COLOR);
    }
    if (bitmap.GetBitmap())
    {
        bpp = 1;
    }

    
    GdkColormap *cmap = gtk_widget_get_default_colormap();

    long pos = 0;
    for (int j = 0; j < bitmap.GetHeight(); j++)
    {
        for (int i = 0; i < bitmap.GetWidth(); i++)
        {
            wxUint32 pixel = gdk_image_get_pixel( gdk_image, i, j );
                if (bpp == 1)
                {
                    if (pixel == 0)
                        {
                    data[pos]   = 0;
                    data[pos+1] = 0;
                    data[pos+2] = 0;
                        }
                        else
                        {
                    data[pos]   = 255;
                    data[pos+1] = 255;
                    data[pos+2] = 255;
                        }
            }
            else if (use_shift)
            {
                data[pos] =   (pixel >> red_shift_right)   << red_shift_left;
                data[pos+1] = (pixel >> green_shift_right) << green_shift_left;
                data[pos+2] = (pixel >> blue_shift_right)  << blue_shift_left;
                } 
            else if (cmap->colors)
            {
                data[pos] =   cmap->colors[pixel].red   >> 8;
                data[pos+1] = cmap->colors[pixel].green >> 8;
                data[pos+2] = cmap->colors[pixel].blue  >> 8;
            } 
            else
            {
                wxFAIL_MSG( wxT("Image conversion failed. Unknown visual type.") );
            } 

            if (gdk_image_mask)
            {
                int mask_pixel = gdk_image_get_pixel( gdk_image_mask, i, j );
                if (mask_pixel == 0)
                {
                    data[pos] = 16;
                    data[pos+1] = 16;
                    data[pos+2] = 16;
                }
            }

            pos += 3;
        }
    }

    gdk_image_destroy( gdk_image );
    if (gdk_image_mask) gdk_image_destroy( gdk_image_mask );
}

#endif

//-----------------------------------------------------------------------------
// Motif conversion routines
//-----------------------------------------------------------------------------

#ifdef __WXMOTIF__
#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif
#include "wx/utils.h"
#include <math.h>

/*

Date: Wed, 05 Jan 2000 11:45:40 +0100
From: Frits Boel <boel@niob.knaw.nl>
To: julian.smart@ukonline.co.uk
Subject: Patch for Motif ConvertToBitmap

Hi Julian,

I've been working on a wxWin application for image processing. From the
beginning, I was surprised by the (lack of) speed of ConvertToBitmap,
till I looked in the source code of image.cpp. I saw that converting a
wxImage to a bitmap with 8-bit pixels is done with comparing every pixel
to the 256 colors of the palet. A very time-consuming piece of code!

Because I wanted a faster application, I've made a 'patch' for this. In
short: every pixel of the image is compared to a sorted list with
colors. If the color is found in the list, the palette entry is
returned; if the color is not found, the color palette is searched and
then the palette entry is returned and the color added to the sorted
list.

Maybe there is another method for this, namely changing the palette
itself (if the colors are known, as is the case with tiffs with a
colormap). I did not look at this, maybe someone else did?

The code of the patch is attached, have a look on it, and maybe you will
ship it with the next release of wxMotif?

Regards,

Frits Boel
Software engineer at Hubrecht Laboratory, The Netherlands.

*/

class wxSearchColor
{
public:
  wxSearchColor( void );
  wxSearchColor( int size, XColor *colors );
  ~wxSearchColor( void );

  int SearchColor( int r, int g, int b );
private:
  int AddColor( unsigned int value, int pos );

  int          size;
  XColor       *colors;
  unsigned int *color;
  int          *entry;

  int bottom;
  int top;
};

wxSearchColor::wxSearchColor( void )
{
  size   = 0;
  colors = (XColor*) NULL;
  color  = (unsigned int *) NULL;
  entry  = (int*) NULL;

  bottom = 0;
  top    = 0;
}

wxSearchColor::wxSearchColor( int size_, XColor *colors_ )
{
    int i;
    size   = size_;
    colors = colors_;
    color  = new unsigned int[size];
    entry  = new int         [size];

    for (i = 0; i < size; i++ ) {
        entry[i] = -1;
    }

    bottom = top = ( size >> 1 );
}

wxSearchColor::~wxSearchColor( void )
{
  if ( color ) delete color;
  if ( entry ) delete entry;
}

int wxSearchColor::SearchColor( int r, int g, int b )
{
  unsigned int value = ( ( ( r * 256 ) + g ) * 256 ) + b;
  int          begin = bottom;
  int          end   = top;
  int          middle;

  while ( begin <= end ) {

    middle = ( begin + end ) >> 1;

    if ( value == color[middle] ) {
      return( entry[middle] );
    } else if ( value < color[middle] ) {
      end = middle - 1;
    } else {
      begin = middle + 1;
    }

  }

  return AddColor( value, middle );
}

int wxSearchColor::AddColor( unsigned int value, int pos )
{
  int i;
  int pixel = -1;
  int max = 3 * (65536);
  for ( i = 0; i < 256; i++ ) {
    int rdiff = ((value >> 8) & 0xFF00 ) - colors[i].red;
    int gdiff = ((value     ) & 0xFF00 ) - colors[i].green;
    int bdiff = ((value << 8) & 0xFF00 ) - colors[i].blue;
    int sum = abs (rdiff) + abs (gdiff) + abs (bdiff);
    if (sum < max) { pixel = i; max = sum; }
  }

  if ( entry[pos] < 0 ) {
    color[pos] = value;
    entry[pos] = pixel;
  } else if ( value < color[pos] ) {

    if ( bottom > 0 ) {
      for ( i = bottom; i < pos; i++ ) {
        color[i-1] = color[i];
        entry[i-1] = entry[i];
      }
      bottom--;
      color[pos-1] = value;
      entry[pos-1] = pixel;
    } else if ( top < size-1 ) {
      for ( i = top; i >= pos; i-- ) {
        color[i+1] = color[i];
        entry[i+1] = entry[i];
      }
      top++;
      color[pos] = value;
      entry[pos] = pixel;
    }

  } else {

    if ( top < size-1 ) {
      for ( i = top; i > pos; i-- ) {
        color[i+1] = color[i];
        entry[i+1] = entry[i];
      }
      top++;
      color[pos+1] = value;
      entry[pos+1] = pixel;
    } else if ( bottom > 0 ) {
      for ( i = bottom; i < pos; i++ ) {
        color[i-1] = color[i];
        entry[i-1] = entry[i];
      }
      bottom--;
      color[pos] = value;
      entry[pos] = pixel;
    }

  }

  return( pixel );
}

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    // Create image

    XImage *data_image = XCreateImage( dpy, vis, bpp, ZPixmap, 0, 0, width, height, 32, 0 );
    data_image->data = (char*) malloc( data_image->bytes_per_line * data_image->height );

    bitmap.Create( width, height, bpp );

    /*
    // Create mask

      GdkImage *mask_image = (GdkImage*) NULL;

        if (HasMask())
        {
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

          mask_image =  gdk_image_new_bitmap( gdk_visual_get_system(), mask_data, width, height );

            wxMask *mask = new wxMask();
            mask->m_bitmap = gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, 1 );

              bitmap.SetMask( mask );
              }
    */

    // Retrieve depth info

    XVisualInfo vinfo_template;
    XVisualInfo *vi;

    vinfo_template.visual = vis;
    vinfo_template.visualid = XVisualIDFromVisual( vis );
    vinfo_template.depth = bpp;
    int nitem = 0;

    vi = XGetVisualInfo( dpy, VisualIDMask|VisualDepthMask, &vinfo_template, &nitem );

    wxCHECK_MSG( vi, wxNullBitmap, wxT("no visual") );

    XFree( vi );

    if ((bpp == 16) && (vi->red_mask != 0xf800)) bpp = 15;
    if (bpp < 8) bpp = 8;

    // Render

    enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
    byte_order b_o = RGB;

    if (bpp >= 24)
    {
        if ((vi->red_mask > vi->green_mask) && (vi->green_mask > vi->blue_mask))      b_o = RGB;
        else if ((vi->red_mask > vi->blue_mask) && (vi->blue_mask > vi->green_mask))  b_o = RGB;
        else if ((vi->blue_mask > vi->red_mask) && (vi->red_mask > vi->green_mask))   b_o = BRG;
        else if ((vi->blue_mask > vi->green_mask) && (vi->green_mask > vi->red_mask)) b_o = BGR;
        else if ((vi->green_mask > vi->red_mask) && (vi->red_mask > vi->blue_mask))   b_o = GRB;
        else if ((vi->green_mask > vi->blue_mask) && (vi->blue_mask > vi->red_mask))  b_o = GBR;
    }

    /*
    int r_mask = GetMaskRed();
    int g_mask = GetMaskGreen();
    int b_mask = GetMaskBlue();
    */

    XColor colors[256];
    if (bpp == 8)
    {
        Colormap cmap = (Colormap) wxTheApp->GetMainColormap( dpy );

        for (int i = 0; i < 256; i++) colors[i].pixel = i;
        XQueryColors( dpy, cmap, colors, 256 );
    }

    wxSearchColor scolor( 256, colors );
    unsigned char* data = GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            /*
            if (HasMask())
            {
            if ((r == r_mask) && (b == b_mask) && (g == g_mask))
            gdk_image_put_pixel( mask_image, x, y, 1 );
            else
            gdk_image_put_pixel( mask_image, x, y, 0 );
            }
            */

            switch (bpp)
            {
            case 8:
                {
#if 0 // Old, slower code
                    int pixel = -1;
                    /*
                    if (wxTheApp->m_colorCube)
                    {
                    pixel = wxTheApp->m_colorCube
                    [ ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + ((b & 0xf8) >> 3) ];
                    }
                    else
                    {
                    */
                    int max = 3 * (65536);
                    for (int i = 0; i < 256; i++)
                    {
                        int rdiff = (r << 8) - colors[i].red;
                        int gdiff = (g << 8) - colors[i].green;
                        int bdiff = (b << 8) - colors[i].blue;
                        int sum = abs (rdiff) + abs (gdiff) + abs (bdiff);
                        if (sum < max) { pixel = i; max = sum; }
                    }
                    /*
                    }
                    */
#endif

                    // And this is all to get the 'right' color...
                    int pixel = scolor.SearchColor( r, g, b );
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 15:
                {
                    int pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 16:
                {
                    int pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 32:
            case 24:
                {
                    int pixel = 0;
                    switch (b_o)
                    {
                    case RGB: pixel = (r << 16) | (g << 8) | b; break;
                    case RBG: pixel = (r << 16) | (b << 8) | g; break;
                    case BRG: pixel = (b << 16) | (r << 8) | g; break;
                    case BGR: pixel = (b << 16) | (g << 8) | r; break;
                    case GRB: pixel = (g << 16) | (r << 8) | b; break;
                    case GBR: pixel = (g << 16) | (b << 8) | r; break;
                    }
                    XPutPixel( data_image, x, y, pixel );
                }
            default: break;
            }
        } // for
    }  // for

    // Blit picture

    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel( dpy, DefaultScreen( dpy ) );
    GC gc = XCreateGC( dpy, RootWindow ( dpy, DefaultScreen(dpy) ), GCForeground, &gcvalues );
    XPutImage( dpy, (Drawable)bitmap.GetPixmap(), gc, data_image, 0, 0, 0, 0, width, height );

    XDestroyImage( data_image );
    XFreeGC( dpy, gc );

    /*
    // Blit mask

      if (HasMask())
      {
      GdkGC *mask_gc = gdk_gc_new( bitmap.GetMask()->GetBitmap() );

        gdk_draw_image( bitmap.GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

          gdk_image_destroy( mask_image );
          gdk_gc_unref( mask_gc );
          }
    */

    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    XImage *ximage = XGetImage( dpy,
        (Drawable)bitmap.GetPixmap(),
        0, 0,
        bitmap.GetWidth(), bitmap.GetHeight(),
        AllPlanes, ZPixmap );

    wxCHECK_RET( ximage, wxT("couldn't create image") );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        XDestroyImage( ximage );
        wxFAIL_MSG( wxT("couldn't create image") );
        return;
    }

    /*
    GdkImage *gdk_image_mask = (GdkImage*) NULL;
    if (bitmap.GetMask())
    {
    gdk_image_mask = gdk_image_get( bitmap.GetMask()->GetBitmap(),
    0, 0,
    bitmap.GetWidth(), bitmap.GetHeight() );

      SetMaskColour( 16, 16, 16 );  // anything unlikely and dividable
      }
    */

    // Retrieve depth info

    XVisualInfo vinfo_template;
    XVisualInfo *vi;

    vinfo_template.visual = vis;
    vinfo_template.visualid = XVisualIDFromVisual( vis );
    vinfo_template.depth = bpp;
    int nitem = 0;

    vi = XGetVisualInfo( dpy, VisualIDMask|VisualDepthMask, &vinfo_template, &nitem );

    wxCHECK_RET( vi, wxT("no visual") );

    if ((bpp == 16) && (vi->red_mask != 0xf800)) bpp = 15;

    XFree( vi );

    XColor colors[256];
    if (bpp == 8)
    {
        Colormap cmap = (Colormap)wxTheApp->GetMainColormap( dpy );

        for (int i = 0; i < 256; i++) colors[i].pixel = i;
        XQueryColors( dpy, cmap, colors, 256 );
    }

    long pos = 0;
    for (int j = 0; j < bitmap.GetHeight(); j++)
    {
        for (int i = 0; i < bitmap.GetWidth(); i++)
        {
            int pixel = XGetPixel( ximage, i, j );
            if (bpp <= 8)
            {
                data[pos] = colors[pixel].red >> 8;
                data[pos+1] = colors[pixel].green >> 8;
                data[pos+2] = colors[pixel].blue >> 8;
            } else if (bpp == 15)
            {
                data[pos] = (pixel >> 7) & 0xf8;
                data[pos+1] = (pixel >> 2) & 0xf8;
                data[pos+2] = (pixel << 3) & 0xf8;
            } else if (bpp == 16)
            {
                data[pos] = (pixel >> 8) & 0xf8;
                data[pos+1] = (pixel >> 3) & 0xfc;
                data[pos+2] = (pixel << 3) & 0xf8;
            } else
            {
                data[pos] = (pixel >> 16) & 0xff;
                data[pos+1] = (pixel >> 8) & 0xff;
                data[pos+2] = pixel & 0xff;
            }

            /*
            if (gdk_image_mask)
            {
            int mask_pixel = gdk_image_get_pixel( gdk_image_mask, i, j );
            if (mask_pixel == 0)
            {
            data[pos] = 16;
            data[pos+1] = 16;
            data[pos+2] = 16;
            }
            }
            */

            pos += 3;
        }
    }

    XDestroyImage( ximage );
    /*
    if (gdk_image_mask) gdk_image_destroy( gdk_image_mask );
    */
}
#endif

#ifdef __WXPM__
// OS/2 Presentation manager conversion routings

wxBitmap wxImage::ConvertToBitmap() const
{
    if ( !Ok() )
        return wxNullBitmap;
    wxBitmap bitmap; // remove
// TODO:
/*
    int sizeLimit = 1024*768*3;

    // width and height of the device-dependent bitmap
    int width = GetWidth();
    int bmpHeight = GetHeight();

    // calc the number of bytes per scanline and padding
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary = bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
    // calc the number of DIBs and heights of DIBs
    int numDIB = 1;
    int hRemain = 0;
    int height = sizeLimit/bytePerLine;
    if( height >= bmpHeight )
        height = bmpHeight;
    else
    {
        numDIB =  bmpHeight / height;
        hRemain = bmpHeight % height;
        if( hRemain >0 )  numDIB++;
    }

    // set bitmap parameters
    wxBitmap bitmap;
    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );
    bitmap.SetWidth( width );
    bitmap.SetHeight( bmpHeight );
    bitmap.SetDepth( wxDisplayDepth() );

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, bitmap, wxT("could not allocate memory for DIB header") );
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = (DWORD)width;
    lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
    lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
    //   the general formula for biSizeImage:
    //      ( ( ( ((DWORD)width*24) +31 ) & ~31 ) >> 3 ) * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *)malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate memory for DIB") );
        free( lpDIBh );
        return bitmap;
    }

    // create and set the device-dependent bitmap
    HDC hdc = ::GetDC(NULL);
    HDC memdc = ::CreateCompatibleDC( hdc );
    HBITMAP hbitmap;
    hbitmap = ::CreateCompatibleBitmap( hdc, width, bmpHeight );
    ::SelectObject( memdc, hbitmap);

    // copy image data into DIB data and then into DDB (in a loop)
    unsigned char *data = GetData();
    int i, j, n;
    int origin = 0;
    unsigned char *ptdata = data;
    unsigned char *ptbits;

    for( n=0; n<numDIB; n++ )
    {
        if( numDIB > 1 && n == numDIB-1 && hRemain > 0 )
        {
            // redefine height and size of the (possibly) last smaller DIB
            // memory is not reallocated
            height = hRemain;
            lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
            lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        }
        ptbits = lpBits;

        for( j=0; j<height; j++ )
        {
            for( i=0; i<width; i++ )
            {
                *(ptbits++) = *(ptdata+2);
                *(ptbits++) = *(ptdata+1);
                *(ptbits++) = *(ptdata  );
                ptdata += 3;
            }
            for( i=0; i< padding; i++ )   *(ptbits++) = 0;
        }
        ::StretchDIBits( memdc, 0, origin, width, height,\
            0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
        origin += height;
        // if numDIB = 1,  lines below can also be used
        //    hbitmap = CreateDIBitmap( hdc, &(lpDIBh->bmiHeader), CBM_INIT, lpBits, lpDIBh, DIB_RGB_COLORS );
        // The above line is equivalent to the following two lines.
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    ::SetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS);
        // or the following lines
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    HDC memdc = ::CreateCompatibleDC( hdc );
        //    ::SelectObject( memdc, hbitmap);
        //    ::SetDIBitsToDevice( memdc, 0, 0, width, height,
        //            0, 0, 0, height, (void *)lpBits, lpDIBh, DIB_RGB_COLORS);
        //    ::SelectObject( memdc, 0 );
        //    ::DeleteDC( memdc );
    }
    bitmap.SetHBITMAP( (WXHBITMAP) hbitmap );

    // similarly, created an mono-bitmap for the possible mask
    if( HasMask() )
    {
        hbitmap = ::CreateBitmap( (WORD)width, (WORD)bmpHeight, 1, 1, NULL );
        ::SelectObject( memdc, hbitmap);
        if( numDIB == 1 )   height = bmpHeight;
        else                height = sizeLimit/bytePerLine;
        lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
        lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        origin = 0;
        unsigned char r = GetMaskRed();
        unsigned char g = GetMaskGreen();
        unsigned char b = GetMaskBlue();
        unsigned char zero = 0, one = 255;
        ptdata = data;
        for( n=0; n<numDIB; n++ )
        {
            if( numDIB > 1 && n == numDIB - 1 && hRemain > 0 )
            {
                // redefine height and size of the (possibly) last smaller DIB
                // memory is not reallocated
                height = hRemain;
                lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
                lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
            }
            ptbits = lpBits;
            for( int j=0; j<height; j++ )
            {
                for(i=0; i<width; i++ )
                {
                    if( (*(ptdata++)!=r) | (*(ptdata++)!=g) | (*(ptdata++)!=b) )
                    {
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                    }
                    else
                    {
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                    }
                }
                for( i=0; i< padding; i++ )   *(ptbits++) = zero;
            }
            ::StretchDIBits( memdc, 0, origin, width, height,\
                0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
            origin += height;
        }
        // create a wxMask object
        wxMask *mask = new wxMask();
        mask->SetMaskBitmap( (WXHBITMAP) hbitmap );
        bitmap.SetMask( mask );
    }

    // free allocated resources
    ::SelectObject( memdc, 0 );
    ::DeleteDC( memdc );
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);

    // check the wxBitmap object
    if( bitmap.GetHBITMAP() )
        bitmap.SetOk( TRUE );
    else
        bitmap.SetOk( FALSE );
*/
    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    // check the bitmap
    if( !bitmap.Ok() )
    {
        wxFAIL_MSG( wxT("invalid bitmap") );
        return;
    }

    // create an wxImage object
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    Create( width, height );
    unsigned char *data = GetData();
    if( !data )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return;
    }

    // calc the number of bytes per scanline and padding in the DIB
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary =  bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
// TODO:
/*
    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    if( !lpDIBh )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB header") );
        free( data );
        return;
    }
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = width;
    lpDIBh->bmiHeader.biHeight = -height;
    lpDIBh->bmiHeader.biSizeImage = bytePerLine * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *) malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB") );
        free( data );
        free( lpDIBh );
        return;
    }

    // copy data from the device-dependent bitmap to the DIB
    HDC hdc = ::GetDC(NULL);
    HBITMAP hbitmap;
    hbitmap = (HBITMAP) bitmap.GetHBITMAP();
    ::GetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );

    // copy DIB data into the wxImage object
    int i, j;
    unsigned char *ptdata = data;
    unsigned char *ptbits = lpBits;
    for( i=0; i<height; i++ )
    {
        for( j=0; j<width; j++ )
        {
            *(ptdata++) = *(ptbits+2);
            *(ptdata++) = *(ptbits+1);
            *(ptdata++) = *(ptbits  );
            ptbits += 3;
        }
        ptbits += padding;
    }

    // similarly, set data according to the possible mask bitmap
    if( bitmap.GetMask() && bitmap.GetMask()->GetMaskBitmap() )
    {
        hbitmap = (HBITMAP) bitmap.GetMask()->GetMaskBitmap();
        // memory DC created, color set, data copied, and memory DC deleted
        HDC memdc = ::CreateCompatibleDC( hdc );
        ::SetTextColor( memdc, RGB( 0, 0, 0 ) );
        ::SetBkColor( memdc, RGB( 255, 255, 255 ) );
        ::GetDIBits( memdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );
        ::DeleteDC( memdc );
        // background color set to RGB(16,16,16) in consistent with wxGTK
        unsigned char r=16, g=16, b=16;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                if( *ptbits != 0 )
                    ptdata += 3;
                else
                {
                    *(ptdata++)  = r;
                    *(ptdata++)  = g;
                    *(ptdata++)  = b;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }
        SetMaskColour( r, g, b );
        SetMask( TRUE );
    }
    else
    {
        SetMask( FALSE );
    }
    // free allocated resources
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);
*/
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
    wxNode *node;
    wxHNode *hnode;
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

        hnode = (wxHNode *) h.Get(key);

        if (!hnode)
        {
            h.Put(key, (wxObject *)(new wxHNode));
            nentries++;
        }
    }

    // delete all HNodes
    h.BeginFind();
    while ((node = h.Next()) != NULL)
        delete (wxHNode *)node->GetData();

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


