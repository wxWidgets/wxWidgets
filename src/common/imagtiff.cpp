/////////////////////////////////////////////////////////////////////////////
// Name:        imagtiff.cpp
// Purpose:     wxImage TIFF handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imagtiff.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_LIBTIFF

#include "wx/imagtiff.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
extern "C"
{
    #include "tiff.h"
    #include "tiffio.h"
}
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

//-----------------------------------------------------------------------------
// wxTIFFHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTIFFHandler,wxImageHandler)

static tsize_t
_tiffNullProc(thandle_t WXUNUSED(handle),
	      tdata_t WXUNUSED(buf),
	      tsize_t WXUNUSED(size))
{
    return (tsize_t) -1;
}

static tsize_t
_tiffReadProc(thandle_t handle, tdata_t buf, tsize_t size)
{
    wxInputStream *stream = (wxInputStream*) handle;
    stream->Read( (void*) buf, (size_t) size );
    return stream->LastRead();
}

static tsize_t
_tiffWriteProc(thandle_t handle, tdata_t buf, tsize_t size)
{
    wxOutputStream *stream = (wxOutputStream*) handle;
    stream->Write( (void*) buf, (size_t) size );
    return stream->LastWrite();
}

static toff_t
_tiffSeekIProc(thandle_t handle, toff_t off, int whence)
{
    wxInputStream *stream = (wxInputStream*) handle;
    wxSeekMode mode;
    switch (whence)
    {
        case SEEK_SET: mode = wxFromStart; break;
        case SEEK_CUR: mode = wxFromCurrent; break;
        case SEEK_END: mode = wxFromEnd; break;
        default:       mode = wxFromCurrent; break;
    }

    return (toff_t)stream->SeekI( (off_t)off, mode );
}

static toff_t
_tiffSeekOProc(thandle_t handle, toff_t off, int whence)
{
    wxOutputStream *stream = (wxOutputStream*) handle;
    wxSeekMode mode;
    switch (whence)
    {
        case SEEK_SET: mode = wxFromStart; break;
        case SEEK_CUR: mode = wxFromCurrent; break;
        case SEEK_END: mode = wxFromEnd; break;
        default:       mode = wxFromCurrent; break;
    }

    return (toff_t)stream->SeekO( (off_t)off, mode );
}

static int
_tiffCloseProc(thandle_t WXUNUSED(handle))
{
    return 0;  // ?
}

static toff_t
_tiffSizeProc(thandle_t handle)
{
    wxStreamBase *stream = (wxStreamBase*) handle;
    return (toff_t) stream->GetSize();
}

static int
_tiffMapProc(thandle_t WXUNUSED(handle),
             tdata_t* WXUNUSED(pbase),
             toff_t* WXUNUSED(psize))
{
    return 0;
}

static void
_tiffUnmapProc(thandle_t WXUNUSED(handle),
               tdata_t WXUNUSED(base),
               toff_t WXUNUSED(size))
{
}

TIFF*
TIFFwxOpen(wxInputStream &stream, const char* name, const char* mode)
{
    TIFF* tif = TIFFClientOpen(name, mode,
        (thandle_t) &stream,
        _tiffReadProc, _tiffWriteProc,
        _tiffSeekIProc, _tiffCloseProc, _tiffSizeProc,
        _tiffMapProc, _tiffUnmapProc);

    return tif;
}

TIFF*
TIFFwxOpen(wxOutputStream &stream, const char* name, const char* mode)
{
    TIFF* tif = TIFFClientOpen(name, mode,
        (thandle_t) &stream,
        _tiffReadProc, _tiffWriteProc,
        _tiffSeekOProc, _tiffCloseProc, _tiffSizeProc,
        _tiffMapProc, _tiffUnmapProc);

    return tif;
}

