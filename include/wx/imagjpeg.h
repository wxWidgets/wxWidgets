/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagjpeg.h
// Purpose:     wxImage JPEG handler
// Author:      Vaclav Slavik
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGJPEG_H_
#define _WX_IMAGJPEG_H_

#include "wx/defs.h"

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG

#include "wx/image.h"
#include "wx/versioninfo.h"

class WXDLLIMPEXP_CORE wxJPEGHandler: public wxImageHandler
{
public:
    wxJPEGHandler() : wxImageHandler(
        wxT("JPEG file"),
        wxT("jpg"),
        wxBITMAP_TYPE_JPEG,
        wxT("image/jpeg"))
    {
        m_altExtensions.Add(wxT("jpeg"));
        m_altExtensions.Add(wxT("jpe"));
    }

    static wxVersionInfo GetLibraryVersionInfo();

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 ) override;
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true ) override;
protected:
    virtual bool DoCanRead( wxInputStream& stream ) override;
#endif

private:
    wxDECLARE_DYNAMIC_CLASS(wxJPEGHandler);
};

#endif // wxUSE_LIBJPEG

#endif // _WX_IMAGJPEG_H_

