/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:     wxToolTip class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKTOOLTIPH__
#define __GTKTOOLTIPH__

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/object.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolTip;
class WXDLLIMPEXP_CORE wxWindow;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolTip : public wxObject
{
public:
    wxToolTip( const wxString &tip );

    // globally change the tooltip parameters
    static void Enable( bool flag );
    static void SetDelay( long msecs );

    // get/set the tooltip text
    void SetTip( const wxString &tip );
    wxString GetTip() const { return m_text; }

    wxWindow *GetWindow() const { return m_window; }
    bool IsOk() const { return m_window != NULL; }


    // wxGTK-only from now on

    // this forwards back to wxWindow::ApplyToolTip()
    void Apply( wxWindow *win );

    // this just sets the given tooltip for the specified widget
    //
    // tip must be already UTF-8 encoded
    static void Apply(GtkWidget *w, const wxCharBuffer& tip);

private:
    wxString     m_text;
    wxWindow    *m_window;

    DECLARE_ABSTRACT_CLASS(wxToolTip)
};

#endif // __GTKTOOLTIPH__
