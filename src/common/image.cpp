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
#if wxUSE_LIBPNG
#include "../png/png.h"
#endif
#if wxUSE_LIBJPEG
extern "C" {
#include <jpeglib.h>
}
#endif
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"

#ifdef __SALFORDC__
#ifdef FAR
#undef FAR
#endif
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------

class wxImageRefData: public wxObjectRefData
{

public:
  wxImageRefData(void);
  ~wxImageRefData(void);

  int             m_width;
  int             m_height;
  unsigned char  *m_data;
  bool            m_hasMask;
  unsigned char   m_maskRed,m_maskGreen,m_maskBlue;
  bool            m_ok;
};

wxImageRefData::wxImageRefData(void)
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

wxImageRefData::~wxImageRefData(void)
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

#if wxUSE_STREAMS
wxImage::wxImage( wxInputStream& stream, long type )
{
    LoadFile( stream, type );
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

    wxCHECK_MSG( Ok(), image, "invlaid image" );

    wxCHECK_MSG( (width > 0) && (height > 0), image, "invalid image size" );

    image.Create( width, height );

    char unsigned *data = image.GetData();

    wxCHECK_MSG( data, image, "unable to create image" );

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
    wxCHECK_RET( Ok(), "invalid image" );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_RET( (x>=0) && (y>=0) && (x<w) && (y<h), "invalid image index" );

    long pos = (y * w + x) * 3;

    M_IMGDATA->m_data[ pos   ] = r;
    M_IMGDATA->m_data[ pos+1 ] = g;
    M_IMGDATA->m_data[ pos+2 ] = b;
}

unsigned char wxImage::GetRed( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, "invalid image index" );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos];
}

unsigned char wxImage::GetGreen( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, "invalid image index" );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+1];
}

unsigned char wxImage::GetBlue( int x, int y )
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    int w = M_IMGDATA->m_width;
    int h = M_IMGDATA->m_height;

    wxCHECK_MSG( (x>=0) && (y>=0) && (x<w) && (y<h), 0, "invalid image index" );

    long pos = (y * w + x) * 3;

    return M_IMGDATA->m_data[pos+2];
}

bool wxImage::Ok() const
{
    return (M_IMGDATA && M_IMGDATA->m_ok);
}

char unsigned *wxImage::GetData() const
{
    wxCHECK_MSG( Ok(), (char unsigned *)NULL, "invalid image" );

    return M_IMGDATA->m_data;
}

void wxImage::SetData( char unsigned *WXUNUSED(data) )
{
    wxCHECK_RET( Ok(), "invalid image" );
}

void wxImage::SetMaskColour( unsigned char r, unsigned char g, unsigned char b )
{
    wxCHECK_RET( Ok(), "invalid image" );

    M_IMGDATA->m_maskRed = r;
    M_IMGDATA->m_maskGreen = g;
    M_IMGDATA->m_maskBlue = b;
    M_IMGDATA->m_hasMask = TRUE;
}

unsigned char wxImage::GetMaskRed() const
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    return M_IMGDATA->m_maskRed;
}

unsigned char wxImage::GetMaskGreen() const
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    return M_IMGDATA->m_maskGreen;
}

unsigned char wxImage::GetMaskBlue() const
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    return M_IMGDATA->m_maskBlue;
}

void wxImage::SetMask( bool mask )
{
    wxCHECK_RET( Ok(), "invalid image" );

    M_IMGDATA->m_hasMask = mask;
}

bool wxImage::HasMask() const
{
    wxCHECK_MSG( Ok(), FALSE, "invalid image" );

    return M_IMGDATA->m_hasMask;
}

int wxImage::GetWidth() const
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

    return M_IMGDATA->m_width;
}

