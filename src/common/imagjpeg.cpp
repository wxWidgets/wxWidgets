/////////////////////////////////////////////////////////////////////////////
// Name:        imagjpeg.cpp
// Purpose:     wxImage JPEG handler
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imagjpeg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_LIBJPEG

#include "wx/imagjpeg.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
extern "C"
{
#ifdef __WATCOMC__
    #define HAVE_BOOLEAN
#endif
    #include "jpeglib.h"
}
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

// For memcpy
#include <string.h>
// For JPEG library error handling
#include <setjmp.h>

#ifdef __SALFORDC__
#ifdef FAR
#undef FAR
#endif
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxJPEGHandler,wxImageHandler)

#if wxUSE_STREAMS

//------------- JPEG Data Source Manager

#define JPEG_IO_BUFFER_SIZE   2048

typedef struct {
    struct jpeg_source_mgr pub;   /* public fields */

    JOCTET* buffer;               /* start of buffer */
    wxInputStream *stream;
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

METHODDEF(void) my_init_source ( j_decompress_ptr WXUNUSED(cinfo) )
{
}

METHODDEF(boolean) my_fill_input_buffer ( j_decompress_ptr cinfo )
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = src->stream->Read(src->buffer, JPEG_IO_BUFFER_SIZE).LastRead();

    if (src->pub.bytes_in_buffer == 0) // check for end-of-stream
    {
        // Insert a fake EOI marker
        src->buffer[0] = 0xFF;
        src->buffer[1] = JPEG_EOI;
        src->pub.bytes_in_buffer = 2;
    }
    return TRUE;
}

METHODDEF(void) my_skip_input_data ( j_decompress_ptr cinfo, long num_bytes )
{
    if (num_bytes > 0)
    {
        my_src_ptr src = (my_src_ptr) cinfo->src;

        while (num_bytes > (long)src->pub.bytes_in_buffer) 
        {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            src->pub.fill_input_buffer(cinfo);
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

METHODDEF(void) my_term_source ( j_decompress_ptr cinfo )
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    if (src->pub.bytes_in_buffer > 0)
        src->stream->SeekI(-(long)src->pub.bytes_in_buffer, wxFromCurrent);
    delete[] src->buffer;
}

void jpeg_wxio_src( j_decompress_ptr cinfo, wxInputStream& infile )
{
    my_src_ptr src;

    if (cinfo->src == NULL) {    /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(my_source_mgr));
        src = (my_src_ptr) cinfo->src;
    }
    src = (my_src_ptr) cinfo->src;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->buffer = new JOCTET[JPEG_IO_BUFFER_SIZE];
    src->pub.next_input_byte = NULL; /* until buffer loaded */
    src->stream = &infile;

    src->pub.init_source = my_init_source;
    src->pub.fill_input_buffer = my_fill_input_buffer;
    src->pub.skip_input_data = my_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = my_term_source;
}


// JPEG error manager:

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  if (cinfo->err->output_message) (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

// temporarily disable the warning C4611 (interaction between '_setjmp' and
// C++ object destruction is non-portable) - I don't see any dtors here
#ifdef __VISUALC__
    #pragma warning(disable:4611)
#endif /* VC++ */

bool wxJPEGHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY tempbuf;
    unsigned char *ptr;
    unsigned stride;

    image->Destroy();
    cinfo.err = jpeg_std_error( &jerr.pub );
    jerr.pub.error_exit = my_error_exit;

    if (!verbose) cinfo.err->output_message=NULL;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
      /* If we get here, the JPEG code has signaled an error.
       * We need to clean up the JPEG object, close the input file, and return.
       */
      if (verbose) 
        wxLogError(_("JPEG: Couldn't load - file is probably corrupted."));
      jpeg_destroy_decompress(&cinfo);
      if (image->Ok()) image->Destroy();
      return FALSE;
    }

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

#define OUTPUT_BUF_SIZE  4096    /* choose an efficiently fwrite'able size */

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

    if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
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

bool wxJPEGHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
    JSAMPLE *image_buffer;
    int stride;                /* physical row width in image buffer */

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (!verbose) cinfo.err->output_message=NULL;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) 
    {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
         if (verbose) 
            wxLogError(_("JPEG: Couldn't save image."));
         jpeg_destroy_compress(&cinfo);
         return FALSE;
    }

    jpeg_create_compress(&cinfo);
    jpeg_wxio_dest(&cinfo, stream);

    cinfo.image_width = image->GetWidth();
    cinfo.image_height = image->GetHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    stride = cinfo.image_width * 3;    /* JSAMPLEs per row in image_buffer */
    image_buffer = image->GetData();
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * stride];
        jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return TRUE;
}

#ifdef __VISUALC__
    #pragma warning(default:4611)
#endif /* VC++ */

bool wxJPEGHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[2];

    stream.Read(&hdr, 2);
    stream.SeekI(-2, wxFromCurrent);
    return (hdr[0] == 0xFF && hdr[1] == 0xD8);
}

#endif   // wxUSE_STREAMS

#endif   // wxUSE_LIBJPEG






