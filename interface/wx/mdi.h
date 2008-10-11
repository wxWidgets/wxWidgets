/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     interface of wxMDIClientWindow
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMDIClientWindow

    An MDI client window is a child of wxMDIParentFrame, and manages zero or
    more wxMDIChildFrame objects.

    @remarks

    The client window is the area where MDI child windows exist. It doesn't have to
    cover the whole parent frame; other windows such as toolbars and a help window
    might coexist with it. There can be scrollbars on a client window, which are
    controlled by the parent window style.

    The wxMDIClientWindow class is usually adequate without further derivation, and
    it is created automatically when the MDI parent frame is created. If the application
    needs to derive a new class, the function wxMDIParentFrame::OnCreateClient() must
    be overridden in order to give an opportunity to use a different class of client
    window.

    Under Windows 95, the client window will automatically have a sunken border style
    when the active child is not maximized, and no border style when a child is maximized.

    @library{wxcore}
    @category{managedwnd}

    @see wxMDIChildFrame, wxMDIParentFrame, wxFrame
*/
class wxMDIClientWindow : public wxWindow
{
public:

    /**
        Default constructor.
    */
    wxMDIClientWindow();

    /**
        Constructor, creating the window.

        @param parent
            The window parent.
        @param style
            The window style. Currently unused.

        @remarks This constructor is called within wxMDIParentFrame::OnCreateClient().

        @see wxMDIParentFrame::wxMDIParentFrame(), wxMDIParentFrame::OnCreateClient()
    */
    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);

    /**
        Destructor.
    */
    virtual ~wxMDIClientWindow();

    /**
        Used in two-step frame construction. See wxMDIClientWindow()
        for further details.
    */
    virtual bool CreateClient(wxMDIParentFrame* parent, long style = 0);
};



/**
    @class wxMDIParentFrame

    An MDI (Multiple Document Interface) parent frame is a window which can contain
    MDI child frames in its own 'desktop'. It is a convenient way to avoid window
    clutter, and is used in many popular Windows applications, such as Microsoft Word(TM).

    @remarks

    There may be multiple MDI parent frames in a single application, but this probably
    only makes sense within programming development environments.

    Child frames may be of class wxMDIChildFrame (contained within the parent frame)
    or wxFrame (shown as a top-level frame).

    An MDI parent frame always has a wxMDIClientWindow associated with it, which is the
    parent for MDI child frames. This client window may be resized to accommodate non-MDI
    windows, as seen in Microsoft Visual C++ (TM) and Microsoft Publisher (TM), where
    a documentation window is placed to one side of the workspace.

    MDI remains popular despite dire warnings from Microsoft itself that MDI is an obsolete
    user interface style.

    The implementation is native in Windows, and simulated under Motif. Under Motif, the
    child window frames will often have a different appearance from other frames because
    the window decorations are simulated.


    @beginStyleTable
    @style{wxCAPTION}
           Puts a caption on the frame.
    @style{wxDEFAULT_FRAME_STYLE}
           Defined as @c wxMINIMIZE_BOX | @c wxMAXIMIZE_BOX | @c wxTHICK_FRAME |
           @c wxSYSTEM_MENU | @c wxCAPTION.
    @style{wxHSCROLL}
           Displays a horizontal scrollbar in the client window, allowing the
           user to view child frames that are off the current view.
    @style{wxICONIZE}
           Display the frame iconized (minimized) (Windows only).
    @style{wxMAXIMIZE}
           Displays the frame maximized (Windows only).
    @style{wxMAXIMIZE_BOX}
           Displays a maximize box on the frame (Windows and Motif only).
    @style{wxMINIMIZE}
           Identical to @c wxICONIZE.
    @style{wxMINIMIZE_BOX}
           Displays a minimize box on the frame (Windows and Motif only).
    @style{wxRESIZE_BORDER}
           Displays a resizeable border around the window (Motif only; for
           Windows, it is implicit in @c wxTHICK_FRAME).
    @style{wxSTAY_ON_TOP}
           Stay on top of other windows (Windows only).
    @style{wxSYSTEM_MENU}
           Displays a system menu (Windows and Motif only).
    @style{wxTHICK_FRAME}
           Displays a thick frame around the window (Windows and Motif only).
    @style{wxVSCROLL}
           Displays a vertical scrollbar in the client window, allowing the
           user to view child frames that are off the current view.
    @style{wxFRAME_NO_WINDOW_MENU}
           Under Windows, removes the Window menu that is normally added
           automatically.
    @endStyleTable

    @library{wxcore}
    @category{managedwnd}

    @see wxMDIChildFrame, wxMDIClientWindow, wxFrame, wxDialog
*/
class wxMDIParentFrame : public wxFrame
{
public:

    /**
        Default constructor.
    */
    wxMDIParentFrame();

