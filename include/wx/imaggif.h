/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.h
// Purpose:     wxImage GIF handler
// Author:      Vaclav Slavik & Guillermo Rodriguez Garcia
// RCS-ID:      $Id$
// Copyright:   (c) Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGGIF_H_
#define _WX_IMAGGIF_H_

#ifdef __GNUG__
#pragma interface "imaggif.h"
#endif

#include "wx/image.h"


//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

#if wxUSE_GIF

class WXDLLEXPORT wxGIFHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxGIFHandler)

public:

  inline wxGIFHandler()
  {
      m_name = "GIF file";
      m_extension = "gif";
      m_type = wxBITMAP_TYPE_GIF;
      m_mime = "image/gif";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
#endif
};
#endif


#endif
  // _WX_IMAGGIF_H_

