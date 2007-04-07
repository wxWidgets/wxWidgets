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
class WXDLLEXPORT wxIcon: public wxGDIObject
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

    wxIcon(WXHICON icon, const wxSize& size);

    virtual ~wxIcon();

    bool LoadFile(const wxString& name, wxBitmapType flags /* = wxBITMAP_TYPE_ICON_RESOURCE */ ,
      int desiredWidth /* = -1 */ , int desiredHeight = -1);
    bool LoadFile(const wxString& name ,wxBitmapType flags = wxBITMAP_TYPE_ICON_RESOURCE )
      { return LoadFile( name , flags , -1 , -1 ) ; }


    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

    bool Ok() const { return IsOk(); }
    bool IsOk() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    void SetWidth(int w);
    void SetHeight(int h);
    void SetDepth(int d);
    void SetOk(bool isOk);

    WXHICON GetHICON() const ;

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
    SetName(wxT("ICON resource"));
    SetExtension(wxEmptyString);
    SetType(wxBITMAP_TYPE_ICON_RESOURCE);
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);

};

#endif
    // _WX_ICON_H_
