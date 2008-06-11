/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/cursor.h
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
class WXDLLIMPEXP_CORE wxCursor : public wxGDIObject
{
public:
    wxCursor();

    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = NULL);

    wxCursor(const wxImage & image) ;
    wxCursor(const char* const* bits);
    wxCursor(const wxString& name,
             wxBitmapType flags = wxBITMAP_TYPE_MACCURSOR_RESOURCE,
             int hotSpotX = 0, int hotSpotY = 0);

    wxCursor(int cursor_type);
    virtual ~wxCursor();

    bool CreateFromXpm(const char* const* bits);

    void MacInstall() const ;

    void SetHCURSOR(WXHCURSOR cursor);
    WXHCURSOR GetHCURSOR() const;

private:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    void CreateFromImage(const wxImage & image) ;

    DECLARE_DYNAMIC_CLASS(wxCursor)
};

extern WXDLLIMPEXP_CORE void wxSetCursor(const wxCursor& cursor);

#endif // _WX_CURSOR_H_
