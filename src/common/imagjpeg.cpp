/////////////////////////////////////////////////////////////////////////////
// Name:        imagjpeg.cpp
// Purpose:     wxImage JPEG handler
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
#if wxUSE_LIBJPEG
extern "C" {
#include <jpeglib.h>
}
#endif
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

// For memcpy
#include <string.h>

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

