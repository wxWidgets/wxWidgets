/////////////////////////////////////////////////////////////////////////////
// Name:        imagpnm.h
// Purpose:     wxImage PNM handler
// Author:      Sylvain Bougnoux
// RCS-ID:      $Id$
// Copyright:   (c) Sylvain Bougnoux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGPNM_H_
#define _WX_IMAGPNM_H_

#ifdef __GNUG__
#pragma interface "imagpnm.h"
#endif

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxPNMHandler
//-----------------------------------------------------------------------------

#if wxUSE_PNM
class WXDLLEXPORT wxPNMHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNMHandler)

public:

  inline wxPNMHandler()
  {
      m_name = "PNM file";
      m_extension = "pnm";
      m_type = wxBITMAP_TYPE_PNM;
      m_mime = "image/pnm";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
#endif
};
#endif


#endif
  // _WX_IMAGPNM_H_

