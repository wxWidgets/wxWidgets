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
    if (m_data) free( m_data );
}

wxList wxImage::sm_handlers;

//-----------------------------------------------------------------------------

#define M_IMGDATA ((wxImageRefData *)m_refData)

#if !USE_SHARED_LIBRARIES
    IMPLEMENT_DYNAMIC_CLASS(wxImage, wxObject)
#endif

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

wxImage wxImage::Scale( int width, int height )
{
    wxImage image;

    wxCHECK_MSG( Ok(), image, _T("invalid image") );

    wxCHECK_MSG( (width > 0) && (height > 0), image, _T("invalid image size") );

    image.Create( width, height );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, _T("unable to create image") );

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

void wxImage::SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), _T("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_RET( (x>=0) && (y>=0) && (x<w) && (y<h), _T("invalid image index") );

    long pos = (y * w + x) * 3;

    M_IMGDATA->m_data[ pos   ] = r;
    M_IMGDATA->m_data[ pos+1 ] = g;
    M_IMGDATA->m_data[ pos+2 ] = b;
}

unsigned char wxImage::GetRed( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, _T("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos];
}

unsigned char wxImage::GetGreen( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, _T("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+1];
}

unsigned char wxImage::GetBlue( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, _T("invalid image index") );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+2];
}

bool wxImage::Ok() const
{
    return (M_IMGDATA && M_IMGDATA->m_ok);
}

char unsigned *wxImage::GetData() const
{
    wxCHECK_MSG( Ok(), (char unsigned *)NULL, _T("invalid image") );

    return M_IMGDATA->m_data;
}

void wxImage::SetData( char unsigned *data )
{
    wxCHECK_RET( Ok(), _T("invalid image") );

    memcpy(M_IMGDATA->m_data, data, M_IMGDATA->m_width * M_IMGDATA->m_height * 3);
}

void wxImage::SetMaskColour( unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), _T("invalid image") );

    M_IMGDATA->m_maskRed = r;
    M_IMGDATA->m_maskGreen = g;
    M_IMGDATA->m_maskBlue = b;
    M_IMGDATA->m_hasMask = TRUE;
}

unsigned char wxImage::GetMaskRed() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    return M_IMGDATA->m_maskRed;
}

unsigned char wxImage::GetMaskGreen() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    return M_IMGDATA->m_maskGreen;
}

unsigned char wxImage::GetMaskBlue() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    return M_IMGDATA->m_maskBlue;
}

void wxImage::SetMask( bool mask )
{
    wxCHECK_RET( Ok(), _T("invalid image") );

    M_IMGDATA->m_hasMask = mask;
}

bool wxImage::HasMask() const
{
    wxCHECK_MSG( Ok(), FALSE, _T("invalid image") );

    return M_IMGDATA->m_hasMask;
}

int wxImage::GetWidth() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    return M_IMGDATA->m_width;
}

int wxImage::GetHeight() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid image") );

    return M_IMGDATA->m_height;
}

bool wxImage::LoadFile( const wxString& filename, long type )
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        return LoadFile(stream, type);
    }

    else {
        wxLogError( _T("Can't load image from file '%s': file does not exist."), filename.c_str() );

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
        return LoadFile(stream, mimetype);
    }

    else {
        wxLogError( _T("Can't load image from file '%s': file does not exist."), filename.c_str() );

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
        return SaveFile(stream, type);
    else
#endif // wxUSE_STREAMS
        return FALSE;
}

bool wxImage::SaveFile( const wxString& filename, const wxString& mimetype )
{
#if wxUSE_STREAMS
    wxFileOutputStream stream(filename);

    if ( stream.LastError() == wxStream_NOERROR )
        return SaveFile(stream, mimetype);
    else
#endif // wxUSE_STREAMS
        return FALSE;
}

