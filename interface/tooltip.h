/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:     documentation for wxToolTip class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxToolTip
    @wxheader{tooltip.h}
    
    This class holds information about a tooltip associated with a window
    (see wxWindow::SetToolTip).
    
    The four static methods, wxToolTip::Enable,
    wxToolTip::SetDelay 
    wxToolTip::SetAutoPop and 
    wxToolTip::SetReshow can be used to globally
    alter tooltips behaviour.
    
    @library{wxcore}
    @category{help}
*/
class wxToolTip : public wxObject
{
public:
    /**
        Constructor.
    */
    wxToolTip(const wxString& tip);

    /**
        Enable or disable tooltips globally.
        
        May not be supported on all platforms (eg. wxCocoa).
    */
    static void Enable(bool flag);

    /**
        Get the tooltip text.
    */
    wxString GetTip();

    /**
        Get the associated window.
    */
    wxWindow* GetWindow();

    /**
        Set the delay after which the tooltip disappears or how long a
        tooltip remains visible.
        May not be supported on all platforms (eg. wxCocoa, GTK, Palmos).
    */
    static void SetAutoPop(long msecs);

    /**
        Set the delay after which the tooltip appears.
        
        May not be supported on all platforms (eg. wxCocoa).
    */
    static void SetDelay(long msecs);

    /**
        Set the delay between subsequent tooltips to appear.
        May not be supported on all platforms (eg. wxCocoa, GTK, Palmos).
    */
    static void SetReshow(long msecs);

    /**
        Set the tooltip text.
    */
    void SetTip(const wxString& tip);
};
