/////////////////////////////////////////////////////////////////////////////
// Name:        tipwin.h
// Purpose:     interface of wxTipWindow
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTipWindow

    Shows simple text in a popup tip window on creation.
    This is used by wxSimpleHelpProvider to show popup help.
    The window automatically destroys itself when the user clicks on it or it
    loses the focus.

    You may also use this class to emulate the tooltips when you need finer
    control over them than what the standard tooltips provide.

    @library{wxcore}
    @category{managedwnd}

    @see @ref wxToolTip
*/
class wxTipWindow : public wxWindow
{
public:
    /**
        The (weak) reference to wxTipWindow.

        wxTipWindow may close itself at any moment, so creating it as usual,
        with `new` and using a raw pointer to it is dangerous. Instead, use
        New() to create it and use the returned Ref which is guaranteed to
        become invalid when the tip window is closed.

        Note that the objects of this type can't be copied, but can be moved.

        To test if this object is still valid, it can be compared to @NULL or
        its explicit bool conversion operator can be used, i.e. either this:

        @code
            wxTipWindow::Ref tip = wxTipWindow::New(...);
            if ( tip != nullptr )
                tip->DoSomething();
        @endcode

        or this:

        @code
            wxTipWindow::Ref tip = wxTipWindow::New(...);
            if ( tip )
                tip->DoSomething();
        @endcode

        works fine.

        @since 3.3.2
    */
    class Ref
    {
    public:
        /**
            Default constructor.

            The object is created in invalid state.
        */
        Ref();

        /**
            Put object in invalid state.
        */
        Ref& operator=(std::nullptr_t);

        /**
            Returns whether the object is in a valid state.

            Object is in valid state if it is associated with a still open
            wxTipWindow.
        */
        bool operator!=(std::nullptr_t) const;

        /**
            Returns whether the tracked object is valid.

            This is equivalent to comparing the object to @NULL.
        */
        explicit operator bool() const;

        /**
            Returns a pointer to the tracked object.

            May only be called if the object is valid.
        */
        wxTipWindow* operator->() const;
    };

    /**
        Replace the deprecated single-step constructor.  See
        Create() for parameters

        @see Create()

        @since 3.3.2
    */
    static Ref New(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100,
                wxRect *rectBound = nullptr);

    /**
        Default constructor.

        If this constructor is used, Create() must be called later to actually
        create the window.

        Prefer using New() instead.

        @since 3.3.2
    */
    wxTipWindow();

    /**
        Use New() instead of this in new code.

        This constructor is preserved only for backward compatibility, don't
        use it in new code.

        @deprecated
    */
    wxTipWindow(wxWindow* parent, const wxString& text,
                wxCoord maxLength = 100,
                wxTipWindow** windowPtr = nullptr,
                wxRect* rectBounds = nullptr);

    /**
        Construct the actual window object.

        The tip is shown immediately.

        @param parent
            The parent window, must be non-null
        @param text
            The text to show, may contain the new line characters
        @param maxLength
            The length of each line, in pixels. Set to a very large
            value to avoid wrapping lines
        @param windowPtr
            Simply passed to SetTipWindowPtr() below, please see its
            documentation for the description of this parameter
        @param rectBounds
            If non-null, passed to SetBoundingRect() below, please see its
            documentation for the description of this parameter

        @since 3.3.2
    */
    bool Create(wxWindow* parent, const wxString& text,
                wxCoord maxLength = 100,
                wxTipWindow** windowPtr = nullptr,
                wxRect* rectBounds = nullptr);

    /**
        By default, the tip window disappears when the user clicks the mouse or presses
        a keyboard key or if it loses focus in any other way - for example because the
        user switched to another application window.

        Additionally, if a non-empty @a rectBound is provided, the tip window will
        also automatically close if the mouse leaves this area. This is useful to
        dismiss the tip mouse when the mouse leaves the object it is associated with.

        @param rectBound
            The bounding rectangle for the mouse in the screen coordinates
    */
    void SetBoundingRect(const wxRect& rectBound);

    /**
        When the tip window closes itself (which may happen at any moment and
        unexpectedly to the caller) it may @NULL out the pointer pointed to by
        @a windowPtr. This is helpful to avoid dereferencing the tip window which
        had been already closed and deleted.
    */
    void SetTipWindowPtr(wxTipWindow** windowPtr);
};

