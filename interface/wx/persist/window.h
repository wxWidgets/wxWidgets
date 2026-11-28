/////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/window.h
// Purpose:     interface of wxPersistentWindow<>
// Author:      Vadim Zeitlin
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Base class for persistent windows.

    Compared to wxPersistentObject this class does three things:
        - Most importantly, wxPersistentWindow catches wxWindowDestroyEvent
        generated when the window is destroyed and saves its properties
        automatically when it happens.
        - It implements GetName() using wxWindow::GetName() so that the derived
        classes don't need to do it.
        - It adds a convenient wxPersistentWindow::Get() accessor returning the
        window object of the correct type.
 */
template <class T>
class wxPersistentWindow : public wxPersistentObject
{
public:
    /// The type of the associated window.
    typedef T WindowType;

    /**
        Constructor for a persistent window object.

        The constructor uses wxEvtHandler::Bind() to catch
        wxWindowDestroyEvent generated when the window is destroyed and call
        wxPersistenceManager::SaveAndUnregister() when this happens. This
        ensures that the window properties are saved and that this object
        itself is deleted when the window is.
     */
    wxPersistentWindow(WindowType *win);

    WindowType *Get() const { return static_cast<WindowType *>(GetWindow()); }
    /**
        Implements the base class pure virtual method using wxWindow::GetName().

        Notice that window names are usually not unique while this function
        must return a unique (at least among the objects of this type) string.
        Because of this you need to specify a non-default window name in its
        constructor when creating it or explicitly call wxWindow::SetName()
        before saving or restoring persistent properties.
     */
    virtual wxString GetName() const;

protected:
    /**
        Save a coordinate, i.e. either a position or a size, in pixels.

        This function must be used instead of wxPersistentObject::SaveValue()
        for the values representing a size or a position in pixels, as it also
        stores the DPI in which the coordinate is expressed, allowing
        RestoreCoord() to rescale it if the DPI changes when the coordinate
        is restored later.

        Note that under the platforms using DPI-independent pixels, such as
        wxGTK and wxOSX, the saved values don't change with the DPI and so no
        DPI is stored for them at all, meaning that they are never rescaled.

        Also note that the values which are not really expressed in pixels,
        such as the special value -1 often used to indicate the absence of a
        value, are not affected by the rescaling and can be saved using this
        function too.

        @param name
            The name of the value in the configuration file.
        @param value
            The value to save.
        @return
            @true if the value was saved or @false if an error occurred.

        @since 3.3.4
     */
    bool SaveCoord(const wxString& name, int value) const;

    /**
        Restore a coordinate saved by SaveCoord().

        If the DPI in which the coordinate is expressed is different from the
        DPI at which it had been saved, it is rescaled accordingly. If the
        latter is unknown, e.g. because the value was saved by a version of
        wxWidgets older than 3.3.4, the coordinate is restored unchanged.

        As explained in SaveCoord() documentation, no DPI is stored under
        the platforms using DPI-independent pixels, so this function simply
        behaves as wxPersistentObject::RestoreValue() there.

        @param name
            The same name as was used by SaveCoord().
        @param value
            Non-@NULL pointer to the variable to fill with the saved value if
            it was read successfully, which is not modified otherwise.
        @return
            @true if the value was successfully read or @false if it was not
            found or an error occurred.

        @since 3.3.4
     */
    bool RestoreCoord(const wxString& name, int* value);
};
