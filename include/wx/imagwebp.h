/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagwebp.h
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGWEBP_H_
#define _WX_IMAGWEBP_H_

#include "wx/image.h"
#include "wx/colour.h"

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBWEBP

#define wxIMAGE_OPTION_WEBP_QUALITY  wxT("WebPQuality")
#define wxIMAGE_OPTION_WEBP_LOSSLESS wxT("WebPLossless") // 0 = undefined (/mixed), 1 = lossy, 2 = lossless


struct wxWebPAnimationFrame
{
    wxImage image;
    wxColour bgColour;
    int duration = 0;
};


class WXDLLIMPEXP_CORE wxWEBPHandler : public wxImageHandler
{
public:
    inline wxWEBPHandler()
    {
        m_name = wxT("WebP file");
        m_extension = wxT("webp");
        m_type = wxBITMAP_TYPE_WEBP;
        m_mime = wxT("image/webp");
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
