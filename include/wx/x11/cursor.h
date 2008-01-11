/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/gdiobj.h"
#include "wx/colour.h"

class WXDLLIMPEXP_FWD_CORE wxImage;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCursor : public wxGDIObject
{
public:
    wxCursor();
    wxCursor( int cursorId );
#if wxUSE_IMAGE
    wxCursor( const wxImage & image );
#endif

    wxCursor( const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=0, wxColour *fg=0, wxColour *bg=0 );
    virtual ~wxCursor();

    // implementation

    WXCursor GetCursor() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // _WX_CURSOR_H_