int wxImage::GetHeight() const
{
    wxCHECK_MSG( Ok(), 0, "invalid image" );

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
        wxLogError( "Can't load image from file '%s': file does not exist.", filename.c_str() );

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

#if wxUSE_STREAMS
bool wxImage::LoadFile( wxInputStream& stream, long type )
{
    UnRef();

    m_refData = new wxImageRefData;

    wxImageHandler *handler = FindHandler(type);

    if (handler == NULL)
    {
        wxLogWarning( "No image handler for type %d defined.", type );

        return FALSE;
    }

    return handler->LoadFile( this, stream );
}

bool wxImage::SaveFile( wxOutputStream& stream, int type )
{
    wxCHECK_MSG( Ok(), FALSE, "invalid image" );

    wxImageHandler *handler = FindHandler(type);

    if (handler == NULL)
    {
      wxLogWarning( "No image handler for type %d defined.", type );

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

void wxImage::InitStandardHandlers()
{
    AddHandler( new wxBMPHandler );
#if wxUSE_LIBPNG
    AddHandler( new wxPNGHandler );
#endif
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
// wxPNGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBPNG

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPNGHandler,wxImageHandler)
#endif


#if wxUSE_STREAMS
static void _PNG_stream_reader( png_structp png_ptr, png_bytep data, png_size_t length )
{
   ((wxInputStream*) png_get_io_ptr( png_ptr )) -> Read(data, length);
}

static void _PNG_stream_writer( png_structp png_ptr, png_bytep data, png_size_t length )
{
   ((wxOutputStream*) png_get_io_ptr( png_ptr )) -> Write(data, length);
}

bool wxPNGHandler::LoadFile( wxImage *image, wxInputStream& stream )
{
    // VZ: as this function uses setjmp() the only fool proof error handling
    //     method is to use goto (setjmp is not really C++ dtors friendly...)
    
    unsigned char **lines = (unsigned char **) NULL;
    unsigned int i;
    png_infop info_ptr = (png_infop) NULL;
    
    image->Destroy();

    png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                                  (voidp) NULL,
                                                  (png_error_ptr) NULL,
                                                  (png_error_ptr) NULL );
    if (!png_ptr)
        goto error;

    info_ptr = png_create_info_struct( png_ptr );
    if (!info_ptr)
        goto error;

    if (setjmp(png_ptr->jmpbuf))
        goto error;

    if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        goto error;

    png_set_read_fn( png_ptr, &stream, _PNG_stream_reader);

    png_uint_32 width,height;
    int bit_depth,color_type,interlace_type;

    png_read_info( png_ptr, info_ptr );
    png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, (int*) NULL, (int*) NULL );

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand( png_ptr );

    png_set_strip_16( png_ptr );
    png_set_packing( png_ptr );
    if (png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand( png_ptr );
    png_set_filler( png_ptr, 0xff, PNG_FILLER_AFTER );

    image->Create( width, height );

    if (!image->Ok())
        goto error;

    lines = (unsigned char **)malloc( height * sizeof(unsigned char *) );
    if (lines == NULL)
        goto error;

    for (i = 0; i < height; i++)
    {
        if ((lines[i] = (unsigned char *)malloc(width * (sizeof(unsigned char) * 4))) == NULL)
        {
            for ( unsigned int n = 0; n < i; n++ )
                free( lines[n] );
            goto error;
        }
    }

    // loaded successfully!
    {
        int transp = 0;
        png_read_image( png_ptr, lines );
        png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
        unsigned char *ptr = image->GetData();
        if ((color_type == PNG_COLOR_TYPE_GRAY) ||
            (color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
        {
            for (unsigned int y = 0; y < height; y++)
            {
                unsigned char *ptr2 = lines[y];
                for (unsigned int x = 0; x < width; x++)
                {
                    unsigned char r = *ptr2++;
                    unsigned char a = *ptr2++;
                    if (a < 128)
                    {
                        *ptr++ = 255;
                        *ptr++ = 0;
                        *ptr++ = 255;
                        transp = 1;
                    }
                    else
                    {
                        *ptr++ = r;
                        *ptr++ = r;
                        *ptr++ = r;
                    }
                }
            }
        }
        else
        {
            for (unsigned int y = 0; y < height; y++)
            {
                unsigned char *ptr2 = lines[y];
                for (unsigned int x = 0; x < width; x++)
                {
                    unsigned char r = *ptr2++;
                    unsigned char g = *ptr2++;
                    unsigned char b = *ptr2++;
                    unsigned char a = *ptr2++;
                    if (a < 128)
                    {
                        *ptr++ = 255;
                        *ptr++ = 0;
                        *ptr++ = 255;
                        transp = 1;
                    }
                    else
                    {
                        if ((r == 255) && (g == 0) && (b == 255)) r = 254;
                        *ptr++ = r;
                        *ptr++ = g;
                        *ptr++ = b;
                    }
                }
            }
        }

        for ( unsigned int j = 0; j < height; j++ )
            free( lines[j] );
        free( lines );

        if (transp)
        {
            image->SetMaskColour( 255, 0, 255 );
        }
        else
        {
            image->SetMask( FALSE );
        }
    }

    return TRUE;

error:
    wxLogError(_("Couldn't load a PNG image - probably file is corrupted."));

    if ( image->Ok() )
    {
        image->Destroy();
    }

    if ( lines )
    {
        free( lines );
    }

    if ( png_ptr )
    {
        if ( info_ptr )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
            free(info_ptr);
        }
        else
            png_destroy_read_struct( &png_ptr, (png_infopp) NULL, (png_infopp) NULL );
    }
    return FALSE;
}


bool wxPNGHandler::SaveFile( wxImage *image, wxOutputStream& stream )
{
  {
    png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
      return FALSE;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
      png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
      return FALSE;
    }

    if (setjmp(png_ptr->jmpbuf))
    {
      png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
      return FALSE;
    }

    png_set_write_fn( png_ptr, &stream, _PNG_stream_writer, NULL);

    png_set_IHDR( png_ptr, info_ptr, image->GetWidth(), image->GetHeight(), 8,
                  PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_color_8 sig_bit;
    sig_bit.red = 8;
    sig_bit.green = 8;
    sig_bit.blue = 8;
    sig_bit.alpha = 8;
    png_set_sBIT( png_ptr, info_ptr, &sig_bit );
    png_write_info( png_ptr, info_ptr );
    png_set_shift( png_ptr, &sig_bit );
    png_set_packing( png_ptr );

    unsigned char *data = (unsigned char *)malloc( image->GetWidth()*4 );
    if (!data)
    {
      png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
      return FALSE;
    }

    for (int y = 0; y < image->GetHeight(); y++)
    {
        unsigned char *ptr = image->GetData() + (y * image->GetWidth() * 3);
        for (int x = 0; x < image->GetWidth(); x++)
        {
            data[(x << 2) + 0] = *ptr++;
            data[(x << 2) + 1] = *ptr++;
            data[(x << 2) + 2] = *ptr++;
            if ((data[(x << 2) + 0] == image->GetMaskRed()) &&
                (data[(x << 2) + 1] == image->GetMaskGreen()) &&
                (data[(x << 2) + 2] == image->GetMaskBlue()))
            {
                data[(x << 2) + 3] = 0;
            }
            else
            {
                data[(x << 2) + 3] = 255;
            }
        }
        png_bytep row_ptr = data;
        png_write_rows( png_ptr, &row_ptr, 1 );
    }

    free(data);
    png_write_end( png_ptr, info_ptr );
    png_destroy_write_struct( &png_ptr, (png_infopp)&info_ptr );
  }
  return TRUE;
}
#endif // wxUSE_STREAMS

#endif

  // wxUSE_LIBPNG


//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxJPEGHandler,wxImageHandler)
#endif

#if wxUSE_STREAMS


//------------- JPEG Data Source Manager

typedef struct {
  struct jpeg_source_mgr pub;   /* public fields */

  JOCTET* buffer;               /* start of buffer */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

METHODDEF(void) my_init_source ( j_decompress_ptr cinfo )
{
}

METHODDEF(boolean) my_fill_input_buffer ( j_decompress_ptr cinfo )
{
  return TRUE;
}

METHODDEF(void) my_skip_input_data ( j_decompress_ptr cinfo, long num_bytes )
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  src->pub.next_input_byte += (size_t) num_bytes;
  src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

METHODDEF(void) my_term_source ( j_decompress_ptr cinfo )
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  free (src->buffer);
}

void jpeg_wxio_src( j_decompress_ptr cinfo, wxInputStream& infile )
{
  my_src_ptr src;

  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
  }
  src = (my_src_ptr) cinfo->src;
  src->pub.bytes_in_buffer = infile.StreamSize(); /* forces fill_input_buffer on first read */
  src->buffer = (JOCTET *) malloc (infile.StreamSize());
  src->pub.next_input_byte = src->buffer; /* until buffer loaded */
  infile.Read(src->buffer, infile.StreamSize());

  src->pub.init_source = my_init_source;
  src->pub.fill_input_buffer = my_fill_input_buffer;
  src->pub.skip_input_data = my_skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = my_term_source;
}



bool wxJPEGHandler::LoadFile( wxImage *image, wxInputStream& stream )
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY tempbuf;
    unsigned char *ptr;
    unsigned stride;

    image->Destroy();
    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &cinfo );
    jpeg_wxio_src( &cinfo, stream );
    jpeg_read_header( &cinfo, TRUE );
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress( &cinfo );

    image->Create( cinfo.image_width, cinfo.image_height );
    if (!image->Ok()) {
      jpeg_finish_decompress( &cinfo );
      jpeg_destroy_decompress( &cinfo );
      return FALSE;
    }
    image->SetMask( FALSE );
    ptr = image->GetData();
    stride = cinfo.output_width * 3;
    tempbuf = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1 );

    while ( cinfo.output_scanline < cinfo.output_height ) {
      jpeg_read_scanlines( &cinfo, tempbuf, 1 );
      memcpy( ptr, tempbuf[0], stride );
      ptr += stride;
    }
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    return TRUE;
}





