/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#ifdef __GNUG__
#pragma interface "statbmp.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBitmapNameStr;

class WXDLLEXPORT wxStaticBitmap: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticBitmap)
 public:
  inline wxStaticBitmap() { }

  inline wxStaticBitmap(wxWindow *parent, wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBitmapNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBitmapNameStr);

  virtual void SetIcon(const wxIcon& icon) { SetBitmap(icon); }
  virtual void SetBitmap(const wxBitmap& bitmap);

  // assert failure is provoked by an attempt to get an icon from bitmap or
  // vice versa
  const wxIcon& GetIcon() const
      { wxASSERT( m_isIcon ); return *m_image.icon; }
  const wxBitmap& GetBitmap() const
      { wxASSERT( !m_isIcon ); return *m_image.bitmap; }

  // overriden base class virtuals
  virtual bool AcceptsFocus() const { return FALSE; }

protected:
  void Init() { m_isIcon = TRUE; m_image.icon = NULL; }
  void Free();

  // TRUE if icon/bitmap is valid
  bool ImageIsOk() const;

  // we can have either an icon or a bitmap
  bool m_isIcon;
  union
  {
      wxIcon *icon;
      wxBitmap *bitmap;
  } m_image;

  virtual wxSize DoGetBestSize();
};

#endif
    // _WX_STATBMP_H_