#if wxUSE_STREAMS
bool wxImage::LoadFile( wxInputStream& stream, long type )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler = FindHandler(type);

    if (handler == NULL)
    {
        wxLogWarning( _T("No image handler for type %d defined."), type );

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
        wxLogWarning( _T("No image handler for type %s defined."), mimetype.GetData() );

        return FALSE;
    }

    return handler->LoadFile( this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, int type )
{
    wxCHECK_MSG( Ok(), FALSE, _T("invalid image") );

    wxImageHandler *handler = FindHandler(type);

    if (handler == NULL)
    {
        wxLogWarning( _T("No image handler for type %d defined."), type );

        return FALSE;
    }

    return handler->SaveFile( this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, const wxString& mimetype )
{
    wxCHECK_MSG( Ok(), FALSE, _T("invalid image") );

    wxImageHandler *handler = FindHandlerMime(mimetype);

    if (handler == NULL)
    {
        wxLogWarning( _T("No image handler for type %s defined."), mimetype.GetData() );

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

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxImageHandler,wxObject)
#endif

#if wxUSE_STREAMS
bool wxImageHandler::LoadFile( wxImage *WXUNUSED(image), wxInputStream& WXUNUSED(stream) )
{
    return FALSE;
}

bool wxImageHandler::SaveFile( wxImage *WXUNUSED(image), wxOutputStream& WXUNUSED(stream) )
{
    return FALSE;
}
#endif // wxUSE_STREAMS

//-----------------------------------------------------------------------------
// MSW conversion routines
//-----------------------------------------------------------------------------

#ifdef __WXMSW__

wxBitmap wxImage::ConvertToBitmap() const
{
    // sizeLimit is the MS upper limit for the DIB size
    int sizeLimit = 1024*768*3;

    // width and height of the device-dependent bitmap
    int width = GetWidth();
    int bmpHeight = GetHeight();

    // calc the number of bytes per scanline and padding
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    div_t lineBoundary = div( bytePerLine, sizeDWORD );
    int padding = 0;
    if( lineBoundary.rem > 0 )
    {
        padding = sizeDWORD - lineBoundary.rem;
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
        div_t result = div( bmpHeight, height );
        numDIB = result.quot;
        hRemain = result.rem;
        if( hRemain >0 )  numDIB++;
    }

    // set bitmap parameters
    wxBitmap bitmap;
    wxCHECK_MSG( Ok(), bitmap, _T("invalid image") );
    bitmap.SetWidth( width );
    bitmap.SetHeight( bmpHeight );
    bitmap.SetDepth( wxDisplayDepth() );

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, bitmap, _T("could not allocate memory for DIB header") );
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
        wxFAIL_MSG( _T("could not allocate memory for DIB") );
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
        //	    0, 0, 0, height, (void *)lpBits, lpDIBh, DIB_RGB_COLORS);
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

    // check the wxBitmap object
    if( bitmap.GetHBITMAP() )
        bitmap.SetOk( TRUE );
    else
        bitmap.SetOk( FALSE );

    return bitmap;
}

wxImage::wxImage( const wxBitmap &bitmap )
{
    // check the bitmap
    if( !bitmap.Ok() )
    {
        wxFAIL_MSG( _T("invalid bitmap") );
        return;
    }

    // create an wxImage object
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    Create( width, height );
    unsigned char *data = GetData();
    if( !data )
    {
        wxFAIL_MSG( _T("could not allocate data for image") );
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
        wxFAIL_MSG( _T("could not allocate data for DIB header") );
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
        wxFAIL_MSG( _T("could not allocate data for DIB") );
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

//-----------------------------------------------------------------------------
// GTK conversion routines
//-----------------------------------------------------------------------------

#ifdef __WXGTK__

#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"

#if (GTK_MINOR_VERSION > 0)
#include "gdk/gdkrgb.h"
#endif

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, _T("invalid image") );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    bitmap.SetPixmap( gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, -1 ) );

     // Retrieve depth

    GdkVisual *visual = gdk_window_get_visual( bitmap.GetPixmap() );
    if (visual == NULL) visual = gdk_window_get_visual( (GdkWindow*) &gdk_root_parent );
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
        mask->m_bitmap = gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, 1 );

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
    wxCHECK_RET( bitmap.Ok(), _T("invalid bitmap") );

    GdkImage *gdk_image = gdk_image_get( bitmap.GetPixmap(),
        0, 0,
        bitmap.GetWidth(), bitmap.GetHeight() );

    wxCHECK_RET( gdk_image, _T("couldn't create image") );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        gdk_image_destroy( gdk_image );
        wxFAIL_MSG( _T("couldn't create image") );
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

    GdkVisual *visual = gdk_window_get_visual( bitmap.GetPixmap() );
    if (visual == NULL) visual = gdk_window_get_visual( (GdkWindow*) &gdk_root_parent );
    int bpp = visual->depth;
    if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;

    GdkColormap *cmap = gtk_widget_get_default_colormap();

    long pos = 0;
    for (int j = 0; j < bitmap.GetHeight(); j++)
    {
        for (int i = 0; i < bitmap.GetWidth(); i++)
        {
            int pixel = gdk_image_get_pixel( gdk_image, i, j );
            if (bpp <= 8)
            {
                data[pos] = cmap->colors[pixel].red >> 8;
                data[pos+1] = cmap->colors[pixel].green >> 8;
                data[pos+2] = cmap->colors[pixel].blue >> 8;
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

#include <Xm/Xm.h>
#include "wx/utils.h"
#include <math.h>

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, _T("invalid image") );

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

    wxCHECK_MSG( vi, wxNullBitmap, _T("no visual") );

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
    wxCHECK_RET( bitmap.Ok(), _T("invalid bitmap") );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    XImage *ximage = XGetImage( dpy,
        (Drawable)bitmap.GetPixmap(),
        0, 0,
        bitmap.GetWidth(), bitmap.GetHeight(),
        AllPlanes, ZPixmap );

    wxCHECK_RET( ximage, _T("couldn't create image") );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        XDestroyImage( ximage );
        wxFAIL_MSG( _T("couldn't create image") );
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

    wxCHECK_RET( vi, _T("no visual") );

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
