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

#ifdef __GNUG__
#pragma interface "imagpng.h"
#endif

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxPNGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNGHandler)

public:

  inline wxPNGHandler()
  {
      m_name = "PNG file";
      m_extension = "png";
      m_type = wxBITMAP_TYPE_PNG;
      m_mime = "image/png";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
#endif
};
#endif


#endif
  // _WX_IMAGPNG_H_

