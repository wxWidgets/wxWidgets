/////////////////////////////////////////////////////////////////////////////
// Name:        wx/anidecod.h
// Purpose:     wxANIDecoder, ANI reader for wxImage and wxAnimation
// Author:      Francesco Montorsi
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIDECOD_H
#define _WX_ANIDECOD_H

#include "wx/defs.h"

#if wxUSE_STREAMS && (wxUSE_ICO_CUR || wxUSE_GIF)

#include "wx/stream.h"
#include "wx/image.h"
#include "wx/animdecod.h"
#include "wx/dynarray.h"


class /*WXDLLIMPEXP_CORE*/ wxANIFrameInfo;      // private implementation detail

// For compatibility purposes, provide wxImageArray class mimicking the legacy
// dynamic array which used to be required by wxGIFHandler::SaveAnimation():
// now we just take a vector of images there, but we want to keep the existing
// code using wxImageArray working (and keep it declared here because this is
// where it used to be, even if this doesn't make much sense).
using wxImageArray = wxBaseArray<wxImage>;

// --------------------------------------------------------------------------
// wxANIDecoder class
// --------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxANIDecoder : public wxAnimationDecoder
{
public:
    // constructor, destructor, etc.
    wxANIDecoder();
    ~wxANIDecoder();


    virtual wxSize GetFrameSize(unsigned int frame) const override;
    virtual wxPoint GetFramePosition(unsigned int frame) const override;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const override;
    virtual long GetDelay(unsigned int frame) const override;
    virtual wxColour GetTransparentColour(unsigned int frame) const override;

    // implementation of wxAnimationDecoder's pure virtuals

    virtual bool Load( wxInputStream& stream ) override;

    bool ConvertToImage(unsigned int frame, wxImage *image) const override;

    wxAnimationDecoder *Clone() const override
        { return new wxANIDecoder; }
    wxAnimationType GetType() const override
        { return wxANIMATION_TYPE_ANI; }

protected:
    // wxAnimationDecoder pure virtual:
    virtual bool DoCanRead( wxInputStream& stream ) const override;
            // modifies current stream position (see wxAnimationDecoder::CanRead)

private:
    // frames stored as wxImage(s): ANI files are meant to be used mostly for animated
    // cursors and thus they do not use any optimization to encode differences between
    // two frames: they are just a list of images to display sequentially.
    std::vector<wxImage> m_images;

    // the info about each image stored in m_images.
    // NB: m_info.GetCount() may differ from m_images.GetCount()!
    std::vector<wxANIFrameInfo> m_info;

    // this is the wxCURHandler used to load the ICON chunk of the ANI files
    static wxCURHandler sm_handler;


    wxDECLARE_NO_COPY_CLASS(wxANIDecoder);
};


#endif  // wxUSE_STREAMS && (wxUSE_ICO_CUR || wxUSE_GIF)

#endif  // _WX_ANIDECOD_H
