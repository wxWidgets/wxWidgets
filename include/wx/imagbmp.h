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

//-----------------------------------------------------------------------------
// wxBMPHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBMPHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxBMPHandler)

public:

  inline wxBMPHandler()
  {
      m_name = "BMP file";
      m_extension = "bmp";
      m_type = wxBITMAP_TYPE_BMP;
      m_mime = "image/bmp";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool DoCanRead( wxInputStream& stream );
#endif
};


#endif
  // _WX_IMAGBMP_H_

