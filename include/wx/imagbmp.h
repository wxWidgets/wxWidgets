/////////////////////////////////////////////////////////////////////////////
// Name:        imagbmp.h
// Purpose:     wxImage BMP handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGBMP_H_
#define _WX_IMAGBMP_H_

#ifdef __GNUG__
#pragma interface "imagbmp.h"
#endif

#include "wx/image.h"

// ----------------------------------------------------------------------------
// wxBMPHandler
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBMPHandler : public wxImageHandler
{
public:
  wxBMPHandler()
  {
      m_name = _T("BMP file");
      m_extension = _T("bmp");
      m_type = wxBITMAP_TYPE_BMP;
      m_mime = _T("image/bmp");
  };

  // saving bitmaps is Windows-only
#ifdef __WIN32__
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
#endif // __WIN32__

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool DoCanRead( wxInputStream& stream );
#endif // wxUSE_STREAMS

  DECLARE_DYNAMIC_CLASS(wxBMPHandler)
};


#endif
  // _WX_IMAGBMP_H_

