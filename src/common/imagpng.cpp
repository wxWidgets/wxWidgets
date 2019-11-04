/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/imagpng.cpp
// Purpose:     wxImage PNG handler
// Author:      Robert Roebling
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_IMAGE && wxUSE_LIBPNG

#include "wx/imagpng.h"
#include "wx/versioninfo.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/palette.h"
    #include "wx/stream.h"
#endif

#include "png.h"

// For memcpy
#include <string.h>

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

// is the pixel with this value of alpha a fully opaque one?
static inline
bool IsOpaque(unsigned char a)
{
    return a == 0xff;
}

// ============================================================================
// wxPNGHandler implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxPNGHandler,wxImageHandler);

#if wxUSE_STREAMS

#ifndef PNGLINKAGEMODE
    #ifdef PNGAPI
        #define PNGLINKAGEMODE PNGAPI
    #else
        #define PNGLINKAGEMODE LINKAGEMODE
    #endif
#endif

namespace
{

// VS: wxPNGInfoStruct declared below is a hack that needs some explanation.
//     First, let me describe what's the problem: libpng uses jmp_buf in
//     its png_struct structure. Unfortunately, this structure is
//     compiler-specific and may vary in size, so if you use libpng compiled
//     as DLL with another compiler than the main executable, it may not work.
//     Luckily, it is still possible to use setjmp() & longjmp() as long as the
//     structure is not part of png_struct.
//
//     Sadly, there's no clean way to attach user-defined data to png_struct.
//     There is only one customizable place, png_struct.io_ptr, which is meant
//     only for I/O routines and is set with png_set_read_fn or
//     png_set_write_fn. The hacky part is that we use io_ptr to store
//     a pointer to wxPNGInfoStruct that holds I/O structures _and_ jmp_buf.

struct wxPNGInfoStruct
{
    jmp_buf jmpbuf;
    bool verbose;

    union
    {
        wxInputStream  *in;
        wxOutputStream *out;
    } stream;

};

#define WX_PNG_INFO(png_ptr) ((wxPNGInfoStruct*)png_get_io_ptr(png_ptr))

// This is another helper struct which is used to pass parameters to
// DoLoadPNGFile(). It allows us to use the usual RAII for freeing memory,
// which wouldn't be possible inside DoLoadPNGFile() because it uses
// setjmp/longjmp() functions for error handling, which are incompatible with
// C++ destructors.
struct wxPNGImageData
{
    wxPNGImageData()
    {
        lines = NULL;
        m_buf = NULL;
        info_ptr = (png_infop) NULL;
        png_ptr = (png_structp) NULL;
        ok = false;
    }

    bool Alloc(png_uint_32 width, png_uint_32 height, unsigned char* buf)
    {
        lines = (unsigned char **)malloc(height * sizeof(unsigned char *));
        if ( !lines )
            return false;

        size_t w = width;
        // if RGB data will be written directly to wxImage buffer
        if (buf)
            w *= 3;
        else
        {
            // allocate intermediate RGBA buffer
            w *= 4;
            buf =
            m_buf = static_cast<unsigned char*>(malloc(w * height));
            if (!m_buf)
                return false;
        }

        lines[0] = buf;
        for (png_uint_32 i = 1; i < height; i++)
            lines[i] = lines[i - 1] + w;

        return true;
    }

    ~wxPNGImageData()
    {
        free(m_buf);
        free( lines );

        if ( png_ptr )
        {
            if ( info_ptr )
                png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
            else
                png_destroy_read_struct( &png_ptr, (png_infopp) NULL, (png_infopp) NULL );
        }
    }

    void DoLoadPNGFile(wxImage* image, wxPNGInfoStruct& wxinfo);

