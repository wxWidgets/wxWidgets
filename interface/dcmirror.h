/////////////////////////////////////////////////////////////////////////////
// Name:        dcmirror.h
// Purpose:     interface of wxMirrorDC
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMirrorDC
    @wxheader{dcmirror.h}

    wxMirrorDC is a simple wrapper class which is always associated with a real
    wxDC object and either forwards all of its operations to it
    without changes (no mirroring takes place) or exchanges @e x and @e y
    coordinates which makes it possible to reuse the same code to draw a figure and
    its mirror -- i.e. reflection related to the diagonal line x == y.

    wxMirrorDC has been added in wxWidgets version 2.5.0.

    @library{wxcore}
    @category{dc}
*/
class wxMirrorDC : public wxDC
{
public:
    /**
        Creates a (maybe) mirrored DC associated with the real @e dc. Everything
        drawn on wxMirrorDC will appear (and maybe mirrored) on @e dc.
        @a mirror specifies if we do mirror (if it is @true) or not (if it is
        @false).
    */
    wxMirrorDC(wxDC& dc, bool mirror);
};

