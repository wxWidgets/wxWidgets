/////////////////////////////////////////////////////////////////////////////
// Name:        imagpng.h
// Purpose:     wxImage PNG handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGPNG_H_
#define _WX_IMAGPNG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "imagpng.h"
#endif

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxPNGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler: public wxImageHandler
{
public:
    inline wxPNGHandler()
    {
        m_name = wxT("PNG file");
        m_extension = wxT("png");
        m_type = wxBITMAP_TYPE_PNG;
        m_mime = wxT("image/png");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
    virtual bool DoCanRead( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxPNGHandler)
};
#endif


#endif
  // _WX_IMAGPNG_H_