    unsigned char** lines;
    unsigned char* m_buf;
    png_infop info_ptr;
    png_structp png_ptr;
    bool ok;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

extern "C"
{

static void PNGLINKAGEMODE wx_PNG_stream_reader( png_structp png_ptr, png_bytep data,
                                                 png_size_t length )
{
    WX_PNG_INFO(png_ptr)->stream.in->Read(data, length);
}

static void PNGLINKAGEMODE wx_PNG_stream_writer( png_structp png_ptr, png_bytep data,
                                                 png_size_t length )
{
    WX_PNG_INFO(png_ptr)->stream.out->Write(data, length);
}

static void
PNGLINKAGEMODE wx_PNG_warning(png_structp png_ptr, png_const_charp message)
{
    wxPNGInfoStruct *info = png_ptr ? WX_PNG_INFO(png_ptr) : NULL;
    if ( !info || info->verbose )
    {
        wxLogWarning( wxString::FromAscii(message) );
    }
}

// from pngerror.c
// so that the libpng doesn't send anything on stderr
static void
PNGLINKAGEMODE wx_PNG_error(png_structp png_ptr, png_const_charp message)
{
    wx_PNG_warning(NULL, message);

    // we're not using libpng built-in jump buffer (see comment before
    // wxPNGInfoStruct above) so we have to return ourselves, otherwise libpng
    // would just abort
    longjmp(WX_PNG_INFO(png_ptr)->jmpbuf, 1);
}

} // extern "C"

// ----------------------------------------------------------------------------
// LoadFile() helpers
// ----------------------------------------------------------------------------

// init the alpha channel for the image and fill it with 1s up to (x, y)
static
unsigned char *InitAlpha(wxImage *image, png_uint_32 x, png_uint_32 y)
{
    // create alpha channel
    image->SetAlpha();

    unsigned char *alpha = image->GetAlpha();

    // set alpha for the pixels we had so far
    png_uint_32 end = y * image->GetWidth() + x;
    // all the previous pixels were opaque
    memset(alpha, 0xff, end);

    return alpha + end;
}

// ----------------------------------------------------------------------------
// reading PNGs
// ----------------------------------------------------------------------------

bool wxPNGHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[4];

    if ( !stream.Read(hdr, WXSIZEOF(hdr)) )     // it's ok to modify the stream position here
        return false;

    return memcmp(hdr, "\211PNG", WXSIZEOF(hdr)) == 0;
}

// convert data from RGB to wxImage format
static
void CopyDataFromPNG(wxImage *image,
                     unsigned char **lines,
                     png_uint_32 width,
                     png_uint_32 height)
{
    // allocated on demand if we have any non-opaque pixels
    unsigned char *alpha = NULL;

    unsigned char *ptrDst = image->GetData();
    {
        for ( png_uint_32 y = 0; y < height; y++ )
        {
            const unsigned char *ptrSrc = lines[y];
            for ( png_uint_32 x = 0; x < width; x++ )
            {
                unsigned char r = *ptrSrc++;
                unsigned char g = *ptrSrc++;
                unsigned char b = *ptrSrc++;
                unsigned char a = *ptrSrc++;

                // the first time we encounter a transparent pixel we must
                // allocate alpha channel for the image
                if ( !IsOpaque(a) && !alpha )
                    alpha = InitAlpha(image, x, y);

                if ( alpha )
                    *alpha++ = a;

                *ptrDst++ = r;
                *ptrDst++ = g;
                *ptrDst++ = b;
            }
        }
    }
}

// temporarily disable the warning C4611 (interaction between '_setjmp' and
// C++ object destruction is non-portable) - I don't see any dtors here
#ifdef __VISUALC__
    #pragma warning(disable:4611)
#endif /* VC++ */

// This function uses wxPNGImageData to store some of its "local" variables in
// order to avoid clobbering these variables by longjmp(): having them inside
// the stack frame of the caller prevents this from happening. It also
// "returns" its result via wxPNGImageData: use its "ok" field to check
// whether loading succeeded or failed.
void
wxPNGImageData::DoLoadPNGFile(wxImage* image, wxPNGInfoStruct& wxinfo)
{
    png_uint_32 width, height = 0;
    int bit_depth, color_type;

    image->Destroy();

    png_ptr = png_create_read_struct
                          (
                            PNG_LIBPNG_VER_STRING,
                            NULL,
                            wx_PNG_error,
                            wx_PNG_warning
                          );
    if (!png_ptr)
        return;

    // NB: please see the comment near wxPNGInfoStruct declaration for
    //     explanation why this line is mandatory
    png_set_read_fn( png_ptr, &wxinfo, wx_PNG_stream_reader);

    info_ptr = png_create_info_struct( png_ptr );
    if (!info_ptr)
        return;

    if (setjmp(wxinfo.jmpbuf))
        return;

    png_read_info( png_ptr, info_ptr );
    png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL );

