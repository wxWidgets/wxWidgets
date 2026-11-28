///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/tabmdi.h
// Purpose:     Documentation of wxAui MDI classes.
// Created:     2016-10-27
// Copyright:   (c) 2016-2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxAuiMDIParentFrame

    wxAuiMDIParentFrame is an MDI parent frame using wxAuiNotebook for showing
    the child frames as notebook pages.

    Unlike wxMDIParentFrame, which uses the native MDI implementation under
    wxMSW and different implementations under other ports, this class always
    uses wxAuiMDIClientWindow as its client window and so always presents its
    children using AUI tabs. This can be useful for applications wanting the
    same tabbed MDI appearance on all platforms.

    Child windows are created as wxAuiMDIChildFrame objects using this frame as
    their parent. The active child receives menu and update UI events before the
    parent frame handles them.

    @library{wxaui}
    @category{aui,managedwnd}

    @see wxAuiMDIChildFrame, wxAuiMDIClientWindow, wxAuiNotebook,
         wxMDIParentFrame
*/
class wxAuiMDIParentFrame : public wxFrame
{
public:
    /**
        Default constructor.

        Use Create() to finish constructing the frame.
    */
    wxAuiMDIParentFrame();

    /**
        Constructor, creating the frame.

        The wxHSCROLL and wxVSCROLL style bits apply to the AUI MDI client
        window, not to the frame itself.
    */
    wxAuiMDIParentFrame(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                        const wxString& name = wxFrameNameStr);

    ~wxAuiMDIParentFrame();

    /**
        Used in two-step frame construction.

        See wxAuiMDIParentFrame() for the parameter descriptions.
    */
    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr );

    /**
        Sets the tab art provider used by the underlying wxAuiNotebook.
    */
    void SetArtProvider(wxAuiTabArt* provider);

    /**
        Returns the tab art provider used by the underlying wxAuiNotebook.
    */
    wxAuiTabArt* GetArtProvider();

    /**
        Returns the notebook used by this frame.
    */
    wxAuiNotebook* GetNotebook() const;

    /**
        Returns the Window menu used for child-window commands.
    */
    wxMenu* GetWindowMenu() const;

    /**
        Replaces the Window menu used for child-window commands.
    */
    void SetWindowMenu(wxMenu* pMenu);

    virtual void SetMenuBar(wxMenuBar *pMenuBar);

    void SetChildMenuBar(wxAuiMDIChildFrame *pChild);

    wxAuiMDIChildFrame *GetActiveChild() const;
    void SetActiveChild(wxAuiMDIChildFrame* pChildFrame);

    /**
        Returns the client window containing the child-frame pages.
    */
    wxAuiMDIClientWindow *GetClientWindow() const;

    /**
        Override this to return a custom wxAuiMDIClientWindow-derived class.

        If you override this function, create the frame using the default
        constructor followed by Create(), otherwise the overridden virtual
        function cannot be called during construction.
    */
    virtual wxAuiMDIClientWindow *OnCreateClient();

    /**
        Has no effect for AUI MDI, because child frames are notebook pages.
    */
    virtual void Cascade();

    /**
        Splits the active AUI notebook page horizontally or vertically.
    */
    virtual void Tile(wxOrientation orient = wxHORIZONTAL);

    /**
        Has no effect for AUI MDI, because child frames cannot be iconized.
    */
    virtual void ArrangeIcons();

    /**
        Activates the child following the current one.
    */
    virtual void ActivateNext();

    /**
        Activates the child preceding the current one.
    */
    virtual void ActivatePrevious();
};


/**
    @class wxAuiMDIChildFrame

    wxAuiMDIChildFrame represents one child document inside an
    wxAuiMDIParentFrame.

    In AUI MDI, child frames are displayed as pages of the parent's
    wxAuiMDIClientWindow, not as independently movable native child frames. The
    child's title is used as the tab label and its menu bar, if any, is shown
    by the parent frame while the child is active.

    Create AUI MDI children with a wxAuiMDIParentFrame parent, not directly
    with the client notebook.

    @library{wxaui}
    @category{aui,managedwnd}

    @see wxAuiMDIParentFrame, wxAuiMDIClientWindow, wxMDIChildFrame
*/
class wxAuiMDIChildFrame : public wxPanel
{
public:
    /**
        Default constructor.

        Use Create() to finish constructing the child frame.
    */
    wxAuiMDIChildFrame();

