/////////////////////////////////////////////////////////////////////////////
// Name:        imagpcx.h
// Purpose:     wxImage PCX handler
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGPCX_H_
#define _WX_IMAGPCX_H_

#ifdef __GNUG__
#pragma interface "imagpcx.h"
#endif

#include "wx/image.h"


//-----------------------------------------------------------------------------
// wxPCXHandler
//-----------------------------------------------------------------------------

#if wxUSE_PCX
class WXDLLEXPORT wxPCXHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPCXHandler)

public:

  inline wxPCXHandler()
  {
      m_name = "PCX file";
      m_extension = "pcx";
      m_type = wxBITMAP_TYPE_PCX;
      m_mime = "image/pcx";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
#endif // wxUSE_STREAMS
};
#endif // wxUSE_PCX


#endif
  // _WX_IMAGPCX_H_

