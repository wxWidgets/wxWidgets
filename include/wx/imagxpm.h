/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.h
// Purpose:     wxImage XPM handler
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGXPM_H_
#define _WX_IMAGXPM_H_

#ifdef __GNUG__
#pragma interface "imagxpm.h"
#endif

#include "wx/image.h"


//-----------------------------------------------------------------------------
// wxXPMHandler
//-----------------------------------------------------------------------------

#if wxUSE_XPM
class WXDLLEXPORT wxXPMHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxXPMHandler)

public:

  inline wxXPMHandler()
  {
      m_name = wxT("XPM file");
      m_extension = wxT("xpm");
      m_type = wxBITMAP_TYPE_XPM;
      m_mime = wxT("image/xpm");
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool DoCanRead( wxInputStream& stream );
#endif
};
#endif // wxUSE_XPM


#endif
  // _WX_IMAGXPM_H_

