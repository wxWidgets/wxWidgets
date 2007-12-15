/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/cursor.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCURSORH__
#define __GTKCURSORH__

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

#if wxUSE_IMAGE
#include "wx/image.h"
#endif

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
              const char maskBits[] = NULL, const wxColour *fg = NULL, const wxColour *bg = NULL );
    virtual ~wxCursor();

    // implementation

    GdkCursor *GetCursor() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // __GTKCURSORH__
