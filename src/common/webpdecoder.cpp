/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/webpdecoder.cpp
// Purpose:     wxWebPDecoder, WebP Animation decoder
// Author:      Maarten Bent
// Created:     2025-02-27
// Copyright:   (c) Maarten Bent
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STREAMS && wxUSE_LIBWEBP

#include "wx/webpdecoder.h"

wxWebPDecoder::wxWebPDecoder()
{
}

wxWebPDecoder::~wxWebPDecoder()
{
}

void wxWebPDecoder::InitWebPHandler() const
{
    if (wxImage::FindHandler(wxBITMAP_TYPE_WEBP) == nullptr)
        wxImage::AddHandler(new wxWEBPHandler);
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
    InitWebPHandler();

    wxImageHandler* handler = wxImage::FindHandler(wxBITMAP_TYPE_WEBP);
    return handler && handler->CanRead(stream);
}

bool wxWebPDecoder::Load(wxInputStream& stream)
{
    m_frames.clear();

    InitWebPHandler();
    wxWEBPHandler* handler =
        wxDynamicCast(wxImage::FindHandler(wxBITMAP_TYPE_WEBP), wxWEBPHandler);
    if (handler)
        handler->LoadAnimation(m_frames, stream);

    m_nFrames = m_frames.size();
    if (m_nFrames > 0)
        m_szAnimation = m_frames.front().image.GetSize();
    else
        m_szAnimation = wxDefaultSize;

    return m_szAnimation != wxDefaultSize;
}

#endif // wxUSE_STREAMS && wxUSE_LIBWEBP
