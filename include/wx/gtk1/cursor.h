/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCURSORH__
#define __GTKCURSORH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxWindow;

class wxCursor;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursor: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

  public:

    wxCursor(void);
    wxCursor( int cursorId );
    wxCursor( const wxCursor &cursor );
    wxCursor( const wxCursor *cursor );
    ~wxCursor(void);
    wxCursor& operator = ( const wxCursor& cursor );
    bool operator == ( const wxCursor& cursor );
    bool operator != ( const wxCursor& cursor );
    bool Ok(void) const;
    
  private:
  public:
  
    friend wxWindow;
  
    GdkCursor *GetCursor(void) const;

    // no data :-)
};

#endif // __GTKCURSORH__
