/////////////////////////////////////////////////////////////////////////////
// Name:        wx/animdecod.h
// Purpose:     wxAnimationDecoder
// Author:      Francesco Montorsi
// CVS-ID:      $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIMDECOD_H
#define _WX_ANIMDECOD_H

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_GIF

#include "wx/stream.h"
#include "wx/image.h"

/*

 Differences between a wxAnimationDecoder and a wxImageHandler:

 1) wxImageHandlers always load an input stream directly into a given wxImage
    object converting from the format-specific data representation to the
    wxImage native format (RGB24).
    wxAnimationDecoders always load an input stream using some optimized format
    to store it which is format-depedent. This allows to store a (possibly big)
    animation using a format which is a good compromise between required memory
    and time required to blit in on the screen.

 2) wxAnimationDecoders contain the animation data in some internal var.
    That's why they derive from wxObjectRefData: they are data which can be shared.

 3) wxAnimationDecoders can be used by a wxImageHandler to retrieve a frame
    in wxImage format; the viceversa cannot be done.

 4) wxAnimationDecoders are decoders only, thus do not support save features.

 5) wxAnimationDecoders are directly used by wxAnimation (generic implementation)
    as wxObjectRefData while they need to be 'wrapped' by a wxImageHandler for
    wxImage uses.

*/


// --------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------

// NB: the values of these enum items are not casual but coincide with the
//     GIF disposal codes. Do not change them !!
enum wxAnimationDisposal
{
    // No disposal specified. The decoder is not required to take any action.
    wxANIM_UNSPECIFIED = -1,

    // Do not dispose. The graphic is to be left in place.
    wxANIM_DONOTREMOVE = 0,

    // Restore to background color. The area used by the graphic must be 
    // restored to the background color.
    wxANIM_TOBACKGROUND = 1,

    // Restore to previous. The decoder is required to restore the area 
    // overwritten by the graphic with what was there prior to rendering the graphic.
    wxANIM_TOPREVIOUS = 2
};

enum wxAnimationType
{
    wxANIMATION_TYPE_INVALID,
    wxANIMATION_TYPE_GIF,
    wxANIMATION_TYPE_ANI,

    wxANIMATION_TYPE_ANY
};


// --------------------------------------------------------------------------
// wxAnimationDecoder class
// --------------------------------------------------------------------------

class WXDLLEXPORT wxAnimationDecoder : public wxObjectRefData
{
protected:
    wxSize m_szAnimation;
    size_t m_nFrames;

    // this is the colour to use for the wxANIM_TOBACKGROUND disposal.
    // if not specified by the animation, it's set to wxNullColour
    wxColour m_background;

public:     // frame specific data getters

    // not all frames may be of the same size; e.g. GIF allows to
    // specify that between two frames only a smaller portion of the
    // entire animation has changed.
    virtual wxSize GetFrameSize(size_t frame) const = 0;

    // the position of this frame in case it's not as big as m_szAnimation
    // or wxPoint(0,0) otherwise.
    virtual wxPoint GetFramePosition(size_t frame) const = 0;

    // what should be done after displaying this frame.
    virtual wxAnimationDisposal GetDisposalMethod(size_t frame) const = 0;

    // the number of milliseconds this frame should be displayed.
    // if returns -1 then the frame must be displayed forever.
    virtual long GetDelay(size_t frame) const = 0;

    // get global data
    wxSize GetAnimationSize() const { return m_szAnimation; }
    wxColour GetBackgroundColour() const { return m_background; }
    size_t GetFrameCount() const { return m_nFrames; }

public:
    wxAnimationDecoder()
    {
        m_background = wxNullColour;
        m_nFrames = 0;
    }
    ~wxAnimationDecoder() {}


    virtual bool Load( wxInputStream& stream ) = 0;
    virtual bool CanRead( wxInputStream& stream ) const = 0;

    virtual wxAnimationDecoder *Clone() const = 0;
    virtual wxAnimationType GetType() const = 0;

    // convert given frame to wxImage
    virtual bool ConvertToImage(size_t frame, wxImage *image) const = 0;
};


#endif  // wxUSE_STREAM && wxUSE_GIF
#endif  // _WX_ANIMDECOD_H

