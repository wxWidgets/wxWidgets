/////////////////////////////////////////////////////////////////////////////
// Name:        pnghand.h
// Purpose:     PNG bitmap handler
// Author:      Julian Smart
// Modified by: 
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PNGHAND_H_
#define _WX_PNGHAND_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "pnghand.h"
#endif

#include "wx/defs.h"

#if wxUSE_LIBPNG

class WXDLLEXPORT wxPNGFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNGFileHandler)
public:
  inline wxPNGFileHandler(void)
  {
    m_name = wxT("PNG bitmap file");
    m_extension = wxT("bmp");
    m_type = wxBITMAP_TYPE_PNG;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};

#endif //wxUSE_LIBPNG

#endif
  // _WX_PNGHAND_H_

