/////////////////////////////////////////////////////////////////////////////
// Name:        taskbar.h
// Purpose:     interface of wxTaskBarIcon
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTaskBarIcon

    This class represents a taskbar icon. A taskbar icon is an icon that appears in
    the 'system tray' and responds to mouse clicks, optionally with a tooltip above it to help provide information.

    @library{wxadv}
    @category{FIXME}
*/
class wxTaskBarIcon : public wxEvtHandler
{
public:
    /**
        Default constructor.
    */
    wxTaskBarIcon();

    /**
        Destroys the wxTaskBarIcon object, removing the icon if not already removed.
    */
    virtual ~wxTaskBarIcon();

    /**
        This method is called by the library when the user requests popup menu
        (on Windows and Unix platforms, this is when the user right-clicks the icon).
        Override this function in order to provide popup menu associated with the icon.
        If CreatePopupMenu returns @NULL (this happens by default),
        no menu is shown, otherwise the menu is
        displayed and then deleted by the library as soon as the user dismisses it.
        The events can be handled by a class derived from wxTaskBarIcon.
    */
    virtual wxMenu* CreatePopupMenu();

    /**
        This method is similar to wxWindow::Destroy and can
        be used to schedule the task bar icon object for the delayed destruction: it
        will be deleted during the next event loop iteration, which allows the task bar
        icon to process any pending events for it before being destroyed.
    */
    void Destroy();

    /**
        Returns @true if SetIcon() was called with no subsequent RemoveIcon().
    */
    bool IsIconInstalled() const;

    /**
        Returns @true if the object initialized successfully.
    */
    bool IsOk() const;

    /**
        Pops up a menu at the current mouse position. The events can be handled by
        a class derived from wxTaskBarIcon.
    */
    virtual bool PopupMenu(wxMenu* menu);

    /**
        Removes the icon previously set with SetIcon().
    */
    virtual bool RemoveIcon();

    /**
        Sets the icon, and optional tooltip text.
    */
    bool SetIcon(const wxIcon& icon, const wxString& tooltip);
};

