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

#ifdef __GNUG__
#pragma interface "imagtiff.h"
#endif

#include "wx/image.h"


//-----------------------------------------------------------------------------
// wxTIFFHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBTIFF
class WXDLLEXPORT wxTIFFHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxTIFFHandler)

public:

  inline wxTIFFHandler()
  {
      m_name = "TIFF file";
      m_extension = "tif";
      m_type = wxBITMAP_TYPE_TIF;
      m_mime = "image/tiff";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
  virtual int GetImageCount( wxInputStream& stream );
#endif
};
#endif


#endif
  // _WX_IMAGTIFF_H_