    /**
        Constructor, creating the window.

        @param parent
            The window parent. This should be @NULL.
        @param id
            The window identifier. It may take a value of -1 to indicate a default
            value.
        @param title
            The caption to be displayed on the frame's title bar.
        @param pos
            The window position. The value @c wxDefaultPosition indicates a default position,
            chosen by either the windowing system or wxWidgets, depending on platform.
        @param size
            The window size. The value @c wxDefaultSize indicates a default size, chosen by
            either the windowing system or wxWidgets, depending on platform.
        @param style
            The window style. See wxMDIParentFrame.
        @param name
            The name of the window. This parameter is used to associate a name
            with the item, allowing the application user to set Motif resource values
            for individual windows.

        @remarks During the construction of the frame, the client window will be
                 created. To use a different class from wxMDIClientWindow, override
                 OnCreateClient().
                 Under Windows 95, the client window will automatically have a
                 sunken border style when the active child is not maximized,
                 and no border style when a child is maximized.

        @see Create(), OnCreateClient()
    */
    wxMDIParentFrame(wxWindow* parent, wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = "frame");

    /**
        Destructor. Destroys all child windows and menu bar if present.
    */
    virtual ~wxMDIParentFrame();

    /**
        Activates the MDI child following the currently active one.

        @see ActivatePrevious()
    */
    virtual void ActivateNext();

    /**
        Activates the MDI child preceding the currently active one.

        @see ActivateNext()
    */
    virtual void ActivatePrevious();

    /**
        Arranges any iconized (minimized) MDI child windows.

        @see Cascade(), Tile()
    */
    virtual void ArrangeIcons();

    /**
        Arranges the MDI child windows in a cascade.

        @see Tile(), ArrangeIcons()
    */
    virtual void Cascade();

    /**
        Used in two-step frame construction.
        See wxMDIParentFrame() for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = "frame");

    /**
        Returns a pointer to the active MDI child, if there is one.
    */
    wxMDIChildFrame* GetActiveChild() const;

    /**
        This gets the size of the frame 'client area' in pixels.

        @param width
            Receives the client width in pixels.
        @param height
            Receives the client height in pixels.

        @remarks

        The client area is the area which may be drawn on by the programmer, excluding
        title bar, border, status bar, and toolbar if present.

        If you wish to manage your own toolbar (or perhaps you have more than one),
        provide an @b OnSize event handler. Call GetClientSize() to find how much space
        there is for your windows and don't forget to set the size and position of
        the MDI client window as well as your toolbar and other windows (but not the
        status bar).

        If you have set a toolbar with wxMDIParentFrame::SetToolbar(), the client size
        returned will have subtracted the toolbar height. However, the available positions
        for the client window and other windows of the frame do not start at zero - you
        must add the toolbar height.

        The position and size of the status bar and toolbar (if known to the frame) are
        always managed by wxMDIParentFrame, regardless of what behaviour is defined in
        your @b OnSize event handler. However, the client window position and size are always
        set in @b OnSize, so if you override this event handler, make sure you deal with the
        client window.

        You do not have to manage the size and position of MDI child windows, since they
        are managed automatically by the client window.

        @see GetToolBar(), SetToolBar(), wxMDIClientWindow

        @beginWxPythonOnly
        The wxPython version of this method takes no arguments and returns a tuple containing
        width and height.
        @endWxPythonOnly
    */
    virtual void GetClientSize(int* width, int* height) const;

    /**
        Returns a pointer to the client window.

        @see OnCreateClient()
    */
    wxMDIClientWindow* GetClientWindow() const;

    /**
        Returns the window being used as the toolbar for this frame.

        @see SetToolBar()
    */
    virtual wxWindow* GetToolBar() const;

    /**
        Returns the current Window menu (added by wxWidgets to the menubar). This
        function
        is available under Windows only.
    */
    wxMenu* GetWindowMenu() const;

    /**
        Override this to return a different kind of client window. If you override this
        function, you must create your parent frame in two stages, or your function will
        never be called, due to the way C++ treats virtual functions called from constructors.
        For example:

        @code
        frame = new MyParentFrame;
        frame->Create(parent, myParentFrameId, wxT("My Parent Frame"));
        @endcode

        @remarks

        You might wish to derive from wxMDIClientWindow in order to implement different
        erase behaviour, for example, such as painting a bitmap on the background.

        Note that it is probably impossible to have a client window that scrolls
        as well as painting a bitmap or pattern, since in @b OnScroll, the scrollbar
        positions always return zero.

        @see GetClientWindow(), wxMDIClientWindow
    */
    virtual wxMDIClientWindow* OnCreateClient();

