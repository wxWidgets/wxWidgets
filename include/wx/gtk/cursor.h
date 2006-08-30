/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_CURSOR_H_
#define _WX_GTK_CURSOR_H_

#include "wx/object.h"

class WXDLLIMPEXP_CORE wxColour;
class WXDLLIMPEXP_CORE wxImage;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCursor: public wxObject
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
    ~wxCursor();
    bool operator == ( const wxCursor& cursor ) const;
    bool operator != ( const wxCursor& cursor ) const;
    bool Ok() const;

    // implementation

    GdkCursor *GetCursor() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // _WX_GTK_CURSOR_H_