bool wxTIFFHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int index )
{
    image->Destroy();

    TIFF *tif = TIFFwxOpen( stream, "image", "r" );

    if (!tif)
    {
        if (verbose)
            wxLogError( _("TIFF: Error loading image.") );

        return FALSE;
    }

    if (!TIFFSetDirectory( tif, (tdir_t)index ))
    {
        if (verbose)
            wxLogError( _("Invalid TIFF image index.") );

        TIFFClose( tif );

        return FALSE;
    }

    uint32 w, h;
    uint32 npixels;
    uint32 *raster;

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &w );
    TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &h );

    npixels = w * h;

    raster = (uint32*) _TIFFmalloc( npixels * sizeof(uint32) );

    if (!raster)
    {
        if (verbose)
            wxLogError( _("TIFF: Couldn't allocate memory.") );
            
        TIFFClose( tif );

        return FALSE;
    }

    image->Create( (int)w, (int)h );
    if (!image->Ok())
    {
        if (verbose)
            wxLogError( _("TIFF: Couldn't allocate memory.") );

        _TIFFfree( raster );
        TIFFClose( tif );

        return FALSE;
    }

    if (!TIFFReadRGBAImage( tif, w, h, raster, 0 ))
    {
        if (verbose)
            wxLogError( _("TIFF: Error reading image.") );

        _TIFFfree( raster );
        image->Destroy();
        TIFFClose( tif );

        return FALSE;
    }

    bool hasmask = FALSE;

    unsigned char *ptr = image->GetData();
    ptr += w*3*(h-1);
    uint32 pos = 0;

    for (uint32 i = 0; i < h; i++)
    {
        for (uint32 j = 0; j < w; j++)
        {
            unsigned char alpha = (unsigned char)TIFFGetA(raster[pos]);
            if (alpha < 127)
            {
                hasmask = TRUE;
                ptr[0] = image->GetMaskRed();
                ptr++;
                ptr[0] = image->GetMaskGreen();
                ptr++;
                ptr[0] = image->GetMaskBlue();
                ptr++;
            }
            else
            {
                ptr[0] = (unsigned char)TIFFGetR(raster[pos]);
                ptr++;
                ptr[0] = (unsigned char)TIFFGetG(raster[pos]);
                ptr++;
                ptr[0] = (unsigned char)TIFFGetB(raster[pos]);
                ptr++;
            }
            pos++;
        }
        ptr -= 2*w*3; // subtract line we just added plus one line
    }

    _TIFFfree( raster );

    TIFFClose( tif );

    image->SetMask( hasmask );

    return TRUE;
}

int wxTIFFHandler::GetImageCount( wxInputStream& stream )
{
    TIFF *tif = TIFFwxOpen( stream, "image", "r" );

    if (!tif)
        return 0;

    int dircount = 0;  // according to the libtiff docs, dircount should be set to 1 here???
    do {
        dircount++;
    } while (TIFFReadDirectory(tif));

    TIFFClose( tif );

    return dircount;
}

bool wxTIFFHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    TIFF *tif = TIFFwxOpen( stream, "image", "w" );

    if (!tif)
    {
        if (verbose)
            wxLogError( _("TIFF: Error saving image.") );

        return FALSE;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,  (uint32)image->GetWidth());
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32)image->GetHeight());
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    
    tsize_t linebytes = (tsize_t)image->GetWidth() * 3;
    unsigned char *buf;
    
    if (TIFFScanlineSize(tif) > linebytes) 
    {
        buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
        if (!buf)
        {
            if (verbose)
                wxLogError( _("TIFF: Couldn't allocate memory.") );

            TIFFClose( tif );

            return FALSE;
        }
    } 
    else 
    {
        buf = NULL;
    }

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
        TIFFDefaultStripSize(tif, (uint32) -1));
        
    unsigned char *ptr = image->GetData();
    for (int row = 0; row < image->GetHeight(); row++) 
    {
	    if (buf)
	        memcpy(buf, ptr, image->GetWidth());
        
	    if (TIFFWriteScanline(tif, buf ? buf : ptr, (uint32)row, 0) < 0)
        {
	        if (verbose)
	            wxLogError( _("TIFF: Error writing image.") );
        
            TIFFClose( tif );
            if (buf)
                _TIFFfree(buf);
                
            return FALSE;
        }
        ptr += image->GetWidth()*3;
    }

    (void) TIFFClose(tif);

    if (buf)
    _TIFFfree(buf);

    return TRUE;
}

bool wxTIFFHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[2];

    stream.Read(&hdr, 2);
    stream.SeekI(-2, wxFromCurrent);

    return ((hdr[0] == 0x49 && hdr[1] == 0x49) ||
            (hdr[0] == 0x4D && hdr[1] == 0x4D));
}


#endif
   // wxUSE_LIBTIFF


