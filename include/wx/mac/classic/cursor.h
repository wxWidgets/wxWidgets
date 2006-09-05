/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     wxCursor class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/bitmap.h"

class WXDLLEXPORT wxCursorRefData: public wxBitmapRefData
{
    DECLARE_NO_COPY_CLASS(wxCursorRefData)
        
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxCursor;
public:
    wxCursorRefData();
    virtual ~wxCursorRefData();

protected:
    WXHCURSOR     m_hCursor;
    bool        m_disposeHandle;
    bool        m_releaseHandle;
    bool        m_isColorCursor ;
    long        m_themeCursor ;
};

#define M_CURSORDATA ((wxCursorRefData *)m_refData)
#define M_CURSORHANDLERDATA ((wxCursorRefData *)bitmap->m_refData)

// Cursor
class WXDLLEXPORT wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

public:
  wxCursor();

  wxCursor(const char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1,
    const char maskBits[] = NULL);

    wxCursor(const wxImage & image) ;
    wxCursor(const char **bits) ;
    wxCursor(char **bits) ;
    wxCursor(const wxString& name, long flags = wxBITMAP_TYPE_MACCURSOR_RESOURCE,
        int hotSpotX = 0, int hotSpotY = 0);

  wxCursor(int cursor_type);
  virtual ~wxCursor();

	bool CreateFromXpm(const char **bits) ;
  virtual bool Ok() const { return (m_refData != NULL && ( M_CURSORDATA->m_hCursor != NULL || M_CURSORDATA->m_themeCursor != -1 ) ) ; }

  inline bool operator == (const wxCursor& cursor) const { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) const { return m_refData != cursor.m_refData; }

    void MacInstall() const ;

  void SetHCURSOR(WXHCURSOR cursor);
  inline WXHCURSOR GetHCURSOR() const { return (M_CURSORDATA ? M_CURSORDATA->m_hCursor : 0); }
private :
    void CreateFromImage(const wxImage & image) ;
};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif
    // _WX_CURSOR_H_
