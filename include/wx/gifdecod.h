/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gifdecod.h
// Purpose:     wxGIFDecoder, GIF reader for wxImage and wxAnimation
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     3.02
// CVS-ID:      $Id$
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GIFDECOD_H
#define _WX_GIFDECOD_H

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_GIF

#include "wx/stream.h"
#include "wx/image.h"
#include "wx/animdecod.h"

// internal utility used to store a frame in 8bit-per-pixel format
class /*WXDLLEXPORT*/ GIFImage;


// --------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------

// Error codes:
//  Note that the error code wxGIF_TRUNCATED means that the image itself
//  is most probably OK, but the decoder didn't reach the end of the data
//  stream; this means that if it was not reading directly from file,
//  the stream will not be correctly positioned. the
//
enum wxGIFErrorCode
{
    wxGIF_OK = 0,                   /* everything was OK */
    wxGIF_INVFORMAT,                /* error in gif header */
    wxGIF_MEMERR,                   /* error allocating memory */
    wxGIF_TRUNCATED                 /* file appears to be truncated */
};

#define MAX_BLOCK_SIZE 256          /* max. block size */


// --------------------------------------------------------------------------
// wxGIFDecoder class
// --------------------------------------------------------------------------

class WXDLLEXPORT wxGIFDecoder : public wxAnimationDecoder
{
private:
    // a wxArray provides a constant access time rather than a linear time
    // like for linked lists.
    wxArrayPtrVoid m_frames;

    // decoder state vars
    int           m_restbits;       /* remaining valid bits */
    unsigned int  m_restbyte;       /* remaining bytes in this block */
    unsigned int  m_lastbyte;       /* last byte read */
    unsigned char m_buffer[MAX_BLOCK_SIZE];     /* buffer for reading */
    unsigned char *m_bufp;          /* pointer to next byte in buffer */

private:
    int getcode(wxInputStream& stream, int bits, int abfin);
    wxGIFErrorCode dgif(wxInputStream& stream, GIFImage *img, int interl, int bits);

public:
    // get data of current frame
    unsigned char* GetData(size_t frame) const;
    unsigned char* GetPalette(size_t frame) const;
    unsigned int GetNcolours(size_t frame) const;
    int GetTransparentColourIndex(size_t frame) const;
    wxColour GetTransparentColour(size_t frame) const;

    virtual wxSize GetFrameSize(size_t frame) const;
    virtual wxPoint GetFramePosition(size_t frame) const;
    virtual wxAnimationDisposal GetDisposalMethod(size_t frame) const;
    virtual long GetDelay(size_t frame) const;

    // GIFs can contain both static images and animations
    bool IsAnimation() const
        { return m_nFrames > 1; }

public:
    // constructor, destructor, etc.
    wxGIFDecoder();
    ~wxGIFDecoder();

    // load function which returns more info than just Load():
    wxGIFErrorCode LoadGIF( wxInputStream& stream );

    // free all internal frames
    void Destroy();

public:     // implementation of wxAnimationDecoder's pure virtuals

    virtual bool CanRead( wxInputStream& stream ) const;
    virtual bool Load( wxInputStream& stream )
        { return LoadGIF(stream) == wxGIF_OK; }

    bool ConvertToImage(size_t frame, wxImage *image) const;

    wxAnimationDecoder *Clone() const
        { return new wxGIFDecoder; }
    wxAnimationType GetType() const
        { return wxANIMATION_TYPE_GIF; }

private:
    DECLARE_NO_COPY_CLASS(wxGIFDecoder)
};


#endif  // wxUSE_STREAM && wxUSE_GIF
#endif  // _WX_GIFDECOD_H
