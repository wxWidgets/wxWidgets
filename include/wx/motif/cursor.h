/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
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

/* Cursor for one display, so we can choose the correct one for
 * the current display.
 */
class wxXCursor : public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxXCursor)

 public:
  WXDisplay*  m_display;
  WXCursor    m_cursor;
};

class WXDLLEXPORT wxCursorRefData: public wxBitmapRefData
{
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxCursor;
public:
    wxCursorRefData();
    ~wxCursorRefData();

    wxList        m_cursors;  // wxXCursor objects, one per display
    wxStockCursor m_cursorId; // wxWindows standard cursor id
};

#define M_CURSORDATA ((wxCursorRefData *)m_refData)
#define M_CURSORHANDLERDATA ((wxCursorRefData *)bitmap->m_refData)

// Cursor
class WXDLLEXPORT wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

public:
  wxCursor();

  // Copy constructors
  inline wxCursor(const wxCursor& cursor) { Ref(cursor); }

  wxCursor(const char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1,
    const char maskBits[] = NULL);

  wxCursor(const wxString& name, long flags = wxBITMAP_TYPE_XBM,
   int hotSpotX = 0, int hotSpotY = 0);

  wxCursor(wxStockCursor id);
  ~wxCursor();

  virtual bool Ok() const { return ((m_refData != NULL) && M_CURSORDATA->m_ok); }

  inline wxCursor& operator = (const wxCursor& cursor) { if (*this == cursor) return (*this); Ref(cursor); return *this; }
  inline bool operator == (const wxCursor& cursor) const { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) const { return m_refData != cursor.m_refData; }

  // Motif-specific.
  // Create/get a cursor for the current display
  WXCursor GetXCursor(WXDisplay* display) ;
  // Make a cursor from standard id
  WXCursor MakeCursor(WXDisplay* display, wxStockCursor id);
};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif
    // _WX_CURSOR_H_
