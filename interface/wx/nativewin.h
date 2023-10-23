#pragma once

///////////////////////////////////////////////////////////////////////////////
// Name:        wx/nativewin.h
// Purpose:     wxNativeWindow documentation.
// Author:      Vadim Zeitlin
// Created:     2015-07-31
// Copyright:   (c) 2015 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxNativeWindow

    Allows embedding a native widget in an application using wxWidgets.

    This class can be used as a bridge between wxWidgets and native GUI
    toolkit, i.e. standard Windows controls under MSW, GTK+ widgets or Cocoa
    views. Using it involves writing code specific to each platform, at the
    very least for creating and destroying the native window, but possibly also
    to handle its events, but this class takes care of all the generic parts.

    @note Check whether @c wxHAS_NATIVE_WINDOW is defined before using this
        class as it is not available under all platforms.

    For example, to embed a native GTK+ "light switch" control in a wxWidgets
    dialog you could do the following:
    @code
        #include <wx/nativewin.h>

        GtkWidget* w = gtk_switch_new();
        wxNativeWindow* switch = new wxNativeWindow(parent, wxID_ANY, w);
        g_object_unref(w);
    @endcode
    and then use @c switch as usual, e.g. add it to a sizer to layout it
    correctly. Of course, you will still have to use the native GTK+ functions
    to handle its events and change or retrieve its state.

    Notice that the native window still remains owned by the caller, to allow
    reusing it later independently of wxWidgets. If you want wxWidgets to
    delete the native window when the wxNativeWindow itself is destroyed, you
    need to explicitly call Disown(). Otherwise you need to perform the
    necessary cleanup in your own code by calling the appropriate
    platform-specific function: under MSW, this is @c \::DestroyWindow(), under
    GTK @c g_object_unref() and under Cocoa -- @c -release:.

    See the "native" page of the @ref page_samples_widgets for the examples of using
    this class under all major platforms.

    @since 3.1.0

    @library{wxcore}
    @category{ctrl}
 */
class wxNativeWindow : public wxWindow
{
public:
    /**
        Default ctor, Create() must be called later to really create the window.
     */
    wxNativeWindow();

    /**
        Create a window from an existing native window handle.

        Notice that this ctor doesn't take the usual @c pos and @c size
        parameters, they're taken from the window handle itself.

        Use GetHandle() to check if the creation was successful, it will return
        0 if the handle was invalid.

        See Create() for the detailed parameters documentation.
     */
    wxNativeWindow(wxWindow* parent, wxWindowID winid, wxNativeWindowHandle handle);

    /**
        Really create the window after using the default ctor to create the C++
        object.

        @param parent A non-null parent window. For the platforms where the
            parent is used for creating the native window (e.g. MSW), this must
            be the wxWindow corresponding to the parent handle used when
            creating the native window.
        @param winid ID for the new window which will be used for the events
            generated by it and can also be used to FindWindowById().
        @param handle A valid native window handle, i.e. HWND under MSW.
        @return @true if the creation was successful or @false if it failed,
            typically because the supplied parameters are invalid.
     */
    bool Create(wxWindow* parent, wxWindowID winid, wxNativeWindowHandle handle);

    /**
        Indicate that the user code yields ownership of the native window.

        This method can be called at most once and after calling it, the native
        window will be destroyed when this wxNativeWindow object is.
     */
    void Disown();
};