typedef struct {
  struct jpeg_destination_mgr pub;

  wxOutputStream *stream;
  JOCTET * buffer;
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */

METHODDEF(void) init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * sizeof(JOCTET));
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

METHODDEF(boolean) empty_output_buffer (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  dest->stream->Write(dest->buffer, OUTPUT_BUF_SIZE);
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
  return TRUE;
}

METHODDEF(void) term_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
  /* Write any data remaining in the buffer */
  if (datacount > 0)
    dest->stream->Write(dest->buffer, datacount);
}

GLOBAL(void) jpeg_wxio_dest (j_compress_ptr cinfo, wxOutputStream& outfile)
{
  my_dest_ptr dest;

  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_destination_mgr));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->stream = &outfile;
}



bool wxJPEGHandler::SaveFile( wxImage *image, wxOutputStream& stream )
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  JSAMPLE *image_buffer;
  int stride;		        /* physical row width in image buffer */

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_wxio_dest(&cinfo, stream);

  cinfo.image_width = image->GetWidth();
  cinfo.image_height = image->GetHeight();
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_start_compress(&cinfo, TRUE);

  stride = cinfo.image_width * 3;	/* JSAMPLEs per row in image_buffer */
  image_buffer = image->GetData();
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &image_buffer[cinfo.next_scanline * stride];
    jpeg_write_scanlines( &cinfo, row_pointer, 1 );
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  return TRUE;
}
#endif // wxUSE_STREAMS

