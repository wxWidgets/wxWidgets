/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     wxCursor class
// Author:      David Elliott <dfe@cox.net>
// Modified by:
// Created:     2002/11/27
// RCS-ID:      
// Copyright:   (c) David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_CURSOR_H_
#define _WX_COCOA_CURSOR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "cursor.h"
#endif

#include "wx/bitmap.h"

class WXDLLEXPORT wxCursorRefData: public wxObjectRefData
{
    DECLARE_NO_COPY_CLASS(wxCursorRefData)
        
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxCursor;
public:
    wxCursorRefData();
    ~wxCursorRefData();

protected:
    int m_width, m_height;
    WX_NSCursor m_hCursor;
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
  wxCursor(const wxCursor& cursor)
      : wxBitmap()
  { Ref(cursor); }

  wxCursor(const char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1,
    const char maskBits[] = NULL);

  wxCursor(const wxString& name, long flags = 0,
   int hotSpotX = 0, int hotSpotY = 0);

  wxCursor(int cursor_type);
  ~wxCursor();

  virtual bool Ok() const { return m_refData ; }

  inline wxCursor& operator = (const wxCursor& cursor) { if (*this == cursor) return (*this); Ref(cursor); return *this; }
  inline bool operator == (const wxCursor& cursor) { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) { return m_refData != cursor.m_refData; }
  
  inline WX_NSCursor GetNSCursor() const
  {	
    return (M_CURSORDATA ? M_CURSORDATA->m_hCursor : 0);
  }

};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif // _WX_COCOA_CURSOR_H_
