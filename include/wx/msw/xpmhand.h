/////////////////////////////////////////////////////////////////////////////
// Name:        xpmhand.h
// Purpose:     XPM bitmap handler
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "xpmhand.h"
#endif

#ifndef _WX_XPMHAND_H_
#define _WX_XPMHAND_H_

class WXDLLEXPORT wxXPMFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxXPMFileHandler)
public:
  inline wxXPMFileHandler(void)
  {
  m_name = "XPM bitmap file";
  m_extension = "xpm";
  m_type = wxBITMAP_TYPE_XPM;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth = -1, int desiredHeight = -1);
  virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};

class WXDLLEXPORT wxXPMDataHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxXPMDataHandler)
public:
  inline wxXPMDataHandler(void)
  {
  m_name = "XPM bitmap data";
  m_extension = "xpm";
  m_type = wxBITMAP_TYPE_XPM_DATA;
  };

  virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
};

#endif
  // _WX_XPMHAND_H_















