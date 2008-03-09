/////////////////////////////////////////////////////////////////////////////
// Name:        dcbuffer.h
// Purpose:     documentation for wxBufferedDC class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxBufferedDC
    @wxheader{dcbuffer.h}

    This class provides a simple way to avoid flicker: when drawing on it,
    everything is in fact first drawn on an in-memory buffer (a
    wxBitmap) and then copied to the screen, using the
    associated wxDC, only once, when this object is destroyed. wxBufferedDC itself
    is typically associated with wxClientDC, if you want to
    use it in your @c EVT_PAINT handler, you should look at
    wxBufferedPaintDC instead.

    When used like this, a valid @e dc must be specified in the constructor
    while the @e buffer bitmap doesn't have to be explicitly provided, by
    default this class will allocate the bitmap of required size itself. However
    using a dedicated bitmap can speed up the redrawing process by eliminating the
    repeated creation and destruction of a possibly big bitmap. Otherwise,
    wxBufferedDC can be used in the same way as any other device context.

    There is another possible use for wxBufferedDC is to use it to maintain a
    backing store for the window contents. In this case, the associated @e dc
    may be @NULL but a valid backing store bitmap should be specified.

    Finally, please note that GTK+ 2.0 as well as OS X provide double buffering
    themselves natively. You can either use wxWindow::IsDoubleBuffered
    to determine whether you need to use buffering or not, or use
    wxAutoBufferedPaintDC to avoid needless double
    buffering on the systems which already do it automatically.

    @library{wxcore}
    @category{dc}

    @seealso
    wxDC, wxMemoryDC, wxBufferedPaintDC, wxAutoBufferedPaintDC
*/
class wxBufferedDC : public wxMemoryDC
{
public:
    //@{
    /**
        If you use the first, default, constructor, you must call one of the
        Init() methods later in order to use the object.
        The other constructors initialize the object immediately and @c Init()
        must not be called after using them.
        
        @param dc
            The underlying DC: everything drawn to this object will be
            flushed to this DC when this object is destroyed.  You may pass @NULL
            in order to just initialize the buffer, and not flush it.
        @param area
            The size of the bitmap to be used for buffering (this bitmap is
            created internally when it is not given explicitly).
        @param buffer
            Explicitly provided bitmap to be used for buffering: this is
            the most efficient solution as the bitmap doesn't have to be recreated each
            time but it also requires more memory as the bitmap is never freed. The
        bitmap
            should have appropriate size, anything drawn outside of its bounds is
        clipped.
        @param style
            wxBUFFER_CLIENT_AREA to indicate that just the client area of
            the window is buffered, or wxBUFFER_VIRTUAL_AREA to indicate that the
        buffer bitmap
            covers the virtual area (in which case PrepareDC is automatically called
        for the actual window
            device context).
    */
    wxBufferedDC();
    wxBufferedDC(wxDC* dc, const wxSize& area,
                 int style = wxBUFFER_CLIENT_AREA);
    wxBufferedDC(wxDC* dc, wxBitmap& buffer,
                 int style = wxBUFFER_CLIENT_AREA);
    //@}

    /**
        Copies everything drawn on the DC so far to the underlying DC associated with
        this object, if any.
    */


    //@{
    /**
        These functions initialize the object created using the default constructor.
        Please see @ref ctor() "constructors documentation" for details.
    */
    void Init(wxDC* dc, const wxSize& area,
              int style = wxBUFFER_CLIENT_AREA);
    void Init(wxDC* dc, wxBitmap& buffer,
              int style = wxBUFFER_CLIENT_AREA);
    //@}
};


/**
    @class wxAutoBufferedPaintDC
    @wxheader{dcbuffer.h}

    This wxDC derivative can be used inside of an @c OnPaint() event handler to
    achieve
    double-buffered drawing. Just create an object of this class instead of
    wxPaintDC
    and make sure wxWindow::SetBackgroundStyle is called
    with wxBG_STYLE_CUSTOM somewhere in the class initialization code, and that's
    all you have
    to do to (mostly) avoid flicker.

    The difference between wxBufferedPaintDC and this class,
    is the lightweigthness - on platforms which have native double-buffering,
    wxAutoBufferedPaintDC is simply
    a typedef of wxPaintDC. Otherwise, it is a typedef of wxBufferedPaintDC.

    @library{wxbase}
    @category{dc}

    @seealso
    wxDC, wxBufferedPaintDC
*/
class wxAutoBufferedPaintDC : public wxBufferedPaintDC
{
public:
    /**
        Constructor. Pass a pointer to the window on which you wish to paint.
    */
    wxAutoBufferedPaintDC(wxWindow* window);
};


/**
    @class wxBufferedPaintDC
    @wxheader{dcbuffer.h}

    This is a subclass of wxBufferedDC which can be used
    inside of an @c OnPaint() event handler. Just create an object of this class
    instead
    of wxPaintDC and make sure wxWindow::SetBackgroundStyle
    is called with wxBG_STYLE_CUSTOM somewhere in the class initialization code,
    and that's all
    you have to do to (mostly) avoid flicker. The only thing to watch out for is
    that if you are
    using this class together with wxScrolledWindow, you probably
    do @b not want to call wxScrolledWindow::PrepareDC on it as it
    already does this internally for the real underlying wxPaintDC.

    @library{wxcore}
    @category{dc}

    @seealso
    wxDC, wxBufferedDC, wxAutoBufferedPaintDC
*/
class wxBufferedPaintDC : public wxBufferedDC
{
public:
    //@{
    /**
        As with @ref wxBufferedDC::ctor wxBufferedDC, you may either provide the
        bitmap to be used for buffering or let this object create one internally (in
        the latter case, the size of the client part of the window is used).
        Pass wxBUFFER_CLIENT_AREA for the @a style parameter to indicate that just the
        client area of
        the window is buffered, or wxBUFFER_VIRTUAL_AREA to indicate that the buffer
        bitmap
        covers the virtual area (in which case PrepareDC is automatically called for
        the actual window
        device context).
    */
    wxBufferedPaintDC(wxWindow* window, wxBitmap& buffer,
                      int style = wxBUFFER_CLIENT_AREA);
    wxBufferedPaintDC(wxWindow* window,
                      int style = wxBUFFER_CLIENT_AREA);
    //@}

    /**
        Copies everything drawn on the DC so far to the window associated with this
        object, using a wxPaintDC.
    */
};
