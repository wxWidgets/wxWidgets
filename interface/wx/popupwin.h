/////////////////////////////////////////////////////////////////////////////
// Name:        popupwin.h
// Purpose:     interface of wxPopupWindow
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPopupWindow

    A special kind of top level window used for popup menus,
    combobox popups and such.

    @library{wxcore}
    @category{managedwnd}

    @see wxDialog, wxFrame
*/

class wxPopupWindow: public wxNonOwnedWindow
{
public:

    /**
      Constructor
    */
    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE);

    /**
      Create method for two-step creation
    */
    bool Create(wxWindow *parent, int flags = wxBORDER_NONE);

    /**
        Move the popup window to the right position, i.e. such that it is
        entirely visible.

        The popup is positioned at ptOrigin + size if it opens below and to the
        right (default), at ptOrigin - sizePopup if it opens above and to the
        left etc.

        @param ptOrigin
            Must be given in screen coordinates!
        @param sizePopup
            The size of the popup window
    */
    virtual void Position(const wxPoint& ptOrigin,
                          const wxSize& sizePopup);
};

/**
    @class wxPopupTransientWindow

    A wxPopupWindow which disappears automatically when the user clicks mouse
    outside it or if it loses focus in any other way.

    This window can be useful for implementing custom combobox-like controls
    for example.

    @library{wxcore}
    @category{managedwnd}

    @see wxPopupWindow
*/

class wxPopupTransientWindow : public wxPopupWindow
{
public:
    /**
        Constructor.
    */
    wxPopupTransientWindow(wxWindow *parent, int flags = wxBORDER_NONE);

    /**
        Popup the window (this will show it too).

        If @a winFocus is non-@NULL, it will be kept focused while this window
        is shown, otherwise this window itself will receive focus. In any case,
        the popup will disappear automatically if it loses focus because of a
        user action.

        @see Dismiss()
    */
    virtual void Popup(wxWindow *focus = NULL);

    /**
        Hide the window.
    */
    virtual void Dismiss();

    /**
        Called when a mouse is pressed while the popup is shown.

        Return @true from here to prevent its normal processing by the popup
        (which consists in dismissing it if the mouse is clicked outside it).
    */
    virtual bool ProcessLeftDown(wxMouseEvent& event);
};
