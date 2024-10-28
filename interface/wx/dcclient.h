/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     interface of wxClientDC and wxPaintDC
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPaintDC

    A wxPaintDC must be constructed if an application wishes to paint on the
    client area of a window from within an EVT_PAINT() event handler. This
    should normally be constructed as a temporary stack object; don't store a
    wxPaintDC object. If you have an EVT_PAINT() handler, you @e must create a
    wxPaintDC object within it even if you don't actually use it.

    Using wxPaintDC within your EVT_PAINT() handler is important because it
    automatically sets the clipping area to the damaged area of the window.
    Attempts to draw outside this area do not appear.

    A wxPaintDC object is initialized to use the same font and colours as the
    window it is associated with.

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC
*/
class wxPaintDC : public wxClientDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxPaintDC(wxWindow* window);
};



/**
    @class wxClientDC

    Deprecated class for drawing on the client area of a window.

    wxClientDC should not be used any longer, please use wxInfoDC instead for
    obtaining information about the device context associated with a window.

    @note While wxClientDC may also be used for drawing on the client area of a
    window from outside an EVT_PAINT() handler in some ports, this does @em not
    work on most of the platforms: neither wxOSX nor wxGTK with GTK 3 Wayland
    backend support this at all, so drawing using wxClientDC simply doesn't
    have any effect there. CanBeUsedForDrawing() can be used to determine
    whether wxClientDC can be used for drawing in the current environment, but
    it is recommended to only draw on the window using wxPaintDC, as this is
    guaranteed to work everywhere. To redraw a small part of the window, use
    wxWindow::RefreshRect() to invalidate just this part and check
    wxWindow::GetUpdateRegion() in the paint event handler to redraw this part
    only.

    wxClientDC objects should normally be constructed as temporary stack
    objects, i.e. don't store a wxClientDC object.

    A wxClientDC object is initialized to use the same font and colours as the
    window it is associated with.

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC, wxPaintDC, wxWindowDC
*/
class wxClientDC : public wxWindowDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxClientDC(wxWindow* window);

    /**
        Return true if drawing on wxClientDC actually works.

        In many environments (currently this includes wxGTK when using Wayland
        backend, wxMSW when using double buffering and wxOSX in all cases),
        wxClientDC can be only used for obtaining information about the device
        context, but not for actually drawing on it. Portable code should avoid
        using wxClientDC completely, as explained in the class documentation,
        but it is also possible to optionally use it only when it does work,
        i.e. when this function returns @true.

        @param window The window that would be used with wxClientDC.

        @since 3.3.0
     */
    static bool CanBeUsedForDrawing(const wxWindow* window);
};



/**
    @class wxWindowDC

    Deprecated class for drawing on the entire window.

    Please don't use this class in the new code, as it doesn't work on modern
    systems any longer and using it is not guaranteed to have any effect at all.

    A wxWindowDC must be constructed if an application wishes to paint on the
    whole area of a window (client and decorations). This should normally be
    constructed as a temporary stack object; don't store a wxWindowDC object.

    To draw on a window from inside an EVT_PAINT() handler, construct a
    wxPaintDC object instead.

    To draw on the client area of a window from outside an EVT_PAINT() handler,
    construct a wxClientDC object.

    A wxWindowDC object is initialized to use the same font and colours as the
    window it is associated with.

    @library{wxcore}
    @category{dc}

    @see wxDC, wxMemoryDC, wxPaintDC, wxClientDC
*/
class wxWindowDC : public wxDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxWindowDC(wxWindow* window);
};

