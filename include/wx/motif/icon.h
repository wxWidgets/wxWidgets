/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
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
    wxIconRefData();
    ~wxIconRefData();
};

#define M_ICONDATA ((wxIconRefData *)m_refData)
#define M_ICONHANDLERDATA ((wxIconRefData *)bitmap->GetRefData())

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

public:
  wxIcon();

  // Copy constructors
  inline wxIcon(const wxIcon& icon) { Ref(icon); }
  inline wxIcon(const wxIcon* icon) { if (icon) Ref(*icon); }

  wxIcon(const char bits[], int width, int height);
  wxIcon(const wxString& name, long flags = wxBITMAP_TYPE_ICO_RESOURCE,
    int desiredWidth = -1, int desiredHeight = -1);
  ~wxIcon();

  bool LoadFile(const wxString& name, long flags = wxBITMAP_TYPE_ICO_RESOURCE,
      int desiredWidth = -1, int desiredHeight = -1);

  inline wxIcon& operator = (const wxIcon& icon) { if (*this == icon) return (*this); Ref(icon); return *this; }
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }

/* TODO */
  virtual bool Ok() const { return (m_refData != NULL) ; }
};

/* Example handlers. TODO: write your own handlers for relevant types.

class WXDLLEXPORT wxICOFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxICOFileHandler)
public:
  inline wxICOFileHandler()
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
  inline wxICOResourceHandler()
  {
	m_name = "ICO resource";
	m_extension = "ico";
	m_type = wxBITMAP_TYPE_ICO_RESOURCE;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth = -1, int desiredHeight = -1);

};

*/

#endif
    // _WX_ICON_H_
