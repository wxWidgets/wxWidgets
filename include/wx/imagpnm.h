/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagpnm.h
// Purpose:     wxImage PNM handler
// Author:      Sylvain Bougnoux
// Copyright:   (c) Sylvain Bougnoux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGPNM_H_
#define _WX_IMAGPNM_H_

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxPNMHandler
//-----------------------------------------------------------------------------

#if wxUSE_PNM
class WXDLLIMPEXP_CORE wxPNMHandler : public wxImageHandler
{
public:
    wxPNMHandler() : wxImageHandler(
        wxT("PNM file"),
        wxT("pnm"),
        wxBITMAP_TYPE_PNM,
        wxT("image/pnm"))
    {
        m_altExtensions.Add(wxT("ppm"));
        m_altExtensions.Add(wxT("pgm"));
        m_altExtensions.Add(wxT("pbm"));
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 ) override;
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true ) override;
protected:
    virtual bool DoCanRead( wxInputStream& stream ) override;
#endif

private:
    wxDECLARE_DYNAMIC_CLASS(wxPNMHandler);
};
#endif


#endif
  // _WX_IMAGPNM_H_

