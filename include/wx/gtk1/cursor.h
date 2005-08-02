/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCURSORH__
#define __GTKCURSORH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"

#if wxUSE_IMAGE
#include "wx/image.h"
#endif

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCursor: public wxObject
{
public:

    wxCursor();
    wxCursor( int cursorId );
    wxCursor( const wxCursor &cursor );
#if wxUSE_IMAGE
    wxCursor( const wxImage & image );
#endif
    wxCursor( const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=0, wxColour *fg=0, wxColour *bg=0 );
    ~wxCursor();
    wxCursor& operator = ( const wxCursor& cursor );
    bool operator == ( const wxCursor& cursor ) const;
    bool operator != ( const wxCursor& cursor ) const;
    bool Ok() const;

    // implementation

    GdkCursor *GetCursor() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // __GTKCURSORH__
