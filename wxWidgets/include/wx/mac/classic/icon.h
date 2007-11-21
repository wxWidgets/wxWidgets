/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#include "wx/bitmap.h"

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
public:
  wxIcon();

  wxIcon(const char **data);
  wxIcon(char **data);
  wxIcon(const char bits[], int width , int height );
  wxIcon(const wxString& name, int flags = wxBITMAP_TYPE_ICON_RESOURCE,
         int desiredWidth = -1, int desiredHeight = -1);
  wxIcon(const wxIconLocation& loc)
  {
      LoadFile(loc.GetFileName(), wxBITMAP_TYPE_ICON);
  }
  virtual ~wxIcon();

  bool LoadFile(const wxString& name, wxBitmapType flags /* = wxBITMAP_TYPE_ICON_RESOURCE */ ,
      int desiredWidth /* = -1 */ , int desiredHeight = -1);
  bool LoadFile(const wxString& name ,wxBitmapType flags = wxBITMAP_TYPE_ICON_RESOURCE )
      { return LoadFile( name , flags , -1 , -1 ) ; }

  bool operator==(const wxIcon& icon) const { return m_refData == icon.m_refData; }
  bool operator!=(const wxIcon& icon) const { return !(*this == icon); }

  // create from bitmap (which should have a mask unless it's monochrome):
  // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
  // ctors, assignment operators...), but it's ok to have such function
  void CopyFromBitmap(const wxBitmap& bmp);

  DECLARE_DYNAMIC_CLASS(wxIcon)
};

/*
class WXDLLEXPORT wxICONFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICONFileHandler)
public:
  inline wxICONFileHandler()
  {
    m_name = "ICO icon file";
    m_extension = "ico";
    m_type = wxBITMAP_TYPE_ICO;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);
};
*/

class WXDLLEXPORT wxICONResourceHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICONResourceHandler)
public:
  inline wxICONResourceHandler()
  {
    m_name = wxT("ICON resource");
    m_extension = wxEmptyString;
    m_type = wxBITMAP_TYPE_ICON_RESOURCE;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);

};

#endif
    // _WX_ICON_H_
