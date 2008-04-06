/////////////////////////////////////////////////////////////////////////////
// Name:        vidmode.h
// Purpose:     interface of wxVideoMode
// Author:      wxWidgets team
// RCS-ID:      $Id: display.h 52634 2008-03-20 13:45:17Z VS $
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxVideoMode
    @wxheader{display.h}

    Determines the sizes and locations of displays connected to the system.

    @library{wxcore}
    @category{FIXME}

    @stdobjects
    ::wxDefaultVideoMode

    @see wxClientDisplayRect(), wxDisplaySize(), wxDisplaySizeMM()
*/
class wxVideoMode
{
public:
    /**
        Constructs this class using the given parameters.
    */
    wxVideoMode(int width = 0, int height = 0, int depth = 0, int freq = 0);

    bool operator==(const wxVideoMode& m) const
    bool operator!=(const wxVideoMode& mode) const

    /**
        Returns true if this mode matches the other one in the sense that all
        non zero fields of the other mode have the same value in this one
        (except for refresh which is allowed to have a greater value).
    */
    bool Matches(const wxVideoMode& other) const;

    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;

    /**
        Returns true if the object has been initialized
    */
    bool IsOk() const;
};

/**
    A global wxVideoMode instance used by wxDisplay.
*/
wxVideoMode wxDefaultVideoMode;
