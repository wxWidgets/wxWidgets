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
    #include "tiffiop.h"
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
_tiffSeekProc(thandle_t handle, toff_t off, int whence)
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

static int
_tiffCloseProc(thandle_t WXUNUSED(handle))
{
    return 0;  // ?
}

static toff_t
_tiffSizeProc(thandle_t handle)
{
    wxInputStream *stream = (wxInputStream*) handle;
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
        _tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
        _tiffMapProc, _tiffUnmapProc);

    if (tif)
        tif->tif_fd = (int) &stream;

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

        return FALSE;
    }

    image->Create( w, h );
    if (!image->Ok())
    {
        if (verbose)
            wxLogError( _("TIFF: Couldn't allocate memory.") );

        _TIFFfree( raster );

        return FALSE;
    }

    if (!TIFFReadRGBAImage( tif, w, h, raster, 0 ))
    {
        if (verbose)
            wxLogError( _("TIFF: Error reading image.") );

        _TIFFfree( raster );
        image->Destroy();

        return FALSE;
    }

    bool hasmask = FALSE;

    unsigned char *ptr = image->GetData();
    uint32 pos = 0;

    for (uint32 i = 0; i < h; i++)
    {
        for (uint32 j = 0; w < h; j++)
        {
            unsigned char alpha = (unsigned char)(raster[pos] >> 24);
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
                ptr[0] = (unsigned char)(raster[pos] >> 16);
                ptr++;
                ptr[0] = (unsigned char)(raster[pos] >> 8);
                ptr++;
                ptr[0] = (unsigned char)(raster[pos]);
                ptr++;
            }
            pos++;
        }
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

bool wxTIFFHandler::SaveFile( wxImage *WXUNUSED(image), wxOutputStream& WXUNUSED(stream), bool WXUNUSED(verbose) )
{
    return FALSE;
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






