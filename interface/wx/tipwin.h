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
        The (weak) reference of class wxTipWindow

        wxTipWindow may close itself, so provide a smart pointer
        that acts as a weak reference to wxTipWindow.

        This is a move-only type.

        Note that this is not a wxWeakRef<> because this is set
        to @NULL when wxTipWindow is closed, which may be
        "long" before wxTipWindow is destroyed, and wxWeakRef<>
        is set to @NULL on object destruction.
    */
    class Ref
    {
    public:
        /**
            Default constructor.

            Tracked object is set to @NULL.
        */
        Ref();

        /**
            Stop tracking wxTipWindow.
        */
        void Reset();

        /**
            Explicit conversion to bool.
            Returns whether the tracked object is not @NULL.
        */
        explicit operator bool() const;

        /**
            Smart pointer member access. Returns a pointer to the tracked object.
            If the internal pointer is @NULL this method will cause an assert in debug mode.
        */
        wxTipWindow* operator->() const;
    };

    /*
        Replace the deprecated single-step constructor.  See
        Create() for parameters

        @see Create()
    */
    static Ref New(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100,
                wxRect *rectBound = nullptr);

    /**
        Default constructor.
    */
    wxTipWindow();

    /**
        @deprecated

        Use two-phase construction instead due to race condition
        where windowPtr may be set to @NULL before it is set by
        result of `new wxTipWindow(...)`.
    */
    wxTipWindow(wxWindow* parent, const wxString& text,
                wxCoord maxLength = 100,
                wxTipWindow** windowPtr = nullptr,
                wxRect* rectBounds = nullptr);

    /**
        Construct the actual window object after creating the C++
        object.  The tip is shown immediately.

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

