//
// iffdecod.h	- image handler for IFF/ILBM images
//
// (c) Steffen Gutmann, 2002
//
// Creation date: 08.01.2002
// Last modified: 12.01.2002
//

#ifndef WX_IIF_DECODE_H
#define WX_IIF_DECODE_H

#ifdef __GNUG__
#pragma interface "iffdecod.h"
#endif

#include "wx/setup.h"
#define wxUSE_IFF 1

#if wxUSE_STREAMS && wxUSE_IFF

#include "wx/stream.h"
#include "wx/image.h"

// --------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------

// Error codes:
//  Note that the error code wxIFF_TRUNCATED means that the image itself
//  is most probably OK, but the decoder didn't reach the end of the data
//  stream; this means that if it was not reading directly from file,
//  the stream will not be correctly positioned.
//

enum
{
    wxIFF_OK = 0,                   /* everything was OK */
    wxIFF_INVFORMAT,                /* error in iff header */
    wxIFF_MEMERR,                   /* error allocating memory */
    wxIFF_TRUNCATED                 /* file appears to be truncated */
};

// --------------------------------------------------------------------------
// wxIFFDecoder class
// --------------------------------------------------------------------------

// internal class for storing IFF image data
class IFFImage
{
public:
    unsigned int w;                 /* width */
    unsigned int h;                 /* height */
    int transparent;                /* transparent color (-1 = none) */
    int colors;			    /* number of colors */
    unsigned char *p;               /* bitmap */
    unsigned char *pal;             /* palette */

    IFFImage() : w(0), h(0), colors(0), p(0), pal(0) {}
    ~IFFImage() { delete [] p; delete [] pal; }
};

class WXDLLEXPORT wxIFFDecoder
{
private:
    IFFImage *m_image;		// image data
    wxInputStream *m_f; 	// input stream
    unsigned char *databuf; 
    unsigned char *picptr; 
    unsigned char *decomp_mem;

    void Destroy();

public:
    // get data of current frame
    unsigned char* GetData() const;
    unsigned char* GetPalette() const;
    int GetNumColors() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    int GetTransparentColour() const;

    // constructor, destructor, etc.
    wxIFFDecoder(wxInputStream *s);
    ~wxIFFDecoder() { Destroy(); }
    bool CanRead();
    int ReadIFF();
    bool ConvertToImage(wxImage *image) const;
};


#endif  // wxUSE_STREAM && wxUSE_IFF
#endif  // _WX_IFFDECOD_H

