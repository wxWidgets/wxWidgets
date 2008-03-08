/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     documentation for wxMDIClientWindow class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMDIClientWindow
    @wxheader{mdi.h}
    
    An MDI client window is a child of wxMDIParentFrame, and manages zero or
    more wxMDIChildFrame objects.
    
    @library{wxcore}
    @category{FIXME}
    
    @seealso
    wxMDIChildFrame, wxMDIParentFrame, wxFrame
*/
class wxMDIClientWindow : public wxWindow
{
public:
    //@{
    /**
        Constructor, creating the window.
        
        @param parent 
        The window parent.
        
        @param style 
        The window style. Currently unused.
        
        @remarks The second style of constructor is called within
                   wxMDIParentFrame::OnCreateClient.
        
        @sa wxMDIParentFrame::wxMDIParentFrame, wxMDIParentFrame::OnCreateClient
    */
    wxMDIClientWindow();
        wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    //@}

    /**
        Destructor.
    */
    ~wxMDIClientWindow();

    /**
        Used in two-step frame construction. See wxMDIClientWindow()
        for further details.
    */
    bool CreateClient(wxMDIParentFrame* parent, long style = 0);
};


/**
    @class wxMDIParentFrame
    @wxheader{mdi.h}
    
    An MDI (Multiple Document Interface) parent frame is a window which can contain
    MDI child frames in its own 'desktop'. It is a convenient way to avoid window
    clutter,
    and is used in many popular Windows applications, such as Microsoft Word(TM).
    
    @beginStyleTable
    @style{wxCAPTION}:
           Puts a caption on the frame.
    @style{wxDEFAULT_FRAME_STYLE}:
           Defined as wxMINIMIZE_BOX |  wxMAXIMIZE_BOX |  wxTHICK_FRAME | 
           wxSYSTEM_MENU |  wxCAPTION.
    @style{wxHSCROLL}:
           Displays a horizontal scrollbar in the client window, allowing the
           user to view child frames that are off the current view.
    @style{wxICONIZE}:
           Display the frame iconized (minimized) (Windows only).
    @style{wxMAXIMIZE}:
           Displays the frame maximized (Windows only).
    @style{wxMAXIMIZE_BOX}:
           Displays a maximize box on the frame (Windows and Motif only).
    @style{wxMINIMIZE}:
           Identical to wxICONIZE.
    @style{wxMINIMIZE_BOX}:
           Displays a minimize box on the frame (Windows and Motif only).
    @style{wxRESIZE_BORDER}:
           Displays a resizeable border around the window (Motif only; for
           Windows, it is implicit in wxTHICK_FRAME).
    @style{wxSTAY_ON_TOP}:
           Stay on top of other windows (Windows only).
    @style{wxSYSTEM_MENU}:
           Displays a system menu (Windows and Motif only).
    @style{wxTHICK_FRAME}:
           Displays a thick frame around the window (Windows and Motif only).
    @style{wxVSCROLL}:
           Displays a vertical scrollbar in the client window, allowing the
           user to view child frames that are off the current view.
    @style{wxFRAME_NO_WINDOW_MENU}:
           Under Windows, removes the Window menu that is normally added
           automatically.
    @endStyleTable
    
    @library{wxcore}
    @category{managedwnd}
    
    @seealso
    wxMDIChildFrame, wxMDIClientWindow, wxFrame, wxDialog
*/
class wxMDIParentFrame : public wxFrame
{
public:
    //@{
    /**
        Constructor, creating the window.
        
        @param parent 
        The window parent. This should be @NULL.
        
        @param id 
        The window identifier. It may take a value of -1 to indicate a default value.
        
        @param title 
        The caption to be displayed on the frame's title bar.
        
        @param pos 
        The window position. The value wxDefaultPosition indicates a default position, chosen by
        either the windowing system or wxWidgets, depending on platform.
        
        @param size 
        The window size. The value wxDefaultSize indicates a default size, chosen by
        either the windowing system or wxWidgets, depending on platform.
        
        @param style 
        The window style. See wxMDIParentFrame.
        
        @param name 
        The name of the window. This parameter is used to associate a name with the
        item,
        allowing the application user to set Motif resource values for
        individual windows.
        
        @remarks During the construction of the frame, the client window will be
                   created. To use a different class from
                   wxMDIClientWindow, override
                   OnCreateClient().
        
        @sa Create(), OnCreateClient()
    */
    wxMDIParentFrame();
        wxMDIParentFrame(wxWindow* parent, wxWindowID id,
                         const wxString& title,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxDEFAULT_FRAME_STYLE |  wxVSCROLL |  wxHSCROLL,
                         const wxString& name = "frame");
    //@}

    /**
        Destructor. Destroys all child windows and menu bar if present.
    */
    ~wxMDIParentFrame();

    /**
        Activates the MDI child following the currently active one.
        
        @sa ActivatePrevious()
    */
    void ActivateNext();

    /**
        Activates the MDI child preceding the currently active one.
        
        @sa ActivateNext()
    */
    void ActivatePrevious();

    /**
        Arranges any iconized (minimized) MDI child windows.
        
        @sa Cascade(), Tile()
    */
    void ArrangeIcons();

    /**
        Arranges the MDI child windows in a cascade.
        
        @sa Tile(), ArrangeIcons()
    */
    void Cascade();

    /**
        Used in two-step frame construction. See wxMDIParentFrame()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE |  wxVSCROLL |  wxHSCROLL,
                const wxString& name = "frame");

    /**
        Returns a pointer to the active MDI child, if there is one.
    */
    wxMDIChildFrame* GetActiveChild();

