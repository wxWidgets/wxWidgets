/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKTOOLTIPH__
#define __GTKTOOLTIPH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxToolTip;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

class wxToolTip: public wxObject
{
public:
  
    wxToolTip() {}
    
    static void Add( wxWindow *tool, const wxString &tip );
    static void Enable( bool flag );
    static void SetDelay( long msecs );

    
};

#endif // __GTKTOOLTIPH__