    /**
        Constructor, creating the child frame.
    */
    wxAuiMDIChildFrame(wxAuiMDIParentFrame *parent,
                       wxWindowID winid,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDEFAULT_FRAME_STYLE,
                       const wxString& name = wxFrameNameStr);

    virtual ~wxAuiMDIChildFrame();

    /**
        Used in two-step child-frame construction.

        See wxAuiMDIChildFrame() for the parameter descriptions.
    */
    bool Create(wxAuiMDIParentFrame *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual void SetMenuBar(wxMenuBar *menuBar);
    virtual wxMenuBar *GetMenuBar() const;

    /**
        Sets the child title, which is used as its notebook tab label.
    */
    virtual void SetTitle(const wxString& title);

    /**
        Returns the child title.
    */
    virtual wxString GetTitle() const;

    virtual void SetIcons(const wxIconBundle& icons);
    virtual const wxIconBundle& GetIcons() const;

    virtual void SetIcon(const wxIcon& icon);
    virtual const wxIcon& GetIcon() const;

    virtual void Activate();
    virtual bool Destroy();

    virtual bool Show(bool show = true);

    // no status bars
    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = 1,
                                         wxWindowID winid = 1,
                                         const wxString& name = wxEmptyString);

    virtual wxStatusBar *GetStatusBar() const;
    virtual void SetStatusText( const wxString &text, int number=0 );
    virtual void SetStatusWidths( int n, const int widths_field[] );

    // no toolbar bars
    virtual wxToolBar* CreateToolBar(long style,
                                     wxWindowID winid,
                                     const wxString& name);
    virtual wxToolBar *GetToolBar() const;

    // no maximize etc
    virtual void Maximize(bool maximize = true);
    virtual void Restore();
    virtual void Iconize(bool iconize  = true);
    virtual bool IsMaximized() const;
    virtual bool IsIconized() const;
    virtual bool ShowFullScreen(bool show, long style);
    virtual bool IsFullScreen() const;

    virtual bool IsTopLevel() const;

    /**
        Sets the AUI MDI parent frame containing this child.
    */
    void SetMDIParentFrame(wxAuiMDIParentFrame* parent);

    /**
        Returns the AUI MDI parent frame containing this child.
    */
    wxAuiMDIParentFrame* GetMDIParentFrame() const;
};

/**
    @class wxAuiMDIClientWindow

    wxAuiMDIClientWindow is the wxAuiNotebook used by wxAuiMDIParentFrame to
    contain wxAuiMDIChildFrame pages.

    Applications normally do not create this class directly. It is created by
    wxAuiMDIParentFrame::OnCreateClient(), which can be overridden to return a
    derived class when custom client-window behaviour is needed.

    @library{wxaui}
    @category{aui,managedwnd}

    @see wxAuiMDIParentFrame, wxAuiMDIChildFrame, wxAuiNotebook
*/
class wxAuiMDIClientWindow : public wxAuiNotebook
{
public:
    /**
        Default constructor.
    */
    wxAuiMDIClientWindow();

    /**
        Constructor, creating the client window.
    */
    wxAuiMDIClientWindow(wxAuiMDIParentFrame *parent, long style = 0);

    /**
        Called by wxAuiMDIParentFrame immediately after creating the client
        window object.
    */
    virtual bool CreateClient(wxAuiMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL);

    /**
        Selects the page at the given index.
    */
    virtual int SetSelection(size_t page);

    /**
        Returns the active child frame, or @NULL if there are no pages.
    */
    virtual wxAuiMDIChildFrame* GetActiveChild();

    /**
        Makes the given child frame active.
    */
    virtual void SetActiveChild(wxAuiMDIChildFrame* pChildFrame);
};
