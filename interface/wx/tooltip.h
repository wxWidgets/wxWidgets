/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:     interface of wxToolTip
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxToolTip
    @wxheader{tooltip.h}

    This class holds information about a tooltip associated with a window (see
    wxWindow::SetToolTip()).

    The four static methods, wxToolTip::Enable(), wxToolTip::SetDelay()
    wxToolTip::SetAutoPop() and wxToolTip::SetReshow() can be used to globally
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

        @note May not be supported on all platforms (eg. wxCocoa).
    */
    static void Enable(bool flag);

    /**
        Get the tooltip text.
    */
    wxString GetTip() const;

    /**
        Get the associated window.
    */
    wxWindow* GetWindow() const;

    /**
        Set the delay after which the tooltip disappears or how long a tooltip
        remains visible.

        @note May not be supported on all platforms (eg. wxCocoa, GTK, Palmos).
    */
    static void SetAutoPop(long msecs);

    /**
        Set the delay after which the tooltip appears.

        @note May not be supported on all platforms (eg. wxCocoa).
    */
    static void SetDelay(long msecs);

    /**
        Set the delay between subsequent tooltips to appear.

        @note May not be supported on all platforms (eg. wxCocoa, GTK, Palmos).
    */
    static void SetReshow(long msecs);

    /**
        Set the tooltip text.
    */
    void SetTip(const wxString& tip);
};