    png_set_expand(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_strip_16( png_ptr );
    png_set_packing( png_ptr );

    image->Create((int)width, (int)height, (bool) false /* no need to init pixels */);

    if (!image->IsOk())
        return;

    const bool needCopy =
        (color_type & PNG_COLOR_MASK_ALPHA) ||
        png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);

    if (!Alloc(width, height, needCopy ? NULL : image->GetData()))
        return;

    png_read_image( png_ptr, lines );
    png_read_end( png_ptr, info_ptr );

#if wxUSE_PALETTE
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_colorp palette = NULL;
        int numPalette = 0;

        (void) png_get_PLTE(png_ptr, info_ptr, &palette, &numPalette);

        unsigned char* r = new unsigned char[numPalette];
        unsigned char* g = new unsigned char[numPalette];
        unsigned char* b = new unsigned char[numPalette];

        for (int j = 0; j < numPalette; j++)
        {
            r[j] = palette[j].red;
            g[j] = palette[j].green;
            b[j] = palette[j].blue;
        }

        image->SetPalette(wxPalette(numPalette, r, g, b));
        delete[] r;
        delete[] g;
        delete[] b;
    }
#endif // wxUSE_PALETTE


    // set the image resolution if it's available
    png_uint_32 resX, resY;
    int unitType;
    if (png_get_pHYs(png_ptr, info_ptr, &resX, &resY, &unitType)
        == PNG_INFO_pHYs)
    {
        wxImageResolution res = wxIMAGE_RESOLUTION_CM;

        switch (unitType)
        {
            default:
                wxLogWarning(_("Unknown PNG resolution unit %d"), unitType);
                wxFALLTHROUGH;

            case PNG_RESOLUTION_UNKNOWN:
                image->SetOption(wxIMAGE_OPTION_RESOLUTIONX, resX);
                image->SetOption(wxIMAGE_OPTION_RESOLUTIONY, resY);

                res = wxIMAGE_RESOLUTION_NONE;
                break;

            case PNG_RESOLUTION_METER:
                /*
                Convert meters to centimeters.
                Use a string to not lose precision (converting to cm and then
                to inch would result in integer rounding error).
                If an app wants an int, GetOptionInt will convert and round
                down for them.
                */
                image->SetOption(wxIMAGE_OPTION_RESOLUTIONX,
                    wxString::FromCDouble((double) resX / 100.0, 2));
                image->SetOption(wxIMAGE_OPTION_RESOLUTIONY,
                    wxString::FromCDouble((double) resY / 100.0, 2));
                break;
        }

        image->SetOption(wxIMAGE_OPTION_RESOLUTIONUNIT, res);
    }


    // loaded successfully, now init wxImage with this data
    if (needCopy)
        CopyDataFromPNG(image, lines, width, height);

    // This will indicate to the caller that loading succeeded.
    ok = true;
}

