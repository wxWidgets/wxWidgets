/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/cursor.h
// Purpose:     wxCursor declaration
// Author:      Vaclav Slavik
// Created:     2006-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_CURSOR_H_
#define _WX_DFB_CURSOR_H_

#include "wx/object.h"
#include "wx/gdicmn.h"

class WXDLLIMPEXP_CORE wxBitmap;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCursor: public wxObject
{
public:
    wxCursor() {}
    wxCursor(int cursorId);
    wxCursor(const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=0, wxColour *fg=0, wxColour *bg=0);
    wxCursor(const wxString& name,
             long flags = wxBITMAP_TYPE_CUR_RESOURCE,
             int hotSpotX = 0, int hotSpotY = 0);

    bool Ok() const { return IsOk(); }
    bool IsOk() const;

    // implementation
    wxBitmap GetBitmap() const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // _WX_DFB_CURSOR_H_
