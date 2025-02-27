/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/webpdecoder.cpp
// Purpose:     wxWebPDecoder, WebP Animation decoder
// Author:
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STREAMS && wxUSE_LIBWEBP

#include "wx/webpdecoder.h"


wxWebPDecoder::wxWebPDecoder()
{
#if wxUSE_LIBWEBP
    wxImage::AddHandler(new wxWEBPHandler);
#endif
}

wxWebPDecoder::~wxWebPDecoder()
{
}

wxSize wxWebPDecoder::GetFrameSize(unsigned int WXUNUSED(frame)) const
{
    return m_szAnimation;
}

wxPoint wxWebPDecoder::GetFramePosition(unsigned int WXUNUSED(frame)) const
{
    return wxPoint(0, 0);
}

wxAnimationDisposal wxWebPDecoder::GetDisposalMethod(unsigned int WXUNUSED(frame)) const
{
    return wxANIM_TOBACKGROUND;
}

long wxWebPDecoder::GetDelay(unsigned int frame) const
{
    if (frame < m_nFrames)
        return m_frames[frame].duration;
    else
        return 0;
}

bool wxWebPDecoder::ConvertToImage(unsigned int frame, wxImage* image) const
{
    if (image && frame < m_nFrames)
        *image = m_frames[frame].image;
    return image && image->IsOk();
}

wxColour wxWebPDecoder::GetTransparentColour(unsigned int frame) const
{
    if (frame < m_nFrames)
        return m_frames[frame].bgColour;
    else
        return wxNullColour;
}

bool wxWebPDecoder::DoCanRead(wxInputStream& stream) const
{
    wxImageHandler* handler = wxImage::FindHandler(wxBITMAP_TYPE_WEBP);
    return handler && handler->CanRead(stream);
}

bool wxWebPDecoder::Load(wxInputStream& stream)
{
    wxWEBPHandler webpHandler;
    webpHandler.LoadAnimation(m_frames, stream);
    m_nFrames = m_frames.size();
    if (m_nFrames > 0)
        m_szAnimation = m_frames.front().image.GetSize();
    else
        m_szAnimation = wxDefaultSize;

    return m_szAnimation != wxDefaultSize;
}

#endif // wxUSE_STREAMS && wxUSE_LIBWEBP
