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
  
    wxToolTip( const wxString &tip );
    ~wxToolTip();
    
    wxString GetTip();
    
    void Enable( bool flag );
    void SetDelay( long msecs );
    bool Ok();

  // implementation
    
    wxString     m_text;
    
    GtkTooltips *m_tooltips;
    GdkColor    *m_bg;
    GdkColor    *m_fg;
    
    void Create( GtkWidget *tool );
};

#endif // __GTKTOOLTIPH__