    /**
        This gets the size of the frame 'client area' in pixels.
        
        @param width 
        Receives the client width in pixels.
        
        @param height 
        Receives the client height in pixels.
        
        @remarks The client area is the area which may be drawn on by the
                   programmer, excluding title bar, border, status bar,
                   and toolbar if present.
        
        @sa GetToolBar(), SetToolBar(),
              wxMDIClientWindow
    */
    virtual void GetClientSize(int* width, int* height);

    /**
        Returns a pointer to the client window.
        
        @sa OnCreateClient()
    */
    wxMDIClientWindow* GetClientWindow();

    /**
        Returns the window being used as the toolbar for this frame.
        
        @sa SetToolBar()
    */
    virtual wxWindow* GetToolBar();

    /**
        Returns the current Window menu (added by wxWidgets to the menubar). This
        function
        is available under Windows only.
    */
    wxMenu* GetWindowMenu();

    /**
        Override this to return a different kind of client window. If you override this
        function,
        you must create your parent frame in two stages, or your function will never be
        called,
        due to the way C++ treats virtual functions called from constructors. For
        example:
        
        @remarks You might wish to derive from wxMDIClientWindow in order to
                   implement different erase behaviour, for example,
                   such as painting a bitmap on the background.
        
        @sa GetClientWindow(), wxMDIClientWindow
    */
    virtual wxMDIClientWindow* OnCreateClient();

    /**
        Sets the window to be used as a toolbar for this
        MDI parent window. It saves the application having to manage the positioning
        of the toolbar MDI client window.
        
        @param toolbar 
        Toolbar to manage.
        
        @remarks When the frame is resized, the toolbar is resized to be the
                   width of the frame client area, and the toolbar
                   height is kept the same.
        
        @sa GetToolBar(), GetClientSize()
    */
    virtual void SetToolBar(wxWindow* toolbar);

    /**
        Call this to change the current Window menu. Ownership of the menu object
        passes to
        the frame when you call this function.
        
        This call is available under Windows only.
        
        To remove the window completely, use the wxFRAME_NO_WINDOW_MENU window style.
    */
    void SetWindowMenu(wxMenu* menu);

    /**
        Tiles the MDI child windows either horizontally or vertically depending on
        whether @e orient is wxHORIZONTAL or wxVERTICAL.
        
        Currently only implemented for MSW, does nothing under the other platforms.
    */
    void Tile(wxOrientation orient = wxHORIZONTAL);
};


/**
    @class wxMDIChildFrame
    @wxheader{mdi.h}
    
    An MDI child frame is a frame that can only exist on a wxMDIClientWindow,
    which is itself a child of wxMDIParentFrame.
    
    @beginStyleTable
    @style{wxCAPTION}:
           Puts a caption on the frame.
    @style{wxDEFAULT_FRAME_STYLE}:
           Defined as wxMINIMIZE_BOX |  wxMAXIMIZE_BOX |  wxTHICK_FRAME | 
           wxSYSTEM_MENU |  wxCAPTION.
    @style{wxICONIZE}:
           Display the frame iconized (minimized) (Windows only).
    @style{wxMAXIMIZE}:
           Displays the frame maximized (Windows only).
    @style{wxMAXIMIZE_BOX}:
           Displays a maximize box on the frame (Windows and Motif only).
    @style{wxMINIMIZE}:
           Identical to wxICONIZE.
    @style{wxMINIMIZE_BOX}:
           Displays a minimize box on the frame (Windows and Motif only).
    @style{wxRESIZE_BORDER}:
           Displays a resizeable border around the window (Motif only; for
           Windows, it is implicit in wxTHICK_FRAME).
    @style{wxSTAY_ON_TOP}:
           Stay on top of other windows (Windows only).
    @style{wxSYSTEM_MENU}:
           Displays a system menu (Windows and Motif only).
    @style{wxTHICK_FRAME}:
           Displays a thick frame around the window (Windows and Motif only).
    @endStyleTable
    
    @library{wxcore}
    @category{managedwnd}
    
    @seealso
    wxMDIClientWindow, wxMDIParentFrame, wxFrame
*/
class wxMDIChildFrame : public wxFrame
{
public:
    //@{
    /**
        Constructor, creating the window.
        
        @param parent 
        The window parent. This should not be @NULL.
        
        @param id 
        The window identifier. It may take a value of -1 to indicate a default value.
        
        @param title 
        The caption to be displayed on the frame's title bar.
        
        @param pos 
        The window position. The value wxDefaultPosition indicates a default position, chosen by
        either the windowing system or wxWidgets, depending on platform.
        
        @param size 
        The window size. The value wxDefaultSize indicates a default size, chosen by
        either the windowing system or wxWidgets, depending on platform.
        
        @param style 
        The window style. See wxMDIChildFrame.
        
        @param name 
        The name of the window. This parameter is used to associate a name with the
        item,
        allowing the application user to set Motif resource values for
        individual windows.
        
        @remarks None.
        
        @sa Create()
    */
    wxMDIChildFrame();
        wxMDIChildFrame(wxMDIParentFrame* parent, wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = "frame");
    //@}

    /**
        Destructor. Destroys all child windows and menu bar if present.
    */
    ~wxMDIChildFrame();

    /**
        Activates this MDI child frame.
        
        @sa Maximize(), Restore()
    */
    void Activate();

    /**
        Used in two-step frame construction. See wxMDIChildFrame()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = "frame");

    /**
        Maximizes this MDI child frame.
        
        @sa Activate(), Restore()
    */
    void Maximize(bool maximize);

    /**
        Restores this MDI child frame (unmaximizes).
    */
    void Restore();
};
