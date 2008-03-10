/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     interface of wxPaintDC
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPaintDC
    @wxheader{dcclient.h}

    A wxPaintDC must be constructed if an application wishes to paint on the
    client area of a window from within an @b OnPaint event.
    This should normally be constructed as a temporary stack object; don't store
    a wxPaintDC object. If you have an OnPaint handler, you @e must create a
    wxPaintDC
    object within it even if you don't actually use it.

    Using wxPaintDC within OnPaint is important because it automatically
    sets the clipping area to the damaged area of the window. Attempts to draw
    outside this area do not appear.

    To draw on a window from outside @b OnPaint, construct a wxClientDC object.

    To draw on the whole window including decorations, construct a wxWindowDC object
    (Windows only).

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC, wxPaintDC, wxWindowDC, wxScreenDC
*/
class wxPaintDC : public wxWindowDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxPaintDC(wxWindow* window);
};



/**
    @class wxClientDC
    @wxheader{dcclient.h}

    A wxClientDC must be constructed if an application wishes to paint on the
    client area of a window from outside an @b OnPaint event.
    This should normally be constructed as a temporary stack object; don't store
    a wxClientDC object.

    To draw on a window from within @b OnPaint, construct a wxPaintDC object.

    To draw on the whole window including decorations, construct a wxWindowDC object
    (Windows only).

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC, wxPaintDC, wxWindowDC, wxScreenDC
*/
class wxClientDC : public wxWindowDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxClientDC(wxWindow* window);
};



/**
    @class wxWindowDC
    @wxheader{dcclient.h}

    A wxWindowDC must be constructed if an application wishes to paint on the
    whole area of a window (client and decorations).
    This should normally be constructed as a temporary stack object; don't store
    a wxWindowDC object.

    To draw on a window from inside @b OnPaint, construct a wxPaintDC object.

    To draw on the client area of a window from outside @b OnPaint, construct a
    wxClientDC object.

    To draw on the whole window including decorations, construct a wxWindowDC object
    (Windows only).

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC, wxPaintDC, wxClientDC, wxScreenDC
*/
class wxWindowDC : public wxDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxWindowDC(wxWindow* window);
};

