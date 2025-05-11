/////////////////////////////////////////////////////////////////////////////
// Name:        wx/webpdecoder.h
// Purpose:     wxWebPDecoder, WebP Animation decoder
// Author:      Maarten Bent
// Created:     2025-02-27
// Copyright:   (c) 2025 Maarten Bent
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBPDECODER_H
#define _WX_WEBPDECODER_H

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_LIBWEBP

#include "wx/stream.h"
#include "wx/image.h"
#include "wx/animdecod.h"
#include "wx/imagwebp.h"


class WXDLLIMPEXP_CORE wxWebPDecoder : public wxAnimationDecoder
{
public:
    wxWebPDecoder();
    ~wxWebPDecoder();

    virtual wxSize GetFrameSize(unsigned int frame) const override;
    virtual wxPoint GetFramePosition(unsigned int frame) const override;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const override;
    virtual long GetDelay(unsigned int frame) const override;
    virtual wxColour GetTransparentColour(unsigned int frame) const override;

    virtual bool Load(wxInputStream& stream) override;
    virtual bool ConvertToImage(unsigned int frame, wxImage* image) const override;

    virtual wxAnimationDecoder* Clone() const override
    {
        return new wxWebPDecoder;
    }
    virtual wxAnimationType GetType() const override
    {
        return wxANIMATION_TYPE_WEBP;
    }

protected:
    virtual bool DoCanRead(wxInputStream& stream) const override;

private:
    std::vector<wxWebPAnimationFrame> m_frames;

    void InitWebPHandler() const;

    wxDECLARE_NO_COPY_CLASS(wxWebPDecoder);
};

#endif  // wxUSE_STREAMS && wxUSE_LIBWEBP

#endif  // _WX_WEBPDECODER_H