#endif

  // wxUSE_LIBJPEG



//-----------------------------------------------------------------------------
// wxBMPHandler
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBMPHandler,wxImageHandler)
#endif

#if wxUSE_STREAMS
bool wxBMPHandler::LoadFile( wxImage *image, wxInputStream& stream )
{
   unsigned char      *data, *ptr;
   int                 done, i, bpp, planes, comp, ncolors, line, column,
                       linesize, linepos, rshift = 0, gshift = 0, bshift = 0;
   unsigned char       aByte;
   short int           word;
   long int            dbuf[4], dword, rmask = 0, gmask = 0, bmask = 0, offset,
                       size;
   off_t               start_offset = stream.TellI();
   signed char         bbuf[4];
   struct _cmap
     {
        unsigned char       r, g, b;
     }
                      *cmap = NULL;
#ifndef BI_RGB
#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#endif

#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif

  image->Destroy();

   done = 0;
   /*
    * Reading the bmp header
    */

   stream.Read(&bbuf, 2);

   stream.Read(dbuf, 4 * 4);

   size = dbuf[0];
   offset = dbuf[2];

   stream.Read(dbuf, 4 * 2);
   int width = (int)dbuf[0];
   int height = (int)dbuf[1];
   if (width > 32767)
     {
        wxLogError( "Image width > 32767 pixels for file\n" );
        return FALSE;
     }
   if (height > 32767)
     {
        wxLogError( "Image height > 32767 pixels for file\n" );
        return FALSE;
     }
   stream.Read(&word, 2);
   planes = (int)word;
   stream.Read(&word, 2);
   bpp = (int)word;
   if (bpp != 1 && bpp != 4 && bpp != 8 && bpp && 16 && bpp != 24 && bpp != 32)
     {
        wxLogError( "unknown bitdepth in file\n" );
        return FALSE;
     }
   stream.Read(dbuf, 4 * 4);
   comp = (int)dbuf[0];
   if (comp != BI_RGB && comp != BI_RLE4 && comp != BI_RLE8 && comp != BI_BITFIELDS)
     {
        wxLogError( "unknown encoding in Windows BMP file\n" );
        return FALSE;
     }
   stream.Read(dbuf, 4 * 2);
   ncolors = (int)dbuf[0];
   if (ncolors == 0)
      ncolors = 1 << bpp;
   /* some more sanity checks */
   if (((comp == BI_RLE4) && (bpp != 4)) || ((comp == BI_RLE8) && (bpp != 8)) || ((comp == BI_BITFIELDS) && (bpp != 16 && bpp != 32)))
     {
        wxLogError( "encoding of BMP doesn't match bitdepth\n" );
        return FALSE;
     }
   if (bpp < 16)
     {
        cmap = (struct _cmap *)malloc(sizeof(struct _cmap) * ncolors);

        if (!cmap)
          {
             wxLogError( "Cannot allocate RAM for color map in BMP file\n" );
             return FALSE;
          }
     }
   else
      cmap = NULL;

   image->Create( width, height );
   ptr = image->GetData();
   if (!ptr)
     {
        wxLogError( "Cannot allocate RAM for RGB data in file\n" );
        if (cmap)
           free(cmap);
        return FALSE;
     }

   /*
    * Reading the palette, if it exists.
    */
   if (bpp < 16 && ncolors != 0)
     {
        for (i = 0; i < ncolors; i++)
          {
             stream.Read(bbuf, 4);
             cmap[i].b = bbuf[0];
             cmap[i].g = bbuf[1];
             cmap[i].r = bbuf[2];
          }
     }
   else if (bpp == 16 || bpp == 32)
     {
        if (comp == BI_BITFIELDS)
          {
             int                 bit = 0;

             stream.Read(dbuf, 4 * 3);
             bmask = dbuf[0];
             gmask = dbuf[1];
             rmask = dbuf[2];
             /* find shift amount.. ugly, but i can't think of a better way */
             for (bit = 0; bit < bpp; bit++)
               {
                  if (bmask & (1 << bit))
                     bshift = bit;
                  if (gmask & (1 << bit))
                     gshift = bit;
                  if (rmask & (1 << bit))
                     rshift = bit;
               }
          }
        else if (bpp == 16)
          {
             rmask = 0x7C00;
             gmask = 0x03E0;
             bmask = 0x001F;
             rshift = 10;
             gshift = 5;
             bshift = 0;
          }
        else if (bpp == 32)
          {
             rmask = 0x00FF0000;
             gmask = 0x0000FF00;
             bmask = 0x000000FF;
             rshift = 16;
             gshift = 8;
             bshift = 0;
          }
     }

   /*
    * Reading the image data
    */
   stream.SeekI(start_offset + offset);
   data = ptr;

   /* set the whole image to the background color */
   if (bpp < 16 && (comp == BI_RLE4 || comp == BI_RLE8))
     {
        for (i = 0; i < width * height; i++)
          {
             *ptr++ = cmap[0].r;
             *ptr++ = cmap[0].g;
             *ptr++ = cmap[0].b;
          }
        ptr = data;
     }
   line = 0;
   column = 0;
#define poffset (line * width * 3 + column * 3)

   /*
    * BMPs are stored upside down... hmmmmmmmmmm....
    */

   linesize = ((width * bpp + 31) / 32) * 4;
   for (line = (height - 1); line >= 0; line--)
     {
        linepos = 0;
        for (column = 0; column < width;)
          {
             if (bpp < 16)
               {
                  int                 index;

                  linepos++;
                  aByte = stream.GetC();
                  if (bpp == 1)
                    {
                       int                 bit = 0;

                       for (bit = 0; bit < 8; bit++)
                         {
                            index = ((aByte & (0x80 >> bit)) ? 1 : 0);
                            ptr[poffset] = cmap[index].r;
                            ptr[poffset + 1] = cmap[index].g;
                            ptr[poffset + 2] = cmap[index].b;
                            column++;
                         }
                    }
                  else if (bpp == 4)
                    {
                       if (comp == BI_RLE4)
                         {
                            wxLogError( "can't deal with 4bit encoded yet.\n");
                            image->Destroy();
                            free(cmap);
                            return FALSE;
                         }
                       else
                         {
                            int                 nibble = 0;

                            for (nibble = 0; nibble < 2; nibble++)
                              {
                                 index = ((aByte & (0xF0 >> nibble * 4)) >> (!nibble * 4));
                                 if (index >= 16)
                                    index = 15;
                                 ptr[poffset] = cmap[index].r;
                                 ptr[poffset + 1] = cmap[index].g;
                                 ptr[poffset + 2] = cmap[index].b;
                                 column++;
                              }
                         }
                    }
                  else if (bpp == 8)
                    {
                       if (comp == BI_RLE8)
                         {
                            unsigned char       first;

                            first = aByte;
                            aByte = stream.GetC();
                            if (first == 0)
                              {
                                 if (aByte == 0)
                                   {
/*                                    column = width; */
                                   }
                                 else if (aByte == 1)
                                   {
                                      column = width;
                                      line = -1;
                                   }
                                 else if (aByte == 2)
                                   {
                                      aByte = stream.GetC();
                                      column += aByte;
                                      linepos = column * bpp / 8;
                                      aByte = stream.GetC();
                                      line += aByte;
                                   }
                                 else
                                   {
                                      int                 absolute = aByte;

                                      for (i = 0; i < absolute; i++)
                                        {
                                           linepos++;
                                           aByte = stream.GetC();
                                           ptr[poffset] = cmap[aByte].r;
                                           ptr[poffset + 1] = cmap[aByte].g;
                                           ptr[poffset + 2] = cmap[aByte].b;
                                           column++;
                                        }
                                      if (absolute & 0x01)
                                         aByte = stream.GetC();
                                   }
                              }
                            else
                              {
                                 for (i = 0; i < first; i++)
                                   {
                                      ptr[poffset] = cmap[aByte].r;
                                      ptr[poffset + 1] = cmap[aByte].g;
                                      ptr[poffset + 2] = cmap[aByte].b;
                                      column++;
                                      linepos++;
                                   }
                              }
                         }
                       else
                         {
                            ptr[poffset] = cmap[aByte].r;
                            ptr[poffset + 1] = cmap[aByte].g;
                            ptr[poffset + 2] = cmap[aByte].b;
                            column++;
                            linepos += size;
                         }
                    }
               }
             else if (bpp == 24)
               {
              stream.Read(&bbuf, 3);
                  linepos += 3;
                  ptr[poffset] = (unsigned char)bbuf[2];
                  ptr[poffset + 1] = (unsigned char)bbuf[1];
                  ptr[poffset + 2] = (unsigned char)bbuf[0];
                  column++;
               }
             else if (bpp == 16)
               {
                  unsigned char       temp;

                  stream.Read(&word, 2);
                  linepos += 2;
                  temp = (word & rmask) >> rshift;
                  ptr[poffset] = temp;
                  temp = (word & gmask) >> gshift;
                  ptr[poffset + 1] = temp;
                  temp = (word & bmask) >> gshift;
                  ptr[poffset + 2] = temp;
                  column++;
               }
             else
               {
                  unsigned char       temp;

                  stream.Read(&dword, 4);
                  linepos += 4;
                  temp = (dword & rmask) >> rshift;
                  ptr[poffset] = temp;
                  temp = (dword & gmask) >> gshift;
                  ptr[poffset + 1] = temp;
                  temp = (dword & bmask) >> bshift;
                  ptr[poffset + 2] = temp;
                  column++;
               }
          }
        while ((linepos < linesize) && (comp != 1) && (comp != 2))
          {
             stream.Read(&aByte, 1);
             linepos += 1;
             if (stream.LastError() != wxStream_NOERROR)
                break;
          }
     }
   if (cmap) free(cmap);

   image->SetMask( FALSE );

   return TRUE;
}
#endif // wxUSE_STREAMS

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
    wxCHECK_MSG( Ok(), bitmap, "invalid image" );
    bitmap.SetWidth( width );
    bitmap.SetHeight( bmpHeight );
    bitmap.SetDepth( wxDisplayDepth() );

