/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagxpm.h
// Purpose:     wxImage XPM handler
// Author:      Vaclav Slavik
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGXPM_H_
#define _WX_IMAGXPM_H_

#include "wx/image.h"

#if wxUSE_XPM

//-----------------------------------------------------------------------------
// wxXPMHandler
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxXPMHandler : public wxImageHandler
{
public:
    wxXPMHandler() : wxImageHandler(
        wxT("XPM file"),
        wxT("xpm"),
        wxBITMAP_TYPE_XPM,
        wxT("image/xpm"))
    {
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 ) override;
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true ) override;
protected:
    virtual bool DoCanRead( wxInputStream& stream ) override;
#endif

private:
    wxDECLARE_DYNAMIC_CLASS(wxXPMHandler);
};

#endif // wxUSE_XPM

#endif // _WX_IMAGXPM_H_
