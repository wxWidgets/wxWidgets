/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKICONH__
#define __GTKICONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxIcon;

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

class wxIcon: public wxBitmap
{
public:
    wxIcon();
    wxIcon( const wxIcon& icon);
    wxIcon( const char **bits, int width=-1, int height=-1 );

    // For compatibility with wxMSW where desired size is sometimes required to
    // distinguish between multiple icons in a resource.
    wxIcon( const wxString& filename, int type = wxBITMAP_TYPE_XPM, 
            int WXUNUSED(desiredWidth)=-1, int WXUNUSED(desiredHeight)=-1 ) :
        wxBitmap(filename, type)
    {
    }
    wxIcon( char **bits, int width=-1, int height=-1 );

    wxIcon& operator = (const wxIcon& icon);
    inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
    inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }
  
private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};


#endif // __GTKICONH__