    /**
        Sets the window to be used as a toolbar for this
        MDI parent window. It saves the application having to manage the positioning
        of the toolbar MDI client window.

        @param toolbar
            Toolbar to manage.

        @remarks

        When the frame is resized, the toolbar is resized to be the width of the frame
        client area, and the toolbar height is kept the same.

        When the frame is resized, the toolbar is resized to be the width of the frame
        client area, and the toolbar height is kept the same.

        The parent of the toolbar must be this frame.

        If you wish to manage your own toolbar (or perhaps you have more than one),
        don't call this function, and instead manage your subwindows and the MDI client
        window by providing an @b OnSize event handler. Call wxMDIParentFrame::GetClientSize()
        to find how much space there is for your windows.

        Note that SDI (normal) frames and MDI child windows must always have their toolbars
        managed by the application.

        @see GetToolBar(), GetClientSize()
    */
    virtual void SetToolBar(wxWindow* toolbar);

    /**
        Call this to change the current Window menu.
        Ownership of the menu object passes to the frame when you call this function.

        This call is available under Windows only.

        To remove the window completely, use the @c wxFRAME_NO_WINDOW_MENU window style.
    */
    void SetWindowMenu(wxMenu* menu);

    /**
        Tiles the MDI child windows either horizontally or vertically depending on
        whether @a orient is @c wxHORIZONTAL or @c wxVERTICAL.

        Currently only implemented for MSW, does nothing under the other platforms.
    */
    virtual void Tile(wxOrientation orient = wxHORIZONTAL);
};



/**
    @class wxMDIChildFrame

    An MDI child frame is a frame that can only exist on a wxMDIClientWindow,
    which is itself a child of wxMDIParentFrame.

    @beginStyleTable
    @style{wxCAPTION}
           Puts a caption on the frame.
    @style{wxDEFAULT_FRAME_STYLE}
           Defined as @c wxMINIMIZE_BOX | @c wxMAXIMIZE_BOX | @c wxTHICK_FRAME |
           @c wxSYSTEM_MENU | @c wxCAPTION.
    @style{wxICONIZE}
           Display the frame iconized (minimized) (Windows only).
    @style{wxMAXIMIZE}
           Displays the frame maximized (Windows only).
    @style{wxMAXIMIZE_BOX}
           Displays a maximize box on the frame (Windows and Motif only).
    @style{wxMINIMIZE}
           Identical to @c wxICONIZE.
    @style{wxMINIMIZE_BOX}
           Displays a minimize box on the frame (Windows and Motif only).
    @style{wxRESIZE_BORDER}
           Displays a resizeable border around the window (Motif only; for
           Windows, it is implicit in @c wxTHICK_FRAME).
    @style{wxSTAY_ON_TOP}
           Stay on top of other windows (Windows only).
    @style{wxSYSTEM_MENU}
           Displays a system menu (Windows and Motif only).
    @style{wxTHICK_FRAME}
           Displays a thick frame around the window (Windows and Motif only).
    @endStyleTable

    @remarks
    Although internally an MDI child frame is a child of the MDI client window,
    in wxWidgets you create it as a child of wxMDIParentFrame.
    You can usually forget that the client window exists.
    MDI child frames are clipped to the area of the MDI client window, and may
    be iconized on the client window.
    You can associate a menubar with a child frame as usual, although an MDI
    child doesn't display its menubar under its own title bar.
    The MDI parent frame's menubar will be changed to reflect the currently
    active child frame.
    If there are currently no children, the parent frame's own menubar will
    be displayed.

    @library{wxcore}
    @category{managedwnd}

    @see wxMDIClientWindow, wxMDIParentFrame, wxFrame
*/
class wxMDIChildFrame : public wxFrame
{
public:

    /**
        Default constructor.
    */
    wxMDIChildFrame();

    /**
        Constructor, creating the window.

        @param parent
            The window parent. This should not be @NULL.
        @param id
            The window identifier. It may take a value of -1 to indicate a default
            value.
        @param title
            The caption to be displayed on the frame's title bar.
        @param pos
            The window position. The value @c wxDefaultPosition indicates a default position,
            chosen by either the windowing system or wxWidgets, depending on platform.
        @param size
            The window size. The value @c wxDefaultSize indicates a default size, chosen by
            either the windowing system or wxWidgets, depending on platform.
        @param style
            The window style. See wxMDIChildFrame.
        @param name
            The name of the window. This parameter is used to associate a name with the
            item, allowing the application user to set Motif resource values for individual
            windows.

        @see Create()
    */
    wxMDIChildFrame(wxMDIParentFrame* parent, wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = "frame");

    /**
        Destructor. Destroys all child windows and menu bar if present.
    */
    virtual ~wxMDIChildFrame();

    /**
        Activates this MDI child frame.

        @see Maximize(), Restore()
    */
    virtual void Activate();

    /**
        Used in two-step frame construction.
        See wxMDIChildFrame() for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = "frame");

    /**
        Maximizes this MDI child frame.

        @see Activate(), Restore()
    */
    void Maximize(bool maximize);

    /**
        Restores this MDI child frame (unmaximizes).
    */
    virtual void Restore();
};

