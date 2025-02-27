/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagwebp.h
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Created:     2024-03-08
// Copyright:   (c) Hermann Höhne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGWEBP_H_
#define _WX_IMAGWEBP_H_

#include "wx/defs.h"

#if wxUSE_LIBWEBP

#include "wx/colour.h"
#include "wx/image.h"
#include "wx/versioninfo.h"

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

namespace wxWebPImageFormat
{
    enum wxWebPImageFormat : int
    {
        Undefined = 0,
        Lossy = 1,
        Lossless = 2
    };
}

#define wxIMAGE_OPTION_WEBP_QUALITY wxS("WebPQuality")
#define wxIMAGE_OPTION_WEBP_FORMAT  wxS("WebPFormat")


struct wxWebPAnimationFrame
{
    wxImage image;
    wxColour bgColour;
    int duration = 0;
};


class WXDLLIMPEXP_CORE wxWEBPHandler : public wxImageHandler
{
public:
    wxWEBPHandler()
    {
        m_name = wxS("WebP file");
        m_extension = wxS("webp");
        m_type = wxBITMAP_TYPE_WEBP;
        m_mime = wxS("image/webp");
    }

    static wxVersionInfo GetLibraryVersionInfo();

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage* image, wxInputStream& stream, bool verbose = true, int index = -1) override;
    virtual bool SaveFile(wxImage* image, wxOutputStream& stream, bool verbose = true) override;
    virtual bool LoadAnimation(std::vector<wxWebPAnimationFrame>& frames, wxInputStream& stream, bool verbose = true);
protected:
    virtual bool DoCanRead(wxInputStream& stream) override;
    virtual int DoGetImageCount(wxInputStream& stream) override;
#endif // wxUSE_STREAMS

private:
    wxDECLARE_DYNAMIC_CLASS(wxWEBPHandler);
};

#endif // wxUSE_LIBWEBP

#endif // _WX_IMAGWEBP_H_
