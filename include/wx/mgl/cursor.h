/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_CURSOR_H__
#define __WX_CURSOR_H__

#ifdef __GNUG__
#pragma interface "cursor.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"

class MGLCursor;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxCursor: public wxObject
{
public:

    wxCursor();
    wxCursor(int cursorId);
    wxCursor(const wxCursor &cursor);
    wxCursor(const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=0, wxColour *fg=0, wxColour *bg=0);
    wxCursor(const wxString& name,
             long flags = wxBITMAP_TYPE_CUR_RESOURCE,
             int hotSpotX = 0, int hotSpotY = 0);
    ~wxCursor();
    wxCursor& operator = ( const wxCursor& cursor );
    bool operator == (const wxCursor& cursor) const;
    bool operator != (const wxCursor& cursor) const;
    bool Ok() const;

    // implementation
    MGLCursor *GetMGLCursor() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // __WX_CURSOR_H__