// create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, bitmap, "could not allocate memory for DIB header" );
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
        wxFAIL_MSG( "could not allocate memory for DIB" );
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
                for( int i=0; i<width; i++ )
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
        wxFAIL_MSG( "invalid bitmap" );
        return;
    }

// create an wxImage object
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();
    Create( width, height ); 
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
}

#endif

#ifdef __WXGTK__

#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, "invalid image" );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    // Create picture

    GdkImage *data_image =
      gdk_image_new( GDK_IMAGE_FASTEST, gdk_visual_get_system(), width, height );

    bitmap.SetPixmap( gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, -1 ) );

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

    // Retrieve depth

    GdkVisual *visual = gdk_window_get_visual( bitmap.GetPixmap() );
    if (visual == NULL) visual = gdk_window_get_visual( (GdkWindow*) &gdk_root_parent );
    int bpp = visual->depth;

    bitmap.SetDepth( bpp );

    if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;
    if (bpp < 8) bpp = 8;

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
    wxCHECK_RET( bitmap.Ok(), "invalid bitmap" );

    GdkImage *gdk_image = gdk_image_get( bitmap.GetPixmap(),
                                         0, 0,
                                         bitmap.GetWidth(), bitmap.GetHeight() );

    wxCHECK_RET( gdk_image, "couldn't create image" );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        gdk_image_destroy( gdk_image );
        wxFAIL_MSG( "couldn't create image" );
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

