/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#ifdef __GNUG__
#pragma interface "cursor.h"
#endif

#include "wx/bitmap.h"

class WXDLLEXPORT wxCursorRefData: public wxBitmapRefData
{
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxCursor;
public:
    wxCursorRefData(void);
    ~wxCursorRefData(void);

protected:
  WXHCURSOR m_hCursor;
  bool m_destroyCursor;
};

#define M_CURSORDATA ((wxCursorRefData *)m_refData)
#define M_CURSORHANDLERDATA ((wxCursorRefData *)bitmap->m_refData)

// Cursor
class WXDLLEXPORT wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

public:
  wxCursor(void);

  // Copy constructors
  inline wxCursor(const wxCursor& cursor) { Ref(cursor); }

  wxCursor(const char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1,
    const char maskBits[] = NULL);
  wxCursor(const wxString& name, long flags = wxBITMAP_TYPE_CUR_RESOURCE,
   int hotSpotX = 0, int hotSpotY = 0);
  wxCursor(int cursor_type);
  ~wxCursor(void);

  virtual bool Ok(void) const { return (m_refData != NULL && M_CURSORDATA->m_hCursor) ; }

  inline wxCursor& operator = (const wxCursor& cursor) { if (*this == cursor) return (*this); Ref(cursor); return *this; }
  inline bool operator == (const wxCursor& cursor) { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) { return m_refData != cursor.m_refData; }

  void SetHCURSOR(WXHCURSOR cursor);
  inline WXHCURSOR GetHCURSOR(void) const { return (M_CURSORDATA ? M_CURSORDATA->m_hCursor : 0); }

  bool FreeResource(bool force = FALSE);
};

#endif
    // _WX_CURSOR_H_
