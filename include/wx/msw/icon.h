/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#ifdef __GNUG__
#pragma interface "icon.h"
#endif

#include "wx/bitmap.h"

class WXDLLEXPORT wxIconRefData: public wxBitmapRefData
{
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxIcon;
public:
    wxIconRefData(void);
    ~wxIconRefData(void);

public:
  WXHICON m_hIcon;
};

#define M_ICONDATA ((wxIconRefData *)m_refData)
#define M_ICONHANDLERDATA ((wxIconRefData *)bitmap->GetRefData())

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

public:
  wxIcon(void);

  // Copy constructors
  inline wxIcon(const wxIcon& icon) { Ref(icon); }
  inline wxIcon(const wxIcon* icon) { /* UnRef(); */ if (icon) Ref(*icon); }

  wxIcon(const char bits[], int width, int height);
  wxIcon(const wxString& name, long flags = wxBITMAP_TYPE_ICO_RESOURCE,
    int desiredWidth = -1, int desiredHeight = -1);
  ~wxIcon(void);

  bool LoadFile(const wxString& name, long flags = wxBITMAP_TYPE_ICO_RESOURCE,
      int desiredWidth = -1, int desiredHeight = -1);

  inline wxIcon& operator = (const wxIcon& icon) { if (*this == icon) return (*this); Ref(icon); return *this; }
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }

  void SetHICON(WXHICON ico);
  inline WXHICON GetHICON(void) const { return (M_ICONDATA ? M_ICONDATA->m_hIcon : 0); }

  virtual bool Ok(void) const { return (m_refData != NULL && M_ICONDATA->m_hIcon) ; }

  bool FreeResource(bool force = FALSE);
};

// TODO: Put these in separate, private header

class WXDLLEXPORT wxICOFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICOFileHandler)
public:
  inline wxICOFileHandler(void)
  {
	m_name = "ICO icon file";
	m_extension = "ico";
	m_type = wxBITMAP_TYPE_ICO;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);
};

class WXDLLEXPORT wxICOResourceHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICOResourceHandler)
public:
  inline wxICOResourceHandler(void)
  {
	m_name = "ICO resource";
	m_extension = "ico";
	m_type = wxBITMAP_TYPE_ICO_RESOURCE;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);

};

#endif
    // _WX_ICON_H_