#ifdef __WXMOTIF__

#include <Xm/Xm.h>
#include "wx/utils.h"
#include <math.h>

wxBitmap wxImage::ConvertToBitmap() const
{
    wxBitmap bitmap;

    wxCHECK_MSG( Ok(), bitmap, "invalid image" );

    int width = GetWidth();
    int height = GetHeight();

    bitmap.SetHeight( height );
    bitmap.SetWidth( width );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    // Create image
    
    XImage *data_image = XCreateImage( dpy, vis, bpp, ZPixmap, 0, 0, width, height, 32, 0 );
    data_image->data = new char[ data_image->bytes_per_line * data_image->height ];

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
    
    if (!vi) 
    {
        printf("no visual.\n" );
        return wxNullBitmap;
    }
	
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
    wxCHECK_RET( bitmap.Ok(), "invalid bitmap" );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );
    
    XImage *ximage = XGetImage( dpy,
                                (Drawable)bitmap.GetPixmap(),
                                0, 0,
                                bitmap.GetWidth(), bitmap.GetHeight(),
				AllPlanes, ZPixmap );

    wxCHECK_RET( ximage, "couldn't create image" );

    Create( bitmap.GetWidth(), bitmap.GetHeight() );
    char unsigned *data = GetData();

    if (!data)
    {
        XDestroyImage( ximage );
        wxFAIL_MSG( "couldn't create image" );
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
    
    if (!vi) 
    {
        printf("no visual.\n" );
        return;
    }
	
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
