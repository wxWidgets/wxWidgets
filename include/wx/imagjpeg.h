/////////////////////////////////////////////////////////////////////////////
// Name:        imagjpeg.h
// Purpose:     wxImage JPEG handler
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGJPEG_H_
#define _WX_IMAGJPEG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "imagjpeg.h"
#endif

#include "wx/image.h"

// defines for wxImage::SetOption
#define wxIMAGE_OPTION_RESOLUTION            wxString(_T("Resolution"))
#define wxIMAGE_OPTION_RESOLUTIONUNIT        wxString(_T("ResolutionUnit"))

enum
{
    wxIMAGE_RESOLUTION_INCHES = 1,
    wxIMAGE_RESOLUTION_CM = 2
};

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG
class WXDLLEXPORT wxJPEGHandler: public wxImageHandler
{
public:
    inline wxJPEGHandler()
    {
        m_name = wxT("JPEG file");
        m_extension = wxT("jpg");
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT("image/jpeg");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
    virtual bool DoCanRead( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxJPEGHandler)
};
#endif



#endif
  // _WX_IMAGJPEG_H_