bool
wxPNGHandler::LoadFile(wxImage *image,
                       wxInputStream& stream,
                       bool verbose,
                       int WXUNUSED(index))
{
    wxPNGInfoStruct wxinfo;
    wxinfo.verbose = verbose;
    wxinfo.stream.in = &stream;

    wxPNGImageData data;
    data.DoLoadPNGFile(image, wxinfo);

    if ( !data.ok )
    {
        if (verbose)
        {
           wxLogError(_("Couldn't load a PNG image - file is corrupted or not enough memory."));
        }

        if ( image->IsOk() )
        {
            image->Destroy();
        }

        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// SaveFile() palette helpers
// ----------------------------------------------------------------------------

typedef wxLongToLongHashMap PaletteMap;

static unsigned long PaletteMakeKey(const png_color_8& clr)
{
    return (wxImageHistogram::MakeKey(clr.red, clr.green, clr.blue) << 8) | clr.alpha;
}

static long PaletteFind(const PaletteMap& palette, const png_color_8& clr)
{
    unsigned long value = PaletteMakeKey(clr);
    PaletteMap::const_iterator it = palette.find(value);

    return (it != palette.end()) ? it->second : wxNOT_FOUND;
}

static long PaletteAdd(PaletteMap *palette, const png_color_8& clr)
{
    unsigned long value = PaletteMakeKey(clr);
    PaletteMap::const_iterator it = palette->find(value);
    size_t index;

    if (it == palette->end())
    {
        index = palette->size();
        (*palette)[value] = index;
    }
    else
    {
        index = it->second;
    }

    return index;
}

// ----------------------------------------------------------------------------
// writing PNGs
// ----------------------------------------------------------------------------

bool wxPNGHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    wxPNGInfoStruct wxinfo;

    wxinfo.verbose = verbose;
    wxinfo.stream.out = &stream;

    png_structp png_ptr = png_create_write_struct
                          (
                            PNG_LIBPNG_VER_STRING,
                            NULL,
                            wx_PNG_error,
                            wx_PNG_warning
                          );
    if (!png_ptr)
    {
        if (verbose)
        {
           wxLogError(_("Couldn't save PNG image."));
        }
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
        if (verbose)
        {
           wxLogError(_("Couldn't save PNG image."));
        }
        return false;
    }

    if (setjmp(wxinfo.jmpbuf))
    {
        png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
        if (verbose)
        {
           wxLogError(_("Couldn't save PNG image."));
        }
        return false;
    }

    // NB: please see the comment near wxPNGInfoStruct declaration for
    //     explanation why this line is mandatory
    png_set_write_fn( png_ptr, &wxinfo, wx_PNG_stream_writer, NULL);

    const int iHeight = image->GetHeight();
    const int iWidth = image->GetWidth();

    const bool bHasPngFormatOption
        = image->HasOption(wxIMAGE_OPTION_PNG_FORMAT);

    int iColorType = bHasPngFormatOption
                            ? image->GetOptionInt(wxIMAGE_OPTION_PNG_FORMAT)
                            : wxPNG_TYPE_COLOUR;

    bool bHasAlpha = image->HasAlpha();
    bool bHasMask = image->HasMask();

    bool bUsePalette = iColorType == wxPNG_TYPE_PALETTE
#if wxUSE_PALETTE
        || (!bHasPngFormatOption && image->HasPalette() )
#endif
    ;

    png_color_8 mask = { 0, 0, 0, 0, 0 };

    if (bHasMask)
    {
        mask.red   = image->GetMaskRed();
        mask.green = image->GetMaskGreen();
        mask.blue  = image->GetMaskBlue();
    }

    PaletteMap palette;

    if (bUsePalette)
    {
        png_color png_rgb  [PNG_MAX_PALETTE_LENGTH];
        png_byte  png_trans[PNG_MAX_PALETTE_LENGTH];

        const unsigned char *pColors = image->GetData();
        const unsigned char* pAlpha  = image->GetAlpha();

        if (bHasMask && !pAlpha)
        {
            // Mask must be first
            PaletteAdd(&palette, mask);
        }

        for (int y = 0; y < iHeight; y++)
        {
            for (int x = 0; x < iWidth; x++)
            {
                png_color_8 rgba;

                rgba.red   = *pColors++;
                rgba.green = *pColors++;
                rgba.blue  = *pColors++;
                rgba.gray  = 0;
                rgba.alpha = (pAlpha && !bHasMask) ? *pAlpha++ : 0;

                // save in our palette
                long index = PaletteAdd(&palette, rgba);

                if (index < PNG_MAX_PALETTE_LENGTH)
                {
                    // save in libpng's palette
                    png_rgb[index].red   = rgba.red;
                    png_rgb[index].green = rgba.green;
                    png_rgb[index].blue  = rgba.blue;
                    png_trans[index]     = rgba.alpha;
                }
                else
                {
                    bUsePalette = false;
                    break;
                }
            }
        }

        if (bUsePalette)
        {
            png_set_PLTE(png_ptr, info_ptr, png_rgb, palette.size());

            if (bHasMask && !pAlpha)
            {
                wxASSERT(PaletteFind(palette, mask) == 0);
                png_trans[0] = 0;
                png_set_tRNS(png_ptr, info_ptr, png_trans, 1, NULL);
            }
            else if (pAlpha && !bHasMask)
            {
                png_set_tRNS(png_ptr, info_ptr, png_trans, palette.size(), NULL);
            }
        }
    }

    /*
    If saving palettised was requested but it was decided we can't use a
    palette then reset the colour type to RGB.
    */
    if (!bUsePalette && iColorType == wxPNG_TYPE_PALETTE)
    {
        iColorType = wxPNG_TYPE_COLOUR;
    }

    bool bUseAlpha = !bUsePalette && (bHasAlpha || bHasMask);

    int iPngColorType;

    if (bUsePalette)
    {
        iPngColorType = PNG_COLOR_TYPE_PALETTE;
        iColorType = wxPNG_TYPE_PALETTE;
    }
    else if ( iColorType==wxPNG_TYPE_COLOUR )
    {
        iPngColorType = bUseAlpha ? PNG_COLOR_TYPE_RGB_ALPHA
                                  : PNG_COLOR_TYPE_RGB;
    }
    else
    {
        iPngColorType = bUseAlpha ? PNG_COLOR_TYPE_GRAY_ALPHA
                                  : PNG_COLOR_TYPE_GRAY;
    }

    if (image->HasOption(wxIMAGE_OPTION_PNG_FILTER))
        png_set_filter( png_ptr, PNG_FILTER_TYPE_BASE, image->GetOptionInt(wxIMAGE_OPTION_PNG_FILTER) );

    if (image->HasOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL))
        png_set_compression_level( png_ptr, image->GetOptionInt(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL) );

    if (image->HasOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL))
        png_set_compression_mem_level( png_ptr, image->GetOptionInt(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL) );

    if (image->HasOption(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY))
        png_set_compression_strategy( png_ptr, image->GetOptionInt(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY) );

    if (image->HasOption(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE))
        png_set_compression_buffer_size( png_ptr, image->GetOptionInt(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE) );

    int iBitDepth = !bUsePalette && image->HasOption(wxIMAGE_OPTION_PNG_BITDEPTH)
                            ? image->GetOptionInt(wxIMAGE_OPTION_PNG_BITDEPTH)
                            : 8;

    png_set_IHDR( png_ptr, info_ptr, image->GetWidth(), image->GetHeight(),
                  iBitDepth, iPngColorType,
                  PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                  PNG_FILTER_TYPE_BASE);

    int iElements;
    png_color_8 sig_bit;

    if ( iPngColorType & PNG_COLOR_MASK_COLOR )
    {
        sig_bit.red =
        sig_bit.green =
        sig_bit.blue = (png_byte)iBitDepth;
        iElements = 3;
    }
    else // grey
    {
        sig_bit.gray = (png_byte)iBitDepth;
        iElements = 1;
    }

    if ( bUseAlpha )
    {
        sig_bit.alpha = (png_byte)iBitDepth;
        iElements++;
    }

    if ( iBitDepth == 16 )
        iElements *= 2;

    // save the image resolution if we have it
    int resX, resY;
    switch ( GetResolutionFromOptions(*image, &resX, &resY) )
    {
        case wxIMAGE_RESOLUTION_INCHES:
            {
                const double INCHES_IN_METER = 10000.0 / 254;
                resX = int(resX * INCHES_IN_METER);
                resY = int(resY * INCHES_IN_METER);
            }
            break;

        case wxIMAGE_RESOLUTION_CM:
            resX *= 100;
            resY *= 100;
            break;

        case wxIMAGE_RESOLUTION_NONE:
            break;

        default:
            wxFAIL_MSG( wxT("unsupported image resolution units") );
    }

    if ( resX && resY )
        png_set_pHYs( png_ptr, info_ptr, resX, resY, PNG_RESOLUTION_METER );

    png_set_sBIT( png_ptr, info_ptr, &sig_bit );
    png_write_info( png_ptr, info_ptr );
    png_set_shift( png_ptr, &sig_bit );
    png_set_packing( png_ptr );

    unsigned char *
        data = (unsigned char *)malloc( image->GetWidth() * iElements );
    if ( !data )
    {
        png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
        return false;
    }

    const unsigned char *
        pAlpha = (const unsigned char *)(bHasAlpha ? image->GetAlpha() : NULL);

    const unsigned char *pColors = image->GetData();

    for (int y = 0; y != iHeight; ++y)
    {
        unsigned char *pData = data;
        for (int x = 0; x != iWidth; x++)
        {
            png_color_8 clr;
            clr.red   = *pColors++;
            clr.green = *pColors++;
            clr.blue  = *pColors++;
            clr.gray  = 0;
            clr.alpha = (bUsePalette && pAlpha) ? *pAlpha++ : 0; // use with wxPNG_TYPE_PALETTE only

            switch ( iColorType )
            {
                default:
                    wxFAIL_MSG( wxT("unknown wxPNG_TYPE_XXX") );
                    wxFALLTHROUGH;

                case wxPNG_TYPE_COLOUR:
                    *pData++ = clr.red;
                    if ( iBitDepth == 16 )
                        *pData++ = 0;
                    *pData++ = clr.green;
                    if ( iBitDepth == 16 )
                        *pData++ = 0;
                    *pData++ = clr.blue;
                    if ( iBitDepth == 16 )
                        *pData++ = 0;
                    break;

                case wxPNG_TYPE_GREY:
                    {
                        // where do these coefficients come from? maybe we
                        // should have image options for them as well?
                        unsigned uiColor =
                            (unsigned) (76.544*(unsigned)clr.red +
                                        150.272*(unsigned)clr.green +
                                        36.864*(unsigned)clr.blue);

                        *pData++ = (unsigned char)((uiColor >> 8) & 0xFF);
                        if ( iBitDepth == 16 )
                            *pData++ = (unsigned char)(uiColor & 0xFF);
                    }
                    break;

                case wxPNG_TYPE_GREY_RED:
                    *pData++ = clr.red;
                    if ( iBitDepth == 16 )
                        *pData++ = 0;
                    break;

                case wxPNG_TYPE_PALETTE:
                    *pData++ = (unsigned char) PaletteFind(palette, clr);
                    break;
            }

            if ( bUseAlpha )
            {
                unsigned char uchAlpha = 255;
                if ( bHasAlpha )
                    uchAlpha = *pAlpha++;

                if ( bHasMask )
                {
                    if ( (clr.red == mask.red)
                            && (clr.green == mask.green)
                                && (clr.blue == mask.blue) )
                        uchAlpha = 0;
                }

                *pData++ = uchAlpha;
                if ( iBitDepth == 16 )
                    *pData++ = 0;
            }
        }

        png_bytep row_ptr = data;
        png_write_rows( png_ptr, &row_ptr, 1 );
    }

    free(data);
    png_write_end( png_ptr, info_ptr );
    png_destroy_write_struct( &png_ptr, (png_infopp)&info_ptr );

    return true;
}

#ifdef __VISUALC__
    #pragma warning(default:4611)
#endif /* VC++ */

#endif  // wxUSE_STREAMS

/*static*/ wxVersionInfo wxPNGHandler::GetLibraryVersionInfo()
{
    // The version string seems to always have a leading space and a trailing
    // new line, get rid of them both.
    wxString str = png_get_header_version(NULL) + 1;
    str.Replace("\n", "");

    return wxVersionInfo("libpng",
                         PNG_LIBPNG_VER_MAJOR,
                         PNG_LIBPNG_VER_MINOR,
                         PNG_LIBPNG_VER_RELEASE,
                         str);
}

#endif  // wxUSE_LIBPNG
