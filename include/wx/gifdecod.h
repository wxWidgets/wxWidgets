/////////////////////////////////////////////////////////////////////////////
// Name:        gifdecod.h
// Purpose:     wxGIFDecoder, GIF reader for wxImage and wxAnimation
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     3.0
// Last rev:    1999/08/10
// Copyright:   (c) Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GIFDECOD_H
#define _WX_GIFDECOD_H

#ifdef __GNUG__
#pragma interface "gifdecod.h"
#endif

#include "wx/setup.h"

#if wxUSE_STREAMS
#include "wx/stream.h"
#include "wx/image.h"


typedef struct _IMAGEN
{
    unsigned int w;                 /* width */
    unsigned int h;                 /* height */
    unsigned int left;              /* x coord (in logical screen) */
    unsigned int top;               /* y coord (in logical screen) */
    int transparent;                /* transparent color (-1 = none) */
    int disposal;                   /* disposal method */
    long delay;                     /* delay in ms (-1 = unused) */
    unsigned char *p;               /* bitmap */
    unsigned char *pal;             /* palette */
    struct _IMAGEN *next;           /* next image */
    struct _IMAGEN *prev;           /* prev image */
} IMAGEN;


/* disposal method */
#define D_UNSPECIFIED   -1          /* not specified */
#define D_DONOTDISPOSE  0           /* do not dispose */
#define D_TOBACKGROUND  1           /* restore to background colour */
#define D_TOPREVIOUS    2           /* restore to previous image */

/* error codes */
#define E_OK            0           /* everything was OK */
#define E_ARCHIVO       -1          /* error opening file */
#define E_FORMATO       -2          /* error in gif header */
#define E_MEMORIA       -3          /* error allocating memory */


class wxGIFDecoder
{
private:
    /* logical screen */
    unsigned int  m_screenw;        /* logical screen width */
    unsigned int  m_screenh;        /* logical screen height */
    int           m_background;     /* background color (-1 = none) */

    /* image data */
    bool          m_anim;           /* animated GIF */
    int           m_nimages;        /* number of images */
    int           m_image;          /* current image */
    IMAGEN        *m_pimage;        /* pointer to current image */
    IMAGEN        *m_pfirst;        /* pointer to first image */
    IMAGEN        *m_plast;         /* pointer to last image */

    /* decoder state vars */
    int           m_restbits;       /* remaining valid bits */
    unsigned int  m_restbyte;       /* remaining bytes in this block */
    unsigned int  m_lastbyte;       /* last byte read */

    wxInputStream *m_f;             /* input file */

private:
    int getcode(int bits, int abfin);
    int dgif(IMAGEN *img, int interl, int bits);

public:
    // constructor, destructor, etc.
    wxGIFDecoder(wxInputStream *s, bool anim = FALSE);
    ~wxGIFDecoder();
    int ReadGIF();
    void Destroy();

    // convert current frame to wxImage
    bool ConvertToImage(wxImage *image) const;

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
};


#endif  // wxUSE_STREAM
#endif  // _WX_GIFDECOD_H

