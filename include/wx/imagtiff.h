/////////////////////////////////////////////////////////////////////////////
// Name:        imagtiff.h
// Purpose:     wxImage TIFF handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGTIFF_H_
#define _WX_IMAGTIFF_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "imagtiff.h"
#endif

#include "wx/image.h"


//-----------------------------------------------------------------------------
// wxTIFFHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBTIFF
class WXDLLEXPORT wxTIFFHandler: public wxImageHandler
{
public:
    inline wxTIFFHandler()
    {
        m_name = wxT("TIFF file");
        m_extension = wxT("tif");
        m_type = wxBITMAP_TYPE_TIF;
        m_mime = wxT("image/tiff");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
    virtual bool DoCanRead( wxInputStream& stream );
    virtual int GetImageCount( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxTIFFHandler)
};
#endif


#endif
  // _WX_IMAGTIFF_H_

