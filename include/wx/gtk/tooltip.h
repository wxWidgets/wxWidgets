/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:     wxToolTip class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKTOOLTIPH__
#define __GTKTOOLTIPH__

#ifdef __GNUG__
    #pragma interface
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/object.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

class wxToolTip;
class wxWindow;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

class wxToolTip : public wxObject
{
public:
    // globally change the tooltip parameters
    static void Enable( bool flag );
    static void SetDelay( long msecs );

    wxToolTip( const wxString &tip );

    // get/set the tooltip text
    void SetTip( const wxString &tip );
    wxString GetTip() const { return m_text; }

    wxWindow *GetWindow() const { return m_window; }
    bool IsOk() const { return m_window != NULL; }

    // implementation
    void Apply( wxWindow *win );

private:
    wxString     m_text;
    wxWindow    *m_window;
};

#endif // __GTKTOOLTIPH__
