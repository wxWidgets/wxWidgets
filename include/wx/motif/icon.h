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

/*
// Same as for wxBitmap
class WXDLLEXPORT wxIconRefData: public wxBitmapRefData
{
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxIcon;
public:
    wxIconRefData();
    ~wxIconRefData();
};
*/

#define M_ICONDATA ((wxBitmapRefData *)m_refData)
#define M_ICONHANDLERDATA ((wxBitmapRefData *)bitmap->GetRefData())

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

public:
  wxIcon();

  // Copy constructors
  inline wxIcon(const wxIcon& icon) { Ref(icon); }

  // Initialize with XBM data
  wxIcon(const char bits[], int width, int height);

  // Initialize with XPM data
  wxIcon(const char **data);
  wxIcon(char **data);

  wxIcon(const wxString& name, long flags = wxBITMAP_TYPE_XPM,
    int desiredWidth = -1, int desiredHeight = -1);
  ~wxIcon();

  bool LoadFile(const wxString& name, long flags = wxBITMAP_TYPE_XPM,
      int desiredWidth = -1, int desiredHeight = -1);
  bool LoadFile(const wxString& name, long type = wxBITMAP_TYPE_XBM)
    { return wxBitmap::LoadFile(name, type); }

  inline wxIcon& operator = (const wxIcon& icon) { if (*this == icon) return (*this); Ref(icon); return *this; }
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }

  virtual bool Ok() const { return ((m_refData != NULL) && (M_ICONDATA->m_ok)); }
};

#endif
    // _WX_ICON_H_
