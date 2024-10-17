/////////////////////////////////////////////////////////////////////////////
// Name:        taskbar.h
// Purpose:     interface of wxTaskBarIcon
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
   On wxOSX/Cocoa the taskbar icon can be in the doc or in the status area.
   This enumeration can be used to select which will be instantiated.
*/
enum wxTaskBarIconType
{
    wxTBI_DOCK,
    wxTBI_CUSTOM_STATUSITEM,
    wxTBI_DEFAULT_TYPE
};



/**
    @class wxTaskBarIconEvent

    The event class used by wxTaskBarIcon.
    For a list of the event macros meant to be used with wxTaskBarIconEvent,
    please look at wxTaskBarIcon description.

    @library{wxcore}
    @category{events}
*/
class wxTaskBarIconEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon);
};


/**
    @class wxTaskBarIcon

    This class represents a taskbar icon.
    A taskbar icon is an icon that appears in the 'system tray' and responds to
    mouse clicks, optionally with a tooltip above it to help provide information.


    @section taskbaricon_linux_note Linux Note

    Taskbar is not supported by all window managers and desktop environments
    under Linux and IsAvailable() function should be used to check whether it
    is supported. In addition, even when the taskbar is supported, some
    environments don't support showing the application-specified tooltip for
    the icon in it.

    Due to this, there's no guarantee that wxTaskBarIcon will work correctly
    under Linux and so the applications should use it only as an optional
    component of their user interface. The user should be required to
    explicitly enable the taskbar icon on Unix, it shouldn't be on by default.

    @beginEventEmissionTable{wxTaskBarIconEvent}
    Note that not all ports are required to send these events and so it's better
    to override wxTaskBarIcon::CreatePopupMenu() or wxTaskBarIcon::GetPopupMenu()
    if all that the application does is that it shows a popup menu in reaction to
    mouse click.
    @event{EVT_TASKBAR_MOVE(func)}
        Process a @c wxEVT_TASKBAR_MOVE event.
    @event{EVT_TASKBAR_LEFT_DOWN(func)}
        Process a @c wxEVT_TASKBAR_LEFT_DOWN event.
    @event{EVT_TASKBAR_LEFT_UP(func)}
        Process a @c wxEVT_TASKBAR_LEFT_UP event.
    @event{EVT_TASKBAR_RIGHT_DOWN(func)}
        Process a @c wxEVT_TASKBAR_RIGHT_DOWN event.
    @event{EVT_TASKBAR_RIGHT_UP(func)}
        Process a @c wxEVT_TASKBAR_RIGHT_UP event.
    @event{EVT_TASKBAR_LEFT_DCLICK(func)}
        Process a @c wxEVT_TASKBAR_LEFT_DCLICK event.
    @event{EVT_TASKBAR_RIGHT_DCLICK(func)}
        Process a @c wxEVT_TASKBAR_RIGHT_DCLICK event.
    @event{EVT_TASKBAR_CLICK(func)}
        This is a synonym for either EVT_TASKBAR_RIGHT_DOWN or UP depending on
        the platform, use this event macro to catch the event which should result
        in the menu being displayed on the current platform.
    @endEventTable

    @library{wxcore}
    @category{misc}
*/
class wxTaskBarIcon : public wxEvtHandler
{
public:
    /**
        Default constructor.  The iconType is only applicable on wxOSX/Cocoa.
    */
    wxTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);

    /**
        Destroys the wxTaskBarIcon object, removing the icon if not already removed.
    */
    virtual ~wxTaskBarIcon();

    /**
        This method is similar to wxWindow::Destroy and can be used to schedule
        the task bar icon object for the delayed destruction: it will be deleted
        during the next event loop iteration, which allows the task bar
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
        Pops up a menu at the current mouse position.
        The events can be handled by a class derived from wxTaskBarIcon.

        @note
        It is recommended to override the CreatePopupMenu() or GetPopupMenu()
        callback instead of calling this method from event handler, because some
        ports (e.g. wxCocoa) may not implement PopupMenu() and mouse click events
        at all.
    */
    virtual bool PopupMenu(wxMenu* menu);

    /**
        Removes the icon previously set with SetIcon().
    */
    virtual bool RemoveIcon();

    /**
        Sets the icon, and optional tooltip text.
    */
    virtual bool SetIcon(const wxBitmapBundle& icon,
                         const wxString& tooltip = wxEmptyString);

    /**
        Returns true if system tray is available in the desktop environment the
        app runs under.

        On Windows and macOS, the tray is always available and this function
        simply returns true.

        On Unix, X11 environment may or may not provide the tray, depending on
        user's desktop environment. Most modern desktops support the tray via
        the System Tray Protocol by freedesktop.org
        (http://freedesktop.org/wiki/Specifications/systemtray-spec).

        @note Tray availability may change during application's lifetime
              under X11 and so applications shouldn't cache the result.

        @note wxTaskBarIcon supports older GNOME 1.2 and KDE 1/2 methods of
              adding icons to tray, but they are unreliable and this method
              doesn't detect them.

        @since 2.9.0
    */
    static bool IsAvailable();

protected:

    /**
        Called by the library when the user requests popup menu if
        GetPopupMenu() is not overridden.

        Override this function in order to provide popup menu associated with
        the icon if you don't want to override GetPopupMenu(), i.e. if you
        prefer creating a new menu every time instead of reusing the same menu.

        If CreatePopupMenu() returns @NULL (this happens by default), no menu
        is shown, otherwise the menu is displayed and then deleted by the
        library as soon as the user dismisses it. If you don't want the menu to
        be destroyed when it is dismissed, override GetPopupMenu() instead.
    */
    virtual wxMenu* CreatePopupMenu();

    /**
        Called by the library when the user requests popup menu.

        On Windows and Unix platforms this happens when the user right-clicks
        the icon, so overriding this method is the simplest way to implement
        the standard behaviour of showing a menu when the taskbar icon is
        clicked.

        If GetPopupMenu() returns @NULL (this happens by default),
        CreatePopupMenu() is called next and its menu is used (if non-null).
        Otherwise the menu returned by GetPopupMenu() is shown and, contrary to
        CreatePopupMenu(), not destroyed when the user dismisses it, allowing
        to reuse the same menu pointer multiple times.

        @since 3.1.5
    */
    virtual wxMenu* GetPopupMenu();
};


wxEventType wxEVT_TASKBAR_MOVE;
wxEventType wxEVT_TASKBAR_LEFT_DOWN;
wxEventType wxEVT_TASKBAR_LEFT_UP;
wxEventType wxEVT_TASKBAR_RIGHT_DOWN;
wxEventType wxEVT_TASKBAR_RIGHT_UP;
wxEventType wxEVT_TASKBAR_LEFT_DCLICK;
wxEventType wxEVT_TASKBAR_RIGHT_DCLICK;
wxEventType wxEVT_TASKBAR_CLICK;
wxEventType wxEVT_TASKBAR_BALLOON_TIMEOUT;
wxEventType wxEVT_TASKBAR_BALLOON_CLICK;
