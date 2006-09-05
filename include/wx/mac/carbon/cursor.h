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
  virtual bool Ok() const ;

  inline bool operator == (const wxCursor& cursor) const { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) const { return m_refData != cursor.m_refData; }

    void MacInstall() const ;

  void SetHCURSOR(WXHCURSOR cursor);
  inline WXHCURSOR GetHCURSOR() const ;
private :
    void CreateFromImage(const wxImage & image) ;
};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif
    // _WX_CURSOR_H_
