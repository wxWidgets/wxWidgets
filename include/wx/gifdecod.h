/////////////////////////////////////////////////////////////////////////////
// Name:        gifdecod.h
// Purpose:     wxGIFDecoder, GIF reader for wxImage and wxAnimation
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     3.02
// CVS-ID:      $Id$
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GIFDECOD_H
#define _WX_GIFDECOD_H

#ifdef __GNUG__
#pragma interface "gifdecod.h"
#endif

#include "wx/setup.h"

#if wxUSE_STREAMS && wxUSE_GIF

#include "wx/stream.h"
#include "wx/image.h"


// --------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------

// Error codes:
//  Note that the error code wxGIF_TRUNCATED means that the image itself
//  is most probably OK, but the decoder didn't reach the end of the data
//  stream; this means that if it was not reading directly from file,
//  the stream will not be correctly positioned. the
//
enum
{
    wxGIF_OK = 0,                   /* everything was OK */
    wxGIF_INVFORMAT,                /* error in gif header */
    wxGIF_MEMERR,                   /* error allocating memory */
    wxGIF_TRUNCATED                 /* file appears to be truncated */
};

// Disposal method
//  Experimental; subject to change.
//
enum
{
    wxGIF_D_UNSPECIFIED = -1,       /* not specified */
    wxGIF_D_DONOTDISPOSE = 0,       /* do not dispose */
    wxGIF_D_TOBACKGROUND = 1,       /* restore to background colour */
    wxGIF_D_TOPREVIOUS = 2          /* restore to previous image */
};


#define MAX_BLOCK_SIZE 256          /* max. block size */


// --------------------------------------------------------------------------
// wxGIFDecoder class
// --------------------------------------------------------------------------

// internal class for storing GIF image data
class GIFImage
{
public:
    unsigned int w;                 /* width */
    unsigned int h;                 /* height */
    unsigned int left;              /* x coord (in logical screen) */
    unsigned int top;               /* y coord (in logical screen) */
    int transparent;                /* transparent color (-1 = none) */
    int disposal;                   /* disposal method (-1 = unspecified) */
    long delay;                     /* delay in ms (-1 = unused) */
    unsigned char *p;               /* bitmap */
    unsigned char *pal;             /* palette */
    GIFImage *next;                 /* next image */
    GIFImage *prev;                 /* prev image */
};


class WXDLLEXPORT wxGIFDecoder
{
private:
    // logical screen
    unsigned int  m_screenw;        /* logical screen width */
    unsigned int  m_screenh;        /* logical screen height */
    int           m_background;     /* background color (-1 = none) */

    // image data
    bool          m_anim;           /* animated GIF */
    int           m_nimages;        /* number of images */
    int           m_image;          /* current image */
    GIFImage      *m_pimage;        /* pointer to current image */
    GIFImage      *m_pfirst;        /* pointer to first image */
    GIFImage      *m_plast;         /* pointer to last image */

    // decoder state vars
    int           m_restbits;       /* remaining valid bits */
    unsigned int  m_restbyte;       /* remaining bytes in this block */
    unsigned int  m_lastbyte;       /* last byte read */
    unsigned char m_buffer[MAX_BLOCK_SIZE];     /* buffer for reading */
    unsigned char *m_bufp;          /* pointer to next byte in buffer */

    // input stream
    wxInputStream *m_f;             /* input stream */

private:
    int getcode(int bits, int abfin);
    int dgif(GIFImage *img, int interl, int bits);

protected:
    // get data of current frame
    int GetFrameIndex() const;
    unsigned char* GetData() const;
    unsigned char* GetPalette() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    unsigned int GetLeft() const;
    unsigned int GetTop() const;
    int GetDisposalMethod() const;
    int GetTransparentColour() const;
    long GetDelay() const;

    // get global data
    unsigned int GetLogicalScreenWidth() const;
    unsigned int GetLogicalScreenHeight() const;
    int GetBackgroundColour() const;
    int GetNumberOfFrames() const;
    bool IsAnimation() const;

    // move through the animation
    bool GoFirstFrame();
    bool GoLastFrame();
    bool GoNextFrame(bool cyclic = FALSE);
    bool GoPrevFrame(bool cyclic = FALSE);
    bool GoFrame(int which);

public:
    // constructor, destructor, etc.
    wxGIFDecoder(wxInputStream *s, bool anim = FALSE);
    ~wxGIFDecoder();
    bool CanRead();
    int ReadGIF();
    void Destroy();

    // convert current frame to wxImage
    bool ConvertToImage(wxImage *image) const;
};


#endif  // wxUSE_STREAM && wxUSE_GIF
#endif  // _WX_GIFDECOD_H

